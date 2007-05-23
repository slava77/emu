// TTCciControl.cc

#include "TTCciControl.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(TTCciControl);

TTCciControl::TTCciControl(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	source_clock_ = "CTC";
	source_orbit_ = "LTC";
	source_trigger_ = "TTL";
	source_bgo_ = "LOCAL";

	getApplicationInfoSpace()->fireItemAvailable("ClockSource", &source_clock_);
	getApplicationInfoSpace()->fireItemAvailable("OrbitSource", &source_orbit_);
	getApplicationInfoSpace()->fireItemAvailable("TriggerSource", &source_trigger_);
	getApplicationInfoSpace()->fireItemAvailable("BGOSource", &source_bgo_);

	xoap::bind(this, &TTCciControl::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onSuspend,   "Suspend",   XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onResume,    "Resume",    XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onResynch,   "Resynch",   XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onHardReset, "HardReset", XDAQ_NS_URI);
	xoap::bind(this, &TTCciControl::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &TTCciControl::stateChanged);
	fsm_.addState('R', "Ready",      this, &TTCciControl::stateChanged);
	fsm_.addState('E', "Enabled",    this, &TTCciControl::stateChanged);
	fsm_.addState('S', "Suspended",  this, &TTCciControl::stateChanged);

	fsm_.addStateTransition(
			'H', 'R', "Configure", this, &TTCciControl::configureAction);
	fsm_.addStateTransition(
			'R', 'E', "Enable",    this, &TTCciControl::enableAction);
	fsm_.addStateTransition(
			'E', 'S', "Suspend",   this, &TTCciControl::suspendAction);
	fsm_.addStateTransition(
			'S', 'E', "Resume",    this, &TTCciControl::resumeAction);
	fsm_.addStateTransition(
			'R', 'R', "Resynch",   this, &TTCciControl::resynchAction);
	fsm_.addStateTransition(
			'R', 'R', "HardReset", this, &TTCciControl::hardResetAction);
	fsm_.addStateTransition(
			'S', 'S', "Resynch",   this, &TTCciControl::resynchAction);
	fsm_.addStateTransition(
			'S', 'S', "HardReset", this, &TTCciControl::hardResetAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &TTCciControl::haltAction);
	fsm_.addStateTransition(
			'R', 'H', "Halt",      this, &TTCciControl::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &TTCciControl::haltAction);
	fsm_.addStateTransition(
			'S', 'H', "Halt",      this, &TTCciControl::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "TTCciControl");
}

xoap::MessageReference TTCciControl::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onSuspend(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Suspend");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onResume(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Resume");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onResynch(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Resynch");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onHardReset(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("HardReset");

	return createReply(message);
}

xoap::MessageReference TTCciControl::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void TTCciControl::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::suspendAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::resumeAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::resynchAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::hardResetAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void TTCciControl::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
