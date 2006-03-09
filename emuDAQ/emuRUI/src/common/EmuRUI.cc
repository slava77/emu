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
#include "emuDAQ/emuReadout/include/EmuFileReader.h"
#include "emuDAQ/emuReadout/include/EmuSpyReader.h"
#include "emuDAQ/emuUtil/include/EmuI2OServer.h"
#include "emuDAQ/emuUtil/include/EmuSOAPServer.h"
#include <sstream>

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

    //
    // EMu-specific
    //
    fileWriter_          = NULL;
    badEventsFileWriter_ = NULL;

    // bind SOAP client credit message callback
    xoap::bind(this, &EmuRUI::onSOAPClientCreditMsg, 
	       "onClientCreditMessage", XDAQ_NS_URI);

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

string EmuRUI::extractParametersFromSOAPClientCreditMsg
(
    xoap::MessageReference msg, int& credits, int& prescaling 
)
throw (emuRUI::exception::Exception)
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
        parameterNode = findNode(parameterList, "nEventCredits");
        string sc              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());
	parameterNode          = findNode(parameterList, "prescalingFactor");
        string sp              = xoap::XMLCh2String(parameterNode->getFirstChild()->getNodeValue());

        LOG4CPLUS_DEBUG(logger_, 
			"Received from "          << clientName << 
			" nEventCredits = "       << sc << 
			", prescalingFactor = 1/" << sp );

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
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Parameter(s) not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuRUI::exception::Exception,
            "Parameter(s) not found");
    }
}

xoap::MessageReference EmuRUI::processSOAPClientCreditMsg( xoap::MessageReference msg )
  throw( emuRUI::exception::Exception )
{
  xoap::MessageReference reply;

  int credits = 0, prescaling = 1;
  string name = extractParametersFromSOAPClientCreditMsg( msg, credits, prescaling );

  // Find out who sent this and add the credits to its corresponding server
  bool knownClient = false;
  for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
    if ( (*c)->name->toString() == name ){
      knownClient = true;
      (*c)->server->addCredits( credits, prescaling );
      // If client descriptor is not known (non-XDAQ client), send data now:
      if ( (*c)->server->getClientDescriptor() == NULL ){
	reply = (*c)->server->getOldestMessagePendingTransmission();
	if ( !reply.isNull() ){ 
// 	  string rs;
// 	  reply->writeTo(rs);
	  LOG4CPLUS_DEBUG(logger_, string("***** Sending data to non-XDAQ SOAP client *****") );// << endl << rs << endl );
	}
      }
      break;
    }
  }

  // If this client is not yet known, create a new (non-persistent) server for it...
  if ( !knownClient ){
    if ( createSOAPServer( name, false ) ){
      // ... and if successfully created, add credits
      for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
	if ( (*c)->server->getClientName() == name ){
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


xoap::MessageReference EmuRUI::onSOAPClientCreditMsg( xoap::MessageReference msg )
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

  applicationBSem_.take();

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
	  catch( emuRUI::exception::Exception e )
	    {
	      XCEPT_RETHROW(xoap::exception::Exception, string("Failed to process SOAP client credit message"), e);
	    }
	  break;
        case 'R': // Ready
        case 'S': // Suspended
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
  
  
  if ( reply.isNull() ) reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  applicationBSem_.give();

  return reply;
}

void EmuRUI::onI2OClientCreditMsg(toolbox::mem::Reference *bufRef)
  // EMu-specific stuff
{
    I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME *msg =
        (I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME*)bufRef->getDataLocation();


    applicationBSem_.take();

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

    // Free the client's event credit message
    bufRef->release();

    applicationBSem_.give();

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


vector< xdaq::ApplicationDescriptor* > EmuRUI::getAppDescriptors(xdaq::ApplicationGroup *appGroup,
								 const string            appClass)
  throw (emuRUI::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
    vector< xdaq::ApplicationDescriptor* > descriptors;
    xdaq::ApplicationDescriptor *descriptor = 0;
    int nbApps = 0;


    try
    {
        descriptors = appGroup->getApplicationDescriptors(appClass);
    }
    catch(emuRUI::exception::Exception e)
    {
        string s;

        s = "Failed to get application descriptors for class: " + appClass;

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }

    nbApps = descriptors.size();

    // Fill application descriptors in instance order
    for(int i=0; i<nbApps; i++)
    {
        try
        {
            descriptor = appGroup->getApplicationDescriptor(appClass, i);
        }
        catch(emuRUI::exception::Exception e)
        {
            stringstream oss;
            string s;

            oss << "Failed to get the application descriptor of ";
            oss << appClass << i;
            s = oss.str();

            XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
        }

        orderedDescriptors.push_back(descriptor);
    }

    return orderedDescriptors;
}

xoap::MessageReference EmuRUI::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emuRUI::exception::Exception)
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
            envelope.createName("properties", appClass, appNamespace);
        xoap::SOAPElement propertiesElement =
            cmdElement.addChildElement(propertiesName);
        xoap::SOAPName propertiesTypeName =
            envelope.createName("type", "xsi",
             "http://www.w3.org/2001/XMLSchema-instance");
        propertiesElement.addAttribute(propertiesTypeName, "soapenc:Struct");
        xoap::SOAPName propertyName =
            envelope.createName(paramName, appClass, appNamespace);
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
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}

DOMNode *EmuRUI::findNode(DOMNodeList *nodeList,
			  const string nodeLocalName)
  throw (emuRUI::exception::Exception)
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

    XCEPT_RAISE(emuRUI::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


string EmuRUI::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emuRUI::exception::Exception)
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
        XCEPT_RETHROW(emuRUI::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuRUI::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

string EmuRUI::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emuRUI::exception::Exception)
{
    string appClass = appDescriptor->getClassName();
    string value    = "";

    try
    {
        xoap::MessageReference msg =
            createParameterGetSOAPMsg(appClass, paramName, paramType);

        xoap::MessageReference reply =
            appContext_->postSOAP(msg, appDescriptor);

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

            XCEPT_RAISE(emuRUI::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emuRUI::exception::Exception, s, e);
    }

    return value;
}


void EmuRUI::getRunAndMaxEventNumber()
  // EMu-specific stuff
  // Gets the run number and maximum number of events from TA
throw (emuRUI::exception::Exception)
{
  xdata::UnsignedLong runNumber = 0;

  vector< xdaq::ApplicationDescriptor* > taDescriptors;

  try
    {
      taDescriptors = getAppDescriptors(appGroup_, "EmuTA");
    }
  catch(xcept::Exception e)
    {
      taDescriptors.clear();
      XCEPT_RETHROW(emuRUI::exception::Exception, 
		    "Failed to get application descriptors for class EmuTA",
		    e);
    }

  string rn="";
  string mn="";
  if      ( taDescriptors.size() == 1 ){
    rn = getScalarParam(taDescriptors[0],"runNumber","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got run number from emuTA: " + rn );
    mn = getScalarParam(taDescriptors[0],"maxNumTriggers","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got maximum number of events from emuTA: " + mn );
  }
  else if ( taDescriptors.size() > 1 ){
    LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		    taDescriptors.size() << " emuTA instances found. Trying first one.");
    rn = getScalarParam(taDescriptors[0],"runNumber","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got run number from emuTA: " + rn );
    mn = getScalarParam(taDescriptors[0],"maxNumTriggers","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got maximum number of events from emuTA: " + mn );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find EmuTA. ==> Run number and maximum number of events are unknown.");
  }

  unsigned int  irn(0);
  istringstream srn(rn);
  srn >> irn;
  runNumber_ = irn;

  unsigned int  imn(0);
  istringstream smn(mn);
  smn >> imn;
  maxEvents_ = imn;

  taDescriptors.clear();

}



