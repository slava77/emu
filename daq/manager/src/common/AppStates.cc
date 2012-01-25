#include "emu/daq/manager/AppStates.h"

#include <sstream>
#include <iomanip>

using namespace std;

emu::daq::manager::AppStates::AppStates()
  : bSem_( toolbox::BSem::FULL )
{}

emu::daq::manager::AppStates::AppStates( const emu::daq::manager::AppStates& other )
  : bSem_        ( toolbox::BSem::EMPTY        )
  , timeOfUpdate_( other.getUnixTimeOfUpdate() )
  , appStates_   ( other.getAppStates()        )
{
  bSem_.give();
}

void
emu::daq::manager::AppStates::setAppState( xdaq::ApplicationDescriptor* ad, const string& state ){
  bSem_.take();
  appStates_[ad] = state;
  time( &timeOfUpdate_ );
  bSem_.give();
}

emu::daq::manager::AppStates&
emu::daq::manager::AppStates::operator=( const emu::daq::manager::AppStates& other ){
  if ( this == &other ) return *this;
  bSem_.take();
  timeOfUpdate_ = other.getUnixTimeOfUpdate();
  appStates_    = other.getAppStates();
  bSem_.give();
  return *this;
}

emu::daq::manager::AppStates&
emu::daq::manager::AppStates::operator=( const map<xdaq::ApplicationDescriptor*, string>& appStates ){
  bSem_.take();
  appStates_.clear();
  appStates_.insert( appStates.begin(), appStates.end() );
  time( &timeOfUpdate_ );
  bSem_.give();
  return *this;
}

set<xdaq::ApplicationDescriptor*>
emu::daq::manager::AppStates::getApps() const {
  set<xdaq::ApplicationDescriptor*> apps;
  map<xdaq::ApplicationDescriptor*, string>::const_iterator s;
  bSem_.take();
  for ( s=appStates_.begin(); s!=appStates_.end(); ++s ) apps.insert( s->first );
  bSem_.give();
  return apps;
}

set<xdaq::ApplicationDescriptor*>
emu::daq::manager::AppStates::getAppsInState( const string& state ) const {
  set<xdaq::ApplicationDescriptor*> apps;
  map<xdaq::ApplicationDescriptor*, string>::const_iterator s;
  bSem_.take();
  for ( s=appStates_.begin(); s!=appStates_.end(); ++s ){
    if ( s->second == state ) apps.insert( s->first );
  }
  bSem_.give();
  return apps;
}

string
emu::daq::manager::AppStates::getTimeOfUpdate() const {
  struct tm lt;
  localtime_r( &timeOfUpdate_, &lt ); // reentrant version for thread safety

  stringstream ss;
  ss << setfill('0') << setw(4) << lt.tm_year+1900 << "-"
     << setfill('0') << setw(2) << lt.tm_mon+1     << "-"
     << setfill('0') << setw(2) << lt.tm_mday      << " "
     << setfill('0') << setw(2) << lt.tm_hour      << ":"
     << setfill('0') << setw(2) << lt.tm_min       << ":"
     << setfill('0') << setw(2) << lt.tm_sec;
  return ss.str();
}

string
emu::daq::manager::AppStates::getCombinedState() const {
  // Combine states:
  // If one is failed, the combined state will also be failed.
  // Else, if one is unknown, the combined state will also be unknown.
  // Else, if all are known but not the same, the combined state will be indefinite.
  string combinedState("UNKNOWN");
  map<xdaq::ApplicationDescriptor*, string>::const_iterator s;
  bSem_.take();
  // First check if any failed:
  for ( s=appStates_.begin(); s!=appStates_.end(); ++s )
    if ( s->second == "Failed" ){
      combinedState = s->second;
      bSem_.give();
      return combinedState;
    }
  // If none failed:
  for ( s=appStates_.begin(); s!=appStates_.end(); ++s ){
    if ( s->second == "UNKNOWN" ){
      combinedState = s->second;
      break;
    }
    else if ( s->second != combinedState && combinedState != "UNKNOWN" ){
      combinedState = "INDEFINITE";
      break;
    }
    else{
      if ( s->second.find( "Mismatch", 0 ) != string::npos )
	// DAQ is still "enabled" while RU is seeing mismatch but has not timed out
	combinedState = "Enabled";
      else
	combinedState = s->second;
    }
  }
  bSem_.give();
  return combinedState;
}

time_t
emu::daq::manager::AppStates::getAgeInSeconds() const {
  bSem_.take();
  time_t ageInSec = getAgeInSec();
  bSem_.give();
  return ageInSec;
}

time_t
emu::daq::manager::AppStates::getAgeInSec() const {
  time_t ageInSec;
  time_t now;
  time( &now );
  ageInSec = now - timeOfUpdate_;
  return ageInSec;
}

bool
emu::daq::manager::AppStates::isEmpty() const {
  if ( appStates_.size() == 0 ) return true;
  bSem_.take();
  map<xdaq::ApplicationDescriptor*, string>::const_iterator s;  
  for ( s=appStates_.begin(); s!=appStates_.end(); ++s ){
    if ( s->second.size() == 0 ){
      bSem_.give();
      return true;
    }
  }
  bSem_.give();
  return false;
}

void
emu::daq::manager::AppStates::clear(){
  bSem_.take();
  appStates_.clear();
  bSem_.give();
}

ostream&
emu::daq::manager::operator<<( ostream& os, emu::daq::manager::AppStates& as ){
  as.bSem_.take();
  os << "Application states updated at " << as.getTimeOfUpdate()
     << " (" << as.timeOfUpdate_
     << "), " << as.getAgeInSec()
     << " seconds ago:" << endl;
  map<xdaq::ApplicationDescriptor*, string>::const_iterator s;  
  for ( s=as.appStates_.begin(); s!=as.appStates_.end(); ++s ){
    os << "   " << s->first->getClassName() << s->first->getInstance()
       << " " << s->second << endl;
  }
  as.bSem_.give();
  os << "Combined state is '" << as.getCombinedState() << "'" << endl;
  return os;
}
