#include "EmuTFMonitor.h"

#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "i2oEmuMonitorMsg.h"

XDAQ_INSTANTIATOR_IMPL(EmuTFMonitor)

EmuTFMonitor::EmuTFMonitor(xdaq::ApplicationStub *stub):xdaq::WebApplication(stub),Task("EmuTFMonitor"){
	// Needed by Web State Machine (?)
	xgi::bind(this, &EmuTFMonitor::Default,  "Default");
	xgi::bind(this, &EmuTFMonitor::dispatch, "dispatch");
	// Define Finite State Machine
	fsm_.addState('H',"Halted");
	fsm_.addState('R',"Ready");
	fsm_.addState('E',"Enabled");
	fsm_.addStateTransition('H','R',"Configuring", this, &EmuTFMonitor::Configuring);
	fsm_.addStateTransition('R','E',"Enabling",    this, &EmuTFMonitor::Enabling);
	fsm_.addStateTransition('E','H',"Halting",     this, &EmuTFMonitor::Halting);
	fsm_.addStateTransition('R','H',"Halting",     this, &EmuTFMonitor::Halting);
	fsm_.setInitialState('H');
	fsm_.reset();
	// Define Web State Machine
	wsm_.addState('H',"Halted",  this, &EmuTFMonitor::stateMachinePage);
	wsm_.addState('R',"Ready",   this, &EmuTFMonitor::stateMachinePage);
	wsm_.addState('E',"Enabled", this, &EmuTFMonitor::stateMachinePage);
	wsm_.addStateTransition('H','R', "Configure", this, &EmuTFMonitor::Configure, &EmuTFMonitor::failurePage);
	wsm_.addStateTransition('R','E', "Enable",    this, &EmuTFMonitor::Enable,    &EmuTFMonitor::failurePage);
	wsm_.addStateTransition('R','H', "Halt",      this, &EmuTFMonitor::Halt,      &EmuTFMonitor::failurePage);
	wsm_.addStateTransition('E','H', "Halt",      this, &EmuTFMonitor::Halt,      &EmuTFMonitor::failurePage);
	wsm_.setInitialState('H');
	// Publish application parameters
	getApplicationInfoSpace()->fireItemAvailable("inputDeviceName", &inputDeviceName_);
	// Define i2o callback 
	i2o::bind(this, &EmuTFMonitor::emuDataMsg, I2O_EMUTFMONITOR_CODE, XDAQ_ORGANIZATION_ID);

	try {
		const xdata::UnsignedLong committedPoolSize_ = 0x500000;
		//toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(committedPoolSize_);
		toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(0x500000);
		toolbox::net::URN urn("toolbox-mem-pool", "EmuTFMonitor_EMU_MsgPool");
		pool_ = toolbox::mem::getMemoryPoolFactory()->createPool(urn, a);
		//pool_->setHighThreshold( (unsigned long) (committedPoolSize_ * 0.7) );
		pool_->setHighThreshold( (unsigned long) (0x500000 * 0.7) );
	} catch(toolbox::mem::exception::Exception& e) {
		exit(0);
	}

}

void EmuTFMonitor::actionPerformed(xdata::Event& e){
/*  // update measurements monitors
  if (e.type() == "ItemRetrieveEvent")
    {
        std::string item = dynamic_cast<xdata::ItemRetrieveEvent&>(e).itemName();
        if ( item == "totalEvents")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Total Events : " << totalEvents_.toString());
        }
        else if ( item == "sessionEvents")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Session Events: " << sessionEvents_.toString());
        }
        else if ( item == "dataRate")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Data Rate: " << dataRate_.toString());
        }
        else if ( item == "readoutMode")
        {
          LOG4CPLUS_INFO(getApplicationLogger(), "Readout Mode : " << readoutMode_.toString());
          disableReadout();
          configureReadout();
        }
        else if ( item == "committedPoolSize")
        {
          LOG4CPLUS_INFO(getApplicationLogger(),
                         toolbox::toString("EmuMonitor's Tid: %d",
                                           i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor())) );
          setMemoryPool();
        }

        else if ( item == "serversClassName")
*/
}

void EmuTFMonitor::stateMachinePage(xgi::Output *out) throw (xgi::exception::Exception){
	*out<<cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict)<<std::endl;
	*out<<cgicc::html().set("lang", "en").set("dir","ltr")<<std::endl;

	xgi::Utils::getPageHeader
		(out,
		"EmuTFMonitor",
		getApplicationDescriptor()->getContextDescriptor()->getURL(),
		getApplicationDescriptor()->getURN(),
		"/daq/xgi/images/Application.gif"
		);

	std::string url = "/";
	url += getApplicationDescriptor()->getURN();
	url += "/dispatch";

	// display FSM
	std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
	std::set<std::string> allInputs = wsm_.getInputs();

	*out<<cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;

	*out<<"<table border cellpadding=10 cellspacing=0>"<<std::endl;
	*out<<"<tr>"<<std::endl;
	*out<<"<th>"<<wsm_.getStateName(wsm_.getCurrentState())<<"</th>"<<std::endl;
	*out<<"</tr>"<<std::endl;
	*out<<"<tr>"<<std::endl;
	for(std::set<std::string>::iterator i=allInputs.begin(); i!=allInputs.end(); i++){
		*out<<"<td>";
		*out<<cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data")<<std::endl;

		if( possibleInputs.find(*i) != possibleInputs.end() )
			*out<<cgicc::input().set("type","submit").set("name","StateInput").set("value",*i);
		else
			*out<<cgicc::input().set("type","submit").set("name","StateInput").set("value",*i).set("disabled", "true");

		*out<<cgicc::form();
		*out<<"</td>"<<std::endl;
	}
	*out<<"</tr>"<<std::endl;
	*out<<"</table>"<<std::endl;

	*out<<cgicc::hr()<<std::endl;
	
	//printParametersTable(out);
	xgi::Utils::getPageFooter(*out);
}

void EmuTFMonitor::failurePage(xgi::Output *out, xgi::exception::Exception& e) throw (xgi::exception::Exception){
	*out<<cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict)<<std::endl;
	*out<<cgicc::html().set("lang","en").set("dir","ltr")<<std::endl;
	xgi::Utils::getPageHeader
		(out, "EmuTFMonitor Failure",
		getApplicationDescriptor()->getContextDescriptor()->getURL(),
		getApplicationDescriptor()->getURN(),
		"/daq/xgi/images/Application.gif"
		);

	*out<<cgicc::br()<<e.what()<<cgicc::br()<<endl;
	std::string url = "/";
	url += getApplicationDescriptor()->getURN();

	*out<<cgicc::br()<<"<a href=\""<<url<<"\">"<<"retry"<<"</a>"<<cgicc::br()<<endl;

	xgi::Utils::getPageFooter(*out);
}

void EmuTFMonitor::Configure(xgi::Input *in) throw (xgi::exception::Exception){
	try {
		toolbox::Event::Reference e(new toolbox::Event("Configuring", this));
		fsm_.fireEvent(e);
	} catch (toolbox::fsm::exception::Exception& e){
		XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
	}
}

void EmuTFMonitor::Enable(xgi::Input *in) throw (xgi::exception::Exception){
	try {
		toolbox::Event::Reference e(new toolbox::Event("Enabling", this));
		fsm_.fireEvent(e);
	} catch (toolbox::fsm::exception::Exception& e) {
		XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
	}
}

void EmuTFMonitor::Halt(xgi::Input *in) throw (xgi::exception::Exception){
	try {
		toolbox::Event::Reference e(new toolbox::Event("Halting", this));
		fsm_.fireEvent(e);
	} catch (toolbox::fsm::exception::Exception& e) {
		XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
	}
}


void EmuTFMonitor::Configuring(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception){}
void EmuTFMonitor::Enabling   (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception){}
void EmuTFMonitor::Halting    (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception){}

int EmuTFMonitor::sendDataRequest(unsigned long last){
	//creditMsgsSent_++;
	//eventsRequested_ = eventsRequested_ +  nEventCredits_;
	toolbox::mem::Reference *ref = 0;
	try {
		const xdata::UnsignedLong maxFrameSize_ = sizeof(I2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME);
		//ref = toolbox::mem::getMemoryPoolFactory()->getFrame(pool_, maxFrameSize_);
		ref = toolbox::mem::getMemoryPoolFactory()->getFrame(pool_, sizeof(I2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME));
		PI2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME frame = (PI2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME) ref->getDataLocation();

		frame->PvtMessageFrame.StdMessageFrame.MsgFlags         = 0;
		frame->PvtMessageFrame.StdMessageFrame.VersionOffset    = 0;

		std::vector<xdaq::ApplicationDescriptor*> dataservers_ = getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuRUI");
		if( dataservers_.size()!=1 ) LOG4CPLUS_ERROR(getApplicationLogger(),"Number of EmuRUIs is not exactly one"); 

		frame->PvtMessageFrame.StdMessageFrame.TargetAddress    = i2o::utils::getAddressMap()->getTid(dataservers_[0]);
		frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress = i2o::utils::getAddressMap()->getTid(getApplicationDescriptor());
		frame->PvtMessageFrame.StdMessageFrame.MessageSize      = (sizeof(I2O_EMUTFMONITOR_CREDIT_MESSAGE_FRAME)) >> 2;

		frame->PvtMessageFrame.StdMessageFrame.Function = I2O_PRIVATE_MESSAGE;
		frame->PvtMessageFrame.XFunctionCode            = I2O_EMUTFMONITOR_CODE;
		frame->PvtMessageFrame.OrganizationID           = XDAQ_ORGANIZATION_ID;

		frame->nEventCredits    = 1;//nEventCredits_;
		frame->prescalingFactor = 1;//prescalingFactor_;

		ref->setDataSize(frame->PvtMessageFrame.StdMessageFrame.MessageSize << 2);
		getApplicationContext()->postFrame(ref,getApplicationDescriptor(),dataservers_[0]);

	} catch (toolbox::mem::exception::Exception & me) {
		//LOG4CPLUS_FATAL (getApplicationLogger(), xcept::stdformat_exception_history(me));
		return 1; // error
	} catch (xdaq::exception::Exception & e) {
/*		// Retry 3 times
		bool retryOK = false;
		for(int k=0; k<3; k++){
			try {
				getApplicationContext()->postFrame(ref,getApplicationDescriptor(),dataservers_[0]);
				retryOK = true;
				break; // if send was successfull, continue to send other messages
			} catch (xdaq::exception::Exception & re) {
				LOG4CPLUS_WARN (getApplicationLogger(), xcept::stdformat_exception_history(re));
			}
		}
*/	}
	return 0; // o.k.
}

void EmuTFMonitor::emuDataMsg(toolbox::mem::Reference *bufRef){
	// Emu-specific stuff

	//dataMessages_.push_back( bufRef );
	//eventsReceived_++;

	// Process the oldest message, i.e., the one at the front of the queue
	toolbox::mem::Reference *oldestMessage = bufRef;//dataMessages_.front();

	I2O_EMUTF_DATA_MESSAGE_FRAME *msg = (I2O_EMUTF_DATA_MESSAGE_FRAME*)oldestMessage->getDataLocation();

	char *startOfPayload = (char*) oldestMessage->getDataLocation() + sizeof(I2O_EMUTF_DATA_MESSAGE_FRAME);

	unsigned long sizeOfPayload = oldestMessage->getDataSize()-sizeof(I2O_EMUTF_DATA_MESSAGE_FRAME);
	unsigned long errorFlag = msg->errorFlag;


std::cout<<"Received a buffer"<<std::endl;

/*
	LOG4CPLUS_INFO(getApplicationLogger(),
		// "Received " << bufRef->getDataSize() <<
		"Received evt#" << eventsReceived_ << " (req: " << eventsRequested_ << ")" <<
		sizeOfPayload << " bytes, run " << msg->runNumber <<
		", errorFlag 0x"  << std::hex << msg->errorFlag << std::dec <<
		" from " << serversClassName_.toString() <<
		":" << msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress <<
		", still holding " << msg->nEventCreditsHeld << " event credits, " <<
		"pool size " << dataMessages_.size());
*/
	//processEvent(reinterpret_cast<const char*>(startOfPayload), sizeOfPayload, errorFlag);
	//usleep(2500);

	// Free the Emu data message
	bufRef->release();
	//dataMessages_.erase( dataMessages_.begin() );
}

int EmuTFMonitor::svc(void){
std::cout<<" Timer "<<std::endl;
return 0;
}