string EmuRUI::createRuiRuPoolName(const unsigned long emuRUIInstance)
{
    stringstream oss;
    string       s;

    oss << "EmuRUI" << emuRUIInstance << "/" << "RU" << instance_ << "_Pool";
    s = oss.str();

    return s;
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
    vector< pair<string, xdata::Serializable*> > params;


    workLoopName_   = "";
    dataBufSize_    = 0x4000;//=16kB //4096;//=4kB
    fedPayloadSize_ = 2048;
    threshold_      = 0x4000000;//=64MB // 0x8000000;//=128MB //67108864; // 64 MB

    params.push_back(pair<string,xdata::Serializable *>
        ("workLoopName", &workLoopName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("dataBufSize", &dataBufSize_));
    params.push_back(pair<string,xdata::Serializable *>
        ("threshold", &threshold_));

    //
    // EMu-specific stuff
    //
    maxEvents_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("maxEvents", &maxEvents_));

    inputDataFormat_ = "DDU";
    nInputDevices_   = 0;
    inputDeviceType_ = "file";

    params.push_back(pair<string,xdata::Serializable *>
		     ("inputDataFormat", &inputDataFormat_));
    params.push_back(pair<string,xdata::Serializable *>
		     ("inputDeviceType", &inputDeviceType_));
    for( unsigned int iDev=0; iDev<maxDevices_; ++iDev ) inputDeviceNames_.push_back("");
    params.push_back(pair<string,xdata::Serializable *> 
		     ("inputDeviceNames", &inputDeviceNames_));


    pathToDataOutFile_   = "/tmp";
    pathToBadEventsFile_ = "/tmp";
    fileSizeInMegaBytes_ = 2;
    params.push_back(pair<string,xdata::Serializable *>
		     ("pathToRUIDataOutFile"  , &pathToDataOutFile_   ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("pathToBadEventsFile"   , &pathToBadEventsFile_ ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("ruiFileSizeInMegaBytes", &fileSizeInMegaBytes_ ));
    passDataOnToRUBuilder_ = true;
    params.push_back(pair<string,xdata::Serializable *>
		     ("passDataOnToRUBuilder", &passDataOnToRUBuilder_));


    for( unsigned int iClient=0; iClient<maxClients_; ++iClient ) {
      clientName_.push_back("");
      clientProtocol_.push_back("I2O");
      clientPoolSize_.push_back(0x100000); // 1MB
      prescaling_.push_back(0);
      onRequest_.push_back(true);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientsClassName", &clientName_));
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
    nEventsRead_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("nEventsRead", &nEventsRead_));

    for( unsigned int iClient=0; iClient<maxClients_; ++iClient ){ 
      creditsHeld_.push_back(0);
      clientPersists_.push_back(true);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("nEventCreditsHeld", &creditsHeld_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientPersists", &clientPersists_));

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

void EmuRUI::destroyDeviceReaders(){
  std::vector<EmuReader*>::iterator r;
  for ( r=deviceReaders_.begin(); r!=deviceReaders_.end(); ++r ){
    LOG4CPLUS_DEBUG(logger_, string("Destroying reader for ") + (*r)->getName() );
    delete *r;
  }
  deviceReaders_.clear();
}

void EmuRUI::createDeviceReaders(){

  // Count devices
  nInputDevices_ = 0;
  for( unsigned int iDev=0; iDev<maxDevices_; ++iDev )
    if ( inputDeviceNames_.at(iDev).toString() != "" ) nInputDevices_++;

  if ( nInputDevices_.value_ == (unsigned int) 0 ) {
    LOG4CPLUS_ERROR(logger_, "Number of input devices is zero?!");
  }

  // Create readers
  int inputDataFormatInt_ = -1;
  if      ( inputDataFormat_ == "DDU" ) inputDataFormatInt_ = EmuReader::DDU;
  else if ( inputDataFormat_ == "DCC" ) inputDataFormatInt_ = EmuReader::DCC;
  else     LOG4CPLUS_ERROR(logger_,"No such data format: " << inputDataFormat_.toString() << 
			   "Use \"DDU\" or \"DCC\"");
  for( unsigned int iDev=0; iDev<nInputDevices_; ++iDev ){
    LOG4CPLUS_INFO(logger_, "Creating " << inputDeviceType_.toString() << 
		   " reader for " << inputDeviceNames_.at(iDev).toString());
    deviceReaders_.push_back(NULL);
    try {
      if      ( inputDeviceType_ == "spy"  )
	deviceReaders_[iDev] = new EmuSpyReader(  inputDeviceNames_.at(iDev).toString(), inputDataFormatInt_ );
      else if ( inputDeviceType_ == "file" )
	deviceReaders_[iDev] = new EmuFileReader( inputDeviceNames_.at(iDev).toString(), inputDataFormatInt_ );
      // TODO: slink
      else     LOG4CPLUS_ERROR(logger_,"Bad device type: " << inputDeviceType_.toString() << 
			       "Use \"file\", \"spy\", or \"slink\"");
    }
    catch(char* e){

      stringstream oss;
      oss << "Failed to create " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceNames_.at(iDev).toString()
	  << ": "                << e;
      LOG4CPLUS_ERROR(logger_, oss.str())

	// Don't raise exception as it would be interpreted as FSM transition error
	//	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
	}
  }

  iCurrentDeviceReader_ = 0;
}

void EmuRUI::destroyServers(){
  std::vector<Client*>::iterator c;
  for ( c=clients_.begin(); c!=clients_.end(); ++c ){
    LOG4CPLUS_INFO(logger_, string("Destroying server for ") + (*c)->server->getClientName() );
    delete (*c)->server;
//     delete (*c)->workLoopActionSignature;
//     delete (*c)->workLoop;
  }
  clients_.clear();
}

bool EmuRUI::createI2OServer( string clientName ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*> ( clientName_.elementAt( iClient )     )) = clientName;
    *(dynamic_cast<xdata::String*> ( clientProtocol_.elementAt( iClient ) )) = "I2O";
    *(dynamic_cast<xdata::Boolean*>( clientPersists_.elementAt( iClient ) )) = true;
    EmuI2OServer* s = new EmuI2OServer( this,
					i2oExceptionHandler_,
					clientName_.elementAt(iClient)->toString(),
					clientPoolSize_.elementAt(iClient),
					prescaling_.elementAt(iClient),
					onRequest_.elementAt(iClient),
					creditsHeld_.elementAt(iClient),
					&logger_ );

    clients_.push_back( new Client( clientName_.elementAt(iClient),
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

bool EmuRUI::createSOAPServer( string clientName, bool persistent ){
  bool created = false;
  unsigned int iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*> (     clientName_.elementAt( iClient ) )) = clientName;
    *(dynamic_cast<xdata::String*> ( clientProtocol_.elementAt( iClient ) )) = "SOAP";
    *(dynamic_cast<xdata::Boolean*>( clientPersists_.elementAt( iClient ) )) = persistent;
    EmuSOAPServer* s = new EmuSOAPServer( this,
					  clientName_.elementAt(iClient)->toString(),
					  clientPersists_.elementAt(iClient),
					  prescaling_.elementAt(iClient),
					  onRequest_.elementAt(iClient),
					  creditsHeld_.elementAt(iClient),
					  &logger_ );
    
    clients_.push_back( new Client( clientName_.elementAt(iClient),
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


void EmuRUI::createServers(){
  for ( int iClient=0; iClient<clientName_.elements(); ++iClient ){
    xdata::Boolean *persists = dynamic_cast<xdata::Boolean*>( clientPersists_.elementAt(iClient) );
    // (Re)create it only if it has a name and is not a temporary server created on the fly
    if ( clientName_.elementAt(iClient)->toString() != "" && persists->value_ ){
      LOG4CPLUS_INFO(logger_,
		     clientName_.elementAt(iClient)->toString() + 
		     "\'s server being created" );
      if ( clientProtocol_.elementAt(iClient)->toString() == "I2O" )
	createI2OServer( clientName_.elementAt(iClient)->toString() );
      else if ( clientProtocol_.elementAt(iClient)->toString() == "SOAP" )
	createSOAPServer( clientName_.elementAt(iClient)->toString() );
      else
	LOG4CPLUS_ERROR(logger_, "Unknown protocol \"" <<
			clientProtocol_.elementAt(iClient)->toString() << 
			"\" for client " <<
			clientName_.elementAt(iClient)->toString() << 
			". Please use \"I2O\" or \"SOAP\".");
    }
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
    getRunAndMaxEventNumber();

    nEventsRead_ = 0;

    nDevicesWithBadData_ = 0;

    destroyDeviceReaders();
    createDeviceReaders();

    // Just in case there's a writer, terminate it in an orderly fashion
    if ( fileWriter_ )
      {
	LOG4CPLUS_INFO( logger_, "Terminating leftover file writer." );
	fileWriter_->endRun();
	delete fileWriter_;
	fileWriter_ = NULL;
      }
    if ( badEventsFileWriter_ )
      {
	LOG4CPLUS_INFO( logger_, "Terminating leftover bad event file writer." );
	badEventsFileWriter_->endRun();
	if ( badEventsFileWriter_->getFileSize() == 0 ) badEventsFileWriter_->removeFile();
	delete badEventsFileWriter_;
	badEventsFileWriter_ = NULL;
      }

    destroyServers();
    createServers();


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


    // server loops
    for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient ){
	
      if( ! clients_[iClient]->workLoopStarted )
	{
	  clients_[iClient]->workLoopActionSignature = toolbox::task::bind
	    (
	     this,
	     &EmuRUI::serverLoopAction,
	     "EmuRUI server loop action"
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

    // EMu specific
    if ( fileWriter_ ){
      fileWriter_->endRun();
      delete fileWriter_;
      fileWriter_ = NULL;
    }
    if ( badEventsFileWriter_ ){
      badEventsFileWriter_->endRun();
      if ( badEventsFileWriter_->getFileSize() == 0 ) badEventsFileWriter_->removeFile();
      delete badEventsFileWriter_;
      badEventsFileWriter_ = NULL;
    }
    for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
      (*c)->workLoopStarted = false;
    }

    workLoopStarted_  = false;
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
  i2o::bind(this, &EmuRUI::onI2OClientCreditMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );

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

bool EmuRUI::serverLoopAction(toolbox::task::WorkLoop *wl)
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
                "EmuRUI" << instance_ << " is in an undefined state");
        }

	applicationBSem_.give();

        // Reschedule this action code
        return isToBeRescheduled;
    }
    catch(xcept::Exception e)
    {
	applicationBSem_.give();

        LOG4CPLUS_FATAL(logger_,
            "Failed to execute \"self-driven\" behaviour"
            << " : " << xcept::stdformat_exception_history(e));

        // Do not reschedule this action code as the application has failed
        return false;
    }
}


void EmuRUI::addDataForClients( const int   runNumber, 
			        const int   nEventsRead,
			        const bool  completesEvent, 
			        char* const data, 
			        const int   dataLength ){
  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->addData( runNumber, nEventsRead, completesEvent, data, dataLength );
}


bool EmuRUI::processAndCommunicate()
{
    
  bool keepRunning = true;

    if( blocksArePendingTransmission_ )
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
      // Send data to clients from here if not from serverLoopAction() in another thread
      //       sendDataToClients();
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

void EmuRUI::createFileWriters(){
	  // terminate old writers, if any
	  if ( fileWriter_ )
	    {
	      fileWriter_->endRun();
	      delete fileWriter_;
	      fileWriter_ = NULL;
	    }
	  if ( badEventsFileWriter_ )
	    {
	      badEventsFileWriter_->endRun();
	      if ( badEventsFileWriter_->getFileSize() == 0 ) badEventsFileWriter_->removeFile();
	      delete badEventsFileWriter_;
	      badEventsFileWriter_ = NULL;
	    }
	  // create new writers if path is not empty
	  if ( pathToDataOutFile_ != string("") && fileSizeInMegaBytes_ > (long unsigned int) 0 )
	    {
	      stringstream ss;
	      ss << "EmuRUI" << instance_;
	      fileWriter_ = new FileWriter( 1000000*fileSizeInMegaBytes_, pathToDataOutFile_.toString(), ss.str(), &logger_ );
	    }
	  if ( fileWriter_ ) fileWriter_->startNewRun( runNumber_.value_ );
	  if ( pathToBadEventsFile_ != string("") && fileSizeInMegaBytes_ > (long unsigned int) 0 )
	    {
	      stringstream ss;
	      ss << "EmuRUI" << instance_ << "_BadEvents";
	      badEventsFileWriter_ = new FileWriter( 1000000*fileSizeInMegaBytes_, pathToBadEventsFile_.toString(), ss.str(), &logger_ );
	    }
	  if ( badEventsFileWriter_ ) badEventsFileWriter_->startNewRun( runNumber_.value_ );
}

bool EmuRUI::continueConstructionOfSuperFrag()
  throw (emuRUI::exception::Exception)
{

//   bool keepRunning = true;
  unsigned int nBytesRead = 0;

  if ( maxEvents_.value_ > 0 && nEventsRead_.value_ >= maxEvents_.value_ ) return false;

  if (deviceReaders_[iCurrentDeviceReader_]) 
    nBytesRead = deviceReaders_[iCurrentDeviceReader_]->readNextEvent();
  
  // No data ==> no business being here. Try to read again later.
  if ( nBytesRead == 0 ) return true;

  if ( nBytesRead < 8 ){
    LOG4CPLUS_ERROR(logger_, 
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    "[" << iCurrentDeviceReader_ << "] read " << nBytesRead << " bytes only.");
  }

  bool lastChunkOfEvent = ( iCurrentDeviceReader_ +1 == nInputDevices_ );
  if ( lastChunkOfEvent ) nEventsRead_++;

  if ( nEventsRead_ == (unsigned long) 1 ) // first event --> a new run
    {
      if ( iCurrentDeviceReader_ == 0 ) // don't do it for all devices...
	{
	  createFileWriters();
	} // if first input device 
    } // if first event 


  bool badData = false;

  // If the EmuRUI to RU memory pool has room for another data block
  if(!ruiRuPool_->isHighThresholdExceeded()){

    char* data;
    int   dataLength  = 0;

    if ( deviceReaders_[iCurrentDeviceReader_] ) {
      //     if ( keepRunning && deviceReaders_[iCurrentDeviceReader_] ) {
      //     if ( true ) { // let's see those too short events too !!!
      
      data       = deviceReaders_[iCurrentDeviceReader_]->data();

      if ( data!=NULL ){
	dataLength = deviceReaders_[iCurrentDeviceReader_]->dataLength();
	if ( dataLength>0 ) eventNumber_ = deviceReaders_[iCurrentDeviceReader_]->eventNumber();

	if ( inputDataFormatInt_ == EmuReader::DDU ){
	  int dataLengthWithoutPadding = getDDUDataLengthWithoutPadding(data,dataLength);
	  if ( dataLengthWithoutPadding >= 0 ){
	    dataLength = dataLengthWithoutPadding;
	    badData    = interestingDDUErrorBitPattern(data,dataLength);
	    if ( badData ) nDevicesWithBadData_++;
	  }
	}
      }

      if ( nEventsRead_ % 100 == 0 )
	LOG4CPLUS_DEBUG(logger_, 
			"Read event "    << eventNumber_                          << 
			" ("             << nEventsRead_                              <<
			" so far) from " << inputDeviceNames_.at(iCurrentDeviceReader_).toString() <<
			", size: "       << dataLength   
			);

    } //  if ( !ruiRuPool_->isHighThresholdExceeded() )
    
    // Write data to files
    if ( fileWriter_ )
      {
	if ( iCurrentDeviceReader_ == 0 ) // don't start a new event for each device...
	  fileWriter_->startNewEvent();
	fileWriter_->writeData( data, dataLength );
      }
    if ( badEventsFileWriter_ )
      {
	if ( nDevicesWithBadData_ == 1 ) // start a new event for the first faulty device
	  badEventsFileWriter_->startNewEvent();
	if ( badData ) badEventsFileWriter_->writeData( data, dataLength );
      }

    // fill block and append it to superfragment
    if ( passDataOnToRUBuilder_ )
      appendNewBlockToSuperFrag( data, dataLength );

    // Store this data to be sent to clients (if any)
//     bool lastChunkOfEvent = ( iCurrentDeviceReader_ +1 == nInputDevices_ );
    addDataForClients( runNumber_.value_, nEventsRead_.value_, lastChunkOfEvent, data, dataLength );


    if ( lastChunkOfEvent ){ // superfragment ready

      if ( passDataOnToRUBuilder_ ){
	// Prepare it for sending to the RU
	setNbBlocksInSuperFragment(superFragBlocks_.size());
	
	// Current super-fragment is now ready to be sent to the RU
	blocksArePendingTransmission_ = true;
      }

      nDevicesWithBadData_ = 0;

    }

    // Move on to the next device
    iCurrentDeviceReader_++;
    iCurrentDeviceReader_ %= nInputDevices_;

  }
  else  LOG4CPLUS_WARN(logger_, "EmuRUI-to-RU memory pool's high threshold exceeded.");
  
  return true;
//   return keepRunning;

}




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

int EmuRUI::getDDUDataLengthWithoutPadding(char* const data, const int dataLength){
  // Get the data length without the padding that may have been added by Gbit Ethernet

  const int minEthPacketSize   = 32; // short (2-byte) words --> 64 bytes
  const int DDUTrailerLength   = 12; // short (2-byte) words --> 24 bytes

  if ( !dataLength ) return 0;
  if ( dataLength%2 ) LOG4CPLUS_ERROR(logger_, "DDU data is odd number of bytes (" << dataLength << ") long" );
  if ( dataLength<DDUTrailerLength*2 ) LOG4CPLUS_ERROR(logger_, 
				      "DDU data is shorter (" << dataLength << " bytes) than trailer" );
  //   printData(data,dataLength);

  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
  // Let's go backward looking for trailer signatures:
  for ( int iShort=dataLength/2-DDUTrailerLength;
	iShort>=0 && iShort>=dataLength/2-(minEthPacketSize+DDUTrailerLength); 
	--iShort ){
    if ( (shortData[iShort+11] & 0xf000) == 0xa000 ) // Probably the trailer.
      // Double check:
      if ( shortData[iShort  ]             == 0x8000 &&
	   shortData[iShort+1]             == 0x8000 &&
	   shortData[iShort+2]             == 0xFFFF &&
	   shortData[iShort+3]             == 0x8000 &&
	   (shortData[iShort+4] & 0xfff0)  == 0x0000    ){
	// The following bit may be set in the production version,
	// so let's not rely on it being 0
	// (shortData[iShort+5] & 0x8000)  == 0x0000    ){
	return 2 * (iShort + DDUTrailerLength);
      }
  }

  stringstream ss;
  ss << "No DDU trailer found within " 
     << 2*minEthPacketSize
     << " bytes of the end of data in "
     << deviceReaders_[iCurrentDeviceReader_]->getName()
     << ". Event number: "
     << deviceReaders_[iCurrentDeviceReader_]->eventNumber();
  LOG4CPLUS_ERROR(logger_,ss.str());
  return -1; // no trailer found

}

int EmuRUI::getDCCDataLengthWithoutPadding(char* const data, const int dataLength){
  // Get the data length without the padding that may have been added by Gbit Ethernet

  const int minEthPacketSize   = 32; // short (2-byte) words --> 64 bytes
  const int DCCTrailerLength   =  8; // short (2-byte) words --> 16 bytes

  if ( !dataLength ) return 0;
  if ( dataLength%2 ) LOG4CPLUS_ERROR(logger_, "DCC data is odd number of bytes (" << dataLength << ") long" );
  if ( dataLength<DCCTrailerLength*2 ) LOG4CPLUS_ERROR(logger_, 
				      "DCC data is shorter (" << dataLength << " bytes) than trailer" );
  //   printData(data,dataLength);

  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
  // Let's go backward looking for trailer signatures:
  for ( int iShort=dataLength/2-DCCTrailerLength;
	iShort>=0 && iShort>=dataLength/2-(minEthPacketSize+DCCTrailerLength); 
	--iShort ){
    if ( (shortData[iShort+3] & 0xff00) == 0xef00 ) // Probably the trailer.
      // Double check:
      if ( (shortData[iShort+4] & 0x000f) == 0x0007 &&
	   (shortData[iShort+7] & 0xff00) == 0xaf00    )
	return 2 * (iShort + DCCTrailerLength);
  }

  stringstream ss;
  ss << "No DCC trailer found within " 
     << 2*minEthPacketSize
     << " bytes of the end of data in "
     << deviceReaders_[iCurrentDeviceReader_]->getName()
     << ". Event number: "
     << deviceReaders_[iCurrentDeviceReader_]->eventNumber();
  LOG4CPLUS_ERROR(logger_,ss.str());
  return -1; // no trailer found

}

bool EmuRUI::interestingDDUErrorBitPattern(char* const data, const int dataLength){
  // At this point dataLength should no longer contain Ethernet padding.

  // Check for interesting error bit patterns (defined by J. Gilmore):
  // 1) Critical Error = Sync Reset or Hard Reset required
  //        DDU Trail bits 5 OR 6   -OR-
  //        DDU Trailer-1 bit 47
  //    ----> Persistent, these bits stay set for all events until
  //         RESET occurs.
  // 2) Error Detected = bad event  (no reset)
  //        DDU Trailer-1 bit 46
  //    ----> Only set for the single event with a detected error.
  // 3) Warning = Buffer Near Full (no reset)
  //        DDU Trailer-1 bit 31  -OR-
  //        DDU Trail bit 4
  //    ----> Remains set until the condition abates.
  // 4) Special Warning (Rare & Interesting occurence, no reset)
  //        DDU Trailer-1 bit 45
  //    ----> Remains set until the condition abates; may be assosiated
  //         with 1) or 2) above.

  bool foundError = false;
  const unsigned long DDUTrailerLength = 24; // bytes
  unsigned short *trailerShortWord =
    reinterpret_cast<unsigned short*>( data + dataLength - DDUTrailerLength );
  
  // 1)
  if ( trailerShortWord[8] & 0x0060 ||     // DDU Trail bits 5 OR 6
       trailerShortWord[6] & 0x8000    ) { // DDU Trailer-1 bit 47
    LOG4CPLUS_ERROR(logger_, 
		    "Critical DDU error in "
		    << deviceReaders_[iCurrentDeviceReader_]->getName()
		    << ". Sync Reset or Hard Reset required. (bit T:5|T:6|T-1:47) Event "
		    << deviceReaders_[iCurrentDeviceReader_]->eventNumber()
		    << " (" << nEventsRead_ << " read)");

    foundError = true;
  }
  // 2)
  if ( trailerShortWord[6] & 0x4000 ) {    // DDU Trailer-1 bit 46
    LOG4CPLUS_ERROR(logger_,
		    "DDU error: bad event read from " 
		    << deviceReaders_[iCurrentDeviceReader_]->getName()
		    << ". (bit T-1:46) Event "
		    << deviceReaders_[iCurrentDeviceReader_]->eventNumber()
		    << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 3)
  if ( trailerShortWord[8] & 0x0001 ||      // DDU Trail bit 4
       trailerShortWord[5] & 0x8000    ) {  // DDU Trailer-1 bit 31
    LOG4CPLUS_WARN(logger_,
		   "DDU buffer near Full in "
		   << deviceReaders_[iCurrentDeviceReader_]->getName() 
		   << ". (bit T:4|T-1:31) Event "
		   << deviceReaders_[iCurrentDeviceReader_]->eventNumber()
		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 4)
  if ( trailerShortWord[6] & 0x2000 ) {      // DDU Trailer-1 bit 45
    LOG4CPLUS_WARN(logger_,
		   "DDU special warning in "
		   << deviceReaders_[iCurrentDeviceReader_]->getName() 
		   << ". (bit T-1:45) Event "
		   << deviceReaders_[iCurrentDeviceReader_]->eventNumber()
		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }

  return foundError;
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


void EmuRUI::printBlocks( deque<toolbox::mem::Reference*> d ){
  deque<toolbox::mem::Reference*>::iterator pos;
  toolbox::mem::Reference *bufRef = 0;
  cout << "--------- " << nEventsRead_ <<  " events" << endl;
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
  
    /////////////////////
    // Fill FED data   //
    /////////////////////

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

    // moved to EmuRUI::setNbBlocksInSuperFragment:    block->eventNumber     = nEventsRead_ % 0x1000000; // 2^24
    block->blockNb         = iCurrentDeviceReader_; // each block carries a whole device's data

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
	 // If we read more than one device, nEventsRead_ is implemented only when
	 // all devices have been read out. This method is invoked after that, so
	 // it is now that we set the run number in all blocks. 
	 // (Event numbering starts from 1.)
	 block->eventNumber = nEventsRead_ % 0x1000000; // 2^24
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
