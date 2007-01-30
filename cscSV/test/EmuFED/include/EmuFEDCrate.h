#ifndef __EMU_FED_CRATE_H__
#define __EMU_FED_CRATE_H__

#include "EmuApplication.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "xdata/UnsignedInteger.h"

class EmuFEDCrate : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	EmuFEDCrate(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onSetTTSBits(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);

private:
    void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

private:
	xdata::UnsignedInteger tts_crate_;
	xdata::UnsignedInteger tts_slot_;
	xdata::UnsignedInteger tts_bits_;
};

#endif  // ifndef __EMU_FED_CRATE_H__
// vim: set ai sw=4 ts=4:
