#include "EmuFU.h"
#include "EmuFUV.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "i2o/Method.h"
#include "interface/evb/i2oEVBMsgs.h"
#include "interface/shared/frl_header.h"
#include "interface/shared/fed_header.h"
#include "interface/shared/fed_trailer.h"
#include "interface/shared/i2oXFunctionCodes.h"
#include "toolbox/utils.h"
#include "toolbox/fsm/FailedEvent.h"
#include "toolbox/mem/HeapAllocator.h"
#include "xcept/tools.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xgi/Method.h"
#include "xoap/domutils.h"
#include "xoap/MessageFactory.h"
#include "xoap/MessageReference.h"
#include "xoap/Method.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"

// EMu-specific stuff
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/net/URL.h"
#include "emu/emuDAQ/emuReadout/include/EmuFileReader.h"
#include "emu/emuDAQ/emuReadout/include/EmuSpyReader.h"
#include "emu/emuDAQ/emuUtil/include/EmuI2OServer.h"
#include "emu/emuDAQ/emuUtil/include/EmuSOAPServer.h"
#include <sstream>



#include <unistd.h>

// Alias used to access the "versioning" namespace EmuFU from within the class EmuFU
namespace EmuFUV = EmuFU;


EmuFU::EmuFU(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

logger_(Logger::getInstance(generateLoggerName())),

bSem_(toolbox::BSem::FULL)
{
    tid_               = 0;
    i2oAddressMap_     = i2o::utils::getAddressMap();
    poolFactory_       = toolbox::mem::getMemoryPoolFactory();
    appInfoSpace_      = getApplicationInfoSpace();
    appDescriptor_     = getApplicationDescriptor();
    appContext_        = getApplicationContext();
    zone_              = appContext_->getDefaultZone();
    xmlClass_          = appDescriptor_->getClassName();
    instance_          = appDescriptor_->getInstance();
    urn_               = appDescriptor_->getURN();
    superFragmentHead_ = 0;
    superFragmentTail_ = 0;
    blockNb_           = 0;
    faultDetected_     = false;

    appDescriptor_->setAttribute("icon",
        "/emu/emuDAQ/emuFU/images/EmuFU64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester(zone_);

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &EmuFU::onI2oException, "onI2oException");

    i2oPoolName_ = createI2oPoolName(instance_);

    try
    {
        i2oPool_ = createHeapAllocatorMemoryPool(poolFactory_, i2oPoolName_);
    }
    catch(xcept::Exception e)
    {
        string s;

        s = "Failed to create " + i2oPoolName_ + " pool";

        XCEPT_RETHROW(xdaq::exception::Exception, s, e);
    }

    buDescriptor_ = 0;
    buTid_        = 0;

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

    //
    // Emu-specific stuff
    //
    workLoopFactory_ = toolbox::task::getWorkLoopFactory();

    fileWriter_ = NULL;

    bindFsmSoapCallbacks();
    bindI2oCallbacks();

    // Bind web interface
    xgi::bind(this, &EmuFU::css           , "styles.css");
    xgi::bind(this, &EmuFU::defaultWebPage, "Default"   );

    // bind SOAP client credit message callback
    xoap::bind(this, &EmuFU::onSOAPClientCreditMsg, 
	       "onClientCreditMessage", XDAQ_NS_URI);

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string EmuFU::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    unsigned long               appInstance    = appDescriptor->getInstance();
    stringstream                oss;
    string                      loggerName;


    oss << appClass << "." << setfill('0') << std::setw(2) << appInstance;
    loggerName = oss.str();

    return loggerName;
}

string EmuFU::extractParametersFromSOAPClientCreditMsg
(
    xoap::MessageReference msg, unsigned int& instance, int& credits, int& prescaling 
)
throw (emuFU::exception::Exception)
{
    try
    {
        xoap::SOAPPart part = msg->getSOAPPart();
        xoap::SOAPEnvelope env = part.getEnvelope();
        xoap::SOAPBody body = env.getBody();
        DOMNode *bodyNode = body.getDOMNode();
        DOMNodeList *bodyList = bodyNode->getChildNodes();
        DOMNode *functionNode = findNode(bodyList, "onClientCreditMessage");
        DOMNodeList *parameterList = functionNode->getChildNodes();
// 	for ( unsigned int i=0; i<parameterList->getLength(); ++i ){
// 	  cout << i << " " << xoap::XMLCh2String(parameterList->item(i)->getNodeName())
// 	       << " " << xoap::XMLCh2String(parameterList->item(i)->getFirstChild()->getNodeValue()) << endl;
// 	}
	DOMNode *parameterNode = findNode(parameterList, "clientName");
        string clientName      = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
        parameterNode = findNode(parameterList, "clientInstance");
        string clientInstance  = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
        parameterNode = findNode(parameterList, "nEventCredits");
        string sc              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
	parameterNode          = findNode(parameterList, "prescalingFactor");
        string sp              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());

        LOG4CPLUS_DEBUG(logger_, 
			"Received from "          << clientName <<
			" instance "              << clientInstance <<
			" nEventCredits = "       << sc << 
			", prescalingFactor = 1/" << sp );

	istringstream ssi(clientInstance);
	int ci;
	ssi >> ci;
	if ( ci >= 0 ) instance = ci;
	else LOG4CPLUS_ERROR(logger_, "Instance of SOAP client " << clientName << "is negative." );

	istringstream ssc(sc);
	int ic;
	ssc >> ic;
	if ( ic > 0 ) credits = ic; 

	istringstream ssp(sp);
	int ip;
	ssp >> ip;
	if ( ip > 0 ) prescaling = ip;

	return clientName;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Parameter(s) not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuFU::exception::Exception,
            "Parameter(s) not found");
    }
}

xoap::MessageReference EmuFU::processSOAPClientCreditMsg( xoap::MessageReference msg )
  throw( emuFU::exception::Exception )
{
  xoap::MessageReference reply;

  unsigned int instance = 0;
  int credits = 0, prescaling = 1;
  string name = extractParametersFromSOAPClientCreditMsg( msg, instance, credits, prescaling );

  // Find out who sent this and add the credits to its corresponding server
  bool knownClient = false;
  for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
    if ( (*c)->name->toString() == name && (*c)->instance->value_ == instance ){
      knownClient = true;
      (*c)->server->addCredits( credits, prescaling );
      // If client descriptor is not known (non-XDAQ client), send data now:
      if ( (*c)->server->getClientDescriptor() == NULL ){
	reply = (*c)->server->getOldestMessagePendingTransmission();
	if ( !reply.isNull() ){ 
// 	  string rs;
// 	  reply->writeTo(rs);
	  LOG4CPLUS_INFO(logger_, "Sent data to external SOAP client " << name );
	}
      }
      break;
    }
  }

  // If this client is not yet known, create a new (non-persistent) server for it...
  if ( !knownClient ){
    if ( createSOAPServer( name, instance, false ) ){
      // ... and if successfully created, add credits
      for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
	if ( (*c)->server->getClientName() == name && (*c)->server->getClientInstance() == instance ){
	  (*c)->server->addCredits( credits, prescaling );
	  break;
	}
      }
    }
    else {
      if ( reply.isNull() ) reply       = xoap::createMessage();
      xoap::SOAPEnvelope envelope       = reply->getSOAPPart().getEnvelope();
      xoap::SOAPName fault              = envelope.createName( "fault" );
      xoap::SOAPBodyElement faultElem   = envelope.getBody().addBodyElement( fault );
      xoap::SOAPName faultcode          = envelope.createName( "faultcode" );
      xoap::SOAPElement faultcodeElem   = faultElem.addChildElement( faultcode );
      faultcodeElem.addTextNode("Server");
      xoap::SOAPName faultstring        = envelope.createName( "faultstring" );
      xoap::SOAPElement faultstringElem = faultElem.addChildElement( faultstring );
      faultstringElem.addTextNode("Could not create an Emu data server for you.");
    }
  }

  return reply;
}


xoap::MessageReference EmuFU::onSOAPClientCreditMsg( xoap::MessageReference msg )
  throw (xoap::exception::Exception)
  // EMu-specific stuff
{

  stringstream ss;
  std::multimap< std::string, std::string, std::less< std::string > > h = msg->getMimeHeaders()->getAllHeaders();
  std::multimap< std::string, std::string, std::less< std::string > >::iterator i;
  ss << "Mime headers (" << h.size() << ")" << endl;
  for( i = h.begin(); i != h.end(); ++i )
    ss << i->first << " " << i->second << endl;
  string s;
  msg->writeTo(s);
  LOG4CPLUS_DEBUG(logger_, string("Received: ") << endl << ss.str() << s);

  bSem_.take();

  xoap::MessageReference reply;

  try
    {
      switch(fsm_.getCurrentState())
        {
        case 'H': // Halted
        case 'F': // Failed
	  break;
        case 'E': // Enabled
	  try
	    {
	      reply = processSOAPClientCreditMsg( msg );
	    }
	  catch( emuFU::exception::Exception e )
	    {
	      XCEPT_RETHROW(xoap::exception::Exception, string("Failed to process SOAP client credit message"), e);
	    }
	  break;
        case 'R': // Ready
        case 'S': // Suspended
	  break;
        default:
	  LOG4CPLUS_ERROR(logger_,
			  "EmuFU in undefined state");
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
  
  
  if ( reply.isNull() ) reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  bSem_.give();

  return reply;
}

void EmuFU::onI2OClientCreditMsg(toolbox::mem::Reference *bufRef)
  // EMu-specific stuff
{
    I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME *msg =
        (I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME*)bufRef->getDataLocation();


    bSem_.take();

    try
    {
        switch(fsm_.getCurrentState())
        {
        case 'H': // Halted
        case 'F': // Failed
            break;
        case 'E': // Enabled
	  // Find out who sent this and add the credits to its corresponding server
	  LOG4CPLUS_DEBUG(logger_, 
			  "Got I2O credit message from client of tid " << 
			  msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress );
	  for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c )
	    {
	      if ( msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress != 0 &&
		   (*c)->server->getClientTid() == msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress )
		{
		  (*c)->server->addCredits( msg->nEventCredits, msg->prescalingFactor );
		  break;
		}
	    }
	  break;
        case 'R': // Ready
        case 'S': // Suspended
	  break;
        default:
	  LOG4CPLUS_ERROR(logger_,
			  "EmuFU in undefined state");
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

    // Free the client's event credit message
    bufRef->release();

    bSem_.give();

}

xdaq::ApplicationDescriptor *EmuFU::getRUBuilderTester
(
    xdaq::Zone *zone
)
{
    xdaq::ApplicationDescriptor *appDescriptor = 0;


    try
    {
        appDescriptor =
            zone->getApplicationDescriptor("EmuDAQManager", 0);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        appDescriptor = 0;
    }

    return appDescriptor;
}

DOMNode *EmuFU::findNode(DOMNodeList *nodeList,
			 const string nodeLocalName)
  throw (emuFU::exception::Exception)
{
    DOMNode            *node = 0;
    string             name  = "";
    unsigned int       i     = 0;


    for(i=0; i<nodeList->getLength(); i++)
    {
        node = nodeList->item(i);

        if(node->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            name = xoap::XMLCh2String(node->getLocalName());

            if(name == nodeLocalName)
            {
                return node;
            }
        }
    }

    XCEPT_RAISE(emuFU::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


string EmuFU::createI2oPoolName(const unsigned long emuFUInstance)
{
    stringstream oss;
    string       s;

    oss << "EmuFU" << emuFUInstance << "_i2oFragmentPool";
    s = oss.str();

    return s;
}


void EmuFU::defineFsm()
throw (emuFU::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &EmuFU::stateChanged);
        fsm_.addState('R', "Ready"    , this, &EmuFU::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &EmuFU::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &EmuFU::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &EmuFU::enableAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &EmuFU::haltAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &EmuFU::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &EmuFU::haltAction);

        fsm_.addStateTransition('H', 'F', "Fail", this, &EmuFU::failAction);
        fsm_.addStateTransition('R', 'F', "Fail", this, &EmuFU::failAction);
        fsm_.addStateTransition('E', 'F', "Fail", this, &EmuFU::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &EmuFU::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &EmuFU::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to reset FSM", e);
    }
}


string EmuFU::generateMonitoringInfoSpaceName
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


vector< pair<string, xdata::Serializable*> > EmuFU::initAndGetStdConfigParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    buInstNb_           = 0;
    nbOutstandingRqsts_ = 80;
    sleepBetweenEvents_ = false;
    sleepIntervalUsec_  = 1000; // 1 millisecond
    nbEventsBeforeExit_ = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("buInstNb", &buInstNb_));
    params.push_back(pair<string,xdata::Serializable *>
        ("nbOutstandingRqsts", &nbOutstandingRqsts_));
    params.push_back(pair<string,xdata::Serializable *>
        ("sleepBetweenEvents", &sleepBetweenEvents_));
    params.push_back(pair<string,xdata::Serializable *>
        ("sleepIntervalUsec" , &sleepIntervalUsec_));
    params.push_back(pair<string,xdata::Serializable *>
        ("nbEventsBeforeExit", &nbEventsBeforeExit_));

    //
    // EMu-specific stuff
    //
    pathToDataOutFile_   = "/tmp";
    fileSizeInMegaBytes_ = 2;
    params.push_back(pair<string,xdata::Serializable *>
		     ("pathToDataOutFile"  , &pathToDataOutFile_   ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileSizeInMegaBytes", &fileSizeInMegaBytes_ ));
    runType_ = "";
    params.push_back(pair<string,xdata::Serializable *>
		     ("runType", &runType_));


    for( unsigned int iClient=0; iClient<maxClients_; ++iClient ) {
      clientName_.push_back("");
      clientInstance_.push_back(0);
      clientProtocol_.push_back("I2O");
      clientPoolSize_.push_back(0x100000); // 1MB
      prescaling_.push_back(0);
      onRequest_.push_back(true);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientsClassName", &clientName_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientsInstance", &clientInstance_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientsProtocol", &clientProtocol_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("poolSizeForClient", &clientPoolSize_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("prescalingForClient", &prescaling_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("sendOnClientsRequestOnly", &onRequest_));

    return params;
}


vector< pair<string, xdata::Serializable*> > EmuFU::initAndGetStdMonitorParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    stateName_         = "Halted";
    nbEventsProcessed_ = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("stateName", &stateName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("nbEventsProcessed", &nbEventsProcessed_));

    //
    // EMu-specific stuff
    //
    params.push_back(pair<string,xdata::Serializable *>
		     ("runNumber", &runNumber_ ));

    for( unsigned int iClient=0; iClient<maxClients_; ++iClient ){ 
      creditsHeld_.push_back(0);
      clientPersists_.push_back(true);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("nEventCreditsHeld", &creditsHeld_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientPersists", &clientPersists_));

    CSCConfigId_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("CSCConfigId", &CSCConfigId_ ));
    TFConfigId_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("TFConfigId", &TFConfigId_ ));

    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
    params.push_back(pair<string,xdata::Serializable *>
        ("runStartTime", &runStartTime_));
    params.push_back(pair<string,xdata::Serializable *>
        ("runStopTime", &runStopTime_));

    return params;
}


void EmuFU::putParamsIntoInfoSpace
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


void EmuFU::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
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

    if(state == 'E')
    {
        try
        {
            allocateNEvents(nbOutstandingRqsts_);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to allocate " << nbOutstandingRqsts_ << " events";
            s = oss.str();

            XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
        }
    }
}


void EmuFU::bindFsmSoapCallbacks()
{
    xoap::bind(this, &EmuFU::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuFU::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &EmuFU::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &EmuFU::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);

    xoap::bind(this, &EmuFU::onReset,           "Reset"    , XDAQ_NS_URI);
}

void EmuFU::moveToFailedState(){ // Emu-specific
  try
    {
      // Move to the failed state
      toolbox::Event::Reference evtRef(new toolbox::Event("Fail", this));
      fsm_.fireEvent(evtRef);
      bSem_.give();
    }
  catch(xcept::Exception e)
    {
      bSem_.give();
      
      LOG4CPLUS_FATAL(logger_,
		      "Failed to move to the Failed state : "
		      << xcept::stdformat_exception_history(e));
    }
}


xoap::MessageReference EmuFU::processSoapFsmCmd(xoap::MessageReference msg)
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


string EmuFU::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emuFU::exception::Exception)
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

    XCEPT_RAISE(emuFU::exception::Exception, "Command name not found");
}


xoap::MessageReference EmuFU::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emuFU::exception::Exception)
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
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuFU::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


void EmuFU::processFsmCommand(const string cmdName)
throw (emuFU::exception::Exception)
{
    toolbox::Event::Reference evtRef(new toolbox::Event(cmdName, this));


    try
    {
        bSem_.take();
        fsm_.fireEvent(evtRef);
        bSem_.give();
    }
    catch(xcept::Exception e)
    {
        bSem_.give();
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        bSem_.give();
        XCEPT_RAISE(emuFU::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}

void EmuFU::destroyServers(){
  std::vector<Client*>::iterator c;
  for ( c=clients_.begin(); c!=clients_.end(); ++c ){
    LOG4CPLUS_INFO(logger_, string("Destroying server for ") + (*c)->server->getClientName() );
    delete (*c)->server;
    *(*c)->creditsHeld = 0;
  }
  clients_.clear();
}

bool EmuFU::createI2OServer( string clientName, unsigned int clientInstance  ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>       ( clientName_.elementAt( iClient )     )) = clientName;
    *(dynamic_cast<xdata::UnsignedLong*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>       ( clientProtocol_.elementAt( iClient ) )) = "I2O";
    *(dynamic_cast<xdata::Boolean*>      ( clientPersists_.elementAt( iClient ) )) = true;
    EmuI2OServer* s = new EmuI2OServer( this,
					i2oExceptionHandler_,
					clientName_.elementAt(iClient)->toString(),
					clientInstance,
					clientPoolSize_.elementAt(iClient),
					prescaling_.elementAt(iClient),
					onRequest_.elementAt(iClient),
					creditsHeld_.elementAt(iClient),
					&logger_ );

    clients_.push_back( new Client( clientName_.elementAt(iClient),
				    clientInstance_.elementAt(iClient),
				    clientPersists_.elementAt(iClient),
				    clientPoolSize_.elementAt(iClient),
				    prescaling_.elementAt(iClient),
				    onRequest_.elementAt(iClient), 
				    creditsHeld_.elementAt(iClient),
				    s ) );
    created = true;
  }
  return created;
}

bool EmuFU::createSOAPServer( string clientName,  unsigned int clientInstance, bool persistent ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>       (     clientName_.elementAt( iClient ) )) = clientName;
    *(dynamic_cast<xdata::UnsignedLong*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>       ( clientProtocol_.elementAt( iClient ) )) = "SOAP";
    *(dynamic_cast<xdata::Boolean*>      ( clientPersists_.elementAt( iClient ) )) = persistent;
    EmuSOAPServer* s = new EmuSOAPServer( this,
					  clientName_.elementAt(iClient)->toString(),
					  clientInstance,
					  clientPersists_.elementAt(iClient),
					  prescaling_.elementAt(iClient),
					  onRequest_.elementAt(iClient),
					  creditsHeld_.elementAt(iClient),
					  &logger_ );
    
    clients_.push_back( new Client( clientName_.elementAt(iClient),
				    clientInstance_.elementAt(iClient),
				    clientPersists_.elementAt(iClient),
				    clientPoolSize_.elementAt(iClient),
				    prescaling_.elementAt(iClient),
				    onRequest_.elementAt(iClient), 
				    creditsHeld_.elementAt(iClient),
				    s ) );
    created = true;
  }
  else 
    LOG4CPLUS_WARN(logger_, 
		   "Maximum number of clients exceeded. Cannot create server for " << clientName );
  return created;
}


void EmuFU::createServers(){
  for ( unsigned int iClient=0; iClient<clientName_.elements(); ++iClient ){
    xdata::Boolean *persists = dynamic_cast<xdata::Boolean*>( clientPersists_.elementAt(iClient) );
    // (Re)create it only if it has a name and is not a temporary server created on the fly
    if ( clientName_.elementAt(iClient)->toString() != "" && persists->value_ ){
      unsigned int clientInstance = 
	(dynamic_cast<xdata::UnsignedLong*> ( clientInstance_.elementAt( iClient ) ))->value_;
      LOG4CPLUS_INFO(logger_,
		     clientName_.elementAt(iClient)->toString() << clientInstance << 
		     "\'s server being created" );
      if ( clientProtocol_.elementAt(iClient)->toString() == "I2O" )
	createI2OServer( clientName_.elementAt(iClient)->toString(), clientInstance );
      else if ( clientProtocol_.elementAt(iClient)->toString() == "SOAP" )
	createSOAPServer( clientName_.elementAt(iClient)->toString(), clientInstance );
      else
	LOG4CPLUS_ERROR(logger_, "Unknown protocol \"" <<
			clientProtocol_.elementAt(iClient)->toString() << 
			"\" for client " <<
			clientName_.elementAt(iClient)->toString() << 
			" instance " <<
			clientInstance <<
			". Please use \"I2O\" or \"SOAP\".");
    }
  }
}

void EmuFU::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

    releaseSuperFragment();

    // Reset the current block number of the super-fragment under-construction.
    blockNb_ = 0;

    // Clean start
    faultDetected_ = false;

    try
    {
        tid_ = i2oAddressMap_->getTid(appDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to get the I2O TID of this application", e);
    }

    nbEventsProcessed_ = 0;

    try
    {
        buDescriptor_ = zone_->getApplicationDescriptor("rubuilder::bu::Application", buInstNb_);
        buTid_        = i2oAddressMap_->getTid(buDescriptor_);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to get application descriptor and I2O for";
        oss << " BU" << instance_;
        s = oss.str();

        XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to get application descriptor and I2O for";
        oss << " BU" << instance_;
        s = oss.str();

        XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
    }
    catch(...)
    {
        stringstream oss;
        string       s;

        oss << "Failed to get application descriptor and I2O for";
        oss << " BU" << instance_;
        oss << " : Unknown exception";
        s = oss.str();

        XCEPT_RAISE(toolbox::fsm::exception::Exception, s);
    }

    //
    // EMu-specific stuff
    //

    // Just in case there's a writer, terminate it in an orderly fashion
    if ( fileWriter_ )
      {
	LOG4CPLUS_WARN( logger_, "Terminating leftover file writer." );
	fileWriter_->endRun();
	delete fileWriter_;
	fileWriter_ = NULL;
      }
    // create new writer if path is not empty
    if ( pathToDataOutFile_ != string("") && (xdata::UnsignedLongT) fileSizeInMegaBytes_ > (long unsigned int) 0 ){
      toolbox::net::URL u( appContext_->getContextDescriptor()->getURL() );
      stringstream app;
      app << "EmuFU";
      app.fill('0');
      app.width(2);
      app << instance_;
      fileWriter_ = new EmuFileWriter( 1000000*fileSizeInMegaBytes_, 
				       pathToDataOutFile_.toString(), 
				       u.getHost(), app.str(), EmuFUV::versions, &logger_ );
    }
    
    // Create an Emu event header
    bool isCalibrationRun = ( runType_.toString().find("Calib") != string::npos );
    bool ruiZeroExists = false;
    set<xdaq::ApplicationDescriptor*> ruiDescriptors = zone_->getApplicationDescriptors( "EmuRUI" );
    for ( set<xdaq::ApplicationDescriptor*>::const_iterator rui=ruiDescriptors.begin(); rui!=ruiDescriptors.end(); ++rui ){
      if ( (*rui)->getInstance() == 0 ){
	ruiZeroExists = true;
	break;
      }
    }
    emuEventHeaderTrailer_ = new emuFU::EmuEventHeaderTrailer( isCalibrationRun, !isCalibrationRun, ruiZeroExists );

    // Recreate servers
    destroyServers();
    createServers();

    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
}


void EmuFU::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
  getRunInfo();

    // server loops
    for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient ){
	
      if( ! clients_[iClient]->workLoopStarted )
	{
	  clients_[iClient]->workLoopActionSignature = toolbox::task::bind
	    (
	     this,
	     &EmuFU::serverLoopAction,
	     "EmuFU server loop action"
	     );

	  if(clients_[iClient]->workLoopName == "")
	    {
	      stringstream oss;
	      oss << xmlClass_ << instance_ << "Server" << iClient << "WorkLoop";
	      clients_[iClient]->workLoopName = oss.str();
	    }

	  try
	    {
	      clients_[iClient]->workLoop =
                workLoopFactory_->getWorkLoop(clients_[iClient]->workLoopName, "waiting");
	    }
	  catch(xcept::Exception e)
	    {
	      string s = "Failed to get work loop : " + clients_[iClient]->workLoopName;

	      XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
	    }

	  try
	    {
	      clients_[iClient]->workLoop->submit(clients_[iClient]->workLoopActionSignature);
	    }
	  catch(xcept::Exception e)
	    {
	      string s = "Failed to submit action to work loop : " +
		clients_[iClient]->workLoopName;


	      XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
	    }

	  if(!clients_[iClient]->workLoop->isActive())
	    {
	      try
		{
		  clients_[iClient]->workLoop->activate();

		  LOG4CPLUS_INFO(logger_,
				 "Activated work loop : " << clients_[iClient]->workLoopName);
		}
	      catch(xcept::Exception e)
		{
		  string s = "Failed to active work loop : " +
		    clients_[iClient]->workLoopName;

		  XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
		}
	    }

	  clients_[iClient]->workLoopStarted = true;
	}
    }

}


void EmuFU::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

  // Get time of end of run
  try{
    runStopTime_ = getScalarParam(*taDescriptors_.begin(),"runStopTime","string");
    LOG4CPLUS_INFO(logger_, "Got run stop time from EmuTA: " << runStopTime_.toString() );
  }
  catch( emuFU::exception::Exception e ){
    LOG4CPLUS_WARN(logger_, "Run stop time will be unknown: " << xcept::stdformat_exception_history(e) );
  }

  // Close data file
  if ( fileWriter_ )
    {
      fileWriter_->endRun( runStopTime_.toString() );
      delete fileWriter_;
      fileWriter_ = NULL;
    }

  delete emuEventHeaderTrailer_;

}


void EmuFU::failAction(toolbox::Event::Reference event)
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


void EmuFU::bindI2oCallbacks()
{
    i2o::bind
    (
        this,
        &EmuFU::I2O_FU_TAKE_Callback,
        I2O_FU_TAKE,
        XDAQ_ORGANIZATION_ID
    );

  // EMu-specific stuff
  i2o::bind(this, &EmuFU::onI2OClientCreditMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );
}


// void EmuFU::clientCreditMsg(toolbox::mem::Reference *bufRef)
//   // EMu-specific stuff
// {
//     I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME *msg =
//         (I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME*)bufRef->getDataLocation();


//     bSem_.take();

//     try
//     {
//         switch(fsm_.getCurrentState())
//         {
//         case 'H': // Halted
//         case 'F': // Failed
//             break;
//         case 'E': // Enabled
// 	  addCreditsFromClients( msg->nEventCredits, msg->prescalingFactor );
//             break;
//         case 'R': // Ready
//         case 'S': // Suspended
// 	  addCreditsFromClients( msg->nEventCredits, msg->prescalingFactor );
//             break;
//         default:
//             LOG4CPLUS_ERROR(logger_,
//                 "EmuFU in undefined state");
//         }
//     }
//     catch(xcept::Exception e)
//     {
//         LOG4CPLUS_ERROR(logger_,
//             "Failed to process client credit message : "
//              << stdformat_exception_history(e));
//     }
//     catch(...)
//     {
//         LOG4CPLUS_ERROR(logger_,
//             "Failed to process client credit message : Unknown exception");
//     }

//     // Free the client's event credit message
//     bufRef->release();

//     bSem_.give();

// }

bool EmuFU::serverLoopAction(toolbox::task::WorkLoop *wl)
{
    try
    {
      bool isToBeRescheduled = true;

      bSem_.take();

        toolbox::fsm::State state = fsm_.getCurrentState();

        switch(state)
        {
        case 'H':  // Halted
        case 'F': // Failed
	  break;
        case 'R':  // Ready
            break;
        case 'E':  // Enabled
	  // Find out from which work loop we dropped in here
	  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient ){
	    if ( clients_[iClient]->workLoop == wl ){
// 	      LOG4CPLUS_DEBUG(logger_, "Sending data from " << clients_[iClient]->workLoopName << " ("<< wl << ")");
	      clients_[iClient]->server->sendData();
	    break;
	    }
	  }
	  break;
        default:
            // Should never get here
            LOG4CPLUS_FATAL(logger_,
                "EmuFU" << instance_ << " is in an undefined state");
        }

	bSem_.give();

        // Reschedule this action code
        return isToBeRescheduled;
    }
    catch(xcept::Exception e)
    {
	bSem_.give();

        LOG4CPLUS_FATAL(logger_,
            "Failed to execute \"self-driven\" behaviour"
            << " : " << xcept::stdformat_exception_history(e));

        // Do not reschedule this action code as the application has failed
        return false;
    }
}


void EmuFU::addDataForClients( const int   runNumber, 
			       const int   nEventsRead,
			       const bool  completesEvent, 
			       char* const data, 
			       const int   dataLength ){
  unsigned short dummyErrorFlag = 0; // We don't have the error info amy more at this point.
  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->addData( runNumber, 
					nEventsRead, 
					completesEvent,
					dummyErrorFlag,
					data, 
					dataLength );
}

void EmuFU::css
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


void EmuFU::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << EmuFUV::versions     << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/emuDAQ/emuFU/images/EmuFU64x64.gif\""<< endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << EmuFUV::versions
        << " " << stateName_.toString() << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"70\">" << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\">"             << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=\"/hyperdaq/images/HyperDAQ.jpg\""             << endl;
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
        *out << "     src=\"/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif\"";
        *out << endl;
        *out << "       alt=\"Manager\""                                << endl;
        *out << "       width=\"64\""                                  << endl;
        *out << "       height=\"64\""                                 << endl;
        *out << "       border=\"\"/>"                                 << endl;
        *out << "    </a>"                                             << endl;
        *out << "    <a href=";
        *out << "\"" << getHref(rubuilderTesterDescriptor_) << "\">"   << endl;
        *out << "      Manager"                                         << endl;
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
        printParamsTable(in, out, "Standard configuration", stdConfigParams_);
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
        printParamsTable(in, out, "Standard monitoring", stdMonitorParams_);
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


void EmuFU::printParamsTable
(
    xgi::Input                                   *in,
    xgi::Output                                  *out,
    const string                                 name,
    vector< pair<string, xdata::Serializable*> > &params
)
throw (xgi::exception::Exception)
{
    vector< pair<string, xdata::Serializable*> >::iterator pos;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;

    *out << "  <tr>"                                                   << endl;
    *out << "    <th colspan=2>"                                       << endl;
    *out << "      " << name                                           << endl;
    *out << "    </th>"                                                << endl;
    *out << "  </tr>"                                                  << endl;


    for(pos = params.begin(); pos != params.end(); ++pos)
    {

	  *out << "  <tr>"                                               << endl;
	  
	  // Name
	  *out << "    <td>"                                             << endl;
	  *out << "      " << pos->first                                 << endl;
	  *out << "    </td>"                                            << endl;
	  
	  // Value
	  *out << "    <td>"                                             << endl;
	  *out << "      " << pos->second->toString()                    << endl;
	  *out << "    </td>"                                            << endl;
	  
	  *out << "  </tr>"                                              << endl;

    }

    *out << "</table>"                                                 << endl;
}

void EmuFU::I2O_FU_TAKE_Callback(toolbox::mem::Reference *bufRef)
{
    bSem_.take();

    toolbox::fsm::State     state = fsm_.getCurrentState();
    toolbox::mem::Reference *next = 0;


    switch(state)
    {
    case 'H': // Halted
    case 'F': // Failed
        bufRef->release();
        break;
    case 'R': // Ready
        LOG4CPLUS_ERROR(logger_,
            "Received a data block whilst in the Ready state");
        bufRef->release();
        break;
    case 'E': // Enabled
        if(faultDetected_)
        {
            bufRef->release();
        }
        else
        {
            // bufRef might point to a chain - break the chain into its
            // separate blocks and process each block individually
            while(bufRef != 0)
            {
                // Break the current block from the chain, making sure the next
                // block is not lost
                next = bufRef->getNextReference();
                bufRef->setNextReference(0);

                // Process the current block
                try
                {
                    processDataBlock(bufRef);
                }
                catch(xcept::Exception e)
                {
                    LOG4CPLUS_ERROR(logger_,
                        "Failed to process data block"
                        << " : " << xcept::stdformat_exception_history(e));
                }

                // Move to the next block (could be 0!)
                bufRef = next;
            }
        }
        break;
    default:
        LOG4CPLUS_ERROR(logger_, "Unknown application state");
        bufRef->release();
    }


    bSem_.give();
}


void EmuFU::processDataBlock(toolbox::mem::Reference *bufRef)
throw (emuFU::exception::Exception)
{
    I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block =
        (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)bufRef->getDataLocation();
    bool superFragmentIsLastOfEvent =
        block->superFragmentNb == (block->nbSuperFragmentsInEvent - 1);
    bool blockIsLastOfSuperFragment =
        block->blockNb == (block->nbBlocksInSuperFragment-1);
    bool blockIsLastOfEvent =
        superFragmentIsLastOfEvent && blockIsLastOfSuperFragment;
    U32 buResourceId = block->buResourceId;


    //
    // EMu-specific stuff
    //
    char         *startOfPayload = (char*) bufRef->getDataLocation() 
      + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
    unsigned long  sizeOfPayload =         bufRef->getDataSize()
      - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
    bool blockIsFirstOfSuperFragment = block->blockNb         == 0;
    bool superFragmentIsFirstOfEvent = block->superFragmentNb == 0;

//     cout << "blockIsFirstOfSuperFragment: " << blockIsFirstOfSuperFragment << endl;
//     cout << "superFragmentIsFirstOfEvent: " << superFragmentIsFirstOfEvent << endl;
//     cout << "superFragmentIsLastOfEvent: " << superFragmentIsLastOfEvent << endl;
//     cout << "blockIsLastOfSuperFragment: " << blockIsLastOfSuperFragment << endl;
//     cout << "blockIsLastOfEvent: " << blockIsLastOfEvent << endl;
//     printBlock( bufRef, true );

    if( superFragmentIsFirstOfEvent && blockIsFirstOfSuperFragment ) // new event is starting
      {

	if ( fileWriter_ )
	  {
	    if ( nbEventsProcessed_.value_ == 0 ){
	      try{
		fileWriter_->startNewRun( runNumber_.value_, 
					  isBookedRunNumber_.value_,
					  runStartTime_, 
					  runType_ );
	      }
	      catch(string e){
		LOG4CPLUS_FATAL( logger_, e );
		moveToFailedState();
	      }
	    }
	    try{
	      fileWriter_->startNewEvent();
	    }
	    catch(string e){
	      LOG4CPLUS_FATAL( logger_, e );
	      moveToFailedState();
	    }
	  }
      }

    // Insert Emu event header at the beginning of this event
    if( superFragmentIsFirstOfEvent && blockIsFirstOfSuperFragment ){
      if ( fileWriter_ ){
	emuEventHeaderTrailer_->setL1ACounter( block->eventNumber );
	// One superfragment (the first) is the trigger, which must be subtracted
	emuEventHeaderTrailer_->setDDUCount( block->nbSuperFragmentsInEvent - 1 );
	emuEventHeaderTrailer_->setCSCConfigId( (xdata::UnsignedLongT) CSCConfigId_ );
	emuEventHeaderTrailer_->setTFConfigId( (xdata::UnsignedLongT) TFConfigId_ );
	try{
	  fileWriter_->writeData( (const char*) emuEventHeaderTrailer_->header(),
				  emuEventHeaderTrailer_->headerSize() );
	}
	catch(string e){
	  LOG4CPLUS_FATAL( logger_, e );
	  moveToFailedState();
	}
      }
    }

    // Write block to file unless it's the first super fragment (=trigger)
    if ( ! superFragmentIsFirstOfEvent ){

      if ( fileWriter_ )
	{
	  try{
	    fileWriter_->writeData( startOfPayload, sizeOfPayload );
	  }
	  catch(string e){
	    LOG4CPLUS_FATAL( logger_, e );
	    moveToFailedState();
	  }
	}
      
      addDataForClients( runNumber_.value_,
			 nbEventsProcessed_.value_,
			 blockIsLastOfEvent,
			 startOfPayload,
			 sizeOfPayload );

    }

    appendBlockToSuperFragment(bufRef);

    if(blockIsLastOfSuperFragment)
    {
        try
        {
            releaseSuperFragment();
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to release super-fragment with";
            oss << " BU resource id: " << buResourceId;
            s = oss.str();

            XCEPT_RETHROW(emuFU::exception::Exception, s, e);
        }
    }

    if(blockIsLastOfEvent)
    {
      // Insert Emu event trailer at the end of this event
	if ( fileWriter_ ){
	  try{
	    fileWriter_->writeData( (const char*) emuEventHeaderTrailer_->trailer(),
				    emuEventHeaderTrailer_->trailerSize() );
	  } 
	  catch(string e){
	    LOG4CPLUS_FATAL( logger_, e );
	    moveToFailedState();
	  }
	}
	
        nbEventsProcessed_.value_++;

        if(sleepBetweenEvents_.value_)
        {
            ::usleep(sleepIntervalUsec_.value_);
        }

        try
        {
            discardEvent(buResourceId);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to discard event with";
            oss << " BU resource id: " << buResourceId;
            s = oss.str();

            XCEPT_RETHROW(emuFU::exception::Exception, s, e);
        }

        try
        {
            allocateNEvents(1);
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuFU::exception::Exception,
                "Failed to allocate an event", e);
        }
    }



}


void EmuFU::printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader )
  //
  // EMu-specific stuff
  //
{
  if ( !bufRef )
    {
      std::cout << "EmuFU::printBlock: no buffer?!" << endl;
      return;
    }

  char         *startOfPayload = (char*) bufRef->getDataLocation() 
    + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
  unsigned long  sizeOfPayload =         bufRef->getDataSize()
    - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
  unsigned short * shorts = reinterpret_cast<unsigned short *>(startOfPayload);
  int nshorts = sizeOfPayload / sizeof(unsigned short);

  if( printMessageHeader )
    {
      std::cout << "EmuFU::printBlock:" << endl;
      I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME* block = 
	(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*) bufRef->getDataLocation();
      std::cout  << 
	"   SFrag " << block->superFragmentNb << 
	" of "   << block->nbSuperFragmentsInEvent << endl;
      std::cout  << 
	"   Block " << block->blockNb << 
	" of "   << block->nbBlocksInSuperFragment <<
	", length " << sizeOfPayload << " bytes" << endl;
    }
  std::cout<<std::hex;
  for(int i = 0; i < nshorts; i+=4)
    {
      std::cout << "      ";
      std::cout.width(4); std::cout.fill('0');
      std::cout << shorts[i+3] << " ";
      std::cout.width(4); std::cout.fill('0');
      std::cout << shorts[i+2] << " ";
      std::cout.width(4); std::cout.fill('0');
      std::cout << shorts[i+1] << " ";
      std::cout.width(4); std::cout.fill('0');
      std::cout << shorts[i] << std::endl;
    }
  std::cout<<std::dec;
  std::cout.width(0);
}

void EmuFU::appendBlockToSuperFragment(toolbox::mem::Reference *bufRef)
{
    if(superFragmentHead_ == 0)
    {
        superFragmentHead_ = bufRef;
        superFragmentTail_ = bufRef;
    }
    else
    {
        superFragmentTail_->setNextReference(bufRef);
        superFragmentTail_ = bufRef;
    }
}


void EmuFU::checkSuperFragment()
throw (emuFU::exception::Exception)
{
    unsigned int len  = getSumOfFedData(superFragmentHead_);
    char         *buf = new char[len];


    try
    {
        fillBufferWithSuperFragment(buf, len, superFragmentHead_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to fill contiguous memory super-fragment", e);
    }

    try
    {
        checkFedTraversal(buf, len);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to traverse FEDs", e);
    }

    delete[] buf;
}


unsigned int EmuFU::getSumOfFedData(toolbox::mem::Reference *bufRef)
{
    char         *blockAddr     = 0;
    char         *frlHeaderAddr = 0;
    frlh_t       *frlHeader     = 0;
    unsigned int nbBytes        = 0;


    while(bufRef != 0)
    {
        blockAddr     = (char *)bufRef->getDataLocation();
        frlHeaderAddr = blockAddr + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
        frlHeader     = (frlh_t*)frlHeaderAddr;

        nbBytes += frlHeader->segsize & FRL_SEGSIZE_MASK;

        bufRef = bufRef->getNextReference();
    }

    return nbBytes;
}


void EmuFU::fillBufferWithSuperFragment
(
    char                    *buf,
    unsigned int            len,
    toolbox::mem::Reference *bufRef
)
throw (emuFU::exception::Exception)
{
    char         *blockAddr     = 0;
    char         *frlHeaderAddr = 0;
    char         *fedAddr       = 0;
    frlh_t       *frlHeader     = 0;
    char         *endPos        = 0;
    char         *pos           = 0;
    unsigned int nbBytes        = 0;


    endPos = buf + len;
    pos    = buf;

    while(bufRef != 0)
    {
        blockAddr     = (char *)bufRef->getDataLocation();
        frlHeaderAddr = blockAddr + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
        fedAddr       = frlHeaderAddr + sizeof(frlh_t);
        frlHeader     = (frlh_t*)frlHeaderAddr;
        nbBytes       = frlHeader->segsize & FRL_SEGSIZE_MASK;

        // If inserting fed bytes would over-shoot end of buffer
        if((pos + nbBytes) > endPos)
        {
            XCEPT_RAISE(emuFU::exception::Exception, "Reached end of buffer");
        }

        memcpy(pos, fedAddr, nbBytes);

        pos += nbBytes;
        bufRef = bufRef->getNextReference();
    }
}


void EmuFU::checkFedTraversal
(
    char         *buf,
    unsigned int len
)
throw (emuFU::exception::Exception)
{
    char         *fedTrailerAddr = 0;
    char         *fedHeaderAddr  = 0;
    fedt_t       *fedTrailer     = 0;
    fedh_t       *fedHeader      = 0;
    unsigned int fedSize         = 0;
    unsigned int fedSourceId     = 0;
    unsigned int eventNumber     = 0;
    unsigned int sumOfFedSizes   = 0;


    fedTrailerAddr = buf + len - sizeof(fedt_t);

    while(fedTrailerAddr > buf)
    {
        fedTrailer = (fedt_t*)fedTrailerAddr;

        fedSize = (fedTrailer->eventsize & (~0xA0000000)) << 3;
        sumOfFedSizes += fedSize;

        fedHeaderAddr = fedTrailerAddr - fedSize + sizeof(fedt_t);

        if(fedHeaderAddr < buf)
        {
            XCEPT_RAISE(emuFU::exception::Exception,
                "Fell off front of super-fragment");
        }

        fedHeader   = (fedh_t*)fedHeaderAddr;
        fedSourceId = fedHeader->sourceid;
        eventNumber = fedHeader->eventid & (~0x50000000);

        // Move to the next FED trailer
        fedTrailerAddr = fedTrailerAddr - fedSize;
    }

    if((fedTrailerAddr + sizeof(fedt_t)) != buf)
    {
        XCEPT_RAISE(emuFU::exception::Exception,
           "End of traversal is not the beginning of the first FED");
    }

    if(sumOfFedSizes != len)
    {
        stringstream oss;
        string       s;

        oss << "Super-fragment size calculated from FED trailers is different";
        oss << " from that calculated from FRL headers.";
        oss << " Super-fragment size from FED trailers: " << sumOfFedSizes;
        oss << " Super-fragment size from FRL headers: " << len;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }
}


void EmuFU::releaseSuperFragment()
{
    if(superFragmentHead_ != 0)
    {
        superFragmentHead_->release();

        superFragmentHead_ = 0;
        superFragmentTail_ = 0;
    }
}


void EmuFU::checkTAPayload(toolbox::mem::Reference *bufRef)
throw (emuFU::exception::Exception)
{
    unsigned long                      bufSize              = 0;
    unsigned long                      minimumBufSize       = 0;
    char                               *payload             = 0;
    frlh_t                             *frlHeader           = 0;
    I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block               = 0;
    unsigned int                       expectedSegSize      = 0;
    unsigned int                       segSize              = 0;
    fedh_t                             *fedHeader           = 0;
    fedt_t                             *fedTrailer          = 0;
    unsigned int                       expectedFedEventSize = 0;
    unsigned int                       fedEventSize         = 0;


    bufSize = bufRef->getDataSize();
    minimumBufSize = sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) +
                     sizeof(frlh_t) + sizeof(fedh_t) + sizeof(fedt_t);

    if(bufSize < minimumBufSize)
    {
        stringstream oss;
        string       s;

        oss << "Trigger message is too small.";
        oss << " Minimum size: " << minimumBufSize;
        oss << " Received: "     << bufSize;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    payload = ((char*)bufRef->getDataLocation()) +
              sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);

    frlHeader = (frlh_t*)(payload);

    block = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)bufRef->getDataLocation();

    if(frlHeader->trigno != block->eventNumber)
    {
        stringstream oss;
        string       s;

        oss << "Trigger FRL header \"trigno\" does not match";
        oss << " RU builder header \"eventNumber\"";
        oss << " trigno: " << frlHeader->trigno;
        oss << " eventNumber: " << block->eventNumber;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    if(frlHeader->segno != 0)
    {
        stringstream oss;
        string       s;

        oss << "Trigger FRL header segment number is not 0.";
        oss << " Received: " << frlHeader->segno;
        s = oss.str();
    }

    expectedSegSize = bufSize - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) -
                      sizeof(frlh_t);
    segSize = frlHeader->segsize & FRL_SEGSIZE_MASK;

    if(segSize != expectedSegSize)
    {
        stringstream oss;
        string       s;

        oss << "Trigger FRL header segment size is not as expected.";
        oss << " Expected: " << expectedSegSize;
        oss << " Received: " << segSize;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    fedHeader = (fedh_t*)(payload + sizeof(frlh_t));

    // Note: Event number is 24 bits
    if((fedHeader->eventid & 0x00FFFFFF) != block->eventNumber)
    {
        stringstream oss;
        string       s;

        oss << "Trigger FED header \"evenid\" does not match";
        oss << " RU builder header \"eventNumber\"";
        oss << " eventid: " << (fedHeader->eventid & 0x00FFFFFF);
        oss << " eventNumber: " << block->eventNumber;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    fedTrailer = (fedt_t*)(payload + sizeof(frlh_t) + segSize - sizeof(fedt_t));

    expectedFedEventSize = expectedSegSize >> 3;
    fedEventSize = fedTrailer->eventsize & FED_EVSZ_MASK;

    if(fedEventSize != expectedFedEventSize)
    {
        stringstream oss;
        string       s;

        oss << "Trigger FED trailer event size is not as expected.";
        oss << " Expected: " << expectedFedEventSize;
        oss << " Received: " << fedEventSize;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }
}


void EmuFU::checkRUIPayload(toolbox::mem::Reference *bufRef)
throw (emuFU::exception::Exception)
{
    char                               *blockAddr      = 0;
    char                               *frlHeaderAddr  = 0;
    I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block          = 0;
    frlh_t                             *frlHeader      = 0;
    unsigned long                      bufSize         = 0;
    unsigned int                       expectedSegSize = 0;
    unsigned int                       segSize         = 0;


    blockAddr     = (char *)bufRef->getDataLocation();
    frlHeaderAddr = blockAddr + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);

    block     = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)blockAddr;
    frlHeader = (frlh_t*)frlHeaderAddr;

    if(block->eventNumber != frlHeader->trigno)
    {
        stringstream oss;
        string       s;

        oss << "Event number of EmuFU header does not match that of FRL header";
        oss << " block->eventNumber: " << block->eventNumber;
        oss << " frlHeader->trigno: " << frlHeader->trigno;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    if(block->blockNb != frlHeader->segno)
    {
        stringstream oss;
        string       s;

        oss << "Block number of EmuFU header does not match that of FRL header";
        oss << " block->blockNb: " << block->blockNb;
        oss << " frlHeader->segno: " << frlHeader->segno;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    if(block->blockNb != blockNb_)
    {
        stringstream oss;
        string       s;

        oss << "Incorrect block number.";
        oss << " Expected: " << blockNb_;
        oss << " Received: " << block->blockNb;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    bufSize = bufRef->getDataSize();
    expectedSegSize = bufSize - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) -
                      sizeof(frlh_t);
    segSize = frlHeader->segsize & FRL_SEGSIZE_MASK;

    if(segSize != expectedSegSize)
    {
        stringstream oss;
        string       s;

        oss << "RUI FRL header segment size is not as expected.";
        oss << " Expected: " << expectedSegSize;
        oss << " Received: " << segSize;
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    // Check that EmuFU and FRL headers agree on end of super-fragment
    if
    (
        (block->blockNb == (block->nbBlocksInSuperFragment - 1)) !=
        ((frlHeader->segsize & FRL_LAST_SEGM) != 0)
    )
    {
        stringstream oss;
        string       s;

        oss << "End of super-fragment of EmuFU header does not match FRL header.";
        oss << " EmuFU header: ";
        oss << (block->blockNb == (block->nbBlocksInSuperFragment - 1));
        oss << " FRL header: ";
        oss << ((frlHeader->segsize & (~FRL_LAST_SEGM)) != 0);
        s = oss.str();

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }

    // If end of super-fragment
    if(block->blockNb == (block->nbBlocksInSuperFragment - 1))
    {
        blockNb_ = 0;
    }
    else
    {
        blockNb_++;
    }
}


void EmuFU::allocateNEvents(const int n)
throw (emuFU::exception::Exception)
{
    toolbox::mem::Reference *bufRef = createBuAllocateMsg
    (
        poolFactory_,
        i2oPool_,
        tid_,
        buTid_,
        n
    );

    try
    {
        appContext_->postFrame
        (
            bufRef,
            appDescriptor_,
            buDescriptor_,
            i2oExceptionHandler_,
            buDescriptor_
        );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to send I2O_BU_ALLOCATE_MESSAGE_FRAME to";
        oss << " BU" << buInstNb_;
        s = oss.str();

        XCEPT_RETHROW(emuFU::exception::Exception, s, e);
    }
}


toolbox::mem::Reference *EmuFU::createBuAllocateMsg
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    toolbox::mem::Pool              *pool,
    const I2O_TID                   taTid,
    const I2O_TID                   buTid,
    const int                       nbEvents
)
throw (emuFU::exception::Exception)
{
    toolbox::mem::Reference       *bufRef = 0;
    I2O_MESSAGE_FRAME             *stdMsg = 0;
    I2O_PRIVATE_MESSAGE_FRAME     *pvtMsg = 0;
    I2O_BU_ALLOCATE_MESSAGE_FRAME *msg    = 0;
    size_t                        msgSize = 0;


    msgSize = sizeof(I2O_BU_ALLOCATE_MESSAGE_FRAME) +
             (nbEvents - 1) * sizeof(BU_ALLOCATE);

    try
    {
        bufRef = poolFactory->getFrame(pool, msgSize);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to get an I2O_BU_ALLOCATE_MESSAGE_FRAME", e);
    }
    bufRef->setDataSize(msgSize);

    stdMsg = (I2O_MESSAGE_FRAME*)bufRef->getDataLocation();
    pvtMsg = (I2O_PRIVATE_MESSAGE_FRAME*)stdMsg;
    msg    = (I2O_BU_ALLOCATE_MESSAGE_FRAME*)stdMsg;

    stdMsg->MessageSize      = msgSize >> 2;
    stdMsg->InitiatorAddress = taTid;
    stdMsg->TargetAddress    = buTid;
    stdMsg->Function         = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset    = 0;
    stdMsg->MsgFlags         = 0;  // Point-to-point

    pvtMsg->XFunctionCode    = I2O_BU_ALLOCATE;
    pvtMsg->OrganizationID   = XDAQ_ORGANIZATION_ID;

    msg->n                   = nbEvents;

    // The EmuFU can specify a transaction id for each individual event requested
    for(int i=0; i<nbEvents; i++)
    {
        msg->allocate[i].fuTransactionId = 1234; // Dummy value
        msg->allocate[i].fset            = 0;    // IGNORED!!!
    }

    return bufRef;
}


void EmuFU::discardEvent(const U32 buResourceId)
throw (emuFU::exception::Exception)
{
    toolbox::mem::Reference *bufRef = createBuDiscardMsg
    (
        poolFactory_,
        i2oPool_,
        tid_,
        buTid_,
        buResourceId
    );

    try
    {
        appContext_->postFrame
        (
            bufRef,
            appDescriptor_,
            buDescriptor_,
            i2oExceptionHandler_,
            buDescriptor_
        );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to send I2O_BU_DISCARD_MESSAGE_FRAME to";
        oss << " BU" << buInstNb_;
        s = oss.str();

        XCEPT_RETHROW(emuFU::exception::Exception, s, e);
    }
}


toolbox::mem::Reference *EmuFU::createBuDiscardMsg
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    toolbox::mem::Pool              *pool,
    const I2O_TID                   taTid,
    const I2O_TID                   buTid,
    const U32                       buResourceId
)
throw (emuFU::exception::Exception)
{
    toolbox::mem::Reference      *bufRef  = 0;
    I2O_MESSAGE_FRAME            *stdMsg  = 0;
    I2O_PRIVATE_MESSAGE_FRAME    *pvtMsg  = 0;
    I2O_BU_DISCARD_MESSAGE_FRAME *msg     = 0;
    size_t                        msgSize = 0;


    msgSize = sizeof(I2O_BU_DISCARD_MESSAGE_FRAME);

    try
    {
        bufRef = poolFactory->getFrame(pool, msgSize);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to get an I2O_BU_DISCARD_MESSAGE_FRAME", e);
    }
    bufRef->setDataSize(msgSize);

    stdMsg = (I2O_MESSAGE_FRAME*)bufRef->getDataLocation();
    pvtMsg = (I2O_PRIVATE_MESSAGE_FRAME*)stdMsg;
    msg    = (I2O_BU_DISCARD_MESSAGE_FRAME*)stdMsg;

    stdMsg->MessageSize      = msgSize >> 2;
    stdMsg->InitiatorAddress = taTid;
    stdMsg->TargetAddress    = buTid;
    stdMsg->Function         = I2O_PRIVATE_MESSAGE;
    stdMsg->VersionOffset    = 0;
    stdMsg->MsgFlags         = 0;  // Point-to-point

    pvtMsg->XFunctionCode    = I2O_BU_DISCARD;
    pvtMsg->OrganizationID   = XDAQ_ORGANIZATION_ID;

    // It is possible to discard more than one event at a time, hence the
    // reason for the array of BU resource ids
    msg->n                   = 1;
    msg->buResourceId[0]     = buResourceId;

    return bufRef;
}


toolbox::mem::Pool *EmuFU::createHeapAllocatorMemoryPool
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    const string                     poolName
)
throw (emuFU::exception::Exception)
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

        XCEPT_RETHROW(emuFU::exception::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to create pool: " + poolName +
                   " : Unknown exception";

        XCEPT_RAISE(emuFU::exception::Exception, s);
    }
}


string EmuFU::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool EmuFU::onI2oException(xcept::Exception &exception, void *context)
{
    LOG4CPLUS_ERROR(logger_,
        " : " << xcept::stdformat_exception_history(exception));

    return true;
}

string EmuFU::createI2oErrorMsg
(
    xdaq::ApplicationDescriptor *source,
    xdaq::ApplicationDescriptor *destination
)
{
    stringstream oss;
    string       s;


    oss << "I2O exception from ";
    oss << source->getClassName();
    oss << " instance " << source->getInstance();
    oss << " to ";
    oss << destination->getClassName();
    oss << " instance " <<destination->getInstance();

    s = oss.str();

    return s;
}

xoap::MessageReference EmuFU::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emuFU::exception::Exception)
{
    string appNamespace = "urn:xdaq-application:" + appClass;
    string paramXsdType = "xsd:" + paramType;

    try
    {
        xoap::MessageReference message = xoap::createMessage();
        xoap::SOAPPart soapPart = message->getSOAPPart();
        xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
        envelope.addNamespaceDeclaration("xsi",
            "http://www.w3.org/2001/XMLSchema-instance");
        envelope.addNamespaceDeclaration("xsd",
            "http://www.w3.org/2001/XMLSchema");
        envelope.addNamespaceDeclaration("soapenc",
            "http://schemas.xmlsoap.org/soap/encoding/");
        xoap::SOAPBody body = envelope.getBody();
        xoap::SOAPName cmdName =
            envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
        xoap::SOAPBodyElement cmdElement =
            body.addBodyElement(cmdName);
        xoap::SOAPName propertiesName =
            envelope.createName("properties", "xapp", appNamespace);
        xoap::SOAPElement propertiesElement =
            cmdElement.addChildElement(propertiesName);
        xoap::SOAPName propertiesTypeName =
            envelope.createName("type", "xsi",
             "http://www.w3.org/2001/XMLSchema-instance");
        propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
        xoap::SOAPName propertyName =
            envelope.createName(paramName, "xapp", appNamespace);
        xoap::SOAPElement propertyElement =
            propertiesElement.addChildElement(propertyName);
        xoap::SOAPName propertyTypeName =
             envelope.createName("type", "xsi",
             "http://www.w3.org/2001/XMLSchema-instance");

        propertyElement.addAttribute(propertyTypeName, paramXsdType);

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}


string EmuFU::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emuFU::exception::Exception)
{
    try
    {
        xoap::SOAPPart part = msg->getSOAPPart();
        xoap::SOAPEnvelope env = part.getEnvelope();
        xoap::SOAPBody body = env.getBody();
        DOMNode *bodyNode = body.getDOMNode();
        DOMNodeList *bodyList = bodyNode->getChildNodes();
        DOMNode *responseNode = findNode(bodyList, "ParameterGetResponse");
        DOMNodeList *responseList = responseNode->getChildNodes();
        DOMNode *propertiesNode = findNode(responseList, "properties");
        DOMNodeList *propertiesList = propertiesNode->getChildNodes();
        DOMNode *paramNode = findNode(propertiesList, paramName);
        DOMNodeList *paramList = paramNode->getChildNodes();
        DOMNode *valueNode = paramList->item(0);
        string paramValue = xoap::XMLCh2String(valueNode->getNodeValue());

        return paramValue;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuFU::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuFU::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

string EmuFU::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emuFU::exception::Exception)
{
    string appClass = appDescriptor->getClassName();
    string value    = "";

    try
    {
        xoap::MessageReference msg =
            createParameterGetSOAPMsg(appClass, paramName, paramType);

        xoap::MessageReference reply =
            appContext_->postSOAP(msg, *appDescriptor_, *appDescriptor);

        // Check if the reply indicates a fault occurred
        xoap::SOAPBody replyBody =
            reply->getSOAPPart().getEnvelope().getBody();

        if(replyBody.hasFault())
        {
            stringstream oss;
            string s;

            oss << "Received fault reply: ";
            oss << replyBody.getFault().getFaultString();
            s = oss.str();

            XCEPT_RAISE(emuFU::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emuFU::exception::Exception, s, e);
    }

    return value;
}


void EmuFU::getRunInfo()
  // Gets the run number and start time from TA
throw (emuFU::exception::Exception)
{
  runNumber_    = 0;
  isBookedRunNumber_ = false;
  runStartTime_ = "YYMMDD_hhmmss_UTC";


  try
    {
      taDescriptors_ = zone_->getApplicationDescriptors( "EmuTA" );
    }
  catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
      taDescriptors_.clear();
      XCEPT_RETHROW(emuFU::exception::Exception, 
		    "Failed to get application descriptors for class EmuTA",
		    e);
    }

  string rn="";
  string mn="";
  string br="";
  if ( taDescriptors_.size() >= 1 ){
    if ( taDescriptors_.size() > 1 )
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      taDescriptors_.size() << " emuTA instances found. Trying first one.");
    rn = getScalarParam(*taDescriptors_.begin(),"runNumber","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got run number from emuTA: " + rn );
    br = getScalarParam(*taDescriptors_.begin(),"isBookedRunNumber","boolean");
    LOG4CPLUS_INFO(logger_, "Got info on run booking from emuTA: " + br );
    runStartTime_ = getScalarParam(*taDescriptors_.begin(),"runStartTime","string");
    LOG4CPLUS_INFO(logger_, "Got run start time from emuTA: " + runStartTime_.toString() );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find EmuTA. ==> Run number and start time are unknown.");
  }

  unsigned int  irn(0);
  istringstream srn(rn);
  srn >> irn;
  runNumber_ = irn;

  isBookedRunNumber_ = ( br == "true" );

}

xoap::MessageReference EmuFU::onReset(xoap::MessageReference msg)
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
  
  try{
    fsm_.reset();
  }
  catch( toolbox::fsm::exception::Exception e ){
    XCEPT_RETHROW(xoap::exception::Exception,
		  "Failed to reset FSM: ", e);
  }

  toolbox::Event::Reference evtRef(new toolbox::Event("Halt", this));
  fsm_.fireEvent( evtRef );
  
  return createFsmResponseMsg(cmdName, stateName_.toString());
}

/**
 * Provides the factory method for the instantiation of CLASS applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuFU)
