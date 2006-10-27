#include "emu/emuDAQ/emuDAQManager/include/EmuDAQManager.h"
#include "emu/emuDAQ/emuDAQManager/include/EmuDAQManagerV.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPHTMLHeader.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPPlainHeader.h"
#include "xcept/include/xcept/tools.h"
#include "xdaq/include/xdaq/NamespaceURI.h"
#include "xdaq/include/xdaq/exception/ApplicationNotFound.h"
#include "xgi/include/xgi/Method.h"
#include "xgi/include/xgi/Utils.h"
#include "xoap/include/xoap/domutils.h"
#include "xoap/include/xoap/MessageFactory.h"
#include "xoap/include/xoap/MessageReference.h"
#include "xoap/include/xoap/Method.h"
#include "xoap/include/xoap/SOAPBody.h"
#include "xoap/include/xoap/SOAPBodyElement.h"
#include "xoap/include/xoap/SOAPEnvelope.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

// Alias used to access the "versioning" namespace EmuDAQManager from within
// the class EmuDAQManager
namespace EmuDAQManagerV = EmuDAQManager;


EmuDAQManager::EmuDAQManager(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
// xdaq::WebApplication(s),
EmuApplication(s),

logger_(Logger::getInstance(generateLoggerName())),
runInfo_(0)
{
    i2oAddressMap_ = i2o::utils::getAddressMap();
    poolFactory_   = toolbox::mem::getMemoryPoolFactory();
    appInfoSpace_  = getApplicationInfoSpace();
    appDescriptor_ = getApplicationDescriptor();
    appContext_    = getApplicationContext();
    appGroup_      = appContext_->getApplicationGroup();
    xmlClass_      = appDescriptor_->getClassName();
    instance_      = appDescriptor_->getInstance();
    urn_           = appDescriptor_->getURN();

    appDescriptor_->setAttribute("icon",
        "/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif");

    // Note that sentinel_ will be zero if the setinel application is not found
    sentinel_ = getSentinel(appContext_);

    // Listen to I2oException context if sentinel was found
    if(sentinel_ != 0)
    {
        try
        {
            try
            {
                // Join sentinel for receiving notifications
                sentinel_->join(this);
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(emuDAQManager::exception::Exception,
                    "Failed to join sentinel", e);
            }

            try
            {
                // Listen for incoming exceptions
                sentinel_->setListener(this, this);
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(emuDAQManager::exception::Exception,
                    "Failed to listen to incomming exceptions", e);
            }

            try
            {
                // Handle exceptions of the "I2oException" context
                sentinel_->attachContext("I2oException", this);
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(emuDAQManager::exception::Exception,
                    "Failed to attach to \"I2oException\" context", e);
            }
        }
        catch(xcept::Exception e)
        {
            LOG4CPLUS_ERROR(logger_,
                "Failed to listen to I2oException context"
                << " : " << xcept::stdformat_exception_history(e));
        }
    }

    getAllAppDescriptors();
    createAllAppStatesVector();

    // Bind web interface
    xgi::bind(this, &EmuDAQManager::css           , "styles.css");
    xgi::bind(this, &EmuDAQManager::defaultWebPage, "Default"   );
    xgi::bind(this, &EmuDAQManager::controlWebPage, "control"   );
    xgi::bind(this, &EmuDAQManager::commandWebPage, "command"   );
    xgi::bind(this, &EmuDAQManager::commentWebPage, "comment"   );
    xgi::bind(this, &EmuDAQManager::machineReadableWebPage,
        "MachineReadable");

    exportParams(appInfoSpace_);

    comments_        = "";
    globalRunNumber_ = "";
    badRun_          = false;

    // Supervisor-specific stuff:
    xoap::bind(this, &EmuDAQManager::onConfigure,     "Configure",     XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onEnable,        "Enable",        XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onDisable,       "Disable",       XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onHalt,          "Halt",          XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onReset,         "Reset",         XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onQueryDAQState, "QueryDAQState", XDAQ_NS_URI);

    fsm_.addState('H', "Halted",     this, &EmuDAQManager::stateChanged);
    fsm_.addState('C', "Configured", this, &EmuDAQManager::stateChanged);
    fsm_.addState('E', "Enabled",    this, &EmuDAQManager::stateChanged);
    
    fsm_.addStateTransition('H', 'C', "Configure", this, &EmuDAQManager::configureAction);
    fsm_.addStateTransition('C', 'C', "Configure", this, &EmuDAQManager::reConfigureAction);
    fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuDAQManager::enableAction);
//     fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuDAQManager::disableAction);
    fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuDAQManager::noAction);
    fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuDAQManager::haltAction);
    fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuDAQManager::haltAction);
    
    fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuDAQManager::noAction);
    fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuDAQManager::noAction);

    fsm_.setInitialState('H');
    fsm_.reset();

    state_ = fsm_.getStateName(fsm_.getCurrentState());
    
    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string EmuDAQManager::generateLoggerName()
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


void EmuDAQManager::getAllAppDescriptors()
{
    try
    {
        evmDescriptors_ = getAppDescriptors(appGroup_, "EVM");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        evmDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class EVM"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        ruDescriptors_ = getAppDescriptors(appGroup_, "RU");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        ruDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class RU"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        buDescriptors_ = getAppDescriptors(appGroup_, "BU");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        buDescriptors_.clear();

        LOG4CPLUS_ERROR(logger_,
            "Failed to get application descriptors for class BU"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        taDescriptors_ = getAppDescriptors(appGroup_, "EmuTA");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        taDescriptors_.clear();

        // Log only a warning as EmuTA may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuTA"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        ruiDescriptors_ = getAppDescriptors(appGroup_, "EmuRUI");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        ruiDescriptors_.clear();

        // Log only a warning as EmuRUIs may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuRUI"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        fuDescriptors_ = getAppDescriptors(appGroup_, "EmuFU");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        fuDescriptors_.clear();

        // Log only a warning as EmuFUs may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuFU"
            << " : " << xcept::stdformat_exception_history(e));
    }

    try
    {
        dqmMonitorDescriptors_ = getAppDescriptors(appGroup_, "EmuMonitor");
    }
    catch(emuDAQManager::exception::Exception e)
    {
        dqmMonitorDescriptors_.clear();

        // Log only a warning as EmuMonitorss may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuMonitor"
            << " : " << xcept::stdformat_exception_history(e));
    }
}


void EmuDAQManager::onException(xcept::Exception &e)
{
    LOG4CPLUS_INFO(logger_, "Received an exception from the sentinel");

    cout << "\n";
    cout << xcept::stdformat_exception_history(e) << "\n";
    cout << flush;
}


vector< xdaq::ApplicationDescriptor* > EmuDAQManager::getAppDescriptors
(
    xdaq::ApplicationGroup *appGroup,
    const string           appClass
)
throw (emuDAQManager::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* > orderedDescriptors;
    vector< xdaq::ApplicationDescriptor* > descriptors;
//     xdaq::ApplicationDescriptor *descriptor = 0;
    int nbApps = 0;


    try
    {
        descriptors = appGroup->getApplicationDescriptors(appClass);
    }
    catch(xcept::Exception e)
    {
        string s;

        s = "Failed to get application descriptors for class: " + appClass;

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }

    nbApps = descriptors.size();

    // Fill application descriptors in instance order allowing non-contiguous numbering
    while( !descriptors.empty() ){
      // Find app with smallest instance number
      unsigned int minInstance = 99999;
      vector< xdaq::ApplicationDescriptor* >::iterator adOfSmallest;
      vector< xdaq::ApplicationDescriptor* >::iterator ad;
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

//     // Fill application descriptors in instance order
//     for(int i=0; i<nbApps; i++)
//     {
//         try
//         {
//             descriptor = appGroup->getApplicationDescriptor(appClass, i);
//         }
//         catch(xcept::Exception e)
//         {
//             stringstream oss;
//             string s;

//             oss << "Failed to get the application descriptor of ";
//             oss << appClass << i;
//             s = oss.str();

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
//         }

//         orderedDescriptors.push_back(descriptor);
//     }

    return orderedDescriptors;
}


sentinel::Interface *EmuDAQManager::getSentinel
(
    xdaq::ApplicationContext *appContext
)
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


void EmuDAQManager::css
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
    *out << "table.params th a"                                        << endl;
    *out << "{"                                                        << endl;
    *out << "color: white;"                                            << endl;
    *out << "}"                                                        << endl;
}


void EmuDAQManager::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    string daqState = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << "Local DAQ " << daqState                                   << endl;
//     *out << xmlClass_ << instance_                                     << endl;
// 	 << " Version "
//         << EmuDAQManagerV::versions 
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
    *out << "src=\"/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif\"" << endl;
    *out << "     alt=\"EmuDAQManager\""                             << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td>"                                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ 
// 	 << " Version " << EmuDAQManagerV::versions 
	 << "  " << fsm_.getStateName(fsm_.getCurrentState())          << endl;
    *out << "    </b>"                                                 << endl;
    *out << "    <br/>Updated at " <<  getDateTime()                   << endl;
    *out << "    &#8212; <span id=\"ageOfPage\"></span> ago "          << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">" << endl;
    *out << "    <a href=\"/" << urn_ << "/control\" target=\"_top\">" << endl;
    *out << "      <img"                                               << endl;
    *out << "   src=\"/emu/emuDAQ/emuDAQManager/images/ctrl64x64.gif\"" << endl;
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
    *out << "       src=\"/daq/xdaq/hyperdaq/images/HyperDAQ.jpg\""    << endl;
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

    printDAQState( out, daqState );

    // Emu: display event number and max number of events
    string runNumber("UNKNOWN");
    string maxNumEvents("UNKNOWN");
    string configTime("UNKNOWN");
    getRunInfoFromTA( &runNumber, &maxNumEvents, &configTime );

    if ( daqState != "Halted" )
      *out << " (Last configured at " << configTime << ")"             << endl;
    *out << "<br/>"                                                    << endl;
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
    *out << "    " << runNumber                                        << endl;
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
    *out << "    " << maxNumEvents;
    if ( maxNumEvents.find("-",0) != string::npos ) *out << " (unlimited)";
    *out                                                               << endl;
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


    // Emu: display RUIs' and FUs' event counts
    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    printEventCountsTable( out, "Events read by EmuRUI's"    , getRUIEventCounts() );
    *out << "</td>"                                                  << endl;
    *out << "<td width=\"64\">"                                      << endl;
    *out << "</td>"                                                  << endl;
    *out << "<td>"                                                   << endl;
    printEventCountsTable( out, "Events processed by EmuFU's", getFUEventCounts()  );      
    *out << "     "                                                  << endl;
    *out << "</td>"                                                  << endl;
    *out << "</tr>"                                                  << endl;
    *out << "</table>"                                               << endl;
    // Emu end: display RUIs' and FUs' event counts

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    printStatesTable( out, "DAQ applications", daqContexts_, daqAppStates_ );
    *out << "</td>"                                                   << endl;
    *out << "<td width=\"16\"/>"                                      << endl;
    *out << "<td>"                                                   << endl;
    queryAppStates( dqmAppStates_ );
    printStatesTable( out, "DQM applications", dqmContexts_, dqmAppStates_ );
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


void EmuDAQManager::printAppInstanceLinks
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

string EmuDAQManager::ageOfPageClock(){
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

void EmuDAQManager::controlWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  *out << "<html>"                                                   << endl;
  *out << "<head>"                                                   << endl;
  *out << "<title>"                                                  << endl;
  *out << "Emu Local DAQ "                                           << endl;
  *out << "</title>"                                                 << endl;
  *out << "</head>"                                                  << endl;
  *out << "<frameset rows=\"90%, *\">"                               << endl;
  *out << "  <frame src=\"command\"/>"                               << endl;
  *out << "  <frame src=\"comment\"/>"                               << endl;
  *out << "</frameset>"                                              << endl;
  *out << "</html>"                                                  << endl;
}

