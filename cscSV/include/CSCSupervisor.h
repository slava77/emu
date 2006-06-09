#ifndef __CSC_SUPERVISOR_H__
#define __CSC_SUPERVISOR_H__

#include "EmuApplication.h"

#include <string>
#include <deque>
#include <utility>  // pair

#include "toolbox/fsm/FiniteStateMachine.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
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
	void webRedirect(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);

private: // XDAQ parameters
	xdata::Vector<xdata::String> config_keys_;
	xdata::Vector<xdata::String> config_modes_;
	xdata::Vector<xdata::String> modes_pc_;
	xdata::Vector<xdata::String> files_pc_;
	xdata::Vector<xdata::String> modes_fc_;
	xdata::Vector<xdata::String> files_fc_;

private:
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

	void sendCommand(string command, string klass);
	xoap::MessageReference createCommandSOAP(string command);
	void setParameter(string klass, string name, string type, string value);
	xoap::MessageReference createParameterSetSOAP(
	        string klass, string name, string type, string value);
	void analyzeReply(
			xoap::MessageReference message, xoap::MessageReference reply,
			xdaq::ApplicationDescriptor *app);

	string getRunmode(xgi::Input *in);
	string getRunNumber(xgi::Input *in);
	string getNEvents(xgi::Input *in);
	int modeToIndex(string mode);

	string getConfigFilename(string type, string mode) const;
	string trim(string orig) const;

	string runmode_;
	string runnumber_;
	string nevents_;

	string error_message_;

	class StateTable
	{
	public:
		void addApplication(CSCSupervisor *sv, string klass);
		void refresh() ;
		void webOutput(xgi::Output *out) throw (xgi::exception::Exception);

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
