#ifndef __EmuProcessDescriptor_h__
#define __EmuProcessDescriptor_h__

#include "xdaq/include/xdaq/ApplicationGroup.h"

#include "toolbox/net/URL.h"
#include "toolbox/net/exception/BadURL.h"
#include "toolbox/net/exception/MalformedURL.h"
// #include "toolbox/net/exception/UnresolvedAddress.h"

#include <string>
#include <set>
#include <time.h>

using namespace std;

class EmuProcessDescriptor;

/// \class EmuProcessDescriptor
/// \brief Describe Emu processes for bookkeeping in EmuFarmer.

class EmuProcessDescriptor{

public:

  /// default constructor
  EmuProcessDescriptor();

  /// constructor from URL

  ///
  /// @param url URL of the process
  ///
  EmuProcessDescriptor( const string& url ) throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL );

  /// copy constructor

  ///
  /// @param ep other instance
  ///
  EmuProcessDescriptor( const EmuProcessDescriptor& ep );

  /// destructor
  ~EmuProcessDescriptor();

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
  /// @param url JobControl's URL
  /// @param jcad JobControl's application descriptor
  ///
  void setJobControl( const string& url, xdaq::ApplicationDescriptor* jcad ){
    jobControlNormalizedURL_ = url;
    jobControlAppDescriptor_ = jcad;
  }

  /// get the normalized URL (http://ip_number:port) of the JobControl responsible for this process

  ///
  ///
  /// @return normalized URL of JobControl responsible for this process
  ///
  string getJobControlNormalizedURL() const { return jobControlNormalizedURL_; }

  /// set the name of the input device (e.g. /dev/schar2) used by this process

  ///
  /// @param name name of the input device
  ///
  void setDeviceName( const string& name ){ deviceName_ = name; }


  /// get the name of the input device (e.g. /dev/schar2) used by this process

  ///
  ///
  /// @return name of the input device
  ///  
  string getDeviceName(){ return deviceName_; }

  /// get application descriptor of JobControl responsible for this process
  ///
  ///
  /// @return application descriptor of JobControl responsible for this process
  ///
  xdaq::ApplicationDescriptor* getJobControlAppDescriptor() const { return jobControlAppDescriptor_; }

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

  /// set start time

  ///
  /// @param t start time
  ///
  void setStartTime( const time_t t ){ startTime_ = t; }

  /// get start time

  ///
  /// @return start time
  ///
  time_t getStartTime() const { return startTime_; }

  /// get the job id JobControl assigned this process

  ///
  /// @return job id that JobControl assigned this process
  ///
  int getJobId() const { return jobId_; }

  /// set the job id JobControl assigned this process

  ///
  /// @param jobId job id that JobControl assigned this process
  ///
  void setJobId( int jobId ){ jobId_ = jobId; }


  /// set the job id JobControl assigned this process

  ///
  /// @param jobId job id that JobControl assigned this process
  ///
  void setJobId( const string& jobId );

  /// get log level this process is to be started with

  ///
  /// @return log level
  ///
  string getStartingLogLevel() const { return startingLogLevel_; }

  /// set log level this process is to be started with

  ///
  /// @param startingLogLevel log level this process is to be started with
  ///
  void setStartingLogLevel( const string& startingLogLevel ){ startingLogLevel_ = startingLogLevel; }

  /// get current log level of this process

  ///
  /// @return current log level of this process
  ///
  string getLogLevel() const { return logLevel_; }

  /// set current log level of this process

  ///
  /// @param logLevel current log level of this process
  ///
  void setLogLevel( const string& logLevel ){ logLevel_ = logLevel; }

  /// get name and instance of applications

  ///
  /// @return name and instance of applications
  ///
  set< pair<string, int> > getApplications() const { return applications_; }

  /// add application run by this process

  /// @param name application name
  /// @param instance application instance
  ///
  void addApplication( const string& name, const int instance ){ applications_.insert( make_pair(name, instance) ); }

  /// TRUE if this process runs the given application
  ///
  /// @param name name of application
  ///
  /// @return TRUE if this process runs the given application
  ///
  bool hasApplication( const string& name ) const;

  /// add all applications of another process to this one

  /// @param ep descriptor of other process
  ///
  void addApplicationsFrom( const EmuProcessDescriptor& ep ){ applications_.insert( ep.applications_.begin(), ep.applications_.end() ); }

  /// print summary of this process (for debugging purposes only)

  ///
  /// @param os print destination
  ///
  void print( ostream& os ) const;

  /// assignment operator

  /// @param ep other process descriptor
  ///
  /// @return this process descriptor
  ///
  EmuProcessDescriptor& operator=( const EmuProcessDescriptor& ep );

  /// less-than operator comparing IP numbers

  /// @param epd other process descriptor
  ///
  /// @return TRUE if this IP number < other process's IP number
  ///
  bool operator<( const EmuProcessDescriptor& epd ) const;


  /// equality operator comparing IP numbers

  /// @param epd other process descriptor
  ///
  /// @return TRUE if this IP number equals other process's IP number
  ///
  bool operator==( const EmuProcessDescriptor& epd ) const;

private:
  /// URL (http://host:port) of this process
  string URL_;
  
  /// the resolved URL (http://ip_number:port)
  string normalizedURL_;

  
  /// the log level for starting the process
  string startingLogLevel_;

  /// the current log level obtained by querying the executive
  string logLevel_;

  ///the (resolved) URL (http://ip_number:port) of the JobControl app in charge of this process
  string jobControlNormalizedURL_;

  /// the device name for the DDU driver
  string deviceName_;

  /// the ApplicationDescriptor of the JobControl app in charge of this process
  xdaq::ApplicationDescriptor* jobControlAppDescriptor_;

  /// whether or not this process is selected for action on the web page
  bool   selected_;

  /// job id assigned by JobControl
  int    jobId_;

  /// start time
  time_t startTime_;

  /// name and instance of applications
  set< pair<string, int> > applications_;
  
};

#endif
