#ifndef __emu_daq_rui_Application_h__
#define __emu_daq_rui_Application_h__

#include "emu/daq/rui/exception/Exception.h"
#include "i2o/i2oDdmLib.h"
#include "i2o/utils/AddressMap.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/task/Timer.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Double.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/Integer64.h"
#include "xdata/Vector.h"
#include "xdata/ItemEvent.h"


#include "emu/daq/reader/Base.h"
#include "emu/daq/server/Base.h"
#include "emu/daq/server/i2oMsg.h"
#include "emu/daq/writer/RawDataFile.h"
#include "emu/daq/writer/RateLimiter.h"
#include "emu/daq/rui/i2oFirstEventNumberMsg.h"
#include "emu/daq/rui/STEPEventCounter.h"
#include "emu/daq/rui/BadEventCount.h"
#include "emu/daq/rui/EventBufferRing.h"
#include "emu/daq/rui/LogMessageCounter.h"
#include "emu/base/FactFinder.h"

using namespace std;

namespace emu { namespace daq { namespace rui {

class Application :
    public emu::base::FactFinder,
    public xdata::ActionListener
{
public:

    /**
     * Define factory method for the instantion of EmuRUI applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    Application(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);


private:
  virtual void actionPerformed(xdata::Event & received ); // inherited from xdata::ActionListener

  bool serverLoopAction(toolbox::task::WorkLoop *wl);

  uint32_t runStartUTC_; /// Unix UTC run start time 

  static const uint32_t maxClients_ = 5; // max possible number of clients

  xdata::Vector<xdata::String>            clientName_;
  xdata::Vector<xdata::UnsignedInteger32> clientInstance_;
  xdata::Vector<xdata::Boolean>           clientPersists_; // whether its server needs to be (re)created on config
  xdata::Vector<xdata::String>            clientProtocol_;
  xdata::Vector<xdata::UnsignedInteger64> clientPoolSize_;
  xdata::Vector<xdata::UnsignedInteger64> prescaling_;
  xdata::Vector<xdata::Boolean>           onRequest_;
  xdata::Vector<xdata::UnsignedInteger64> creditsHeld_;
  struct Client {
    xdata::String                  *name;
    xdata::UnsignedInteger32       *instance;
    xdata::Boolean                 *persists;
    xdata::UnsignedInteger64       *poolSize;
    xdata::UnsignedInteger64       *prescaling;
    xdata::Boolean                 *onRequest;
    xdata::UnsignedInteger64       *creditsHeld;
    emu::daq::server::Base         *server;
    toolbox::task::WorkLoop        *workLoop;
    string                          workLoopName;
    bool                            workLoopStarted;
    toolbox::task::ActionSignature *workLoopActionSignature;
    Client( xdata::Serializable*            n=NULL,
	    xdata::Serializable*            i=NULL,
	    xdata::Serializable*            e=NULL,
	    xdata::Serializable*            s=NULL,
	    xdata::Serializable*            p=NULL,
	    xdata::Serializable*            r=NULL,
	    xdata::Serializable*            c=NULL,
	    emu::daq::server::Base*         S=NULL,
            toolbox::task::WorkLoop*        wl =NULL,
	    string                          wln="",
	    bool                            wls=false,
	    toolbox::task::ActionSignature* wla=NULL   ){
      name                    = dynamic_cast<xdata::String*>           ( n );
      instance                = dynamic_cast<xdata::UnsignedInteger32*>( i );
      persists                = dynamic_cast<xdata::Boolean*>          ( e );
      poolSize                = dynamic_cast<xdata::UnsignedInteger64*>( s );
      prescaling              = dynamic_cast<xdata::UnsignedInteger64*>( p );
      onRequest               = dynamic_cast<xdata::Boolean*>          ( r );
      creditsHeld             = dynamic_cast<xdata::UnsignedInteger64*>( c );
      server                  = S;
      workLoop                = wl;
      workLoopName            = wln;
      workLoopStarted         = wls;
      workLoopActionSignature = wla;
    }
  };
  std::vector<Client*> clients_;

  emu::daq::reader::Base*             deviceReader_;	     // device reader
  xdata::String                       inputDeviceName_;      // input device name (file path or board number)
  xdata::String                       hardwareMnemonic_;     // mnemonic name for the piece of hardware to read
  xdata::String                       inputDeviceType_;      // spy, slink or file
  xdata::String                       inputDataFormat_;      // "DDU" or "DCC"
  uint32_t                            inputDataFormatInt_;   // emu::daq::reader::Base::DDU or emu::daq::reader::Base::DCC
  uint64_t                            previousEventNumber_;

  void createFileWriter();
  void destroyFileWriter();
  void createDeviceReader();
  void destroyDeviceReader();
  void createServers();
  void destroyServers();
  bool createI2OServer( string clientName, uint32_t clientInstance );
  bool createSOAPServer( string clientName, uint32_t clientInstance, bool persistent=true );
  void onI2OClientCreditMsg(toolbox::mem::Reference *bufRef);
  xoap::MessageReference onSOAPClientCreditMsg( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference processSOAPClientCreditMsg( xoap::MessageReference msg );
  void addDataForClients(const uint32_t runNumber, 
			 const uint32_t runStartUTC,
			 const uint64_t nEventsRead,
			 const emu::daq::server::PositionInEvent_t position, 
			 const uint16_t errorFlag, 
			 char* const data, 
			 const size_t   dataLength );
  void makeClientsLastBlockEndEvent();
  void insertEmptySuperFragments( const uint64_t fromEventNumber, const uint64_t toEventNumber )
    throw (emu::daq::rui::exception::Exception);
  void ensureContiguousEventNumber();
  void moveToFailedState( const string reason );

  xdata::UnsignedInteger64                 nEventsRead_;
  xdata::UnsignedInteger64                 nReadingPasses_; ///< Total number of reading passes so far.
  xdata::UnsignedInteger64                 maxNBlocksInEvent_; ///< Max number of reading passes (blocks) in any event so far.
  xdata::UnsignedInteger64                 nEventsOfMultipleBlocks_;	///< Number of events read out in more than one pass (and thus put in multiple blocks).
  xdata::String                       persistentDDUError_;
  emu::daq::writer::RawDataFile       *fileWriter_;
  xdata::Vector<xdata::String>        dataFileNames_;
  uint64_t                            nReadingPassesInEvent_;
  bool                                insideEvent_;
  uint16_t                            errorFlag_;
  bool                                ableToWriteToDisk_;
  void updateDataFileNames();

  // file writing rate limiter
  emu::daq::writer::RateLimiter       *rateLimiter_;
  xdata::UnsignedInteger64            fileWritingRateLimitInHz_;
  xdata::UnsignedInteger64            fileWritingRateSampleSize_;
  xdata::Boolean                      fileWritingVetoed_;

  // managing bad events and their context
  xdata::Boolean                      writeBadEventsOnly_; ///< If TRUE, only the bad events are to be written to files, with a limited context.
  xdata::UnsignedInteger64            nToWriteBeforeBadEvent_; ///< Number of events to write as bad event's leading context.
  xdata::UnsignedInteger64            nToWriteAfterBadEvent_; ///< Number of events to write as bad event's trailing context.
  bool                                isBadEvent_; ///< This event is bad.
  uint64_t                            countSinceBadEvent_; ///< Number of events read so far since the last bad event.
  emu::daq::rui::EventBufferRing      eventBufferRing_; ///< Buffer to store the bad event and its leading context.
  emu::daq::rui::BadEventCount        badEventCount_; ///< Bad event count with progressive prescaler

  // In STEP runs, count on each DDU input the number of events it's contributed to with data
  bool                                isSTEPRun_;
  emu::daq::rui::STEPEventCounter     STEPEventCounter_;
  xoap::MessageReference onSTEPQuery( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference onExcludeDDUInputs( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference onIncludeDDUInputs( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference maskDDUInputs( const bool in, const xoap::MessageReference msg )
    throw (xoap::exception::Exception);

  bool hasHeader( char* const data, const size_t dataLength );
  bool hasTrailer( char* const data, const size_t dataLength );
  bool interestingDDUErrorBitPattern(char* const data, const size_t dataLength);

  void printData(std::ostream& os, char* data, const size_t dataLength);
  void writeDataToFile( const char* const data, const size_t dataLength, const bool newEvent=false );
  void writeDataWithContextToFile(  char* const data, const size_t dataLength, const bool newEvent );

  void printBlocks( deque<toolbox::mem::Reference*> d );
  void fillBlock(toolbox::mem::Reference *bufRef,
		 const char*              data,
		 const size_t             dataLength,
		 const uint64_t           eventNumber 
		 )
    throw (emu::daq::rui::exception::Exception);


  toolbox::mem::Pool *ruiTaPool_;
  I2O_TID emuTATid_;
  vector< xdaq::ApplicationDescriptor* > taDescriptors_;
  void getTidOfEmuTA()
    throw ( xcept::Exception );
  void sendEventNumberToTA( uint64_t firstEventNumber )
    throw ( xcept::Exception );

  vector< xdaq::ApplicationDescriptor* > getAppDescriptors(xdaq::Zone *zone,
							   const string            appClass)
    throw (emu::daq::rui::exception::Exception);

  time_t toUnixTime( const std::string YYMMDD_hhmmss_UTC );

  void getRunInfo() throw (emu::daq::rui::exception::Exception);

  emu::base::Fact           findFact( const emu::base::Component& component, const string& factType );
  emu::base::FactCollection findFacts();

    /**
     * Pointer to the descriptor of the RUBuilderTester application.
     *
     * It is normal for this pointer to be 0 if the RUBuilderTester application      * cannot be found.
     */
    xdaq::ApplicationDescriptor *rubuilderTesterDescriptor_;

    /**
     * I2o exception handler.
     */
    toolbox::exception::HandlerSignature *i2oExceptionHandler_;

    /**
     * The logger of this application.
     */
    Logger logger_;

  /// 
  /// Counter and progressive prescaler of log messages.
  ///
  /// @return 
  ///
    LogMessageCounter logMessageCounter_;

    /**
     * Binary semaphore used to protect the internal data structures of the
     * application from multithreaded access.
     */
    toolbox::BSem applicationBSem_;

    /**
     * The name of the info space that contains exported parameters used for
     * monitoring.
     */
    string monitoringInfoSpaceName_;

    /**
     * Info space that contains exported parameters used for monitoring.
     */
    xdata::InfoSpace *monitoringInfoSpace_;

    /**
     * The finite state machine of the application.
     */
    toolbox::fsm::FiniteStateMachine fsm_;

    /**
     * USed to access the work loop factory without a function call.
     */
    toolbox::task::WorkLoopFactory *workLoopFactory_;

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
     * The I2O tid of the application.
     */
    I2O_TID tid_;

    /**
     * The application descriptor of the RU to which the EmuRUI pushes
     * super-fragments.
     */
    xdaq::ApplicationDescriptor *ruDescriptor_;

    /**
     * The I2O tid of the RU to which the EmuRUI pushes
     * super-fragments.
     */
    I2O_TID ruTid_;

    /**
     * The action signature of the loop that generates dummy super-fragments.
     */
    toolbox::task::ActionSignature *workLoopActionSignature_;

    /**
     * The work loop that executes the loop that generates dummy
     * super-fragments.
     */
    toolbox::task::WorkLoop *workLoop_;

    /**
     * True if the work loop that executes the loop that generates dummy
     * super-fragments has been started, else false.
     */
    bool workLoopStarted_;

    /**
     * Name of the memory pool used between the EmuRUI and RU.
     */
    string ruiRuPoolName_;

    /**
     * Memory pool used between the EmuRUI and RU.
     */
    toolbox::mem::Pool *ruiRuPool_;

    /**
     * The application's standard configuration parameters.
     */
    vector< pair<string, xdata::Serializable *> > stdConfigParams_;

    /**
     * The application's standard monitoring parameters.
     */
    vector< pair<string, xdata::Serializable *> > stdMonitorParams_;


    /////////////////////////////////////////////////////////////
    // Beginning of exported parameters used for configuration //
    /////////////////////////////////////////////////////////////

    /**
     * Exported read/write parameter specifying the name of the work loop that
     * executes the behaviour of the EmuRUI.
     *
     * If this string is "" when the EmuRUI is configured, then the EmuRUI will
     * name and create its own work loop.
     */
    xdata::String workLoopName_;

    /**
     * Exported read/write parameter specifying the size of an event data I2O
     * message frame.
     */
    xdata::UnsignedInteger64 dataBufSize_;

    /**
     * Exported read/write parameter specifiying the size of payload per FED
     * per event.
     *
     * fedPayloadSize is in bytes.
     */
    xdata::UnsignedInteger64 fedPayloadSize_;

    /**
     * Exported read/write parameter specifiying the source id of each FED
     * supplying data to a single super-fragment.
     *
     * The size of this vector specifies the number of FEDs that supply data
     * to a single super-fragment.
     */
    xdata::Vector<xdata::UnsignedInteger64> fedSourceIds_;

    /**
     * Exported read/write parameter specifying the threshold of the event data
     * pool between the EmuRUI and RU.
     */
    xdata::UnsignedInteger64 threshold_;

    xdata::String       pathToDataOutFile_;    // the path to the file to write the data into (no file written if "")
    xdata::UnsignedInteger64 fileSizeInMegaBytes_;  // when the file size exceeds this, no more events will be written to it (no file written if <=0)
    xdata::Integer64    maxEvents_;            // stop reading from DDU after this many events
    xdata::Boolean      passDataOnToRUBuilder_;// it true, data is sent to the event builder
    xdata::UnsignedInteger32 runNumber_;            // run number to be obtained from TA
    xdata::Boolean      isBookedRunNumber_;    // whether or not this run number was booked in the database
    xdata::String       runStartTime_;         // run start time to be included in the file name
    xdata::String       runStopTime_;          // run stop time to be included in the metafile
    xdata::String       runType_;              // run type to be included in the file name
    xdata::String       reasonForFailure_;     // the reason for going into Failed state

    /////////////////////////////////////////////////////////////
    // End of exported parameters used for configuration       //
    /////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////
    // Beginning of exported parameters used for monitoring    //
    /////////////////////////////////////////////////////////////

    /**
     * Exported read-only parameter specifying the current state of the
     * application.
     */
    xdata::String stateName_;

    /**
     * Exported read-only parameter specifying the event number of the next
     * dummy super-fragment to be created.
     */
    xdata::UnsignedInteger64 eventNumber_;

    /////////////////////////////////////////////////////////////
    // End of exported parameters used for monitoring          //
    /////////////////////////////////////////////////////////////

    /**
     * The blocks of the super-fragment under construction.
     */
/*     vector<toolbox::mem::Reference*> superFragBlocks_; */
    deque<toolbox::mem::Reference*> superFragBlocks_; // BK

    /**
     * True if there are one or more super-fragment blocks waiting to be sent
     * to the RU.
     */
    bool blocksArePendingTransmission_;

    /**
     * Returns the name to be given to the logger of this application.
     */
    string generateLoggerName();

    /**
     * Returns a pointer to the descriptor of the RUBuilderTester application,
     * or 0 if the application cannot be found, which will be the case when
     * testing is not being performed.
     */
    xdaq::ApplicationDescriptor *getRUBuilderTester
    (
     xdaq::Zone *zone
    );

    /**
     * Defines the finite state machine of the application.
     */
    void defineFsm()
    throw (emu::daq::rui::exception::Exception);

    /**
     * Initialises and returns the application's standard configuration
     * parameters.
     */
    vector< pair<string, xdata::Serializable*> > initAndGetStdConfigParams();

    /**
     * Initialises and returns the application's standard monitoring
     * parameters.
     */
    vector< pair<string, xdata::Serializable*> > initAndGetStdMonitorParams();

    /**
     * Puts the specified parameters into the specified info space.
     */
    void putParamsIntoInfoSpace
    (
        vector< pair<string, xdata::Serializable*> > &params,
        xdata::InfoSpace                             *s
    );

  /// 
  /// Attaches listeners to some infospace members
  ///
    void attachListeners();

    /**
     * Binds the SOAP callbacks required to implement the finite state machine
     * of the application.
     */
    void bindFsmSoapCallbacks();

    /**
     * Binds the I2O callbacks of the application.
     */
    void bindI2oCallbacks();

    /**
     * Creates the CSS file for this application.
     */
    void css
    (
        xgi::Input  *in,
        xgi::Output *out
    )
    throw (xgi::exception::Exception);

    /**
     * The default web page of the application.
     */
    void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

    /**
     * Prints the specified parameters as an HTML table with the specified name.
     */
    void printParamsTable
    (
        xgi::Input                                   *in,
        xgi::Output                                  *out,
        const string                                 name,
        vector< pair<string, xdata::Serializable*> > &params
    )
    throw (xgi::exception::Exception);

    /**
     * SOAP Callback used to process an Fsm SOAP command.
     */
    xoap::MessageReference onConfigure(xoap::MessageReference msg)
      throw (xoap::exception::Exception);

    xoap::MessageReference onEnable(xoap::MessageReference msg)
      throw (xoap::exception::Exception);

    xoap::MessageReference onHalt(xoap::MessageReference msg)
      throw (xoap::exception::Exception);

    xoap::MessageReference onReset(xoap::MessageReference msg)
      throw (xoap::exception::Exception);

    xoap::MessageReference onTerminate(xoap::MessageReference msg);

    void timeExpired(toolbox::task::TimerEvent& e);

    /**
     * Extracts the name of the command represented by the specified SOAP
     * message.
     */
    // string extractCmdNameFromSoapMsg(xoap::MessageReference msg)
    // throw (emu::daq::rui::exception::Exception);

    /**
     * Creates a SOAP response message to a state machine state change request.
     */
    // xoap::MessageReference createFsmResponseMsg
    // (
    //     const string cmd,
    //     const string state
    // )
    // throw (emu::daq::rui::exception::Exception);

    /**
     * Processes the specified command for the finite state machine.
     */
    void processFsmCommand(const string cmdName)
    throw (emu::daq::rui::exception::Exception);

    void startATCP()
      throw (emu::daq::rui::exception::Exception);

    void noAction(toolbox::Event::Reference e)
      throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Halted->Ready transition.
     */
    void configureAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Ready->Enabled transition.
     */
    void enableAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * ANY STATE->Halted transition.
     */
    void haltAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * ANY STATE->Failed transition.
     */
    void failAction(toolbox::Event::Reference event)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback invoked when the state machine of the application has changed.
     */
    void stateChanged(toolbox::fsm::FiniteStateMachine & fsm)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Returns the name of the memory pool between the EmuRUI and RU.
     */
    string createRuiRuPoolName(const uint32_t emuRUIInstance);

    /**
     * Returns a "HeapAllocator" memory pool with the specified name.
     */
    toolbox::mem::Pool *createHeapAllocatorMemoryPool
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        const string                     poolName
    )
    throw (emu::daq::rui::exception::Exception);

    /**
     * Entry point for the work loop responsible for generating dummy
     * super-fragments.
     */
    bool workLoopAction(toolbox::task::WorkLoop *wl);

    /**
     * The behaviour of the EmuRUI when it is enabled.
     */
    int32_t processAndCommunicate();

    /**
     * Tries to send the next super-fragment block to the RU.
     *
     * Failure may occur if there is back pressure from the EmuRUI to RU peer
     * transport.
     */
    void sendNextPendingBlock()
    throw (emu::daq::rui::exception::Exception);

    /**
     * Continues the construction of the dummy super-fragment.
     */
//     void continueConstructionOfSuperFrag()
    int32_t continueConstructionOfSuperFrag()
    throw (emu::daq::rui::exception::Exception);

    int32_t continueSTEPRun()
    throw (emu::daq::rui::exception::Exception);

    /**
     * Appends a new block from the EmuRUI/RU memory pool to the super-fragment
     * under contruction.
     */
//     void appendNewBlockToSuperFrag()
    void appendNewBlockToSuperFrag( const char* data, const size_t dataLength, const uint64_t eventNumber )
    throw (emu::daq::rui::exception::Exception);

    /**
     * Fills each of the super-fragment's blocks with the total number of
     * blocks making up the super-fragment.
     */
//     void setNbBlocksInSuperFragment(const unsigned int nbBlocks);
    void finalizeSuperFragment();

    /**
     * Returns the hyper-text reference of the specified application.
     */
    string getHref(xdaq::ApplicationDescriptor *appDescriptor);

    /**
     * Invoked when there is an I2O exception.
     */
    bool onI2oException(xcept::Exception &exception, void *context);

    /**
     * Creates and returns the error message of an I2O exception by specifying
     * the source and destination involved.
     */
    string createI2oErrorMsg
    (
        xdaq::ApplicationDescriptor *source,
        xdaq::ApplicationDescriptor *destination
    );

  // void getParameters( xdaq::ApplicationDescriptor *target, emu::soap::Parameters &parameters );
};

}}} // namespace emu::daq::rui

#endif
