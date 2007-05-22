#ifndef __EMU_PERIPHERAL_CRATE_H__
#define __EMU_PERIPHERAL_CRATE_H__

#include "EmuApplication.h"

#include "xdata/String.h"
#include "toolbox/fsm/FiniteStateMachine.h"

class EmuPeripheralCrate : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	EmuPeripheralCrate(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onCalCFEB(xoap::MessageReference message)
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

	xdata::String xml_file_name_;
};

#endif  // ifndef __EMU_PERIPHERAL_CRATE_H__
// vim: set ai sw=4 ts=4:
