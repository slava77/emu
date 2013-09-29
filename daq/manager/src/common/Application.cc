#include "emu/daq/manager/Application.h"
#include "emu/daq/manager/version.h"
#include "emu/daq/manager/alarm/Alarm.h"
#include "emu/base/Alarm.h"
#include "emu/daq/rui/STEPEventCounter.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "cgicc/HTTPResponseHeader.h"
#include "xcept/tools.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xgi/Method.h"
#include "xgi/Utils.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"

#include "toolbox/regex.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "emu/base/TypedFact.h"
#include "emu/base/ApplicationStatusFact.h"
#include "emu/daq/manager/FactTypes.h"

#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"

#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h" 

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <algorithm>
#include <limits>

emu::daq::manager::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) 
  : xdaq::WebApplication(s),
    emu::base::Supervised(s),
    emu::base::WebReporter(s),
    emu::base::FactFinder( s, emu::base::FactCollection::LOCAL_DAQ, 0 ),
    logger_(Logger::getInstance(generateLoggerName())),
    inFSMTransition_( false ),
    watchdog_( NULL ),
    runInfo_(NULL)
{
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
        "/emu/daq/manager/images/EmuDAQManager64x64.gif");

    getAllAppDescriptors();

    // Bind web interface
    xgi::bind(this, &emu::daq::manager::Application::defaultWebPage, "Default"   );
    xgi::bind(this, &emu::daq::manager::Application::controlWebPage, "control"   );
    xgi::bind(this, &emu::daq::manager::Application::commandWebPage, "command"   );
    xgi::bind(this, &emu::daq::manager::Application::commentWebPage, "comment"   );

    exportParams(appInfoSpace_);
    createAllAppStates();

    abortedRun_ = true;

    comments_        = "";
    globalRunNumber_ = "";
    badRun_          = false;

    // Supervisor-specific stuff:
    xoap::bind(this, &emu::daq::manager::Application::onConfigure,     "Configure",     XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::manager::Application::onEnable,        "Enable",        XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::manager::Application::onDisable,       "Disable",       XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::manager::Application::onHalt,          "Halt",          XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::manager::Application::onReset,         "Reset",         XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::manager::Application::onQueryRunSummary,  "QueryRunSummary",  XDAQ_NS_URI);

    // Try driving FSM by asynchronous SOAP in order for DQM hang-ups not to block us (and, eventually, the global run).
    // Execute FSM transitions in a separate thread.
    workLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("emu::daq::manager::Application", "waiting");
    workLoop_->activate();
    configureSignature_ = toolbox::task::bind(this, &emu::daq::manager::Application::configureActionInWorkLoop, "configureActionInWorkLoop");
    enableSignature_    = toolbox::task::bind(this, &emu::daq::manager::Application::enableActionInWorkLoop,    "enableActionInWorkLoop");
    haltSignature_      = toolbox::task::bind(this, &emu::daq::manager::Application::haltActionInWorkLoop,      "haltActionInWorkLoop");

    fsm_.addState('H', "Halted",     this, &emu::daq::manager::Application::stateChanged);
    fsm_.addState('C', "Configured", this, &emu::daq::manager::Application::stateChanged);
    fsm_.addState('E', "Enabled",    this, &emu::daq::manager::Application::stateChanged);
    
    fsm_.addStateTransition('H', 'C', "Configure", this, &emu::daq::manager::Application::configureAction);
    fsm_.addStateTransition('C', 'C', "Configure", this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('C', 'E', "Enable",    this, &emu::daq::manager::Application::enableAction);
    fsm_.addStateTransition('E', 'E', "Disable",   this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('E', 'E', "Configure", this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('C', 'H', "Halt",      this, &emu::daq::manager::Application::haltAction);
    fsm_.addStateTransition('E', 'H', "Halt",      this, &emu::daq::manager::Application::haltAction);
    fsm_.addStateTransition('H', 'H', "Enable",    this, &emu::daq::manager::Application::noAction);    
    fsm_.addStateTransition('H', 'H', "Halt",      this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('E', 'E', "Enable",    this, &emu::daq::manager::Application::noAction);

    fsm_.setFailedStateTransitionChanged(this, &emu::daq::manager::Application::stateChanged);

    // Set the inital state according to the combined state of the DAQ (useful if DAQ Manager crashes and is restarted)
    char initialState = 'H';
    try{
      queryAppStates();
      string combinedState = currentAppStates_.getCombinedState();

      stringstream ss;
      ss << "Current state of the DAQ is " << combinedState;
      cout << ss.str() << endl << flush;
      LOG4CPLUS_INFO( getApplicationLogger(), ss.str() );

      if      ( combinedState == "Ready"   ) initialState = 'C';
      else if ( combinedState == "Enabled" ) initialState = 'E';
    }
    catch(xcept::Exception& e){
      stringstream ss;
      ss << "Failed to query the DAQ state. The DAQ Manager's state will be 'Halted' at its construction." << xcept::stdformat_exception_history(e);
      LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );      
    }
    fsm_.setInitialState( initialState );
    fsm_.reset();

    inFSMTransition_ = false;

    state_ = fsm_.getStateName(fsm_.getCurrentState());

    stringstream timerName;
    timerName << "DAQWatchdog." << getApplicationDescriptor()->getClassName() << "." << getApplicationDescriptor()->getInstance();
    try{
      toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer( timerName.str() );
      toolbox::TimeInterval interval( 30, 0 ); // period in sec
      toolbox::TimeVal start( toolbox::TimeVal::gettimeofday() + toolbox::TimeVal( 2, 0 ) ); // start in 2 seconds from now
      timer->scheduleAtFixedRate( start, this, interval,  0, "" );
    } catch(xcept::Exception& e){
      stringstream ss;
      ss << "Failed to create " << timerName << " , therefore no scheduled check of state of DAQ applications will be done: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_WARN( getApplicationLogger(), ss.str() );
      XCEPT_DECLARE( xcept::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    }

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::manager::Application::generateLoggerName()
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


void emu::daq::manager::Application::getAllAppDescriptors()
{
    try
    {
        evmDescriptors_ = getAppDescriptors(zone_, "rubuilder::evm::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        evmDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class EVM"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss0;
        ss0 << 
            "Failed to get application descriptors for class EVM"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss0.str(), e );
        this->notifyQualified( "error", eObj );
    }

    try
    {
//         ruDescriptors_ = getAppDescriptors(zone_, "RU");
        ruDescriptors_ = getAppDescriptors(zone_, "rubuilder::ru::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        ruDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class RU"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss1;
        ss1 << 
            "Failed to get application descriptors for class RU"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss1.str(), e );
        this->notifyQualified( "error", eObj );
    }

    try
    {
//         buDescriptors_ = getAppDescriptors(zone_, "BU");
        buDescriptors_ = getAppDescriptors(zone_, "rubuilder::bu::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        buDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class BU"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss2;
        ss2 << 
            "Failed to get application descriptors for class BU"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss2.str(), e );
        this->notifyQualified( "error", eObj );
    }

    try
    {
        taDescriptors_ = getAppDescriptors(zone_, "emu::daq::ta::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        taDescriptors_.clear();

        // Log only a warning as emu::daq::ta::Application may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class emu::daq::ta::Application"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss3;
        ss3 << 
            "Failed to get application descriptors for class emu::daq::ta::Application"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss3.str(), e );
        this->notifyQualified( "warning", eObj );
    }

    try
    {
        ruiDescriptors_ = getAppDescriptors(zone_, "emu::daq::rui::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        ruiDescriptors_.clear();

        // Log only a warning as emu::daq::rui::Applications may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class emu::daq::rui::Application"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss4;
        ss4 << 
            "Failed to get application descriptors for class emu::daq::rui::Application"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss4.str(), e );
        this->notifyQualified( "warning", eObj );
    }

    try
    {
        fuDescriptors_ = getAppDescriptors(zone_, "emu::daq::fu::Application");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        fuDescriptors_.clear();

        // Log only a warning as emu::daq::fu::Applications may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class emu::daq::fu::Application"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss5;
        ss5 << 
            "Failed to get application descriptors for class emu::daq::fu::Application"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss5.str(), e );
        this->notifyQualified( "warning", eObj );
    }

}


void emu::daq::manager::Application::onException(xcept::Exception &e)
{
  LOG4CPLUS_INFO(logger_, "Received an exception from the sentinel: " 
		 << xcept::stdformat_exception_history(e));
}


vector< xdaq::ApplicationDescriptor* > emu::daq::manager::Application::getAppDescriptors
(
    xdaq::Zone             *zone,
    const string           appClass
)
throw (emu::daq::manager::exception::Exception)
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

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
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


void emu::daq::manager::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    // Recycle commands may be issued from here, too. Therefore:
    processCommandForm(in, out);

    daqState_ = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/emu/daq/manager/html/emudaqmanager.css\"/>"      << endl;
    *out << "<title>"                                                  << endl;
    *out << "Local DAQ " << daqState_.toString()                       << endl;
//     *out << xmlClass_ << instance_                                     << endl;
// 	 << " Version "
//         << emudaqmanager::versions 
// 	 << endl;
    *out << "</title>"                                                 << endl;
    *out << "<script type=\"text/javascript\" src=\"/emu/daq/manager/html/emudaqmanager.js\"></script>" << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body onload=\"countSeconds()\">"                         << endl;
    *out << "<form method=\"get\" action=\"/" << urn_ << "/\">"        << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "src=\"/emu/daq/manager/images/EmuDAQManager64x64.gif\""   << endl;
    *out << "     alt=\"emu::daq::manager::Application\""              << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td>"                                                   << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ 
// 	 << " Version " << emudaqmanager::versions 
	 << "  " << fsm_.getStateName(fsm_.getCurrentState())          << endl;
    *out << "    </b>"                                                 << endl;
    *out << "    <br/>Updated at " <<  getDateTime()                   << endl;
    *out << "    &#8212; <span id=\"ageOfPage\"></span> ago "          << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">" << endl;
    *out << "    <a href=\"/" << urn_ << "/control\" target=\"_top\">" << endl;
    *out << "      <img"                                               << endl;
    *out << "   src=\"/emu/daq/manager/images/ctrl64x64.gif\""         << endl;
    *out << "       alt=\"Control\""                                   << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/" << urn_ << "/control\" target=\"_top\">" << endl;
    *out << "      Control"                                            << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td width=\"32\">"                                      << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"70\">" << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\" target=\"_top\">"<< endl;
    *out << "      <img"                                               << endl;
    *out << "       src=\"/hyperdaq/images/HyperDAQ.jpg\""             << endl;
    *out << "       alt=\"HyperDAQ\""                                  << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\" target=\"_top\">"<< endl;
    *out << "      HyperDAQ"                                           << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
//     *out << "<tr>"                                                     << endl;
//     *out << "  <td colspan=\"4\">"                                     << endl;
//     *out << "    Updated at " <<  getDateTime()                        << endl;
//     *out << "    &#8212; <span id=\"ageOfPage\"></span> ago "          << endl;
//     *out << "  </td>"                                                  << endl;
//     *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "<hr/>"                                                    << endl;

    printDAQState( out, daqState_.toString() );

    // Emu: display event number and max number of events
    string runNumber("UNKNOWN");
    string maxNumEvents("UNKNOWN");
    string runStartTime("UNKNOWN");
    string runStopTime("UNKNOWN");
    getRunInfoFromTA( &runNumber, &maxNumEvents, &runStartTime, &runStopTime );

    if ( daqState_.toString() == "Enabled" )
      *out << "<br/>Started at " << runStartTime                       << endl;
    if ( daqState_.toString() == "Halted" ){
      *out << "<br/>Started at " << runStartTime 
	   << "<br/>Stopped at " << runStopTime                        << endl;
    }
    *out << "<br/>"                                                    << endl;

    stringstream STEPCountsTable;
    bool         STEPHasFinished = true;
    if ( runType_.toString().find("STEP",0) != string::npos ){
      STEPHasFinished = printSTEPCountsTable( STEPCountsTable, false );
      *out << "STEP has ";
      if ( !STEPHasFinished ) *out << "<span style=\"font-weight: bold; color:#ff0000;\">not</span> ";
      *out << "finished.";
      *out << "<br/>"                                                    << endl;
    }

    *out << "<br/>"                                                    << endl;

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">" << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Run number"                                         << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    if ( fsm_.getCurrentState() == 'E' )
      *out << "    " << runNumber                                      << endl;
    else
      *out << "    " << runNumber_.toString()                          << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</td>"                                                  << endl;
    *out << "<td width=\"32\"/>"                                     << endl;
    *out << "<td>"                                                   << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Run type"                                           << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    *out << "    " << runType_.toString()                              << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</td>"                                                  << endl;
    *out << "<td width=\"32\"/>"                                     << endl;
    *out << "<td>"                                                   << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Max number of events"                               << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    if ( fsm_.getCurrentState() == 'E' ){
      *out << "    " << maxNumEvents;
      if ( maxNumEvents.find("-",0) != string::npos ) 
	*out << " (unlimited)";
    }
    else{
      *out << "    " << maxNumberOfEvents_.toString()                  << endl;
      if ( maxNumberOfEvents_.toString().find("-",0) != string::npos ) 
	*out << " (unlimited)";
    }
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</td>"                                                  << endl;
    *out << "<td width=\"32\"/>"                                     << endl;
    *out << "<td>"                                                   << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">" << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Global run number"                                  << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    *out << "    " << globalRunNumber_                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</td>"                                                  << endl;
    *out << "<td width=\"32\"/>"                                     << endl;
    *out << "<td>"                                                   << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">" << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Comments"                                           << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    *out << "    " << comments_                                        << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "</td>"                                                  << endl;
    *out << "</tr>"                                                  << endl;
    *out << "</table>"                                               << endl;

    *out << "</form>"                                                  << endl;

    // Display RUIs' and FUs' event counts
    if ( runType_.toString().find("STEP",0) != string::npos ){
      *out << STEPCountsTable.str();
    }
    else{
      printEventCountsTable( out, "Events read by emu::daq::rui::Applications"    , getRUIEventCounts(), false );
    }
    *out << "<br/>"                                                  << endl;
    if ( runType_.toString().find("STEP",0) == string::npos && buildEvents_.value_ ){
      printEventCountsTable( out, "Events processed by emu::daq::fu::Applications", getFUEventCounts() , false );
      *out << "<br/>"                                                  << endl;
    }

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    statesTableToHtml( out, "DAQ applications", daqContexts_, currentAppStates_ );
    *out << "</td>"                                                   << endl;
    *out << "<td width=\"16\"/>"                                      << endl;
    *out << "<td>"                                                   << endl;
    *out << "</td>"                                                   << endl;
    *out << "</tr>"                                                   << endl;
    *out << "</table>"                                               << endl;

    if ( buildEvents_.value_ ){
    vector< vector< pair<string,string> > > evmStats=getStats(evmDescriptors_);
    vector< vector< pair<string,string> > > ruStats =getStats(ruDescriptors_);
    vector< vector< pair<string,string> > > buStats =getStats(buDescriptors_);

    *out << "<table border=\"0\">"                                     << endl;
    *out << "<tr valign=\"top\">"                                      << endl;
    *out << "<td>"                                                     << endl;
    if(evmDescriptors_.size() > 0)
    {
        vector< pair<string,string> > eventNb;

        try
        {
            eventNb = getEventNbFromEVM(evmDescriptors_[0]);
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(xgi::exception::Exception,
                "Failed to get event number from EVM", e);
        }

        try
        {
            printParamsTable(in, out, evmDescriptors_[0], eventNb);
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(xgi::exception::Exception,
                "Failed to print current event number of EVM", e);
        }
    }
    *out << "</td>"                                                    << endl;
    *out << "<td width=\"64\">"                                        << endl;
    *out << "</td>"                                                    << endl;
    *out << "<td>"                                                     << endl;
    try
    {
        printParamsTables(in, out, evmDescriptors_, evmStats);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print the parameter table of the EVM", e);
    }
    *out << "</td>"                                                    << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "<br>"                                                     << endl;

    try
    {
        printParamsTables(in, out, ruDescriptors_, ruStats);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print the parameter tables of the RUs", e);
    }

    *out << "<br>"                                                     << endl;

    try
    {
        printParamsTables(in, out, buDescriptors_, buStats);
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print the parameter tables of the BUs", e);
    }

    *out << "<br>"                                                     << endl;

    }// if ( buildEvents_.value_ )

    *out << "</body>"                                                  << endl;

    *out << "</html>"                                                  << endl;

}


void emu::daq::manager::Application::printAppInstanceLinks
(
    xgi::Output                             *out,
    vector< xdaq::ApplicationDescriptor* >  &appDescriptors
)
{
    vector< xdaq::ApplicationDescriptor* >::iterator pos;

    for(pos=appDescriptors.begin(); pos != appDescriptors.end(); pos++)
    {
        *out << " <a href=\"" << getHref(*pos) << "\">";
        *out << (*pos)->getInstance();
        *out << "</a>";
    }
}


void emu::daq::manager::Application::controlWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  *out << "<html>"                                                   << endl;
  *out << "<head>"                                                   << endl;
  *out << "<title>"                                                  << endl;
  *out << "Emu Local DAQ "                                           << endl;
  *out << "</title>"                                                 << endl;
  *out << "</head>"                                                  << endl;
  *out << "  <frameset rows=\"90%, *\">"                             << endl;
  *out << "    <frame src=\"command\"/>"                             << endl;
  *out << "    <frame src=\"comment\"/>"                             << endl;
  *out << "  </frameset>"                                            << endl;
  *out << "</html>"                                                  << endl;
}

void emu::daq::manager::Application::webRedirect(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string url = in->getenv("PATH_TRANSLATED");
  
  cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location", url.substr(0, url.find("?")));
  // cout << "PATH_TRANSLATED " << url << endl << flush;
  // cout << "PATH_INFO " << in->getenv("PATH_INFO") << endl << flush;
  // cout << "Redirecting to " << url.substr(0, url.find("?")) << endl << flush;
}

void emu::daq::manager::Application::printUserComments( xgi::Output *out ){
  *out << "<table border=\"0\" rules=\"none\">"                  << endl;
  *out << "  <tr><td>Global run number:</td><td>" << globalRunNumber_;
  *out << "</td></tr>"                                           << endl;
  *out << "  <tr><td>Bad run:</td><td>" << 
    ( badRun_? "<span style=\"font-weight: bold; color:#ff0000;\">true</span>" : "false" ) ;
  *out << "</td></tr>"                                           << endl;
  *out << "  <tr><td>Comments:</td><td>" << comments_;
  *out << "</td></tr>"                                           << endl;
  *out << "</table>"                                             << endl;
}

void emu::daq::manager::Application::stringReplace( string& inThis, const string& This, const string& withThis ){
  string::size_type atIndex = 0;
  while( ( atIndex = inThis.find( This, atIndex )) != string::npos ){
    inThis.replace( atIndex, This.size(), withThis, 0, withThis.size() );
    atIndex += withThis.size();
  }
}

string emu::daq::manager::Application::textToHtml( const string text ){
  // Just replace new line and carriage return with <br/>
  string html = text;

  stringReplace( html, "\n", "<br/>" );
  stringReplace( html, "\r", "<br/>" );
//   stringReplace( html, "\\", "\\\\" );
  stringReplace( html, "\"", "\\\"" );
//   stringReplace( html, "'", "\'");

  return html;
}

