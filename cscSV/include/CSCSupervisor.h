#ifndef __CSC_SUPERVISOR_H__
#define __CSC_SUPERVISOR_H__

#include "EmuApplication.h"

#include <string>
#include <deque>
#include <map>

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoop.h"
#include "BSem.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xgi/Method.h"

class CSCSupervisor : public EmuApplication
{
	class StateTable;
	friend class StateTable;

public:
	XDAQ_INSTANTIATOR();

	CSCSupervisor(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onReset(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onSetTTS(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// HyperDAQ interface
	void webDefault(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webConfigure(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webEnable(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webDisable(xgi::Input *in, xgi::Output *out)
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

	// work loop call-back functions
	bool configureAction(toolbox::task::WorkLoop *wl);
	bool haltAction(toolbox::task::WorkLoop *wl);
	bool calibrationAction(toolbox::task::WorkLoop *wl);

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
	void setTTSAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);

private: // XDAQ parameters
	xdata::String run_type_;
	xdata::UnsignedLong run_number_;

	xdata::Vector<xdata::String> config_keys_;
	xdata::Vector<xdata::String> pc_keys_;
	xdata::Vector<xdata::String> pc_configs_;
	xdata::Vector<xdata::String> fc_keys_;
	xdata::Vector<xdata::String> fc_configs_;

	xdata::String daq_mode_;
	xdata::String trigger_config_;
	xdata::String ttc_source_;

	xdata::String tts_crate_;
	xdata::String tts_slot_;
	xdata::String tts_bits_;

private:
	toolbox::task::WorkLoop *wl_;
	BSem wl_semaphore_;
	toolbox::task::ActionSignature *configure_signature_, *halt_signature_;
	toolbox::task::ActionSignature *calibration_signature_;

	void submit(toolbox::task::ActionSignature *signature);

	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

	void sendCommand(string command, string klass)
			throw (xoap::exception::Exception, xdaq::exception::Exception);
	void sendCommand(string command, string klass, int instance)
			throw (xoap::exception::Exception, xdaq::exception::Exception);
	void sendCommandWithAttr(
			string command, map<string, string> attr, string klass)
			throw (xoap::exception::Exception, xdaq::exception::Exception);

	xoap::MessageReference createCommandSOAP(string command);
	xoap::MessageReference createCommandSOAPWithAttr(
			string command, map<string, string> attr);

	void setParameter(string klass, string name, string type, string value);
	xoap::MessageReference createParameterSetSOAP(
	        string klass, string name, string type, string value);
	xoap::MessageReference createParameterGetSOAP(
	        string klass, string name, string type);
	xoap::MessageReference createParameterGetSOAP2(
	        string klass, int length, string names[], string types[]);
	void analyzeReply(
			xoap::MessageReference message, xoap::MessageReference reply,
			xdaq::ApplicationDescriptor *app);
	string extractParameter(xoap::MessageReference message, string name);
	void refreshConfigParameters();

	string getCGIParameter(xgi::Input *in, string name);
	int keyToIndex(const string key);

	string getCrateConfig(const string type, const string key) const;
	bool isCalibrationMode();
	string trim(string orig) const;
	string toString(const long int i) const;

	xdaq::ApplicationDescriptor *daq_descr_, *tf_descr_, *ttc_descr_;
	xoap::MessageReference daq_param_, tf_param_, ttc_param_;
	xoap::MessageReference daq_configured_param_, daq_state_param_;

	string getDAQMode();
	string getTFConfig();
	string getTTCciSource();
	bool isDAQConfiguredInGlobal();
	string getLocalDAQState();

	bool isDAQManagerControlled(string command);

	int nevents_;
	unsigned int step_counter_;

	string error_message_;

	bool keep_refresh_;

	class StateTable
	{
	public:
		void addApplication(CSCSupervisor *sv, string klass);
		void refresh() ;
		string getState(string klass, unsigned int instance);
		void webOutput(xgi::Output *out, string sv_state)
				throw (xgi::exception::Exception);

	private:
		xoap::MessageReference createStateSOAP(string klass);
		string extractState(xoap::MessageReference message, string klass);

		CSCSupervisor *sv_;
		vector<pair<xdaq::ApplicationDescriptor *, string> > table_;
	} state_table_;

	class LastLog
	{
	public:
		void size(unsigned int size);
		unsigned int size() const;
		void add(string message);
		void webOutput(xgi::Output *out) throw (xgi::exception::Exception);

	private:
		unsigned int size_;
		deque<string> messages_;
	} last_log_;
};

#endif  // ifndef __CSC_SUPERVISOR_H__
// vim: set ai sw=4 ts=4:
