#ifndef __EmuRUI_h__
#define __EmuRUI_h__

// #include "emuDAQ/emuRUI/include/emuRUI/SuperFragmentGenerator.h"
#include "emuDAQ/emuRUI/include/emuRUI/exception/Exception.h"
#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "i2o/utils/include/i2o/utils/AddressMap.h"
#include "sentinel/include/sentinel/Interface.h"
#include "toolbox/include/toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "toolbox/include/toolbox/task/Action.h"
#include "toolbox/include/toolbox/task/WorkLoop.h"
#include "toolbox/include/toolbox/task/WorkLoopFactory.h"
#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/Double.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Vector.h"
#include "xdata/include/xdata/Integer.h"

/* // EMu-specific stuff */
#include "emuDAQ/emuUtil/include/FileWriter.h"
#include "emuDAQ/emuReadout/include/EmuReader.h"
#include "emuDAQ/emuClient/include/i2oEmuClientMsg.h"
#include "emuDAQ/emuUtil/include/EmuServer.h"

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


  static const unsigned int maxDevices_ = 5; // max possible number of input devices
  static const unsigned int maxClients_ = 5; // max possible number of clients

  xdata::Vector<xdata::String>       clientName_;
  xdata::Vector<xdata::Boolean>      clientPersists_; // whether its server needs to be (re)created on config
  xdata::Vector<xdata::String>       clientProtocol_;
  xdata::Vector<xdata::UnsignedLong> clientPoolSize_;
  xdata::Vector<xdata::UnsignedLong> prescaling_;
  xdata::Vector<xdata::Boolean>      onRequest_;
  xdata::Vector<xdata::UnsignedLong> creditsHeld_;
  struct Client {
    xdata::String                  *name;
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

  std::vector<EmuReader*>             deviceReaders_;	     // vector of device readers
  unsigned int                        iCurrentDeviceReader_; // index of device reader currently active
  xdata::UnsignedLong                 nInputDevices_;        // number of input devices
  xdata::Vector<xdata::String>        inputDeviceNames_;     // vector of input device names (file path or board number)
  xdata::String                       inputDeviceType_;      // spy, slink or file
  xdata::String                       inputDataFormat_;      // "DDU" or "DCC"
  int                                 inputDataFormatInt_;   // EmuReader::DDU or EmuReader::DCC

  void createDeviceReaders();
  void destroyDeviceReaders();
  void createServers();
  void destroyServers();
  bool createI2OServer( string clientName );
  bool createSOAPServer( string clientName, bool persistent=true );
  void onI2OClientCreditMsg(toolbox::mem::Reference *bufRef);
  xoap::MessageReference onSOAPClientCreditMsg( xoap::MessageReference msg )
    throw (xoap::exception::Exception);
  string extractParametersFromSOAPClientCreditMsg( xoap::MessageReference msg, int& credits, int& prescaling )
    throw (emuRUI::exception::Exception);
  xoap::MessageReference processSOAPClientCreditMsg( xoap::MessageReference msg )
    throw( emuRUI::exception::Exception );
  void addDataForClients(const int   runNumber, 
			 const int   nEventsRead,
			 const bool  completesEvent, 
			 char* const data, 
			 const int   dataLength );


  xdata::UnsignedLong                 nEventsRead_;
  FileWriter                         *fileWriter_;
  FileWriter                         *badEventsFileWriter_;
  int                                 nDevicesWithBadData_;

  int  getDDUDataLengthWithoutPadding(char* const data, const int dataLength);
  int  getDCCDataLengthWithoutPadding(char* const data, const int dataLength);
  bool interestingDDUErrorBitPattern(char* const data, const int dataLength);
  void printData(char* data, const int dataLength);


  void printBlocks( deque<toolbox::mem::Reference*> d );
  void fillBlock(toolbox::mem::Reference *bufRef,
		 char*                    data,
		 const unsigned int       dataLength)
    throw (emuRUI::exception::Exception);
  

  vector< xdaq::ApplicationDescriptor* > getAppDescriptors(xdaq::ApplicationGroup *appGroup,
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

  void getRunAndMaxEventNumber() throw (emuRUI::exception::Exception);





    /**
     * Pointer to the descriptor of the RUBuilderTester application.
     *
     * It is normal for this pointer to be 0 if the RUBuilderTester application      * cannot be found.
     */
    xdaq::ApplicationDescriptor *rubuilderTesterDescriptor_;

//     /**
//      * Generator of dummy super-fragments.
//      */
//     emuRUI::SuperFragmentGenerator superFragGenerator_;

    /**
     * The sentinel used by this application.
     *
     * Note that this pointer maybe equal to zero if no sentinel is found.
     */
    sentinel::Interface *sentinel_;

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
    BSem applicationBSem_;

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
     * Used to access the application's group without a function call.
     */
    xdaq::ApplicationGroup *appGroup_;

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
    xdata::UnsignedLong maxEvents_;            // stop reading from DDU after this many events
    xdata::Boolean      passDataOnToRUBuilder_;// it true, data is sent to the event builder
    xdata::UnsignedLong runNumber_;            // run number to be obtained from TA

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
        xdaq::ApplicationGroup *appGroup
    );

    /**
     * Returns a pointer to the sentinel to be used by this application or 0
     * if the sentinel could not be found.
     */
    sentinel::Interface *getSentinel(xdaq::ApplicationContext *appContext);

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

    string serializableScalarToString(xdata::Serializable *s);

    string serializableUnsignedLongToString(xdata::Serializable *s);

    string serializableDoubleToString(xdata::Serializable *s);

    string serializableStringToString(xdata::Serializable *s);

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
    bool processAndCommunicate(); //BK

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
    bool continueConstructionOfSuperFrag()
    throw (emuRUI::exception::Exception);

    /**
     * Appends a new block from the EmuRUI/RU memory pool to the super-fragment
     * under contruction.
     */
//     void appendNewBlockToSuperFrag()
    void appendNewBlockToSuperFrag( char* data, unsigned long dataLength )
    throw (emuRUI::exception::Exception);

    /**
     * Fills each of the super-fragment's blocks with the total number of
     * blocks making up the super-fragment.
     */
    void setNbBlocksInSuperFragment(const unsigned int nbBlocks);

    /**
     * Returns the hyper-text reference of the specified application.
     */
    string getHref(xdaq::ApplicationDescriptor *appDescriptor);

    /**
     * Invoked when there is an I2O exception.
     */
    bool onI2oException(xcept::Exception &exception, void *context);

    /**
     * Creates and returns an I2O exception to be passed to the sentinel.
     */
    emuRUI::exception::Exception createI2oExceptionForSentinel
    (
        xcept::Exception            &i2oException,
        xdaq::ApplicationDescriptor *notifier,
        xdaq::ApplicationDescriptor *source,
        xdaq::ApplicationDescriptor *destination
    );

    /**
     * Creates and returns the error message of an I2O exception by specifying
     * the source and destination involved.
     */
    string createI2oErrorMsg
    (
        xdaq::ApplicationDescriptor *source,
        xdaq::ApplicationDescriptor *destination
    );

    /**
     * Returns the value to be given to the "notfier" field of an exception
     * for the sentinel.
     */
    string createValueForSentinelNotifierProperty
    (
        xdaq::ApplicationDescriptor *notifier
    );
};


#endif