void EmuDAQManager::printUserComments( xgi::Output *out ){
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

string EmuDAQManager::textToHtml( const string text ){
  // Just replace new line and carriage return with <br/>
  string html = text;
  string::size_type index = 0;
  while ( ( index = html.find_first_of( "\n" ) ) != string::npos )
    html.replace( index, 1, "<br/>" );
  while ( ( index = html.find_first_of( "\r" ) ) != string::npos )
    html.replace( index, 1, "<br/>" );
  return html;
}

void EmuDAQManager::commentWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
  processCommentForm(in);

  *out << "<html>"                                                   << endl;

  *out << "<head>"                                                   << endl;
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

void EmuDAQManager::processCommentForm(xgi::Input *in)
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

void EmuDAQManager::commandWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
    processCommandForm(in);

    string daqState = getDAQState();

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;

    if ( fsm_.getCurrentState() == 'E' ){
     *out << "<meta http-equiv=\"refresh\" content=\"5\">"              << endl;
    }
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << "Emu Local DAQ " << daqState                               << endl;
//     *out << xmlClass_ << instance_ << " Version " << EmuDAQManagerV::versions
//         << " CONTROL" << endl;
    *out << "</title>"                                                 << endl;
    *out << ageOfPageClock();
    *out << "</head>"                                                  << endl;

    *out << "<body onload=\"countSeconds()\">"                         << endl;
    *out << "<form method=\"get\" action=\"/" << urn_ << "/command\">" << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "src=\"/emu/emuDAQ/emuDAQManager/images/ctrl64x64.gif\""    << endl;
    *out << "     alt=\"Control\""                                     << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td>"                                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ 
// 	 << " Version " << EmuDAQManagerV::versions 
	 << "  " << fsm_.getStateName(fsm_.getCurrentState())          << endl;
    *out << "    </b>"                                                 << endl;
    *out << "    <br/>Updated at " <<  getDateTime()                   << endl;
    *out << "    &#8212; <span id=\"ageOfPage\"></span> ago "          << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"70\">" << endl;
    *out << "    <a href=\"/urn:xdaq-application:lid=3\" target=\"_top\">"             << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=\"/daq/xdaq/hyperdaq/images/HyperDAQ.jpg\""    << endl;
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
    *out << "\"/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif\""     << endl;
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

    *out << "<hr/>"                                                    << endl;

    printDAQState( out, daqState );


    // Emu: display event number and max number of events
    string runNumber("UNKNOWN");
    string maxNumEvents("UNKNOWN");
    string configTime("UNKNOWN");
    getRunInfoFromTA( &runNumber, &maxNumEvents, &configTime );

    if ( daqState != "Halted" )
      *out << " (Configured at " << configTime << ")"             << endl;
    *out << "<br/>"                                                    << endl;
    *out << "<br/>"                                                    << endl;

    if ( fsm_.getCurrentState() == 'H' ){
      *out << "Set run number: "                                     << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"text\""                                       << endl;
      *out << " name=\"runnumber\""                                  << endl;
      *out << " title=\"Run number.\""                               << endl;
      *out << " alt=\"run number\""                                  << endl;
      *out << " value=\"" << runNumber << "\""                       << endl;
      *out << " size=\"10\""                                         << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

      *out << "Select run type: "                                    << endl;
      *out << "<select"                                              ;
      *out << " name=\"runtype\""                                    ;
      *out << " size=\"1\""                                          ;
      *out << "/>  "                                                 ;
      for ( int iType=0; iType<runTypes_.elements(); ++iType ){
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
      *out << " value=\"" << maxNumEvents << "\""                    << endl;
      *out << " size=\"10\""                                         << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;

      *out << "Build events: "                                       << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"checkbox\""                                   << endl;
      *out << " name=\"buildevents\""                                << endl;
      *out << " title=\"If checked, events will be built.\""         << endl;
      *out << " alt=\"build events\""                                << endl;
      if ( buildEvents_.value_ ) *out << " checked"                  << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;
    }
    else{ // in a state other than halted
      *out << "<table border=\"0\" rules=\"none\" width=\"100%\">"   << endl;
      *out << "<tr>"                                                 << endl;

      *out << "<td align=\"left\">"                                  << endl;
      *out << "<table border=\"0\" rules=\"none\">"                  << endl;
      *out << "  <tr><td>Run number:</td><td>" << runNumber;
      if ( isBookedRunNumber_ ) *out << " (booked)";
      else *out << " (<span style=\"font-weight: bold; color:#ff0000;\">not</span> booked)";
      *out << "</td></tr>"                                           << endl;
      *out << "  <tr><td>Run type:</td><td>" << runType_.toString();
      *out << "</td></tr>"                                           << endl;
      *out << "  <tr><td>Maximum number of events:</td><td>" << maxNumEvents;
      if ( maxNumEvents.find("-",0) != string::npos ) *out << " (unlimited)";
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


      *out << "<input"                                               << endl;
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
      *out << "/>"                                                   << endl;

      // In case the user has changed his mind, allow him to halt from 'configured' state.
      // Also, if EmuDAQManager is in "Halted" state, but DAQ is not (because EmuDAQManager 
      // was restarted, for example), allow DAQ to be stopped. 
      if ( fsm_.getCurrentState() == 'C' ||
	   ( fsm_.getCurrentState() == 'H' && daqState != "Halted" ) )
	{
	  *out << "<input"                                               << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"stop\""                                      << endl;
	  *out << "/>"                                                   << endl;
	}

      // If DAQ is in "Failed" state, but EmuDAQManager isn't, place a reset button
      // (if EmuDAQManager is in 'failed' state, we should already have one).
      if ( fsm_.getCurrentState() != 'F' && daqState == "Failed" ){
	  *out << "<input"                                               << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"reset\""                                     << endl;
	  *out << "/>"                                                   << endl;
      }
      

      *out << "<input"                                               << endl;
      *out << " type=\"checkbox\""                                   << endl;
      *out << " name=\"controldqm\""                                << endl;
      *out << " title=\"If checked, DQM's state will be changed too.\""  << endl;
      *out << " alt=\"control dqm\""                                << endl;
      if ( controlDQM_.value_ ) *out << " checked"                  << endl;
      *out << "/>  "                                                 << endl;
      *out << " DQM too"                                             << endl;

    *out << "</form>"                                                  << endl;
    *out << "<br>"                                                     << endl;
    *out << "<br>"                                                     << endl;

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    printEventCountsTable( out, "Events read by EmuRUI's"    , getRUIEventCounts() );
    *out << "<td width=\"64\">"                                      << endl;
    *out << "</td>"                                                  << endl;
    *out << "</td>"                                                  << endl;
    *out << "<td>"                                                   << endl;
    printEventCountsTable( out, "Events processed by EmuFU's", getFUEventCounts()  );      
    *out << "     "                                                  << endl;
    *out << "</td>"                                                  << endl;
    *out << "</tr>"                                                  << endl;
    *out << "</table>"                                               << endl;

    *out << "<br/>"                                                    << endl;
    *out << "<br/>"                                                    << endl;

    *out << "<table border=\"0\">"                                   << endl;
    *out << "<tr valign=\"top\">"                                    << endl;
    *out << "<td>"                                                   << endl;
    printStatesTable( out, "DAQ applications", daqContexts_, daqAppStates_ );
    *out << "</td>"                                                   << endl;
    *out << "<td width=\"16\"/>"                                      << endl;
    *out << "<td>"                                                   << endl;
    queryAppStates( dqmAppStates_ );
    printStatesTable( out, "DQM applications", dqmContexts_, dqmAppStates_ );
    *out << "</td>"                                                   << endl;
    *out << "</tr>"                                                   << endl;
    *out << "</table>"                                               << endl;

    *out << "</body>"                                                  << endl;

    *out << "</html>"                                                  << endl;
}



void EmuDAQManager::getRunInfoFromTA( string* runnum, string* maxevents, string* configtime ){
    if ( taDescriptors_.size() ){
      if ( taDescriptors_.size() > 1 ){
	LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.");
      }
      try
	{
	  *runnum = getScalarParam(taDescriptors_[0],"runNumber","unsignedLong");
	}
      catch(xcept::Exception e)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get run number from TA0: " << 
			  xcept::stdformat_exception_history(e) );
	}
      try
	{
	  *maxevents = getScalarParam(taDescriptors_[0],"maxNumTriggers","integer");
	}
      catch(xcept::Exception e)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get maximum number of events from TA0: " << 
			  xcept::stdformat_exception_history(e) );
	}
      try
	{
	  *configtime = getScalarParam(taDescriptors_[0],"runStartTime","string");
	  *configtime = reformatConfigTime( *configtime );
	}
      catch(xcept::Exception e)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get time of configuration from TA0: " << 
			  xcept::stdformat_exception_history(e) );
	}
    }
    else{
      LOG4CPLUS_ERROR(logger_,"No TA found.");
    }
}

void EmuDAQManager::getTriggerSources()
  // Gets the trigger sources from TTCci
{
  TTCci_ClockSource_   = "UNKNOWN";
  TTCci_OrbitSource_   = "UNKNOWN";
  TTCci_TriggerSource_ = "UNKNOWN";
  TTCci_BGOSource_     = "UNKNOWN";

  vector< xdaq::ApplicationDescriptor* > appDescriptors;
  try
    {
      appDescriptors = getAppDescriptors(appGroup_, "TTCciControl");
    }
  catch(xcept::Exception e)
    {
      appDescriptors.clear();
      LOG4CPLUS_ERROR(logger_,
		      "Failed to get application descriptors for class TTCciControl: "
		      << stdformat_exception_history(e));
    }

  if ( appDescriptors.size() >= 1 ){
    if ( appDescriptors.size() > 1 )
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      appDescriptors.size() << 
		      " TTCciControl instances found. Trying first one.");
    TTCci_ClockSource_   = getScalarParam(appDescriptors[0],"ClockSource"  ,"string");
    TTCci_OrbitSource_   = getScalarParam(appDescriptors[0],"OrbitSource"  ,"string");
    TTCci_TriggerSource_ = getScalarParam(appDescriptors[0],"TriggerSource","string");
    TTCci_BGOSource_     = getScalarParam(appDescriptors[0],"BGOSource"    ,"string");
    LOG4CPLUS_INFO(logger_, "Got trigger sources  from TTCciControl:" 
		   << " ClockSource: "   << TTCci_ClockSource_.toString()
		   << " OrbitSource: "   << TTCci_OrbitSource_.toString()
		   << " TriggerSource: " << TTCci_TriggerSource_.toString()
		   << " BGOSource: "     << TTCci_BGOSource_.toString() );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find TTCciControl. ==> Trigger sources are unknown.");
  }
}

void EmuDAQManager::getTriggerMode()
  // Gets the trigger mode from TF_hyperDAQ
{
  TF_triggerMode_ = "UNKNOWN";

  vector< xdaq::ApplicationDescriptor* > appDescriptors;
  try
    {
      appDescriptors = getAppDescriptors(appGroup_, "TF_hyperDAQ");
    }
  catch(xcept::Exception e)
    {
      appDescriptors.clear();
      LOG4CPLUS_ERROR(logger_,
		      "Failed to get application descriptors for class TF_hyperDAQ"
		      << stdformat_exception_history(e));
    }

  if ( appDescriptors.size() >= 1 ){
    if ( appDescriptors.size() > 1 )
      LOG4CPLUS_ERROR(logger_, "The embarassement of riches: " << 
		      appDescriptors.size() << " TF_hyperDAQ instances found. Trying first one.");
    TF_triggerMode_ = getScalarParam(appDescriptors[0],"triggerMode","string");
    LOG4CPLUS_INFO(logger_, "Got trigger mode from TF_hyperDAQ: " 
		   << TF_triggerMode_.toString() );
  }
  else{
    LOG4CPLUS_ERROR(logger_, "Did not find TF_hyperDAQ. ==> Trigger mode is unknown.");
  }
}

int EmuDAQManager::purgeIntNumberString( string* s ){
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

int EmuDAQManager::stringToInt( const string* const s ){
  int i;
  stringstream ss;
  ss << *s;
  ss >> i;
  return i;
}

void EmuDAQManager::processCommandForm(xgi::Input *in)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);

    std::vector<cgicc::FormEntry> fev = cgi.getElements();
    std::vector<cgicc::FormEntry>::iterator fe;

    // Check if DQM needs controlling
    // Apparently the query string does not even include the checkbox element if it's not checked...
    controlDQM_ = false;
    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
      if ( fe->getName() == "controldqm" && fe->getValue() == "on" ){
	controlDQM_ = true;
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
	    // Emu: run type will be queried by EmuRUI's and EmuFU's
	    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
	      if ( fe->getName() == "runtype" ){
		// cgicc::form_iterator runTypeElement = cgi.getElement("runtype");
		if ( fe->isEmpty() ){
		  LOG4CPLUS_ERROR(logger_, "Failed to get run type from http form");
		}
		else{
		  runType_.fromString( fe->getValue() );
		}
	      }
	    // Emu: buildEvents will be queried by EmuRUI's
	    // Apparently the query string does not even include the checkbox element if it's not checked...
	    buildEvents_ = false;
	    for ( fe=fev.begin(); fe!=fev.end(); ++ fe )
	      if ( fe->getName() == "buildevents" && fe->getValue() == "on" ){
		buildEvents_ = true;
		break;
	      }
	    // Emu: set run number in emuTA to the value given by the user on the control page
	    cgicc::form_iterator runNumElement = cgi.getElement("runnumber");
	    cgicc::form_iterator maxEvtElement = cgi.getElement("maxevents");
	    string runNumber     = (*runNumElement).getValue();
	    string maxNumEvents  = (*maxEvtElement).getValue();
	    purgeIntNumberString( &runNumber );
	    purgeIntNumberString( &maxNumEvents );
	    runNumber_.fromString( runNumber );
	    maxNumberOfEvents_.fromString( maxNumEvents );
	    LOG4CPLUS_INFO(logger_, "maxNumEvents: " + maxNumEvents );
	    LOG4CPLUS_INFO(logger_, "maxNumberOfEvents_: " + maxNumberOfEvents_.toString() );

	    fireEvent("Configure");
	  }
	else if ( (cmdName == "start") && fsm_.getCurrentState() == 'C' )
	  {
	    fireEvent("Enable");
	  }
        else if( cmdName == "stop"
		 && ( fsm_.getCurrentState() == 'C' ||
		      fsm_.getCurrentState() == 'E'    )
		 )
	  {
	    fireEvent("Halt");
	  }
        else if( cmdName == "reset" )
	  {
	    resetAction();
	    try{
	      fsm_.reset();
	    }
	    catch( toolbox::fsm::exception::Exception e ){
	      XCEPT_RETHROW(xgi::exception::Exception,
			    "Failed to reset FSM: ", e);
	    }
	    fireEvent("Halt");
	  }
    }
}


void EmuDAQManager::printParamsTables
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


void EmuDAQManager::printParamsTable
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


string EmuDAQManager::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


vector< pair<string,string> > EmuDAQManager::getEventNbFromEVM
(
    xdaq::ApplicationDescriptor *evmDescriptor
)
throw (emuDAQManager::exception::Exception)
{
    string                        eventNb;
    vector< pair<string,string> > params;

    try
    {
        eventNb = getScalarParam(evmDescriptor, "eventNb", "unsignedLong");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to get eventNb from EVM", e);
    }

    params.push_back(pair<string,string>("eventNb", eventNb));

    return params;
}


vector< vector< pair<string,string> > > EmuDAQManager::getStats
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


vector< pair<string,string> > EmuDAQManager::getStats
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
    }
    stats.push_back(pair<string,string>("deltaT", s));

    try
    {
        s = getScalarParam(appDescriptor, "deltaN", "unsignedLong");
        deltaN = atoi(s.c_str());
        retrievedDeltaN = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaN = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaN"
            << " : " << xcept::stdformat_exception_history(e));
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
    }
    stats.push_back(pair<string,string>("deltaSumOfSquares", s));

    try
    {
        s = getScalarParam(appDescriptor, "deltaSumOfSizes", "unsignedLong");
        deltaSumOfSizes = atoi(s.c_str());
        retrievedDeltaSumOfSizes = true;
    }
    catch(xcept::Exception e)
    {
        s = "UNKNOWN";
        retrievedDeltaSumOfSizes = false;

        LOG4CPLUS_ERROR(logger_, "Failed to get deltaSumOfSizes"
            << " : " << xcept::stdformat_exception_history(e));
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

void EmuDAQManager::createAllAppStatesVector(){
  //
  // DAQ
  //
  daqAppStates_.clear();
  daqContexts_.clear();
  vector<xdaq::ApplicationDescriptor*> allApps;
  allApps.insert( allApps.end(), evmDescriptors_.begin(), evmDescriptors_.end() );
  allApps.insert( allApps.end(), buDescriptors_ .begin(), buDescriptors_ .end() );
  allApps.insert( allApps.end(), ruDescriptors_ .begin(), ruDescriptors_ .end() );
  allApps.insert( allApps.end(), taDescriptors_ .begin(), taDescriptors_ .end() );
  allApps.insert( allApps.end(), ruiDescriptors_.begin(), ruiDescriptors_.end() );
  allApps.insert( allApps.end(), fuDescriptors_ .begin(), fuDescriptors_ .end() );
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



void EmuDAQManager::queryAppStates( vector< pair<xdaq::ApplicationDescriptor*, string> > &appStates ){
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
	LOG4CPLUS_ERROR(logger_, "Failed to get state"
			<< " : " << xcept::stdformat_exception_history(e));
      }
    as->second = s;
  }
}

string EmuDAQManager::getDAQState(){
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
      combinedState = s->second;
    }
  }
  return combinedState;
}

void EmuDAQManager::printDAQState( xgi::Output *out, string state ){
  map<string, string> bgcolor;
  bgcolor["Halted" ] = "#ff0000";
  bgcolor["Ready"  ] = "#ffff00";
  bgcolor["Enabled"] = "#00ff00";
  bgcolor["Failed" ] = "#000000";
  bgcolor["UNKNOWN"] = "#888888";

  map<string, string> color;
  color["Halted" ] = "#000000";
  color["Ready"  ] = "#000000";
  color["Enabled"] = "#000000";
  color["Failed" ] = "#ffffff";
  color["UNKNOWN"] = "#ffffff";

  map<string, string> decoration;
  decoration["Halted" ] = "none";
  decoration["Ready"  ] = "none";
  decoration["Enabled"] = "none";
  decoration["Failed" ] = "blink";
  decoration["UNKNOWN"] = "none";

  *out << " DAQ is in <a href=\"#states\">";
  *out << "<span align=\"center\" ";
  *out << "style=\"";
  *out << "background-color:" << bgcolor[state];
  *out << "; color:"          << color[state];
  *out << "; text-decoration:"<< decoration[state];
  *out << "\">";
  *out << " " << state << " ";
  *out << "</span>";
  *out << "</a> state."                                              << endl;
}


void EmuDAQManager::printStatesTable( xgi::Output *out,
				      string title,
				      set<string> &contexts,
				      vector< pair<xdaq::ApplicationDescriptor*, string> > &appStates  )
  throw (xgi::exception::Exception)
{
  map<string, string> bgcolor;
  bgcolor["Halted" ] = "#ff0000";
  bgcolor["Ready"  ] = "#ffff00";
  bgcolor["Enabled"] = "#00ff00";
  bgcolor["Failed" ] = "#000000";
  bgcolor["UNKNOWN"] = "#888888";

  map<string, string> color;
  color["Halted" ] = "#000000";
  color["Ready"  ] = "#000000";
  color["Enabled"] = "#000000";
  color["Failed" ] = "#ffffff";
  color["UNKNOWN"] = "#ffffff";

  map<string, string> decoration;
  decoration["Halted" ] = "none";
  decoration["Ready"  ] = "none";
  decoration["Enabled"] = "none";
  decoration["Failed" ] = "blink";
  decoration["UNKNOWN"] = "none";

  *out << "<a name=\"states\"/>"                                         << endl;
  *out << "<table frame=\"void\" rules=\"rows|cols\" class=\"params\">"  << endl;
  
  *out << "<tr>"                                                         << endl;
  *out << "  <th colspan=2 align=\"center\">"                            << endl;
  *out << title                                                          << endl;
  *out << "  </th>"                                                      << endl;
  *out << "</tr>"                                                         << endl;
  *out << "<tr>"                                                         << endl;
  *out << "  <th colspan=2 align=\"center\">"                            << endl;
  *out << "     Color code: "                                            << endl;
  map<string, string>::iterator col;
  for ( col=color.begin(); col!=color.end(); ++col ){
    *out << "     <span align=\"center\" ";
    *out << "style=\"";
    *out << "background-color:" << bgcolor[col->first];
    *out << "; color:"          << col->second;
    *out << "\">";
    *out << " " << col->first << " ";
    *out << "</span>" << endl;
  }
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
	  if ( s->first->getClassName() == "EmuRUI" &&
	       hardwareMnemonics_.find( s->first->getInstance() ) != hardwareMnemonics_.end() )
	    appName << "[" << hardwareMnemonics_[s->first->getInstance()] << "]";
	  *out << "     <span align=\"center\" ";
	  *out << "style=\"";
	  *out << "background-color:" << bgcolor[s->second];
	  *out << "; color:"          << color[s->second];
	  *out << "; text-decoration:"<< decoration[s->second];
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


void EmuDAQManager::configureDAQ()
  throw (emuDAQManager::exception::Exception)
{
    bool evmGenerateDummyTriggers   = taDescriptors_.size()  == 0;
    bool rusGenerateDummySuperFrags = ruiDescriptors_.size() == 0;
    bool busDropEvents              = fuDescriptors_.size()  == 0;

    comments_        = "";
    globalRunNumber_ = "";
    badRun_          = false;

    try
    {
        checkThereIsARuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Not enough applications to make a RU builder", e);
    }

    try
    {
        setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to tell EVM whether or not to generate dummy triggers", e);
    }

    try
    {
        setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
         "Failed to tell RUs whether or not to generate dummy super-fragments",
         e);
    }

    try
    {
        setBUsDropEvents(busDropEvents);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to tell BUs whether or not drop events", e);
    }

    // If the TA is present then start it as an imaginary trigger
    if ( taDescriptors_.size() ){
      if ( taDescriptors_.size() > 1 ){
	LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.");
      }
      // move to enableAction START
      string runNumber    = runNumber_.toString();
      string maxNumEvents = maxNumberOfEvents_.toString();
      try
	{
	  setScalarParam(taDescriptors_[0],"runNumber","unsignedLong",runNumber);
	  LOG4CPLUS_INFO(logger_,"Set run number to " + runNumber );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			"Failed to set run number to "  + runNumber, e);
	}
      try
	{
	  setScalarParam(taDescriptors_[0],"isBookedRunNumber","boolean",string(isBookedRunNumber_?"true":"false"));
	  LOG4CPLUS_INFO(logger_,string("Set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false.") );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			string("Failed to set isBookedRunNumber to ") + string(isBookedRunNumber_?"true.":"false."), e);
	}
      try
	{
	  setScalarParam(taDescriptors_[0],"maxNumTriggers","integer",maxNumEvents);
	  LOG4CPLUS_INFO(logger_,"Set maximum number of events to " + maxNumEvents );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			"Failed to set maximum number of events to "  + maxNumEvents, e);
	}
      // move to enableAction END
        try
        {
            configureTrigger();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to configure trigger", e);
        }
    }
    else{
      LOG4CPLUS_ERROR(logger_,"No TA found.");
    }

    try
    {
        configureRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to configure RU builder", e);
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
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to configure FED builder", e);
        }
    }

    // FUs
    if(fuDescriptors_.size() > 0)
    {
        try
        {
            configureFilterFarm();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to configure filter farm", e);
        }
    }
}


void EmuDAQManager::startDAQ()
throw (emuDAQManager::exception::Exception)
{
    bool evmGenerateDummyTriggers   = taDescriptors_.size()  == 0;
    bool rusGenerateDummySuperFrags = ruiDescriptors_.size() == 0;
    bool busDropEvents              = fuDescriptors_.size()  == 0;


    try
    {
        checkThereIsARuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Not enough applications to make a RU builder", e);
    }

    try
    {
        setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to tell EVM whether or not to generate dummy triggers", e);
    }

    try
    {
        setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
         "Failed to tell RUs whether or not to generate dummy super-fragments",
         e);
    }

    try
    {
        setBUsDropEvents(busDropEvents);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to tell BUs whether or not drop events", e);
    }

    // If the TA is present then start it as an imaginary trigger
    if(taDescriptors_.size() > 0)
    {
        try
        {
            startTrigger();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to start trigger", e);
        }
    }

    try
    {
        startRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to start RU builder", e);
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
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to start FED builder", e);
        }
    }

    // If FUs are present then start them as an imafinary filter farm
    if(fuDescriptors_.size() > 0)
    {
        try
        {
            startFilterFarm();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to start filter farm", e);
        }
    }
}


void EmuDAQManager::checkThereIsARuBuilder()
throw (emuDAQManager::exception::Exception)
{
    if(evmDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQManager::exception::Exception, "EVM not found");
    }

    if(buDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQManager::exception::Exception, "No BUs");
    }

    if(ruDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQManager::exception::Exception, "No RUs");
    }
}


void EmuDAQManager::setEVMGenerateDummyTriggers(const bool value)
throw (emuDAQManager::exception::Exception)
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

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}


void EmuDAQManager::setRUsGenerateDummySuperFrags(const bool value)
throw (emuDAQManager::exception::Exception)
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::setBUsDropEvents(const bool value)
throw (emuDAQManager::exception::Exception)
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::configureTrigger()
throw (emuDAQManager::exception::Exception)
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

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}

void EmuDAQManager::startTrigger()
throw (emuDAQManager::exception::Exception)
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

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}


void EmuDAQManager::configureRuBuilder()
throw (emuDAQManager::exception::Exception)
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
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}

void EmuDAQManager::startRuBuilder()
throw (emuDAQManager::exception::Exception)
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::startFedBuilder()
throw (emuDAQManager::exception::Exception)
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}

void EmuDAQManager::configureFedBuilder()
throw (emuDAQManager::exception::Exception)
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
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			"Failed to set run type for " + app.str() + " to "  + runType_.toString(), e);
	}
      try
	{
	  setScalarParam(*pos,"passDataOnToRUBuilder","boolean",buildEvents_.toString());
	  LOG4CPLUS_INFO(logger_,"Set event building " + buildEvents_.toString() );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
//         }
//     }
}


void EmuDAQManager::configureFilterFarm()
throw (emuDAQManager::exception::Exception)
{
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
	  setScalarParam(*pos,"runType","string",runType_.toString());
	  LOG4CPLUS_INFO(logger_,"Set run type for " + app.str() + " to " + runType_.toString() );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			"Failed to set run type for " + app.str() + " to "  + runType_.toString(), e);
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
//         }
//     }
}


void EmuDAQManager::startFilterFarm()
throw (emuDAQManager::exception::Exception)
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::stopDAQ()
throw (emuDAQManager::exception::Exception)
{
    // If imaginary FED builder was started
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            stopFedBuilder();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to stop EmuRUIs", e);
        }
    }

    try
    {
        stopRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to stop RU builder", e);
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
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to stop trigger", e);
        }
    }

    // If imaginary filter farm was started
    if(fuDescriptors_.size() > 0)
    {
        try
        {
            stopFilterFarm();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to stop filter farm", e);
        }
    }
}

void EmuDAQManager::controlDQM( const string action )
  throw (emuDAQManager::exception::Exception)
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
	
// 	XCEPT_RETHROW(emuDAQManager::exception::Exception, oss.str(), e);

	// Don't raise exception here. Go on to try to deal with the others.
	LOG4CPLUS_ERROR(logger_, "Failed to " << action << " " 
			<< (*mon)->getClassName() << (*mon)->getInstance() << " "
			<< xcept::stdformat_exception_history(e));
      }
  }
 
}


void EmuDAQManager::resetDAQ()
throw (emuDAQManager::exception::Exception)
{

  // Reset EmuRUIs
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            resetApps( ruiDescriptors_ );
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to reset EmuRUIs", e);
        }
    }

    // Builder cannot be reset. Halt it instead.
    try
    {
        stopRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to stop RU builder", e);
    }

    // EmuTA cannot be reset. (Should it?) Halt it instead.
    if(taDescriptors_.size() > 0)
    {
        try
        {
            stopTrigger();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to stop trigger", e);
        }
    }


  // Reset EmuFUs
    if(fuDescriptors_.size() > 0)
    {
        try
        {
            resetApps( fuDescriptors_ );
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQManager::exception::Exception,
                "Failed to reset EmuFUs", e);
        }
    }
}

void EmuDAQManager::resetApps( vector< xdaq::ApplicationDescriptor* > apps )
  throw (emuDAQManager::exception::Exception){

    for( vector< xdaq::ApplicationDescriptor* >::iterator pos = apps.begin(); pos != apps.end(); pos++)
    {
        try
        {
            sendFSMEventToApp("Reset", *pos);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string       s;

            oss << "Failed to reset ";
            oss << (*pos)->getClassName() << (*pos)->getInstance();
            s = oss.str();

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::stopRuBuilder()
throw (emuDAQManager::exception::Exception)
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
//         XCEPT_RETHROW(emuDAQManager::exception::Exception,
//             "Failed to halt EVM", e);
	// Don't raise exception here. Go on to try to stop the others.
	LOG4CPLUS_ERROR(logger_, "Failed to halt EVM : " << xcept::stdformat_exception_history(e));
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others.
	    LOG4CPLUS_ERROR(logger_, s << " : " << xcept::stdformat_exception_history(e));
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others.
	    LOG4CPLUS_ERROR(logger_, s << " : " << xcept::stdformat_exception_history(e));
        }
    }
}


void EmuDAQManager::stopFedBuilder()
throw (emuDAQManager::exception::Exception)
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

//             XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
	    // Don't raise exception here. Go on to try to stop the others first.
	    LOG4CPLUS_ERROR(logger_, s );

	    oss_all << " | " << s << " : " << xcept::stdformat_exception_history(e);
        }
    }
    if ( oss_all.str() != "" )
      XCEPT_RAISE( emuDAQManager::exception::Exception, oss_all.str() );
}


void EmuDAQManager::stopTrigger()
throw (emuDAQManager::exception::Exception)
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

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}


void EmuDAQManager::stopFilterFarm()
throw (emuDAQManager::exception::Exception)
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

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }
    }
}


void EmuDAQManager::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emuDAQManager::exception::Exception)
{
    try
    {
        xoap::MessageReference msg = createSimpleSOAPCmdMsg(eventName);
        xoap::MessageReference reply =
            appContext_->postSOAP(msg, appDescriptor);

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

            XCEPT_RAISE(emuDAQManager::exception::Exception, s);
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

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}


xoap::MessageReference EmuDAQManager::createSimpleSOAPCmdMsg
(
    const string cmdName
)
throw (emuDAQManager::exception::Exception)
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
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}


string EmuDAQManager::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emuDAQManager::exception::Exception)
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

            XCEPT_RAISE(emuDAQManager::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }

    return value;
}


void EmuDAQManager::setScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType,
    const string                 paramValue
)
throw (emuDAQManager::exception::Exception)
{
    string appClass = appDescriptor->getClassName();


    try
    {
        xoap::MessageReference msg = createParameterSetSOAPMsg(appClass,
                                     paramName, paramType, paramValue);

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

            XCEPT_RAISE(emuDAQManager::exception::Exception, s);
        }
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to set scalar parameter";

        XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
    }
}


xoap::MessageReference EmuDAQManager::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emuDAQManager::exception::Exception)
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
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}


xoap::MessageReference EmuDAQManager::createParameterSetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType,
    const string paramValue
)
throw (emuDAQManager::exception::Exception)
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

        propertyElement.addTextNode(paramValue);

        return message;
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Failed to create ParameterSet SOAP message for parameter " +
            paramName + " of type " + paramType + " with value " + paramValue,
            e);
    }
}


string EmuDAQManager::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emuDAQManager::exception::Exception)
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
        XCEPT_RETHROW(emuDAQManager::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuDAQManager::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}


DOMNode *EmuDAQManager::findNode
(
    DOMNodeList *nodeList,
    const string nodeLocalName
)
throw (emuDAQManager::exception::Exception)
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

    XCEPT_RAISE(emuDAQManager::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


void EmuDAQManager::machineReadableWebPage(xgi::Input *in, xgi::Output *out)
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
    catch(emuDAQManager::exception::Exception e)
    {
        monitoringInfo = "unreachable";

        LOG4CPLUS_ERROR(logger_,
            "Failed to get monitoringInfo parameter from EVM"
            << " : " << xcept::stdformat_exception_history(e));
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
        }

        *out << "BU" << i << "," << monitoringInfo << endl;
    }
}


void EmuDAQManager::exportParams(xdata::InfoSpace *s)
{
    // Emu:
  curlCommand_  = "curl";
  curlCookies_  = ".curlCookies";
  CMSUserFile_  = "";
  eLogUserFile_ = "";
  eLogURL_      = "";

  s->fireItemAvailable( "curlCommand",  &curlCommand_  );
  s->fireItemAvailable( "curlCookies", 	&curlCookies_  );
  s->fireItemAvailable( "CMSUserFile", 	&CMSUserFile_  );
  s->fireItemAvailable( "eLogUserFile",	&eLogUserFile_ );
  s->fireItemAvailable( "eLogURL",     	&eLogURL_      );

  s->fireItemAvailable( "peripheralCrateConfigFiles", &peripheralCrateConfigFiles_ );

  runDbBookingCommand_ = "java -jar runnumberbooker.jar";
  runDbWritingCommand_ = "java -jar runinfowriter.jar";
  runDbAddress_        = "dbc:oracle:thin:@oracms.cern.ch:10121:omds";
  runDbUserFile_       = "";
  s->fireItemAvailable( "runDbBookingCommand", &runDbBookingCommand_ );
  s->fireItemAvailable( "runDbWritingCommand", &runDbWritingCommand_ );
  s->fireItemAvailable( "runDbAddress",        &runDbAddress_        );
  s->fireItemAvailable( "runDbUserFile",       &runDbUserFile_       );


    runNumber_         = 0;
    maxNumberOfEvents_ = 0;
    runType_           = "Monitor";
    buildEvents_       = false;
    s->fireItemAvailable("runNumber",         &runNumber_        );
    s->fireItemAvailable("maxNumberOfEvents", &maxNumberOfEvents_);
    s->fireItemAvailable("runType",           &runType_          );
    s->fireItemAvailable("runTypes",          &runTypes_         );
    s->fireItemAvailable("buildEvents",       &buildEvents_      );


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


    controlDQM_ = true;
    s->fireItemAvailable("controlDQM",&controlDQM_);

    daqState_ = "UNKNOWN";
    s->fireItemAvailable("daqState",&daqState_);
    s->addItemRetrieveListener("daqState",this);
}


void EmuDAQManager::printSoapMsgToStdOut(xoap::MessageReference message)
{
    DOMNode *node = message->getEnvelope();
    string msgStr;


    xoap::dumpTree(node, msgStr);

    cout << "*************** MESSAGE START ****************\n";
    cout << msgStr << "\n";
    cout << "*************** MESSAGE FINISH ***************\n";
    cout << flush;
}


vector< vector<string> > EmuDAQManager::getRUIEventCounts()
  // Emu specific
{
  vector< vector<string> > ec;

  vector< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = ruiDescriptors_.begin(); rui!=ruiDescriptors_.end(); ++rui ){
    string       count;
    stringstream name;
    string       href;
    try
    {
      name << "EmuRUI" << setfill('0') << setw(2) << (*rui)->getInstance();
      if ( hardwareMnemonics_.find( (*rui)->getInstance() ) != hardwareMnemonics_.end() )
	name << "[" << hardwareMnemonics_[(*rui)->getInstance()] << "]";
      count = getScalarParam( (*rui), "nEventsRead", "unsignedLong" );
      href  = getHref( *rui );
    }
    catch(xcept::Exception e)
    {
      href  = getHref( appDescriptor_ ) + "/control"; // self
      count = "UNKNOWN";
      LOG4CPLUS_ERROR(logger_, "Failed to get event count of " << name.str()
		      << " : " << xcept::stdformat_exception_history(e));
    }
    vector<string> sv;
    sv.push_back( href       );
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }

  return ec;
}

vector< vector<string> > EmuDAQManager::getFUEventCounts()
  // Emu specific
{
  vector< vector<string> > ec;

  unsigned int totalProcessed = 0;
  vector< xdaq::ApplicationDescriptor* >::iterator fu;
  for ( fu = fuDescriptors_.begin(); fu!=fuDescriptors_.end(); ++fu ){
    string       count;
    stringstream name;
    string       href;
    unsigned int nProcessed = 0;
    stringstream ss;
    try
    {
      href  = getHref( *fu );
      name << "EmuFU" << setfill('0') << setw(2) << (*fu)->getInstance();
      count = getScalarParam( (*fu), "nbEventsProcessed", "unsignedLong" );
      ss << count;
      ss >> nProcessed;
      totalProcessed += nProcessed;
    }
    catch(xcept::Exception e)
    {
      href  = getHref( appDescriptor_ ) + "/control"; // self
      count = "UNKNOWN";
      LOG4CPLUS_ERROR(logger_, "Failed to get event count of " << name.str()
		      << " : " << xcept::stdformat_exception_history(e));
    }
    vector<string> sv;
    sv.push_back( href       );
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }
  stringstream sst;
  sst << totalProcessed;
  vector<string> svt;
  svt.push_back( getHref( appDescriptor_ ) + "/control" ); // self
  svt.push_back( "Total" );
  svt.push_back( sst.str()  );
  ec.push_back( svt );

  return ec;
}

void EmuDAQManager::printEventCountsTable
(
    xgi::Output              *out,
    string                    title,
    vector< vector<string> >  counts 
) // Emu specific
{
    int nbRows    = counts.size();


    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">"   << endl;

    *out << "<tr>"                                                     << endl;
    *out << "  <th colspan=2 align=\"center\">"                        << endl;
    *out << "    <b>"                                                  << endl;
    *out << title                                                      << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;

    for(int row=0; row<nbRows; row++)
    {
        *out << "<tr>"                                                 << endl;
        *out << "  <td align=\"left\">"                                               << endl;
	*out << "      <a href=\"" <<counts[row][0] << "\" target=\"_blank\">"        << endl;
        *out <<             counts[row][1]                             << endl;
	*out << "      </a>"                                           << endl;
        *out << "  </td>"                                              << endl;
        *out << "  <td align=\"right\">"                                               << endl;
        *out << "    " << counts[row][2]                               << endl;
        *out << "  </td>"                                              << endl;
        *out << "</tr>"                                                << endl;
    }

    *out << "</table>"                                                 << endl;
}

string EmuDAQManager::getDateTime(){
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

void EmuDAQManager::getMnemonicNames(){
  // Loop over EmuRUIs querying their hardware's mnemonic names

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
	}
    }
}

string EmuDAQManager::reformatConfigTime( string configtime ){
  // reformat from YYMMDD_hhmmss_UTC to YYYY-MM-DD hh:mm:ss UTC
  string reformatted("");
  reformatted += "20";
  reformatted += configtime.substr(0,2);
  reformatted += "-";
  reformatted += configtime.substr(2,2);
  reformatted += "-";
  reformatted += configtime.substr(4,2);
  reformatted += " ";
  reformatted += configtime.substr(7,2);
  reformatted += ":";
  reformatted += configtime.substr(9,2);
  reformatted += ":";
  reformatted += configtime.substr(11,2);
  reformatted += " UTC";
  return reformatted;
}

void EmuDAQManager::bookRunNumber(){

  isBookedRunNumber_ = false;

  // Don't book debug runs:
  if ( runType_.toString() == "Debug" ) return;
  
  // Just in case it's left over from the previuos run:
  if ( runInfo_ ) {
    delete runInfo_; 
    runInfo_ = 0;
  }

  try
    {
      runInfo_ = EmuRunInfo::Instance( runDbBookingCommand_.toString(),
				       runDbWritingCommand_.toString(),
				       runDbUserFile_.toString(),
				       runDbAddress_.toString() );
    }
  catch( string e )
    {
      LOG4CPLUS_ERROR(logger_, e);
    }

  if ( runInfo_ ){

    string sequence = "CSC:" + runType_.toString();
    
    LOG4CPLUS_INFO(logger_, "Booking run number with " <<
		   runDbBookingCommand_.toString() << " at " <<
		   runDbAddress_.toString()  << " for " << sequence );
    
    bool success = runInfo_->bookRunNumber( sequence );
    
    if ( success ){
      isBookedRunNumber_ = true;
      runNumber_         = runInfo_->runNumber();
      runSequenceNumber_ = runInfo_->runSequenceNumber();
      LOG4CPLUS_INFO(logger_, "Booked run rumber " << runNumber_.toString() <<
		     " (" << sequence << " " << runSequenceNumber_.toString() << ")");
    }
    else LOG4CPLUS_ERROR(logger_,
			 "Failed to book run number: " 
			 <<  runInfo_->errorMessage()
			 << "| Falling back to run number " << runNumber_.value_ 
			 << " specified by user." );
  } // if ( runInfo_ ){

}

void EmuDAQManager::updateRunInfoDb( bool postToELogToo ){
  // Update run info db and post to eLog as well

  // Don't write about debug runs:
  if ( runType_.toString() == "Debug" ) return;

  // If it's not a debug run, it should normally have been booked. Inform the user that it somehow wasn't.
  if ( !isBookedRunNumber_ ) LOG4CPLUS_WARN(logger_, "Nothing written to run database as no run number was booked.");

    stringstream subjectToELog;
    subjectToELog << "Emu local run " << runNumber_.value_
		  << " (" << runType_.value_ << ")" << ( badRun_? " is bad" : "" );

    stringstream htmlMessageToELog;
    htmlMessageToELog << " <b>Emu local run</b><br/><br/>"; // Attention: Body must not start with html tag (elog feature...)
    htmlMessageToELog << "<table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run number</td><td>" << runNumber_.value_ << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">bad run</td><td>" << ( badRun_? "true" : "false" ) << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">global run number</td><td>" << globalRunNumber_ << "</td></tr>";

//     stringstream textMessageToELog;
//     textMessageToELog << "Run number " << runNumber_.value_ << ", ";
//     textMessageToELog << ( badRun_? "bad" : "good" ) << " run, " ;
//     textMessageToELog << "Global run number " << globalRunNumber_ << ", ";

    bool success = false;
    string name, value, nameSpace;

    nameSpace = "run";
    name      = "type";
    value     = runType_.value_;
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run type</td><td>" << runType_.value_ << "</td></tr>";
//     textMessageToELog << "Run type " << runType_.value_ << ", ";
   if ( isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(logger_,
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() ); }
    }

    string configTime("UNKNOWN");
    try
      {
	configTime = getScalarParam(taDescriptors_[0],"runStartTime","string");
	configTime = reformatConfigTime( configTime );
      }
    catch(xcept::Exception e)
      {
	LOG4CPLUS_ERROR(logger_,"Failed to get time of configuration from TA0: " << 
			xcept::stdformat_exception_history(e) );
      }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">start time</td><td>" << configTime << "</td></tr>";
//     textMessageToELog << "Start time " << configTime << ", ";
    nameSpace = "time";
    name      = "start";
    value     = configTime;
    if ( isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(logger_,
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() ); }
    }
    
    nameSpace = "time";
    name      = "stop";
    value     = getDateTime();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">stop time</td><td>" << value << "</td></tr>";
//     textMessageToELog << "Stop time " << value << ", ";
    if ( isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value ); }
      else          { LOG4CPLUS_ERROR(logger_,
				      "Failed to write " << nameSpace << ":" << name << 
				      " to run database " << runDbAddress_.toString() ); }
    }

    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">comments</td><td>" << textToHtml(comments_) << "</td></tr>";

    getTriggerMode();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">Track Finder</td>";
    htmlMessageToELog << "<td><table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger mode" << "</td><td align=\"right\">" 
		      << TF_triggerMode_.toString() << "</td></tr>";
    htmlMessageToELog << "</table></td></tr>";

    getTriggerSources();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">TTCci</td>";
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

    vector< vector<string> > counts = getFUEventCounts();
    if ( counts.size() > 0 ){
      int nFUs = counts.size()-1; // the last element is the sum of all FUs' event counts
      nameSpace = "events";
      name      = "EmuFU";
      value     = counts.at(nFUs).at(2); // the last element is the sum of all FUs' event counts
      htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events built</td><td>" << value << "</td></tr>";
//       textMessageToELog << value << " events built, ";
      if ( isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value ); }
	else          { LOG4CPLUS_ERROR(logger_,
					"Failed to write " << nameSpace << ":" << name << 
					" to run database " << runDbAddress_.toString() ); }
      }
    }

    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events read</td><td><table>";
    counts.clear();
    counts = getRUIEventCounts();
    int nRUIs = counts.size();
    nameSpace = "events";
    for ( int rui=0; rui<nRUIs; ++rui ){
      name  = counts.at(rui).at(1);
      value = counts.at(rui).at(2);
      htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << name << "</td><td align=\"right\">" << value << "</td></tr>";
//       textMessageToELog << name << " " << value << ", ";
      if ( isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){ LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value ); }
	else          { LOG4CPLUS_ERROR(logger_,
					"Failed to write " << nameSpace << ":" << name << 
					" to run database " << runDbAddress_.toString() ); }
      }
    }
    htmlMessageToELog << "</table>";

    htmlMessageToELog << "</td></tr></table>";

//     textMessageToELog << comments_;

    if ( postToELogToo ){
      vector<string> attachments;
      for ( int i=0; i<peripheralCrateConfigFiles_.elements(); ++i ){
	xdata::String* f = dynamic_cast<xdata::String*>(peripheralCrateConfigFiles_.elementAt(i));
	attachments.push_back( f->toString() );
      }
      postToELog( subjectToELog.str(), htmlMessageToELog.str(), &attachments );
//       postToELog( subjectToELog.str(), textMessageToELog.str(), &attachments );
    }
}

void EmuDAQManager::postToELog( string subject, string body, vector<string> *attachments ){
  // Post to eLog:
  EmuELog *eel;
  try
    {
      eel = new EmuELog(curlCommand_.toString(),
			curlCookies_.toString(),
			CMSUserFile_.toString(),
			eLogUserFile_.toString(),
			eLogURL_.toString());
    }
  catch( string e ){
    LOG4CPLUS_ERROR(logger_, e);
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
xoap::MessageReference EmuDAQManager::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onReset(xoap::MessageReference message)
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

xoap::MessageReference EmuDAQManager::onQueryDAQState(xoap::MessageReference message)
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
//   xoap::SOAPElement daqStateElement = responseNameElement.addChildElement( daqStateName );
  daqStateElement.addAttribute( xsiType, "xsd:string" );

  string daqState = getDAQState();

  daqStateElement.addTextNode( daqState );

  return reply;
}

void EmuDAQManager::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   

    createAllAppStatesVector();

    try
      {
	bookRunNumber();
      }
    catch(...)
      {
	LOG4CPLUS_ERROR(logger_,
			"Failed to book run number. Falling back to run number " 
			<< runNumber_.value_ << " specified by user." );
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
		       "Failed to get mnemonic names from EmuRUIs: " 
		       << xcept::stdformat_exception_history(ex) );
      }

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{

    try
      {
	startDAQ();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to configure EmuDAQ", ex);
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

void EmuDAQManager::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  // Do nothing.

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{

    try
      {
	updateRunInfoDb( true );
      }
    catch(...)
      {
	LOG4CPLUS_ERROR(logger_,
			"Failed to book run number. Falling back to run number " 
			<< runNumber_.value_ << " specified by user." );
      }
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

void EmuDAQManager::reConfigureAction(toolbox::Event::Reference e)
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

void EmuDAQManager::noAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
  // Inaction...
  LOG4CPLUS_WARN(getApplicationLogger(), e->type() 
		 << " attempted when already " 
		 << fsm_.getStateName(fsm_.getCurrentState()));
}

void EmuDAQManager::resetAction()
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
	XCEPT_RETHROW(toolbox::fsm::exception::Exception, ss.str(), ex);
      }

    if ( controlDQM_.value_ ){
      // DQM's EmuMonitors cannot be reset. Halt them instead.
      try
	{
	  controlDQM( "Halt" );
	}
      catch(xcept::Exception ex)
	{
	  XCEPT_RETHROW(toolbox::fsm::exception::Exception,
			"Failed to configure the EmuMonitors of DQM", ex);
	}
    }

}

void EmuDAQManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

void EmuDAQManager::actionPerformed(xdata::Event & received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  if ( e.itemName() == "daqState" ) daqState_ = getDAQState();

//   LOG4CPLUS_INFO(logger_, 
// 		 "Received an InfoSpace event" <<
// 		 " Event type: " << e.type() <<
// 		 " Event name: " << e.itemName() <<
// 		 " Serializable: " << std::hex << e.item() << std::dec <<
// 		 " Type of serializable: " << e.item()->type() );
}


/**
 * Provides the factory method for the instantiation of EmuDAQManager
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuDAQManager)
