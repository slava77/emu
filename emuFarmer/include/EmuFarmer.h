#ifndef __EmuFarmer_h__
#define __EmuFarmer_h__

#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"

#include "EmuXalanTransformerProblemListener.h"
#include "EmuProcessDescriptor.h"

#include <set>
#include <map>
// #include <multimap>
#include <vector>

//---------------------
#include "xoap/MessageReference.h"

/// \class EmuFarmer 
/// \brief Dispatch XDAQ executives on the Emu farm via JobControl.

class EmuFarmer : public xdaq::WebApplication
{

public:

  /// define factory method for the instantion of EmuFarmer applications
  XDAQ_INSTANTIATOR();
  
  /// constructor
  EmuFarmer(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

  /// the logger of this application
  Logger logger_;

private:
  ////////////////////////////////////////////////////////
  // Beginning of exported parameters for configuration //
  ////////////////////////////////////////////////////////

  ///
  /// \defgroup EmuFarmer_config_param EmuFarmer parameters exported for configuration
  ///
/// @{

  ///
  /// URL of the file containing the list of subsystem config files to be merged
  ///
  xdata::String configFilesList_;

  ///
  /// the file containing the list of subsystem config files to be merged, relative to $BUILD_HOME
  ///
  xdata::String configFilesListRel_;

  /**
   * URL of the XSL transformation file used for merging the subsystem config files
   */
  xdata::String mergingXSLT_;

  /**
   * directory where the merged config file is to be written
   */
  xdata::String mergedConfigDir_;

  /**
   * URL of the directory where the merged config file is
   */
  xdata::String mergedConfigDirURL_;

  /**
   * the directory where the merged config file is, relative to $BUILD_HOME
   */
  xdata::String mergedConfigDirRel_;

  /**
   * user to run the DAQ processes
   */
  xdata::String daqUser_;

  /**
   * user to run the DQM processes
   */
  xdata::String dqmUser_;

  /**
   * user to run the FED processes
   */
  xdata::String fedUser_;

  /**
   * user to run the TF processes
   */
  xdata::String tfUser_;

  /**
   * user to run the DAQManager process
   */
  xdata::String managerUser_;

  /**
   * user to run the CSCSupervisor process
   */
  xdata::String supervisorUser_;

  /**
   * user to run the PC processes
   */
  xdata::String pcUser_;

  /**
   * user to run the LTC processes
   */
  xdata::String ltcUser_;




  /**
   * DAQ applications
   */
  xdata::Vector<xdata::String> daqApplicationNames_;

  /**
   * DQM applications
   */
  xdata::Vector<xdata::String> dqmApplicationNames_;

  /**
   * FED applications
   */
  xdata::Vector<xdata::String> fedApplicationNames_;

  /**
   * TF applications
   */
  xdata::Vector<xdata::String> tfApplicationNames_;

  /**
   * DAQManager applications
   */
  xdata::Vector<xdata::String> managerApplicationNames_;

  /**
   * CSCSupervisor applications
   */
  xdata::Vector<xdata::String> supervisorApplicationNames_;

  /**
   * PC applications
   */
  xdata::Vector<xdata::String> pcApplicationNames_;

  /**
   * LTC applications
   */
  xdata::Vector<xdata::String> ltcApplicationNames_;

  /**
   * path to the home directories (e.g., /home)
   */
  xdata::String pathToHome_;

  /**
   * path to the command to start XDAQ processes relative to the owner's home directory
   */
  xdata::String commandToStartXDAQ_;

  /**
   * path to the command to reload the DDU drivers relative to the owner's home directory
   */
  xdata::String commandToReloadDriversForDAQ_;

  /**
   * path to the command to reload the PC drivers relative to the owner's home directory
   */
  xdata::String commandToReloadDriversForPC_;

  /**
   * applications whose executive should be started with log level DEBUG
   */
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_DEBUG_;

  /**
   * applications whose executive should be started with log level INFO
   */
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_INFO_;

  /**
   * applications whose executive should be started with log level ERROR
   */
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_ERROR_;
/// @}

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  ///
  /// process group --> user map
  ///
  map< string, xdata::String* > users_;

  ///
  /// process group --> applications map
  ///
  map< string, xdata::Vector<xdata::String>* > applicationNames_;

  ///
  /// name of the merged config file
  ///
  string       mergedConfigFileName_;

  ///
  /// URL of the merged config file
  ///
  string       mergedConfigFileURL_;

  ///
  /// DOM of the merged config file
  ///
  DOMDocument* mergedConfigFileDOM_;

  /**
   *  host_port --> group_name map
   */
  map< string, string > emuGroups_;

  /**
   *  host_port --> EmuProcessDescriptor map
   */
  map< string, EmuProcessDescriptor > emuProcessDescriptors_;

  /**
   *  group_name --> host_port multimap (the inverse of emuGroups_)
   */
  multimap< string, string > emuProcesses_;

  /**
   *  application_name --> log_level map
   */
  map< string, string > logLevels_;

  ///
  /// how the process group tables are to be placed on the web page
  ///
  static const char* pageLayout_[][2];


  /// Generates logger name.

  ///
  /// @return logger name
  ///  
  string generateLoggerName();

  /// Binds web address to a method.
  void bindWebInterface();

  /// Exports xdata parameters.
  void exportParams();

  /// Creates process group --> user map.
  void mapUserNames();

  /// Creates process group --> application_names map.
  void mapApplicationNames();


  /// Creates application_name --> log_level map.
  void mapLogLevels();


  /// Merges subsystems' config files and loads the merges file.

  ///
  /// @return error message
  ///
  string createConfigFile();

  /// Merges subsystems' config files.

  ///
  /// @return error message
  ///
  string mergeConfigFile();

  /// Load merged config file.
  void loadConfigFile();

  /// Gets the XML document and creates its DOM.

  /// Copied from xdaq::ApplicationContextImpl::loadDOM
  /// @param pathname URL of the XML document
  ///
  /// @return DOM of the XML document
  ///
  DOMDocument* loadDOM( const std::string& pathname )
    throw (xdaq::exception::Exception);

  /// Walks through the merged config file and group the contexts.
  void collectEmuProcesses();

  /// Assigns log levels to processes.

  /// Assign log levels to processes. By default it's WARN.
  /// If some of its applications are listed otherwise in EmuFarmer's config file,
  /// assign the lowest severity one.
  void assignLogLevels();

  /// Assign Emu processes a JobControl process responsible for them.

  /// Loops over all JobControl apps, tries to resolve their URLs, and assigns
  /// their resolved URLs and applicationDescriptors to all Emu processes on the same host.
  void assignJobControlProcesses();

  /// Makes the JobControl --> Emu_processes_on_the_same_host_as_JobControl assignments.

  /// Assigns jobControl's URL and ApplicationDescriptor to all Emu processes running on the same host.
  /// @param jobControl JobControl's process descriptor
  /// @param jcad JobControl's application descriptor
  ///
  void assignJobControlProcess( EmuProcessDescriptor& jobControl, xdaq::ApplicationDescriptor* jcad );

  /// Finds out which group this application belongs to.

  ///
  /// @param applicationName application's name
  ///
  /// @return application group's name ["other" if application is not in \ref applicationNames_ ]
  ///
  string applicationGroupName( const string& applicationName );

  /// Is name an element of this application name array?

  ///
  /// @param name application name
  /// @param array array of application names
  ///
  /// @return TRUE if name is an element of array
  ///
  bool isContained( const string& name, xdata::Vector<xdata::String>* array );

  /// Creates the default web page.

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  //   void css(xgi::Input *in, xgi::Output *out);

  //   void javascript(xgi::Input *in, xgi::Output *out);

  /// Creates an html table showing the URL of the file listing the subsystems' config files.

  ///
  /// @param out cgi output
  ///
  void configListFileTable(xgi::Output *out);

  /// Creates an html table showing the URL of the merged config file.

  ///
  /// @param out cgi output
  /// @param message error message to display
  ///
  void configFileTable(xgi::Output *out, const string& message);

  /// Creates html buttons associated with actions.

  /// Creates four visible buttons for start/stop/restart/poll.
  /// These are NOT submit buttons, they only invoke a javascript function
  /// which in turn will submit the form (provided the user confirms the action).
  /// @param out cgi output
  ///
  void actionButtons(xgi::Output *out);

  /// Creates html table for a process group.

  ///
  /// @param groupName group name
  /// @param out cgi output
  ///
  void processGroupTable(const string& groupName, xgi::Output *out);

  /// Processes the form submitted by the web user, and performs the requested actions.

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  /// @return error message
  ///
  string processForm(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  /// Performs action on the processes selected by the user.

  ///
  /// @param action name of action
  /// @param fev cgi vector from html form entry containing the selected processes' host:port
  ///
  /// @return TRUE if successful
  ///
  bool actOnEmuProcesses( const string& action, const vector<cgicc::FormEntry>& fev )
    throw (xdaq::exception::Exception);


  /// Performs action on a process.

  ///
  /// @param action name of action
  /// @param url URL of the process
  ///
  /// @return TRUE if successful
  ///
  bool actOnEmuProcess( const string& action, const string& url )
    throw (xdaq::exception::Exception);

  /// Polls all URL's found selected in the form entry and returns those not yet running.

  ///
  /// @param fev cgi vector from html form entry containing the selected processes' URL (host:port)
  ///
  /// @return vector containing the URL's of the processes that are not yet running
  ///
  vector<cgicc::FormEntry> pollExecutives( const vector<cgicc::FormEntry> fev );


  /// Polls all XDAQ processes.
  void pollAllExecutives();

  /// Gets the log level from the executive at URL.

  ///
  /// @param URL XDAQ process's URL
  ///
  /// @return FALSE on failure
  ///
  bool pollExecutive( const string& URL );

  /// Reloads drivers for a process group.

  ///
  /// @param fev fev cgi vector from html form entry containing the selected processes' URL (host:port)
  /// @param group process group name
  ///
  void reloadDrivers( const vector<cgicc::FormEntry>& fev, const string group )
    throw (xdaq::exception::Exception);

  /// Creates SOAP message to start a XDAQ process at the given URL.

  ///
  /// @param url host:port of the XDAQ process to be started
  ///
  /// @return reference to SOAP message
  ///
  xoap::MessageReference createSOAPCommandToHatch( const string& url );

  /// Creates SOAP message to stop a XDAQ process at the given URL.

  ///
  /// @param url host:port of the XDAQ process to be started
  ///
  /// @return reference to SOAP message
  ///
  xoap::MessageReference createSOAPCommandToCull( const string& url );

  /// Creates SOAP message to reload the driver used by an application at the given URL.

  ///
  /// @param url host:port of the XDAQ process to be started
  ///
  /// @return reference to SOAP message
  ///
  xoap::MessageReference createSOAPCommandToReload( const string& url );

  /// Finds a node in a list of nodes in a DOM.

  ///
  /// @param nodeList list of nodes
  /// @param nodeLocalName local (namespace-less) name of sought node
  ///
  /// @return found node
  ///
  DOMNode* findNode( DOMNodeList *nodeList, const string& nodeLocalName )
    throw (xdaq::exception::Exception);

  /// Redirects to the base URL, stripped of any form entry.

  /// Used for preventing the previous action from being repeated if the user hits the browser's 'reload' button.
  /// @param in cgi input
  /// @param out cgi output
  ///
  void webRedirect(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  /// Gets UTC date and time.

  ///
  ///
  /// @return UTC date and time in YYYYMMDD_hhmmss_UTC format
  ///
  string getDateTime();

  /// print names and values from a list of nodes (for debugging purposes only)

  ///
  /// @param List list of nodes
  ///
  /// @return names and values
  ///
  string printNodeList( DOMNodeList *List );

  /// Prints maps, etc. for debugging purposes, starting with message.

  ///
  /// @param message print this first
  ///
  void debugPrint( const string& message = "" );

  /// Creates SOAP message to query an exported parameter.

  ///
  /// @param appClass application class
  /// @param paramName name of parameter
  /// @param paramType type of parameter
  ///
  /// @return SOAP message
  ///
  xoap::MessageReference createParameterGetSOAPMsg
  ( const string appClass,
    const string paramName,
    const string paramType )
    throw (xdaq::exception::Exception);

  /// Parses SOAP message to extract a scalar parameter from it.

  ///
  /// @param msg SOAP message to be parsed
  /// @param paramName name of parameter to be extracted
  ///
  /// @return value of extracted parameter
  ///
  string extractScalarParameterValueFromSoapMsg
  ( xoap::MessageReference msg,
    const string           paramName )
    throw (xdaq::exception::Exception);

  /// Posts a SOAP message to another application.

  ///
  /// @param message SOAP message to be posted
  /// @param URL URL of executive running this application
  /// @param localId application's local id
  ///
  /// @return 
  ///
  xoap::MessageReference postSOAP( xoap::MessageReference message, 
				   const string& URL,
				   const int localId ) 
    throw (xdaq::exception::Exception);
};

#endif
