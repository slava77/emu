// #include "evb/examples/ta/include/TA.h"
// #include "evb/examples/ta/include/TAV.h"
#include "emu/emuDAQ/emuTA/include/EmuTA.h"
#include "emu/emuDAQ/emuTA/include/EmuTAV.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPHTMLHeader.h"
#include "extern/cgicc/linuxx86/include/cgicc/HTTPPlainHeader.h"
#include "i2o/include/i2o/Method.h"
#include "interface/evb/include/i2oEVBMsgs.h"
#include "interface/shared/include/i2oXFunctionCodes.h"
#include "toolbox/include/toolbox/utils.h"
#include "toolbox/include/toolbox/fsm/FailedEvent.h"
#include "toolbox/include/toolbox/mem/HeapAllocator.h"
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

#include <netinet/in.h>

// Alias used to access the "versioning" namespace EmuTA from within the class EmuTA
namespace EmuTAV = EmuTA;


EmuTA::EmuTA(xdaq::ApplicationStub *s)
throw (xdaq::exception::Exception) :
xdaq::WebApplication(s),

logger_(Logger::getInstance(generateLoggerName())),

bSem_(BSem::FULL)
{
    tid_           = 0;
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
        "/emu/emuDAQ/emuTA/images/EmuTA64x64.gif");

    // Note that rubuilderTesterDescriptor_ will be zero if the
    // RUBuilderTester application is not found
    rubuilderTesterDescriptor_ = getRUBuilderTester(appGroup_);

    // Note that sentinel_ will be zero if the setinel application is not found
    sentinel_ = getSentinel(appContext_);

    i2oExceptionHandler_ =
        toolbox::exception::bind(this, &EmuTA::onI2oException, "onI2oException");

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

    // Bind web interface
    xgi::bind(this, &EmuTA::css           , "styles.css");
    xgi::bind(this, &EmuTA::defaultWebPage, "Default"   );

    LOG4CPLUS_INFO(logger_, "End of constructor");
}


string EmuTA::generateLoggerName()
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


xdaq::ApplicationDescriptor *EmuTA::getRUBuilderTester
(
    xdaq::ApplicationGroup *appGroup
)
{
    xdaq::ApplicationDescriptor *appDescriptor = 0;


    try
    {
        appDescriptor =
//             appGroup->getApplicationDescriptor("EmuDAQtester", 0);
            appGroup->getApplicationDescriptor("EmuDAQManager", 0);
    }
    catch(xcept::Exception e)
    {
        appDescriptor = 0;
    }

    return appDescriptor;
}


sentinel::Interface *EmuTA::getSentinel(xdaq::ApplicationContext *appContext)
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


void EmuTA::defineFsm()
throw (emuTA::exception::Exception)
{
    try
    {
        // Define FSM states
        fsm_.addState('H', "Halted"   , this, &EmuTA::stateChanged);
        fsm_.addState('R', "Ready"    , this, &EmuTA::stateChanged);
        fsm_.addState('E', "Enabled"  , this, &EmuTA::stateChanged);
        fsm_.addState('S', "Suspended", this, &EmuTA::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to define FSM states", e);
    }

    try
    {
        // Explicitly set the name of the Failed state
        fsm_.setStateName('F', "Failed");
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to explicitly set the name of the Failed state", e);
    }

    try
    {
        // Define FSM transitions
        fsm_.addStateTransition('H', 'R', "Configure", this,
            &EmuTA::configureAction);
        fsm_.addStateTransition('R', 'E', "Enable"   , this,
            &EmuTA::enableAction);
        fsm_.addStateTransition('E', 'S', "Suspend"  , this,
            &EmuTA::suspendAction);
        fsm_.addStateTransition('S', 'E', "Resume"   , this,
            &EmuTA::resumeAction);
        fsm_.addStateTransition('H', 'H', "Halt"     , this,
            &EmuTA::haltAction);
        fsm_.addStateTransition('R', 'H', "Halt"     , this,
            &EmuTA::haltAction);
        fsm_.addStateTransition('E', 'H', "Halt"     , this,
            &EmuTA::haltAction);
        fsm_.addStateTransition('S', 'H', "Halt"     , this,
            &EmuTA::haltAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to define FSM transitions", e);
    }

    try
    {
        fsm_.setFailedStateTransitionAction(this, &EmuTA::failAction);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to set action for failed state transition", e);
    }

    try
    {
        fsm_.setFailedStateTransitionChanged(this, &EmuTA::stateChanged);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to set state changed callback for failed state transition",
            e);
    }

    try
    {
        fsm_.setInitialState('H');
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to set state initial state of FSM", e);
    }

    try
    {
        fsm_.reset();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to reset FSM", e);
    }
}


string EmuTA::generateMonitoringInfoSpaceName
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


vector< pair<string, xdata::Serializable*> > EmuTA::initAndGetStdConfigParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    triggerSourceId_ = 0;

    params.push_back(pair<string,xdata::Serializable *>
        ("triggerSourceId", &triggerSourceId_));

    // Emu specific
//     runNumber_ = 0;
    params.push_back(pair<string,xdata::Serializable *>
        ("runNumber", &runNumber_));
    params.push_back(pair<string,xdata::Serializable *>
        ("maxNumTriggers", &maxNumTriggers_));

    return params;
}


vector< pair<string, xdata::Serializable*> > EmuTA::initAndGetStdMonitorParams()
{
    vector< pair<string, xdata::Serializable*> > params;


    stateName_          = "Halted";
    nbCreditsHeld_      = 0;
    eventNumber_        = 1;

    params.push_back(pair<string,xdata::Serializable *>
        ("stateName", &stateName_));
    params.push_back(pair<string,xdata::Serializable *>
        ("nbCreditsHeld", &nbCreditsHeld_));
    params.push_back(pair<string,xdata::Serializable *>
        ("eventNumber", &eventNumber_));

    return params;
}


void EmuTA::putParamsIntoInfoSpace
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


void EmuTA::stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
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

    // If the EmuTA has moved to the Enabled state and there are some held credits
    if((state == 'E') && (nbCreditsHeld_ != (unsigned long)0))
    {
        // Send triggers for the held credits
        sendNTriggers(nbCreditsHeld_);
        nbCreditsHeld_ = 0;  // Credits have been used up
    }
}


void EmuTA::bindFsmSoapCallbacks()
{
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Enable"   , XDAQ_NS_URI);
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Suspend"  , XDAQ_NS_URI);
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Resume"   , XDAQ_NS_URI);
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Halt"     , XDAQ_NS_URI);
    xoap::bind(this, &EmuTA::processSoapFsmCmd, "Fail"     , XDAQ_NS_URI);
}


void EmuTA::bindI2oCallbacks()
{
    i2o::bind
    (
        this,
        &EmuTA::taCreditMsg,
        I2O_TA_CREDIT,
        XDAQ_ORGANIZATION_ID
    );
}


void EmuTA::css
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


void EmuTA::defaultWebPage(xgi::Input *in, xgi::Output *out)
throw (xgi::exception::Exception)
{
    *out << "<html>"                                                   << endl;

    *out << "<head>"                                                   << endl;
    *out << "<link type=\"text/css\" rel=\"stylesheet\"";
    *out << " href=\"/" << urn_ << "/styles.css\"/>"                   << endl;
    *out << "<title>"                                                  << endl;
    *out << xmlClass_ << instance_ << " Version " << EmuTAV::versions     << endl;
    *out << "</title>"                                                 << endl;
    *out << "</head>"                                                  << endl;

    *out << "<body>"                                                   << endl;

    *out << "<table border=\"0\" width=\"100%\">"                      << endl;
    *out << "<tr>"                                                     << endl;
    *out << "  <td align=\"left\">"                                    << endl;
    *out << "    <img"                                                 << endl;
    *out << "     align=\"middle\""                                    << endl;
    *out << "     src=\"/emu/emuDAQ/emuTA/images/EmuTA64x64.gif\""     << endl;
    *out << "     alt=\"Main\""                                        << endl;
    *out << "     width=\"64\""                                        << endl;
    *out << "     height=\"64\""                                       << endl;
    *out << "     border=\"\"/>"                                       << endl;
    *out << "    <b>"                                                  << endl;
    *out << "      " << xmlClass_ << instance_ << " Version " << EmuTAV::versions
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
        *out << "     src=\"/emu/emuDAQ/emuDAQManager/images/EmuDAQManager64x64.gif\"";
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


void EmuTA::printParamsTable
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


string EmuTA::serializableScalarToString(xdata::Serializable *s)
{
    if(s->type() == "unsigned long") return serializableUnsignedLongToString(s);
    if(s->type() == "double"       ) return serializableDoubleToString(s);
    if(s->type() == "string"       ) return serializableStringToString(s);
    if(s->type() == "bool"         ) return serializableBooleanToString(s);

    return "Unsupported type";
}


string EmuTA::serializableUnsignedLongToString(xdata::Serializable *s)
{
    xdata::UnsignedLong *v = dynamic_cast<xdata::UnsignedLong*>(s);

    return v->toString();
}


string EmuTA::serializableDoubleToString(xdata::Serializable *s)
{
    xdata::Double *v = dynamic_cast<xdata::Double*>(s);

    return v->toString();
}


string EmuTA::serializableStringToString(xdata::Serializable *s)
{
    xdata::String *v  = dynamic_cast<xdata::String*>(s);
    string        str = v->toString();


    if(str == "")
    {
        str = "\"\"";
    }

    return str;
}


string EmuTA::serializableBooleanToString(xdata::Serializable *s)
{
    xdata::Boolean *v = dynamic_cast<xdata::Boolean*>(s);

    return v->toString();
}


xoap::MessageReference EmuTA::processSoapFsmCmd(xoap::MessageReference msg)
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


string EmuTA::extractCmdNameFromSoapMsg(xoap::MessageReference msg)
throw (emuTA::exception::Exception)
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

    XCEPT_RAISE(emuTA::exception::Exception, "Command name not found");
}


xoap::MessageReference EmuTA::createFsmResponseMsg
(
    const string cmd,
    const string state
)
throw (emuTA::exception::Exception)
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
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state,  e);
    }
    catch(...)
    {
        XCEPT_RAISE(emuTA::exception::Exception,
            "Failed to create FSM response message for cmd " +
            cmd + " and state " + state);
    }
}


void EmuTA::processFsmCommand(const string cmdName)
throw (emuTA::exception::Exception)
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
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to fire event on finite state machine", e);
    }
    catch(...)
    {
        bSem_.give();
        XCEPT_RAISE(emuTA::exception::Exception,
            "Failed to fire event on finite state machine : Unknown exception");
    }
}


void EmuTA::configureAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    try
    {
        getAppDescriptorsAndTids();
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(toolbox::fsm::exception::Exception,
            "Failed to get application descriptors and tids", e);
    }
    
    LOG4CPLUS_INFO(logger_, toolbox::toString("Configured run number: %d", (unsigned long) runNumber_));
    LOG4CPLUS_INFO(logger_, "End of configureAction");
}


void EmuTA::getAppDescriptorsAndTids()
throw (emuTA::exception::Exception)
{
    try
    {
        tid_ = i2oAddressMap_->getTid(appDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to get the I2O TID of this application", e);
    }

    // Avoid repeated function calls to obtain EVM descriptor and tid
    try
    {
        evmDescriptor_ = appGroup_->getApplicationDescriptor("EVM", 0);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to get the application descriptor of the EVM", e);
    }
    try
    {
        evmTid_ = i2oAddressMap_->getTid(evmDescriptor_);
    }
    catch(xcept::Exception e)
    {
        XCEPT_RETHROW(emuTA::exception::Exception,
            "Failed to get the I2O TID of the EVM", e);
    }
}


void EmuTA::enableAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing
}


