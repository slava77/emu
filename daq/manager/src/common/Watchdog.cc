#include "emu/daq/manager/Watchdog.h"
//#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"
#include <algorithm>

emu::daq::manager::Watchdog::Watchdog( emu::daq::manager::Application *owner )
  : application_( owner ){
  insertAllApps( currentAppStates_ );
}

void
emu::daq::manager::Watchdog::insertAllApps( emu::daq::manager::AppStates& appStates ){
  std::cout << "[emu::daq::manager::Watchdog::insertAllApps] application_->buildEvents_.value_ = " << application_->buildEvents_.value_ << std::endl; std::cout.flush();
  if ( application_->buildEvents_.value_ ){ 
    appStates.insertApps( application_->evmDescriptors_.begin(), application_->evmDescriptors_.end() );
    appStates.insertApps( application_->buDescriptors_ .begin(), application_->buDescriptors_ .end() );
    appStates.insertApps( application_->ruDescriptors_ .begin(), application_->ruDescriptors_ .end() );
    appStates.insertApps( application_->fuDescriptors_ .begin(), application_->fuDescriptors_ .end() );
  }
  appStates.insertApps( application_->taDescriptors_ .begin(), application_->taDescriptors_ .end() );
  appStates.insertApps( application_->ruiDescriptors_.begin(), application_->ruiDescriptors_.end() );
}

void
emu::daq::manager::Watchdog::patrol(){

  emu::soap::Messenger m( application_ );

  // Make sure it's only updated if currentAppStates_ is not empty. (It may be empty during configuring, for instance.)
  if ( ! currentAppStates_.isEmpty() ) previousAppStates_ = currentAppStates_;

  std::set<xdaq::ApplicationDescriptor*> apps = currentAppStates_.getApps();
  std::set<xdaq::ApplicationDescriptor*>::iterator a;
  for ( a=apps.begin(); a!=apps.end(); ++a ){
    xdata::String s( "UNKNOWN" );
    try{
      m.getParameters( *a, emu::soap::Parameters().add( "stateName", &s ) );
    }
    catch(xcept::Exception &e){
      s = "UNKNOWN";
      std::stringstream ss;
      ss << "Failed to get state of " << (*a)->getClassName() << "." << (*a)->getInstance() << " : " ;
      LOG4CPLUS_WARN(application_->logger_, ss.str() + xcept::stdformat_exception_history(e));
      XCEPT_DECLARE_NESTED( emu::daq::manager::exception::Exception, eObj, ss.str(), e );
      application_->notifyQualified( "warning", eObj );
    }
    currentAppStates_.setAppState( *a, s.toString() );
  }
    //cout << "Previous " << previousAppStates_;
    //cout << "Current "  << currentAppStates_;
}

set<xdaq::ApplicationDescriptor*>
emu::daq::manager::Watchdog::getAppsInStates( const std::vector<std::string>& states ) const {
  // The collection of apps that have been in <state> since the previous patrol
  set<xdaq::ApplicationDescriptor*> apps;

  // No reliable conclusion until both previous and current app states are meaningful:
  if ( currentAppStates_.isEmpty() || previousAppStates_.isEmpty() ) return apps;

  // All the apps that are in any of the states given by the argument:
  set<xdaq::ApplicationDescriptor*> prevApps;
  set<xdaq::ApplicationDescriptor*> currApps;
  // Loop over all state names:
  for ( vector<string>::const_iterator s=states.begin(); s!=states.end(); ++s ){
    set<xdaq::ApplicationDescriptor*> pa = previousAppStates_.getAppsInState( *s );
    set<xdaq::ApplicationDescriptor*> ca =  currentAppStates_.getAppsInState( *s );
    // Add apps that are in this state:
    prevApps.insert( pa.begin(), pa.end() );
    currApps.insert( ca.begin(), ca.end() );
  }

  // Collect those that are in both sets (previous and current) (set_intersection is in <algorithm>):
  set_intersection( prevApps.begin(), prevApps.end(),
		    currApps.begin(), currApps.end(),
		    std::insert_iterator< set<xdaq::ApplicationDescriptor*> >( apps, apps.begin()) );
  return apps;
}

ostream&
emu::daq::manager::operator<<( ostream& os,  emu::daq::manager::Watchdog& wd ){
  os << "Previous:" << endl << wd.previousAppStates_ << endl 
     << "Current :" << endl << wd.currentAppStates_  << endl; 
  return os;
}
