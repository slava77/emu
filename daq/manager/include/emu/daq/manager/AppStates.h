#ifndef __emu_daq_manager_AppStates_h__
#define __emu_daq_manager_AppStates_h__

#include "toolbox/BSem.h"
#include "xdaq/ApplicationDescriptor.h"

#include <string>
#include <ostream>
#include <map>
#include <set>
#include <utility>
#include <time.h>

using namespace std;

namespace emu { namespace daq { namespace manager {

  class AppStates {
  public:
    friend ostream& operator<<( ostream& os,  emu::daq::manager::AppStates& as );
    
    AppStates();
    AppStates( const emu::daq::manager::AppStates& other );
    void setAppState( xdaq::ApplicationDescriptor* ad, const string& state );
    emu::daq::manager::AppStates& operator=( const emu::daq::manager::AppStates& other );
    emu::daq::manager::AppStates& operator=( const map<xdaq::ApplicationDescriptor*, string>& appStates );
    map<xdaq::ApplicationDescriptor*, string> getAppStates() const { return appStates_; }
    time_t getUnixTimeOfUpdate() const { return timeOfUpdate_; }
    string getTimeOfUpdate() const;
    set<xdaq::ApplicationDescriptor*> getApps() const;
    set<xdaq::ApplicationDescriptor*> getAppsInState( const string& state ) const;
    string getCombinedState() const;
    time_t getAgeInSeconds() const;
    
    template <class Iterator>
    void insertApps( Iterator begin, Iterator end ){
      bSem_.take();
      for ( Iterator i=begin; i!=end; ++i ){
	appStates_[*i] = "";
      }
      bSem_.give();
    }
    
  private:
    mutable toolbox::BSem bSem_; // exempt from constness
    time_t timeOfUpdate_;
    map<xdaq::ApplicationDescriptor*, string> appStates_;
  };
  
}}} // namespace emu::daq::manager

#endif
