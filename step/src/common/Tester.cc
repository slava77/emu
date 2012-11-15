#include "emu/step/Tester.h"


#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "toolbox/task/WorkLoopFactory.h"
#include "xcept/tools.h"

using namespace emu::utils;

emu::step::Tester::Tester( xdaq::ApplicationStub *s ) 
  : xdaq::WebApplication( s )
  , emu::step::Application( s )
  , testId_( "0" )
  , testDone_( false )
  , test_( NULL )
  , workLoop_( NULL )
  , testSignature_( NULL )
{
  exportParameters();
  testSignature_ = toolbox::task::bind( this, &emu::step::Tester::testInWorkLoop, "testInWorkLoop" );
  workLoopName_ = getApplicationDescriptor()->getClassName() + "." 
    + emu::utils::stringFrom<int>( getApplicationDescriptor()->getInstance() );
}

void emu::step::Tester::exportParameters(){
  xdata::InfoSpace *s = getApplicationInfoSpace();
  s->fireItemAvailable( "group"                     , &group_                      );
  s->fireItemAvailable( "testParametersFileName"    , &testParametersFileName_     );
  s->fireItemAvailable( "vmeSettingsFileName"       , &vmeSettingsFileName_        );
  s->fireItemAvailable( "specialVMESettingsFileName", &specialVMESettingsFileName_ );
  s->fireItemAvailable( "testId"                    , &testId_                     );
  s->fireItemAvailable( "fakeTests"                 , &fakeTests_                  );
  s->fireItemAvailable( "testDone"                  , &testDone_                   );
  s->fireItemAvailable( "crateIds"                  , &crateIds_                   );
  s->fireItemAvailable( "chamberLabels"             , &chamberLabels_              );
  s->fireItemAvailable( "progress"                  , &progress_                   );
  // s->fireItemAvailable( "", &_ );
  s->addItemRetrieveListener( "progress", this );
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

void emu::step::Tester::configureAction( toolbox::Event::Reference e ){
  LOG4CPLUS_INFO( logger_, "Configuring." );

  delete test_;
  test_ = NULL;

  testDone_ = false;

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
  
  // Schedule test procedure to be executed in a separate thread
  try{
    workLoop_->submit( testSignature_ );
    LOG4CPLUS_INFO( logger_, "Submitted test action to " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to submit test action to work loop.", e );
  }
  LOG4CPLUS_INFO( logger_, "Configured." );
}

void emu::step::Tester::enableAction( toolbox::Event::Reference e ){
  LOG4CPLUS_INFO( logger_, "Enabling." );
  // Execute the test procedure in a separate thread:
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop.", e );
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

bool emu::step::Tester::testInWorkLoop( toolbox::task::WorkLoop *wl ){
  if ( test_ ){
    LOG4CPLUS_INFO( logger_, "Executing test " << test_->getId() );
    test_->execute();
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

}

const string emu::step::Tester::workLoopType_( "waiting" );

/**
 * Provides the factory method for the instantiation of applications.
 */
XDAQ_INSTANTIATOR_IMPL(emu::step::Tester)
