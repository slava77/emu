#include "emu/daq/ta/Application.h"
#include "emu/daq/ta/version.h"
#include "emu/daq/rui/i2oFirstEventNumberMsg.h"
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
    unsigned long               appInstance    = appDescriptor->getInstance();
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
            &emu::daq::ta::Application::haltAction);
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


string emu::daq::ta::Application::generateMonitoringInfoSpaceName
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


vector< pair<string, xdata::Serializable*> > emu::daq::ta::Application::initAndGetStdConfigParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    triggerSourceId_ = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("triggerSourceId", &triggerSourceId_));

    // Emu specific
    runStartTime_ = "YYMMDD_hhmmss_UTC";
    runStopTime_  = "YYMMDD_hhmmss_UTC";
    runNumber_    = 0;
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
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Suspend"  , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Resume"   , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &emu::daq::ta::Application::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);
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
  return xoap::createMessage();
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
        *out << "      " << serializableScalarToString(pos->second)    << endl;
        *out << "    </td>"                                            << endl;

        *out << "  </tr>"                                              << endl;
    }

    *out << "</table>"                                                 << endl;
}


string emu::daq::ta::Application::serializableScalarToString(xdata::Serializable *s)
{
    if(s->type() == "unsigned long") return serializableUnsignedLongToString(s);
    if(s->type() == "int"          ) return serializableIntegerToString(s);
    if(s->type() == "double"       ) return serializableDoubleToString(s);
    if(s->type() == "string"       ) return serializableStringToString(s);
    if(s->type() == "bool"         ) return serializableBooleanToString(s);

    return "Unsupported type";
}


string emu::daq::ta::Application::serializableUnsignedLongToString(xdata::Serializable *s)
{
    xdata::UnsignedLong *v = dynamic_cast<xdata::UnsignedLong*>(s);

    return v->toString();
}

string emu::daq::ta::Application::serializableIntegerToString(xdata::Serializable *s)
{
    xdata::Integer *v = dynamic_cast<xdata::Integer*>(s);

    return v->toString();
}


string emu::daq::ta::Application::serializableDoubleToString(xdata::Serializable *s)
{
    xdata::Double *v = dynamic_cast<xdata::Double*>(s);

    return v->toString();
}


string emu::daq::ta::Application::serializableStringToString(xdata::Serializable *s)
{
    xdata::String *v  = dynamic_cast<xdata::String*>(s);
    string        str = v->toString();


    if(str == "")
    {
        str = "\"\"";
    }

    return str;
}


string emu::daq::ta::Application::serializableBooleanToString(xdata::Serializable *s)
{
    xdata::Boolean *v = dynamic_cast<xdata::Boolean*>(s);

    return v->toString();
}


xoap::MessageReference emu::daq::ta::Application::processSoapFsmCmd(xoap::MessageReference msg)
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


string emu::daq::ta::Application::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emu::daq::ta::exception::Exception)
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

    XCEPT_RAISE(emu::daq::ta::exception::Exception, "Command name not found");
}


xoap::MessageReference emu::daq::ta::Application::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emu::daq::ta::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::ta::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


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
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t ); // Unversal Coordinated Time

  stringstream ss;
  ss << setfill('0') << setw(2) << tm->tm_year%100
     << setfill('0') << setw(2) << tm->tm_mon+1
     << setfill('0') << setw(2) << tm->tm_mday      << "_"
     << setfill('0') << setw(2) << tm->tm_hour
     << setfill('0') << setw(2) << tm->tm_min
     << setfill('0') << setw(2) << tm->tm_sec       << "_UTC";

  return ss.str();
}
xoap::MessageReference 
emu::daq::ta::Application::createSimpleSOAPCmdMsg( const string cmdName )
  throw (emu::daq::ta::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to create simple SOAP command message for cmdName " +
            cmdName, e);
    }
}

void 
emu::daq::ta::Application::sendFSMEventToApp
(
    const string                 eventName,
    xdaq::ApplicationDescriptor* appDescriptor
)
throw (emu::daq::ta::exception::Exception)
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

            XCEPT_RAISE(emu::daq::ta::exception::Exception, s);
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

        XCEPT_RETHROW(emu::daq::ta::exception::Exception, s, e);
    }
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
  catch(emu::daq::ta::exception::Exception e){
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
      catch(emu::daq::ta::exception::Exception e)
	{
	  stringstream oss;
	  oss << "Failed to get state of " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	  XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
	}

      if ( atcpState != "Halted" ) continue;

      // Configure ATCP
      try{
	sendFSMEventToApp("Configure", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to configure " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
      }

      // Enable ATCP
      try{
	sendFSMEventToApp("Enable", *atcpd);
      }
      catch(xcept::Exception e){
	stringstream oss;
	oss << "Failed to enable " << (*atcpd)->getClassName() << (*atcpd)->getInstance();
	XCEPT_RETHROW(emu::daq::ta::exception::Exception, oss.str(), e);
      }

    }

  }
  
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


void emu::daq::ta::Application::sendNTriggers(const unsigned int n)
throw (emu::daq::ta::exception::Exception)
{
    toolbox::mem::Reference *bufRef = 0;
    unsigned int            i       = 0;


    for(i=0; 
	i<n && ( maxNumTriggers_.value_ < 0 || 
		 eventNumber_.value_    < biggestFirstEventNumber_ + (unsigned long)(maxNumTriggers_.value_) ); 
	i++)
    {
      // In the builder the trigger number must match the data event number. As the latter wraps around 2^24,
      // so too must the trigger number:
      unsigned int eventNumber = eventNumber_.value_ % 0x1000000;
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
  //
  // EMu-specific stuff
  //
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
  unsigned long  sizeOfPayload =         bufRef->getDataSize()
    - sizeof(I2O_EVENT_DATA_BLOCK_MESSAGE_FRAME);
  unsigned short * shorts = reinterpret_cast<unsigned short *>(startOfPayload);
  int nshorts = sizeOfPayload / sizeof(unsigned short);
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
            LOG4CPLUS_ERROR(logger_,
                "emu::daq::ta::Application in undefined state");
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process trigger credit message : "
             << stdformat_exception_history(e));
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process trigger credit message : Unknown exception");
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
    int instance;

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
	  }

	  break;
        case 'R': // Ready
        case 'S': // Suspended
            break;
        default:
            LOG4CPLUS_ERROR(logger_,
                "emu::daq::ta::Application in undefined state");
        }
    }
    catch(xcept::Exception e)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process first event number message : "
             << stdformat_exception_history(e));
    }
    catch(...)
    {
        LOG4CPLUS_ERROR(logger_,
            "Failed to process first event number message : Unknown exception");
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
    int nbApps = 0;

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

string emu::daq::ta::Application::getScalarParam
(
    xdaq::ApplicationDescriptor* appDescriptor,
    const string                 paramName,
    const string                 paramType
)
throw (emu::daq::ta::exception::Exception)
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

            XCEPT_RAISE(emu::daq::ta::exception::Exception, s);
        }

        value = extractScalarParameterValueFromSoapMsg(reply, paramName);
    }
    catch(xcept::Exception e)
    {
        string s = "Failed to get scalar parameter from application";

        XCEPT_RETHROW(emu::daq::ta::exception::Exception, s, e);
    }

    return value;
}

string emu::daq::ta::Application::extractScalarParameterValueFromSoapMsg
(
    xoap::MessageReference msg,
    const string           paramName
)
throw (emu::daq::ta::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Parameter " + paramName + " not found", e);
    }
    catch(...)
    {
        XCEPT_RAISE(emu::daq::ta::exception::Exception,
            "Parameter " + paramName + " not found");
    }
}

xoap::MessageReference emu::daq::ta::Application::createParameterGetSOAPMsg
(
    const string appClass,
    const string paramName,
    const string paramType
)
throw (emu::daq::ta::exception::Exception)
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
        XCEPT_RETHROW(emu::daq::ta::exception::Exception,
            "Failed to create ParameterGet SOAP message for parameter " +
            paramName + " of type " + paramType, e);
    }
}

DOMNode *emu::daq::ta::Application::findNode(DOMNodeList *nodeList,
			  const string nodeLocalName)
  throw (emu::daq::ta::exception::Exception)
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

    XCEPT_RAISE(emu::daq::ta::exception::Exception,
        "Failed to find node with local name: " + nodeLocalName);
}

/**
 * Provides the factory method for the instantiation of emu::daq::ta::Application.
 */
XDAQ_INSTANTIATOR_IMPL(emu::daq::ta::Application)

