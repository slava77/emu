#include "emu/step/Test.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "emu/pc/Crate.h"
#include "emu/pc/CCB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/ALCTController.h"

#include "xcept/tools.h"

#include "log4cplus/helpers/sleep.h" // sleepmillis()

#include <iomanip>
#include <cmath>

using namespace emu::utils;

emu::step::Test::Test( const string& id, 
		       const string& group,
		       const string& testParametersXML, 
		       const string& generalSettingsXML,
		       const string& specialSettingsXML,
		       const bool    isFake,
		       Logger*       pLogger               )
  : TestParameters( id, testParametersXML, pLogger )
  , group_( group )
  , isFake_( isFake )
  , isToStop_( false )
  , iEvent_( 0 )
  , procedure_( getProcedure( id ) ){

  stringstream ss;
  if ( ! procedure_ ){
    ss << "Failed to create test " << id_ << " because no procedure is defined for it.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  try{
    bsem_.take();
    createEndcap( generalSettingsXML, specialSettingsXML );
    bsem_.give();
  }
  catch ( xcept::Exception &e ){
    bsem_.give();
    ss << "Failed to create test " << id_ << ".";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
}

//emu::step::Test::~Test(){}

void emu::step::Test::execute(){
  configureCrates();
  enableTrigger();
  ( this->*emu::step::Test::procedure_ )();
  disableTrigger();
}

double emu::step::Test::getProgress(){
  double progress;
  bsem_.take();
  progress = ( nEvents_ ? 100 * double( iEvent_ ) / double( nEvents_ ) : double( 0 ) );
  bsem_.give();
  // cout << " iEvent_   " << iEvent_
  //      << " nEvents_  " << nEvents_
  //      << " progress  " << progress
  //      << endl;
  return progress;
}

void ( emu::step::Test::* emu::step::Test::getProcedure( const string& testId ) )(){
  if ( isFake_         ) return &emu::step::Test::_fake;
  if ( testId == "11"  ) return &emu::step::Test::_11;
  if ( testId == "12"  ) return &emu::step::Test::_12;
  if ( testId == "13"  ) return &emu::step::Test::_13;
  if ( testId == "14"  ) return &emu::step::Test::_14;
  if ( testId == "15"  ) return &emu::step::Test::_15;
  if ( testId == "16"  ) return &emu::step::Test::_16;
  if ( testId == "17"  ) return &emu::step::Test::_17;
  if ( testId == "17b" ) return &emu::step::Test::_17b;
  if ( testId == "18"  ) return &emu::step::Test::_18;
  if ( testId == "19"  ) return &emu::step::Test::_19;
  if ( testId == "21"  ) return &emu::step::Test::_21;
  if ( testId == "25"  ) return &emu::step::Test::_25;
  if ( testId == "27"  ) return &emu::step::Test::_27;
  return 0;
}


void emu::step::Test::createEndcap( const string& generalSettingsXML,
				    const string& specialSettingsXML  ){

  // cout << generalSettingsXML << endl;
  // cout << specialSettingsXML << endl;

  // Get the parameters to be changed for this test:
  vector< pair< string, string > > xpaths = utils::getSelectedNodesValues( specialSettingsXML, 
									   "//settings/test[@id='" + id_ + "']/set/@xpath" );
  vector< pair< string, string > > values = utils::getSelectedNodesValues( specialSettingsXML, 
									   "//settings/test[@id='" + id_ + "']/set/@value" );
  // cout << xpaths << endl;
  // cout << values << endl;
  
  // Change those parameters:
  map< string, string > valuesMap;
  vector< pair< string, string > >::const_iterator x, v;
  for ( x = xpaths.begin(), v = values.begin(); x != xpaths.end() && v != values.end(); ++x, ++v ) valuesMap[x->second] = v->second;
  // cout << valuesMap << endl;
  VME_XML_ = utils::setSelectedNodesValues( generalSettingsXML, valuesMap );

  // Save XML in file:
  string fileName( "VME_" + withoutChars( " \t:;<>'\",?/~`!@#$%^&*()=[]|\\", group_ ) + "_Test" + id_ + ".xml" );
  utils::writeFile( fileName, VME_XML_ );

  // Parse it (Endcap will be created in the process):
  parser_.parseFile( fileName.c_str() );
}

string emu::step::Test::withoutChars( const string& chars, const string& str ){
  // Remove characters listed in chars from str.
  string s( str );
  size_t pos = 0;
  while( ( pos = s.find_first_of( chars, pos ) ) != string::npos ) s.erase( pos, 1 );
  return s;
}

int emu::step::Test::getDDUInputFiberMask( int crateId, int dduSlot ){
  stringstream xPath;
  xPath << "//PeripheralCrate[@crateID='" << crateId << "']/DDU[@slot='" << dduSlot << "']/input[@dmbSlot!='']/@number";
  std::vector< std::pair< std::string, std::string > > usedInputs = utils::getSelectedNodesValues( VME_XML_, xPath.str() );
  uint16_t mask = 0;
  for ( std::vector< std::pair< std::string, std::string > >::const_iterator i = usedInputs.begin(); i != usedInputs.end(); ++i ){
    int inputNumber = utils::stringTo<int>( i->second );
    if ( 0 <= inputNumber && inputNumber <= 14 ) mask |= ( 0x0001 << inputNumber );
  }
  return mask;
}

void emu::step::Test::configureCrates(){

  cout << "emu::step::Test::configureCrates: Test " << id_ << " isFake_ = " << isFake_ << endl << flush;
  if ( isFake_ ) return;

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    if ( (*crate)->IsAlive() ){
       // (Power-on chambers if not already on) & write flash
      if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*crate)->configure( 0 ) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
      (*crate)->configure( 0 );
       // Perform a Hard-Reset to all modules in the crate and ensure configuration is uploaded to eproms
      if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*crate)->ccb()->HardReset_crate() in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
      (*crate)->ccb()->HardReset_crate();
      // Need to wait a bit for hard reset to finish, otherwise IsAlive() will be FALSE.
      ::sleep( 1 );
      // Set ccb mode once more separately, as it is screwed up inside ccb->configure
      if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*crate)->ccb()->setCCBMode( emu::pc::CCB::VMEFPGA ) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
      (*crate)->ccb()->setCCBMode( emu::pc::CCB::VMEFPGA );
      // Disable all triggers 
      if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*crate)->ccb()->DisableL1a() in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
      (*crate)->ccb()->DisableL1a();
      vector<emu::pc::DDU*> ddus = (*crate)->ddus();
      if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, ddus.size() << " DDUs" ); }
      for ( vector<emu::pc::DDU*>::iterator ddu = ddus.begin(); ddu != ddus.end(); ++ddu ){
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "DDU at " << *ddu ); }
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "DDU's controller at " << (*ddu)->getTheController() << ", DDU in slot " << (*ddu)->slot() ); }
	int dduInputFiberMask = getDDUInputFiberMask( (*crate)->CrateID(), (*ddu)->slot() );
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeFlashKillFiber(" << dduInputFiberMask << ") in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	// (*ddu)->writeFlashKillFiber(1);
	(*ddu)->writeFlashKillFiber( dduInputFiberMask );
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeGbEPrescale(0xF0F0) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	(*ddu)->writeGbEPrescale(0xF0F0);
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeFakeL1(0x8787) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	(*ddu)->writeFakeL1(0x8787);
      }
    }
    else{
      XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
    }
  }

}

void emu::step::Test::enableTrigger(){

  cout << "emu::step::Test::enableTrigger: Test " << id_ << " isFake_ = " << isFake_ << endl << flush;
  if ( isFake_ ) return;

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    if ( true ){ //( (*crate)->IsAlive() ){
	(*crate)->ccb()->l1aReset();
	(*crate)->ccb()->startTrigger(); // necessary for tmb to start triggering (alct should work with just L1A reset and bc0)
	(*crate)->ccb()->bc0(); 
    }
    else{
      XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
    }
  }

}

void emu::step::Test::disableTrigger(){

  cout << "emu::step::Test::disableTrigger: Test " << id_ << " isFake_ = " << isFake_ << endl << flush;

  if ( isFake_ ) return;

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    if ( true ){//( (*crate)->IsAlive() ){
      // disable all triggers 
      (*crate)->ccb()->DisableL1a();
    }
    else{
      XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
    }
  }
}

void emu::step::Test::setUpDMB( emu::pc::DAQMB *dmb ){

	//test values take them maybe out
	char dmbstatus[11];
	int cfeb_clk_delay=31;
	int pre_block_end=7;
	int feb_cable_delay=0;
	int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
	float dac=1.00;
	std::cout<<"start dword: "<<dword<<std::endl;
	// the below should be done via XML params
	// tmb->DisableALCTInputs();
	// tmb->DisableCLCTInputs();
	// tmb_EnableClctExtTrig(tmb);
	// ccb_EnableL1aFromDmbCfebCalibX(ccb);
	// ccb_SetExtTrigDelay(ccb, 34);
	// std::cout<<"extTrigDelay "<<34<<std::endl;
	//dmb->set_dac(0, (float)tcs.t17.dmb_test_pulse_amp * 5. / 256.);
	//loop over strips - 16
	
	dmb->calctrl_fifomrst();
          usleep(5000);
	   
          dmb->restoreCFEBIdle();
          dmb->restoreMotherboardIdle();
	  //float dac=1.00;
          dmb->set_cal_dac(dac,dac);
          dmb->setcaldelay(dword);

	  //dmb->settrgsrc(1);
	  dmb->fxpreblkend(pre_block_end);
	  dmb->SetCfebClkDelay(cfeb_clk_delay);
	  dmb->setfebdelay(dmb->GetKillFlatClk());
	  dmb->load_feb_clk_delay();
	  if(dmb->GetCfebCableDelay() == 1){
	    // In the calibration, we set all cfeb_cable_delay=0 for all DMB's for
	    // timing analysis.  If the collision setting is normally cfeb_cable_delay=1,
	    // in order to ensure the proper timing of the CFEB DAV, we will need to 
	    // adjust cfeb_dav_cable_delay to follow the change of cfeb_cable_delay...
	    dmb->SetCfebDavCableDelay(dmb->GetCfebDavCableDelay()+1);
	    std::cout<<"in cable delay==1 : "<<dmb->GetCfebDavCableDelay()+1<<"/"<<dmb->GetCableDelay()<<std::endl;
	  }
	  dmb->setcbldly(dmb->GetCableDelay());
	  dmb->SetCfebCableDelay(feb_cable_delay);
	  dmb->calctrl_global();
	  //
	  // Now check the DAQMB status.  Did the configuration "take"?
	  std::cout << "After config: check status " << std::endl;
	  usleep(50);
	  dmb->dmb_readstatus(dmbstatus);
	  if( ((dmbstatus[9]&0xff)==0x00 || (dmbstatus[9]&0xff)==0xff) || 
 	      ((dmbstatus[8]&0xff)==0x00 || (dmbstatus[8]&0xff)==0xff) ||
 	      ((dmbstatus[7]&0xff)==0x00 || (dmbstatus[7]&0xff)==0xff) ||
 	      ((dmbstatus[6]&0xff)==0x00 || (dmbstatus[6]&0xff)==0xff) ||
	      ((dmbstatus[0]&0xff)!=0x21)                              ) {
	    std::cout << "... config check not OK for DMB "  << std::endl;
	  }

}

void emu::step::Test::_11(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_11 starting" ); }
  
  uint64_t durationInSec = parameters_["durationInSec"];
  bsem_.take();
  nEvents_ = durationInSec;
  bsem_.give();

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    (*crate)->ccb()->EnableL1aFromTmbL1aReq();
    if ( isToStop_ ) return;
  }

  // Wait for durationInSec
  while ( iEvent_ < durationInSec ){
    bsem_.take();
    iEvent_++;
    bsem_.give();
    if ( isToStop_ ) return;
    ::sleep( 1 );
  }
}

// void emu::step::Test::_12(){ // Retest it
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_12 (serial) starting" ); }
  
//   const uint64_t nStrips = 6; // strips to scan, never changes
//   uint64_t events_per_strip = parameters_["events_per_strip"];
//   string pulseAmpNameBase( "alct_test_pulse_amp_" );
//   ostream noBuffer( NULL );

//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//     for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
//       nPulses += nStrips * events_per_strip;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     // if ( (*crate)->IsAlive() ){
//       // if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() ); }

//       (*crate)->ccb()->EnableL1aFromSyncAdb();

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
// 	// if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() ); }

// 	(*tmb)->EnableClctExtTrig();

// 	emu::pc::ALCTController* alct = (*tmb)->alctController();
// 	// GetChamberType returns ME<station><ring>, e.g., ME31. Strip ME part, and use the rest to create parameter name.
// 	map<string,int>::iterator pulseAmp = parameters_.find( pulseAmpNameBase + alct->GetChamberType().substr( 2 ) );
// 	if ( pulseAmp == parameters_.end() ){
// 	  XCEPT_RAISE( xcept::Exception, "No parameter called '" + pulseAmpNameBase + alct->GetChamberType().substr( 2 ) + "' found for test 12." );
// 	}

// 	for ( uint64_t iStrip = 0; iStrip < nStrips; ++iStrip ){
// 	  uint64_t stripMask = ( uint64_t(1) << iStrip );
// 	  alct->SetUpPulsing( pulseAmp->second, PULSE_LAYERS, stripMask, ADB_SYNC );

// 	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
// 	  for ( uint64_t iPulse = 1; iPulse <= events_per_strip; ++iPulse ){
// 	    (*crate)->ccb()->GenerateAlctAdbSync();
// 	    bsem_.take();
// 	    iEvent_++;
// 	    bsem_.give();
// 	    if (iPulse % 100 == 0) {
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 		   << ", strip " << iStrip+1 << "/" << nStrips
// 		   << ", pulses " << iPulse << "/" << events_per_strip << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	    }
// 	    if ( isToStop_ ) return;
// 	    // Need to slow down otherwise ALCT data will be missing in some events:
// 	    usleep(1000);
// 	  }
// 	  (*crate)->ccb()->RedirectOutput( &cout ); // get back ccb output
	  
// 	}
//       }


//     // }
//     // else{
//     //   XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
//     // }
//   }

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_12 (serial) ending" ); }
// }

void emu::step::Test::_12(){ // OK
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_12 (parallel) starting" ); }
  
  const uint64_t nStrips = 6; // strips to scan, never changes
  uint64_t events_per_strip = parameters_["events_per_strip"];
  string pulseAmpNameBase( "alct_test_pulse_amp_" );
  ostream noBuffer( NULL );

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * nStrips * events_per_strip;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    // if ( (*crate)->IsAlive() ){
    // if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() ); }

    (*crate)->ccb()->EnableL1aFromSyncAdb();

    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      (*tmb)->EnableClctExtTrig();
    } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

    for ( uint64_t iStrip = 0; iStrip < nStrips; ++iStrip ){

      uint64_t stripMask = ( uint64_t(1) << iStrip );
      for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	emu::pc::ALCTController* alct = (*tmb)->alctController();
	// GetChamberType returns ME<station><ring>, e.g., ME31. Strip ME part, and use the rest to create parameter name.
	map<string,int>::iterator pulseAmp = parameters_.find( pulseAmpNameBase + alct->GetChamberType().substr( 2 ) );
	if ( pulseAmp == parameters_.end() ){
	  XCEPT_RAISE( xcept::Exception, "No parameter called '" + pulseAmpNameBase + alct->GetChamberType().substr( 2 ) + "' found for test 12." );
	}
	alct->SetUpPulsing( pulseAmp->second, PULSE_LAYERS, stripMask, ADB_SYNC );
      }

      (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
      for ( uint64_t iPulse = 1; iPulse <= events_per_strip; ++iPulse ){
	(*crate)->ccb()->GenerateAlctAdbSync();
	bsem_.take();
	iEvent_++;
	bsem_.give();
	if (iPulse % 100 == 0) {
	  if ( pLogger_ ){
	    stringstream ss;
	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	       << ", strip " << iStrip+1 << "/" << nStrips
	       << ", pulses " << iPulse << "/" << events_per_strip << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
	  }
	}
	if ( isToStop_ ) return;
	// Need to slow down otherwise ALCT data will be missing in some events:
	usleep(1000);
      }
      (*crate)->ccb()->RedirectOutput( &cout ); // get back ccb output
	  
    }

    // }
    // else{
    //   XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
    // }
  }

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_12 (parallel) ending" ); }
}

// void emu::step::Test::_13(){ // Tested OK with old /home/cscme42/STEP/data/xml/p2.2.01/t13.xml
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_13 (serial) starting" ); }

//   uint64_t tpamps_per_run       = parameters_["tpamps_per_run"]; // how many test pulse amplitudes to scan
//   uint64_t thresholds_per_tpamp = parameters_["thresholds_per_tpamp"]; // number of thresholds to scan with each test pulse amp
//   uint64_t threshold_first      = parameters_["threshold_first"]; // first thresh
//   uint64_t threshold_step       = parameters_["threshold_step"]; // threshold step
//   uint64_t events_per_threshold = parameters_["events_per_threshold"]; // events per each threshold
//   uint64_t tpamp_first          = parameters_["tpamp_first"];
//   uint64_t tpamp_step           = parameters_["tpamp_step"];

//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     nPulses += (*crate)->tmbs().size() * events_per_threshold * thresholds_per_tpamp * tpamps_per_run;
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     //if ( (*crate)->IsAlive() ){
//       //cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

//       (*crate)->ccb()->EnableL1aFromSyncAdb();

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
// 	//cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

// 	(*tmb)->EnableClctExtTrig();

// 	emu::pc::ALCTController* alct = (*tmb)->alctController();

// 	for ( uint64_t iAmp = 0; iAmp < tpamps_per_run; ++iAmp ){
// 	  uint64_t amplitude = tpamp_first + iAmp * tpamp_step;
// 	  uint64_t afebGroupMask = 0x7f;
// 	  //cout << "    Amplitude " << iAmp << ": " << amplitude << endl << flush;

// 	  for ( uint64_t iThreshold = 0; iThreshold < thresholds_per_tpamp; ++iThreshold ){
// 	    uint64_t threshold = threshold_first + threshold_step * iThreshold;
// 	    //cout << "      Threshold " << iThreshold << ": " << threshold << endl << flush;
	    
// 	    for ( int64_t c = 0; c <= alct->MaximumUserIndex(); c++){
// 	      alct->SetAfebThreshold(c, threshold);
// 	    }

// 	    alct->WriteAfebThresholds();
	    
// 	    alct->SetUpPulsing(	amplitude, PULSE_AFEBS, afebGroupMask, ADB_SYNC );

// 	    (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	    
// 	    for ( uint64_t iPulse = 1; iPulse <= events_per_threshold; ++iPulse ){
// 	      (*crate)->ccb()->GenerateAlctAdbSync();
// 	      usleep(1000);
// 	      bsem_.take();
// 	      iEvent_++;
// 	      bsem_.give();
// 	      if (iPulse % 100 == 0) {
// 		if ( pLogger_ ){
// 		  stringstream ss;
// 		  ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		     << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 		     << ", amplitude " << iAmp+1 << "/" << tpamps_per_run
// 		     << ", threshold " << iThreshold+1 << "/" << thresholds_per_tpamp
// 		     << ", pulses " << iPulse << "/" << events_per_threshold << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		  LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 		}
// 	      }
// 	      if ( isToStop_ ) return;
// 	    } // for (iPulse = 1; iPulse <= events_per_threshold; ++iPulse)
// 	    (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	  } // for ( uint64_t iThreshold = 0; iThreshold < thresholds_per_tpamp; ++iThreshold )

// 	} // for ( uint64_t iAmp = 0; iAmp < tpamps_per_run; ++iAmp )

//       } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

//       //} // if ( (*crate)->IsAlive() )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_13 (serial) ending" ); }
// }

void emu::step::Test::_13(){ // Tested OK with old /home/cscme42/STEP/data/xml/p2.2.01/t13.xml
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_13 (parallel) starting" ); }

  uint64_t tpamps_per_run       = parameters_["tpamps_per_run"]; // how many test pulse amplitudes to scan
  uint64_t thresholds_per_tpamp = parameters_["thresholds_per_tpamp"]; // number of thresholds to scan with each test pulse amp
  uint64_t threshold_first      = parameters_["threshold_first"]; // first thresh
  uint64_t threshold_step       = parameters_["threshold_step"]; // threshold step
  uint64_t events_per_threshold = parameters_["events_per_threshold"]; // events per each threshold
  uint64_t tpamp_first          = parameters_["tpamp_first"];
  uint64_t tpamp_step           = parameters_["tpamp_step"];

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * events_per_threshold * thresholds_per_tpamp * tpamps_per_run;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    //if ( (*crate)->IsAlive() ){
    //cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

    (*crate)->ccb()->EnableL1aFromSyncAdb();
    
    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      (*tmb)->EnableClctExtTrig();
    }
    
    for ( uint64_t iAmp = 0; iAmp < tpamps_per_run; ++iAmp ){
      uint64_t amplitude = tpamp_first + iAmp * tpamp_step;
      uint64_t afebGroupMask = 0x7f;
      //cout << "    Amplitude " << iAmp << ": " << amplitude << endl << flush;
      
      for ( uint64_t iThreshold = 0; iThreshold < thresholds_per_tpamp; ++iThreshold ){
	uint64_t threshold = threshold_first + threshold_step * iThreshold;
	//cout << "      Threshold " << iThreshold << ": " << threshold << endl << flush;
	
	for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	  emu::pc::ALCTController* alct = (*tmb)->alctController();
	  for ( int64_t c = 0; c <= alct->MaximumUserIndex(); c++){
	    alct->SetAfebThreshold(c, threshold);
	  }
	  alct->WriteAfebThresholds();
	  alct->SetUpPulsing(	amplitude, PULSE_AFEBS, afebGroupMask, ADB_SYNC );
	} // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )
	
	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	
	for ( uint64_t iPulse = 1; iPulse <= events_per_threshold; ++iPulse ){
	  (*crate)->ccb()->GenerateAlctAdbSync();
	  usleep(1000);
	  bsem_.take();
	  iEvent_++;
	  bsem_.give();
	  if (iPulse % 100 == 0) {
	    if ( pLogger_ ){
	      stringstream ss;
	      ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
		 << ", amplitude " << iAmp+1 << "/" << tpamps_per_run
		 << ", threshold " << iThreshold+1 << "/" << thresholds_per_tpamp
		 << ", pulses " << iPulse << "/" << events_per_threshold << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
	    }
	  }
	  if ( isToStop_ ) return;
	} // for (iPulse = 1; iPulse <= events_per_threshold; ++iPulse)
	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	
      } // for ( uint64_t iThreshold = 0; iThreshold < thresholds_per_tpamp; ++iThreshold )
      
    } // for ( uint64_t iAmp = 0; iAmp < tpamps_per_run; ++iAmp )
    
    //} // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_13 (parallel) ending" ); }
}

// void emu::step::Test::_14(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_14 (serial) starting" ); }

//   uint64_t delays_per_run      = parameters_["delays_per_run"];
//   uint64_t delay_first         = parameters_["delay_first"];
//   uint64_t delay_step          = parameters_["delay_step"];
//   uint64_t events_per_delay    = parameters_["events_per_delay"];
//   uint64_t alct_test_pulse_amp = parameters_["alct_test_pulse_amp"];

//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     nPulses += (*crate)->tmbs().size() * delays_per_run * events_per_delay;
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     // if ( (*crate)->IsAlive() ){
//       cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

//       (*crate)->ccb()->EnableL1aFromASyncAdb();

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
// 	cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

// 	(*tmb)->EnableClctExtTrig();

// 	emu::pc::ALCTController* alct = (*tmb)->alctController();
// 	uint64_t afebGroupMask = 0x3fff; // all afebs
// 	alct->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_ASYNC );

// 	for ( uint64_t iDelay = 0; iDelay < delays_per_run; ++iDelay ){
// 	  uint64_t delay = delay_first + iDelay * delay_step;
// 	  cout << "    Delay " << iDelay << ": " << delay << endl << flush;

// 	  for ( int64_t c = 0; c <= alct->MaximumUserIndex(); c++){
// 	    alct->SetAsicDelay(c, delay);
// 	  }
	  
// 	  alct->WriteAsicDelaysAndPatterns();
	  
// 	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	  
// 	  for ( uint64_t iPulse = 1; iPulse <= events_per_delay; ++iPulse ){
// 	    (*crate)->ccb()->GenerateAlctAdbASync();
// 	    usleep(1000);
// 	    bsem_.take();
// 	    iEvent_++;
// 	    bsem_.give();
// 	    if (iPulse % 100 == 0) {
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 		   << ", delay " << iDelay+1 << "/" << delays_per_run
// 		   << ", pulses " << iPulse << "/" << events_per_delay << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	    }
// 	    if ( isToStop_ ) return;
// 	  } // for (iPulse = 1; iPulse <= events_per_delay; ++iPulse)

// 	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	} // for ( uint64_t iDelay = 0; iDelay < tpamps_per_run; ++iDelay )

//       } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

