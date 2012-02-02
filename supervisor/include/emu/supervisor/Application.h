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
#include "xdata/Integer64.h"
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

	XDAQ_INSTANTIATOR();

        Application(xdaq::ApplicationStub *stub);// throw (xcept::Exception);

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
	xoap::MessageReference onRunSequence(xoap::MessageReference message)
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
	void webRunSequence(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webCalibPC(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webRedirect(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

	// work loop call-back functions
	bool configureAction(toolbox::task::WorkLoop *wl);
	bool stopAction(toolbox::task::WorkLoop *wl);
	bool haltAction(toolbox::task::WorkLoop *wl);
	bool startAction(toolbox::task::WorkLoop *wl);
	bool calibrationAction(toolbox::task::WorkLoop *wl);
	bool calibrationSequencer(toolbox::task::WorkLoop *wl);

	// State transitions
	void configureAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void startAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void stopAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void resetAction() throw (toolbox::fsm::exception::Exception);
	void setTTSAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void noAction(toolbox::Event::Reference e) 
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
		xdata::String ttcci_;
	};

        xdaq::ApplicationDescriptor* appDescriptor_;
        Logger logger_;

        xdata::Boolean isInCalibrationSequence_; /// An automatic sequence of calibration runs is being executed.
	xdata::String run_type_;
	xdata::UnsignedInteger run_number_;
	xdata::UnsignedInteger runSequenceNumber_;

	xdata::Vector<xdata::String> config_keys_;
	xdata::Vector<xdata::Bag<CalibParam> > calib_params_;
	xdata::Vector<xdata::String> pc_keys_;
	xdata::Vector<xdata::String> pc_configs_;
	xdata::Vector<xdata::String> fc_keys_;
	xdata::Vector<xdata::String> fc_configs_;

	xdata::String daq_mode_;
	xdata::String ttc_source_;

	xdata::UnsignedInteger tts_id_;
	xdata::UnsignedInteger tts_bits_;
        xdaq2rc::RcmsStateNotifier rcmsStateNotifier_;

	xdata::String          TFCellOpState_;
	xdata::String          TFCellOpName_;
	xdata::String          TFCellClass_;
	xdata::UnsignedInteger TFCellInstance_;

	toolbox::task::WorkLoop *wl_;
	toolbox::BSem wl_semaphore_;
	toolbox::task::ActionSignature *configure_signature_, *stop_signature_, *halt_signature_, *start_signature_;
	toolbox::task::ActionSignature *calibration_signature_;
	toolbox::task::WorkLoop *calib_wl_;
	toolbox::task::ActionSignature *sequencer_signature_;
	bool quit_calibration_;

	void submit(toolbox::task::ActionSignature *signature);

        void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
	  throw (toolbox::fsm::exception::Exception);
        void transitionFailed(toolbox::Event::Reference event)
	  throw (toolbox::fsm::exception::Exception);

        void sendCalibrationStatus( unsigned int iRun, unsigned int nRuns, unsigned int iStep, unsigned int nSteps );

	void refreshConfigParameters();

	string getCGIParameter(xgi::Input *in, string name);
	int keyToIndex(const string key);

	bool isCalibrationMode();
	bool isAlctCalibrationMode();
	int getCalibParamIndex(const string name);

	xdaq::ApplicationDescriptor *daq_descr_, *tf_descr_, *ttc_descr_;

        void getAppDescriptors();

  //////////////////////////////////////////////////////////////

  void sendCommandCellOpInit();
 
  void sendCommandCell(string command);

  std::string OpGetStateCell();

  void OpResetCell();

  void sendCommandCellOpkill();

  //////////////////////////////////////////////////////////////

  bool waitForTFCellOpToReach( const string targetState, const unsigned int seconds );

  bool waitForAppsToReach( const string targetState, const int seconds=-1 );

	string getDAQMode();
	string getTTCciSource();
	string getLocalDAQState();

	bool isDAQManagerControlled(string command);

        bool waitForDAQToExecute( const string command, const unsigned int seconds, const bool poll = false );

	xdata::Integer64 nevents_;
	unsigned int step_counter_;

	string error_message_;

	bool keep_refresh_;
	bool hide_tts_control_;

        xdata::Boolean controlTFCellOp_;

        xdata::String tf_key_;       // Track Finder Key
	
	emu::supervisor::RunInfo *runInfo_;         // communicates with run database
	xdata::String runDbBookingCommand_; // e.g. "java -jar runnumberbooker.jar"
	xdata::String runDbWritingCommand_; // e.g. "java -jar runinfowriter.jar"
	xdata::String runDbAddress_;        // e.g. "dbc:oracle:thin:@oracms.cern.ch:10121:omds"
	xdata::String runDbUserFile_;       // file that contains the username:password for run db user
	bool isBookedRunNumber_;
	void bookRunNumber();
	void writeRunInfo( bool toDatabase );

        bool isCommandFromWeb_; // TRUE if command issued from web interface

	class StateTable
	{
	  friend ostream& operator<<( ostream& os, const emu::supervisor::Application::StateTable& st );

	public:
		StateTable(Application *app);
		void addApplication(string klass);
		void refresh( bool forceRefresh=true );
		string getState(string klass, unsigned int instance) const;
		bool isValidState(string expected) const;
		void webOutput(xgi::Output *out, string sv_state)
				throw (xgi::exception::Exception);
	        Application* getApplication() const { return app_; }
	        const vector<pair<xdaq::ApplicationDescriptor *, string> >* getTable() const { return &table_; }

	private:
		Application *app_;
	        mutable toolbox::BSem bSem_;
	        time_t lastRefreshTime_;
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

    ostream& operator<<( ostream& os, const emu::supervisor::Application::StateTable& st );

}} // namespace emu::supervisor

#endif
