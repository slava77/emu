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

logger_(Logger::getInstance(generateLoggerName()))

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

//     testConfigured_ = false;
//     testStarted_    = false;

    // Bind web interface
    xgi::bind(this, &EmuDAQManager::css           , "styles.css");
    xgi::bind(this, &EmuDAQManager::defaultWebPage, "Default"   );
    xgi::bind(this, &EmuDAQManager::controlWebPage, "control"   );
    xgi::bind(this, &EmuDAQManager::machineReadableWebPage,
        "MachineReadable");

    exportMonitoringParams(appInfoSpace_);

    // Supervisor-specific stuff:
    xoap::bind(this, &EmuDAQManager::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onEnable,    "Enable",    XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onDisable,   "Disable",   XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onHalt,      "Halt",      XDAQ_NS_URI);
    xoap::bind(this, &EmuDAQManager::onQuery,     "Query",     XDAQ_NS_URI);
    
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
    xdaq::ApplicationDescriptor *descriptor = 0;
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

    // Fill application descriptors in instance order
    for(int i=0; i<nbApps; i++)
    {
        try
        {
            descriptor = appGroup->getApplicationDescriptor(appClass, i);
        }
        catch(xcept::Exception e)
        {
            stringstream oss;
            string s;

            oss << "Failed to get the application descriptor of ";
            oss << appClass << i;
            s = oss.str();

            XCEPT_RETHROW(emuDAQManager::exception::Exception, s, e);
        }

        orderedDescriptors.push_back(descriptor);
    }

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
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<meta http-equiv=\"Refresh\" content=\"5\">"              << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version "
        << EmuDAQManagerV::versions << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;
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
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version "
	 << EmuDAQManagerV::versions << "  "
	 << fsm_.getStateName(fsm_.getCurrentState())                  << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\">"                               << endl;
    *out << "    EmuTA ";
    printAppInstanceLinks(out, taDescriptors_);
    *out << "<br/>"                                                    << endl;
    *out << "    EVM ";
    printAppInstanceLinks(out, evmDescriptors_);
    *out << "<br/>"                                                    << endl;
    *out << "    EmuRUI ";
    printAppInstanceLinks(out, ruiDescriptors_);
    *out << "<br/>"                                                    << endl;
    *out << "    RU ";
    printAppInstanceLinks(out, ruDescriptors_);
    *out << "<br/>"                                                    << endl;
    *out << "    BU ";
    printAppInstanceLinks(out, buDescriptors_);
    *out << "<br/>"                                                    << endl;
    *out << "    EmuFU ";
    printAppInstanceLinks(out, fuDescriptors_);
    *out                                                               << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">" << endl;
    *out << "    <a href=\"/" << urn_ << "/control\">"                 << endl;
    *out << "      <img"                                               << endl;
    *out << "   src=\"/emu/emuDAQ/emuDAQManager/images/ctrl64x64.gif\"" << endl;
    *out << "       alt=\"Control\""                                   << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/" << urn_ << "/control\">"                 << endl;
    *out << "      Control"                                            << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td width=\"32\">"                                      << endl;
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
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "<br/>"                                                    << endl;
    *out << " Updated at " <<  getDateTime()                           << endl;
    *out << "<br/>"                                                    << endl;

    *out << "<hr/>"                                                    << endl;

    *out << " DAQ is in <a href=\"#states\">" <<  getDAQState()        << endl;
    *out << "</a> state."                                              << endl;
    *out << "<br/>"                                                    << endl;
    *out << "<br/>"                                                    << endl;


    // Emu: display event number and max number of events
    string runNumber("unknown");
    string maxNumEvents("unknown");
    getRunInfoFromTA( &runNumber, &maxNumEvents );
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
    *out << "</table>"                                               << endl;
    *out << "<td width=\"64\">"                                      << endl;
    *out << "</td>"                                                  << endl;
    *out << "</td>"                                                  << endl;
    *out << "<td>"                                                   << endl;
    *out << "<table frame=\"void\" rules=\"rows\" class=\"params\">" << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <th align=\"center\">"                                  << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      Max number of events"                               << endl;
    *out << "    </b>"                                                 << endl;
    *out << "  </th>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td>"                                                   << endl;
    *out << "    " << maxNumEvents                                     << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                               << endl;
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

    *out << "<br/>"                                                    << endl;
    printStatesTable( out );
    *out << "<br/>"                                                    << endl;

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


void EmuDAQManager::controlWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    processControlForm(in);

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;

    if ( fsm_.getCurrentState() == 'E' ){
     *out << "<meta http-equiv=\"refresh\" content=\"5\">"              << endl;
    }
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << EmuDAQManagerV::versions
        << " CONTROL" << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;
    *out << "<form method=\"get\" action=\"/" << urn_ << "/control\">" << endl;

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
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version "
	 << EmuDAQManagerV::versions << "  "
	 << fsm_.getStateName(fsm_.getCurrentState())                  << endl;
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
    *out << "  <td width=\"32\">"                                      << endl;
    *out << "  </td>"                                                  << endl;
    *out << "  <td class=\"app_links\" align=\"center\" width=\"64\">" << endl;
    *out << "    <a href=\"/" << urn_ << "/\">"                        << endl;
    *out << "      <img"                                               << endl;
    *out << "       src=";
    *out << "\"/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif\""     << endl;
    *out << "       alt=\"Main\""                                      << endl;
    *out << "       width=\"64\""                                      << endl;
    *out << "       height=\"64\""                                     << endl;
    *out << "       border=\"\"/>"                                     << endl;
    *out << "    </a>"                                                 << endl;
    *out << "    <a href=\"/" << urn_ << "/\">"                        << endl;
    *out << "      Main"                                               << endl;
    *out << "    </a>"                                                 << endl;
    *out << "  </td>"                                                  << endl;
    *out << "</tr>"                                                    << endl;
    *out << "</table>"                                                 << endl;

    *out << "<br/>"                                                    << endl;
    *out << " Updated at " <<  getDateTime();
    *out << "<br/>"                                                    << endl;

    *out << "<hr/>"                                                    << endl;

    *out << " DAQ is in <a href=\"#states\">" <<  getDAQState()        << endl;
    *out << "</a> state."                                              << endl;
    *out << "<br/>"                                                    << endl;
    *out << "<br/>"                                                    << endl;

    string runNumber("unknown");
    string maxNumEvents("unknown");
    getRunInfoFromTA( &runNumber, &maxNumEvents );

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

      *out << "Set maximum number of events: "                       << endl;
      *out << "<input"                                               << endl;
      *out << " type=\"text\""                                       << endl;
      *out << " name=\"maxevents\""                                  << endl;
      *out << " title=\"Readout will stop after this many events.\"" << endl;
      *out << " alt=\"maximum number of events\""                    << endl;
      *out << " value=\"" << maxNumEvents << "\""                    << endl;
      *out << " size=\"10\""                                         << endl;
      *out << "/>  "                                                 << endl;
      *out << "<br>"                                                 << endl;
    }
    else{
      *out << "Run " << runNumber                                    << endl;
      *out << "<br>"                                                 << endl;
      *out << "       Maximum number of events: " << maxNumEvents    << endl;
      *out << "<br>"                                                 << endl;
    }


//     if ( fsm_.getCurrentState() != 'F' ){

      *out << "<input"                                                   << endl;
      *out << " type=\"submit\""                                         << endl;
      *out << " name=\"command\""                                        << endl;
      
      if ( fsm_.getCurrentState() == 'H' )
	{
	  *out << " value=\"configure\""                                 << endl;
	}
      
      if ( fsm_.getCurrentState() == 'E' )
	{
	  *out << " value=\"stop\""                                      << endl;
	}
      else
	{
	  *out << " value=\"start\""                                     << endl;
	}
      
      if ( fsm_.getCurrentState() == 'C' )
	{
	  *out << "/>"                                                   << endl;
	  *out << "<input"                                               << endl;
	  *out << " type=\"submit\""                                     << endl;
	  *out << " name=\"command\""                                    << endl;
	  *out << " value=\"stop\""                                      << endl;
	  *out << "/>"                                                   << endl;
	}
//     }
      
    *out << "</form>"                                                  << endl;
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
    printStatesTable( out );

    *out << "</body>"                                                  << endl;

    *out << "</html>"                                                  << endl;
}



void EmuDAQManager::getRunInfoFromTA( string* runnum, string* maxevents ){
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
	  *maxevents = getScalarParam(taDescriptors_[0],"maxNumTriggers","unsignedLong");
	}
      catch(xcept::Exception e)
	{
	  LOG4CPLUS_ERROR(logger_,"Failed to get maximum number of events from TA0: " << 
			  xcept::stdformat_exception_history(e) );
	}
    }
    else{
      LOG4CPLUS_ERROR(logger_,"No TA found.");
    }
}

int EmuDAQManager::purgeIntNumberString( string* s ){
  // Emu: purge string of all non-numeric characters
  int nCharactersErased = 0;
  for ( string::size_type i = s->find_first_not_of("0123456789",0); 
	i != string::npos; 
	i = s->find_first_not_of("0123456789",i) ){
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

void EmuDAQManager::processControlForm(xgi::Input *in)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);
    cgicc::form_iterator cmdElement    = cgi.getElement("command");

    // If there is a command from the html form
    if(cmdElement != cgi.getElements().end())
    {
        string cmdName = (*cmdElement).getValue();

// 	if ( (cmdName == "configure") && fsm_.getCurrentState() == 'H' )
	if ( (cmdName == "configure") )
	  {
	    // Emu: set run number in emuTA to the value given by the user on the control page
	    cgicc::form_iterator runNumElement = cgi.getElement("runnumber");
	    cgicc::form_iterator maxEvtElement = cgi.getElement("maxevents");
	    string runNumber     = (*runNumElement).getValue();
	    string maxNumEvents  = (*maxEvtElement).getValue();
	    purgeIntNumberString( &runNumber );
	    purgeIntNumberString( &maxNumEvents );
	    runNumber_.fromString( runNumber );
	    maxNumberOfEvents_.fromString( maxNumEvents );

	    fireEvent("Configure");
	  }
	else if ( (cmdName == "start") && fsm_.getCurrentState() == 'C' )
	  {
	    fireEvent("Enable");
	  }
        else if( ( cmdName == "stop"                ) && 
		 ( fsm_.getCurrentState() == 'C' ||
		   fsm_.getCurrentState() == 'E'    )    )
	  {
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
  vector<xdaq::ApplicationDescriptor*> allApps;
  allApps.insert( allApps.end(), evmDescriptors_.begin(), evmDescriptors_.end() );
  allApps.insert( allApps.end(), buDescriptors_ .begin(), buDescriptors_ .end() );
  allApps.insert( allApps.end(), ruDescriptors_ .begin(), ruDescriptors_ .end() );
  allApps.insert( allApps.end(), taDescriptors_ .begin(), taDescriptors_ .end() );
  allApps.insert( allApps.end(), ruiDescriptors_.begin(), ruiDescriptors_.end() );
  allApps.insert( allApps.end(), fuDescriptors_ .begin(), fuDescriptors_ .end() );
  vector<xdaq::ApplicationDescriptor*>::iterator a;
  for ( a=allApps.begin(); a!=allApps.end(); ++a ){
    allAppStates_.push_back( make_pair( *a, string("UNKNOWN") ) );
    // Collect different contexts too
    contexts_.insert( (*a)->getContextDescriptor()->getURL() );
  }
}

void EmuDAQManager::queryAllAppStates(){
  vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator as;
  for ( as=allAppStates_.begin(); as!=allAppStates_.end(); ++as ){
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
  queryAllAppStates();

  // Combine states:
  // If one is failed, the combined state will also be failed.
  // Else, if one is unknown, the combined state will also be unknown.
  // Else, if all are known but not the same, the combined state will be indefinite.
  string combinedState("UNKNOWN");
  vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator s;
  // First check if any failed:
  for ( s=allAppStates_.begin(); s!=allAppStates_.end(); ++s )
    if ( s->second == "Failed" ){
      combinedState = s->second;
      return combinedState;
    }
  // If none failed:
  for ( s=allAppStates_.begin(); s!=allAppStates_.end(); ++s ){
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


void EmuDAQManager::printStatesTable( xgi::Output *out )
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
  *out << "  <th>"                                                       << endl;
  *out << "     States' color code: "                                    << endl;
  *out << "  </th>"                                                      << endl;
  *out << "  <th>"                                                       << endl;
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
  for ( c=contexts_.begin(); c!=contexts_.end(); ++c ){
    {

      *out << "<tr>"                                                    << endl;

      *out << "  <th>"                                                  << endl;
      *out << "    <a href=\"" << *c << "\">" << c->substr(7) << "</a>" << endl;
      *out << "  </th>"                                                 << endl;
      
      *out << "  <td>"                                                  << endl;
      vector< pair<xdaq::ApplicationDescriptor*, string> >::iterator s;
      for ( s=allAppStates_.begin(); s!=allAppStates_.end(); ++s )
	if ( *c == s->first->getContextDescriptor()->getURL() )
	{
	  *out << "     <span align=\"center\" ";
	  *out << "style=\"";
	  *out << "background-color:" << bgcolor[s->second];
	  *out << "; color:"          << color[s->second];
	  *out << "; text-decoration:"<< decoration[s->second];
	  *out << "\">";
	  *out << " " << s->first->getClassName() << s->first->getInstance() << " ";
	  *out << "</span>" << endl;
      }
      *out << "  </td>"                                                 << endl;

      *out << "</tr>"                                                   << endl;
    }
  }
  *out << "</table>"                                                    << endl;
}




// void EmuDAQManager::configureTest()
// throw (emuDAQManager::exception::Exception)
// {
//     bool evmGenerateDummyTriggers   = taDescriptors_.size()  == 0;
//     bool rusGenerateDummySuperFrags = ruiDescriptors_.size() == 0;
//     bool busDropEvents              = fuDescriptors_.size()  == 0;


//     try
//     {
//         checkThereIsARuBuilder();
//     }
//     catch(xcept::Exception e)
//     {
//         XCEPT_RETHROW(emuDAQManager::exception::Exception,
//             "Not enough applications to make a RU builder", e);
//     }

//     try
//     {
//         setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
//     }
//     catch(xcept::Exception e)
//     {
//         XCEPT_RETHROW(emuDAQManager::exception::Exception,
//             "Failed to tell EVM whether or not to generate dummy triggers", e);
//     }

//     try
//     {
//         setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
//     }
//     catch(xcept::Exception e)
//     {
//         XCEPT_RETHROW(emuDAQManager::exception::Exception,
//          "Failed to tell RUs whether or not to generate dummy super-fragments",
//          e);
//     }

//     try
//     {
//         setBUsDropEvents(busDropEvents);
//     }
//     catch(xcept::Exception e)
//     {
//         XCEPT_RETHROW(emuDAQManager::exception::Exception,
//             "Failed to tell BUs whether or not drop events", e);
//     }

//     // If the TA is present then start it as an imaginary trigger
//     if(taDescriptors_.size() > 0)
//     {
//         try
//         {
//             configureTrigger();
//         }
//         catch(xcept::Exception e)
//         {
//             XCEPT_RETHROW(emuDAQManager::exception::Exception,
//                 "Failed to configure trigger", e);
//         }
//     }

// //     try
// //     {
// //         startRuBuilder();
// //     }
// //     catch(xcept::Exception e)
// //     {
// //         XCEPT_RETHROW(emuDAQManager::exception::Exception,
// //             "Failed to start RU builder", e);
// //     }

//     // If RUIs are present then start them as an imaginary FED builder
//     if(ruiDescriptors_.size() > 0)
//     {
//         try
//         {
//             configureFedBuilder();
//         }
//         catch(xcept::Exception e)
//         {
//             XCEPT_RETHROW(emuDAQManager::exception::Exception,
//                 "Failed to configure FED builder", e);
//         }
//     }

//     // If FUs are present then start them as an imafinary filter farm
//     if(fuDescriptors_.size() > 0)
//     {
//         try
//         {
//             configureFilterFarm();
//         }
//         catch(xcept::Exception e)
//         {
//             XCEPT_RETHROW(emuDAQManager::exception::Exception,
//                 "Failed to configure filter farm", e);
//         }
//     }
// }

void EmuDAQManager::configureTest()
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
    if ( taDescriptors_.size() ){
      if ( taDescriptors_.size() > 1 ){
	LOG4CPLUS_WARN(logger_,"The embarassment of riches: " << taDescriptors_.size() <<
		       " TA instances found. Will use TA0.");
      }
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
	  setScalarParam(taDescriptors_[0],"maxNumTriggers","unsignedLong",maxNumEvents);
	  LOG4CPLUS_INFO(logger_,"Set maximum number of events to " + maxNumEvents );
	}
      catch(xcept::Exception e)
	{
	  XCEPT_RETHROW(emuDAQManager::exception::Exception,
			"Failed to set maximum number of events to "  + maxNumEvents, e);
	}
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

    // If RUIs are present then start them as an imaginary FED builder
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

    // If FUs are present then start them as an imafinary filter farm
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

void EmuDAQManager::startTest()
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


void EmuDAQManager::stopTest()
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


void EmuDAQManager::exportMonitoringParams(xdata::InfoSpace *s)
{
    stateName_ = "Enabled";

    s->fireItemAvailable("stateName", &stateName_);

    // Emu:
    runNumber_         = 0;
    maxNumberOfEvents_ = 0;
    s->fireItemAvailable("runNumber",         &runNumber_        );
    s->fireItemAvailable("maxNumberOfEvents", &maxNumberOfEvents_);
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
      name << "EmuRUI" << (*rui)->getInstance();
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
      name << "EmuFU" << (*fu)->getInstance();
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
	*out << "      <a href=\"" <<counts[row][0] << "\">"           << endl;
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
  tm = localtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << " "
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec;

  return ss.str();
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

xoap::MessageReference EmuDAQManager::onQuery(xoap::MessageReference message)
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

    try
      {
	configureTest();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to configure EmuDAQ", ex);
      }

  LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{

    try
      {
	startTest();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to configure EmuDAQ", ex);
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
	stopTest();
      }
    catch(xcept::Exception ex)
      {
	stringstream ss;
	ss << "Failed to stop EmuDAQ: " << xcept::stdformat_exception_history(ex);
	XCEPT_RETHROW(toolbox::fsm::exception::Exception, ss.str(), ex);
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::reConfigureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   

    try
      {
	stopTest();
      }
    catch(xcept::Exception ex)
      {
	XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		      "Failed to stop EmuDAQ before reconfiguration", ex);
      }

    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());

    try
      {
	configureTest();
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

void EmuDAQManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}



/**
 * Provides the factory method for the instantiation of EmuDAQManager
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuDAQManager)
