#ifndef __emu_farmer_ProcessDescriptor_h__
#define __emu_farmer_ProcessDescriptor_h__

#include "xdaq/ApplicationGroup.h"

#include "toolbox/net/URL.h"
#include "toolbox/net/exception/BadURL.h"
#include "toolbox/net/exception/MalformedURL.h"
// #include "toolbox/net/exception/UnresolvedAddress.h"

#include <string>
#include <set>
#include <map>
#include <time.h>

using namespace std;

namespace emu { namespace farmer {

class ProcessDescriptor;

/// \class ProcessDescriptor
/// \brief Describe Emu processes for bookkeeping in EmuFarmer.

class ProcessDescriptor{

public:

  /// default constructor
  ProcessDescriptor();

  /// constructor from URL

  ///
  /// @param url URL of the process
  ///
  ProcessDescriptor( const string& url ) throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL );

  /// destructor
  ~ProcessDescriptor();

  /// set NAME of this process

  ///
  /// @param name name
  ///
  void setName( const string& name ){ name_ = name; }

  /// get Name of this process

  ///
  ///
  /// @return Name of this process
  ///
  string getName() const { return name_; };

  /// set URL of this process

  ///
  /// @param url URL
  ///
  void setURL( const string& url ) throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL );

  /// get URL of this process

  ///
  ///
  /// @return URL of this process
  ///
  string getURL() const { return URL_; };

  /// get normalized URL (http://ip_number:port) of this process

  ///
  ///
  /// @return normalized URL of this process
  ///
  string getNormalizedURL() const { return normalizedURL_; };

  /// get path of this process

  ///
  ///
  /// @return local application id number of this process
  ///
  int getLid() const;

  /// get path of this process

  ///
  ///
  /// @return path of this process
  ///
  string getPath() const;

  /// get host of this process

  ///
  ///
  /// @return host of this process
  ///
  string getHost() const;


  /// get normalized host (IP number) of this process

  ///
  ///
  /// @return normalized host of this process
  ///
  string getNormalizedHost() const;


  /// get port of this process

  ///
  ///
  /// @return port of this process
  ///
  int getPort() const;


  /// set the JobControl responsible for this process

  ///
  /// @param jobControlURI JobControl's URI (http://host:port/appURN)
  ///
  void setJobControlURI( const string& jobControlURI ){ jobControlURI_ = jobControlURI; }

  /// set the JobControl responsible for this process

  ///
  /// @param url JobControl's URL
  /// @param jcad JobControl's application descriptor
  ///
  string getJobControlURI(){ return jobControlURI_; }


  /// get the normalized URL (http://ip_number:port) of the JobControl responsible for this process

  ///
  ///
  /// @return normalized URL of JobControl responsible for this process
  ///
  string getJobControlNormalizedURL() const { return jobControlNormalizedURL_; }

  /// set the name of the user to run this process

  ///
  /// @param name user to run this process
  ///
  void setUser( const string& user ){ user_ = user; }

  /// get the name of the user to run this process

  ///
  /// @return user to run this process
  ///
  string getUser() const { return user_; }



  /// set the full path to the XDAQ configuration XML file

  ///
  /// @param xdaqConfigPath full path to the XDAQ configuration XML file
  ///
  void setXdaqConfigPath( const string& xdaqConfigPath ){ xdaqConfigPath_ = xdaqConfigPath; }

  /// get the full path to the XDAQ configuration XML file

  ///
  /// @return full path to the XDAQ configuration XML file
  ///
  string getXdaqConfigPath() const { return xdaqConfigPath_; }


  /// set status of this process 'selected' (i.e., by the user on the web page)

  ///
  /// @param s TRUE if the user selected this process
  ///
  void setSelected( bool s=true ){ selected_ = s; }

  /// whether or not the status of this process is 'selected' (i.e., by the user on the web page)

  ///
  /// @return TRUE if the user selected this process
  ///
  bool isSelected() const { return selected_; }

  /// get the job id JobControl assigned this process

  ///
  /// @return job id that JobControl assigned this process
  ///
  string getJid() const;

//   /// set the job id JobControl assigned this process

//   ///
//   /// @param jid job id that JobControl assigned this process
//   ///
//   void setJid( const string jid ){ jid_ = jid; }


  /// get current state of this process

  ///
  /// @return current state of this process
  ///
  string getState() const { return state_; }

  /// set current state of this process

  ///
  /// @param state current state of this process
  ///
  void setState( const string& state ){ state_ = state; }

  /// get environment string of this process

  ///
  /// @return environment variables of this process
  ///
  map<string,string> getEnvironmentVariables() const { return environmentVariables_; }

  /// set environment string of this process

  ///
  /// @param environmentString environment string of this process
  ///
  void setEnvironmentString( const string& environmentString );


  /// print summary of this process (for debugging purposes only)

  ///
  /// @param os print destination
  ///
  void print( ostream& os ) const;


  /// less-than operator comparing IP numbers

  /// @param epd other process descriptor
  ///
  /// @return TRUE if this IP number < other process's IP number
  ///
  bool operator<( const ProcessDescriptor& epd ) const;


  /// equality operator comparing IP numbers

  /// @param epd other process descriptor
  ///
  /// @return TRUE if this IP number equals other process's IP number
  ///
  bool operator==( const ProcessDescriptor& epd ) const;

private:
  /// name of this process
  string name_;
  
  /// URL (http://host:port[/path]) of this process
  string URL_;
  
  /// the resolved URL (http://ip_number:port)
  string normalizedURL_;
  
  /// the state obtained by querying the executive [OFF|Initial] or application [Halted|Configured|Enabled]
  string state_;

  ///the (resolved) URL (http://ip_number:port) of the JobControl app in charge of this process
  string jobControlNormalizedURL_;

  ///the URL (http://ip_number:port/appURN) of the JobControl app in charge of this process
  string jobControlURI_;

  /// the environment string for this process
  string environmentString_;

  /// the user to run this process
  string user_;

  /// the full path to the XDAQ configuration XML file
  string xdaqConfigPath_;

  /// whether or not this process is selected for action on the web page
  bool   selected_;

//   /// job id assigned by JobControl
//   string jid_;
  
  /// name and value of environment variables
  map<string,string> environmentVariables_;
  
};

}}

#endif
