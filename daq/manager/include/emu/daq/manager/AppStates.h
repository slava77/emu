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
    set<xdaq::ApplicationDescriptor*> getApps() const;
    set<xdaq::ApplicationDescriptor*> getAppsInState( const string& state ) const;
    time_t getAgeInSeconds() const;
    string getCombinedState() const;
    bool isEmpty() const; /// Empty if it contains no apps or any app it contains has no state assigned to it.
    void clear();
    
    template <class Iterator>
    void insertApps( Iterator begin, Iterator end ){
      bSem_.take();
      for ( Iterator i=begin; i!=end; ++i ){
	appStates_[*i] = "";
      }
      bSem_.give();
    }
    
  private:
    string getTimeOfUpdate() const; /// Not thread safe. Only invoke it from within thread-safe methods.
    time_t getAgeInSec() const; /// Not thread safe. Only invoke it from within thread-safe methods.

    mutable toolbox::BSem bSem_; // exempt from constness
    time_t timeOfUpdate_;
    map<xdaq::ApplicationDescriptor*, string> appStates_;
  };

  ostream& operator<<( ostream& os, emu::daq::manager::AppStates& as );

}}} // namespace emu::daq::manager

#endif
