// CSCSupervisor.cc

#include "CSCSupervisor.h"

#include <sstream>

#include "xdaq/NamespaceURI.h"
#include "xdaq/ApplicationGroup.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"

#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"

using namespace std;
using namespace cgicc;

XDAQ_INSTANTIATOR_IMPL(CSCSupervisor);

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";
static const unsigned int N_LOG_MESSAGES = 10;
static const string STATE_UNKNOWN = "unknown";

CSCSupervisor::CSCSupervisor(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub),
		runmode_(""), runnumber_(""), nevents_(""), error_message_("")
{
	getApplicationInfoSpace()->fireItemAvailable("RunType", &x_runmode_);
	getApplicationInfoSpace()->fireItemAvailable("RunNumber", &x_runnumber_);
	getApplicationInfoSpace()->fireItemAvailable("configKeys", &config_keys_);
	getApplicationInfoSpace()->fireItemAvailable("configModes", &config_modes_);
	getApplicationInfoSpace()->fireItemAvailable("modesForPC", &modes_pc_);
	getApplicationInfoSpace()->fireItemAvailable("filesForPC", &files_pc_);
	getApplicationInfoSpace()->fireItemAvailable("modesForFC", &modes_fc_);
	getApplicationInfoSpace()->fireItemAvailable("filesForFC", &files_fc_);

	xgi::bind(this, &CSCSupervisor::webDefault,   "Default");
	xgi::bind(this, &CSCSupervisor::webConfigure, "Configure");
	xgi::bind(this, &CSCSupervisor::webEnable,    "Enable");
	xgi::bind(this, &CSCSupervisor::webDisable,   "Disable");
	xgi::bind(this, &CSCSupervisor::webHalt,      "Halt");
	xgi::bind(this, &CSCSupervisor::webReset,     "Reset");

	xoap::bind(this, &CSCSupervisor::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onHalt,      "Halt",      XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onReset,     "Reset",     XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &CSCSupervisor::stateChanged);
	fsm_.addState('C', "Configured", this, &CSCSupervisor::stateChanged);
	fsm_.addState('E', "Enabled",    this, &CSCSupervisor::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &CSCSupervisor::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &CSCSupervisor::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &CSCSupervisor::enableAction);
	fsm_.addStateTransition(
			'E', 'C', "Disable",   this, &CSCSupervisor::disableAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &CSCSupervisor::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &CSCSupervisor::haltAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &CSCSupervisor::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	state_table_.addApplication(this, "EmuFCrateSOAP");
	state_table_.addApplication(this, "EmuPeripheralCrate");
	state_table_.addApplication(this, "EmuDAQManager");
	state_table_.addApplication(this, "LTCControl");

	last_log_.size(N_LOG_MESSAGES);

	LOG4CPLUS_INFO(getApplicationLogger(), "CSCSupervisor");
}

xoap::MessageReference CSCSupervisor::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	runmode_ = x_runmode_;
	runnumber_ = x_runnumber_.toString();
	nevents_ = 1000;

	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

xoap::MessageReference CSCSupervisor::onReset(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	resetAction();
	fireEvent("Halt");

	return createReply(message);
}

void CSCSupervisor::webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	// Header
	*out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	*out << html() << endl;

	*out << head() << endl;
	*out << title("CSCSupervisor") << endl;
	*out << cgicc::link().set("rel", "stylesheet")
			.set("href", "/daq/lib/linux/x86/cscsv.css")
			.set("type", "text/css") << endl;
	*out << head() << endl;

	// Body
	*out << body() << endl;

	// Error message, if exists.
	if (!error_message_.empty()) {
		*out  << p() << span().set("style", "color: red;")
				<< error_message_ << span() << p() << endl;
		error_message_ = "";
	}

	// Config listbox
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Configure") << endl;

	int n_keys = config_keys_.size();

	*out << "Run Type: " << endl;
	*out << cgicc::select().set("name", "runtype") << endl;

	int selected_index = modeToIndex(runmode_);

	for (int i = 0; i < n_keys; ++i) {
		if (i == selected_index) {
			*out << option()
					.set("label", (string)config_keys_[i])
					.set("value", (string)config_modes_[i])
					.set("selected", "");
		} else {
			*out << option()
					.set("label", (string)config_keys_[i])
					.set("value", (string)config_modes_[i]);
		}
		*out << (string)config_keys_[i] << option() << endl;
	}

	*out << cgicc::select() << br() << endl;
	
	*out << "Run Number: " << endl;
	*out << input().set("type", "text")
			.set("name", "runnumber")
			.set("value", runnumber_)
			.set("size", "40") << br() << endl;

	*out << "Max # of Events: " << endl;
	*out << input().set("type", "text")
			.set("name", "nevents")
			.set("value", nevents_)
			.set("size", "40") << br() << endl;

	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Configure") << endl;
	*out << form() << endl;

	// Buttons
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Enable") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Enable") << endl;
	*out << form() << endl;

	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Disable") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Disable") << endl;
	*out << form() << endl;

	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Halt") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Halt") << endl;
	*out << form() << endl;

	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Reset") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Reset") << endl;
	*out << form() << endl;

	// Application states
	state_table_.webOutput(out, (string)state_);

	*out << hr() << endl;

	last_log_.webOutput(out);

	*out << body() << html() << endl;
}

void CSCSupervisor::webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	runmode_ = getRunmode(in);
	runnumber_ = getRunNumber(in);
	nevents_ = getNEvents(in);

	if (runmode_.empty()) { error_message_ += "Please select run type.\n"; }
	if (runnumber_.empty()) { error_message_ += "Please set run number.\n"; }
	if (nevents_.empty()) { error_message_ += "Please set max # of events.\n"; }

	if (error_message_.empty()) {
		fireEvent("Configure");
	}

	webRedirect(in, out);
}

void CSCSupervisor::webEnable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Enable");

	webRedirect(in, out);
}

void CSCSupervisor::webDisable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Disable");

	webRedirect(in, out);
}

void CSCSupervisor::webHalt(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Halt");

	webRedirect(in, out);
}

void CSCSupervisor::webReset(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	resetAction();
	fireEvent("Halt");

	webRedirect(in, out);
}

void CSCSupervisor::webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
			url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

void CSCSupervisor::configureAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");
	LOG4CPLUS_DEBUG(getApplicationLogger(), "runmode_: " << runmode_
			<< " runnumber_: " << runnumber_ << " nevents_: " << nevents_);

	try {
		if (state_table_.getState("EmuDAQManager", 0) == "Configured") {
			sendCommand("Halt", "EmuDAQManager");
		}
		if (state_table_.getState("LTCControl", 0) == "Ready") {
			sendCommand("Halt", "LTCControl");
		}
		setParameter("EmuPeripheralCrate", "xmlFileName", "xsd:string",
				trim(getConfigFilename("PC", runmode_)));
		setParameter("EmuDAQManager",
				"runNumber", "xsd:unsignedLong", runnumber_);
		setParameter("EmuDAQManager",
				"maxNumberOfEvents", "xsd:unsignedLong", nevents_);
		sendCommand("Configure", "EmuFCrateSOAP");
		sendCommand("Configure", "EmuPeripheralCrate");
		sendCommand("Configure", "EmuDAQManager");
		sendCommand("Configure", "LTCControl");
	} catch (xoap::exception::Exception e) {
		LOG4CPLUS_ERROR(getApplicationLogger(),
				"Exception in " << evt->type() << ": " << e.what());
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		LOG4CPLUS_ERROR(getApplicationLogger(),
				"Exception in " << evt->type() << ": " << e.what());
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::enableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
		if (state_table_.getState("EmuDAQManager", 0) == "Halted") {
			setParameter("EmuDAQManager",
					"runNumber", "xsd:unsignedLong", runnumber_);
			setParameter("EmuDAQManager",
					"maxNumberOfEvents", "xsd:unsignedLong", nevents_);
			sendCommand("Configure", "EmuDAQManager");
		}
		sendCommand("Enable", "EmuPeripheralCrate");
		sendCommand("Enable", "EmuDAQManager");
		sendCommand("Enable", "LTCControl");
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::disableAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
		sendCommand("Halt", "LTCControl");
		sendCommand("Halt", "EmuDAQManager");
		sendCommand("Disable", "EmuPeripheralCrate");
		sendCommand("Configure", "LTCControl");
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::haltAction(toolbox::Event::Reference evt) 
		throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(begin)");

	try {
		sendCommand("Halt", "EmuFCrateSOAP");
		sendCommand("Halt", "EmuPeripheralCrate");
		sendCommand("Halt", "EmuDAQManager");
		sendCommand("Halt", "LTCControl");
	} catch (xoap::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"SOAP fault was returned", e);
	} catch (xdaq::exception::Exception e) {
		XCEPT_RETHROW(toolbox::fsm::exception::Exception,
				"Failed to send a command", e);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), evt->type() << "(end)");
}

void CSCSupervisor::resetAction() throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(begin)");

	fsm_.reset();

	LOG4CPLUS_DEBUG(getApplicationLogger(), "reset(end)");
}

void CSCSupervisor::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    EmuApplication::stateChanged(fsm);
}

void CSCSupervisor::sendCommand(string command, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	vector<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getApplicationGroup()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);
	xoap::MessageReference reply;

	// send the message one-by-one
	vector<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		// postSOAP() may throw an exception when failed.
		reply = getApplicationContext()->postSOAP(message, *i);

		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference CSCSupervisor::createCommandSOAP(string command)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(
			command, "xdaq", "urn:xdaq-soap:3.0");
	envelope.getBody().addBodyElement(name);

	return message;
}

void CSCSupervisor::setParameter(
		string klass, string name, string type, string value)
{
	// find applications
	vector<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getApplicationGroup()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createParameterSetSOAP(
			klass, name, type, value);
	xoap::MessageReference reply;

	// send the message one-by-one
	vector<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		reply = getApplicationContext()->postSOAP(message, *i);
		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference CSCSupervisor::createParameterSetSOAP(
		string klass, string name, string type, string value)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterSet", "xdaq", "urn:xdaq-soap:3.0");
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			name, klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode(value);

	return message;
}

void CSCSupervisor::analyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app)
{
	string message_str, reply_str;

	reply->writeTo(reply_str);
	ostringstream s;
	s << "Reply from "
			<< app->getClassName() << "(" << app->getInstance() << ")" << endl
			<< reply_str;
	last_log_.add(s.str());
	LOG4CPLUS_DEBUG(getApplicationLogger(), reply_str);

	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

	// do nothing when no fault
	if (!body.hasFault()) { return; }

	ostringstream error;

	error << "SOAP message: " << endl;
	message->writeTo(message_str);
	error << message_str << endl;
	error << "Fault string: " << endl;
	error << reply_str << endl;

	LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
	XCEPT_RAISE(xoap::exception::Exception, "SOAP fault: \n" + reply_str);

	return;
}

string CSCSupervisor::getRunmode(xgi::Input *in)
{
	cgicc::Cgicc cgi(in);
	string runmode;

	form_iterator i = cgi.getElement("runtype");
	if (i != cgi.getElements().end()) {
		runmode = (*i).getValue();
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(),
			"==== run type:" << modeToIndex(runmode) << ":" << runmode);

	return runmode;
}

string CSCSupervisor::getRunNumber(xgi::Input *in)
{
	cgicc::Cgicc cgi(in);
	string runnumber;

	form_iterator i = cgi.getElement("runnumber");
	if (i != cgi.getElements().end()) {
		runnumber = (*i).getValue();
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "==== run number:" << runnumber);

	return runnumber;
}

string CSCSupervisor::getNEvents(xgi::Input *in)
{
	cgicc::Cgicc cgi(in);
	string nevents;

	form_iterator i = cgi.getElement("nevents");
	if (i != cgi.getElements().end()) {
		nevents = (*i).getValue();
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "==== # of events:" << nevents);

	return nevents;
}

int CSCSupervisor::modeToIndex(string mode)
{
	int index = 0;
	for (unsigned int i = 0; i < config_modes_.size(); ++i) {
		if (config_modes_[i] == mode) {
			index = i;
			break;
		}
	}

	return index;
}

string CSCSupervisor::getConfigFilename(string type, string mode) const
{
	xdata::Vector<xdata::String> modes;
	xdata::Vector<xdata::String> files;

	if (type == "PC") {
		modes = modes_pc_;
		files = files_pc_;
	} else if (type == "FC") {
		modes = modes_fc_;
		files = files_fc_;
	} else {
		return "";
	}

	string result = "";
	for (unsigned int i = 0; i < modes.size(); ++i) {
		if (modes[i] == mode) {
			result = files[i];
			break;
		}
	}

	return result;
}

string CSCSupervisor::trim(string orig) const
{
	string s = orig;

	s.erase(0, s.find_first_not_of(" \t\n"));
	s.erase(s.find_last_not_of(" \t\n") + 1);

	return s;
}

void CSCSupervisor::StateTable::addApplication(CSCSupervisor *sv, string klass)
{
	sv_ = sv;

	// find applications
	vector<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = sv->getApplicationContext()->getApplicationGroup()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// add to the table
	vector<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		table_.push_back(
				pair<xdaq::ApplicationDescriptor *, string>(*i, "NULL"));
	}
}

void CSCSupervisor::StateTable::refresh()
{
	string klass = "";
	xoap::MessageReference message, reply;

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass != i->first->getClassName()) {
			klass = i->first->getClassName();
			message = createStateSOAP(klass);
		}

		try {
			reply = sv_->getApplicationContext()->postSOAP(message, i->first);
			sv_->analyzeReply(message, reply, i->first);

			i->second = extractState(reply, klass);
		} catch (xdaq::exception::Exception e) {
			i->second = STATE_UNKNOWN;
		}
	}
}

string CSCSupervisor::StateTable::getState(string klass, unsigned int instance)
{
	string state = "";

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass == i->first->getClassName()
				&& instance == i->first->getInstance()) {
			state = i->second;
			break;
		}
	}

	return state;
}

void CSCSupervisor::StateTable::webOutput(xgi::Output *out, string sv_state)
		throw (xgi::exception::Exception)
{
	refresh();
	*out << table() << tbody() << endl;

	// My state
	*out << tr();
	*out << td() << "CSCSupervisor" << "(" << "0" << ")" << td();
	*out << td().set("class", sv_state) << sv_state << td();
	*out << tr() << endl;

	// Applications
	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		string klass = i->first->getClassName();
		int instance = i->first->getInstance();
		string state = i->second;

		*out << tr();
		*out << td() << klass << "(" << instance << ")" << td();
		*out << td().set("class", state) << state << td();
		*out << tr() << endl;
	}

	*out << tbody() << table() << endl;
}

xoap::MessageReference CSCSupervisor::StateTable::createStateSOAP(
		string klass)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
	xoap::SOAPName properties = envelope.createName(
			"properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			"stateName", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, "xsd:string");

	return message;
}

string CSCSupervisor::StateTable::extractState(xoap::MessageReference message, string klass)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement state = properties.getChildElements(
			*(new xoap::SOAPName("stateName", "", "")))[0];

	return state.getValue();
}

void CSCSupervisor::LastLog::size(unsigned int size)
{
	size_ = size;
}

unsigned int CSCSupervisor::LastLog::size() const
{
	return size_;
}

void CSCSupervisor::LastLog::add(string message)
{
	messages_.push_back(message);
	if (messages_.size() > size_) { messages_.pop_front(); }
}

void CSCSupervisor::LastLog::webOutput(xgi::Output *out)
		throw (xgi::exception::Exception)
{
	*out << "Last " << messages_.size() << " log messages:" << br() << endl;
	*out << textarea().set("cols", "120").set("rows", "20")
			.set("readonly").set("class", "log") << endl;

	deque<string>::iterator i = messages_.begin();
	for (; i != messages_.end(); ++i) {
		*out << *i << endl;
	}

	*out << textarea() << endl;
}

// End of file
// vim: set ai sw=4 ts=4:
