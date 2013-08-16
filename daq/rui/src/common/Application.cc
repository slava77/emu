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
#include "xdata/InfoSpaceFactory.h"

#include <unistd.h>

#include "toolbox/mem/CommittedHeapAllocator.h"
#include "toolbox/net/URL.h"
#include "toolbox/task/TimerFactory.h"
#include "emu/daq/reader/RawDataFile.h"
#include "emu/daq/reader/Spy.h"
#include "emu/daq/server/I2O.h"
#include "emu/daq/server/SOAP.h"
#include <sstream>
#include <limits>
#include "xdata/soap/Serializer.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "emu/base/TypedFact.h"
#include "emu/daq/rui/FactTypes.h"
#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"

#include "emu/utils/System.h"

emu::daq::rui::Application::Application(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  emu::base::FactFinder( s, emu::base::FactCollection::LOCAL_DAQ, 0 ),
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
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(xdaq::exception::Exception,
            "Failed to define finite state machine", e);
    }

    // Initialise and group parameters
    stdConfigParams_  = initAndGetStdConfigParams();
    stdMonitorParams_ = initAndGetStdMonitorParams();

    // Fill the application's default info space
    putParamsIntoInfoSpace(stdConfigParams_ , appInfoSpace_);
    putParamsIntoInfoSpace(stdMonitorParams_, appInfoSpace_);
    attachListeners();

    bindFsmSoapCallbacks();
    bindI2oCallbacks();

    // Bind web interface
    xgi::bind(this, &emu::daq::rui::Application::css           , "styles.css");
    xgi::bind(this, &emu::daq::rui::Application::defaultWebPage, "Default"   );

    deviceReader_          = NULL;
    fileWriter_            = NULL;
    rateLimiter_           = NULL;
    nReadingPassesInEvent_ = 0;
    insideEvent_           = false;
    errorFlag_             = 0;
    previousEventNumber_   = 0;
    runStartUTC_           = 0;
    ableToWriteToDisk_     = true;

    // bind SOAP client credit message callback
    xoap::bind(this, &emu::daq::rui::Application::onSOAPClientCreditMsg, 
	       "onClientCreditMessage", XDAQ_NS_URI);

    // bind STEP-related SOAP messages
    xoap::bind(this, &emu::daq::rui::Application::onSTEPQuery       ,"STEPQuery"       , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onExcludeDDUInputs,"excludeDDUInputs", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onIncludeDDUInputs,"includeDDUInputs", XDAQ_NS_URI);

    // bind termination SOAP message
    xoap::bind(this, &emu::daq::rui::Application::onTerminate, "Terminate", XDAQ_NS_URI);

    // Memory pool for i2o messages to emu::daq::ta::Application
    stringstream ruiTaPoolName;
    ruiTaPoolName << "EmuRUI" << instance_ << "-to-EmuTA";
    ruiTaPool_ = createHeapAllocatorMemoryPool(poolFactory_, ruiTaPoolName.str());

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::rui::Application::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    uint32_t                    appInstance    = appDescriptor->getInstance();
    stringstream                oss;
    string                      loggerName;


    oss << appClass << "." << setfill('0') << std::setw(2) << appInstance;
    loggerName = oss.str();

    return loggerName;
}


xoap::MessageReference emu::daq::rui::Application::processSOAPClientCreditMsg( xoap::MessageReference msg )
{
  xoap::MessageReference reply;

  xdata::UnsignedInteger instance = 0;
  xdata::Integer         credits = 0;
  xdata::Integer         prescaling = 1;
  xdata::String          name;     

  emu::soap::extractParameters( msg, 
				emu::soap::Parameters()
				.add( "clientInstance"  , &instance   ) 
				.add( "nEventCredits"   , &credits    ) 
				.add( "prescalingFactor", &prescaling ) 
				.add( "clientName"      , &name       ) );

  // Find out who sent this and add the credits to its corresponding server
  bool knownClient = false;
  for ( std::vector<Client*>::iterator c=clients_.begin(); c!=clients_.end(); ++c ){
    if ( (*c)->name->value_ == name.value_ && (*c)->instance->value_ == instance.value_ ){
      knownClient = true;
      (*c)->server->addCredits( credits, prescaling );
      // If client descriptor is not known (non-XDAQ client), send data now:
      if ( (*c)->server->getClientDescriptor() == NULL ){
	reply = (*c)->server->getOldestMessagePendingTransmission();
	if ( !reply.isNull() ){ 
// 	  string rs;
// 	  reply->writeTo(rs);
	  LOG4CPLUS_INFO(logger_, "Sent data to external SOAP client " << name.toString() );
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
	if ( (*c)->server->getClientName() == name.toString() && (*c)->server->getClientInstance() == instance.value_ ){
	  (*c)->server->addCredits( credits, prescaling );
	  break;
	}
      }
    }
    else {
      reply = emu::soap::createFaultReply( "Server", "Could not create an Emu data server for you." );
    }
  }

  return reply;
}


xoap::MessageReference emu::daq::rui::Application::onSOAPClientCreditMsg( xoap::MessageReference msg )
  throw (xoap::exception::Exception)
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
	  catch( xcept::Exception &e )
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
	  stringstream ss1;
	  ss1 << 
			  "emu::daq::rui::Application in undefined state";
	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss1.str() );
	  this->notifyQualified( "error", eObj );
        }
    }
  catch(xcept::Exception &e)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process SOAP client credit message : "
		      << stdformat_exception_history(e));
      stringstream ss2;
      ss2 << 
		      "Failed to process SOAP client credit message : "
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss2.str(), e );
      this->notifyQualified( "error", eObj );
    }
  catch(...)
    {
      LOG4CPLUS_ERROR(logger_,
		      "Failed to process SOAP client credit message : Unknown exception");
      stringstream ss3;
      ss3 << 
		      "Failed to process SOAP client credit message : Unknown exception";
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss3.str() );
      this->notifyQualified( "error", eObj );
    }
  
  try{
    if ( reply.isNull() ) reply = emu::soap::createMessage( "onMessageResponse" );
  }catch( xcept::Exception &e ){
    XCEPT_RETHROW(xoap::exception::Exception, string("Failed to create reply to SOAP client credit message"), e);
  }

  applicationBSem_.give();

  return reply;
}

void emu::daq::rui::Application::onI2OClientCreditMsg(toolbox::mem::Reference *bufRef)
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
	  stringstream ss4;
	  ss4 << 
			  "emu::daq::rui::Application in undefined state";
	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss4.str() );
	  this->notifyQualified( "error", eObj );
        }
    }
    catch(xcept::Exception &e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process I2O client credit message : "
             << stdformat_exception_history(e));
        stringstream ss5;
        ss5 << 
            "Failed to process I2O client credit message : "
             ;
        XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss5.str(), e );
        this->notifyQualified( "error", eObj );
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process I2O client credit message : Unknown exception");
        stringstream ss6;
        ss6 << 
            "Failed to process I2O client credit message : Unknown exception";
        XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss6.str() );
        this->notifyQualified( "error", eObj );
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


vector< xdaq::ApplicationDescriptor* > emu::daq::rui::Application::getAppDescriptors
(
    xdaq::Zone             *zone,
    const string           appClass
)
throw (emu::daq::rui::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
    set< xdaq::ApplicationDescriptor* > descriptors;
    size_t nbApps = 0;

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
      uint32_t minInstance = 99999;
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
  catch(emu::daq::rui::exception::Exception &e)
    {
      taDescriptors.clear();
      XCEPT_RETHROW(emu::daq::rui::exception::Exception, 
		    "Failed to get application descriptors for class emu::daq::ta::Application",
		    e);
    }

  if ( taDescriptors.size() >= 1 ){
    if ( taDescriptors.size() > 1 ){
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      taDescriptors.size() << " emuTA instances found. Trying first one.");
      stringstream ss7;
      ss7 <<  "The embarassement of riches: " << 
		      taDescriptors.size() << " emuTA instances found. Trying first one.";
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss7.str() );
      this->notifyQualified( "error", eObj );
    }

    try{
      emu::soap::Messenger( this ).getParameters( taDescriptors[0], 
						  emu::soap::Parameters()
						  .add( "runNumber"        , &runNumber_         )
						  .add( "maxNumTriggers"   , &maxEvents_         )
						  .add( "isBookedRunNumber", &isBookedRunNumber_ )
						  .add( "runStartTime"     , &runStartTime_      ) );
      runStartUTC_ = toUnixTime( runStartTime_ );
      LOG4CPLUS_INFO( logger_, 
		      "Got from emu::daq::ta run number: " + runNumber_.toString() +
		      ", maximum number of events: " + maxEvents_.toString() +
		      ", whether run is booked: " + isBookedRunNumber_.toString() +
		      ", run start time: " + runStartTime_.toString() + " or " << runStartUTC_ );
    }catch(xcept::Exception &e){
      XCEPT_RETHROW(emu::daq::rui::exception::Exception, "Failed to get run information from emu::daq::ta::Application", e);
    }

  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find emu::daq::ta::Application. ==> Run number, start time, and maximum number of events are unknown.");
    stringstream ss8;
    ss8 <<  "Did not find emu::daq::ta::Application. ==> Run number, start time, and maximum number of events are unknown.";
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss8.str() );
    this->notifyQualified( "error", eObj );
  }

  taDescriptors.clear();

}

string emu::daq::rui::Application::createRuiRuPoolName(const uint32_t emuRUIInstance)
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
    catch (xcept::Exception &e)
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
    catch(xcept::Exception &e)
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
            &emu::daq::rui::Application::noAction);
        fsm_.addStateTransition('R', 'R', "Configure", this,
            &emu::daq::rui::Application::noAction);
        fsm_.addStateTransition('E', 'E', "Enable"   , this,
            &emu::daq::rui::Application::noAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &emu::daq::rui::Application::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &emu::daq::rui::Application::haltAction);

        fsm_.addStateTransition('H', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
        fsm_.addStateTransition('R', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
        fsm_.addStateTransition('E', 'F', "Fail", this, &emu::daq::rui::Application::failAction);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &emu::daq::rui::Application::failAction);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &emu::daq::rui::Application::stateChanged);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::rui::exception::Exception,
            "Failed to reset FSM", e);
    }
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
    fileSizeInMegaBytes_ = 2;
    params.push_back(pair<string,xdata::Serializable *>
		     ("pathToRUIDataOutFile"  , &pathToDataOutFile_   ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("ruiFileSizeInMegaBytes", &fileSizeInMegaBytes_ ));
    passDataOnToRUBuilder_ = true;
    params.push_back(pair<string,xdata::Serializable *>
		     ("passDataOnToRUBuilder", &passDataOnToRUBuilder_));

    runNumber_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
		     ("runNumber", &runNumber_));
    runType_ = "";
    params.push_back(pair<string,xdata::Serializable *>
		     ("runType", &runType_));

    fileWritingRateLimitInHz_  =  2000;
    fileWritingRateSampleSize_ = 10000;
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingRateLimitInHz", &fileWritingRateLimitInHz_ ));
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingRateSampleSize", &fileWritingRateSampleSize_ ));

    writeBadEventsOnly_     = true;
    nToWriteBeforeBadEvent_ = 10;
    nToWriteAfterBadEvent_  = 100;
    params.push_back( pair<string,xdata::Serializable *>( "writeBadEventsOnly"    , &writeBadEventsOnly_     ) );
    params.push_back( pair<string,xdata::Serializable *>( "nToWriteBeforeBadEvent", &nToWriteBeforeBadEvent_ ) );
    params.push_back( pair<string,xdata::Serializable *>( "nToWriteAfterBadEvent" , &nToWriteAfterBadEvent_  ) );

    for( size_t iClient=0; iClient<maxClients_; ++iClient ) {
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


    nEventsRead_ = 0;
    params.push_back(pair<string,xdata::Serializable *>("nEventsRead", &nEventsRead_));
    nReadingPasses_ = 0;
    params.push_back(pair<string,xdata::Serializable *>("nReadingPasses", &nReadingPasses_));
    maxNBlocksInEvent_ = 0;
    params.push_back(pair<string,xdata::Serializable *>("maxNBlocksInEvent", &maxNBlocksInEvent_));
    nEventsOfMultipleBlocks_ = 0;
    params.push_back(pair<string,xdata::Serializable *>("nEventsOfMultipleBlocks", &nEventsOfMultipleBlocks_));
    badEventCount_ = 0;
    params.push_back(pair<string,xdata::Serializable *>("badEventCount", &badEventCount_));

    persistentDDUError_ = "";
    params.push_back(pair<string,xdata::Serializable *>("persistentDDUError", &persistentDDUError_));

    dataFileNames_.clear();
    params.push_back(pair<string,xdata::Serializable *>("dataFileNames", &dataFileNames_));

    fileWritingVetoed_ = false;
    params.push_back(pair<string,xdata::Serializable *>
		     ("fileWritingVetoed", &fileWritingVetoed_ ));

    for( size_t iClient=0; iClient<maxClients_; ++iClient ){ 
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

    reasonForFailure_ = "";
    params.push_back(pair<string,xdata::Serializable *>
		     ("reasonForFailure", &reasonForFailure_));
    
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

void emu::daq::rui::Application::attachListeners(){
  appInfoSpace_->addItemRetrieveListener("dataFileNames",this);
}

void emu::daq::rui::Application::stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
throw (toolbox::fsm::exception::Exception)
{
    toolbox::fsm::State state = fsm.getCurrentState();


    try
    {
        stateName_ = fsm.getStateName(state);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to set exported parameter stateName", e);
    }
}


void emu::daq::rui::Application::bindFsmSoapCallbacks()
{
    xoap::bind(this, &emu::daq::rui::Application::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onEnable   , "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onHalt     , "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::rui::Application::onReset    , "Reset"    , XDAQ_NS_URI);
}


xoap::MessageReference emu::daq::rui::Application::onConfigure(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Configure");
    return emu::soap::createMessage( "ConfigureResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Configure' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Configure' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::rui::Application::onEnable(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Enable");
    return emu::soap::createMessage( "EnableResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Enable' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Enable' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::rui::Application::onHalt(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Halt");
    return emu::soap::createMessage( "HaltResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Halt' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Halt' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::rui::Application::onReset(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    fsm_.reset();
    toolbox::Event::Reference evtRef(new toolbox::Event("Halt", this));
    fsm_.fireEvent( evtRef );
    return emu::soap::createMessage( "ResetResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Reset' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Reset' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::rui::Application::onTerminate(xoap::MessageReference msg){
  const int32_t terminationDelay = 5; // seconds to allow log messages to be sent before termination
  std::multimap<std::string, std::string, std::less<std::string> >& allHeaders = msg->getMimeHeaders()->getAllHeaders();
  std::multimap<std::string, std::string, std::less<std::string> >::iterator i;
  std::string user_agent("");
  std::string host("");
  for (i = allHeaders.begin(); i != allHeaders.end(); i++){
    if ( i->first == "user-agent" ) user_agent = i->second;
    if ( i->first ==       "host" )       host = i->second;
  }
  stringstream ss;
  ss << "Received SOAP command from " << user_agent 
     << " on " << host 
     << " to terminate process. Exiting in " << terminationDelay << " seconds.";

  stringstream timerName;
  timerName << "TerminationTimer." << getApplicationDescriptor()->getClassName() 
	    <<                 "." << getApplicationDescriptor()->getInstance();
  try{
    toolbox::task::Timer *timer = toolbox::task::getTimerFactory()->createTimer( timerName.str() );
    toolbox::TimeVal start( toolbox::TimeVal::gettimeofday() + toolbox::TimeVal( terminationDelay, 0 ) );
    timer->schedule( this, start, 0, timerName.str() );
    LOG4CPLUS_FATAL(logger_, ss.str());
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss.str() );
    this->notifyQualified( "fatal", eObj );
  } catch(xcept::Exception& e){
    ss.str() = "";
    ss << "Received SOAP command from " << user_agent 
       << " on " << host
       << ", but failed to create " << timerName.str() 
       << " , therefore this process will not be terminated. ";
    LOG4CPLUS_ERROR( getApplicationLogger(), ss.str()+xcept::stdformat_exception_history(e) );
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj1, ss.str(), e );
    this->notifyQualified( "error", eObj1 );
    XCEPT_RETHROW(xoap::exception::Exception, ss.str(), e);
  }
  
  return emu::soap::createMessage( "TerminateResponse" );
}

void
emu::daq::rui::Application::timeExpired(toolbox::task::TimerEvent& e){
  exit(0);
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
    catch(xcept::Exception &e)
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

void emu::daq::rui::Application::createDeviceReader(){

  // Create readers
  inputDataFormatInt_ = 999999;
  if      ( inputDataFormat_ == "DDU" ) inputDataFormatInt_ = emu::daq::reader::Base::DDU;
  else if ( inputDataFormat_ == "DCC" ) inputDataFormatInt_ = emu::daq::reader::Base::DCC;
  else if ( inputDataFormat_ == "DMB" ) inputDataFormatInt_ = emu::daq::reader::Base::DMB;
  else{
	stringstream oss;
	oss << "No such data format: " << inputDataFormat_.toString() << 
	  "Use \"DDU\" or \"DCC\" or \"DMB\"";
// 	LOG4CPLUS_FATAL(logger_, oss.str());
// 	stringstream ss12;
// 	ss12 <<  oss.str();
// 	XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss12.str() );
// 	this->notifyQualified( "fatal", eObj );
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
//       stringstream ss13;
//       ss13 <<  oss.str();
//       XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss13.str() );
//       this->notifyQualified( "fatal", eObj );
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    try {
      if      ( inputDeviceType_ == "spy"  )
	deviceReader_ = new emu::daq::reader::Spy(  inputDeviceName_.toString(), inputDataFormatInt_, false );
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
//       stringstream ss14;
//       ss14 <<  oss.str();
//       XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss14.str() );
//       this->notifyQualified( "fatal", eObj );
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    catch(...){
      stringstream oss;
      oss << "Failed to create " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
//       LOG4CPLUS_FATAL(logger_, oss.str());
//       stringstream ss15;
//       ss15 <<  oss.str();
//       XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss15.str() );
//       this->notifyQualified( "fatal", eObj );
      XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_)
      if ( deviceReader_->getLogMessage().length() > 0 )
	LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());

}


void emu::daq::rui::Application::destroyServers(){
  std::vector<Client*>::iterator c;
  for ( c=clients_.begin(); c!=clients_.end(); ++c ){
    LOG4CPLUS_INFO(logger_, string("Destroying server for ") + (*c)->server->getClientName() );
    delete (*c)->server;
    *(*c)->creditsHeld = 0;
  }
  clients_.clear();
}

bool emu::daq::rui::Application::createI2OServer( string clientName, uint32_t clientInstance  ){
  bool created = false;
  size_t iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>            ( clientName_.elementAt( iClient )     )) = clientName;
    *(dynamic_cast<xdata::UnsignedInteger32*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>            ( clientProtocol_.elementAt( iClient ) )) = "I2O";
    *(dynamic_cast<xdata::Boolean*>           ( clientPersists_.elementAt( iClient ) )) = true;
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

bool emu::daq::rui::Application::createSOAPServer( string clientName,  uint32_t clientInstance, bool persistent ){
  bool created = false;
  size_t iClient = clients_.size();
  if ( iClient < maxClients_ ){
    *(dynamic_cast<xdata::String*>          (     clientName_.elementAt( iClient ) )) = clientName;
    *(dynamic_cast<xdata::UnsignedInteger*> ( clientInstance_.elementAt( iClient ) )) = clientInstance;
    *(dynamic_cast<xdata::String*>          ( clientProtocol_.elementAt( iClient ) )) = "SOAP";
    *(dynamic_cast<xdata::Boolean*>         ( clientPersists_.elementAt( iClient ) )) = persistent;
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
  else{
    LOG4CPLUS_WARN(logger_, 
		   "Maximum number of clients exceeded. Cannot create server for " << clientName );
    stringstream ss21;
    ss21 <<  
		   "Maximum number of clients exceeded. Cannot create server for " << clientName ;
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss21.str() );
    this->notifyQualified( "warning", eObj );
  }
  return created;
}


void emu::daq::rui::Application::createServers(){
  for ( size_t iClient=0; iClient<clientName_.elements(); ++iClient ){
    xdata::Boolean *persists = dynamic_cast<xdata::Boolean*>( clientPersists_.elementAt(iClient) );
    // (Re)create it only if it has a name and is not a temporary server created on the fly
    if ( clientName_.elementAt(iClient)->toString() != "" && persists->value_ ){
      uint32_t clientInstance = 
	(dynamic_cast<xdata::UnsignedInteger32*> ( clientInstance_.elementAt( iClient ) ))->value_;
      LOG4CPLUS_INFO(logger_,
		     clientName_.elementAt(iClient)->toString() << clientInstance << 
		     "\'s server being created" );
      if ( clientProtocol_.elementAt(iClient)->toString() == "I2O" )
	createI2OServer( clientName_.elementAt(iClient)->toString(), clientInstance );
      else if ( clientProtocol_.elementAt(iClient)->toString() == "SOAP" )
	createSOAPServer( clientName_.elementAt(iClient)->toString(), clientInstance );
      else{
	LOG4CPLUS_ERROR(logger_, "Unknown protocol \"" <<
			clientProtocol_.elementAt(iClient)->toString() << 
			"\" for client " <<
			clientName_.elementAt(iClient)->toString() << 
			" instance " <<
			clientInstance <<
			". Please use \"I2O\" or \"SOAP\".");
	stringstream ss22;
	ss22 <<  "Unknown protocol \"" <<
			clientProtocol_.elementAt(iClient)->toString() << 
			"\" for client " <<
			clientName_.elementAt(iClient)->toString() << 
			" instance " <<
			clientInstance <<
			". Please use \"I2O\" or \"SOAP\".";
	XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss22.str() );
	this->notifyQualified( "error", eObj );
      }
    }
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
  catch(emu::daq::rui::exception::Exception &e){
    LOG4CPLUS_WARN(logger_, "Failed to get atcp descriptors : " 
		   + xcept::stdformat_exception_history(e) );
    stringstream ss23;
    ss23 <<  "Failed to get atcp descriptors : " 
		    ;
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss23.str(), e );
    this->notifyQualified( "warning", eObj );
    atcpDescriptors.clear();
  }

  //   std::cout << atcpDescriptors.size() << " atcpDescriptors" << std::endl;

  emu::soap::Messenger m( this );

  vector < xdaq::ApplicationDescriptor* >::const_iterator atcpd;
  for ( atcpd = atcpDescriptors.begin(); atcpd != atcpDescriptors.end(); ++atcpd ){

    //     std::cout << appDescriptor_->getContextDescriptor()->getURL() << "         "
    // 	      << (*atcpd)->getContextDescriptor()->getURL() << std::endl;

    // Handle only the pt::atcp::PeerTransportATCP that's in the same Context
    if ( (*atcpd)->getContextDescriptor() == appDescriptor_->getContextDescriptor() ){

      // ATCP may already have been started. Check its state.
      xdata::String atcpState;
      try{
      	m.getParameters( *atcpd, emu::soap::Parameters().add( "stateName", &atcpState ) );
      }
      catch(xcept::Exception &e)
      	{
      	  stringstream oss;
      	  oss << "Failed to get state of " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
      	  XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
      	}

      if ( atcpState.toString() != "Halted" ) continue;

      // Configure ATCP
      try{
	m.sendCommand( *atcpd, "Configure" );
      }
      catch(xcept::Exception &e){
	stringstream oss;
	oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
      }

      // Enable ATCP
      try{
	m.sendCommand( *atcpd, "Enable" );
      }
      catch(xcept::Exception &e){
	stringstream oss;
	oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::rui::exception::Exception, oss.str(), e);
      }

    }

  }
  
}

void emu::daq::rui::Application::noAction(toolbox::Event::Reference e)
  throw (toolbox::fsm::exception::Exception)
{
  // Inaction...
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() 
		 << " attempted when already " 
		 << fsm_.getStateName(fsm_.getCurrentState()));
}

void emu::daq::rui::Application::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
  logMessageCounter_.reset();

  destroyFileWriter();

  // ATCP must be started explicitly
  try{
    startATCP();
  }
  catch(xcept::Exception &e){
    XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to start ATCP ", e);
  }

    try
    {
        tid_ = i2oAddressMap_->getTid(appDescriptor_);
    }
    catch(xcept::Exception &e)
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
    catch(xcept::Exception &e)
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
    } catch( xcept::Exception &e ) {
      LOG4CPLUS_WARN(logger_,"Failed to get i2o target id of emu::daq::ta::Application: " +  xcept::stdformat_exception_history(e));
      stringstream ss24;
      ss24 << "Failed to get i2o target id of emu::daq::ta::Application: " ;
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss24.str(), e );
      this->notifyQualified( "warning", eObj );
    }

    nReadingPasses_          = 0;
    maxNBlocksInEvent_       = 0;
    nEventsOfMultipleBlocks_ = 0;

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

    // Managing bad events and their context
    if ( writeBadEventsOnly_.value_ ){
      LOG4CPLUS_INFO(logger_, "Writing bad events only and " << nToWriteBeforeBadEvent_.toString() << " events before and " << nToWriteAfterBadEvent_.toString() << " after.");
      // Set the size of the buffer ring storing the events preceding the bad one
      eventBufferRing_.setSize( nToWriteBeforeBadEvent_.value_ );
      eventBufferRing_.emptyEventBuffers();
      // A countSinceBadEvent_ below nToWriteAfterBadEvent_ would trigger writing a trailing context to file.
      countSinceBadEvent_ = nToWriteAfterBadEvent_.value_+1;
    }
    badEventCount_ = 0;
}


void emu::daq::rui::Application::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{

    try{
      getRunInfo();
    }
    catch(emu::daq::rui::exception::Exception &e){
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
        catch(xcept::Exception &e)
        {
            string s = "Failed to get work loop : " + workLoopName_.toString();

            XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
        }

        try
        {
            workLoop_->submit(workLoopActionSignature_);
        }
        catch(xcept::Exception &e)
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
            catch(xcept::Exception &e)
            {
                string s = "Failed to active work loop : " +
                           workLoopName_.toString();

                XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
            }
        }

        workLoopStarted_ = true;
    }

    // server loops
    for ( size_t iClient=0; iClient<clients_.size(); ++iClient ){
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
	      catch(xcept::Exception &e)
		{
		  string s = "Failed to get work loop : " + clients_[iClient]->workLoopName;

		  XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
		}

	      try
		{
		  clients_[iClient]->workLoop->submit(clients_[iClient]->workLoopActionSignature);
		}
	      catch(xcept::Exception &e)
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
		  catch(xcept::Exception &e)
		    {
		      string s = "Failed to active work loop : " +
			clients_[iClient]->workLoopName;

		      XCEPT_RETHROW(toolbox::fsm::exception::Exception, s, e);
		    }
		}

	      clients_[iClient]->workLoopStarted = true;
	    } // if( ! clients_[iClient]->workLoopStarted )
	} // if ( clientProtocol_.elementAt( iClient )->toString() == "SOAP" )
    } // for ( size_t iClient=0; iClient<clients_.size(); ++iClient )

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
      emu::soap::Messenger( this ).getParameters( taDescriptors_[0], emu::soap::Parameters().add( "runStopTime", &runStopTime_ ) );
      LOG4CPLUS_INFO(logger_, "Got run stop time from emu::daq::ta::Application: " << runStopTime_.toString() );
    }
    catch( xcept::Exception &e ){
      LOG4CPLUS_WARN(logger_, "Run stop time will be unknown: " << xcept::stdformat_exception_history(e) );
      stringstream ss25;
      ss25 <<  "Run stop time will be unknown: "  ;
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss25.str(), e );
      this->notifyQualified( "warning", eObj );
    }

    // Close data file
    if ( fileWriter_ ){
      fileWriter_->endRun( runStopTime_.toString() );
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
      if ( typeid(*event) == typeid(toolbox::fsm::FailedEvent) ){
        toolbox::fsm::FailedEvent &failedEvent =
            dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
        xcept::Exception exception = failedEvent.getException();

        stringstream oss;
        oss << "Failure occurred when performing transition from "
	    << failedEvent.getFromState() << " to " << failedEvent.getToState()
	    << "; Exception history: " << xcept::stdformat_exception_history(exception);

	reasonForFailure_ = oss.str();

	applicationBSem_.give();

        LOG4CPLUS_FATAL(logger_, oss.str() );
        XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, oss.str() );
        this->notifyQualified( "fatal", eObj );
      }
    }catch( xcept::Exception& e ){
      stringstream ss27;
      ss27 <<  "Caught exception while moving to Failed state: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }catch( std::exception& e ){
      stringstream ss27;
      ss27 <<  "Caught exception while moving to Failed state: " << e.what();
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }catch(...){
      stringstream ss27;
      ss27 <<  "Caught an unknown exception while moving to Failed state.";
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }

    sendFacts();
}

void emu::daq::rui::Application::bindI2oCallbacks()
{

  i2o::bind(this, &emu::daq::rui::Application::onI2OClientCreditMsg, I2O_EMUCLIENT_CODE, XDAQ_ORGANIZATION_ID );

}

void emu::daq::rui::Application::moveToFailedState( const string reason ){
  // Use this from inside the work loop to force the FSM to Failed state 

  try
    {
      // Move to the failed state
      toolbox::Event::Reference evtRef(new toolbox::Event("Fail", this));
      reasonForFailure_ = reason;
      fsm_.fireEvent(evtRef);
      applicationBSem_.give();
    }
  catch(xcept::Exception &e)
    {
      applicationBSem_.give();
      
      LOG4CPLUS_FATAL(logger_,
		      "Failed to move to the Failed state : "
		      << xcept::stdformat_exception_history(e));
      stringstream ss28;
      ss28 << "Failed to move to the Failed state : ";
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss28.str(), e );
      this->notifyQualified( "fatal", eObj );
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
  // Normally, parameter "datafilenames" is only updated when retrieved by a SOAP query. Update it now explicitly:
  updateDataFileNames();

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
    catch(xcept::Exception &e)
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
    catch(xcept::Exception &e)
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
  bool isToBeRescheduled    = true;
  int32_t  pauseForOtherThreads = 0;
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

      // Read out data (and pass it on to RU if events are built):
      try {
	pauseForOtherThreads = processAndCommunicate();
	isToBeRescheduled    = ( pauseForOtherThreads >= 0 );
      }
      catch (xcept::Exception &e) {
	LOG4CPLUS_FATAL(logger_, "Failed to execute \"self-driven\" behaviour"
			<< " : " << xcept::stdformat_exception_history(e));
	stringstream ss29;
	ss29 <<  "Failed to execute \"self-driven\" behaviour"
			<< " : " ;
	XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss29.str(), e );
	this->notifyQualified( "fatal", eObj );
	    
	try {
	  // Move to the failed state
	  toolbox::Event::Reference evtRef(new toolbox::Event("Fail", this));
	  fsm_.fireEvent(evtRef);
	  applicationBSem_.give();
	}
	catch(xcept::Exception &e) {
	  applicationBSem_.give();
	  LOG4CPLUS_FATAL(logger_, "Failed to move to the Failed state : "
			  << xcept::stdformat_exception_history(e));
	  stringstream ss30;
	  ss30 <<  "Failed to move to the Failed state : "
			  ;
	  XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss30.str(), e );
	  this->notifyQualified( "fatal", eObj );
	}
	// Do not reschedule this action code as the application has failed
	applicationBSem_.give();
	return false;
      }

      // Pass data on to clients
      // Run the servers too in the readout thread.
      for ( size_t iClient=0; iClient<clients_.size(); ++iClient ){
	// Service only I2O clients here in the readout loop as I2O messages
	// are non-blocking (fire & forget).
	if ( clientProtocol_.elementAt( iClient )->toString() == "I2O" ){
	  try {
	    clients_[iClient]->server->sendData();
	  }
	  catch(xcept::Exception &e) {
	    stringstream msg;
	    msg << "emu::daq::rui::Application" << instance_ << " failed to send data to its client via I2O: ";
	    if ( logMessageCounter_.isToBeLogged( msg.str() ) ){
	      stringstream ss;
	      ss << "[Occurrence " << logMessageCounter_.getCount( msg.str() ) << "] " << msg.str();
	      LOG4CPLUS_WARN(logger_, ss.str() << xcept::stdformat_exception_history(e) );
	      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss.str(), e );
	      this->notifyQualified( "warning", eObj );
	    }
	  }
	}
      }

      break;
    default:
      // Should never get here
      LOG4CPLUS_FATAL(logger_,
		      "emu::daq::rui::Application" << instance_ << " is in an undefined state");
      stringstream ss32;
      ss32 << 
		      "emu::daq::rui::Application" << instance_ << " is in an undefined state";
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss32.str() );
      this->notifyQualified( "fatal", eObj );
    }

  applicationBSem_.give();

  if ( pauseForOtherThreads > 0 ) usleep( (uint32_t) pauseForOtherThreads );

  // Reschedule this action code
  return isToBeRescheduled;
}

bool emu::daq::rui::Application::serverLoopAction(toolbox::task::WorkLoop *wl)
{
    try
    {
      int32_t  pauseForOtherThreads = 0;

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
	  for ( size_t iClient=0; iClient<clients_.size(); ++iClient ){
	    if ( clients_[iClient]->workLoop == wl ){
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
            stringstream ss33;
            ss33 << 
                "emu::daq::rui::Application" << instance_ << " is in an undefined state";
            XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss33.str() );
            this->notifyQualified( "error", eObj );
        }

	applicationBSem_.give();

	if ( pauseForOtherThreads > 0 ) usleep( (uint32_t) pauseForOtherThreads );

    }
    catch(xcept::Exception &e)
    {
	applicationBSem_.give();

        LOG4CPLUS_WARN(logger_,
            "Failed to execute \"self-driven\" behaviour"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss34;
        ss34 << 
            "Failed to execute \"self-driven\" behaviour"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss34.str(), e );
        this->notifyQualified( "warning", eObj );
    }

    // Reschedule this action code
    return true;
}


void emu::daq::rui::Application::addDataForClients( const uint32_t runNumber, 
						    const uint32_t runStartUTC,
						    const uint64_t nEventsRead,
						    const emu::daq::server::PositionInEvent_t position,
						    const uint16_t errorFlag, 
						    char* const data, 
						    const size_t dataLength ){
  for ( size_t iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->addData( runNumber, runStartUTC, nEventsRead, position, 
					errorFlag, data, dataLength );
}

void emu::daq::rui::Application::makeClientsLastBlockEndEvent(){
  for ( size_t iClient=0; iClient<clients_.size(); ++iClient )
    clients_[iClient]->server->makeLastBlockEndEvent();
}


int32_t emu::daq::rui::Application::processAndCommunicate()
{
  int32_t pauseForOtherThreads = 0;

    if( blocksArePendingTransmission_ )
    {
        try
        {
            sendNextPendingBlock();
        }
        catch(xcept::Exception &e)
        {
            LOG4CPLUS_WARN(logger_,
                "Failed to send data block to RU" << instance_ << "."
                << "Will try again later");
            stringstream ss35;
            ss35 << 
                "Failed to send data block to RU" << instance_ << "."
                << "Will try again later";
            XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss35.str() );
            this->notifyQualified( "warning", eObj );
        }
    }
    else
    {
        try
        {
	  if ( isSTEPRun_ ) pauseForOtherThreads = continueSTEPRun();
	  else              pauseForOtherThreads = continueConstructionOfSuperFrag();
        }
        catch(xcept::Exception &e)
        {
            LOG4CPLUS_ERROR(logger_,
                "Failed to contnue construction of super-fragment"
                << " : " << stdformat_exception_history(e));
            stringstream ss36;
            ss36 << 
                "Failed to contnue construction of super-fragment"
                << " : " ;
            XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss36.str(), e );
            this->notifyQualified( "error", eObj );
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
    catch(xcept::Exception &e)
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

void emu::daq::rui::Application::destroyFileWriter(){
  if ( fileWriter_ ){
    fileWriter_->endRun();
    delete fileWriter_;
    fileWriter_ = NULL;
  }
  // Destroy rate limiter, too, if any.
  if ( rateLimiter_ ){
    delete rateLimiter_;
    rateLimiter_ = NULL;
  }
}

void emu::daq::rui::Application::createFileWriter(){
  // Just in case there's a writer, terminate it in an orderly fashion
  destroyFileWriter();

  // create new writers if path is not empty
  if ( pathToDataOutFile_ != string("") && 
       (xdata::UnsignedInteger64) fileSizeInMegaBytes_ > (uint64_t) 0 )
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
      stringstream ss39;
      ss39 <<  "A calibration run or a STEP run has been started without specifying a directory and/or maximum size for data files. Please set \"pathToRUIDataOutFile\" and \"ruiFileSizeInMegaBytes\" to nonzero values in the XML configuration file." ;
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss39.str() );
      this->notifyQualified( "fatal", eObj );
      moveToFailedState( ss39.str() );
    }

  // inform the file writer about the new run
  try{
    if ( fileWriter_ ) fileWriter_->startNewRun( runNumber_.value_, 
						 isBookedRunNumber_.value_,
						 runStartTime_, 
						 ( writeBadEventsOnly_.value_ ? "BadEvents" : runType_ ) );
  }
  catch(string e){
    LOG4CPLUS_FATAL( logger_, e );
    stringstream ss40;
    ss40 <<  e ;
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss40.str() );
    this->notifyQualified( "fatal", eObj );
    moveToFailedState( ss40.str() );
  }
}

void emu::daq::rui::Application::writeDataToFile( const char* const data, const size_t dataLength, const bool newEvent ){
  if ( fileWriter_ ){
    try{
      if ( newEvent ){
	fileWriter_->startNewEvent();
	if ( rateLimiter_ ) fileWritingVetoed_ = ! rateLimiter_->acceptEvent();
      }
      if ( ! fileWritingVetoed_.value_ ){
	fileWriter_->writeData( data, dataLength );
	ableToWriteToDisk_ = true;
      }
    } catch(string e) {
      stringstream ss;
      ss <<  e ;
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss.str() );
      // Move to failed state unless we're writing bad events only:
      if ( writeBadEventsOnly_.value_ ){
	// Cry only once, the first time:
	if ( ableToWriteToDisk_ ){
	  LOG4CPLUS_ERROR( logger_, e );
	  this->notifyQualified( "error", eObj );
	  ableToWriteToDisk_ = false;
	}
      }
      else{
	LOG4CPLUS_FATAL( logger_, e );
	this->notifyQualified( "fatal", eObj );
	moveToFailedState( ss.str() );
      }
    }
  }
}

