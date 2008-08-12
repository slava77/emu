#ifndef __EmuRUI_h__
#define __EmuRUI_h__

// #include "emuDAQ/emuRUI/include/emuRUI/SuperFragmentGenerator.h"
#include "emuRUI/exception/Exception.h"
#include "i2o/i2oDdmLib.h"
#include "i2o/utils/AddressMap.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Double.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Vector.h"
#include "xdata/Integer.h"

/* // EMu-specific stuff */
#include "emu/emuDAQ/emuUtil/include/EmuFileWriter.h"
// #include "emu/emuDAQ/emuReadout/include/EmuReader.h"
#include "emu/emuDAQ/emuReadout/include/EmuSpyReader.h"
#include "emu/emuDAQ/emuClient/include/i2oEmuClientMsg.h"
#include "emu/emuDAQ/emuUtil/include/EmuServer.h"
#include "i2oEmuFirstEventNumberMsg.h"
#include "emuRUI/STEPEventCounter.h"

using namespace std;


/**
 * Example Readout Unit Interface (RUI) to be copied and modified by end-users.
 */
class EmuRUI :
public xdaq::WebApplication
{
public:

    /**
     * Define factory method for the instantion of EmuRUI applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    EmuRUI(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);


private:

  // EMu-specific stuff

  bool serverLoopAction(toolbox::task::WorkLoop *wl);

  int runStartUTC_; /// Unix UTC run start time 

  static const unsigned int maxDevices_ = 5; // max possible number of input devices
  static const unsigned int maxClients_ = 5; // max possible number of clients

  xdata::Vector<xdata::String>       clientName_;
  xdata::Vector<xdata::UnsignedLong> clientInstance_;
  xdata::Vector<xdata::Boolean>      clientPersists_; // whether its server needs to be (re)created on config
  xdata::Vector<xdata::String>       clientProtocol_;
  xdata::Vector<xdata::UnsignedLong> clientPoolSize_;
  xdata::Vector<xdata::UnsignedLong> prescaling_;
  xdata::Vector<xdata::Boolean>      onRequest_;
  xdata::Vector<xdata::UnsignedLong> creditsHeld_;
  struct Client {
    xdata::String                  *name;
    xdata::UnsignedLong            *instance;
    xdata::Boolean                 *persists;
    xdata::UnsignedLong            *poolSize;
    xdata::UnsignedLong            *prescaling;
    xdata::Boolean                 *onRequest;
    xdata::UnsignedLong            *creditsHeld;
    EmuServer                      *server;
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
	    EmuServer*                      S=NULL,
            toolbox::task::WorkLoop*        wl =NULL,
	    string                          wln="",
	    bool                            wls=false,
	    toolbox::task::ActionSignature* wla=NULL   ){
      name                    = dynamic_cast<xdata::String*>      ( n );
      instance                = dynamic_cast<xdata::UnsignedLong*>( i );
      persists                = dynamic_cast<xdata::Boolean*>     ( e );
      poolSize                = dynamic_cast<xdata::UnsignedLong*>( s );
      prescaling              = dynamic_cast<xdata::UnsignedLong*>( p );
      onRequest               = dynamic_cast<xdata::Boolean*>     ( r );
      creditsHeld             = dynamic_cast<xdata::UnsignedLong*>( c );
      server                  = S;
      workLoop                = wl;
      workLoopName            = wln;
      workLoopStarted         = wls;
      workLoopActionSignature = wla;
    }
  };
  std::vector<Client*> clients_;

  EmuReader*                          deviceReader_;	     // device reader
  xdata::String                       inputDeviceName_;      // input device name (file path or board number)
  xdata::String                       hardwareMnemonic_;     // mnemonic name for the piece of hardware to read
  xdata::String                       inputDeviceType_;      // spy, slink or file
  xdata::String                       inputDataFormat_;      // "DDU" or "DCC"
// here it gets overwritten in TF DDU(!?!?); move it away:   int                                 inputDataFormatInt_;   // EmuReader::DDU or EmuReader::DCC
  int dummy_;
  int                                 inputDataFormatInt_;   // EmuReader::DDU or EmuReader::DCC
  unsigned int                        previousEventNumber_;

  void createFileWriters();
  void createDeviceReader();
  void destroyDeviceReader();
//   void createDeviceReaders();
//   void destroyDeviceReaders();
  void createServers();
  void destroyServers();
  bool createI2OServer( string clientName, unsigned int clientInstance );
  bool createSOAPServer( string clientName, unsigned int clientInstance, bool persistent=true );
  void onI2OClientCreditMsg(toolbox::mem::Reference *bufRef);
  xoap::MessageReference onSOAPClientCreditMsg( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  string extractParametersFromSOAPClientCreditMsg( xoap::MessageReference msg, unsigned int& instance, int& credits, int& prescaling )
    throw (emuRUI::exception::Exception);
  xoap::MessageReference processSOAPClientCreditMsg( xoap::MessageReference msg )
    throw( emuRUI::exception::Exception );
  void addDataForClients(const int   runNumber, 
			 const int   runStartUTC,
			 const int   nEventsRead,
			 const bool  completesEvent, 
			 const unsigned short errorFlag, 
			 char* const data, 
			 const int   dataLength );
  void makeClientsLastBlockCompleteEvent();
  void insertEmptySuperFragments( const unsigned long fromEventNumber, const unsigned long toEventNumber )
    throw (emuRUI::exception::Exception);
  void ensureContiguousEventNumber();
  void moveToFailedState();

  xdata::UnsignedLong                 nEventsRead_;
  xdata::String                       persistentDDUError_;
  EmuFileWriter                      *fileWriter_;
  EmuFileWriter                      *badEventsFileWriter_;
  int                                 nReadingPassesInEvent_;
  bool                                insideEvent_;
  unsigned short                      errorFlag_;
  // In STEP runs, count on each DDU input the number of events it's contributed to with data
  bool                                isSTEPRun_;
  emuRUI::STEPEventCounter            STEPEventCounter_;
  xoap::MessageReference onSTEPQuery( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference onExcludeDDUInputs( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference onIncludeDDUInputs( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  xoap::MessageReference maskDDUInputs( const bool in, const xoap::MessageReference msg )
    throw (xoap::exception::Exception);

  bool hasHeader( char* const data, const int dataLength );
  bool hasTrailer( char* const data, const int dataLength );
//   int  getDDUDataLengthWithoutPadding(char* const data, const int dataLength);
//   int  getDCCDataLengthWithoutPadding(char* const data, const int dataLength);
  bool interestingDDUErrorBitPattern(char* const data, const int dataLength);
//   unsigned short incrementPassesCounter( const unsigned short errorFlag );
  void printData(std::ostream& os, char* data, const int dataLength);
  void writeDataToFile(  char* const data, const int dataLength, const bool newEvent=false );

  void printBlocks( deque<toolbox::mem::Reference*> d );
  void fillBlock(toolbox::mem::Reference *bufRef,
		 const char*              data,
		 const unsigned int       dataLength,
		 const unsigned long      eventNumber 
		 )
    throw (emuRUI::exception::Exception);


  toolbox::mem::Pool *ruiTaPool_;
  I2O_TID emuTATid_;
  vector< xdaq::ApplicationDescriptor* > taDescriptors_;
  void getTidOfEmuTA()
    throw ( xcept::Exception );
  void sendEventNumberToTA( unsigned long firstEventNumber )
    throw ( xcept::Exception );

  vector< xdaq::ApplicationDescriptor* > getAppDescriptors(xdaq::Zone *zone,
							   const string            appClass)
    throw (emuRUI::exception::Exception);

  xoap::MessageReference createParameterGetSOAPMsg(const string appClass,
						   const string paramName,
						   const string paramType)
    throw (emuRUI::exception::Exception);

  DOMNode *findNode(DOMNodeList *nodeList,
		    const string nodeLocalName)
    throw (emuRUI::exception::Exception);

  string extractScalarParameterValueFromSoapMsg(xoap::MessageReference msg,
						const string           paramName)
    throw (emuRUI::exception::Exception);
  
  string getScalarParam(xdaq::ApplicationDescriptor* appDescriptor,
			const string                 paramName,
			const string                 paramType)
    throw (emuRUI::exception::Exception);

  time_t toUnixTime( const std::string YYMMDD_hhmmss_UTC );

  void getRunInfo() throw (emuRUI::exception::Exception);

  xoap::MessageReference onReset(xoap::MessageReference msg)
    throw (xoap::exception::Exception);

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
    unsigned long instance_;

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
    xdata::UnsignedLong dataBufSize_;

    /**
     * Exported read/write parameter specifiying the size of payload per FED
     * per event.
     *
     * fedPayloadSize is in bytes.
     */
    xdata::UnsignedLong fedPayloadSize_;

    /**
     * Exported read/write parameter specifiying the source id of each FED
     * supplying data to a single super-fragment.
     *
     * The size of this vector specifies the number of FEDs that supply data
     * to a single super-fragment.
     */
    xdata::Vector<xdata::UnsignedLong> fedSourceIds_;

    /**
     * Exported read/write parameter specifying the threshold of the event data
     * pool between the EmuRUI and RU.
     */
    xdata::UnsignedLong threshold_;

    //
    // EMu-specific stuff
    //
    xdata::String       pathToDataOutFile_;    // the path to the file to write the data into (no file written if "")
    xdata::String       pathToBadEventsFile_;    // the path to the file to write the bad events into (no file written if "")
    xdata::UnsignedLong fileSizeInMegaBytes_;  // when the file size exceeds this, no more events will be written to it (no file written if <=0)
    xdata::Integer      maxEvents_;            // stop reading from DDU after this many events
    xdata::Boolean      passDataOnToRUBuilder_;// it true, data is sent to the event builder
    xdata::UnsignedLong runNumber_;            // run number to be obtained from TA
    xdata::Boolean      isBookedRunNumber_;    // whether or not this run number was booked in the database
    xdata::String       runStartTime_;         // run start time to be included in the file name
    xdata::String       runStopTime_;          // run stop time to be included in the metafile
    xdata::String       runType_;              // run type to be included in the file name

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
    xdata::UnsignedLong eventNumber_;

    /////////////////////////////////////////////////////////////
    // End of exported parameters used for monitoring          //
    /////////////////////////////////////////////////////////////

//   // DEBUG START
//   EmuClock *ec_rwl; // for the readout workloop
//   EmuClock *ec_swl; // for the server workloop
//   unsigned int visitCount_rwl, visitCount_swl;
  
//   // DEBUG END

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
    throw (emuRUI::exception::Exception);

    /**
     * Returns the name of the info space that contains exported parameters
     * for monitoring.
     */
    string generateMonitoringInfoSpaceName
    (
        const string        appClass,
        const unsigned long appInstance
    );

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
    xoap::MessageReference processSoapFsmCmd(xoap::MessageReference msg)
    throw (xoap::exception::Exception);

    /**
     * Extracts the name of the command represented by the specified SOAP
     * message.
     */
    string extractCmdNameFromSoapMsg(xoap::MessageReference msg)
    throw (emuRUI::exception::Exception);

    /**
     * Creates a SOAP response message to a state machine state change request.
     */
    xoap::MessageReference createFsmResponseMsg
    (
        const string cmd,
        const string state
    )
    throw (emuRUI::exception::Exception);

    /**
     * Processes the specified command for the finite state machine.
     */
    void processFsmCommand(const string cmdName)
    throw (emuRUI::exception::Exception);

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
    string createRuiRuPoolName(const unsigned long emuRUIInstance);

    /**
     * Returns a "HeapAllocator" memory pool with the specified name.
     */
    toolbox::mem::Pool *createHeapAllocatorMemoryPool
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        const string                     poolName
    )
    throw (emuRUI::exception::Exception);

    /**
     * Entry point for the work loop responsible for generating dummy
     * super-fragments.
     */
    bool workLoopAction(toolbox::task::WorkLoop *wl);

    /**
     * The behaviour of the EmuRUI when it is enabled.
     */
/*     void processAndCommunicate(); */
    int processAndCommunicate(); //BK

    /**
     * Tries to send the next super-fragment block to the RU.
     *
     * Failure may occur if there is back pressure from the EmuRUI to RU peer
     * transport.
     */
    void sendNextPendingBlock()
    throw (emuRUI::exception::Exception);

    /**
     * Continues the construction of the dummy super-fragment.
     */
//     void continueConstructionOfSuperFrag()
    int continueConstructionOfSuperFrag()
    throw (emuRUI::exception::Exception);

    int continueSTEPRun()
    throw (emuRUI::exception::Exception);

    /**
     * Appends a new block from the EmuRUI/RU memory pool to the super-fragment
     * under contruction.
     */
//     void appendNewBlockToSuperFrag()
    void appendNewBlockToSuperFrag( const char* data, const unsigned long dataLength, const unsigned long eventNumber )
    throw (emuRUI::exception::Exception);

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

};


#endif
