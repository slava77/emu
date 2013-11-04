#include "emu/step/Manager.h"

#include "emu/step/version.h"
#include "emu/step/Test.h"

#include "emu/soap/ToolBox.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"
#include "emu/utils/Cgi.h"
#include "emu/utils/Chamber.h"

#include "xdata/Integer64.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/regex.h"
#include "xcept/tools.h"

#include <sys/time.h>

#include <cmath>
#include <iomanip>

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
  s->fireItemAvailable( "configurationXSLFileName"  , &configurationXSLFileName_   );
  s->fireItemAvailable( "testParametersFileName"    , &testParametersFileName_     );
  s->fireItemAvailable( "specialVMESettingsFileName", &specialVMESettingsFileName_ );
  s->fireItemAvailable( "analysisScriptName"        , &analysisScriptName_         );
  s->fireItemAvailable( "analysisExeName"           , &analysisExeName_            );
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
    m.getParameters( *app,
		     emu::soap::Parameters()
		     .add( "group", &group )
		     .add( "vmeSettingsFileName", &fileName )
		     );
    testerDescriptors_     [group.toString()] = *app;
    pCrateSettingsFileNames[group.toString()] = fileName.toString();
    LOG4CPLUS_INFO( logger_, "Found " << (*app)->getClassName() << " instance " << (*app)->getInstance() << " for group '" << group.toString() << "' with settings in " << fileName.toString() );
  }

  // cout << pCrateSettingsFileNames << endl;

  configuration_ = new emu::step::Configuration( namespace_, 
						 configurationXSLFileName_,
						 testParametersFileName_,
						 pCrateSettingsFileNames );
}


void emu::step::Manager::startFED( bool inPassthroughMode ){
  // If we use FED crate(s), we control the FED system here. (If the DDU is in the PCrate, we'll set it up together with it for each individual test.)
  set<xdaq::ApplicationDescriptor *> apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors( "emu::fed::Communicator" );

  if ( apps.size() == 0 ){
    LOG4CPLUS_WARN( logger_, "No emu::fed::Communicator applications found. Will look for DDU in the peripheral crate instead." );
    return;
  }
  else{
    LOG4CPLUS_INFO( logger_, "Found " << apps.size() << " emu::fed::Communicator application(s)." );
  }

  // Make three attempts to set up FED. (Sometimes it fails...)
  const int nAttempts = 3;
  bool successful = false;
  int iAttempt = 1;
  while ( iAttempt <= nAttempts && !successful ){
    try{
      // Get the FED settings file names
      set<string> fedSettingsFileNames;
      emu::soap::Messenger m( this );
      for ( set<xdaq::ApplicationDescriptor*>::iterator app = apps.begin(); app != apps.end(); ++app ) {
	xdata::String xmlFileName;
	m.getParameters( *app, emu::soap::Parameters().add( "xmlFileName", &xmlFileName ) );
	fedSettingsFileNames.insert( xmlFileName.toString() );
	LOG4CPLUS_INFO( logger_, "Found " << (*app)->getClassName() << " of instance " << (*app)->getInstance() << " with settings in " << xmlFileName.toString() );
      }
      
      // Build XPath expression to unkill tested chambers' fibers in the FED settings files
      xdata::Vector<xdata::String> chamberLabels = configuration_->getChamberLabels();
      for ( set<string>::iterator fn = fedSettingsFileNames.begin(); fn != fedSettingsFileNames.end(); ++fn ) {
	string fedSettingsXML = emu::utils::readFile( emu::utils::performExpansions( *fn ) );
	if ( fedSettingsXML.size() == 0 ){
	  XCEPT_RAISE( xcept::Exception, *fn + " could not be read in or is empty." );
	}
	// First kill all chambers' fibers...
	fedSettingsXML = emu::utils::setSelectedNodesValues( fedSettingsXML, "//FEDSystem/FEDCrate/DDU/Fiber/@KILLED" , "1" );
	// ...then unkill the tested chambers'
	for ( size_t iChamber = 0; iChamber < chamberLabels.elements(); ++iChamber ){
	  // In the FED settings XML file, chamber names are zero-padded, but without "ME", e.g., CHAMBER="-1/2/08"
	  // while in the PCrate settings XML file, it's the other way round...
	  fedSettingsXML = emu::utils::setSelectedNodesValues( fedSettingsXML, "//FEDSystem/FEDCrate/DDU/Fiber[@CHAMBER='" + emu::utils::Chamber( ( dynamic_cast<xdata::String*>( chamberLabels.elementAt( iChamber ) ) )->toString() ).name().substr( 2 ) + "']/@KILLED", "0" );
	}
	// cout << fedSettingsXML << endl;
	utils::writeFile( *fn, fedSettingsXML );
	LOG4CPLUS_INFO( logger_, "Updated FED Crates' configuration XML in " << *fn );
      }
      
      // Halt all Communicators
      m.sendCommand( "emu::fed::Communicator", "Halt" );
      // Configure DDUs in passthrough mode
      xdata::Boolean dduInPassthroughMode = inPassthroughMode;
      m.setParameters( "emu::fed::Communicator", emu::soap::Parameters().add( "dduInPassthroughMode" , &dduInPassthroughMode ) );
      m.sendCommand( "emu::fed::Communicator", "Configure" );
      sleep( 1 );
      // Start all Communicators
      m.sendCommand( "emu::fed::Communicator", "Enable" );
      sleep( 1 );
      successful = true;
    }
    catch ( xcept::Exception &e ){
      ostringstream oss;
      oss << "Attempt " << iAttempt << " to set up FED failed. " << xcept::stdformat_exception_history( e );
      LOG4CPLUS_WARN( logger_, oss.str() );
      if ( iAttempt == nAttempts ){
	oss.str("");
	oss << "All " << nAttempts << " attempts to set up FED failed.";
	XCEPT_RETHROW( xcept::Exception, oss.str(), e );
      }
      sleep( 2 );
    }
  } // while ( iAttempt <= nAttempts && !successful ){
  LOG4CPLUS_INFO( logger_, "Configured DDUs in " << ( inPassthroughMode ? "passthrough" : "normal" ) << " mode." );
}

void emu::step::Manager::haltFED(){
  // If we use FED crate(s), we control the FED system here. (If the DDU is in the PCrate, we'll set it up together with it for each individual test.)
  if ( getApplicationContext()->getDefaultZone()->getApplicationDescriptors( "emu::fed::Communicator" ).size() == 0 ) return;

  emu::soap::Messenger m( this );
  // Set flag in Communicators to normal (not passthrough) mode so that on next configure the DDUs are *not* configured for passthrough mode
  // in case a non-STEP run follows.
  xdata::Boolean dduInPassthroughMode = false;
  m.setParameters( "emu::fed::Communicator", emu::soap::Parameters().add( "dduInPassthroughMode" , &dduInPassthroughMode ) );
  // Halt all Communicators
  m.sendCommand( "emu::fed::Communicator", "Halt" );
}

void emu::step::Manager::configureAction(toolbox::Event::Reference e){
  LOG4CPLUS_INFO( logger_, "emu::step::Manager::configureAction" );

  dataFileNames_.clear();

  configuration_->setTestStatus( "idle", "This test is not running yet." );

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
    LOG4CPLUS_INFO( logger_, "Submitted test action to " << workLoopType_ << " work loop " << workLoopName_ );
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

  dataFileNames_.clear();

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

  // Halt the FED
  try{
    haltFED();
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to halt the FED system.", e );
  }
}

void emu::step::Manager::resetAction(toolbox::Event::Reference e){
  LOG4CPLUS_DEBUG( logger_, "emu::step::Manager::resetAction" );
  delete configuration_;
  configuration_ = NULL;
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
	xdata::Vector<xdata::String> crateIds      = configuration_->getCrateIds     ( t->first );
	xdata::Vector<xdata::String> chamberLabels = configuration_->getChamberLabels( t->first );
	m.setParameters( t->second, 
			 emu::soap::Parameters()
			 .add( "testParametersFileName"    , &testParametersFileName_     )
			 .add( "specialVMESettingsFileName", &specialVMESettingsFileName_ )
			 .add( "crateIds"                  , &crateIds                    )
			 .add( "chamberLabels"             , &chamberLabels               )
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

    xdata::String testId = *dynamic_cast<xdata::String*>( testIds.elementAt( iTest ) );
    emu::step::TestParameters testParameters( testId.toString(), configuration_->getTestParametersXML(), &logger_ );
    bsem_.take();
    isCurrentTestDurationUndefined_ = ( testParameters.getNEvents() > 0 );
    bsem_.give();

    try{
      //
      // Set the FED crate to normal (not passthrough) mode to immunize it to ODMB data bursts on hard reset
      //
      try{
	startFED( false );
      } catch( xcept::Exception &e ){
	XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to set the FED system to normal mode.", e );
      }
      //
      // Set test id in all Tester apps
      //
      m.setParameters( "emu::step::Tester", emu::soap::Parameters().add( "testId", &testId ) );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Configure all Tester apps and local DAQ
      //
      configuration_->setTestStatus( testId, "configuring", "This test is being prepared." );
      const uint64_t daqTimeOutInSeconds = 15;
      m.sendCommand( "emu::daq::manager::Application", "Halt" );      
      if ( ! waitForDAQToExecute( "Halt", daqTimeOutInSeconds ) ){
	XCEPT_RAISE( xcept::Exception, string( "DAQ failed to execute 'Halt' in ") + utils::stringFrom<uint64_t>( daqTimeOutInSeconds ) + " seconds." );
      }
      xdata::String             runType = string( ( (bool) isCurrentTestDurationUndefined_ ) ? "STEP_" : "Test_" ) + testId.toString();
      xdata::Integer64  maxNumberOfEvents = ( ( (bool) isCurrentTestDurationUndefined_ ) ? (int) testParameters.getNEvents() : -1 ); // unlimited if negative
      xdata::Boolean writeBadEventsOnly = false;
      m.setParameters( "emu::daq::manager::Application", 
		       emu::soap::Parameters()
		       .add( "runType"           , &runType            )
		       .add( "maxNumberOfEvents" , &maxNumberOfEvents  )
		       .add( "writeBadEventsOnly", &writeBadEventsOnly ) );
      m.sendCommand( "emu::daq::manager::Application", "Configure" );      
      m.sendCommand( "emu::step::Tester", "Configure" );
      if ( ! waitForDAQToExecute( "Configure", daqTimeOutInSeconds ) ){
	XCEPT_RAISE( xcept::Exception, string( "DAQ failed to execute 'Configure' in ") + utils::stringFrom<uint64_t>( daqTimeOutInSeconds ) + " seconds." );
      }
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      // Be sure configuration has finished before enabling
      waitForTestsToConfigure();

      //
      // Set up and start the FED crate
      //
      try{
	startFED( true );
      } catch( xcept::Exception &e ){
	XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to start the FED system.", e );
      }
      //
      // Enable all Tester apps
      //
      // First enable the local DAQ
      m.sendCommand( "emu::daq::manager::Application", "Enable" );
      if ( ! waitForDAQToExecute( "Enable", daqTimeOutInSeconds ) ){
	XCEPT_RAISE( xcept::Exception, string( "DAQ failed to execute 'Enable' in ") + utils::stringFrom<uint64_t>( daqTimeOutInSeconds ) + " seconds." );
      }
      configuration_->setTestStatus( testId, "running", "This test is still in progress." );
      // Get the number and start time of run and the data directories from the local DAQ and pass them on to the Testers
      xdata::String runStartTime;
      xdata::UnsignedInteger32 runNumber;
      xdata::Vector<xdata::String> dataDirNames; // all RUIs' data directory names
      m.getParameters( "emu::daq::manager::Application", 0, 
		       emu::soap::Parameters()
		       .add( "runNumber"   , &runNumber    )
		       .add( "runStartTime", &runStartTime )
		       .add( "dataDirNames", &dataDirNames ) );
      m.setParameters( "emu::step::Tester",
		       emu::soap::Parameters()
		       .add( "runNumber"   , &runNumber    )
		       .add( "runStartTime", &runStartTime )
		       .add( "dataDirNames", &dataDirNames ) );
      // The Testers can be enabled now
      m.sendCommand( "emu::step::Tester", "Enable" );
      waitForTestsToFinish( (bool) isCurrentTestDurationUndefined_ );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Halt all Tester apps
      //
      m.sendCommand( "emu::step::Tester", "Halt" );
      m.sendCommand( "emu::daq::manager::Application", "Halt" );
      if ( ! waitForDAQToExecute( "Halt", daqTimeOutInSeconds ) ){
	XCEPT_RAISE( xcept::Exception, string( "DAQ failed to execute 'Halt' in ") + utils::stringFrom<uint64_t>( daqTimeOutInSeconds ) + " seconds." );
      }
      string details = checkDataCompleteness( testId.toString() ); 
      if ( details.size() == 0 ) configuration_->setTestStatus( testId, "done"      , "This test has finished." );
      else                       configuration_->setTestStatus( testId, "incomplete", details                   );
      if ( fsm_.getCurrentState() == 'H' ) return false; // Get out of here if it's been stopped in the meantime.
      //
      // Run analysis
      //
      runAnalysis( testId.toString(0) );
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

string emu::step::Manager::checkDataCompleteness( const string& testId ){
  ostringstream oss;
  try{
    emu::soap::Messenger m( this );
    //
    // Get total number of events to be taken for the current test
    //
    set<uint64_t> testsNEvents;
    for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app ){
      xdata::UnsignedInteger64 nEvents;
      m.getParameters( app->second , emu::soap::Parameters().add( "nEvents", &nEvents ) );
      testsNEvents.insert( uint64_t( nEvents ) );
    }
    if ( testsNEvents.size() > 1 ){
      oss << "Groups have different total number events to take for test " << testId 
	  << ". Cannot indicate whether DAQ has read the required number of events.";
	LOG4CPLUS_WARN( logger_, oss.str() );
      return oss.str();
    }
    //
    // Get number events read by each RUI (DDU) and check if they complete the test.
    //
    xdata::Vector<xdata::UnsignedInteger64> rui_counts;
    xdata::Vector<xdata::UnsignedInteger32> rui_instances;
    xdata::String xs;
    emu::soap::extractParameters( m.sendCommand( "emu::daq::manager::Application", 0, "QueryRunSummary" ), 
    				  emu::soap::Parameters()
				  .add( "rui_instances", &rui_instances )
				  .add( "rui_counts"   , &rui_counts    )                                 );
    for ( size_t i = 0; i < rui_counts.elements(); i++ ){
      // In timed tests, where the number of events to take is not predefined (and nEvents=0), 
      // we cannot tell if the data file is complete.
      if ( *testsNEvents.begin() != 0 ){
	if ( *testsNEvents.begin() != *dynamic_cast<xdata::UnsignedInteger64*>( rui_counts.elementAt( i ) ) ){
	  oss << "RUI "         << ( dynamic_cast<xdata::UnsignedInteger32*>( rui_instances.elementAt( i ) ) )->toString()
	      << " read "       << ( dynamic_cast<xdata::UnsignedInteger64*>(    rui_counts.elementAt( i ) ) )->toString()
	      << " of "         << *testsNEvents.begin() 
	      << " events. \n";
	}
      }
    }
  }
  catch( xcept::Exception& e ){
    LOG4CPLUS_ERROR( logger_, "Failed to check the completeness of data for test " << testId << " : " << xcept::stdformat_exception_history( e ) );
    bsem_.give();
  }
  catch( std::exception& e ){
    LOG4CPLUS_ERROR( logger_, "Failed to check the completeness of data for test " << testId << " : " << e.what() );
    bsem_.give();
  }
  catch( ... ){
    LOG4CPLUS_ERROR( logger_, "Failed to check the completeness of data for test " << testId << " : Unknown exception." );
    bsem_.give();
  }
  return oss.str();
}

void emu::step::Manager::updateChamberMaps(){
  // Collect chamber maps from all Testers
  try{
    chamberMaps_.clear();
    emu::soap::Messenger m( this );
    for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app ){
      xdata::Vector< xdata::Bag<ChamberMap> > maps;
      m.getParameters( app->second , emu::soap::Parameters().add( "chamberMaps", &maps ) );
      if ( maps.elements() == 0 ){
	chamberMaps_.clear();
	LOG4CPLUS_WARN( logger_, "Group '" + app->first + "' has DDU missing. No chamber mapping will be created at all. (Only a problem on a test stand.)" );
	return;
      }
      for ( size_t iMap = 0; iMap < maps.size(); ++iMap ){
	chamberMaps_.push_back( maps[iMap] );
      }
    }
    stringstream ss;
    ss << "Chamber maps: [";
    for ( size_t iMap = 0; iMap < chamberMaps_.size(); ++iMap ){
      ss << chamberMaps_[iMap].bag.toString() << (iMap+1<chamberMaps_.size()?", ":"");
    }
    ss << "]";
    LOG4CPLUS_INFO( logger_, ss.str() );
  } catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to get chamber maps.", e );
  } catch( std::exception& e ){
    XCEPT_RAISE( xcept::Exception, string( "Failed to get chamber maps: ") + e.what() );
  } catch( ... ){
    XCEPT_RAISE( xcept::Exception, "Failed to get chamber maps. Unknown exception." );
  }
}

void emu::step::Manager::updateDataFileNames(){
  try{
    emu::soap::Messenger m( this );
    // Get data file names written in this test
    xdata::Vector<xdata::String> dataFileNames;
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "dataFileNames", &dataFileNames ) );
    // cout << "dataFileNames" << dataFileNames.toString() << endl;
    // Add them to the list of all data file names written in this run
    for ( size_t iFile = 0; iFile < dataFileNames.elements(); iFile++ ){
      dataFileNames_.insert( ( dynamic_cast<xdata::String*>( dataFileNames.elementAt( iFile ) ) )->toString() );
    }
  } catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to get data file names.", e );
  } catch( std::exception& e ){
    XCEPT_RAISE( xcept::Exception, string( "Failed to get data file names: ") + e.what() );
  } catch( ... ){
    XCEPT_RAISE( xcept::Exception, "Failed to get data file names. Unknown exception." );
  }
}

void emu::step::Manager::runAnalysis( const string& testId ){
  // Update data file names
  try{
    bsem_.take();
    updateDataFileNames();
    updateChamberMaps();
    bsem_.give();
  } catch( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xcept::Exception, string("Failed to run analysis of test ") + testId + ".", e );
  }

  // Invoke analysis script as
  // analysisScriptName analysisExeName 'dataFile1 dataFile2 ...' 'crateId1 dmbSlot1 chamberLabel1' 'crateId2 dmbSlot2 chamberLabel2' ...
  stringstream command;
  command << analysisScriptName_.toString() << " " << analysisExeName_.toString() << " '";
  uint32_t nDataFilesOfThisTest = 0;
  for ( set<string>::iterator dfn = dataFileNames_.begin(); dfn != dataFileNames_.end(); ++dfn ){
    if ( dfn->find( "Test_"+testId ) != string::npos || dfn->find( "STEP_"+testId ) != string::npos ){
      command << ( nDataFilesOfThisTest == 0 ? "" : " " ) << *dfn;
      ++nDataFilesOfThisTest;
    }
  }
  command << "'";
  for ( size_t iMap = 0; iMap < chamberMaps_.size(); ++iMap ){
    command << " '" << chamberMaps_[iMap].bag.crateId_.toString()
	    << " " << chamberMaps_[iMap].bag.dmbSlot_.toString()
	    << " " << chamberMaps_[iMap].bag.chamberLabel_.toString()
	    << "'";
  }
  command << " > /tmp/STEPAnalysis_Test_" << testId << "_" << utils::getDateTime( true ) << ".log 2>&1 &";
  LOG4CPLUS_INFO( logger_, "Executing shell command\n" + command.str() );
  utils::execShellCommand( command.str() );
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
    // First of all, create configuration if none exists yet:
    if ( configuration_ == NULL ) createConfiguration();

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
	    stringstream ss;
	    ss << "Refused to configure due to an attempt to change a configuration of modification time " << receivedModTime << " while the current configuration's modification time is " << configuration_->getModificationTime();
	    XCEPT_RAISE( xcept::Exception, ss.str() );
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
    stringstream ss;
    ss << "Failed to execute " + action.begin()->second;
    XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
    bsem_.give();
    moveToFailedState( ee );
    LOG4CPLUS_ERROR( logger_, xcept::stdformat_exception_history( ee ) );
    *out << createXMLWebPage();
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

  // Data file names
  ss << "  <es:dataFiles>" << endl;
  for ( set<string>::iterator dfn = dataFileNames_.begin(); dfn != dataFileNames_.end(); ++dfn ){
    ss << "    <es:dataFile>" << *dfn << "</es:dataFile>" << endl;  
  }
  ss << "  </es:dataFiles>" << endl;
 

  ss << "</es:Manager>";

  return emu::utils::appendToSelectedNode( ss.str(), "/es:Manager", configuration_->getXML() );
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
    if ( fsm_.getCurrentState() == 'H' || fsm_.getCurrentState() == 'F' ) return true; // Get out of here if we've been stopped in the meantime.
    m.getParameters( "emu::daq::manager::Application", 0, emu::soap::Parameters().add( "daqState", &daqState ) );
    if ( daqState.toString() == "Failed" ){
      LOG4CPLUS_ERROR( logger_, "Local DAQ is in 'Failed' state. Please destroy and recreate local DAQ." );
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


void emu::step::Manager::waitForTestsToConfigure(){
  emu::soap::Messenger m( this );
  while ( fsm_.getCurrentState() != 'H' && fsm_.getCurrentState() != 'F' ){
    bool allConfigured = true;
    // Query the Tester apps
    map<string,pair<double,string> > groupsProgress; // group -> ( progress, message )
    for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app ){
      xdata::String  reasonForFailure;
      xdata::Double  progress;
      xdata::Boolean testConfigured;
      m.getParameters( app->second, 
		       emu::soap::Parameters()
		       .add( "reasonForFailure", &reasonForFailure    ) // empty if not in failed state
		       .add( "progress"        , &progress            )
		       .add( "testConfigured"  , &testConfigured      ) );
      groupsProgress[app->first] = make_pair<double,string>( progress , reasonForFailure );
      if ( reasonForFailure.toString().size() > 0 ) allConfigured &= true;
      else                                          allConfigured &= (bool) testConfigured;
      LOG4CPLUS_INFO( logger_, app->first << ( (bool)testConfigured ? " is configured." : " is not yet configured." ) );
      if ( testConfigured ){ LOG4CPLUS_INFO( logger_, app->first << " is configured."); }
    }
    //cout << groupsProgress << endl;
    configuration_->setTestProgress( groupsProgress );
    if ( allConfigured ) { return; }
    ::sleep( 1 );
  }
}

void emu::step::Manager::waitForTestsToFinish( const bool isTestDurationUndefined ){
  emu::soap::Messenger m( this );
  while ( fsm_.getCurrentState() != 'H' && fsm_.getCurrentState() != 'F' ){
    updateDataFileNames();
    bool allFinished = true;
    if ( isTestDurationUndefined ){
      // Query the local DAQ
      xdata::String  reasonForFailure;
      xdata::Integer64 maxNumberOfEvents;
      xdata::UnsignedInteger64 STEPCount;
      m.getParameters( "emu::daq::manager::Application", 0, 
		       emu::soap::Parameters()
		       .add( "reasonForFailure" , &reasonForFailure  ) // empty if not in failed state
		       .add( "maxNumberOfEvents", &maxNumberOfEvents )
		       .add( "STEPCount"        , &STEPCount         ) );
      if ( (uint64_t) maxNumberOfEvents > 0 ){
	double progress = 100. * double( STEPCount.value_ ) / double( maxNumberOfEvents.value_ ); // in %
	// Assign every group the same progress. It would be complicated to attribute, and it wouldn't make much sense anyway.
	map<string,pair<double,string> > groupsProgress; // group -> ( progress, message )
	map<string,xdaq::ApplicationDescriptor*>::iterator app;
	for ( app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app ){
	  groupsProgress[app->first] = make_pair<double,string>( progress, reasonForFailure );
	}
	configuration_->setTestProgress( groupsProgress );
      }
      allFinished = ( (int64_t) STEPCount.value_ >= maxNumberOfEvents );
      if ( allFinished ){ LOG4CPLUS_INFO( logger_, "STEP is done."); }
    }
    else{
      // Query the Tester apps
      map<string,pair<double,string> > groupsProgress; // group -> ( progress, message )
      for ( map<string,xdaq::ApplicationDescriptor*>::iterator app = testerDescriptors_.begin(); app != testerDescriptors_.end(); ++app ){
	xdata::String  reasonForFailure;
	xdata::Double  progress;
	xdata::Boolean testDone;
	m.getParameters( app->second, 
			 emu::soap::Parameters()
			 .add( "reasonForFailure", &reasonForFailure    ) // empty if not in failed state
			 .add( "progress"        , &progress            )
			 .add( "testDone"        , &testDone            ) );
	groupsProgress[app->first] = make_pair<double,string>( progress , reasonForFailure );
	if ( reasonForFailure.toString().size() > 0 ) allFinished &= true;
	else                                          allFinished &= (bool) testDone;
	// LOG4CPLUS_INFO( logger_, app->first << ( (bool) testDone ? " is done." : " is not yet done." ) );
	if ( testDone ){ LOG4CPLUS_INFO( logger_, app->first << " is done."); }
      }
      //cout << groupsProgress << endl;
      configuration_->setTestProgress( groupsProgress );
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
