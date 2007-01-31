// EmuFCrateSOAP.cc

#include "EmuFCrateSOAP.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(EmuFCrateSOAP);

EmuFCrateSOAP::EmuFCrateSOAP(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuFCrateSOAP::onConfigure,  "Configure",  XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateSOAP::onHalt,       "Halt",       XDAQ_NS_URI);
	xoap::bind(this, &EmuFCrateSOAP::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);

	getApplicationInfoSpace()->fireItemAvailable("TTSCrate", &tts_crate_);
	getApplicationInfoSpace()->fireItemAvailable("TTSSlot",  &tts_slot_);
	getApplicationInfoSpace()->fireItemAvailable("TTSBits",  &tts_bits_);

	fsm_.addState('H', "Halted",     this, &EmuFCrateSOAP::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFCrateSOAP::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuFCrateSOAP::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuFCrateSOAP::configureAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &EmuFCrateSOAP::haltAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuFCrateSOAP::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuFCrateSOAP");
}

xoap::MessageReference EmuFCrateSOAP::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuFCrateSOAP::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

xoap::MessageReference EmuFCrateSOAP::onSetTTSBits(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
    LOG4CPLUS_INFO(getApplicationLogger(),
			"Setting TTS: crate " << tts_crate_ << " slot " << tts_slot_
			<< " bits " << hex << tts_bits_ << dec);

	return createReply(message);
}

void EmuFCrateSOAP::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrateSOAP::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFCrateSOAP::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
