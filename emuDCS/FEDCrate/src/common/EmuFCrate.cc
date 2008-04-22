#include "EmuFCrate.h"

// addition for STEP
#include "Crate.h"
#include "DDU.h"
#include "DCC.h"
#include "IRQData.h"
// end addition for STEP


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

//#include "IRQThread.h"

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
	runNumber_(0)
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
	//xoap::bind(this, &EmuFCrate::onGetParameters, "GetParameters", XDAQ_NS_URI);

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
	fsm_.addStateTransition(
		'H', 'H', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	fsm_.addStateTransition(
		'C', 'C', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	fsm_.addStateTransition(
		'E', 'E', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);

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
	getApplicationInfoSpace()->fireItemAvailable("RunNumber", &runNumber_);

	// HyperDAQ pages
	xgi::bind(this, &EmuFCrate::webDefault, "Default");
	xgi::bind(this, &EmuFCrate::webFire, "Fire");
	xgi::bind(this, &EmuFCrate::webConfigure, "Configure");
	xgi::bind(this, &EmuFCrate::webSetTTSBits, "SetTTSBits");

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
	sprintf(filebuf,"EmuFCrate%d-%s.log",getApplicationDescriptor()->getInstance(),datebuf);

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

	cout << "    EmuFCrate::onConfigure: Fireevent Configure \n " << endl;
	
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Configure");
	
	fireEvent("Configure");
	cout <<"    EmuFCrate::onConfigure: return from fireevent configure " << endl;
	
	return createReply(message);
}



xoap::MessageReference EmuFCrate::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{

	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Enable");

	fireEvent("Enable");

	return createReply(message);
}



xoap::MessageReference EmuFCrate::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Remote SOAP state change requested: Disable");

	fireEvent("Disable");

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
	cout << "EmuFCrate: inside onSetTTSBits" << endl;
	fireEvent("SetTTSBits");

	// seems to occur last...but it gives the correct readback!
	cout << "EmuFCrate: onSetTTSBits, ttsBits_=" << ttsBits_.toString() << endl;
	string strmess;
	message->writeTo(strmess);
	cout << " Message:  "<< strmess << endl;



//	return createReply(message); // copy and modify this function here:
	string command = "";

	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(
			command + "Response", "xdaq", XDAQ_NS_URI);
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

	return reply;
}


/*
xoap::MessageReference EmuFCrate::onGetParameters(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{

	xoap::MessageReference reply = createReply(message);
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xdata::soap::Serializer serializer;
	serializer.exportAll(getApplicationInfoSpace(), dynamic_cast<DOMElement*>(body.getDOMNode()), true);
	
	reply->writeTo(cout);
	cout << endl;
	return reply;

}
*/


void EmuFCrate::configureAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
	// calls only EmuFController::init().
	// configure() is called in the nexe 'Enable' step.
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Configure");
	cout << "    enter EmuFCrate::configureAction " << endl;

	if (soapLocal_) {
		soapLocal_ = false;
		soapConfigured_ = false;
	} else {
		soapConfigured_ = true;
	}


// JRG: note that the HardReset & Resynch should already be done by this point!

	SetConfFile(xmlFile_);  // set it here automatically now, rather than
							// manual selection later with HyperDAQ
	cout << " EmuFCrate Configure from Soap: using file " << xmlFile_.toString() << endl;
	init();  // in real CMS running there will not be downloading of registers,
			// so this will just define the Crates/DDUs/DCCs for FED Apps.


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

	int Fail=0;
	unsigned short int count=0;
	std::vector<Crate*> myCrates = selector().crates();
	for(unsigned i = 0; i < myCrates.size(); ++i){
		// find DDUs in each crate
		std::vector<DDU*> myDdus = myCrates[i]->ddus();
		vector<DCC *> myDccs = myCrates[i]->dccs();
		for(unsigned j =0; j < myDdus.size(); ++j){
		  if(myDdus[j]->slot()==28){
		    cout <<"   Setting FMM Error Disable for Crate " << myCrates[i]->number() << endl;
		    myDdus[j]->vmepara_wr_fmmreg(0xFED0+(count&0x000f));
		  }
		}

		for(unsigned j =0; j < myDdus.size(); ++j){
			cout << " EmuFCrate: Checking DDU configure status for Crate " << myCrates[i]->number() << " slot " << myDdus[j]->slot() << endl;

			if (myDdus[j]->slot() < 21){
				unsigned short int RegRead=myDdus[j]->vmepara_rd_fmmreg();
				if(RegRead!=(0xFED0+(count&0x000f)))cout << "    fmmreg broadcast check is wrong, got " << hex << RegRead << " should be FED0+" << count << dec << endl;


				int mySlot=myDdus[j]->slot();
				//  myDdus[j]->ddu_reset(); // sync reset via VME
				//  unsigned long int thisL1A=myDdus[j]->ddu_rdscaler();

				unsigned short int CSCstat = myDdus[j]->vmepara_CSCstat();
				myDdus[j]->ddu_fpgastat(); //  & 0xdecfffff  <<- note the mask
				unsigned long int DDUfpgaStat=(((myDdus[j]->ddu_code1&0x0000ffff)<<16)|(myDdus[j]->ddu_code0&0x0000ffff))&0xdecfffff;  // <<- note the mask
				myDdus[j]->infpgastat(INFPGA0); // & 0xf7eedfff  <<- note the mask
				unsigned long int INfpga0Stat=(((myDdus[j]->infpga_code1&0x0000ffff)<<16)|(myDdus[j]->infpga_code0&0x0000ffff))&0xf7eedfff;  // <<- note the mask
				myDdus[j]->infpgastat(INFPGA1); // & 0xf7eedfff  <<- note the mask
				unsigned long int INfpga1Stat=(((myDdus[j]->infpga_code1&0x0000ffff)<<16)|(myDdus[j]->infpga_code0&0x0000ffff))&0xf7eedfff;  // <<- note the mask
				//	      cout << "   DDU Status for slot " << mySlot << " (hex) " << hex << CSCstat << " " << DDUfpgaStat << " " << INfpga0Stat << " " << INfpga1Stat << dec << endl;
				printf("   DDU Status for slot %2d: 0x%04x  0x%08x  0x%08x  0x%08x\n",mySlot,(unsigned int) CSCstat,(unsigned int) DDUfpgaStat,(unsigned int) INfpga0Stat,(unsigned int) INfpga1Stat);

				long int LiveFibers = (myDdus[j]->infpga_int_CheckFiber(INFPGA0)&0x000000ff);
				LiveFibers |= ((myDdus[j]->infpga_int_CheckFiber(INFPGA1)&0x000000ff)<<8);
				int killFiber = (myDdus[j]->read_int_page1()&0xffff);
				// cout << "   LiveFibers for slot " << mySlot << ": 0x" << hex << LiveFibers << dec << endl;
				// cout << "   killFiber for slot " << mySlot << ": 0x" << hex << killFiber << dec << endl;
				printf("   LiveFibers for slot %2d: 0x%04x     killFiber=%04x\n",mySlot,(unsigned int) LiveFibers,(unsigned int) killFiber);
				unsigned long int thisL1A = myDdus[j]->ddu_int_rdscaler();
				cout << "   L1A Scaler = " << thisL1A << endl;
				if(INfpga0Stat>0){
					Fail++;
					cout<<"     * DDU configure failure due to INFPGA0 error *" <<endl;
				}
				if(INfpga1Stat>0){
					Fail++;
					cout<<"     * DDU configure failure due to INFPGA1 error *" <<endl;
				}
				if(DDUfpgaStat>0){
					Fail++;
					cout<<"     * DDU configure failure due to DDUFPGA error *" <<endl;
				}
				if(CSCstat>0){
					Fail++;
					cout<<"     * DDU configure failure due to CSCstat error *" <<endl;
				}
				if(killFiber != (LiveFibers&0x0000ffff)){
					Fail++;
					cout<<"     * DDU configure failure due to Live Fiber mismatch *" <<endl;
				}
				if(thisL1A>0){
					Fail++;
					cout<<"     * DDU configure failure due to L1A scaler not Reset *" <<endl;
				}
				if(Fail>0){
					cout<<"   **** DDU configure has failed, setting EXCEPTION.  Fail=" << Fail <<endl;
				}
			}
		}
		
		// Now check the fifoinuse parameter from the DCC
		vector<DCC *>::iterator idcc;
		for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
			int fifoinuse = (*idcc)->mctrl_rd_fifoinuse() & 0x3FF;
			if (fifoinuse != (*idcc)->fifoinuse_) {
				cout << "     * DCC configure failure due to FifoInUse mismatch *" << endl;
				cout << "     * saw 0x" << hex << fifoinuse << dec << ", expected 0x" << hex << (*idcc)->fifoinuse_ << dec << " *" << endl;
				Fail++;
			}
		}
	}
	count++;

	//cout << "Now trying to fill dccInOut_" << endl;
	// At this point, we have crates, so we can set up the dccInOut_ vector.
	dccInOut_.clear();
	vector<Crate *>::iterator iCrate;
	for (iCrate = myCrates.begin(); iCrate != myCrates.end(); iCrate++) {
		vector<DCC *> myDccs = (*iCrate)->dccs();
		xdata::Vector<xdata::UnsignedInteger> crateV;
		//cout << "Crate Number " << (*iCrate)->number() << endl;
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
	}

// On Failure of above, set this:	XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
//	if(Fail>0) XCEPT_RAISE(toolbox::fsm::exception::Exception, "error in EmuFCrate::configureAction");
// JRG, finally we need to Broadcast the "FMM Error Disable" signal to both FED Crates.

	cout << " EmuFCrate:  Received Message Configure" << endl ;
	cout << "    leave EmuFCrate::configureAction " << std:: endl;

}



void EmuFCrate::enableAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_INFO(getApplicationLogger(), "Received SOAP message: Enable");
	soapLocal_ = false;

	configure();
	cout << "Received Message Enable" << endl ;
	
	// You have to wipe the thread manager and start over.
	TM = new IRQThreadManager();
	vector<Crate *> myCrates = getCrates();
	for (unsigned int i=0; i<myCrates.size(); i++) {
		TM->attachCrate(myCrates[i]);
	}
	TM->startThreads();
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



void EmuFCrate::setTTSBitsAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
	// set sTTS bits
	writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);

	// read back sTTS bits
	ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);

	// seems to occur next-to-last...
	cout << "EmuFCrate:  Received Message SetTTSBits" << endl ;
}



// HyperDAQ pages
void EmuFCrate::Title(xgi::Output *out, string myTitle = "EmuFCrate") {
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
	*out << cgicc::html() << endl;
	*out << cgicc::head() << endl;
	CSS(out);
	*out << cgicc::title(myTitle) << endl;
	
	// Auto-refreshing
	*out << meta()
		.set("http-equiv","refresh")
		.set("content","30;/" + getApplicationDescriptor()->getURN() ) << endl;
	
	*out << cgicc::head() << endl;

	*out << cgicc::br();
}



void EmuFCrate::CSS(xgi::Output *out) {
	*out << cgicc::style() << endl;
	*out << cgicc::comment() << endl;
	*out << "div.title { width: 80%; margin: 20px auto 20px auto; text-align: center; color: #000000; font-size: 16pt; font-weight: bold; }" << endl;
	*out << "fieldset.fieldset { width: 90%; margin: 10px auto 10px auto; padding: 2px 2px 2px 2px; border: 2px solid #555555; background-color: #EEEEEE; }" << endl;
	*out << "div.legend { width: 100%; padding-left: 20px; margin-bottom: 10px; color: #0000DD; font-size: 12pt; font-weight: bold; }" << endl;
	*out << ".Halted, .Enabled, .Disabled, .Configured, .Failed, .unknown { padding: 2px; background-color: #000; font-family: monospace;}" << endl;
	*out << ".Halted {color: #F99;}" << endl;
	*out << ".Enabled {color: #9F9;}" << endl;
	*out << ".Disabled {color: #FF9;}" << endl;
	*out << ".Configured {color: #99F;}" << endl;
	*out << ".Failed, .unknown {color: #F99; font-weight: bold; text-decoration: blink;}" << endl;
	*out << cgicc::comment() << endl;
	*out << cgicc::style() << endl;
}



void EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{

	ostringstream sTitle;
	sTitle << "EmuFCrate(" << getApplicationDescriptor()->getInstance() << ")";
	Title(out,sTitle.str());
	
	*out << cgicc::div(sTitle.str())
		.set("class","title") << endl;

	*out << br() << endl;

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
		*out << cgicc::form() << endl;
	
	} else {
		*out << "EmuFCrate has been configured through SOAP." << endl;
		*out << cgicc::br() << "Send the Halt signal to manually change states." << endl;
	}
	*out << cgicc::div() << endl;
	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << endl;
	*out << cgicc::br() << endl;
	
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
				.set("href",url.str())
				.set("target","_blank") << endl;

		}

		*out << cgicc::fieldset() << endl;;
	}

	// sTTS control
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
	
	
	*out << cgicc::fieldset()
		.set("class","fieldset") << endl;
	
	if (state_.toString() == "Enabled") {
		*out << cgicc::div("IRQ Monitoring Enabled")
			.set("class","legend") << endl;

		for (unsigned int i=0; i<getCrates().size(); i++) {
		
			int crateNumber = getCrates()[i]->number();
			
			// Status table
			*out << cgicc::table()
				.set("style","width: 90%; margin: 10px auto 10px auto; border: solid 2px #900; border-collapse: collapse;") << endl;
				
			*out << cgicc::tr()
				.set("style","background-color: #900; color: #FFF; text-align: center; font-size: 12pt; font-weight: bold;") << endl;
				
			*out << cgicc::td()
				.set("colspan","5") << endl;
			*out << "Crate " << crateNumber << endl;
			*out << cgicc::td() << endl;
			
			*out << cgicc::tr() << endl;
			
			*out << cgicc::tr()
				.set("style","background-color: #900; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold;") << endl;
				
			*out << cgicc::td()
				.set("colspan","5") << endl;
			
			time_t startTime = TM->data()->startTime[crateNumber];
			time_t tickTime = TM->data()->tickTime[crateNumber];
			
			tm *startTimeInfo = localtime(&startTime);
			*out << "[ Began " << asctime(startTimeInfo) << "-- ";
			*out << TM->data()->ticks[crateNumber] << " ticks -- ";
			tm *tickTimeInfo = localtime(&tickTime);
			*out << "last tick " << asctime(tickTimeInfo) << "]" << endl;
			*out << cgicc::td() << endl;
			*out << cgicc::tr() << endl;
			
			*out << cgicc::tr()
				.set("style","background-color: #900; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold; border: solid 1px #000") << endl;
			*out << cgicc::td("Slot") << endl;
			*out << cgicc::td("nIRQ") << endl;
			*out << cgicc::td("Last Time") << endl;
			*out << cgicc::td("Last Error") << endl;
			*out << cgicc::td("Accumulated Error") << endl;
			*out << cgicc::tr() << endl;

			for (int i=0; i<21; i++) {
				if (TM->data()->lastError[crateNumber][i] == 0) continue;
				
				*out << cgicc::tr() << endl;
				
				*out << cgicc::td()
					.set("style","border: 1px solid #000;") << endl;
				*out << i;
				*out << cgicc::td() << endl;
				
				*out << cgicc::td()
					.set("style","border: 1px solid #000;") << endl;
				*out << TM->data()->dduCount[crateNumber][i];
				*out << cgicc::td() << endl;
				
				*out << cgicc::td()
					.set("style","border: 1px solid #000;") << endl;
				time_t interruptTime = TM->data()->lastErrorTime[crateNumber][i];
				struct tm* interruptTimeInfo = localtime(&interruptTime);
				*out << asctime(interruptTimeInfo);
				*out << cgicc::td() << endl;

				*out << cgicc::td()
					.set("style","border: 1px solid #000;") << endl;
				*out << hex << TM->data()->lastError[crateNumber][i] << dec << "h";
				*out << cgicc::td() << endl;

				*out << cgicc::td()
					.set("style","border: 1px solid #000;") << endl;
				*out << hex << TM->data()->accError[crateNumber][i] << dec << "h";
				*out << cgicc::td() << endl;

				*out << tr() << endl;
				
			}

			*out << table() << endl;

		}

	} else {
		*out << cgicc::div("IRQ Monitoring Disabled")
			.set("class","legend") << endl;
		*out << cgicc::span("Set state to \"Enabled\" to begin IRQ monitoring threads.")
			.set("style","color: #A00; font-size: 11pt;") << endl;
	}
	
	*out << cgicc::fieldset() << endl;
	
	*out << cgicc::body() << endl;
	*out << cgicc::html() << endl;
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
		ostringstream log;
		log << "Local FSM state change requested: " << action;
		LOG4CPLUS_INFO(getApplicationLogger(), log.str());
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



void EmuFCrate::webSetTTSBits(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	/*
	ttsCrateStr_ = getCGIParameter(in, "ttscrate");
	ttsSlotStr_  = getCGIParameter(in, "ttsslot");
	ttsBitsStr_  = getCGIParameter(in, "ttsbits");
	*/

	ttsCrate_.fromString(getCGIParameter(in, "ttscrate"));
	ttsSlot_.fromString(getCGIParameter(in, "ttsslot"));
	ttsBits_.fromString(getCGIParameter(in, "ttsbits"));

	fireEvent("SetTTSBits");

	/*
	ttsCrateStr_ = ttsCrate_.toString();
	ttsSlotStr_  = ttsSlot_.toString();
	ttsBitsStr_  = ttsBits_.toString();
	*/

	cout << "EmuFCrate:  inside webSetTTSBits" << endl ;

	webRedirect(in, out);
}



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



//
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


// End of file
// vim: set sw=4 ts=4:
