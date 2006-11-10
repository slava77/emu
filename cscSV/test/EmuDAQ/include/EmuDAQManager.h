#ifndef __EMU_DAQ_MANAGER_H__
#define __EMU_DAQ_MANAGER_H__

#include "EmuApplication.h"

#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "toolbox/fsm/FiniteStateMachine.h"

class EmuDAQManager : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	EmuDAQManager(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);

private:
    void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

	xdata::UnsignedLong run_number_;
	xdata::Integer max_n_events_;
	xdata::Boolean mode_, configured_global_;
	xdata::String daq_state_;
};

#endif  // ifndef __EMU_DAQ_MANAGER_H__
// vim: set sw=4 ts=4:
