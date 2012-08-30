#include "emu/step/Manager.h"

#include "emu/step/version.h"
#include "emu/step/Test.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"
#include "emu/utils/Cgi.h"

#include "toolbox/task/WorkLoopFactory.h"
#include "xcept/tools.h"

#include <sys/time.h>

#include <cmath>

using namespace emu::utils;

emu::step::Manager::Manager( xdaq::ApplicationStub *s )
  : xdaq::WebApplication( s )
  , emu::step::Application( s )
  , configuration_( NULL )
{
  exportParameters();
  bindWebInterface();
  testSequenceSignature_ = toolbox::task::bind( this, &emu::step::Manager::testSequenceInWorkLoop, "testSequenceInWorkLoop" );
  workLoopName_ = getApplicationDescriptor()->getClassName() + "." 
    + emu::utils::stringFrom<int>( getApplicationDescriptor()->getInstance() );
}

void emu::step::Manager::exportParameters(){
  xdata::InfoSpace *s = getApplicationInfoSpace();
  s->fireItemAvailable( "testParametersFileName"    , &testParametersFileName_     );
  s->fireItemAvailable( "specialVMESettingsFileName", &specialVMESettingsFileName_ );
  // s->fireItemAvailable( "", &_ );
}

void emu::step::Manager::bindWebInterface(){
  xgi::bind( this, &emu::step::Manager::defaultWebPage, "Default" );
  xgi::bind( this, &emu::step::Manager::controlWebPage, "control" );
}


void emu::step::Manager::createConfiguration(){
  set<xdaq::ApplicationDescriptor *> apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors( "emu::step::Tester" );

  if ( apps.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No emu::step::Tester applications found." );
  }

  map<string,string> pCrateSettingsFileNames;

  emu::soap::Messenger m( this );
  for ( std::set<xdaq::ApplicationDescriptor*>::iterator app = apps.begin(); app != apps.end(); ++app ) {
    xdata::String group;
    xdata::String fileName;
    m.getParameters( *app, emu::soap::Parameters()
		     .add( "group", &group )
		     .add( "vmeSettingsFileName", &fileName )
		     );
    testerDescriptors_     [group.toString()] = *app;
    pCrateSettingsFileNames[group.toString()] = fileName.toString();
    LOG4CPLUS_INFO( logger_, "Found " << (*app)->getClassName() << " instance " << (*app)->getInstance() << " for group '" << group.toString() << "' with settings in " << fileName.toString() );
  }

  // cout << pCrateSettingsFileNames << endl;

  configuration_ = new emu::step::Configuration( namespace_, 
						 testParametersFileName_,
						 pCrateSettingsFileNames );
}

void emu::step::Manager::configureAction(toolbox::Event::Reference e){
  LOG4CPLUS_INFO( logger_, "emu::step::Manager::configureAction" );

  configuration_->setTestStatus( "idle" );

  // Remove leftover work loop, if any...
  // if ( workLoop_->isActive() ){
  //   try{
  //     workLoop_->cancel();
  //     LOG4CPLUS_INFO( logger_, "Cancelled work loop." );
  //   } catch( xcept::Exception &e ){
  //     XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to cancel work loop.", e );
  //   }
  // }
  try{
    list<toolbox::task::WorkLoop*> workloops = toolbox::task::getWorkLoopFactory()->getWorkLoops();
    LOG4CPLUS_DEBUG( logger_, "Found " << workloops.size() << " work loops." );
    for ( list<toolbox::task::WorkLoop*>::const_iterator wl = workloops.begin(); wl != workloops.end(); ++ wl ){
      LOG4CPLUS_DEBUG( logger_, "Found " << (*wl)->getType() << " work loop " << (*wl)->getName() );
    }
    // It's tricky to find a work loop by name as 
    // toolbox::task::WorkLoopFactory::getWorkLoop(const std::string & name, const std::string & type)
    // creates a work loop named '<name>/<type>' for some reason.
    // Let's just try to remove it blindly instead, and catch the exception it throws if no such work loop exists.
    toolbox::task::getWorkLoopFactory()->removeWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_DEBUG( logger_, "Removed " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    LOG4CPLUS_WARN( logger_, "Failed to remove leftover work loop: " << xcept::stdformat_exception_history( e ) );
  }
  //...and get a new one
  try{
    workLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_INFO( logger_, "Created " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    stringstream ss;
    ss << "Failed recreate " << workLoopType_ << " work loop " << workLoopName_;
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, ss.str(), e );
  }

  // Schedule test procedure to be executed in a separate thread
  try{
    workLoop_->submit( testSequenceSignature_ );
    LOG4CPLUS_INFO( logger_, "Submitted tes action to " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to submit test action to work loop.", e );
  }

}

void emu::step::Manager::enableAction(toolbox::Event::Reference e){
  LOG4CPLUS_DEBUG( logger_, "emu::step::Manager::enableAction" );
  // Execute the sequence of tests in a separate thread:
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop.", e );
    }
  }
}

void emu::step::Manager::haltAction(toolbox::Event::Reference e){
  LOG4CPLUS_DEBUG( logger_, "emu::step::Manager::haltAction" );

  emu::soap::Messenger m( this );

  try{
    m.sendCommand( "emu::step::Tester", "Halt" );
    LOG4CPLUS_INFO( logger_, "Sent 'Halt' command to emu::step::Tester applications." );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to send 'Halt' command to emu::step::Tester applications", e );
  }

  try{
    m.sendCommand( "emu::daq::manager::Application", "Halt" );
    LOG4CPLUS_INFO( logger_, "Sent 'Halt' command to emu::daq::manager::Application" );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to send 'Halt' command to emu::daq::manager::Application", e );
  }

  // stopAction( e );

  // // Remove work loop
  // try{
  //   list<toolbox::task::WorkLoop*> workloops = toolbox::task::getWorkLoopFactory()->getWorkLoops();
  //   LOG4CPLUS_DEBUG( logger_, "Found " << workloops.size() << " work loops." );
  //   for ( list<toolbox::task::WorkLoop*>::const_iterator wl = workloops.begin(); wl != workloops.end(); ++ wl ){
  //     LOG4CPLUS_DEBUG( logger_, "Found " << (*wl)->getType() << " work loop " << (*wl)->getName() );
  //   }
  //   // It's tricky to find a work loop by name as 
  //   // toolbox::task::WorkLoopFactory::getWorkLoop(const std::string & name, const std::string & type)
  //   // creates a work loop named '<name>/<type>' for some reason.
  //   // Let's just try to remove it blindly instead, and catch the exception it throws if no such work loop exists.
  //   toolbox::task::getWorkLoopFactory()->removeWorkLoop( workLoopName_, workLoopType_ );
  //   LOG4CPLUS_INFO( logger_, "Removed " << workLoopType_ << " work loop " << workLoopName_ );
  // } catch( xcept::Exception &e ){
  //   LOG4CPLUS_WARN( logger_, "Failed to remove work loop: " << xcept::stdformat_exception_history( e ) );
  // }

}

void emu::step::Manager::stopAction(toolbox::Event::Reference e){
  LOG4CPLUS_DEBUG( logger_, "emu::step::Manager::stopAction" );

  emu::soap::Messenger m( this );

  try{
    m.sendCommand( "emu::step::Tester", "Stop" );
    LOG4CPLUS_INFO( logger_, "Sent 'Stop' command to emu::step::Tester applications." );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to send 'Stop' command to emu::step::Tester applications.", e );
  }
	  
  // // Just stop the task of the work loop without removing the scheduled method
  // if ( workLoop_->isActive() ){
  //   try{
  //     workLoop_->cancel();
  //     LOG4CPLUS_INFO( logger_, "Cancelled work loop." );
  //   } catch( xcept::Exception &e ){
  //     XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to cancel work loop.", e );
  //   }
  // }
}

bool emu::step::Manager::testSequenceInWorkLoop( toolbox::task::WorkLoop *wl ){

  xdata::Vector<xdata::String> testIds = configuration_->getTestIds();

  emu::soap::Messenger m( this );

  //
  // Set parameters in Tester apps
  //
  for ( map<string,xdaq::ApplicationDescriptor*>::const_iterator t = testerDescriptors_.begin(); t != testerDescriptors_.end(); ++t ){
    if ( t->second ){
      try{
	xdata::Vector<xdata::String> crateIds = configuration_->getCrateIds( t->first );
	m.setParameters( t->second, 
			 emu::soap::Parameters()
			 .add( "testParametersFileName"    , &testParametersFileName_     )
			 .add( "specialVMESettingsFileName", &specialVMESettingsFileName_ )
			 .add( "crateIds"                  , &crateIds                    )
			 );
      }
      catch ( xcept::Exception& e ){
	stringstream ss;
	ss << "Failed to set parameters in " << t->second->getClassName() 
	   << "." << t->second->getInstance() 
	   << " for " << t->first;
	XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
	LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
	moveToFailedState( ee );
	return false;
      }
    } // if ( t->second )
  } // for ( map<string,xdaq::ApplicationDescriptor*>::const_iterator t = testerDescriptors_.begin(); t != testerDescriptors_.end(); ++t )
    
  if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.


  //
  // Loop over tests and execute them
  //
  for ( size_t iTest = 0; iTest < testIds.elements(); iTest++ ){

    xdata::String testId = *dynamic_cast<xdata::String*>( testIds.elementAt( iTest ) );;
    emu::step::TestParameters testParameters( testId.toString(), configuration_->getTestParametersXML(), &logger_ );
    bsem_.take();
    isCurrentTestPassive_ = ( testParameters.getNEvents() > 0 );
    bsem_.give();

    try{
      //
      // Set test id in all Tester apps
      //
      m.setParameters( "emu::step::Tester", emu::soap::Parameters().add( "testId", &testId ) );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Configure all Tester apps
      //
      xdata::String             runType = string( ( (bool) isCurrentTestPassive_ ) ? "STEP_" : "Test_" ) + testId.toString();
      xdata::Integer  maxNumberOfEvents = -1; // unlimited if negative
      xdata::Boolean writeBadEventsOnly = false;
      m.setParameters( "emu::daq::manager::Application", 
		       emu::soap::Parameters()
		       .add( "runType"           , &runType            )
		       .add( "maxNumberOfEvents" , &maxNumberOfEvents  )
		       .add( "writeBadEventsOnly", &writeBadEventsOnly ) );
      m.sendCommand( "emu::daq::manager::Application", "Configure" );      
      m.sendCommand( "emu::step::Tester", "Configure" );
      waitForDAQToExecute( "Configure", 3 );
      configuration_->setTestStatus( testId, "running" );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Enable all Tester apps
      //
      m.sendCommand( "emu::daq::manager::Application", "Enable" );
      waitForDAQToExecute( "Enable", 3 );
      m.sendCommand( "emu::step::Tester", "Enable" );
      waitForTestsToFinish( (bool) isCurrentTestPassive_ );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Halt all Tester apps
      //
      m.sendCommand( "emu::step::Tester", "Halt" );
      m.sendCommand( "emu::daq::manager::Application", "Halt" );
      waitForDAQToExecute( "Halt", 3 );
      configuration_->setTestStatus( testId, "done" );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
    }
    catch ( xcept::Exception& e ){
      stringstream ss;
      ss << "Test " << testId.toString() << " (" << iTest+1 << "/" << testIds.elements() << ") failed";
      XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
      LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
      moveToFailedState( ee );
      return false;
    }
  } // for ( size_t iTest = 0; iTest < testIds.elements(); iTest++ ){
  
  return false;
}

void emu::step::Manager::defaultWebPage(xgi::Input *in, xgi::Output *out)
//throw (xgi::exception::Exception)
{
  bsem_.take();
  try{
    if ( configuration_ == NULL ) createConfiguration();
    *out << createXMLWebPage();
  } catch( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to create default web page.", e );
  } catch( std::exception& e ){
    bsem_.give();
    XCEPT_RAISE( xgi::exception::Exception, string( "Failed to create default web page: ") + e.what() );
  } catch( ... ){
    bsem_.give();
    XCEPT_RAISE( xgi::exception::Exception, "Failed to create default web page. Unknown exception." );
  }
  bsem_.give();
}

void emu::step::Manager::controlWebPage(xgi::Input *in, xgi::Output *out)
//throw (xgi::exception::Exception)
{
  bsem_.take();
  cgicc::Cgicc cgi( in );
  vector<cgicc::FormEntry> fev = cgi.getElements();

  //
  // Find out what to do:
  //

  multimap<string,string> action;
  try{
    // Drive FSM?
    action = emu::utils::selectFromQueryString( fev, "^fsm$" );
    if ( action.size() == 1 ){

      multimap<string,string> modTime = emu::utils::selectFromQueryString( fev, "^modificationTime$" );
      string receivedModTime( modTime.size() ? modTime.begin()->second : "" );

      if ( action.begin()->second.compare( "Configure" ) == 0 ){

	if ( fsm_.getCurrentState() == 'H' ){
	  // Configure only if the local and remote configurations have the same unique id:
	  if ( configuration_->getModificationTime() == receivedModTime ){
	    multimap<string,string> selection = emu::utils::selectFromQueryString( fev, "^/" );
	    configuration_->setSelection( selection );
	    fireEvent( "Configure" );
	  }
	  else{
	    LOG4CPLUS_WARN( logger_, "Refused to configure due to an attempt to change a configuration of modification time " << receivedModTime << " while the current configuration's modification time is " << configuration_->getModificationTime() );
	  }
	}

      }
      else if ( action.begin()->second.compare( "Enable" ) == 0 ){

	if ( fsm_.getCurrentState() == 'C' ){
	  fireEvent( "Enable" );
	}

      }
      else if ( action.begin()->second.compare( "Halt" ) == 0 ){

	if ( fsm_.getCurrentState() == 'C' || fsm_.getCurrentState() == 'E' ){
	  fireEvent( "Halt" );
	}

      }
      else if ( action.begin()->second.compare( "Stop" ) == 0 ){

	if ( fsm_.getCurrentState() == 'E' ){
	  fireEvent( "Stop" );
	}

      }
      else if ( action.begin()->second.compare( "Reset" ) == 0 ){

	if ( fsm_.getCurrentState() == 'F' ){
	  fireEvent( "Reset" );
	}

      }
    }

  } catch ( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xgi::exception::Exception, string("Failed to execute ") + action.begin()->second, e );
  }

  emu::utils::redirectTo( applicationURLPath_, out );

  bsem_.give();

  return;
}

string emu::step::Manager::createXMLWebPage(){
  stringstream ss;
  if ( applicationURLPath_.size() == 0 ){
    ss << "/urn:xdaq-application:lid=" << getApplicationDescriptor()->getLocalId();
    applicationURLPath_ = ss.str();
    ss.str("");
  }

  ss << "<?xml version=\"1.0\"?>" << endl
     << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/step/html/Manager_XSLT.xml\"?>" << endl 
     << "<es:Manager xmlns:es=\""               << namespace_ 
     <<       "\" htmlDir=\""                   << "/emu/step/html" 
     <<       "\" softwareVersion=\""           << emustep::versions 
     <<       "\">"                             << endl;

  // Application info
  ss << "  <es:application urlPath=\""  << applicationURLPath_ 
     <<                "\" dateTime=\"" << emu::utils::getDateTime()
     <<                "\" state=\""    << fsm_.getStateName( fsm_.getCurrentState() )
     <<                "\">" << endl;

  // Message, if any
  if ( fsm_.getCurrentState() == 'F' ){
    ss << "    <es:message>" << reasonForFailure_.toString() << "</es:message>" << endl;
  }

  ss << "  </es:application>" << endl;
  ss << "</es:Manager>";

  updateProgress();
  return emu::utils::appendToSelectedNode( ss.str(), "/es:Manager", configuration_->getXML() );
}

void emu::step::Manager::updateProgress(){
  map<string,double> groupsProgress;
  emu::soap::Messenger m( this );
  xdata::Double progress;
  for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app )
    {
      m.getParameters( app->second, emu::soap::Parameters().add( "progress", &progress ) );
      groupsProgress[app->first] = double( progress );
    }
  // LOG4CPLUS_INFO( logger_, "Progress: " << groupsProgress );
  configuration_->setTestProgress( groupsProgress );
}

bool emu::step::Manager::waitForDAQToExecute( const string command, const uint64_t seconds ){
  string expectedState;
  if      ( command == "Configure" ){ expectedState = "Ready";   }
  else if ( command == "Enable"    ){ expectedState = "Enabled"; }
  else if ( command == "Halt"      ){ expectedState = "Halted";  }
  else                              { return true; }

  // Poll, and return TRUE if and only if DAQ gets into the expected state before timeout.
  emu::soap::Messenger m( this );
  xdata::String  daqState;
  for ( uint64_t i=0; i<=seconds; ++i ){
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "daqState", &daqState ) );
    if ( daqState.toString() != "Halted"  && daqState.toString() != "Ready" && 
	 daqState.toString() != "Enabled" && daqState.toString() != "INDEFINITE" ){
      LOG4CPLUS_ERROR( logger_, "Local DAQ is in " << daqState.toString() << " state. Please destroy and recreate local DAQ." );
      return false;
    }
    if ( daqState.toString() == expectedState ){ return true; }
    LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for local DAQ to get " 
		    << expectedState << ". It is still in " << daqState.toString() << " state." );
    ::sleep( 1 );
  }

  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		   << ". It is in " << daqState.toString() << " state." );
  return false;
}

// bool emu::step::Manager::waitForTestersToExecute( const string command, const uint64_t seconds ){
//   string expectedState;
//   if      ( command == "Configure" ){ expectedState = "Configured";   }
//   else if ( command == "Enable"    ){ expectedState = "Enabled"; }
//   else if ( command == "Halt"      ){ expectedState = "Halted";  }
//   else                              { return true; }

//   // Poll, and return TRUE if and only if a Tester gets into the expected state before timeout.
//   emu::soap::Messenger m( this );
//   xdata::String state;
//   uint64_t i = 0;
//   for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin();
// 	app != testerDescriptors_.end() && i <= seconds; 
// 	++app, ++i ){
//     m.getParameters( app->second, emu::soap::Parameters().add( "state", &state ) );
//     if ( state.toString() != "Halted"  && state.toString() != "Configured" && state.toString() != "Enabled" ){
//       LOG4CPLUS_ERROR( logger_, app->second->getClassName() << " instance " << app->second->getInstance() 
// 		       << " (PCrate group '" << app->first << "') "
// 		       << " is in " << state.toString() << " state." );
//       return false;
//     }
//     if ( state.toString() == expectedState ){ return true; }
//     ::sleep( 1 );
//   }

//   LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for the emu::step::Tester applications to get " << expectedState );
//   return false;
// }

void emu::step::Manager::waitForTestsToFinish( const bool isTestPassive ){
  emu::soap::Messenger m( this );
  while ( true ){
    bool allFinished = true;
    if ( isTestPassive ){
      // For passive tests, query the local DAQ
      xdata::Boolean STEPFinished;
      m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "STEPFinished", &STEPFinished ) );
      allFinished &= (bool) STEPFinished;
      if ( STEPFinished ){ LOG4CPLUS_INFO( logger_, "STEP is done."); }
    }
    else{
      // For active tests, query the Tester apps
      for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin();
	    app != testerDescriptors_.end(); 
	    ++app ){
	xdata::Boolean testDone;
	m.getParameters( app->second, emu::soap::Parameters().add( "testDone", &testDone ) );
	allFinished &= (bool) testDone;
	// LOG4CPLUS_INFO( logger_, app->first << ( (bool) testDone ? " is done." : " is not yet done." ) );
	if ( testDone ){ LOG4CPLUS_INFO( logger_, app->first << " is done."); }
      }
    }
    if ( allFinished ) { return; }
    ::sleep( 1 );
  }
}

const string emu::step::Manager::namespace_( "http://cms.cern.ch/emu/step" );
const string emu::step::Manager::workLoopType_( "waiting" );

/**
 * Provides the factory method for the instantiation of applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::step::Manager)
