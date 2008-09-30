/*****************************************************************************\
* $Id: EmuFCrate.cc,v 3.46 2008/09/30 08:32:40 paste Exp $
*
* $Log: EmuFCrate.cc,v $
* Revision 3.46  2008/09/30 08:32:40  paste
* Updated IRQ Threads so that the endcap name is mentioned in the log filename
*
* Revision 3.45  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.44  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.43  2008/09/19 23:13:59  paste
* Fixed a small bug in disabling of error reporting, added missing file.
*
* Revision 3.42  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.41  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.40  2008/08/26 13:09:02  paste
* Documentation update.
*
* Revision 3.39  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.38  2008/08/18 08:30:15  paste
* Update to fix error propagation from IRQ threads to EmuFCrateManager.
*
* Revision 3.35  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.34  2008/08/15 09:59:22  paste
* Fixed bug where transitions to Halted state while threads were not active caused a crash.
*
* Revision 3.33  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "EmuFCrate.h"

#include <iomanip>
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xgi/Method.h"

#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"

#include "cgicc/HTMLClasses.h"

#include "FEDCrate.h"
#include "DDU.h"
#include "Chamber.h"
#include "DCC.h"
#include "IRQData.h"
#include "VMEController.h"
#include "FEDCrateParser.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrate);

EmuFCrate::EmuFCrate(xdaq::ApplicationStub *s):
	EmuFEDApplication(s),
	BHandles_(""),
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
	getApplicationInfoSpace()->fireItemAvailable("BHandles", &BHandles_);

	// HyperDAQ pages
	xgi::bind(this, &EmuFCrate::webDefault, "Default");
	xgi::bind(this, &EmuFCrate::webFire, "Fire");
	xgi::bind(this, &EmuFCrate::webConfigure, "Configure");
	// PGK We can let the CSCSV do all the TTS business.
	//xgi::bind(this, &EmuFCrate::webSetTTSBits, "SetTTSBits");

	// Logger/Appender
	// log file format: EmuFEDYYYY-DOY-HHMMSS_rRUNNUMBER.log
	char datebuf[55];
	char filebuf[255];
	std::time_t theTime = time(NULL);

	std::strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H:%M:%S", localtime(&theTime));
	std::sprintf(filebuf,"EmuFCrate-%s.log",datebuf);

	log4cplus::SharedAppenderPtr myAppend = new log4cplus::FileAppender(filebuf);
	myAppend->setName("EmuFCrateAppender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	getApplicationLogger().addAppender(myAppend);

	// TEMP
	getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);

	// PGK is an idiot.  Forgetting this leads to disasters.
	TM = new emu::fed::IRQThreadManager(endcap_);

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
	//std::cout << "EmuFCrate: inside onSetTTSBits" << std::endl;
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP command: SetTTSBits");
	// PGK We don't need a state transition here.  This is a simple routine.
	//fireEvent("SetTTSBits");

	// set sTTS bits
	writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);
	// read back sTTS bits
	ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);

	//std::cout << "EmuFCrate: onSetTTSBits, ttsBits_=" << ttsBits_.toString() << std::endl;
	LOG4CPLUS_DEBUG(getApplicationLogger(), "ttsBits_=" << ttsBits_.toString());
	
	//std::string strmess;
	//message->writeTo(strmess);
	//std::cout << " Message:  "<< strmess << std::endl;

	//return createReply(message); // copy and modify this function here:

	// PGK Technically, nothing is expecting anything in particular from this
	//  routine.  As long as we don't send a SOAP message with a fault in it,
	//  the message is ignored.  So let's just send a dummy response.
/*
	std::string command = "";

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

	std::string strrply;
	reply->writeTo(strrply);
	std::cout << " Reply:  " << strrply << std::endl;


// JRG, try postSOAP:
	std::string klass="EmuFCrateManager";
	int instance = 0;
	std::cout << "  * EmuFCrate: trying postSOAP" << std::endl;
	xdaq::ApplicationDescriptor *app;
	try {
		app = getApplicationContext()->getDefaultZone()
		->getApplicationDescriptor(klass, instance);
		std::cout << "  * EmuFCrate: postSOAP, got application instance=" << instance << klass << std::endl;
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		std::cout << "  * EmuFCrate: postSOAP, application not found! " << instance << klass << std::endl;
		return reply; // Do nothing if the target doesn't exist
	}

	xoap::MessageReference rereply;

	// send the message
	std::cout << "  * EmuFCrate: onSetTTSBitsResponse, sending Soap Response" << std::endl;
	rereply = getApplicationContext()->postSOAP(reply, app);
	std::cout << "  * EmuFCrate: onSetTTSBitsResponse, got Soap rereply " << std::endl;
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
	//std::cout << "    enter EmuFCrate::configureAction " << std::endl;

	// Determine whether this is a local configure or a configure from SOAP
	if (soapLocal_) {
		soapLocal_ = false;
		soapConfigured_ = false;
	} else {
		soapConfigured_ = true;
	}

	// The run type is given to us via SOAP.  If it's not set yet, tough rocks.
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

	// PGK Easier parsing.  Less confusing.
	LOG4CPLUS_INFO(getApplicationLogger(),"EmuFCrate::configureAction using XML file " << xmlFile_.toString());
	emu::fed::FEDCrateParser parser;
	parser.parseFile(xmlFile_.toString().c_str());

	// From the parser, set the crates.
	crateVector.clear();
	crateVector = parser.getCrates();

	// Get the name of this endcap from the parser, too.  This is specified in the XML
	// for convenience.
	endcap_ = parser.getName();
	
	// First, we must make a system to parse out strings.  String-fu!
	// Strings looke like this:  "Crate# BHandle# Crate# BHandle# Crate# BHandle#..."
	std::map< int, int > BHandles;
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Got old handles: " << BHandles_.toString());

	// Parse out the string into handles (if they have already been opened by HyperDAQ)
	std::stringstream sHandles(BHandles_.toString());
	int buffer;
	while (sHandles >> buffer) {
		int crateNumber = buffer;
		sHandles >> buffer;
		int BHandle = buffer;
		
		BHandles[crateNumber] = BHandle;
	}

	// Now we have to see if we need new handles from the crate vector.
	std::ostringstream newHandles;

	for (std::vector< emu::fed::FEDCrate * >::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
		// The controller knows its handle.
		emu::fed::VMEController *myController = (*iCrate)->getController();

		// A handle of -1 means that opening of the handle failed, so someone has
		// already opened it (HyperDAQ)
		if (myController->getBHandle() == -1) {
			LOG4CPLUS_INFO(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has already been opened by someone else.  Looking for the BHandle...");

			// If that is the case, we can look up the handle.  Handles are stored by
			// create NUMBER (not index), so we need to check against our crate number.
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_INFO(getApplicationLogger(),"Found handle " << iHandle->second);
				(*iCrate)->setBHandle(iHandle->second);

				// In this case, just copy the old handle information to our new string.
				newHandles << iHandle->first << " " << iHandle->second << " ";
				
				break;
			}
		} else {

			// If the handle is a reasonable number, we assume that we created it.  Use it.
			LOG4CPLUS_INFO(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has been first opened by this application.  Saving the BHandle...");
			
			bool replaced = false;

			// It may be the case that the handle was opened at some point in the past,
			// then closed.  If that is the case, we need to reset the handle in the string.
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_INFO(getApplicationLogger(),"Resetting handle (was " << iHandle->second << ")");
				replaced = true;
				newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			}

			// If nothing was replaced (we didn't find the crate number in the previous
			// string), then we write a new one.
			if (!replaced) newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			
		}

	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Saving new handles: " << newHandles.str());
	
	BHandles_ = newHandles.str();


	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	for (std::vector< emu::fed::FEDCrate * >::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {

		// Only reset if we have a DCC in the crate.
		std::vector< emu::fed::DCC * > dccs = (*iCrate)->getDCCs();

		// Don't reset crate 5 (TF)
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_INFO(getApplicationLogger(), "HARD RESET THROUGH DCC!");
			try {
				dccs[0]->crateHardReset();
			} catch (emu::fed::FEDException &e) {
				LOG4CPLUS_ERROR(getApplicationLogger(), "CAEN error detected.");
			}
		}

		// Now we do the configure.  This is big.
		LOG4CPLUS_INFO(getApplicationLogger(), "Configuring crate " << (*iCrate)->number());
		(*iCrate)->configure();
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


	// PGK At this point, we need to check to see if the constants from the
	//  XML file have been properly loaded into the DDUs.  This will call
	//  updateFlashAction, which will automatically load everything as needed.
	for (std::vector< emu::fed::FEDCrate * >::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {

		std::vector<emu::fed::DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<emu::fed::DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {

			LOG4CPLUS_INFO(getApplicationLogger(), "Reading flash values for crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot());

			uint32_t flashKillFiber = (*iDDU)->readFlashKillFiber();
			uint32_t fpgaKillFiber = (*iDDU)->readKillFiber();
			uint32_t xmlKillFiber = (*iDDU)->getKillFiber();

			LOG4CPLUS_DEBUG(getApplicationLogger(), "killFiber: XML(" << std::hex << xmlKillFiber << std::dec << ") fpga(" << std::hex << fpgaKillFiber << std::dec << ") flash(" << std::hex << flashKillFiber << std::dec << ")");

			if ((flashKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
				LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML killFiber disagree:  reloading flash");
				(*iDDU)->writeFlashKillFiber(xmlKillFiber & 0x7fff);
			}
			if (fpgaKillFiber != xmlKillFiber) {
				LOG4CPLUS_INFO(getApplicationLogger(),"fpga and XML killFiber disagree:  reloading fpga");
				(*iDDU)->writeKillFiber(xmlKillFiber);
			}

			uint16_t fpgaGbEPrescale = (*iDDU)->readGbEPrescale();
			uint16_t xmlGbEPrescale = (*iDDU)->getGbEPrescale();
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "GbE_Prescale: XML(" << std::hex << xmlGbEPrescale << std::dec << ") fpga(" << std::hex << fpgaGbEPrescale << std::dec << ")");
			
			if ((fpgaGbEPrescale & 0xf) != xmlGbEPrescale) {
				LOG4CPLUS_INFO(getApplicationLogger(),"fpga and XML killFiber disagree:  reloading fpga");
				(*iDDU)->writeGbEPrescale(xmlGbEPrescale);
			}

			// Now we should check if the RUI matches the flash value and
			//  update it as needed.
			uint16_t flashRUI = (*iDDU)->readFlashRUI();
			uint16_t targetRUI = (*iCrate)->getRUI((*iDDU)->slot());

			LOG4CPLUS_DEBUG(getApplicationLogger(),"RUI: flash(" << flashRUI << ") calculated(" << targetRUI << ")");
			
			if (flashRUI != targetRUI) {
				LOG4CPLUS_INFO(getApplicationLogger(),"Flash and calculated RUI disagree:  reloading flash");
				(*iDDU)->writeFlashRUI(targetRUI);
			}
		}
	}

	int Fail=0;
	unsigned short int count=0;
	for(unsigned i = 0; i < crateVector.size(); ++i){
		// find DDUs in each crate
		std::vector<emu::fed::DDU *> myDdus = crateVector[i]->getDDUs();
		std::vector<emu::fed::DCC *> myDccs = crateVector[i]->getDCCs();

		// Set FMM error disable.  Not on TF, though
		if (crateVector[i]->number() < 5) {
			crateVector[i]->getBroadcastDDU()->writeFMM(0xFED0);
		}

		for(unsigned j =0; j < myDdus.size(); ++j){
			LOG4CPLUS_DEBUG(getApplicationLogger(), "checking DDU configure status, Crate " << crateVector[i]->number() << " slot " << myDdus[j]->slot());
			//std::cout << " EmuFCrate: Checking DDU configure status for Crate " << crateVector[i]->number() << " slot " << myDdus[j]->slot() << std::endl;

			if (myDdus[j]->slot() < 21){
				int FMMReg = myDdus[j]->readFMM();
				//int FMMReg = myDdus[j]->readFMMReg();
				//if(RegRead!=(0xFED0+(count&0x000f)))std::cout << "    fmmreg broadcast check is wrong, got " << std::hex << RegRead << " should be FED0+" << count << std::dec << std::endl;
				if (FMMReg!=(0xFED0)) {
					LOG4CPLUS_WARN(getApplicationLogger(), "fmmreg broadcast check is wrong, got " << std::hex << FMMReg << ", should be 0xFED" << count);
				}

				int CSCStat = myDdus[j]->readCSCStatus();
				unsigned long int dduFPGAStat = myDdus[j]->readFPGAStatus(emu::fed::DDUFPGA)&0xdecfffff;  // <<- note the mask
				unsigned long int inFPGA0Stat = myDdus[j]->readFPGAStatus(emu::fed::INFPGA0) & 0xf7eedfff;  // <<- note the mask
				unsigned long int inFPGA1Stat = myDdus[j]->readFPGAStatus(emu::fed::INFPGA1) & 0xf7eedfff;  // <<- note the mask

				LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU Status for slot " << std::dec << myDdus[j]->slot() << ": 0x" << std::hex << CSCStat << " 0x" << dduFPGAStat << " 0x" << inFPGA0Stat << " 0x" << inFPGA1Stat << std::dec);

				long int liveFibers = (myDdus[j]->readFiberStatus(emu::fed::INFPGA0)&0x000000ff) | ((myDdus[j]->readFiberStatus(emu::fed::INFPGA1)&0x000000ff)<<8);
				int killFiber = (myDdus[j]->readKillFiber() & 0x7fff);

				LOG4CPLUS_INFO(getApplicationLogger(), "liveFibers/killFibers for slot " << std::dec << myDdus[j]->slot() << ": 0x" << std::hex << liveFibers << " 0x" << killFiber << std::dec);

				unsigned long int thisL1A = myDdus[j]->readL1Scaler(emu::fed::DDUFPGA);
				LOG4CPLUS_DEBUG(getApplicationLogger(), "L1A Scalar for slot " << std::dec << myDdus[j]->slot() << ": " << thisL1A);

				if (inFPGA0Stat) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to INFPGA0 error");
					//std::cout<<"     * DDU configure failure due to INFPGA0 error *" <<std::endl;
				}
				if (inFPGA1Stat) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to INFPGA1 error");
					//std::cout<<"     * DDU configure failure due to INFPGA1 error *" <<std::endl;
				}
				if (dduFPGAStat) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to DDUFPGA error");
					//std::cout<<"     * DDU configure failure due to DDUFPGA error *" <<std::endl;
				}
				if (CSCStat) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to CSCStat error");
					//std::cout<<"     * DDU configure failure due to CSCstat error *" <<std::endl;
				}
				/*
				if (killFiber != (0x7fff - (liveFibers&0x7fff))) {
				//if (killFiber&0x7fff != liveFibers&0x7fff) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to liveFiber/killFiber mismatch.  liveFiber 0x" << std::hex << liveFibers << ", killfiber 0x" << killFiber);
					//std::cout<<"     * DDU configure failure due to Live Fiber mismatch *" <<std::endl; 76ff 77ff 79ff 7aff 7bff
				}
				*/
				if (thisL1A) {
					Fail++;
					LOG4CPLUS_ERROR(getApplicationLogger(), "DDU configure failure due to L1A Scaler not reset");
					//std::cout<<"     * DDU configure failure due to L1A scaler not Reset *" <<std::endl;
				}
				//if(Fail>0){
					//LOG4CPLUS_ERROR(getApplicationLogger(), Fail << " total DDU failures, throwing exception");
					//std::cout<<"   **** DDU configure has failed, setting EXCEPTION.  Fail=" << Fail <<std::endl;
				//}
			}
		}

		// Now check the fifoinuse parameter from the DCC
		std::vector<emu::fed::DCC *>::iterator idcc;
		for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
			int fifoinuse = (*idcc)->readFIFOInUse() & 0x3FF;
			if (fifoinuse != (*idcc)->getFIFOInUse()) {
				LOG4CPLUS_ERROR(getApplicationLogger(), "DCC configure failure due to FIFOInUse mismatch");
				//std::cout << "     * DCC configure failure due to FifoInUse mismatch *" << std::endl;
				//std::cout << "     * saw 0x" << std::hex << fifoinuse << std::dec << ", expected 0x" << std::hex << (*idcc)->fifoinuse_ << std::dec << " *" << std::endl;
				Fail++;
			}
		}
	}
	count++;

	//std::cout << "Now trying to fill dccInOut_" << std::endl;
	// At this point, we have crates, so we can set up the dccInOut_ std::vector.

	// On Failure of above, set this:	XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
	if (Fail) {
		//XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
		LOG4CPLUS_WARN(getApplicationLogger(), "number of failures: " << Fail << ".   Continuing in a possibly bad state (failure is not an option!)");
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Leaving EmuFCrate::configureAction");
	//std::cout << " EmuFCrate:  Received Message Configure" << std::endl;
	//std::cout << "    leave EmuFCrate::configureAction " << std:: std::endl;

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
	for (std::vector< emu::fed::FEDCrate * >::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
		std::vector< emu::fed::DCC * > dccs = (*iCrate)->getDCCs();
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_INFO(getApplicationLogger(), "SYNC RESET THROUGH DCC!");
			dccs[0]->crateSyncReset();
		}
	}

	// PGK You have to wipe the thread manager and start over.
	delete TM;
	TM = new emu::fed::IRQThreadManager(endcap_);
	for (unsigned int i=0; i<crateVector.size(); i++) {
		if (crateVector[i]->number() > 4) continue;
		TM->attachCrate(crateVector[i]);
	}
	// PGK We now have the run number from CSCSV
	TM->startThreads(runNumber_);
}



void EmuFCrate::disableAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Disable");
	std::cout << "Received Message Disable" << std::endl ;
	soapLocal_ = false;

	TM->endThreads();
	//TM->killThreads();
}



void EmuFCrate::haltAction(toolbox::Event::Reference e)
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Halt");
	std::cout << "Received Message Halt" << std::endl;
	soapConfigured_ = false;
	soapLocal_ = false;

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Calling IRQThreadManager::endThreads...");
	TM->endThreads();
	LOG4CPLUS_DEBUG(getApplicationLogger(), "...Returned from IRQThreadManager::endThreads.");

}



// HyperDAQ pages
void EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	std::stringstream sTitle;
	sTitle << "EmuFCrate(" << getApplicationDescriptor()->getInstance() << ") " << endcap_.toString();
	*out << Header(sTitle.str());

	// Manual state changing
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("Manual state changes")
		.set("class","legend") << std::endl;

	*out << cgicc::div();
	*out << "Present state: ";
	*out << cgicc::span(state_.toString())
		.set("class",state_.toString()) << std::endl;
	*out << cgicc::div();

	*out << cgicc::div() << std::endl;
	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action","/" + getApplicationDescriptor()->getURN() + "/Fire")
			.set("method","GET") << std::endl;
		if (state_.toString() == "Halted" || state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Configure") << std::endl;
		}
		if (state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Enable") << std::endl;
		}
		if (state_.toString() == "Enabled") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Disable") << std::endl;
		}
		if (state_.toString() == "Halted" || state_.toString() == "Configured" || state_.toString() == "Enabled" || state_.toString() == "Failed" || state_.toString() == STATE_UNKNOWN) {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Halt") << std::endl;
		}
		// PGK Command to update the Flashes on the DDUs
		// PGK Now done automatically at configure.
		/*
		if (state_.toString() == "Failed" || state_.toString() == "Configured") {
			*out << cgicc::br() << std::endl;
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","UpdateFlash") << std::endl;
		} else {
			*out << cgicc::br() << "Crate-wide flash updates can only be performed from the Configured or Failed states." << std::endl;
		}
		*/
		*out << cgicc::form() << std::endl;

	} else {
		*out << "EmuFCrate has been configured through SOAP." << std::endl;
		*out << cgicc::br() << "Send the Halt signal to manually change states." << std::endl;
	}
	*out << cgicc::div() << std::endl;
	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << std::endl;

	*out << cgicc::fieldset() << std::endl;

	// HyperDAQ?
	std::set<xdaq::ApplicationDescriptor * > hddescriptors =
		getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuFCrateHyperDAQ");

	if (hddescriptors.size()) {

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("EmuFCrateHyperDAQ")
			.set("class","legend") << std::endl;

		std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
		for ( itDescriptor = hddescriptors.begin(); itDescriptor != hddescriptors.end(); itDescriptor++ ) {
			if ((*itDescriptor)->getInstance() != getApplicationDescriptor()->getInstance()) continue;
			std::stringstream className;
			className << (*itDescriptor)->getClassName() << "(" << (*itDescriptor)->getInstance() << ")";
			std::stringstream url;
			url << (*itDescriptor)->getContextDescriptor()->getURL() << "/" << (*itDescriptor)->getURN();

			*out << cgicc::a(className.str())
				.set("href",url.str()) << std::endl;

		}

		*out << cgicc::fieldset() << std::endl;;
	}

	// sTTS control
	// PGK Let the CSCSV handle all these controls.
	/*
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("sTTS Control")
		.set("class","legend") << std::endl;

	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action", "/" + getApplicationDescriptor()->getURN() + "/SetTTSBits") << std::endl;

		*out << cgicc::div() << std::endl;
		*out << cgicc::span("Crate # (1-4): ") << std::endl;
		*out << cgicc::input()
			.set("name", "ttscrate")
			.set("type", "text")
			.set("value", ttsCrate_.toString())
			.set("size", "3") << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::div() << std::endl;
		*out << cgicc::span("Slot # (4-13): ") << std::endl;
		*out << cgicc::input()
			.set("name", "ttsslot")
			.set("type", "text")
			.set("value", ttsSlot_.toString())
			.set("size", "3") << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::div() << std::endl;
		*out << cgicc::span("TTS value (0-15, decimal): ") << std::endl;
		*out << cgicc::input()
			.set("name", "ttsbits")
			.set("type", "text")
			.set("value", ttsBits_.toString())
			.set("size", "3") << std::endl;
		*out << cgicc::div() << std::endl;

		*out << cgicc::input()
			.set("type", "submit")
			.set("name", "command")
			.set("value", "SetTTSBits") << std::endl;

		*out << cgicc::form() << std::endl;
	} else {
		*out << "EmuFCrate has been configured through SOAP." << std::endl;
		*out << cgicc::br() << "Send the Halt signal to manually set TTS bits." << std::endl;
	}

	*out << cgicc::fieldset() << std::endl;
	*/

	// IRQ Monitoring
	// Hide the TrackFinder...
	if (endcap_.toString() != "TrackFinder") {
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;

		if (state_.toString() == "Enabled") {
			*out << cgicc::div("IRQ Monitoring Enabled")
				.set("class","legend") << std::endl;

			for (std::vector<emu::fed::FEDCrate *>::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {

				int crateNumber = (*iCrate)->number();

				// Status table
				*out << cgicc::table()
					.set("style","width: 90%; margin: 10px auto 10px auto; border: solid 2px #009; border-collapse: collapse;") << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 12pt; font-weight: bold;") << std::endl;

				*out << cgicc::td()
					.set("colspan","6") << std::endl;
				*out << "Crate " << crateNumber << std::endl;
				*out << cgicc::td() << std::endl;

				*out << cgicc::tr() << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold;") << std::endl;

				*out << cgicc::td()
					.set("colspan","6") << std::endl;

				time_t startTime = TM->data()->startTime[(*iCrate)];
				time_t tickTime = TM->data()->tickTime[(*iCrate)];

				tm *startTimeInfo = localtime(&startTime);
				*out << "Thread started " << asctime(startTimeInfo) << cgicc::br();
				*out << TM->data()->ticks[(*iCrate)] << " ticks, ";
				tm *tickTimeInfo = localtime(&tickTime);
				*out << "last tick " << asctime(tickTimeInfo) << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold; border: solid 1px #000") << std::endl;
				*out << cgicc::td("Time of error") << std::endl;
				*out << cgicc::td("Slot") << std::endl;
				*out << cgicc::td("RUI") << std::endl;
				*out << cgicc::td("Fiber(s)") << std::endl;
				*out << cgicc::td("Chamber(s)") << std::endl;
				*out << cgicc::td("Action taken") << std::endl;
				*out << cgicc::tr() << std::endl;

				std::vector<emu::fed::IRQError *> errorVector = TM->data()->errorVectors[(*iCrate)];
				// Print something pretty if there is no error
				if (errorVector.size() == 0) {
					*out << cgicc::tr() << std::endl;
					*out << cgicc::td()
						.set("colspan","6")
						.set("style","border: 1px solid #000;")
						.set("class","undefined") << std::endl;
					*out << "No errors detected (yet)" << std::endl;
					*out << cgicc::td() << std::endl;
					*out << cgicc::tr() << std::endl;
				} else {

					for (std::vector<emu::fed::IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
						// Mark the error as grey if there has been a reset.
						std::string errorClass = "";
						std::string chamberClass = "error";
						if ((*iError)->reset) {
							errorClass = "undefined";
							chamberClass = "undefined";
						}

						*out << cgicc::tr() << std::endl;
						
						// Time
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						time_t interruptTime = (*iError)->errorTime;
						struct tm* interruptTimeInfo = localtime(&interruptTime);
						*out << asctime(interruptTimeInfo) << std::endl;
						*out << cgicc::td() << std::endl;

						// Slot
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iError)->ddu->slot() << std::endl;
						*out << cgicc::td() << std::endl;

						// RUI
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iCrate)->getRUI((*iError)->ddu->slot()) << std::endl;
						*out << cgicc::td() << std::endl;

						// Fibers
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) *out << iFiber << " ";
						}
						*out << cgicc::td() << std::endl;

						// Chambers
						*out << cgicc::td()
							.set("class",chamberClass)
							.set("style","border: 1px solid #000;") << std::endl;
						for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) {
								if (iFiber == 15) *out << "DDU ";
								else *out << (*iError)->ddu->getChamber(iFiber)->name() << " ";
								// Pointer-foo!
							}
						}
						*out << cgicc::td() << std::endl;

						// Action performed
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iError)->action << std::endl;
						*out << cgicc::td() << std::endl;
						
						*out << cgicc::tr() << std::endl;
					}
				}

				*out << cgicc::table() << std::endl;

			}

		} else {
			*out << cgicc::div("IRQ Monitoring Disabled")
				.set("class","legend") << std::endl;
			*out << cgicc::span("Set state to \"Enabled\" to begin IRQ monitoring threads.")
				.set("style","color: #A00; font-size: 11pt;") << std::endl;
		}

		*out << cgicc::fieldset() << std::endl;
	} // End hiding from TrackFinder.

	*out << Footer() << std::endl;
}



void EmuFCrate::webFire(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	cgicc::Cgicc cgi(in);
	soapLocal_ = true;

	std::string action = "";
	cgicc::form_iterator name = cgi.getElement("action");
	if(name != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
		std::cout << "webFire action: " << action << std::endl;
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

	std::cout << "EmuFCrate:  inside webSetTTSBits" << std::endl ;

	webRedirect(in, out);
}
*/


void EmuFCrate::webRedirect(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
	std::string url = in->getenv("PATH_TRANSLATED");

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}



std::string EmuFCrate::getCGIParameter(xgi::Input *in, std::string name)
{
	cgicc::Cgicc cgi(in);
	std::string value;

	cgicc::form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}




void EmuFCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception)
{
	std::cout << " stateChanged called " << std::endl;
	EmuApplication::stateChanged(fsm);

	std::stringstream log;
	log << "FSM state changed to " << state_.toString();
	LOG4CPLUS_INFO(getApplicationLogger(), log.str());
}



// PGK Ugly, but it must be done.  We have to update the parameters that the
//  EmuFCrateManager asks for or else they won't be updated!
xoap::MessageReference EmuFCrate::onGetParameters(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	if (state_.toString() == "Enabled") {
		// dccInOut update
		dccInOut_.clear();
		errorChambers_ = "";

		// PGK Update the DCC rate info.  This is confusing as all get-out.
		
		// FTW!
		std::ostringstream errorChamberString;
		
		for (std::vector<emu::fed::FEDCrate *>::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
			int crateNumber = (*iCrate)->number();
			if (crateNumber > 4) continue; // Skip TF
			std::vector<emu::fed::DCC *> myDccs = (*iCrate)->getDCCs();
			xdata::Vector<xdata::UnsignedInteger> crateV;
			//std::cout << "Updating getParameters Crate Number " << (*iCrate)->number() << std::endl;
			crateV.push_back((*iCrate)->number());
			std::vector<emu::fed::DCC *>::iterator idcc;
			for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
				unsigned short int status[6];
				int dr[6];
				for (int igu=0;igu<6;igu++) {
					status[igu]=(*idcc)->readRate(igu);
					dr[igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//std::cout << "Slink0: " << dr[0] << std::endl;

				for (int igu=6;igu<12;igu++) {
					status[igu-6]=(*idcc)->readRate(igu);
					dr[igu-6]=((status[igu-6]&0x3fff)<<(((status[igu-6]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//std::cout << "Slink1: " << dr[0] << std::endl;
			}
			//std::cout << "Pushing back." << std::endl;
			dccInOut_.push_back(crateV);
			
			// Now for the chamber errors from IRQ...

			std::vector<emu::fed::IRQError *> errorVector = TM->data()->errorVectors[(*iCrate)];
			for (std::vector<emu::fed::IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
				//LOG4CPLUS_DEBUG(getApplicationLogger(), "I think that there is an error on crate " << (*iCrate)->number() << " slot " << (*iError)->ddu->slot() << " with reset " << (*iError)->reset);
				// Skip things that have already been reset (we think)
				if ((*iError)->reset) continue;
				// Report the chamber names and RUI names that are in an error state.
				for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
					if ((*iError)->fibers & (1<<iFiber)) {
						//LOG4CPLUS_DEBUG(getApplicationLogger(), "I think that there is an error on crate " << (*iCrate)->number() << " slot " << (*iError)->ddu->slot() << " fiber " << iFiber << " with reset " << (*iError)->reset);
						if (errorChamberString.str() != "") errorChamberString << ", ";
						if (iFiber == 15) {
							errorChamberString << "RUI#" << (*iCrate)->getRUI((*iError)->ddu->slot());
						} else errorChamberString << (*iError)->ddu->getChamber(iFiber)->name() + " ";
					}
				}
			}
		}
		
		// FTW!
		errorChambers_ = errorChamberString.str();
	}
	// PGK Following is just what EmuFEDApplication does.

	xdata::soap::Serializer serializer;

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

	//bool first = true;

	std::map<std::string, xdata::Serializable *>::iterator iParam;
	for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
		std::string name = (*iParam).first;
		xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
		xoap::SOAPElement element = bodyElement.addChildElement(elementName);
		serializer.exportAll((*iParam).second, dynamic_cast<DOMElement*>(element.getDOMNode()), true);
		//first = false;
	}

	//reply->writeTo(std::cout);
	//std::cout << std::endl;
	return reply;

}


// Stolen from the now-defunct EmuFController
void EmuFCrate::writeTTSBits(int crate, int slot, unsigned int bits)
{
	
	//std::cout << "### EmuFController::writeTTSBits on " << crate << " " << slot << std::endl;
	for (std::vector<emu::fed::FEDCrate *>::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
		if ((*iCrate)->number() != crate) continue;
	
		if (slot == 8 || slot == 18) {
			
			std::vector<emu::fed::DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<emu::fed::DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
	
				(*iDCC)->writeFMM((bits | 0x10) & 0xffff);
			}
		} else {
			
			std::vector<emu::fed::DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<emu::fed::DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				
				(*iDDU)->writeFMM((bits | 0xf0e0) & 0xffff);
			}
		}
	}
}


// Stolen from the now-defunct EmuFController
unsigned int EmuFCrate::readTTSBits(int crate, int slot)
{
	
	//std::cout << "### EmuFController::readTTSBits on " << crate << " " << slot << std::endl;
	for (std::vector<emu::fed::FEDCrate *>::iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); iCrate++) {
		if ((*iCrate)->number() != crate) continue;
		
		if (slot == 8 || slot == 18) {
			
			std::vector<emu::fed::DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<emu::fed::DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
				
				return (*iDCC)->readFMM() & 0xf;
			}
		} else {
			
			std::vector<emu::fed::DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<emu::fed::DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				
				return (*iDDU)->readFMM() & 0xf;
			}
		}
	}
	
	return 0xface;

}

// End of file
// vim: set sw=4 ts=4:
