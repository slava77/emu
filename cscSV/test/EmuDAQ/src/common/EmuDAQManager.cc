// EmuDAQManager.cc

#include "EmuDAQManager.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(EmuDAQManager);

EmuDAQManager::EmuDAQManager(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	run_number_ = 0;
	max_n_events_ = 0;
	mode_ = true;
	configured_global_ = false;
	daq_state_ = "Unknown";

	getApplicationInfoSpace()->fireItemAvailable("runNumber", &run_number_);
	getApplicationInfoSpace()->fireItemAvailable("maxNumberOfEvents", &max_n_events_);
	getApplicationInfoSpace()->fireItemAvailable("globalMode", &mode_);
	getApplicationInfoSpace()->fireItemAvailable("configuredInGlobalMode", &configured_global_);
	getApplicationInfoSpace()->fireItemAvailable("daqState", &daq_state_);

	xoap::bind(this, &EmuDAQManager::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuDAQManager::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuDAQManager::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuDAQManager::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuDAQManager::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuDAQManager::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuDAQManager::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &EmuDAQManager::enableAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &EmuDAQManager::haltAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuDAQManager::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &EmuDAQManager::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuDAQManager");
}

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

void EmuDAQManager::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	daq_state_ = state_;

	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set sw=4 ts=4:
