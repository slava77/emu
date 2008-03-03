#ifndef __EmuFU_h__
#define __EmuFU_h__

#include "emuFU/exception/Exception.h"
#include "i2o/i2oDdmLib.h"
#include "i2o/utils/AddressMap.h"
#include "toolbox/BSem.h"
#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/mem/MemoryPoolFactory.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/Double.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Vector.h"

#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "emu/emuDAQ/emuTA/include/SliceTestTriggerChunk.h"
#include "emu/emuDAQ/emuUtil/include/EmuFileWriter.h"
#include "emu/emuDAQ/emuClient/include/i2oEmuClientMsg.h"
#include "emu/emuDAQ/emuUtil/include/EmuServer.h"

#include "emuFU/EmuEventHeaderTrailer.h"

using namespace std;

/// Filter unit for built events.

/// This filter unit is transparent, i.e., it makes no selection whatsoever.
/// It performs the following functions:
/// \li Gets a built event from the builder.
/// \li Drops the trigger block.
/// \li Wraps the event in DCC header/trailer.
/// \li Gets credits from client(s), and transmits events to them.
/// \li Writes data to local files.
class EmuFU :
public xdaq::WebApplication
{
public:

    /**
     * Define factory method for the instantion of EmuFU applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    EmuFU(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);


private:

    //
    // EMu-specific stuff
    //

  toolbox::task::WorkLoopFactory *workLoopFactory_;
  bool serverLoopAction(toolbox::task::WorkLoop *wl);


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

  void createServers();
  void destroyServers();
  bool createI2OServer( string clientName, unsigned int clientInstance );
  bool createSOAPServer( string clientName, unsigned int clientInstance, bool persistent=true );
  void onI2OClientCreditMsg(toolbox::mem::Reference *bufRef);
  xoap::MessageReference onSOAPClientCreditMsg( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  string extractParametersFromSOAPClientCreditMsg( xoap::MessageReference msg, unsigned int& instance, int& credits, int& prescaling )
    throw (emuFU::exception::Exception);
  xoap::MessageReference processSOAPClientCreditMsg( xoap::MessageReference msg )
    throw( emuFU::exception::Exception );
  void addDataForClients(const int   runNumber, 
			 const int   nEventsRead,
			 const bool  completesEvent, 
			 char* const data, 
			 const int   dataLength );
  void moveToFailedState();
  EmuFileWriter *fileWriter_;
  void printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader=false );
  DOMNode *findNode(DOMNodeList *nodeList,
		    const string nodeLocalName)
    throw (emuFU::exception::Exception);
  xoap::MessageReference createParameterGetSOAPMsg(const string appClass,
						   const string paramName,
						   const string paramType)
    throw (emuFU::exception::Exception);
  string extractScalarParameterValueFromSoapMsg(xoap::MessageReference msg,
						const string           paramName)
    throw (emuFU::exception::Exception);
  string getScalarParam(xdaq::ApplicationDescriptor* appDescriptor,
			const string                 paramName,
			const string                 paramType)
    throw (emuFU::exception::Exception);
  void getRunInfo() throw (emuFU::exception::Exception);

  xoap::MessageReference onReset(xoap::MessageReference msg)
    throw (xoap::exception::Exception);

  /// Emu event header/trailer
  emuFU::EmuEventHeaderTrailer* emuEventHeaderTrailer_;

  /// Pointer to the descriptor of the EmuDAQManager application.
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
     * Protects the EmuFU from multithreaded access.
     */
    toolbox::BSem bSem_;

    /**
     * Name of the memory pool for creating EmuFU to BU I2O control messages.
     */
    string i2oPoolName_;

    /**
     * Memory pool used for creating EmuFU to BU I2O control messages.
     */
    toolbox::mem::Pool *i2oPool_;

    /**
     * The application descriptor of the BU from which the EmuFU will request
     * events.
     */
    xdaq::ApplicationDescriptor *buDescriptor_;

    /**
     * The I2O tid of the BU from which the EmuFU will request events.
     */
    I2O_TID buTid_;

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


    //
    // EMu-specific stuff
    //
    xdata::String       pathToDataOutFile_;   ///< the path to the file to write the data into (no file written if "")
    xdata::UnsignedLong fileSizeInMegaBytes_; ///< when the file size exceeds this, no more events will be written to it (no file written if <=0)
    xdata::UnsignedLong runNumber_;           ///< run number to be obtained from EmuTA
    xdata::Boolean      isBookedRunNumber_;   ///< whether or not this run number was booked in the database, to be obtained from EmuTA
    xdata::String       runStartTime_;        ///< run start time to be included in the file name, to be obtained from EmuTA
    xdata::String       runType_;             ///< run type to be included in the file name

    xdata::UnsignedLong CSCConfigId_;         ///< unique id of CSC configuration obtained from CSC Function Manager
    xdata::UnsignedLong TFConfigId_;          ///< unique id of Track Finder configuration obtained from TF Function Manager

    /**
     * Exported read/write parameter - The instance number of BU that the EmuFU
     * will request events from.
     */
    xdata::UnsignedLong buInstNb_;

    /**
     * Exported read/write parameter - Number of requests the EmuFU should keep
     * outstanding between itself and the BU servicing its requests.
     */
    xdata::UnsignedLong nbOutstandingRqsts_;

    /**
     * Exported read-only parameter specifying whether or not the EmuFU should
     * sleep between events.
     *
     * This is used to simulate the time taken by a real EmuFU to process an
     * event.
     */
    xdata::Boolean sleepBetweenEvents_;

    /**
     * Exported read-only parameter specifying the time in micro seconds the
     * EmuFU should sleep when simulating the time taken by a real EmuFU to process
     * an event.
     */
    xdata::UnsignedLong sleepIntervalUsec_;

    /**
     * Exported read/write parameter specifying the number of events before the
     * EmuFU will call exit() - the default value of 0 means that the EmuFU will
     * never call exit().
     *
     * This exported parameter is for the sole purpose of testing a BU's
     * ability to tolerate a EmuFU crashing.  A EmuFU is configured to call exit when
     * it is to emulate a crash.
     */
    xdata::UnsignedLong nbEventsBeforeExit_;

    /////////////////////////////////////////////////////////////
    // End of exported parameters used for configuration       //
    /////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////
    // Beginning of exported parameters used for monitoring //
    //////////////////////////////////////////////////////////

    /**
     * Exported read-only parameter specifying the current state of the
     * application.
     */
    xdata::String stateName_;

    /**
     * Exported read-only parameter - Total number of event the EmuFU has
     * processed since it was last
     * configured.
     */
    xdata::UnsignedLong nbEventsProcessed_;

    //////////////////////////////////////////////////////////
    // End of exported parameters used for monitoring       //
    //////////////////////////////////////////////////////////


    /**
     * Head of super-fragment under-construction.
     */
    toolbox::mem::Reference *superFragmentHead_;

    /**
     * Tail of super-fragment under-construction.
     */
    toolbox::mem::Reference *superFragmentTail_;

    /**
     * Current block number of the super-fragment under construction.
     */
    unsigned int blockNb_;

    /**
     * True if a fault has been detected in the event data.
     */
    bool faultDetected_;

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
     * Defines the finite state machine of the application.
     */
    void defineFsm()
    throw (emuFU::exception::Exception);

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

  /// Serializes xdata scalar into std::string.
    string serializableScalarToString(xdata::Serializable *s);

  /// Serializes xdata unsigned long into std::string.
    string serializableUnsignedLongToString(xdata::Serializable *s);

  /// Serializes xdata double into std::string.
    string serializableDoubleToString(xdata::Serializable *s);

  /// Serializes xdata string into std::string.
    string serializableStringToString(xdata::Serializable *s);

  /// Serializes xdata boolean into std::string.
    string serializableBooleanToString(xdata::Serializable *s);

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
    throw (emuFU::exception::Exception);

    /**
     * Creates a SOAP response message to a state machine state change request.
     */
    xoap::MessageReference createFsmResponseMsg
    (
        const string cmd,
        const string state
    )
    throw (emuFU::exception::Exception);

    /**
     * Processes the specified command for the finite state machine.
     */
    void processFsmCommand(const string cmdName)
    throw (emuFU::exception::Exception);

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
     * Invoked when an event data block has been received from the BU.
     */
    void I2O_FU_TAKE_Callback(toolbox::mem::Reference *bufRef);

    /**
     * Processes the specified data block.
     */
    void processDataBlock(toolbox::mem::Reference *bufRef)
    throw (emuFU::exception::Exception);

    /**
     * Checks the payload sent by the TA.
     */
    void checkTAPayload(toolbox::mem::Reference *bufRef)
    throw (emuFU::exception::Exception);

    /**
     * Checks the payload sent by a RUI.
     */
    void checkRUIPayload(toolbox::mem::Reference *bufRef)
    throw (emuFU::exception::Exception);

    /**
     * Appends the specified block to the end of the super-fragment under
     * construction.
     */
    void appendBlockToSuperFragment(toolbox::mem::Reference *bufRef);

    /**
     * Checks the current super-fragment under construction.
     *
     * Note that the super-fragment under constrcution must be complete when
     * this method is called.
     */
    void checkSuperFragment()
    throw (emuFU::exception::Exception);

    /**
     * Returns the size in bytes of the sum of all the FED data that contained
     * witin the specified super-fragment.
     */
    unsigned int getSumOfFedData(toolbox::mem::Reference *bufRef);

    /**
     * Fills the specifed buffer with all the FED data from the specified
     * super-fragment.
     */
    void fillBufferWithSuperFragment
    (
        char                    *buf,
        unsigned int            len,
        toolbox::mem::Reference *bufRef
    )
    throw (emuFU::exception::Exception);

    /**
     * Checks to see if the FED data in the specified buffer can be traversed
     * from trailer to header.
     */
    void checkFedTraversal
    (
        char         *buf,
        unsigned int len
    )
    throw (emuFU::exception::Exception);

    /**
     * Releases the memory used by the super-fragment under construction.
     */
    void releaseSuperFragment();

    /**
     * Creates and then sends an I2O_BU_ALLOCATE_MESSAGE_FRAME to the BU.
     */
    void allocateNEvents(const int n)
    throw (emuFU::exception::Exception);

    /**
     * Returns a new I2O_BU_ALLOCATE_MESSAGE_FRAME representing a request for
     * the specified number of events.
     */
    toolbox::mem::Reference *createBuAllocateMsg
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        toolbox::mem::Pool              *pool,
        const I2O_TID                   taTid,
        const I2O_TID                   buTid,
        const int                       nbEvents
    )
    throw (emuFU::exception::Exception);

    /**
     * Creates and then sends an I2O_BU_DISCARD_MESSAGE_FRAME to the BU.
     */
    void discardEvent(const U32 buResourceId)
    throw (emuFU::exception::Exception);

    /**
     * Returns a new I2O_BU_DISCARD_MESSAGE_FRAME representing a request to
     * discard the event with the specified BU resource id.
     */
    toolbox::mem::Reference *createBuDiscardMsg
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        toolbox::mem::Pool              *pool,
        const I2O_TID                   taTid,
        const I2O_TID                   buTid,
        const U32                       buResourceId
    )
    throw (emuFU::exception::Exception);

    /**
     * Returns the name of the memory pool used for creating EmuFU to BU I2O
     * control messages.
     */
    string createI2oPoolName(const unsigned long emuFUInstance);

    /**
     * Returns a "HeapAllocator" memory pool with the specified name.
     */
    toolbox::mem::Pool *createHeapAllocatorMemoryPool
    (
        toolbox::mem::MemoryPoolFactory *poolFactory,
        const string                     poolName
    )
    throw (emuFU::exception::Exception);

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
