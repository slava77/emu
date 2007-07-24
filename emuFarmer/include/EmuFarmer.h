#ifndef __EmuFarmer_h__
#define __EmuFarmer_h__

#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/Boolean.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedInteger32.h"
#include "xdata/include/xdata/Vector.h"

#include "emu/emuFarmer/include/EmuXalanTransformerProblemListener.h"
#include "emu/emuFarmer/include/EmuProcessDescriptor.h"

#include <set>
#include <map>
// #include <multimap>
#include <vector>

//---------------------
#include "xoap/MessageReference.h"

/**
 * Dispatches XDAQ executives on the Emu farm via JobControl.
 */
class EmuFarmer : public xdaq::WebApplication
{
public:

  /**
   * Define factory method for the instantion of EmuFarmer applications.
   */
  XDAQ_INSTANTIATOR();
  
  /**
   * Constructor.
   */
  EmuFarmer(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);


  /**
   * The logger of this application.
   */
  Logger logger_;

private:
  ////////////////////////////////////////////////////////
  // Beginning of exported parameters for configuration //
  ////////////////////////////////////////////////////////

  /**
   * 
   */
  xdata::String configFilesList_;

  /**
   * 
   */
  xdata::String mergingXSLT_;

  /**
   * 
   */
  xdata::String mergedConfigDir_;

  /**
   * 
   */
  xdata::String mergedConfigDirURL_;


  /**
   * 
   */
  xdata::String daqUser_;

  /**
   * 
   */
  xdata::String dqmUser_;

  /**
   * 
   */
  xdata::String fedUser_;

  /**
   * 
   */
  xdata::String tfUser_;

  /**
   * 
   */
  xdata::String managerUser_;

  /**
   * 
   */
  xdata::String supervisorUser_;

  /**
   * 
   */
  xdata::String pcUser_;

  /**
   * 
   */
  xdata::String ltcUser_;




  /**
   * 
   */
  xdata::Vector<xdata::String> daqApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> dqmApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> fedApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> tfApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> managerApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> supervisorApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> pcApplicationNames_;

  /**
   * 
   */
  xdata::Vector<xdata::String> ltcApplicationNames_;

  /**
   * 
   */
  xdata::String commandToStartXDAQ_;

  /**
   * 
   */
  xdata::String commandToReloadDriversForDAQ_;

  /**
   * 
   */
  xdata::String commandToReloadDriversForPC_;

  /**
   * Lists of applications whose executive should be started with log level other than WARN
   */
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_DEBUG_;
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_INFO_;
  xdata::Vector<xdata::String> ApplicationsWithLogLevel_ERROR_;

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  map< string, xdata::String* > users_;
  map< string, xdata::Vector<xdata::String>* > applicationNames_;
  string       mergedConfigFileURL_;
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

  static const char* pageLayout_[][2];

  string generateLoggerName();
  void bindWebInterface();
  void exportParams();
  void mapUserNames();
  void mapApplicationNames();
  void mapLogLevels();
  string createConfigFile();
  string mergeConfigFile();
  void loadConfigFile();
  DOMDocument* loadDOM( const std::string& pathname )
    throw (xdaq::exception::Exception);
  void collectEmuProcesses();
  void assignLogLevels();
  void assignJobControlProcesses();
  void assignJobControlProcess( EmuProcessDescriptor& jobControl, xdaq::ApplicationDescriptor* jcad );
  string applicationGroupName( const string& applicationName );
  bool isContained( const string& name, xdata::Vector<xdata::String>* array );
  void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  void css(xgi::Input *in, xgi::Output *out);
  void javascript(xgi::Input *in, xgi::Output *out);
  void configListFileTable(xgi::Output *out);
  void configFileTable(xgi::Output *out, const string& message);
  void actionButtons(xgi::Output *out);
  void processGroupTable(const string& groupName, xgi::Output *out);
  string processForm(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  bool actOnEmuProcesses( const string& action, const vector<cgicc::FormEntry>& fev )
    throw (xdaq::exception::Exception);
  bool actOnEmuProcess( const string& action, const string& url )
    throw (xdaq::exception::Exception);
  vector<cgicc::FormEntry> pollExecutives( const vector<cgicc::FormEntry> fev );
  void pollAllExecutives();
  bool pollExecutive( const string& URL );
  void reloadDrivers( const vector<cgicc::FormEntry>& fev, const string group )
    throw (xdaq::exception::Exception);
  xoap::MessageReference createSOAPCommandToHatch( const string& url );
  xoap::MessageReference createSOAPCommandToCull( const string& url );
  xoap::MessageReference createSOAPCommandToReload( const string& url );
  DOMNode* findNode( DOMNodeList *nodeList, const string& nodeLocalName )
    throw (xdaq::exception::Exception);
  void webRedirect(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);
  string getDateTime();
  string printNodeList( DOMNodeList *List );
  void debugPrint( const string& message = "" );
  xoap::MessageReference createParameterGetSOAPMsg
  ( const string appClass,
    const string paramName,
    const string paramType )
    throw (xdaq::exception::Exception);
  string extractScalarParameterValueFromSoapMsg
  ( xoap::MessageReference msg,
    const string           paramName )
    throw (xdaq::exception::Exception);
  xoap::MessageReference postSOAP( xoap::MessageReference message, 
				   const string& URL,
				   const int localId ) 
    throw (xdaq::exception::Exception);
};

#endif