void emu::daq::manager::Application::governorForm(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  // Select who's the governor, i.e., whose order we obey: central run control or CSC shift

  processGovernorForm(in, out);

//  *out << "<html>"                                                   << endl;

//   *out << "<head>"                                                   << endl;
//   *out << "<title>"                                                  << endl;
//   *out << "Governor"                                                  << endl;
//   *out << "</title>"                                                 << endl;
//   *out << "</head>"                                                  << endl;

//   *out << "<body>"                                                   << endl;
  *out << "<form method=\"get\" action=\"/" << urn_ << "/command\">" << endl;

  *out << "<table border=\"0\" width=\"80%\" align=\"center\" valign=\"center\">"<< endl;
  *out << "<tr>"                                                     << endl;
  *out << "  <td align=\"left\">"                                    << endl;
  *out << " Select who should configure, start and stop the CSC local run: "    << endl;
  *out << "<select name=\"governor\" title=\"Whose commands are to be obeyed.\">"<< endl;
  *out << " <option value=\"supervised\">[ Supervisor | FM | Central RC ] (<em>supervised mode</em>)</option>"<< endl;
  *out << " <option value=\"unsupervised\">CSC Shift from this page (<em>unsupervised mode</em>)</option>"    << endl;
  *out << "</select>"                                                           << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"right\">"                                   << endl;
  *out << "<input"                                                   << endl;
  *out << " class=\"button\""                                        << endl;
  *out << " type=\"submit\""                                         << endl;
  *out << " name=\"commit\""                                         << endl;
  *out << " title=\"Commit your selection.\""                        << endl;
  *out << " value=\"commit\""                                        << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "<tr>"                                                     << endl;
  *out << "</table>"                                                 << endl;

  *out << "</form>"                                                  << endl;

//   *out << "</body>"                                                  << endl;

//   *out << "</html>"                                                  << endl;
}

void emu::daq::manager::Application::processGovernorForm(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);

    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    std::vector<cgicc::FormEntry>::iterator fe;

    for ( fe=fev.begin(); fe!=fev.end(); ++ fe ){
 
//      cout << fe->getName() << ": " << fe->getValue() << endl;

      if ( fe->getName() == "governor" ){
	bool userWantsSupervised = ( fe->getValue() == "supervised");
// 	cout << "userWantsSupervised        " << userWantsSupervised << endl
// 	     << "configuredInSupervisedMode " << configuredInSupervisedMode_.value_ << endl
// 	     << "CurrentState           " << fsm_.getCurrentState() << endl;
	
	if ( daqState_.toString() == "Halted" ){
	  // DAQ's halted, everything's permitted.
	  warningsToDisplay_ = "";
	  bool change = ( supervisedMode_.value_ != userWantsSupervised );
	  supervisedMode_ = userWantsSupervised;
	  if ( change ) appInfoSpace_->fireItemValueChanged( "supervisedMode" );
	}
	else{
	  // DAQ's most probably been configured.
	  if ( !supervisedMode_.value_ && userWantsSupervised ){
	    // User wants to switch to central RC.
	    if ( !configuredInSupervisedMode_.value_ ){
	      // Warn him if we are in a locally configured run.
	      warningsToDisplay_ = "<p><span style=\"color:#ff0000; font-weight:bold; text-decoration:blink\">Warning</span>:";
	      warningsToDisplay_ += "You have chosen <em>supervised mode</em> (i.e., handed over the control to [CSC Supervisor | CSC Function Manager | Central Run Control]) in a run configured in <em>unsupervised mode</em> (i.e., by CSC Shift from this web page). ";
	      warningsToDisplay_ += "The current local run will be automatically stopped in sync with the global run, and the subsequent local runs will also be automatically started and stopped in sync with the global ones.";
	    }
	    else{
	      warningsToDisplay_ = "";
	    }
	    bool change = ( supervisedMode_.value_ != userWantsSupervised );
	    supervisedMode_ = userWantsSupervised;
	    if ( change ) appInfoSpace_->fireItemValueChanged( "supervisedMode" );
	  }
	  else{
	    // Warn the user when he wants to take control back from central RC...
	    if ( supervisedMode_.value_ && !userWantsSupervised && configuredInSupervisedMode_.value_ && daqState_.toString() != "Halted" ){
	      warningsToDisplay_  = "<p><span style=\"color:#ff0000; font-weight:bold; text-decoration:blink\">Warning</span>:";
	      warningsToDisplay_ += "You have chosen <em>unsupervised mode</em> (i.e., control by CSC Shift from this web page) in a run run configured in <em>supervised mode</em> (i.e., by [CSC Supervisor | CSC Function Manager | Central Run Control]).";
	      warningsToDisplay_ += "It is preferable that such runs be started and stopped in <em>supervised mode</em>. ";
	      warningsToDisplay_ += "Consider going back to <em>supervised mode</em>.</p>";
	    }
	    // ...but do as he requested.
	    bool change = ( supervisedMode_.value_ != userWantsSupervised );
	    supervisedMode_ = userWantsSupervised;
	    if ( change ) appInfoSpace_->fireItemValueChanged( "supervisedMode" );
	  }
	}

	break;
      }

    }
    
    *out << warningsToDisplay_;

    if ( supervisedMode_.value_ ){
      REVOKE_ALARM( "unsupervised", NULL );
    }
    else {
      RAISE_ALARM( emu::daq::manager::alarm::Unsupervised, "unsupervised", "warning", "Local DAQ is in unsupervised mode.", "", &logger_ );
    }

    if ( supervisedMode_.value_ ) setParametersForSupervisedMode();

}

void emu::daq::manager::Application::commentWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  processCommentForm(in);

  *out << "<html>"                                                   << endl;

  *out << "<head>"                                                   << endl;
  *out << "<link type=\"text/css\" rel=\"stylesheet\"";
  *out << " href=\"/emu/daq/manager/html/emudaqmanager.css\"/>"     << endl;
  *out << "<title>"                                                  << endl;
  *out << "Comments"                                                 << endl;
  *out << "</title>"                                                 << endl;
  *out << "</head>"                                                  << endl;

  *out << "<body>"                                                   << endl;

  *out << "<form method=\"get\" action=\"/" << urn_ << "/comment\">" << endl;

  *out << "<table border=\"0\" width=\"100%\">"                      << endl;
  *out << "<tr>"                                                     << endl;

  *out << "  <td align=\"left\">"                                    << endl;
  *out << "Comments: "                                               << endl;
  *out << "<input"                                                   << endl;
  *out << " type=\"text\""                                           << endl;
  *out << " name=\"comments\""                                       << endl;
  *out << " title=\"Your comment. (A one-liner only.)\""             << endl;
  *out << " alt=\"your comments\""                                   << endl;
  *out << " value=\"" << comments_ << "\""                           << endl;
  *out << " size=\"40\""                                             << endl;
  *out << "/>  "                                                     << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"left\">"                                    << endl;
  *out << "Global run number: "                                      << endl;
  *out << "<input"                                                   << endl;
  *out << " type=\"text\""                                           << endl;
  *out << " name=\"globalrunnumber\""                                << endl;
  *out << " title=\"The number of the global run in parallel.\""     << endl;
  *out << " alt=\"global run number\""                               << endl;
  *out << " value=\"" << globalRunNumber_ << "\""                    << endl;
  *out << " size=\"8\""                                              << endl;
  if ( supervisedMode_.value_ ) *out << " disabled=\"true\""             << endl;
  *out << "/>  "                                                     << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"left\">"                                    << endl;
  *out << " Bad run:"                                                << endl;
  *out << "<input"                                                   << endl;
  *out << " type=\"checkbox\""                                       << endl;
  *out << " name=\"badrun\""                                         << endl;
  *out << " title=\"Check if this run is to be ignored.\""           << endl;
  *out << " alt=\"bad run\""                                         << endl;
  if ( badRun_ ) *out << " checked"                                  << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "  <td align=\"right\">"                                   << endl;
  *out << "<input"                                                   << endl;
  *out << " class=\"button\""                                        << endl;
  *out << " type=\"submit\""                                         << endl;
  *out << " name=\"refresh\""                                        << endl;
  *out << " title=\"Refresh comments. They will be saved when the run is stopped.\""<< endl;
  *out << " value=\"refresh\""                                       << endl;
  *out << "/>"                                                       << endl;
  *out << "  </td>"                                                  << endl;

  *out << "<tr>"                                                     << endl;
  *out << "</table>"                                                 << endl;

  *out << "</form>"                                                  << endl;

  *out << "</body>"                                                  << endl;

  *out << "</html>"                                                  << endl;
}

void emu::daq::manager::Application::processCommentForm(xgi::Input *in)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);

    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    std::vector<cgicc::FormEntry>::iterator fe;

    badRun_ = false;
    for ( fe=fev.begin(); fe!=fev.end(); ++ fe ){
      // Apparently the query string does not even include the checkbox element if it's not checked...
      if ( fe->getName() == "badrun" && fe->getValue() == "on" ) 
	badRun_ = true;
      if ( fe->getName() == "globalrunnumber" )
	globalRunNumber_ = fe->getValue();
      if ( fe->getName() == "comments" )
	comments_ = fe->getValue();
    }

}

void emu::daq::manager::Application::commandWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  processCommandForm(in, out);

    daqState_ = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;

    if ( fsm_.getCurrentState() == 'E' ){
     *out << "<meta http-equiv=\"refresh\" content=\"5\">"              << endl;
    }
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/emu/daq/manager/html/emudaqmanager.css\"/>"      << endl;
    *out << "<title>"                                                  << endl;
    *out << "Emu Local DAQ " << daqState_.toString()                               << endl;
//     *out << xmlClass_ << instance_ << " Version " << emudaqmanager::versions
//         << " CONTROL" << endl;
    *out << "</title>"                                                 << endl;
    *out << "<script type=\"text/javascript\" src=\"/emu/daq/manager/html/emudaqmanager.js\"></script>" << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body onload=\"countSeconds()\">"                         << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "src=\"/emu/daq/manager/images/ctrl64x64.gif\""            << endl;
    *out << "     alt=\"Control\""                                     << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td>"                                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ 
// 	 << " Version " << emudaqmanager::versions 
	 << "  " << fsm_.getStateName(fsm_.getCurrentState())          << endl;
    *out << "    </b>"                                                 << endl;
    *out << "    <br/>Updated at " <<  getDateTime()                   << endl;
    *out << "    &#8212; <span id=\"ageOfPage\"></span> ago "          << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"70\">" << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\" target=\"_top\">"             << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=\"/hyperdaq/images/HyperDAQ.jpg\""             << endl;
    *out << "       alt=\"HyperDAQ\""                                  << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\" target=\"_top\">"             << endl;
    *out << "      HyperDAQ"                                           << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td width=\"32\">"                                      << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">" << endl;
    *out << "    <a href=\"/" << urn_ << "/\" target=\"_top\">"                        << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=";
    *out << "\"/emu/daq/manager/images/EmuDAQManager64x64.gif\""       << endl;
    *out << "       alt=\"Main\""                                      << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/" << urn_ << "/\" target=\"_top\">"                        << endl;
    *out << "      Main"                                               << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    // Get info from TA...
    string runNumber("UNKNOWN");
    string maxNumEvents("UNKNOWN");
    string runStartTime("UNKNOWN");
    string runStopTime("UNKNOWN");
    getRunInfoFromTA( &runNumber, &maxNumEvents, &runStartTime, &runStopTime );
    if ( runNumber    != "UNKNOWN" ) runNumber_.fromString( runNumber );
    if ( maxNumEvents != "UNKNOWN" ) maxNumberOfEvents_.fromString( maxNumEvents );

    // ...but let the user overwrite them by selecting supervised mode.
    *out << "<hr/>"                                                    << endl;
    governorForm( in, out );
    *out << "<hr/>"                                                    << endl;


    printDAQState( out, daqState_.toString() );


    if ( daqState_.toString() == "Enabled" )
      *out << "<br/>Started at " << runStartTime                       << endl;
    if ( daqState_.toString() == "Halted" ){
      *out << "<br/>Started at " << runStartTime 
	   << "<br/>Stopped at " << runStopTime                        << endl;
    }
    *out << "<br/>"                                                    << endl;

    stringstream STEPCountsTable;
    bool         STEPHasFinished = true;
    if ( runType_.toString().find("STEP",0) != string::npos ){
      STEPHasFinished = printSTEPCountsTable( STEPCountsTable, true );
      *out << "STEP has ";
      if ( !STEPHasFinished ) *out << "<span style=\"font-weight: bold; color:#ff0000;\">not</span> ";
      *out << "finished.";
      *out << "<br/>"                                                    << endl;
    }

    *out << "<br/>"                                                    << endl;

    *out << "<form method=\"get\" action=\"/" << urn_ << "/command\">" << endl;

    if ( fsm_.getCurrentState() == 'H' ){

      *out << "Select run type: "                                    << endl;
      *out << "<select"                                              ;
      *out << " name=\"runtype\""                                    ;
      *out << " size=\"1\""                                          ;
      if ( supervisedMode_.value_ ) *out << " disabled=\"true\""         << endl;
      *out << "/>  "                                                 ;
      for ( size_t iType=0; iType<runTypes_.elements(); ++iType ){
	xdata::String* runtype = dynamic_cast<xdata::String*>(runTypes_.elementAt(iType));
	*out << "<option value=\"" << runtype->toString() << "\"";
	if ( runtype->toString() == runType_.toString() )
	  *out << " selected";
	*out << ">" << runtype->toString();
      }
      *out << "</select>"                                            << endl;
      *out << "<br>"                                                 << endl;

      *out << "Set maximum number of events (-1 for unlimited): "    << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"text\""                                       << endl;
      *out << " name=\"maxevents\""                                  << endl;
      *out << " title=\"Readout will stop after this many events.\"" << endl;
      *out << " alt=\"maximum number of events\""                    << endl;
      *out << " value=\"" << maxNumberOfEvents_.toString() << "\""   << endl;
      *out << " size=\"10\""                                         << endl;
      if ( supervisedMode_.value_ ) *out << " disabled=\"true\""     << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

      *out << "Build events: "                                       << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"checkbox\""                                   << endl;
      *out << " name=\"buildevents\""                                << endl;
      *out << " title=\"If checked, events will be built.\""         << endl;
      *out << " alt=\"build events\""                                << endl;
      if ( buildEvents_.value_ ) *out << " checked"                  << endl;
      if ( supervisedMode_.value_ ) *out << " disabled=\"true\""     << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

      *out << "Write bad events only: "                              << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"checkbox\""                                   << endl;
      *out << " name=\"writeBadEventsOnly\""                         << endl;
      *out << " title=\"If checked, only bad events (with some neighboring events) will be written to file. To be used in global runs.\""  << endl;
      *out << " alt=\"writeBadEventsOnly\""                          << endl;
      if ( writeBadEventsOnly_.value_ ) *out << " checked"           << endl;
      if ( supervisedMode_.value_ ) *out << " disabled=\"true\""     << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

    }
    else{ // in a state other than halted
      *out << "<table border=\"0\" rules=\"none\" width=\"100%\">"   << endl;
      *out << "<tr>"                                                 << endl;

      *out << "<td align=\"left\">"                                  << endl;
      *out << "<table border=\"0\" rules=\"none\">"                  << endl;
      *out << "  <tr><td>Run number:</td><td>" << runNumber;
      if ( fsm_.getCurrentState() == 'C' ){
	if ( runType_.toString() == "Debug" || runType_.toString().find("STEP",0) != string::npos ) 
	  *out << " (will <span style=\"font-weight: bold; color:#ff0000;\">not</span> be booked)";
	else
	  *out << " (will be replaced with a booked one)";
      }
      else{
	if ( bool(isBookedRunNumber_) ) 
	  *out << " (booked)";
	else if ( !configuredInSupervisedMode_ ) 
	  *out << " (<span style=\"font-weight: bold; color:#ff0000;\">not</span> booked)";
      }
      *out << "</td></tr>"                                           << endl;
      *out << "  <tr><td>Run type:</td><td>" << runType_.toString();
      *out << "</td></tr>"                                           << endl;
      if ( fsm_.getCurrentState() == 'E' ){
	*out << "  <tr><td>Maximum number of events:</td><td>" << maxNumEvents;
	if ( maxNumEvents.find("-",0) != string::npos ) 
	  *out << " (unlimited)";
      }
      else{
	*out << "  <tr><td>Maximum number of events:</td><td>" << maxNumberOfEvents_.toString();
	if ( maxNumberOfEvents_.toString().find("-",0) != string::npos ) 
	  *out << " (unlimited)";
      }
      *out << "</td></tr>"                                           << endl;
      *out << "  <tr><td>Build events:</td><td>" << buildEvents_ .toString();
      *out << "</td></tr>"                                           << endl;
      *out << "  <tr><td>Write bad events only:</td><td>" << writeBadEventsOnly_.toString();
      *out << "</td></tr>"                                           << endl;
      *out << "</table>"                                             << endl;
      *out << "</td>"                                                << endl;

      *out << "<td width=\"32\"/>"                                   << endl;

      *out << "<td align=\"right\">"                                 << endl;
      printUserComments( out );
      *out << "</td>"                                                << endl;
      *out << "</table>"                                             << endl;
    }



    //
    // Control buttons
    //
    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;

    *out << "<td>"                                                     << endl;
      *out << "<input"                                               << endl;
      *out << " class=\"button\""                                    << endl;
      *out << " type=\"submit\""                                     << endl;
      *out << " name=\"command\""                                    << endl;
      if ( fsm_.getCurrentState() == 'H' )
	*out << " value=\"configure\""                               << endl;
      else if ( fsm_.getCurrentState() == 'E' )
	*out << " value=\"stop\""                                    << endl;
      else if ( fsm_.getCurrentState() == 'C' )
	*out << " value=\"start\""                                   << endl;
      else if ( fsm_.getCurrentState() == 'F' )
	*out << " value=\"reset\""                                   << endl;
      if ( supervisedMode_.value_ ) *out << " disabled=\"true\""         << endl;
      *out << "/>"                                                   << endl;

      // In case the user has changed his mind, allow him to halt from 'configured' state.
      if ( fsm_.getCurrentState() == 'C' )
	{
	  *out << "<input"                                               << endl;
	  *out << " class=\"button\""                                    << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"stop\""                                      << endl;
	  if ( supervisedMode_.value_ ) *out << " disabled=\"true\""         << endl;
	  *out << "/>"                                                   << endl;
	}

      // If DAQ is in "Failed" state, but emu::daq::manager::Application isn't, place a reset button
      // (if emu::daq::manager::Application is in 'failed' state, we should already have one).
      // Also, if emu::daq::manager::Application is in "Halted" state, but DAQ is not (because emu::daq::manager::Application 
      // was restarted, for example), allow DAQ to be reset. 
      if ( ( fsm_.getCurrentState() != 'F' && daqState_.toString() == "Failed" ) ||
	   ( fsm_.getCurrentState() == 'H' && daqState_.toString() != "Halted" )    ){
	  *out << "<input"                                               << endl;
	  *out << " class=\"button\""                                    << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"reset\""                                     << endl;
	  if ( supervisedMode_.value_ ) *out << " disabled=\"true\""         << endl;
	  *out << "/>"                                                   << endl;
      }
      

    *out << "</td>"                                                      << endl;

    *out << "<td/>"                                                      << endl;
    *out << "<tr>"                                                       << endl;
    *out << "</table>"                                                   << endl;
       

    *out << "<br>"                                                       << endl;
    *out << "<br>"                                                       << endl;

    *out << "</form>"                                                    << endl;

    if ( runType_.toString().find("STEP",0) != string::npos )
      *out << STEPCountsTable.str();
    else
      printEventCountsTable( out, "Events read by emu::daq::rui::Applications", getRUIEventCounts(), true );
    *out << "<br/>"                                                      << endl;
    if ( runType_.toString().find("STEP",0) == string::npos && buildEvents_.value_ ){
      printEventCountsTable( out, "Events processed by emu::daq::fu::Applications", getFUEventCounts(), false );      
      *out << "<br/>"                                                    << endl;
    }

    *out << "<br/>"                                                      << endl;
    *out << "<br/>"                                                      << endl;

    *out << "<table border=\"0\">"                                       << endl;
    *out << "<tr valign=\"top\">"                                        << endl;
    *out << "<td>"                                                       << endl;
    statesTableToHtml( out, "DAQ applications", daqContexts_, currentAppStates_ );
    *out << "</td>"                                                      << endl;
    *out << "<td width=\"16\"/>"                                         << endl;
    *out << "<td>"                                                       << endl;
    *out << "</td>"                                                      << endl;
    *out << "</tr>"                                                      << endl;
    *out << "</table>"                                                   << endl;

    *out << "</body>"                                                    << endl;

    *out << "</html>"                                                    << endl;
}

vector<emu::base::WebReportItem> 
emu::daq::manager::Application::materialToReportOnPage1(){
  vector<emu::base::WebReportItem> items;

  string controlURL = getHref( appDescriptor_ ) + "/control";

  // Title
  items.push_back( emu::base::WebReportItem( "title",
                                             "Local DAQ",
                                             "",
                                             "Click to visit the local DAQ Manager page.",
                                             "",
                                             controlURL ) );

  // State
  string state = getDAQState();
  string valueTip;
  if ( ! ( state == "Enabled" || state == "Ready" || state == "Halted" ) ) 
    valueTip = "Local DAQ may need attention. Recreate it, or click to control it manually.";
  items.push_back( emu::base::WebReportItem( "state",
                                             state,
                                             "The overall state of local DAQ.",
                                             valueTip,
                                             controlURL,
                                             controlURL ) );

  // Run number
  items.push_back( emu::base::WebReportItem( "#",
                                             runNumber_.toString(),
                                             "Local run number. In unsupervised mode, this may be different from the global one.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );

  // Run type
  items.push_back( emu::base::WebReportItem( "type",
                                             runType_.toString(),
                                             "Run type.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );

  // Run controller
  items.push_back( emu::base::WebReportItem( "ctrl",
                                             ( bool( isGlobalInControl_ ) ? "global" : "local" ),
                                             "Whether local DAQ is controlled locally or by global DAQ.",
                                             ( bool( isGlobalInControl_ ) ? "Local DAQ was started centrally, in sync with global DAQ." : "Local DAQ was started locally, independently of central DAQ." ),
                                             controlURL,
                                             controlURL ) );

  // Start and end times
  string runNumber("UNKNOWN");
  string maxNumEvents("UNKNOWN");
  string runStartTime("UNKNOWN");
  string runStopTime("UNKNOWN");
  getRunInfoFromTA( &runNumber, &maxNumEvents, &runStartTime, &runStopTime );
  items.push_back( emu::base::WebReportItem( "start",
                                             runStartTime,
                                             "The start time of the local run in UTC.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );

  items.push_back( emu::base::WebReportItem( "stop",
					     ( ( state == "Enabled" || state == "Ready" ) ? "not yet" : runStopTime ), 
                                             "The stop time of the local run in UTC.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );

  // Progress of calibration runs
  items.push_back( emu::base::WebReportItem( "calib runIndex",
                                             calibRunIndex_.toString(),
                                             "Current run's index in the calibration sequence.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );
  items.push_back( emu::base::WebReportItem( "calib nRuns",
                                             calibNRuns_.toString(),
                                             "Total number of runs in the calibration sequence.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );
  items.push_back( emu::base::WebReportItem( "calib stepIndex",
                                             calibStepIndex_.toString(),
                                             "Index of the current step.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );
  items.push_back( emu::base::WebReportItem( "calib nSteps",
                                             calibNSteps_.toString(),
                                             "Total number of steps in this calibration run.",
                                             "Click to visit the local DAQ Manager page.",
                                             controlURL,
                                             controlURL ) );

  // Min and max RUI counts
  int64_t maxCount = -1;
  int64_t minCount = 2000000000;
  int64_t minCountIndex = -1;
  int64_t maxCountIndex = -1;
  vector< map< string,string > > counts = getRUIEventCounts();
  for ( size_t iRUI=0; iRUI<counts.size(); ++iRUI ){
    stringstream ss;
    ss << counts.at(iRUI)["count"];
    int64_t count;
    ss >> count;
    if ( count < minCount ) { minCount = count; minCountIndex = iRUI; }
    if ( count > maxCount ) { maxCount = count; maxCountIndex = iRUI; }
  }
  try{
    stringstream nss;
    nss << "Events read from " << counts.at(minCountIndex)["hwName"] << " by RUI " << counts.at(minCountIndex)["appInst"] << ".";
    items.push_back( emu::base::WebReportItem( "min events",
					       counts.at(minCountIndex)["count"],
					       "The lowest number of events read by any RUI. In local runs, this should remain close to 'max'.",
					       nss.str(),
					       controlURL, controlURL ) );
    stringstream xss;
    xss << "Events read from " << counts.at(maxCountIndex)["hwName"] << " by RUI " << counts.at(maxCountIndex)["appInst"] << ".";
    items.push_back( emu::base::WebReportItem( "max events",
					       counts.at(maxCountIndex)["count"],
					       "The highest number of events read by any RUI.",
					       xss.str(),
					       controlURL, controlURL ) );
  }
  catch( const std::exception& e ){
    LOG4CPLUS_WARN(logger_, "Failed to report min and max number of events to Page 1 : " << e.what() );
    stringstream ss8;
    ss8 <<  "Failed to report min and max number of events to Page 1 : " << e.what() ;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss8.str() );
    this->notifyQualified( "warning", eObj );
  }
  
  return items;
}


emu::base::Fact
emu::daq::manager::Application::findFact( const emu::base::Component& component, const string& factType ) {
  cout << "*** emu::daq::manager::Application::findFact " << component << " : " << factType << endl;

  vector<string> matches;

  if ( factType  == LocalDAQStatusFact::getTypeName() 
       &&
       component == emu::base::Component("emu::daq::manager::Application") ){
    cout << currentAppStates_;
    queryAppStates();
    cout << currentAppStates_;
    daqState_ = currentAppStates_.getCombinedState();
    emu::base::TypedFact<LocalDAQStatusFact> ds;
    ds.setRun( runNumber_.toString() )
      .setComponent( component )
      .setSeverity( emu::base::Fact::INFO )
      .setDescription( "The status of the local DAQ." )
      .setParameter( LocalDAQStatusFact::runType,          runType_.toString()                       )
      .setParameter( LocalDAQStatusFact::state,            fsm_.getStateName(fsm_.getCurrentState()) )
      .setParameter( LocalDAQStatusFact::daqState,         daqState_.toString()                      )
      .setParameter( LocalDAQStatusFact::isSupervised,     supervisedMode_.toString()                )
      .setParameter( LocalDAQStatusFact::isBuildingEvents, buildEvents_.toString()                   );
    cout << ds;
    cout << "emu::daq::manager::Application::findFact ***" << endl;
    return ds;
  }
  else if ( factType == emu::base::ApplicationStatusFact::getTypeName() ){
    if ( component.isMatchedBy( "^(emu::daq::rui::Application|RUI)([0-9]+)$", matches ) ) {
      if ( matches.size() == 3 ){
	stringstream iss( matches[2] );
	uint32_t instance;
	iss >> instance;
	queryAppStates();
	daqState_ = currentAppStates_.getCombinedState();
	for ( vector< xdaq::ApplicationDescriptor* >::iterator ruid=ruiDescriptors_.begin(); ruid!=ruiDescriptors_.end(); ++ruid ){
	  if ( (*ruid)->getInstance() == instance ){
	    map<xdaq::ApplicationDescriptor*, string> cas = currentAppStates_.getAppStates();
	    for ( map<xdaq::ApplicationDescriptor*, string>::iterator s=cas.begin(); s!=cas.end(); ++s ){
	      if ( s->first == (*ruid) ){
		string ruiState = s->second; 
		emu::base::TypedFact<emu::base::ApplicationStatusFact> as;
		as.setRun( runNumber_.toString() )
		  .setComponent( component )
		  .setParameter( emu::base::ApplicationStatusFact::state, ruiState );
		if  ( ruiState == "UNKNOWN" ) 
		  as.setSeverity( emu::base::Fact::FATAL )
		    .setDescription( matches[0] + " is unreachable. It may have crashed, or the network may be down." );
		else if ( ruiState == "Failed"  )
		  as.setSeverity( emu::base::Fact::FATAL )
		    .setDescription( matches[0] + " is in 'Failed' state." );
		else
		  as.setSeverity( emu::base::Fact::INFO  )
		    .setDescription( matches[0] + " is in '" + ruiState + "' state." );
		cout << as;
		cout << "emu::daq::manager::Application::findFact ***" << endl;
		return as;
	      }
	    }
	  }
	}
      } // if ( matches.size() == 3 )
    } // if ( component.isMatchedBy( "^(emu::daq::rui::Application|RUI)([0-9]+)$", matches ) )
    else if ( component == emu::base::Component("emu::daq::manager::Application") ){
      string daqManagerState = fsm_.getStateName(fsm_.getCurrentState());
      emu::base::TypedFact<emu::base::ApplicationStatusFact> as;
      as.setRun( runNumber_.toString() )
	.setComponent( component )
	.setParameter( emu::base::ApplicationStatusFact::state, daqManagerState );
      if ( daqManagerState == "Failed"  )
	as.setSeverity( emu::base::Fact::FATAL )
	  .setDescription( matches[0] + " is in 'Failed' state." );
      else
	as.setSeverity( emu::base::Fact::INFO  )
	  .setDescription( matches[0] + " is in '" + daqManagerState + "' state." );
      cout << as;
      cout << "emu::daq::manager::Application::findFact ***" << endl;
      return as;
    }
  }

  stringstream ss;
  ss << "Failed to find fact of type \"" << factType
     << "\" on component \"" << component
     << "\" requested by expert system.";
  LOG4CPLUS_WARN( logger_, ss.str() );
  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
  this->notifyQualified( "warning", eObj );

  // Return an untyped empty fact if no typed fact was found:
  return emu::base::Fact();
}

emu::base::FactCollection
emu::daq::manager::Application::findFacts() {
  emu::base::FactCollection fc;
  cout << "*** emu::daq::manager::Application::findFacts" << endl;

  // Report DAQ state.
  queryAppStates();
  daqState_ = currentAppStates_.getCombinedState();
  emu::base::TypedFact<LocalDAQStatusFact> ds;
  ds.setComponentId( "emu::daq::manager::Application" )
//   ds.setComponentId( "DAQManager" )
    .setRun( runNumber_.toString() )
    .setSeverity( emu::base::Fact::INFO )
    .setParameter( LocalDAQStatusFact::runType,          runType_.toString()                       )
    .setParameter( LocalDAQStatusFact::state,            fsm_.getStateName(fsm_.getCurrentState()) )
    .setParameter( LocalDAQStatusFact::daqState,         daqState_.toString()                      )
    .setParameter( LocalDAQStatusFact::isSupervised,     supervisedMode_.toString()                )
    .setParameter( LocalDAQStatusFact::isBuildingEvents, buildEvents_.toString()                   );
  fc.addFact( ds );

  // Report crashed RUIs, if any.
  if ( daqState_.toString() == "UNKNOWN" ){
    for ( vector< xdaq::ApplicationDescriptor* >::iterator ruid=ruiDescriptors_.begin(); ruid!=ruiDescriptors_.end(); ++ruid ){
      map<xdaq::ApplicationDescriptor*, string> cas = currentAppStates_.getAppStates();
      for ( map<xdaq::ApplicationDescriptor*, string>::iterator s=cas.begin(); s!=cas.end(); ++s ){
	if ( s->first == (*ruid) && s->second == "UNKNOWN" ){
	  stringstream ruiName;
	  ruiName << (*ruid)->getClassName() << setfill('0') << setw(2) << (*ruid)->getInstance();
// 	  ruiName << "RUI" << setfill('0') << setw(2) << (*ruid)->getInstance();
	  emu::base::TypedFact<emu::base::ApplicationStatusFact> as;
	  as.setComponentId( ruiName.str() )
	    .setRun( runNumber_.toString() )
	    .setSeverity( emu::base::Fact::FATAL )
	    .setDescription( ruiName.str() + " is unreachable. It may have crashed, or the network may be down." )
	    .setParameter( emu::base::ApplicationStatusFact::state, "UNKNOWN" );
	  fc.addFact( as );
	}
      }
    }
  }
  cout << fc << endl;

  cout << "emu::daq::manager::Application::findFacts ***" << endl;
  return fc;
}


void emu::daq::manager::Application::setParametersForSupervisedMode(){
  // Prepare for obeying Central Run Control commands
  globalRunNumber_        = runNumber_.toString();
  isBookedRunNumber_      = true;
}

void emu::daq::manager::Application::getRunInfoFromTA( string* runnum, string* maxevents, string* starttime, string* stoptime ){
    if ( taDescriptors_.size() ){
      if ( taDescriptors_.size() > 1 ){
	LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.");
	stringstream ss9;
	ss9 << "The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.";
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss9.str() );
	this->notifyQualified( "warning", eObj );
      }

      xdata::UnsignedInteger32 runNumber;
      xdata::Integer64    maxNumTriggers;
      xdata::String       runStartTime;
      xdata::String       runStopTime;
      try
	{
	  emu::soap::Messenger( this ).getParameters( taDescriptors_[0],
						      emu::soap::Parameters()
						      .add( "runNumber"     , &runNumber      )
						      .add( "maxNumTriggers", &maxNumTriggers )
						      .add( "runStartTime"  , &runStartTime   )
						      .add( "runStopTime"   , &runStopTime    ) );
	  *runnum    = runNumber.toString()     ;
	  *maxevents = maxNumTriggers.toString();
	  *starttime = runStartTime.toString()  ;
	  *stoptime  = runStopTime.toString()   ;
	  *starttime = reformatTime( *starttime );
	  *stoptime  = reformatTime( *stoptime  );
	}
      catch(xcept::Exception &e)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get run info from TA0: " << 
			  xcept::stdformat_exception_history(e) );
	  stringstream ss10;
	  ss10 << "Failed to get run info from TA0: ";
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss10.str(), e );
	  this->notifyQualified( "error", eObj );
	}
      catch(...)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get run info from TA0.");
	  stringstream ss11;
	  ss11 << "Failed to get run info from TA0.";
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss11.str() );
	  this->notifyQualified( "error", eObj );
	}

    }
    else{
      LOG4CPLUS_ERROR(logger_,"No TA found.");
      stringstream ss12;
      ss12 << "No TA found.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss12.str() );
      this->notifyQualified( "error", eObj );
    }
}

void emu::daq::manager::Application::getTriggerSources()
  // Gets the trigger sources from TTCci
{
  TTCci_ClockSource_   = "UNKNOWN";
  TTCci_OrbitSource_   = "UNKNOWN";
  TTCci_TriggerSource_ = "UNKNOWN";
  TTCci_BGOSource_     = "UNKNOWN";

  vector< xdaq::ApplicationDescriptor* > appDescriptors;
  try
    {
      appDescriptors = getAppDescriptors(zone_, "TTCciControl");
    }
  catch(xcept::Exception &e)
    {
      appDescriptors.clear();
      LOG4CPLUS_WARN(logger_,
		      "Failed to get application descriptors for class TTCciControl: "
		      << stdformat_exception_history(e));
      stringstream ss13;
      ss13 << 
		      "Failed to get application descriptors for class TTCciControl: "
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss13.str(), e );
      this->notifyQualified( "warning", eObj );
    }

  if ( appDescriptors.size() >= 1 ){
    if ( appDescriptors.size() > 1 ){
      LOG4CPLUS_WARN(logger_, "The embarassement of riches: " << 
		      appDescriptors.size() << 
		      " TTCciControl instances found. Trying first one.");
      stringstream ss14;
      ss14 <<  "The embarassement of riches: " << 
		      appDescriptors.size() << 
		      " TTCciControl instances found. Trying first one.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss14.str() );
      this->notifyQualified( "warning", eObj );
    }
    try
      {
	emu::soap::Messenger( this ).getParameters( appDescriptors[0],
						    emu::soap::Parameters()
						    .add( "ClockSource"  , &TTCci_ClockSource_   )
						    .add( "OrbitSource"  , &TTCci_OrbitSource_   )
						    .add( "TriggerSource", &TTCci_TriggerSource_ )
						    .add( "BGOSource"    , &TTCci_BGOSource_     ) );
	
	LOG4CPLUS_INFO(logger_, "Got trigger sources  from TTCciControl:" 
		       << " ClockSource: "   << TTCci_ClockSource_.toString()
		       << " OrbitSource: "   << TTCci_OrbitSource_.toString()
		       << " TriggerSource: " << TTCci_TriggerSource_.toString()
		       << " BGOSource: "     << TTCci_BGOSource_.toString() );
      }
      catch(emu::daq::manager::exception::Exception e)
	{
	  LOG4CPLUS_WARN(logger_,"Failed to get trigger sources from TTCciControl: " << 
			  xcept::stdformat_exception_history(e) );
	  stringstream ss15;
	  ss15 << "Failed to get trigger sources from TTCciControl: ";
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss15.str(), e );
	  this->notifyQualified( "warning", eObj );
	}
      catch(...)
	{
	  LOG4CPLUS_WARN(logger_,"Failed to get trigger sources from TTCciControl."); 
	  stringstream ss16;
	  ss16 << "Failed to get trigger sources from TTCciControl."; 
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss16.str() );
	  this->notifyQualified( "warning", eObj );
	}
    
  }
  else{
    LOG4CPLUS_WARN(logger_, "Did not find TTCciControl. ==> Trigger sources are unknown.");
    stringstream ss17;
    ss17 <<  "Did not find TTCciControl. ==> Trigger sources are unknown.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss17.str() );
    this->notifyQualified( "warning", eObj );
  }
}


uint32_t emu::daq::manager::Application::purgeIntNumberString( string* s ){
  // Emu: purge string of all non-numeric characters
  uint32_t nCharactersErased = 0;
  for ( string::size_type i = s->find_first_not_of("-0123456789",0); 
	i != string::npos; 
	i = s->find_first_not_of("-0123456789",i) ){
    s->erase(i,1);
    nCharactersErased++;
  }
  return nCharactersErased;
}

void emu::daq::manager::Application::processCommandForm(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);

    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    std::vector<cgicc::FormEntry>::iterator fe;

    if ( fev.size() ) webRedirect(in, out);

    //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
    //cout << "QueryString: \"" << env.getQueryString() << "\"" << endl << flush;

    // for ( fe=fev.begin(); fe!=fev.end(); ++fe )
    //   cout << "name: " << fe->getName() << " value: \"" << fe->getValue() << "\"" << " value size: " << fe->getValue().size() << endl << flush;
    // cout << "---------------------------------" << endl << flush;


    // If there is a command from the html form
    cgicc::form_iterator cmdElement = cgi.getElement("command");
    if(cmdElement != cgi.getElements().end())
    {
        string cmdName = (*cmdElement).getValue();

// 	if ( (cmdName == "configure") && fsm_.getCurrentState() == 'H' )
	if ( (cmdName == "configure") )
	  {
	    // Emu: run type will be queried by emu::daq::rui::Applications and emu::daq::fu::Applications
	    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
	      if ( fe->getName() == "runtype" ){
		// cgicc::form_iterator runTypeElement = cgi.getElement("runtype");
		if ( fe->isEmpty() ){
		  LOG4CPLUS_ERROR(logger_, "Failed to get run type from http form");
		  stringstream ss22;
		  ss22 <<  "Failed to get run type from http form";
		  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss22.str() );
		  this->notifyQualified( "error", eObj );
		}
		else{
		  runType_.fromString( fe->getValue() );
		}
	      }

	    // Apparently the query string does not even include the checkbox element if it's not checked...
	    cgicc::form_iterator buildEventsElement = cgi.getElement("buildevents");
	    buildEvents_ = ( buildEventsElement != cgi.getElements().end() );

	    // Emu: set run number to 1. If booking is successful, it will be replaced by the booked one.
	    runNumber_ = 1;

	    cgicc::form_iterator maxEvtElement = cgi.getElement("maxevents");
	    if( maxEvtElement != cgi.getElements().end() ){
	      string maxNumEvents  = (*maxEvtElement).getValue();
	      purgeIntNumberString( &maxNumEvents );
	      maxNumberOfEvents_.fromString( maxNumEvents );
	    }

	    // Check if only bad events are to be written to file.
	    cgicc::form_iterator writeBadEventsOnlyElement = cgi.getElement("writeBadEventsOnly");
	    writeBadEventsOnly_ = ( writeBadEventsOnlyElement != cgi.getElements().end() );

	    // Obviously, global cannot be in control if command is issued from web page
	    isGlobalInControl_ = false;

	    fireEvent("Configure");
	  }
	else if ( (cmdName == "start") && fsm_.getCurrentState() == 'C' )
	  {
	    fireEvent("Enable");
	  }
        else if( cmdName == "stop" )
	  {
	    // Obviously, global cannot be in control if command is issued from web page
	    isGlobalInControl_ = false;

	    fireEvent("Halt");
	  }
        else if( cmdName == "reset" )
	  {
	    // Obviously, global cannot be in control if command is issued from web page
	    isGlobalInControl_ = false;

	    resetAction();
	    try{
	      fsm_.reset();
	    }
	    catch( toolbox::fsm::exception::Exception e ){
	      LOG4CPLUS_ERROR(logger_, "Failed to reset FSM: " << 
			      xcept::stdformat_exception_history(e) );
	      stringstream ss23;
	      ss23 <<  "Failed to reset FSM: ";
	      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss23.str(), e );
	      this->notifyQualified( "error", eObj );
	    }
	    fireEvent("Halt");
	  }
    }

    // If there is a request to mask from the html form
    cmdElement = cgi.getElement("mask");
    if( cmdElement != cgi.getElements().end() )
      {
        string cmdName = (*cmdElement).getValue();
	
	if ( cmdName == "count in" )
	  {
	    maskDDUInputs( true, fev );
	  }
	else if ( cmdName == "count out" )
	  {
	    maskDDUInputs( false, fev );
	  }
      }

    // If there is a request to recycle (i.e., to terminate to be then restarted automatically) an RUI process:
    cmdElement = cgi.getElement("recycle");
    if( cmdElement != cgi.getElements().end() )
      {
        stringstream ss( (*cmdElement).getValue() );
	uint32_t ruiInstance;
	ss >> ruiInstance;
	cout << "Recycle RUI " << ruiInstance << endl << flush;
	try{
	  emu::soap::Messenger( this ).sendCommand( "emu::daq::rui::Application", ruiInstance, "Terminate" );
	}
	catch(xcept::Exception &e){
	  ss.str() = "";
	  ss << "Failed to recycle emu::daq::rui::Application." << (*cmdElement).getValue();
	  XCEPT_RETHROW( xgi::exception::Exception, ss.str(), e );
	}
      }
}

void emu::daq::manager::Application::printParamsTables
(
    xgi::Input                              *in,
    xgi::Output                             *out,
    vector< xdaq::ApplicationDescriptor* >  &appDescriptors,
    vector< vector< pair<string,string> > > paramsOfApps
)
throw (xgi::exception::Exception)
{
  vector< xdaq::ApplicationDescriptor* >::size_type nbApps = appDescriptors.size();
    xdaq::ApplicationDescriptor   *appDescriptor = 0;
    vector< pair<string,string> > params;


    *out << "<table border=\"0\">"                                     << endl;
    *out << "<tr>"                                                     << endl;

    for(vector< xdaq::ApplicationDescriptor* >::size_type i=0; i<nbApps; i++)
    {
        // Put space between each table of application statistics
        if(i != 0)
        {
            *out << "<td width=\"64\"></td>"                           << endl;
        }

        appDescriptor = appDescriptors[i];
        params        = paramsOfApps[i];

        *out << "<td>"                                                 << endl;
        try
        {
            printParamsTable(in, out, appDescriptor, params);
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to print parameter table for ";
            oss << appDescriptor->getClassName();
            oss << appDescriptor->getInstance();
            s = oss.str();

            XCEPT_RETHROW(xgi::exception::Exception, s, e);
        }
        *out << "</td>"                                                << endl;
    }

    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;
}


void emu::daq::manager::Application::printParamsTable
(
    xgi::Input                    *in,
    xgi::Output                   *out,
    xdaq::ApplicationDescriptor   *appDescriptor,
    vector< pair<string,string> > params
)
throw (xgi::exception::Exception)
{
    string        className = appDescriptor->getClassName();
    uint32_t      instance  = appDescriptor->getInstance();
    string        href      = getHref(appDescriptor);
    vector< pair<string,string> >::size_type nbRows = params.size();


    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"  << endl;

    *out << "<tr>"                                                     << endl;
    *out << "  <th colspan=2 align=\"center\">"                        << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      <a href=\"" << href << "\">"                        << endl;
    *out << "        " << className << instance                        << endl;
    *out << "      </a>"                                               << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;

    for(size_t row=0; row<nbRows; row++)
    {
        *out << "<tr>"                                                 << endl;
        *out << "  <td>"                                               << endl;
        *out << "    " << params[row].first                            << endl;
        *out << "  </td>"                                              << endl;
        *out << "  <td>"                                               << endl;
        *out << "    " << params[row].second                           << endl;
        *out << "  </td>"                                              << endl;
        *out << "</tr>"                                                << endl;
    }

    *out << "</table>"                                                 << endl;
}


string emu::daq::manager::Application::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


vector< pair<string,string> > emu::daq::manager::Application::getEventNbFromEVM
(
    xdaq::ApplicationDescriptor *evmDescriptor
)
throw (emu::daq::manager::exception::Exception)
{
  xdata::UnsignedInteger64 eventNb;
  vector< pair<string,string> > params;

  try
    {
      emu::soap::Messenger( this ).getParameters( evmDescriptor, emu::soap::Parameters().add( "lastEventNumberFromTrigger", &eventNb ) );
    }
  catch(xcept::Exception &e)
    {
      XCEPT_RETHROW(emu::daq::manager::exception::Exception, "Failed to get eventNb from EVM", e);
    }
  
  params.push_back(pair<string,string>("eventNb", eventNb.toString()));
  
  return params;
}


vector< vector< pair<string,string> > > emu::daq::manager::Application::getStats
(
    vector< xdaq::ApplicationDescriptor* > &appDescriptors
)
{
    vector< xdaq::ApplicationDescriptor* >::size_type nbApps = appDescriptors.size();
    vector< xdaq::ApplicationDescriptor* >::size_type i      = 0;
    vector< vector< pair<string,string> > > stats;


    for(i=0; i<nbApps; i++)
    {
        stats.push_back(getStats(appDescriptors[i]));
    }

    return stats;
}


vector< pair<string,string> > emu::daq::manager::Application::getStats
(
    xdaq::ApplicationDescriptor *appDescriptor
)
{
    vector< pair<string,string> > stats;
    xdata::String                 s                          = "";
    xdata::Double                 deltaT                     = 0.0;
    xdata::UnsignedInteger64           deltaN                     = 0;
    xdata::Double                 deltaSumOfSquares          = 0.0;
    xdata::UnsignedInteger64           deltaSumOfSizes            = 0;

    bool successfullyRetrieved = false;

    try{
      emu::soap::Messenger( this ).getParameters( appDescriptor,
						  emu::soap::Parameters()
						  .add( "stateName"        , &s                 )
						  .add( "deltaT"           , &deltaT            )
						  .add( "deltaN"           , &deltaN            )
						  .add( "deltaSumOfSquares", &deltaSumOfSquares )
						  .add( "deltaSumOfSizes"  , &deltaSumOfSizes   ) );
      successfullyRetrieved = true;
    } catch ( xcept::Exception &e ){
      stringstream ss;
      ss <<  "Failed to get stats from " << appDescriptor->getClassName() << "." << appDescriptor->getInstance() << ": " ;
      LOG4CPLUS_ERROR(logger_, ss.str() + xcept::stdformat_exception_history(e));
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss.str(), e );
      this->notifyQualified( "error", eObj );
    }

    if ( successfullyRetrieved 
	 && deltaT.isFinite()
	 && deltaN.isFinite()
	 && deltaSumOfSquares.isFinite()
	 && deltaSumOfSizes.isFinite()   ){
      stats.push_back( pair<string,string>( "state"            , s                .toString() ) );
      stats.push_back( pair<string,string>( "deltaT"           , deltaT           .toString() ) );
      stats.push_back( pair<string,string>( "deltaN"           , deltaN           .toString() ) );
      stats.push_back( pair<string,string>( "deltaSumOfSquares", deltaSumOfSquares.toString() ) );
      stats.push_back( pair<string,string>( "deltaSumOfSizes"  , deltaSumOfSizes  .toString() ) );
      if ( deltaT != 0. ){
	xdata::Double throughput = deltaSumOfSizes / deltaT / 1000000.0;
	xdata::Double rate =                deltaN / deltaT / 1000.0;
	stats.push_back( pair<string,string>( "throughput", throughput.toString() + " MB/s" ) );
	stats.push_back( pair<string,string>( "rate"      , rate.toString()       + " kHz"  ) );
      }
      else{
	stats.push_back( pair<string,string>( "throughput", "DIV BY 0" ) );
	stats.push_back( pair<string,string>( "rate"      , "DIV BY 0" ) );
      }
      if ( uint64_t(deltaN) != 0 ){
	xdata::Double average = deltaSumOfSizes / deltaN / 1000.0;
	double meanOfSquares = deltaSumOfSquares / ((double)deltaN);
	double mean          = ((double)deltaSumOfSizes) / ((double)deltaN);
	double squareOfMean  = mean * mean;
	double variance      = meanOfSquares - squareOfMean;
	if ( variance < 0. ) variance = 0.; // Variance maybe negative due to lack of precision
	xdata::Double rms  = sqrt(variance) / 1000.0;
	stats.push_back( pair<string,string>( "average", average.toString() + " kB" ) );
	stats.push_back( pair<string,string>( "rms"    , rms.toString()     + " kB" ) );
      }
      else{
	stats.push_back( pair<string,string>( "average", "DIV BY 0" ) );
	stats.push_back( pair<string,string>( "rms"    , "DIV BY 0" ) );
      }
    }		       
    else{	       
      stats.push_back( pair<string,string>( "state"            , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "deltaT"           , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "deltaN"           , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "deltaSumOfSquares", "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "deltaSumOfSizes"  , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "throughput"       , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "rate"             , "UNKNOWN" ) );
      stats.push_back( pair<string,string>( "rms"              , "UNKNOWN" ) );
    }

    return stats;
}

void emu::daq::manager::Application::getDataPaths(){
  emu::soap::Messenger m( this );
  
  dataFileNames_.clear(); // All RUIs' all data file names
  dataDirNames_.clear(); // All RUIs' data directory names
  for ( vector<xdaq::ApplicationDescriptor*>::iterator rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); ++rui ){
    string ruiHost;
    xdata::Vector<xdata::String> dataFileNames;
    xdata::String dataDirName;
    try{
      m.getParameters( *rui, emu::soap::Parameters().add( "dataFileNames", &dataFileNames ).add( "pathToRUIDataOutFile", &dataDirName ) );
      ruiHost = toolbox::net::URL( (*rui)->getContextDescriptor()->getURL() ).getHost();
    }
    catch(xcept::Exception &e){
      stringstream ss;
      ss << "Failed to get list of data files from " << (*rui)->getClassName() << "." << (*rui)->getInstance() << " : " ;
      LOG4CPLUS_WARN(logger_, ss.str() + xcept::stdformat_exception_history(e));
    }
    // Add this RUI's data file names with the host name prepended:
    for ( size_t i = 0; i < dataFileNames.elements(); ++i ){
      dataFileNames_.push_back( ruiHost + ":" + ( dynamic_cast<xdata::String*> ( dataFileNames.elementAt(i)) )->toString() );
    }
    // Add this RUI's data directory name with the host name prepended:
    dataDirNames_.push_back( ruiHost + ":" + dataDirName.toString() );
  }
}


void emu::daq::manager::Application::createAllAppStates(){
  currentAppStates_.clear();
  if ( buildEvents_.value_ ){ 
    currentAppStates_.insertApps( evmDescriptors_.begin(), evmDescriptors_.end() );
    currentAppStates_.insertApps( buDescriptors_ .begin(), buDescriptors_ .end() );
    currentAppStates_.insertApps( ruDescriptors_ .begin(), ruDescriptors_ .end() );
    currentAppStates_.insertApps( fuDescriptors_ .begin(), fuDescriptors_ .end() );
  }
  currentAppStates_.insertApps( taDescriptors_ .begin(), taDescriptors_ .end() );
  currentAppStates_.insertApps( ruiDescriptors_.begin(), ruiDescriptors_.end() );

  daqContexts_.clear();
  set<xdaq::ApplicationDescriptor*> apps( currentAppStates_.getApps() );
  set<xdaq::ApplicationDescriptor*>::iterator a;
  for ( a=apps.begin(); a!=apps.end(); ++a ){
    // Collect different contexts too
    daqContexts_.insert( (*a)->getContextDescriptor()->getURL() );
  }
}


void emu::daq::manager::Application::queryAppStates(){
  emu::soap::Messenger m( this );
  // Perform query only if state info at least 1 second old.
  if ( currentAppStates_.getAgeInSeconds() > 1 ){
    set<xdaq::ApplicationDescriptor*> apps = currentAppStates_.getApps();
    set<xdaq::ApplicationDescriptor*>::iterator a;
    for ( a=apps.begin(); a!=apps.end(); ++a ){
      xdata::String s( "UNKNOWN" );
      try
	{
	  m.getParameters( *a, emu::soap::Parameters().add( "stateName", &s ) );
	}
      catch(xcept::Exception &e)
	{
	  s = "UNKNOWN";
	  stringstream ss;
	  ss << "Failed to get state of " << (*a)->getClassName() << "." << (*a)->getInstance() << " : " ;
	  LOG4CPLUS_WARN(logger_, ss.str() + xcept::stdformat_exception_history(e));
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss.str(), e );
	  this->notifyQualified( "warning", eObj );
	}
      currentAppStates_.setAppState( *a, s.toString() );
    }
    //cout << "Previous " << previousAppStates_;
    //cout << "Current "  << currentAppStates_;
  }
}

string emu::daq::manager::Application::getDAQState(){
  //cout << "*** emu::daq::manager::Application::getDAQState" << endl;
  // Update previousAppStates_ here because findFact(s) doesn't call this method. (It calls queryAppStates().)
  // Make sure it's only updated if currentAppStates_ is not empty. (It may be empty during configuring, for instance.)
  if ( ! currentAppStates_.isEmpty() ) previousAppStates_ = currentAppStates_;
  queryAppStates();
  reportCrashedApps();
  //cout << "emu::daq::manager::Application::getDAQState ***" << endl;
  if      ( ! currentAppStates_.isEmpty()  ) return currentAppStates_. getCombinedState();
  else if ( ! previousAppStates_.isEmpty() ) return previousAppStates_.getCombinedState();
  return string("");
}

void emu::daq::manager::Application::reportCrashedApps(){
  // Report to expert system any apps that may have creashed since the last query.  
  //cout << "Previous " << previousAppStates_;
  //cout << "Current "  << currentAppStates_;
  // No point checking if states are still empty:
  if ( currentAppStates_.isEmpty() ) return;

  string combinedState = currentAppStates_.getCombinedState();
  if ( combinedState == "UNKNOWN" 
       &&
       !( currentAppStates_.getAppStates() == previousAppStates_.getAppStates() ) ){
    // Look for newly crashed apps
    set<xdaq::ApplicationDescriptor*> previousCrashes = previousAppStates_.getAppsInState( "UNKNOWN" );
    set<xdaq::ApplicationDescriptor*> currentCrashes  = currentAppStates_ .getAppsInState( "UNKNOWN" );
    set<xdaq::ApplicationDescriptor*> newCrashes;
    // set_difference is in <algorithm>
    set_difference(currentCrashes .begin(), currentCrashes .end(),
		   previousCrashes.begin(), previousCrashes.end(),
		   insert_iterator< set<xdaq::ApplicationDescriptor*> >(newCrashes, newCrashes.begin()) );
    // Report newly crashed apps
    for ( set<xdaq::ApplicationDescriptor*>::iterator nca = newCrashes.begin(); nca != newCrashes.end(); ++nca ){
      if ( binary_search( ruiDescriptors_.begin(), ruiDescriptors_.end(), *nca ) ){
	stringstream ruiName;
	ruiName << (*nca)->getClassName() << setfill('0') << setw(2) << (*nca)->getInstance();
	sendFact( ruiName.str(), emu::base::ApplicationStatusFact::getTypeName() );
      }
    }
  }
}


void emu::daq::manager::Application::printDAQState( xgi::Output *out, string state ){
  map<string, string> bgcolor;

  bgcolor["Halted" ] = "#0000ff";
  bgcolor["Ready"  ] = "#ffff00";
  bgcolor["Enabled"] = "#00ff00";
  bgcolor["Mismatch"] = "#008800";
  bgcolor["TimedOut"] = "#888800";
  bgcolor["Failed" ] = "#000000";
  bgcolor["UNKNOWN"] = "#888888";

  map<string, string> color;
  color["Halted" ] = "#000000";
  color["Ready"  ] = "#000000";
  color["Enabled"] = "#000000";
  color["Mismatch"] = "#000000";
  color["TimedOut"] = "#000000";
  color["Failed" ] = "#ffffff";
  color["UNKNOWN"] = "#ffffff";

  map<string, string> decoration;
  decoration["Halted" ] = "none";
  decoration["Ready"  ] = "none";
  decoration["Enabled"] = "none";
  decoration["Mismatch"] = "blink";
  decoration["TimedOut"] = "blink";
  decoration["Failed" ] = "blink";
  decoration["UNKNOWN"] = "none";

  *out << " Local DAQ is in <a href=\"" << getHref( appDescriptor_ ) << "#states\"" 
       << " title=\"click to see applications' states\">";
  *out << "<span align=\"center\" ";
  *out << "style=\"";
  *out << "background-color:" << bgcolor[state];
  *out << "; color:"          << color[state];
  *out << "; text-decoration:"<< decoration[state];
  *out << "\">";
  *out << " " << state << " ";
  *out << "</span>";
  *out << "</a> state, ";
  *out << ( supervisedMode_.value_ ? 
	    "<em>supervised</em> mode (controlled by <span style=\"font-weight:bold; border-color:#000000; border-style:solid; border-width:thin; padding:2px\">Supervisor | Function Manager | Central Run Control</span>)." : 
	    "<em>unsupervised</em> mode (controlled from this page by  <span style=\"font-weight:bold; border-color:#000000; border-style:solid; border-width:thin; padding:2px\">CSC Shift</span>)." ) << endl;
}


void emu::daq::manager::Application::statesTableToHtml( xgi::Output *out,
							string title,
							set<string> &contexts,
							AppStates &as  )
  throw (xgi::exception::Exception)
{
  map<string, string> bgcolor;

  bgcolor["Halted" ] = "#0000ff";
  bgcolor["Ready"  ] = "#ffff00";
  bgcolor["Enabled"] = "#00ff00";
  bgcolor["Mismatch"] = "#008800";
  bgcolor["TimedOut"] = "#888800";
  bgcolor["Failed" ] = "#000000";
  bgcolor["UNKNOWN"] = "#888888";

  map<string, string> color;
  color["Halted" ] = "#000000";
  color["Ready"  ] = "#000000";
  color["Enabled"] = "#000000";
  color["Mismatch"] = "#000000";
  color["TimedOut"] = "#000000";
  color["Failed" ] = "#ffffff";
  color["UNKNOWN"] = "#ffffff";

  map<string, string> decoration;
  decoration["Halted" ] = "none";
  decoration["Ready"  ] = "none";
  decoration["Enabled"] = "none";
  decoration["Mismatch"] = "blink";
  decoration["TimedOut"] = "blink";
  decoration["Failed" ] = "blink";
  decoration["UNKNOWN"] = "none";

  map<xdaq::ApplicationDescriptor*, string> appStates = as.getAppStates();
  // First find out if any application (RU) is in "Mismatch..." or "TimedOut..." state
  bool isMismatch = false;
  bool isTimedOut = false;
  for ( map<xdaq::ApplicationDescriptor*, string>::iterator s 
	  = appStates.begin(); s!=appStates.end(); ++s ){
    if ( s->second.find( "Mismatch", 0 ) != string::npos ) isMismatch = true;
    if ( s->second.find( "TimedOut", 0 ) != string::npos ) isTimedOut = true;
  }

  *out << "<a name=\"states\"/>"                                         << endl;
  *out << "<table frame=\"void\" rules=\"rows|cols\" class=\"params\">"  << endl;
  
  *out << "<tr>"                                                         << endl;
  *out << "  <th colspan=2 align=\"center\">"                            << endl;
  *out << title                                                          << endl;
  *out << "  </th>"                                                      << endl;
  *out << "</tr>"                                                        << endl;
  *out << "<tr>"                                                         << endl;
  *out << "  <th colspan=2>"                                             << endl;
  *out << "   <a class=\"with_popup\" style=\"float: left;\" href=\"" << 
       getHref( appDescriptor_ ) << "\"> Color code";
  map<string, string>::iterator col;
  for ( col=color.begin(); col!=color.end(); ++col ){
    // Don't show color key for "Mismatch" or "TimedOut" if no app is in those states.
    if ( !isMismatch && col->first == "Mismatch" ) continue;
    if ( !isTimedOut && col->first == "TimedOut" ) continue;
    *out << "<span class=\"popup\" ";
    *out << "style=\"";
    *out << "background-color:" << bgcolor[col->first];
    *out << "; color:"          << col->second;
    *out << "\">";
    *out << " " << col->first << " ";
    *out << "</span>";
  }
  *out << "  </a>"                                                       << endl;
  *out << "  </th>"                                                      << endl;
  *out << "</tr>"                                                        << endl;

  set<string>::iterator c;
  for ( c=contexts.begin(); c!=contexts.end(); ++c ){
    {

      *out << "<tr>"                                                    << endl;

      *out << "  <th>"                                                  << endl;
      *out << "    <a href=\"" << *c << "\" target=\"_blank\">" << c->substr(7) << "</a>" << endl;
      *out << "  </th>"                                                 << endl;
      
      *out << "  <td>"                                                  << endl;
      map<xdaq::ApplicationDescriptor*, string>::iterator s;
      for ( s=appStates.begin(); s!=appStates.end(); ++s )
	if ( *c == s->first->getContextDescriptor()->getURL() )
	{
	  stringstream appName;
	  appName << s->first->getClassName() << s->first->getInstance();
	  if ( s->first->getClassName() == "emu::daq::rui::Application" &&
	       hardwareMnemonics_.find( s->first->getInstance() ) != hardwareMnemonics_.end() )
	    appName << "[" << hardwareMnemonics_[s->first->getInstance()] << "]";
	  *out << "     <span align=\"center\" ";
	  *out << "style=\"";
	  string state = s->second;
	  if ( s->second.find( "Mismatch", 0 ) != string::npos ) state = "Mismatch";
	  if ( s->second.find( "TimedOut", 0 ) != string::npos ) state = "TimedOut";
	  *out << "background-color:" << bgcolor[state];
	  *out << "; color:"          << color[state];
	  *out << "; text-decoration:"<< decoration[state];
	  *out << "\">";
	  *out << " " << appName.str() << " ";
	  *out << "</span>" << endl;
      }
      *out << "  </td>"                                                 << endl;

      *out << "</tr>"                                                   << endl;
    }
  }
  *out << "</table>"                                                    << endl;
}


void emu::daq::manager::Application::configureDAQ()
  throw (emu::daq::manager::exception::Exception)
{
    bool evmGenerateDummyTriggers   = taDescriptors_.size()  == 0;
    bool rusGenerateDummySuperFrags = ruiDescriptors_.size() == 0;
    bool busDropEvents              = fuDescriptors_.size()  == 0;

    // Forget previous run number.
    runNumber_ = 1;

    globalRunNumber_ = "";
    // If run number is booked, it will be on "Enable". For the time being, it's not booked.
    isBookedRunNumber_ = false;
    // All runs are assumed bad until started. Reset it to false once it's started.
    badRun_ = true;
    // It may be aborted after "Configure". Reset comment to null once the run is started.
    comments_ = "aborted after configuration";
    // Assumed aborted until started.
    abortedRun_ = true;

    if ( buildEvents_.value_ ){
      try
	{
	  checkThereIsARuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Not enough applications to make a RU builder", e);
	}
      
      try
	{
	  setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell EVM whether or not to generate dummy triggers", e);
	}
      
      try
	{
	  setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell RUs whether or not to generate dummy super-fragments",
			e);
	}

      try
	{
	  setBUsDropEvents(busDropEvents);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell BUs whether or not drop events", e);
	}
    }

    configureTA();

    if ( buildEvents_.value_ ){
      try
	{
	  configureRuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to configure RU builder", e);
	}
    }

    // RUIs
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
	  for( vector<xdaq::ApplicationDescriptor*>::const_iterator rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); ++rui ){
	    configureRUI( *rui );
	  }
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to configure RUIs", e);
        }
    }

    if ( buildEvents_.value_ ){
      // FUs
      if(fuDescriptors_.size() > 0)
	{
	  try
	    {
	      configureFilterFarm();
	    }
	  catch(xcept::Exception &e)
	    {
	      XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			    "Failed to configure filter farm", e);
	    }
	}
    }
}


void emu::daq::manager::Application::startDAQ()
throw (emu::daq::manager::exception::Exception)
{
    bool evmGenerateDummyTriggers   = taDescriptors_.size()  == 0;
    bool rusGenerateDummySuperFrags = ruiDescriptors_.size() == 0;
    bool busDropEvents              = fuDescriptors_.size()  == 0;


    if ( buildEvents_.value_ ){
      try
	{
	  checkThereIsARuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Not enough applications to make a RU builder", e);
	}
      
      try
	{
	  setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell EVM whether or not to generate dummy triggers", e);
	}

      try
	{
	  setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell RUs whether or not to generate dummy super-fragments",
			e);
	}
      
      try
	{
	  setBUsDropEvents(busDropEvents);
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell BUs whether or not drop events", e);
	}
    }

    // If the TA is present then start it as an imaginary trigger
    if(taDescriptors_.size() > 0)
    {
      try
	{
	  emu::soap::Messenger( this ).setParameters( taDescriptors_[0], 
					      emu::soap::Parameters()
					      .add( "runNumber"        , &runNumber_         )
					      .add( "maxNumTriggers"   , &maxNumberOfEvents_ )
					      .add( "isBookedRunNumber", &isBookedRunNumber_ ) );
	  LOG4CPLUS_INFO(logger_,
			 "Set run number to " + runNumber_.toString() + 
			 ", maximum number of events to " + maxNumberOfEvents_.toString() +
			 ", isBookedRunNumber to " + isBookedRunNumber_.toString() );
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run number to " + runNumber_.toString() + 
			", maximum number of events to " + maxNumberOfEvents_.toString(), 
			e);
	}
      try
        {
            enableTA();
        }
      catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to start trigger", e);
        }
    }

    if ( buildEvents_.value_ ){
      try
	{
	  startRuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to start RU builder", e);
	}
    }

    // If RUIs are present then start them as an imaginary FED builder
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
	  for( vector<xdaq::ApplicationDescriptor*>::const_iterator rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); ++rui ){
	    enableRUI( *rui );
	  }
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to enable RUIs: ", e);
        }
    }

    if ( buildEvents_.value_ ){
      // If FUs are present then start them as an imafinary filter farm
      if(fuDescriptors_.size() > 0)
	{
	  try
	    {
	      startFilterFarm();
	    }
	  catch(xcept::Exception &e)
	    {
	      XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			    "Failed to start filter farm", e);
	    }
	}
    }

    // Once started, all runs are assumed good until proven otherwise.
    badRun_ = false;
    // No comment by default.
    comments_ = "";
    // If it's gotten this far, this run is obviously not aborted.
    abortedRun_ = false;
}


void emu::daq::manager::Application::checkThereIsARuBuilder()
throw (emu::daq::manager::exception::Exception)
{
    if(evmDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emu::daq::manager::exception::Exception, "EVM not found");
    }

    if(buDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emu::daq::manager::exception::Exception, "No BUs");
    }

    if(ruDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emu::daq::manager::exception::Exception, "No RUs");
    }
}


void emu::daq::manager::Application::setEVMGenerateDummyTriggers(const bool value)
throw (emu::daq::manager::exception::Exception)
{
    xdata::Boolean v( value );
    
    try
    {
        emu::soap::Messenger( this ).setParameters( evmDescriptors_[0], emu::soap::Parameters().add( "generateDummyTriggers", &v ) );
    }
    catch(xcept::Exception &e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to set generateDummyTriggers of ";
        oss << evmDescriptors_[0]->getClassName();
        oss << evmDescriptors_[0]->getInstance();
        oss << " to " << v.toString();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


void emu::daq::manager::Application::setRUsGenerateDummySuperFrags(const bool value)
throw (emu::daq::manager::exception::Exception)
{
    xdata::Boolean v( value );
    
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;

    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
	    emu::soap::Messenger( this ).setParameters( *pos, emu::soap::Parameters().add( "generateDummySuperFragments", &v ) );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to set generateDummySuperFragments of ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            oss << " to " << v.toString();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::setBUsDropEvents(const bool value)
throw (emu::daq::manager::exception::Exception)
{
    xdata::Boolean v( value );
    
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;

    for(pos = buDescriptors_.begin(); pos != buDescriptors_.end(); pos++)
    {
        try
        {
	    emu::soap::Messenger( this ).setParameters( *pos, emu::soap::Parameters().add( "dropEventData", &v ) );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to set dropEventData of ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            oss << " to " << v.toString();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}



void emu::daq::manager::Application::enableTA(){
    /////////////////
    // Enable TA   //
    /////////////////

    try
    {
        emu::soap::Messenger( this ).sendCommand( taDescriptors_[0], "Enable" );
    }
    catch(xcept::Exception &e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to enable ";
        oss << taDescriptors_[0]->getClassName();
        oss << taDescriptors_[0]->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


void emu::daq::manager::Application::configureRuBuilder()
throw (emu::daq::manager::exception::Exception)
{
    emu::soap::Messenger m( this );
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////////
    // Configure EVM //
    ///////////////////

    try
    {
        m.sendCommand( evmDescriptors_[0], "Configure");
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to configure EVM", e);
    }


    ///////////////////
    // Configure BUs //
    ///////////////////

    for(pos = buDescriptors_.begin(); pos != buDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Configure" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }


    ///////////////////
    // Configure RUs //
    ///////////////////

    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Configure" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}

void emu::daq::manager::Application::startRuBuilder()
throw (emu::daq::manager::exception::Exception)
{
    emu::soap::Messenger m( this );

    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;
    ////////////////
    // Enable RUs //
    ////////////////

    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Enable" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }


    ////////////////
    // Enable EVM //
    ////////////////

    try
    {
        m.sendCommand( evmDescriptors_[0], "Enable" );
    }
    catch(xcept::Exception &e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to enable EVM", e);
    }


    ////////////////
    // Enable BUs //
    ////////////////

    for(pos = buDescriptors_.begin(); pos != buDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Enable" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::enableRUI( xdaq::ApplicationDescriptor* ruiDescriptor ){
  try
    {
      emu::soap::Messenger( this ).sendCommand( ruiDescriptor, "Enable" );
    }
  catch(xcept::Exception &e)
    {
      stringstream oss;
      string       s;
      
      oss << "Failed to enable ";
      oss << ruiDescriptor->getClassName() << ruiDescriptor->getInstance();
      s = oss.str();
      
      XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}

void
emu::daq::manager::Application::configureTA(){
  // If the TA is present then start it as an imaginary trigger
  if ( taDescriptors_.size() ){
    if ( taDescriptors_.size() > 1 ){
      LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		     " TA instances found. Will use TA0.");
      stringstream ss30;
      ss30 << "The embarassment of riches: " << taDescriptors_.size() <<
	" TA instances found. Will use TA0.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss30.str() );
      this->notifyQualified( "warning", eObj );
    }
    
    try
      {
	emu::soap::Messenger( this ).setParameters( taDescriptors_[0], 
						    emu::soap::Parameters()
						    .add( "runNumber"     , &runNumber_         )
						    .add( "maxNumTriggers", &maxNumberOfEvents_ ) );
	LOG4CPLUS_INFO(logger_,
		       "Set run number to " + runNumber_.toString() + 
		       ", maximum number of events to " + maxNumberOfEvents_.toString() );
      }
    catch(xcept::Exception &e)
      {
	XCEPT_RETHROW(emu::daq::manager::exception::Exception,
		      "Failed to set run number to " + runNumber_.toString() + 
		      ", maximum number of events to " + maxNumberOfEvents_.toString(), 
		      e);
      }
    
    try
      {
        emu::soap::Messenger( this ).sendCommand( taDescriptors_[0], "Configure" );
      }
    catch(xcept::Exception &e)
      {
        stringstream oss;
        string       s;
	
        oss << "Failed to configure ";
        oss << taDescriptors_[0]->getClassName();
        oss << taDescriptors_[0]->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
      }
  }
  else{
    LOG4CPLUS_ERROR(logger_,"No TA found.");
    stringstream ss31;
    ss31 << "No TA found.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss31.str() );
    this->notifyQualified( "error", eObj );
  }
}

void emu::daq::manager::Application::configureRUI( xdaq::ApplicationDescriptor* ruiDescriptor ){
    emu::soap::Messenger m( this );

    stringstream app;
    app << ruiDescriptor->getClassName() << ruiDescriptor->getInstance();
    try
      {
	m.setParameters( ruiDescriptor, 
			 emu::soap::Parameters()
			 .add( "runType"              , &runType_     )
			 .add( "writeBadEventsOnly"   , &writeBadEventsOnly_ )
			 .add( "passDataOnToRUBuilder", &buildEvents_ ) );
	LOG4CPLUS_INFO(logger_,"Set run type for " + app.str() + " to " + runType_.toString() + ", event building " + buildEvents_.toString());
      }
    catch(xcept::Exception &e)
      {
	XCEPT_RETHROW(emu::daq::manager::exception::Exception,
		      "Failed to set run type for " + app.str() + " to " + runType_.toString() + ", event building " + buildEvents_.toString(), e);
      }
    
    try
      {
	m.sendCommand( ruiDescriptor, "Configure" );
      }
    catch(xcept::Exception &e)
      {
	stringstream oss;
	string       s;
	
	oss << "Failed to configure ";
	oss << ruiDescriptor->getClassName() << ruiDescriptor->getInstance();
	s = oss.str();
	
	XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
      }

}


void emu::daq::manager::Application::configureFilterFarm()
  //throw (emu::daq::manager::exception::Exception)
throw (xcept::Exception)
{
    emu::soap::Messenger m( this );
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;

    ///////////////////
    // Configure FUs //
    ///////////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
      stringstream  app;
      app << (*pos)->getClassName() << (*pos)->getInstance();
      try
	{
	  m.setParameters( *pos, 
			   emu::soap::Parameters()
			   .add( "runType"    , &runType_     )
			   .add( "CSCConfigId", &CSCConfigId_ ) 
			   .add( "TFConfigId" , &TFConfigId_  ) );
	  
	  LOG4CPLUS_INFO(logger_,
			 "Set run type for " + app.str() + " to " + runType_.toString() +
			 ", CSC Config Id to " + CSCConfigId_.toString() +
			 ", TF Config Id to " + TFConfigId_.toString()
			 );
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run type for " + app.str() + " to " + runType_.toString() +
			 ", CSC Config Id to " + CSCConfigId_.toString() +
			 ", TF Config Id to " + TFConfigId_.toString(), e);
	}



        try
        {
	    m.sendCommand( *pos, "Configure" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::startFilterFarm()
throw (emu::daq::manager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;

    ////////////////
    // Enable FUs //
    ////////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
        try
        {
	    emu::soap::Messenger( this ).sendCommand( *pos, "Enable" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to enable ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::stopDAQ()
throw (emu::daq::manager::exception::Exception)
{

    // Instruct TA to generate stop time now, otherwise it'll be too late for emu::daq::rui::Applications.
    emu::soap::Messenger( this ).sendCommand( taDescriptors_[0], "generateRunStopTime" );

    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            stopFedBuilder();
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to stop emu::daq::rui::Applications", e);
        }
    }

    if ( buildEvents_.value_ ){
      try
	{
	  stopRuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to stop RU builder", e);
	}
    }

    // If imaginary trigger was started
    if(taDescriptors_.size() > 0)
    {
        try
        {
            stopTrigger();
        }
        catch(xcept::Exception &e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to stop trigger", e);
        }
    }

    if ( buildEvents_.value_ ){
      // If imaginary filter farm was started
      if(fuDescriptors_.size() > 0)
	{
	  try
	    {
	      stopFilterFarm();
	    }
	  catch(xcept::Exception &e)
	    {
	      XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			    "Failed to stop filter farm", e);
	    }
	}
    }
}


void emu::daq::manager::Application::resetDAQ()
throw (emu::daq::manager::exception::Exception)
{
  stringstream oss;

  // Reset emu::daq::rui::Applications
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            resetApps( ruiDescriptors_ );
        }
        catch(xcept::Exception &e)
        {
	  // Don't rethrow here, but log an error message and go ahead and reset the rest
//             XCEPT_RETHROW(emu::daq::manager::exception::Exception,
//                 "Failed to reset emu::daq::rui::Applications", e);
            oss << " Failed to reset emu::daq::rui::Applications: " << xcept::stdformat_exception_history(e);
	    LOG4CPLUS_ERROR(logger_, "Failed to reset emu::daq::rui::Applications: " << xcept::stdformat_exception_history(e) );
	    stringstream ss35;
	    ss35 <<  "Failed to reset emu::daq::rui::Applications: "  ;
	    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss35.str(), e );
	    this->notifyQualified( "error", eObj );
        }
    }

    if ( buildEvents_.value_ ){
      // Builder cannot be reset. Halt it instead.
      try
	{
	  stopRuBuilder();
	}
      catch(xcept::Exception &e)
	{
	  // Don't rethrow here, but log an error message and go ahead and reset the rest
	  //         XCEPT_RETHROW(emu::daq::manager::exception::Exception,
	  //             "Failed to stop RU builder", e);
	  oss << " Failed to stop RU builder: " << xcept::stdformat_exception_history(e);
	  LOG4CPLUS_ERROR(logger_, "Failed to stop RU builder: " << xcept::stdformat_exception_history(e) );
	  stringstream ss36;
	  ss36 <<  "Failed to stop RU builder: "  ;
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss36.str(), e );
	  this->notifyQualified( "error", eObj );
	}
    }

    // emu::daq::ta::Application cannot be reset. (Should it?) Halt it instead.
    if(taDescriptors_.size() > 0)
    {
        try
        {
            stopTrigger();
        }
        catch(xcept::Exception &e)
        {
	  // Don't rethrow here, but log an error message and go ahead and reset the rest
//             XCEPT_RETHROW(emu::daq::manager::exception::Exception,
//                 "Failed to stop trigger", e);
	  oss << " Failed to stop emu::daq::ta::Application: " << xcept::stdformat_exception_history(e);
	  LOG4CPLUS_ERROR(logger_, "Failed to stop emu::daq::ta::Application: " << xcept::stdformat_exception_history(e) );
	  stringstream ss37;
	  ss37 <<  "Failed to stop emu::daq::ta::Application: "  ;
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss37.str(), e );
	  this->notifyQualified( "error", eObj );
        }
    }


    if ( buildEvents_.value_ ){
      // Reset emu::daq::fu::Applications
      if(fuDescriptors_.size() > 0)
	{
	  try
	    {
	      resetApps( fuDescriptors_ );
	    }
	  catch(xcept::Exception &e)
	    {
	      // Don't rethrow here, but log an error message and go ahead and reset the rest
	      //             XCEPT_RETHROW(emu::daq::manager::exception::Exception,
	      //                 "Failed to reset emu::daq::fu::Applications", e);
	      oss << " Failed to reset emu::daq::fu::Applications: " << xcept::stdformat_exception_history(e);
	      LOG4CPLUS_ERROR(logger_, "Failed to reset emu::daq::fu::Applications: " << xcept::stdformat_exception_history(e) );
	      stringstream ss38;
	      ss38 <<  "Failed to reset emu::daq::fu::Applications: "  ;
	      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss38.str(), e );
	      this->notifyQualified( "error", eObj );
	    }
	}
    }

    if ( oss.str().size() ){
      XCEPT_RAISE(emu::daq::manager::exception::Exception, "Failed to reset DAQ: " + oss.str() );
    }
    
}

void emu::daq::manager::Application::resetApps( vector< xdaq::ApplicationDescriptor* > apps )
  throw (emu::daq::manager::exception::Exception){

  emu::soap::Messenger m( this );

  stringstream oss;

    for( vector< xdaq::ApplicationDescriptor* >::iterator pos = apps.begin(); pos != apps.end(); pos++)
    {
        try
        {
	  m.sendCommand( *pos, "Reset" );
        }
        catch(xcept::Exception &e)
        {
	  // Don't rethrow here, but log an error message and go ahead and reset the rest
            oss << (*pos)->getClassName() 
		<< (*pos)->getInstance() << " "
		<< xcept::stdformat_exception_history(e) << " | ";

	    LOG4CPLUS_ERROR(logger_, "Failed to reset " 
			    << (*pos)->getClassName() 
			    << (*pos)->getInstance() << " "
			    << xcept::stdformat_exception_history(e) );
	    stringstream ss39;
	    ss39 <<  "Failed to reset " 
			    << (*pos)->getClassName() 
			    << (*pos)->getInstance() << " "
			     ;
	    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss39.str(), e );
	    this->notifyQualified( "error", eObj );
        }
    }

    if ( oss.str().size() ){
      XCEPT_RAISE(emu::daq::manager::exception::Exception, "Failed to reset: " + oss.str() );
    }
}


void emu::daq::manager::Application::stopRuBuilder()
throw (emu::daq::manager::exception::Exception)
{
    emu::soap::Messenger m( this );

    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    //////////////
    // Halt EVM //
    //////////////

    try
    {
        m.sendCommand( evmDescriptors_[0], "Halt" );
    }
    catch(xcept::Exception &e)
    {
//         XCEPT_RETHROW(emu::daq::manager::exception::Exception,
//             "Failed to halt EVM", e);
	// Don't raise exception here. Go on to try to stop the others.
	LOG4CPLUS_ERROR(logger_, "Failed to halt EVM : " << xcept::stdformat_exception_history(e));
	stringstream ss40;
	ss40 <<  "Failed to halt EVM : " ;
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss40.str(), e );
	this->notifyQualified( "error", eObj );
    }


    //////////////
    // Halt BUs //
    //////////////

    for(pos = buDescriptors_.begin(); pos != buDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Halt" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to halt ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others.
	    LOG4CPLUS_ERROR(logger_, s << " : " << xcept::stdformat_exception_history(e));
	    stringstream ss41;
	    ss41 <<  s << " : " ;
	    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss41.str(), e );
	    this->notifyQualified( "error", eObj );
        }
    }


    //////////////
    // Halt RUs //
    //////////////

    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Halt" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to halt ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others.
	    LOG4CPLUS_ERROR(logger_, s << " : " << xcept::stdformat_exception_history(e));
	    stringstream ss42;
	    ss42 <<  s << " : " ;
	    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss42.str(), e );
	    this->notifyQualified( "error", eObj );
        }
    }
}


void emu::daq::manager::Application::stopFedBuilder()
throw (emu::daq::manager::exception::Exception)
{
    emu::soap::Messenger m( this );
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt RUIs //
    ///////////////

    stringstream oss_all;

    for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Halt" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to halt ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others first.
	    LOG4CPLUS_ERROR(logger_, s );
	    stringstream ss43;
	    ss43 <<  s ;
	    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss43.str() );
	    this->notifyQualified( "error", eObj );

	    oss_all << " | " << s << " : " << xcept::stdformat_exception_history(e);
        }
    }
    if ( oss_all.str() != "" )
      XCEPT_RAISE( emu::daq::manager::exception::Exception, oss_all.str() );
}


void emu::daq::manager::Application::stopTrigger()
throw (emu::daq::manager::exception::Exception)
{
    /////////////
    // Halt TA //
    /////////////

    try
    {
        emu::soap::Messenger( this ).sendCommand( taDescriptors_[0], "Halt" );
    }
    catch(xcept::Exception &e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to halt ";
        oss << taDescriptors_[0]->getClassName() << taDescriptors_[0]->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


void emu::daq::manager::Application::stopFilterFarm()
throw (emu::daq::manager::exception::Exception)
{
    emu::soap::Messenger m( this );
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt FUs  //
    ///////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
        try
        {
            m.sendCommand( *pos, "Halt" );
        }
        catch(xcept::Exception &e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to halt ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::exportParams(xdata::InfoSpace *s)
{

  s->addItemChangedListener("stateName",this);

  supervisedMode_             = true;
  configuredInSupervisedMode_ = true;
  s->fireItemAvailable( "supervisedMode",  &supervisedMode_  );
  s->fireItemAvailable( "configuredInSupervisedMode",  &configuredInSupervisedMode_  );

  s->addItemChangedListener("supervisedMode",this);

  runDbBookingCommand_ = "java -jar runnumberbooker.jar";
  runDbWritingCommand_ = "java -jar runinfowriter.jar";
  runDbAddress_        = "dbc:oracle:thin:@oracms.cern.ch:10121:omds";
  runDbUserFile_       = "";
  s->fireItemAvailable( "runDbBookingCommand", &runDbBookingCommand_ );
  s->fireItemAvailable( "runDbWritingCommand", &runDbWritingCommand_ );
  s->fireItemAvailable( "runDbAddress",        &runDbAddress_        );
  s->fireItemAvailable( "runDbUserFile",       &runDbUserFile_       );


  runNumber_         = 1;
  maxNumberOfEvents_ = 0;
  runType_           = "Monitor";
  isGlobalInControl_ = false;
  buildEvents_       = false;
  calibRunIndex_     = 0;
  calibNRuns_        = 0;
  calibStepIndex_    = 0;
  calibNSteps_       = 0;
  s->fireItemAvailable("runNumber",         &runNumber_        );
  s->fireItemAvailable("maxNumberOfEvents", &maxNumberOfEvents_);
  s->fireItemAvailable("runStartTime",      &runStartTime_);
  s->fireItemAvailable("runType",           &runType_          );
  s->fireItemAvailable("runTypes",          &runTypes_         );
  s->fireItemAvailable("isGlobalInControl", &isGlobalInControl_);
  s->fireItemAvailable("buildEvents",       &buildEvents_      );
  s->fireItemAvailable("calibRunIndex" ,    &calibRunIndex_    );
  s->fireItemAvailable("calibNRuns"    ,    &calibNRuns_       );
  s->fireItemAvailable("calibStepIndex",    &calibStepIndex_   );
  s->fireItemAvailable("calibNSteps"   ,    &calibNSteps_      );
  s->fireItemAvailable("dataFileNames",     &dataFileNames_    );
  s->fireItemAvailable("dataDirNames",      &dataDirNames_     );

  s->addItemRetrieveListener("runStartTime" ,this);
  s->addItemRetrieveListener("dataFileNames",this);
  s->addItemRetrieveListener("dataDirNames" ,this);

  // Parameters to obtain from TTCciControl
  TTCci_ClockSource_   = "UNKNOWN";
  TTCci_OrbitSource_   = "UNKNOWN";
  TTCci_TriggerSource_ = "UNKNOWN";
  TTCci_BGOSource_     = "UNKNOWN";
  s->fireItemAvailable("TTCci_ClockSource",   &TTCci_ClockSource_);
  s->fireItemAvailable("TTCci_OrbitSource",   &TTCci_OrbitSource_);
  s->fireItemAvailable("TTCci_TriggerSource", &TTCci_TriggerSource_);
  s->fireItemAvailable("TTCci_BGOSource",     &TTCci_BGOSource_);
  
    writeBadEventsOnly_ = true;
    s->fireItemAvailable("writeBadEventsOnly",&writeBadEventsOnly_);

    daqState_ = "UNKNOWN";
    s->fireItemAvailable("daqState",&daqState_);
    s->addItemRetrieveListener("daqState",this);

    STEPCount_ = 0;
    s->fireItemAvailable("STEPCount",&STEPCount_);
    s->addItemRetrieveListener("STEPCount",this);

    STEPFinished_ = false;
    s->fireItemAvailable("STEPFinished",&STEPFinished_);
    s->addItemRetrieveListener("STEPFinished",this);

    hardwareMapping_ = "/emu/daq/xml/RUI-to-chamber_mapping.xml";
    s->fireItemAvailable("hardwareMapping",&hardwareMapping_);
}


void emu::daq::manager::Application::printSoapMsgToStdOut(xoap::MessageReference message)
{
    DOMNode *node = message->getEnvelope();
    string msgStr;


    xoap::dumpTree(node, msgStr);

    cout << "*************** MESSAGE START ****************\n";
    cout << msgStr << "\n";
    cout << "*************** MESSAGE FINISH ***************\n";
    cout << flush;
}

vector< map< string,string > > emu::daq::manager::Application::getRUIEventCounts()
{
  vector< map< string,string > > ec;

  emu::soap::Messenger m( this );

  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = ruiDescriptors_.begin(); rui!=ruiDescriptors_.end(); ++rui ){
    stringstream appInst;
    string       appURL;
    string       hwName = "chambers"; 
    string       hwMapURL;
    string       count;
    xdata::UnsignedInteger64 nEventsRead;
    xdata::String       dduError;
 
    try
      {
	appInst << setfill('0') << setw(2) << (*rui)->getInstance();
	if ( hardwareMnemonics_.find( (*rui)->getInstance() ) != hardwareMnemonics_.end() ){
	  hwName = hardwareMnemonics_[(*rui)->getInstance()];
	}
	hwMapURL = appDescriptor_->getContextDescriptor()->getURL() + "/" +
	  hardwareMapping_.toString() + "#RUI." + appInst.str();
	appURL = getHref( *rui );

	m.getParameters( *rui, 
			 emu::soap::Parameters()
			 .add( "nEventsRead"       , &nEventsRead )
			 .add( "persistentDDUError", &dduError    ) );
	count = nEventsRead.toString();
      }
    catch(xcept::Exception &e)
    {
      appURL = getHref( appDescriptor_ ) + "/control"; // self
      count  = "UNKNOWN";
      LOG4CPLUS_WARN(logger_, "Failed to get event count of emu::daq::rui::Application." << appInst.str()
		     << " : " << xcept::stdformat_exception_history(e));
      stringstream ss51;
      ss51 <<  "Failed to get event count of emu::daq::rui::Application." << appInst.str()
		     << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss51.str(), e );
      this->notifyQualified( "warning", eObj );
    }
    map< string,string > sm;
    sm[ "appName"  ] = "emu::daq::rui::Application";
    sm[ "appInst"  ] = appInst.str();
    sm[ "appURL"   ] = appURL;
    sm[ "hwName"   ] = hwName;
    sm[ "hwMapURL" ] = hwMapURL;
    sm[ "count"    ] = count;
    sm[ "dduError" ] = dduError.toString();
    ec.push_back( sm );
  }

  return ec;
}

vector< map< string,string > > emu::daq::manager::Application::getFUEventCounts()
{
  vector< map< string,string > > ec;

  emu::soap::Messenger m( this );  

  xdata::UnsignedInteger64 totalProcessed = 0;
  vector< xdaq::ApplicationDescriptor* >::iterator fu;
  for ( fu = fuDescriptors_.begin(); fu!=fuDescriptors_.end(); ++fu ){
    stringstream appInst;
    string       appURL;
    string       count;
    try
    {
      appURL  = getHref( *fu );
      appInst << setfill('0') << setw(2) << (*fu)->getInstance();
      
      xdata::UnsignedInteger64 nProcessed;
      m.getParameters( *fu, emu::soap::Parameters().add( "nbEventsProcessed", &nProcessed ) );
      count = nProcessed.toString();
      if ( ! nProcessed.isNaN() ) totalProcessed = totalProcessed + nProcessed;
    }
    catch(xcept::Exception &e)
    {
      appURL = getHref( appDescriptor_ ) + "/control"; // self
      count = "UNKNOWN";
      LOG4CPLUS_WARN(logger_, "Failed to get event count of emu::daq::fu::Application." << appInst.str()
		     << " : " << xcept::stdformat_exception_history(e));
      stringstream ss52;
      ss52 <<  "Failed to get event count of emu::daq::fu::Application." << appInst.str()
		     << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss52.str(), e );
      this->notifyQualified( "warning", eObj );
    }
    map< string,string > sm;
    sm[ "appName"  ] = "emu::daq::fu::Application";
    sm[ "appInst"  ] = appInst.str();
    sm[ "appURL"   ] = appURL;
    sm[ "count"    ] = count;
    ec.push_back( sm );
  }
  map< string,string > smt;
  smt[ "appInst"  ] = "Total";
  smt[ "appURL"   ] = getHref( appDescriptor_ ) + "/control";;
  smt[ "count"    ] = totalProcessed.toString();
  ec.push_back( smt );

  return ec;
}


void emu::daq::manager::Application::printEventCountsTable
(
    xgi::Output                      *out,
    string                           title,
    vector< map< string,string > >   counts,
    bool                             control
)
{
    const size_t superColWidth = ( control ? 4 : 3 ); // [columns]
    size_t nCounts = counts.size();
    if ( nCounts == 0 ) return;
    size_t nSuperCols = 6;
    if ( nCounts < nSuperCols ) nSuperCols = nCounts;
    size_t nRows = nCounts/nSuperCols + (nCounts%nSuperCols?1:0);

    if ( control ) *out << "<form name=\"countTable\" method=\"get\" action=\"/" << urn_ << "/command\">" << endl;

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;

    *out << "<tr>"                                                     << endl;
    *out << "  <th colspan=" << nSuperCols*superColWidth << " align=\"center\">" << endl;
    *out << "    <b>"                                                  << endl;
    *out << title                                                      << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;

    for (size_t row=0; row<nRows; row++){
      *out << "<tr>"                                                   << endl;
      for (size_t superCol=0; superCol<nSuperCols; superCol++){
// 	size_t iCount = superCol * nRows + row;
	size_t iCount = row * nSuperCols + superCol;

	  if ( iCount < nCounts ){
	    *out << "  <th align=\"center\">"                          << endl;
	    *out << "      <a href=\"" <<counts[iCount]["appURL"] << "\"";
	    if ( counts[iCount].find("appName") != counts[iCount].end() )
	      *out <<         " title=\"click to visit "
		   <<           counts[iCount]["appName"] << "."
		   <<           counts[iCount]["appInst"] << "\"";
	    *out <<         " target=\"_blank\">"
		 <<             counts[iCount]["appInst"]
		 <<       "</a>"                                       << endl;
	    *out << "  </th>"                                          << endl;
	  }
	  else{
	    *out << "  <td/>"                                          << endl;
	  }

	  *out << "  <td align=\"left\" style=\"padding:0 15px 0 5px;\">" << endl;
	  if ( iCount < nCounts ){
	    if ( counts[iCount].find("hwName") != counts[iCount].end() ){ // have element for hardware (chamber) name
	      *out << "      <a href=\"" << counts[iCount]["hwMapURL"] << "\""
	           <<         " title=\"click to see which chambers are read out by "
	           <<           counts[iCount]["appName"] << "."
	           <<           counts[iCount]["appInst"] << "\""
	           <<         " target=\"_blank\">"
	           <<           counts[iCount]["hwName"] << "</a>"     << endl;
	    }
	  }
	  *out << "  </td>"                                            << endl;

	  *out << "  <td align=\"right\" style=\"padding:0 15px 0 5px;\">"<< endl;
	  if ( iCount < nCounts ){
	      *out << "    " << counts[iCount]["count"]                << endl;
	  }
	  *out << "  </td>"                                            << endl;

	  if ( control ){
	    *out << "  <td align=\"right\" style=\"padding:0 15px 0 5px;\">"<< endl;
	    if ( iCount < nCounts ){
	      *out << "<input"
		   << " class=\"button\""
		   << " type=\"submit\""
		   << " value=\"&#x2672;\""
		   << " name=\"" << counts[iCount]["appInst"] << "\""
		   << " onclick=\"onRecycle( '" << counts[iCount]["appInst"] << "' )\""
		   << " title=\"Recycle " << counts[iCount]["appName"]
		   << "."                 << counts[iCount]["appInst"]
		   << ". This may take a couple of minutes.\"/>"            << endl;
	    }
	    *out << "  </td>"                                               << endl;
	  } // if ( control )
      } // for (size_t superCol=0; superCol<nSuperCols; superCol++){
      *out << "</tr>"                                                  << endl;
    } // for (size_t row=0; row<nRows; row++){

    *out << "</table>"                                                 << endl;
    if ( control ) *out << "</form>"                                                  << endl;
}

bool emu::daq::manager::Application::printSTEPCountsTable( stringstream& out, bool control ){
  // Prints STEP counts. 
  // If control is true, prints checkboxes for masking inputs.
  // Returns true if STEP has finished, false otherwise.

  bool isFinished = true;

  // Nothing to do if not in STEP run.
  if ( runType_.toString().find("STEP",0) == string::npos ) return isFinished;

  if ( control ) out << "<form name=\"STEPCountsTable\" method=\"get\" action=\"/" << urn_ << "/command\">" << endl;

  out << "<table frame=\"void\" rules=\"rows cols\" class=\"params\">" << endl;
  
  out << "<tr>"                                                        << endl;
  out << "  <th colspan=" << 4 + emu::daq::rui::STEPEventCounter::maxDDUInputs_ 
       <<      " align=\"center\">";
  out <<       "<b>Event counts</b>";
  out << "  </th>"                                                     << endl;
  out << "</tr>"                                                       << endl;

  out << "<tr>"                                                        << endl;
  out <<   "<th colspan=4/>";
  out <<   "<th colspan=" << emu::daq::rui::STEPEventCounter::maxDDUInputs_ << ">";
  if ( control ){
    out <<       "<input type=\"submit\"";
    out <<           " style=\"float: left; z-index: 1; background-color: #ffffff;\"";
    out <<           " name=\"mask\"";
    out <<           " title=\"Count selected DDU inputs in.\"";
    out <<           " value=\"count in\"";
    out <<       "/> ";
    out <<       "<input type=\"submit\"";
    out <<           " style=\"float: right; z-index: 1; background-color: #cccccc;\"";
    out <<           " name=\"mask\"";
    out <<           " title=\"Count selected DDU inputs out.\"";
    out <<           " value=\"count out\"";
    out <<       "/>";
    out <<       "<span style=\"z-index: 0;\">DDU input</span>";
  }
  else{
    out << "DDU input";
  }
  out <<   "</th>"                                                     << endl;
  out << "</tr>"                                                       << endl;

  out << "<tr>"                                                        << endl;
  out <<   "<th/><th/>";
  out <<   "<th>read</th>";
  out <<   "<th>accepted</th>";
  for ( uint32_t i = 0; i < emu::daq::rui::STEPEventCounter::maxDDUInputs_; ++i )
    out <<   "<th>" << i << "</th>";
  out << "</tr>"                                                       << endl;

  emu::soap::Messenger m( this );

  // Loop over RUIs and query them for STEP info, and write a table row
  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for(rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); rui++){

    // Get STEP info from emu::daq::rui::Application
    xdata::String                      persistentDDUError = "";
    xdata::UnsignedInteger64                eventsRead  = 0;
    xdata::UnsignedInteger64                totalCount  = 0;
    xdata::UnsignedInteger64                lowestCount = 0;
    xdata::Vector<xdata::UnsignedInteger64> counts;
    xdata::Vector<xdata::Boolean>      masks;
    xdata::Vector<xdata::Boolean>      liveInputs;

    try{
      emu::soap::extractParameters( m.sendCommand( *rui, "STEPQuery" ),
				    emu::soap::Parameters()
				    .add( "PersistentDDUError", &persistentDDUError )
				    .add( "EventsRead"        , &eventsRead         )
				    .add( "TotalCount"        , &totalCount         )
				    .add( "LowestCount"       , &lowestCount        )
				    .add( "Counts"            , &counts             )
				    .add( "Masks"             , &masks              )
				    .add( "LiveInputs"        , &liveInputs         ) );

      isFinished &= ( (int64_t) lowestCount.value_ >= maxNumberOfEvents_.value_ ); 
    } catch( emu::daq::manager::exception::Exception e ){
      LOG4CPLUS_WARN( logger_, "Failed to get STEP info from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss53;
      ss53 <<  "Failed to get STEP info from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss53.str(), e );
      this->notifyQualified( "warning", eObj );
      isFinished = false;
    } catch (xoap::exception::Exception& e){
      LOG4CPLUS_WARN( logger_, "Failed to parse STEP info from reply from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss54;
      ss54 <<  "Failed to parse STEP info from reply from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss54.str(), e );
      this->notifyQualified( "warning", eObj );
      isFinished = false;
    }
    
    // Write HTML table row

    // First column: emu::daq::rui::Application and instance
    stringstream name;
    string       href = getHref( appDescriptor_ ) + "/control"; // self;
    string       dduError = "";
    name << "emu::daq::rui::Application " << setfill('0') << setw(2) << (*rui)->getInstance();
    href  = getHref( *rui );
    out << "<tr>"                                                 << endl;
    out << "  <th align=\"left\">";
    out << "      <a href=\"" << href 
	<<       "\" target=\"_blank\" title=\"click to visit emu::daq::rui::Application\">";
    out <<             name.str();
    out << "      </a>";
    out << "  </th>"                                              << endl;

    // Second column: hardware name...
    out << "  <th align=\"left\">";
    out << "      <a href=\"" << appDescriptor_->getContextDescriptor()->getURL() << "/"
	<< hardwareMapping_.toString() << "#RUI." << (*rui)->getInstance()
	<<       "\" target=\"_blank\" title=\"click to view chambers\">";
    if ( hardwareMnemonics_.find( (*rui)->getInstance() ) != hardwareMnemonics_.end() )
      out << hardwareMnemonics_[(*rui)->getInstance()];
    out << "      </a>";
    out << "  </th>"                                              << endl;


    // Third column: number of events read
    out << "  <td align=\"right\">";
    if ( persistentDDUError.toString().size() > 0 ){ // DDU in error
      out << "      <a href=\"" << href << "\""
	   <<         " title=\"" << persistentDDUError.toString() << "\""
	   <<         " style=\"color:#ffffff;"
	   <<                  "background-color:#000000;"
	   <<                  "text-decoration:underline blink\""
	   <<         "\">"
	   <<           eventsRead.toString() << "</a>";
    }
    else{ // DDU OK
      out << "    " << eventsRead.toString();
    }
    out << "  </td>"                                              << endl;

    // Fourth column: number of events accepted
    out << "  <td align=\"right\">";
    out << "    " << totalCount.toString();
    out << "  </td>"                                              << endl;

    // The remaining 15 columns: event count on each DDU input
    for ( size_t i = 0; i < counts.elements(); ++i ){
      stringstream DDUInput;
      DDUInput << "EmuRUI." << (*rui)->getInstance() << "." << i;
      out << "  <td align=\"right\"";
      if ( masks.elementAt(i)->toString() == "true" ) 
	out <<     "class=\"masked\"";
      else if ( liveInputs.elementAt(i)->toString() == "true" &&
		(int64_t) ( dynamic_cast<xdata::UnsignedInteger64*> ( counts.elementAt(i)) )->value_ < maxNumberOfEvents_.value_ )
	out <<     "class=\"notFinished\"";
      out <<   ">";
      
      out << "<a href=\"" 
	  << appDescriptor_->getContextDescriptor()->getURL() << "/"
	  << hardwareMapping_.toString() << "#RUI." << (*rui)->getInstance() << "." << i << "\""
	  << " title=\"click to view chamber\""
	  << " target=\"_blank\">";
      if ( liveInputs.elementAt(i)->toString() == "true" )
	out << counts.elementAt(i)->toString();
      else
	out << "&#8212;";
      out << "</a>";
      if ( control ){
	out <<       "<input type=\"checkbox\"";
	out <<             " name=\"" << DDUInput.str() << "\"";
	out <<             " title=\"DDU Input\"";
	out <<             " alt=\"DDU Input\"";
	out <<        "/>";
      }
      out <<   "</td>"                                            << endl;
    } // for ( size_t i = 0; i < counts.elements(); ++i )

    out << "</tr>"                                                << endl;

  } // for(rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); rui++)

  out << "</table>"                                                 << endl;

  if ( control ) out << "</form>"                                   << endl;

  return isFinished;
}


bool emu::daq::manager::Application::isSTEPFinished(){
  // Return TRUE if all DDUs' all live and unmasked inputs have produced the requested number of events.
  // Also record the lowest of any RUI's lowest count.
  bool isFinished = true;
  uint64_t minCount = numeric_limits<uint64_t>::max();

  emu::soap::Messenger m( this );

  // Loop over RUIs and query them for STEP info
  vector< xdaq::ApplicationDescriptor* >::iterator pos;
  for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++){

    xdata::UnsignedInteger64 lowestCount = 0;

    try{
      emu::soap::extractParameters( m.sendCommand( *pos, "STEPQuery" ),
				    emu::soap::Parameters().add( "LowestCount", &lowestCount ) );
    } catch( emu::daq::manager::exception::Exception e ){
      LOG4CPLUS_WARN( logger_, "Failed to get STEP info from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss55;
      ss55 <<  "Failed to get STEP info from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss55.str(), e );
      this->notifyQualified( "warning", eObj );
      isFinished = false;
    } catch (xoap::exception::Exception& e){
      LOG4CPLUS_WARN( logger_, "Failed to parse STEP info from reply from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss56;
      ss56 <<  "Failed to parse STEP info from reply from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss56.str(), e );
      this->notifyQualified( "warning", eObj );
      isFinished = false;
    }

    if ( minCount > (uint64_t) lowestCount.value_ ) minCount = (uint64_t) lowestCount.value_;
    isFinished &= ( (int64_t) lowestCount.value_ >= maxNumberOfEvents_.value_ ); 
    
  }

  STEPCount_ = minCount;
  return isFinished;
}

void emu::daq::manager::Application::maskDDUInputs( const bool in, const std::vector<cgicc::FormEntry>& fev ){

  //
  // Collect the RUI instances and DDU input indices that are to be masked
  //

  std::map< uint32_t, std::set<uint32_t> > instanceToInputs; // an RUI_instance --> [DDU inputs] map

  std::vector<cgicc::FormEntry>::const_iterator fe;
  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
    std::vector<std::string> matches; // matches[1] will be the instance, matches[2] the DDU input
    if ( toolbox::regx_match( fe->getName(), "^EmuRUI.[0-9]{1,2}.[0-9]{1,2}$") ){ // Make sure there's a match...
      toolbox::regx_match( fe->getName(), "^EmuRUI.([0-9]{1,2}).([0-9]{1,2})$", matches ); // ...because this crashes if no match.
      stringstream instance( matches[1] );
      stringstream input   ( matches[2] );
      uint32_t ins; instance >> ins;
      uint32_t inp; input    >> inp;
      std::map< uint32_t, std::set<uint32_t> >::iterator i = instanceToInputs.find( ins );
      if ( i == instanceToInputs.end() ){
	// First selected input of this instance
	std::set<uint32_t> inputs;
	inputs.insert( inp );
	instanceToInputs[ins] = inputs;
      }
      else{
	// This isntance has already input(s) selected
	i->second.insert( inp );
      }
    }
  }

  //
  // Loop over RUIs that have inputs selected and send them their list
  //

  std::map< uint32_t, std::set<uint32_t> >::iterator i;
  for ( i = instanceToInputs.begin(); i!= instanceToInputs.end(); ++i ){
    sendDDUInputMask( in, i->first, i->second );
  }

}

void emu::daq::manager::Application::sendDDUInputMask( const bool                    in, 
						       const uint32_t            ruiInstance, 
						       const std::set<uint32_t>& inputs ){
  // Sends a mask to a RUI exclude or include DDU inputs in the STEP event counts

  string commandName = "excludeDDUInputs";
  if ( in ) commandName = "includeDDUInputs";

  // Find the application descriptor for RUI of this instance
  xdaq::ApplicationDescriptor* ruiDescriptor = NULL;
  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = ruiDescriptors_.begin(); rui!=ruiDescriptors_.end(); ++rui )
    if ( (*rui)->getInstance() == ruiInstance ) ruiDescriptor = (*rui);
  if ( ruiDescriptor == NULL ){
    LOG4CPLUS_ERROR(logger_, "Failed to get descriptor of emu::daq::rui::Application instance " << ruiInstance );
    stringstream ss57;
    ss57 <<  "Failed to get descriptor of emu::daq::rui::Application instance " << ruiInstance ;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss57.str() );
    this->notifyQualified( "error", eObj );
    return;
  }

  // Transfer inputs into an xdata vector to be serialized into SOAP
  xdata::Vector<xdata::UnsignedInteger32> inputsToMask;
  std::set<uint32_t>::iterator i;
  for( i = inputs.begin(); i != inputs.end(); ++i ){
    inputsToMask.push_back( *i );
  }


  try{
    emu::soap::Messenger( this ).sendCommand( ruiDescriptor, commandName, emu::soap::Parameters().add( "DDUInputs", &inputsToMask ) );
  } catch( xcept::Exception &e ){
    LOG4CPLUS_ERROR(logger_,
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": " << xcept::stdformat_exception_history(e) );
    stringstream ss59;
    ss59 << "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance << ": "  ;
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss59.str(), e );
    this->notifyQualified( "error", eObj );
  } catch( ... ){
    LOG4CPLUS_ERROR(logger_,
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance << ": Unknown exception." );
    stringstream ss61;
    ss61 << "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
	 << ": Unknown exception." ;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss61.str() );
    this->notifyQualified( "error", eObj );
  }
  
  return;

}

string emu::daq::manager::Application::getDateTime(){
  time_t t;
  struct tm tm;

  time ( &t );
  gmtime_r( &t, &tm ); // reentrant version for thread safety

  stringstream ss;
  ss << setfill('0') << setw(4) << tm.tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm.tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm.tm_mday      << " "
     << setfill('0') << setw(2) << tm.tm_hour      << ":"
     << setfill('0') << setw(2) << tm.tm_min       << ":"
     << setfill('0') << setw(2) << tm.tm_sec       << " UTC";

  return ss.str();
}

void emu::daq::manager::Application::getMnemonicNames(){
  // Loop over emu::daq::rui::Applications querying their hardware's mnemonic names

  hardwareMnemonics_.clear();

  emu::soap::Messenger m( this );

  vector< xdaq::ApplicationDescriptor* >::iterator pos;
  
  for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
      stringstream app;
      uint32_t instance = (*pos)->getInstance();
      app << (*pos)->getClassName() << "." << instance;
      try
	{
	  xdata::String hwm;
	  m.getParameters( *pos, emu::soap::Parameters().add( "hardwareMnemonic", &hwm ) );
	  hardwareMnemonics_[instance] = hwm.toString();
	  LOG4CPLUS_DEBUG(logger_,"Got mnemonic name from " + app.str() + ": " + hardwareMnemonics_[instance] );
	}
      catch(xcept::Exception &e)
	{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to get mnemonic name from " << app.str() << ": " 
			  << xcept::stdformat_exception_history(e) );
	  stringstream ss62;
	  ss62 << 
			  "Failed to get mnemonic name from " << app.str() << ": " 
			   ;
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss62.str(), e );
	  this->notifyQualified( "error", eObj );
	}
    }
}

string emu::daq::manager::Application::reformatTime( string time ){
  // reformat from YYMMDD_hhmmss_UTC to YYYY-MM-DD hh:mm:ss UTC
  string reformatted("");
  reformatted += "20";
  reformatted += time.substr(0,2);
  reformatted += "-";
  reformatted += time.substr(2,2);
  reformatted += "-";
  reformatted += time.substr(4,2);
  reformatted += " ";
  reformatted += time.substr(7,2);
  reformatted += ":";
  reformatted += time.substr(9,2);
  reformatted += ":";
  reformatted += time.substr(11,2);
  reformatted += " UTC";
  return reformatted;
}

void emu::daq::manager::Application::bookRunNumber(){

  isBookedRunNumber_ = false;

  // Don't book debug runs:
  if ( runType_.toString() == "Debug" ||  runType_.toString().find("STEP",0) != string::npos ) return;
  
  // Just in case it's left over from the previuos run:
  if ( runInfo_ ) {
    delete runInfo_; 
    runInfo_ = NULL;
  }

  try
    {
      runInfo_ = emu::supervisor::RunInfo::Instance( runDbBookingCommand_.toString(),
						     runDbWritingCommand_.toString(),
						     runDbUserFile_.toString(),
						     runDbAddress_.toString() );
    }
  catch( string e )
    {
      LOG4CPLUS_ERROR(logger_, e);
      stringstream ss63;
      ss63 <<  e;
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss63.str() );
      this->notifyQualified( "error", eObj );
    }

  if ( runInfo_ ){

//     string sequence = "CSC:" + runType_.toString(); // stand-alone database in MTCC
    const string sequence = "CMS.CSC"; // central run database
    
    LOG4CPLUS_INFO(logger_, "Booking run number with " <<
		   runDbBookingCommand_.toString() << " at " <<
		   runDbAddress_.toString()  << " for " << sequence );
    
  
    bool success = runInfo_->bookRunNumber( sequence );
    
//     LOG4CPLUS_WARN(logger_, "Booking run number with " << runInfo_ << " " <<
// 		   runDbBookingCommand_.toString() << " at " <<
// 		   runDbAddress_.toString()  << " for " << sequence << ": "
// 		   << runInfo_->errorMessage() );
//     stringstream ss64;
//     ss64 <<  "Booking run number with " << runInfo_ << " " <<
// 		   runDbBookingCommand_.toString() << " at " <<
// 		   runDbAddress_.toString()  << " for " << sequence << ": "
// 		   << runInfo_->errorMessage() ;
//     XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss64.str() );
//     this->notifyQualified( "warning", eObj );
    
    if ( success ){
      isBookedRunNumber_ = true;
      runNumber_         = runInfo_->runNumber();
      runSequenceNumber_ = runInfo_->runSequenceNumber();
      LOG4CPLUS_INFO(logger_, "Booked run rumber " << runNumber_.toString() <<
		     " (" << sequence << " " << runSequenceNumber_.toString() << ")");
    }
    else {
      LOG4CPLUS_ERROR(logger_,
		      "<![CDATA[ Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << runNumber_.value_ << " ]]>" );
      stringstream ss65;
      ss65 << 
		      "<![CDATA[ Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << runNumber_.value_ << " ]]>" ;
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss65.str() );
      this->notifyQualified( "error", eObj );
    }
  } // if ( runInfo_ ){

}

void emu::daq::manager::Application::writeRunInfo(){
  // Update run info db

  // Don't write about debug runs:
  if ( runType_.toString() == "Debug" ||  runType_.toString().find("STEP",0) != string::npos ) return;

  // Don't write about aborted runs either:
  if ( abortedRun_ ) return;

  // If it's not a debug run, it should normally have been booked. Inform the user that it somehow wasn't.
  if ( !bool(isBookedRunNumber_) ){ 
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as no run number was booked.");
    stringstream ss66;
    ss66 <<  "Nothing written to run database as no run number was booked.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss66.str() );
    this->notifyQualified( "warning", eObj );
  }

  // If no runInfo, this run must have been booked by the Supervisor or the FM. We're not supposed to write anything then.
  if (  runInfo_ == NULL ){ 
    stringstream ss;
    ss <<  "Nothing written to run database by emu::daq::Manager as this run was not booked by emu::daq::Manager.";
    LOG4CPLUS_WARN(logger_, ss.str() );
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
    return;
  }


    bool success = false;
    const string nameSpace = "CMS.CSC";
    string name, value;

    //
    // run type
    //
    name      = "run_type";
    value     = runType_.value_;
    if (  bool(isBookedRunNumber_) ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss67;
	ss67 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss67.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    emu::soap::Messenger m( this );

    //
    // start time
    //
    string runStartTime("UNKNOWN");
    try
      {
	xdata::String startTime;
	m.getParameters( taDescriptors_[0], emu::soap::Parameters().add( "runStartTime", &startTime ) );
	runStartTime = reformatTime( startTime.toString() );
      }
    catch(xcept::Exception &e)
      {
	LOG4CPLUS_ERROR(logger_,"Failed to get time of run start from TA0: " << 
			xcept::stdformat_exception_history(e) );
	stringstream ss68;
	ss68 << "Failed to get time of run start from TA0: ";
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss68.str(), e );
	this->notifyQualified( "error", eObj );
      }
    name      = "start_time";
    value     = runStartTime;
    if (  bool(isBookedRunNumber_) ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss69;
	ss69 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss69.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    
    //
    // stop time
    //
    string runStopTime("UNKNOWN");
    try
      {
	xdata::String stopTime;
	m.getParameters( taDescriptors_[0], emu::soap::Parameters().add( "runStopTime", &stopTime ) );
	runStopTime = reformatTime( stopTime.toString() );
      }
    catch(xcept::Exception &e)
      {
	LOG4CPLUS_ERROR(logger_,"Failed to get time of stopping the run from TA0: " << 
			xcept::stdformat_exception_history(e) );
	stringstream ss70;
	ss70 << "Failed to get time of stopping the run from TA0: ";
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss70.str(), e );
	this->notifyQualified( "error", eObj );
      }
    name      = "stop_time";
    value     = runStopTime;
    if (  bool(isBookedRunNumber_) ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss71;
	ss71 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss71.str() );
	this->notifyQualified( "error", eObj );
      }
    }


    //
    // trigger sources
    //
    getTriggerSources();
    name  = "clock_source";
    value = TTCci_ClockSource_.toString();
    if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss73;
	ss73 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss73.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "orbit_source";
    value = TTCci_OrbitSource_.toString();
    if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{ 
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss74;
	ss74 << 
	  "Failed to write " << nameSpace << ":" << name << 
	  " to run database " << runDbAddress_.toString() <<
	  " : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss74.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "trigger_source";
    value = TTCci_TriggerSource_.toString();
    if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() );
	stringstream ss75;
	ss75 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss75.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "BGO_source";
    value = TTCci_BGOSource_.toString();
    if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss76;
	ss76 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss76.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // emu::daq::fu::Application event count
    //
    vector< map< string,string > > counts = getFUEventCounts();
    if ( counts.size() > 0 ){
      size_t nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
      name      = "built_events";
      value     = counts.at(nFUs)["count"]; // the last element is the sum of all FUs' event counts
      if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
			 nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss77;
	  ss77 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss77.str() );
	  this->notifyQualified( "error", eObj );
	}
      }
    }

    //
    // emu::daq::rui::Application event counts
    //
    counts.clear();
    counts = getRUIEventCounts();
    size_t nRUIs = counts.size();
    for ( size_t rui=0; rui<nRUIs; ++rui ){
      name  = "EmuRUI"+counts.at(rui)["appInst"];
      value = counts.at(rui)["count"];
      if (  bool(isBookedRunNumber_) && runInfo_ != NULL ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
			 nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss78;
	  ss78 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss78.str() );
	  this->notifyQualified( "error", eObj );
	}
      }
    }

}


// Supervisor-specific stuff
xoap::MessageReference emu::daq::manager::Application::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
  // fireEvent("Configure");
  // Execute it in another thread:
  
  workLoop_->submit(configureSignature_);

	return createReply(message);
}

xoap::MessageReference emu::daq::manager::Application::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
  // fireEvent("Enable");
  // Execute it in another thread:
  
  workLoop_->submit(enableSignature_);

	return createReply(message);
}

xoap::MessageReference emu::daq::manager::Application::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference emu::daq::manager::Application::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
  // fireEvent("Halt");
  // Execute it in another thread:
  
  workLoop_->submit(haltSignature_);

	return createReply(message);
}

xoap::MessageReference emu::daq::manager::Application::onReset(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{

  resetAction();

  try{
    fsm_.reset();
  }
  catch( toolbox::fsm::exception::Exception e ){
    XCEPT_RETHROW(xoap::exception::Exception,
		  "Failed to reset FSM: ", e);
  }

	fireEvent("Halt");

	return createReply(message);
}


xoap::MessageReference emu::daq::manager::Application::onQueryRunSummary(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  // Create reply message
  xoap::MessageReference reply = xoap::createMessage();

  try{
    // Start and end times
    string runNumber("UNKNOWN");
    string maxNumEvents("UNKNOWN");
    string runStartTime("UNKNOWN");
    string runStopTime("UNKNOWN");
    getRunInfoFromTA( &runNumber, &maxNumEvents, &runStartTime, &runStopTime );
    xdata::String start_time = runStartTime; // xdata can readily be serialized into SOAP...
    xdata::String stop_time  = runStopTime;
    xdata::UnsignedInteger32 run_number; run_number.fromString( runNumber );

    // FU event count
    xdata::UnsignedInteger64 built_events( 0 );
    vector< map< string,string > > counts = getFUEventCounts();
    if ( counts.size() > 0 ){
      size_t nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
      built_events.fromString( counts.at(nFUs)["count"] ); // the last element is the sum of all FUs' event counts
    }

    // RUI event counts and instances
    counts = getRUIEventCounts();
    size_t nRUIs = counts.size();
    xdata::Vector<xdata::UnsignedInteger64> rui_counts; // xdata can readily be serialized into SOAP...
    xdata::Vector<xdata::UnsignedInteger32> rui_instances; // xdata can readily be serialized into SOAP...
    for( size_t iRUI=0; iRUI<nRUIs; ++iRUI ){
      xdata::UnsignedInteger64 c; c.fromString( counts.at(iRUI)["count"]   );
      stringstream ss;
      ss << counts.at(iRUI)["appInst"];
      uint32_t appInst;
      ss >> appInst;
      xdata::UnsignedInteger32 i( appInst );
      rui_counts   .push_back( c );
      rui_instances.push_back( i );
    }

    xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
    emu::soap::includeParameters( reply,
				  &body,
				  emu::soap::Parameters()
				  .add( "start_time"   , &start_time    )
				  .add( "stop_time"    , &stop_time     )
				  .add( "run_number"   , &run_number    )
				  .add( "built_events" , &built_events  )
				  .add( "rui_counts"   , &rui_counts    )
				  .add( "rui_instances", &rui_instances ) );
    
  } 
  catch( xcept::Exception &e ){
    LOG4CPLUS_ERROR(logger_, "Failed to create run summary: " << xcept::stdformat_exception_history(e) );
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, "Failed to create run summary: ", e );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(xoap::exception::Exception, "Failed to create run summary: ", e);  
  }
  catch( std::exception &e ){
    LOG4CPLUS_ERROR(logger_, "Failed to create run summary: " << e.what());
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, string("Failed to create run summary: ") + e.what());
    this->notifyQualified( "error", eObj );
    XCEPT_RAISE(xoap::exception::Exception, string("Failed to create run summary: ") + e.what());
  }

  return reply;
}


void emu::daq::manager::Application::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{
  inFSMTransition_ = true;

  // Simulate slow transition
  //::sleep(15);
  // Simulate crash
  //exit(1);

    createAllAppStates();

    // Parameters will be set again in supervised mode on enable, 
    // but just for the display lets set them now:
    if ( supervisedMode_.value_ ) setParametersForSupervisedMode();
    // Set configuredInSupervisedMode_ to true if and when configuration succeeds.
    configuredInSupervisedMode_ = false;

    warningsToDisplay_ = "";

    if ( supervisedMode_.value_ ){
      REVOKE_ALARM( "unsupervised", NULL );
    }
    else {
      RAISE_ALARM( emu::daq::manager::alarm::Unsupervised, "unsupervised", "warning", "Local DAQ is in unsupervised mode.", "", &logger_ );
    }

    try
      {
	configureDAQ();
      }
    catch(xcept::Exception &ex)
      {
	inFSMTransition_ = false;
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to configure EmuDAQ", ex);
      }

    try
      {
	getMnemonicNames();
      }
    catch(xcept::Exception &ex)
      {
	LOG4CPLUS_ERROR(logger_, 
		       "Failed to get mnemonic names from emu::daq::rui::Applications: " 
		       << xcept::stdformat_exception_history(ex) );
	stringstream ss80;
	ss80 <<  
		       "Failed to get mnemonic names from emu::daq::rui::Applications: " 
		        ;
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss80.str(), ex );
	this->notifyQualified( "error", eObj );
      }

    // Successfully configured in supervised mode.
    if ( supervisedMode_.value_ ) configuredInSupervisedMode_ = true;


  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

  inFSMTransition_ = false;
}

void emu::daq::manager::Application::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  inFSMTransition_ = true;

    if ( supervisedMode_.value_ ){
      setParametersForSupervisedMode();
    }
    else{
      try
	{
	  bookRunNumber();
	}
      catch(...)
	{
	  LOG4CPLUS_ERROR(logger_,
			  "<![CDATA[ Failed to book run number. ==> Falling back to run number " 
			  << runNumber_.value_ << " ]]>" );
	  stringstream ss81;
	  ss81 << 
			  "<![CDATA[ Failed to book run number. ==> Falling back to run number " 
			  << runNumber_.value_ << " ]]>" ;
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss81.str() );
	  this->notifyQualified( "error", eObj );
	}
    }

    try
      {
	startDAQ();
      }
    catch(xcept::Exception &ex)
      {
	inFSMTransition_ = false;
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to enable EmuDAQ", ex);
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
    inFSMTransition_ = false;
}

void emu::daq::manager::Application::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  // Do nothing.

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void emu::daq::manager::Application::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  inFSMTransition_ = true;

    try
      {
	stopDAQ();
      }
    catch(xcept::Exception &ex)
      {
	inFSMTransition_ = false;
	stringstream ss;
	ss << "Failed to stop EmuDAQ: " << xcept::stdformat_exception_history(ex);
	XCEPT_RETHROW(toolbox::fsm::exception::Exception, ss.str(), ex);
      }

    try
      {
	// Write to database only if not in supervised mode.
	if ( !supervisedMode_.value_ ) writeRunInfo();
      }
    catch(...)
      {
	LOG4CPLUS_ERROR(logger_,
			"Failed to update run info database and/or to post to e-log" );
	stringstream ss82;
	ss82 << 
			"Failed to update run info database and/or to post to e-log" ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss82.str() );
	this->notifyQualified( "error", eObj );
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

    inFSMTransition_ = false;
}

void emu::daq::manager::Application::reConfigureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
  inFSMTransition_ = true;

    try
      {
	stopDAQ();
      }
    catch(xcept::Exception &ex)
      {
	inFSMTransition_ = false;
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to stop EmuDAQ before reconfiguration", ex);
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

    try
      {
	configureDAQ();
      }
    catch(xcept::Exception &ex)
      {
	inFSMTransition_ = false;
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to reconfigure EmuDAQ", ex);
      }

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

  inFSMTransition_ = false;
}

void emu::daq::manager::Application::noAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  // Inaction...
  stringstream ss;
  ss << e->type() 
     << " attempted when in " 
     << fsm_.getStateName(fsm_.getCurrentState())
     << " state.";
  LOG4CPLUS_WARN(getApplicationLogger(), ss.str() );
  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
  this->notifyQualified( "warning", eObj );
}

void emu::daq::manager::Application::resetAction()
		throw (toolbox::fsm::exception::Exception)
{

    try
      {
	resetDAQ();
      }
    catch(xcept::Exception &ex)
      {
	stringstream ss;
	ss << "Failed to reset EmuDAQ: " << xcept::stdformat_exception_history(ex);
// 	XCEPT_RETHROW(toolbox::fsm::exception::Exception, ss.str(), ex);
	LOG4CPLUS_ERROR(logger_, ss.str());
	stringstream ss84;
	ss84 <<  ss.str();
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss84.str() );
	this->notifyQualified( "error", eObj );
      }

    inFSMTransition_ = false;

}

bool emu::daq::manager::Application::configureActionInWorkLoop(toolbox::task::WorkLoop *wl){
  // Let's wait a sec to allow the async FSM SOAP command to safely return
  ::sleep(1);

  fireEvent("Configure");
  return false;
}

bool emu::daq::manager::Application::enableActionInWorkLoop(toolbox::task::WorkLoop *wl){
  // Let's wait a sec to allow the async FSM SOAP command to safely return
  ::sleep(1);

  fireEvent("Enable");
  return false;
}

bool emu::daq::manager::Application::haltActionInWorkLoop(toolbox::task::WorkLoop *wl){
  // Let's wait a sec to allow the async FSM SOAP command to safely return
  ::sleep(1);

  fireEvent("Halt");
  return false;
}

void emu::daq::manager::Application::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	emu::base::Supervised::stateChanged(fsm);
	appInfoSpace_->fireItemValueChanged( "stateName" );
}

void emu::daq::manager::Application::actionPerformed(xdata::Event & received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  LOG4CPLUS_INFO(logger_, 
		 "Received an InfoSpace event" <<
		 " Event type: " << e.type() <<
		 " Event name: " << e.itemName() <<
		 " Serializable: " << std::hex << e.item() << std::dec <<
		 " Type of serializable: " << e.item()->type() );

  if      ( e.itemName() == "daqState"       && e.type() == "ItemRetrieveEvent" ){ daqState_ = getDAQState();        }
  else if ( e.itemName() == "dataFileNames"  && e.type() == "ItemRetrieveEvent" ){ getDataPaths();                   }
  else if ( e.itemName() == "dataDirNames"   && e.type() == "ItemRetrieveEvent" ){ getDataPaths();                   }
  else if ( e.itemName() == "STEPFinished"   && e.type() == "ItemRetrieveEvent" ){ STEPFinished_ = isSTEPFinished(); }
  else if ( e.itemName() == "STEPCount"      && e.type() == "ItemRetrieveEvent" ){ STEPFinished_ = isSTEPFinished(); }
  else if ( e.itemName() == "supervisedMode" && e.type() == "ItemChangedEvent"  ){
    sendFact( "emu::daq::manager::Application", LocalDAQStatusFact::getTypeName() );
  }
  else if ( e.itemName() == "stateName"      && e.type() == "ItemChangedEvent"  ){
    if ( state_ == "Halted" || state_ == "Enabled" || state_ == "Failed" )
    sendFact( "emu::daq::manager::Application", LocalDAQStatusFact::getTypeName() );
  }
  else if ( e.itemName() == "runStartTime"   && e.type() == "ItemRetrieveEvent" ){
    emu::soap::Messenger( this ).getParameters( taDescriptors_[0], emu::soap::Parameters().add( "runStartTime" , &runStartTime_ ) );
  }

  // Commented out because this may happen too late, and then the RUIs won't be configured properly:
  // else if ( e.itemName() == "isGlobalInControl" && e.type() == "ItemChangedEvent" ){
  //   writeBadEventsOnly_ = isGlobalInControl_.value_;
  // }

}

void
emu::daq::manager::Application::configureRestartedApps(){
  // Get the halted apps, which were presumably restarted.
  vector<string> halted; // sysnonyms of "Halted"
  halted.push_back( "Halted" );
  set<xdaq::ApplicationDescriptor*> haltedApps( watchdog_->getAppsInStates( halted ) );
  // First configure the TA, if it's been restarted
  if ( taDescriptors_.size() > 0 && haltedApps.find( taDescriptors_.at(0) )!=haltedApps.end() ){
    configureTA();
    stringstream ss;
    ss << "Watchdog configured " << taDescriptors_.at(0)->getClassName() << "." << taDescriptors_.at(0)->getInstance();
    LOG4CPLUS_WARN( logger_, ss.str() );
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  }
  // Then configure the RUIs that have been restarted
  for( vector<xdaq::ApplicationDescriptor*>::const_iterator rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); ++rui ){
    if ( haltedApps.find( *rui )!=haltedApps.end() ){
      configureRUI( *rui );
      stringstream ss;
      ss << "Watchdog configured " << (*rui)->getClassName() << "." << (*rui)->getInstance();
      LOG4CPLUS_WARN( logger_, ss.str() );
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    }
  }
}

void
emu::daq::manager::Application::enableRestartedApps(){
  // Get the configured apps, which were presumably restarted.
  vector<string> configured; // sysnonyms of "Configured"
  configured.push_back( "Configured" );
  configured.push_back( "Ready"      );
  set<xdaq::ApplicationDescriptor*> configuredApps( watchdog_->getAppsInStates( configured ) );
  // First configure the TA, if it's been restarted
  if ( taDescriptors_.size() > 0 && configuredApps.find( taDescriptors_.at(0) )!=configuredApps.end() ){
    enableTA();
    stringstream ss;
    ss << "Watchdog enabled" << taDescriptors_.at(0)->getClassName() << "." << taDescriptors_.at(0)->getInstance();
    LOG4CPLUS_WARN( logger_, ss.str() );
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
    this->notifyQualified( "warning", eObj );
  }
  // Then configure the RUIs that have been restarted
  for( vector<xdaq::ApplicationDescriptor*>::const_iterator rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); ++rui ){
    if ( configuredApps.find( *rui )!=configuredApps.end() ){
      enableRUI( *rui );
      stringstream ss;
      ss << "Watchdog enabled " << (*rui)->getClassName() << "." << (*rui)->getInstance();
      LOG4CPLUS_WARN( logger_, ss.str() );
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss.str() );
      this->notifyQualified( "warning", eObj );
    }
  }
}

void
emu::daq::manager::Application::timeExpired(toolbox::task::TimerEvent& e){
  // Send out the watchdog to look for restarted applications and herd them back to the proper FSM state.

  // Hold back the watchdog while daq::manager is in state transitions
  if ( inFSMTransition_ ) return;

  // Watchdog cannot be constructed in app's constructor as it needs some exported parameters to have been set already.
  if ( watchdog_ == NULL ) watchdog_ = new emu::daq::manager::Watchdog( this );

  vector<string> halted;
  halted.push_back( "Halted" );
  vector<string> configured;
  configured.push_back( "Configured" );
  configured.push_back( "Ready"      );
  LOG4CPLUS_INFO( logger_, "Time expired for event " << e.type() );
  try{
    switch(fsm_.getCurrentState()){
    case 'H': // Halted
      break;
    case 'C': // Configured
      watchdog_->patrol();
      LOG4CPLUS_INFO( logger_, "Watchdog after patrol" << endl << *watchdog_ );
      if ( watchdog_->getAppsInStates( halted ).size() > 0 && !inFSMTransition_  ) configureRestartedApps();
      break;
    case 'E': // Enabled
      watchdog_->patrol();
      LOG4CPLUS_INFO( logger_, "Watchdog after first patrol" << endl << *watchdog_ );
      if ( watchdog_->getAppsInStates( halted ).size() > 0 && !inFSMTransition_ ) configureRestartedApps();
      watchdog_->patrol();
      LOG4CPLUS_INFO( logger_, "Watchdog after second patrol" << endl << *watchdog_ );
      if ( watchdog_->getAppsInStates( configured ).size() > 0 && !inFSMTransition_ ) enableRestartedApps();
      break;
    default:
      break;
    }
  } catch( xcept::Exception& e ){
    LOG4CPLUS_ERROR( logger_, "Watchdog failed to execute scheduled patrol: " << xcept::stdformat_exception_history(e) );
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, "Watchdog failed to execute scheduled patrol: ", e );
    this->notifyQualified( "error", eObj );
  }
}

/**
 * Provides the factory method for the instantiation of emu::daq::manager::Application
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::manager::Application)
