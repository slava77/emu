#include "emu/step/Tester.h"


#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"
#include "emu/utils/Chamber.h"

#include "toolbox/task/WorkLoopFactory.h"
#include "xcept/tools.h"

using namespace emu::utils;

emu::step::Tester::Tester( xdaq::ApplicationStub *s ) 
  : xdaq::WebApplication( s )
  , emu::step::Application( s )
  , testId_( "0" )
  , testConfigured_( false )  
  , testDone_( false )
  , test_( NULL )
  , workLoop_( NULL )
  , configureTestSignature_( NULL )
  , enableTestSignature_( NULL )
{
  exportParameters();
  configureTestSignature_ = toolbox::task::bind( this, &emu::step::Tester::configureTestInWorkLoop, "configureTestInWorkLoop" );
  enableTestSignature_    = toolbox::task::bind( this, &emu::step::Tester::enableTestInWorkLoop   , "enableTestInWorkLoop"    );
  workLoopName_ = getApplicationDescriptor()->getClassName() + "." 
    + emu::utils::stringFrom<int>( getApplicationDescriptor()->getInstance() );
}

vector<string> emu::step::Tester::getDataDirNamesSTL(){
  vector<string> dirs;
  for ( xdata::Vector<xdata::String>::iterator d = dataDirNames_.begin(); d != dataDirNames_.end(); ++d ) dirs.push_back( ( dynamic_cast<xdata::String*>( &*d ) )->toString() );
  return dirs;
}

void emu::step::Tester::exportParameters(){
  xdata::InfoSpace *s = getApplicationInfoSpace();
  s->fireItemAvailable( "group"                     , &group_                      );
  s->fireItemAvailable( "testParametersFileName"    , &testParametersFileName_     );
  s->fireItemAvailable( "vmeSettingsFileName"       , &vmeSettingsFileName_        );
  s->fireItemAvailable( "specialVMESettingsFileName", &specialVMESettingsFileName_ );
  s->fireItemAvailable( "testId"                    , &testId_                     );
  s->fireItemAvailable( "fakeTests"                 , &fakeTests_                  );
  s->fireItemAvailable( "testConfigured"            , &testConfigured_             );
  s->fireItemAvailable( "testDone"                  , &testDone_                   );
  s->fireItemAvailable( "crateIds"                  , &crateIds_                   );
  s->fireItemAvailable( "chamberLabels"             , &chamberLabels_              );
  s->fireItemAvailable( "progress"                  , &progress_                   );
  s->fireItemAvailable( "nEvents"                   , &nEvents_                    );
  s->fireItemAvailable( "chamberMaps"               , &chamberMaps_                );
  s->fireItemAvailable( "runNumber"                 , &runNumber_                  );
  s->fireItemAvailable( "runStartTime"              , &runStartTime_               );
  s->fireItemAvailable( "dataDirNames"              , &dataDirNames_               );
  // s->fireItemAvailable( "", &_ );
  s->addItemRetrieveListener( "progress", this );
  s->addItemRetrieveListener( "nEvents" , this );
}

void emu::step::Tester::loadFiles(){
  testParametersXML_     = emu::utils::readFile( emu::utils::performExpansions( testParametersFileName_     ) );
  if ( testParametersXML_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, testParametersFileName_.toString() + " could not be read in or is empty." );
  }
  vmeSettingsXML_        = emu::utils::readFile( emu::utils::performExpansions( vmeSettingsFileName_        ) );
  if ( vmeSettingsXML_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, vmeSettingsFileName_.toString() + " could not be read in or is empty." );
  }
  specialVMESettingsXML_ = emu::utils::readFile( emu::utils::performExpansions( specialVMESettingsFileName_ ) );
  if ( specialVMESettingsXML_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, specialVMESettingsFileName_.toString() + " could not be read in or is empty." );
  }
}

string emu::step::Tester::selectCratesAndChambers( const string& vmeSettingsXML ){
  // If nothing is selected, return empty EmuSystem:
  if ( crateIds_.elements() == 0 || chamberLabels_.elements() == 0 ) return "<EmuSystem/>";

  // Build XPath expression selecting all unwanted crates
  stringstream xpath;
  xpath << "//EmuSystem/PeripheralCrate[";
  for ( size_t i = 0; i < crateIds_.elements(); ++i ){
    xpath << "@crateID!='" << ( dynamic_cast<xdata::String*>( crateIds_.elementAt(i) ) )->toString() << "'";
    if ( i+1 < crateIds_.elements() ) xpath << " and ";
  }
  xpath << "]";
  // Remove unwanted crates
  string selectedCratesOnlyXML( emu::utils::removeSelectedNode( vmeSettingsXML, xpath.str() ) );

  // Reuse xpath variable
  xpath.clear();
  xpath.str("");

  // Build XPath expression selecting all unwanted chambers
  xpath << "//EmuSystem/PeripheralCrate/CSC[";
  for ( size_t i = 0; i < chamberLabels_.elements(); ++i ){
    xpath << "@label!='" << ( dynamic_cast<xdata::String*>( chamberLabels_.elementAt(i) ) )->toString() << "'";
    if ( i+1 < chamberLabels_.elements() ) xpath << " and ";
  }
  xpath << "]";
  // Remove unwanted chambers
  return emu::utils::removeSelectedNode( selectedCratesOnlyXML, xpath.str() );
}

void emu::step::Tester::createChamberMaps( const string& selectedVMESettingsXML ){
  // Get the chamber-mapping parameters from the VME config (containing the selected chambers only) and fill the exported parameter chamberMaps with them.
  // This map will be needed to let the analysis program know what kind of chamber's data it's dealing with when STEP is run on a test stand. 
  // This mapping is well defined for the on-detector chambers, but it keeps changing on the test stands.
  // Peripheral crates on test stands should have a DDU.

  chamberMaps_.clear();

  // Look for a DDU in each crate to see if we're running on a test stand. If not, don't fill chamberMaps, it won't be needed.
  vector< pair< string, string > > cratesWithNoDDU( utils::getSelectedNodesValues( selectedVMESettingsXML, "//PeripheralCrate[not(DDU)]/@label" ) );
  if ( cratesWithNoDDU.size() > 0 ){
    ostringstream oss;
    oss << cratesWithNoDDU;
    LOG4CPLUS_INFO( logger_, "No special chamber maps will be created as in group " + group_.toString() + " the following crates have no DDU: " + oss.str() );
    return; // Chamber maps will be empty, the canonical mapping should be used instead in the analysis.
  }

  // Get the chamber labels
  vector< pair< string, string > > labels( utils::getSelectedNodesValues( selectedVMESettingsXML, "//PeripheralCrate/CSC/@label" ) );

  // For each chamber label, get the corresponding DMB slot and crate id
  for ( vector< pair< string, string > >::const_iterator l = labels.begin(); l != labels.end(); ++l ){
    xdata::Bag<ChamberMap> map;
    map.bag.chamberLabel_ = emu::utils::Chamber( l->second ).name(); // l->first is the node name, l->second the node value
    map.bag.dmbSlot_      = utils::getSelectedNodeValue( selectedVMESettingsXML, "//PeripheralCrate/CSC[@label='" + l->second + "']/DAQMB/@slot" );
    map.bag.crateId_      = utils::getSelectedNodeValue( selectedVMESettingsXML, "//PeripheralCrate[CSC/@label='" + l->second + "']/@crateID" );
    chamberMaps_.push_back( map );
  }
  
}

void emu::step::Tester::configureAction( toolbox::Event::Reference e ){
  LOG4CPLUS_INFO( logger_, "Configuring." );

  delete test_;
  test_ = NULL;

  testConfigured_ = false;
  testDone_       = false;

  // Create the test
  try{
    loadFiles();
    string selectedVMESettingsXML = selectCratesAndChambers( vmeSettingsXML_ );
    test_ = new Test( testId_,
		      group_,
		      testParametersXML_,
		      selectedVMESettingsXML,
		      specialVMESettingsXML_,
		      (bool) fakeTests_,
		      &logger_                );
    createChamberMaps( selectedVMESettingsXML );
    LOG4CPLUS_INFO( logger_, "Created " << ( (bool) fakeTests_ ? "fake " : "" ) << " test " << testId_.toString() );
  }
  catch ( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to configure.", e );
  }

  // Create work loop if it doesn't yet exist
  if ( workLoop_ == NULL ){
    try{
      workLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( workLoopName_, workLoopType_ );
      LOG4CPLUS_INFO( logger_, "Created " << workLoopType_ << " work loop " << workLoopName_ );
    } catch( xcept::Exception &e ){
      stringstream ss;
      ss << "Failed recreate " << workLoopType_ << " work loop " << workLoopName_;
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, ss.str(), e );
    }
  }

  // Cancel workloop if it's still active
  if ( workLoop_->isActive() ){
    try{
      workLoop_->cancel();
      LOG4CPLUS_INFO( logger_, "Cancelled work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to cancel work loop.", e );
    }
  }
  
  // Schedule test procedure to be configured in a separate thread
  try{
    workLoop_->submit( configureTestSignature_ );
    LOG4CPLUS_INFO( logger_, "Submitted configure action to " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to submit configure action to work loop.", e );
  }

  // Configure the test procedure in a separate thread:
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop for configuring test." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop for configuring test.", e );
    }
  }
}

void emu::step::Tester::enableAction( toolbox::Event::Reference e ){
  LOG4CPLUS_INFO( logger_, "Enabling." );

  testDone_ = false;

  // Cancel workloop
  if ( workLoop_->isActive() ){
    try{
      workLoop_->cancel();
      LOG4CPLUS_INFO( logger_, "Cancelled work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to cancel work loop.", e );
    }
  }
  
  // Schedule test procedure to be executed in a separate thread
  try{
    workLoop_->submit( enableTestSignature_ );
    LOG4CPLUS_INFO( logger_, "Submitted enable action to " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to submit configure action to work loop.", e );
  }

  // Execute the test procedure in a separate thread:
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop for executing test." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop for executing test.", e );
    }
  }
}

void emu::step::Tester::haltAction( toolbox::Event::Reference e ){
  if ( test_ ){
    LOG4CPLUS_INFO( logger_, "Stopping test " << test_->getId() );
    test_->stop();
  }
  // while ( true ){
  //   cout << "Work loop is active: " << workLoop_->isActive() << endl << flush;
  //   ::sleep(1);
  // }
}

void emu::step::Tester::stopAction( toolbox::Event::Reference e ){
  haltAction( e );
}

bool emu::step::Tester::configureTestInWorkLoop( toolbox::task::WorkLoop *wl ){
  if ( test_ ){
    LOG4CPLUS_INFO( logger_, "Executing test " << test_->getId() );
    try{
      test_->configure();
    }
    catch ( xcept::Exception& e ){
      stringstream ss;
      ss << "Failed to configure test " <<  test_->getId();
      XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
      LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
      moveToFailedState( ee );
      return false;
    }
    testConfigured_ = true;
    LOG4CPLUS_INFO( logger_, "Configured test " << test_->getId() );
  }
  else{
    LOG4CPLUS_ERROR( logger_, "No test has been defined." );
  }
  return false;
}

bool emu::step::Tester::enableTestInWorkLoop( toolbox::task::WorkLoop *wl ){
  if ( test_ ){
    LOG4CPLUS_INFO( logger_, "Executing test " << test_->getId() );
    try{
      test_->setRunStartTime( runStartTime_ ).setRunNumber( runNumber_ ).setDataDirNames( getDataDirNamesSTL() ).enable();
    }
    catch ( xcept::Exception& e ){
      stringstream ss;
      ss << "Failed to execute test " <<  test_->getId();
      XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
      LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
      moveToFailedState( ee );
      return false;
    }
    testDone_ = true;
    LOG4CPLUS_INFO( logger_, "Executed test " << test_->getId() );
  }
  else{
    LOG4CPLUS_ERROR( logger_, "No test has been defined." );
  }
  return false;
}

void emu::step::Tester::actionPerformed( xdata::Event& received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  LOG4CPLUS_DEBUG(logger_, 
		  "Received an InfoSpace event" <<
		  " Event type: " << e.type() <<
		  " Event name: " << e.itemName() <<
		  " Serializable: " << std::hex << e.item() << std::dec <<
		  " Type of serializable: " << e.item()->type() );

  if ( e.itemName() == "progress" && e.type() == "ItemRetrieveEvent" ){
    if ( test_ ) progress_ = test_->getProgress();
    else         progress_ = double( 0 );
  }
  else if ( e.itemName() == "nEvents" && e.type() == "ItemRetrieveEvent" ){
    if ( test_ ) nEvents_ = test_->getNEvents();
    else         nEvents_ = 0;
  }
}

const string emu::step::Tester::workLoopType_( "waiting" );

/**
 * Provides the factory method for the instantiation of applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::step::Tester)
