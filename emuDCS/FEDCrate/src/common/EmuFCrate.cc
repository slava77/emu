#include "EmuFCrate.h"

// addition for STEP
#include "Crate.h"
#include "DDU.h"
#include "DCC.h"
#include "IRQData.h"
// end addition for STEP

#include <iomanip>

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"  // xoap::bind()
#include "xgi/Method.h"  // xgi::bind()

// Includes added for createReply:
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()

#include "cgicc/HTMLClasses.h"

using namespace cgicc;
using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuFCrate);

static const string STATE_UNKNOWN = "unknown";

EmuFCrate::EmuFCrate(xdaq::ApplicationStub *s):
	LocalEmuApplication(s),
	ttsID_(0),
	ttsCrate_(0),
	ttsSlot_(0),
	ttsBits_(0),
	soapConfigured_(false),
	soapLocal_(false),
	endcap_("?")
{
	//
	// State machine definition
	//

	// SOAP call-back functions, which relays to *Action method.
	xoap::bind(this, &EmuFCrate::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onHalt,      "Halt",      XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);
	//xoap::bind(this, &EmuFCrate::onUpdateFlash, "UpdateFlash", XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// 'fsm_' is defined in EmuApplication
	fsm_.addState('H', "Halted",     this, &EmuFCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFCrate::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuFCrate::stateChanged);

	fsm_.addStateTransition(
		'H', 'C', "Configure", this, &EmuFCrate::configureAction);
	fsm_.addStateTransition(
		'C', 'C', "Configure", this, &EmuFCrate::configureAction);
	fsm_.addStateTransition(
		'C', 'E', "Enable",    this, &EmuFCrate::enableAction);
	fsm_.addStateTransition(
		'E', 'C', "Disable",   this, &EmuFCrate::disableAction);
	fsm_.addStateTransition(
		'C', 'H', "Halt",      this, &EmuFCrate::haltAction);
	fsm_.addStateTransition(
		'E', 'H', "Halt",      this, &EmuFCrate::haltAction);
	fsm_.addStateTransition(
		'H', 'H', "Halt",      this, &EmuFCrate::haltAction);
	fsm_.addStateTransition(
		'F', 'H', "Halt",      this, &EmuFCrate::haltAction);
	// PGK We don't need these to be state transitions.
	//fsm_.addStateTransition(
	//	'H', 'H', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	//fsm_.addStateTransition(
	//	'C', 'C', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	//fsm_.addStateTransition(
	//	'E', 'E', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	//fsm_.addStateTransition(
	//	'C', 'C', "UpdateFlash", this, &EmuFCrate::updateFlashAction);
	//fsm_.addStateTransition(
	//	'F', 'H', "UpdateFlash", this, &EmuFCrate::updateFlashAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	// 'state_' is defined in EmuApplication
	state_ = fsm_.getStateName(fsm_.getCurrentState());

	// Exported parameters
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	getApplicationInfoSpace()->fireItemAvailable("ttsID", &ttsID_);
	getApplicationInfoSpace()->fireItemAvailable("ttsCrate", &ttsCrate_);
	getApplicationInfoSpace()->fireItemAvailable("ttsSlot",  &ttsSlot_);
	getApplicationInfoSpace()->fireItemAvailable("ttsBits",  &ttsBits_);
	getApplicationInfoSpace()->fireItemAvailable("dccInOut", &dccInOut_);
	getApplicationInfoSpace()->fireItemAvailable("errorChambers", &errorChambers_);
	getApplicationInfoSpace()->fireItemAvailable("endcap", &endcap_);

	// HyperDAQ pages
	xgi::bind(this, &EmuFCrate::webDefault, "Default");
	xgi::bind(this, &EmuFCrate::webFire, "Fire");
	xgi::bind(this, &EmuFCrate::webConfigure, "Configure");
	// PGK We can let the CSCSV do all the TTS business.
	//xgi::bind(this, &EmuFCrate::webSetTTSBits, "SetTTSBits");

	// addition for STEP
	getApplicationInfoSpace()->fireItemAvailable ("step_killfiber", &step_killfiber_);
	xoap::bind(this, &EmuFCrate::onPassthru,  "Passthru", XDAQ_NS_URI);
	// end addition for STEP

	// Logger/Appender
	// log file format: EmuFEDYYYY-DOY-HHMMSS_rRUNNUMBER.log
	char datebuf[55];
	char filebuf[255];
	time_t theTime = time(NULL);

	strftime(datebuf, sizeof(datebuf), "%Y-%j-%H%M%S", localtime(&theTime));
	sprintf(filebuf,"EmuFCrate%d-%s.log",getApplicationDescriptor()->getLocalId(),datebuf);

	log4cplus::SharedAppenderPtr myAppend = new FileAppender(filebuf);
	myAppend->setName("EmuFCrateAppender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	getApplicationLogger().addAppender(myAppend);

	// PGK is an idiot.  Forgetting this leads to disasters.
	TM = new IRQThreadManager();

}



xoap::MessageReference EmuFCrate::onConfigure(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Configure");
	
	// PGK I avoid errors at all cost.
	if (state_.toString() == "Enabled" || state_.toString() == "Failed") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Configured is not a valid transition.  Fixing by going to Halted first.");
		fireEvent("Halt");
	}

	fireEvent("Configure");

	return createReply(message);
}



xoap::MessageReference EmuFCrate::onEnable(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Enable");
	// PGK I avoid errors at all cost.
	if (state_.toString() == "Halted" || state_.toString() == "Failed") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Enabled is not a valid transition.  Fixing by going to Halted->Configured first.");
		fireEvent("Halt");
		fireEvent("Configure");
	}

	fireEvent("Enable");

	return createReply(message);
}



xoap::MessageReference EmuFCrate::onDisable(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Disable");

	// PGK I avoid errors at all cost.
	if (state_.toString() != "Enabled") {
		LOG4CPLUS_WARN(getApplicationLogger(), state_.toString() <<"->Configured via \"Disable\" is not a valid transition.  Fixing by doing Halted->Configured instead.");
		fireEvent("Halt");
		fireEvent("Configure");
	} else {
		fireEvent("Disable");
	}

	return createReply(message);
}



xoap::MessageReference EmuFCrate::onHalt(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Halt");

	fireEvent("Halt");

	return createReply(message);
}



xoap::MessageReference EmuFCrate::onSetTTSBits(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
	//cout << "EmuFCrate: inside onSetTTSBits" << endl;
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP command: SetTTSBits");
	// PGK We don't need a state transition here.  This is a simple routine.
	//fireEvent("SetTTSBits");

	// set sTTS bits
	writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);
	// read back sTTS bits
	ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);

	//cout << "EmuFCrate: onSetTTSBits, ttsBits_=" << ttsBits_.toString() << endl;
	LOG4CPLUS_DEBUG(getApplicationLogger(), "ttsBits_=" << ttsBits_.toString());
	
	//string strmess;
	//message->writeTo(strmess);
	//cout << " Message:  "<< strmess << endl;

	//return createReply(message); // copy and modify this function here:

	// PGK Technically, nothing is expecting anything in particular from this
	//  routine.  As long as we don't send a SOAP message with a fault in it,
	//  the message is ignored.  So let's just send a dummy response.
/*
	string command = "";

	DOMNodeList *elements = message->getSOAPPart().getEnvelope().getBody().getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

// JRG, return the ttsBits readback value too:
	xoap::SOAPName responseStatus = envelope.createName(
			"setTTSBitsStatus", "ttsBits", ttsBits_.toString());
	envelope.getBody().addBodyElement(responseStatus);

	string strrply;
	reply->writeTo(strrply);
	cout << " Reply:  " << strrply << endl;


// JRG, try postSOAP:
	string klass="EmuFCrateManager";
	int instance = 0;
	cout << "  * EmuFCrate: trying postSOAP" << endl;
	xdaq::ApplicationDescriptor *app;
	try {
		app = getApplicationContext()->getDefaultZone()
		->getApplicationDescriptor(klass, instance);
		cout << "  * EmuFCrate: postSOAP, got application instance=" << instance << klass << endl;
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		cout << "  * EmuFCrate: postSOAP, application not found! " << instance << klass << endl;
		return reply; // Do nothing if the target doesn't exist
	}

	xoap::MessageReference rereply;

	// send the message
	cout << "  * EmuFCrate: onSetTTSBitsResponse, sending Soap Response" << endl;
	rereply = getApplicationContext()->postSOAP(reply, app);
	cout << "  * EmuFCrate: onSetTTSBitsResponse, got Soap rereply " << endl;
*/

	// PGK Remember:  you can always steal the TTSBits status via SOAP if you
	//  really, really want it.
	return createReply(message);
}


// PGK This is now done automatically at Configure
/*
xoap::MessageReference EmuFCrate::onUpdateFlash(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	fireEvent("UpdateFlash");

	return createReply(message);

}
*/



void EmuFCrate::configureAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Entering EmuFCrate::configureAction");
	//cout << "    enter EmuFCrate::configureAction " << endl;

	if (soapLocal_) {
		soapLocal_ = false;
		soapConfigured_ = false;
	} else {
		soapConfigured_ = true;
	}

	// The run type is given to us via SOAP.
	if (runType_.toString() != "") {
		LOG4CPLUS_INFO(getApplicationLogger(), "Run type is " << runType_.toString());
	} else {
		LOG4CPLUS_INFO(getApplicationLogger(), "Run type is empty.  Assuming run type \"Debug\"");
	}
	if (runType_.toString() == "Debug" || runType_.toString() == "") {
		getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
	} else {
		getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
	}

	// JRG: note that the HardReset & Resynch should already be done by this point!

	// PGK This simply sets the configuration file correctly in the
	//  EmuFController object.
	// set it here automatically now,
	// rather then manual selection later with HyperDAQ
	SetConfFile(xmlFile_);
	LOG4CPLUS_INFO(getApplicationLogger(),"EmuFCrate::configureAction using XML file " << xmlFile_.toString());

	// PGK This calls the XML parser, builds the crate objects, and sets them
	//  in the EmuFController object.  Access the crates via this->getCrates()
	//  or selector().crates().
	init();
	// in real CMS running there will not be downloading of registers,
	// so this will just define the Crates/DDUs/DCCs for FED Apps.

	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	std::vector< Crate * > myCrates = getCrates();
	for (std::vector< Crate * >::iterator iCrate = myCrates.begin(); iCrate != myCrates.end(); iCrate++) {
		std::vector< DCC * > dccs = (*iCrate)->dccs();
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_INFO(getApplicationLogger(), "HARD RESET THROUGH DCC!");
			dccs[0]->crateHardReset();
		}
	}

// JRG, add loop over all DDUs in the FED Crates
//   1) verify the L1A scalers at the DDU FPGA == 0, use  DDU::ddu_rdscaler()
//   2) get the Live Fibers "Link Active" from the InFPGAs and check these
//	  against the Flash settings (or DB settings or the XML?)
//	   -use  DDU::infpga_CheckFiber(enum DEVTYPE dv)
//	   -use "Kill Channel Mask" from  DDU::read_page1()
//   3) check that the DDU "CSC Error Status" is Zero (like top page).
//         -Read "CSC status summary for FMM" from  DDU::vmepara_CSCstat()
//	   -Verify the 32-bit status from DDUFPGA == 0?
//              use  DDU::ddu_fpgastat()&0xdecfffff   <<- note the mask
//	   -Verify the 32-bit status from InFPGAs == 0?
//              use  DDU::infpgastat(enum DEVTYPE dv)&0xf7eedfff   <<- note the mask
//	 -->> definitely need to ignore some bits though!  see the masks

	// PGK EmuFCrate inherits from EmuFController, so this is
	//  EmuFController::configure.  It loads the constants defined in the
	//  configuration XML to the DDUs and the DCC.
	configure();  //JRG Moved to configureAction, 4/29/08

	// Now is a good time to set what endcap we are.
	if (myCrates.size() == 1) {
		if (myCrates[0]->number() == 5) endcap_ = "TrackFinder";
		else if (myCrates[0]->number() == 0) endcap_ = "Test Crate";
	} else if (myCrates.size() == 2) {
		if (myCrates[0]->number() == 1 && myCrates[1]->number() == 2
			|| myCrates[0]->number() == 2 && myCrates[1]->number() == 1) endcap_ = "Plus-Side";
		else if (myCrates[0]->number() == 3 && myCrates[1]->number() == 4
			|| myCrates[0]->number() == 4 && myCrates[1]->number() == 3) endcap_ = "Minus-Side";
	}


	// PGK At this point, we need to check to see if the constants from the
	//  XML file have been properly loaded into the DDUs.  This will call
	//  updateFlashAction, which will automatically load everything as needed.
	for (std::vector< Crate * >::iterator iCrate = myCrates.begin(); iCrate != myCrates.end(); iCrate++) {
		if ((*iCrate)->number() > 4) continue;
		//cout << "Checking crate " << (*iCrate)->number() << endl;
		vector<DDU *>::iterator iDDU;
		vector<DDU *> ddus = (*iCrate)->ddus();
		for (iDDU = ddus.begin(); iDDU != ddus.end(); iDDU++) {

			if ((*iDDU)->slot() > 21) continue;

			LOG4CPLUS_INFO(getApplicationLogger(), "Reading flash values for crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot());

			//int flashKillFiber = (*iDDU)->readFlashKillFiber();
			int flashKillFiber = (*iDDU)->read_page1();
			//long int fpgaKillFiber = (*iDDU)->readKillFiber();
			long int fpgaKillFiber = (*iDDU)->ddu_rdkillfiber();
			long int xmlKillFiber = (*iDDU)->killfiber_;
			//unsigned short int DDUGbEPrescale = (*iDDU)->readGbEPrescale() & 0xf;
			//unsigned short int dduGbEPrescale = (*iDDU)->vmepara_rd_GbEprescale() & 0xf;
			//unsigned short int xmlGbEPrescale = (*iDDU)->gbe_prescale_ & 0xf;

			//cout << "Slot " << setw(2) << (*iDDU)->slot() << " killFiber: XML(" << hex << xmlKillFiber << dec << ") fpga(" << hex << fpgaKillFiber << dec << ") flash(" << hex << flashKillFiber << dec << ")" << endl;
			//cout << "        GbEPrescale: XML(" << hex << (*iDDU)->gbe_prescale_ << dec << ") DDU(" << hex << dduGbEPrescale << dec << ")" << endl;
			LOG4CPLUS_DEBUG(getApplicationLogger(), "killFiber: XML(" << hex << xmlKillFiber << dec << ") fpga(" << hex << fpgaKillFiber << dec << ") flash(" << hex << flashKillFiber << dec << ")");

			if ((flashKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
				//cout << "... reflashing killFiber register ..." << endl;
				LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML killFiber disagree:  reloading flash");
				//(*iDDU)->writeFlashKillFiber(xmlKillFiber & 0x7fff);
				(*iDDU)->vmepara_wr_inreg(xmlKillFiber & 0x7fff);
				(*iDDU)->write_page1();
			}
			if (fpgaKillFiber != xmlKillFiber) {
				//cout << "... reloading FPGA killFiber register ..." << endl;
				LOG4CPLUS_INFO(getApplicationLogger(),"fpga and XML killFiber disagree:  reloading fpga");
				//(*iDDU)->writeKillFiber(xmlKillFiber);
				(*iDDU)->ddu_loadkillfiber(xmlKillFiber);
			}
			// PGK GbEPrescale is depricated.
			/*
			if ((*iDDU)->gbe_prescale_ != DDUgbePrescale) {
				cout << "... reloading GbEPrescale register ..." << endl;
				(*iDDU)->vmepara_wr_GbEprescale((*iDDU)->gbe_prescale_);
			}
			*/

			// Now we should check if the RUI matches the flash value and
			//  update it as needed.
			int flashRUI = (*iDDU)->read_page7();
			int targetRUI = (*iCrate)->getRUI((*iDDU)->slot());

			LOG4CPLUS_DEBUG(getApplicationLogger(),"RUI: flash(" << flashRUI << ") calculated(" << targetRUI << ")");
			
			if (flashRUI != targetRUI) {
				LOG4CPLUS_INFO(getApplicationLogger(),"Flash and calculated RUI disagree:  reloading flash");
				(*iDDU)->vmepara_wr_inreg(targetRUI);
				(*iDDU)->write_page7();
			}
		}
	}

	int Fail=0;
	unsigned short int count=0;
	for(unsigned i = 0; i < myCrates.size(); ++i){
		// find DDUs in each crate
		std::vector<DDU*> myDdus = myCrates[i]->ddus();
		vector<DCC *> myDccs = myCrates[i]->dccs();
		for(unsigned j =0; j < myDdus.size(); ++j){
			if(myDdus[j]->slot()==28){
				if (myCrates[i]->number() > 4) { // Track finder
					LOG4CPLUS_DEBUG(getApplicationLogger(), "broadcasting FMM Error Enable to Crate " << myCrates[i]->number());
					myDdus[j]->vmepara_wr_fmmreg(0x0000);
				} else {
					LOG4CPLUS_DEBUG(getApplicationLogger(), "broadcasting FMM Error Disable to Crate " << myCrates[i]->number());
					//cout <<"   Setting FMM Error Disable for Crate " << myCrates[i]->number() << endl;
					myDdus[j]->vmepara_wr_fmmreg(0xFED0+(count&0x000f));
				}
			}
		}

		for(unsigned j =0; j < myDdus.size(); ++j){
			LOG4CPLUS_DEBUG(getApplicationLogger(), "checking DDU configure status, Crate " << myCrates[i]->number() << " slot " << myDdus[j]->slot());
			//cout << " EmuFCrate: Checking DDU configure status for Crate " << myCrates[i]->number() << " slot " << myDdus[j]->slot() << endl;

			if (myDdus[j]->slot() < 21){
				int FMMReg = myDdus[j]->vmepara_rd_fmmreg();
				//int FMMReg = myDdus[j]->readFMMReg();
				//if(RegRead!=(0xFED0+(count&0x000f)))cout << "    fmmreg broadcast check is wrong, got " << hex << RegRead << " should be FED0+" << count << dec << endl;
				if (FMMReg!=(0xFED0+(count&0x000f))) {
					LOG4CPLUS_WARN(getApplicationLogger(), "fmmreg broadcast check is wrong, got " << hex << FMMReg << ", should be 0xFED" << count);
				}


				int mySlot = myDdus[j]->slot();
				//  myDdus[j]->ddu_reset(); // sync reset via VME
				//  unsigned long int thisL1A=myDdus[j]->ddu_rdscaler();

				int CSCStat = myDdus[j]->vmepara_CSCstat();
				//int CSCStat = myDdus[j]->readCSCStat();
				//myDdus[j]->ddu_fpgastat(); //  & 0xdecfffff  <<- note the mask
				unsigned long int dduFPGAStat = myDdus[j]->ddu_fpgastat()&0xdecfffff;  // <<- note the mask
				//unsigned long int dduFPGAStat = myDdus[j]->readFPGAStat(DDUFPGA) & 0xdecfffff;
				//myDdus[j]->infpgastat(INFPGA0); // & 0xf7eedfff  <<- note the mask
				unsigned long int inFPGA0Stat = myDdus[j]->infpgastat(INFPGA0) & 0xf7eedfff;  // <<- note the mask
				//unsigned long int inFPGA0Stat = myDdus[j]->readFPGAStat(INFPGA0) & 0xf7eedfff;
				//myDdus[j]->infpgastat(INFPGA1); // & 0xf7eedfff  <<- note the mask
				unsigned long int inFPGA1Stat = myDdus[j]->infpgastat(INFPGA1) & 0xf7eedfff;  // <<- note the mask
				//unsigned long int inFPGA1Stat = myDdus[j]->readFPGAStat(INFPGA0) & 0xf7eedfff;

				//cout << "   DDU Status for slot " << mySlot << " (hex) " << hex << CSCstat << " " << DDUfpgaStat << " " << INfpga0Stat << " " << INfpga1Stat << dec << endl;
				LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU Status for slot " << dec << mySlot << ": 0x" << hex << CSCStat << " 0x" << dduFPGAStat << " 0x" << inFPGA0Stat << " 0x" << inFPGA1Stat << dec);
				//printf("   DDU Status for slot %2d: 0x%04x  0x%08x  0x%08x  0x%08x\n",mySlot,(unsigned int) CSCstat,(unsigned int) DDUfpgaStat,(unsigned int) INfpga0Stat,(unsigned int) INfpga1Stat);

				long int liveFibers = (myDdus[j]->infpga_CheckFiber(INFPGA0)&0x000000ff) | ((myDdus[j]->infpga_int_CheckFiber(INFPGA1)&0x000000ff)<<8);
				int killFiber = (myDdus[j]->read_page1()&0xffff);
				//int liveFibers = (myDdus[j]->checkFiber(INFPGA0) | (myDdus[j]->checkFiber(INFPGA0) << 8));
				//int killFiber = myDdus[j]->readKillFiber()&0x7fff;
				// cout << "   LiveFibers for slot " << mySlot << ": 0x" << hex << LiveFibers << dec << endl;
				// cout << "   killFiber for slot " << mySlot << ": 0x" << hex << killFiber << dec << endl;
				//printf("   LiveFibers for slot %2d: 0x%04x     killFiber=%04x\n",mySlot,(unsigned int) LiveFibers,(unsigned int) killFiber);
				LOG4CPLUS_INFO(getApplicationLogger(), "liveFibers/killFibers for slot " << dec << mySlot << ": 0x" << hex << liveFibers << " 0x" << killFiber << dec);

				unsigned long int thisL1A = myDdus[j]->ddu_rdscaler();
				//long unsigned int thisL1A = myDdus[j]->readL1Scalar(DDUFPGA);
				//cout << "   L1A Scaler = " << thisL1A << endl;
				LOG4CPLUS_DEBUG(getApplicationLogger(), "L1A Scalar for slot " << dec << mySlot << ": " << thisL1A);

				if (inFPGA0Stat>0) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to INFPGA0 error");
					//cout<<"     * DDU configure failure due to INFPGA0 error *" <<endl;
				}
				if (inFPGA1Stat>0) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to INFPGA1 error");
					//cout<<"     * DDU configure failure due to INFPGA1 error *" <<endl;
				}
				if (dduFPGAStat>0) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to DDUFPGA error");
					//cout<<"     * DDU configure failure due to DDUFPGA error *" <<endl;
				}
				if (CSCStat>0) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to CSCStat error");
					//cout<<"     * DDU configure failure due to CSCstat error *" <<endl;
				}
				/*
				if (killFiber != (0x7fff - (liveFibers&0x7fff))) {
				//if (killFiber&0x7fff != liveFibers&0x7fff) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to liveFiber/killFiber mismatch.  liveFiber 0x" << hex << liveFibers << ", killfiber 0x" << killFiber);
					//cout<<"     * DDU configure failure due to Live Fiber mismatch *" <<endl; 76ff 77ff 79ff 7aff 7bff
				}
				*/
				if (thisL1A>0) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to L1A Scaler not reset");
					//cout<<"     * DDU configure failure due to L1A scaler not Reset *" <<endl;
				}
				//if(Fail>0){
					//LOG4CPLUS_ERROR(getApplicationLogger(), Fail << " total DDU failures, throwing exception");
					//cout<<"   **** DDU configure has failed, setting EXCEPTION.  Fail=" << Fail <<endl;
				//}
			}
		}

		// Now check the fifoinuse parameter from the DCC
		vector<DCC *>::iterator idcc;
		for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
			int fifoinuse = (*idcc)->mctrl_rd_fifoinuse() & 0x3FF;
			if (fifoinuse != (*idcc)->fifoinuse_) {
				LOG4CPLUS_ERROR(getApplicationLogger(), "DCC configure failure due to FIFOInUse mismatch");
				//cout << "     * DCC configure failure due to FifoInUse mismatch *" << endl;
				//cout << "     * saw 0x" << hex << fifoinuse << dec << ", expected 0x" << hex << (*idcc)->fifoinuse_ << dec << " *" << endl;
				Fail++;
			}
		}
	}
	count++;

	//cout << "Now trying to fill dccInOut_" << endl;
	// At this point, we have crates, so we can set up the dccInOut_ vector.

	// On Failure of above, set this:	XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
	if(Fail>0) {
		//XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
		LOG4CPLUS_WARN(getApplicationLogger(), "number of failures: " << Fail << ".   Continuing in a possibly bad state (failure is not an option!)");
	}
	// JRG, finally we need to Broadcast the "FMM Error Disable" signal to both FED Crates.

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Leaving EmuFCrate::configureAction");
	//cout << " EmuFCrate:  Received Message Configure" << endl;
	//cout << "    leave EmuFCrate::configureAction " << std:: endl;

}



void EmuFCrate::enableAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Enable");
	soapLocal_ = false;

	// PGK If the run number is not set, this is a debug run.
	LOG4CPLUS_INFO(getApplicationLogger(), "The run number is " << runNumber_.toString());
	if (runNumber_.toString() == "" || runNumber_.toString() == "0") {
		LOG4CPLUS_INFO(getApplicationLogger(), "Run number not set, assuming run type \"Debug\"");
		getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
	}

	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	std::vector< Crate * > myCrates = getCrates();
	for (std::vector< Crate * >::iterator iCrate = myCrates.begin(); iCrate != myCrates.end(); iCrate++) {
		std::vector< DCC * > dccs = (*iCrate)->dccs();
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_INFO(getApplicationLogger(), "SYNC RESET THROUGH DCC!");
			dccs[0]->crateSyncReset();
		}
	}

	// PGK You have to wipe the thread manager and start over.
	TM = new IRQThreadManager();
	for (unsigned int i=0; i<myCrates.size(); i++) {
		if (myCrates[i]->number() > 4) continue;
		TM->attachCrate(myCrates[i]);
	}
	// PGK We now have the run number from CSCSV
	TM->startThreads(runNumber_);
}



void EmuFCrate::disableAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Disable");
	cout << "Received Message Disable" << endl ;
	soapLocal_ = false;

	TM->endThreads();
	//TM->killThreads();
}



void EmuFCrate::haltAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Halt");
	cout << "Received Message Halt" << endl ;
	soapConfigured_ = false;
	soapLocal_ = false;

	TM->endThreads();

}


// PGK We don't need this to be a transition.  We just perform this action
//  on reception of the SOAP command.
/*
void EmuFCrate::setTTSBitsAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	// set sTTS bits
	writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);

	// read back sTTS bits
	ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);

	// seems to occur next-to-last...
	cout << "EmuFCrate:  Received Message SetTTSBits" << endl;
}
*/

// PGK This is now done automatically at Configure.
/*
void EmuFCrate::updateFlashAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{

	//cout << "EmuFCrate:  Received Message UpdateFlash" << endl;
	LOG4CPLUS_INFO(getApplicationLogger(), "Entering EmuFCrate::updateFlashAction");

	vector<Crate *>::iterator iCrate;
	vector<Crate *> crates = getCrates();

	for (iCrate = crates.begin(); iCrate != crates.end(); iCrate++) {
		if ((*iCrate)->number() > 4) continue;
		//cout << "Checking crate " << (*iCrate)->number() << endl;
		vector<DDU *>::iterator iDDU;
		vector<DDU *> ddus = (*iCrate)->ddus();
		for (iDDU = ddus.begin(); iDDU != ddus.end(); iDDU++) {

			if ((*iDDU)->slot() > 21) continue;

			LOG4CPLUS_INFO(getApplicationLogger(), "Reading flash values for crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot());

			//int flashKillFiber = (*iDDU)->readFlashKillFiber();
			int flashKillFiber = (*iDDU)->read_page1();
			//long int fpgaKillFiber = (*iDDU)->readKillFiber();
			long int fpgaKillFiber = (*iDDU)->ddu_rdkillfiber();
			long int xmlKillFiber = (*iDDU)->killfiber_;
			//unsigned short int DDUGbEPrescale = (*iDDU)->readGbEPrescale() & 0xf;
			//unsigned short int dduGbEPrescale = (*iDDU)->vmepara_rd_GbEprescale() & 0xf;
			//unsigned short int xmlGbEPrescale = (*iDDU)->gbe_prescale_ & 0xf;

			//cout << "Slot " << setw(2) << (*iDDU)->slot() << " killFiber: XML(" << hex << xmlKillFiber << dec << ") fpga(" << hex << fpgaKillFiber << dec << ") flash(" << hex << flashKillFiber << dec << ")" << endl;
			//cout << "        GbEPrescale: XML(" << hex << (*iDDU)->gbe_prescale_ << dec << ") DDU(" << hex << dduGbEPrescale << dec << ")" << endl;
			LOG4CPLUS_DEBUG(getApplicationLogger(), "killFiber: XML(" << hex << xmlKillFiber << dec << ") fpga(" << hex << fpgaKillFiber << dec << ") flash(" << hex << flashKillFiber << dec << ")");

			if (flashKillFiber != (xmlKillFiber & 0x7fff)) {
				//cout << "... reflashing killFiber register ..." << endl;
				LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML disagree:  reloading flash");
				//(*iDDU)->writeFlashKillFiber(xmlKillFiber & 0x7fff);
				(*iDDU)->vmepara_wr_inreg(xmlKillFiber & 0x7fff);
				(*iDDU)->write_page1();
			}
			if (fpgaKillFiber != xmlKillFiber) {
				//cout << "... reloading FPGA killFiber register ..." << endl;
				LOG4CPLUS_INFO(getApplicationLogger(),"fpga and XML disagree:  reloading fpga");
				//(*iDDU)->writeKillFiber(xmlKillFiber);
				(*iDDU)->ddu_loadkillfiber(xmlKillFiber);
			}
			
			if ((*iDDU)->gbe_prescale_ != DDUgbePrescale) {
				cout << "... reloading GbEPrescale register ..." << endl;
				(*iDDU)->vmepara_wr_GbEprescale((*iDDU)->gbe_prescale_);
			}
			
		}
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(),"Leaving EmuFCrate::updateFlashAction");
	//cout << "UpdateFlash done!" << endl;

}
*/

// HyperDAQ pages
void EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	ostringstream sTitle;
	sTitle << "EmuFCrate(" << getApplicationDescriptor()->getInstance() << ") " << endcap_.toString();
	*out << Header(sTitle.str());

	// Manual state changing
	*out << fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("Manual state changes")
		.set("class","legend") << endl;

	*out << cgicc::div();
	*out << "Present state: ";
	*out << cgicc::span(state_.toString())
		.set("class",state_.toString()) << endl;
	*out << cgicc::div();

	*out << cgicc::div() << endl;
	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action","/" + getApplicationDescriptor()->getURN() + "/Fire")
			.set("method","GET") << endl;
		if (state_.toString() == "Halted" || state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Configure") << endl;
		}
		if (state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Enable") << endl;
		}
		if (state_.toString() == "Enabled") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Disable") << endl;
		}
		if (state_.toString() == "Halted" || state_.toString() == "Configured" || state_.toString() == "Enabled" || state_.toString() == "Failed" || state_.toString() == STATE_UNKNOWN) {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Halt") << endl;
		}
		// PGK Command to update the Flashes on the DDUs
		// PGK Now done automatically at configure.
		/*
		if (state_.toString() == "Failed" || state_.toString() == "Configured") {
			*out << cgicc::br() << endl;
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","UpdateFlash") << endl;
		} else {
			*out << cgicc::br() << "Crate-wide flash updates can only be performed from the Configured or Failed states." << endl;
		}
		*/
		*out << cgicc::form() << endl;

	} else {
		*out << "EmuFCrate has been configured through SOAP." << endl;
		*out << cgicc::br() << "Send the Halt signal to manually change states." << endl;
	}
	*out << cgicc::div() << endl;
	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;

	*out << cgicc::fieldset() << endl;

	// HyperDAQ?
	std::set<xdaq::ApplicationDescriptor * > hddescriptors =
		getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuFCrateHyperDAQ");

	if (hddescriptors.size()) {

		*out << fieldset()
			.set("class","fieldset") << endl;
		*out << cgicc::div("EmuFCrateHyperDAQ")
			.set("class","legend") << endl;

		std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
		for ( itDescriptor = hddescriptors.begin(); itDescriptor != hddescriptors.end(); itDescriptor++ ) {
			if ((*itDescriptor)->getInstance() != getApplicationDescriptor()->getInstance()) continue;
			ostringstream className;
			className << (*itDescriptor)->getClassName() << "(" << (*itDescriptor)->getInstance() << ")";
			ostringstream url;
			url << (*itDescriptor)->getContextDescriptor()->getURL() << "/" << (*itDescriptor)->getURN();

			*out << cgicc::a(className.str())
				.set("href",url.str()) << endl;

		}

		*out << cgicc::fieldset() << endl;;
	}

	// sTTS control
	// PGK Let the CSCSV handle all these controls.
	/*
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	*out << cgicc::div("sTTS Control")
		.set("class","legend") << endl;

	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action", "/" + getApplicationDescriptor()->getURN() + "/SetTTSBits") << endl;

		*out << cgicc::div() << endl;
		*out << cgicc::span("Crate # (1-4): ") << endl;
		*out << cgicc::input()
			.set("name", "ttscrate")
			.set("type", "text")
			.set("value", ttsCrate_.toString())
			.set("size", "3") << endl;
		*out << cgicc::div() << endl;

		*out << cgicc::div() << endl;
		*out << cgicc::span("Slot # (4-13): ") << endl;
		*out << cgicc::input()
			.set("name", "ttsslot")
			.set("type", "text")
			.set("value", ttsSlot_.toString())
			.set("size", "3") << endl;
		*out << cgicc::div() << endl;

		*out << cgicc::div() << endl;
		*out << cgicc::span("TTS value (0-15, decimal): ") << endl;
		*out << cgicc::input()
			.set("name", "ttsbits")
			.set("type", "text")
			.set("value", ttsBits_.toString())
			.set("size", "3") << endl;
		*out << cgicc::div() << endl;

		*out << cgicc::input()
			.set("type", "submit")
			.set("name", "command")
			.set("value", "SetTTSBits") << endl;

		*out << cgicc::form() << endl;
	} else {
		*out << "EmuFCrate has been configured through SOAP." << endl;
		*out << cgicc::br() << "Send the Halt signal to manually set TTS bits." << endl;
	}

	*out << cgicc::fieldset() << endl;
	*/

	// IRQ Monitoring
	// Hide the TrackFinder...
	if (endcap_.toString() != "TrackFinder") {
		*out << cgicc::fieldset()
			.set("class","fieldset") << endl;

		if (state_.toString() == "Enabled") {
			*out << cgicc::div("IRQ Monitoring Enabled")
				.set("class","legend") << endl;

			vector<Crate *> crateVector = getCrates();
			vector<Crate *>::iterator iCrate;
			for (iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {

				int crateNumber = (*iCrate)->number();

				// Status table
				*out << cgicc::table()
					.set("style","width: 90%; margin: 10px auto 10px auto; border: solid 2px #009; border-collapse: collapse;") << endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 12pt; font-weight: bold;") << endl;

				*out << cgicc::td()
					.set("colspan","6") << endl;
				*out << "Crate " << crateNumber << endl;
				*out << cgicc::td() << endl;

				*out << cgicc::tr() << endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold;") << endl;

				*out << cgicc::td()
					.set("colspan","6") << endl;

				time_t startTime = TM->data()->startTime[(*iCrate)];
				time_t tickTime = TM->data()->tickTime[(*iCrate)];

				tm *startTimeInfo = localtime(&startTime);
				*out << "Thread started " << asctime(startTimeInfo) << cgicc::br();
				*out << TM->data()->ticks[(*iCrate)] << " ticks, ";
				tm *tickTimeInfo = localtime(&tickTime);
				*out << "last tick " << asctime(tickTimeInfo) << endl;
				*out << cgicc::td() << endl;
				*out << cgicc::tr() << endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold; border: solid 1px #000") << endl;
				*out << cgicc::td("Time of error") << endl;
				*out << cgicc::td("Slot") << endl;
				*out << cgicc::td("RUI") << endl;
				*out << cgicc::td("Fiber(s)") << endl;
				*out << cgicc::td("Chamber(s)") << endl;
				*out << cgicc::td("Action taken") << endl;
				*out << cgicc::tr() << endl;

				std::vector<IRQError *> errorVector = TM->data()->errorVectors[(*iCrate)];
				// Print something pretty if there is no error
				if (errorVector.size() == 0) {
					*out << cgicc::tr() << endl;
					*out << cgicc::td()
						.set("colspan","6")
						.set("style","border: 1px solid #000;")
						.set("class","undefined") << endl;
					*out << "No errors detected (yet)" << endl;
					*out << cgicc::td() << endl;
					*out << cgicc::tr() << endl;
				} else {

					for (std::vector<IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
						// Mark the error as grey if there has been a reset.
						std::string errorClass = "";
						std::string chamberClass = "error";
						if ((*iError)->reset) {
							errorClass = "undefined";
							chamberClass = "undefined";
						}

						*out << cgicc::tr() << endl;
						
						// Time
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << endl;
						time_t interruptTime = (*iError)->errorTime;
						struct tm* interruptTimeInfo = localtime(&interruptTime);
						*out << asctime(interruptTimeInfo) << endl;
						*out << cgicc::td() << endl;

						// Slot
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << endl;
						*out << (*iError)->ddu->slot() << endl;
						*out << cgicc::td() << endl;

						// RUI
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << endl;
						*out << (*iCrate)->getRUI((*iError)->ddu->slot()) << endl;
						*out << cgicc::td() << endl;

						// Fibers
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << endl;
						for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) *out << iFiber << " ";
						}
						*out << cgicc::td() << endl;

						// Chambers
						*out << cgicc::td()
							.set("class",chamberClass)
							.set("style","border: 1px solid #000;") << endl;
						for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) {
								if (iFiber == 15) *out << "DDU ";
								else *out << (*iError)->ddu->getChamber(iFiber)->name() << " ";
								// Pointer-foo!
							}
						}
						*out << cgicc::td() << endl;

						// Action performed
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << endl;
						*out << (*iError)->action << endl;
						*out << cgicc::td() << endl;
						
						*out << cgicc::tr() << endl;
					}
				}

				*out << cgicc::table() << endl;

			}

		} else {
			*out << cgicc::div("IRQ Monitoring Disabled")
				.set("class","legend") << endl;
			*out << cgicc::span("Set state to \"Enabled\" to begin IRQ monitoring threads.")
				.set("style","color: #A00; font-size: 11pt;") << endl;
		}

		*out << cgicc::fieldset() << endl;
	} // End hiding from TrackFinder.

	*out << Footer() << endl;
}



void EmuFCrate::webFire(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	cgicc::Cgicc cgi(in);
	soapLocal_ = true;

	string action = "";
	cgicc::form_iterator name = cgi.getElement("action");
	if(name != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
		cout << "webFire action: " << action << endl;
		LOG4CPLUS_INFO(getApplicationLogger(), "Local FSM state change requested: " << action);
		fireEvent(action);
	}

	webRedirect(in, out);
}



void EmuFCrate::webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Configure");

	fireEvent("Enable");

	webRedirect(in, out);
}


/*
void EmuFCrate::webSetTTSBits(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{

	ttsCrate_.fromString(getCGIParameter(in, "ttscrate"));
	ttsSlot_.fromString(getCGIParameter(in, "ttsslot"));
	ttsBits_.fromString(getCGIParameter(in, "ttsbits"));

	fireEvent("SetTTSBits");

	cout << "EmuFCrate:  inside webSetTTSBits" << endl ;

	webRedirect(in, out);
}
*/


void EmuFCrate::webRedirect(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}



string EmuFCrate::getCGIParameter(xgi::Input *in, string name)
{
	cgicc::Cgicc cgi(in);
	string value;

	form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}




void EmuFCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception)
{
	cout << " stateChanged called " << endl;
	EmuApplication::stateChanged(fsm);

	ostringstream log;
	log << "FSM state changed to " << state_.toString();
	LOG4CPLUS_INFO(getApplicationLogger(), log.str());
}



// addition for STEP

// this routine is written in the assumption that we have only one DDU at this time, for STEP setup.
// if used on multy-ddu setup, will program them all with the same killfiber.
// user must program step_killfiber_ before calling this routine.
xoap::MessageReference EmuFCrate::onPassthru(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
	// find crates
	std::vector<Crate*> myCrates = selector().crates();

	for(unsigned i = 0; i < myCrates.size(); ++i)
	{
		// find DDUs in each crate
		std::vector<DDU*> myDdus = myCrates[i]->ddus();
		for(unsigned j =0; j < myDdus.size(); ++j)
		{
			cout << "Setting passthru mode for crate: " << i << " DDU: " << j << " slot: " << myDdus[j]->slot()
				 << " fiber mask: 0x" << hex << step_killfiber_.toString() << dec << endl;

			if (myDdus[j]->slot() < 21)
			{
				myDdus[j]->vmepara_wr_GbEprescale (0xf0f0); // no prescaling
				myDdus[j]->vmepara_wr_fakel1reg   (0x8787); // fake L1A for each event
				myDdus[j]->ddu_loadkillfiber      (step_killfiber_); // user selects which inputs to use
				myDdus[j]->ddu_reset(); // sync reset via VME
			}
		}
	}
	return createReply(message);
}

// end addition for STEP


// PGK Ugly, but it must be done.  We have to update the parameters that the
//  EmuFCrateManager asks for or else they won't be updated!
xoap::MessageReference EmuFCrate::onGetParameters(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	if (state_.toString() == "Enabled") {
		// dccInOut update
		dccInOut_.clear();
		errorChambers_.clear();

		vector<Crate *> myCrates = getCrates();
		vector<Crate *>::iterator iCrate;

		// PGK Update the DCC rate info.  This is confusing as all get-out.
		for (iCrate = myCrates.begin(); iCrate != myCrates.end(); iCrate++) {
			int crateNumber = (*iCrate)->number();
			if (crateNumber > 4) continue; // Skip TF
			vector<DCC *> myDccs = (*iCrate)->dccs();
			xdata::Vector<xdata::UnsignedInteger> crateV;
			//cout << "Updating getParameters Crate Number " << (*iCrate)->number() << endl;
			crateV.push_back((*iCrate)->number());
			vector<DCC *>::iterator idcc;
			for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
				unsigned short int status[6];
				int dr[6];
				for (int igu=0;igu<6;igu++) {
					status[igu]=(*idcc)->mctrl_ratemon(igu);
					dr[igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//cout << "Slink0: " << dr[0] << endl;

				for (int igu=6;igu<12;igu++) {
					status[igu-6]=(*idcc)->mctrl_ratemon(igu);
					dr[igu-6]=((status[igu-6]&0x3fff)<<(((status[igu-6]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//cout << "Slink1: " << dr[0] << endl;
			}
			//cout << "Pushing back." << endl;
			dccInOut_.push_back(crateV);

			// errorChambers update
			errorChambers_.clear();
			
			std::vector<IRQError *> errorVector = TM->data()->errorVectors[(*iCrate)];
			for (std::vector<IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
				// Skip things that have already been reset (we think)
				if ((*iError)->reset) continue;
				// Report the chamber names and RUI names that are in an error state.
				for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
					if ((*iError)->fibers & (1<<iFiber)) {
						if (iFiber == 15) {
							ostringstream ruiStream;
							ruiStream << "RUI#" << (*iCrate)->getRUI((*iError)->ddu->slot());
							errorChambers_.push_back(ruiStream.str());
						} else errorChambers_.push_back((*iError)->ddu->getChamber(iFiber)->name());
					}
				}
			}
		}
	}
	// PGK Following is just what LocalEmuApplication does.

	xdata::soap::Serializer serializer;

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

	//bool first = true;

	std::map<string, xdata::Serializable *>::iterator iParam;
	for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
		string name = (*iParam).first;
		xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
		xoap::SOAPElement element = bodyElement.addChildElement(elementName);
		serializer.exportAll((*iParam).second, dynamic_cast<DOMElement*>(element.getDOMNode()), true);
		//first = false;
	}

	//reply->writeTo(cout);
	//cout << endl;
	return reply;

}



// End of file
// vim: set sw=4 ts=4:
