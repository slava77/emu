#include "emu/step/Test.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "emu/pc/Crate.h"
#include "emu/pc/CCB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ODAQMB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/ALCTController.h"

#include "xcept/tools.h"

#include "log4cplus/helpers/sleep.h" // sleepmillis()

#include <iomanip>
#include <cmath>

using namespace emu::utils;

emu::step::Test::Test( const string& id, 
		       const string& testParametersXML, 
		       const string& generalSettingsXML,
		       const string& specialSettingsXML,
		       const bool    isFake,
		       Logger*       pLogger               )
  : TestParameters( id, testParametersXML, pLogger )
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
  string VME_XML = utils::setSelectedNodesValues( generalSettingsXML, valuesMap );

  // Save XML in file:
  string fileName( "VME_Test_" + id_ + ".xml" );
  utils::writeFile( fileName, VME_XML );

  // Parse it (Endcap will be created in the process):
  parser_.parseFile( fileName.c_str() );
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
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeFlashKillFiber(1) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	(*ddu)->writeFlashKillFiber(1);
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeGbEPrescale(0xF0F0) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	(*ddu)->writeGbEPrescale(0xF0F0); // TODO: double check this
	if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "(*ddu)->writeFakeL1(0x8787) in " << ((*crate)->IsAlive()?"live":"dead") << " crate " << (*crate)->GetLabel() ); }
	(*ddu)->writeFakeL1(0x8787); // TODO: double check this
      }
      // TODO: prepare DDUs in FED crates.
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
  cout << "emu::step::Test::_11" << endl;
  
  // Passive test, progress should be monitored in local DAQ. (TODO)
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    (*crate)->ccb()->EnableL1aFromTmbL1aReq();
    if ( isToStop_ ) return;
  }
}

void emu::step::Test::_12(){ // Retest it
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_12 starting" ); }
  
  const uint64_t nStrips = 6; // strips to scan, never changes
  uint64_t events_per_strip = parameters_["events_per_strip"];
  string pulseAmpNameBase( "alct_test_pulse_amp_" );
  ostream noBuffer( NULL );

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();

  //
  // Count pulses to deliver
  //

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
    for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){
      nPulses += nStrips * events_per_strip;
    }
  }
  bsem_.take();
  nEvents_ = nPulses;
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
	// if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() ); }

	(*tmb)->EnableClctExtTrig();

	emu::pc::ALCTController* alct = (*tmb)->alctController();
	// GetChamberType returns ME<station><ring>, e.g., ME31. Strip ME part, and use the rest to create parameter name.
	map<string,int>::iterator pulseAmp = parameters_.find( pulseAmpNameBase + alct->GetChamberType().substr( 2 ) );
	if ( pulseAmp == parameters_.end() ){
	  XCEPT_RAISE( xcept::Exception, "No parameter called '" + pulseAmpNameBase + alct->GetChamberType().substr( 2 ) + "' found for test 12." );
	}

	for ( uint64_t iStrip = 0; iStrip < nStrips; ++iStrip ){
	  uint64_t stripMask = ( uint64_t(1) << iStrip );
	  alct->SetUpPulsing( pulseAmp->second, PULSE_LAYERS, stripMask, ADB_SYNC );

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
		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
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
      }


    // }
    // else{
    //   XCEPT_RAISE( xcept::Exception, "Crate " + (*crate)->GetLabel() + " is dead or incommunicado." );
    // }
  }

}

void emu::step::Test::_13(){ // Tested OK with old /home/cscme42/STEP/data/xml/p2.2.01/t13.xml
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_13 starting" ); }

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

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    nPulses += (*crate)->tmbs().size() * events_per_threshold * thresholds_per_tpamp * tpamps_per_run;
  }
  bsem_.take();
  nEvents_ = nPulses;
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
	//cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

	(*tmb)->EnableClctExtTrig();

	emu::pc::ALCTController* alct = (*tmb)->alctController();

	for ( uint64_t iAmp = 0; iAmp < tpamps_per_run; ++iAmp ){
	  uint64_t amplitude = tpamp_first + iAmp * tpamp_step;
	  uint64_t afebGroupMask = 0x7f;
	  //cout << "    Amplitude " << iAmp << ": " << amplitude << endl << flush;

	  for ( uint64_t iThreshold = 0; iThreshold < thresholds_per_tpamp; ++iThreshold ){
	    uint64_t threshold = threshold_first + threshold_step * iThreshold;
	    //cout << "      Threshold " << iThreshold << ": " << threshold << endl << flush;
	    
	    for ( int64_t c = 0; c <= alct->MaximumUserIndex(); c++){
	      alct->SetAfebThreshold(c, threshold);
	    }

	    alct->WriteAfebThresholds();
	    
	    alct->SetUpPulsing(	amplitude, PULSE_AFEBS, afebGroupMask, ADB_SYNC );

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
		     << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
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

      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

      //} // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_14(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_14 starting" ); }

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

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    nPulses += (*crate)->tmbs().size() * delays_per_run * events_per_delay;
  }
  bsem_.take();
  nEvents_ = nPulses;
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

	emu::pc::ALCTController* alct = (*tmb)->alctController();
	uint64_t afebGroupMask = 0x3fff; // all afebs
	alct->SetUpPulsing( alct_test_pulse_amp, PULSE_AFEBS, afebGroupMask, ADB_ASYNC );

	for ( uint64_t iDelay = 0; iDelay < delays_per_run; ++iDelay ){
	  uint64_t delay = delay_first + iDelay * delay_step;
	  cout << "    Delay " << iDelay << ": " << delay << endl << flush;

	  for ( int64_t c = 0; c <= alct->MaximumUserIndex(); c++){
	    alct->SetAsicDelay(c, delay);
	  }
	  
	  alct->WriteAsicDelaysAndPatterns();
	  
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
		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
		   << ", delay " << iDelay+1 << "/" << delays_per_run
		   << ", pulses " << iPulse << "/" << events_per_delay << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
		LOG4CPLUS_INFO( *pLogger_, ss.str() );
	      }
	    }
	    if ( isToStop_ ) return;
	  } // for (iPulse = 1; iPulse <= events_per_delay; ++iPulse)

	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

	} // for ( uint64_t iDelay = 0; iDelay < tpamps_per_run; ++iDelay )

      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

    // } // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_15(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_15 starting" ); }

  uint64_t events_total = parameters_["events_total"];

  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count triggers to deliver
  //

  uint64_t nTriggers = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    nTriggers += (*crate)->tmbs().size() * events_total;
  }
  bsem_.take();
  nEvents_ = nTriggers;
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
	cout << "  TMB " << tmb-tmbs.begin() << " in slot " << (*tmb)->slot() << endl << flush;

	(*tmb)->EnableClctExtTrig();
	
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
		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
		   << ", triggers " << iTrigger << "/" << events_total << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
		LOG4CPLUS_INFO( *pLogger_, ss.str() );
	      }
	  }
	  if ( isToStop_ ) return;
	} // for (iTrigger = 1; iTrigger <= events_total; ++iTrigger)

	(*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

    // } // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_16(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_16 starting" ); }

  const uint64_t nLayerPairs = 3; // Pairs of layers to scan, never changes. (Scans 2 layers at a time.)
  uint64_t events_per_layer    = parameters_["events_per_layer"];
  uint64_t alct_test_pulse_amp = parameters_["alct_test_pulse_amp"];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  uint64_t nTriggers = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
      nTriggers += (*crate)->tmbs().size() * nLayerPairs * events_per_layer;
  }
  bsem_.take();
  nEvents_ = nTriggers;
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
	
	for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair ){ // layer loop
	  //cout << "    Layers " << iLayerPair*2+1 << " and  " << iLayerPair*2+2 << endl << flush;

	  // reprogram standby register to enable 2 layers at a time
	  const int standby_fmask[nLayerPairs] = {066, 055, 033};
	  for (int lct_chip = 0; lct_chip < alct->MaximumUserIndex() / 6; lct_chip++){
	      int astandby = standby_fmask[iLayerPair];
	      if ( pLogger_ ){
		LOG4CPLUS_INFO( *pLogger_, "Setting standby " << lct_chip << " to 0x" << hex << astandby << dec );
	      }
	      for (int afeb = 0; afeb < 6; afeb++){
		  alct->SetStandbyRegister_(lct_chip*6 + afeb, (astandby >> afeb) & 1);
		  // if ( pLogger_ ){
		  //   LOG4CPLUS_INFO( *pLogger_, "alct->SetStandbyRegister_( " << lct_chip*6 + afeb << ", 0x" << hex << ( (astandby >> afeb) & 1 ) << dec << " )" );
		  // }
	      }
	  }
	  alct->WriteStandbyRegister_();
	  
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
		   << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
		   << ", layer pairs " << iLayerPair+1 << "/" << nLayerPairs
		   << ", pulses " << iPulse << "/" << events_per_layer << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
		LOG4CPLUS_INFO( *pLogger_, ss.str() );
	      }
	    }
	    if ( isToStop_ ) return;
	  } // for (iPulse = 1; iPulse <= events_per_layer; ++iPulse)

	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output

	} // for ( uint64_t iLayerPair = 0; iLayerPair < nLayerPairs; ++iLayerPair )

      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

    // } // if ( (*crate)->IsAlive() )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_17(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17 starting" ); }

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
    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      nPulses += strips_per_run * delays_per_strip * events_per_delay;
    }
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

      for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

	for ( uint64_t iDelay = 0; iDelay < delays_per_strip; ++iDelay ){
	  uint64_t timesetting = iDelay%10 + 5;
	  (*dmb)->set_cal_tim_pulse( timesetting );
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
		   << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
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

    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_17b(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_17b starting" ); }

  uint64_t events_per_pulsedac = parameters_["events_per_pulsedac"];
  uint64_t pulse_dac_settings  = parameters_["pulse_dac_settings"];
  // uint64_t  = parameters_[""];
  // uint64_t  = parameters_[""];
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
    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      nPulses += strips_per_run * pulse_dac_settings * events_per_pulsedac;
    }
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

      for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

	for ( uint64_t iDACSetting = 0; iDACSetting < pulse_dac_settings; ++iDACSetting ){
	  double dac = 0.1 + 0.25*iDACSetting; //actual test at P5
	  (*dmb)->set_cal_dac( dac, dac );
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
		   << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
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
      
    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_18(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_18 starting" ); }

  // Passive test, progress should be monitored in local DAQ. (TODO)
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    (*crate)->ccb()->EnableL1aFromTmbL1aReq(); // TODO: via XML params?
  }

}

void emu::step::Test::_19(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_19 starting" ); }

  uint64_t events_per_thresh    = parameters_["events_per_thresh"];
  uint64_t threshs_per_tpamp    = parameters_["threshs_per_tpamp"];
  uint64_t thresh_first         = parameters_["thresh_first"];
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

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      nPulses += events_per_thresh * threshs_per_tpamp * dmb_tpamps_per_strip * strips_per_run;
    }
  }
  bsem_.take();
  nEvents_ = nPulses;
  bsem_.give();

  //
  // Deliver pulses
  //

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    
    (*crate)->ccb()->EnableL1aFromDmbCfebCalibX(); // TODO: via XML
    (*crate)->ccb()->SetExtTrigDelay( 19 ); // TODO: via XML

    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){

      emu::pc::TMB* tmb = (*crate)->GetChamber( *dmb )->GetTMB();
      tmb->EnableClctExtTrig(); // TODO: via XML
      
      for ( uint64_t iStrip = 0; iStrip < strips_per_run; ++iStrip ){
	(*dmb)->set_ext_chanx( iStrip * strip_step + strip_first - 1 ); // strips start from 1 in config file (is that important for analysis?)
	(*dmb)->buck_shift();
	(*dmb)->settrgsrc(0); // disable DMB's own trigger, LCT, should be via XML	

	for ( uint64_t iAmp = 0; iAmp < dmb_tpamps_per_strip; ++iAmp ){
	  float dac = iAmp * dmb_tpamp_step + dmb_tpamp_first;
	  (*dmb)->set_dac( 0, dac * 5. / 4096. ); // dac values in t19 assume 12-bit DAC
	  // calculate thresh_first based on current dac value
	  thresh_first = max( int64_t( 0 ), (int64_t)(dac * scale_turnoff / 16 - range_turnoff) );

	  for ( uint64_t iThreshold = 0; iThreshold < threshs_per_tpamp; ++iThreshold ){
	    // set cfeb thresholds (for the entire test)
	    float threshold = (float)( iThreshold * thresh_step + thresh_first ) / 1000.;
	    (*dmb)->set_comp_thresh( threshold );
    
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
		     << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
		     << ", strip " << iStrip+1 << "/" << strips_per_run
		     << ", amplitude " << iAmp+1 << "/" << dmb_tpamps_per_strip
		     << ", threshold=" << threshold << " " << iThreshold+1 << "/" << threshs_per_tpamp
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

    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_21(){
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_21 starting" ); }
  uint64_t dmb_test_pulse_amp = parameters_["dmb_test_pulse_amp"];
  uint64_t cfeb_threshold = parameters_["cfeb_threshold"];
  uint64_t events_per_hstrip  = parameters_["events_per_hstrip"];
  uint64_t hstrips_per_run = parameters_["hstrips_per_run"];
  uint64_t hstrip_first = parameters_["hstrip_first"];
  uint64_t hstrip_step = parameters_["hstrip_step"];
  // uint64_t  = parameters_[""];
  // uint64_t  = parameters_[""];
  // uint64_t  = parameters_[""];
  
  vector<emu::pc::Crate*> crates = parser_.GetEmuEndcap()->crates();
  ostream noBuffer( NULL );

  //
  // Count pulses to deliver
  //

  uint64_t nPulses = 0;
  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){
    vector<emu::pc::DAQMB *> dmbs = (*crate)->daqmbs(); // TODO: for ODAQMBs, too
    for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb ){
      nPulses += events_per_hstrip * hstrips_per_run;
    }
  }
  bsem_.take();
  nEvents_ = nPulses;
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
      
      for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip ){

	int64_t halfStrip = iHalfStrip * hstrip_step + hstrip_first - 1;
	
	(*dmb)->trighalfx( halfStrip );

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
		 << ", DMB " << dmb-dmbs.begin()+1 << "/" << dmbs.size() << " in slot " << (*dmb)->slot()
		 << ", half strip " << iHalfStrip+1 << "/" << hstrips_per_run << " (" << halfStrip << ")"
		 << ", pulses " << iPulse  << "/" << events_per_hstrip << " ("<< iEvent_ << " of " << nEvents_ << " in total)";
	      LOG4CPLUS_INFO( *pLogger_, ss.str() );
	    }
	  }
	  if ( isToStop_ ) return;
	  
	  (*crate)->ccb()->RedirectOutput (&cout); // get back ccb output
	  
	} // for ( uint64_t iPulse = 1; iPulse <= events_per_thresh; ++iPulse )

      } // for ( uint64_t iHalfStrip = 0; iHalfStrip < hstrips_per_run; ++iHalfStrip )

    } // for ( vector<emu::pc::DAQMB*>::iterator dmb = dmbs.begin(); dmb != dmbs.end(); ++dmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

}

void emu::step::Test::_25(){
  // TODO: update trigger counting procedure (see CVS)
  if ( pLogger_ ){ LOG4CPLUS_INFO( *pLogger_, "emu::step::Test::_25 starting" ); }

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

  struct timeval start, end;

  usleep(100);

  for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate ){

      (*crate)->ccb()->WriteRegister( 0x96, 0 ); // enable L1A counter // NEEDED???
      ::usleep(100); // NEEDED???

      vector<emu::pc::TMB*> tmbs = (*crate)->tmbs();
      for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb ){

	emu::pc::ALCTController* alct = (*tmb)->alctController();
	alct->configure();
	::usleep(100);

	stringstream timeStampFileName;
	ofstream timeStampFile;
	timeStampFile.open( timeStampFileName.str().c_str() );

	for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting ){

	  if ( iTriggerSetting == 0 ){
	    alct->SetPretrigNumberOfLayers( 1 );
	  }
	  else{
	    alct->SetPretrigNumberOfLayers( 2 );
	  }
	  alct->SetPretrigNumberOfPattern( iTriggerSetting + 1 );
	  alct->WriteConfigurationReg();

	  (*crate)->ccb()->WriteRegister( 0x96, 0 ); // enable L1A counter
	  (*crate)->ccb()->WriteRegister( 0x94, 0 ); // zero L1A counter

	  (*crate)->ccb()->WriteRegister( 0x20, 0x1edd ); // CSRB1=0x20, 0x1edd->enable CCB to send L1A on TMB request
	  gettimeofday( &start, NULL );
	  log4cplus::helpers::sleepmillis( iTriggerSetting * 250 );
	  (*crate)->ccb()->WriteRegister( 0x20, 0x1af9 ); // CSRB1=0x20, 0x1af9->disable CCB to send L1A on TMB request
	  gettimeofday( &end, NULL );

	  timeStampFile << "msecs " << end.tv_sec*1000 - start.tv_sec*1000 + end.tv_usec/1000. - start.tv_usec/1000. <<endl;

	  bsem_.take();
	  iEvent_++;
	  bsem_.give();

	  uint32_t l1a_counter_low_bits  = (*crate)->ccb()->ReadRegister( 0x90 ) & 0xffff; // read lower 16 bits
	  uint32_t l1a_counter_high_bits = (*crate)->ccb()->ReadRegister( 0x92 ) & 0xffff; // read higher 16 bits
	  uint32_t l1a_counter           = l1a_counter_low_bits | (l1a_counter_high_bits << 16); // merge into counter
	  if ( pLogger_ ){
	    stringstream ss;
	    ss << "Crate "  << (*crate)->GetLabel() << " "<< crate-crates.begin()+1 << "/" << crates.size()
	       << ", TMB " << tmb-tmbs.begin()+1 << "/" << tmbs.size() << " in slot " << (*tmb)->slot()
	       << ", trigger setting " << iTriggerSetting+1 << "/" << trig_settings
	       << ", L1A " << l1a_counter;
	    LOG4CPLUS_INFO( *pLogger_, ss.str() );
	  }
	  
	} // for ( uint64_t iTriggerSetting = 0; iTriggerSetting < trig_settings; ++iTriggerSetting )

      } // for ( vector<emu::pc::TMB*>::iterator tmb = tmbs.begin(); tmb != tmbs.end(); ++tmb )

  } // for ( vector<emu::pc::Crate*>::iterator crate = crates.begin(); crate != crates.end(); ++crate )

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
