#include "emu/daq/fu/Application.h"
#include "emu/daq/fu/version.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "i2o/Method.h"
#include "interface/evb/i2oEVBMsgs.h"
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
#include "emu/daq/server/I2O.h"
#include "emu/daq/server/SOAP.h"
#include <sstream>

#include <unistd.h>


emu::daq::fu::Application::Application(xdaq::ApplicationStub *s)
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
        "/emu/daq/fu/images/EmuFU64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester(zone_);

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &emu::daq::fu::Application::onI2oException, "onI2oException");

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

    runStartUTC_ = 0;

    fileWriter_ = NULL;

    bindFsmSoapCallbacks();
    bindI2oCallbacks();

    // Bind web interface
    xgi::bind(this, &emu::daq::fu::Application::css           , "styles.css");
    xgi::bind(this, &emu::daq::fu::Application::defaultWebPage, "Default"   );

    // bind SOAP client credit message callback
    xoap::bind(this, &emu::daq::fu::Application::onSOAPClientCreditMsg, 
	       "onClientCreditMessage", XDAQ_NS_URI);

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::fu::Application::generateLoggerName()
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

string emu::daq::fu::Application::extractParametersFromSOAPClientCreditMsg
(
    xoap::MessageReference msg, unsigned int& instance, int& credits, int& prescaling 
)
throw (emu::daq::fu::exception::Exception)
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
	if ( ci >= 0 ){
	  instance = ci;
	}
	else{
	  LOG4CPLUS_ERROR(logger_, "Instance of SOAP client " << clientName << "is negative." );
	 stringstream ss0;
	 ss0 <<  "Instance of SOAP client " << clientName << "is negative." ;
	 XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss0.str() );
	 this->notifyQualified( "error", eObj );
	}

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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Parameter(s) not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::fu::exception::Exception,
            "Parameter(s) not found");
    }
}

xoap::MessageReference emu::daq::fu::Application::processSOAPClientCreditMsg( xoap::MessageReference msg )
  throw( emu::daq::fu::exception::Exception )
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


xoap::MessageReference emu::daq::fu::Application::onSOAPClientCreditMsg( xoap::MessageReference msg )
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
	  catch( emu::daq::fu::exception::Exception e )
	    {
	      XCEPT_RETHROW(xoap::exception::Exception, string("Failed to process SOAP client credit message"), e);
	    }
	  break;
        case 'R': // Ready
        case 'S': // Suspended
	  break;
        default:
	  LOG4CPLUS_ERROR(logger_,
			  "emu::daq::fu::Application in undefined state");
	  stringstream ss1;
	  ss1 << 
			  "emu::daq::fu::Application in undefined state";
	  XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss1.str() );
	  this->notifyQualified( "error", eObj );
        }
    }
  catch(xcept::Exception e)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process client credit message : "
		      << stdformat_exception_history(e));
      stringstream ss2;
      ss2 << 
		      "Failed to process client credit message : "
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss2.str(), e );
      this->notifyQualified( "error", eObj );
    }
  catch(...)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process client credit message : Unknown exception");
      stringstream ss3;
      ss3 << 
		      "Failed to process client credit message : Unknown exception";
      XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss3.str() );
      this->notifyQualified( "error", eObj );
    }
  
  
  if ( reply.isNull() ) reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  bSem_.give();

  return reply;
}

void emu::daq::fu::Application::onI2OClientCreditMsg(toolbox::mem::Reference *bufRef)
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
			  "emu::daq::fu::Application in undefined state");
	  stringstream ss4;
	  ss4 << 
			  "emu::daq::fu::Application in undefined state";
	  XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss4.str() );
	  this->notifyQualified( "error", eObj );
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process client credit message : "
             << stdformat_exception_history(e));
        stringstream ss5;
        ss5 << 
            "Failed to process client credit message : "
             ;
        XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss5.str(), e );
        this->notifyQualified( "error", eObj );
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process client credit message : Unknown exception");
        stringstream ss6;
        ss6 << 
            "Failed to process client credit message : Unknown exception";
        XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss6.str() );
        this->notifyQualified( "error", eObj );
    }

    // Free the client's event credit message
    bufRef->release();

    bSem_.give();

}

