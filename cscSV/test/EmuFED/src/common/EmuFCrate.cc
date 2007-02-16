// EmuFCrate.cc

#include "EmuFCrate.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrate);

EmuFCrate::EmuFCrate(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuFCrate::onConfigure,  "Configure",  XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onEnable,     "Enable",     XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onDisable,    "Disable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onHalt,       "Halt",       XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrate::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);

	getApplicationInfoSpace()->fireItemAvailable("ttsCrate", &tts_crate_);
	getApplicationInfoSpace()->fireItemAvailable("ttsSlot",  &tts_slot_);
	getApplicationInfoSpace()->fireItemAvailable("ttsBits",  &tts_bits_);

	fsm_.addState('H', "Halted",     this, &EmuFCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFCrate::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuFCrate::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuFCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuFCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &EmuFCrate::enableAction);
	fsm_.addStateTransition(
			'E', 'C', "Disable",   this, &EmuFCrate::disableAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &EmuFCrate::haltAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuFCrate::haltAction);

	fsm_.addStateTransition(
			'E', 'E', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuFCrate");
}

xoap::MessageReference EmuFCrate::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuFCrate::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference EmuFCrate::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference EmuFCrate::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

xoap::MessageReference EmuFCrate::onSetTTSBits(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
    LOG4CPLUS_INFO(getApplicationLogger(),
			"Setting TTS: crate " << tts_crate_ << " slot " << tts_slot_
			<< " bits " << hex << tts_bits_ << dec);

	fireEvent("SetTTSBits");

	return createReply(message);
}

void EmuFCrate::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrate::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrate::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrate::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrate::setTTSBitsAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
