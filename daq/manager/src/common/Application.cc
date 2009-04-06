#include "emu/daq/manager/Application.h"
#include "emu/daq/manager/version.h"
#include "emu/daq/manager/alarm/Alarm.h"
#include "emu/base/Alarm.h"
#include "emu/daq/rui/STEPEventCounter.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTTPPlainHeader.h"
#include "xcept/tools.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xgi/Method.h"
#include "xgi/Utils.h"
#include "xoap/domutils.h"
#include "xoap/MessageFactory.h"
#include "xoap/MessageReference.h"
#include "xoap/Method.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"

#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"
#include "xdata/soap/Serializer.h"
#include "toolbox/regex.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

// For emu::daq::manager::Application::postSOAP
#include "toolbox/net/URL.h"
#include "pt/PeerTransportAgent.h"
#include "pt/PeerTransportSender.h"
#include "pt/SOAPMessenger.h"

// #include "emu/daq/manager/exception/Alarm.h"
// #include "sentinel/utils/Alarm.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

emu::daq::manager::Application::Application(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),
emu::base::Supervised(s),
emu::base::WebReporter(s),
logger_(Logger::getInstance(generateLoggerName())),
runInfo_(0)
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
    createAllAppStatesVector();

    // Bind web interface
    xgi::bind(this, &emu::daq::manager::Application::css           , "styles.css");
    xgi::bind(this, &emu::daq::manager::Application::defaultWebPage, "Default"   );
    xgi::bind(this, &emu::daq::manager::Application::controlWebPage, "control"   );
    xgi::bind(this, &emu::daq::manager::Application::commandWebPage, "command"   );
    xgi::bind(this, &emu::daq::manager::Application::commentWebPage, "comment"   );
    xgi::bind(this, &emu::daq::manager::Application::machineReadableWebPage,
        "MachineReadable");

    exportParams(appInfoSpace_);

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
    fsm_.addStateTransition('C', 'C', "Configure", this, &emu::daq::manager::Application::reConfigureAction);
    fsm_.addStateTransition('C', 'E', "Enable",    this, &emu::daq::manager::Application::enableAction);
    fsm_.addStateTransition('E', 'C', "Disable",   this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('C', 'H', "Halt",      this, &emu::daq::manager::Application::haltAction);
    fsm_.addStateTransition('E', 'H', "Halt",      this, &emu::daq::manager::Application::haltAction);
    
    fsm_.addStateTransition('H', 'H', "Halt",      this, &emu::daq::manager::Application::noAction);
    fsm_.addStateTransition('E', 'E', "Enable",    this, &emu::daq::manager::Application::noAction);

    fsm_.setInitialState('H');
    fsm_.reset();

    state_ = fsm_.getStateName(fsm_.getCurrentState());
    
    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string emu::daq::manager::Application::generateLoggerName()
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


void emu::daq::manager::Application::getAllAppDescriptors()
{
    try
    {
//         evmDescriptors_ = getAppDescriptors(zone_, "EVM");
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
        this->notifyQualified( "warn", eObj );
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
        this->notifyQualified( "warn", eObj );
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
        this->notifyQualified( "warn", eObj );
    }

    try
    {
        dqmMonitorDescriptors_ = getAppDescriptors(zone_, "EmuMonitor");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        dqmMonitorDescriptors_.clear();

        // Log only a warning as EmuMonitorss may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuMonitor"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss6;
        ss6 << 
            "Failed to get application descriptors for class EmuMonitor"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss6.str(), e );
        this->notifyQualified( "warn", eObj );
    }

    vector< xdaq::ApplicationDescriptor* > dqmTFMonitorDescriptors_; // There's only one, but anyway
    try
    {
        dqmTFMonitorDescriptors_ = getAppDescriptors(zone_, "EmuTFMonitor");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        dqmTFMonitorDescriptors_.clear();

        // Log only a warning as EmuMonitorss may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuTFMonitor"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss7;
        ss7 << 
            "Failed to get application descriptors for class EmuTFMonitor"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss7.str(), e );
        this->notifyQualified( "warn", eObj );
    }

    // Append EmuTFMonitor's descriptor to EmuTFMonitors'
    dqmMonitorDescriptors_.insert(dqmMonitorDescriptors_.end(),
				  dqmTFMonitorDescriptors_.begin(), dqmTFMonitorDescriptors_.end());
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
    int nbApps = 0;


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


void emu::daq::manager::Application::css
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
    *out << "font-family: Arial;"                                      << endl;
    *out << "}"                                                        << endl;
    *out                                                               << endl;
    *out << "input.button"                                             << endl;
    *out << "{"                                                        << endl;
    *out << "color: white;"                                            << endl;
    *out << "background-color: #222288;"                               << endl;
    *out << "border-color: #222288;"                                   << endl;
    *out << "font-family: Arial;"                                      << endl;
    *out << "font-weight: bold;";
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
    *out << "background-color: #AAAAFF;"                               << endl;
    *out << "}"                                                        << endl;
    *out << "table.params th a"                                        << endl;
    *out << "{"                                                        << endl;
    *out << "color: white;"                                            << endl;
    *out << "}"                                                        << endl;

    *out << "a.with_popup .popup {"                                    << endl;
    *out << "	display: none;"                                        << endl;
    *out << "}"                                                        << endl;
    *out                                                               << endl;
    *out << "a.with_popup:hover .popup{"                               << endl;
    *out << "	display: inline;"                                      << endl;
    *out << "}"                                                        << endl;

    *out << "td.masked {";
    *out << "background-color: #cccccc;";
    *out << "}"                                                        << endl;

    *out << "td.notFinished {";
    *out << "font-weight: bold;";
    *out << "color: #ff0000;";
    *out << "}"                                                        << endl;
}


void emu::daq::manager::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  daqState_ = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << "Local DAQ " << daqState_.toString()                       << endl;
//     *out << xmlClass_ << instance_                                     << endl;
// 	 << " Version "
//         << emudaqmanager::versions 
// 	 << endl;
    *out << "</title>"                                                 << endl;
    *out << ageOfPageClock();
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


    // Display RUIs' and FUs' event counts
    if ( runType_.toString().find("STEP",0) != string::npos ){
      *out << STEPCountsTable.str();
    }
    else{
      printEventCountsTable( out, "Events read by emu::daq::rui::Applications"    , getRUIEventCounts() );
    }
    *out << "<br/>"                                                  << endl;
    if ( runType_.toString().find("STEP",0) == string::npos && buildEvents_.value_ ){
      printEventCountsTable( out, "Events processed by emu::daq::fu::Applications", getFUEventCounts()  );
      *out << "<br/>"                                                  << endl;
    }

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    printStatesTable( out, "DAQ applications", daqContexts_, daqAppStates_ );
    *out << "</td>"                                                   << endl;
    *out << "<td width=\"16\"/>"                                      << endl;
    *out << "<td>"                                                   << endl;
//     queryAppStates( dqmAppStates_ );
//     printStatesTable( out, "DQM applications", dqmContexts_, dqmAppStates_ );
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
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(xgi::exception::Exception,
                "Failed to get event number from EVM", e);
        }

        try
        {
            printParamsTable(in, out, evmDescriptors_[0], eventNb);
        }
        catch(xcept::Exception e)
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
    catch(xcept::Exception e)
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
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print the parameter tables of the RUs", e);
    }

    *out << "<br>"                                                     << endl;

    try
    {
        printParamsTables(in, out, buDescriptors_, buStats);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(xgi::exception::Exception,
            "Failed to print the parameter tables of the BUs", e);
    }

    *out << "<br>"                                                     << endl;

    }// if ( buildEvents_.value_ )

    *out << "</form>"                                                  << endl;
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

string emu::daq::manager::Application::ageOfPageClock(){
  stringstream ss;
  ss << "<script type=\"text/javascript\">"                        << endl;
  ss << "   ageOfPage=0"                                           << endl;
  ss << "   function countSeconds(){"                              << endl;
  ss << "      hours=Math.floor(ageOfPage/3600)"                   << endl;
  ss << "      minutes=Math.floor(ageOfPage/60)%60"                << endl;
  ss << "      age=\"\""                                           << endl;
  ss << "      if (hours) age+=hours+\" h \""                      << endl;
  ss << "      if (minutes) age+=minutes+\" m \""                  << endl;
  ss << "      age+=ageOfPage%60+\" s \""                          << endl;
  ss << "      document.getElementById('ageOfPage').innerHTML=age" << endl;
  ss << "      ageOfPage=ageOfPage+1"                              << endl;
  ss << "      setTimeout('countSeconds()',1000)"                  << endl;
  ss << "   }"                                                     << endl;
  ss << "</script>"                                                << endl;
  return ss.str();
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
  *out << " <option value=\"global\">[ Supervisor | FM | Central RC ] (<em>supervised mode</em>)</option>"<< endl;
  *out << " <option value=\"local\">CSC Shift from this page (<em>unsupervised mode</em>)</option>"                << endl;
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
	bool userWantsGlobal = ( fe->getValue() == "global");
// 	cout << "userWantsGlobal        " << userWantsGlobal << endl
// 	     << "configuredInGlobalMode " << configuredInGlobalMode_.value_ << endl
// 	     << "CurrentState           " << fsm_.getCurrentState() << endl;
	
	if ( daqState_.toString() == "Halted" ){
	  // DAQ's halted, everything's permitted.
	  warningsToDisplay_ = "";
	  globalMode_ = userWantsGlobal;
	}
	else{
	  // DAQ's most probably been configured.
	  if ( !globalMode_.value_ && userWantsGlobal ){
	    // User wants to switch to central RC.
	    if ( !configuredInGlobalMode_.value_ ){
	      // Warn him if we are in a locally configured run.
	      warningsToDisplay_ = "<p><span style=\"color:#ff0000; font-weight:bold; text-decoration:blink\">Warning</span>:";
	      warningsToDisplay_ += "You have chosen <em>supervised mode</em> (i.e., handed over the control to [CSC Supervisor | CSC Function Manager | Central Run Control]) in a run configured in <em>unsupervised mode</em> (i.e., by CSC Shift from this web page). ";
	      warningsToDisplay_ += "Such runs can only be stopped in <em>unsupervised mode</em>. ";
	      warningsToDisplay_ += "Consider going back to <em>unsupervised mode</em>.</p>";
	    }
	    else{
	      warningsToDisplay_ = "";
	    }
	    globalMode_ = userWantsGlobal;
	  }
	  else{
	    // Warn the user when he wants to take control back from central RC...
	    if ( globalMode_.value_ && !userWantsGlobal && configuredInGlobalMode_.value_ && daqState_.toString() != "Halted" ){
	      warningsToDisplay_  = "<p><span style=\"color:#ff0000; font-weight:bold; text-decoration:blink\">Warning</span>:";
	      warningsToDisplay_ += "You have chosen <em>unsupervised mode</em> (i.e., control by CSC Shift from this web page) in a run run configured in <em>supervised mode</em> (i.e., by [CSC Supervisor | CSC Function Manager | Central Run Control]).";
	      warningsToDisplay_ += "It is preferable that such runs be started and stopped in <em>supervised mode</em>. ";
	      warningsToDisplay_ += "Consider going back to <em>supervised mode</em>.</p>";
	    }
	    // ...but do as he requested.
	    globalMode_ = userWantsGlobal;
	  }
	}

	break;
      }

    }
    
    *out << warningsToDisplay_;

    if ( globalMode_.value_ ){
      RAISE_ALARM( emu::daq::manager::alarm::Unsupervised, "unsupervised", "warn", "Local DAQ is in unsupervised mode.", "", &logger_ );
    }
    else {
      REVOKE_ALARM( "unsupervised", NULL );
    }

    if ( globalMode_.value_ ) setParametersForGlobalMode();

}

void emu::daq::manager::Application::commentWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  processCommentForm(in);

  *out << "<html>"                                                   << endl;

  *out << "<head>"                                                   << endl;
  *out << "<link type=\"text/css\" rel=\"stylesheet\"";
  *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
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
  if ( globalMode_.value_ ) *out << " disabled=\"true\""             << endl;
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
    processCommandForm(in);

    daqState_ = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;

    if ( fsm_.getCurrentState() == 'E' ){
     *out << "<meta http-equiv=\"refresh\" content=\"5\">"              << endl;
    }
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << "Emu Local DAQ " << daqState_.toString()                               << endl;
//     *out << xmlClass_ << instance_ << " Version " << emudaqmanager::versions
//         << " CONTROL" << endl;
    *out << "</title>"                                                 << endl;
    *out << ageOfPageClock();
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

    // ...but let the user overwrite them by selecting global mode.
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
      if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
      *out << "/>  "                                                 ;
      for ( unsigned int iType=0; iType<runTypes_.elements(); ++iType ){
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
      if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

      *out << "Build events: "                                       << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"checkbox\""                                   << endl;
      *out << " name=\"buildevents\""                                << endl;
      *out << " title=\"If checked, events will be built.\""         << endl;
      *out << " alt=\"build events\""                                << endl;
      if ( buildEvents_.value_ ) *out << " checked"                  << endl;
      if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
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
	if ( isBookedRunNumber_ ) 
	  *out << " (booked)";
	else if ( !configuredInGlobalMode_ ) 
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
      if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
      *out << "/>"                                                   << endl;

      // In case the user has changed his mind, allow him to halt from 'configured' state.
      if ( fsm_.getCurrentState() == 'C' )
	{
	  *out << "<input"                                               << endl;
	  *out << " class=\"button\""                                    << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"stop\""                                      << endl;
	  if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
	  *out << "/>"                                                   << endl;
	}

      // If DAQ is in "Failed" state, but emu::daq::manager::Application isn't, place a reset button
      // (if emu::daq::manager::Application is in 'failed' state, we should already have one).
      // Also, if emu::daq::manager::Application is in "Halted" state, but DAQ is not (because emu::daq::manager::Application 
      // was restarted, for example), allow DAQ to be reset. 
      if ( fsm_.getCurrentState() != 'F' && daqState_.toString() == "Failed" ||
	   ( fsm_.getCurrentState() == 'H' && daqState_.toString() != "Halted" ) ){
	  *out << "<input"                                               << endl;
	  *out << " class=\"button\""                                    << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"reset\""                                     << endl;
	  if ( globalMode_.value_ ) *out << " disabled=\"true\""         << endl;
	  *out << "/>"                                                   << endl;
      }
      

      *out << "<input"                                                   << endl;
      *out << " type=\"checkbox\""                                       << endl;
      *out << " name=\"controldqm\""                                     << endl;
      *out << " title=\"If checked, DQM's state will be changed too.\""  << endl;
      *out << " alt=\"control dqm\""                                     << endl;
      if ( controlDQM_.value_ ) *out << " checked"                       << endl;
      if ( globalMode_.value_ ) *out << " disabled=\"true\""             << endl;
      *out << "/>  "                                                     << endl;
      *out << " DQM too"                                                 << endl;
    *out << "</td>"                                                     << endl;

    // Direct DQM control buttons (useful after DQM crash and restart).
    *out << "<td>"                                                     << endl;
      *out << "<fieldset style=\"float:right\">"                         << endl;
      *out << "	 <legend>DQM direct control</legend>"                    << endl;
      *out << "<input class=\"button\" type=\"submit\" name=\"command\"" << endl;
      *out << " value=\"configure DQM\""                                 << endl;
      *out << "/>"                                                       << endl;
      *out << "<input class=\"button\" type=\"submit\" name=\"command\"" << endl;
      *out << " value=\"start DQM\""                                     << endl;
      *out << "/>"                                                       << endl;
      *out << "<input class=\"button\" type=\"submit\" name=\"command\"" << endl;
      *out << " value=\"stop DQM\""                                      << endl;
      *out << "/>"                                                       << endl;
      *out << "</fieldset>"                                              << endl;
    *out << "<tr>"                                                     << endl;
    *out << "</table>"                                                   << endl;
       

    *out << "<br>"                                                       << endl;
    *out << "<br>"                                                       << endl;

    if ( runType_.toString().find("STEP",0) != string::npos )
      *out << STEPCountsTable.str();
    else
      printEventCountsTable( out, "Events read by emu::daq::rui::Applications", getRUIEventCounts() );
    *out << "<br/>"                                                      << endl;
    if ( runType_.toString().find("STEP",0) == string::npos && buildEvents_.value_ ){
      printEventCountsTable( out, "Events processed by emu::daq::fu::Applications", getFUEventCounts()  );      
      *out << "<br/>"                                                    << endl;
    }

    *out << "</form>"                                                    << endl;

    *out << "<br/>"                                                      << endl;
    *out << "<br/>"                                                      << endl;

    *out << "<table border=\"0\">"                                       << endl;
    *out << "<tr valign=\"top\">"                                        << endl;
    *out << "<td>"                                                       << endl;
    printStatesTable( out, "DAQ applications", daqContexts_, daqAppStates_ );
    *out << "</td>"                                                      << endl;
    *out << "<td width=\"16\"/>"                                         << endl;
    *out << "<td>"                                                       << endl;
//     queryAppStates( dqmAppStates_ );
//     printStatesTable( out, "DQM applications", dqmContexts_, dqmAppStates_ );
    *out << "</td>"                                                      << endl;
    *out << "</tr>"                                                      << endl;
    *out << "</table>"                                                   << endl;

    *out << "</body>"                                                    << endl;

    *out << "</html>"                                                    << endl;
}

vector<emu::base::WebReportItem> 
emu::daq::manager::Application::materialToReportOnPage1(){
  vector<emu::base::WebReportItem> items;

  // State
  string state = getDAQState();
  string valueTip;
  string controlURL = getHref( appDescriptor_ ) + "/control";
  if ( ! ( state == "Enabled" || state == "Ready" || state == "Halted" ) ) 
    valueTip = "Local DAQ may need attention. Click to control it manually.";
  items.push_back( emu::base::WebReportItem( "state",
                                             state,
                                             "The overall state of local DAQ.",
                                             valueTip,
                                             controlURL,
                                             controlURL ) );

  // Min and max RUI counts
  int maxCount = -1;
  int minCount = 2000000000;
  int minCountIndex = -1;
  int maxCountIndex = -1;
  vector< map< string,string > > counts = getRUIEventCounts();
  for ( unsigned int iRUI=0; iRUI<counts.size(); ++iRUI ){
    stringstream ss;
    ss << counts.at(iRUI)["count"];
    int count;
    ss >> count;
    if ( count < minCount ) { minCount = count; minCountIndex = iRUI; }
    if ( count > maxCount ) { maxCount = count; maxCountIndex = iRUI; }
  }
  try{
    stringstream nss;
    nss << "Events read from " << counts.at(minCountIndex)["hwName"] << " by RUI " << counts.at(minCountIndex)["appInst"] << ".";
    items.push_back( emu::base::WebReportItem( "min events",
					       counts.at(minCountIndex)["count"],
					       "The lowest number of events read by an RUI. It should remain close to 'max events read'.",
					       nss.str(),
					       "", "" ) );
    stringstream xss;
    xss << "Events read from " << counts.at(maxCountIndex)["hwName"] << " by RUI " << counts.at(maxCountIndex)["appInst"] << ".";
    items.push_back( emu::base::WebReportItem( "max events",
					       counts.at(maxCountIndex)["count"],
					       "The highest number of events read by an RUI.",
					       xss.str(),
					       "", "" ) );
  }
  catch( const std::exception& e ){
    LOG4CPLUS_WARN(logger_, "Failed to report min and max number of events to Page 1 : " << e.what() );
    stringstream ss8;
    ss8 <<  "Failed to report min and max number of events to Page 1 : " << e.what() ;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss8.str() );
    this->notifyQualified( "warn", eObj );
  }
  
  return items;
}

void emu::daq::manager::Application::setParametersForGlobalMode(){
  // Prepare for obeying Central Run Control commands
//   runType_                = "Monitor";
//   maxNumberOfEvents_      = -1;
//   buildEvents_            = false;
  controlDQM_             = false;
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
	this->notifyQualified( "warn", eObj );
      }

      map <string,string> namesAndTypes;
      namesAndTypes["runNumber"     ] = "unsignedLong";
      namesAndTypes["maxNumTriggers"] = "integer";
      namesAndTypes["runStartTime"  ] = "string";
      namesAndTypes["runStopTime"   ] = "string";
      try
	{
	  map <string,string> namesAndValues = getScalarParams(taDescriptors_[0],namesAndTypes);
	  *runnum    = namesAndValues["runNumber"     ];
	  *maxevents = namesAndValues["maxNumTriggers"];
	  *starttime = namesAndValues["runStartTime"  ];
	  *stoptime  = namesAndValues["runStopTime"   ];
	  *starttime = reformatTime( *starttime );
	  *stoptime  = reformatTime( *stoptime  );
	}
      catch(xcept::Exception e)
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
  catch(xcept::Exception e)
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
      this->notifyQualified( "warn", eObj );
    }

  if ( appDescriptors.size() >= 1 ){
    if ( appDescriptors.size() > 1 )
      LOG4CPLUS_WARN(logger_, "The embarassement of riches: " << 
		      appDescriptors.size() << 
		      " TTCciControl instances found. Trying first one.");
      stringstream ss14;
      ss14 <<  "The embarassement of riches: " << 
		      appDescriptors.size() << 
		      " TTCciControl instances found. Trying first one.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss14.str() );
      this->notifyQualified( "warn", eObj );
    map <string,string> namesAndTypes;
    namesAndTypes["ClockSource"  ] = "string";
    namesAndTypes["OrbitSource"  ] = "string";
    namesAndTypes["TriggerSource"] = "string";
    namesAndTypes["BGOSource"    ] = "string";
    try
      {
	map <string,string> namesAndValues = getScalarParams(appDescriptors[0],namesAndTypes);
	TTCci_ClockSource_   = namesAndValues["ClockSource"  ];
	TTCci_OrbitSource_   = namesAndValues["OrbitSource"  ];
	TTCci_TriggerSource_ = namesAndValues["TriggerSource"];
	TTCci_BGOSource_     = namesAndValues["BGOSource"    ];
	
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
	  this->notifyQualified( "warn", eObj );
	}
      catch(...)
	{
	  LOG4CPLUS_WARN(logger_,"Failed to get trigger sources from TTCciControl."); 
	  stringstream ss16;
	  ss16 << "Failed to get trigger sources from TTCciControl."; 
	  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss16.str() );
	  this->notifyQualified( "warn", eObj );
	}
    
  }
  else{
    LOG4CPLUS_WARN(logger_, "Did not find TTCciControl. ==> Trigger sources are unknown.");
    stringstream ss17;
    ss17 <<  "Did not find TTCciControl. ==> Trigger sources are unknown.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss17.str() );
    this->notifyQualified( "warn", eObj );
  }
}

void emu::daq::manager::Application::getTriggerMode()
  // Gets the trigger mode from TF_hyperDAQ
{
  TF_triggerMode_ = "UNKNOWN";

  vector< xdaq::ApplicationDescriptor* > appDescriptors;
  try
    {
      appDescriptors = getAppDescriptors(zone_, "TF_hyperDAQ");
    }
  catch(xcept::Exception e)
    {
      appDescriptors.clear();
      LOG4CPLUS_WARN(logger_,
		      "Failed to get application descriptors for class TF_hyperDAQ"
		      << stdformat_exception_history(e));
      stringstream ss18;
      ss18 << 
		      "Failed to get application descriptors for class TF_hyperDAQ"
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss18.str(), e );
      this->notifyQualified( "warn", eObj );
    }

  if ( appDescriptors.size() >= 1 ){
    if ( appDescriptors.size() > 1 )
      LOG4CPLUS_WARN(logger_, "The embarassement of riches: " << 
		      appDescriptors.size() << " TF_hyperDAQ instances found. Trying first one.");
      stringstream ss19;
      ss19 <<  "The embarassement of riches: " << 
		      appDescriptors.size() << " TF_hyperDAQ instances found. Trying first one.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss19.str() );
      this->notifyQualified( "warn", eObj );
    try{
      TF_triggerMode_ = getScalarParam(appDescriptors[0],"triggerMode","string");
      LOG4CPLUS_INFO(logger_, "Got trigger mode from TF_hyperDAQ: " 
		     << TF_triggerMode_.toString() );
    }
    catch( emu::daq::manager::exception::Exception e ){
      LOG4CPLUS_WARN(logger_, "Failed to get trigger mode from " 
		     << appDescriptors[0]->getClassName()
		     << appDescriptors[0]->getInstance() << " "
		     << xcept::stdformat_exception_history(e) );
      stringstream ss20;
      ss20 <<  "Failed to get trigger mode from " 
		     << appDescriptors[0]->getClassName()
		     << appDescriptors[0]->getInstance() << " "
		      ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss20.str(), e );
      this->notifyQualified( "warn", eObj );
    }
  }
  else{
    LOG4CPLUS_WARN(logger_, "Did not find TF_hyperDAQ. ==> Trigger mode is unknown.");
    stringstream ss21;
    ss21 <<  "Did not find TF_hyperDAQ. ==> Trigger mode is unknown.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss21.str() );
    this->notifyQualified( "warn", eObj );
  }
}

int emu::daq::manager::Application::purgeIntNumberString( string* s ){
  // Emu: purge string of all non-numeric characters
  int nCharactersErased = 0;
  for ( string::size_type i = s->find_first_not_of("-0123456789",0); 
	i != string::npos; 
	i = s->find_first_not_of("-0123456789",i) ){
    s->erase(i,1);
    nCharactersErased++;
  }
  return nCharactersErased;
}

int emu::daq::manager::Application::stringToInt( const string* const s ){
  int i;
  stringstream ss;
  ss << *s;
  ss >> i;
  return i;
}

void emu::daq::manager::Application::processCommandForm(xgi::Input *in)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);

    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    std::vector<cgicc::FormEntry>::iterator fe;

//     for ( fe=fev.begin(); fe!=fev.end(); ++fe )
//       cout << fe->getName() << " " << fe->getValue() << endl;
//     cout << "---------------------------------" << endl;

    // Check if DQM needs controlling
    // Apparently the query string does not even include the checkbox element if it's not checked...
    // Set controlDQM_ later, if and only if a DAQ control button was pressed.
    bool controlDQM_checked = false;
    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
      if ( fe->getName() == "controldqm" && fe->getValue() == "on" ){
	controlDQM_checked = true;
	break;
      }

    // If there is a command from the html form
    cgicc::form_iterator cmdElement    = cgi.getElement("command");
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
	    // Emu: buildEvents will be queried by emu::daq::rui::Applications
	    // Apparently the query string does not even include the checkbox element if it's not checked...
	    buildEvents_ = false;
	    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
	      if ( fe->getName() == "buildevents" && fe->getValue() == "on" ){
		buildEvents_ = true;
		break;
	      }
	    // Emu: set run number to 1. If booking is successful, it will be replaced by the booked one.
	    runNumber_ = 1;

	    cgicc::form_iterator maxEvtElement = cgi.getElement("maxevents");
	    if( maxEvtElement != cgi.getElements().end() ){
	      string maxNumEvents  = (*maxEvtElement).getValue();
	      purgeIntNumberString( &maxNumEvents );
	      maxNumberOfEvents_.fromString( maxNumEvents );
	    }
// 	    LOG4CPLUS_INFO(logger_, "maxNumEvents: " + maxNumEvents );
// 	    LOG4CPLUS_INFO(logger_, "maxNumberOfEvents_: " + maxNumberOfEvents_.toString() );

            // Set controlDQM_ if and only if a DAQ control button was pressed.
	    controlDQM_ = controlDQM_checked;

	    fireEvent("Configure");
	  }
	else if ( (cmdName == "start") && fsm_.getCurrentState() == 'C' )
	  {
            // Set controlDQM_ if and only if a DAQ control button was pressed.
	    controlDQM_ = controlDQM_checked;

	    fireEvent("Enable");
	  }
        else if( cmdName == "stop" )
	  {
            // Set controlDQM_ if and only if a DAQ control button was pressed.
	    controlDQM_ = controlDQM_checked;

	    fireEvent("Halt");
	  }
        else if( cmdName == "reset" )
	  {
            // Set controlDQM_ if and only if a DAQ control button was pressed.
	    controlDQM_ = controlDQM_checked;

	    resetAction();
	    try{
	      fsm_.reset();
	    }
	    catch( toolbox::fsm::exception::Exception e ){
// 	      XCEPT_RETHROW(xgi::exception::Exception,
// 			    "Failed to reset FSM: ", e);
	      LOG4CPLUS_ERROR(logger_, "Failed to reset FSM: " << 
			      xcept::stdformat_exception_history(e) );
	      stringstream ss23;
	      ss23 <<  "Failed to reset FSM: ";
	      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss23.str(), e );
	      this->notifyQualified( "error", eObj );
	    }
	    fireEvent("Halt");
	  }
        else if( cmdName == "configure DQM" )
	  {
	    controlDQM( "Configure" );
	  }
        else if( cmdName == "start DQM" )
	  {
	    controlDQM( "Enable" );
	  }
        else if( cmdName == "stop DQM" )
	  {
	    controlDQM( "Halt" );
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
    int                           nbApps         = appDescriptors.size();
    xdaq::ApplicationDescriptor   *appDescriptor = 0;
    vector< pair<string,string> > params;


    *out << "<table border=\"0\">"                                     << endl;
    *out << "<tr>"                                                     << endl;

    for(int i=0; i<nbApps; i++)
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
        catch(xcept::Exception e)
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
    unsigned long instance  = appDescriptor->getInstance();
    string        href      = getHref(appDescriptor);
    int           nbRows    = params.size();


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

    for(int row=0; row<nbRows; row++)
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
    string                        eventNb;
    vector< pair<string,string> > params;

    try
    {
        eventNb = getScalarParam(evmDescriptor, "lastEventNumberFromTrigger", "unsignedInt");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to get eventNb from EVM", e);
    }

    params.push_back(pair<string,string>("eventNb", eventNb));

    return params;
}


vector< vector< pair<string,string> > > emu::daq::manager::Application::getStats
(
    vector< xdaq::ApplicationDescriptor* > &appDescriptors
)
{
    unsigned int                            nbApps = appDescriptors.size();
    unsigned int                            i      = 0;
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
    string                        s                          = "";
    double                        deltaT                     = 0.0;
    unsigned long                 deltaN                     = 0;
    double                        deltaSumOfSquares          = 0.0;
    unsigned long                 deltaSumOfSizes            = 0;
    bool                          retrievedDeltaT            = false;
    bool                          retrievedDeltaN            = false;
    bool                          retrievedDeltaSumOfSquares = false;
    bool                          retrievedDeltaSumOfSizes   = false;


    try
    {
        s = getScalarParam(appDescriptor, "stateName", "string");
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";

        LOG4CPLUS_ERROR(logger_, "Failed to get state"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss24;
        ss24 <<  "Failed to get state"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss24.str(), e );
        this->notifyQualified( "error", eObj );
    }
    stats.push_back(pair<string,string>("state", s));

    try
    {
        s = getScalarParam(appDescriptor, "deltaT", "double");
        deltaT = atof(s.c_str());
        retrievedDeltaT = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaT = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaT"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss25;
        ss25 <<  "Failed to get deltaT"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss25.str(), e );
        this->notifyQualified( "error", eObj );
    }
    stats.push_back(pair<string,string>("deltaT", s));

    try
    {
        s = getScalarParam(appDescriptor, "deltaN", "unsignedInt");
        deltaN = atoi(s.c_str());
        retrievedDeltaN = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaN = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaN"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss26;
        ss26 <<  "Failed to get deltaN"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss26.str(), e );
        this->notifyQualified( "error", eObj );
    }
    stats.push_back(pair<string,string>("deltaN", s));


    try
    {
        s = getScalarParam(appDescriptor, "deltaSumOfSquares", "double");
        deltaSumOfSquares = atof(s.c_str());
        retrievedDeltaSumOfSquares = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaSumOfSquares = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaSumOfSquares"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss27;
        ss27 <<  "Failed to get deltaSumOfSquares"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss27.str(), e );
        this->notifyQualified( "error", eObj );
    }
    stats.push_back(pair<string,string>("deltaSumOfSquares", s));

    try
    {
        s = getScalarParam(appDescriptor, "deltaSumOfSizes", "unsignedInt");
        deltaSumOfSizes = atoi(s.c_str());
        retrievedDeltaSumOfSizes = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaSumOfSizes = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaSumOfSizes"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss28;
        ss28 <<  "Failed to get deltaSumOfSizes"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss28.str(), e );
        this->notifyQualified( "error", eObj );
    }
    stats.push_back(pair<string,string>("deltaSumOfSizes", s));

    if(retrievedDeltaSumOfSizes && retrievedDeltaT)
    {
        // Avoid divide by zero
        if(deltaT != 0.0)
        {
            double throughput = deltaSumOfSizes / deltaT / 1000000.0;

            stringstream oss;
            string       s;
        
            oss << throughput << " MB/s";
            s = oss.str();

            stats.push_back(pair<string,string>("throughput", s));
        }
        else
        {
            stats.push_back(pair<string,string>("throughput", "DIV BY 0"));
        }
    }
    else
    {
        stats.push_back(pair<string,string>("throughput", "UNKNOWN"));
    }

    if(retrievedDeltaSumOfSizes && retrievedDeltaN)
    {
        // Avoid divide by zero
        if(deltaN != 0)
        {
            double average = deltaSumOfSizes / deltaN / 1000.0;

            stringstream oss;
            string       s;

            oss << average << " KB";
            s = oss.str();

            stats.push_back(pair<string,string>("average", s));
        }
        else
        {
             stats.push_back(pair<string,string>("average", "DIV BY 0"));
        }
    }
    else
    {
        stats.push_back(pair<string,string>("average", "UNKNOWN"));
    }

    if(retrievedDeltaN && retrievedDeltaT)
    {
        // Avoid divide by zero
        if(deltaT != 0.0)
        {
            double rate = ((double)deltaN) / deltaT / 1000.0;

            stringstream oss;
            string       s;

            oss << rate << " KHz";
            s = oss.str();

            stats.push_back(pair<string,string>("rate", s));
        }
        else
        {
            stats.push_back(pair<string,string>("rate", "DIV BY 0"));
        }
    }
    else
    {
        stats.push_back(pair<string,string>("rate", "UNKNOWN"));
    }

    if(retrievedDeltaSumOfSquares && retrievedDeltaN &&
       retrievedDeltaSumOfSizes)
    {
        // Avoid divide by zero
        if(deltaN != 0)
        {
            double meanOfSquares = deltaSumOfSquares / ((double)deltaN);
            double mean          = ((double)deltaSumOfSizes) / ((double)deltaN);
            double squareOfMean  = mean * mean;
            double variance      = meanOfSquares - squareOfMean;

            // Variance maybe negative due to lack of precision
            if(variance < 0)
            {
                variance = 0.0;
            }

            double rms  = sqrt(variance) / 1000.0;

            stringstream oss;
            string       s;

            oss << rms << " KB";
            s = oss.str();

            stats.push_back(pair<string,string>("rms", s));
        }
        else
        {
            stats.push_back(pair<string,string>("rms", "DIV BY 0"));
        }
    }
    else
    {
        stats.push_back(pair<string,string>("rms", "UNKNOWN"));
    }

    return stats;
}

void emu::daq::manager::Application::createAllAppStatesVector(){
  //
  // DAQ
  //
  daqAppStates_.clear();
  daqContexts_.clear();
  vector<xdaq::ApplicationDescriptor*> allApps;
  if ( buildEvents_.value_ ){ 
    allApps.insert( allApps.end(), evmDescriptors_.begin(), evmDescriptors_.end() );
    allApps.insert( allApps.end(), buDescriptors_ .begin(), buDescriptors_ .end() );
    allApps.insert( allApps.end(), ruDescriptors_ .begin(), ruDescriptors_ .end() );
    allApps.insert( allApps.end(), fuDescriptors_ .begin(), fuDescriptors_ .end() );
  }
  allApps.insert( allApps.end(), taDescriptors_ .begin(), taDescriptors_ .end() );
  allApps.insert( allApps.end(), ruiDescriptors_.begin(), ruiDescriptors_.end() );
  vector<xdaq::ApplicationDescriptor*>::iterator a;
  for ( a=allApps.begin(); a!=allApps.end(); ++a ){
    daqAppStates_.push_back( make_pair( *a, string("UNKNOWN") ) );
    // Collect different contexts too
    daqContexts_.insert( (*a)->getContextDescriptor()->getURL() );
  }
  //
  // DQM
  //
  dqmAppStates_.clear();
  dqmContexts_.clear();
  for ( a=dqmMonitorDescriptors_.begin(); a!=dqmMonitorDescriptors_.end(); ++a ){
    dqmAppStates_.push_back( make_pair( *a, string("UNKNOWN") ) );
    // Collect different contexts too
    dqmContexts_.insert( (*a)->getContextDescriptor()->getURL() );
  }
}



void emu::daq::manager::Application::queryAppStates( vector< pair<xdaq::ApplicationDescriptor*, string> > &appStates ){
  vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator as;
  for ( as=appStates.begin(); as!=appStates.end(); ++as ){
    string s;
    try
      {
	s = getScalarParam(as->first, "stateName", "string");
      }
    catch(xcept::Exception e)
      {
	s = "UNKNOWN";
	LOG4CPLUS_WARN(logger_, "Failed to get state"
			<< " : " << xcept::stdformat_exception_history(e));
	stringstream ss29;
	ss29 <<  "Failed to get state"
			<< " : " ;
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss29.str(), e );
	this->notifyQualified( "warn", eObj );
      }
    as->second = s;
  }
}

string emu::daq::manager::Application::getDAQState(){
//   queryAllAppStates();
  queryAppStates( daqAppStates_ );

  // Combine states:
  // If one is failed, the combined state will also be failed.
  // Else, if one is unknown, the combined state will also be unknown.
  // Else, if all are known but not the same, the combined state will be indefinite.
  string combinedState("UNKNOWN");
  vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator s;
  // First check if any failed:
  for ( s=daqAppStates_.begin(); s!=daqAppStates_.end(); ++s )
    if ( s->second == "Failed" ){
      combinedState = s->second;
      return combinedState;
    }
  // If none failed:
  for ( s=daqAppStates_.begin(); s!=daqAppStates_.end(); ++s ){
    if ( s->second == "UNKNOWN" ){
      combinedState = s->second;
      break;
    }
    else if ( s->second != combinedState && combinedState != "UNKNOWN" ){
      combinedState = "INDEFINITE";
      break;
    }
    else{
      if ( s->second.find( "Mismatch", 0 ) != string::npos )
	// DAQ is still "enabled" while RU is seeing mismatch but has not timed out
	combinedState = "Enabled";
      else
	combinedState = s->second;
    }
  }
  return combinedState;
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
  *out << ( globalMode_.value_ ? 
	    "<em>supervised</em> mode (controlled by <span style=\"font-weight:bold; border-color:#000000; border-style:solid; border-width:thin; padding:2px\">Supervisor | Function Manager | Central Run Control</span>)." : 
	    "<em>unsupervised</em> mode (controlled from this page by  <span style=\"font-weight:bold; border-color:#000000; border-style:solid; border-width:thin; padding:2px\">CSC Shift</span>)." ) << endl;
}


void emu::daq::manager::Application::printStatesTable( xgi::Output *out,
				      string title,
				      set<string> &contexts,
				      vector< pair<xdaq::ApplicationDescriptor*, string> > &appStates  )
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

  // First find out if any application (RU) is in "Mismatch..." or "TimedOut..." state
  bool isMismatch = false;
  bool isTimedOut = false;
  for ( vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator s 
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
      vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator s;
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

void 
emu::daq::manager::Application::startATCP()
  throw (emu::daq::manager::exception::Exception){
  // configure and enable all pt::atcp::PeerTransportATCP

  std::cout << "In emu::daq::manager::Application::startATCP()" << std::endl;

  vector < xdaq::ApplicationDescriptor* > atcpDescriptors;
  try
    {
      atcpDescriptors = getAppDescriptors(zone_, "pt::atcp::PeerTransportATCP");
    }
  catch(emu::daq::manager::exception::Exception e)
    {
      atcpDescriptors.clear();
    }

  std::cout << atcpDescriptors.size() << " atcpDescriptors" << std::endl;

  vector < xdaq::ApplicationDescriptor* >::const_iterator atcpd;
  for ( atcpd = atcpDescriptors.begin(); atcpd != atcpDescriptors.end(); ++atcpd ){

    std::cout << appDescriptor_->getContextDescriptor()->getURL() << "         "
	      << (*atcpd)->getContextDescriptor()->getURL() << std::endl;

//     // ATCP may already have been started. Check its state.
//     string atcpState;
//     try{
//       atcpState = getScalarParam( *atcpd, "stateName", "string" );
//     }
//     catch(emu::daq::manager::exception::Exception e)
//     {
//       stringstream oss;
//       oss << "Failed to get state of " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
//       XCEPT_RETHROW(emu::daq::manager::exception::Exception, oss.str(), e);
//     }

//     if ( atcpState != "Halted" ) continue;

//     // Configure ATCP
//     try{
//       sendFSMEventToApp("Configure", *atcpd);
//     }
//     catch(xcept::Exception e){
//       stringstream oss;
//       oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
//       XCEPT_RETHROW(emu::daq::manager::exception::Exception, oss.str(), e);
//     }

//     // Enable ATCP
//     try{
//       sendFSMEventToApp("Enable", *atcpd);
//     }
//     catch(xcept::Exception e){
//       stringstream oss;
//       oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
//       XCEPT_RETHROW(emu::daq::manager::exception::Exception, oss.str(), e);
//     }

  }
  
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
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Not enough applications to make a RU builder", e);
	}
      
      try
	{
	  setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell EVM whether or not to generate dummy triggers", e);
	}
      
      try
	{
	  setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell RUs whether or not to generate dummy super-fragments",
			e);
	}

      try
	{
	  setBUsDropEvents(busDropEvents);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell BUs whether or not drop events", e);
	}
    }

    // If the TA is present then start it as an imaginary trigger
    if ( taDescriptors_.size() ){
      if ( taDescriptors_.size() > 1 ){
	LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.");
	stringstream ss30;
	ss30 << "The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.";
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss30.str() );
	this->notifyQualified( "warn", eObj );
      }
      string runNumber    = runNumber_.toString();
      try
	{
	  setScalarParam(taDescriptors_[0],"runNumber","unsignedLong",runNumber);
	  LOG4CPLUS_INFO(logger_,"Set run number to " + runNumber );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run number to "  + runNumber, e);
	}
      // move to enableAction START
//       try
// 	{
// 	  setScalarParam(taDescriptors_[0],"isBookedRunNumber","boolean",string(isBookedRunNumber_?"true":"false"));
// 	  LOG4CPLUS_INFO(logger_,string("Set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false.") );
// 	}
//       catch(xcept::Exception e)
// 	{
// 	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
// 			string("Failed to set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false."), e);
// 	}
      // move to enableAction END

      // ATCP must be started explicitly
      try{
	startATCP();
      }
      catch(xcept::Exception e){
	XCEPT_RETHROW(emu::daq::manager::exception::Exception, "Failed to start ATCP ", e);
      }

      string maxNumEvents = maxNumberOfEvents_.toString();
      try
	{
	  setScalarParam(taDescriptors_[0],"maxNumTriggers","integer",maxNumEvents);
	  LOG4CPLUS_INFO(logger_,"Set maximum number of events to " + maxNumEvents );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set maximum number of events to "  + maxNumEvents, e);
	}
        try
        {
            configureTrigger();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to configure trigger", e);
        }
    }
    else{
      LOG4CPLUS_ERROR(logger_,"No TA found.");
      stringstream ss31;
      ss31 << "No TA found.";
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss31.str() );
      this->notifyQualified( "error", eObj );
    }

    if ( buildEvents_.value_ ){
      try
	{
	  configureRuBuilder();
	}
      catch(xcept::Exception e)
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
            configureFedBuilder();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to configure FED builder", e);
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
	  catch(xcept::Exception e)
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
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Not enough applications to make a RU builder", e);
	}
      
      try
	{
	  setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell EVM whether or not to generate dummy triggers", e);
	}

      try
	{
	  setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell RUs whether or not to generate dummy super-fragments",
			e);
	}
      
      try
	{
	  setBUsDropEvents(busDropEvents);
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to tell BUs whether or not drop events", e);
	}
    }

    // If the TA is present then start it as an imaginary trigger
    if(taDescriptors_.size() > 0)
    {
      // MOVED FROM configureDAQ BEGIN
      string runNumber    = runNumber_.toString();
      string maxNumEvents = maxNumberOfEvents_.toString();
      try
	{
	  setScalarParam(taDescriptors_[0],"runNumber","unsignedLong",runNumber);
	  LOG4CPLUS_INFO(logger_,"Set run number to " + runNumber );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run number to "  + runNumber, e);
	}
      try
	{
	  setScalarParam(taDescriptors_[0],"isBookedRunNumber","boolean",string(isBookedRunNumber_?"true":"false"));
	  LOG4CPLUS_INFO(logger_,string("Set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false.") );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			string("Failed to set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false."), e);
	}
      try
	{
	  setScalarParam(taDescriptors_[0],"maxNumTriggers","integer",maxNumEvents);
	  LOG4CPLUS_INFO(logger_,"Set maximum number of events to " + maxNumEvents );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set maximum number of events to "  + maxNumEvents, e);
	}
      // MOVED FROM configureDAQ END
        try
        {
            startTrigger();
        }
        catch(xcept::Exception e)
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
      catch(xcept::Exception e)
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
            startFedBuilder();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emu::daq::manager::exception::Exception,
                "Failed to start FED builder", e);
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
	  catch(xcept::Exception e)
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
    string valueStr = value ? "true" : "false";


    try
    {
        setScalarParam(evmDescriptors_[0], "generateDummyTriggers", "boolean",
            valueStr);
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to set generateDummyTriggers of ";
        oss << evmDescriptors_[0]->getClassName();
        oss << evmDescriptors_[0]->getInstance();
        oss << " to " << valueStr;
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


void emu::daq::manager::Application::setRUsGenerateDummySuperFrags(const bool value)
throw (emu::daq::manager::exception::Exception)
{
    string valueStr = value ? "true" : "false";
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
            setScalarParam(*pos, "generateDummySuperFragments", "boolean",
                valueStr);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to set generateDummySuperFragments of ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            oss << " to " << valueStr;
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::setBUsDropEvents(const bool value)
throw (emu::daq::manager::exception::Exception)
{
    string valueStr = value ? "true" : "false";
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    for(pos = buDescriptors_.begin(); pos != buDescriptors_.end(); pos++)
    {
        try
        {
            setScalarParam(*pos, "dropEventData", "boolean", valueStr);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to set dropEventData of ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            oss << " to " << valueStr;
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }
}


void emu::daq::manager::Application::configureTrigger()
throw (emu::daq::manager::exception::Exception)
{
    //////////////////
    // Configure TA //
    //////////////////

    try
    {
        sendFSMEventToApp("Configure", taDescriptors_[0]);
    }
    catch(xcept::Exception e)
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

void emu::daq::manager::Application::startTrigger()
throw (emu::daq::manager::exception::Exception)
{
    /////////////////
    // Enable TA   //
    /////////////////

    try
    {
        sendFSMEventToApp("Enable", taDescriptors_[0]);
    }
    catch(xcept::Exception e)
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
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////////
    // Configure EVM //
    ///////////////////

    try
    {
        sendFSMEventToApp("Configure", evmDescriptors_[0]);
    }
    catch(xcept::Exception e)
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
            sendFSMEventToApp("Configure", *pos);
        }
        catch(xcept::Exception e)
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
            sendFSMEventToApp("Configure", *pos);
        }
        catch(xcept::Exception e)
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
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;
    ////////////////
    // Enable RUs //
    ////////////////

    for(pos = ruDescriptors_.begin(); pos != ruDescriptors_.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Enable", *pos);
        }
        catch(xcept::Exception e)
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
        sendFSMEventToApp("Enable", evmDescriptors_[0]);
    }
    catch(xcept::Exception e)
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
            sendFSMEventToApp("Enable", *pos);
        }
        catch(xcept::Exception e)
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


void emu::daq::manager::Application::startFedBuilder()
throw (emu::daq::manager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


//     ////////////////////
//     // Configure RUIs //
//     ////////////////////

//     for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
//     {
//         try
//         {
//             sendFSMEventToApp("Configure", *pos);
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string       s;

//             oss << "Failed to configure ";
//             oss << (*pos)->getClassName() << (*pos)->getInstance();
//             s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
//         }
//     }


    /////////////////
    // Enable RUIs //
    /////////////////

    for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Enable", *pos);
        }
        catch(xcept::Exception e)
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

void emu::daq::manager::Application::configureFedBuilder()
throw (emu::daq::manager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ////////////////////
    // Configure RUIs //
    ////////////////////

    for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
      stringstream app;
      app << (*pos)->getClassName() << (*pos)->getInstance();
      try
	{
	  setScalarParam(*pos,"runType","string",runType_.toString());
	  LOG4CPLUS_INFO(logger_,"Set run type for " + app.str() + " to " + runType_.toString() );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run type for " + app.str() + " to "  + runType_.toString(), e);
	}
      try
	{
	  setScalarParam(*pos,"passDataOnToRUBuilder","boolean",buildEvents_.toString());
	  LOG4CPLUS_INFO(logger_,"Set event building " + buildEvents_.toString() );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set event building "  + buildEvents_.toString(), e);
	}
      
        try
        {
            sendFSMEventToApp("Configure", *pos);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }


//     /////////////////
//     // Enable RUIs //
//     /////////////////

//     for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
//     {
//         try
//         {
//             sendFSMEventToApp("Enable", *pos);
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string       s;

//             oss << "Failed to enable ";
//             oss << (*pos)->getClassName() << (*pos)->getInstance();
//             s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
//         }
//     }
}


void emu::daq::manager::Application::configureFilterFarm()
  //throw (emu::daq::manager::exception::Exception)
throw (xcept::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;

    getIdsOfRunningConfigurationsFromFM();

    ///////////////////
    // Configure FUs //
    ///////////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
      stringstream  app;
      app << (*pos)->getClassName() << (*pos)->getInstance();
      try
	{
	  setScalarParam(*pos,"runType","string",runType_.toString());
	  LOG4CPLUS_INFO(logger_,"Set run type for " + app.str() + " to " + runType_.toString() );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			"Failed to set run type for " + app.str() + " to "  + runType_.toString(), e);
	}

      try
	{
	  setScalarParam(*pos,"CSCConfigId","unsignedLong",CSCConfigId_.toString());
	  LOG4CPLUS_INFO(logger_,"Set CSC configuration id in " + app.str() + " to " + CSCConfigId_.toString() );
	}
      catch(emu::daq::manager::exception::Exception e)
	{
	  // xdata/src/common/soap/InfoSpaceSerializer.cc puts an XML tag into the exception it throws, 
	  // causing the logger to drop it ==> put it in CDATA
	  // TODO: for each postSOAP...
	  LOG4CPLUS_ERROR(logger_, "Failed to set CSC configuration id in " + app.str() 
			  + " to " + CSCConfigId_.toString() + 
			  " : <![CDATA[" + xcept::stdformat_exception_history(e) + "]]>");
	  stringstream ss32;
	  ss32 <<  "Failed to set CSC configuration id in " + app.str() 
			  + " to " + CSCConfigId_.toString() + 
			  " : <![CDATA["  + "]]>";
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss32.str(), e );
	  this->notifyQualified( "error", eObj );
	}

      try
	{
	  setScalarParam(*pos,"TFConfigId","unsignedLong",TFConfigId_.toString());
	  LOG4CPLUS_INFO(logger_,"Set TF configuration id in " + app.str() + " to " + TFConfigId_.toString() );
	}
      catch(emu::daq::manager::exception::Exception e)
	{
	  // xdata/src/common/soap/InfoSpaceSerializer.cc puts an XML tag into the exception it throws, 
	  // causing the logger to drop it ==> put it in CDATA
	  // TODO: for each postSOAP...
	  LOG4CPLUS_ERROR(logger_, "Failed to set TF configuration id in " + app.str() 
			  + " to " + TFConfigId_.toString() + 
			  " : <![CDATA[" + xcept::stdformat_exception_history(e) + "]]>");
	  stringstream ss33;
	  ss33 <<  "Failed to set TF configuration id in " + app.str() 
			  + " to " + TFConfigId_.toString() + 
			  " : <![CDATA["  + "]]>";
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss33.str(), e );
	  this->notifyQualified( "error", eObj );
	}


        try
        {
            sendFSMEventToApp("Configure", *pos);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to configure ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
        }
    }


//     ////////////////
//     // Enable FUs //
//     ////////////////

//     for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
//     {
//         try
//         {
//             sendFSMEventToApp("Enable", *pos);
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string       s;

//             oss << "Failed to enable ";
//             oss << (*pos)->getClassName() << (*pos)->getInstance();
//             s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
//         }
//     }
}


void emu::daq::manager::Application::startFilterFarm()
throw (emu::daq::manager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


//     ///////////////////
//     // Configure FUs //
//     ///////////////////

//     for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
//     {
//         try
//         {
//             sendFSMEventToApp("Configure", *pos);
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string       s;

//             oss << "Failed to configure ";
//             oss << (*pos)->getClassName() << (*pos)->getInstance();
//             s = oss.str();

//             XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
//         }
//     }


    ////////////////
    // Enable FUs //
    ////////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Enable", *pos);
        }
        catch(xcept::Exception e)
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
  makeTAGenerateRunStopTime();

    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            stopFedBuilder();
        }
        catch(xcept::Exception e)
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
      catch(xcept::Exception e)
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
        catch(xcept::Exception e)
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
	  catch(xcept::Exception e)
	    {
	      XCEPT_RETHROW(emu::daq::manager::exception::Exception,
			    "Failed to stop filter farm", e);
	    }
	}
    }
}

void emu::daq::manager::Application::controlDQM( const string action )
  throw (emu::daq::manager::exception::Exception)
{
  // EmuMonitors of DQM
  vector< xdaq::ApplicationDescriptor* >::iterator mon;
  for( mon = dqmMonitorDescriptors_.begin(); mon != dqmMonitorDescriptors_.end(); ++mon ){

    try
      {
	sendFSMEventToApp(action, *mon);
      }
    catch(xcept::Exception e)
      {
// 	stringstream oss;
	
// 	oss << "Failed to " << action << " ";
// 	oss << (*mon)->getClassName() << (*mon)->getInstance();
	
// 	XCEPT_RETHROW(emu::daq::manager::exception::Exception, oss.str(), e);

	// Don't raise exception here. Go on to try to deal with the others.
	LOG4CPLUS_ERROR(logger_, "Failed to " << action << " " 
			<< (*mon)->getClassName() << (*mon)->getInstance() << " "
			<< xcept::stdformat_exception_history(e));
	stringstream ss34;
	ss34 <<  "Failed to " << action << " " 
			<< (*mon)->getClassName() << (*mon)->getInstance() << " "
			;
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss34.str(), e );
	this->notifyQualified( "error", eObj );
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
        catch(xcept::Exception e)
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
      catch(xcept::Exception e)
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
        catch(xcept::Exception e)
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
	  catch(xcept::Exception e)
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

  stringstream oss;

    for( vector< xdaq::ApplicationDescriptor* >::iterator pos = apps.begin(); pos != apps.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Reset", *pos);
        }
        catch(xcept::Exception e)
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
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    //////////////
    // Halt EVM //
    //////////////

    try
    {
        sendFSMEventToApp("Halt", evmDescriptors_[0]);
    }
    catch(xcept::Exception e)
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
            sendFSMEventToApp("Halt", *pos);
        }
        catch(xcept::Exception e)
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
            sendFSMEventToApp("Halt", *pos);
        }
        catch(xcept::Exception e)
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
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt RUIs //
    ///////////////

    stringstream oss_all;

    for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Halt", *pos);
        }
        catch(xcept::Exception e)
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
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    /////////////
    // Halt TA //
    /////////////

    try
    {
        sendFSMEventToApp("Halt", taDescriptors_[0]);
    }
    catch(xcept::Exception e)
    {
        stringstream oss;
        string       s;

        oss << "Failed to halt ";
        oss << (*pos)->getClassName() << (*pos)->getInstance();
        s = oss.str();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


void emu::daq::manager::Application::stopFilterFarm()
throw (emu::daq::manager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt FUs  //
    ///////////////

    for(pos = fuDescriptors_.begin(); pos != fuDescriptors_.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Halt", *pos);
        }
        catch(xcept::Exception e)
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


void emu::daq::manager::Application::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emu::daq::manager::exception::Exception)
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

            XCEPT_RAISE(emu::daq::manager::exception::Exception, s);
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

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


xoap::MessageReference emu::daq::manager::Application::createSimpleSOAPCmdMsg
(
    const string cmdName
)
throw (emu::daq::manager::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}


map<string,string> emu::daq::manager::Application::getScalarParams
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const map<string,string>     paramNamesAndTypes
)
throw (emu::daq::manager::exception::Exception)
{
    string appClass = appDescriptor->getClassName();
    map<string,string> paramNamesAndValues;

    try
    {
        xoap::MessageReference msg =
            createParametersGetSOAPMsg(appClass, paramNamesAndTypes);

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

            XCEPT_RAISE(emu::daq::manager::exception::Exception, s);
        }

        paramNamesAndValues = extractScalarParameterValuesFromSoapMsg(reply, paramNamesAndTypes);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }

    return paramNamesAndValues;
}

string emu::daq::manager::Application::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emu::daq::manager::exception::Exception)
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

            XCEPT_RAISE(emu::daq::manager::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        stringstream s;
	s << "Failed to get scalar parameter " 
	  << paramName << " from application " 
	  << appDescriptor->getClassName() << appDescriptor->getInstance();

        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s.str(), e);
    }

    return value;
}


void emu::daq::manager::Application::setScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType,
    const string                 paramValue
)
throw (emu::daq::manager::exception::Exception)
{
    string appClass = appDescriptor->getClassName();

    try
    {
        xoap::MessageReference msg = createParameterSetSOAPMsg(appClass,
                                     paramName, paramType, paramValue);

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

            XCEPT_RAISE(emu::daq::manager::exception::Exception, s);
        }
    }
    catch(xdaq::exception::Exception e)
    {
        string s = "Failed to set scalar parameter";
        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        string s = "Failed to set scalar parameter";
        XCEPT_RETHROW(emu::daq::manager::exception::Exception, s, e);
    }
}


xoap::MessageReference emu::daq::manager::Application::createParametersGetSOAPMsg
(
    const string             appClass,
    const map<string,string> paramNamesAndTypes
)
throw (emu::daq::manager::exception::Exception)
{
    string appNamespace = "urn:xdaq-application:" + appClass;

    string problemParams = "";
    for ( map<string,string>::const_iterator pnt = paramNamesAndTypes.begin(); 
	  pnt != paramNamesAndTypes.end(); ++pnt ) problemParams += pnt->first + "(" + pnt->second + ") ";

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

	for ( map<string,string>::const_iterator pnt = paramNamesAndTypes.begin(); 
	      pnt != paramNamesAndTypes.end(); ++pnt ){
	  problemParams = pnt->first + "(" + pnt->second + ") ";

	  xoap::SOAPName propertyName =
            envelope.createName(pnt->first, "xapp", appNamespace);
	  xoap::SOAPElement propertyElement =
            propertiesElement.addChildElement(propertyName);
	  xoap::SOAPName propertyTypeName =
	    envelope.createName("type", "xsi",
				"http://www.w3.org/2001/XMLSchema-instance");

	  propertyElement.addAttribute(propertyTypeName, "xsd:" + pnt->second);

	}

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            problemParams, e);
    }
}

xoap::MessageReference emu::daq::manager::Application::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emu::daq::manager::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}


xoap::MessageReference emu::daq::manager::Application::createParameterSetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType,
    const string paramValue
)
throw (emu::daq::manager::exception::Exception)
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
            envelope.createName("ParameterSet", "xdaq", "urn:xdaq-soap:3.0");
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

        propertyElement.addTextNode(paramValue);

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Failed to create ParameterSet SOAP message for parameter " +
            paramName + " of type " + paramType + " with value " + paramValue,
            e);
    }
}


map<string,string> emu::daq::manager::Application::extractScalarParameterValuesFromSoapMsg
(
    xoap::MessageReference   msg,
    const map<string,string> paramNamesAndTypes
)
throw (emu::daq::manager::exception::Exception)
{
    map<string,string> paramNamesAndValues;

    string paramName = "";
    for ( map<string,string>::const_iterator pnt = paramNamesAndTypes.begin(); 
	  pnt != paramNamesAndTypes.end(); ++pnt ) paramName += pnt->first + " ";

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
	
	for ( map<string,string>::const_iterator pnt = paramNamesAndTypes.begin(); 
	      pnt != paramNamesAndTypes.end(); ++pnt ){
	  paramName = pnt->first;
	  DOMNode *paramNode = findNode(propertiesList, pnt->first);
	  DOMNodeList *paramList = paramNode->getChildNodes();
	  DOMNode *valueNode = paramList->item(0);
	  if ( valueNode )
	    paramNamesAndValues[paramName] = xoap::XMLCh2String(valueNode->getNodeValue());
	  else
	    paramNamesAndValues[paramName] = "";
	}

        return paramNamesAndValues;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::manager::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

string emu::daq::manager::Application::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emu::daq::manager::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::manager::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::manager::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}


DOMNode *emu::daq::manager::Application::findNode
(
    DOMNodeList *nodeList,
    const string nodeLocalName
)
throw (emu::daq::manager::exception::Exception)
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

    XCEPT_RAISE(emu::daq::manager::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}

xoap::MessageReference emu::daq::manager::Application::postSOAP( xoap::MessageReference message, 
						const string& URL,
						const string& SOAPAction ) 
  throw (xdaq::exception::Exception)
// Adapted from xdaq::ApplicationContextImpl::postSOAP.
// This is necessary for sending SOAP to contexts not defined in this process's config file.
{
	
  bool setSOAPAction = false;
  if ( message->getMimeHeaders()->getHeader("SOAPAction").size() == 0 )
    {
      message->getMimeHeaders()->setHeader("SOAPAction", SOAPAction);		
      setSOAPAction = true;
    }
	
  xoap::SOAPBody b = message->getSOAPPart().getEnvelope().getBody();
  DOMNode* node = b.getDOMNode();
	
  DOMNodeList* bodyList = node->getChildNodes();
  DOMNode* command = bodyList->item(0);
	
  if (command->getNodeType() == DOMNode::ELEMENT_NODE) 
    {                

      try{
	// Check format of URL
	toolbox::net::URL u( URL );
      }
      catch (toolbox::net::exception::MalformedURL& mu){
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", mu);
      }

      try
	{	
	  // Local dispatch: if remote and local address are on same host, get local messenger
			
	  // Get the address on the fly from the URL
	  pt::Address::Reference remoteAddress = pt::getPeerTransportAgent()
	    ->createAddress(URL,"soap");
				
	  pt::Address::Reference localAddress = 
	    pt::getPeerTransportAgent()->createAddress(getApplicationDescriptor()->getContextDescriptor()->getURL(),"soap");
			
	  // force here protocol http, service soap, because at this point we know over withc protocol/service to send.
	  // this allows specifying a host URL without the SOAP service qualifier
	  //		
	  std::string protocol = remoteAddress->getProtocol();
			
	  pt::PeerTransportSender* s = dynamic_cast<pt::PeerTransportSender*>(pt::getPeerTransportAgent()->getPeerTransport (protocol, "soap", pt::Sender));

	  // These two lines cannot be merges, since a reference that is a temporary object
	  // would delete the contained object pointer immediately after use.
	  //
	  pt::Messenger::Reference mr = s->getMessenger(remoteAddress, localAddress);
	  pt::SOAPMessenger& m = dynamic_cast<pt::SOAPMessenger&>(*mr);
	  xoap::MessageReference rep = m.send(message);	 
			
	  if (setSOAPAction)
	    {
	      message->getMimeHeaders()->removeHeader("SOAPAction");
	    }
	  return rep;
	}
      catch (xdaq::exception::HostNotFound& hnf)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", hnf);
	} 
      catch (xdaq::exception::ApplicationDescriptorNotFound& acnf)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", acnf);
	}
      catch (pt::exception::Exception& pte)
	{
	  XCEPT_RETHROW (xdaq::exception::Exception, "Failed to post SOAP message", pte);
	}
      catch(std::exception& se)
	{
	  XCEPT_RAISE (xdaq::exception::Exception, se.what());
	}
      catch(...)
	{
	  XCEPT_RAISE (xdaq::exception::Exception, "Failed to post SOAP message, unknown exception");
	}
    } 
  else
    {
      /*applicationDescriptorFactory_.unlock();
       */
      XCEPT_RAISE (xdaq::exception::Exception, "Bad SOAP message. Cannot find command tag");
    }

}

xoap::MessageReference emu::daq::manager::Application::makeTAGenerateRunStopTime(){

  xoap::MessageReference reply;

  try{
    
    // Create query message
    xoap::MessageReference message = createSimpleSOAPCmdMsg("generateRunStopTime");

    // Post it
    reply = appContext_->postSOAP(message, *appDescriptor_, *taDescriptors_[0]);
    
    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    
    if(replyBody.hasFault())
      {
	LOG4CPLUS_WARN(logger_, "Failed to make emu::daq::ta::Application generate run stop time: Received fault reply: " 
		       << replyBody.getFault().getFaultString() );
	stringstream ss44;
	ss44 <<  "Failed to make emu::daq::ta::Application generate run stop time: Received fault reply: " 
		       << replyBody.getFault().getFaultString() ;
	XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss44.str() );
	this->notifyQualified( "warn", eObj );
      }
  } catch(xcept::Exception e){
    LOG4CPLUS_WARN(logger_, "Failed to make emu::daq::ta::Application generate run stop time: " 
		   << xcept::stdformat_exception_history(e));
    stringstream ss45;
    ss45 <<  "Failed to make emu::daq::ta::Application generate run stop time: " 
		   ;
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss45.str(), e );
    this->notifyQualified( "warn", eObj );

  }

  return reply;
}


vector<string> emu::daq::manager::Application::parseRunningConfigurationsReplyFromFM( xoap::MessageReference reply ){

  vector<string> runningConfigs;

  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");

  std::stringstream ss;
  reply->writeTo( ss );
  DOMDocument* doc = parser->parse( ss.str() );
  DOMNodeList *URIs = doc->getElementsByTagName( xoap::XStr("URI") );
  for ( XMLSize_t i=0; i<URIs->getLength(); ++i ){
    runningConfigs.push_back( xoap::XMLCh2String( URIs->item(i)->getTextContent() ) );
  }

  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  return runningConfigs;
}


vector<string> emu::daq::manager::Application::getRunningConfigurationsFromFM( const string& baseURL )
  throw (emu::daq::manager::exception::Exception){

  vector<string> runningConfigs;

  string serviceURL = baseURL + "/rcms/services/FMLifeCycle";

  // Create SOAP message
  xoap::MessageReference message;
  try{
    message = xoap::createMessage();
    
    xoap::SOAPPart     soapPart = message->getSOAPPart();
    xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
    
    envelope.addNamespaceDeclaration("xsi" ,"http://www.w3.org/2001/XMLSchema-instance");
    envelope.addNamespaceDeclaration("xsd" ,"http://www.w3.org/2001/XMLSchema");
    envelope.addNamespaceDeclaration("soap","http://schemas.xmlsoap.org/soap/envelope/");
    
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName name = envelope.createName("getRunningConfigurations","ns1","urn:FMLifeCycle");
    body.addBodyElement( name );
  }
  catch(xcept::Exception &e){
    XCEPT_RETHROW(emu::daq::manager::exception::Exception,
		  "Failed to create message to " + serviceURL + " : ", 
		  e);
  }
  catch(...){
    XCEPT_RAISE(emu::daq::manager::exception::Exception,
		"Failed to create message to " + serviceURL);
  }

  // Send SOAP message

  cout << endl << "Sending to " << serviceURL << endl; 
  message->writeTo( cout );
  cout.flush();
  cout << endl;

  xoap::MessageReference reply;
  try{
    xoap::MessageReference reply = postSOAP( message, serviceURL, "urn:FMLifeCycle" );

    cout << endl << "Received reply from " << serviceURL << endl; 
    reply->writeTo( cout );
    cout.flush();
    cout << endl;

    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    
    if(replyBody.hasFault()){ // TODO: find out why hasFault() doesn't work
      std::stringstream ss;
      ss << "Received fault reply from " << serviceURL << " : "
	 << replyBody.getFault().getFaultString();
      XCEPT_RAISE(emu::daq::manager::exception::Exception, ss.str());
    }

    runningConfigs = parseRunningConfigurationsReplyFromFM( reply );

  }
  catch(xdaq::exception::Exception &e){
    XCEPT_RETHROW(emu::daq::manager::exception::Exception, 
		  "Failed to get running configurations from " + serviceURL + " : ",
		  e);
  }
  
  for ( unsigned int i=0; i<runningConfigs.size(); ++i ) cout << "Config " << i << "  " << runningConfigs[i] << endl;

  return runningConfigs;
}

string emu::daq::manager::Application::parseConfigParameterReplyFromFM( xoap::MessageReference reply )
  throw(emu::daq::manager::exception::Exception){

  string parameterValue;

  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");

  std::stringstream ss;
  reply->writeTo( ss );
  DOMDocument* doc = parser->parse( ss.str() );
  DOMNodeList *values = doc->getElementsByTagName( xoap::XStr("value") );
  if ( values->getLength() == 1 ){
    parameterValue = xoap::XMLCh2String( values->item(0)->getTextContent() );
  }
  else{
    stringstream ss;
    ss << "Got " << (unsigned int) values->getLength() << " values in reply to getParameter SOAP to FM";
    XCEPT_RAISE(emu::daq::manager::exception::Exception, ss.str() );
  }

  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  return parameterValue;
}


string emu::daq::manager::Application::getConfigParameterFromFM( const string& configurationURL,
						const string& parameterName )
  throw (emu::daq::manager::exception::Exception){

  string parameterValue;

  // Parse URL
  string baseURL;
  string configurationURN;
  vector<string> matches; // matches[1] will be the base URL (http://host:port), matches[2] the configuration URN
  if ( toolbox::regx_match( configurationURL, "^http://.*:[0-9]{2,5}/.*$") ){ // Make sure there's a match...
    toolbox::regx_match( configurationURL, "^(http://.*:[0-9]{2,5})/(.*)$", matches ); // ...because this crashes if no match.
    baseURL          = matches[1];
    configurationURN = matches[2];
  }
  else{
    return parameterValue;
  }
  string serviceURL = baseURL + "/rcms/services/ParameterController";

  // Create SOAP message
  xoap::MessageReference message;
  try{
    message = xoap::createMessage();
    
    xoap::SOAPPart     soapPart = message->getSOAPPart();
    xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
    
    envelope.addNamespaceDeclaration("xsi"    ,"http://www.w3.org/2001/XMLSchema-instance");
    envelope.addNamespaceDeclaration("xsd"    ,"http://www.w3.org/2001/XMLSchema");
    envelope.addNamespaceDeclaration("soap"   ,"http://schemas.xmlsoap.org/soap/envelope/");
    envelope.addNamespaceDeclaration("soapenc","http://schemas.xmlsoap.org/soap/encoding/");
    envelope.addNamespaceDeclaration("napesp1","http://namespaces.soaplite.com/perl");
    
    xoap::SOAPBody body = envelope.getBody();

    // <getParameter>
    xoap::SOAPName name = envelope.createName("getParameter","ns1","http://parameter.ws.fm.rcms");
    xoap::SOAPBodyElement getParameterElement = body.addBodyElement( name );

    // <uriPath>
    name = envelope.createName("uriPath","","");
    xoap::SOAPElement uriPathElement = getParameterElement.addChildElement(name);
    xoap::SOAPName attName = envelope.createName("xsi:type","","");
    uriPathElement.addAttribute( attName, "soapenc:string" );
    uriPathElement.setTextContent( configurationURL );

    // <requested>
    name = envelope.createName("requested","","");
    xoap::SOAPElement requestedElement = getParameterElement.addChildElement(name);
    attName = envelope.createName("xsi:type","","");
    requestedElement.addAttribute( attName, "namesp1:ParameterBean" );

    // <item>
    name = envelope.createName("item","","");
    xoap::SOAPElement itemElement = requestedElement.addChildElement(name);

    // <name>
    name = envelope.createName("name","","");
    xoap::SOAPElement nameElement = itemElement.addChildElement(name);
    attName = envelope.createName("xsi:type","","");
    nameElement.addAttribute( attName, "soapenc:string" );
    nameElement.setTextContent( parameterName );

//     // <type> // probably unnecessary for getParameter
//     name = envelope.createName("type","","");
//     xoap::SOAPElement typeElement = itemElement.addChildElement(name);
//     attName = envelope.createName("xsi:type","","");
//     typeElement.addAttribute( attName, "soapenc:string" );
//     typeElement.setTextContent("rcms.fm.fw.parameter.type.StringT");

//     // <value> // probably unnecessary for getParameter
//     name = envelope.createName("value","","");
//     xoap::SOAPElement valueElement = itemElement.addChildElement(name);
//     attName = envelope.createName("xsi:type","","");
//     valueElement.addAttribute( attName, "soapenc:string" );
  }
  catch(xcept::Exception &e){
    XCEPT_RETHROW(emu::daq::manager::exception::Exception,"Failed to create getParameter message to FM", e);
  }
  catch(...){
    XCEPT_RAISE(emu::daq::manager::exception::Exception, "Failed to create getParameter message to FM");
  }

  // Send SOAP message

  cout << endl << "Sending to " << serviceURL << endl; 
  message->writeTo( cout );
  cout.flush();
  cout << endl;

  xoap::MessageReference reply;
  try{
    xoap::MessageReference reply = postSOAP( message, serviceURL, configurationURN );

    cout << endl << "Received reply from " << serviceURL << endl; 
    reply->writeTo( cout );
    cout.flush();
    cout << endl;

    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    
    if(replyBody.hasFault()){ // TODO: find out why hasFault() doesn't work
      std::stringstream ss;
      ss << "Received fault reply from " << serviceURL << " : "
	 << replyBody.getFault().getFaultString();
      cout << endl << ss.str() << endl; cout.flush();
      XCEPT_RAISE(emu::daq::manager::exception::Exception, ss.str());
    }

    parameterValue = parseConfigParameterReplyFromFM( reply );

  }
  catch(xdaq::exception::Exception &e){
    XCEPT_RETHROW(emu::daq::manager::exception::Exception, 
		  "Failed to get parameter " + parameterName 
		  + " of configuration " + configurationURN 
		  + " from " + serviceURL + " : ",
		  e);
  }
  catch(emu::daq::manager::exception::Exception &e){
    XCEPT_RETHROW(emu::daq::manager::exception::Exception, 
		  "Failed to get parameter " + parameterName 
		  + " of configuration " + configurationURN 
		  + " from " + serviceURL + " : ",
		  e);
  }

  cout << parameterName << ": " << parameterValue << endl;
  
  return parameterValue;
}

void emu::daq::manager::Application::getIdsOfRunningConfigurationsFromFM(){

  vector<string> runningConfigs;

  // From CSC Function Manager
  CSCConfigId_ = 0;
  try{
    runningConfigs = getRunningConfigurationsFromFM( CSC_FM_URL_.toString() );
    for ( vector<string>::iterator rc = runningConfigs.begin(); rc != runningConfigs.end(); ++rc ){
      cout << *rc << "  matches \"" << RegexMatchingCSCConfigName_.toString() << "\" ? : " 
	   << toolbox::regx_match( *rc, RegexMatchingCSCConfigName_.toString() )
	   << endl;
      if ( toolbox::regx_match( *rc, RegexMatchingCSCConfigName_.toString() ) ){
	string configState = getConfigParameterFromFM( *rc, "STATE" );
	if ( configState.find("Configur") != string::npos ){
	  CSCConfigId_.fromString( getConfigParameterFromFM( *rc, "SID" ) );
	  LOG4CPLUS_INFO(logger_,"Got CSC config id " << CSCConfigId_.toString()
			 << " from " << *rc << " in state \"" << configState << "\"" );
	  break;
	}
      }
    }
    if ( CSCConfigId_.toString() == "0" ){
      LOG4CPLUS_WARN(logger_, "<![CDATA[ Found in CSC FM no CSC configuration matching \""
		     << RegexMatchingCSCConfigName_.toString()
		     << "\" in state \"Configur*\"."
		     << " ==> CSC configuarion id will be 0. ]]>" );
      stringstream ss46;
      ss46 <<  "<![CDATA[ Found in CSC FM no CSC configuration matching \""
		     << RegexMatchingCSCConfigName_.toString()
		     << "\" in state \"Configur*\"."
		     << " ==> CSC configuarion id will be 0. ]]>" ;
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss46.str() );
      this->notifyQualified( "warn", eObj );
    }
  }
  catch(emu::daq::manager::exception::Exception &e){
    LOG4CPLUS_WARN(logger_,
		   "Failed to get unique id of CSC configuration from CSC FM"
		   << " : " << xcept::stdformat_exception_history(e));
    stringstream ss47;
    ss47 << 
		   "Failed to get unique id of CSC configuration from CSC FM"
		   << " : " ;
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss47.str(), e );
    this->notifyQualified( "warn", eObj );
  }

  // From Track Finder Cell: TODO
  TFConfigId_ = 0;
}


void emu::daq::manager::Application::machineReadableWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    string       monitoringInfo = "";
    unsigned int nbRUs          = ruDescriptors_.size();
    unsigned int nbBUs          = buDescriptors_.size();
    unsigned int i              = 0;


    out->getHTTPResponseHeader().addHeader("Content-Type", "text/plain");

    *out << "Version,1"                                    << endl;
    *out << "nbRUs," << nbRUs                              << endl;
    *out << "nbBUs," << nbBUs                              << endl;
    *out << "classInstance,state,dt,dn,dsumofsquares,dsum" << endl;

    try
    {
        monitoringInfo = getScalarParam(evmDescriptors_[0],
            "monitoringInfo", "string");
    }
    catch(emu::daq::manager::exception::Exception e)
    {
        monitoringInfo = "unreachable";

        LOG4CPLUS_ERROR(logger_,
            "Failed to get monitoringInfo parameter from EVM"
            << " : " << xcept::stdformat_exception_history(e));
        stringstream ss48;
        ss48 << 
            "Failed to get monitoringInfo parameter from EVM"
            << " : " ;
        XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss48.str(), e );
        this->notifyQualified( "error", eObj );
    }

    *out << "EVM0," << monitoringInfo << endl;

    for(i=0; i<nbRUs; i++)
    {
        try
        {
            monitoringInfo = getScalarParam(ruDescriptors_[i],
                "monitoringInfo", "string");
        }
        catch(xcept::Exception e)
        {
            monitoringInfo = "unreachable";

            LOG4CPLUS_ERROR(logger_,
                "Failed to get monitoringInfo parameter from RU" << i
                << " : " << xcept::stdformat_exception_history(e));
            stringstream ss49;
            ss49 << 
                "Failed to get monitoringInfo parameter from RU" << i
                << " : " ;
            XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss49.str(), e );
            this->notifyQualified( "error", eObj );
        }

        *out << "RU" << i << "," << monitoringInfo << endl;
    }

    for(i=0; i<nbBUs; i++)
    {
        try
        {
            monitoringInfo = getScalarParam(buDescriptors_[i],
                "monitoringInfo", "string");
        }
        catch(xcept::Exception e)
        {
            monitoringInfo = "unreachable";

            LOG4CPLUS_ERROR(logger_,
                "Failed to get monitoringInfo parameter from BU" << i
                << " : " << xcept::stdformat_exception_history(e));
            stringstream ss50;
            ss50 << 
                "Failed to get monitoringInfo parameter from BU" << i
                << " : " ;
            XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss50.str(), e );
            this->notifyQualified( "error", eObj );
        }

        *out << "BU" << i << "," << monitoringInfo << endl;
    }
}


void emu::daq::manager::Application::exportParams(xdata::InfoSpace *s)
{

  globalMode_             = false;
  configuredInGlobalMode_ = false;
  s->fireItemAvailable( "globalMode",  &globalMode_  );
  s->fireItemAvailable( "configuredInGlobalMode",  &configuredInGlobalMode_  );

  postToELog_   = true;
  curlHost_     = "cmsusr1.cms";
  curlCommand_  = "curl";
  curlCookies_  = ".curlCookies";
  CMSUserFile_  = "";
  eLogUserFile_ = "";
  eLogURL_      = "";

  s->fireItemAvailable( "postToELog",   &postToELog_   );
  s->fireItemAvailable( "curlHost",     &curlHost_     );
  s->fireItemAvailable( "curlCommand",  &curlCommand_  );
  s->fireItemAvailable( "curlCookies", 	&curlCookies_  );
  s->fireItemAvailable( "CMSUserFile", 	&CMSUserFile_  );
  s->fireItemAvailable( "eLogUserFile",	&eLogUserFile_ );
  s->fireItemAvailable( "eLogURL",     	&eLogURL_      );

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
    buildEvents_       = false;
    s->fireItemAvailable("runNumber",         &runNumber_        );
    s->fireItemAvailable("maxNumberOfEvents", &maxNumberOfEvents_);
    s->fireItemAvailable("runType",           &runType_          );
    s->fireItemAvailable("runTypes",          &runTypes_         );
    s->fireItemAvailable("buildEvents",       &buildEvents_      );

//     s->addItemChangedListener("runNumber",this);


  // Parameters to obtain from TTCciControl
  TTCci_ClockSource_   = "UNKNOWN";
  TTCci_OrbitSource_   = "UNKNOWN";
  TTCci_TriggerSource_ = "UNKNOWN";
  TTCci_BGOSource_     = "UNKNOWN";
  s->fireItemAvailable("TTCci_ClockSource",   &TTCci_ClockSource_);
  s->fireItemAvailable("TTCci_OrbitSource",   &TTCci_OrbitSource_);
  s->fireItemAvailable("TTCci_TriggerSource", &TTCci_TriggerSource_);
  s->fireItemAvailable("TTCci_BGOSource",     &TTCci_BGOSource_);

  // Parameters to obtain from TF_hyperDAQ
  TF_triggerMode_ = "UNKNOWN";
  s->fireItemAvailable("TF_triggerMode", &TF_triggerMode_);

  // FM-related parameters
  TFConfigId_  = 0;
  CSCConfigId_ = 0;
  s->fireItemAvailable("TF_FM_URL"                 , &TF_FM_URL_                  );
  s->fireItemAvailable("CSC_FM_URL"		   , &CSC_FM_URL_                 );
  s->fireItemAvailable("RegexMatchingTFConfigName" , &RegexMatchingTFConfigName_  );
  s->fireItemAvailable("RegexMatchingCSCConfigName", &RegexMatchingCSCConfigName_ );
  s->fireItemAvailable("TFConfigId"		   , &TFConfigId_                 );
  s->fireItemAvailable("CSCConfigId"               , &CSCConfigId_                );
  
    controlDQM_ = false;
    s->fireItemAvailable("controlDQM",&controlDQM_);

    daqState_ = "UNKNOWN";
    s->fireItemAvailable("daqState",&daqState_);
    s->addItemRetrieveListener("daqState",this);

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

  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = ruiDescriptors_.begin(); rui!=ruiDescriptors_.end(); ++rui ){
    stringstream appInst;
    string       appURL;
    string       hwName = "chambers"; 
    string       hwMapURL;
    string       count;
    string       dduError;
    try
      {
	appInst << setfill('0') << setw(2) << (*rui)->getInstance();
	if ( hardwareMnemonics_.find( (*rui)->getInstance() ) != hardwareMnemonics_.end() ){
	  hwName = hardwareMnemonics_[(*rui)->getInstance()];
	}
	hwMapURL = appDescriptor_->getContextDescriptor()->getURL() + "/" +
	  hardwareMapping_.toString() + "#RUI." + appInst.str();
	appURL = getHref( *rui );
	map <string,string> namesAndTypes;
	namesAndTypes["nEventsRead"       ] = "unsignedLong";
	namesAndTypes["persistentDDUError"] = "string";
	map <string,string> namesAndValues = getScalarParams((*rui), namesAndTypes);
	count    = namesAndValues["nEventsRead"       ];
	dduError = namesAndValues["persistentDDUError"];
      }
    catch(xcept::Exception e)
    {
      appURL   = getHref( appDescriptor_ ) + "/control"; // self
      count    = "UNKNOWN";
      dduError = "";
      LOG4CPLUS_WARN(logger_, "Failed to get event count of emu::daq::rui::Application." << appInst.str()
		     << " : " << xcept::stdformat_exception_history(e));
      stringstream ss51;
      ss51 <<  "Failed to get event count of emu::daq::rui::Application." << appInst.str()
		     << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss51.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    map< string,string > sm;
    sm[ "appName"  ] = "emu::daq::rui::Application";
    sm[ "appInst"  ] = appInst.str();
    sm[ "appURL"   ] = appURL;
    sm[ "hwName"   ] = hwName;
    sm[ "hwMapURL" ] = hwMapURL;
    sm[ "count"    ] = count;
    sm[ "dduError" ] = dduError;
    ec.push_back( sm );
  }

  return ec;
}

vector< map< string,string > > emu::daq::manager::Application::getFUEventCounts()
{
  vector< map< string,string > > ec;

  unsigned int totalProcessed = 0;
  vector< xdaq::ApplicationDescriptor* >::iterator fu;
  for ( fu = fuDescriptors_.begin(); fu!=fuDescriptors_.end(); ++fu ){
    stringstream appInst;
    string       appURL;
    string       count;
    unsigned int nProcessed = 0;
    stringstream ss;
    try
    {
      appURL  = getHref( *fu );
      appInst << setfill('0') << setw(2) << (*fu)->getInstance();
      count = getScalarParam( (*fu), "nbEventsProcessed", "unsignedLong" );
      ss << count;
      ss >> nProcessed;
      totalProcessed += nProcessed;
    }
    catch(xcept::Exception e)
    {
      appURL = getHref( appDescriptor_ ) + "/control"; // self
      count = "UNKNOWN";
      LOG4CPLUS_WARN(logger_, "Failed to get event count of emu::daq::fu::Application." << appInst.str()
		     << " : " << xcept::stdformat_exception_history(e));
      stringstream ss52;
      ss52 <<  "Failed to get event count of emu::daq::fu::Application." << appInst.str()
		     << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss52.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    map< string,string > sm;
    sm[ "appName"  ] = "emu::daq::fu::Application";
    sm[ "appInst"  ] = appInst.str();
    sm[ "appURL"   ] = appURL;
    sm[ "count"    ] = count;
    ec.push_back( sm );
  }
  stringstream sst;
  sst << totalProcessed;
  map< string,string > smt;
  smt[ "appInst"  ] = "Total";
  smt[ "appURL"   ] = getHref( appDescriptor_ ) + "/control";;
  smt[ "count"    ] = sst.str();
  ec.push_back( smt );

  return ec;
}


void emu::daq::manager::Application::printEventCountsTable
(
    xgi::Output              *out,
    string                    title,
    vector< map< string,string > >   counts 
) // Emu specific
{
    const int superColWidth = 3; // [columns]
    int nCounts = counts.size();
    int nSuperCols = 6;
    if ( nCounts < nSuperCols ) nSuperCols = nCounts;
    int nRows = nCounts/nSuperCols + (nCounts%nSuperCols?1:0);

    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;

    *out << "<tr>"                                                     << endl;
    *out << "  <th colspan=" << nSuperCols*superColWidth << " align=\"center\">" << endl;
    *out << "    <b>"                                                  << endl;
    *out << title                                                      << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;

    for (int row=0; row<nRows; row++){
      *out << "<tr>"                                                   << endl;
      for (int superCol=0; superCol<nSuperCols; superCol++){
// 	int iCount = superCol * nRows + row;
	int iCount = row * nSuperCols + superCol;

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
	    if ( counts[iCount].find("dduError") != counts[iCount].end() ){ // have element for DDU error
	      if ( counts[iCount]["dduError"].size() > 0 ){ // DDU in error
		string href   = getHref( appDescriptor_ ) + "/control"; // self
		string target = "_self";
		try{
		  href   = getHref( zone_->getApplicationDescriptor("EmuFCrateHyperDAQ",0) );
		  target = "_blank";
		}
		catch(...){
		  href = getHref( appDescriptor_ ) + "/control"; // self
		  target = "_self";
		}
		*out << "      <a href=\"" << href << "\""
		     <<         " title=\"" << counts[iCount]["dduError"] << "\""
		     <<         " style=\"color:#ffffff;"
		     <<                  "background-color:#000000;"
		     <<                  "text-decoration:underline blink\""
		     <<         " target=\"" << target << "\">"
		     <<           counts[iCount]["count"] << "</a>"    << endl;
	      }
	      else{ // DDU OK
		*out << "    " << counts[iCount]["count"]              << endl;
	      }
	    }
	    else{ // no element for DDU error
	      *out << "    " << counts[iCount]["count"]                << endl;
	    }
	  }
	  *out << "  </td>"                                            << endl;
      } // for (int superCol=0; superCol<nSuperCols; superCol++){
      *out << "</tr>"                                                  << endl;
    } // for (int row=0; row<nRows; row++){

    *out << "</table>"                                                 << endl;
}

bool emu::daq::manager::Application::printSTEPCountsTable( stringstream& out, bool control ){
  // Prints STEP counts. 
  // If control is true, prints checkboxes for masking inputs.
  // Returns true if STEP has finished, false otherwise.

  bool isFinished = true;

  // Nothing to do if not in STEP run.
  if ( runType_.toString().find("STEP",0) == string::npos ) return isFinished;

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
  for ( unsigned int i = 0; i < emu::daq::rui::STEPEventCounter::maxDDUInputs_; ++i )
    out <<   "<th>" << i << "</th>";
  out << "</tr>"                                                       << endl;

  xoap::MessageReference reply;

  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");

  xdata::soap::Serializer serializer;

  // Loop over RUIs and query them for STEP info, and write a table row
  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for(rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); rui++){

    // Get STEP info from emu::daq::rui::Application

    xdata::String                      persistentDDUError = "";
    xdata::UnsignedLong                eventsRead  = 0;
    xdata::UnsignedLong                totalCount  = 0;
    xdata::UnsignedLong                lowestCount = 0;
    xdata::Vector<xdata::UnsignedLong> counts;
    xdata::Vector<xdata::Boolean>      masks;
    xdata::Vector<xdata::Boolean>      liveInputs;

    try{
      reply = querySTEP( *rui );
      std::stringstream ss;
      reply->writeTo( ss );
      DOMDocument* doc = parser->parse( ss.str() );

      DOMNode* n;
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("PersistentDDUError") )->item(0);
      serializer.import( &persistentDDUError, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("EventsRead") )->item(0);
      serializer.import( &eventsRead, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("TotalCount") )->item(0);
      serializer.import( &totalCount, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("LowestCount") )->item(0);
      serializer.import( &lowestCount, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("Counts") )->item(0);
      serializer.import( &counts, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("Masks") )->item(0);
      serializer.import( &masks, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("LiveInputs") )->item(0);
      serializer.import( &liveInputs, n );

      isFinished &= ( (int) lowestCount.value_ >= maxNumberOfEvents_.value_ ); 

    } catch( emu::daq::manager::exception::Exception e ){
      LOG4CPLUS_WARN( logger_, "Failed to get STEP info from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss53;
      ss53 <<  "Failed to get STEP info from " 
		      << (*rui)->getClassName() << (*rui)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss53.str(), e );
      this->notifyQualified( "warn", eObj );
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
      this->notifyQualified( "warn", eObj );
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
    for ( unsigned int i = 0; i < counts.elements(); ++i ){
      stringstream DDUInput;
      DDUInput << "EmuRUI." << (*rui)->getInstance() << "." << i;
      out << "  <td align=\"right\"";
      if ( masks.elementAt(i)->toString() == "true" ) 
	out <<     "class=\"masked\"";
      else if ( liveInputs.elementAt(i)->toString() == "true" &&
		(int) ( dynamic_cast<xdata::UnsignedLong*> ( counts.elementAt(i)) )->value_ < maxNumberOfEvents_.value_ )
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
    } // for ( unsigned int i = 0; i < counts.elements(); ++i )

    out << "</tr>"                                                << endl;

  } // for(rui = ruiDescriptors_.begin(); rui != ruiDescriptors_.end(); rui++)

  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  out << "</table>"                                                 << endl;

  return isFinished;
}

xoap::MessageReference emu::daq::manager::Application::querySTEP( xdaq::ApplicationDescriptor* ruiDescriptor )
  throw (emu::daq::manager::exception::Exception){

  xoap::MessageReference reply;

  try{
    
    // Create query message
    xoap::MessageReference message = createSimpleSOAPCmdMsg("STEPQuery");

    // Post it
    reply = appContext_->postSOAP(message, *appDescriptor_, *ruiDescriptor);
    
    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    
    if(replyBody.hasFault())
      {
	stringstream oss;
	oss << "Received fault reply from ";
	oss << ruiDescriptor->getClassName();
	oss << ruiDescriptor->getInstance();
	oss << " : " << replyBody.getFault().getFaultString();
	XCEPT_RAISE(emu::daq::manager::exception::Exception, oss.str());
      }
  } catch(xcept::Exception e){
    stringstream oss;    
    oss << "Failed to query STEP info in ";
    oss << ruiDescriptor->getClassName();
    oss << ruiDescriptor->getInstance();
    XCEPT_RETHROW(emu::daq::manager::exception::Exception, oss.str(), e);
  }

  return reply;
}

bool emu::daq::manager::Application::isSTEPFinished(){
  // Return TRUE if all DDUs' all live and unmasked inputs have produced the requested number of events.
  bool isFinished = true;
  
  xoap::MessageReference reply;

  xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");

  xdata::soap::Serializer serializer;

  // Loop over RUIs and query them for STEP info
  vector< xdaq::ApplicationDescriptor* >::iterator pos;
  for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++){

    xdata::UnsignedLong lowestCount = 0;

    try{
      reply = querySTEP( *pos );
      std::stringstream ss;
      reply->writeTo( ss );
      DOMDocument* doc = parser->parse( ss.str() );
      DOMNodeList* dataNode = doc->getElementsByTagNameNS(xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("LowestCount") );
      DOMNode* n = dataNode->item(0);
      serializer.import( &lowestCount, n );
//       cout << "Lowest count = " << lowestCount.toString() << endl << ss.str() << endl;
    } catch( emu::daq::manager::exception::Exception e ){
      LOG4CPLUS_WARN( logger_, "Failed to get STEP info from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " << xcept::stdformat_exception_history(e));
      stringstream ss55;
      ss55 <<  "Failed to get STEP info from " 
		      << (*pos)->getClassName() << (*pos)->getInstance() 
		      << " : " ;
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss55.str(), e );
      this->notifyQualified( "warn", eObj );
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
      this->notifyQualified( "warn", eObj );
      isFinished = false;
    }

    isFinished &= ( (int) lowestCount.value_ >= maxNumberOfEvents_.value_ ); 
    
  }

  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");

  return isFinished;
}

void emu::daq::manager::Application::maskDDUInputs( const bool in, const std::vector<cgicc::FormEntry>& fev ){

  //
  // Collect the RUI instances and DDU input indices that are to be masked
  //

  std::map< unsigned int, std::set<unsigned int> > instanceToInputs; // an RUI_instance --> [DDU inputs] map

  std::vector<cgicc::FormEntry>::const_iterator fe;
  for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
    std::vector<std::string> matches; // matches[1] will be the instance, matches[2] the DDU input
    if ( toolbox::regx_match( fe->getName(), "^EmuRUI.[0-9]{1,2}.[0-9]{1,2}$") ){ // Make sure there's a match...
      toolbox::regx_match( fe->getName(), "^EmuRUI.([0-9]{1,2}).([0-9]{1,2})$", matches ); // ...because this crashes if no match.
      stringstream instance( matches[1] );
      stringstream input   ( matches[2] );
      unsigned int ins; instance >> ins;
      unsigned int inp; input    >> inp;
      std::map< unsigned int, std::set<unsigned int> >::iterator i = instanceToInputs.find( ins );
      if ( i == instanceToInputs.end() ){
	// First selected input of this instance
	std::set<unsigned int> inputs;
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

  std::map< unsigned int, std::set<unsigned int> >::iterator i;
  for ( i = instanceToInputs.begin(); i!= instanceToInputs.end(); ++i ){
    sendDDUInputMask( in, i->first, i->second );
  }

}

void emu::daq::manager::Application::sendDDUInputMask( const bool                    in, 
				      const unsigned int            ruiInstance, 
				      const std::set<unsigned int>& inputs ){
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

  // Transfer inputs into an xdata vector as that can readily be serialized into SOAP
  xdata::Vector<xdata::UnsignedLong> inputsToMask;
  std::set<unsigned int>::iterator i;
  for( i = inputs.begin(); i != inputs.end(); ++i ){
    inputsToMask.push_back( *i );
  }


  try{

    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPPart soapPart = message->getSOAPPart();
    xoap::SOAPEnvelope envelope = soapPart.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();

    xoap::SOAPName cmdName = envelope.createName(commandName, "xdaq", "urn:xdaq-soap:3.0");
    xoap::SOAPBodyElement bodyElement = body.addBodyElement( cmdName );

    xoap::SOAPName arrayName = envelope.createName("DDUInputs", "xdaq", "urn:xdaq-soap:3.0");
    xoap::SOAPElement array = bodyElement.addChildElement( arrayName );

    xdata::soap::Serializer serializer;
    serializer.exportAll(&inputsToMask, dynamic_cast<DOMElement*>(array.getDOMNode()), true);

//     message->writeTo( std::cout ); std::cout << std:: endl;

    xoap::MessageReference reply = appContext_->postSOAP( message, *appDescriptor_, *ruiDescriptor );
    
//     reply->writeTo( std::cout ); std::cout << std:: endl;

    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    if(replyBody.hasFault()){
      LOG4CPLUS_ERROR(logger_, 
		      "Received fault reply to DDU input mask command from emu::daq::rui::Application instance " 
		      << ruiInstance << ": " << replyBody.getFault().getFaultString() );
      stringstream ss58;
      ss58 <<  
		      "Received fault reply to DDU input mask command from emu::daq::rui::Application instance " 
		      << ruiInstance << ": " << replyBody.getFault().getFaultString() ;
      XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss58.str() );
      this->notifyQualified( "error", eObj );
    }

  } catch( xdaq::exception::Exception e ){
    LOG4CPLUS_ERROR(logger_,
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": " << xcept::stdformat_exception_history(e) );
    stringstream ss59;
    ss59 << 
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": "  ;
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss59.str(), e );
    this->notifyQualified( "error", eObj );
  } catch( xoap::exception::Exception e ){
    LOG4CPLUS_ERROR(logger_,
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": " << xcept::stdformat_exception_history(e) );
    stringstream ss60;
    ss60 << 
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": "  ;
    XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss60.str(), e );
    this->notifyQualified( "error", eObj );
  } catch( ... ){
    LOG4CPLUS_ERROR(logger_,
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": Unknown exception." );
    stringstream ss61;
    ss61 << 
		    "Failed to mask DDU inputs in emu::daq::rui::Application instance " << ruiInstance 
		    << ": Unknown exception." ;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss61.str() );
    this->notifyQualified( "error", eObj );
  }
  
  return;

}

string emu::daq::manager::Application::getDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << " "
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec       << " UTC";

  return ss.str();
}

void emu::daq::manager::Application::getMnemonicNames(){
  // Loop over emu::daq::rui::Applications querying their hardware's mnemonic names

  hardwareMnemonics_.clear();

  vector< xdaq::ApplicationDescriptor* >::iterator pos;
  
  for(pos = ruiDescriptors_.begin(); pos != ruiDescriptors_.end(); pos++)
    {
      stringstream app;
      int instance = (*pos)->getInstance();
      app << (*pos)->getClassName() << instance;
      try
	{
	  hardwareMnemonics_[instance] = getScalarParam(*pos,"hardwareMnemonic","string");
	  LOG4CPLUS_DEBUG(logger_,"Got mnemonic name from " + app.str() + ": " + hardwareMnemonics_[instance] );
	}
      catch(xcept::Exception e)
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
    runInfo_ = 0;
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
//     this->notifyQualified( "warn", eObj );
    
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

void emu::daq::manager::Application::writeRunInfo( bool toDatabase, bool toELog ){
  // Update run info db and post to eLog as well

  // Don't write about debug runs:
  if ( runType_.toString() == "Debug" ||  runType_.toString().find("STEP",0) != string::npos ) return;

  // Don't write about aborted runs either:
  if ( abortedRun_ ) return;

  // If it's not a debug run, it should normally have been booked. Inform the user that it somehow wasn't.
  if ( toDatabase && !isBookedRunNumber_ ){ 
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as no run number was booked.");
    stringstream ss66;
    ss66 <<  "Nothing written to run database as no run number was booked.";
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss66.str() );
    this->notifyQualified( "warn", eObj );
  }

    stringstream subjectToELog;
    subjectToELog << "Emu local run " << runNumber_.value_
		  << " (" << runType_.value_ << ")" << ( badRun_? " is bad" : "" );

    //
    // run number; bad run; global run number
    //
    stringstream htmlMessageToELog;
    htmlMessageToELog << " <b>Emu local run</b><br/><br/>"; // Attention: Body must not start with html tag (elog feature...)
    htmlMessageToELog << "<table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run number</td><td>" << runNumber_.value_ << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">bad run</td><td>" << ( badRun_? "true" : "false" ) << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">global run number</td><td>" << globalRunNumber_ << "</td></tr>";

    bool success = false;
    const string nameSpace = "CMS.CSC";
    string name, value;

    //
    // run type
    //
    name      = "run_type";
    value     = runType_.value_;
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run type</td><td>" << runType_.value_ << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
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

    //
    // start time
    //
    string runStartTime("UNKNOWN");
    try
      {
	runStartTime = getScalarParam(taDescriptors_[0],"runStartTime","string");
	runStartTime = reformatTime( runStartTime );
      }
    catch(xcept::Exception e)
      {
	LOG4CPLUS_ERROR(logger_,"Failed to get time of run start from TA0: " << 
			xcept::stdformat_exception_history(e) );
	stringstream ss68;
	ss68 << "Failed to get time of run start from TA0: ";
	XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss68.str(), e );
	this->notifyQualified( "error", eObj );
      }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">start time</td><td>" << runStartTime << "</td></tr>";
    name      = "start_time";
    value     = runStartTime;
    if ( toDatabase && isBookedRunNumber_ ){
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
	runStopTime = getScalarParam(taDescriptors_[0],"runStopTime","string");
	runStopTime = reformatTime( runStopTime );
      }
    catch(xcept::Exception e)
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
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">stop time</td><td>" << value << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
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
    // comments
    //
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">comments</td><td>" << textToHtml(comments_) << "</td></tr>";


//     //
//     // trigger mode
//     //
//     getTriggerMode();
//     htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">Track Finder</td>";
//     htmlMessageToELog << "<td><table>";
//     htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger mode" << "</td><td align=\"right\">" 
// 		      << TF_triggerMode_.toString() << "</td></tr>";
//     htmlMessageToELog << "</table></td></tr>";
//     name  = "trigger_mode";
//     value = TF_triggerMode_.toString();
//     if ( toDatabase && isBookedRunNumber_ ){
//       success = runInfo_->writeRunInfo( name, value, nameSpace );
//       if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
// 				     nameSpace << ":" << name << " = " << value ); }
//       else          { LOG4CPLUS_ERROR(logger_,
// 				      "Failed to write " << nameSpace << ":" << name << 
// 				      " to run database " << runDbAddress_.toString() <<
// 				      " : " << runInfo_->errorMessage() ); }
//       else          { stringstream ss72;
//       else          { ss72 << 
// 				      "Failed to write " << nameSpace << ":" << name << 
// 				      " to run database " << runDbAddress_.toString() <<
// 				      " : " << runInfo_->errorMessage() ; }
//       else          { XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss72.str() );
//       else          { this->notifyQualified( "error", eObj );
//     }

    //
    // trigger sources
    //
    getTriggerSources();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">TTCci (TF)</td>";
    htmlMessageToELog << "<td><table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "clock source"   << "</td><td align=\"right\">" 
		      << TTCci_ClockSource_.toString()   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "orbit source"   << "</td><td align=\"right\">" 
		      << TTCci_OrbitSource_.toString()   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger source" << "</td><td align=\"right\">" 
		      << TTCci_TriggerSource_.toString() << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "BGO source"     << "</td><td align=\"right\">" 
		      << TTCci_BGOSource_.toString()     << "</td></tr>";
    htmlMessageToELog << "</table></td></tr>";
    name  = "clock_source";
    value = TTCci_ClockSource_.toString();
    if ( toDatabase && isBookedRunNumber_ ){
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
    if ( toDatabase && isBookedRunNumber_ ){
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
    if ( toDatabase && isBookedRunNumber_ ){
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
    if ( toDatabase && isBookedRunNumber_ ){
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
      int nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
      name      = "built_events";
      value     = counts.at(nFUs)["count"]; // the last element is the sum of all FUs' event counts
      htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events built</td><td>" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
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
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events read</td><td><table>";
    counts.clear();
    counts = getRUIEventCounts();
    int nRUIs = counts.size();
    for ( int rui=0; rui<nRUIs; ++rui ){
      name  = "EmuRUI"+counts.at(rui)["appInst"];
      value = counts.at(rui)["count"];
      htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << name << "</td><td align=\"right\">" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
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
    htmlMessageToELog << "</table>";

    htmlMessageToELog << "</td></tr></table>";


    if ( toELog && postToELog_.value_ ){
      vector<string> attachments;
      postToELog( subjectToELog.str(), htmlMessageToELog.str(), &attachments );

      // Just in case submission to e-log failed...
      cout << 
	"\n========================================================================\n" <<
	"If automatic posting to eLog address " << eLogURL_.toString() << 
	" failed, post this manually:\nSubject: " << subjectToELog.str() << 
	"\nBody:\n" << htmlMessageToELog.str() <<
	"\n========================================================================\n";
    }
}


void emu::daq::manager::Application::postToELog( string subject, string body, vector<string> *attachments ){
  // Post to eLog:
  emu::supervisor::ELog *eel;
  try
    {
      eel = new emu::supervisor::ELog(curlHost_.toString(),
				      curlCommand_.toString(),
				      curlCookies_.toString(),
				      CMSUserFile_.toString(),
				      eLogUserFile_.toString(),
				      eLogURL_.toString());
    }
  catch( string e ){
    LOG4CPLUS_ERROR(logger_, e);
    stringstream ss79;
    ss79 <<  e;
    XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss79.str() );
    this->notifyQualified( "error", eObj );
    eel = 0;
  }
  if ( eel ) {
    string attachmentList;
    if ( attachments )
      for ( vector<string>::iterator attm = attachments->begin(); attm != attachments->end(); ++attm )
	attachmentList += *attm + "\n";
    LOG4CPLUS_INFO(logger_, 
		   "Posting to eLog address " << eLogURL_.toString() << 
		   " as user " << eel->eLogUser() << " (" << eel->CMSUser() << ") " <<
		   ":\nSubject: " << subject << 
		   "\nBody:\n" << body <<
		   "\nAttachments:\n" << attachmentList );
    eel->postMessage( subject, body, attachments );
  }
  delete eel;
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

xoap::MessageReference emu::daq::manager::Application::onQueryDAQState(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  xoap::MessageReference      reply = createReply(message);

  xoap::SOAPEnvelope       envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPName            xsiType = envelope.createName("type",
							  "xsi",
							  "http://www.w3.org/1999/XMLSchema-instance");
  xoap::SOAPName       daqStateName = envelope.createName( "daqState" );
  xoap::SOAPBody               body = envelope.getBody();
  xoap::SOAPElement daqStateElement = body.addBodyElement( daqStateName );
  daqStateElement.addAttribute( xsiType, "xsd:string" );

  daqState_ = getDAQState();

  daqStateElement.addTextNode( daqState_.toString() );

  return reply;
}

xoap::MessageReference emu::daq::manager::Application::onQueryRunSummary(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  // Create reply message
  xoap::MessageReference reply = xoap::createMessage();
  xoap::SOAPEnvelope  envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody          body = envelope.getBody();
  xdata::soap::Serializer serializer;

  // Start and end times
  string runNumber("UNKNOWN");
  string maxNumEvents("UNKNOWN");
  string runStartTime("UNKNOWN");
  string runStopTime("UNKNOWN");
  getRunInfoFromTA( &runNumber, &maxNumEvents, &runStartTime, &runStopTime );
  xdata::String start_time = runStartTime; // xdata can readily be serialized into SOAP...
  xdata::String stop_time  = runStopTime;
  xdata::UnsignedLong run_number; run_number.fromString( runNumber );
  xoap::SOAPName name = envelope.createName("start_time", "xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPBodyElement bodyElement = body.addBodyElement( name );
  serializer.exportAll(&start_time, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  name = envelope.createName("stop_time", "xdaq", "urn:xdaq-soap:3.0");
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&stop_time, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  name = envelope.createName("run_number", "xdaq", "urn:xdaq-soap:3.0");
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&run_number, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  
  // FU event count
  xdata::String built_events = "0";
  vector< map< string,string > > counts = getFUEventCounts();
  if ( counts.size() > 0 ){
    int nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
    built_events = counts.at(nFUs)["count"]; // the last element is the sum of all FUs' event counts
  }
  name = envelope.createName("built_events", "xdaq", "urn:xdaq-soap:3.0");
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&built_events, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  
  // RUI event counts and instances
  counts = getRUIEventCounts();
  int nRUIs = counts.size();
  xdata::Vector<xdata::String> rui_counts; // xdata can readily be serialized into SOAP...
  xdata::Vector<xdata::String> rui_instances; // xdata can readily be serialized into SOAP...
  for( int iRUI=0; iRUI<nRUIs; ++iRUI ){
    rui_counts.push_back( counts.at(iRUI)["count"] );
    rui_instances.push_back( counts.at(iRUI)["appInst"] );
  }
  name = envelope.createName("rui_counts", "xdaq", "urn:xdaq-soap:3.0");
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&rui_counts, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  name = envelope.createName("rui_instances", "xdaq", "urn:xdaq-soap:3.0");
  bodyElement = body.addBodyElement( name );
  serializer.exportAll(&rui_instances, dynamic_cast<DOMElement*>(bodyElement.getDOMNode()), true);
  
  return reply;
}

void emu::daq::manager::Application::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{
  // Simulate slow transition
  //::sleep(15);
  // Simulate crash
  //exit(1);

    createAllAppStatesVector();

    // Parameters will be set again in global mode on enable, 
    // but just for the display lets set them now:
    if ( globalMode_.value_ ) setParametersForGlobalMode();
    // Set configuredInGlobalMode_ to true if and when configuration succeeds.
    configuredInGlobalMode_ = false;

    warningsToDisplay_ = "";

    if ( globalMode_.value_ ){
      RAISE_ALARM( emu::daq::manager::alarm::Unsupervised, "unsupervised", "warn", "Local DAQ is in unsupervised mode.", "", &logger_ );
    }
    else {
      REVOKE_ALARM( "unsupervised", NULL );
    }

    try
      {
	configureDAQ();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to configure EmuDAQ", ex);
      }

    if ( controlDQM_.value_ ){
      try
	{
	  controlDQM( "Configure" );
	}
      catch(xcept::Exception ex)
	{
	  XCEPT_RETHROW(toolbox::fsm::exception::Exception,
			"Failed to configure the EmuMonitors of DQM", ex);
	}
    }

    try
      {
	getMnemonicNames();
      }
    catch(xcept::Exception ex)
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

    // Successfully configured in global mode.
    if ( globalMode_.value_ ) configuredInGlobalMode_ = true;


  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void emu::daq::manager::Application::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{

    if ( globalMode_.value_ ){
      setParametersForGlobalMode();
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
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to enable EmuDAQ", ex);
      }

    if ( controlDQM_.value_ ){
      try
	{
	  controlDQM( "Enable" );
	}
      catch(xcept::Exception ex)
	{
	  XCEPT_RETHROW(toolbox::fsm::exception::Exception,
			"Failed to enable the EmuMonitors of DQM", ex);
	}
    }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
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

    try
      {
	stopDAQ();
      }
    catch(xcept::Exception ex)
      {
	stringstream ss;
	ss << "Failed to stop EmuDAQ: " << xcept::stdformat_exception_history(ex);
	XCEPT_RETHROW(toolbox::fsm::exception::Exception, ss.str(), ex);
      }

    try
      {
	// Write to database only if not in global mode.
	writeRunInfo( !globalMode_.value_ , true );
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

    if ( controlDQM_.value_ ){
      try
	{
	  controlDQM( "Halt" );
	}
      catch(xcept::Exception ex)
	{
	  XCEPT_RETHROW(toolbox::fsm::exception::Exception,
			"Failed to halt the EmuMonitors of DQM", ex);
	}
    }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void emu::daq::manager::Application::reConfigureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   

    try
      {
	stopDAQ();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to stop EmuDAQ before reconfiguration", ex);
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

    try
      {
	configureDAQ();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to reconfigure EmuDAQ", ex);
      }

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void emu::daq::manager::Application::noAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  // Inaction...
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() 
		 << " attempted when already " 
		 << fsm_.getStateName(fsm_.getCurrentState()));
  stringstream ss83;
  ss83 << e->type() 
       << " attempted when already " 
       << fsm_.getStateName(fsm_.getCurrentState());
  XCEPT_DECLARE( emu::daq::manager::exception::Exception, eObj, ss83.str() );
  this->notifyQualified( "warn", eObj );
}

void emu::daq::manager::Application::resetAction()
		throw (toolbox::fsm::exception::Exception)
{

    try
      {
	resetDAQ();
      }
    catch(xcept::Exception ex)
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

    if ( controlDQM_.value_ ){
      // DQM's EmuMonitors cannot be reset. Halt them instead.
      try
	{
	  controlDQM( "Halt" );
	}
      catch(xcept::Exception ex)
	{
// 	  XCEPT_RETHROW(toolbox::fsm::exception::Exception,
// 			"Failed to configure the EmuMonitors of DQM", ex);
	  LOG4CPLUS_ERROR(logger_, "Failed to configure the EmuMonitors of DQM" <<
			  xcept::stdformat_exception_history(ex) );
	  stringstream ss85;
	  ss85 <<  "Failed to configure the EmuMonitors of DQM";
	  XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss85.str(), ex );
	  this->notifyQualified( "error", eObj );
	}
    }

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
}

void emu::daq::manager::Application::actionPerformed(xdata::Event & received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  if      ( e.itemName() == "daqState"     && e.type() == "ItemRetrieveEvent" ) daqState_ = getDAQState();
  else if ( e.itemName() == "STEPFinished" && e.type() == "ItemRetrieveEvent" ) STEPFinished_ = isSTEPFinished();

//   LOG4CPLUS_INFO(logger_, 
// 		 "Received an InfoSpace event" <<
// 		 " Event type: " << e.type() <<
// 		 " Event name: " << e.itemName() <<
// 		 " Serializable: " << std::hex << e.item() << std::dec <<
// 		 " Type of serializable: " << e.item()->type() );
}


/**
 * Provides the factory method for the instantiation of emu::daq::manager::Application
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::manager::Application)