xdaq::ApplicationDescriptor *emu::daq::fu::Application::getRUBuilderTester
(
    xdaq::Zone *zone
)
{
    xdaq::ApplicationDescriptor *appDescriptor = 0;


    try
    {
        appDescriptor =
            zone->getApplicationDescriptor("emu::daq::manager::Application", 0);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        appDescriptor = 0;
    }

    return appDescriptor;
}

DOMNode *emu::daq::fu::Application::findNode(DOMNodeList *nodeList,
			 const string nodeLocalName)
  throw (emu::daq::fu::exception::Exception)
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

    XCEPT_RAISE(emu::daq::fu::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


string emu::daq::fu::Application::createI2oPoolName(const unsigned long emuFUInstance)
{
    stringstream oss;
    string       s;

    oss << "EmuFU" << emuFUInstance << "_i2oFragmentPool";
    s = oss.str();

    return s;
}


void emu::daq::fu::Application::defineFsm()
throw (emu::daq::fu::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &emu::daq::fu::Application::stateChanged);
        fsm_.addState('R', "Ready"    , this, &emu::daq::fu::Application::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &emu::daq::fu::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &emu::daq::fu::Application::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &emu::daq::fu::Application::enableAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &emu::daq::fu::Application::haltAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &emu::daq::fu::Application::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &emu::daq::fu::Application::haltAction);

        fsm_.addStateTransition('H', 'F', "Fail", this, &emu::daq::fu::Application::failAction);
        fsm_.addStateTransition('R', 'F', "Fail", this, &emu::daq::fu::Application::failAction);
        fsm_.addStateTransition('E', 'F', "Fail", this, &emu::daq::fu::Application::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &emu::daq::fu::Application::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &emu::daq::fu::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to reset FSM", e);
    }
}


string emu::daq::fu::Application::generateMonitoringInfoSpaceName
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


vector< pair<string, xdata::Serializable*> > emu::daq::fu::Application::initAndGetStdConfigParams()
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


vector< pair<string, xdata::Serializable*> > emu::daq::fu::Application::initAndGetStdMonitorParams()
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


void emu::daq::fu::Application::putParamsIntoInfoSpace
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


void emu::daq::fu::Application::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
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


void emu::daq::fu::Application::bindFsmSoapCallbacks()
{
    xoap::bind(this, &emu::daq::fu::Application::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::fu::Application::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::fu::Application::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::fu::Application::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);

    xoap::bind(this, &emu::daq::fu::Application::onReset,           "Reset"    , XDAQ_NS_URI);
}

void emu::daq::fu::Application::moveToFailedState(){ // Emu-specific
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
      stringstream ss7;
      ss7 << 
		      "Failed to move to the Failed state : "
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss7.str(), e );
      this->notifyQualified( "fatal", eObj );
    }
}


xoap::MessageReference emu::daq::fu::Application::processSoapFsmCmd(xoap::MessageReference msg)
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
        stringstream ss8;
        ss8 << 
            s << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss8.str(), e );
        this->notifyQualified( "error", eObj );
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
        stringstream ss9;
        ss9 << 
            s << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss9.str(), e );
        this->notifyQualified( "error", eObj );
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
        stringstream ss10;
        ss10 <<  s;
        XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss10.str() );
        this->notifyQualified( "error", eObj );
        XCEPT_RAISE(xoap::exception::Exception, s);
    }
}


string emu::daq::fu::Application::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emu::daq::fu::exception::Exception)
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

    XCEPT_RAISE(emu::daq::fu::exception::Exception, "Command name not found");
}


xoap::MessageReference emu::daq::fu::Application::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::fu::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


void emu::daq::fu::Application::processFsmCommand(const string cmdName)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        bSem_.give();
        XCEPT_RAISE(emu::daq::fu::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}

void emu::daq::fu::Application::destroyServers(){
  std::vector<Client*>::iterator c;
  for ( c=clients_.begin(); c!=clients_.end(); ++c ){
    LOG4CPLUS_INFO(logger_, string("Destroying server for ") + (*c)->server->getClientName() );
    delete (*c)->server;
    *(*c)->creditsHeld = 0;
  }
  clients_.clear();
}

