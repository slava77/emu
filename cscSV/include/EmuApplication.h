#ifndef __EMU_APPLICATION_H__
#define __EMU_APPLICATION_H__

#include "xdaq/WebApplication.h"

#include <string>
#include "xdata/String.h"
#include "toolbox/fsm/FiniteStateMachine.h"

class EmuApplication : public xdaq::WebApplication  
{
public:
	XDAQ_INSTANTIATOR();

	EmuApplication(xdaq::ApplicationStub *stub)
			throw (xdaq::exception::Exception);

protected:
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);
	void transitionFailed(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);
	void fireEvent(std::string event) throw (toolbox::fsm::exception::Exception);

	xoap::MessageReference createReply(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	toolbox::fsm::FiniteStateMachine fsm_;
	xdata::String state_;
};

#endif  // ifndef __EMU_APPLICATION_H__
// vim: set ai sw=4 ts=4:
