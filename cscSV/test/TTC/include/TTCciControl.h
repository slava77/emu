#ifndef __TTCCI_CONTROL_H__
#define __TTCCI_CONTROL_H__

#include "EmuApplication.h"

#include "xdata/String.h"

class TTCciControl : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	TTCciControl(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onSuspend(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onResume(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onResynch(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHardReset(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onCyclic(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void suspendAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void resumeAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void resynchAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void hardResetAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);

private:
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

	xdata::String source_clock_;
	xdata::String source_orbit_;
	xdata::String source_trigger_;
	xdata::String source_bgo_;
};

#endif  // ifndef __TTCCI_CONTROL_H__
// vim: set sw=4 ts=4:
