#ifndef __EmuDAQtester_h__
#define __EmuDAQtester_h__

#include "emu/emuDAQ/emuDAQtester/include/emuDAQtester/exception/Exception.h"
#include "extern/i2o/include/i2o/i2oDdmLib.h"
#include "i2o/utils/include/i2o/utils/AddressMap.h"
#include "sentinel/include/sentinel/Interface.h"
#include "sentinel/include/sentinel/Listener.h"
#include "toolbox/include/toolbox/mem/MemoryPoolFactory.h"
#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/Boolean.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedLong.h"

#include <string>

using namespace std;


/**
 * Tests the RU builder applications.
 */
class EmuDAQtester :
public xdaq::WebApplication,
public sentinel::Listener
{
public:

    /**
     * Define factory method for the instantion of EmuDAQtester applications.
     */
    XDAQ_INSTANTIATOR();

    /**
     * Constructor.
     */
    EmuDAQtester(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

    /**
     * Invoked when an exception has been received from the sentinel.
     */
    void onException(xcept::Exception &e);


private:

    /**
     * The sentinel used by this application.
     *
     * Note that this pointer maybe equal to zero if no sentinel is found.
     */
    sentinel::Interface *sentinel_;

    /**
     * The logger of this application.
     */
    Logger logger_;

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
    xdata::String stateName_;

    /////////////////////////////////////////////////////
    // End of exported parameters for monitoring       //
    /////////////////////////////////////////////////////


    /**
     * Returns the name to be given to the logger of this application.
     */
    string generateLoggerName();

    /**
     * Get the descriptors of all the applications the EmuDAQtester will
     * need to control.
     */
    void getAllAppDescriptors();

    /**
     * Returns a pointer to the sentinel to be used by this application or 0
     * if the sentinel could not be found.
     */
    sentinel::Interface *getSentinel(xdaq::ApplicationContext *appContext);

    /**
     * Returns a vector of application descriptors order by instance number.
     */
    vector< xdaq::ApplicationDescriptor* > getAppDescriptors
    (
        xdaq::ApplicationGroup *appGroup,
        const string           appClass
    )
    throw (emuDAQtester::exception::Exception);

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
     * Web page used to control the RU builder.
     */
    void controlWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  int purgeIntNumberString( string* s ); // Emu

    /**
     * Processes the form sent from the control web page.
     */
    void processControlForm(xgi::Input *in)
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
    throw (emuDAQtester::exception::Exception);

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
     * Displays a web page of monitoring information that can be easily
     * parsed by software.
     */
    void machineReadableWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

    /**
     * Exports the parameters responsible for showing monitoring information.
     */
    void exportMonitoringParams(xdata::InfoSpace *s);

    /**
     * Starts the test.
     */
    void configureTest()
    throw (emuDAQtester::exception::Exception);
    void startTest()
    throw (emuDAQtester::exception::Exception);

    /**
     * Checks that there is a minimum set of applications to make a RU builder.
     */
    void checkThereIsARuBuilder()
    throw (emuDAQtester::exception::Exception);

    /**
     * Tell the EVM whether or not it should generate dummy triggers.
     */
    void setEVMGenerateDummyTriggers(const bool value)
    throw (emuDAQtester::exception::Exception);

    /**
     * Tell the RUs whether or not they should generate dummy super-fragments.
     */
    void setRUsGenerateDummySuperFrags(const bool vale)
    throw (emuDAQtester::exception::Exception);

    /**
     * Tells the BUs whether or not to drop events.
     */
    void setBUsDropEvents(const bool value)
    throw (emuDAQtester::exception::Exception);

    /**
     * Starts the imaginary trigger, i.e. the TA.
     */
    void configureTrigger()
    throw (emuDAQtester::exception::Exception);
    void startTrigger()
    throw (emuDAQtester::exception::Exception);

    /**
     * Starts just the RU builder, i.e. the BUs, EVM and RUs.
     */
    void startRuBuilder()
    throw (emuDAQtester::exception::Exception);

    /**
     * Starts the imaginary FED builder, i.e. the RUIs.
     */
    void configureFedBuilder()
    throw (emuDAQtester::exception::Exception);
    void startFedBuilder()
    throw (emuDAQtester::exception::Exception);

    /**
     * Starts the imaginary filter farm, i.e. the FUs.
     */
    void configureFilterFarm()
    throw (emuDAQtester::exception::Exception);
    void startFilterFarm()
    throw (emuDAQtester::exception::Exception);

    /**
     * Stops the test.
     */
    void stopTest()
    throw (emuDAQtester::exception::Exception);

    /**
     * Stops the imaginary FED builder, i.e. the RUIs.
     */
    void stopFedBuilder()
    throw (emuDAQtester::exception::Exception);

    /**
     * Stops just the RU builder, i.e. BUs, EVM and RUs.
     */
    void stopRuBuilder()
    throw (emuDAQtester::exception::Exception);

    /**
     * Stops the imaginary trigger, i.e. the TA.
     */
    void stopTrigger()
    throw (emuDAQtester::exception::Exception);

    /**
     * Stops the imaginary filter farm, i.e. the FUs.
     */
    void stopFilterFarm()
    throw (emuDAQtester::exception::Exception);

    /**
     * Sends the specified FSM event as a SOAP message to the specified
     * application.  An exception is raised if the application does not reply
     * successfully with a SOAP response.
     */
    void sendFSMEventToApp
    (
        const string                 eventName,
        xdaq::ApplicationDescriptor* appDescriptor
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Creates a simple SOAP message representing a command with no
     * parameters.
     */
    xoap::MessageReference createSimpleSOAPCmdMsg(const string cmdName)
    throw (emuDAQtester::exception::Exception);

    /**
     * Gets and returns the value of the specified parameter from the specified
     * application.
     */
    string getScalarParam
    (
        xdaq::ApplicationDescriptor* appDescriptor,
        const string                 paramName,
        const string                 paramType
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Sets the specified parameter of the specified application to the
     * specified value.
     */
    void setScalarParam
    (
        xdaq::ApplicationDescriptor* appDescriptor,
        const string                 paramName,
        const string                 paramType,
        const string                 paramValue
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Creates a ParameterGet SOAP message.
     */
    xoap::MessageReference createParameterGetSOAPMsg
    (
        const string appClass,
        const string paramName,
        const string paramType
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Creates a ParameterSet SOAP message.
     */
    xoap::MessageReference createParameterSetSOAPMsg
    (
        const string appClass,
        const string paramName,
        const string paramType,
        const string paramValue
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Returns the value of the specified parameter from the specified SOAP
     * message.
     */
    string extractScalarParameterValueFromSoapMsg
    (
        xoap::MessageReference msg,
        const string           paramName
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Retruns the node with the specified local name from the specified list
     * of node.  An exception is thrown if the node is not found.
     */
    DOMNode *findNode
    (
        DOMNodeList *nodeList,
        const string nodeLocalName
    )
    throw (emuDAQtester::exception::Exception);

    /**
     * Prints the specified soap message to standard out.
     */
    void printSoapMsgToStdOut(xoap::MessageReference message);
};


#endif
