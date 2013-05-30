#ifndef __emu_daq_manager_Application_h__
#define __emu_daq_manager_Application_h__

#include <stdint.h>

#include "emu/daq/manager/exception/Exception.h"
#include "emu/daq/manager/Watchdog.h"
#include "emu/daq/manager/AppStates.h"
#include "i2o/i2oDdmLib.h"
#include "i2o/utils/AddressMap.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/Integer64.h"
#include "xdata/Vector.h"
#include "emu/supervisor/RunInfo.h"
#include "emu/base/Supervised.h"
#include "emu/base/WebReporter.h"
#include "emu/base/FactFinder.h"
#include "xdata/ItemEvent.h"

#include "toolbox/task/WorkLoop.h"
#include "toolbox/BSem.h"

#include <string>

using namespace std;

namespace emu { namespace daq { namespace manager {

      class Watchdog;

class Application 
  : public emu::base::Supervised,
    public emu::base::WebReporter,
    public emu::base::FactFinder,
    public xdata::ActionListener
{

  friend class Watchdog;

public:

    /**
     * Define factory method for the instantion of emu::daq::manager::Application applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    Application(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

    /**
     * Invoked when an exception has been received from the sentinel.
     */
    void onException(xcept::Exception &e);

private:

    /**
     * The logger of this application.
     */
    Logger logger_;

  bool inFSMTransition_;

  emu::daq::manager::Watchdog* watchdog_;

    /**
     * Used to access the I2O address map without a function call.
     */
    i2o::utils::AddressMap *i2oAddressMap_;

    /**
     * Used to access the memory pool factory without a function call.
     */
    toolbox::mem::MemoryPoolFactory *poolFactory_;

    /**
     * Used to access the application's info space without a function call.
     */
    xdata::InfoSpace *appInfoSpace_;

    /**
     * Used to access the application's descriptor without a function call.
     */
    xdaq::ApplicationDescriptor *appDescriptor_;

    /**
     * Used to access the application's context without a function call.
     */
    xdaq::ApplicationContext *appContext_;

    /**
     * Used to access the application's zone without a function call.
     */
    xdaq::Zone *zone_;

    /**
     * The XML class name of the application.
     */
    string xmlClass_;

    /**
     * The instance number of the application.
     */
    uint32_t instance_;

    /**
     * The application's URN.
     */
    string urn_;

    /**
     * The application descriptors of all the EVMs (there should only be 1
     * evm!).
     */
    vector< xdaq::ApplicationDescriptor* > evmDescriptors_;

    /**
     * The application descriptors of all the BUs.
     */
    vector< xdaq::ApplicationDescriptor* > buDescriptors_;

    /**
     * The application descriptors of all of the RUs.
     */
    vector< xdaq::ApplicationDescriptor* > ruDescriptors_;

    /**
     * The application descriptors of all of the TAs (there should only be 1
     * TA!).
     */
    vector< xdaq::ApplicationDescriptor* > taDescriptors_;

    /**
     * The application descriptors of all of the RUIs.
     */
    vector< xdaq::ApplicationDescriptor* > ruiDescriptors_;

    /**
     * The application descriptors of all of the FUs.
     */
    vector< xdaq::ApplicationDescriptor* > fuDescriptors_;

    /**
     * True if the test of the RU builder applications has been started, else
     * false.
     */
    bool testConfigured_;
    bool testStarted_;


    /////////////////////////////////////////////////////
    // Beginning of exported parameters for monitoring //
    /////////////////////////////////////////////////////

    /**
     * Exported read-only parameter specifying the current state of the
     * application.
     */
    // (Moved to EmuApplication)   xdata::String stateName_;

    /////////////////////////////////////////////////////
    // End of exported parameters for monitoring       //
    /////////////////////////////////////////////////////


    /**
     * Returns the name to be given to the logger of this application.
     */
    string generateLoggerName();

    /**
     * Get the descriptors of all the applications the emu::daq::manager::Application will
     * need to control.
     */
    void getAllAppDescriptors();

    /**
     * Returns a vector of application descriptors order by instance number.
     */
    vector< xdaq::ApplicationDescriptor* > getAppDescriptors
    (
     xdaq::Zone             *zone,
     const string           appClass
    )
    throw (emu::daq::manager::exception::Exception);

    /**
     * The default web page of the application.
     */
    void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

    /**
     * Web page used to control the RU builder.
     */
    void controlWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

    void webRedirect(xgi::Input *in, xgi::Output *out)
      throw (xgi::exception::Exception);

  vector<emu::base::WebReportItem> materialToReportOnPage1();

  emu::base::Fact           findFact( const emu::base::Component& component, const string& factType );
  emu::base::FactCollection findFacts();


  void governorForm(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  void processGovernorForm(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  void commentWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  void processCommentForm(xgi::Input *in)
    throw (xgi::exception::Exception);
  void commandWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  void printUserComments( xgi::Output *out );
  void stringReplace( string& inThis, const string& This, const string& withThis );
  string textToHtml( const string text );

  xdata::Boolean supervisedMode_; // true if we obey central run control
  xdata::Boolean configuredInSupervisedMode_; // true if this run was configured on central run control's command

  string comments_;        // The comments entered by the user.
  string globalRunNumber_; // The global run number entered by the user.
  bool   badRun_;          // User's judgement on the run.

  emu::supervisor::RunInfo *runInfo_; // communicates with run database

  xdata::String runDbBookingCommand_; // e.g. "java -jar runnumberbooker.jar"
  xdata::String runDbWritingCommand_; // e.g. "java -jar runinfowriter.jar"
  xdata::String runDbAddress_;        // e.g. "dbc:oracle:thin:@oracms.cern.ch:10121:omds"
  xdata::String runDbUserFile_;       // file that contains the username:password for run db user
  void bookRunNumber();
  void writeRunInfo();
  xdata::Boolean isBookedRunNumber_;
  bool abortedRun_; // The run is aborted if it is never started (only configured).

  xdata::UnsignedInteger32 runNumber_;
  xdata::UnsignedInteger32 runSequenceNumber_;
  xdata::Integer64 maxNumberOfEvents_;
  xdata::String runStartTime_;
  xdata::Vector<xdata::String> runTypes_; // all possible run types
  xdata::String runType_; // the current run type
  xdata::Boolean isGlobalInControl_;
  xdata::Boolean writeBadEventsOnly_;
  xdata::Boolean buildEvents_;
  xdata::String daqState_; // the combined state of the DAQ applications
  xdata::UnsignedInteger32 calibRunIndex_;
  xdata::UnsignedInteger32 calibNRuns_;
  xdata::UnsignedInteger32 calibStepIndex_;
  xdata::UnsignedInteger32 calibNSteps_;
  xdata::Vector<xdata::String> dataFileNames_; // all RUIs' all data file names
  xdata::Vector<xdata::String> dataDirNames_; // all RUIs' data directory names
  uint32_t purgeIntNumberString( string* s );

  xdata::String hardwareMapping_; // file of the hardware mapping (path relative to XDAQ_ROOT

  emu::daq::manager::AppStates currentAppStates_;
  emu::daq::manager::AppStates previousAppStates_;

  vector< map< string,string > > getRUIEventCounts();
  vector< map< string,string > > getFUEventCounts();
  void printEventCountsTable( xgi::Output              *out,
			      string                    title,
			      vector< map< string,string > > counts, 
			      bool                      control );
  string getDateTime();
  void   getRunInfoFromTA( string* runnum, string* maxevents, string* starttime, string* stoptime );
  string reformatTime( string time );
  vector< pair<xdaq::ApplicationDescriptor*, string> > daqAppStates_;
  set<string> daqContexts_; // all different DAQ contexts with apps controlled by emu::daq::manager::Application
  void   getDataPaths();	///< get All RUIs' data directories and data file names
  void   createAllAppStates();
  void   queryAppStates();
  string getDAQState();
  void reportCrashedApps();
  void   statesTableToHtml( xgi::Output *out,
			   string title,
			   set<string> &contexts,
			   AppStates &as )
    throw (xgi::exception::Exception);
  void getMnemonicNames();
  map<uint32_t,string> hardwareMnemonics_; // hardwareMnemonics[EmuRUI_instance]
  void printDAQState( xgi::Output *out, string state );

  virtual void actionPerformed(xdata::Event & received ); // inherited from xdata::ActionListener
  void configureRestartedApps();
  void enableRestartedApps();

  // For driving the FSM by asynchronous SOAP.
  toolbox::task::WorkLoop *workLoop_;
//   toolbox::BSem bsem_;
  toolbox::task::ActionSignature *configureSignature_;
  toolbox::task::ActionSignature *enableSignature_;
  toolbox::task::ActionSignature *haltSignature_;

  // Parameters to obtain from TTCciControl
  xdata::String TTCci_ClockSource_;
  xdata::String TTCci_OrbitSource_;
  xdata::String TTCci_TriggerSource_;
  xdata::String TTCci_BGOSource_;
  void getTriggerSources();

  void setParametersForSupervisedMode();

  string warningsToDisplay_;

  // STEP-specific stuff
  xdata::UnsignedInteger64 STEPCount_; // The lowest of any RUI's lowest count in a STEP run.
  xdata::Boolean STEPFinished_; // set to TRUE when all DDUs' all live and unmasked inputs have produced the requested number of events
  bool printSTEPCountsTable( stringstream& out, bool control );
  bool isSTEPFinished();
  void maskDDUInputs( const bool in, const std::vector<cgicc::FormEntry>& fev );
  void sendDDUInputMask( const bool in, const uint32_t ruiInstance, const std::set<uint32_t>& inputs );


  // FM-specific stuff
  xdata::String TF_FM_URL_; /// (base) URL (http://host:port) of Track Finder Funtion Manager
  xdata::String CSC_FM_URL_; /// (base) URL (http://host:port) of CSC Funtion Manager
  xdata::String RegexMatchingTFConfigName_; /// regular expression matching the name of the Track Finder configuration
  xdata::String RegexMatchingCSCConfigName_; /// regular expression matching the name of the CSC configuration
  xdata::UnsignedInteger32 TFConfigId_; /// unique id of the Track Finder configuration
  xdata::UnsignedInteger32 CSCConfigId_; /// unique id of the CSC configuration

    /**
     * Processes the form sent from the control web page.
     */
    void processCommandForm(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

    /**
     * Prints hyper-text links to the specified applications.
     */
    void printAppInstanceLinks
    (
        xgi::Output                            *out,
        vector< xdaq::ApplicationDescriptor* > &appDescriptors
    );

    /**
     * Prints a row of HTML tables, where each table displays parameters
     * gathered from a single RU builder application.
     */
    void printParamsTables
    (
        xgi::Input                              *in,
        xgi::Output                             *out,
        vector< xdaq::ApplicationDescriptor* >  &appDescriptors,
        vector< vector< pair<string,string> > > paramsOfApps
    )
    throw (xgi::exception::Exception);

    /**
     * Prints an HTML table displaying parameters gathered from a RU builder
     * application.
     */
    void printParamsTable
    (
        xgi::Input                    *in,
        xgi::Output                   *out,
        xdaq::ApplicationDescriptor   *appDescriptor,
        vector< pair<string,string> > params
    )
    throw (xgi::exception::Exception);

    /**
     * Returns the hyper-text reference of the specified application.
     */
    string getHref(xdaq::ApplicationDescriptor *appDescriptor);

    /**
     * Gets and returns the current event number from the EVM.
     */
    vector< pair<string,string> > getEventNbFromEVM
    (
        xdaq::ApplicationDescriptor *evmDescriptor
    )
    throw (emu::daq::manager::exception::Exception);

    /**
     * Gets and returns the values of statistics parameters exported by the
     * specified applications.
     */
    vector< vector< pair<string,string> > > getStats
    (
        vector< xdaq::ApplicationDescriptor* > &appDescriptors
    );

    /**
     * Get and returns the values of statistics parameters exported by the
     * specified application.
     */
    vector< pair<string,string> > getStats
    (
        xdaq::ApplicationDescriptor *appDescriptor
    );

    /**
     * Exports the parameters.
     */
    void exportParams(xdata::InfoSpace *s);

    void configureDAQ()
    throw (emu::daq::manager::exception::Exception);
    /**
     * Starts the DAQ.
     */
    void startDAQ()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Checks that there is a minimum set of applications to make a RU builder.
     */
    void checkThereIsARuBuilder()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Tell the EVM whether or not it should generate dummy triggers.
     */
    void setEVMGenerateDummyTriggers(const bool value)
    throw (emu::daq::manager::exception::Exception);

    /**
     * Tell the RUs whether or not they should generate dummy super-fragments.
     */
    void setRUsGenerateDummySuperFrags(const bool vale)
    throw (emu::daq::manager::exception::Exception);

    /**
     * Tells the BUs whether or not to drop events.
     */
    void setBUsDropEvents(const bool value)
    throw (emu::daq::manager::exception::Exception);

    /**
     * Configures the imaginary trigger, i.e. the TA.
     */
    void configureTA();

    /**
     * Starts the imaginary trigger, i.e. the TA.
     */
    void enableTA();

    /**
     * Starts just the RU builder, i.e. the BUs, EVM and RUs.
     */
    void configureRuBuilder()
      throw (emu::daq::manager::exception::Exception);
    void startRuBuilder()
      throw (emu::daq::manager::exception::Exception);

    /**
     * Configures a RUI.
     */
    void configureRUI( xdaq::ApplicationDescriptor* ruiDescriptor );

    /**
     * Starts a RUI.
     */
    void enableRUI( xdaq::ApplicationDescriptor* ruiDescriptor );

    /**
     * Starts the imaginary filter farm, i.e. the FUs.
     */
    void configureFilterFarm()
      throw (xcept::Exception);
//     throw (emu::daq::manager::exception::Exception);
    void startFilterFarm()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Stops the test.
     */
    void stopDAQ()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Stops the imaginary FED builder, i.e. the RUIs.
     */
    void stopFedBuilder()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Stops just the RU builder, i.e. BUs, EVM and RUs.
     */
    void stopRuBuilder()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Stops the imaginary trigger, i.e. the TA.
     */
    void stopTrigger()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Stops the imaginary filter farm, i.e. the FUs.
     */
    void stopFilterFarm()
    throw (emu::daq::manager::exception::Exception);

    /**
     * Resets applications.
     */
    void resetApps( vector< xdaq::ApplicationDescriptor* > apps )
    throw (emu::daq::manager::exception::Exception);

    /**
     * Resets EmuRUIs and EmuFUs.
     */
    void resetDAQ()
    throw (emu::daq::manager::exception::Exception);


    /**
     * Prints the specified soap message to standard out.
     */
    void printSoapMsgToStdOut(xoap::MessageReference message);

  // Supervisor-specific stuff:
public:
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
  xoap::MessageReference onQueryRunSummary(xoap::MessageReference message)
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
  
  void reConfigureAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);
  void noAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);
  void resetAction()
    throw (toolbox::fsm::exception::Exception);

  bool configureActionInWorkLoop(toolbox::task::WorkLoop *wl);
  bool enableActionInWorkLoop(toolbox::task::WorkLoop *wl);
  bool haltActionInWorkLoop(toolbox::task::WorkLoop *wl);

private:
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception);
  void timeExpired(toolbox::task::TimerEvent& e);

};

}}} // namespace emu::daq::manager

#endif