void emu::daq::rui::Application::writeDataWithContextToFile(  char* const data, const size_t dataLength, const bool newEvent ){
  if ( writeBadEventsOnly_.value_ ){
    // We're to write bad events only (and their context).
    // Add data to the buffer ring:
    if ( countSinceBadEvent_ <= nToWriteAfterBadEvent_ ){
      // We're still writing the trailing context of the bad event.
      // Write this data block to file unconditionally:
      writeDataToFile( data, dataLength, newEvent );
      // Increment the counter of events read since the last bad event if this data block belongs to a new event:
      if ( newEvent ) countSinceBadEvent_++;
      //LOG4CPLUS_WARN(logger_, "Writing bad event trailing context, event +" << countSinceBadEvent_ );
    } // if ( countSinceBadEvent_ <= nToWriteAfterBadEvent_ )
    else{
      // We're no longer writing the trailing context.
      // Store every event in the buffer as they may need to be written as the leading context of a bad event to come:
      eventBufferRing_.addData( dataLength, data, newEvent );
      if ( isBadEvent_ ){
	// LOG4CPLUS_INFO(logger_, "Found bad event.");
	// Turns out this is a bad event.
	// (isBadEvent_ can only be set TRUE once the trailer has been read in, i.e., the event is complete.)
	++badEventCount_;
	// Write this bad event to file if and only if it passes prescaling:
	if ( badEventCount_.passesPrescaling() ){
	  // Get the bad event and its preceding events and write them to file as the leading context:
	  list<const emu::daq::rui::EventBuffer*> ebl( eventBufferRing_.getEventBuffers() );
	  for ( list<const emu::daq::rui::EventBuffer*>::const_iterator i=ebl.begin(); i!=ebl.end(); ++i ){
	    writeDataToFile( (*i)->getEvent(), (*i)->getEventSize(), true );
	    //LOG4CPLUS_WARN(logger_, "Writing bad event leading context, event -" << --countToBadEvent );
	  }
	  // Zero the counter of events read since the last bad event:
	  countSinceBadEvent_ = 0;
	} // if ( badEventCount_.passesPrescaling() )
	// Clear the buffer holding the bad event and the ones preceding it:
	eventBufferRing_.emptyEventBuffers();
      } // if ( isBadEvent_ )
    } // if ( countSinceBadEvent_ <= nToWriteAfterBadEvent_ ) else
  } // if ( writeBadEventsOnly_.value_ )
  else{
    // We're to write all events unconditionally.
    writeDataToFile( data, dataLength, newEvent );
  } // if ( writeBadEventsOnly_.value_ ) else
}

int32_t emu::daq::rui::Application::continueConstructionOfSuperFrag()
  throw (emu::daq::rui::exception::Exception){

  // Possible return values
  const int32_t extraPauseForOtherThreads   = 5000; // [microsecond]
  const int32_t noExtraPauseForOtherThreads = 0;    // [microsecond]
  const int32_t notToBeRescheduled          = -1;

  uint64_t nBytesRead = 0;

  if ( maxEvents_.value_ >= 0 && nEventsRead_.value_ >= (uint64_t) maxEvents_.value_ ) 
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
  if ( (xdata::UnsignedInteger64) nEventsRead_ == (uint64_t) 0 && ! deviceReader_->isResetAndEnabled() ){
    try{
      deviceReader_->resetAndEnable();
    }
    catch(std::runtime_error e){

      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": "                << e.what();
      LOG4CPLUS_FATAL(logger_, oss.str());
      stringstream ss43;
      ss43 <<  oss.str();
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss43.str() );
      this->notifyQualified( "fatal", eObj );
      moveToFailedState( ss43.str() );
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }
    catch(...){
      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
      LOG4CPLUS_FATAL(logger_, oss.str());
      stringstream ss44;
      ss44 <<  oss.str();
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss44.str() );
      this->notifyQualified( "fatal", eObj );
      moveToFailedState( ss44.str() );
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_->getLogMessage().length() > 0 )
      LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());
  } // if ( nEventsRead_ == 0 )

    // See if there's something to read and then read it:
  try{
    nBytesRead = deviceReader_->readNextEvent();
  }
  catch(...){
    stringstream oss;
    oss << "Failed to read from " << inputDeviceName_.toString()
	<< ": unknown exception.";
    LOG4CPLUS_ERROR(logger_, oss.str());
    stringstream ss45;
    ss45 <<  oss.str();
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss45.str() );
    this->notifyQualified( "error", eObj );
  }

  // cout << "================================"
  //      << "\nTime " << emu::utils::getDateTime()
  //      << "   Data length " << deviceReader_->dataLength() << "   Bytes read " << nBytesRead
  //      << endl;
  // if ( deviceReader_->dataLength() > 0 ){
  //   cout << "Error flag   " << deviceReader_->getErrorFlag()
  // 	 << "\nEvent number " << deviceReader_->eventNumber()
  // 	 << "\nLog message  " << deviceReader_->getLogMessage() << "\n";
  //   printData( cout, deviceReader_->data(), deviceReader_->dataLength() );
  // }

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

  errorFlag_ = deviceReader_->getErrorFlag();

  if ( nBytesRead < 8 ){
    LOG4CPLUS_ERROR(logger_, 
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    " read " << nBytesRead << " bytes only.");
    stringstream ss46;
    ss46 <<  
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    " read " << nBytesRead << " bytes only.";
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss46.str() );
    this->notifyQualified( "error", eObj );
  }

  nReadingPassesInEvent_++;
  nReadingPasses_++;

  if ( (xdata::UnsignedInteger64) nEventsRead_ == (uint64_t) 0 && nReadingPassesInEvent_ == 1 ) {
    // first event started --> a new run

    insideEvent_ = false;
    createFileWriter();

    // inform emuTA about the L1A number of the first event read
    try{
      sendEventNumberToTA( deviceReader_->eventNumber() );
    } catch( xcept::Exception &e ) {
      LOG4CPLUS_WARN(logger_,"Failed to inform emuTA about the L1A number of the first event read: " +  xcept::stdformat_exception_history(e));
      stringstream ss47;
      ss47 << "Failed to inform emuTA about the L1A number of the first event read: " ;
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss47.str(), e );
      this->notifyQualified( "warning", eObj );
    }

  }

  char* data;
  size_t dataLength  = 0;

  data = deviceReader_->data();

  if ( data!=NULL ){

    isBadEvent_ = false;

    dataLength = deviceReader_->dataLength();
    if ( dataLength>0 ) {

      bool header  = hasHeader(data,dataLength);
      bool trailer = hasTrailer(data,dataLength);

      // Position of data in the event:
      emu::daq::server::PositionInEvent_t positionInEvent = emu::daq::server::continuesEvent; // It's somewhere inside unless...
      if ( header ){
	positionInEvent = (emu::daq::server::PositionInEvent_t)( positionInEvent | emu::daq::server::startsEvent ); // ...it's at the beginning...
      }
      if ( trailer ){
	positionInEvent = (emu::daq::server::PositionInEvent_t)( positionInEvent | emu::daq::server::endsEvent );   // ...and/or at the end.
      }

      // Update errorFlag:
      if ( !header  ) errorFlag_ |= emu::daq::reader::Spy::HeaderMissing;
      if ( !trailer ) errorFlag_ |= emu::daq::reader::Spy::TrailerMissing;

      if ( trailer && inputDataFormatInt_ == emu::daq::reader::Base::DDU ) isBadEvent_ = interestingDDUErrorBitPattern(data,dataLength);

      stringstream ss;
      ss << "Inside event: " << insideEvent_
	 << " Last L1A: " << eventNumber_
	 << " N read: " << nEventsRead_.toString()
	 << " This L1A: " << deviceReader_->eventNumber()
	 << " Length: " << dataLength
	 << " Header: " << header
	 << " Trailer: " << trailer;
      if ( inputDeviceType_ == "spy"  ){
	ss << " Packets: " << ( ( errorFlag_ && 0x0F00 ) >> 8 );
	if ( errorFlag_ & 0x00ff ){
	  ss << " Errors: "
	     << (errorFlag_ & emu::daq::reader::Spy::EndOfEventMissing ? "EndOfEventMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Timeout ? "Timeout " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::PacketsMissing ? "PacketsMissing " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::LoopOverwrite ? "LoopOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::BufferOverwrite ? "BufferOverwrite " : "" )
	     << (errorFlag_ & emu::daq::reader::Spy::Oversized ? "Oversized" : "" );
	//   LOG4CPLUS_WARN(logger_, ss.str());
	//   stringstream ss48;
	//   ss48 <<  ss.str();
	//   XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss48.str() );
	//   this->notifyQualified( "warning", eObj );
 	}
	// else{
	  LOG4CPLUS_INFO(logger_, ss.str());
	// }
      }

      // ss << "\n"; printData(ss,data,dataLength); cout << ss.str() << endl;

      if ( insideEvent_ ) {

	writeDataWithContextToFile( data, dataLength, header );

	// Count events that it took more than one pass to read out (and will thus be sent on in more than one block)
	if ( nReadingPassesInEvent_ > maxNBlocksInEvent_.value_ ) maxNBlocksInEvent_ = nReadingPassesInEvent_;
	if ( nReadingPassesInEvent_ > 1 ) nEventsOfMultipleBlocks_++;

	if ( header ){
	  // LOG4CPLUS_WARN(logger_, 
	  // 		  "No trailer in event " << eventNumber_ << 
	  // 		  " ("             << nEventsRead_ <<
	  // 		  " so far) from " << inputDeviceName_.toString() <<
	  // 		  ", size: "       << dataLength );

	  // Prepare the old block(s) to be sent out. 
	  // They will be assumed to belong to the previous known event number.
	  if ( passDataOnToRUBuilder_.value_ ) finalizeSuperFragment();
	  previousEventNumber_ = eventNumber_;
	  nEventsRead_++;
	  // Mark the last block for clients
	  makeClientsLastBlockEndEvent();
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
	    stringstream ss50;
	    ss50 <<  "Event fragment dropped. (EmuRUI-to-RU memory pool's high threshold exceeded.)";
	    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss50.str() );
	    this->notifyQualified( "warning", eObj );
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

	// if ( !header && trailer ){
	//   LOG4CPLUS_WARN(logger_, 
	// 		  "No header in event " << eventNumber_ << 
	// 		  " ("             << nEventsRead_ <<
	// 		  " so far) from " << inputDeviceName_.toString() <<
	// 		  ", size: "       << dataLength );
	// }

	writeDataWithContextToFile( data, dataLength, header || trailer );

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
	    stringstream ss52;
	    ss52 <<  "Event fragment dropped. (EmuRUI-to-RU memory pool's high threshold exceeded.)";
	    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss52.str() );
	    this->notifyQualified( "warning", eObj );
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
      addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );

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

int32_t emu::daq::rui::Application::continueSTEPRun()
  throw (emu::daq::rui::exception::Exception){

  // Possible return values
  const int32_t extraPauseForOtherThreads   = 5000; // [microsecond]
  const int32_t noExtraPauseForOtherThreads = 0;    // [microsecond]
  const int32_t notToBeRescheduled          = -1;

  uint64_t nBytesRead = 0;

  if ( maxEvents_.value_ >= 0 && STEPEventCounter_.getLowestCount() >= (uint64_t) maxEvents_.value_ )
    return notToBeRescheduled;

  if (deviceReader_ == NULL) return notToBeRescheduled;

  // Prepare to read the first event if we have not yet done so:
  if ( (xdata::UnsignedInteger64) nEventsRead_ == (uint64_t) 0 && ! deviceReader_->isResetAndEnabled() ){
    try{
      deviceReader_->resetAndEnable();
    }
    catch(std::runtime_error e){

      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": "                << e.what();
      LOG4CPLUS_FATAL(logger_, oss.str());
      stringstream ss53;
      ss53 <<  oss.str();
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss53.str() );
      this->notifyQualified( "fatal", eObj );
      moveToFailedState( ss53.str() );
    }
    catch(...){
      stringstream oss;
      oss << "Failed to reset and/or enable " << inputDeviceType_.toString()
	  << " reader for "      << inputDeviceName_.toString()
	  << ": unknown exception.";
      LOG4CPLUS_FATAL(logger_, oss.str());
      stringstream ss54;
      ss54 <<  oss.str();
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss54.str() );
      this->notifyQualified( "fatal", eObj );
      moveToFailedState( ss54.str() );
      // 	XCEPT_RAISE(toolbox::fsm::exception::Exception, oss.str());
    }

    if ( deviceReader_->getLogMessage().length() > 0 )
      LOG4CPLUS_INFO(logger_, deviceReader_->getLogMessage());
  } // if ( nEventsRead_ == 0 )

    // See if there's something to read and then read it:
  try{
    nBytesRead = deviceReader_->readNextEvent();
  }
  catch(...){
    stringstream oss;
    oss << "Failed to read from " << inputDeviceName_.toString()
	<< ": unknown exception.";
    LOG4CPLUS_ERROR(logger_, oss.str());
    stringstream ss55;
    ss55 <<  oss.str();
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss55.str() );
    this->notifyQualified( "error", eObj );
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
    stringstream ss56;
    ss56 <<  
		    " " << inputDataFormat_.toString() << inputDeviceType_.toString() << 
		    " read " << nBytesRead << " bytes only.";
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss56.str() );
    this->notifyQualified( "error", eObj );
  }

  nReadingPassesInEvent_++;

  if ( (xdata::UnsignedInteger64) nEventsRead_ == (uint64_t) 0 && nReadingPassesInEvent_ == 1 ) {
    // first event started --> a new run

    insideEvent_ = false;
    createFileWriter();

    // inform emuTA about the L1A number of the first event read
    try{
      sendEventNumberToTA( deviceReader_->eventNumber() );
    } catch( xcept::Exception &e ) {
      LOG4CPLUS_WARN(logger_,"Failed to inform emuTA about the L1A number of the first event read: " +  xcept::stdformat_exception_history(e));
      stringstream ss57;
      ss57 << "Failed to inform emuTA about the L1A number of the first event read: " ;
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss57.str(), e );
      this->notifyQualified( "warning", eObj );
    }

  }

  char* data;
  size_t dataLength  = 0;

  data = deviceReader_->data();

  if ( data!=NULL ){

    dataLength = deviceReader_->dataLength();
    if ( dataLength>0 ) {

      bool header  = hasHeader(data,dataLength);
      bool trailer = hasTrailer(data,dataLength);

      // Position of data in the event:
      emu::daq::server::PositionInEvent_t positionInEvent = emu::daq::server::continuesEvent; // It's somewhere inside unless...
      if ( header ){
	positionInEvent = (emu::daq::server::PositionInEvent_t)( positionInEvent | emu::daq::server::startsEvent ); // ...it's at the beginning...
      }
      if ( trailer ){
	positionInEvent = (emu::daq::server::PositionInEvent_t)( positionInEvent | emu::daq::server::endsEvent );   // ...and/or at the end.
      }

      if ( header ) nEventsRead_++;

      if ( trailer && inputDataFormatInt_ == emu::daq::reader::Base::DDU ) interestingDDUErrorBitPattern(data,dataLength);

      if ( header && ! STEPEventCounter_.isInitialized() ){
	uint64_t maxEvents;
	if ( maxEvents_.value_ < 0 ) maxEvents = numeric_limits<uint64_t>::max();
	else                         maxEvents = (uint64_t) maxEvents_.value_;
	STEPEventCounter_.initialize( maxEvents, data );
      }

//       stringstream ss;
//       ss << "Inside event: " << insideEvent_
// 	 << " Last L1A: " << eventNumber_
// 	 << " N read: " << nEventsRead_.toString()
// 	 << " This L1A: " << deviceReader_->eventNumber()
// 	 << " Length: " << dataLength
// 	 << " Header: " << header
// 	 << " Trailer: " << trailer
// 	 << " STEP counts: " << STEPEventCounter_.print();
//       if ( inputDeviceType_ == "spy"  ){
// 	ss << " Packets: " << ( ( errorFlag_ && 0x0F00 ) >> 8 );
// 	if ( errorFlag_ & 0x00ff ){
// 	  ss << " Errors: "
// 	     << (errorFlag_ & emu::daq::reader::Spy::EndOfEventMissing ? "EndOfEventMissing " : "" )
// 	     << (errorFlag_ & emu::daq::reader::Spy::Timeout ? "Timeout " : "" )
// 	     << (errorFlag_ & emu::daq::reader::Spy::PacketsMissing ? "PacketsMissing " : "" )
// 	     << (errorFlag_ & emu::daq::reader::Spy::LoopOverwrite ? "LoopOverwrite " : "" )
// 	     << (errorFlag_ & emu::daq::reader::Spy::BufferOverwrite ? "BufferOverwrite " : "" )
// 	     << (errorFlag_ & emu::daq::reader::Spy::Oversized ? "Oversized" : "" );
// 	  LOG4CPLUS_WARN(logger_, ss.str());
// 	  stringstream ss58;
// 	  ss58 <<  ss.str();
// 	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss58.str() );
// 	  this->notifyQualified( "warning", eObj );
// 	}
// 	else{
// 	  LOG4CPLUS_INFO(logger_, ss.str());
// 	}
//       }
//       else{
// 	LOG4CPLUS_INFO(logger_, ss.str());
//       }

//       printData(ss,data,dataLength);

      if ( insideEvent_ ) { // In STEP runs, insideEvent_ means that we are inside a *needed* event.

	if ( !header && !trailer ){
	  writeDataToFile( data, dataLength );
	  // Store this data to be sent to clients (if any)
	  addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
	} // if ( !header && !trailer )

	else if ( header && !trailer ){
	  LOG4CPLUS_WARN(logger_, 
			 "No trailer in event " << eventNumber_ << 
			 " ("             << nEventsRead_ <<
			 " so far) from " << inputDeviceName_.toString() <<
			 ", size: "       << dataLength );
// 	  stringstream ss59;
// 	  ss59 <<  
// 			 "No trailer in event " << eventNumber_ << 
// 			 " ("             << nEventsRead_ <<
// 			 " so far) from " << inputDeviceName_.toString() <<
// 			 ", size: "       << dataLength ;
// 	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss59.str() );
// 	  this->notifyQualified( "warning", eObj );

	  // Mark the last block for clients
	  makeClientsLastBlockEndEvent();
	  insideEvent_ = false;
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started, reset counter of passes
	    nReadingPassesInEvent_ = 1;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
	    insideEvent_ = true;
	  }
	} // if ( header && !trailer )

	else if ( !header && trailer ){
	  writeDataToFile( data, dataLength );
	  insideEvent_ = false;
	  // Store this data to be sent to clients (if any)
	  addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
	} // if ( !header && trailer )

	else if ( header && trailer ){
	  LOG4CPLUS_WARN(logger_, 
			 "No trailer in event " << eventNumber_ << 
			 " ("             << nEventsRead_ <<
			 " so far) from " << inputDeviceName_.toString() <<
			 ", size: "       << dataLength );
// 	  stringstream ss60;
// 	  ss60 <<  
// 			 "No trailer in event " << eventNumber_ << 
// 			 " ("             << nEventsRead_ <<
// 			 " so far) from " << inputDeviceName_.toString() <<
// 			 ", size: "       << dataLength ;
// 	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss60.str() );
// 	  this->notifyQualified( "warning", eObj );

	  // Mark the last block for clients
	  makeClientsLastBlockEndEvent();
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started and ended, reset counter of passes
	    nReadingPassesInEvent_ = 0;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
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
// 	  stringstream ss61;
// 	  ss61 <<  
// 			  "No header in event after " << eventNumber_ << 
// 			  " ("             << nEventsRead_ <<
// 			  " so far) from " << inputDeviceName_.toString() <<
// 			  ", size: "       << dataLength ;
// 	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss61.str() );
// 	  this->notifyQualified( "warning", eObj );
	} // if ( !header && !trailer )

	else if ( header && !trailer ){
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started, reset counter of passes
	    nReadingPassesInEvent_ = 1;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
	    insideEvent_ = true;
	  }
	} // if ( header && !trailer )

	else if ( !header && trailer ){
	  LOG4CPLUS_WARN(logger_, 
			  "No header in event after " << eventNumber_ << 
			  " ("             << nEventsRead_ <<
			  " so far) from " << inputDeviceName_.toString() <<
			  ", size: "       << dataLength );
// 	  stringstream ss62;
// 	  ss62 <<  
// 			  "No header in event after " << eventNumber_ << 
// 			  " ("             << nEventsRead_ <<
// 			  " so far) from " << inputDeviceName_.toString() <<
// 			  ", size: "       << dataLength ;
// 	  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss62.str() );
// 	  this->notifyQualified( "warning", eObj );
	} // if ( !header && trailer )

	else if ( header && trailer ){
	  if ( STEPEventCounter_.isNeededEvent( data ) ){
	    writeDataToFile( data, dataLength, true );
	    // Get the new event number.
	    eventNumber_ = deviceReader_->eventNumber();
	    // New event started and ended, reset counter of passes
	    nReadingPassesInEvent_ = 0;
	    // Store this data to be sent to clients (if any)
	    addDataForClients( runNumber_.value_, runStartUTC_, nEventsRead_.value_, positionInEvent, errorFlag_, data, dataLength );
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

  const uint64_t maxL1A = 0xffffff;

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
      stringstream ss63;
      ss63 <<  "event number = previous event number = " << eventNumber_ ;
      XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss63.str() );
      this->notifyQualified( "error", eObj );
    }
  }
  catch( emu::daq::rui::exception::Exception &e ){
    LOG4CPLUS_ERROR(logger_, "Failed to insert empty super-fragments: " << stdformat_exception_history(e));
    stringstream ss64;
    ss64 <<  "Failed to insert empty super-fragments: " ;
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss64.str(), e );
    this->notifyQualified( "error", eObj );
  }
}

void emu::daq::rui::Application::insertEmptySuperFragments( const uint64_t fromEventNumber, const uint64_t toEventNumber )
  throw (emu::daq::rui::exception::Exception){

  const char* data     = NULL;
  const size_t dataLength = 0;

  LOG4CPLUS_WARN(logger_, "Inserting " << toEventNumber-fromEventNumber+1 
		 << " empty events (" << fromEventNumber << " through " << toEventNumber << ")");  
//   stringstream ss65;
//   ss65 <<  "Inserting " << toEventNumber-fromEventNumber+1 
// 		 << " empty events (" << fromEventNumber << " through " << toEventNumber << ")";  
//   XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss65.str() );
//   this->notifyQualified( "warning", eObj );

  for ( uint64_t eventNumber = fromEventNumber; eventNumber <= toEventNumber; ++eventNumber ){
    appendNewBlockToSuperFrag( data, dataLength, eventNumber );
    nEventsRead_++;
    previousEventNumber_ = eventNumber;

// //DEBUG_START
//     const int32_t DBGdataLength = 8;
//     char DBGdata[DBGdataLength] = {'a','b','c','d','e','f','g','h'};
//     appendNewBlockToSuperFrag( &DBGdata[0], DBGdataLength, eventNumber );
// //DEBUG_END
  }

}

void emu::daq::rui::Application::printData(std::ostream& os, char* data, const size_t dataLength){
  uint16_t *shortData = reinterpret_cast<uint16_t *>(data);
  os << "_________________________________" << endl;
  os << "                +3   +2   +1   +0" << endl;
  for(size_t i = 0; i < dataLength/2; i+=4)
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

bool emu::daq::rui::Application::hasHeader( char* const data, const size_t dataLength ){
  // By now data must have been stripped of any filler words.
  const size_t DDUHeaderLength = 24; // bytes
  const size_t DCCHeaderLength = 16; // bytes
  const size_t DMBHeaderLength = 16; // bytes
  bool headerFound = false;
  uint16_t *shortData = reinterpret_cast<uint16_t *>(data);

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
  else if ( inputDataFormatInt_ == emu::daq::reader::Base::DMB ){
    if ( dataLength < DMBHeaderLength ) return false; // can the data be split in the header???
    headerFound = ( (shortData[0]&0xf000)==0x9000 && 
		    (shortData[1]&0xf000)==0x9000 && 
		    (shortData[2]&0xf000)==0x9000 && 
		    (shortData[3]&0xf000)==0x9000 && 
		    (shortData[4]&0xf000)==0xa000 && 
		    (shortData[5]&0xf000)==0xa000 && 
		    (shortData[6]&0xf000)==0xa000 && 
		    (shortData[7]&0xf000)==0xa000    );
  }
  return headerFound;
}

bool emu::daq::rui::Application::hasTrailer( char* const data, const size_t dataLength ){
  // By now data must have been stripped of any filler words.
  const size_t DDUTrailerLength = 24; // bytes
  const size_t DCCTrailerLength = 16; // bytes
  const size_t DMBTrailerLength = 16; // bytes
  bool trailerFound = false;
  uint16_t *shortData = reinterpret_cast<uint16_t *>(data);
//   std::cout << "inputDataFormatInt ?=? emu::daq::reader::Base::DDU " << inputDataFormatInt_ << " ?=? " << emu::daq::reader::Base::DDU << std::endl << std::flush;
  if ( inputDataFormatInt_ == emu::daq::reader::Base::DDU ){
    if ( dataLength >= DDUTrailerLength ){
      // All three 64-bit trailer words must be found
      const size_t start = (dataLength - DDUTrailerLength) / 2;
      trailerFound = ( shortData[start+0] == 0x8000 &&
		       shortData[start+1] == 0x8000 &&
		       shortData[start+2] == 0xffff &&
		       shortData[start+3] == 0x8000 &&
		       ( shortData[start+11] & 0xf000 ) == 0xa000 );
    }
    else if ( dataLength == 2 * DDUTrailerLength / 3 || dataLength == DDUTrailerLength / 3 ){
      // Only the last one or two 64-bit trailer words can possibly be here.
      // Only the last 64-bit trailer word has a 4-bit-long fixed part. Not much to go by, but if the data
      // is this short, it must be the rump of a trailer. Let's see if the hard coded bits are indeed there:
      const size_t start = (dataLength - DDUTrailerLength/3) / 2;
      trailerFound = ( ( shortData[start+3] & 0xf000 ) == 0xa000 );
    }
    else{
      // This should never happen: data length is not integer multiple of 64 bits...
      trailerFound = false;
    }

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
    size_t start = (dataLength - DCCTrailerLength) / 2;
    trailerFound = ( (shortData[start+3] & 0xff00) == 0xef00 &&
		     (shortData[start+4] & 0x000f) == 0x0003 &&
		     (shortData[start+7] & 0xff00) == 0xaf00    );
  }
  else if ( inputDataFormatInt_ == emu::daq::reader::Base::DMB ){
    if ( dataLength < DMBTrailerLength ) return false; // can the data be split in the trailer???
    const size_t start = (dataLength - DMBTrailerLength) / 2;
    trailerFound = ( (shortData[start+0]&0xf000)==0xf000 && 
		     (shortData[start+1]&0xf000)==0xf000 && 
		     (shortData[start+2]&0xf000)==0xf000 && 
		     (shortData[start+3]&0xf000)==0xf000 && 
		     (shortData[start+4]&0xf000)==0xe000 && 
		     (shortData[start+5]&0xf000)==0xe000 && 
		     (shortData[start+6]&0xf000)==0xe000 && 
		     (shortData[start+7]&0xf000)==0xe000    );
  }
  return trailerFound;
}


bool emu::daq::rui::Application::interestingDDUErrorBitPattern(char* const data, const size_t dataLength){
  // At this point dataLength should no longer contain Ethernet padding.

  // Check for interesting error bit patterns (defined by J. Gilmore):
  // 1) Error Detected = bad event  (no reset)
  //        DDU Trailer-1 bit 46
  //    ----> Only set for the single event with a detected error.
  // 2) Warning = Buffer Near Full (no reset)
  //        TTS overflow warning
  //    ----> Remains set until the condition abates.
  // 3) Special Warning (Rare & Interesting occurence, no reset)
  //        DDU Trailer-1 bit 45
  //    ----> Remains set until the condition abates; 

  bool foundError = false;
  const size_t DDUTrailerLength = 24; // bytes
  uint16_t *trailerShortWord =
    reinterpret_cast<uint16_t*>( data + dataLength - DDUTrailerLength );
  
  // TTS/FMM status defined in C.D.F.
  const uint16_t FED_Overflow  = 0x0010;
  //const uint16_t  FED_OutOfSync = 0x0020;
  //const uint16_t  FED_Error     = 0x00C0;

  // 1)
  if ( trailerShortWord[6] & 0x4000 ) {    // DDU Trailer-1 bit 46
//     LOG4CPLUS_ERROR(logger_,
//     		    "DDU error: bad event read from " 
//     		    << deviceReader_->getName()
//     		    << ". (bit T-1:46) Event "
//     		    << deviceReader_->eventNumber()
//     		    << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 2)
  if ( trailerShortWord[8] & FED_Overflow ) {
//     LOG4CPLUS_WARN(logger_,
//     		   "DDU buffer near Full in "
//     		   << deviceReader_->getName() 
//     		   << ". (bit T:4) Event "
//     		   << deviceReader_->eventNumber()
//     		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }
  // 3)
  if ( trailerShortWord[6] & 0x2000 ) {      // DDU Trailer-1 bit 45
//     LOG4CPLUS_WARN(logger_,
//     		   "DDU special warning in "
//     		   << deviceReader_->getName() 
//     		   << ". (bit T-1:45) Event "
//     		   << deviceReader_->eventNumber()
//     		   << " (" << nEventsRead_ << " read)");
    foundError = true;
  }

  return foundError;
}

void emu::daq::rui::Application::appendNewBlockToSuperFrag( const char* data, const size_t dataLength, const uint64_t eventNumber )
throw (emu::daq::rui::exception::Exception)
{
    toolbox::mem::Reference *bufRef = 0;

    size_t dataBufSize = sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME) 
                                + dataLength;

    // Get a free block from the RUI/RU pool
    try
    {
        bufRef = poolFactory_->getFrame(ruiRuPool_, dataBufSize );
    }
    catch(xcept::Exception &e)
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
    catch(xcept::Exception &e)
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
    const size_t             dataLength,
    const uint64_t           eventNumber
)
throw (emu::daq::rui::exception::Exception)
{
    char         *blockAddr        = 0;
    char         *fedAddr          = 0;
    size_t i2oMessageSize    = 0;

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
     size_t blockCount = 0;
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
    stringstream ss70;
    ss70 <<  "I2O excpetion: " ;
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss70.str(), exception );
    this->notifyQualified( "error", eObj );

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


void emu::daq::rui::Application::getTidOfEmuTA()
  throw ( xcept::Exception ){

  // First emu::daq::ta::Application's application descriptor
  taDescriptors_.clear();
  try{
    taDescriptors_ = getAppDescriptors(zone_, "emu::daq::ta::Application");
  } catch(emu::daq::rui::exception::Exception &e) {
    XCEPT_RETHROW( xcept::Exception, 
		  "Failed to get application descriptor for class emu::daq::ta::Application", e);
  }

  if ( taDescriptors_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, 
		 "Failed to get application descriptor for class emu::daq::ta::Application");
  }

  if ( taDescriptors_.size() >= 2 ){
    LOG4CPLUS_WARN(logger_,"Got more than one application descriptors for class emu::daq::ta::Application. Using the first one.");
    stringstream ss72;
    ss72 << "Got more than one application descriptors for class emu::daq::ta::Application. Using the first one.";
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss72.str() );
    this->notifyQualified( "warning", eObj );
  }
  
  // Now the Tid
  i2o::utils::AddressMap *i2oAddressMap  = i2o::utils::getAddressMap();

  emuTATid_ = i2oAddressMap->getTid( taDescriptors_[0] );

}


void emu::daq::rui::Application::sendEventNumberToTA( uint64_t firstEventNumber )
  throw ( xcept::Exception ){

  const size_t frameSize = sizeof(I2O_EMUCLIENT_CREDIT_MESSAGE_FRAME) + sizeof(firstEventNumber);

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
      stringstream ss73;
      ss73 <<  "Tried to send first event number to emu::daq::ta::Application: ";
      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss73.str(), e );
      this->notifyQualified( "warning", eObj );
      // Retry 3 times
      bool retryOK = false;
      for (int32_t k = 0; k < 3; k++)
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
	      stringstream ss74;
	      ss74 <<  "Retried to send first event number to emu::daq::ta::Application: ";
	      XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, ss74.str(), re );
	      this->notifyQualified( "warning", eObj );
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

  try{
    // Transfer STEP info into xdata as those can readily be serialized into SOAP
    xdata::UnsignedInteger64                totalCount  = STEPEventCounter_.getNEvents();
    xdata::UnsignedInteger64                lowestCount = STEPEventCounter_.getLowestCount();
    xdata::Vector<xdata::UnsignedInteger64> counts;
    xdata::Vector<xdata::Boolean>           masks;
    xdata::Vector<xdata::Boolean>           liveInputs;
    for( uint32_t iInput=0; iInput < emu::daq::rui::STEPEventCounter::maxDDUInputs_; ++iInput ){
      counts.push_back( STEPEventCounter_.getCount( iInput ) );
      masks.push_back( STEPEventCounter_.isMaskedInput( iInput ) );
      liveInputs.push_back( STEPEventCounter_.isLiveInput( iInput ) );
    }

    return createMessage( "STEPQueryResponse", 
			  emu::soap::Parameters()
			  .add( "PersistentDDUError", &persistentDDUError_ )
			  .add( "EventsRead"        , &nEventsRead_        )
			  .add( "TotalCount"        , &totalCount          )
			  .add( "LowestCount"       , &lowestCount         )
			  .add( "Counts"            , &counts              )
			  .add( "Masks"             , &masks               )
			  .add( "LiveInputs"        , &liveInputs          ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'STEPQuery' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::rui::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'STEPQuery' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }

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

  try{

    xdata::Vector<xdata::UnsignedInteger32> DDUInputs;
    emu::soap::extractParameters( msg, emu::soap::Parameters().add( "DDUInputs", &DDUInputs ) );

    for ( size_t i = 0; i < DDUInputs.elements(); ++i ){
      int32_t dduInputIndex = ( dynamic_cast<xdata::UnsignedInteger32*> ( DDUInputs.elementAt(i)) )->value_;
      if ( in ) STEPEventCounter_.unmaskInput( dduInputIndex );
      else      STEPEventCounter_.maskInput( dduInputIndex );
    }

    return xoap::createMessage();

  } catch( xcept::Exception &e ){
    XCEPT_RETHROW(xoap::exception::Exception, "Failed to mask DDU inputs.", e );
  } catch( ... ){
    XCEPT_RAISE(xoap::exception::Exception, "Failed to mask DDU inputs: Unknown exception." );
  }

}


emu::base::Fact
emu::daq::rui::Application::findFact( const emu::base::Component& component, const string& factType ) {
  cout << "*** emu::daq::rui::Application::findFact" << endl;

  stringstream component_id;
  component_id << xmlClass_ << setfill('0') << setw(2) << instance_;

  string state = fsm_.getStateName(fsm_.getCurrentState());
  uint64_t nEventsWritten = 0;
  if ( fileWriter_ ) nEventsWritten = fileWriter_->getNumberOfEventsWritten();

  if ( factType == RUIStatusFact::getTypeName() 
       &&
       component == emu::base::Component( component_id.str() ) ) {
    emu::base::TypedFact<RUIStatusFact> rs;
    rs.setRun( runNumber_.toString() )
      .setParameter( RUIStatusFact::state,           state                          )
      .setParameter( RUIStatusFact::runType,         runType_.toString()            )
      .setParameter( RUIStatusFact::nEventsRead,     nEventsRead_.value_            )
      .setParameter( RUIStatusFact::isWritingToFile, ( fileWriter_ ? true : false ) )
      .setParameter( RUIStatusFact::nEventsWritten,  nEventsWritten                 );
    if ( state == "Failed" ) rs.setDescription( reasonForFailure_.toString() )
			       .setSeverity( emu::base::Fact::FATAL );
    else                     rs.setDescription( "The status of RUI." )
			       .setSeverity( emu::base::Fact::INFO );
  }

  stringstream ss;
  ss << "Failed to find fact of type \"" << factType
     << "\" on component \"" << component
     << "\" requested by expert system.";
  LOG4CPLUS_WARN( logger_, ss.str() );
  XCEPT_DECLARE( emu::daq::rui::exception::Exception, eObj, ss.str() );
  this->notifyQualified( "warning", eObj );

  // Return an untyped empty fact if no typed fact was found:
  return emu::base::Fact();
}

emu::base::FactCollection
emu::daq::rui::Application::findFacts() {
  emu::base::FactCollection fc;
  cout << "*** emu::daq::rui::Application::findFacts" << endl;

  stringstream component_id;
  component_id << xmlClass_ << setfill('0') << setw(2) << instance_;

  string state = fsm_.getStateName(fsm_.getCurrentState());
  uint64_t nEventsWritten = 0;
  if ( fileWriter_ ) nEventsWritten = fileWriter_->getNumberOfEventsWritten();

  cout << "Run number " << runNumber_.value_ << " " << runNumber_.toString() << endl;

  emu::base::TypedFact<RUIStatusFact> rs;
  rs.setComponentId( component_id.str() )
    .setRun( runNumber_.toString() )
    .setParameter( RUIStatusFact::state,           state                          )
    .setParameter( RUIStatusFact::runType,         runType_.toString()            )
    .setParameter( RUIStatusFact::nEventsRead,     nEventsRead_.value_            )
    .setParameter( RUIStatusFact::isWritingToFile, ( fileWriter_ ? true : false ) )
    .setParameter( RUIStatusFact::nEventsWritten,  nEventsWritten                 );
  if ( state == "Failed" ) rs.setDescription( reasonForFailure_.toString() )
			     .setSeverity( emu::base::Fact::FATAL );
  else                     rs.setDescription( "The status of RUI." )
			     .setSeverity( emu::base::Fact::INFO );
  fc.addFact( rs );

  cout << fc << endl;

  cout << "emu::daq::rui::Application::findFacts ***" << endl;
  return fc;
}

void emu::daq::rui::Application::updateDataFileNames(){
  // Update the value of exported xdata parameter "dataFileNames".
  applicationBSem_.take();
  dataFileNames_.clear();
  if ( fileWriter_ ){
    vector<string> dataFileNames = fileWriter_->getFileNames();
    for ( vector<string>::const_iterator dfn=dataFileNames.begin(); dfn!=dataFileNames.end(); ++dfn ){
      dataFileNames_.push_back( *dfn );
    }
  }
  applicationBSem_.give();
}

void emu::daq::rui::Application::actionPerformed(xdata::Event & received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  LOG4CPLUS_INFO(logger_, 
                 "Received an InfoSpace event" <<
                 " Event type: " << e.type() <<
                 " Event name: " << e.itemName() <<
                 " Serializable: " << std::hex << e.item() << std::dec <<
                 " Type of serializable: " << e.item()->type() );

  if ( e.itemName() == "dataFileNames" && e.type() == "ItemRetrieveEvent" ){
    updateDataFileNames();
  }
}

/**
 * Provides the factory method for the instantiation of emu::daq::rui::Application.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::rui::Application)
