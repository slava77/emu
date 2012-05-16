#ifndef _emuSTEPManager_h_
#define _emuSTEPManager_h_

#include "emuSTEPApplication.h"

#include "xgi/Method.h"
#include "xgi/WSM.h"
#include "xgi/Utils.h"
#include "xgi/Input.h"
#include "xgi/Output.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/BSem.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"


#include "cgicc/HTMLClasses.h"


class emuSTEPManager : public emuSTEPApplication
{

 public:

    XDAQ_INSTANTIATOR();

    emuSTEPManager(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

// SOAP interface
    xoap::MessageReference onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception);
    xoap::MessageReference onStart(xoap::MessageReference message)
		throw (xoap::exception::Exception);
    xoap::MessageReference onShow(xoap::MessageReference message)
		throw (xoap::exception::Exception);
    xoap::MessageReference onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception);
    xoap::MessageReference onReset(xoap::MessageReference message)
		throw (xoap::exception::Exception);
    xoap::MessageReference onSetTTS(xoap::MessageReference message)
		throw (xoap::exception::Exception);

    // HyperDAQ interface
    void webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webStart(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webShow(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webHalt(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webReset(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webSetTTS(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webCalibPC(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
    void webDefault(xgi::Input * in, xgi::Output * out ) 
		throw (xgi::exception::Exception);
	void webChamSel(xgi::Input *in, xgi::Output *out)
        throw (xgi::exception::Exception);

	// work loop call-back functions
    bool configureAction(toolbox::task::WorkLoop *wl);
    bool haltAction(toolbox::task::WorkLoop *wl);
//    bool calibrationAction(toolbox::task::WorkLoop *wl);

    // State transitions
    void configureAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
    void enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
    void disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
    void haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
    void resetAction() throw (toolbox::fsm::exception::Exception);

 protected:

    toolbox::fsm::FiniteStateMachine fsm_;
    xdata::String state_;
    void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception);
    xoap::MessageReference createReply(xoap::MessageReference message)
                        throw (xoap::exception::Exception);
     void fireEvent(std::string event) throw (toolbox::fsm::exception::Exception);

 private:
	toolbox::task::WorkLoop *wl_;
	toolbox::task::ActionSignature *configure_signature_, *halt_signature_;

	xdata::String run_type_; 
	string xmlFileName;
	xdata::UnsignedLong run_number_, new_run_number_;
	long int nevents_, alctpulseamp_;
	bool ddu_in_[16];
	xdata::Vector<xdata::String> config_keys_;
	string error_message_;
	xdata::String endcap_, chamtype_, chamnum_;

	std::string toString(const long int i) const;
	std::string toString(const long int i, int mindig) const;


};

#endif