void EmuTA::suspendAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing
}


void EmuTA::resumeAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
    // Do nothing
}


void EmuTA::haltAction(toolbox::Event::Reference e)
throw (toolbox::fsm::exception::Exception)
{
  // Emu: Increment run number
  runNumber_++;

    // Reset the dummy event number
    eventNumber_ = 1;

    // Reset the number of credits held
    nbCreditsHeld_ = 0;
}


void EmuTA::failAction(toolbox::Event::Reference event)
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


void EmuTA::sendNTriggers(const unsigned int n)
throw (emuTA::exception::Exception)
{
    toolbox::mem::Reference *bufRef = 0;
    unsigned int            i       = 0;


//     for(i=0; i<n; i++)
    for(i=0; i<n && eventNumber_<maxNumTriggers_+1; i++)
    {
        bufRef = triggerGenerator_.generate
        (
            poolFactory_,            // poolFactory
            triggerPool_,            // pool
            tid_,                    // initiatorAddress
            evmTid_,                 // targetAddress
            triggerSourceId_.value_, // triggerSourceId
            eventNumber_.value_,     // eventNumber
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

            XCEPT_RETHROW(emuTA::exception::Exception, s, e);
        }
        catch(...)
        {
            stringstream oss;
            string s;

            oss << "Failed to send dummy trigger";
            oss << " (eventNumber=" << eventNumber_ << ")";
            oss << " : Unknown exception";
            s = oss.str();

            XCEPT_RAISE(emuTA::exception::Exception, s);
        }

        // Increment the event number taking into account the event number of
        // CMS will be 24-bits (2 to the power of 24 = 16777216)
        eventNumber_ = (eventNumber_ + 1) % 16777216;
    }
//     LOG4CPLUS_INFO(logger_, "Sent " << eventNumber_ << " triggers; holding " << nbCreditsHeld_);

}

void EmuTA::printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader )
  //
  // EMu-specific stuff
  //
{
  if( printMessageHeader )
    {
      std::cout << "EmuTA::printBlock:" << endl;
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


void EmuTA::taCreditMsg(toolbox::mem::Reference *bufRef)
{
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
                "EmuTA in undefined state");
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


string EmuTA::getHref(xdaq::ApplicationDescriptor *appDescriptor)
{
    string href;


    href  = appDescriptor->getContextDescriptor()->getURL();
    href += "/";
    href += appDescriptor->getURN();

    return href;
}


bool EmuTA::onI2oException(xcept::Exception &exception, void *context)
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


emuTA::exception::Exception EmuTA::createI2oExceptionForSentinel
(
    xcept::Exception            &i2oException,
    xdaq::ApplicationDescriptor *notifier,
    xdaq::ApplicationDescriptor *source,
    xdaq::ApplicationDescriptor *destination
)
{
    string errorMsg      = createI2oErrorMsg(source, destination);
    string notifierValue = createValueForSentinelNotifierProperty(notifier);


    emuTA::exception::Exception exception("emuTA::exception::Exception", errorMsg,
        __FILE__, __LINE__, __FUNCTION__, i2oException);

    exception.setProperty("notifier", notifierValue);
    exception.setProperty("qualifiedErrorSchemaURI",
   "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
    exception.setProperty("dateTime", toolbox::getDateTime());
    exception.setProperty("sessionID", "none");
    exception.setProperty("severity", "ERROR");

    return exception;
}


string EmuTA::createI2oErrorMsg
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


string EmuTA::createValueForSentinelNotifierProperty
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
 * Provides the factory method for the instantiation of EmuTA applications.
 */
XDAQ_INSTANTIATOR_IMPL(EmuTA)

