// LTCControl.cc

#include "LTCControl.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(LTCControl);

LTCControl::LTCControl(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &LTCControl::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onSuspend,   "Suspend",   XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onResume,    "Resume",    XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onResynch,   "Resynch",   XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onHardReset, "HardReset", XDAQ_NS_URI);
	xoap::bind(this, &LTCControl::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &LTCControl::stateChanged);
	fsm_.addState('R', "Ready",      this, &LTCControl::stateChanged);
	fsm_.addState('E', "Enabled",    this, &LTCControl::stateChanged);
	fsm_.addState('S', "Suspended",  this, &LTCControl::stateChanged);

	fsm_.addStateTransition(
			'H', 'R', "Configure", this, &LTCControl::configureAction);
	fsm_.addStateTransition(
			'R', 'E', "Enable",    this, &LTCControl::enableAction);
	fsm_.addStateTransition(
			'E', 'S', "Suspend",   this, &LTCControl::suspendAction);
	fsm_.addStateTransition(
			'S', 'E', "Resume",    this, &LTCControl::resumeAction);
	fsm_.addStateTransition(
			'R', 'R', "Resynch",   this, &LTCControl::resynchAction);
	fsm_.addStateTransition(
			'R', 'R', "HardReset", this, &LTCControl::hardResetAction);
	fsm_.addStateTransition(
			'S', 'S', "Resynch",   this, &LTCControl::resynchAction);
	fsm_.addStateTransition(
			'S', 'S', "HardReset", this, &LTCControl::hardResetAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &LTCControl::haltAction);
	fsm_.addStateTransition(
			'R', 'H', "Halt",      this, &LTCControl::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &LTCControl::haltAction);
	fsm_.addStateTransition(
			'S', 'H', "Halt",      this, &LTCControl::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "LTCControl");
}

xoap::MessageReference LTCControl::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference LTCControl::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference LTCControl::onSuspend(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Suspend");

	return createReply(message);
}

xoap::MessageReference LTCControl::onResume(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Resume");

	return createReply(message);
}

xoap::MessageReference LTCControl::onResynch(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Resynch");

	return createReply(message);
}

xoap::MessageReference LTCControl::onHardReset(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("HardReset");

	return createReply(message);
}

xoap::MessageReference LTCControl::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void LTCControl::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::suspendAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::resumeAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::resynchAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::hardResetAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void LTCControl::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
