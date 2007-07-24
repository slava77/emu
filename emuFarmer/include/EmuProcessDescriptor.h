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

class EmuProcessDescriptor{

public:
  EmuProcessDescriptor();
  EmuProcessDescriptor( const string& url ) throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL );
  EmuProcessDescriptor( const EmuProcessDescriptor& ep );
  ~EmuProcessDescriptor();
  void setURL( const string& url ) throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL );
  string getURL() const { return URL_; };
  string getNormalizedURL() const { return normalizedURL_; };
  string getHost() const;
  string getNormalizedHost() const;
  int getPort() const;
  void setJobControl( const string& url, xdaq::ApplicationDescriptor* jcad ){
    jobControlNormalizedURL_ = url;
    jobControlAppDescriptor_ = jcad;
  }
  string getJobControlNormalizedURL() const { return jobControlNormalizedURL_; }
  void setDeviceName( const string& name ){ deviceName_ = name; }
  string getDeviceName(){ return deviceName_; }
  xdaq::ApplicationDescriptor* getJobControlAppDescriptor() const { return jobControlAppDescriptor_; }
  void setSelected( bool s=true ){ selected_ = s; }
  bool isSelected() const { return selected_; }
  void setStartTime( const time_t t ){ startTime_ = t; }
  time_t getStartTime() const { return startTime_; }
  int getJobId() const { return jobId_; }
  void setJobId( int jobId ){ jobId_ = jobId; }
  void setJobId( const string& jobId );
  string getStartingLogLevel() const { return startingLogLevel_; }
  void setStartingLogLevel( const string& startingLogLevel ){ startingLogLevel_ = startingLogLevel; }
  string getLogLevel() const { return logLevel_; }
  void setLogLevel( const string& logLevel ){ logLevel_ = logLevel; }
  set< pair<string, int> > getApplications() const { return applications_; }
  void addApplication( const string& name, const int instance ){ applications_.insert( make_pair(name, instance) ); }
  bool hasApplication( const string& name ) const;
  void addApplicationsFrom( const EmuProcessDescriptor& ep ){ applications_.insert( ep.applications_.begin(), ep.applications_.end() ); }
  void print( ostream& os ) const;
  EmuProcessDescriptor& operator=( const EmuProcessDescriptor& ep );
  bool operator<( const EmuProcessDescriptor& epd ) const;
  bool operator==( const EmuProcessDescriptor& epd ) const;

private:
  string URL_;
  /**
   * the resolved URL
   */
  string normalizedURL_;
  /**
   * the log level for starting the process
   */
  string startingLogLevel_;
  /**
   * the current log level obtained by querying the executive
   */
  string logLevel_;
  /**
   * the (resolved) URL of the JobControl app in charge of this process
   */
  string jobControlNormalizedURL_;
  /**
   * the device name for the DDU driver
   */
  string deviceName_;
  /**
   * the ApplicationDescriptor of the JobControl app in charge of this process
   */
  xdaq::ApplicationDescriptor* jobControlAppDescriptor_;
  /**
   * whether or not this process is selected for action on the web page
   */
  bool   selected_;

  int    jobId_;
  time_t startTime_;
  set< pair<string, int> > applications_;
  
};

#endif
