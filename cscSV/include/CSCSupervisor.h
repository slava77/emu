#ifndef __CSC_SUPERVISOR_H__
#define __CSC_SUPERVISOR_H__

#include "EmuApplication.h"

#include <string>
#include <deque>
#include <map>

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/BSem.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/Integer.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger.h"
#include "xgi/Method.h"
#include "EmuRunInfo.h"
#include "xdaq2rc/RcmsStateNotifier.h"

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
	void webSwitchTTS(xgi::Input *in, xgi::Output *out)
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
	class CalibParam
	{
	public:
		void registerFields(xdata::Bag<CalibParam> *bag);

		xdata::String key_;
		xdata::String command_;
		xdata::UnsignedInteger loop_;
		xdata::UnsignedInteger delay_;
		xdata::String ltc_;
	};

        xdaq::ApplicationDescriptor* appDescriptor_;
        Logger logger_;

	xdata::String run_type_;
	xdata::UnsignedLong run_number_;
	xdata::UnsignedLong runSequenceNumber_;

	xdata::Vector<xdata::String> config_keys_;
	xdata::Vector<xdata::Bag<CalibParam> > calib_params_;
	xdata::Vector<xdata::String> pc_keys_;
	xdata::Vector<xdata::String> pc_configs_;
	xdata::Vector<xdata::String> fc_keys_;
	xdata::Vector<xdata::String> fc_configs_;

	xdata::String daq_mode_;
	xdata::String trigger_config_;
	xdata::String ttc_source_;

	xdata::UnsignedInteger tts_id_;
	xdata::UnsignedInteger tts_bits_;
        xdaq2rc::RcmsStateNotifier rcmsStateNotifier_;
private:
	toolbox::task::WorkLoop *wl_;
	toolbox::BSem wl_semaphore_;
	toolbox::task::ActionSignature *configure_signature_, *halt_signature_;
	toolbox::task::ActionSignature *calibration_signature_;
	bool quit_calibration_;
	std::map<string, string> start_attr, stop_attr;

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
	xoap::MessageReference createParameterGetSOAP(
			string klass, map<string, string> name_type);
	void analyzeReply(
			xoap::MessageReference message, xoap::MessageReference reply,
			xdaq::ApplicationDescriptor *app);
	string extractParameter(xoap::MessageReference message, string name);
	void refreshConfigParameters();

	string getCGIParameter(xgi::Input *in, string name);
	int keyToIndex(const string key);

	string getCrateConfig(const string type, const string key) const;
	bool isCalibrationMode();
	int getCalibParamIndex(const string name);

	string trim(string orig) const;
	string toString(const long int i) const;

	xdaq::ApplicationDescriptor *daq_descr_, *tf_descr_, *ttc_descr_;
	xoap::MessageReference daq_param_, tf_param_, ttc_param_;
	bool daq_notavailable_;

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
	bool hide_tts_control_;

	xdata::String curlCommand_;         // the curl command's full path
	xdata::String curlCookies_;         // file for cookies
	xdata::String CMSUserFile_;         // file that contains the username:password for CMS user
	xdata::String eLogUserFile_;        // file that contains the username:password:author for eLog user
	xdata::String eLogURL_;             // eLog's URL 
	xdata::Vector<xdata::String> peripheralCrateConfigFiles_; // files to be attached to elog post
	
	EmuRunInfo *runInfo_;               // communicates with run database
	xdata::String runDbBookingCommand_; // e.g. "java -jar runnumberbooker.jar"
	xdata::String runDbWritingCommand_; // e.g. "java -jar runinfowriter.jar"
	xdata::String runDbAddress_;        // e.g. "dbc:oracle:thin:@oracms.cern.ch:10121:omds"
	xdata::String runDbUserFile_;       // file that contains the username:password for run db user
	bool isBookedRunNumber_;
	void bookRunNumber();
	void writeRunInfo( bool toDatabase, bool toELog );
	void postToELog( string subject, string body, vector<string> *attachments );
	vector< vector<string> > getFUEventCounts();
	vector< vector<string> > getRUIEventCounts();
	string reformatTime( string time );
        xoap::MessageReference getRunSummary()
	  throw( xcept::Exception );

	class StateTable
	{
	public:
		StateTable(CSCSupervisor *sv);
		void addApplication(string klass);
		void refresh() ;
		string getState(string klass, unsigned int instance);
		bool isValidState(string expected);
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
		string getTime(void) const;

		unsigned int size_;
		deque<string> messages_;
	} last_log_;
};

#endif  // ifndef __CSC_SUPERVISOR_H__
// vim: set ai sw=4 ts=4:
