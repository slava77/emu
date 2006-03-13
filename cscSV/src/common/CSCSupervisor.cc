// CSCSupervisor.cc

#include "CSCSupervisor.h"

#include "xdaq/NamespaceURI.h"
#include "xdaq/ApplicationGroup.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"

#include "cgicc/HTMLClasses.h"

using namespace std;
using namespace cgicc;

XDAQ_INSTANTIATOR_IMPL(CSCSupervisor);

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";

CSCSupervisor::CSCSupervisor(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub), runtype("")
{
	getApplicationInfoSpace()->fireItemAvailable("configKeys", &config_keys_);
	getApplicationInfoSpace()->fireItemAvailable("configFiles", &config_files_);

	xgi::bind(this, &CSCSupervisor::webDefault,   "Default");
	xgi::bind(this, &CSCSupervisor::webConfigure, "Configure");
	xgi::bind(this, &CSCSupervisor::webEnable,    "Enable");
	xgi::bind(this, &CSCSupervisor::webDisable,   "Disable");
	xgi::bind(this, &CSCSupervisor::webHalt,      "Halt");

	xoap::bind(this, &CSCSupervisor::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &CSCSupervisor::onHalt,      "Halt",      XDAQ_NS_URI);

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

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "CSCSupervisor");
}

xoap::MessageReference CSCSupervisor::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	runtype = getRuntype(message);

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

void CSCSupervisor::webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	// Header
	*out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	*out << html() << endl;

	*out << head() << endl;
	*out << title("CSCSupervisor") << endl;
	*out << head() << endl;

	// Body
	*out << body() << endl;

	// Config listbox
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Configure") << endl;

	int n_keys = config_keys_.size();

	*out << cgicc::select()
			.set("name", "runtype")
			.set("size", "5") << endl;

	for (int i = 0; i < n_keys; ++i) {
		*out << option()
				.set("label", (string)config_keys_[i])
				.set("value", (string)config_files_[i])
				<< (string)config_keys_[i] << option() << endl;
	}

	*out << cgicc::select() << endl;

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

	*out << body() << html() << endl;
}

void CSCSupervisor::webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	runtype = getRuntype(in);

	fireEvent("Configure");

	webDefault(in, out);
}

void CSCSupervisor::webEnable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Enable");

	webDefault(in, out);
}

void CSCSupervisor::webDisable(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Disable");

	webDefault(in, out);
}

void CSCSupervisor::webHalt(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Halt");

	webDefault(in, out);
}

void CSCSupervisor::configureAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	setParameter("EmuPeripheralCrate", 0, "xmlFileName", "xsd:string", runtype);
	sendCommand("Configure", "EmuPeripheralCrate", 0);

	sendCommand("Configure", "EmuFEDCrate", 0);
	sendCommand("Configure", "EmuDAQManager", 0);

	LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void CSCSupervisor::enableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	sendCommand("Enable", "EmuPeripheralCrate", 0);
	sendCommand("Enable", "EmuDAQManager", 0);

	LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void CSCSupervisor::disableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	sendCommand("Disable", "EmuDAQManager", 0);
	sendCommand("Disable", "EmuPeripheralCrate", 0);

	LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void CSCSupervisor::haltAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	sendCommand("Halt", "EmuPeripheralCrate", 0);
	sendCommand("Halt", "EmuFEDCrate", 0);
	sendCommand("Halt", "EmuDAQManager", 0);

	LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void CSCSupervisor::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    EmuApplication::stateChanged(fsm);
}

void CSCSupervisor::sendCommand(string command, string klass, int instance)
		throw (toolbox::fsm::exception::Exception)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPPart soap = message->getSOAPPart();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(
			command, "xdaq", "urn:xdaq-soap:3.0");
	envelope.getBody().addBodyElement(name);

	xdaq::ApplicationDescriptor *target = getApplicationContext()
			->getApplicationGroup()->getApplicationDescriptor(klass, instance);

	getApplicationContext()->postSOAP(message, target);
}

void CSCSupervisor::setParameter(
		string klass, int instance, string name, string type, string value)
		throw (toolbox::fsm::exception::Exception)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPPart soap = message->getSOAPPart();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();

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

	xdaq::ApplicationDescriptor *target = getApplicationContext()
			->getApplicationGroup()->getApplicationDescriptor(klass, instance);

	getApplicationContext()->postSOAP(message, target);
}

string CSCSupervisor::getRuntype(xoap::MessageReference message)
{
	return "physics";
}

string CSCSupervisor::getRuntype(xgi::Input *in)
{
	cgicc::Cgicc cgi(in);
	string runtype;

	form_iterator i = cgi.getElement("runtype");
	if (i != cgi.getElements().end()) {
		runtype = (*i).getValue();
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "==== run type:" << runtype);

	return runtype;
}

// End of file
// vim: set ai sw=4 ts=4:
