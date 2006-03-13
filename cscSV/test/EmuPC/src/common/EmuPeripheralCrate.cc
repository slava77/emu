// EmuPeripheralCrate.cc

#include "EmuPeripheralCrate.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrate);

EmuPeripheralCrate::EmuPeripheralCrate(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xml_file_name_ = "";
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xml_file_name_);

	xoap::bind(this, &EmuPeripheralCrate::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuPeripheralCrate::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuPeripheralCrate::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &EmuPeripheralCrate::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuPeripheralCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuPeripheralCrate::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrate::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuPeripheralCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuPeripheralCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &EmuPeripheralCrate::enableAction);
	fsm_.addStateTransition(
			'E', 'C', "Disable",   this, &EmuPeripheralCrate::disableAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate");
}

xoap::MessageReference EmuPeripheralCrate::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuPeripheralCrate::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference EmuPeripheralCrate::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference EmuPeripheralCrate::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void EmuPeripheralCrate::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type() << ":" << (string)xmlFileName_);
}

void EmuPeripheralCrate::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuPeripheralCrate::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuPeripheralCrate::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuPeripheralCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
