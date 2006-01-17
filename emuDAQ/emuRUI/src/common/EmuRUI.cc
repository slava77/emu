#include "emuDAQ/emuRUI/include/EmuRUI.h"
#include "emuDAQ/emuRUI/include/EmuRUIV.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPHTMLHeader.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPPlainHeader.h"
#include "i2o/include/i2o/Method.h"
#include "interface/evb/include/i2oEVBMsgs.h"
#include "interface/shared/include/i2oXFunctionCodes.h"
#include "toolbox/include/toolbox/utils.h"
#include "toolbox/include/toolbox/fsm/FailedEvent.h"
#include "toolbox/include/toolbox/mem/HeapAllocator.h"
#include "toolbox/include/toolbox/task/Action.h"
#include "toolbox/include/toolbox/task/WorkLoop.h"
#include "toolbox/include/toolbox/task/WorkLoopFactory.h"
#include "xcept/include/xcept/tools.h"
#include "xdaq/include/xdaq/NamespaceURI.h"
#include "xdaq/include/xdaq/exception/ApplicationNotFound.h"
#include "xgi/include/xgi/Method.h"
#include "xoap/include/xoap/domutils.h"
#include "xoap/include/xoap/MessageFactory.h"
#include "xoap/include/xoap/MessageReference.h"
#include "xoap/include/xoap/Method.h"
#include "xoap/include/xoap/SOAPBody.h"
#include "xoap/include/xoap/SOAPBodyElement.h"
#include "xoap/include/xoap/SOAPEnvelope.h"

#include <unistd.h>

// EMu-specific stuff
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "emuDAQ/DDUReadout/include/FileReaderDDU.h"
#include "emuDAQ/DDUReadout/include/HardwareDDU.h"

// Alias used to access the "versioning" namespace EmuRUI from within the class EmuRUI
namespace EmuRUIV = EmuRUI;


EmuRUI::EmuRUI(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

logger_(Logger::getInstance(generateLoggerName())),

applicationBSem_(BSem::FULL)

{
    blocksArePendingTransmission_ = false;
    tid_                          = 0;
    workLoopActionSignature_      = 0;
    workLoop_                     = 0;
    workLoopStarted_              = false;
    ruiRuPool_                    = 0;
    superFragBlocks_.clear();


    workLoopFactory_ = toolbox::task::getWorkLoopFactory();
    i2oAddressMap_   = i2o::utils::getAddressMap();
    poolFactory_     = toolbox::mem::getMemoryPoolFactory();
    appInfoSpace_    = getApplicationInfoSpace();
    appDescriptor_   = getApplicationDescriptor();
    appContext_      = getApplicationContext();
    appGroup_        = appContext_->getApplicationGroup();
    xmlClass_        = appDescriptor_->getClassName();
    instance_        = appDescriptor_->getInstance();
    urn_             = appDescriptor_->getURN();

    appDescriptor_->setAttribute("icon",
        "/emu/emuDAQ/emuRUI/images/rui64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester(appGroup_);

    // Note that sentinel_ will be zero if the setinel application is not found
    sentinel_ = getSentinel(appContext_);

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &EmuRUI::onI2oException, "onI2oException");

    ruiRuPoolName_ = createRuiRuPoolName(instance_);
    ruiRuPool_ = createHeapAllocatorMemoryPool(poolFactory_, ruiRuPoolName_);

    try
    {
        defineFsm();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xdaq::exception::Exception,
            "Failed to define finite state machine", e);
    }

    // Initialise and group parameters
    stdConfigParams_  = initAndGetStdConfigParams();
    stdMonitorParams_ = initAndGetStdMonitorParams();

    // Create info space for monitoring
    monitoringInfoSpaceName_ =
        generateMonitoringInfoSpaceName(xmlClass_, instance_);
    monitoringInfoSpace_ =
        xdata::InfoSpace::get(monitoringInfoSpaceName_);

    // Fill the application's default info space
    putParamsIntoInfoSpace(stdConfigParams_ , appInfoSpace_);
    putParamsIntoInfoSpace(stdMonitorParams_, appInfoSpace_);

    // Fill the application's monitorable info space
    putParamsIntoInfoSpace(stdMonitorParams_, monitoringInfoSpace_);

    bindFsmSoapCallbacks();
    bindI2oCallbacks();

    // Bind web interface
    xgi::bind(this, &EmuRUI::css           , "styles.css");
    xgi::bind(this, &EmuRUI::defaultWebPage, "Default"   );

    // EMu-specific
    DQMonitorTid_   = 0;
    ruiDqmPoolName_ = createRuiDqmPoolName();
    ruiDqmPool_     = createCommittedHeapAllocatorMemoryPool(poolFactory_, ruiDqmPoolName_);
    dataForDQMArePendingTransmission_ = false;

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string EmuRUI::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    unsigned long               appInstance    = appDescriptor->getInstance();
    stringstream                oss;
    string                      loggerName;


    oss << appClass << appInstance;
    loggerName = oss.str();

    return loggerName;
}


void EmuRUI::clientCreditMsg(toolbox::mem::Reference *bufRef)
  // EMu-specific stuff
{
    I2O_TOYCLIENT_CREDIT_MESSAGE_FRAME *msg =
        (I2O_TOYCLIENT_CREDIT_MESSAGE_FRAME*)bufRef->getDataLocation();


    applicationBSem_.take();

    try
    {
        switch(fsm_.getCurrentState())
        {
        case 'H': // Halted
        case 'F': // Failed
            break;
        case 'E': // Enabled
            nEventCreditsHeld_.value_ += msg->nEventCredits;
	    LOG4CPLUS_INFO(logger_, 
			   "Received from DQM credits for " << msg->nEventCredits << 
			   " events prescaled by 1/" << msg->prescalingFactor << 
			   ". Now holding " << nEventCreditsHeld_ );
            break;
        case 'R': // Ready
        case 'S': // Suspended
            nEventCreditsHeld_.value_ += msg->nEventCredits;  // Hold credits
	    LOG4CPLUS_INFO(logger_, 
			   "Received from DQM credits for " << msg->nEventCredits << 
			   " events prescaled by 1/" << msg->prescalingFactor << 
			   ". Now holding " << nEventCreditsHeld_ );
            break;
        default:
            LOG4CPLUS_ERROR(logger_,
                "EmuRUI in undefined state");
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process client credit message : "
             << stdformat_exception_history(e));
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process client credit message : Unknown exception");
    }

    applicationBSem_.give();

    // Free the toyclient event credit message
    bufRef->release();
}

xdaq::ApplicationDescriptor *EmuRUI::getDQMonitor
(
    xdaq::ApplicationGroup *appGroup
)
  // EMu-specific stuff
{
    xdaq::ApplicationDescriptor *appDescriptor = 0;


    try
    {
        appDescriptor =
//             appGroup->getApplicationDescriptor("DQMonitor", 0);
            appGroup->getApplicationDescriptor("ToyClient", 0);
    }
    catch(xcept::Exception e)
    {
        appDescriptor = 0;
    }

    return appDescriptor;
}

xdaq::ApplicationDescriptor *EmuRUI::getRUBuilderTester
(
    xdaq::ApplicationGroup *appGroup
)
{
    xdaq::ApplicationDescriptor *appDescriptor = 0;


    try
    {
        appDescriptor =
            appGroup->getApplicationDescriptor("EmuDAQtester", 0);
    }
    catch(xcept::Exception e)
    {
        appDescriptor = 0;
    }

    return appDescriptor;
}


sentinel::Interface *EmuRUI::getSentinel(xdaq::ApplicationContext *appContext)
{
    xdaq::Application   *application = 0;
    sentinel::Interface *sentinel    = 0;


    try
    {
        application = appContext->getFirstApplication("Sentinel");

        LOG4CPLUS_INFO(logger_, "Found sentinel");
    }
    catch(xdaq::exception::ApplicationNotFound e)
    {
        LOG4CPLUS_WARN(logger_, "Did not find sentinel");

        return 0;
    }

    sentinel = dynamic_cast<sentinel::Interface*>(application);

    return sentinel;
}

xdata::UnsignedLong* EmuRUI::getRunNumber()
  // EMu-specific stuff
  // Gets the run number from TA
{
    xdaq::Application   *application = 0;
    xdata::UnsignedLong *runNumber   = 0;

    try
    {
        application = appContext_->getFirstApplication("EmuTA");
        LOG4CPLUS_INFO(logger_, "Found EmuTA");
    }
    catch(xdaq::exception::ApplicationNotFound e)
    {
        LOG4CPLUS_WARN(logger_, "Did not find EmuTA. ==> Run number will be unknown.");
        return 0;
    }

    xdata::InfoSpace *TAis = application->getApplicationInfoSpace();
//     cout << "Name of TA's infoSpace: " << TAis->name() << endl;

    try
    {
      xdata::Serializable *rn = TAis->find("runNumber");
      runNumber = dynamic_cast<xdata::UnsignedLong*>(rn);
      LOG4CPLUS_INFO(logger_, "Found run number in EmuTA: " << *runNumber );
    }
    catch(xdaq::exception::Exception e)
    {
      LOG4CPLUS_WARN(logger_, "Did not find runNumber in EmuTA");
      return 0;
    }

    return runNumber;
}


string EmuRUI::createRuiRuPoolName(const unsigned long emuRUIInstance)
{
    stringstream oss;
    string       s;

    oss << "EmuRUI" << emuRUIInstance << "/" << "RU" << instance_ << "_Pool";
    s = oss.str();

    return s;
}

string EmuRUI::createRuiDqmPoolName()
  // EMu-specific
{
    stringstream oss;

    oss << "EmuRUI" << instance_ << "_DQM_Pool";

    return oss.str();
}


toolbox::mem::Pool *EmuRUI::createHeapAllocatorMemoryPool
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    const string                     poolName
)
throw (emuRUI::exception::Exception)
{
    try
    {
        toolbox::net::URN urn("toolbox-mem-pool", poolName);
        toolbox::mem::HeapAllocator* a = new toolbox::mem::HeapAllocator();
        toolbox::mem::Pool *pool = poolFactory->createPool(urn, a);

        return pool;
    }
    catch (xcept::Exception e)
    {
        string s = "Failed to create pool: " + poolName;

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to create pool: " + poolName +
                   " : Unknown exception";

        XCEPT_RAISE(emuRUI::exception::Exception, s);
    }
}

toolbox::mem::Pool *EmuRUI::createCommittedHeapAllocatorMemoryPool
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    const string                     poolName
)
throw (emuRUI::exception::Exception)
{
    try
    {
        toolbox::net::URN urn("toolbox-mem-pool", poolName);
        toolbox::mem::CommittedHeapAllocator* a = new toolbox::mem::CommittedHeapAllocator(ruiDqmPoolSize_.value_);
        toolbox::mem::Pool *pool = poolFactory->createPool(urn, a);
	pool->setHighThreshold ( (unsigned long) (ruiDqmPoolSize_ * 0.7));

        return pool;
    }
    catch (xcept::Exception e)
    {
        string s = "Failed to set up commited pool: " + poolName;

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to set up commited pool: " + poolName +
                   " : Unknown exception";

        XCEPT_RAISE(emuRUI::exception::Exception, s);
    }
}


void EmuRUI::defineFsm()
throw (emuRUI::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &EmuRUI::stateChanged);
        fsm_.addState('R', "Ready"    , this, &EmuRUI::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &EmuRUI::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &EmuRUI::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &EmuRUI::enableAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &EmuRUI::haltAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &EmuRUI::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &EmuRUI::haltAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &EmuRUI::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &EmuRUI::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to reset FSM", e);
    }
}


string EmuRUI::generateMonitoringInfoSpaceName
(
    const string        appClass,
    const unsigned long appInstance
)
{
    stringstream oss;
    string       name;

    oss << "urn:xdaq-monitorable:" << appClass << ":" << appInstance;
    name = oss.str();

    return name;
}


vector< pair<string, xdata::Serializable*> > EmuRUI::initAndGetStdConfigParams()
{
//     unsigned int firstSourceId = 0;
//     unsigned int lastSourceId  = 0;
//     unsigned int sourceId      = 0;
    vector< pair<string, xdata::Serializable*> > params;


    workLoopName_   = "";
    dataBufSize_    = 0x4000;//=16kB //4096;//=4kB
    fedPayloadSize_ = 2048;
    threshold_      = 0x4000000;//=64MB // 0x8000000;//=128MB //67108864; // 64 MB

//     // Default is 8 FEDs per super-fragment
//     // Trigger has FED source id 0, RU0 has 1 to 8, RU1 has 9 to 16, etc.
//     firstSourceId = (instance_ * 8) + 1;
//     lastSourceId  = (instance_ * 8) + 8;

//     for(sourceId=firstSourceId; sourceId<=lastSourceId; sourceId++)
//     {
//         fedSourceIds_.push_back(sourceId);
//     }

    params.push_back(pair<string,xdata::Serializable *>
        ("workLoopName", &workLoopName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("dataBufSize", &dataBufSize_));
//     params.push_back(pair<string,xdata::Serializable *>
//         ("fedPayloadSize", &fedPayloadSize_));
    params.push_back(pair<string,xdata::Serializable *>
        ("threshold", &threshold_));
//     params.push_back(pair<string,xdata::Serializable *>
//         ("fedSourceIds", &fedSourceIds_));

    //
    // EMu-specific stuff
    //
    maxEvents_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("maxEvents", &maxEvents_));
    DDUMode_ = "file";
    params.push_back(pair<string,xdata::Serializable *>
		     ("DDUMode", &DDUMode_));
    maxDDUReadFailures_ = 1;
//     params.push_back(pair<string,xdata::Serializable *>
// 		     ("maxDDUReadFailures", &maxDDUReadFailures_));
    nDDUs_   = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("nDDUs", &nDDUs_));
    // Init DDUInput_ and DDUSourceId_ vectors.
    int maxDDUs = 8;
    for( int iDDU=0; iDDU<maxDDUs; ++iDDU ){
      DDUInput_.push_back("not used");
//       DDUSourceId_.push_back(0);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("DDUInput", &DDUInput_));
//     params.push_back(pair<string,xdata::Serializable *> 
// 		     ("DDUSourceId", &DDUSourceId_));

//     // Also, name their elements as long as PropertiesEditor is buggy.
//     for( unsigned int iDDU=0; iDDU<DDUInput_.size(); ++iDDU ){
//       stringstream oss;
//       oss << "DDUInput" << iDDU;
//       params.push_back(pair<string,xdata::Serializable *> 
// 		     ( oss.str(), &DDUInput_.at(iDDU)));
// //       oss.str("");
// //       oss << "DDUSourceId" << iDDU;
// //       params.push_back(pair<string,xdata::Serializable *> 
// // 		     ( oss.str(), &DDUSourceId_.at(iDDU)));
//     }

    pathToDataOutFile_   = "/tmp";
    fileSizeInMegaBytes_ = 2;
    params.push_back(pair<string,xdata::Serializable *>
		     ("pathToRUIDataOutFile"  , &pathToDataOutFile_   ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("ruiFileSizeInMegaBytes", &fileSizeInMegaBytes_ ));
    passDataOnToRUBuilder_ = true;
    params.push_back(pair<string,xdata::Serializable *>
		     ("passDataOnToRUBuilder", &passDataOnToRUBuilder_));

    sendDQMDataOnRequestOnly_ = true;
    params.push_back(pair<string,xdata::Serializable *>
		     ("sendDQMDataOnRequestOnly", &sendDQMDataOnRequestOnly_));
    prescalingForDQM_ = 100;
    params.push_back(pair<string,xdata::Serializable *>
		     ("prescalingForDQM", &prescalingForDQM_));
    ruiDqmPoolSize_ = 0x100000; // 1MB
    params.push_back(pair<string,xdata::Serializable *>
		     ("ruiDqmPoolSize", &ruiDqmPoolSize_));

    return params;
}


vector< pair<string, xdata::Serializable*> > EmuRUI::initAndGetStdMonitorParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    stateName_   = "Halted";
    eventNumber_ = 1;

    params.push_back(pair<string,xdata::Serializable *>
        ("stateName", &stateName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("eventNumber", &eventNumber_));


    //
    // EMu-specific stuff
    //
    nEvents_ = 1;
    params.push_back(pair<string,xdata::Serializable *>
		     ("nEvents", &nEvents_));

    nEventCreditsHeld_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("nEventCreditsHeld", &nEventCreditsHeld_));

    return params;
}


void EmuRUI::putParamsIntoInfoSpace
(
    vector< pair<string, xdata::Serializable*> > &params,
    xdata::InfoSpace                             *s
)
{
    vector< pair<string, xdata::Serializable*> >::iterator pos;

    for(pos = params.begin(); pos != params.end(); pos++)
    {
        s->fireItemAvailable(pos->first, pos->second);
    }
}


void EmuRUI::stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
throw (toolbox::fsm::exception::Exception)
{
    toolbox::fsm::State state = fsm.getCurrentState();


    try
    {
        stateName_ = fsm.getStateName(state);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to set exported parameter stateName", e);
    }
}


void EmuRUI::bindFsmSoapCallbacks()
{
    xoap::bind(this, &EmuRUI::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuRUI::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &EmuRUI::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &EmuRUI::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);
}


xoap::MessageReference EmuRUI::processSoapFsmCmd(xoap::MessageReference msg)
throw (xoap::exception::Exception)
{
    string cmdName = "";


    try
    {
        cmdName = extractCmdNameFromSoapMsg(msg);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to extract command name from SOAP command message";

        LOG4CPLUS_ERROR(logger_,
            s << " : " << xcept::stdformat_exception_history(e));
        XCEPT_RETHROW(xoap::exception::Exception, s, e);
    }

    try
    {
        processFsmCommand(cmdName);
        return createFsmResponseMsg(cmdName, stateName_.toString());
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to process command: " << cmdName;
        s = oss.str();

        LOG4CPLUS_ERROR(logger_,
            s << " : " << xcept::stdformat_exception_history(e));
        XCEPT_RETHROW(xoap::exception::Exception, s, e);
    }
    catch(...)
    {
        stringstream oss;
        string       s;

        oss << "Failed to process command: " << cmdName << ". ";
        oss << "Unknown exception";
        s = oss.str();

        LOG4CPLUS_ERROR(logger_, s);
        XCEPT_RAISE(xoap::exception::Exception, s);
    }
}


string EmuRUI::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emuRUI::exception::Exception)
{
    xoap::SOAPPart     part      = msg->getSOAPPart();
    xoap::SOAPEnvelope env       = part.getEnvelope();
    xoap::SOAPBody     body      = env.getBody();
    DOMNode            *node     = body.getDOMNode();
    DOMNodeList        *bodyList = node->getChildNodes();
    DOMNode            *command  = 0;
    string             cmdName   = "";
    unsigned int       i         = 0;


    for(i=0; i<bodyList->getLength(); i++)
    {
        command = bodyList->item(i);

        if(command->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            cmdName = xoap::XMLCh2String(command->getLocalName());
            return cmdName;
        }
    }

    XCEPT_RAISE(emuRUI::exception::Exception, "Command name not found");
}


xoap::MessageReference EmuRUI::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emuRUI::exception::Exception)
{
    try
    {
        xoap::MessageReference message = xoap::createMessage();
        xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
        xoap::SOAPBody body = envelope.getBody();
        string responseString = cmd + "Response";
        xoap::SOAPName responseName =
            envelope.createName(responseString, "xdaq", XDAQ_NS_URI);
        xoap::SOAPBodyElement responseElement =
            body.addBodyElement(responseName);
        xoap::SOAPName stateName =
            envelope.createName("state", "xdaq", XDAQ_NS_URI);
        xoap::SOAPElement stateElement =
            responseElement.addChildElement(stateName);
        xoap::SOAPName attributeName =
            envelope.createName("stateName", "xdaq", XDAQ_NS_URI);


        stateElement.addAttribute(attributeName, state);

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuRUI::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


void EmuRUI::processFsmCommand(const string cmdName)
throw (emuRUI::exception::Exception)
{
    toolbox::Event::Reference evtRef(new toolbox::Event(cmdName, this));


    try
    {
        applicationBSem_.take();
        fsm_.fireEvent(evtRef);
        applicationBSem_.give();
    }
    catch(xcept::Exception e)
    {
        applicationBSem_.give();

        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        applicationBSem_.give();

        XCEPT_RAISE(emuRUI::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}


void EmuRUI::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    try
    {
        tid_ = i2oAddressMap_->getTid(appDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception, 
            "Failed to get the I2O TID of this application", e);
    }

    // Avoid repeated function calls to obtain RU descriptor and tid
    try
    {
        ruDescriptor_ = appGroup_->getApplicationDescriptor("RU", instance_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xdaq::exception::Exception,
            "Failed to get the descriptor of this application", e);
    }
    try
    {
        ruTid_ = i2oAddressMap_->getTid(ruDescriptor_);
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to get the I2O TID of RU" << instance_;
        s = oss.str();

        XCEPT_RETHROW(xdaq::exception::Exception, s ,e);
    }

    ruiRuPool_->setHighThreshold(threshold_ - dataBufSize_);

    LOG4CPLUS_INFO(logger_,
                      "Set threshold of event data pool (dataBufSize="
                   << dataBufSize_ << ") to "
                   << threshold_);

    //
    // EMu-specific stuff
    //
    runNumber_ = *getRunNumber();

    if ( nDDUs_.value_ == (unsigned int) 0 ) {
      LOG4CPLUS_FATAL(logger_, "Number of DDUs is zero?!");
//       XCEPT_RAISE(xcept::Exception, "Number of DDUs is zero?!");
      XCEPT_RAISE(xdaq::exception::Exception, "Number of DDUs is zero?!");
    }
    nDDUReadFailures_ = new unsigned int[nDDUs_];
    DDU_              = new DDUReader*[nDDUs_];
    for( unsigned int iDDU=0; iDDU<nDDUs_; ++iDDU ){
      LOG4CPLUS_INFO(logger_, "Creating " << DDUMode_.toString() << 
		     " reader for " << DDUInput_.at(iDDU).toString());
      try {
	if      ( DDUMode_ == "hardware" )
	  DDU_[iDDU] = new HardwareDDU( DDUInput_.at(iDDU).toString() );
	else if ( DDUMode_ == "file"     ) 
	  DDU_[iDDU] = new FileReaderDDU( DDUInput_.at(iDDU).toString() );
	else     LOG4CPLUS_ERROR(logger_,"Bad DDU mode: " << DDUMode_.toString() << 
				 "Use \"file\" or \"hardware\"");
      }
      catch(xcept::Exception e){
	nDDUReadFailures_[iDDU] = maxDDUReadFailures_;

        stringstream oss;
        oss << "Failed to create " << DDUMode_.toString() << 
	  " reader for " << DDUInput_.at(iDDU).toString(); 
//         XCEPT_RETHROW(toolbox::fsm::exception::Exception, oss.str(), e);
//         XCEPT_RETHROW(xdaq::exception::Exception, oss.str(), e);
      }
    }

    iCurrentDDU_ = 0;

    // Just in case there's a writer, terminate it in an orderly fashion
    if ( fileWriter_ )
      {
	fileWriter_->endRun();
	delete fileWriter_;
	fileWriter_ = NULL;
      }

    nEvents_ = 1;

    DQMonitorDescriptor_ = getDQMonitor(appGroup_);
    if ( DQMonitorDescriptor_ ){
      LOG4CPLUS_INFO(logger_,"DQM found.");
      try
	{
	  DQMonitorTid_ = i2oAddressMap_->getTid(DQMonitorDescriptor_);
	}
      catch(xcept::Exception e)
	{
	  LOG4CPLUS_WARN(logger_,"Failed to get DQM's Tid.");
	}
    }
    else{
      LOG4CPLUS_INFO(logger_,"No DQM found.");
    }

  // Emu: start work loop upon enable, not upon config
//     if(!workLoopStarted_)
//     {
//         workLoopActionSignature_ = toolbox::task::bind
//         (
//             this,
//             &EmuRUI::workLoopAction,
//             "EmuRUI work loop action"
//         );

//         if(workLoopName_ == "")
//         {
//             stringstream oss;
//             oss << xmlClass_ << instance_ << "WorkLoop";
//             workLoopName_ = oss.str();
//         }

//         try
//         {
//             workLoop_ =
//                 workLoopFactory_->getWorkLoop(workLoopName_, "waiting");
//         }
//         catch(xcept::Exception e)
//         {
//             string s = "Failed to get work loop : " + workLoopName_.toString();

//             XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
//         }

//         try
//         {
//             workLoop_->submit(workLoopActionSignature_);
//         }
//         catch(xcept::Exception e)
//         {
//             string s = "Failed to submit action to work loop : " +
//                        workLoopName_.toString();


//             XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
//         }

//         if(!workLoop_->isActive())
//         {
//             try
//             {
//                 workLoop_->activate();

//                 LOG4CPLUS_INFO(logger_,
//                       "Activated work loop : " << workLoopName_.toString());
//             }
//             catch(xcept::Exception e)
//             {
//                 string s = "Failed to active work loop : " +
//                            workLoopName_.toString();

//                 XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
//             }
//         }

//         workLoopStarted_ = true;
//     }
}


void EmuRUI::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing

  // Emu: start work loop upon enable, not upon config
    if(!workLoopStarted_)
    {
        workLoopActionSignature_ = toolbox::task::bind
        (
            this,
            &EmuRUI::workLoopAction,
            "EmuRUI work loop action"
        );

        if(workLoopName_ == "")
        {
            stringstream oss;
            oss << xmlClass_ << instance_ << "WorkLoop";
            workLoopName_ = oss.str();
        }

        try
        {
            workLoop_ =
                workLoopFactory_->getWorkLoop(workLoopName_, "waiting");
        }
        catch(xcept::Exception e)
        {
            string s = "Failed to get work loop : " + workLoopName_.toString();

            XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
        }

        try
        {
            workLoop_->submit(workLoopActionSignature_);
        }
        catch(xcept::Exception e)
        {
            string s = "Failed to submit action to work loop : " +
                       workLoopName_.toString();


            XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
        }

        if(!workLoop_->isActive())
        {
            try
            {
                workLoop_->activate();

                LOG4CPLUS_INFO(logger_,
                      "Activated work loop : " << workLoopName_.toString());
            }
            catch(xcept::Exception e)
            {
                string s = "Failed to active work loop : " +
                           workLoopName_.toString();

                XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
            }
        }

        workLoopStarted_ = true;
    }
}


void EmuRUI::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
//     vector<toolbox::mem::Reference*>::iterator pos;
    deque<toolbox::mem::Reference*>::iterator pos; // BK
    toolbox::mem::Reference *bufRef = 0;


    for(pos=superFragBlocks_.begin(); pos!=superFragBlocks_.end(); pos++)
    {
        bufRef = *pos;
        bufRef->release();
    }

    superFragBlocks_.clear();

    blocksArePendingTransmission_ = false;

    // Reset event number
    eventNumber_ = 1;

//     // Reset super-fragment generator
//     superFragGenerator_.startSuperFragment(0, 0, 0, 0);

    // EMu specific
    if ( fileWriter_ ){
      fileWriter_->endRun();
      delete fileWriter_;
      fileWriter_ = NULL;
    }
    workLoopStarted_  = false;
    nDDUReadFailures_ = 0;
}


void EmuRUI::failAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
    try
    {
        toolbox::fsm::FailedEvent &failedEvent =
            dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
        xcept::Exception exception = failedEvent.getException();

        stringstream oss;
        string       s;

        oss << "Failure occurred when performing transition from: ";
        oss << failedEvent.getFromState();
        oss <<  " to: ";
        oss << failedEvent.getToState();
        oss << " exception history: ";
        oss << xcept::stdformat_exception_history(exception);
        s = oss.str();

        LOG4CPLUS_FATAL(logger_, s);
    }
    catch(bad_cast)
    {
        LOG4CPLUS_FATAL(logger_, "Moving to Failed state");
    }
}


void EmuRUI::bindI2oCallbacks()
{
    // Do nothing

  // EMu-specific stuff
  i2o::bind(this, &EmuRUI::clientCreditMsg, I2O_TOYCLIENT_CODE, XDAQ_ORGANIZATION_ID );
}


void EmuRUI::css
(
    xgi::Input  *in,
    xgi::Output *out
)
throw (xgi::exception::Exception)
{
    out->getHTTPResponseHeader().addHeader("Content-Type", "text/css");

    *out << "body"                                                     << endl;
    *out << "{"                                                        << endl;
    *out << "background-color: white;"                                 << endl;
    *out << "font-family:Arial;"                                       << endl;
    *out << "}"                                                        << endl;
    *out                                                               << endl;
    *out << ".app_links"                                               << endl;
    *out << "{"                                                        << endl;
    *out << "font-size: 14px;"                                         << endl;
    *out << "line-height: 14px;"                                       << endl;
    *out << "}"                                                        << endl;
    *out                                                               << endl;
    *out << "table.params th"                                          << endl;
    *out << "{"                                                        << endl;
    *out << "color: white;"                                            << endl;
    *out << "background-color: #66F;"                                  << endl;
    *out << "}"                                                        << endl;
}


void EmuRUI::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << EmuRUIV::versions    << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/emuDAQ/emuRUI/images/rui64x64.gif\""   << endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << EmuRUIV::versions
        << " " << stateName_.toString() << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"70\">" << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\">"             << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=\"/daq/xdaq/hyperdaq/images/HyperDAQ.jpg\""    << endl;
    *out << "       alt=\"HyperDAQ\""                                  << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\">"             << endl;
    *out << "      HyperDAQ"                                           << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;

    if(rubuilderTesterDescriptor_ != 0)
    {
        *out << "  <td width=\"32\">"                                  << endl;
        *out << "  </td>"                                              << endl;
        *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">"
            << endl;
        *out << "    <a href=";
        *out << "\"" << getHref(rubuilderTesterDescriptor_) << "\">"   << endl;
        *out << "      <img"                                           << endl;
        *out << "     src=\"/emu/emuDAQ/emuDAQtester/images/rubtest64x64.gif\"";
        *out << endl;
        *out << "       alt=\"Tester\""                                << endl;
        *out << "       width=\"64\""                                  << endl;
        *out << "       height=\"64\""                                 << endl;
        *out << "       border=\"\"/>"                                 << endl;
        *out << "    </a>"                                             << endl;
        *out << "    <a href=";
        *out << "\"" << getHref(rubuilderTesterDescriptor_) << "\">"   << endl;
        *out << "      Tester"                                         << endl;
        *out << "    </a>"                                             << endl;
        *out << "  </td>"                                              << endl;
    }

    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "<hr/>"                                                    << endl;

    *out << "<table>"                                                  << endl;
    *out << "<tr valign=\"top\">"                                      << endl;
    *out << "  <td>"                                                   << endl;
    try
    {
        printParamsTable(in, out, "Configuration", stdConfigParams_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print standard configuration table", e);
    }
    *out << "  </td>"                                                  << endl;
    *out << "  <td width=\"64\">"                                      << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td>"                                                   << endl;
    try
    {
        printParamsTable(in, out, "Monitoring", stdMonitorParams_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print standard monitoring table", e);
    }
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</body>"                                                  << endl;

    *out << "</html>"                                                  << endl;
}


void EmuRUI::printParamsTable
(
    xgi::Input                                   *in,
    xgi::Output                                  *out,
    const string                                 name,
    vector< pair<string, xdata::Serializable*> > &params
)
throw (xgi::exception::Exception)
{
    vector< pair<string, xdata::Serializable*> >::iterator pos;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"  << endl;

    *out << "  <tr>"                                                   << endl;
    *out << "    <th colspan=2>"                                       << endl;
    *out << "      " << name                                           << endl;
    *out << "    </th>"                                                << endl;
    *out << "  </tr>"                                                  << endl;


    for(pos = params.begin(); pos != params.end(); ++pos)
    {

      //
      // EMu-specific stuff
      //
//       cout << pos->second->type() << "    " << pos->first << "    " << pos->second << endl;
      
      if ( pos->second->type() == "vector" ){

	// Q: How do I determine the type of a xdata::Vector's elements 
	// when it's only known as a xdata::Serializable?

// 	xdata::Vector<xdata::String> *ulv = 
// 	  dynamic_cast<xdata::Vector<xdata::String>* > (pos->second); // this works, but begs the question...
// 	cout << "vector    " << ulv->elementAt(0)->type() << endl;

//  	xdata::Vector<xdata::Serializable> *xsv = 
// 	  dynamic_cast<xdata::Vector<xdata::Serializable> * > (pos->second); // this yields xsv=NULL...

//  	xdata::Vector<xdata::Serializable> *xsv = 
// 	  reinterpret_cast<xdata::Vector<xdata::Serializable> * > (pos->second); // this doesn't work

 	xdata::Vector<xdata::Serializable> *xsv = 
	  static_cast<xdata::Vector<xdata::Serializable> * > (pos->second); // that's it!

// 	    xdata::Vector<xdata::Serializable>::iterator xsv_it;
// 	    for ( xsv_it=xsv->begin(); xsv_it != xsv->end(); ++xsv_it )
// 	      cout << "   type " << xsv_it->type() << endl; // crashes on second iteration...

	for ( int i=0; i<xsv->elements(); ++i ){
// 	  cout << "   type " << xsv->elementAt(i)->type() << "    " << endl;
	  
	  *out << "  <tr>"                                               << endl;
	  
	  // Name
	  *out << "    <td>"                                             << endl;
	  *out << "      " << pos->first << "[" << i << "]"              << endl;
	  *out << "    </td>"                                            << endl;
	  
	  // Value
	  *out << "    <td>"                                             << endl;
	  *out << "      " << serializableScalarToString(xsv->elementAt(i))    << endl;
	  *out << "    </td>"                                            << endl;
	  
	  *out << "  </tr>"                                              << endl;

	}

      }
      else{

        *out << "  <tr>"                                               << endl;

        // Name
        *out << "    <td>"                                             << endl;
        *out << "      " << pos->first                                 << endl;
        *out << "    </td>"                                            << endl;

        // Value
        *out << "    <td>"                                             << endl;
        *out << "      " << serializableScalarToString(pos->second)    << endl;
        *out << "    </td>"                                            << endl;

        *out << "  </tr>"                                              << endl;
      }
    }

    *out << "</table>"                                                 << endl;
}


string EmuRUI::serializableScalarToString(xdata::Serializable *s)
{
    if(s->type() == "unsigned long") return serializableUnsignedLongToString(s);
    if(s->type() == "double"       ) return serializableDoubleToString(s);
    if(s->type() == "string"       ) return serializableStringToString(s);
    if(s->type() == "bool"         ) return serializableBooleanToString(s);

    return "Unsupported type";
}


string EmuRUI::serializableUnsignedLongToString(xdata::Serializable *s)
{
    xdata::UnsignedLong *v = dynamic_cast<xdata::UnsignedLong*>(s);

    return v->toString();
}


string EmuRUI::serializableDoubleToString(xdata::Serializable *s)
{
    xdata::Double *v = dynamic_cast<xdata::Double*>(s);

    return v->toString();
}


string EmuRUI::serializableStringToString(xdata::Serializable *s)
{
    xdata::String *v  = dynamic_cast<xdata::String*>(s);
    string        str = v->toString();


    if(str == "")
    {
        str = "\"\"";
    }

    return str;
}


string EmuRUI::serializableBooleanToString(xdata::Serializable *s)
{
    xdata::Boolean *v = dynamic_cast<xdata::Boolean*>(s);

    return v->toString();
}


// bool EmuRUI::workLoopAction(toolbox::task::WorkLoop *wl)
// {
//     try
//     {
//         applicationBSem_.take();

//         toolbox::fsm::State state = fsm_.getCurrentState();

//         switch(state)
//         {
//         case 'H':  // Halted
//         case 'R':  // Ready
//             break;
//         case 'E':  // Enabled
//             processAndCommunicate();
//             break;
//         default:
//             // Should never get here
//             LOG4CPLUS_FATAL(logger_,
//                 "EmuRUI" << instance_ << " is in an undefined state");
//         }

//         applicationBSem_.give();

//         // Reschedule this action code
//         return true;
//     }
//     catch(xcept::Exception e)
//     {
//         LOG4CPLUS_FATAL(logger_,
//             "Failed to execute \"self-driven\" behaviour"
//             << " : " << xcept::stdformat_exception_history(e));

//         try
//         {
//             // Move to the failed state
//             toolbox::Event::Reference evtRef(new toolbox::Event("Fail", this));
//             fsm_.fireEvent(evtRef);
//             applicationBSem_.give();
//         }
//         catch(xcept::Exception e)
//         {
//             applicationBSem_.give();

//             LOG4CPLUS_FATAL(logger_,
//                 "Failed to move to the Failed state : "
//                 << xcept::stdformat_exception_history(e));
//         }

//         // Do not reschedule this action code as the application has failed
//         return false;
//     }
// }

bool EmuRUI::workLoopAction(toolbox::task::WorkLoop *wl)
{
    try
    {
      bool isToBeRescheduled = true;

        applicationBSem_.take();

        toolbox::fsm::State state = fsm_.getCurrentState();

        switch(state)
        {
        case 'H':  // Halted
        case 'R':  // Ready
            break;
        case 'E':  // Enabled
            isToBeRescheduled = processAndCommunicate();
            break;
        default:
            // Should never get here
            LOG4CPLUS_FATAL(logger_,
                "EmuRUI" << instance_ << " is in an undefined state");
        }

        applicationBSem_.give();

        // Reschedule this action code
        return isToBeRescheduled;
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_FATAL(logger_,
            "Failed to execute \"self-driven\" behaviour"
            << " : " << xcept::stdformat_exception_history(e));

        try
        {
            // Move to the failed state
            toolbox::Event::Reference evtRef(new toolbox::Event("Fail", this));
            fsm_.fireEvent(evtRef);
            applicationBSem_.give();
        }
        catch(xcept::Exception e)
        {
            applicationBSem_.give();

            LOG4CPLUS_FATAL(logger_,
                "Failed to move to the Failed state : "
                << xcept::stdformat_exception_history(e));
        }

        // Do not reschedule this action code as the application has failed
        return false;
    }
}

bool EmuRUI::processAndCommunicate()
{

//             // see if we need to send empty events before this one
//             while(sendEmptyEvents_ && (nEvents_ < eventn)) {
//               sendEmptyEvent(nEvents_);
//               LOG4CPLUS_INFO(logger_, "adding empty event " << nEvents_ << " from source " << sourceId_ ));
//               ++nEvents_;
//             }
//             // see if the event needs a Common Data Format header
//             if(addCDF_) {
//               addCDFAndSend(data, dataLength, eventn, ruTid_); 
//             } else {
// 	      sendMessage(data, dataLength, eventn, ruTid_);
//             }

// 	    LOG4CPLUS_DEBUG(logger_, "frame sent to " << ruTid_);

//             // write event locally, if asked to
// 	    if (eventWriter_ != NULL) {
// 		eventWriter_->writeEvent(data, dataLength);
// 	    }

//             // see if the event needs to be pushed to the data quality monitor
//             // maybe there's a more efficient thing to do than repacking.
//             if(prescaleForPushingToDQM_ != 0 && 
//                dataQualityMonitorTid_ != NIL_TARGET_ID) {
//               // only send if it passes the prescale
//               if((eventNumber_ % prescaleForPushingToDQM_) == 0) {
// 		if(addCDF_){
// 		  //The dqm would like to know where the data is coming from
// 		  //if we are reading from a source with no CDF header/trailer
// 		  //so add one and send.
// 		  addCDFAndSend(data,dataLength,eventn,dataQualityMonitorTid_);
// 		}
// 		else{
//                 sendMessage(data, dataLength, 
//                                  eventn, dataQualityMonitorTid_);
// 		}
// 		LOG4CPLUS_DEBUG(logger_, "frame sent to " << dataQualityMonitorTid_);
//               }	      
//             }
    
  bool keepRunning = true;

    if( blocksArePendingTransmission_ || dataForDQMArePendingTransmission_ )
    {
      if( blocksArePendingTransmission_ )
        try
        {
            sendNextPendingBlock();
        }
        catch(xcept::Exception e)
        {
            LOG4CPLUS_WARN(logger_,
                "Failed to send data block to RU" << instance_ << "."
                << "Will try again later");
        }
      if( dataForDQMArePendingTransmission_ )
        try
        {
            sendNextPendingBlockToDQM();
        }
        catch(xcept::Exception e)
        {
            LOG4CPLUS_WARN(logger_,
                "Failed to send data block to DQM" << instance_ << "."
                << "Will try again later");
        }
    }
    else
    {
        try
        {
            keepRunning = continueConstructionOfSuperFrag();
        }
        catch(xcept::Exception e)
        {
            LOG4CPLUS_ERROR(logger_,
                "Failed to contnue construction of super-fragment"
                << " : " << stdformat_exception_history(e));
        }
    }


  return keepRunning;
}

// void EmuRUI::processAndCommunicate()
// {
//     if(blocksArePendingTransmission_)
//     {
//         try
//         {
//             sendNextPendingBlock();
//         }
//         catch(xcept::Exception e)
//         {
//             LOG4CPLUS_WARN(logger_,
//                 "Failed to send data block to RU" << instance_ << "."
//                 << "Will try again later");
//         }
//     }
//     else
//     {
//         try
//         {
//             continueConstructionOfSuperFrag();
//         }
//         catch(xcept::Exception e)
//         {
//             LOG4CPLUS_ERROR(logger_,
//                 "Failed to contnue construction of super-fragment"
//                 << " : " << stdformat_exception_history(e));
//         }
//     }
// }


void EmuRUI::sendNextPendingBlock()
throw (emuRUI::exception::Exception)
{
    try
    {
        appContext_->postFrame
        (
            superFragBlocks_.front(),
            appDescriptor_,
            ruDescriptor_,
            i2oExceptionHandler_,
            ruDescriptor_
        );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to send block to RU" << ruDescriptor_->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }

    superFragBlocks_.erase(superFragBlocks_.begin());

    blocksArePendingTransmission_ = superFragBlocks_.size() > 0;
}

void EmuRUI::sendNextPendingBlockToDQM()
throw (emuRUI::exception::Exception)
  // EMu-specific
{
    try
    {
        appContext_->postFrame
        (
            dataBlocksForDQM_.front(),
            appDescriptor_,
            DQMonitorDescriptor_,
            i2oExceptionHandler_,
            DQMonitorDescriptor_
        );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to send block to DQM" << DQMonitorDescriptor_->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }

    dataBlocksForDQM_.erase(dataBlocksForDQM_.begin());

    dataForDQMArePendingTransmission_ = dataBlocksForDQM_.size() > 0;

    if ( !dataForDQMArePendingTransmission_  && nEventCreditsHeld_.value_ ) --nEventCreditsHeld_;
}


bool EmuRUI::continueConstructionOfSuperFrag()
throw (emuRUI::exception::Exception)
{

  bool keepRunning;

  if ( maxEvents_.value_ > 0 && nEvents_.value_ > maxEvents_.value_ ) return false;

  // If the EmuRUI to RU memory pool has room for another data block
  if(!ruiRuPool_->isHighThresholdExceeded()){

    keepRunning = DDU_[iCurrentDDU_]->readNextEvent();

    if ( !keepRunning ) {
      LOG4CPLUS_ERROR(logger_, "DDU[" << iCurrentDDU_ << "] read error.");
      if ( fileWriter_ ){
	  fileWriter_->endRun();
	  delete fileWriter_;
	  fileWriter_ = NULL;
	}
    }

    char* data;
    int   dataLength  = 0;
    int   eventNumber = 0;

    if ( keepRunning ) {
//     if ( true ) { // let's see those too short events too !!!
      
      data       = DDU_[iCurrentDDU_]->data();
      dataLength = DDU_[iCurrentDDU_]->dataLength();
      if ( dataLength>0 ) eventNumber = DDU_[iCurrentDDU_]->eventNumber();

      dataLength = getDDUDataLengthWithoutPadding(data,dataLength);

//       if ( nEvents_ % 100 == 0 )
      LOG4CPLUS_INFO(logger_, 
		     "Read event " << eventNumber                           << 
		     " of "        << nEvents_                              <<
		     " from "      << DDUInput_.at(iCurrentDDU_).toString() <<
		     ", size: "    << dataLength   
		     );

      if ( nEvents_ == (unsigned long) 1 ) // first event --> a new run
	if ( iCurrentDDU_ == 0 ) // don't do it for all DDU's...
	  {
	    // terminate old writer, if any
	    if ( fileWriter_ )
	      {
		fileWriter_->endRun();
		delete fileWriter_;
	      fileWriter_ = NULL;
	      }
	    // create new writer if path is not empty
	    if ( pathToDataOutFile_ != string("") && fileSizeInMegaBytes_ > (long unsigned int) 0 ){
	      stringstream ss;
	      ss << "EmuRUI" << instance_;
	      fileWriter_ = new FileWriter( 1000000*fileSizeInMegaBytes_, pathToDataOutFile_.toString(), ss.str(), &logger_ );
	    }
	    if ( fileWriter_ ) fileWriter_->startNewRun( runNumber_.value_ );
	  }
    }
    
    if ( fileWriter_ )
      {
	if ( iCurrentDDU_ == 0 ) // don't start a new event for each DDU...
	  fileWriter_->startNewEvent();
	fileWriter_->writeData( data, dataLength );
      }

    // fill block and append it to superfragment
    if ( passDataOnToRUBuilder_ )
      appendNewBlockToSuperFrag( data, dataLength );

    // fill block and append it to data for DQM
    bool isToBeSentToDQM = false;
    if ( sendDQMDataOnRequestOnly_ ){
      if ( nEventCreditsHeld_.value_ && prescalingForDQM_.value_ ){
	if ( nEvents_.value_ % prescalingForDQM_.value_ == 1 ){
	  if( !ruiDqmPool_->isHighThresholdExceeded() )
	    isToBeSentToDQM = true;
	  else  LOG4CPLUS_WARN(logger_, "EmuRUI to DQM memory pool's high threshold exceeded.");
	}
      }
    }
    else if ( prescalingForDQM_.value_ ){
      if ( nEvents_.value_ % prescalingForDQM_.value_ == 1 ){
	if( !ruiDqmPool_->isHighThresholdExceeded() )
	  isToBeSentToDQM = true;
	else  LOG4CPLUS_WARN(logger_, "EmuRUI to DQM memory pool's high threshold exceeded.");
      }
    }

    if ( isToBeSentToDQM ) 
      try{
	appendNewBlockToDataForDQM( data, dataLength );
      }
      catch(xcept::Exception e){
	LOG4CPLUS_ERROR(logger_,
			"Failed to append data to be sent to DQM"
			<< " : " << stdformat_exception_history(e));
      }
    

    if ( iCurrentDDU_ +1 == nDDUs_ ){ // superfragment ready

      if ( passDataOnToRUBuilder_ ){
	// Prepare it for sending to the RU
	setNbBlocksInSuperFragment(superFragBlocks_.size());
	
	// Current super-fragment is now ready to be sent to the RU
	blocksArePendingTransmission_ = true;
      }

      if ( isToBeSentToDQM )
	dataForDQMArePendingTransmission_ = true;
      
      //
      nEvents_++;
    }

    iCurrentDDU_++;
    iCurrentDDU_ %= nDDUs_;

  }
  else  LOG4CPLUS_WARN(logger_, "EmuRUI to RU memory pool's high threshold exceeded.");
  
//   return true;
  return keepRunning;




//     // If there is no super-fragment under construction
//     if(superFragGenerator_.reachedEndOfSuperFragment())
//     {
//         superFragGenerator_.startSuperFragment
//         (
//             fedPayloadSize_,
//             &fedSourceIds_,
//             ruTid_,
//             eventNumber_
//         );

//         // The event number of CMS will be 24-bits
//         // 2 to the power of 24 = 16777216
//         eventNumber_ = (eventNumber_ + 1) % 16777216;
//     }
//     // Else if the EmuRUI to RU memory pool has room for another data block
//     else if(!ruiRuPool_->isHighThresholdExceeded())
//     {
//         try
//         {
//             appendNewBlockToSuperFrag();
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string       s;

//             oss << "Failed to append new block to super-fragment";
//             oss << " : " << stdformat_exception_history(e);
//             s = oss.str();

//             XCEPT_RETHROW(rui::exception::Exception, s, e);
//         }

//         // If the super-fragment under construction is complete
//         if(superFragGenerator_.reachedEndOfSuperFragment())
//         {
//              // Prepare it for sending to the RU
//              setNbBlocksInSuperFragment(superFragBlocks_.size());

//              // Current super-fragment is now ready to be sent to the RU
//              blocksArePendingTransmission_ = true;
//         }
//     }

//     return true;

}


// void EmuRUI::appendNewBlockToSuperFrag()
// throw (rui::exception::Exception)
// {
//     toolbox::mem::Reference *bufRef = 0;

//     // Get a free block from the EmuRUI/RU pool
//     try
//     {
//         bufRef = poolFactory_->getFrame(ruiRuPool_, dataBufSize_);
//     }
//     catch(xcept::Exception e)
//     {
//         stringstream oss;
//         string       s;

//         oss << "Failed to allocate a data block from the ";
//         oss << ruiRuPoolName_ << " pool";
//         oss << " : " << stdformat_exception_history(e);
//         s = oss.str();

//         XCEPT_RETHROW(rui::exception::Exception, s, e);
//     }

//     cout 
//       << "bufRef " << bufRef->getDataOffset()
//       << ",  "     << bufRef->getDataSize() << endl;

//     // Fill block with super-fragment data
//     try
//     {
//         superFragGenerator_.fillBlock(bufRef, dataBufSize_);
//     }
//     catch(xcept::Exception e)
//     {
//         stringstream oss;
//         string       s;

//         oss << "Failed to fill super-fragment data block";
//         oss << " : " << stdformat_exception_history(e);
//         s = oss.str();

//         LOG4CPLUS_ERROR(logger_, s);
//     }

//     // Append block to super-fragment under construction
//     superFragBlocks_.push_back(bufRef);

//     printSuperFragment();
// }


void EmuRUI::printData(char* data, const int dataLength){
  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
  cout << "_________________________________" << endl;
  cout << "                +3   +2   +1   +0" << endl;
  for(int i = 0; i < dataLength/2; i+=4)
    {
      std::cout << std::dec;
      std::cout.width(8); std::cout.fill(' ');
      std::cout << i;
      std::cout << "      ";
      std::cout << std::hex;
      std::cout.width(4); std::cout.fill('0');    
      std::cout << shortData[i+3] << " ";
      std::cout.width(4); std::cout.fill('0');    
      std::cout << shortData[i+2] << " ";
      std::cout.width(4); std::cout.fill('0');    
      std::cout << shortData[i+1] << " ";
      std::cout.width(4); std::cout.fill('0');    
      std::cout << shortData[i  ] << std::endl;
    }
  std::cout<<std::dec;
  std::cout.width(0);
}

int EmuRUI::getDDUDataLengthWithoutPadding(char* data, const int dataLength){
  // EMu-specific stuff
  // Get the data length without the padding that may have been added by Gbit Ethernet

  const int minEthPacketSize   = 32; // short (2-byte) words --> 64 bytes
  const int DDUTrailerLength   = 12; // short (2-byte) words --> 24 bytes

  if ( !dataLength ) return 0;
  if ( dataLength%2 ) LOG4CPLUS_ERROR(logger_, "DDU data is odd number of bytes (" << dataLength << ") long" );
  if ( dataLength<DDUTrailerLength*2 ) LOG4CPLUS_ERROR(logger_, 
				      "DDU data is shorter (" << dataLength << " bytes) than trailer" );

//   printData(data,dataLength);

  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
  int strippedDataLength    = dataLength/2; // short (2-byte) words
  // Let's go backward looking for trailer signatures:
  for ( int iShort=dataLength/2-DDUTrailerLength; 
	iShort>=0 && iShort>=dataLength/2-(minEthPacketSize+DDUTrailerLength); 
	--iShort ){
    if ( (shortData[iShort+11] & 0xf000) == 0xa000 ) // probably the trailer
      // Double check:
      if ( shortData[iShort  ]             == 0x8000 &&
	   shortData[iShort+1]             == 0x8000 &&
	   shortData[iShort+2]             == 0xFFFF &&
	   shortData[iShort+3]             == 0x8000 &&
	   (shortData[iShort+4] & 0xfff0)  == 0x0000 &&
	   (shortData[iShort+5] & 0x8000)  == 0x0000    ){
// 	LOG4CPLUS_DEBUG(logger_, "Trailer found");
	strippedDataLength = iShort + DDUTrailerLength;
	break;
      }
  }

  return 2*strippedDataLength;

}

void EmuRUI::appendNewBlockToSuperFrag( char* data, unsigned long dataLength )
throw (emuRUI::exception::Exception)
{
    toolbox::mem::Reference *bufRef = 0;

    unsigned long dataBufSize = sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) 
                                + dataLength;

    // Get a free block from the RUI/RU pool
    try
    {
        bufRef = poolFactory_->getFrame(ruiRuPool_, dataBufSize );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to allocate a data block from the ";
        oss << ruiRuPoolName_ << " pool";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }


    // Fill block with super-fragment data
    try
    {
         fillBlock( bufRef, data, dataLength );
//     cout 
//       << "bufRef " << bufRef->getDataOffset()
//       << ", "      << bufRef->getDataSize() 
//       << " data "  << data << endl;
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to fill super-fragment data block";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        LOG4CPLUS_ERROR(logger_, s);
    }

    // Append block to super-fragment under construction
    superFragBlocks_.push_back(bufRef);
    
//     printBlocks( superFragBlocks_ );
}

void EmuRUI::appendNewBlockToDataForDQM( char* data, unsigned long dataLength )
throw (emuRUI::exception::Exception)
  // EMu-specific stuff
{
    toolbox::mem::Reference *bufRef = 0;

    unsigned long dataBufSize = sizeof(I2O_RUI_DATA_MESSAGE_FRAME) 
                                + dataLength;

    // Get a free block from the RUI/DQM pool
    try
      {
        bufRef = poolFactory_->getFrame(ruiDqmPool_, dataBufSize );
      }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to allocate a data block from the ";
        oss << ruiDqmPoolName_ << " pool";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }


    // Fill block with DDUs' data
    try
    {
         fillBlockForDQM( bufRef, data, dataLength );
//     cout 
//       << "bufRef " << bufRef->getDataOffset()
//       << ", "      << bufRef->getDataSize() 
//       << " data "  << data << endl;
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to fill DDUs' data block";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        LOG4CPLUS_ERROR(logger_, s);
    }

    // Append block to super-fragment under construction
    dataBlocksForDQM_.push_back(bufRef);
    
//     printBlocks( dataBlocksForDQM_ );
}

void EmuRUI::printBlocks( deque<toolbox::mem::Reference*> d ){
  deque<toolbox::mem::Reference*>::iterator pos;
  toolbox::mem::Reference *bufRef = 0;
  cout << "--------- " << nEvents_ <<  " events" << endl;
  for(pos=d.begin(); pos!=d.end(); pos++)
    {
      bufRef = *pos;
      cout 
	<< " " << bufRef->getDataSize();
    }
  cout << endl;
}

void EmuRUI::fillBlock(
    toolbox::mem::Reference *bufRef,
    char*                    data,
    const unsigned int       dataLength
)
throw (emuRUI::exception::Exception)
{
    char         *blockAddr        = 0;
    char         *fedAddr          = 0;
    unsigned int i2oMessageSize    = 0;

    ///////////////////////////////////////////////////////////
    // Calculate addresses of block, FRL header and FED data //
    ///////////////////////////////////////////////////////////

    blockAddr = (char*)bufRef->getDataLocation();
    fedAddr   = blockAddr + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);


    ///////////////////////////////////////////////
    // Set the data size of the buffer reference //
    ///////////////////////////////////////////////

    // I2O message size in bytes
    i2oMessageSize = sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) + dataLength;
    bufRef->setDataSize(i2oMessageSize);

    ///////////////////////////
    // Fill block with zeros //
    ///////////////////////////

    ::memset(blockAddr, 0, i2oMessageSize );
  
    ///////////////////////////
    // Fill FED (DDU) data   //
    ///////////////////////////

    ::memcpy( fedAddr, data, dataLength );

    /////////////////////////////
    // Fill RU  headers        //
    /////////////////////////////


    I2O_MESSAGE_FRAME                  *stdMsg        = 0;
    I2O_PRIVATE_MESSAGE_FRAME          *pvtMsg        = 0;
    I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block         = 0;

    stdMsg    = (I2O_MESSAGE_FRAME*)blockAddr;
    pvtMsg    = (I2O_PRIVATE_MESSAGE_FRAME*)blockAddr;
    block     = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)blockAddr;

    stdMsg->MessageSize    = i2oMessageSize >> 2;
    stdMsg->TargetAddress  = ruTid_;
    stdMsg->Function       = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset  = 0;
    stdMsg->MsgFlags       = 0;  // Point-to-point

    pvtMsg->XFunctionCode  = I2O_RU_DATA_READY;
    pvtMsg->OrganizationID = XDAQ_ORGANIZATION_ID;

//     block->eventNumber     = eventNumber_;
    block->eventNumber     = nEvents_ % 0x1000000; // 2^24
    block->blockNb         = iCurrentDDU_; // each block carries a whole DDU

}

void EmuRUI::fillBlockForDQM(
    toolbox::mem::Reference *bufRef,
    char*                    data,
    const unsigned int       dataLength
)
throw (emuRUI::exception::Exception)
{
    char         *blockAddr        = 0;
    char         *fedAddr          = 0;
    unsigned int i2oMessageSize    = 0;

    ////////////////////////////////////////////////
    // Calculate addresses of block, and FED data //
    ////////////////////////////////////////////////

    blockAddr = (char*)bufRef->getDataLocation();
    fedAddr   = blockAddr + sizeof(I2O_RUI_DATA_MESSAGE_FRAME);


    ///////////////////////////////////////////////
    // Set the data size of the buffer reference //
    ///////////////////////////////////////////////

    // I2O message size in bytes
    i2oMessageSize = sizeof(I2O_RUI_DATA_MESSAGE_FRAME) + dataLength;
    bufRef->setDataSize(i2oMessageSize);

    ///////////////////////////
    // Fill block with zeros //
    ///////////////////////////

    ::memset(blockAddr, 0, i2oMessageSize );
  
    ///////////////////////////
    // Fill FED (DDU) data   //
    ///////////////////////////

    ::memcpy( fedAddr, data, dataLength );

    /////////////////////////////
    // Fill DQM headers        //
    /////////////////////////////


    I2O_MESSAGE_FRAME                  *stdMsg        = 0;
    I2O_PRIVATE_MESSAGE_FRAME          *pvtMsg        = 0;
    I2O_RUI_DATA_MESSAGE_FRAME         *block         = 0;

    stdMsg    = (I2O_MESSAGE_FRAME*)blockAddr;
    pvtMsg    = (I2O_PRIVATE_MESSAGE_FRAME*)blockAddr;
    block     = (I2O_RUI_DATA_MESSAGE_FRAME*)blockAddr;

    stdMsg->MessageSize    = i2oMessageSize >> 2;
    stdMsg->TargetAddress  = DQMonitorTid_;
    stdMsg->Function       = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset  = 0;
    stdMsg->MsgFlags       = 0;  // Point-to-point

    pvtMsg->XFunctionCode  = I2O_TOYCLIENT_CODE;
    pvtMsg->OrganizationID = XDAQ_ORGANIZATION_ID;

    block->runNumber         = runNumber_;
    block->nEventCreditsHeld = nEventCreditsHeld_;

}

void EmuRUI::setNbBlocksInSuperFragment(const unsigned int nbBlocks)
{
//      vector<toolbox::mem::Reference*>::iterator pos;
     deque<toolbox::mem::Reference*>::iterator pos; // BK
     toolbox::mem::Reference            *bufRef = 0;
     I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block  = 0;


     for(pos=superFragBlocks_.begin(); pos!=superFragBlocks_.end(); pos++)
     {
         bufRef = *pos;
         block = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)bufRef->getDataLocation();
         block->nbBlocksInSuperFragment = nbBlocks;
     }
}


string EmuRUI::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool EmuRUI::onI2oException(xcept::Exception &exception, void *context)
{
    xdaq::ApplicationDescriptor *destDescriptor =
        (xdaq::ApplicationDescriptor *)context;
    xcept::Exception exceptionForSentinel =
        createI2oExceptionForSentinel(exception, appDescriptor_,
            appDescriptor_, destDescriptor);


    if(sentinel_ != 0)
    {
        sentinel_->notify(exceptionForSentinel, this);
    }

    LOG4CPLUS_ERROR(logger_,
        " : " << xcept::stdformat_exception_history(exceptionForSentinel));

    return true;
}


emuRUI::exception::Exception EmuRUI::createI2oExceptionForSentinel
(
    xcept::Exception            &i2oException,
    xdaq::ApplicationDescriptor *notifier,
    xdaq::ApplicationDescriptor *source,
    xdaq::ApplicationDescriptor *destination
)
{
    string errorMsg      = createI2oErrorMsg(source, destination);
    string notifierValue = createValueForSentinelNotifierProperty(notifier);


    emuRUI::exception::Exception exception("emuRUI::exception::Exception", errorMsg,
        __FILE__, __LINE__, __FUNCTION__, i2oException);

    exception.setProperty("notifier", notifierValue);
    exception.setProperty("qualifiedErrorSchemaURI",
   "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
    exception.setProperty("dateTime", toolbox::getDateTime());
    exception.setProperty("sessionID", "none");
    exception.setProperty("severity", "ERROR");

    return exception;
}


string EmuRUI::createI2oErrorMsg
(
    xdaq::ApplicationDescriptor *source,
    xdaq::ApplicationDescriptor *destination
)
{
    stringstream oss;
    string       s;


    oss << "I2O exception from ";
    oss << source->getClassName();
    oss << " instance ";
    oss << source->getInstance();
    oss << " to ";
    oss << destination->getClassName();
    oss << " instance ";
    oss << destination->getInstance();

    s = oss.str();

    return s;
}


string EmuRUI::createValueForSentinelNotifierProperty
(
    xdaq::ApplicationDescriptor *notifier
)
{
    stringstream oss;
    string       s;


    oss << notifier->getContextDescriptor()->getURL();
    oss << "/";
    oss << notifier->getURN();

    s = oss.str();

    return s;
}


/**
 * Provides the factory method for the instantiation of EmuRUI applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuRUI)
