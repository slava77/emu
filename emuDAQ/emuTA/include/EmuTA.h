#ifndef __EmuTA_h__
#define __EmuTA_h__

// #include "evb/examples/ta/include/ta/TriggerGenerator.h"
// #include "evb/examples/ta/include/ta/exception/Exception.h"
#include "emu/emuDAQ/emuTA/include/emuTA/TriggerGenerator.h"
#include "emu/emuDAQ/emuTA/include/emuTA/exception/Exception.h"
#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "i2o/utils/include/i2o/utils/AddressMap.h"
#include "sentinel/include/sentinel/Interface.h"
#include "toolbox/include/BSem.h"
#include "toolbox/include/toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/Boolean.h"
#include "xdata/include/xdata/Double.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Integer.h"


using namespace std;


/**
 * Example trigger adapter (TA) to be copied and modified by end-users.
 */
class EmuTA :
public xdaq::WebApplication
{
public:

    /**
     * Define factory method for the instantion of EmuTA applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    EmuTA(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);


private:

    /**
     * Pointer to the descriptor of the RUBuilderTester application.
     *
     * It is normal for this pointer to be 0 if the RUBuilderTester application      * cannot be found.
     */
    xdaq::ApplicationDescriptor *rubuilderTesterDescriptor_;

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
     * The name of the info space that contains exported parameters used for
     * monitoring.
     */
    string monitoringInfoSpaceName_;

    /**
     * Info space that contains exported parameters used for monitoring.
     */
    xdata::InfoSpace *monitoringInfoSpace_;

    /**
     * Binary semaphore used to protect the EmuTA from multithreaded access.
     */
    BSem bSem_;

    /**
     * The finite state machine of the application.
     */
    toolbox::fsm::FiniteStateMachine fsm_;

    /**
     * The generator of dummy triggers.
     */
    emuTA::TriggerGenerator triggerGenerator_;

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
     * The application descriptor of the EVM.
     */
    xdaq::ApplicationDescriptor *evmDescriptor_;

    /**
     * The I2O tid of the EVM.
     */
    I2O_TID evmTid_;

    /**
     * Memory pool from which buffers for dummy triggers are taken.
     */
    toolbox::mem::Pool *triggerPool_;

    /**
     * The application's standard configuration parameters.
     */
    vector< pair<string, xdata::Serializable *> > stdConfigParams_;

    /**
     * The application's standard monitoring parameters.
     */
    vector< pair<string, xdata::Serializable *> > stdMonitorParams_;


    ////////////////////////////////////////////////////////
    // Beginning of exported parameters for configuration //
    ////////////////////////////////////////////////////////

    /**
     * Exported read/write parameter specifying the trigger source id that is
     * to be put into each dummy trigger.
     */
    xdata::UnsignedLong triggerSourceId_;

  //
  // EMu-specific stuff
  //
    xdata::String       runStartTime_;
    xdata::UnsignedLong runNumber_;
    xdata::Integer      maxNumTriggers_;


    ////////////////////////////////////////////////////////
    // End of exported parameters for configuration       //
    ////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////
    // Beginning of exported parameters used for monitoring //
    //////////////////////////////////////////////////////////

    /**
     * Exported read-only parameter specifying the current state of the
     * application.
     */
    xdata::String stateName_;

    /**
     * Exported read-only parameter specifying the number of trigger credits
     * currently held by the EmuTA.
     */
    xdata::UnsignedLong nbCreditsHeld_;

    /**
     * Exported read-only parameter specifying the next trigger event number.
     */
    xdata::UnsignedLong eventNumber_;

    //////////////////////////////////////////////////////////
    // End of exported parameters used for monitoring       //
    //////////////////////////////////////////////////////////


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
    throw (emuTA::exception::Exception);

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
    throw (emuTA::exception::Exception);

    /**
     * Creates a SOAP response message to a state machine state change request.
     */
    xoap::MessageReference createFsmResponseMsg
    (
        const string cmd,
        const string state
    )
    throw (emuTA::exception::Exception);

    /**
     * Processes the specified command for the finite state machine.
     */
    void processFsmCommand(const string cmdName)
    throw (emuTA::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Halted->Ready transition.
     */
    void configureAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Gets the application descriptors and tids of this application and
     * those with which it communicates.
     */
    void getAppDescriptorsAndTids()
    throw (emuTA::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Ready->Enabled transition.
     */
    void enableAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Enabled->Suspended transition.
     */
    void suspendAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * Suspended->Enabled transition.
     */
    void resumeAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * ANY STATE BUT READY->Halted transition.
     */
    void haltAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception);

    /**
     * Callback implementing the action to be executed on the
     * READY->Halted transition.
     */
    void haltActionComingFromReady(toolbox::Event::Reference e)
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
     * Sends n triggers to the EVM.
     */
    void sendNTriggers(const unsigned int n)
    throw (emuTA::exception::Exception);

    /**
     * I2O callback routine invoked when a trigger credit count has been
     * received from the EVM.
     */
    void taCreditMsg(toolbox::mem::Reference *bufRef);

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
    emuTA::exception::Exception createI2oExceptionForSentinel
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
  

  //
  // EMu-specific stuff
  //

  string getDateTime();
  void printBlock( toolbox::mem::Reference *bufRef, bool printMessageHeader );
};


#endif
