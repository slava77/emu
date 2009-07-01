/*****************************************************************************\
* $Id: Manager.cc,v 1.12 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#include "emu/fed/Manager.h"

#include <vector>
#include <sstream>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "xdaq2rc/RcmsStateNotifier.h"
#include "emu/base/Alarm.h"
#include "emu/fed/JSONSpiritWriter.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Manager)

emu::fed::Manager::Manager(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::base::Supervised(stub),
emu::fed::Application(stub),
emu::fed::Supervised(stub),
emu::base::WebReporter(stub),
ttsID_(0),
ttsBits_(0)
{

	// Variables that are to be made available to other applications
	xdata::InfoSpace *infoSpace = getApplicationInfoSpace();
	infoSpace->fireItemAvailable("ttsID", &ttsID_);
	infoSpace->fireItemAvailable("ttsBits", &ttsBits_);

	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Manager::webDefault, "Default");
	xgi::bind(this, &emu::fed::Manager::webGetSystemStatus, "GetSystemStatus");
	xgi::bind(this, &emu::fed::Manager::webGetCommunicatorStatus, "GetCommunicatorStatus");

	// SOAP call-back functions which fire the transitions to the FSM
	BIND_DEFAULT_SOAP2FSM_ACTION(Manager, Configure);
	BIND_DEFAULT_SOAP2FSM_ACTION(Manager, Enable);
	BIND_DEFAULT_SOAP2FSM_ACTION(Manager, Disable);
	BIND_DEFAULT_SOAP2FSM_ACTION(Manager, Halt);

	// Other SOAP call-back functions
	xoap::bind(this, &emu::fed::Manager::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);

	// FSM state definitions and state-change call-back functions
	fsm_.addState('H', "Halted", this, &emu::fed::Manager::stateChanged);
	fsm_.addState('C', "Configured", this, &emu::fed::Manager::stateChanged);
	fsm_.addState('E', "Enabled", this, &emu::fed::Manager::stateChanged);
	fsm_.addState('U', "Indefinite", this, &emu::fed::Manager::stateChanged);

	// FSM transition definitions
	fsm_.addStateTransition('H', 'C', "Configure", this, &emu::fed::Manager::configureAction); // valid
	fsm_.addStateTransition('C', 'C', "Configure", this, &emu::fed::Manager::configureAction); // valid
	fsm_.addStateTransition('E', 'C', "Configure", this, &emu::fed::Manager::configureAction); // invalid
	fsm_.addStateTransition('U', 'C', "Configure", this, &emu::fed::Manager::configureAction); // valid
	fsm_.addStateTransition('F', 'C', "Configure", this, &emu::fed::Manager::configureAction); // invalid

	fsm_.addStateTransition('H', 'C', "Disable", this, &emu::fed::Manager::disableAction); // invalid
	fsm_.addStateTransition('C', 'C', "Disable", this, &emu::fed::Manager::disableAction); // invalid
	fsm_.addStateTransition('E', 'C', "Disable", this, &emu::fed::Manager::disableAction); // valid
	fsm_.addStateTransition('U', 'C', "Disable", this, &emu::fed::Manager::disableAction); // invalid
	fsm_.addStateTransition('F', 'C', "Disable", this, &emu::fed::Manager::disableAction); // invalid
	
	fsm_.addStateTransition('H', 'E', "Enable", this, &emu::fed::Manager::enableAction); // invalid
	fsm_.addStateTransition('C', 'E', "Enable", this, &emu::fed::Manager::enableAction); // valid
	fsm_.addStateTransition('E', 'E', "Enable", this, &emu::fed::Manager::enableAction); // invalid
	fsm_.addStateTransition('U', 'E', "Enable", this, &emu::fed::Manager::enableAction); // invalid
	fsm_.addStateTransition('F', 'E', "Enable", this, &emu::fed::Manager::enableAction); // invalid
	
	fsm_.addStateTransition('H', 'H', "Halt", this, &emu::fed::Manager::haltAction); // valid
	fsm_.addStateTransition('C', 'H', "Halt", this, &emu::fed::Manager::haltAction); // valid
	fsm_.addStateTransition('E', 'H', "Halt", this, &emu::fed::Manager::haltAction); // valid
	fsm_.addStateTransition('U', 'H', "Halt", this, &emu::fed::Manager::haltAction); // valid
	fsm_.addStateTransition('F', 'H', "Halt", this, &emu::fed::Manager::haltAction); // valid

	fsm_.addStateTransition('H', 'U', "Unknown", this, &emu::fed::Manager::unknownAction); // invalid
	fsm_.addStateTransition('C', 'U', "Unknown", this, &emu::fed::Manager::unknownAction); // invalid
	fsm_.addStateTransition('E', 'U', "Unknown", this, &emu::fed::Manager::unknownAction); // invalid
	fsm_.addStateTransition('U', 'U', "Unknown", this, &emu::fed::Manager::unknownAction); // invalid
	fsm_.addStateTransition('F', 'U', "Unknown", this, &emu::fed::Manager::unknownAction); // invalid

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

}


// HyperDAQ pages
void emu::fed::Manager::webDefault(xgi::Input *in, xgi::Output *out)
{
	
	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("errorFlasher.js");
	jsFileNames.push_back("definitions.js");
	jsFileNames.push_back("manager.js");
	jsFileNames.push_back("common.js");
	*out << Header("FED Crate Manager", jsFileNames);

	
	// Current condition of the FED system
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_System_Status_titlebar") << std::endl;
	*out << cgicc::div("FED System Status")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_System_Status_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_System_Status_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_System_Status_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_System_Status_dialog") << std::endl;
	
	*out << cgicc::img()
		.set("id", "statusicon")
		.set("src", "/emu/emuDCS/FEDApps/images/dialog-warning.png")
		.set("alt", "Status Icon") << std::endl;
	
	*out << cgicc::div()
		.set("class", "category") << std::endl;
	*out << "Current state: ";
	*out << cgicc::span("Unknown")
		.set("class", "Unknown")
		.set("id", "manager_state") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "description")
		.set("id", "status_description") << std::endl;
	*out << "The Manager has not yet contacted the Communicator applications.  Until that happens, the system will be in an unknown state." << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "description") << std::endl;
	cgicc::input checkBox;
	checkBox.set("type", "checkbox")
		.set("id", "enable_buttons")
		.set("name", "enable_buttons");
	std::string disableButtons = "true";
	if (ignoreSOAP_) checkBox.set("checked", "true");
	*out << checkBox << std::endl;
	*out << cgicc::label()
		.set("for", "enable_buttons") << std::endl;
	*out << "Enable manual state changes (not recommended)" << std::endl;
	*out << cgicc::label() << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::button()
		.set("class", "left button statechange")
		.set("id", "halt_button")
		.set("command", "Halt")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/process-stop.png");
	*out << "Halt" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "enable_button")
		.set("command", "Enable")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/go-next.png");
	*out << "Enable" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "disable_button")
		.set("command", "Disable")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/go-previous.png");
	*out << "Disable" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::button()
		.set("class", "right button statechange")
		.set("id", "configure_button")
		.set("command", "Configure")
		.set("disabled", "true")
		.set("style", "display: none") << std::endl;
	*out << cgicc::img()
		.set("class", "icon")
		.set("src", "/emu/emuDCS/FEDApps/images/view-refresh.png");
	*out << "Configure" << std::endl;
	*out << cgicc::button() << std::endl;
	
	*out << cgicc::fieldset() << std::endl;
	
	
	// Current condition of the individual Communicator systems
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Communicator_Status_titlebar") << std::endl;
	*out << cgicc::div("FED Communicator Application Status")
		.set("class", "titletext");
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_Communicator_Status_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Communicator_Status_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Communicator_Status_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog default_width")
		.set("id", "FED_Communicator_Status_dialog") << std::endl;
		
	*out << cgicc::div("Waiting for data from the Communicator applications...")
		.set("class", "description deleteme") << std::endl;
	
	*out << cgicc::fieldset() << std::endl;
	
	*out << cgicc::textarea()
		.set("id", "debug")
		.set("style", "display: none") << std::endl;
	*out << cgicc::textarea() << std::endl;

	*out << Footer() << std::endl;

}



void emu::fed::Manager::webGetSystemStatus(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Push back my FSM state
	output.push_back(JSONSpirit::Pair("state", state_.toString()));
	
	// And now return everything as JSON
	*out << JSONSpirit::write(output);
}



void emu::fed::Manager::webGetCommunicatorStatus(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Get the info from the underlying states
	JSONSpirit::Array underlyingStatus = getUnderlyingStatus();

	// Add that to the JSON output
	output.push_back(JSONSpirit::Pair("communicators", underlyingStatus));
	
	// And now return everything as JSON
	*out << JSONSpirit::write(output);
}



std::vector<emu::base::WebReportItem> emu::fed::Manager::materialToReportOnPage1()
{
	std::vector<emu::base::WebReportItem> report;
	
	std::ostringstream managerURL;
	managerURL << getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << getApplicationDescriptor()->getURN();
	
	// Application title
	report.push_back(emu::base::WebReportItem("title", "FED System", "", "", "", managerURL.str()));
	
	// States of underlying Communicators
	JSONSpirit::Array underlyingStatus = getUnderlyingStatus();
	
	// Calculate my state
	report.push_back(emu::base::WebReportItem("State", getManagerState(state_.toString(), underlyingStatus), "Current state of the FED finite state machine", "", "", managerURL.str()));
	
	float dccInRate = 0;
	float dccOutRate = 0;
	
	for (JSONSpirit::Array::const_iterator iApp = underlyingStatus.begin(); iApp != underlyingStatus.end(); iApp++) {
		
		std::string systemName = "?";
		std::string fmmErrors = "0";
		std::string url = "";

		JSONSpirit::Object appObject = iApp->get_obj();
		for (JSONSpirit::Object::const_iterator iPair = appObject.begin(); iPair != appObject.end(); iPair++) {
			
			// System name
			if (iPair->name_ == "systemName") {
				if (iPair->value_.get_str() == "Track-Finder") systemName = "TF";
				else if (iPair->value_.get_str() == "Plus-Side") {
					systemName = "ME+";
				} else if (iPair->value_.get_str() == "Minus-Side") {
					systemName = "ME-";
				}
			}
	
			// FMM errors per endcap
			else if (iPair->name_ == "fibersWithErrors") {
				std::ostringstream fiberStream;
				fiberStream << iPair->value_.get_int();
				fmmErrors = fiberStream.str();
			}
			
			// Sum up dcc input/output averages
			else if (iPair->name_ == "totalDCCInputRate") dccInRate += iPair->value_.get_real();
			else if (iPair->name_ == "totalDCCOutputRate") dccOutRate += iPair->value_.get_real();
			
			// Figure out my monitor's URL
			else if (iPair->name_ == "monitorURL") url = iPair->value_.get_str();
		}
		
		// Push back the report for the system
		report.push_back(emu::base::WebReportItem(systemName + " Errors", fmmErrors, "Number of fiber inputs currently reporting errors (since the last reset or resync)", "", "", url));
	}
	
	// Push back the heartbeats
	std::ostringstream inRateStream;
	inRateStream << dccInRate;
	std::ostringstream outRateStream;
	outRateStream << dccOutRate;
	report.push_back(emu::base::WebReportItem("DCC Total Input Rate", inRateStream.str(), "DCC input rate totaled over all DDUs (in bytes/s)", "", "", ""));
	report.push_back(emu::base::WebReportItem("DCC Total Output Rate", outRateStream.str(), "DCC output rate totaled over all S-Links (in bytes/s)", "", "", ""));
	
	return report;
}



void emu::fed::Manager::configureAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Configure");

	if (state_.toString() != "Halted") {
		std::ostringstream error;
		error << state_.toString() << "->Configured via action 'Configure' is not valid:  transitioning through 'Halted' first";
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("WARN", e);

		fireEvent("Halt");
	}

	try{
		sendSOAPCommand("Configure","emu::fed::Communicator");
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception in configuring Manager";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}

	std::string underlyingStates = getManagerState("Configured", getUnderlyingStatus());
	if (underlyingStates == "Failed") {
		std::ostringstream error;
		error << "Failure in achieving consistant underlying FSM states";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("FATAL", e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	} else if (underlyingStates == "Unknown") {
		try {
			fireEvent("Unknown");
		} catch (toolbox::fsm::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception transitioning to 'Unknown' state.  What the . . . ?";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			notifyQualified("FATAL", e);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
		}
	}
}



void emu::fed::Manager::enableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Enable");

	if (state_.toString() != "Configured") {
		std::ostringstream error;
		error << state_.toString() << "->Enabled via action 'Enable' is not valid:  transitioning through 'Halted' and 'Configured' first";
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("WARN", e);
		
		fireEvent("Halt");
		fireEvent("Configure");
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "The run number is " << runNumber_.toString());

	// PGK Now send the run number to the Communicators.
	try {
		setParameter("Communicator", "runNumber", "xsd:unsignedLong", runNumber_.toString());
	} catch (emu::fed::exception::SOAPException &e) {
		std::ostringstream error;
		error << "Unable to send runNumber to Communicators";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
	}

	try{
		sendSOAPCommand("Enable","emu::fed::Communicator");
	} catch (emu::fed::exception::SOAPException &e) {
		std::ostringstream error;
		error << "Exception in enabling Communicators";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}

	std::string underlyingStates = getManagerState("Enabled", getUnderlyingStatus());
	if (underlyingStates == "Failed") {
		std::ostringstream error;
		error << "Failure in achieving consistant underlying FSM states";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("FATAL", e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	} else if (underlyingStates == "Unknown") {
		try {
			fireEvent("Unknown");
		} catch (toolbox::fsm::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception transitioning to 'Unknown' state.  What the . . . ?";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			notifyQualified("FATAL", e);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
		}
	}
}



void emu::fed::Manager::disableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transtion received:  Disable");

	if (state_.toString() != "Enabled") {
		std::ostringstream error;
		error << state_.toString() << "->Configured via action 'Disable' is not valid:  doing 'Halted'->'Configured' instead";
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("WARN", e);

		fireEvent("Halt");
		fireEvent("Configure");

	} else {

		try{
			sendSOAPCommand("Disable","emu::fed::Communicator");
		} catch (emu::fed::exception::SOAPException &e) {
			std::ostringstream error;
			error << "Exception in disabling Communicators";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
			notifyQualified("FATAL", e2);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
		}
	}

	std::string underlyingStates = getManagerState("Configured", getUnderlyingStatus());
	if (underlyingStates == "Failed") {
		std::ostringstream error;
		error << "Failure in achieving consistant underlying FSM states";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("FATAL", e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	} else if (underlyingStates == "Unknown") {
		try {
			fireEvent("Unknown");
		} catch (toolbox::fsm::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception transitioning to 'Unknown' state.  What the . . . ?";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			notifyQualified("FATAL", e);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
		}
	}
}



void emu::fed::Manager::haltAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM transition received:  Halt");

	try{
		sendSOAPCommand("Halt","emu::fed::Communicator");
	} catch (emu::fed::exception::SOAPException &e) {
		std::ostringstream error;
		error << "Exception in halting Communicators";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SoftwareException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}

	std::string underlyingStates = getManagerState("Halted", getUnderlyingStatus());
	if (underlyingStates == "Failed") {
		std::ostringstream error;
		error << "Failure in achieving consistant underlying FSM states";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("FATAL", e);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
	} else if (underlyingStates == "Unknown") {
		try {
			fireEvent("Unknown");
		} catch (toolbox::fsm::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception transitioning to 'Unknown' state.  What the . . . ?";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			notifyQualified("FATAL", e);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e);
		}
	}
}



void emu::fed::Manager::unknownAction(toolbox::Event::Reference event)
{
	std::ostringstream error;
	error << "FSM transition to Unknown encountered";
	LOG4CPLUS_WARN(getApplicationLogger(), error.str());
	XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
	notifyQualified("WARN", e);
}



std::string emu::fed::Manager::getManagerState(const std::string &targetState, const JSONSpirit::Array &underlyingStatus)
{
	
	// This map is to make sure everybody is in the same state.
	std::map<const std::string, unsigned int> stateMap;
	
	for (JSONSpirit::Array::const_iterator iApp = underlyingStatus.begin(); iApp != underlyingStatus.end(); iApp++) {
		
		JSONSpirit::Object appObject = iApp->get_obj();
		for (JSONSpirit::Object::const_iterator iPair = appObject.begin(); iPair != appObject.end(); iPair++) {
			if (iPair->name_ == "state") {
				std::string state = iPair->value_.get_str();
				//LOG4CPLUS_DEBUG(getApplicationLogger(), "State of underlying Communicator: " << state);
				
				if (state == "Failed" || state == "Unknown") {
					std::ostringstream error;
					error << "One or more Communicator application is in a Failed or Unknown state";
					LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::exception::SoftwareException, e, error.str());
					notifyQualified("FATAL", e);
					return "Failed";
				}
				
				stateMap[state]++;
				break;
			}
		}
		
	}
	
	if (stateMap.size() > 1) {
		std::ostringstream error;
		error << "Inconsistant state across Communicator applications";
		XCEPT_DECLARE(emu::fed::exception::SoftwareException, e, error.str());
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		notifyQualified("WARN", e);
		return "Unknown";
	}

	if (stateMap.size() == 0 || stateMap.begin()->first != targetState) {
		std::ostringstream error;
		error << "Communicator applications are not in the target state of " << targetState;
		XCEPT_DECLARE(emu::fed::exception::SoftwareException, e, error.str());
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		notifyQualified("FATAL", e);
		return "Failed";
	}
	
	//LOG4CPLUS_DEBUG(getApplicationLogger(), "All Communicator applications are in the target state of " << targetState);
	return targetState;
}



JSONSpirit::Array emu::fed::Manager::getUnderlyingStatus()
{
	
	JSONSpirit::Array returnMe;
	
	std::set<xdaq::ApplicationDescriptor *> descriptors = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("emu::fed::Communicator");
	
	for (std::set<xdaq::ApplicationDescriptor *>::iterator iDescriptor = descriptors.begin(); iDescriptor != descriptors.end(); iDescriptor++) {
	
		JSONSpirit::Object applicationObject;
		
		applicationObject.push_back(JSONSpirit::Pair("instance", (int) (*iDescriptor)->getInstance()));
		
		std::ostringstream urlStream;
		urlStream << (*iDescriptor)->getContextDescriptor()->getURL() << "/" << (*iDescriptor)->getURN();
		applicationObject.push_back(JSONSpirit::Pair("url", urlStream.str()));
		
		// Ping the Communicators for their information.
		xoap::MessageReference reply;
		try {
			reply = getParameters((*iDescriptor));
			REVOKE_ALARM("ManagerGetStatus", NULL);
		} catch (emu::fed::exception::SOAPException &e) {
			std::ostringstream error;
			error << "Unable to get parameters from application '" << (*iDescriptor)->getClassName() << "' instance " << (*iDescriptor)->getInstance();
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::SOAPException, "ManagerGetStatus", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
			continue;
		}
		
		JSONSpirit::Pair systemNamePair = toJSONPair<xdata::String, std::string>(reply, "systemName", "(unnamed instance)");
		applicationObject.push_back(systemNamePair);
		applicationObject.push_back(toJSONPair<xdata::String, std::string>(reply, "state", "Unknown"));
		applicationObject.push_back(toJSONPair<xdata::String, std::string>(reply, "configMode", "Unknown"));
		applicationObject.push_back(toJSONPair<xdata::Boolean, bool>(reply, "ignoreSOAP", false));
		applicationObject.push_back(toJSONPair<xdata::UnsignedInteger, int>(reply, "fibersWithErrors", 0));
		applicationObject.push_back(toJSONPair<xdata::Float, double>(reply, "totalDCCInputRate", 0));
		applicationObject.push_back(toJSONPair<xdata::Float, double>(reply, "totalDCCOutputRate", 0));
		
		// Get the Monitor URL that matches this application
		// Figure out which monitor matches this communicator
		std::string monitorURL;
		try {
			xdaq::ApplicationDescriptor *monitorApp = findMatchingApplication<xdata::String, std::string>("emu::fed::Monitor", "systemName", systemNamePair.value_.get_str());
			std::ostringstream monitorStream;
			monitorStream << monitorApp->getContextDescriptor()->getURL() << "/" << monitorApp->getURN();
			monitorURL = monitorStream.str();
		} catch (emu::fed::exception::SoftwareException &e) {
			// do nothing
		}
		applicationObject.push_back(JSONSpirit::Pair("monitorURL", monitorURL));
		
		// Do the same for the Commander applications
		std::string commanderURL;
		try {
			xdaq::ApplicationDescriptor *commanderApp = findMatchingApplication<xdata::String, std::string>("emu::fed::Commander", "systemName", systemNamePair.value_.get_str());
			std::ostringstream commanderStream;
			commanderStream << commanderApp->getContextDescriptor()->getURL() << "/" << commanderApp->getURN();
			commanderURL = commanderStream.str();
		} catch (emu::fed::exception::SoftwareException &e) {
			// do nothing
		}
		applicationObject.push_back(JSONSpirit::Pair("commanderURL", commanderURL));
		
		
		returnMe.push_back(applicationObject);
	}
	
	return returnMe;
	
}



xoap::MessageReference emu::fed::Manager::onSetTTSBits(xoap::MessageReference message)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP command received: SetTTSBits, ttsID_=" << ttsID_ << ", ttsBits_=" << std::hex << ttsBits_ << std::dec);
	
	// JRG, decode the Source ID into Crate/Slot locations
	// PGK The ttsID_ is given to us by the CSCSV, so we don't have to do
	//  anything to get it.
	
	xdata::UnsignedInteger crateNumber = 0;
	xdata::UnsignedInteger slotNumber = 0;
	int ttsID = ttsID_; // Cached for easier manipulations
	
	if (ttsID == 760) { // TF-DDU
		crateNumber = 5;
		slotNumber = 2;
		
	} else if ((ttsID >= 750 && ttsID <= 756) || (ttsID >= 880 && ttsID <= 886)) { // DCCs
		// Determine if this is a master or slave DCC (for super-LHC)
		if (ttsID > 756) slotNumber = 17;
		else slotNumber = 8;
		// Determine crate
		crateNumber = (ttsID%10) / 2 + 1;
		
	} else if (ttsID >= 831 && ttsID <= 869 && ttsID%10) { // DDUs
		// Determine slot number.  Note that there is no super-LHC checking here!
		slotNumber = ttsID%10 + 2;
		if ((unsigned int) slotNumber > 7) slotNumber++; // slot 8 is DCC
		// Determine crate number.  This is rediculous.
		if (ttsID >= 831 && ttsID <= 839) crateNumber = 2;
		else if (ttsID >= 841 && ttsID <= 849) crateNumber = 1;
		else if (ttsID >= 851 && ttsID <= 859) crateNumber = 4;
		else crateNumber = 3;
		
	} else {
		std::ostringstream error;
		error << "ttsID_=" << ttsID_.toString() << " is out-of-bounds for the CSC FEDs";
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e, error.str());
		notifyQualified("WARN", e);
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		return createReply(message);
	}

	try {

		// This will send the paramters to all the Communicator applications.
		setParameter("Communicator", "ttsCrate", "xsd:unsignedInt", crateNumber.toString());
		setParameter("Communicator", "ttsSlot", "xsd:unsignedInt", slotNumber.toString());
		setParameter("Communicator", "ttsBits", "xsd:int", ttsBits_.toString());
		
		// The Communicator applications will decide if they should do anything based on the crates they conmmand.
		sendSOAPCommand("SetTTSBits", "emu::fed::Communicator");
		
	} catch (emu::fed::exception::SoftwareException &e) {
		std::ostringstream error;
		error << "Exception caught in completing setTTSBits";
		XCEPT_DECLARE_NESTED(emu::fed::exception::OutOfBoundsException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
	}
	
	return createReply(message);
	
}

