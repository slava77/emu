#include "emu/daq/ta/Application.h"
#include "emu/daq/ta/version.h"
#include "emu/daq/rui/i2oFirstEventNumberMsg.h"
#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"
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
#include "xdata/InfoSpaceFactory.h"

#include <netinet/in.h>

#include <time.h>
#include <iomanip>
#include <unistd.h>


emu::daq::ta::Application::Application(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

logger_(Logger::getInstance(generateLoggerName())),

bSem_(toolbox::BSem::FULL)
{
    tid_           = 0;
    i2oAddressMap_ = i2o::utils::getAddressMap();
    poolFactory_   = toolbox::mem::getMemoryPoolFactory();
    appInfoSpace_  = getApplicationInfoSpace();
    appDescriptor_ = getApplicationDescriptor();
    appContext_    = getApplicationContext();
    zone_          = appContext_->getDefaultZone();
    xmlClass_      = appDescriptor_->getClassName();
    instance_      = appDescriptor_->getInstance();
    urn_           = appDescriptor_->getURN();

    appDescriptor_->setAttribute("icon",
        "/emu/daq/ta/images/EmuTA64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester(zone_);

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &emu::daq::ta::Application::onI2oException, "onI2oException");

    // Create a memory pool for dummy triggers
    try
    {
        toolbox::net::URN urn("toolbox-mem-pool", "Dummy trigger pool");
        toolbox::mem::HeapAllocator* a = new toolbox::mem::HeapAllocator();

        triggerPool_ = poolFactory_->createPool(urn, a);

        LOG4CPLUS_INFO(logger_, "Created dummy trigger pool");
    }
    catch (toolbox::mem::exception::Exception e)
    {
        XCEPT_RETHROW(xdaq::exception::Exception,
            "Failed to create dummy trigger pool", e);
    }

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

    // Fill the application's default info space
    putParamsIntoInfoSpace(stdConfigParams_ , appInfoSpace_);
    putParamsIntoInfoSpace(stdMonitorParams_, appInfoSpace_);
    
    bindFsmSoapCallbacks();
    bindI2oCallbacks();

    // Bind other SOAP callbacks
    xoap::bind(this, &emu::daq::ta::Application::onGenerateRunStopTime, "generateRunStopTime", XDAQ_NS_URI);

    // Bind web interface
    xgi::bind(this, &emu::daq::ta::Application::css           , "styles.css");
    xgi::bind(this, &emu::daq::ta::Application::defaultWebPage, "Default"   );

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::ta::Application::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    uint32_t                    appInstance    = appDescriptor->getInstance();
    stringstream                oss;
    string                      loggerName;


    oss << appClass << appInstance;
    loggerName = oss.str();

    return loggerName;
}


xdaq::ApplicationDescriptor *emu::daq::ta::Application::getRUBuilderTester
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

void emu::daq::ta::Application::defineFsm()
throw (emu::daq::ta::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &emu::daq::ta::Application::stateChanged);
        fsm_.addState('R', "Ready"    , this, &emu::daq::ta::Application::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &emu::daq::ta::Application::stateChanged);
        fsm_.addState('S', "Suspended", this, &emu::daq::ta::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &emu::daq::ta::Application::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &emu::daq::ta::Application::enableAction);
        fsm_.addStateTransition('E', 'S', "Suspend"  , this,
            &emu::daq::ta::Application::suspendAction);
        fsm_.addStateTransition('S', 'E', "Resume"   , this,
            &emu::daq::ta::Application::resumeAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &emu::daq::ta::Application::noAction);
        fsm_.addStateTransition('R', 'R', "Configure", this,
            &emu::daq::ta::Application::noAction);
        fsm_.addStateTransition('E', 'E', "Enable"   , this,
            &emu::daq::ta::Application::noAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &emu::daq::ta::Application::haltActionComingFromReady);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &emu::daq::ta::Application::haltAction);
        fsm_.addStateTransition('S', 'H', "Halt"     , this,
            &emu::daq::ta::Application::haltAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &emu::daq::ta::Application::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &emu::daq::ta::Application::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to reset FSM", e);
    }
}


vector< pair<string, xdata::Serializable*> > emu::daq::ta::Application::initAndGetStdConfigParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    triggerSourceId_ = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("triggerSourceId", &triggerSourceId_));


    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
    runNumber_    = 1;
    isBookedRunNumber_ = false;
    maxNumTriggers_ = -1;
    params.push_back(pair<string,xdata::Serializable *>
        ("runStartTime", &runStartTime_));
    params.push_back(pair<string,xdata::Serializable *>
        ("runStopTime", &runStopTime_));
    params.push_back(pair<string,xdata::Serializable *>
        ("runNumber", &runNumber_));
    params.push_back(pair<string,xdata::Serializable *>
        ("isBookedRunNumber", &isBookedRunNumber_));
    params.push_back(pair<string,xdata::Serializable *>
        ("maxNumTriggers", &maxNumTriggers_));

    return params;
}


vector< pair<string, xdata::Serializable*> > emu::daq::ta::Application::initAndGetStdMonitorParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    stateName_          = "Halted";
    nbCreditsHeld_      = 0;
    eventNumber_        = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("stateName", &stateName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("nbCreditsHeld", &nbCreditsHeld_));
    params.push_back(pair<string,xdata::Serializable *>
        ("eventNumber", &eventNumber_));

    return params;
}


void emu::daq::ta::Application::putParamsIntoInfoSpace
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


void emu::daq::ta::Application::stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
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

    // If the emu::daq::ta::Application has moved to the Enabled state and there are some held credits
//     if((state == 'E') && ((xdata::UnsignedLongT) nbCreditsHeld_ != (unsigned long)0))
//     {
//         // Send triggers for the held credits
//         sendNTriggers(nbCreditsHeld_);
//         nbCreditsHeld_ = 0;  // Credits have been used up
//     }
}


void emu::daq::ta::Application::bindFsmSoapCallbacks()
{
    xoap::bind(this, &emu::daq::ta::Application::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::onEnable   , "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::onSuspend  , "Suspend"  , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::onResume   , "Resume"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::onHalt     , "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::onReset    , "Reset"    , XDAQ_NS_URI);
}

xoap::MessageReference emu::daq::ta::Application::onConfigure(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Configure");
    return emu::soap::createMessage( "ConfigureResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Configure' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Configure' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::ta::Application::onEnable(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Enable");
    return emu::soap::createMessage( "EnableResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Enable' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Enable' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::ta::Application::onSuspend(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Suspend");
    return emu::soap::createMessage( "SuspendResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Suspend' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Suspend' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::ta::Application::onResume(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Resume");
    return emu::soap::createMessage( "ResumeResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Resume' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Resume' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::ta::Application::onHalt(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  try{
    processFsmCommand("Halt");
    return emu::soap::createMessage( "HaltResponse", emu::soap::Parameters().add( "stateName", &stateName_ ) );
  }
  catch(xcept::Exception &e){
    string s = "Failed to process command 'Halt' : ";
    LOG4CPLUS_ERROR(logger_, s << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Halt' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

xoap::MessageReference emu::daq::ta::Application::onReset(xoap::MessageReference msg)
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
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, s, e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, s, e);
  }
  catch(...){
    string s = "Failed to process command 'Reset' : Unknown exception.";
    LOG4CPLUS_ERROR(logger_, s );
    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, s );
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, s);
  }
}

void emu::daq::ta::Application::bindI2oCallbacks()
{
    i2o::bind
    (
        this,
        &emu::daq::ta::Application::taCreditMsg,
        I2O_TA_CREDIT,
        XDAQ_ORGANIZATION_ID
    );

    i2o::bind
    (
        this,
        &emu::daq::ta::Application::firstEventNumberMsg,
        I2O_EMU_FIRST_EVENT_NUMBER_CODE,
        XDAQ_ORGANIZATION_ID
    );
}

xoap::MessageReference emu::daq::ta::Application::onGenerateRunStopTime( xoap::MessageReference msg ){
  runStopTime_ = getDateTime();

  // Create reply message
  return emu::soap::createMessage( "generateRunStopTimeResponse" );
}

void emu::daq::ta::Application::css
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


void emu::daq::ta::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << emudaqta::versions     << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/daq/ta/images/EmuTA64x64.gif\""     << endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << emudaqta::versions
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
        *out << "       alt=\"Manager\""                               << endl;
        *out << "       width=\"64\""                                  << endl;
        *out << "       height=\"64\""                                 << endl;
        *out << "       border=\"\"/>"                                 << endl;
        *out << "    </a>"                                             << endl;
        *out << "    <a href=";
        *out << "\"" << getHref(rubuilderTesterDescriptor_) << "\">"   << endl;
        *out << "      Manager"                                        << endl;
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


void emu::daq::ta::Application::printParamsTable
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


    for(pos = params.begin(); pos != params.end(); pos++)
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


// string emu::daq::ta::Application::serializableScalarToString(xdata::Serializable *s)
// {
//     if(s->type() == "unsigned long") return serializableUnsignedLongToString(s);
//     if(s->type() == "int"          ) return serializableIntegerToString(s);
//     if(s->type() == "double"       ) return serializableDoubleToString(s);
//     if(s->type() == "string"       ) return serializableStringToString(s);
//     if(s->type() == "bool"         ) return serializableBooleanToString(s);

//     return "Unsupported type";
// }


// string emu::daq::ta::Application::serializableUnsignedLongToString(xdata::Serializable *s)
// {
//     xdata::UnsignedLong *v = dynamic_cast<xdata::UnsignedLong*>(s);

//     return v->toString();
// }

// string emu::daq::ta::Application::serializableIntegerToString(xdata::Serializable *s)
// {
//     xdata::Integer *v = dynamic_cast<xdata::Integer*>(s);

//     return v->toString();
// }


// string emu::daq::ta::Application::serializableDoubleToString(xdata::Serializable *s)
// {
//     xdata::Double *v = dynamic_cast<xdata::Double*>(s);

//     return v->toString();
// }


// string emu::daq::ta::Application::serializableStringToString(xdata::Serializable *s)
// {
//     xdata::String *v  = dynamic_cast<xdata::String*>(s);
//     string        str = v->toString();


//     if(str == "")
//     {
//         str = "\"\"";
//     }

//     return str;
// }


// string emu::daq::ta::Application::serializableBooleanToString(xdata::Serializable *s)
// {
//     xdata::Boolean *v = dynamic_cast<xdata::Boolean*>(s);

//     return v->toString();
// }


void emu::daq::ta::Application::processFsmCommand(const string cmdName)
throw (emu::daq::ta::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        bSem_.give();
        XCEPT_RAISE(emu::daq::ta::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}

string emu::daq::ta::Application::getDateTime(){
  time_t t;
  struct tm tm;

  time ( &t );
  gmtime_r( &t, &tm ); // Unversal Coordinated Time (reentrant version for thread safety)

  stringstream ss;
  ss << setfill('0') << setw(2) << tm.tm_year%100
     << setfill('0') << setw(2) << tm.tm_mon+1
     << setfill('0') << setw(2) << tm.tm_mday      << "_"
     << setfill('0') << setw(2) << tm.tm_hour
     << setfill('0') << setw(2) << tm.tm_min
     << setfill('0') << setw(2) << tm.tm_sec       << "_UTC";

  return ss.str();
}


void 
emu::daq::ta::Application::startATCP()
  throw (emu::daq::ta::exception::Exception){
  // configure and enable all pt::atcp::PeerTransportATCP

  std::cout << "In emu::daq::ta::Application::startATCP()" << std::endl;

  vector < xdaq::ApplicationDescriptor* > atcpDescriptors;
  try{
      atcpDescriptors = getAppDescriptors(zone_, "pt::atcp::PeerTransportATCP");
  }
  catch(emu::daq::ta::exception::Exception &e){
    LOG4CPLUS_WARN(logger_, "Failed to get atcp descriptors : " 
		   + xcept::stdformat_exception_history(e) );
    stringstream ss23;
    ss23 <<  "Failed to get atcp descriptors : " 
		    ;
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, ss23.str(), e );
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
      	  XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
      	}

      if ( atcpState.toString() != "Halted" ) continue;

      // Configure ATCP
      try{
	m.sendCommand( *atcpd, "Configure" );
      }
      catch(xcept::Exception &e){
	stringstream oss;
	oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
      }

      // Enable ATCP
      try{
	m.sendCommand( *atcpd, "Enable" );
      }
      catch(xcept::Exception &e){
	stringstream oss;
	oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
      }

    }

  }
  
}

void emu::daq::ta::Application::noAction(toolbox::Event::Reference e)
  throw (toolbox::fsm::exception::Exception)
{
  // Inaction...
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() 
		 << " attempted when already " 
		 << fsm_.getStateName(fsm_.getCurrentState()));
}

void emu::daq::ta::Application::configureAction(toolbox::Event::Reference e)
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
        getAppDescriptorsAndTids();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to get application descriptors and tids", e);
    }

    runStopTime_ = "YYMMDD_hhmmss_UTC";
    
    nEmuRUIs_ = zone_->getApplicationDescriptors("emu::daq::rui::Application").size();
    nVotesForFirstEventNumber_ = 0;
    biggestFirstEventNumber_   = 0;
    smallestFirstEventNumber_  = 0x1000000; // 2^24 (L1A is an unsigned 24-bit number)

    // MOVED TO enableAction START
//     runStartTime_ = getDateTime();

//     LOG4CPLUS_INFO(logger_, 
// 		   "Configured run number " << runNumber_.toString() <<
// 		   ", time " << runStartTime_.value_ );
    // MOVED TO enableAction END
    LOG4CPLUS_INFO(logger_, "End of configureAction");
}


void emu::daq::ta::Application::getAppDescriptorsAndTids()
throw (emu::daq::ta::exception::Exception)
{
    try
    {
        tid_ = i2oAddressMap_->getTid(appDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to get the I2O TID of this application", e);
    }

    // Avoid repeated function calls to obtain EVM descriptor and tid
    try
    {
//         evmDescriptor_ = zone_->getApplicationDescriptor("EVM", 0);
        evmDescriptor_ = zone_->getApplicationDescriptor("rubuilder::evm::Application", 0);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to get the application descriptor of the EVM", e);
    }
    try
    {
        evmTid_ = i2oAddressMap_->getTid(evmDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to get the I2O TID of the EVM", e);
    }
}


void emu::daq::ta::Application::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
  // MOVED FROM configureAction START
    runStartTime_ = getDateTime();

    LOG4CPLUS_INFO(logger_, 
		   "Started run number " << runNumber_.toString() <<
		   " at time " << runStartTime_.value_ );
  // MOVED FROM configureAction END
}


void emu::daq::ta::Application::suspendAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing
}


void emu::daq::ta::Application::resumeAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing
}


void emu::daq::ta::Application::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Reset the dummy event number
    eventNumber_ = 0;

    // Reset the number of credits held
    nbCreditsHeld_ = 0;
}

void emu::daq::ta::Application::haltActionComingFromReady(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Reset the dummy event number
    eventNumber_ = 0;

    // Reset the number of credits held
    nbCreditsHeld_ = 0;
}

void emu::daq::ta::Application::failAction(toolbox::Event::Reference event)
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

        LOG4CPLUS_FATAL(logger_, oss.str() );
        XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, oss.str() );
        this->notifyQualified( "fatal", eObj );
      }
    }catch( xcept::Exception& e ){
      stringstream ss27;
      ss27 <<  "Caught exception while moving to Failed state: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }catch( std::exception& e ){
      stringstream ss27;
      ss27 <<  "Caught exception while moving to Failed state: " << e.what();
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }catch(...){
      stringstream ss27;
      ss27 <<  "Caught an unknown exception while moving to Failed state.";
      LOG4CPLUS_FATAL(logger_, ss27.str() );
      XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss27.str() );
      this->notifyQualified( "fatal", eObj );
    }
}

void emu::daq::ta::Application::sendNTriggers(const uint64_t n)
throw (emu::daq::ta::exception::Exception)
{
    toolbox::mem::Reference *bufRef = 0;
    uint64_t                i       = 0;


    for(i=0; 
	i<n && ( maxNumTriggers_.value_ < 0 || 
		 eventNumber_.value_    < biggestFirstEventNumber_ + maxNumTriggers_.value_ ); 
	i++)
    {
      // In the builder the trigger number must match the data event number. As the latter wraps around 2^24,
      // so too must the trigger number:
       uint64_t eventNumber = eventNumber_.value_ % 0x1000000;
        bufRef = triggerGenerator_.generate
        (
            poolFactory_,            // poolFactory
            triggerPool_,            // pool
            tid_,                    // initiatorAddress
            evmTid_,                 // targetAddress
            triggerSourceId_.value_, // triggerSourceId
            eventNumber,             // eventNumber
            runNumber_.value_        // runNumber
        );

// 	if ( i==0 ) printBlock( bufRef, true );

        try
        {
            appContext_->postFrame
            (
                bufRef,
                appDescriptor_,
                evmDescriptor_,
                i2oExceptionHandler_,
                evmDescriptor_
            );
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string s;

            oss << "Failed to send dummy trigger";
            oss << " (eventNumber=" << eventNumber_ << ")";
            s = oss.str();

            XCEPT_RETHROW(emu::daq::ta::exception::Exception, s, e);
        }
        catch(...)
        {
            stringstream oss;
            string s;

            oss << "Failed to send dummy trigger";
            oss << " (eventNumber=" << eventNumber_ << ")";
            oss << " : Unknown exception";
            s = oss.str();

            XCEPT_RAISE(emu::daq::ta::exception::Exception, s);
        }

        eventNumber_++;
    }
//     LOG4CPLUS_INFO(logger_, "Sent " << eventNumber_ << " triggers; holding " << nbCreditsHeld_);

}

void emu::daq::ta::Application::printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader )
{
  if( printMessageHeader )
    {
      std::cout << "emu::daq::ta::Application::printBlock:" << endl;
      I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME* block = 
	(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME*) bufRef->getDataLocation();
      std::cout  << 
	"   SFrag " << block->superFragmentNb << 
	" of "   << block->nbSuperFragmentsInEvent << endl;
      std::cout  << 
	"   Block " << block->blockNb << 
	" of "   << block->nbBlocksInSuperFragment << endl;
    }
  char         *startOfPayload = (char*) bufRef->getDataLocation() 
    + sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
  size_t  sizeOfPayload =         bufRef->getDataSize()
    - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
  uint16_t* shorts = reinterpret_cast<uint16_t *>(startOfPayload);
  size_t nshorts = sizeOfPayload / sizeof(uint16_t);
  std::cout<<std::hex;
  for(size_t i = 0; i < nshorts; i+=4)
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


void emu::daq::ta::Application::taCreditMsg(toolbox::mem::Reference *bufRef)
{

  // Wait for all votes for first event number to arrive, but don't hog the CPU by idle-looping:
  while( nVotesForFirstEventNumber_ < nEmuRUIs_ ) usleep( 1000 );


    I2O_TA_CREDIT_MESSAGE_FRAME *msg =
        (I2O_TA_CREDIT_MESSAGE_FRAME*)bufRef->getDataLocation();


    bSem_.take();

    try
    {
        switch(fsm_.getCurrentState())
        {
        case 'H': // Halted
        case 'F': // Failed
            break;
        case 'E': // Enabled
            sendNTriggers(msg->nbCredits);
            break;
        case 'R': // Ready
        case 'S': // Suspended
            nbCreditsHeld_.value_ += msg->nbCredits;  // Hold credits
            break;
        default:
	  {
            LOG4CPLUS_ERROR(logger_,
			    "emu::daq::ta::Application in undefined state");
            stringstream ss6;
            ss6 << "emu::daq::ta::Application in undefined state";
            XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss6.str() );
            this->notifyQualified( "error", eObj );
	  }
	}
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process trigger credit message : "
             << stdformat_exception_history(e));
        stringstream ss7;
        ss7 << 
            "Failed to process trigger credit message : "
             ;
        XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, ss7.str(), e );
        this->notifyQualified( "error", eObj );
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process trigger credit message : Unknown exception");
        stringstream ss8;
        ss8 << 
            "Failed to process trigger credit message : Unknown exception";
        XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss8.str() );
        this->notifyQualified( "error", eObj );
    }

    bSem_.give();

    // Free the trigger credits message
    bufRef->release();
}

void emu::daq::ta::Application::firstEventNumberMsg(toolbox::mem::Reference *bufRef)
{
    I2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME *msg =
      (I2O_EMU_FIRST_EVENT_NUMBER_MESSAGE_FRAME*)bufRef->getDataLocation();

    I2O_TID tid;
    string name;
    uint32_t instance;

    bSem_.take();

    try
    {
        switch(fsm_.getCurrentState())
        {
        case 'H': // Halted
        case 'F': // Failed
            break;
        case 'E': // Enabled

	  tid = msg->PvtMessageFrame.StdMessageFrame.InitiatorAddress;
	  name = i2o::utils::getAddressMap()->getApplicationDescriptor(tid)->getClassName();
	  instance = i2o::utils::getAddressMap()->getApplicationDescriptor(tid)->getInstance();

	  nVotesForFirstEventNumber_++;

	  if ( nVotesForFirstEventNumber_ <= nEmuRUIs_ ){
	    if ( msg->firstEventNumber > biggestFirstEventNumber_  ) biggestFirstEventNumber_  = msg->firstEventNumber;
	    if ( msg->firstEventNumber < smallestFirstEventNumber_ ) smallestFirstEventNumber_ = msg->firstEventNumber;
	    eventNumber_ = biggestFirstEventNumber_;
	    LOG4CPLUS_INFO(logger_,"Got first event number " 
			   << msg->firstEventNumber << " from " 
			   << name << instance << " ("
			   << nVotesForFirstEventNumber_ << " of "
			   << nEmuRUIs_ << ")");
	    if ( nVotesForFirstEventNumber_ == nEmuRUIs_ ){
	      LOG4CPLUS_INFO(logger_,"Got first event number from every EmuRUI. Their range is [" 
			     << smallestFirstEventNumber_ << ", " << biggestFirstEventNumber_ << "]" );
	    }
	  }
	  else{
	    LOG4CPLUS_ERROR(logger_,"Got more votes (" << nVotesForFirstEventNumber_ 
			    << ") for first event number than there are EmuRUIs (" << nEmuRUIs_ << ")?!" );
	    stringstream ss9;
	    ss9 << "Got more votes (" << nVotesForFirstEventNumber_ 
			    << ") for first event number than there are EmuRUIs (" << nEmuRUIs_ << ")?!" ;
	    XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss9.str() );
	    this->notifyQualified( "error", eObj );
	  }

	  break;
        case 'R': // Ready
        case 'S': // Suspended
            break;
        default:
            LOG4CPLUS_ERROR(logger_,
                "emu::daq::ta::Application in undefined state");
            stringstream ss10;
            ss10 << 
                "emu::daq::ta::Application in undefined state";
            XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss10.str() );
            this->notifyQualified( "error", eObj );
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process first event number message : "
             << stdformat_exception_history(e));
        stringstream ss11;
        ss11 << 
            "Failed to process first event number message : "
             ;
        XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, ss11.str(), e );
        this->notifyQualified( "error", eObj );
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process first event number message : Unknown exception");
        stringstream ss12;
        ss12 << 
            "Failed to process first event number message : Unknown exception";
        XCEPT_DECLARE( emu::daq::ta::exception::Exception, eObj, ss12.str() );
        this->notifyQualified( "error", eObj );
    }

    bSem_.give();

    // Free the trigger credits message
    bufRef->release();
}


string emu::daq::ta::Application::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool emu::daq::ta::Application::onI2oException(xcept::Exception &exception, void *context)
{
    LOG4CPLUS_ERROR(logger_,
        " I2O excpetion: " << xcept::stdformat_exception_history(exception));
    stringstream ss13;
    ss13 << 
        " I2O excpetion: " ;
    XCEPT_DECLARE_NESTED( emu::daq::ta::exception::Exception, eObj, ss13.str(), exception );
    this->notifyQualified( "error", eObj );

    return true;
}


string emu::daq::ta::Application::createI2oErrorMsg
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

vector< xdaq::ApplicationDescriptor* > emu::daq::ta::Application::getAppDescriptors
(
    xdaq::Zone             *zone,
    const string           appClass
)
throw (emu::daq::ta::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
    set< xdaq::ApplicationDescriptor* > descriptors;
    set< xdaq::ApplicationDescriptor* >::size_type nbApps = 0;

    try
    {
        descriptors = zone->getApplicationDescriptors(appClass);
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e)
    {
        string s;

        s = "Failed to get application descriptors for class: " + appClass;

        XCEPT_RETHROW(emu::daq::ta::exception::Exception, s, e);
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


/**
 * Provides the factory method for the instantiation of emu::daq::ta::Application.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::ta::Application)

