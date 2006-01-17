#include "emu/emuDAQ/emuDAQtester/include/EmuDAQtester.h"
#include "emu/emuDAQ/emuDAQtester/include/EmuDAQtesterV.h"
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

// Alias used to access the "versioning" namespace EmuDAQtester from within
// the class EmuDAQtester
namespace EmuDAQtesterV = EmuDAQtester;


EmuDAQtester::EmuDAQtester(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

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
        "/emu/emuDAQ/emuDAQtester/images/rubtest64x64.gif");

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
                XCEPT_RETHROW(emuDAQtester::exception::Exception,
                    "Failed to join sentinel", e);
            }

            try
            {
                // Listen for incoming exceptions
                sentinel_->setListener(this, this);
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(emuDAQtester::exception::Exception,
                    "Failed to listen to incomming exceptions", e);
            }

            try
            {
                // Handle exceptions of the "I2oException" context
                sentinel_->attachContext("I2oException", this);
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(emuDAQtester::exception::Exception,
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

    testConfigured_ = false;
    testStarted_    = false;

    // Bind web interface
    xgi::bind(this, &EmuDAQtester::css           , "styles.css");
    xgi::bind(this, &EmuDAQtester::defaultWebPage, "Default"   );
    xgi::bind(this, &EmuDAQtester::controlWebPage, "control"   );
    xgi::bind(this, &EmuDAQtester::machineReadableWebPage,
        "MachineReadable");

    exportMonitoringParams(appInfoSpace_);

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string EmuDAQtester::generateLoggerName()
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


void EmuDAQtester::getAllAppDescriptors()
{
    try
    {
        evmDescriptors_ = getAppDescriptors(appGroup_, "EVM");
    }
    catch(emuDAQtester::exception::Exception e)
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
    catch(emuDAQtester::exception::Exception e)
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
    catch(emuDAQtester::exception::Exception e)
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
    catch(emuDAQtester::exception::Exception e)
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
    catch(emuDAQtester::exception::Exception e)
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
    catch(emuDAQtester::exception::Exception e)
    {
        fuDescriptors_.clear();

        // Log only a warning as EmuFUs may not exist
        LOG4CPLUS_WARN(logger_,
            "Failed to get application descriptors for class EmuFU"
            << " : " << xcept::stdformat_exception_history(e));
    }
}


void EmuDAQtester::onException(xcept::Exception &e)
{
    LOG4CPLUS_INFO(logger_, "Received an exception from the sentinel");

    cout << "\n";
    cout << xcept::stdformat_exception_history(e) << "\n";
    cout << flush;
}


vector< xdaq::ApplicationDescriptor* > EmuDAQtester::getAppDescriptors
(
    xdaq::ApplicationGroup *appGroup,
    const string           appClass
)
throw (emuDAQtester::exception::Exception)
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }

        orderedDescriptors.push_back(descriptor);
    }

    return orderedDescriptors;
}


sentinel::Interface *EmuDAQtester::getSentinel
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


void EmuDAQtester::css
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


void EmuDAQtester::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
//  *out << "<meta http-equiv=\"Refresh\" content=\"1\">"              << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version "
        << EmuDAQtesterV::versions << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;
    *out << "<form method=\"get\" action=\"/" << urn_ << "/\">"        << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "src=\"/emu/emuDAQ/emuDAQtester/images/rubtest64x64.gif\"" << endl;
    *out << "     alt=\"EmuDAQtester\""                             << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version "
        << EmuDAQtesterV::versions << endl;
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
    *out << "   src=\"/emu/emuDAQ/emuDAQtester/images/ctrl64x64.gif\"" << endl;
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

    *out << "<hr/>"                                                    << endl;

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


void EmuDAQtester::printAppInstanceLinks
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


void EmuDAQtester::controlWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    processControlForm(in);

    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << EmuDAQtesterV::versions
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
    *out << "src=\"/emu/emuDAQ/emuDAQtester/images/ctrl64x64.gif\""    << endl;
    *out << "     alt=\"Control\""                                     << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version "
        << EmuDAQtesterV::versions << endl;
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
    *out << "\"/emu/emuDAQ/emuDAQtester/images/rubtest64x64.gif\""     << endl;
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

    *out << "<hr/>"                                                    << endl;

    *out << "<input"                                                   << endl;
    *out << " type=\"submit\""                                         << endl;
    *out << " name=\"command\""                                        << endl;

    if(!testConfigured_)
      {
        *out << " value=\"configure\""                                 << endl;
      }
    else if(testStarted_)
      {
        *out << " value=\"stop\""                                      << endl;
      }
    else
      {
        *out << " value=\"start\""                                     << endl;
      }

    *out << "/>"                                                       << endl;

    *out << "</form>"                                                  << endl;
    *out << "</body>"                                                  << endl;

    *out << "</html>"                                                  << endl;
}


void EmuDAQtester::processControlForm(xgi::Input *in)
throw (xgi::exception::Exception)
{
    cgicc::Cgicc         cgi(in);
    cgicc::form_iterator cmdElement = cgi.getElement("command");
    string               cmdName    = "";


    // If their is a command from the html form
    if(cmdElement != cgi.getElements().end())
    {
        cmdName = (*cmdElement).getValue();

        if((cmdName == "configure") && (!testConfigured_))
	  {
            try
            {
                configureTest();
                testConfigured_ = true;
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(xgi::exception::Exception,
                    "Failed to configure test", e);
            }
	  }
        else if((cmdName == "start") && (!testStarted_))
        {
            try
            {
                startTest();
                testStarted_ = true;
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(xgi::exception::Exception,
                    "Failed to start test", e);
            }
        }
        else if((cmdName == "stop") && testStarted_)
        {
            try
            {
                stopTest();
                testStarted_ = false;
                testConfigured_ = false;
            }
            catch(xcept::Exception e)
            {
                XCEPT_RETHROW(xgi::exception::Exception,
                    "Failed to stop test", e);
            }
        }
    }
}


void EmuDAQtester::printParamsTables
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


void EmuDAQtester::printParamsTable
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


string EmuDAQtester::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


vector< pair<string,string> > EmuDAQtester::getEventNbFromEVM
(
    xdaq::ApplicationDescriptor *evmDescriptor
)
throw (emuDAQtester::exception::Exception)
{
    string                        eventNb;
    vector< pair<string,string> > params;

    try
    {
        eventNb = getScalarParam(evmDescriptor, "eventNb", "unsignedLong");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to get eventNb from EVM", e);
    }

    params.push_back(pair<string,string>("eventNb", eventNb));

    return params;
}


vector< vector< pair<string,string> > > EmuDAQtester::getStats
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


vector< pair<string,string> > EmuDAQtester::getStats
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


void EmuDAQtester::configureTest()
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Not enough applications to make a RU builder", e);
    }

    try
    {
        setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to tell EVM whether or not to generate dummy triggers", e);
    }

    try
    {
        setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
         "Failed to tell RUs whether or not to generate dummy super-fragments",
         e);
    }

    try
    {
        setBUsDropEvents(busDropEvents);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to tell BUs whether or not drop events", e);
    }

    // If the TA is present then start it as an imaginary trigger
    if(taDescriptors_.size() > 0)
    {
        try
        {
            configureTrigger();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to configure trigger", e);
        }
    }

//     try
//     {
//         startRuBuilder();
//     }
//     catch(xcept::Exception e)
//     {
//         XCEPT_RETHROW(emuDAQtester::exception::Exception,
//             "Failed to start RU builder", e);
//     }

    // If RUIs are present then start them as an imaginary FED builder
    if(ruiDescriptors_.size() > 0)
    {
        try
        {
            configureFedBuilder();
        }
        catch(xcept::Exception e)
        {
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to configure filter farm", e);
        }
    }
}

void EmuDAQtester::startTest()
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Not enough applications to make a RU builder", e);
    }

    try
    {
        setEVMGenerateDummyTriggers(evmGenerateDummyTriggers);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to tell EVM whether or not to generate dummy triggers", e);
    }

    try
    {
        setRUsGenerateDummySuperFrags(rusGenerateDummySuperFrags);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
         "Failed to tell RUs whether or not to generate dummy super-fragments",
         e);
    }

    try
    {
        setBUsDropEvents(busDropEvents);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to start trigger", e);
        }
    }

    try
    {
        startRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to start filter farm", e);
        }
    }
}


void EmuDAQtester::checkThereIsARuBuilder()
throw (emuDAQtester::exception::Exception)
{
    if(evmDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQtester::exception::Exception, "EVM not found");
    }

    if(buDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQtester::exception::Exception, "No BUs");
    }

    if(ruDescriptors_.size() < 1)
    {
        XCEPT_RAISE(emuDAQtester::exception::Exception, "No RUs");
    }
}


void EmuDAQtester::setEVMGenerateDummyTriggers(const bool value)
throw (emuDAQtester::exception::Exception)
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}


void EmuDAQtester::setRUsGenerateDummySuperFrags(const bool value)
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::setBUsDropEvents(const bool value)
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::configureTrigger()
throw (emuDAQtester::exception::Exception)
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}

void EmuDAQtester::startTrigger()
throw (emuDAQtester::exception::Exception)
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}


void EmuDAQtester::startRuBuilder()
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }


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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::startFedBuilder()
throw (emuDAQtester::exception::Exception)
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

//             XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}

void EmuDAQtester::configureFedBuilder()
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

//             XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
//         }
//     }
}


void EmuDAQtester::configureFilterFarm()
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

//             XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
//         }
//     }
}


void EmuDAQtester::startFilterFarm()
throw (emuDAQtester::exception::Exception)
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

//             XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::stopTest()
throw (emuDAQtester::exception::Exception)
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to stop FED builder", e);
        }
    }

    try
    {
        stopRuBuilder();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
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
            XCEPT_RETHROW(emuDAQtester::exception::Exception,
                "Failed to stop filter farm", e);
        }
    }
}


void EmuDAQtester::stopRuBuilder()
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to halt EVM", e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::stopFedBuilder()
throw (emuDAQtester::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt RUIs //
    ///////////////

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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::stopTrigger()
throw (emuDAQtester::exception::Exception)
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}


void EmuDAQtester::stopFilterFarm()
throw (emuDAQtester::exception::Exception)
{
    vector< xdaq::ApplicationDescriptor* >::const_iterator pos;


    ///////////////
    // Halt RUIs //
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

            XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
        }
    }
}


void EmuDAQtester::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RAISE(emuDAQtester::exception::Exception, s);
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

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}


xoap::MessageReference EmuDAQtester::createSimpleSOAPCmdMsg
(
    const string cmdName
)
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}


string EmuDAQtester::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RAISE(emuDAQtester::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }

    return value;
}


void EmuDAQtester::setScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType,
    const string                 paramValue
)
throw (emuDAQtester::exception::Exception)
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

            XCEPT_RAISE(emuDAQtester::exception::Exception, s);
        }
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to set scalar parameter";

        XCEPT_RETHROW(emuDAQtester::exception::Exception, s, e);
    }
}


xoap::MessageReference EmuDAQtester::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}


xoap::MessageReference EmuDAQtester::createParameterSetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType,
    const string paramValue
)
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Failed to create ParameterSet SOAP message for parameter " +
            paramName + " of type " + paramType + " with value " + paramValue,
            e);
    }
}


string EmuDAQtester::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emuDAQtester::exception::Exception)
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
        XCEPT_RETHROW(emuDAQtester::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuDAQtester::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}


DOMNode *EmuDAQtester::findNode
(
    DOMNodeList *nodeList,
    const string nodeLocalName
)
throw (emuDAQtester::exception::Exception)
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

    XCEPT_RAISE(emuDAQtester::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}


void EmuDAQtester::machineReadableWebPage(xgi::Input *in, xgi::Output *out)
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
    catch(emuDAQtester::exception::Exception e)
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


void EmuDAQtester::exportMonitoringParams(xdata::InfoSpace *s)
{
    stateName_ = "Enabled";

    s->fireItemAvailable("stateName", &stateName_);
}


void EmuDAQtester::printSoapMsgToStdOut(xoap::MessageReference message)
{
    DOMNode *node = message->getEnvelope();
    string msgStr;


    xoap::dumpTree(node, msgStr);

    cout << "*************** MESSAGE START ****************\n";
    cout << msgStr << "\n";
    cout << "*************** MESSAGE FINISH ***************\n";
    cout << flush;
}


/**
 * Provides the factory method for the instantiation of EmuDAQtester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuDAQtester)
