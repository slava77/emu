#ifndef EmuTFMonitor_h
#define EmuTFMonitor_h

#include "xdaq.h"
#include "xdata.h"
#include "toolbox.h"
#include "xoap.h"
#include "xgi.h"

#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"

#include "i2oEmuMonitorMsg.h"

using namespace toolbox;

class EmuTFMonitor: public xdaq::WebApplication, xdata::ActionListener, Task {
private:
	toolbox::fsm::FiniteStateMachine fsm_;
	xgi::WSM                         wsm_;
	xdata::String                    inputDeviceName_;
	toolbox::mem::Pool               *pool_;

public:
	// Callback for requesting current exported parameter values
	void actionPerformed (xdata::Event& e);
	// Following functions are needed by Web State Machine (?)
	void Default(xgi::Input *in, xgi::Output *out ) throw (xgi::exception::Exception) {
		wsm_.displayPage(out);
	}
	void dispatch(xgi::Input *in, xgi::Output *out) throw (xgi::exception::Exception) {
		cgicc::Cgicc cgi(in);                 
		//const cgicc::CgiEnvironment& env = cgi.getEnvironment();
		cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
		std::string stateInput = (*stateInputElement).getValue();
		wsm_.fireEvent(stateInput,in,out);
	}
	void stateMachinePage(xgi::Output *out) throw (xgi::exception::Exception);
	void failurePage     (xgi::Output *out, xgi::exception::Exception& e) throw (xgi::exception::Exception);
	// Finite State Machine
	void Configuring(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
	void Enabling   (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
	void Halting    (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
	// Following functions tie up Web State Machine to Finite State Machine
	void Configure(xgi::Input *in) throw (xgi::exception::Exception);
	void Enable   (xgi::Input *in) throw (xgi::exception::Exception);
	void Halt     (xgi::Input *in) throw (xgi::exception::Exception);

	// Ask for data and get it
	int  sendDataRequest(unsigned long last);
	void emuDataMsg(toolbox::mem::Reference *bufRef);
	// Task's function
	int svc(void);

	xoap::MessageReference fireEvent(xoap::MessageReference msg) throw (xoap::exception::Exception);

	EmuTFMonitor(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	XDAQ_INSTANTIATOR();
};

#endif
