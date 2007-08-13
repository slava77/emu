#ifndef _EmuFCrate_h_
#define _EmuFCrate_h_

#include "EmuApplication.h"
#include "EmuFController.h"

#include "xdata/String.h"

class EmuFCrate : public EmuApplication, public EmuFController
{

public:
	XDAQ_INSTANTIATOR();

	EmuFCrate(xdaq::ApplicationStub *s);

	// SOAP call-back methods
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	xoap::MessageReference onSetTTSBits(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// Action methods called at state transitions.
	void configureAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);

	void setTTSBitsAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);

	// HyperDAQ pages
	void webDefault(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webConfigure(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

	void webSetTTSBits(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

private:
	//
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);
	//
	void webRedirect(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

	//
	string getCGIParameter(xgi::Input *in, string name);

private:
	xdata::String xmlFile_;

	xdata::UnsignedInteger ttsCrate_;
	xdata::UnsignedInteger ttsSlot_;
	xdata::UnsignedInteger ttsBits_;

	string ttsCrateStr_, ttsSlotStr_, ttsBitsStr_;


// addition for STEP

 public:

	xdata::UnsignedInteger step_killfiber_;

	xoap::MessageReference onPassthru(xoap::MessageReference message)
		throw (xoap::exception::Exception);

// end addition for STEP

};

#endif  // ifndef _EmuFCrate_h_
// vim: set sw=4 ts=4:
