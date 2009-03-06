#include "emu/daq/rui/Application.h"
#include "emu/daq/rui/version.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "i2o/Method.h"
#include "interface/evb/i2oEVBMsgs.h"
#include "interface/shared/i2oXFunctionCodes.h"
#include "toolbox/utils.h"
#include "toolbox/fsm/FailedEvent.h"
#include "toolbox/mem/HeapAllocator.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
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

#include <unistd.h>
// EMu-specific stuff
#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/net/URL.h"
#include "emu/daq/reader/RawDataFile.h"
#include "emu/daq/reader/Spy.h"
#include "emu/daq/server/I2O.h"
#include "emu/daq/server/SOAP.h"
#include <sstream>
#include "xdata/soap/Serializer.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"


emu::daq::rui::Application::Application(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

logger_(Logger::getInstance(generateLoggerName())),

applicationBSem_(toolbox::BSem::FULL)

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
    zone_            = appContext_->getDefaultZone();
    xmlClass_        = appDescriptor_->getClassName();
    instance_        = appDescriptor_->getInstance();
    urn_             = appDescriptor_->getURN();

    appDescriptor_->setAttribute("icon",
        "/emu/daq/rui/images/EmuRUI64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester( zone_ );

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &emu::daq::rui::Application::onI2oException, "onI2oException");

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
    xgi::bind(this, &emu::daq::rui::Application::css           , "styles.css");
    xgi::bind(this, &emu::daq::rui::Application::defaultWebPage, "Default"   );

    //
    // EMu-specific
    //
    deviceReader_          = NULL;
    fileWriter_            = NULL;
    rateLimiter_           = NULL;
    badEventsFileWriter_   = NULL;
    nReadingPassesInEvent_ = 0;
    insideEvent_           = false;
    errorFlag_             = 0;
    previousEventNumber_   = 0;
    runStartUTC_           = 0;

    // bind SOAP client credit message callback
    xoap::bind(this, &emu::daq::rui::Application::onSOAPClientCreditMsg, 
	       "onClientCreditMessage", XDAQ_NS_URI);

    // bind STEP-related SOAP messages
    xoap::bind(this, &emu::daq::rui::Application::onSTEPQuery       ,"STEPQuery"       , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onExcludeDDUInputs,"excludeDDUInputs", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onIncludeDDUInputs,"includeDDUInputs", XDAQ_NS_URI);

    // Memory pool for i2o messages to emu::daq::ta::Application
    stringstream ruiTaPoolName;
    ruiTaPoolName << "EmuRUI" << instance_ << "-to-EmuTA";
    ruiTaPool_ = createHeapAllocatorMemoryPool(poolFactory_, ruiTaPoolName.str());

//     // DEBUG START
//     visitCount_rwl = 0;
//     visitCount_swl = 0;
//     ec_rwl = new EmuClock(100);
//     ec_swl = new EmuClock(100);
//     // DEBUG END

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::rui::Application::generateLoggerName()
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

string emu::daq::rui::Application::extractParametersFromSOAPClientCreditMsg
(
    xoap::MessageReference msg, unsigned int& instance, int& credits, int& prescaling 
)
throw (emu::daq::rui::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Parameter(s) not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::rui::exception::Exception,
            "Parameter(s) not found");
    }
}

xoap::MessageReference emu::daq::rui::Application::processSOAPClientCreditMsg( xoap::MessageReference msg )
  throw( emu::daq::rui::exception::Exception )
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


xoap::MessageReference emu::daq::rui::Application::onSOAPClientCreditMsg( xoap::MessageReference msg )
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
	  catch( emu::daq::rui::exception::Exception e )
	    {
	      XCEPT_RETHROW(xoap::exception::Exception, string("Failed to process SOAP client credit message"), e);
	    }
	  break;
        case 'R': // Ready
        case 'S': // Suspended
	  break;
        default:
	  LOG4CPLUS_ERROR(logger_,
			  "emu::daq::rui::Application in undefined state");
        }
    }
  catch(xcept::Exception e)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process SOAP client credit message : "
		      << stdformat_exception_history(e));
    }
  catch(...)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process SOAP client credit message : Unknown exception");
    }
  
  
  if ( reply.isNull() ) reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
  envelope.getBody().addBodyElement ( responseName );

  applicationBSem_.give();

  return reply;
}

void emu::daq::rui::Application::onI2OClientCreditMsg(toolbox::mem::Reference *bufRef)
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
			  "emu::daq::rui::Application in undefined state");
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process I2O client credit message : "
             << stdformat_exception_history(e));
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process I2O client credit message : Unknown exception");
    }

    // Free the client's event credit message
    bufRef->release();

    applicationBSem_.give();

}


xdaq::ApplicationDescriptor *emu::daq::rui::Application::getRUBuilderTester
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

// vector< xdaq::ApplicationDescriptor* > emu::daq::rui::Application::getAppDescriptors(xdaq::Zone *zone,
// 										     const string            appClass)
//   throw (emu::daq::rui::exception::Exception)
// {
//     vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
//     set< xdaq::ApplicationDescriptor* > descriptors;
//     xdaq::ApplicationDescriptor *descriptor = 0;
//     int nbApps = 0;


//     try
//     {
//         descriptors = zone->getApplicationDescriptors(appClass);
//     }
//     catch(xdaq::exception::ApplicationDescriptorNotFound e)
//     {
//         string s;

//         s = "Failed to get application descriptors for class: " + appClass;

//         XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
//     }

//     nbApps = descriptors.size();

//     // Fill application descriptors in instance order
//     for(int i=0; i<nbApps; i++)
//     {
//         try
//         {
//             descriptor = zone->getApplicationDescriptor(appClass, i);
//         }
// 	catch(xdaq::exception::ApplicationDescriptorNotFound e)
//         {
//             stringstream oss;
//             string s;

//             oss << "Failed to get the application descriptor of ";
//             oss << appClass << i;
//             s = oss.str();

//             XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
//         }

//         orderedDescriptors.push_back(descriptor);
//     }

//     return orderedDescriptors;
// }

vector< xdaq::ApplicationDescriptor* > emu::daq::rui::Application::getAppDescriptors
(
    xdaq::Zone             *zone,
    const string           appClass
)
throw (emu::daq::rui::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
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

xoap::MessageReference emu::daq::rui::Application::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emu::daq::rui::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}

DOMNode *emu::daq::rui::Application::findNode(DOMNodeList *nodeList,
			  const string nodeLocalName)
  throw (emu::daq::rui::exception::Exception)
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

    XCEPT_RAISE(emu::daq::rui::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


string emu::daq::rui::Application::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emu::daq::rui::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::rui::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

string emu::daq::rui::Application::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emu::daq::rui::exception::Exception)
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

            XCEPT_RAISE(emu::daq::rui::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }

    return value;
}

time_t emu::daq::rui::Application::toUnixTime( const std::string YYMMDD_hhmmss_UTC ){
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


void emu::daq::rui::Application::getRunInfo()
  // EMu-specific stuff
  // Gets the run number and maximum number of events from TA
throw (emu::daq::rui::exception::Exception)
{
  runNumber_    = 0;
  isBookedRunNumber_ = false;
  runStartTime_ = "YYMMDD_hhmmss_UTC";
  runStartUTC_  = 0;
  maxEvents_    = 0;

  vector< xdaq::ApplicationDescriptor* > taDescriptors;

  try
    {
      taDescriptors = getAppDescriptors(zone_, "emu::daq::ta::Application");
    }
  catch(emu::daq::rui::exception::Exception e)
    {
      taDescriptors.clear();
      XCEPT_RETHROW(emu::daq::rui::exception::Exception, 
		    "Failed to get application descriptors for class emu::daq::ta::Application",
		    e);
    }

  string rn="";
  string mn="";
  string br="";
  if ( taDescriptors.size() >= 1 ){
    if ( taDescriptors.size() > 1 )
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      taDescriptors.size() << " emuTA instances found. Trying first one.");
    rn = getScalarParam(taDescriptors[0],"runNumber","unsignedLong");
    LOG4CPLUS_INFO(logger_, "Got run number from emuTA: " + rn );
    mn = getScalarParam(taDescriptors[0],"maxNumTriggers","integer");
    LOG4CPLUS_INFO(logger_, "Got maximum number of events from emuTA: " + mn );
    br = getScalarParam(taDescriptors[0],"isBookedRunNumber","boolean");
    LOG4CPLUS_INFO(logger_, "Got info on run booking from emuTA: " + br );
    runStartTime_ = getScalarParam(taDescriptors[0],"runStartTime","string");
    runStartUTC_ = toUnixTime( runStartTime_ );
    LOG4CPLUS_INFO(logger_, "Got run start time from emuTA: " + runStartTime_.toString()  + " or " << runStartUTC_ );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find emu::daq::ta::Application. ==> Run number, start time, and maximum number of events are unknown.");
  }

  unsigned int  irn(0);
  istringstream srn(rn);
  srn >> irn;
  runNumber_ = irn;

  isBookedRunNumber_ = ( br == "true" );

//   unsigned int  imn(0);
  long  imn(0);
  istringstream smn(mn);
  smn >> imn;
  maxEvents_ = imn;

  taDescriptors.clear();

}

string emu::daq::rui::Application::createRuiRuPoolName(const unsigned long emuRUIInstance)
{
    stringstream oss;
    string       s;

    oss << "EmuRUI" << emuRUIInstance << "/" << "RU" << instance_ << "_Pool";
    s = oss.str();

    return s;
}



toolbox::mem::Pool *emu::daq::rui::Application::createHeapAllocatorMemoryPool
(
    toolbox::mem::MemoryPoolFactory *poolFactory,
    const string                     poolName
)
throw (emu::daq::rui::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }
    catch(...)
    {
        string s = "Failed to create pool: " + poolName +
                   " : Unknown exception";

        XCEPT_RAISE(emu::daq::rui::exception::Exception, s);
    }
}



void emu::daq::rui::Application::defineFsm()
throw (emu::daq::rui::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &emu::daq::rui::Application::stateChanged);
        fsm_.addState('R', "Ready"    , this, &emu::daq::rui::Application::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &emu::daq::rui::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &emu::daq::rui::Application::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &emu::daq::rui::Application::enableAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &emu::daq::rui::Application::haltAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &emu::daq::rui::Application::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &emu::daq::rui::Application::haltAction);

        fsm_.addStateTransition('H', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
        fsm_.addStateTransition('R', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
        fsm_.addStateTransition('E', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &emu::daq::rui::Application::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &emu::daq::rui::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to reset FSM", e);
    }
}


string emu::daq::rui::Application::generateMonitoringInfoSpaceName
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


vector< pair<string, xdata::Serializable*> > emu::daq::rui::Application::initAndGetStdConfigParams()
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
    inputDeviceType_ = "file";

    params.push_back(pair<string,xdata::Serializable *>
		     ("inputDataFormat", &inputDataFormat_));
    params.push_back(pair<string,xdata::Serializable *>
		     ("inputDeviceType", &inputDeviceType_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("inputDeviceName", &inputDeviceName_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("hardwareMnemonic", &hardwareMnemonic_));


    pathToDataOutFile_   = "";
    pathToBadEventsFile_ = "";
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
    runType_ = "";
    params.push_back(pair<string,xdata::Serializable *>
		     ("runType", &runType_));
    
    fileWritingRateLimitInHz_  =  2000;
    fileWritingRateSampleSize_ = 10000;
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingRateLimitInHz", &fileWritingRateLimitInHz_ ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingRateSampleSize", &fileWritingRateSampleSize_ ));

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


vector< pair<string, xdata::Serializable*> > emu::daq::rui::Application::initAndGetStdMonitorParams()
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
    persistentDDUError_ = "";
    params.push_back(pair<string,xdata::Serializable *>
		     ("persistentDDUError", &persistentDDUError_));

    fileWritingVetoed_ = false;
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingVetoed", &fileWritingVetoed_ ));

    for( unsigned int iClient=0; iClient<maxClients_; ++iClient ){ 
      creditsHeld_.push_back(0);
      clientPersists_.push_back(true);
    }
    params.push_back(pair<string,xdata::Serializable *> 
		     ("nEventCreditsHeld", &creditsHeld_));
    params.push_back(pair<string,xdata::Serializable *> 
		     ("clientPersists", &clientPersists_));

    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
    params.push_back(pair<string,xdata::Serializable *>
        ("runStartTime", &runStartTime_));
    params.push_back(pair<string,xdata::Serializable *>
        ("runStopTime", &runStopTime_));


    return params;
}


void emu::daq::rui::Application::putParamsIntoInfoSpace
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


void emu::daq::rui::Application::stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
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


void emu::daq::rui::Application::bindFsmSoapCallbacks()
{
    xoap::bind(this, &emu::daq::rui::Application::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);

    xoap::bind(this, &emu::daq::rui::Application::onReset,           "Reset"    , XDAQ_NS_URI);
}


xoap::MessageReference emu::daq::rui::Application::processSoapFsmCmd(xoap::MessageReference msg)
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


string emu::daq::rui::Application::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emu::daq::rui::exception::Exception)
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

    XCEPT_RAISE(emu::daq::rui::exception::Exception, "Command name not found");
}


xoap::MessageReference emu::daq::rui::Application::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emu::daq::rui::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::rui::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


void emu::daq::rui::Application::processFsmCommand(const string cmdName)
throw (emu::daq::rui::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        applicationBSem_.give();

        XCEPT_RAISE(emu::daq::rui::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}

void emu::daq::rui::Application::destroyDeviceReader(){
  if ( deviceReader_ != NULL )
    LOG4CPLUS_DEBUG(logger_, string("Destroying reader for ") + deviceReader_->getName() );
  delete deviceReader_;
  deviceReader_ = NULL;
}

// void emu::daq::rui::Application::destroyDeviceReaders(){
//   std::vector<EmuReader*>::iterator r;
//   for ( r=deviceReaders_.begin(); r!=deviceReaders_.end(); ++r ){
//     LOG4CPLUS_DEBUG(logger_, string("Destroying reader for ") + (*r)->getName() );
//     delete *r;
//   }
//   deviceReaders_.clear();
// }

void emu::daq::rui::Application::createDeviceReader(){
  // Version with single device

  // Create readers
  inputDataFormatInt_ = -1;
  if      ( inputDataFormat_ == "DDU" ) inputDataFormatInt_ = emu::daq::reader::Base::DDU;
  else if ( inputDataFormat_ == "DCC" ) inputDataFormatInt_ = emu::daq::reader::Base::DCC;
  else{
	stringstream oss;
	oss << "No such data format: " << inputDataFormat_.toString() << 
	  "Use \"DDU\" or \"DCC\"";
// 	LOG4CPLUS_FATAL(logger_, oss.str());
	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
  }

    LOG4CPLUS_INFO(logger_, "Creating " << inputDeviceType_.toString() << 
		   " reader for " << inputDeviceName_.toString());
    deviceReader_ = NULL;
    if ( inputDeviceType_ != "spy" && inputDeviceType_ != "file" ){
      stringstream oss;
      oss << "Bad device type: " << inputDeviceType_.toString() << 
	"Use \"file\", \"spy\", or \"slink\"";
//       LOG4CPLUS_FATAL(logger_, oss.str());
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    try {
      if      ( inputDeviceType_ == "spy"  )
	deviceReader_ = new emu::daq::reader::Spy(  inputDeviceName_.toString(), inputDataFormatInt_ );
      else if ( inputDeviceType_ == "file" )
	deviceReader_ = new emu::daq::reader::RawDataFile( inputDeviceName_.toString(), inputDataFormatInt_ );
      // TODO: slink
    }
    catch(std::runtime_error e){

      stringstream oss;
      oss << "Failed to create " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": "                << e.what();
//       LOG4CPLUS_FATAL(logger_, oss.str());
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    catch(...){
      stringstream oss;
      oss << "Failed to create " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
//       LOG4CPLUS_FATAL(logger_, oss.str());
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_)
      if ( deviceReader_->getLogMessage().length() > 0 )
	LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());

}

// void emu::daq::rui::Application::createDeviceReaders(){
//   // Version with multiple devices

//   // Count devices
//   nInputDevices_ = 0;
//   for( unsigned int iDev=0; iDev<maxDevices_; ++iDev )
//     if ( inputDeviceNames_.at(iDev).toString() != "" ) nInputDevices_++;

//   if ( nInputDevices_.value_ == (unsigned int) 0 ) {
// 	stringstream oss;
// 	oss << "Number of input devices is zero?!";
// 	LOG4CPLUS_FATAL(logger_, oss.str());
// 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
//   }

//   // Create readers
//   int inputDataFormatInt_ = -1;
//   if      ( inputDataFormat_ == "DDU" ) inputDataFormatInt_ = emu::daq::reader::Base::DDU;
//   else if ( inputDataFormat_ == "DCC" ) inputDataFormatInt_ = emu::daq::reader::Base::DCC;
//   else{
// 	stringstream oss;
// 	oss << "No such data format: " << inputDataFormat_.toString() << 
// 	  "Use \"DDU\" or \"DCC\"";
// 	LOG4CPLUS_FATAL(logger_, oss.str());
// 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
//   }

//   for( unsigned int iDev=0; iDev<nInputDevices_; ++iDev ){
//     LOG4CPLUS_INFO(logger_, "Creating " << inputDeviceType_.toString() << 
// 		   " reader for " << inputDeviceNames_.at(iDev).toString());
//     deviceReaders_.push_back(NULL);
//     if ( inputDeviceType_ != "spy" && inputDeviceType_ != "file" ){
//       stringstream oss;
//       oss << "Bad device type: " << inputDeviceType_.toString() << 
// 	"Use \"file\", \"spy\", or \"slink\"";
//       LOG4CPLUS_FATAL(logger_, oss.str());
//       XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
//     }
//     try {
//       if      ( inputDeviceType_ == "spy"  )
// 	deviceReaders_[iDev] = new emu::daq::reader::Spy(  inputDeviceNames_.at(iDev).toString(), inputDataFormatInt_ );
//       else if ( inputDeviceType_ == "file" )
// 	deviceReaders_[iDev] = new emu::daq::reader::RawDataFile( inputDeviceNames_.at(iDev).toString(), inputDataFormatInt_ );
//       // TODO: slink
//     }
//     catch(std::runtime_error e){

//       stringstream oss;
//       oss << "Failed to create " << inputDeviceType_.toString()
// 	  << " reader for "      << inputDeviceNames_.at(iDev).toString()
// 	  << ": "                << e.what();
//       LOG4CPLUS_FATAL(logger_, oss.str());
	
//       XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
//     }
//     catch(...){
//       stringstream oss;
//       oss << "Failed to create " << inputDeviceType_.toString()
// 	  << " reader for "      << inputDeviceNames_.at(iDev).toString()
// 	  << ": unknown exception.";
//       LOG4CPLUS_FATAL(logger_, oss.str());
//       XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
//     }

//     if ( deviceReaders_[iCurrentDeviceReader_]->getLogMessage().length() > 0 )
//       LOG4CPLUS_INFO(logger_, deviceReaders_[iCurrentDeviceReader_]->getLogMessage());

//   }

//   iCurrentDeviceReader_ = 0;
// }

void emu::daq::rui::Application::destroyServers(){
  std::vector<Client*>::iterator c;
  for ( c=clients_.begin(); c!=clients_.end(); ++c ){
    LOG4CPLUS_INFO(logger_, string("Destroying server for ") + (*c)->server->getClientName() );
    delete (*c)->server;
    *(*c)->creditsHeld = 0;
  }
  clients_.clear();
}

bool emu::daq::rui::Application::createI2OServer( string clientName, unsigned int clientInstance  ){
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

bool emu::daq::rui::Application::createSOAPServer( string clientName,  unsigned int clientInstance, bool persistent ){
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
  return created;
}


void emu::daq::rui::Application::createServers(){
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

xoap::MessageReference 
emu::daq::rui::Application::createSimpleSOAPCmdMsg( const string cmdName )
  throw (emu::daq::rui::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}

void 
emu::daq::rui::Application::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emu::daq::rui::exception::Exception)
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

            XCEPT_RAISE(emu::daq::rui::exception::Exception, s);
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }
}

void 
emu::daq::rui::Application::startATCP()
  throw (emu::daq::rui::exception::Exception){
  // configure and enable all pt::atcp::PeerTransportATCP

  std::cout << "In emu::daq::rui::Application::startATCP()" << std::endl;

  vector < xdaq::ApplicationDescriptor* > atcpDescriptors;
  try{
      atcpDescriptors = getAppDescriptors(zone_, "pt::atcp::PeerTransportATCP");
  }
  catch(emu::daq::rui::exception::Exception e){
    LOG4CPLUS_WARN(logger_, "Failed to get atcp descriptors : " 
		   + xcept::stdformat_exception_history(e) );
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
      catch(emu::daq::rui::exception::Exception e)
	{
	  stringstream oss;
	  oss << "Failed to get state of " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	  XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
	}

      if ( atcpState != "Halted" ) continue;

      // Configure ATCP
      try{
	sendFSMEventToApp("Configure", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
      }

      // Enable ATCP
      try{
	sendFSMEventToApp("Enable", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
      }

    }

  }
  
}


void emu::daq::rui::Application::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
  // ATCP must be started explicitly
  try{
    startATCP();
  }
  catch(xcept::Exception e){
    XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to start ATCP ", e);
  }

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
        ruDescriptor_ = zone_->getApplicationDescriptor("rubuilder::ru::Application", instance_);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound  e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
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

        XCEPT_RETHROW(toolbox::fsm::exception::Exception, s ,e);
    }

    ruiRuPool_->setHighThreshold(threshold_ - dataBufSize_);

    LOG4CPLUS_INFO(logger_,
                      "Set threshold of event data pool (dataBufSize="
                   << dataBufSize_ << ") to "
                   << threshold_);

    try{
      getTidOfEmuTA();
    } catch( xcept::Exception e ) {
      LOG4CPLUS_WARN(logger_,"Failed to get i2o target id of emu::daq::ta::Application: " +  xcept::stdformat_exception_history(e));
    }

    nReadingPassesInEvent_ = 0;
    insideEvent_           = false;
    errorFlag_             = 0;

    nEventsRead_ = 0;

    persistentDDUError_ = "";

    destroyDeviceReader();
    createDeviceReader();

    workLoopStarted_ = false; // make sure work loop action will be (re)submitted in case it's no longer rescheduled

    destroyServers();
    createServers();

    runStartUTC_  = 0;
    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";

    // Find out if this is going to be a STEP run
    isSTEPRun_ = ( runType_.toString().find("STEP",0) != string::npos );
    if ( isSTEPRun_ && inputDataFormatInt_ == emu::daq::reader::Base::DCC ){
      XCEPT_RAISE(toolbox::fsm::exception::Exception,"STEP runs can only be taken with DDU. Running STEP with DCC is not supported.");
    }
    if ( isSTEPRun_ ) STEPEventCounter_.reset();

}


void emu::daq::rui::Application::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

    try{
      getRunInfo();
    }
    catch(emu::daq::rui::exception::Exception e){
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to get run number and max events from emu::daq::ta::Application", e);
    }

    if(!workLoopStarted_)
    {
        workLoopActionSignature_ = toolbox::task::bind
        (
            this,
            &emu::daq::rui::Application::workLoopAction,
            "emu::daq::rui::Application work loop action"
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
      // Start separate server loops for SOAP servers only as SOAP messaging is synchronous
      // and therefore blocking. (I2O messages don't block, they just "fire & forget".)
      if ( clientProtocol_.elementAt( iClient )->toString() == "SOAP" )
	{

	  if( ! clients_[iClient]->workLoopStarted )
	    {
	      clients_[iClient]->workLoopActionSignature = toolbox::task::bind
		(
		 this,
		 &emu::daq::rui::Application::serverLoopAction,
		 "emu::daq::rui::Application server loop action"
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
		    workLoopFactory_->getWorkLoop(clients_[iClient]->workLoopName, "polling");
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
	    } // if( ! clients_[iClient]->workLoopStarted )
	} // if ( clientProtocol_.elementAt( iClient )->toString() == "SOAP" )
    } // for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )

}

void emu::daq::rui::Application::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

    deque<toolbox::mem::Reference*>::iterator pos;
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
    // Reset previous event number
    previousEventNumber_ = 0;

    // Get time of end of run
    try{
      runStopTime_ = getScalarParam(taDescriptors_[0],"runStopTime","string");
      LOG4CPLUS_INFO(logger_, "Got run stop time from emu::daq::ta::Application: " << runStopTime_.toString() );
    }
    catch( emu::daq::rui::exception::Exception e ){
      LOG4CPLUS_WARN(logger_, "Run stop time will be unknown: " << xcept::stdformat_exception_history(e) );
    }

    // Close data file
    if ( fileWriter_ ){
      fileWriter_->endRun( runStopTime_.toString() );
      delete fileWriter_;
      fileWriter_ = NULL;
    }
    if ( badEventsFileWriter_ ){
      badEventsFileWriter_->endRun( runStopTime_.toString() );
      if ( badEventsFileWriter_->getFileSize() == 0 ) badEventsFileWriter_->removeFile();
      delete badEventsFileWriter_;
      badEventsFileWriter_ = NULL;
    }
    // Destroy rate limiter, too.
    if ( rateLimiter_ ){
      delete rateLimiter_;
      rateLimiter_ = NULL;
    }

}


void emu::daq::rui::Application::failAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
    try
    {
        toolbox::fsm::FailedEvent &failedEvent =
            dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
        xcept::Exception exception = failedEvent.getException();

        stringstream oss;
        string       s;

        oss << "Failure occurred when performing transition from ";
        oss << failedEvent.getFromState();
        oss <<  " to ";
        oss << failedEvent.getToState();
        oss << "; Exception history: ";
        oss << xcept::stdformat_exception_history(exception);
        s = oss.str();

        LOG4CPLUS_FATAL(logger_, s);
    }
    catch(bad_cast)
    {
        LOG4CPLUS_FATAL(logger_, "Caught bad_cast exception while moving to Failed state." );
    }
}

void emu::daq::rui::Application::bindI2oCallbacks()
{

  i2o::bind(this, &emu::daq::rui::Application::onI2OClientCreditMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );

}

void emu::daq::rui::Application::moveToFailedState(){
  // Use this from inside the work loop to force the FSM to Failed state 

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
}

void emu::daq::rui::Application::css
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


void emu::daq::rui::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << emudaqrui::versions    << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/daq/rui/images/EmuRUI64x64.gif\""   << endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << emudaqrui::versions
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


void emu::daq::rui::Application::printParamsTable
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

bool emu::daq::rui::Application::workLoopAction(toolbox::task::WorkLoop *wl)
{
    try
    {
      bool isToBeRescheduled    = true;
      int  pauseForOtherThreads = 0;
        applicationBSem_.take();

        toolbox::fsm::State state = fsm_.getCurrentState();

        switch(state)
        {
        case 'H':  // Halted
        case 'F':  // Failed
        case 'R':  // Ready
	  // Pause or else this thread will hog the CPU like an idle loop.
	  pauseForOtherThreads = 5000;
	  break;
        case 'E':  // Enabled
// 	  // DEBUG START
// 	  visitCount_rwl++;
// 	  if ( ec_rwl->timeIsUp() ){
// 	    std::cout << "  " << workLoopName_.toString()
// 		      << "   readout loop: " << visitCount_rwl
// 		      << "   server loop: " << visitCount_swl
// 		      << std::endl << std::flush;
// 	    LOG4CPLUS_INFO(logger_,
// 			   "  " << workLoopName_.toString()
// 			   << "   readout loop: " << visitCount_rwl
// 			   << "   server loop: " << visitCount_swl
// 			   );
// 	  }
// 	  // DEBUG END
	  pauseForOtherThreads = processAndCommunicate();
	  isToBeRescheduled    = ( pauseForOtherThreads >= 0 );

	  // Run the servers too in the readout thread.
	  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient ){
	    // Service only I2O clients here in the readout loop as I2O messages
	    // are non-blocking (fire & forget).
	    if ( clientProtocol_.elementAt( iClient )->toString() == "I2O" ){
	      clients_[iClient]->server->sendData();
	    }
	  }

	  break;
        default:
	  // Should never get here
	  LOG4CPLUS_FATAL(logger_,
			  "emu::daq::rui::Application" << instance_ << " is in an undefined state");
        }

        applicationBSem_.give();

	if ( pauseForOtherThreads > 0 ) usleep( (unsigned int) pauseForOtherThreads );

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

bool emu::daq::rui::Application::serverLoopAction(toolbox::task::WorkLoop *wl)
{
    try
    {
      int  pauseForOtherThreads = 0;

      applicationBSem_.take();

        toolbox::fsm::State state = fsm_.getCurrentState();

        switch(state)
        {
        case 'H':  // Halted
        case 'F':  // Failed
        case 'R':  // Ready
	  // Pause or else this thread will hog the CPU like an idle loop.
	  pauseForOtherThreads = 5000;
	  break;
        case 'E':  // Enabled
	  // Find out from which work loop we dropped in here
	  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient ){
	    if ( clients_[iClient]->workLoop == wl ){
// 	      // DEBUG START
// 	      visitCount_swl++;
// 	      if ( ec_swl->timeIsUp() ){
// 		std::cout << "  " << clients_[iClient]->workLoopName 
// 			  << "   readout loop: " << visitCount_rwl
// 			  << "   server loop: " << visitCount_swl
// 			  << std::endl << std::flush;
// 		LOG4CPLUS_INFO(logger_,
// 			       "  " << clients_[iClient]->workLoopName 
// 			       << "   readout loop: " << visitCount_rwl
// 			       << "   server loop: " << visitCount_swl
// 			       );
// 	      }
// 	      // DEBUG END
// 	      LOG4CPLUS_INFO(logger_, "Sending data from " << clients_[iClient]->workLoopName << " ("<< wl << ")");
	      clients_[iClient]->server->sendData();
	    break;
	    }
	  }
	  break;
        default:
            // Should never get here
            LOG4CPLUS_ERROR(logger_,
                "emu::daq::rui::Application" << instance_ << " is in an undefined state");
        }

	applicationBSem_.give();

	if ( pauseForOtherThreads > 0 ) usleep( (unsigned int) pauseForOtherThreads );

    }
    catch(xcept::Exception e)
    {
	applicationBSem_.give();

        LOG4CPLUS_WARN(logger_,
            "Failed to execute \"self-driven\" behaviour"
            << " : " << xcept::stdformat_exception_history(e));
    }

    // Reschedule this action code
    return true;
}


void emu::daq::rui::Application::addDataForClients( const int   runNumber, 
				const int   runStartUTC,
			        const int   nEventsRead,
			        const bool  completesEvent, 
				const unsigned short errorFlag, 
			        char* const data, 
			        const int   dataLength ){
  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->addData( runNumber, runStartUTC, nEventsRead, completesEvent, 
					errorFlag, data, dataLength );
}

void emu::daq::rui::Application::makeClientsLastBlockCompleteEvent(){
  for ( unsigned int iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->makeLastBlockCompleteEvent();
}


int emu::daq::rui::Application::processAndCommunicate()
{
  int pauseForOtherThreads = 0;

    if( blocksArePendingTransmission_ )
    {
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
    }
    else
    {
        try
        {
	  if ( isSTEPRun_ ) pauseForOtherThreads = continueSTEPRun();
	  else              pauseForOtherThreads = continueConstructionOfSuperFrag();
        }
        catch(xcept::Exception e)
        {
            LOG4CPLUS_ERROR(logger_,
                "Failed to contnue construction of super-fragment"
                << " : " << stdformat_exception_history(e));
        }
    }

  return pauseForOtherThreads;
}



void emu::daq::rui::Application::sendNextPendingBlock()
throw (emu::daq::rui::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }

    superFragBlocks_.erase(superFragBlocks_.begin());

    blocksArePendingTransmission_ = (superFragBlocks_.size() > 0);

}

void emu::daq::rui::Application::createFileWriters(){
    // Just in case there's a writer, terminate it in an orderly fashion
    if ( fileWriter_ )
      {
	LOG4CPLUS_WARN( logger_, "Terminating leftover file writer." );
	fileWriter_->endRun();
	delete fileWriter_;
	fileWriter_ = NULL;
      }
    if ( badEventsFileWriter_ )
      {
	LOG4CPLUS_WARN( logger_, "Terminating leftover bad event file writer." );
	badEventsFileWriter_->endRun();
	if ( badEventsFileWriter_->getFileSize() == 0 ) badEventsFileWriter_->removeFile();
	delete badEventsFileWriter_;
	badEventsFileWriter_ = NULL;
      }
    // Destroy rate limiter, too, if any.
    if ( rateLimiter_ )
      {
	delete rateLimiter_;
	rateLimiter_ = NULL;
      }

	  // create new writers if path is not empty
	  if ( pathToDataOutFile_ != string("") && 
	       (xdata::UnsignedLongT) fileSizeInMegaBytes_ > (long unsigned int) 0 )
	    {
	      toolbox::net::URL u( appContext_->getContextDescriptor()->getURL() );
	      fileWriter_ = new emu::daq::writer::RawDataFile( 1000000*fileSizeInMegaBytes_, 
							       pathToDataOutFile_.toString(), 
							       u.getHost(), "EmuRUI", instance_, emudaqrui::versions, &logger_ );
	      // Create a rate limiter, but not for calibration or STEP runs.
	      if ( runType_.toString() == "Monitor" ||
		   runType_.toString() == "Debug"      ){
		rateLimiter_ = new emu::daq::writer::RateLimiter( fileWritingRateLimitInHz_, fileWritingRateSampleSize_ );
		fileWritingVetoed_ = false;
	      }
	    }
	  else if ( runType_.toString() != "Monitor" &&
		    runType_.toString() != "Debug"      ) // must be a calibration or STEP run...
	    {
	      LOG4CPLUS_FATAL( logger_, "A calibration run or a STEP run has been started without specifying a directory and/or maximum size for data files. Please set \"pathToRUIDataOutFile\" and \"ruiFileSizeInMegaBytes\" to nonzero values in the XML configuration file." );
	      moveToFailedState();
	    }

	  // inform the file writer about the new run
	  try{
	    if ( fileWriter_ ) fileWriter_->startNewRun( runNumber_.value_, 
							 isBookedRunNumber_.value_,
							 runStartTime_, 
							 runType_ );
	  }
	  catch(string e){
	    LOG4CPLUS_FATAL( logger_, e );
	    moveToFailedState();
	  }
	  if ( pathToBadEventsFile_ != string("") && 
	       (xdata::UnsignedLongT) fileSizeInMegaBytes_ > (long unsigned int) 0 )
	    {
	      toolbox::net::URL u( appContext_->getContextDescriptor()->getURL() );
// 	      badEventsFileWriter_ = new emu::daq::writer::RawDataFile( 1000000*fileSizeInMegaBytes_, pathToDataOutFile_.toString(), "EmuRUI", instance_, emudaqrui::versions, &logger_ );
	      badEventsFileWriter_ = new emu::daq::writer::RawDataFile( 1000000*fileSizeInMegaBytes_, 
									pathToDataOutFile_.toString(), 
									u.getHost(), "EmuRUI", instance_, emudaqrui::versions, &logger_ );
	    }
	  if ( badEventsFileWriter_ ){
	    try{
	      badEventsFileWriter_->startNewRun( runNumber_.value_, 
						 isBookedRunNumber_.value_, 
						 runStartTime_, 
						 string("BadEvents") );
	    }
	    catch(string e){
	      LOG4CPLUS_ERROR( logger_, e );
	      // Don't moveToFailedState, bad events file is not worth stopping the run for.
	    }
	  }
}

void emu::daq::rui::Application::writeDataToFile(  char* const data, const int dataLength, const bool newEvent ){
  if ( fileWriter_ ){
    try{
      if ( newEvent ){
	fileWriter_->startNewEvent();
	fileWritingVetoed_ = ! rateLimiter_->acceptEvent();
      }
      if ( ! fileWritingVetoed_.value_ ) fileWriter_->writeData( data, dataLength );
    } catch(string e) {
      LOG4CPLUS_FATAL( logger_, e );
      moveToFailedState();
    }
  }
}

int emu::daq::rui::Application::continueConstructionOfSuperFrag()
  throw (emu::daq::rui::exception::Exception){

  // Possible return values
  const int extraPauseForOtherThreads   = 5000; // [microsecond]
  const int noExtraPauseForOtherThreads = 0;    // [microsecond]
  const int notToBeRescheduled          = -1;

  unsigned int nBytesRead = 0;

  if ( maxEvents_.value_ >= 0 && nEventsRead_.value_ >= (unsigned long) maxEvents_.value_ ) 
    return notToBeRescheduled;

//   //DEBUG_START
//   insertEmptySuperFragments( eventNumber_, eventNumber_+4 );
//   eventNumber_ = eventNumber_ + 5;
//   nEventsRead_ = nEventsRead_ + 5;
//   blocksArePendingTransmission_ = passDataOnToRUBuilder_.value_;
//   return noExtraPauseForOtherThreads;
//   //DEBUG_END

  if (deviceReader_ == NULL) return notToBeRescheduled;

  // Prepare to read the first first event if we have not yet done so:
  if ( (xdata::UnsignedLongT) nEventsRead_ == (unsigned long) 0 && ! deviceReader_->isResetAndEnabled() ){
    try{
      deviceReader_->resetAndEnable();
    }
    catch(std::runtime_error e){

      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": "                << e.what();
      LOG4CPLUS_FATAL(logger_, oss.str());
      moveToFailedState();
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    catch(...){
      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
      LOG4CPLUS_FATAL(logger_, oss.str());
      moveToFailedState();
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_->getLogMessage().length() > 0 )
      LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());
  } // if ( nEventsRead_ == (unsigned long) 0 )

    // See if there's something to read and then read it:
  try{
    nBytesRead = deviceReader_->readNextEvent();
  }
  catch(...){
    stringstream oss;
    oss << "Failed to read from " << inputDeviceName_.toString()
	<< ": unknown exception.";
    LOG4CPLUS_ERROR(logger_, oss.str());
  }

  if ( deviceReader_->getLogMessage().length() > 0 )
    LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());

  if ( nBytesRead == 0 ){
    // No data ==> no business being here. Try to read again later.
    // But in this case give the other threads a break 
    // in case a 'Halt' FSM state transition needs to be fired.
    // Otherwise they may have to wait a couple of seconds or 
    // sometimes minutes (!) to take the semaphore...
    return extraPauseForOtherThreads;
  }

  errorFlag_ |= deviceReader_->getErrorFlag();

  if ( nBytesRead < 8 ){
    LOG4CPLUS_ERROR(logger_, 
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    " read " << nBytesRead << " bytes only.");
  }

  nReadingPassesInEvent_++;

  if ( (xdata::UnsignedLongT) nEventsRead_ == (unsigned long) 0 && nReadingPassesInEvent_ == 1 ) {
    // first event started --> a new run

    insideEvent_ = false;
    createFileWriters();

    // inform emuTA about the L1A number of the first event read
    try{
      sendEventNumberToTA( deviceReader_->eventNumber() );
    } catch( xcept::Exception e ) {
      LOG4CPLUS_WARN(logger_,"Failed to inform emuTA about the L1A number of the first event read: " +  xcept::stdformat_exception_history(e));
    }

  }

  char* data;
  int   dataLength  = 0;

  data = deviceReader_->data();

  if ( data!=NULL ){

    dataLength = deviceReader_->dataLength();
    if ( dataLength>0 ) {

      bool header  = hasHeader(data,dataLength);
      bool trailer = hasTrailer(data,dataLength);

      if ( trailer && inputDataFormatInt_ == emu::daq::reader::Base::DDU ) interestingDDUErrorBitPattern(data,dataLength);

      stringstream ss;
      ss << "Inside event: " << insideEvent_
	 << " Last L1A: " << eventNumber_
	 << " N read: " << nEventsRead_.toString()
	 << " This L1A: " << deviceReader_->eventNumber()
	 << " Length: " << dataLength
	 << " Header: " << header
	 << " Trailer: " << trailer;
      if ( inputDeviceType_ == "spy"  ){
	ss << " Packets: " << ( errorFlag_ >> 8 );
	if ( errorFlag_ & 0x00ff ){
	  ss << " Errors: "
	     << (errorFlag_ & emu::daq::reader::Spy::EndOfEventMissing ? "EndOfEventMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Timeout ? "Timeout " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::PacketsMissing ? "PacketsMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::LoopOverwrite ? "LoopOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::BufferOverwrite ? "BufferOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Oversized ? "Oversized" : "" );
	  LOG4CPLUS_WARN(logger_, ss.str());
	}
// 	else{
// 	  LOG4CPLUS_INFO(logger_, ss.str());
// 	}
      }

//       printData(ss,data,dataLength);

      if ( insideEvent_ ) {

	writeDataToFile( data, dataLength, header );

	if ( header ){
	  LOG4CPLUS_WARN(logger_, 
			  "No trailer in event " << eventNumber_ << 
			  " ("             << nEventsRead_ <<
			  " so far) from " << inputDeviceName_.toString() <<
			  ", size: "       << dataLength );
	  // Prepare the old block(s) to be sent out. 
	  // They will be assumed to belong to the previous known event number.
	  if ( passDataOnToRUBuilder_.value_ ) finalizeSuperFragment();
	  previousEventNumber_ = eventNumber_;
	  nEventsRead_++;
	  // Mark the last block for clients
	  makeClientsLastBlockCompleteEvent();
	  // Get the new event number.
	  eventNumber_ = deviceReader_->eventNumber();
	  // Ensure there's no gap in the events by inserting dummy super-fragments if necessary.
	  if ( passDataOnToRUBuilder_.value_ ) ensureContiguousEventNumber();
	  // New event started, reset counter of passes
	  nReadingPassesInEvent_ = 1;
	} // if ( header )

	if ( passDataOnToRUBuilder_.value_ ){
	  // If the EmuRUI to RU memory pool has room for another data block
	  if(!ruiRuPool_->isHighThresholdExceeded()){
	    // Fill block and append it to superfragment
	    appendNewBlockToSuperFrag( data, dataLength, eventNumber_ );
	  }
	  else { 
	    LOG4CPLUS_WARN(logger_, "Event fragment dropped. (EmuRUI-to-RU memory pool's high threshold exceeded.)");
	  }
	}

	if ( trailer ){
	  // Prepare the block(s) to be sent out.
	  // They will be assumed to belong to the previous known event number.
	  if ( passDataOnToRUBuilder_.value_ ) finalizeSuperFragment();
	  insideEvent_ = false;
	  previousEventNumber_ = eventNumber_;
	  nEventsRead_++;
	  // Current super-fragment is now ready to be sent to the RU
	  blocksArePendingTransmission_ = passDataOnToRUBuilder_.value_;
	} // if ( trailer )

      } // if ( insideEvent_ )
      else {

	if ( !header && trailer ){
	  LOG4CPLUS_WARN(logger_, 
			  "No header in event " << eventNumber_ << 
			  " ("             << nEventsRead_ <<
			  " so far) from " << inputDeviceName_.toString() <<
			  ", size: "       << dataLength );
	}

	writeDataToFile( data, dataLength, header || trailer );

	if ( header ){
	  // Get the new event number.
	  eventNumber_ = deviceReader_->eventNumber();
	  // Ensure there's no gap in the events by inserting dummy super-fragments if necessary.
	  if ( passDataOnToRUBuilder_.value_ ) ensureContiguousEventNumber();
	  // New event started, reset counter of passes
	  nReadingPassesInEvent_ = 1;
	}

	if ( passDataOnToRUBuilder_.value_ ){
	  // If the EmuRUI to RU memory pool has room for another data block
	  if(!ruiRuPool_->isHighThresholdExceeded()){
	    // Fill block and append it to superfragment
	    appendNewBlockToSuperFrag( data, dataLength, eventNumber_ );
	  }
	  else { 
	    LOG4CPLUS_WARN(logger_, "Event fragment dropped. (EmuRUI-to-RU memory pool's high threshold exceeded.)");
	  }
	}
	insideEvent_ = true;

	if ( trailer ){
	  // Prepare the block(s) to be sent out.
	  if ( passDataOnToRUBuilder_.value_ ) finalizeSuperFragment();
	  insideEvent_ = false;
	  previousEventNumber_ = eventNumber_;
	  nEventsRead_++;
	  // Current super-fragment is now ready to be sent to the RU
	  blocksArePendingTransmission_ = passDataOnToRUBuilder_.value_;
	}

      } // if ( insideEvent_ ) else

      // Store this data to be sent to clients (if any)
      addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
      // TO DO: handle abnormal cases too
      if ( trailer ) errorFlag_ = 0;

    } // if ( dataLength>0 )

  } // if ( data!=NULL )

  if ( ( nEventsRead_.value_+1 <   10                                        ) ||
       ( nEventsRead_.value_+1 <  100 && (nEventsRead_.value_+1) %   10 == 0 ) ||
       ( nEventsRead_.value_+1 < 1000 && (nEventsRead_.value_+1) %  100 == 0 ) ||
       (                                 (nEventsRead_.value_+1) % 1000 == 0 )    ){
    LOG4CPLUS_DEBUG(logger_, 
		    "Read event "    << eventNumber_ << 
		    " ("             << nEventsRead_ <<
		    " so far) from " << inputDeviceName_.toString() <<
		    ", size: "       << dataLength );
  }


  return noExtraPauseForOtherThreads;

}

int emu::daq::rui::Application::continueSTEPRun()
  throw (emu::daq::rui::exception::Exception){

  // Possible return values
  const int extraPauseForOtherThreads   = 5000; // [microsecond]
  const int noExtraPauseForOtherThreads = 0;    // [microsecond]
  const int notToBeRescheduled          = -1;

  unsigned int nBytesRead = 0;

  if ( maxEvents_.value_ >= 0 && STEPEventCounter_.getLowestCount() >= (unsigned long) maxEvents_.value_ )
    return notToBeRescheduled;

  if (deviceReader_ == NULL) return notToBeRescheduled;

  // Prepare to read the first event if we have not yet done so:
  if ( (xdata::UnsignedLongT) nEventsRead_ == (unsigned long) 0 && ! deviceReader_->isResetAndEnabled() ){
    try{
      deviceReader_->resetAndEnable();
    }
    catch(std::runtime_error e){

      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": "                << e.what();
      LOG4CPLUS_FATAL(logger_, oss.str());
      moveToFailedState();
    }
    catch(...){
      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
      LOG4CPLUS_FATAL(logger_, oss.str());
      moveToFailedState();
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_->getLogMessage().length() > 0 )
      LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());
  } // if ( nEventsRead_ == (unsigned long) 0 )

    // See if there's something to read and then read it:
  try{
    nBytesRead = deviceReader_->readNextEvent();
  }
  catch(...){
    stringstream oss;
    oss << "Failed to read from " << inputDeviceName_.toString()
	<< ": unknown exception.";
    LOG4CPLUS_ERROR(logger_, oss.str());
  }

  if ( deviceReader_->getLogMessage().length() > 0 )
    LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());

  if ( nBytesRead == 0 ){
    // No data ==> no business being here. Try to read again later.
    // But in this case give the other threads a break 
    // in case a 'Halt' FSM state transition needs to be fired.
    // Otherwise they may have to wait a couple of seconds or 
    // sometimes minutes (!) to take the semaphore...
    return extraPauseForOtherThreads;
  }

  errorFlag_ |= deviceReader_->getErrorFlag();

  if ( nBytesRead < 8 ){
    LOG4CPLUS_ERROR(logger_, 
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    " read " << nBytesRead << " bytes only.");
  }

  nReadingPassesInEvent_++;

  if ( (xdata::UnsignedLongT) nEventsRead_ == (unsigned long) 0 && nReadingPassesInEvent_ == 1 ) {
    // first event started --> a new run

    insideEvent_ = false;
    createFileWriters();

    // inform emuTA about the L1A number of the first event read
    try{
      sendEventNumberToTA( deviceReader_->eventNumber() );
    } catch( xcept::Exception e ) {
      LOG4CPLUS_WARN(logger_,"Failed to inform emuTA about the L1A number of the first event read: " +  xcept::stdformat_exception_history(e));
    }

  }

  char* data;
  int   dataLength  = 0;

  data = deviceReader_->data();

  if ( data!=NULL ){

    dataLength = deviceReader_->dataLength();
    if ( dataLength>0 ) {


      bool header  = hasHeader(data,dataLength);
      bool trailer = hasTrailer(data,dataLength);

//       if (header) printData( cout, data, 24 ); // print the DDU header only

      if ( header ) nEventsRead_++;

      if ( trailer && inputDataFormatInt_ == emu::daq::reader::Base::DDU ) interestingDDUErrorBitPattern(data,dataLength);

      if ( header && ! STEPEventCounter_.isInitialized() ){
	unsigned int maxEvents;
	if ( maxEvents_.value_ < 0 ) maxEvents = 0xffffffff;
	else                         maxEvents = (unsigned int) maxEvents_.value_;
	STEPEventCounter_.initialize( maxEvents, data );
      }

      stringstream ss;
      ss << "Inside event: " << insideEvent_
	 << " Last L1A: " << eventNumber_
	 << " N read: " << nEventsRead_.toString()
	 << " This L1A: " << deviceReader_->eventNumber()
	 << " Length: " << dataLength
	 << " Header: " << header
	 << " Trailer: " << trailer
	 << " STEP counts: " << STEPEventCounter_.print();
      if ( inputDeviceType_ == "spy"  ){
	ss << " Packets: " << ( errorFlag_ >> 8 );
	if ( errorFlag_ & 0x00ff ){
	  ss << " Errors: "
	     << (errorFlag_ & emu::daq::reader::Spy::EndOfEventMissing ? "EndOfEventMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Timeout ? "Timeout " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::PacketsMissing ? "PacketsMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::LoopOverwrite ? "LoopOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::BufferOverwrite ? "BufferOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Oversized ? "Oversized" : "" );
	  LOG4CPLUS_WARN(logger_, ss.str());
	}
	else{
	  LOG4CPLUS_INFO(logger_, ss.str());
	}
      }
      else{
	LOG4CPLUS_INFO(logger_, ss.str());
      }
//       printData(ss,data,dataLength);

      if ( insideEvent_ ) { // In STEP runs, insideEvent_ means that we are inside a *needed* event.

	if ( !header && !trailer ){
	  writeDataToFile( data, dataLength );
	  // Store this data to be sent to clients (if any)
	  addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	} // if ( !header && !trailer )

	else if ( header && !trailer ){
	  LOG4CPLUS_WARN(logger_, 
			 "No trailer in event " << eventNumber_ << 
			 " ("             << nEventsRead_ <<
			 " so far) from " << inputDeviceName_.toString() <<
			 ", size: "       << dataLength );
	  // Mark the last block for clients
	  makeClientsLastBlockCompleteEvent();
	  insideEvent_ = false;
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started, reset counter of passes
	    nReadingPassesInEvent_ = 1;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	    insideEvent_ = true;
	  }
	} // if ( header && !trailer )

	else if ( !header && trailer ){
	  writeDataToFile( data, dataLength );
	  insideEvent_ = false;
	  // Store this data to be sent to clients (if any)
	  addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	} // if ( !header && trailer )

	else if ( header && trailer ){
	  LOG4CPLUS_WARN(logger_, 
			 "No trailer in event " << eventNumber_ << 
			 " ("             << nEventsRead_ <<
			 " so far) from " << inputDeviceName_.toString() <<
			 ", size: "       << dataLength );
	  // Mark the last block for clients
	  makeClientsLastBlockCompleteEvent();
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started and ended, reset counter of passes
	    nReadingPassesInEvent_ = 0;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	  }
	  insideEvent_ = false;
	} // if ( header && trailer )


      } // if ( insideEvent_ )
      else {

	if ( !header && !trailer ){
	  LOG4CPLUS_WARN(logger_, 
			  "No header in event after " << eventNumber_ << 
			  " ("             << nEventsRead_ <<
			  " so far) from " << inputDeviceName_.toString() <<
			  ", size: "       << dataLength );
	} // if ( !header && !trailer )

	else if ( header && !trailer ){
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started, reset counter of passes
	    nReadingPassesInEvent_ = 1;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	    insideEvent_ = true;
	  }
	} // if ( header && !trailer )

	else if ( !header && trailer ){
	  LOG4CPLUS_WARN(logger_, 
			  "No header in event after " << eventNumber_ << 
			  " ("             << nEventsRead_ <<
			  " so far) from " << inputDeviceName_.toString() <<
			  ", size: "       << dataLength );
	} // if ( !header && trailer )

	else if ( header && trailer ){
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started and ended, reset counter of passes
	    nReadingPassesInEvent_ = 0;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, trailer, errorFlag_, data, dataLength );
	  }
	  insideEvent_ = false;
	} // if ( header && trailer )

      } // if ( insideEvent_ ) else

      if ( trailer ) errorFlag_ = 0;

    } // if ( dataLength>0 )

  } // if ( data!=NULL )


  return noExtraPauseForOtherThreads;

}


void emu::daq::rui::Application::ensureContiguousEventNumber(){
  // If event number is incremented by more than one, 
  // fill the gap with empty events (super fragments) to make the event builder happy.
  // Take into account that the max possible L1A value (eventNumber_) is 2^24-1 = 0xffffff

  // Don't do it if this is the first event read (i.e., previous event number is zero)
  if ( previousEventNumber_ == 0 ) return;

  const unsigned int maxL1A = 0xffffff;

  try{

    if ( previousEventNumber_ < eventNumber_ ){ // not wrapped around maxL1A
      
      if ( previousEventNumber_ + 1 < eventNumber_ )
	insertEmptySuperFragments( previousEventNumber_ + 1, eventNumber_-1 );
      
    }
    else if ( previousEventNumber_ > eventNumber_ ){ // wrapped around maxL1A
      
      if ( previousEventNumber_ < maxL1A )
	insertEmptySuperFragments( previousEventNumber_ + 1, maxL1A );

      if ( 0 < eventNumber_ )
	insertEmptySuperFragments( 0, eventNumber_-1 );
      
    }
    else{
      LOG4CPLUS_ERROR(logger_, "event number = previous event number = " << eventNumber_ );
    }
  }
  catch( emu::daq::rui::exception::Exception e ){
    LOG4CPLUS_ERROR(logger_, "Failed to insert empty super-fragments: " << stdformat_exception_history(e));
  }
}

void emu::daq::rui::Application::insertEmptySuperFragments( const unsigned long fromEventNumber, const unsigned long toEventNumber )
  throw (emu::daq::rui::exception::Exception){

  const char* data     = NULL;
  const int dataLength = 0;

  LOG4CPLUS_WARN(logger_, "Inserting " << toEventNumber-fromEventNumber+1 
		 << " empty events (" << fromEventNumber << " through " << toEventNumber << ")");  

  for ( unsigned long eventNumber = fromEventNumber; eventNumber <= toEventNumber; ++eventNumber ){
    appendNewBlockToSuperFrag( data, dataLength, eventNumber );
    nEventsRead_++;
    previousEventNumber_ = eventNumber;

// //DEBUG_START
//     const int DBGdataLength = 8;
//     char DBGdata[DBGdataLength] = {'a','b','c','d','e','f','g','h'};
//     appendNewBlockToSuperFrag( &DBGdata[0], DBGdataLength, eventNumber );
// //DEBUG_END
  }

}

void emu::daq::rui::Application::printData(std::ostream& os, char* data, const int dataLength){
  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
  os << "_________________________________" << endl;
  os << "                +3   +2   +1   +0" << endl;
  for(int i = 0; i < dataLength/2; i+=4)
    {
      os << std::dec;
      os.width(8); os.fill(' ');
      os << i;
      os << "      ";
      os << std::hex;
      os.width(4); os.fill('0');    
      os << shortData[i+3] << " ";
      os.width(4); os.fill('0');    
      os << shortData[i+2] << " ";
      os.width(4); os.fill('0');    
      os << shortData[i+1] << " ";
      os.width(4); os.fill('0');    
      os << shortData[i  ] << std::endl;
    }
  os<<std::dec;
  os.width(0);
}

bool emu::daq::rui::Application::hasHeader( char* const data, const int dataLength ){
  // By now data must have been stripped of any filler words.
  const int DDUHeaderLength = 24; // bytes
  const int DCCHeaderLength = 16; // bytes
  bool headerFound = false;
  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);

  if ( inputDataFormatInt_ == emu::daq::reader::Base::DDU ){
    if ( dataLength < DDUHeaderLength ) return false; // can the data be split in the header???
    headerFound = ( (shortData[3] & 0xf000) == 0x5000 &&
		    shortData[5]            == 0x8000 &&
		    shortData[6]            == 0x0001 &&
		    shortData[7]            == 0x8000    );
//     stringstream ss;
//     ss <<
//       " (shortData[3] & 0xf000) " << std::hex << std::setw(4) << (shortData[3] & 0xf000) <<
//       " shortData[5] " << std::hex << std::setw(4) << shortData[5] <<
//       " shortData[6] " << std::hex << std::setw(4) << shortData[6] <<
//       " shortData[7] " << std::hex << std::setw(4) << shortData[7] <<
//       " headerFound " << std::dec << headerFound   << std::endl;
//     printData(ss,data,dataLength);
//     std::cout << ss.str() << std::endl << std::flush;
//     LOG4CPLUS_INFO(logger_,ss.str());
  }
  else if ( inputDataFormatInt_ == emu::daq::reader::Base::DCC ){
  if ( dataLength < DCCHeaderLength ) return false; // can the data be split in the header???
    headerFound = ( (shortData[0] & 0x00ff) == 0x005f &&
		    (shortData[3] & 0xf000) == 0x5000 &&
		    (shortData[7] & 0xff00) == 0xd900    );
  }
  return headerFound;
}

bool emu::daq::rui::Application::hasTrailer( char* const data, const int dataLength ){
  // By now data must have been stripped of any filler words.
  const int DDUTrailerLength = 24; // bytes
  const int DCCTrailerLength = 16; // bytes
  bool trailerFound = false;
  unsigned short *shortData = reinterpret_cast<unsigned short *>(data);
//   std::cout << "inputDataFormatInt ?=? emu::daq::reader::Base::DDU " << inputDataFormatInt_ << " ?=? " << emu::daq::reader::Base::DDU << std::endl << std::flush;
  if ( inputDataFormatInt_ == emu::daq::reader::Base::DDU ){
    if ( dataLength < DDUTrailerLength ) return false; // can the data be split in the trailer???
    int start = (dataLength - DDUTrailerLength) / 2;
    trailerFound = ( shortData[start+0] == 0x8000 &&
		     shortData[start+1] == 0x8000 &&
		     shortData[start+2] == 0xffff &&
		     shortData[start+3] == 0x8000 &&
		     ( shortData[start+11] & 0xf000 ) == 0xa000 );
//     stringstream ss;
//     ss <<
//       " shortData[start+0] " << std::hex << std::setw(4) << shortData[start+0] <<
//       " shortData[start+1] " << std::hex << std::setw(4) << shortData[start+1] <<
//       " shortData[start+2] " << std::hex << std::setw(4) << shortData[start+2] <<
//       " shortData[start+3] " << std::hex << std::setw(4) << shortData[start+3] <<
//       " ( shortData[start+11] & 0xf000 ) " << std::hex << std::setw(4) << ( shortData[start+11] & 0xf000 ) <<
//       " trailerFound " << std::dec << trailerFound;
//     std::cout << ss.str() << std::endl << std::flush;
//     LOG4CPLUS_INFO(logger_, ss.str());
  }
  else if ( inputDataFormatInt_ == emu::daq::reader::Base::DCC ){
    if ( dataLength < DCCTrailerLength ) return false; // can the data be split in the trailer???
    int start = (dataLength - DCCTrailerLength) / 2;
    trailerFound = ( (shortData[start+3] & 0xff00) == 0xef00 &&
		     (shortData[start+4] & 0x000f) == 0x0003 &&
		     (shortData[start+7] & 0xff00) == 0xaf00    );
  }
  return trailerFound;
}


bool emu::daq::rui::Application::interestingDDUErrorBitPattern(char* const data, const int dataLength){
  // At this point dataLength should no longer contain Ethernet padding.

  // Check for interesting error bit patterns (defined by J. Gilmore):
  // 1) Critical Error = Sync Reset or Hard Reset required
  //        TTS out of sync or in error   -OR-
  //        DDU Trailer-1 bit 47
  //    ----> Persistent, these bits stay set for all events until
  //         RESET occurs.
  // 2) Error Detected = bad event  (no reset)
  //        DDU Trailer-1 bit 46
  //    ----> Only set for the single event with a detected error.
  // 3) Warning = Buffer Near Full (no reset)
  //        DDU Trailer-1 bit 31  -OR-
  //        TTS overflow warning
  //    ----> Remains set until the condition abates.
  // 4) Special Warning (Rare & Interesting occurence, no reset)
  //        DDU Trailer-1 bit 45
  //    ----> Remains set until the condition abates; may be assosiated
  //         with 1) or 2) above.

  bool foundError = false;
  const unsigned long DDUTrailerLength = 24; // bytes
  unsigned short *trailerShortWord =
    reinterpret_cast<unsigned short*>( data + dataLength - DDUTrailerLength );
  
  // TTS/FMM status defined in C.D.F.
  const short FED_Overflow  = 0x0010;
  const short FED_OutOfSync = 0x0020;
  const short FED_Error     = 0x00C0;

  // 1)
  if ( trailerShortWord[8] & FED_OutOfSync            ||
       (trailerShortWord[8] & FED_Error) == FED_Error || // FED_Error has 2 bits set!!!
       trailerShortWord[6] & 0x8000           ) { // DDU Trailer-1 bit 47
    if ( persistentDDUError_.toString().size() == 0 ){
      stringstream ss;
      ss << "DDU error: Sync or Hard Reset required. First in event "
	 << deviceReader_->eventNumber()
	 << " (after " << nEventsRead_+1 << " read)";
      persistentDDUError_ = ss.str();
    }
//     LOG4CPLUS_ERROR(logger_, "Critical DDU error in "
//        << deviceReader_->getName() << "[" << hardwareMnemonic_.toString() << "]"
//        << ". Sync Reset or Hard Reset required. (bit T:5|T:6&7|T-1:47) Event "
//        << deviceReader_->eventNumber()
//        << " (" << nEventsRead_+1 << " read)");
    // << " FED_OutOfSync: " << short(trailerShortWord[8] & FED_OutOfSync)
    // << " FED_Error: " << short(trailerShortWord[8] & FED_Error) 
    // << " DDU Trailer-1 bit 47: " << short(trailerShortWord[6] & 0x8000) );
    foundError = true;
  }
  // 2)
  if ( trailerShortWord[6] & 0x4000 ) {    // DDU Trailer-1 bit 46
//     LOG4CPLUS_ERROR(logger_,
// 		    "DDU error: bad event read from " 
// 		    << deviceReader_->getName()
// 		    << ". (bit T-1:46) Event "
// 		    << deviceReader_->eventNumber()
// 		    << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 3)
  if ( trailerShortWord[8] & FED_Overflow ||
       trailerShortWord[5] & 0x8000          ) {  // DDU Trailer-1 bit 31
//     LOG4CPLUS_WARN(logger_,
// 		   "DDU buffer near Full in "
// 		   << deviceReader_->getName() 
// 		   << ". (bit T:4|T-1:31) Event "
// 		   << deviceReader_->eventNumber()
// 		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 4)
  if ( trailerShortWord[6] & 0x2000 ) {      // DDU Trailer-1 bit 45
//     LOG4CPLUS_WARN(logger_,
// 		   "DDU special warning in "
// 		   << deviceReader_->getName() 
// 		   << ". (bit T-1:45) Event "
// 		   << deviceReader_->eventNumber()
// 		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }

  return foundError;
}

void emu::daq::rui::Application::appendNewBlockToSuperFrag( const char* data, const unsigned long dataLength, const unsigned long eventNumber )
throw (emu::daq::rui::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }


    // Fill block with super-fragment data
    try
    {
         fillBlock( bufRef, data, dataLength, eventNumber );
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to fill super-fragment data block";
        oss << " : " << stdformat_exception_history(e);
        s = oss.str();

        XCEPT_RETHROW(emu::daq::rui::exception::Exception, s, e);
    }

    // Append block to super-fragment under construction
    superFragBlocks_.push_back(bufRef);
    
//     printBlocks( superFragBlocks_ );
}


void emu::daq::rui::Application::printBlocks( deque<toolbox::mem::Reference*> d ){
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

void emu::daq::rui::Application::fillBlock(
    toolbox::mem::Reference *bufRef,
    const char*              data,
    const unsigned int       dataLength,
    const unsigned long      eventNumber
)
throw (emu::daq::rui::exception::Exception)
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

    if ( data != NULL && dataLength != 0 ) ::memcpy( fedAddr, data, dataLength );

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

    block->eventNumber = eventNumber;

    if ( data == NULL || dataLength == 0 ){
      // Empty event, which needs 1 block only
      block->blockNb = 0;
      block->nbBlocksInSuperFragment = 1;
    }
// //DEBUG_START
//       block->blockNb = 0;
//       block->nbBlocksInSuperFragment = 1;
// //DEBUG_END
}


void emu::daq::rui::Application::finalizeSuperFragment()
{

     deque<toolbox::mem::Reference*>::iterator pos; // BK
     toolbox::mem::Reference            *bufRef = 0;
     I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME *block  = 0;

     // Count and number blocks belonging to current event
     int blockCount = 0;
     for(pos=superFragBlocks_.begin(); pos!=superFragBlocks_.end(); ++pos){
       bufRef = *pos;
       block = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)bufRef->getDataLocation();
       if ( eventNumber_.value_ == block->eventNumber ){
	 block->blockNb = blockCount;
	 blockCount++;
       }
     }
     
     // Update block count field in all blocks of this event
     for(pos=superFragBlocks_.begin(); pos!=superFragBlocks_.end(); ++pos){
       bufRef = *pos;
       block = (I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*)bufRef->getDataLocation();
       if ( eventNumber_.value_ == block->eventNumber ){
	 block->nbBlocksInSuperFragment = blockCount;
       }
     }
}


string emu::daq::rui::Application::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool emu::daq::rui::Application::onI2oException(xcept::Exception &exception, void *context)
{

    LOG4CPLUS_ERROR(logger_, "I2O excpetion: " << xcept::stdformat_exception_history(exception));

    return true;
}

string emu::daq::rui::Application::createI2oErrorMsg
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

xoap::MessageReference emu::daq::rui::Application::onReset(xoap::MessageReference msg)
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

void emu::daq::rui::Application::getTidOfEmuTA()
  throw ( xcept::Exception ){

  // First emu::daq::ta::Application's application descriptor
  taDescriptors_.clear();
  try{
    taDescriptors_ = getAppDescriptors(zone_, "emu::daq::ta::Application");
  } catch(emu::daq::rui::exception::Exception e) {
    XCEPT_RETHROW( xcept::Exception, 
		  "Failed to get application descriptor for class emu::daq::ta::Application", e);
  }

  if ( taDescriptors_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, 
		 "Failed to get application descriptor for class emu::daq::ta::Application");
  }

  if ( taDescriptors_.size() >= 2 ){
    LOG4CPLUS_WARN(logger_,"Got more than one application descriptors for class emu::daq::ta::Application. Using the first one.");
  }
  
  // Now the Tid
  i2o::utils::AddressMap *i2oAddressMap  = i2o::utils::getAddressMap();

  emuTATid_ = i2oAddressMap->getTid( taDescriptors_[0] );

}


void emu::daq::rui::Application::sendEventNumberToTA( unsigned long firstEventNumber )
  throw ( xcept::Exception ){

  const unsigned long frameSize = sizeof(I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME) + sizeof(firstEventNumber);

  toolbox::mem::Reference *ref = 0;
  try 
    {
      ref = toolbox::mem::getMemoryPoolFactory()->getFrame(ruiTaPool_, frameSize);
      
      PI2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME frame = (PI2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME) ref->getDataLocation();   
      
      
      frame->PvtMessageFrame.StdMessageFrame.MsgFlags         = 0;
      frame->PvtMessageFrame.StdMessageFrame.VersionOffset    = 0;
      frame->PvtMessageFrame.StdMessageFrame.TargetAddress    = emuTATid_;
      frame->PvtMessageFrame.StdMessageFrame.InitiatorAddress = tid_;
      frame->PvtMessageFrame.StdMessageFrame.MessageSize      = (sizeof(I2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME)) >> 2;
      
      frame->PvtMessageFrame.StdMessageFrame.Function = I2O_PRIVATE_MESSAGE;
      frame->PvtMessageFrame.XFunctionCode            = I2O_EMU_FIRST_EVENT_NUMBER_CODE;
      frame->PvtMessageFrame.OrganizationID           = XDAQ_ORGANIZATION_ID;
      
      frame->firstEventNumber = firstEventNumber;
      
      ref->setDataSize(frame->PvtMessageFrame.StdMessageFrame.MessageSize << 2);
      appContext_->postFrame(ref, appDescriptor_, taDescriptors_[0], i2oExceptionHandler_, taDescriptors_[0]);
      LOG4CPLUS_INFO(logger_,
		     "Sent first event number " << firstEventNumber <<
		     " to emu::daq::ta::Application of tid: " << frame->PvtMessageFrame.StdMessageFrame.TargetAddress );
    } 
  catch (toolbox::mem::exception::Exception & me)
    {
      XCEPT_RETHROW( xcept::Exception, xcept::stdformat_exception_history(me), me );
    }
  catch (xdaq::exception::Exception & e)
    // Actually we may never catch this, as i2oExceptionHandler_ will. And if  we didn't use i2oExceptionHandler_, 
    // the Executive would catch the I20 exceptions.
    {
      LOG4CPLUS_WARN(logger_, "Tried to send first event number to emu::daq::ta::Application: " + 
		     xcept::stdformat_exception_history(e));
      // Retry 3 times
      bool retryOK = false;
      for (int k = 0; k < 3; k++)
	{
	  try
	    {
	      appContext_->postFrame(ref, appDescriptor_, taDescriptors_[0], i2oExceptionHandler_, taDescriptors_[0]);
	      retryOK = true;
	      break;
	    }
	  catch (xdaq::exception::Exception & re)
	    {
	      LOG4CPLUS_WARN(logger_, "Retried to send first event number to emu::daq::ta::Application: " + 
			     xcept::stdformat_exception_history(re));
	    }
	}
      
      if (!retryOK)
	{
	  ref->release();
	  XCEPT_RAISE( xcept::Exception, "Failed to send first event number I2O frame after 3 retries" );
	}
    }
}

xoap::MessageReference emu::daq::rui::Application::onSTEPQuery( xoap::MessageReference msg )
  throw (xoap::exception::Exception){
  
  // Declare a SOAP serializer
  xdata::soap::Serializer serializer;

  // Create reply message
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();

  // Transfer STEP info into xdata as those can readily be serialized into SOAP
  xdata::UnsignedLong                totalCount  = STEPEventCounter_.getNEvents();
  xdata::UnsignedLong                lowestCount = STEPEventCounter_.getLowestCount();
  xdata::Vector<xdata::UnsignedLong> counts;
  xdata::Vector<xdata::Boolean>      masks;
  xdata::Vector<xdata::Boolean>      liveInputs;
  for( unsigned int iInput=0; iInput < emu::daq::rui::STEPEventCounter::maxDDUInputs_; ++iInput ){
    counts.push_back( STEPEventCounter_.getCount( iInput ) );
    masks.push_back( STEPEventCounter_.isMaskedInput( iInput ) );
    liveInputs.push_back( STEPEventCounter_.isLiveInput( iInput ) );
  }

  // Serialize the persistent DDU error
  xoap::SOAPName name = envelope.createName("PersistentDDUError", "xdaq", "urn:xdaq-soap:3.0");;
  xoap::SOAPBodyElement bodyElement = body.addBodyElement( name );
  serializer.exportAll(&persistentDDUError_, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize the number of events read
  name = envelope.createName("EventsRead", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&nEventsRead_, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize the total number of events accepted
  name = envelope.createName("TotalCount", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&totalCount, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize the lowest count
  name = envelope.createName("LowestCount", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&lowestCount, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize counts
  name = envelope.createName("Counts", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&counts, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize masks
  name = envelope.createName("Masks", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&masks, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  // Serialize live inputs
  name = envelope.createName("LiveInputs", "xdaq", "urn:xdaq-soap:3.0");;
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&liveInputs, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);

  return reply;
}

xoap::MessageReference emu::daq::rui::Application::onExcludeDDUInputs( xoap::MessageReference msg )
  throw (xoap::exception::Exception){
  // Exclude DDU inputs from STEP count 
  
  return maskDDUInputs( false, msg );
}

xoap::MessageReference emu::daq::rui::Application::onIncludeDDUInputs( xoap::MessageReference msg )
  throw (xoap::exception::Exception){
  // Include DDU inputs in STEP count 

  return maskDDUInputs( true, msg );
}

xoap::MessageReference emu::daq::rui::Application::maskDDUInputs( const bool in, const xoap::MessageReference msg )
  throw (xoap::exception::Exception){
  // Set mask on DDU inputs for STEP count 

  // Create reply message
  xoap::MessageReference reply = xoap::createMessage();

  // Create a parser
  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");

  // Create a (de)serializer
  xdata::soap::Serializer serializer;

  // xdata::soap::Serializer serializes into xdata
  xdata::Vector<xdata::UnsignedLong> DDUInputs;

  try{
    stringstream ss;
    msg->writeTo( ss );
    DOMDocument* doc = parser->parse( ss.str() );
    
    DOMNode* n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), 
					      xoap::XStr("DDUInputs")         )->item(0);
    serializer.import( &DDUInputs, n );

    for ( unsigned int i = 0; i < DDUInputs.elements(); ++i ){
      int dduInputIndex = (int)( dynamic_cast<xdata::UnsignedLong*> ( DDUInputs.elementAt(i)) )->value_;
      if ( in ) STEPEventCounter_.unmaskInput( dduInputIndex );
      else      STEPEventCounter_.maskInput( dduInputIndex );
    }
    
  } catch( xoap::exception::Exception e ){
    XCEPT_RETHROW(xoap::exception::Exception, "Failed to mask DDU inputs.", e );
  } catch( ... ){
    XCEPT_RAISE(xoap::exception::Exception, "Failed to mask DDU inputs: Unknown exception." );
  }

  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  return reply;
}


/**
 * Provides the factory method for the instantiation of emu::daq::rui::Application.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::rui::Application)
