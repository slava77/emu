#ifndef __emu_supervisor_Application_h__
#define __emu_supervisor_Application_h__

#include "emu/base/Supervised.h"

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
#include "emu/supervisor/RunInfo.h"
#include "xdaq2rc/RcmsStateNotifier.h"

namespace emu {
  namespace supervisor {

class Application : public emu::base::Supervised
{
public:

  class StateTable;
  friend class emu::supervisor::Application::StateTable;
  friend ostream& operator<<( ostream& os, emu::supervisor::Application::StateTable& table );

	XDAQ_INSTANTIATOR();

	Application(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onStart(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onStop(xoap::MessageReference message)
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
	void webStart(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webStop(xgi::Input *in, xgi::Output *out)
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
	bool startAction(toolbox::task::WorkLoop *wl);
	bool calibrationAction(toolbox::task::WorkLoop *wl);

	// State transitions
	void configureAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
  //void configuredAction(toolbox::Event::Reference e) 
  //	throw (toolbox::fsm::exception::Exception);
	void startAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void stopAction(toolbox::Event::Reference e) 
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



	toolbox::task::WorkLoop *wl_;
	toolbox::BSem wl_semaphore_;
	toolbox::task::ActionSignature *configure_signature_, *halt_signature_, *start_signature_;
	toolbox::task::ActionSignature *calibration_signature_;
	bool quit_calibration_;
	std::map<string, string> start_attr, stop_attr;

	void submit(toolbox::task::ActionSignature *signature);

	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);
        void transitionFailed(toolbox::Event::Reference event)
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
  //////////////////////////////////////////////////////////////
  std::string getCommand(xoap::MessageReference msg);
  std::string getNameSpace(xoap::MessageReference msg);
  bool isAsync(xoap::MessageReference msg);
  std::string getCid(xoap::MessageReference msg);
  std::string getSid(xoap::MessageReference msg);
  std::string getOpid(xoap::MessageReference msg);
  std::string getOperation(xoap::MessageReference msg);
  std::string getCallbackFun(xoap::MessageReference msg);
  std::string getCallbackUrl(xoap::MessageReference msg);
  std::string getCallbackUrn(xoap::MessageReference msg);
  xdata::Serializable* getXdaqParameter(xoap::MessageReference msg);
  xdata::Serializable* analyse(DOMNode* com);
  xdata::Serializable* analyseSoapBag(DOMNode* com);
  xdata::Serializable* analyseSoapVector(DOMNode* com);
  xdata::Serializable* getPayload(xoap::MessageReference msg); 
  //methods used in CellOpSendCom object
  std::string getOpComName(xoap::MessageReference msg);
  std::map<std::string, xdata::Serializable*> getOpComParamList(xoap::MessageReference msg);
  //methods used in CellOpInit
  std::string getInitOpName(xoap::MessageReference msg_);
  std::string getInitOpId(xoap::MessageReference msg_);

  void sendCommandCellOpInit(string klass, int instance)
    throw (xoap::exception::Exception, xdaq::exception::Exception);
 
 void sendCommandCell(string command, string klass, int instance)
    throw (xoap::exception::Exception, xdaq::exception::Exception);

  std::string OpGetStateCell(string klass, int instance)
    throw (xoap::exception::Exception, xdaq::exception::Exception);

  void OpResetCell(string klass, int instance)
    throw (xoap::exception::Exception, xdaq::exception::Exception);

  void sendCommandCellOpkill(string klass, int instance)
    throw (xoap::exception::Exception, xdaq::exception::Exception);

  xoap::MessageReference doSoapOpInit(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, std::map<std::string,xdata::Serializable*> param, const std::string& cb,const std::string& url,const std::string& urn, const std::string& opId="");
 
  xoap::MessageReference doSoapOpSendComand(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& opid, const std::string& command, std::map<std::string,xdata::Serializable*> param,  const std::string& cb,const std::string& url,const std::string& urn);

  xoap::MessageReference doSoapOpGetState(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& opid, const std::string& cb,const std::string& url,const std::string& urn);

  xoap::MessageReference doSoapOpKill(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, const std::string& cb,const std::string& url,const std::string& urn); 

 xoap::MessageReference doSoapOpReset(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, const std::string& cb,const std::string& url,const std::string& urn);
  //////////////////////////////////////////////////////////////
	string getDAQMode();
	string getTFConfig();
	string getTTCciSource();
	bool isDAQConfiguredInGlobal();
	string getLocalDAQState();

	bool isDAQManagerControlled(string command);

        bool waitForDAQToExecute( const string command, const unsigned int seconds, const bool poll = false );

	int nevents_;
	unsigned int step_counter_;

	string error_message_;

	bool keep_refresh_;
	bool hide_tts_control_;

	xdata::String curlHost_;            // host on which to execute the curl command
	xdata::String curlCommand_;         // the curl command's full path
	xdata::String curlCookies_;         // file for cookies
	xdata::String CMSUserFile_;         // file that contains the username:password for CMS user
	xdata::String eLogUserFile_;        // file that contains the username:password:author for eLog user
	xdata::String eLogURL_;             // eLog's URL 
	xdata::Vector<xdata::String> peripheralCrateConfigFiles_; // files to be attached to elog post
	
	emu::supervisor::RunInfo *runInfo_;         // communicates with run database
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
	  friend ostream& emu::supervisor::operator<<( ostream& os, emu::supervisor::Application::StateTable& table );
	public:
		StateTable(Application *app);
		void addApplication(string klass);
		void refresh() ;
		string getState(string klass, unsigned int instance);
		bool isValidState(string expected);
		void webOutput(xgi::Output *out, string sv_state)
				throw (xgi::exception::Exception);
	        Application* getApplication(){ return app_; }
	        vector<pair<xdaq::ApplicationDescriptor *, string> >* getTable(){ return &table_; }

	private:
		xoap::MessageReference createStateSOAP(string klass);
		string extractState(xoap::MessageReference message, string klass);

		Application *app_;
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

}} // namespace emu::supervisor

#endif

