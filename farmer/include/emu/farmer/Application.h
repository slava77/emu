#ifndef __emu_farmer_Application_h__
#define __emu_farmer_Application_h__

#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"

#include "emu/farmer/EmuXalanTransformerProblemListener.h"
#include "emu/farmer/ProcessDescriptor.h"

#include <set>
#include <map>
// #include <multimap>
#include <vector>

//---------------------
#include "xoap/MessageReference.h"

namespace emu { namespace farmer {

/// \class emu::farmer::Application
/// \brief Dispatch XDAQ executives on the Emu farm via JobControl.

class Application : public xdaq::WebApplication
{

public:

  /// define factory method for the instantion of EmuFarmer applications
  XDAQ_INSTANTIATOR();
  
  /// constructor
  Application(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

  /// the logger of this application
  Logger logger_;

private:
  ////////////////////////////////////////////////////////
  // Beginning of exported parameters for configuration //
  ////////////////////////////////////////////////////////

  ///
  /// \defgroup Application_config_param Application parameters exported for configuration
  ///
/// @{

  ///
  /// 
  ///
  xdata::String outputDir_;

  ///
  /// 
  ///
  xdata::String rs3UserFile_;

  ///
  /// 
  ///
  xdata::String uploadingCommand_;

  ///
  /// 
  ///
  xdata::String generatingCommand_;

  ///
  /// 
  ///
  xdata::String jobControlClass_;

  ///
  /// 
  ///
  xdata::String executiveClass_;

/// @}

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  ///
  ///
  ///
  string XDAQ_ROOT_;

  ///
  ///
  ///
  string defaultMapping_;

  ///
  ///
  ///
  string editedMapping_;

  ///
  ///
  ///
  string processingInstructionSetter_;

  ///
  ///
  ///
  string statusInserter_;

  ///
  ///
  ///
  string selectedDUCKName_;

  ///
  ///
  ///
  string selectedDUCKOriginal_;

  ///
  ///
  ///
  string selectedDUCK_;

  ///
  /// XDAQ executive config path --> XDAQ executive config
  ///
  map< string, string > xdaqConfigs_;

  ///
  /// URI --> ProcessDescriptor map
  ///
  map< string, ProcessDescriptor > processDescriptors_;


  /// Generates logger name.

  ///
  /// @return logger name
  ///  
  string generateLoggerName();

  /// Binds web address to a method.
  void bindWebInterface();

  /// Exports xdata parameters.
  void exportParams();

  void startExecutives();

  void configureExecutives();

  void createExecutives();

  void destroyExecutives();

  void pollProcesses();

  string generateConfigFilesWithScript()
    throw( xcept::Exception );

  void generateConfigFiles()
    throw( xcept::Exception );

  void getEnv();

  string getRS3UserInfo()
    throw( xcept::Exception );

  string hidePw( const string& in )
    throw( xcept::Exception );

  string uploadDUCKFile( const string duckFileName )
    throw( xcept::Exception );

  string recreateMapping( std::vector<cgicc::FormEntry>& fev )
    throw( xcept::Exception );

  string setProcessingInstruction( const string XML, const string xslURI )
    throw( xcept::Exception );

  string insertStatuses( const string DUCK );

  string insertStatus( const string DUCK, const ProcessDescriptor* pd )
    throw( xcept::Exception );

  void createProcessDescriptors()
    throw( xcept::Exception );

  void assignJobControls();

  /// Creates a display web page.

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void displayWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void editorWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void dispatcherWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void selectorWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  /// Gets UTC date and time.

  ///
  ///
  /// @return UTC date and time in YYYYMMDD_hhmmss_UTC format
  ///
  string getDateTime();

};

}}

#endif