bool emu::daq::fu::Application::createI2OServer( string clientName, unsigned int clientInstance  ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>       ( clientName_.elementAt( iClient )     )) = clientName;
    *(dynamic_cast<xdata::UnsignedLong*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>       ( clientProtocol_.elementAt( iClient ) )) = "I2O";
    *(dynamic_cast<xdata::Boolean*>      ( clientPersists_.elementAt( iClient ) )) = true;
    emu::daq::server::I2O* s = new emu::daq::server::I2O( this,
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

bool emu::daq::fu::Application::createSOAPServer( string clientName,  unsigned int clientInstance, bool persistent ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>       (     clientName_.elementAt( iClient ) )) = clientName;
    *(dynamic_cast<xdata::UnsignedLong*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>       ( clientProtocol_.elementAt( iClient ) )) = "SOAP";
    *(dynamic_cast<xdata::Boolean*>      ( clientPersists_.elementAt( iClient ) )) = persistent;
    emu::daq::server::SOAP* s = new emu::daq::server::SOAP( this,
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
    stringstream ss11;
    ss11 <<  
		   "Maximum number of clients exceeded. Cannot create server for " << clientName ;
    XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss11.str() );
    this->notifyQualified( "warn", eObj );
  return created;
}


void emu::daq::fu::Application::createServers(){
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
	stringstream ss12;
	ss12 <<  "Unknown protocol \"" <<
			clientProtocol_.elementAt(iClient)->toString() << 
			"\" for client " <<
			clientName_.elementAt(iClient)->toString() << 
			" instance " <<
			clientInstance <<
			". Please use \"I2O\" or \"SOAP\".";
	XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss12.str() );
	this->notifyQualified( "error", eObj );
    }
  }
}

xoap::MessageReference 
emu::daq::fu::Application::createSimpleSOAPCmdMsg( const string cmdName )
  throw (emu::daq::fu::exception::Exception)
{
    try
    {
        xoap::MessageReference message = xoap::createMessage();
        xoap::SOAPPart soapPart = message->getSOAPPart();
        xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
        xoap::SOAPBody body = envelope.getBody();
        xoap::SOAPName cmdSOAPName =
            envelope.createName(cmdName, "xdaq", "urn:xdaq-soap:3.0");

        body.addBodyElement(cmdSOAPName);

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}

void 
emu::daq::fu::Application::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emu::daq::fu::exception::Exception)
{
    try
    {
        xoap::MessageReference msg = createSimpleSOAPCmdMsg(eventName);
        xoap::MessageReference reply =
            appContext_->postSOAP(msg, *appDescriptor_, *appDescriptor);

        // Check if the reply indicates a fault occurred
        xoap::SOAPBody replyBody =
            reply->getSOAPPart().getEnvelope().getBody();

        if(replyBody.hasFault())
        {
            stringstream oss;
            string s;

            oss << "Received fault reply from ";
            oss << appDescriptor->getClassName();
            oss << appDescriptor->getInstance();
            oss << " : " << replyBody.getFault().getFaultString();
            s = oss.str();

            XCEPT_RAISE(emu::daq::fu::exception::Exception, s);
        }
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to send FSM event to ";
        oss << appDescriptor->getClassName();
        oss << appDescriptor->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }
}

void 
emu::daq::fu::Application::startATCP()
  throw (emu::daq::fu::exception::Exception){
  // configure and enable all pt::atcp::PeerTransportATCP

  std::cout << "In emu::daq::fu::Application::startATCP()" << std::endl;

  vector < xdaq::ApplicationDescriptor* > atcpDescriptors;
  try{
      atcpDescriptors = getAppDescriptors(zone_, "pt::atcp::PeerTransportATCP");
  }
  catch(emu::daq::fu::exception::Exception e){
    LOG4CPLUS_WARN(logger_, "Failed to get atcp descriptors : " 
		   + xcept::stdformat_exception_history(e) );
    stringstream ss13;
    ss13 <<  "Failed to get atcp descriptors : " 
		    ;
    XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss13.str(), e );
    this->notifyQualified( "warn", eObj );
    atcpDescriptors.clear();
  }

  std::cout << atcpDescriptors.size() << " atcpDescriptors" << std::endl;

  vector < xdaq::ApplicationDescriptor* >::const_iterator atcpd;
  for ( atcpd = atcpDescriptors.begin(); atcpd != atcpDescriptors.end(); ++atcpd ){

    std::cout << appDescriptor_->getContextDescriptor()->getURL() << "         "
	      << (*atcpd)->getContextDescriptor()->getURL() << std::endl;

    // Handle only the pt::atcp::PeerTransportATCP that's in the same Context
    if ( (*atcpd)->getContextDescriptor() == appDescriptor_->getContextDescriptor() ){

      // ATCP may already have been started. Check its state.
      string atcpState;
      try{
	atcpState = getScalarParam( *atcpd, "stateName", "string" );
      }
      catch(emu::daq::fu::exception::Exception e)
	{
	  stringstream oss;
	  oss << "Failed to get state of " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	  XCEPT_RETHROW(emu::daq::fu::exception::Exception, oss.str(), e);
	}

      if ( atcpState != "Halted" ) continue;

      // Configure ATCP
      try{
	sendFSMEventToApp("Configure", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::fu::exception::Exception, oss.str(), e);
      }

      // Enable ATCP
      try{
	sendFSMEventToApp("Enable", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::fu::exception::Exception, oss.str(), e);
      }

    }

  }
  
}

void emu::daq::fu::Application::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

  // ATCP must be started explicitly
  try{
    startATCP();
  }
  catch(xcept::Exception e){
    XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to start ATCP ", e);
  }

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
	stringstream ss14;
	ss14 <<  "Terminating leftover file writer." ;
	XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss14.str() );
	this->notifyQualified( "warn", eObj );
	fileWriter_->endRun();
	delete fileWriter_;
	fileWriter_ = NULL;
      }
    // create new writer if path is not empty
    if ( pathToDataOutFile_ != string("") && (xdata::UnsignedLongT) fileSizeInMegaBytes_ > (long unsigned int) 0 ){
      toolbox::net::URL u( appContext_->getContextDescriptor()->getURL() );
      fileWriter_ = new emu::daq::writer::RawDataFile( 1000000*fileSizeInMegaBytes_, 
						       pathToDataOutFile_.toString(), 
						       u.getHost(), "EmuFU", instance_, emudaqfu::versions, &logger_ );
    }
    
    // Create an Emu event header
    bool isCalibrationRun = ( runType_.toString().find("Calib") != string::npos );
    bool ruiZeroExists = false;
    set<xdaq::ApplicationDescriptor*> ruiDescriptors = zone_->getApplicationDescriptors( "emu::daq::rui::Application" );
    for ( set<xdaq::ApplicationDescriptor*>::const_iterator rui=ruiDescriptors.begin(); rui!=ruiDescriptors.end(); ++rui ){
      if ( (*rui)->getInstance() == 0 ){
	ruiZeroExists = true;
	break;
      }
    }
    emuEventHeaderTrailer_ = new emu::daq::fu::EmuEventHeaderTrailer( isCalibrationRun, !isCalibrationRun, ruiZeroExists );

    // Recreate servers
    destroyServers();
    createServers();

    runStartUTC_  = 0;
    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
}


void emu::daq::fu::Application::enableAction(toolbox::Event::Reference e)
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
	     &emu::daq::fu::Application::serverLoopAction,
	     "emu::daq::fu::Application server loop action"
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


void emu::daq::fu::Application::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

  // Get time of end of run
  try{
    if ( *taDescriptors_.begin() != NULL ){
      runStopTime_ = getScalarParam(*taDescriptors_.begin(),"runStopTime","string");
      LOG4CPLUS_INFO(logger_, "Got run stop time from emu::daq::ta::Application: " << runStopTime_.toString() );
    }
  }
  catch( emu::daq::fu::exception::Exception e ){
    LOG4CPLUS_WARN(logger_, "Run stop time will be unknown: " << xcept::stdformat_exception_history(e) );
    stringstream ss15;
    ss15 <<  "Run stop time will be unknown: "  ;
    XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss15.str(), e );
    this->notifyQualified( "warn", eObj );
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


void emu::daq::fu::Application::failAction(toolbox::Event::Reference event)
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
        stringstream ss16;
        ss16 <<  s;
        XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss16.str() );
        this->notifyQualified( "fatal", eObj );
    }
    catch(bad_cast)
    {
        LOG4CPLUS_FATAL(logger_, "Moving to Failed state");
        stringstream ss17;
        ss17 <<  "Moving to Failed state";
        XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss17.str() );
        this->notifyQualified( "fatal", eObj );
    }
}


void emu::daq::fu::Application::bindI2oCallbacks()
{
    i2o::bind
    (
        this,
        &emu::daq::fu::Application::I2O_FU_TAKE_Callback,
        I2O_FU_TAKE,
        XDAQ_ORGANIZATION_ID
    );

  // EMu-specific stuff
  i2o::bind(this, &emu::daq::fu::Application::onI2OClientCreditMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );
}



bool emu::daq::fu::Application::serverLoopAction(toolbox::task::WorkLoop *wl)
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
                "emu::daq::fu::Application" << instance_ << " is in an undefined state");
            stringstream ss18;
            ss18 << 
                "emu::daq::fu::Application" << instance_ << " is in an undefined state";
            XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss18.str() );
            this->notifyQualified( "fatal", eObj );
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
        stringstream ss19;
        ss19 << 
            "Failed to execute \"self-driven\" behaviour"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss19.str(), e );
        this->notifyQualified( "fatal", eObj );

        // Do not reschedule this action code as the application has failed
        return false;
    }
}


void emu::daq::fu::Application::addDataForClients( const int   runNumber,
			       const int   runStartUTC,
			       const int   nEventsRead,
			       const bool  completesEvent, 
			       char* const data, 
			       const int   dataLength ){
  unsigned short dummyErrorFlag = 0; // We don't have the error info amy more at this point.
  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->addData( runNumber,
					runStartUTC,
					nEventsRead, 
					completesEvent,
					dummyErrorFlag,
					data, 
					dataLength );
}

void emu::daq::fu::Application::css
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


void emu::daq::fu::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << emudaqfu::versions<< endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/daq/fu/images/EmuFU64x64.gif\""<< endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << emudaqfu::versions
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
        *out << "     src=\"/emu/daq/manager/images/EmuDAQManager64x64.gif\"";
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


void emu::daq::fu::Application::printParamsTable
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

void emu::daq::fu::Application::I2O_FU_TAKE_Callback(toolbox::mem::Reference *bufRef)
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
      {
	LOG4CPLUS_ERROR(logger_,
			"Received a data block whilst in the Ready state");
	stringstream ss20;
	ss20 << "Received a data block whilst in the Ready state";
	XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss20.str() );
	this->notifyQualified( "error", eObj );
	bufRef->release();
      }
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
                    stringstream ss21;
                    ss21 << 
                        "Failed to process data block"
                        << " : " ;
                    XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss21.str(), e );
                    this->notifyQualified( "error", eObj );
                }

                // Move to the next block (could be 0!)
                bufRef = next;
            }
        }
        break;
    default:
      {
        LOG4CPLUS_ERROR(logger_, "Unknown application state");
        stringstream ss22;
        ss22 <<  "Unknown application state";
        XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss22.str() );
        this->notifyQualified( "error", eObj );
        bufRef->release();
      }
    }


    bSem_.give();
}


void emu::daq::fu::Application::processDataBlock(toolbox::mem::Reference *bufRef)
throw (emu::daq::fu::exception::Exception)
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
		stringstream ss23;
		ss23 <<  e ;
		XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss23.str() );
		this->notifyQualified( "fatal", eObj );
		moveToFailedState();
	      }
	    }
	    try{
	      fileWriter_->startNewEvent();
	    }
	    catch(string e){
	      LOG4CPLUS_FATAL( logger_, e );
	      stringstream ss24;
	      ss24 <<  e ;
	      XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss24.str() );
	      this->notifyQualified( "fatal", eObj );
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
	  stringstream ss25;
	  ss25 <<  e ;
	  XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss25.str() );
	  this->notifyQualified( "fatal", eObj );
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
	    stringstream ss26;
	    ss26 <<  e ;
	    XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss26.str() );
	    this->notifyQualified( "fatal", eObj );
	    moveToFailedState();
	  }
	}
      
      addDataForClients( runNumber_.value_,
			 runStartUTC_,
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

            XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
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
	    stringstream ss27;
	    ss27 <<  e ;
	    XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss27.str() );
	    this->notifyQualified( "fatal", eObj );
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

            XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
        }

        try
        {
            allocateNEvents(1);
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emu::daq::fu::exception::Exception,
                "Failed to allocate an event", e);
        }
    }



}


void emu::daq::fu::Application::printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader )
  //
  // EMu-specific stuff
  //
{
  if ( !bufRef )
    {
      std::cout << "emu::daq::fu::Application::printBlock: no buffer?!" << endl;
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
      std::cout << "emu::daq::fu::Application::printBlock:" << endl;
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

void emu::daq::fu::Application::appendBlockToSuperFragment(toolbox::mem::Reference *bufRef)
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


void emu::daq::fu::Application::releaseSuperFragment()
{
    if(superFragmentHead_ != 0)
    {
        superFragmentHead_->release();

        superFragmentHead_ = 0;
        superFragmentTail_ = 0;
    }
}


void emu::daq::fu::Application::allocateNEvents(const int n)
throw (emu::daq::fu::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }
}


toolbox::mem::Reference *emu::daq::fu::Application::createBuAllocateMsg
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    toolbox::mem::Pool              *pool,
    const I2O_TID                   taTid,
    const I2O_TID                   buTid,
    const int                       nbEvents
)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
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


void emu::daq::fu::Application::discardEvent(const U32 buResourceId)
throw (emu::daq::fu::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }
}


toolbox::mem::Reference *emu::daq::fu::Application::createBuDiscardMsg
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    toolbox::mem::Pool              *pool,
    const I2O_TID                   taTid,
    const I2O_TID                   buTid,
    const U32                       buResourceId
)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
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


toolbox::mem::Pool *emu::daq::fu::Application::createHeapAllocatorMemoryPool
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    const string                     poolName
)
throw (emu::daq::fu::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to create pool: " + poolName +
                   " : Unknown exception";

        XCEPT_RAISE(emu::daq::fu::exception::Exception, s);
    }
}


string emu::daq::fu::Application::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool emu::daq::fu::Application::onI2oException(xcept::Exception &exception, void *context)
{
    LOG4CPLUS_ERROR(logger_,
        " : " << xcept::stdformat_exception_history(exception));
    stringstream ss28;
    ss28 << 
        " : " ;
    XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss28.str(), exception );
    this->notifyQualified( "error", eObj );

    return true;
}

string emu::daq::fu::Application::createI2oErrorMsg
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

xoap::MessageReference emu::daq::fu::Application::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}


string emu::daq::fu::Application::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emu::daq::fu::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::fu::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::fu::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

string emu::daq::fu::Application::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emu::daq::fu::exception::Exception)
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

            XCEPT_RAISE(emu::daq::fu::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }

    return value;
}

time_t emu::daq::fu::Application::toUnixTime( const std::string YYMMDD_hhmmss_UTC ){
  if ( YYMMDD_hhmmss_UTC.size() < 17 ) return time_t(0);

  struct tm stm;
  std::stringstream ss;

  ss << YYMMDD_hhmmss_UTC.substr( 0,2); ss >> stm.tm_year; ss.clear(); stm.tm_year += 100;
  ss << YYMMDD_hhmmss_UTC.substr( 2,2); ss >> stm.tm_mon;  ss.clear(); stm.tm_mon  -= 1;
  ss << YYMMDD_hhmmss_UTC.substr( 4,2); ss >> stm.tm_mday; ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr( 7,2); ss >> stm.tm_hour; ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr( 9,2); ss >> stm.tm_min;  ss.clear();
  ss << YYMMDD_hhmmss_UTC.substr(11,2); ss >> stm.tm_sec;  ss.clear();

  time_t unixTime = mktime( &stm );

  return ( unixTime < 0 ? time_t(0) : unixTime );
}


void emu::daq::fu::Application::getRunInfo()
  // Gets the run number and start time from TA
throw (emu::daq::fu::exception::Exception)
{
  runNumber_    = 0;
  isBookedRunNumber_ = false;
  runStartTime_ = "YYMMDD_hhmmss_UTC";
  runStartUTC_ = 0;

  try
    {
      taDescriptors_ = zone_->getApplicationDescriptors( "emu::daq::ta::Application" );
    }
  catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
      taDescriptors_.clear();
      XCEPT_RETHROW(emu::daq::fu::exception::Exception, 
		    "Failed to get application descriptors for class emu::daq::ta::Application",
		    e);
    }

  string rn="";
  string mn="";
  string br="";
  if ( taDescriptors_.size() >= 1 ){
    if ( taDescriptors_.size() > 1 )
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      taDescriptors_.size() << " emu::daq::ta::Application instances found. Trying first one.");
      stringstream ss29;
      ss29 <<  "The embarassement of riches: " << 
		      taDescriptors_.size() << " emu::daq::ta::Application instances found. Trying first one.";
      XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss29.str() );
      this->notifyQualified( "error", eObj );
    rn = getScalarParam(*taDescriptors_.begin(),"runNumber","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got run number from emu::daq::ta::Application: " + rn );
    br = getScalarParam(*taDescriptors_.begin(),"isBookedRunNumber","boolean");
    LOG4CPLUS_INFO(logger_, "Got info on run booking from emu::daq::ta::Application: " + br );
    runStartTime_ = getScalarParam(*taDescriptors_.begin(),"runStartTime","string");
    runStartUTC_ = toUnixTime( runStartTime_ );
    LOG4CPLUS_INFO(logger_, "Got run start time from emu::daq::ta::Application: " + runStartTime_.toString() + " or " << runStartUTC_ );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find emu::daq::ta::Application. ==> Run number and start time are unknown.");
    stringstream ss30;
    ss30 <<  "Did not find emu::daq::ta::Application. ==> Run number and start time are unknown.";
    XCEPT_DECLARE( emu::daq::fu::exception::Exception, eObj, ss30.str() );
    this->notifyQualified( "error", eObj );
  }

  unsigned int  irn(0);
  istringstream srn(rn);
  srn >> irn;
  runNumber_ = irn;

  isBookedRunNumber_ = ( br == "true" );

}

xoap::MessageReference emu::daq::fu::Application::onReset(xoap::MessageReference msg)
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
      stringstream ss31;
      ss31 << 
		      s << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::fu::exception::Exception, eObj, ss31.str(), e );
      this->notifyQualified( "error", eObj );
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

vector< xdaq::ApplicationDescriptor* > emu::daq::fu::Application::getAppDescriptors
(
    xdaq::Zone             *zone,
    const string           appClass
)
throw (emu::daq::fu::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
    set< xdaq::ApplicationDescriptor* > descriptors;
    int nbApps = 0;

    try
    {
        descriptors = zone->getApplicationDescriptors(appClass);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        string s;

        s = "Failed to get application descriptors for class: " + appClass;

        XCEPT_RETHROW(emu::daq::fu::exception::Exception, s, e);
    }

    nbApps = descriptors.size();

    // Fill application descriptors in instance order allowing non-contiguous numbering
    while( !descriptors.empty() ){
      // Find app with smallest instance number
      unsigned int minInstance = 99999;
      set< xdaq::ApplicationDescriptor* >::iterator adOfSmallest;
      set< xdaq::ApplicationDescriptor* >::iterator ad;
      for ( ad=descriptors.begin(); ad!=descriptors.end(); ++ad )
	if ( (*ad)->getInstance() < minInstance ){
	  adOfSmallest = ad;
	  minInstance  = (*ad)->getInstance();
	}
      // Append it to the ordered vector
      orderedDescriptors.push_back( *adOfSmallest );
      // Remove it from the unordered vector
      descriptors.erase( adOfSmallest );
    }

    return orderedDescriptors;
}

/**
 * Provides the factory method for the instantiation of CLASS applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::fu::Application)