//     // } // if ( (*crate)->IsAlive() )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_14 (serial) starting" ); }
// }

void emu::step::Test::_14(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_14 (parallel) starting" ); }

  uint64_t delays_per_run      = parameters_["delays_per_run"];
  uint64_t delay_first         = parameters_["delay_first"];
  uint64_t delay_step          = parameters_["delay_step"];
  uint64_t events_per_delay    = parameters_["events_per_delay"];
  uint64_t alct_test_pulse_amp = parameters_["alct_test_pulse_amp"];

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * delays_per_run * events_per_delay;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    // if ( (*crate)->IsAlive() ){
      cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

      (*crate)->ccb()->EnableL1aFromASyncAdb();

      vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
      for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;
	(*tmb)->EnableClctExtTrig();
	uint64_t afebGroupMask = 0x3fff; // all afebs
	(*tmb)->alctController()->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_ASYNC );
      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

      for ( uint64_t iDelay = 0; iDelay < delays_per_run; ++iDelay ){
	uint64_t delay = delay_first + iDelay * delay_step;
	cout << "    Delay " << iDelay << ": " << delay << endl << flush;
	
	for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	  for ( int64_t c = 0; c <= (*tmb)->alctController()->MaximumUserIndex(); c++ ){
	    (*tmb)->alctController()->SetAsicDelay(c, delay);
	  }	  
	  (*tmb)->alctController()->WriteAsicDelaysAndPatterns();
	} // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	
	for ( uint64_t iPulse = 1; iPulse <= events_per_delay; ++iPulse ){
	  (*crate)->ccb()->GenerateAlctAdbASync();
	  usleep(1000);
	  bsem_.take();
	  iEvent_++;
	  bsem_.give();
	  if (iPulse % 100 == 0) {
	    if ( pLogger_ ){
	      stringstream ss;
	      ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
		 << ", delay " << iDelay+1 << "/" << delays_per_run
		 << ", pulses " << iPulse << "/" << events_per_delay << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
	    }
	  }
	  if ( isToStop_ ) return;
	} // for (iPulse = 1; iPulse <= events_per_delay; ++iPulse)
	
	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	
      } // for ( uint64_t iDelay = 0; iDelay < tpamps_per_run; ++iDelay )
      
      // } // if ( (*crate)->IsAlive() )
      
  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_14 (parallel) starting" ); }
}

// void emu::step::Test::_15(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_15 (serial) starting" ); }

//   uint64_t events_total = parameters_["events_total"];

//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count triggers to deliver
//   //

//   uint64_t nTriggers = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     nTriggers += (*crate)->tmbs().size() * events_total;
//   }
//   bsem_.take();
//   nEvents_ = nTriggers;
//   bsem_.give();

//   //
//   // Deliver triggers
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     // if ( (*crate)->IsAlive() ){
//       cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

//       (*crate)->ccb()->EnableL1aFromVme();
//       (*crate)->ccb()->SetExtTrigDelay( 0 ); // TODO: make configurable

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
// 	cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

// 	(*tmb)->EnableClctExtTrig();
	
// 	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	
// 	for ( uint64_t iTrigger = 1; iTrigger <= events_total; ++iTrigger ){
// 	  (*crate)->ccb()->GenerateL1A();
// 	  usleep(10);
// 	  bsem_.take();
// 	  iEvent_++;
// 	  bsem_.give();
// 	  if ( iTrigger % 100 == 0 ){
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 		   << ", triggers " << iTrigger << "/" << events_total << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	  }
// 	  if ( isToStop_ ) return;
// 	} // for (iTrigger = 1; iTrigger <= events_total; ++iTrigger)

// 	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

//       } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

//     // } // if ( (*crate)->IsAlive() )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_15 (serial) ending" ); }
// }

void emu::step::Test::_15(){ // OK
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_15 (parallel) starting" ); }

  uint64_t events_total = parameters_["events_total"];

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count triggers to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * events_total;
  bsem_.give();

  //
  // Deliver triggers
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    // if ( (*crate)->IsAlive() ){
      cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

      (*crate)->ccb()->EnableL1aFromVme();
      (*crate)->ccb()->SetExtTrigDelay( 0 ); // TODO: make configurable

      vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
      for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	// cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;
	(*tmb)->EnableClctExtTrig();
      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

      (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	
      for ( uint64_t iTrigger = 1; iTrigger <= events_total; ++iTrigger ){
	(*crate)->ccb()->GenerateL1A();
	usleep(10);
	bsem_.take();
	iEvent_++;
	bsem_.give();
	if ( iTrigger % 100 == 0 ){
	  if ( pLogger_ ){
	    stringstream ss;
	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	       << ", triggers " << iTrigger << "/" << events_total << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
	  }
	}
	if ( isToStop_ ) return;
      } // for (iTrigger = 1; iTrigger <= events_total; ++iTrigger)
      
      (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
      
    // } // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_15 (parallel) ending" ); }
}

// void emu::step::Test::_16(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_16 (serial) starting" ); }

//   const uint64_t nLayerPairs = 3; // Pairs of layers to scan, never changes. (Scans 2 layers at a time.)
//   uint64_t events_per_layer    = parameters_["events_per_layer"];
//   uint64_t alct_test_pulse_amp = parameters_["alct_test_pulse_amp"];
  
//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nTriggers = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//       nTriggers += (*crate)->tmbs().size() * nLayerPairs * events_per_layer;
//   }
//   bsem_.take();
//   nEvents_ = nTriggers;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     // if ( (*crate)->IsAlive() ){
//       //cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;

//       (*crate)->ccb()->EnableL1aFromSyncAdb();
//       (*crate)->ccb()->SetExtTrigDelay( 20 ); // TODO: make configurable

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
// 	//cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

// 	emu::pc::ALCTController* alct = (*tmb)->alctController();
// 	uint64_t afebGroupMask = 0x7f; // AFEB mask - pulse all of them
// 	alct->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_SYNC );

//       	(*tmb)->EnableClctExtTrig();

// 	alct->SetInvertPulse_(ON);    
// 	alct->FillTriggerRegister_();
// 	alct->WriteTriggerRegister_();
	
// 	for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair ){ // layer loop
// 	  //cout << "    Layers " << iLayerPair*2+1 << " and  " << iLayerPair*2+2 << endl << flush;

// 	  // reprogram standby register to enable 2 layers at a time
// 	  const int standby_fmask[nLayerPairs] = {066, 055, 033};
// 	  for (int lct_chip = 0; lct_chip < alct->MaximumUserIndex() / 6; lct_chip++){
// 	      int astandby = standby_fmask[iLayerPair];
// 	      if ( pLogger_ ){
// 		LOG4CPLUS_INFO( *pLogger_, "Setting standby " << lct_chip << " to 0x" << hex << astandby << dec );
// 	      }
// 	      for (int afeb = 0; afeb < 6; afeb++){
// 		  alct->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
// 		  // if ( pLogger_ ){
// 		  //   LOG4CPLUS_INFO( *pLogger_, "alct->SetStandbyRegister_( " << lct_chip*6 + afeb << ", 0x" << hex << ( (astandby >> afeb) & 1 ) << dec << " )" );
// 		  // }
// 	      }
// 	  }
// 	  alct->WriteStandbyRegister_();
	  
// 	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it

// 	  for ( uint64_t iPulse = 1; iPulse <= events_per_layer; ++iPulse ){
// 	    (*crate)->ccb()->GenerateAlctAdbSync();
// 	    usleep(10);
// 	    bsem_.take();
// 	    iEvent_++;
// 	    bsem_.give();
// 	    if (iPulse % 100 == 0) {
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 		   << ", layer pairs " << iLayerPair+1 << "/" << nLayerPairs
// 		   << ", pulses " << iPulse << "/" << events_per_layer << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	    }
// 	    if ( isToStop_ ) return;
// 	  } // for (iPulse = 1; iPulse <= events_per_layer; ++iPulse)

// 	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	} // for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair )

//       } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

//     // } // if ( (*crate)->IsAlive() )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_16 (serial) ending" ); }
// }

void emu::step::Test::_16(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_16 (parallel) starting" ); }

  const uint64_t nLayerPairs = 3; // Pairs of layers to scan, never changes. (Scans 2 layers at a time.)
  uint64_t events_per_layer    = parameters_["events_per_layer"];
  uint64_t alct_test_pulse_amp = parameters_["alct_test_pulse_amp"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * nLayerPairs * events_per_layer;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    // if ( (*crate)->IsAlive() ){
    //cout << "Crate " << crate-crates.begin() << " : " << (*crate)->GetLabel() << endl << flush;
    
    (*crate)->ccb()->EnableL1aFromSyncAdb();
    (*crate)->ccb()->SetExtTrigDelay( 20 ); // TODO: make configurable
    
    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      //cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;
      emu::pc::ALCTController* alct = (*tmb)->alctController();
      uint64_t afebGroupMask = 0x7f; // AFEB mask - pulse all of them
      alct->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_SYNC );
      
      (*tmb)->EnableClctExtTrig();
      
      alct->SetInvertPulse_(ON);    
      alct->FillTriggerRegister_();
      alct->WriteTriggerRegister_();
    } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

    for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair ){ // layer loop
      //cout << "    Layers " << iLayerPair*2+1 << " and  " << iLayerPair*2+2 << endl << flush;
      
      for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
	emu::pc::ALCTController* alct = (*tmb)->alctController();
	// reprogram standby register to enable 2 layers at a time
	const int standby_fmask[nLayerPairs] = {066, 055, 033};
	for (int lct_chip = 0; lct_chip < alct->MaximumUserIndex() / 6; lct_chip++){
	  int astandby = standby_fmask[iLayerPair];
	  if ( pLogger_ ){
	    LOG4CPLUS_INFO( *pLogger_, "Setting standby " << lct_chip << " to 0x" << hex << astandby << dec );
	  }
	  for (int afeb = 0; afeb < 6; afeb++){
	    alct->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
	  }
	}
	alct->WriteStandbyRegister_();
      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )
      
      (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
      
      for ( uint64_t iPulse = 1; iPulse <= events_per_layer; ++iPulse ){
	(*crate)->ccb()->GenerateAlctAdbSync();
	usleep(10);
	bsem_.take();
	iEvent_++;
	bsem_.give();
	if (iPulse % 100 == 0) {
	  if ( pLogger_ ){
	    stringstream ss;
	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	       << ", layer pairs " << iLayerPair+1 << "/" << nLayerPairs
	       << ", pulses " << iPulse << "/" << events_per_layer << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
	  }
	}
	if ( isToStop_ ) return;
      } // for (iPulse = 1; iPulse <= events_per_layer; ++iPulse)
      
      (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
      
    } // for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair )
    
    // } // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_16 (parallel) ending" ); }
}

// void emu::step::Test::_17(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17 (serial) starting" ); }

//   uint64_t events_per_delay = parameters_["events_per_delay"];
//   uint64_t delays_per_strip = parameters_["delays_per_strip"];
//   uint64_t strips_per_run   = parameters_["strips_per_run"];
//   uint64_t strip_first      = parameters_["strip_first"];
//   uint64_t strip_step       = parameters_["strip_step"];
  
//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
//       nPulses += strips_per_run * delays_per_strip * events_per_delay;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
//     (*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
//     (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: make configurable
    
//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){

//       emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
//       tmb->DisableALCTInputs();
//       tmb->DisableCLCTInputs();
//       tmb->EnableClctExtTrig();
      
//       setUpDMB( *dmb );

//       for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
// 	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
// 	(*dmb)->buck_shift();
// 	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

// 	for ( uint64_t iDelay = 0; iDelay < delays_per_strip; ++iDelay ){
// 	  uint64_t timesetting = iDelay%10 + 5;
// 	  (*dmb)->set_cal_tim_pulse( timesetting );
// 	  log4cplus::helpers::sleepmillis(400);

// 	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it

// 	  for ( uint64_t iPulse = 1; iPulse <= events_per_delay; ++iPulse ){
// 	    (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
// 	    log4cplus::helpers::sleepmillis(1);
// 	    bsem_.take();
// 	    iEvent_++;
// 	    bsem_.give();
// 	    if (iPulse % events_per_delay == 0) {
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
// 		   << ", strip " << iStrip+1 << "/" << strips_per_run
// 		   << ", delay " << iDelay+1 << "/" << delays_per_strip
// 		   << ", pulses " << iPulse << "/" << events_per_delay << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	    }
// 	    if ( isToStop_ ) return;
// 	  } // for (iPulse = 1; iPulse <= events_per_delay; ++iPulse)

// 	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	} // for ( uint64_t iDelay = 0; iDelay < delays_per_strip; ++iDelay )

//       } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){

//     } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17 (serial) ending" ); }
// }

void emu::step::Test::_17(){ // OK
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17 (parallel) starting" ); }

  uint64_t events_per_delay = parameters_["events_per_delay"];
  uint64_t delays_per_strip = parameters_["delays_per_strip"];
  uint64_t strips_per_run   = parameters_["strips_per_run"];
  uint64_t strip_first      = parameters_["strip_first"];
  uint64_t strip_step       = parameters_["strip_step"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    nPulses += strips_per_run * delays_per_strip * events_per_delay;
  }
  bsem_.take();
  nEvents_ = nPulses;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
    (*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
    (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: make configurable
    
    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too

    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
      tmb->DisableALCTInputs();
      tmb->DisableCLCTInputs();
      tmb->EnableClctExtTrig();
      setUpDMB( *dmb );
    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

    for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
      
      for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	
      } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

      for ( uint64_t iDelay = 0; iDelay < delays_per_strip; ++iDelay ){

	uint64_t timesetting = iDelay%10 + 5;
	for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	  (*dmb)->set_cal_tim_pulse( timesetting );
	} // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
	log4cplus::helpers::sleepmillis(400);

	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it

	for ( uint64_t iPulse = 1; iPulse <= events_per_delay; ++iPulse ){
	  (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
	  log4cplus::helpers::sleepmillis(1);
	  bsem_.take();
	  iEvent_++;
	  bsem_.give();
	  if (iPulse % events_per_delay == 0) {
	    if ( pLogger_ ){
	      stringstream ss;
	      ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
		 << ", strip " << iStrip+1 << "/" << strips_per_run
		 << ", delay " << iDelay+1 << "/" << delays_per_strip
		 << ", pulses " << iPulse << "/" << events_per_delay << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
	    }
	  }
	  if ( isToStop_ ) return;
	} // for (iPulse = 1; iPulse <= events_per_delay; ++iPulse)
	  
	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	  
      } // for ( uint64_t iDelay = 0; iDelay < delays_per_strip; ++iDelay )
	
    } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17 (parallel) ending" ); }
}

// void emu::step::Test::_17b(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17b (serial) starting" ); }

//   uint64_t events_per_pulsedac = parameters_["events_per_pulsedac"];
//   uint64_t pulse_dac_settings  = parameters_["pulse_dac_settings"];
//   // uint64_t  = parameters_[""];
//   // uint64_t  = parameters_[""];
//   uint64_t strips_per_run      = parameters_["strips_per_run"];
//   uint64_t strip_first         = parameters_["strip_first"];
//   uint64_t strip_step          = parameters_["strip_step"];
  
//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
//       nPulses += strips_per_run * pulse_dac_settings * events_per_pulsedac;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
//     (*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
//     (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: make configurable

//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){

//       emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
//       tmb->DisableALCTInputs();
//       tmb->DisableCLCTInputs();
//       tmb->EnableClctExtTrig();
      
//       setUpDMB( *dmb );

//       for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
// 	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
// 	(*dmb)->buck_shift();
// 	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

// 	for ( uint64_t iDACSetting = 0; iDACSetting < pulse_dac_settings; ++iDACSetting ){
// 	  double dac = 0.1 + 0.25*iDACSetting; //actual test at P5
// 	  (*dmb)->set_cal_dac( dac, dac );
// 	  ::usleep( 100 );

// 	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
    
// 	  for ( uint64_t iPulse = 1; iPulse <= events_per_pulsedac; ++iPulse ){
// 	    (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
// 	    ::usleep( 10 );
// 	    bsem_.take();
// 	    iEvent_++;
// 	    bsem_.give();
// 	    if (iPulse % events_per_pulsedac == 0) {
// 	      if ( pLogger_ ){
// 		stringstream ss;
// 		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		   << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
// 		   << ", strip " << iStrip+1 << "/" << strips_per_run
// 		   << ", DAC setting " << iDACSetting+1 << "/" << pulse_dac_settings
// 		   << ", pulses " << iPulse << "/" << events_per_pulsedac << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	      }
// 	    }
// 	    if ( isToStop_ ) return;
// 	  } // for ( uint64_t iPulse = 1; iPulse <= events_per_pulsedac; ++iPulse )

// 	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	} // for ( uint64_t iDACSetting = 0; iDACSetting < pulse_dac_settings; ++iDACSetting )

//       } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip )
      
//     } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17b (serial) ending" ); }
// }

void emu::step::Test::_17b(){ // OK
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17b (parallel) starting" ); }

  uint64_t events_per_pulsedac = parameters_["events_per_pulsedac"];
  uint64_t pulse_dac_settings  = parameters_["pulse_dac_settings"];
  uint64_t strips_per_run      = parameters_["strips_per_run"];
  uint64_t strip_first         = parameters_["strip_first"];
  uint64_t strip_step          = parameters_["strip_step"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    nPulses += strips_per_run * pulse_dac_settings * events_per_pulsedac;
  }
  bsem_.take();
  nEvents_ = nPulses;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
    (*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
    (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: make configurable

    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too

    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
      tmb->DisableALCTInputs();
      tmb->DisableCLCTInputs();
      tmb->EnableClctExtTrig();
      setUpDMB( *dmb );
    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

    for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){

      for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	
      } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

      for ( uint64_t iDACSetting = 0; iDACSetting < pulse_dac_settings; ++iDACSetting ){

	double dac = 0.1 + 0.25*iDACSetting; //actual test at P5
	for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	  (*dmb)->set_cal_dac( dac, dac );
	} // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
	::usleep( 100 );

	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
    
	for ( uint64_t iPulse = 1; iPulse <= events_per_pulsedac; ++iPulse ){
	  (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
	  ::usleep( 10 );
	  bsem_.take();
	  iEvent_++;
	  bsem_.give();
	  if (iPulse % events_per_pulsedac == 0) {
	    if ( pLogger_ ){
	      stringstream ss;
	      ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
		 << ", strip " << iStrip+1 << "/" << strips_per_run
		 << ", DAC setting " << iDACSetting+1 << "/" << pulse_dac_settings
		 << ", pulses " << iPulse << "/" << events_per_pulsedac << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
	    }
	  }
	  if ( isToStop_ ) return;
	} // for ( uint64_t iPulse = 1; iPulse <= events_per_pulsedac; ++iPulse )

	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

      } // for ( uint64_t iDACSetting = 0; iDACSetting < pulse_dac_settings; ++iDACSetting )

    } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip )
      
  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17b (parallel) ending" ); }
}

void emu::step::Test::_18(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_18 starting" ); }

  // Test of undefined duration, progress should be monitored in local DAQ.
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    (*crate)->ccb()->EnableL1aFromTmbL1aReq(); // TODO: via XML params?
  }

  // Let's stay here until we're told to stop. Only then should we go on to disable trigger.
  while( true ){
    if ( isToStop_ ) return;
    ::sleep( 1 );
  }
}

// void emu::step::Test::_19(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_19 (serial) starting" ); }

//   uint64_t events_per_thresh    = parameters_["events_per_thresh"];
//   uint64_t threshs_per_tpamp    = parameters_["threshs_per_tpamp"];
//   uint64_t thresh_first         = parameters_["thresh_first"];
//   uint64_t thresh_step          = parameters_["thresh_step"];
//   uint64_t dmb_tpamps_per_strip = parameters_["dmb_tpamps_per_strip"];
//   uint64_t dmb_tpamp_first      = parameters_["dmb_tpamp_first"];
//   uint64_t dmb_tpamp_step       = parameters_["dmb_tpamp_step"];
//   uint64_t scale_turnoff        = parameters_["scale_turnoff"];
//   uint64_t range_turnoff        = parameters_["range_turnoff"];
//   uint64_t strips_per_run       = parameters_["strips_per_run"];
//   uint64_t strip_first          = parameters_["strip_first"];
//   uint64_t strip_step           = parameters_["strip_step"];
  
//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
//       nPulses += events_per_thresh * threshs_per_tpamp * dmb_tpamps_per_strip * strips_per_run;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
//     (*crate)->ccb()->EnableL1aFromDmbCfebCalibX(); // TODO: via XML
//     (*crate)->ccb()->SetExtTrigDelay( 19 ); // TODO: via XML

//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){

//       emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
//       tmb->EnableClctExtTrig(); // TODO: via XML
      
//       for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
// 	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
// 	(*dmb)->buck_shift();
// 	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

// 	for ( uint64_t iAmp = 0; iAmp < dmb_tpamps_per_strip; ++iAmp ){
// 	  float dac = iAmp * dmb_tpamp_step + dmb_tpamp_first;
// 	  (*dmb)->set_dac( 0, dac * 5. / 4096. ); // dac values in t19 assume 12-bit DAC
// 	  // calculate thresh_first based on current dac value
// 	  thresh_first = max( int64_t( 0 ), (int64_t)(dac * scale_turnoff / 16 - range_turnoff) );

// 	  for ( uint64_t iThreshold = 0; iThreshold < threshs_per_tpamp; ++iThreshold ){
// 	    // set cfeb thresholds (for the entire test)
// 	    float threshold = (float)( iThreshold * thresh_step + thresh_first ) / 1000.;
// 	    (*dmb)->set_comp_thresh( threshold );
    
// 	    (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it

// 	    for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse ){
// 	      (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
// 	      ::usleep( 10 );
// 	      bsem_.take();
// 	      iEvent_++;
// 	      bsem_.give();
// 	      if (iPulse % events_per_thresh == 0) {
// 		if ( pLogger_ ){
// 		  stringstream ss;
// 		  ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		     << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
// 		     << ", strip " << iStrip+1 << "/" << strips_per_run
// 		     << ", amplitude " << iAmp+1 << "/" << dmb_tpamps_per_strip
// 		     << ", threshold=" << threshold << " " << iThreshold+1 << "/" << threshs_per_tpamp
// 		     << ", pulses " << iPulse << "/" << events_per_thresh << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 		  LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 		}
// 	      }
// 	      if ( isToStop_ ) return;

// 	      (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

// 	    } // for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse )
	    
// 	  } // for ( uint64_t iThreshold = 0; iThreshold < threshs_per_tpamp; ++iThreshold )
	  
// 	} // for ( uint64_t iAmp = 0; iAmp < dmb_tpamps_per_strip; ++iAmp )
	
//       } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip )

//     } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_19 (serial) ending" ); }
// }

void emu::step::Test::_19(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_19 (parallel) starting" ); }

  uint64_t events_per_thresh    = parameters_["events_per_thresh"];
  uint64_t threshs_per_tpamp    = parameters_["threshs_per_tpamp"];
  //uint64_t thresh_first         = parameters_["thresh_first"];
  uint64_t thresh_step          = parameters_["thresh_step"];
  uint64_t dmb_tpamps_per_strip = parameters_["dmb_tpamps_per_strip"];
  uint64_t dmb_tpamp_first      = parameters_["dmb_tpamp_first"];
  uint64_t dmb_tpamp_step       = parameters_["dmb_tpamp_step"];
  uint64_t scale_turnoff        = parameters_["scale_turnoff"];
  uint64_t range_turnoff        = parameters_["range_turnoff"];
  uint64_t strips_per_run       = parameters_["strips_per_run"];
  uint64_t strip_first          = parameters_["strip_first"];
  uint64_t strip_step           = parameters_["strip_step"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * events_per_thresh * threshs_per_tpamp * dmb_tpamps_per_strip * strips_per_run;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
    (*crate)->ccb()->EnableL1aFromDmbCfebCalibX(); // TODO: via XML
    (*crate)->ccb()->SetExtTrigDelay( 19 ); // TODO: via XML

    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs();
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
      tmb->EnableClctExtTrig(); // TODO: via XML
    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )


    for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){

      for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	
      } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
      
      for ( uint64_t iAmp = 0; iAmp < dmb_tpamps_per_strip; ++iAmp ){
	
	vector<uint64_t> first_thresholds; // the first threshold for each DMB
	for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	  float dac = iAmp * dmb_tpamp_step + dmb_tpamp_first;
	  (*dmb)->set_dac( 0, dac * 5. / 4096. ); // dac values in t19 assume 12-bit DAC
	  // calculate first thresholds based on current dac value
	  first_thresholds.push_back( max( int64_t( 0 ), (int64_t)(dac * scale_turnoff / 16 - range_turnoff) ) );
	} // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
	
	
	for ( uint64_t iThreshold = 0; iThreshold < threshs_per_tpamp; ++iThreshold ){
	  
	  vector<uint64_t>::const_iterator first_threshold = first_thresholds.begin();
	  for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	    // set cfeb thresholds (for the entire test)
	    float threshold = (float)( iThreshold * thresh_step + *first_threshold ) / 1000.;
	    (*dmb)->set_comp_thresh( threshold );
	    ++first_threshold;
	  } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
	  
	  (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	  
	  for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse ){
	    (*crate)->ccb()->GenerateDmbCfebCalib0(); // pulse
	    ::usleep( 10 );
	    bsem_.take();
	    iEvent_++;
	    bsem_.give();
	    if (iPulse % events_per_thresh == 0) {
	      if ( pLogger_ ){
		stringstream ss;
		ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
		   << ", strip " << iStrip+1 << "/" << strips_per_run
		   << ", amplitude " << iAmp+1 << "/" << dmb_tpamps_per_strip
		   << ", threshold " << " " << iThreshold+1 << "/" << threshs_per_tpamp
		   << ", pulses " << iPulse << "/" << events_per_thresh << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
		LOG4CPLUS_INFO( *pLogger_, ss.str() );
	      }
	    }
	    if ( isToStop_ ) return;
	    
	    (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	    
	  } // for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse )
	  
	} // for ( uint64_t iThreshold = 0; iThreshold < threshs_per_tpamp; ++iThreshold )
	
      } // for ( uint64_t iAmp = 0; iAmp < dmb_tpamps_per_strip; ++iAmp )
      
    } // for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip )
    
  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )
  
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_19 (parallel) ending" ); }
}

// void emu::step::Test::_21(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_21 (serial) starting" ); }
//   uint64_t dmb_test_pulse_amp = parameters_["dmb_test_pulse_amp"];
//   uint64_t cfeb_threshold = parameters_["cfeb_threshold"];
//   uint64_t events_per_hstrip  = parameters_["events_per_hstrip"];
//   uint64_t hstrips_per_run = parameters_["hstrips_per_run"];
//   uint64_t hstrip_first = parameters_["hstrip_first"];
//   uint64_t hstrip_step = parameters_["hstrip_step"];
//   // uint64_t  = parameters_[""];
//   // uint64_t  = parameters_[""];
//   // uint64_t  = parameters_[""];
  
//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
//   ostream noBuffer( NULL );

//   //
//   // Count pulses to deliver
//   //

//   uint64_t nPulses = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
//       nPulses += events_per_hstrip * hstrips_per_run;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nPulses;
//   bsem_.give();

//   //
//   // Deliver pulses
//   //

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
//     (*crate)->ccb()->EnableL1aFromDmbCfebCalibX(); // TODO: via XML
//     (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: via XML

//     vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
//     for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
//       (*dmb)->set_dac( (float)dmb_test_pulse_amp * 5. / 256., 0 ); // set inject amplitude - first parameter (same for the entire test) // TODO: via XML
//       (*dmb)->set_comp_thresh( (float)cfeb_threshold / 1000. ); // set cfeb thresholds (for the entire test) // TODO: via XML
//       (*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT // TODO: via XML

//       emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
//       tmb->EnableClctExtTrig(); // TODO: via XML
      
//       for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip ){

// 	int64_t halfStrip = iHalfStrip * hstrip_step + hstrip_first - 1;
	
// 	(*dmb)->trighalfx( halfStrip );

// 	(*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
	
// 	for ( uint64_t iPulse = 1; iPulse <= events_per_hstrip; ++iPulse ){
// 	  (*crate)->ccb()->GenerateDmbCfebCalib1(); // pulse
// 	  ::usleep( 10 );
// 	  bsem_.take();
// 	  iEvent_++;
// 	  bsem_.give();
// 	  if (iPulse % 100 == 0) {
// 	    if ( pLogger_ ){
// 	      stringstream ss;
// 	      ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 		 << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
// 		 << ", half strip " << iHalfStrip+1 << "/" << hstrips_per_run << " (" << halfStrip << ")"
// 		 << ", pulses " << iPulse  << "/" << events_per_hstrip << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
// 	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	    }
// 	  }
// 	  if ( isToStop_ ) return;
	  
// 	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	  
// 	} // for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse )

//       } // for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip )

//     } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_21 (serial) ending" ); }
// }

void emu::step::Test::_21(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_21 (parallel) starting" ); }
  uint64_t dmb_test_pulse_amp = parameters_["dmb_test_pulse_amp"];
  uint64_t cfeb_threshold = parameters_["cfeb_threshold"];
  uint64_t events_per_hstrip  = parameters_["events_per_hstrip"];
  uint64_t hstrips_per_run = parameters_["hstrips_per_run"];
  uint64_t hstrip_first = parameters_["hstrip_first"];
  uint64_t hstrip_step = parameters_["hstrip_step"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  bsem_.take();
  nEvents_ = crates.size() * events_per_hstrip * hstrips_per_run;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
    (*crate)->ccb()->EnableL1aFromDmbCfebCalibX(); // TODO: via XML
    (*crate)->ccb()->SetExtTrigDelay( 17 ); // TODO: via XML

    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      (*dmb)->set_dac( (float)dmb_test_pulse_amp * 5. / 256., 0 ); // set inject amplitude - first parameter (same for the entire test) // TODO: via XML
      (*dmb)->set_comp_thresh( (float)cfeb_threshold / 1000. ); // set cfeb thresholds (for the entire test) // TODO: via XML
      (*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT // TODO: via XML

      emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
      tmb->EnableClctExtTrig(); // TODO: via XML
    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

    for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip ){
      
      int64_t halfStrip = iHalfStrip * hstrip_step + hstrip_first - 1;
      
      for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
	(*dmb)->trighalfx( halfStrip );
      } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )
      
      (*crate)->ccb()->RedirectOutput( &noBuffer ); // ccb prints a line on each test pulse - waste it
      
      for ( uint64_t iPulse = 1; iPulse <= events_per_hstrip; ++iPulse ){
	(*crate)->ccb()->GenerateDmbCfebCalib1(); // pulse
	::usleep( 10 );
	bsem_.take();
	iEvent_++;
	bsem_.give();
	if (iPulse % 100 == 0) {
	  if ( pLogger_ ){
	    stringstream ss;
	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	       << ", half strip " << iHalfStrip+1 << "/" << hstrips_per_run << " (" << halfStrip << ")"
	       << ", pulses " << iPulse  << "/" << events_per_hstrip << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
	  }
	}
	if ( isToStop_ ) return;
	
	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	
      } // for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse )
      
    } // for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip )
    
  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )
  
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_21 (parallel) ending" ); }
}

// void emu::step::Test::_25(){
//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_25 (single chamber) starting" ); }

//   uint64_t trig_settings       = parameters_["trig_settings"];

//   ostream noBuffer( NULL );

//   vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();

//   //
//   // Count settings to apply
//   //

//   uint64_t nSettings = 0;
//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
//     vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//     for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
//       nSettings += trig_settings;
//     }
//   }
//   bsem_.take();
//   nEvents_ = nSettings;
//   bsem_.give();

//   //
//   // Apply settings
//   //

//   string dateTime( emu::utils::getDateTime( true ) );

//   struct timeval start, end;

//   usleep(100);

//   for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){

//       vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
//       for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){

// 	emu::pc::ALCTController* alct = (*tmb)->alctController();
// 	alct->configure();
// 	::usleep(100);

// 	stringstream timeStampFileName;
// 	timeStampFileName << "Test25_"  << (*crate)->GetLabel()
// 			  << "_TMBslot" << (*tmb)->slot()
// 			  << "_"        << dateTime
// 			  << ".txt";
// 	ofstream timeStampFile;
// 	timeStampFile.open( timeStampFileName.str().c_str() );
// 	timeStampFile << "#crate    :\t" << (*crate)->GetLabel() << endl;
// 	timeStampFile << "#TMB slot :\t" << (*tmb)->slot() << endl;
// 	timeStampFile << "#chamber  :\t" << (*tmb)->getChamber()->GetLabel() << endl;
// 	timeStampFile << "#time [ms]\tevent counts" << endl;
// 	for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting ){

// 	  if ( iTriggerSetting == 0 ){
// 	    alct->SetPretrigNumberOfLayers( 1 );
// 	  }
// 	  else{
// 	    alct->SetPretrigNumberOfLayers( 2 );
// 	  }
// 	  alct->SetPretrigNumberOfPattern( iTriggerSetting + 1 );
// 	  alct->WriteConfigurationReg();

// 	  log4cplus::helpers::sleepmillis( ( iTriggerSetting == 1 ? 50000 : 20000 ) );

// 	  (*crate)->ccb()->WriteRegister( emu::pc::CCB::enableL1aCounter, 0 );
// 	  (*crate)->ccb()->WriteRegister( emu::pc::CCB::resetL1aCounter , 0 ); // zero L1A counter

// 	  // (*crate)->ccb()->WriteRegister( emu::pc::CCB::CSRB1, 0x1edd ); // CSRB1=0x20; enable CCB to send L1A on TMB request; same as CCB::EnableL1aFromTmbL1aReq
// 	  (*crate)->ccb()->EnableL1aFromTmbL1aReq();
// 	  gettimeofday( &start, NULL );
// 	  log4cplus::helpers::sleepmillis( ( iTriggerSetting == 0 ? 5000 : (iTriggerSetting+1)*10000 ) );
// 	  // (*crate)->ccb()->WriteRegister( emu::pc::CCB::CSRB1, 0x1af9 ); // CSRB1=0x20; disable CCB to send L1A on TMB request; same as CCB::EnableL1aFromDmbCfebCalibX
// 	  (*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
// 	  gettimeofday( &end, NULL );

// 	  bsem_.take();
// 	  iEvent_++;
// 	  bsem_.give();

// 	  uint32_t l1a_counter_LSB = (*crate)->ccb()->ReadRegister( emu::pc::CCB::readL1aCounterLSB ) & 0xffff; // read lower 16 bits
// 	  uint32_t l1a_counter_MSB = (*crate)->ccb()->ReadRegister( emu::pc::CCB::readL1aCounterMSB ) & 0xffff; // read higher 16 bits
// 	  uint32_t l1a_counter     = l1a_counter_LSB | (l1a_counter_MSB << 16); // merge into counter
// 	  timeStampFile 
// 	    << ( end.tv_sec - start.tv_sec) * 1000 + ( end.tv_usec - start.tv_usec ) / 1000. 
// 	    << "\t\t" << l1a_counter << endl;

// 	  if ( pLogger_ ){
// 	    stringstream ss;
// 	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
// 	       << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
// 	       << ", trigger setting " << iTriggerSetting+1 << "/" << trig_settings
// 	       << ", L1A " << l1a_counter;
// 	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
// 	  }
	  
// 	} // for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting )

// 	timeStampFile.close();

//       } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

//   } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

//   if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_25 (single chamber) ending" ); }
// }

void emu::step::Test::_25(){
  // TODO:
  // This can be run with one chamber per crate at a time while the other chambers in the crate must be disabled.
  // In principle, it could be run in parallel in different crates, but only if their data all go into different files...
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_25 (multichamber) starting" ); }

  uint64_t trig_settings       = parameters_["trig_settings"];

  ostream noBuffer( NULL );

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();

  //
  // Count settings to apply
  //

  uint64_t nSettings = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      nSettings += trig_settings;
    }
  }
  bsem_.take();
  nEvents_ = nSettings;
  bsem_.give();

  //
  // Apply settings
  //

  string dateTime( emu::utils::getDateTime( true ) );

  struct timeval start, end;

  usleep(100);

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){

    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();

    // Loop over TMBs to test them one by one.
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      
      // Disable ALCT trigger in all TMBs...
      for ( vector<emu::pc::TMB*>::iterator t = tmbs.begin(); t != tmbs.end(); ++t ){
	(*t)->SetAlctPatternTrigEnable( 0 ); // same as alct_pretrig_enable="0" in XML
	(*t)->SetTmbAllowAlct( 0 );          // same as alct_trig_enable="0" in XML
      } // for ( vector<emu::pc::TMB*>::iterator t = tmbs.begin(); t != tmbs.end(); ++t )
      // ...except for the one that's being tested:
      (*tmb)->SetAlctPatternTrigEnable( 1 ); // same as alct_pretrig_enable="1" in XML
      (*tmb)->SetTmbAllowAlct( 1 );          // same as alct_trig_enable="1" in XML

      emu::pc::ALCTController* alct = (*tmb)->alctController();
      alct->configure();
      ::usleep(100);
      
      stringstream timeStampFileName;
      timeStampFileName << "Test25_"  << (*crate)->GetLabel()
			<< "_TMBslot" << (*tmb)->slot()
			<< "_"        << dateTime
			<< ".txt";
      ofstream timeStampFile;
      timeStampFile.open( timeStampFileName.str().c_str() );
      timeStampFile << "#crate    :\t" << (*crate)->GetLabel() << endl;
      timeStampFile << "#TMB slot :\t" << (*tmb)->slot() << endl;
      timeStampFile << "#chamber  :\t" << (*tmb)->getChamber()->GetLabel() << endl;
      timeStampFile << "#time [ms]\tevent counts" << endl;
      for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting ){
	
	if ( iTriggerSetting == 0 ){
	  alct->SetPretrigNumberOfLayers( 1 );
	}
	else{
	  alct->SetPretrigNumberOfLayers( 2 );
	}
	alct->SetPretrigNumberOfPattern( iTriggerSetting + 1 );
	alct->WriteConfigurationReg();
	
	log4cplus::helpers::sleepmillis( ( iTriggerSetting == 1 ? 50000 : 20000 ) );
	
	(*crate)->ccb()->WriteRegister( emu::pc::CCB::enableL1aCounter, 0 );
	(*crate)->ccb()->WriteRegister( emu::pc::CCB::resetL1aCounter , 0 ); // zero L1A counter
	
	// (*crate)->ccb()->WriteRegister( emu::pc::CCB::CSRB1, 0x1edd ); // CSRB1=0x20; enable CCB to send L1A on TMB request; same as CCB::EnableL1aFromTmbL1aReq
	(*crate)->ccb()->EnableL1aFromTmbL1aReq();
	gettimeofday( &start, NULL );
	log4cplus::helpers::sleepmillis( ( iTriggerSetting == 0 ? 5000 : (iTriggerSetting+1)*10000 ) );
	// (*crate)->ccb()->WriteRegister( emu::pc::CCB::CSRB1, 0x1af9 ); // CSRB1=0x20; disable CCB to send L1A on TMB request; same as CCB::EnableL1aFromDmbCfebCalibX
	(*crate)->ccb()->EnableL1aFromDmbCfebCalibX();
	gettimeofday( &end, NULL );
	
	bsem_.take();
	iEvent_++;
	bsem_.give();
	
	uint32_t l1a_counter_LSB = (*crate)->ccb()->ReadRegister( emu::pc::CCB::readL1aCounterLSB ) & 0xffff; // read lower 16 bits
	uint32_t l1a_counter_MSB = (*crate)->ccb()->ReadRegister( emu::pc::CCB::readL1aCounterMSB ) & 0xffff; // read higher 16 bits
	uint32_t l1a_counter     = l1a_counter_LSB | (l1a_counter_MSB << 16); // merge into counter
	timeStampFile 
	  << ( end.tv_sec - start.tv_sec) * 1000 + ( end.tv_usec - start.tv_usec ) / 1000. 
	  << "\t\t" << l1a_counter << endl;
	
	if ( pLogger_ ){
	  stringstream ss;
	  ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	     << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
	     << ", trigger setting " << iTriggerSetting+1 << "/" << trig_settings
	     << ", L1A " << l1a_counter;
	  LOG4CPLUS_INFO( *pLogger_, ss.str() );
	}
	
      } // for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting )
      
      timeStampFile.close();
      
    } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_25 (multichamber) ending" ); }
}

void emu::step::Test::_27(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_27 starting" ); }
  
  // Test of undefined duration, progress should be monitored in local DAQ.
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    (*crate)->ccb()->EnableL1aFromTmbL1aReq();
    if ( isToStop_ ) return;
  }

  // Let's stay here until we're told to stop. Only then should we go on to disable trigger.
  while( true ){
    if ( isToStop_ ) return;
    ::sleep( 1 );
  }
}

void emu::step::Test::_fake(){
  cout << "emu::step::Test::_fake called" << endl << flush;
  uint64_t nEvents = 10;
  bsem_.take();
  nEvents_ = nEvents;
  bsem_.give();
  for ( uint64_t iEvent = 0; iEvent < nEvents; ++iEvent ){
    if ( isToStop_ ){
      cout << "emu::step::Test::_fake interrupted." << endl << flush;
      return;
    }
    cout << "  Event " << iEvent << endl << flush;
    bsem_.take();
    iEvent_ = iEvent;
    bsem_.give();
    ::sleep( 1 );    
  }
  bsem_.take();
  iEvent_ = nEvents;
  bsem_.give();
  cout << "emu::step::Test::_fake returning" << endl << flush;
}
