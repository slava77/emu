//-----------------------------------------------------------------------
// $Id: ChamberUtilities.cc,v 1.20 2009/04/16 12:33:07 rakness Exp $
// $Log: ChamberUtilities.cc,v $
// Revision 1.20  2009/04/16 12:33:07  rakness
// make alct rx-tx scan faster, display statistics, flag problems, include maximization of total number of delay values around the good value
//
// Revision 1.18  2009/04/04 10:44:51  rakness
// Update for TMB firmware 2009 March 16
//
// Revision 1.17  2009/03/25 11:37:22  liu
// move header files to include/emu/pc
//
// Revision 1.16  2009/03/24 17:41:13  rakness
// Sufficient set of ALCT-TMB Loopback tests to determine alct_rx/tx_phase_delay
//
// Revision 1.15  2009/03/06 16:45:29  rakness
// add methods for ALCT-TMB loopback
//
// Revision 1.14  2008/11/24 17:50:39  rakness
// update for TMB version 18 Nov 2008
//
// Revision 1.13  2008/08/25 09:36:16  rakness
// specify precisely position of L1A in ALCT receipt window
//
// Revision 1.12  2008/08/13 11:30:52  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 1.11  2008/07/16 17:28:35  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 1.10  2008/06/29 17:42:25  rakness
// add radioactive trigger mode for scans
//
// Revision 1.9  2008/05/23 13:00:23  rakness
// set extra latency values in order to properly scan tmb_lct_cable_delay
//
// Revision 1.8  2008/05/14 13:46:58  rakness
// change default ranges to better match current latency
//
// Revision 1.7  2008/04/28 15:42:24  rakness
// bind L1A and DAV buttons to methods
//
// Revision 1.6  2008/03/27 14:58:12  rakness
// streamline L1A and DAV measurements; modify output for configuration check; add comments to TMB firmware downloading
//
// Revision 1.3  2008/03/12 11:43:33  rakness
// measured sync parameters tagged by chamber; remove ALCT firmware downloading until it can be made robust
//
// Revision 1.2  2008/01/07 15:08:44  rakness
// add xml parameters:  clct_stagger, clct_blanking, clct_pattern_id_thresh, aff_thresh, min_clct_separation.  Remove xml parameter:  clct_distrip_pretrig_thresh
//
// Revision 1.1  2007/12/26 11:23:49  liu
// new parser and applications
//
// Revision 3.40  2007/12/06 15:12:45  rakness
// make scan parameters for synchronization configurable from hyperDAQ
//
// Revision 3.39  2007/09/26 08:42:48  rakness
// open scan windows to find L1A from different sources, slightly re-order ::automatic() to make DAV cable delay scan more robust by finding L1A windows first
//
// Revision 3.38  2007/09/11 11:34:30  rakness
// Find Distrip Hot Channels
//
// Revision 3.37  2007/08/24 16:06:55  rakness
// make synchronization with pulsing more robust/combine ALCT+TMB L1A delay determination into one method
//
// Revision 3.36  2007/08/21 16:01:37  rakness
// comment out FindALCTvpf()--FindAlctInClctMatchWindow() should be used instead
//
// Revision 3.35  2007/08/17 09:39:17  rakness
// flag (obviously) bad results in ALCT-CLCT matching measurement
//
// Revision 3.34  2007/08/15 12:40:57  rakness
// determine sync parameters w/1 button, clean up output, control level of std::cout with debug_
//
// Revision 3.33  2007/08/13 14:17:22  rakness
// allow synchronization of full trigger/DAQ paths with TTC pulsing
//
// Revision 3.32  2007/08/09 09:24:24  rakness
// ALCT-CLCT coincidence pulsing using teststrip in ChamberUtilities
//
// Revision 3.31  2007/08/08 13:23:02  rakness
// sleep after setting mpc_output_enable for reading DMB delays and scopes
//
// Revision 3.30  2007/08/06 14:17:47  rakness
// fix bug of hitting edges of windows when computing mean
//
// Revision 3.29  2007/08/03 14:38:06  rakness
// use new TMB::WriteRegister(int)
//
// Revision 3.28  2007/08/01 11:31:15  rakness
// fix bug in window/histo sizes in ALCT in CLCT match window
//
// Revision 3.27  2007/07/26 13:09:35  rakness
// update CFEB rx scan for CLCT key layer 3 -> 2 change
//
// Revision 3.25  2007/07/17 16:25:15  liu
// remove CCBStartTrigger()
//
// Revision 3.24  2007/07/05 07:29:09  rakness
// add debug_ flag, make testing more robust, comment out old l1a timing routines
//
// Revision 3.23  2007/06/26 14:40:10  rakness
// mpc_tx_delay compensation fixed
//
// Revision 3.22  2007/06/22 10:27:49  rakness
// Include (correlated) mpc_tx_delay determination based on match_trig_alct_delay
//
// Revision 3.21  2007/06/21 16:14:02  rakness
// online measurement of ALCT in CLCT matching window
//
// Revision 3.20  2007/06/14 15:33:17  rakness
// scan only 2 RPCs for RAT/TMB phase
//
// Revision 3.19  2007/06/14 08:47:55  rakness
// make winner scan more robust
//
// Revision 3.18  2007/06/12 09:56:27  rakness
// remove TMB distrip from CLCT phase scan
//
// Revision 3.17  2007/06/07 12:57:55  rakness
// make scans more robust
//
// Revision 3.16  2007/06/01 14:57:49  gujh
// Add "include <cmath>" and change the 'abs' to 'fabs' on line 2488
//
// Revision 3.15  2007/05/30 16:48:27  rakness
// DMB cable_delay scans/tools
//
// Revision 3.14  2007/05/16 13:48:38  rakness
// mpc_output_enable=1 in AFF to L1A measurement
//
// Revision 3.13  2007/05/14 17:37:15  rakness
// AFF to L1A timing button
//
// Revision 3.12  2007/03/22 18:34:45  rakness
// weighted average in ALCT-CLCT match, and TMB/ALCT-L1A scans
//
// Revision 3.11  2007/03/14 14:24:50  rakness
// write sync values to summary file
//
// Revision 3.10  2006/10/19 13:01:07  rakness
// RPC-RAT delay scan in hyperDAQ
//
// Revision 3.9  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.8  2006/10/12 17:52:13  mey
// Update
//
// Revision 3.7  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.6  2006/10/02 13:45:31  mey
// Update
//
// Revision 3.5  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.4  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.3  2006/07/23 15:42:51  rakness
// index Hot Channel Mask from 0
//
// Revision 3.2  2006/07/23 14:04:06  rakness
// encapsulate RAT, update configure()
//
// Revision 3.1  2006/07/22 16:13:02  rakness
// clean up RAT/add JTAG checking
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.39  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 1.38  2006/07/19 18:11:17  rakness
// Error checking on ALCTNEW
//
// Revision 1.37  2006/07/18 07:52:30  rakness
// ALCTTiming with ALCTNEW
//
// Revision 1.36  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 1.35  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 1.34  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 1.33  2006/06/20 11:35:11  rakness
// output of cfebchamberscan
//
// Revision 1.32  2006/06/20 10:03:11  mey
// Reduced dac setting
//
// Revision 1.31  2006/06/01 16:05:00  rakness
// update
//
// Revision 1.30  2006/05/31 13:10:53  mey
// Update
//
// Revision 1.29  2006/05/29 15:25:36  rakness
// log timing scan output in summary
//
// Revision 1.28  2006/05/18 08:35:44  mey
// Update
//
// Revision 1.27  2006/05/17 14:16:44  mey
// Update
//
// Revision 1.26  2006/05/12 08:03:06  mey
// Update
//
// Revision 1.25  2006/05/11 09:37:31  mey
// Update
//
// Revision 1.24  2006/05/10 10:24:32  mey
// Update
//
// Revision 1.23  2006/05/10 10:07:56  mey
// Update
//
// Revision 1.22  2006/05/10 09:54:20  mey
// Update
//
// Revision 1.21  2006/05/09 14:56:29  mey
// Update
//
// Revision 1.20  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 1.19  2006/04/21 11:50:49  mey
// include/EmuPeripheralCrate.h
//
// Revision 1.18  2006/04/05 22:46:24  mey
// Bug fix
//
// Revision 1.17  2006/03/30 13:55:38  mey
// Update
//
// Revision 1.16  2006/03/28 10:44:21  mey
// Update
//
// Revision 1.15  2006/03/24 16:40:36  mey
// Update
//
// Revision 1.14  2006/03/24 14:35:04  mey
// Update
//
// Revision 1.13  2006/03/23 08:24:58  mey
// Update
//
// Revision 1.12  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 1.11  2006/02/21 12:44:00  mey
// fixed bug
//
// Revision 1.10  2006/02/20 13:31:14  mey
// Update
//
// Revision 1.9  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 1.8  2006/02/15 10:49:13  mey
// Fixed pulsing for ME1
//
// Revision 1.7  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 1.6  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 1.5  2006/01/18 12:46:48  mey
// Update
//
// Revision 1.4  2006/01/16 20:29:06  mey
// Update
//
// Revision 1.3  2006/01/12 22:36:27  mey
// UPdate
//
// Revision 1.2  2006/01/12 11:32:43  mey
// Update
//
// Revision 1.1  2006/01/11 08:54:15  mey
// Update
//
// Revision 1.5  2005/12/16 17:49:39  mey
// Update
//
// Revision 1.4  2005/12/14 08:32:36  mey
// Update
//
// Revision 1.3  2005/12/06 13:30:10  mey
// Update
//
// Revision 1.2  2005/11/30 14:58:02  mey
// Update tests
//
// Revision 1.1  2005/10/28 13:09:04  mey
// Timing class
//
//
#include <stdio.h>
#include <iomanip>
#include <cmath>
#include <unistd.h> 
#include <string>
//
#include "emu/pc/ChamberUtilities.h"
#include "emu/pc/TMB_constants.h"
//
namespace emu {
  namespace pc {

ChamberUtilities::ChamberUtilities(){
  //
  debug_ = 1;      // debug >=  1 = normal output to std::cout
  //                  debug >=  5 = analysis debug output
  //                  debug >= 10 = hardware (registers) debug output
  //
  beginning = 0;
  thisTMB   = 0;
  thisDMB   = 0;
  //
  UsePulsing_ = false;
  //
  Npulses_ = 2;
  comparing_with_clct_ = false;
  //
  use_measured_values_ = false;
  //
  pause_between_data_reads_ = 10000; // default number of microseconds to wait between data reads
  number_of_data_reads_     = 100;    // default number of data reads
  //
  pause_at_each_setting_    = 1;     // default number of seconds to wait at each delay value
  min_alct_l1a_delay_value_ = 134;
  max_alct_l1a_delay_value_ = 148;
  min_tmb_l1a_delay_value_  = 115; 
  max_tmb_l1a_delay_value_  = 129; 
  //
  MyOutput_ = &std::cout ;
  //
  // parameters to determine
  for( int i=0; i<5; i++) 
    CFEBrxPhase_[i] = -1;
  ALCTtxPhase_       = -1;
  ALCTrxPhase_       = -1;
  RatTmbDelay_       = -1;
  for(int i=0; i<2;i++) 
    RpcRatDelay_[i] = -1;
  ALCTvpf_           = -1;
  ALCTvpf            = -1;
  measured_match_trig_alct_delay_ = -1;
  measured_mpc_tx_delay_          = -1;
  MPCdelay_          = -1;
  AlctDavCableDelay_ = -1;
  TmbLctCableDelay_  = -1;
  CfebDavCableDelay_ = -1;
  TMBL1aTiming_      = -1;
  ALCTL1aDelay_      = -1;
  //
  best_average_aff_to_l1a_counter_ = -1.;
  best_average_alct_dav_scope_ = -1.;
  best_average_cfeb_dav_scope_ = -1.;
  //
  // measured values
  AffToL1aAverageValue_ = -1;
  CfebDavAverageValue_  = -1;
  TmbDavAverageValue_   = -1;
  AlctDavAverageValue_  = -1;
  AffToL1aScopeAverageValue_ = -1;
  CfebDavScopeAverageValue_  = -1;
  TmbDavScopeAverageValue_   = -1;
  AlctDavScopeAverageValue_  = -1;
  //
  for (int i=0;i<5;i++) 
    for (int j=0; j<32; j++) {
      CFEBStripScan_[i][j] = -1;
    }
  for (int i=0;i<112;i++) ALCTWireScan_[i] = -1;
  //
  // ranges over which to analyze DMB histograms
  ScopeMin_        = 0;
  ScopeMax_        = 4;
  AffToL1aValueMin_= 110;
  AffToL1aValueMax_= 140;
  CfebDavValueMin_ = 0;
  CfebDavValueMax_ = 10;
  TmbDavValueMin_  = 0;
  TmbDavValueMax_  = 10;
  AlctDavValueMin_ = 20;
  AlctDavValueMax_ = 30;
  //
}
//
//
ChamberUtilities::~ChamberUtilities(){
  //
  //
}
/////////////////////////////////////////////////////////////////////////////////
// peripheral crate parameter measurements
/////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------
// clock phases
//----------------------------------------------
void ChamberUtilities::CFEBTiming(){
  //
  if (debug_) {
    std::cout << "***************************" << std::endl;
    std::cout << "    Find cfeb[0-4]delay:" << std::endl;
    std::cout << "***************************" << std::endl;
  }
  (*MyOutput_) << "***************************" << std::endl;
  (*MyOutput_) << "    Find cfeb[0-4]delay:" << std::endl;
  (*MyOutput_) << "***************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Set up for this test...
  // Get initial values:
  int initial_clct_pretrig_enable           = thisTMB->GetClctPatternTrigEnable();    //0x68
  int initial_clct_trig_enable              = thisTMB->GetTmbAllowClct();             //0x86
  int initial_clct_halfstrip_pretrig_thresh = thisTMB->GetHsPretrigThresh();          //0x70
  int initial_clct_pattern_thresh           = thisTMB->GetMinHitsPattern();           //0x70
  int initial_layer_trig_enable             = thisTMB->GetEnableLayerTrigger();       //0xf0
  int initial_ignore_ccb_startstop          = thisTMB->GetIgnoreCcbStartStop();       //0x2c
  int initial_cfeb0_phase = thisTMB->GetCFEB0delay();                                 //0x18
  int initial_cfeb1_phase = thisTMB->GetCFEB1delay();                                 //0x1a
  int initial_cfeb2_phase = thisTMB->GetCFEB2delay();                                 //0x1a
  int initial_cfeb3_phase = thisTMB->GetCFEB3delay();                                 //0x1a
  int initial_cfeb4_phase = thisTMB->GetCFEB4delay();                                 //0x1a
  //
  // Enable this TMB for this test
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(6);
  thisTMB->SetMinHitsPattern(6);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(0);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(0);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  comparing_with_clct_ = true;
  thisTMB->StartTTC();
  ::sleep(1);
  //
  //
  int MaxTimeDelay=13;
  //
  int Muons[5][MaxTimeDelay];
  int MuonsWork[5][2*MaxTimeDelay];
  //
  for(int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      Muons[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for(int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      MuonsWork[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){
    //
    //    (*MyOutput_) << " Setting TimeDelay to " << TimeDelay << std::endl;
    //
    thisTMB->tmb_clk_delays(TimeDelay,0) ;
    thisTMB->tmb_clk_delays(TimeDelay,1) ;
    thisTMB->tmb_clk_delays(TimeDelay,2) ;
    thisTMB->tmb_clk_delays(TimeDelay,3) ;
    thisTMB->tmb_clk_delays(TimeDelay,4) ;
    //
    int CLCTInputList[5] = {0x1,0x2,0x4,0x8,0x10};
    //
    int last_pulsed_halfstrip[5] = {};
    int random_halfstrip[5] = {};
    //
    for (int Nmuons=0; Nmuons<2; Nmuons++){
      //
      for (int List=0; List<5; List++){
	//
	usleep(50);
	//
	// To prevent problems with data persisting in the TMB registers, 
	// generate a random halfstrip to pulse which is not the same as the 
	// last valid halfstrip for this CFEB. 
	// In addition, this should be away from the edges of the CFEB...
	while (random_halfstrip[List] == last_pulsed_halfstrip[List] ) {
	  random_halfstrip[List] = (int) (rand()/(RAND_MAX+0.01)*28);  // random number between 0 and 28
	  random_halfstrip[List] += 2;                                 // translate to between 2 and 30
	}
	//
	PulseCFEB(random_halfstrip[List],CLCTInputList[List]);	
	//
	int pulsed_halfstrip = List*32 + random_halfstrip[List];
	//
	usleep(50);
	//
	if (debug_) {
	  std::cout << "TimeDelay=" << TimeDelay << std::endl;
	  std::cout << "CLCTInput=" << List      
		    << ", halfstrip in CFEB =" << random_halfstrip[List]
		    << ", halfstrip in chamber =" << pulsed_halfstrip
		    << ", Nmuons=" << Nmuons 
		    << std::endl;
	}
	//
	int clct0patternId    = thisTMB->GetCLCT0PatternId();
	int clct0nhit         = thisTMB->GetCLCT0Nhit();
	int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	if (debug_) {
	  std::cout << "clct0patternId=" << clct0patternId
		    << ", clct0hstp=" << clct0keyHalfStrip      
		    << ", nHits =" << clct0nhit;
	}
	//
	//	int clct1patternId    = thisTMB->GetCLCT1PatternId();
	//	int clct1nhit         = thisTMB->GetCLCT1Nhit();
	//	int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	//
	if ( clct0patternId == 10 && clct0keyHalfStrip == pulsed_halfstrip && clct0nhit == 6 ) {
	  Muons[List][TimeDelay]++;
	  if (debug_) std::cout << " found" << std::endl;
	  last_pulsed_halfstrip[List] = random_halfstrip[List];
	} else {
	  if (debug_) std::cout << " NOT found" << std::endl;
	}
	//	if ( clct1nhit == 6 && clct1keyHalfStrip == 16 && clct1cfeb == List ) 
	//	  Muons[clct1cfeb][TimeDelay]++;
      }
    }
  }
  //
  float CFEBMeanN[5], CFEBMean[5];
  //
  for( int i=0; i<5; i++) {
    CFEBMean[i]  = 0 ;
    CFEBMeanN[i] = 0 ;
  }
  //
  // Print number of muons found versus cfeb[0-4]delay for each CFEB
  (*MyOutput_) << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) 
    (*MyOutput_) << std::setw(5) << TimeDelay ;
  (*MyOutput_) << std::endl ;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      (*MyOutput_) << std::setw(5) << Muons[CFEBs][TimeDelay] ;
    }     
    (*MyOutput_) << std::endl ;
  }   
  (*MyOutput_) << std::endl ;
  //
  int TimeDelay;
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      MuonsWork[CFEBs][TimeDelay] = Muons[CFEBs][TimeDelay] ;
    }
  }
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    TimeDelay=0;
    while (Muons[CFEBs][TimeDelay]>0) {
      MuonsWork[CFEBs][TimeDelay+13] = Muons[CFEBs][TimeDelay] ;
      MuonsWork[CFEBs][TimeDelay] = 0 ;
      TimeDelay++;
    }
  }
  //
  // Print number of muons found versus cfeb[0-4]delay into the next phase for each CFEB
  (*MyOutput_) << "TimeDelay Fixed for Delay Wrapping " << std::endl ;
  (*MyOutput_) << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) 
    (*MyOutput_) << std::setw(5) << TimeDelay ;
  (*MyOutput_) << std::endl;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
      if ( MuonsWork[CFEBs][TimeDelay] > 0  ) {
	CFEBMean[CFEBs]  += ((float) TimeDelay) * ((float) MuonsWork[CFEBs][TimeDelay])  ; 
	CFEBMeanN[CFEBs] += (float) MuonsWork[CFEBs][TimeDelay] ; 
      }
      (*MyOutput_) << std::setw(5) << MuonsWork[CFEBs][TimeDelay] ;
    }     
    (*MyOutput_) << std::endl ;
  }   
  (*MyOutput_) << std::endl ;
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) {
    if (CFEBMeanN[CFEBs] > 8) {
      CFEBMean[CFEBs] /= CFEBMeanN[CFEBs] ;
    } else {
      CFEBMean[CFEBs] = -999.;
    }
    if (CFEBMean[CFEBs] > 12 ) 
      CFEBMean[CFEBs] -= 13 ;
    (*MyOutput_) << " CFEB" << CFEBs << " mean delay = " << CFEBMean[CFEBs] ;
  }
  (*MyOutput_) << std::endl;
  //
  for( int i=0; i<5; i++) {
    CFEBrxPhase_[i] = RoundOff(CFEBMean[i]);
    (*MyOutput_) << "Best value is cfeb" << i << "delay = " << CFEBrxPhase_[i] << std::endl;
  }
  //
  // return to initial values:
  thisTMB->SetClctPatternTrigEnable(initial_clct_pretrig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(initial_clct_trig_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_halfstrip_pretrig_thresh);
  thisTMB->SetMinHitsPattern(initial_clct_pattern_thresh);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(initial_ignore_ccb_startstop);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(initial_layer_trig_enable);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting cfeb[0-4]delay phases to measured values..." << std::endl;
    thisTMB->SetCFEB0delay(CFEBrxPhase_[0]);
    thisTMB->SetCFEB1delay(CFEBrxPhase_[1]);
    thisTMB->SetCFEB2delay(CFEBrxPhase_[2]);
    thisTMB->SetCFEB3delay(CFEBrxPhase_[3]);
    thisTMB->SetCFEB4delay(CFEBrxPhase_[4]);
  } else {
    (*MyOutput_) << "Reverting back to original cfeb[0-4]delay phase values..." << std::endl;
    thisTMB->SetCFEB0delay(initial_cfeb0_phase);
    thisTMB->SetCFEB1delay(initial_cfeb1_phase);
    thisTMB->SetCFEB2delay(initial_cfeb2_phase);
    thisTMB->SetCFEB3delay(initial_cfeb3_phase);
    thisTMB->SetCFEB4delay(initial_cfeb4_phase);
  }
  thisTMB->WriteRegister(vme_ddd1_adr);
  thisTMB->WriteRegister(vme_ddd2_adr);
  ::sleep(1);
  //
  comparing_with_clct_ = false;
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return;
}
//
int ChamberUtilities::Find_alct_tx_with_ALCT_to_TMB_evenodd() {
  //
  // default 1000 passes for each tx, wire-pair
  //
  return Find_alct_tx_with_ALCT_to_TMB_evenodd(1000);
}
//
int ChamberUtilities::Find_alct_tx_with_ALCT_to_TMB_evenodd(int number_of_passes) {
  //
  if (debug_) {
    std::cout << "*******************************************************" << std::endl;
    std::cout << "Find alct_tx_clock_delay with alternating 1's and 0's:" << std::endl;
    std::cout << "*******************************************************" << std::endl;
  }
  (*MyOutput_) << "*******************************************************" << std::endl;
  (*MyOutput_) << "Find alct_tx_clock_delay with alternating 1's and 0's:" << std::endl;
  (*MyOutput_) << "*******************************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_alct_tx_phase      = thisTMB->GetAlctTXclockDelay();
  //
  int initial_fire_l1a_oneshot  = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  int initial_sequencer_command = thisTMB->GetAlctSequencerCommand();
  int initial_demux_mode        = thisTMB->GetAlctDemuxMode();
  int initial_ALCT_read_address = thisTMB->GetAlctRawReadAddress();
  //
  int value = thisTMB->ReadRegister(alct_sync_ctrl_adr);
  if (debug_>=10) std::cout << "begin routine (0x" << std::hex << alct_sync_ctrl_adr << ") = 0x" << value << std::endl;
  //
  // Set up for this test:
  // turn off the CCB inputs...
  thisTMB->SetFireL1AOneshot(0);
  thisTMB->SetIgnoreCCBRx(1);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  // put the ALCT into "transmit Teven/Todd mode"
  thisTMB->SetAlctSequencerCommand(SEND_EVENODD);
  thisTMB->WriteRegister(alct_cfg_adr);
  //
  // Error accumulators
  int alct_tx_bad[13][28] = {};
  int alct_sync_1st_err_ff[13] = {};
  int alct_sync_2nd_err_ff[13] = {};
  //
  // expected patterns:
  const int alct_1st_expect = 0xAAAAAAA;	// Teven
  const int alct_2nd_expect = 0x5555555;	// Todd 
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int ddd_delay=0; ddd_delay<=12; ddd_delay++) {
      //
      // Step alct tx clock delay
      if (debug_>=10) std::cout << "Set alct_tx_clock_delay = " << ddd_delay << std::endl;
      thisTMB->tmb_clk_delays(ddd_delay,6);
      //
      if (debug_>=10) std::cout << "Clear errors in flipflops..." << std::endl;
      thisTMB->SetALCTSyncRxDataDelay(0);         //Set depth where to look for the data
      thisTMB->SetALCTSyncClearErrors(1);         // Clear TMB data check flipflops
      thisTMB->WriteRegister(alct_sync_ctrl_adr);
      //
      value = thisTMB->ReadRegister(alct_sync_ctrl_adr);
      if (debug_>=10) std::cout << "after clear errors (0x" << std::hex << alct_sync_ctrl_adr << ") = 0x" << value << std::endl;
      //
      // Unclear error flipflops, after this write, the errors are being tallied by TMB firmware
      if (debug_>=10) std::cout << "Unclear errors in flipflops..." << std::endl;
      thisTMB->SetALCTSyncClearErrors(0);
      thisTMB->WriteRegister(alct_sync_ctrl_adr);
      //
      value = thisTMB->ReadRegister(alct_sync_ctrl_adr);
      if (debug_>=10) std::cout << "after unclear errors (0x" << std::hex << alct_sync_ctrl_adr << ") = 0x" << value << std::endl;
      //
      // Read TMB received demux data
      int alct_demux_rd[8] = {};
      //
      // loop over 1st/2nd demux words
      for (int i=0; i<=7; i++) { 
	//
	if (debug_>=10) std::cout << "set read address..." << std::endl;
	// Tell TMB that it should be receiving DEMUX data (i.e., 1's and 0's) rather than RAW data (anything)
	thisTMB->SetAlctDemuxMode(DEMUX_DATA);
	thisTMB->SetAlctRawReadAddress(i);
	thisTMB->WriteRegister(alctfifo1_adr);
	//
	if (debug_>=10) std::cout << "read data..." << std::endl;
	thisTMB->ReadRegister(alctfifo2_adr);
	alct_demux_rd[i] = thisTMB->GetReadAlctDemuxData();
      }
      // assemble the readback data from the ALCT Raw Hits address
      int alct_sync_rxdata_1st = alct_demux_rd[0] | (alct_demux_rd[1] << 14);
      int alct_sync_rxdata_2nd = alct_demux_rd[2] | (alct_demux_rd[3] << 14);
      //
      // This is the data which the TMB firmware is comparing with and storing the non-matches in the flip-flop
      int alct_sync_expect_1st = alct_demux_rd[4] | (alct_demux_rd[5] << 14);
      int alct_sync_expect_2nd = alct_demux_rd[6] | (alct_demux_rd[7] << 14);
      //      
      // Read TMB data check flipflops
      value = thisTMB->ReadRegister(alct_sync_ctrl_adr);
      if (debug_>=10) std::cout << "after check flip flops alct_sync_ctrl_adr (" << std::hex << alct_sync_ctrl_adr << ") = 0x" << value << std::endl;
      //      
      int alct_sync_1st_err = thisTMB->GetReadALCTSync1stError();
      int alct_sync_2nd_err = thisTMB->GetReadALCTSync2ndError();
      //
      alct_sync_1st_err_ff[ddd_delay] |= thisTMB->GetReadALCTSync1stErrorLatched();
      alct_sync_2nd_err_ff[ddd_delay] |= thisTMB->GetReadALCTSync2ndErrorLatched();

      if (ipass==0) {
	printf("Teven/Todd: ddd_delay=%2i ",ddd_delay);
	printf("rxdata_1st=%8.8X rxdata_2nd=%8.8X ",alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	printf("1st_err/latched=%1i/%1i 2nd_err=%1i/%1i\n",alct_sync_1st_err,alct_sync_1st_err_ff[ddd_delay],alct_sync_2nd_err,alct_sync_2nd_err_ff[ddd_delay]);
      }
      //
      // Compare received bits to expected pattern
      if (alct_sync_expect_1st != alct_1st_expect) {
	(*MyOutput_) << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
	std::cout    << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
      }
      //
      if (alct_sync_expect_2nd != alct_2nd_expect) {
	(*MyOutput_) << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
	std::cout    << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
      }
      //
      // Compare them bit-by-bit
      for (int ibit=0; ibit<=27; ibit++) {
	int ibit_1st_expected = (alct_1st_expect	>> ibit) & 0x1;
	int ibit_2nd_expected = (alct_2nd_expect	>> ibit) & 0x1;
	int ibit_1st_received = (alct_sync_rxdata_1st	>> ibit) & 0x1;
	int ibit_2nd_received = (alct_sync_rxdata_2nd	>> ibit) & 0x1;
	if ((ibit_1st_expected !=  ibit_1st_received) || (ibit_2nd_expected !=  ibit_2nd_received)) alct_tx_bad[ddd_delay][ibit]++;
      }	
    }	
    if (debug_) 
      if (ipass%100==0) printf("\t%4i\r",number_of_passes-ipass);
  }
  //
  // sum up badness over all the cable pairs
  int nbad[13] = {};
  //
  for (int ddd_delay=0; ddd_delay<=12; ddd_delay++) 
    for (int ibit=0; ibit<=27; ++ibit) 
      nbad[ddd_delay] += alct_tx_bad[ddd_delay][ibit];
  //
  (*MyOutput_) << "--> Summed over all data pairs: " << std::endl;
  std::cout    << "--> Summed over all data pairs: " << std::endl;
  (*MyOutput_) << " alct_tx    bad data count" << std::endl;
  std::cout    << " alct_tx    bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  std::cout << "---------   --------------" << std::endl;
  for (int ddd_delay = 0; ddd_delay <=12; ddd_delay++) {
    (*MyOutput_) << "    " << std::hex << ddd_delay << "           " << std::dec << nbad[ddd_delay] <<std::endl;
    std::cout    << "    " << std::hex << ddd_delay << "           " << std::dec << nbad[ddd_delay] <<std::endl;
  }
  //
  ALCTtxPhase_ = window_analysis(nbad,13);
  //
  (*MyOutput_) << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl << std::endl;
  std::cout    << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl << std::endl;
  //
  //  
  // Display bad bits vs delay
  (*MyOutput_) << "------------------------------------------------" << std::endl;
  std::cout    << "------------------------------------------------" << std::endl;
  (*MyOutput_) << "    Cable Pair Errors vs alct_tx_clock_Delay"     << std::endl;
  std::cout    << "    Cable Pair Errors vs alct_tx_clock_Delay"     << std::endl;
  (*MyOutput_) << "------------------------------------------------" << std::endl;
  std::cout    << "------------------------------------------------" << std::endl;
  //
  (*MyOutput_) << "    delay";
  std::cout    << "    delay";
  for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
    (*MyOutput_) << std::setw(5) << ddd_delay;	
    std::cout    << std::setw(5) << ddd_delay;	
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  //
  (*MyOutput_) << "pair      ";
  std::cout    << "pair      ";
  for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
    (*MyOutput_) << " ----";
    std::cout    << " ----";
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  
  for (int ibit=0; ibit<=27; ++ibit) {
    (*MyOutput_) << std::setw(3) << ibit << "       ";
    std::cout    << std::setw(3) << ibit << "       ";
    for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
      (*MyOutput_) << std::setw(5) << alct_tx_bad[ddd_delay][ibit];
      std::cout    << std::setw(5) << alct_tx_bad[ddd_delay][ibit];
    }
    (*MyOutput_) << std::endl;
    std::cout    << std::endl;
  }
  //
  // return back to previous conditions
  thisTMB->SetFireL1AOneshot(initial_fire_l1a_oneshot);
  thisTMB->SetIgnoreCCBRx(initial_ignore_ccb_rx);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetAlctSequencerCommand(initial_sequencer_command);
  thisTMB->WriteRegister(alct_cfg_adr);
  //
  thisTMB->SetAlctDemuxMode(initial_demux_mode);
  thisTMB->SetAlctRawReadAddress(initial_ALCT_read_address);
  thisTMB->WriteRegister(alctfifo1_adr);
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_tx_clock_delays to measured values..." << std::endl;
    std::cout    << "Setting alct_tx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctTXclockDelay(ALCTtxPhase_);
    thisTMB->tmb_clk_delays(ALCTtxPhase_,6);    
  } else {
    (*MyOutput_) << "Reverting back to original alct_tx_clock_delay value..." << std::endl;
    std::cout    << "Reverting back to original alct_tx_clock_delay value..." << std::endl;
    thisTMB->SetAlctTXclockDelay(initial_alct_tx_phase);
    thisTMB->tmb_clk_delays(initial_alct_tx_phase,6);
  }
  //
  return ALCTtxPhase_;
}
//
int ChamberUtilities::Find_alct_rx_with_TMB_to_ALCT_evenodd() {
  //
  // default 1000 passes for each rx, wire-pair
  //
  return Find_alct_rx_with_TMB_to_ALCT_evenodd(1000);
}
//
int ChamberUtilities::Find_alct_rx_with_TMB_to_ALCT_evenodd(int number_of_passes) {
  //
  if (debug_) {
    std::cout << "*******************************************************" << std::endl;
    std::cout << "Find alct_rx_clock_delay with alternating 1's and 0's:" << std::endl;
    std::cout << "*******************************************************" << std::endl;
  }
  (*MyOutput_) << "*******************************************************" << std::endl;
  (*MyOutput_) << "Find alct_rx_clock_delay with alternating 1's and 0's:" << std::endl;
  (*MyOutput_) << "*******************************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_alct_rx_phase      = thisTMB->GetAlctRXclockDelay();
  //
  int initial_fire_l1a_oneshot  = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  int initial_sequencer_command = thisTMB->GetAlctSequencerCommand();
  int initial_demux_mode        = thisTMB->GetAlctDemuxMode();
  int initial_ALCT_read_address = thisTMB->GetAlctRawReadAddress();
  //
  // Set up for this test:
  // turn off the CCB inputs...
  thisTMB->SetFireL1AOneshot(0);
  thisTMB->SetIgnoreCCBRx(1);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  // Error accumulators
  int alct_rx_bad[13][28] = {};
  int alct_sync_1st_err_ff[13] = {};
  int alct_sync_2nd_err_ff[13] = {};
  //
  // expected patterns:
  const int alct_1st_expect = 0x2AA;	//1st-in-time: Teven = 10'b10 1010 1010
  const int alct_2nd_expect = 0x155;	//2nd-in-time: Todd  = 10'b01 0101 0101
  //
  // data to fill the TMB data banks:
  int data_to_loopback_1st[3] = {alct_1st_expect, 0, 0};   
  int data_to_loopback_2nd[3] = {alct_2nd_expect, 0, 0};   
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int ddd_delay=0; ddd_delay<=12; ddd_delay++) {
      //
      // Step alct rx clock delay
      thisTMB->tmb_clk_delays(ddd_delay,5);
      //
      // Fill data banks with the bits to loop back
      int register_address = 0;
      //
      for (int bank=0; bank<3; bank++) {
	if (bank == 0) register_address = LOOPBACK_BITS_IN_ALCT_BANK_0;
	if (bank == 1) register_address = LOOPBACK_BITS_IN_ALCT_BANK_1;
	if (bank == 2) register_address = LOOPBACK_BITS_IN_ALCT_BANK_2;
	//
	thisTMB->SetAlctSequencerCommand(register_address);
	thisTMB->WriteRegister(alct_cfg_adr);
	//
	// first 80MHz frame
	thisTMB->SetALCTSyncTxData1st(data_to_loopback_1st[bank]);
	thisTMB->WriteRegister(alct_sync_txdata_1st_adr);
	//
	// second 80MHz frame
	thisTMB->SetALCTSyncTxData2nd(data_to_loopback_2nd[bank]);
	thisTMB->WriteRegister(alct_sync_txdata_2nd_adr);
      }
      //
      //Set depth where to look for the data
      thisTMB->SetALCTSyncRxDataDelay(0);
      //
      // Clear TMB data check flipflops
      thisTMB->SetALCTSyncClearErrors(1);
      thisTMB->WriteRegister(alct_sync_ctrl_adr);
      //
      // Unclear error flipflops, after this write, the errors are being tallied by TMB firmware
      thisTMB->SetALCTSyncClearErrors(0);
      thisTMB->WriteRegister(alct_sync_ctrl_adr);
      //
      // Read TMB received demux data
      int alct_demux_rd[8] = {};
      //
      // Tell TMB that it should be receiving DEMUX data (i.e., 1's and 0's) rather than RAW data (anything)
      thisTMB->SetAlctDemuxMode(DEMUX_DATA);
      //
      // loop over 1st/2nd demux words
      for (int i=0; i<=7; i++) { 
	//
	thisTMB->SetAlctRawReadAddress(i);
	thisTMB->WriteRegister(alctfifo1_adr);
	//
	thisTMB->ReadRegister(alctfifo2_adr);
	alct_demux_rd[i] = thisTMB->GetReadAlctDemuxData();
      }
      // assemble the readback data from the ALCT Raw Hits address
      int alct_sync_rxdata_1st = alct_demux_rd[0] | (alct_demux_rd[1] << 14);
      int alct_sync_rxdata_2nd = alct_demux_rd[2] | (alct_demux_rd[3] << 14);
      //
      // This is the data which the TMB firmware is comparing with and storing the non-matches in the flip-flop
      int alct_sync_expect_1st = alct_demux_rd[4] | (alct_demux_rd[5] << 14);
      int alct_sync_expect_2nd = alct_demux_rd[6] | (alct_demux_rd[7] << 14);
      //      
      // Read TMB data check flipflops
      thisTMB->ReadRegister(alct_sync_ctrl_adr);
      //      
      int alct_sync_1st_err = thisTMB->GetReadALCTSync1stError();
      int alct_sync_2nd_err = thisTMB->GetReadALCTSync2ndError();
      //
      alct_sync_1st_err_ff[ddd_delay] |= thisTMB->GetReadALCTSync1stErrorLatched();
      alct_sync_2nd_err_ff[ddd_delay] |= thisTMB->GetReadALCTSync2ndErrorLatched();

      if (ipass==0 && debug_) {
	printf("Teven/Todd: ddd_delay=%2i ",ddd_delay);
	printf("rxdata_1st=%8.8X rxdata_2nd=%8.8X ",alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	printf("1st_err/latched=%1i/%1i 2nd_err=%1i/%1i\n",
	       alct_sync_1st_err,alct_sync_1st_err_ff[ddd_delay],
	       alct_sync_2nd_err,alct_sync_2nd_err_ff[ddd_delay]);
      }
      //
      // Compare received bits to expected pattern
      if (alct_sync_expect_1st != alct_1st_expect) {
	(*MyOutput_) << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
	std::cout    << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
      }
      //
      if (alct_sync_expect_2nd != alct_2nd_expect) {
	(*MyOutput_) << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
	std::cout    << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
      }
      //
      // Compare them bit-by-bit
      for (int ibit=0; ibit<=27; ibit++) {
	int ibit_1st_expected = (alct_1st_expect	>> ibit) & 0x1;
	int ibit_2nd_expected = (alct_2nd_expect	>> ibit) & 0x1;
	int ibit_1st_received = (alct_sync_rxdata_1st	>> ibit) & 0x1;
	int ibit_2nd_received = (alct_sync_rxdata_2nd	>> ibit) & 0x1;
	if ((ibit_1st_expected !=  ibit_1st_received) || (ibit_2nd_expected !=  ibit_2nd_received)) alct_rx_bad[ddd_delay][ibit]++;
      }	
    }	
    if (debug_) 
      if (ipass%100==0) printf("\t%4i\r",number_of_passes-ipass);
  }
  //
  // sum up badness over all the cable pairs
  int nbad[13] = {};
  //
  for (int ddd_delay=0; ddd_delay<=12; ddd_delay++) 
    for (int ibit=0; ibit<=27; ++ibit) 
      nbad[ddd_delay] += alct_rx_bad[ddd_delay][ibit];
  //
  (*MyOutput_) << "--> Summed over all data pairs: " << std::endl;
  std::cout    << "--> Summed over all data pairs: " << std::endl;
  (*MyOutput_) << " alct_rx    bad data count" << std::endl;
  std::cout    << " alct_rx    bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  std::cout    << "---------   --------------" << std::endl;
  for (int ddd_delay = 0; ddd_delay <=12; ddd_delay++) {
    (*MyOutput_) << "    " << std::hex << ddd_delay << "           " << std::dec << nbad[ddd_delay] <<std::endl;
    std::cout    << "    " << std::hex << ddd_delay << "           " << std::dec << nbad[ddd_delay] <<std::endl;
  }
  //
  ALCTrxPhase_ = window_analysis(nbad,13);
  //
  (*MyOutput_) << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl << std::endl;
  std::cout    << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl << std::endl;
  //
  //  
  // Display bad bits vs delay
  (*MyOutput_) << "------------------------------------------------" << std::endl;
  std::cout    << "------------------------------------------------" << std::endl;
  (*MyOutput_) << "    Cable Pair Errors vs alct_rx_clock_Delay"     << std::endl;
  std::cout    << "    Cable Pair Errors vs alct_rx_clock_Delay"     << std::endl;
  (*MyOutput_) << "------------------------------------------------" << std::endl;
  std::cout    << "------------------------------------------------" << std::endl;
  //
  (*MyOutput_) << "    delay";
  std::cout    << "    delay";
  for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
    (*MyOutput_) << std::setw(5) << ddd_delay;	
    std::cout    << std::setw(5) << ddd_delay;	
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  //
  (*MyOutput_) << "pair      ";
  std::cout    << "pair      ";
  for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
    (*MyOutput_) << " ----";
    std::cout    << " ----";
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  
  for (int ibit=0; ibit<=27; ++ibit) {
    (*MyOutput_) << std::setw(3) << ibit << "       ";
    std::cout    << std::setw(3) << ibit << "       ";
    for (int ddd_delay=0; ddd_delay<=12; ++ddd_delay) {
      (*MyOutput_) << std::setw(5) << alct_rx_bad[ddd_delay][ibit];
      std::cout    << std::setw(5) << alct_rx_bad[ddd_delay][ibit];
    } 
   (*MyOutput_) << std::endl;
   std::cout    << std::endl;
  }
  //
  // return back to previous conditions
  thisTMB->SetFireL1AOneshot(initial_fire_l1a_oneshot);
  thisTMB->SetIgnoreCCBRx(initial_ignore_ccb_rx);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetAlctSequencerCommand(initial_sequencer_command);
  thisTMB->WriteRegister(alct_cfg_adr);
  //
  thisTMB->SetAlctDemuxMode(initial_demux_mode);
  thisTMB->SetAlctRawReadAddress(initial_ALCT_read_address);
  thisTMB->WriteRegister(alctfifo1_adr);
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_rx_clock_delays to measured values..." << std::endl;
    std::cout    << "Setting alct_rx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctRXclockDelay(ALCTrxPhase_);
    thisTMB->tmb_clk_delays(ALCTrxPhase_,5);    
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx_clock_delay value..." << std::endl;
    std::cout    << "Reverting back to original alct_rx_clock_delay value..." << std::endl;
    thisTMB->SetAlctRXclockDelay(initial_alct_rx_phase);
    thisTMB->tmb_clk_delays(initial_alct_rx_phase,5);
  }
  //
  return ALCTrxPhase_;
}
//
int ChamberUtilities::TMB_to_ALCT_walking_ones() {
  //
  // default 1000 passes for each rx, wire-pair
  //
  return TMB_to_ALCT_walking_ones(1000);
}
//
int ChamberUtilities::TMB_to_ALCT_walking_ones(int number_of_passes) {
  //
  if (debug_) {
    std::cout << "**************************************************" << std::endl;
    std::cout << "Test all cable lines by looping back walking ones:" << std::endl;
    std::cout << "**************************************************" << std::endl;
  }
  (*MyOutput_) << "**************************************************" << std::endl;
  (*MyOutput_) << "Test all cable lines by looping back walking ones:" << std::endl;
  (*MyOutput_) << "**************************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_fire_l1a_oneshot  = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  int initial_sequencer_command = thisTMB->GetAlctSequencerCommand();
  int initial_demux_mode        = thisTMB->GetAlctDemuxMode();
  int initial_ALCT_read_address = thisTMB->GetAlctRawReadAddress();
  //
  // Set up for this test:
  // turn off the CCB inputs...
  thisTMB->SetFireL1AOneshot(0);
  thisTMB->SetIgnoreCCBRx(1);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  // Error accumulators
  int alct_walking1_err[28][2][28] = {};
  int alct_walking1_hit[28][2][28][2] = {};
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int ifirstsecond=0; ifirstsecond<2; ifirstsecond++) {
      for (int itransmit=0; itransmit<28; itransmit++) {
	//
	// expected patterns:
	int alct_1st_expect = (1 << itransmit) * (ifirstsecond==0);
	int alct_2nd_expect = (1 << itransmit) * (ifirstsecond==1);
	//
	// Fill data banks with the above data...
	//
	// bank = 0 holds bits 0-9; bank = 1 holds bit 10-19; bank = 2 holds bit 20-27
	int ibit  = itransmit % 10;
	int ibank = itransmit / 10;
	//
	// "1st_bank" holds the stuff transmitted/received in first 80MHz phase
	int alct_1st_bank[3] = {};
	alct_1st_bank[ibank] = (1 << ibit) * (ifirstsecond==0);
	//
	// "2nd_bank" holds the stuff transmitted/received in second 80MHz phase
	int alct_2nd_bank[3] = {};
	alct_2nd_bank[ibank] = (1 << ibit) * (ifirstsecond==1);
	//
	int register_address = 0;
	//
	for (int bank=0; bank<3; bank++) { 
	  //
	  if (bank == 0) register_address = LOOPBACK_BITS_IN_ALCT_BANK_0;
	  if (bank == 1) register_address = LOOPBACK_BITS_IN_ALCT_BANK_1;
	  if (bank == 2) register_address = LOOPBACK_BITS_IN_ALCT_BANK_2;
	  //
	  thisTMB->SetAlctSequencerCommand(register_address);
	  thisTMB->WriteRegister(alct_cfg_adr);
	  //
	  // first 80MHz frame
	  thisTMB->SetALCTSyncTxData1st(alct_1st_bank[bank]);
	  thisTMB->WriteRegister(alct_sync_txdata_1st_adr);
	  //
	  // second 80MHz frame
	  thisTMB->SetALCTSyncTxData2nd(alct_2nd_bank[bank]);
	  thisTMB->WriteRegister(alct_sync_txdata_2nd_adr);
	}
	//
	// Read TMB received demux data
	int alct_demux_rd[8] = {};
	//
	// Tell TMB that it should be receiving DEMUX data (i.e., 1's and 0's) rather than RAW data (anything)
	thisTMB->SetAlctDemuxMode(DEMUX_DATA);
	//
	// loop over 1st/2nd demux words
	for (int i=0; i<=7; i++) { 
	  //
	  thisTMB->SetAlctRawReadAddress(i);
	  thisTMB->WriteRegister(alctfifo1_adr);
	  //
	  thisTMB->ReadRegister(alctfifo2_adr);
	  alct_demux_rd[i] = thisTMB->GetReadAlctDemuxData();
	}
	// assemble the readback data from the ALCT Raw Hits address
	int alct_sync_rxdata_1st = alct_demux_rd[0] | (alct_demux_rd[1] << 14);
	int alct_sync_rxdata_2nd = alct_demux_rd[2] | (alct_demux_rd[3] << 14);
	//
	// This is the data which the TMB firmware is comparing with and storing the non-matches in the flip-flop
	int alct_sync_expect_1st = alct_demux_rd[4] | (alct_demux_rd[5] << 14);
	int alct_sync_expect_2nd = alct_demux_rd[6] | (alct_demux_rd[7] << 14);
	//      
	//	
	if (ipass==0 && debug_>5) {
	  char dash1[2]={'-','1'};
	  printf("\t%1i %2i tx",ifirstsecond,itransmit);
	  printf("\t1st "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(i==itransmit)*(ifirstsecond==0)]);
	  printf(" 2nd ");  for (int i=0; i<=27; ++i) printf("%1c",dash1[(i==itransmit)*(ifirstsecond==1)]);
	  printf("\n");
	  printf("\t     rx");
	  printf(" 1st "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_rxdata_1st >> i) & 0x1]);
	  printf(" 2nd "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_rxdata_2nd >> i) & 0x1]);
	  printf("\n");
	}
	//
	// Compare received bits to expected pattern
	if (alct_sync_expect_1st != alct_1st_expect) {
	  (*MyOutput_) << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		       << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
	  std::cout    << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		       << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
	}
	//
	if (alct_sync_expect_2nd != alct_2nd_expect) {
	  (*MyOutput_) << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		       << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
	  std::cout    << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		       << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
	}
	//
	// Compare them bit-by-bit
	for (int ibit=0; ibit<=27; ibit++) {
	  int ibit_1st_expected = (alct_1st_expect	>> ibit) & 0x1;
	  int ibit_2nd_expected = (alct_2nd_expect	>> ibit) & 0x1;
	  int ibit_1st_received = (alct_sync_rxdata_1st	>> ibit) & 0x1;
	  int ibit_2nd_received = (alct_sync_rxdata_2nd	>> ibit) & 0x1;
	  //
	  if ((ibit_1st_expected !=  ibit_1st_received) || (ibit_2nd_expected !=  ibit_2nd_received)) alct_walking1_err[itransmit][ifirstsecond][ibit]++;
	  //
	  alct_walking1_hit[itransmit][ifirstsecond][ibit][0]=alct_walking1_hit[itransmit][ifirstsecond][ibit][0]+ibit_1st_received;
	  alct_walking1_hit[itransmit][ifirstsecond][ibit][1]=alct_walking1_hit[itransmit][ifirstsecond][ibit][1]+ibit_2nd_received;
	}	
      }	
      if (debug_) 
	if (ipass%100==0) printf("\t%4i\r",number_of_passes-ipass);
    }
  }
  //
  int total_bad_bits = 0;
  //
  (*MyOutput_) << "TMB-ALCT Walking 1 Loopback " << std::dec << number_of_passes << " times: Any bit hit displays a 1" << std::endl; 
  std::cout    << "TMB-ALCT Walking 1 Loopback " << std::dec << number_of_passes << " times: Any bit hit displays a 1" << std::endl; 
  (*MyOutput_) << "                            1         2         3         4         5" << std::endl; 
  std::cout    << "                            1         2         3         4         5" << std::endl; 
  (*MyOutput_) << "  1st|2nd        TxBit      Rx=01234567890123456789012345678901234567890123456789012345" << std::endl;
  std::cout    << "  1st|2nd        TxBit      Rx=01234567890123456789012345678901234567890123456789012345" << std::endl;
  //
  for (int ifirstsecond=0; ifirstsecond<=1;  ++ifirstsecond) {	// loop over 1st-in-time, 2nd-in-time
    for (int itransmit=0; itransmit<=27; ++itransmit) {	// loop over 28 bits
      //
      (*MyOutput_)<< "80MHz phase=" << std::setw(2) << std::dec << ifirstsecond <<", cable pair= " << std::setw(2) << itransmit << " ";
      std::cout   << "80MHz phase=" << std::setw(2) << std::dec << ifirstsecond <<", cable pair= " << std::setw(2) << itransmit << " ";
      //
      for (int ibit=0; ibit<=27; ++ibit) {
	if ( alct_walking1_hit[itransmit][ifirstsecond][ibit][0]!=0 ) {
	  (*MyOutput_) << "1";
	  std::cout    << "1";
	} else {
	  (*MyOutput_) << "-";
	  std::cout    << "-";
	}
      }
      for (int ibit=0; ibit<=27; ++ibit) {
	if ( alct_walking1_hit[itransmit][ifirstsecond][ibit][1]!=0 ) {
	  (*MyOutput_) << "1";
	  std::cout    << "1";
	} else {
	  (*MyOutput_) << "-";
	  std::cout    << "-";
	}
      }
      //
      int nbad=0;
      for (int ibit=0; ibit<=27; ++ibit) {nbad=nbad+alct_walking1_err[itransmit][ifirstsecond][ibit];}
      if ( nbad!=0 ) {
	(*MyOutput_) << " Fail" << std::endl;
	std::cout    << " Fail" << std::endl;
      } else {
	(*MyOutput_) << " Pass" << std::endl;
	std::cout    << " Pass" << std::endl;
      }
      total_bad_bits += nbad;
    }
  }
  //
  (*MyOutput_) << "Total number of bad bits = " << total_bad_bits << std::endl;
  std::cout    << "Total number of bad bits = " << total_bad_bits << std::endl;
  //
  // return back to previous conditions
  thisTMB->SetFireL1AOneshot(initial_fire_l1a_oneshot);
  thisTMB->SetIgnoreCCBRx(initial_ignore_ccb_rx);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetAlctSequencerCommand(initial_sequencer_command);
  thisTMB->WriteRegister(alct_cfg_adr);
  //
  thisTMB->SetAlctDemuxMode(initial_demux_mode);
  thisTMB->SetAlctRawReadAddress(initial_ALCT_read_address);
  thisTMB->WriteRegister(alctfifo1_adr);
  //
  return total_bad_bits;
}
//
int ChamberUtilities::ALCT_TMB_TimingUsingRandomLoopback() {
  //
  if (debug_) {
    std::cout << "***************************************************" << std::endl;
    std::cout << "Find alct_[rx,tx]_phase_delay using random loopback" << std::endl;
    std::cout << "***************************************************" << std::endl;
  }
  (*MyOutput_) << "***************************************************" << std::endl;
  (*MyOutput_) << "Find alct_[rx,tx]_phase_delay using random loopback" << std::endl;
  (*MyOutput_) << "***************************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_alct_tx_phase      = thisTMB->GetAlctTXclockDelay();
  int initial_alct_rx_phase      = thisTMB->GetAlctRXclockDelay();
  //
  int initial_fire_l1a_oneshot  = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  //
  int initial_sequencer_command   = thisTMB->GetAlctSequencerCommand();
  //
  int initial_alct_sync_rx_data_delay = thisTMB->GetALCTSyncRxDataDelay();
  int initial_alct_sync_tx_random     = thisTMB->GetALCTSyncTXRandom();
  //
  int initial_demux_mode        = thisTMB->GetAlctDemuxMode();
  int initial_ALCT_read_address = thisTMB->GetAlctRawReadAddress();
  //
  // Set up for this test:
  // turn off the CCB inputs...
  thisTMB->SetFireL1AOneshot(0);
  thisTMB->SetIgnoreCCBRx(1);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetAlctSequencerCommand(LOOPBACK_RANDOM);
  thisTMB->WriteRegister(alct_cfg_adr);	
  //
  //
  // Not sure where in the FIFO the data looped back is going to show up...
  // Do a quick scan to look if there is ANY good data here...
  //
  int good_depth = -999;
  int n_pipe_depth = 0;
  //
  for (int pipe_depth=0; pipe_depth<16; pipe_depth++) {
    //
    std::cout << "Scanning at pipeline depth = " << pipe_depth;
    int good_data = 0;
    //
    for (int rx_value=0; rx_value<13; rx_value++) {
      thisTMB->tmb_clk_delays(rx_value,5);
      //
      for (int tx_value=0; tx_value<13; tx_value++) {
	thisTMB->tmb_clk_delays(tx_value,6);
	//
	//Set depth where to look for the data
	thisTMB->SetALCTSyncRxDataDelay(pipe_depth);
	//
	thisTMB->SetALCTSyncTXRandom(1);
	//
	// Clear TMB data check flipflops
	thisTMB->SetALCTSyncClearErrors(1);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// Unclear error flipflops, after this write, the errors are being tallied by TMB firmware
	thisTMB->SetALCTSyncClearErrors(0);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// Read TMB data check flipflops
	thisTMB->ReadRegister(alct_sync_ctrl_adr);
	//
	if ( thisTMB->GetReadALCTSync1stErrorLatched()==0 && thisTMB->GetReadALCTSync2ndErrorLatched()==0 ) good_data++; 
	//
      }
    }
    std::cout << "... Number of good spots = " << good_data << std::endl;
    //
    if (good_data > 0) {
      good_depth = pipe_depth;
      n_pipe_depth++;
    }
  }
  //
  if (n_pipe_depth > 1) {
    std::cout << "ERROR:  Two pipe depths with good data.  Need to set alct_posneg differently on this chamber" << std::endl;
    ALCTtxPhase_ = -900;
    ALCTrxPhase_ = -900;
    return -1;
  }
  //
  if (good_depth < 0) {
    std::cout << "ERROR:  No pipe depths with good data." << std::endl;
    ALCTtxPhase_ = -900;
    ALCTrxPhase_ = -900;
    return -1;
  }
  //
  //
  std::cout << "More thorough scan at pipeline depth = " << good_depth << std::endl;
  //
  // A display (statistics) array...
  int alct_tx_rx_display[13][13] = {};
  //
  // An analysis array...
  int alct_tx_rx_analyze[13][13] = {};
  //
  for (int rx_value=0; rx_value<13; rx_value++) {
    thisTMB->tmb_clk_delays(rx_value,5);
    //
    std::cout << "Scanning tx values at rx = " << rx_value << std::endl;
    //
    for (int tx_value=0; tx_value<13; tx_value++) {
      thisTMB->tmb_clk_delays(tx_value,6);
      //
      //Set depth where to look for the data
      thisTMB->SetALCTSyncRxDataDelay(good_depth);
      //
      thisTMB->SetALCTSyncTXRandom(1);
      //
      bool go_quick = false;
      //
      for (int check_loop=0; check_loop<1000; check_loop++) {
	//
	// Clear TMB data check flipflops
	thisTMB->SetALCTSyncClearErrors(1);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// Unclear error flipflops, after this write, the errors are being tallied by TMB firmware
	thisTMB->SetALCTSyncClearErrors(0);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	if (!go_quick) ::usleep(1000);
	//
	// Read TMB data check flipflops
	thisTMB->ReadRegister(alct_sync_ctrl_adr);
	//      
	alct_tx_rx_display[rx_value][tx_value] += (thisTMB->GetReadALCTSync1stErrorLatched() | thisTMB->GetReadALCTSync2ndErrorLatched());
	//
	// no sense wasting time on this rx,tx value.  It has badness...
	if (thisTMB->GetReadALCTSync1stErrorLatched() || thisTMB->GetReadALCTSync2ndErrorLatched()) go_quick = true;
	//
	// OK for this read:  read it again, slowly...
	if (!thisTMB->GetReadALCTSync1stErrorLatched() && !thisTMB->GetReadALCTSync2ndErrorLatched()) go_quick = false;
	//
      }
      //
      if ( alct_tx_rx_display[rx_value][tx_value] == 0 ) alct_tx_rx_analyze[rx_value][tx_value] = good_depth;
      //
      if (debug_>5) {   // Read TMB received demux data just to see what is going on...
	//
	int alct_demux_rd[8] = {};
	//
	// Tell TMB that it should be receiving DEMUX data (i.e., 1's and 0's) rather than RAW data (anything)
	thisTMB->SetAlctDemuxMode(DEMUX_DATA);
	//
	// loop over 1st/2nd demux words
	for (int i=0; i<8; i++) { 
	  //
	  thisTMB->SetAlctRawReadAddress(i);
	  thisTMB->WriteRegister(alctfifo1_adr);
	  //
	  thisTMB->ReadRegister(alctfifo2_adr);
	  alct_demux_rd[i] = thisTMB->GetReadAlctDemuxData();
	}
	// assemble the readback data from the ALCT Raw Hits address
	int alct_sync_rxdata_1st = alct_demux_rd[0] | (alct_demux_rd[1] << 14);
	int alct_sync_rxdata_2nd = alct_demux_rd[2] | (alct_demux_rd[3] << 14);
	//
	// This is the data which the TMB firmware is comparing with and storing the non-matches in the flip-flop
	int alct_sync_expect_1st = alct_demux_rd[4] | (alct_demux_rd[5] << 14);
	int alct_sync_expect_2nd = alct_demux_rd[6] | (alct_demux_rd[7] << 14);
	//      
	printf("Latch OK=%8i depth=%2i ddd_delay_tx=%2i ddd_delay_rx=%2i", alct_tx_rx_display[rx_value][tx_value],good_depth,tx_value,rx_value);
	printf("  read 1st=%8.8X 2nd=%8.8X ", alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	printf("expect 1st=%8.8X 2nd=%8.8X\n",alct_sync_expect_1st,alct_sync_expect_2nd);
      }
    }
  }
  //
  //
  // Display the statistics:
  (*MyOutput_) << "Result (tx vs. rx)   tx ----> " << std::endl;
  std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
  (*MyOutput_) << "         00   01   02   03   04   05   06   07   08   09   10   11   12" << std::endl;
  std::cout    << "         00   01   02   03   04   05   06   07   08   09   10   11   12" << std::endl;
  (*MyOutput_) << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
  std::cout    << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
  //
  for (int rx_value=0; rx_value<13; ++rx_value) {
    //
    (*MyOutput_) << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    //
    for (int tx_value=0; tx_value<13; ++tx_value) {
      //
      (*MyOutput_) << std::dec << std::setw(4) << alct_tx_rx_display[rx_value][tx_value] << " ";
      std::cout    << std::dec << std::setw(4) << alct_tx_rx_display[rx_value][tx_value] << " ";
      //
    }
    (*MyOutput_) << std::endl;    
    std::cout    << std::endl;    
  }
  //
  //  if (debug_>5) {  
    // Display the analyzed data
    //
    std::cout << "analyzed data..." << std::endl;
    //
    std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
    std::cout    << "         00   01   02   03   04   05   06   07   08   09   10   11   12" << std::endl;
    std::cout    << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
    //
    for (int rx_value=0; rx_value<13; ++rx_value) {
      std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
      //
      for (int tx_value=0; tx_value<13; ++tx_value) 
	std::cout    << std::dec << std::setw(4) << alct_tx_rx_analyze[rx_value][tx_value] << " ";
      //
      std::cout    << std::endl;    
    }
    //  }
  //
  ALCT_phase_analysis(alct_tx_rx_analyze);
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    std::cout    << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctTXclockDelay(ALCTtxPhase_);
    thisTMB->tmb_clk_delays(ALCTtxPhase_,6);
    //
    thisTMB->SetAlctRXclockDelay(ALCTrxPhase_);
    thisTMB->tmb_clk_delays(ALCTrxPhase_,5);
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    std::cout    << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    thisTMB->SetAlctTXclockDelay(initial_alct_tx_phase);
    thisTMB->tmb_clk_delays(initial_alct_tx_phase,6);
    //
    thisTMB->SetAlctRXclockDelay(initial_alct_rx_phase);
    thisTMB->tmb_clk_delays(initial_alct_rx_phase,5);
  }
  //
  // return back to previous conditions
  thisTMB->SetFireL1AOneshot(initial_fire_l1a_oneshot);
  thisTMB->SetIgnoreCCBRx(initial_ignore_ccb_rx);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetALCTSyncRxDataDelay(initial_alct_sync_rx_data_delay);
  thisTMB->SetALCTSyncTXRandom(initial_alct_sync_tx_random);
  thisTMB->WriteRegister(alct_sync_ctrl_adr);
  //
  thisTMB->SetAlctSequencerCommand(initial_sequencer_command);
  thisTMB->WriteRegister(alct_cfg_adr);
  //
  thisTMB->SetAlctDemuxMode(initial_demux_mode);
  thisTMB->SetAlctRawReadAddress(initial_ALCT_read_address);
  thisTMB->WriteRegister(alctfifo1_adr);
  //
  thisTMB->RedirectOutput(MyOutput_);
  alct->RedirectOutput(MyOutput_);
  //
  return ALCTtxPhase_;
}
//
void ChamberUtilities::ALCT_TMB_Loopback() {
  //
  bool initial_use_measured_values = use_measured_values_;
  PropagateMeasuredValues(true);
  //
  // Find a "good enough" value of the alct_tx_clock_delay with alternating 1's and 0's
  if (Find_alct_tx_with_ALCT_to_TMB_evenodd() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  ::sleep(2);
  //
  if (Find_alct_rx_with_TMB_to_ALCT_evenodd() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  ::sleep(2);
  //
  // Now check the cable to make sure all pins are transmitting data OK
  if (TMB_to_ALCT_walking_ones() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  ::sleep(2);
  //
  // Now measure alct_[rx,tx]_phase_delay with random data...
  if (ALCT_TMB_TimingUsingRandomLoopback() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  PropagateMeasuredValues(initial_use_measured_values);
  //
  return;
}
//
void ChamberUtilities::ALCTTiming(){
  //
  if (debug_) {
    std::cout << "******************************" << std::endl;
    std::cout << "Find alct_[rx,tx]_clock_delay:" << std::endl;
    std::cout << "******************************" << std::endl;
  }
  (*MyOutput_) << "******************************" << std::endl;
  (*MyOutput_) << "Find alct_[rx,tx]_clock_delay:" << std::endl;
  (*MyOutput_) << "******************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_alct_tx_phase        = thisTMB->GetALCTtxPhase();           //0x16
  int initial_alct_rx_phase        = thisTMB->GetALCTrxPhase();           //0x16
  int initial_alct_pat_trig_enable = thisTMB->GetAlctPatternTrigEnable(); //0x68
  int initial_enable_clct_inputs   = thisTMB->GetEnableCLCTInputs();      //0x42
  int initial_send_empty           = alct->GetSendEmpty();                //configuration register
  //
  // set up for this test
  alct->SetSendEmpty(1);
  alct->WriteConfigurationReg();
  if (debug_>=10) {
    alct->ReadConfigurationReg();
    alct->PrintConfigurationReg();
  }
  //
  thisTMB->SetAlctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetEnableCLCTInputs(0);
  thisTMB->WriteRegister(cfeb_inj_adr);
  //
  thisTMB->StartTTC();
  ::sleep(1);
  //
  const int maxTimeBins = 13;
  int selected[13][13];
  int selected2[13][13];
  int selected3[13][13];
  int ALCTWordCount[13][13];
  int rxtx_timing[13][13];
  int ALCTConfDone[13][13];
  //
  //
  for (int j=0;j<maxTimeBins;j++){
    for (int k=0;k<maxTimeBins;k++) {
      selected[j][k]  = 0;
      selected2[j][k] = 0;
      selected3[j][k] = 0;
      ALCTWordCount[k][j] = 0;
      ALCTConfDone[k][j] = 0;
    }
  }
  //
  for (int j=0;j<maxTimeBins;j++){
    for (int k=0;k<maxTimeBins;k++) {
      //
      int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetNumberOfChannelsInAlct())/6/4);
      int keyWG2 = (alct->GetNumberOfChannelsInAlct())/6-keyWG;
      //
      if (debug_) std::cout << "Injecting at wiregroup = " << std::dec << keyWG << std::endl;
      //
      for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct()/6); channel++) {
	  if (channel==keyWG) {
	    alct->SetHotChannelMask(layer,channel,ON);
	  } else {
	    alct->SetHotChannelMask(layer,channel,OFF);
	  }
	}
      }
      alct->WriteHotChannelMask();
      if (debug_>=10) {
	alct->ReadHotChannelMask();
	alct->PrintHotChannelMask();
      }
      //
      //
      //while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
      //
      if (debug_) std::cout << "Set alct_rx_clock_delay = " << std::dec << k << " alct_tx_clock_delay = " << j << std::endl;
      thisTMB->tmb_clk_delays(k,5) ;
      thisTMB->tmb_clk_delays(j,6) ;	 
      thisTMB->ResetALCTRAMAddress();
      PulseTestStrips();
      ::usleep(100);
      //
      thisTMB->DecodeALCT();
      if (debug_>=10) thisTMB->PrintALCT();	  
      //
      int alct0_quality   = thisTMB->GetAlct0Quality();
      int alct1_quality   = thisTMB->GetAlct1Quality();
      //      int alct0_bxn       = thisTMB->GetAlct0FirstBxn();
      //      int alct1_bxn       = thisTMB->GetAlct1SecondBxn();
      int alct0_key       = thisTMB->GetAlct0FirstKey();
      int alct1_key       = thisTMB->GetAlct1SecondKey();
      ALCTWordCount[k][j] = thisTMB->GetALCTWordCount();
      ALCTConfDone[k][j]  = thisTMB->ReadRegister(0x38) & 0x1;
      //
      if (debug_ >= 10) std::cout << "ALCT Wordcount: 0x" << std::hex << ALCTWordCount[k][j] <<std::endl;
      //
      if ( alct0_quality == 3 && alct0_key == keyWG ) selected[k][j]++;
      //
      if ( alct1_quality == 3 && alct1_key == keyWG ) selected[k][j]++;
      //
      if ( alct0_quality == 3 && alct0_key == keyWG2) selected2[k][j]++;
      //
      if ( alct1_quality == 3 && alct1_key == keyWG2) selected2[k][j]++;
      //
      if ( (alct0_quality == 3 && alct0_key == keyWG) &&
	   (alct1_quality == 3 && alct1_key == keyWG) ) 
	selected3[k][j]++;
      //
      if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
	   (alct1_quality == 3 && alct1_key == keyWG2) ) 
	selected3[k][j]++;
      //
      if ( (alct0_quality == 3 && alct0_key == keyWG) &&
	   (alct1_quality == 3 && alct1_key == keyWG2) ) 
	selected3[k][j]++;
      //
      if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
	   (alct1_quality == 3 && alct1_key == keyWG ) ) 
	selected3[k][j]++;
      //
    }
  }
  //
  if (debug_>= 5) {
    std::cout << "ALCT WordCount  (tx vs rx)   tx ---->" << std::endl;
    for (int j=0;j<maxTimeBins;j++) {
      std::cout << " rx =" << j << ": ";
      for (int k=0;k<maxTimeBins;k++) 
	std::cout << std::hex << std::setw(2) << (ALCTWordCount[j][k]&0xffff) << " ";
      std::cout << std::endl;
    }
    std::cout << std::endl;
    //
    std::cout << "ALCT Configuration Done (tx vs rx)   tx ----> " << std::endl;
    for (int j=0;j<maxTimeBins;j++) {
      std::cout  << " rx =" << j << ": ";
      for (int k=0;k<maxTimeBins;k++) 
	std::cout << std::hex << std::setw(2) << (ALCTConfDone[j][k]&0xffff) << " " ;
      std::cout << std::endl;
    }
    std::cout << std::endl;
    //
    std::cout << "Selected 1 (tx vs rx)   tx ----> " << std::endl;
    for (int j=0;j<maxTimeBins;j++) {
      std::cout << " rx =" << j << ": ";
      for (int k=0;k<maxTimeBins;k++) 
	std::cout << selected[j][k] << " " ;
      std::cout << std::endl;
    }
    std::cout << std::endl;
    //
    std::cout << "Selected 2 (tx vs rx)   tx ----> " << std::endl;
    for (int j=0;j<maxTimeBins;j++) {
      std::cout << " rx =" << j << ": ";
      for (int k=0;k<maxTimeBins;k++) 
	std::cout << selected2[j][k] << " " ;
      std::cout << std::endl;
    }
    std::cout << std::endl;
    //
    std::cout << "Selected 3 (tx vs. rx)   tx ----> " << std::endl;
    for (int j=0;j<maxTimeBins;j++) {
      std::cout << " rx =" << j << ": ";
      for (int k=0;k<maxTimeBins;k++) 
	std::cout << selected3[j][k] << " " ;
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  //
  (*MyOutput_) << "Result (tx vs. rx)   tx ----> " << std::endl;
  (*MyOutput_) << "         00   01   02   03   04   05   06   07   08   09   10   11   12" << std::endl;
  (*MyOutput_) << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
  for (int j=0;j<maxTimeBins;j++){
    (*MyOutput_) << " rx =" << std::dec << std::setw(2) << j << " " ; 
    for (int k=0;k<maxTimeBins;k++) {
      if ( ALCTConfDone[j][k] > 0 ) {
	(*MyOutput_) << std::hex << std::setw(4) << (ALCTWordCount[j][k]&0xffff) << " ";
	rxtx_timing[j][k] = ALCTWordCount[j][k];
      } else {
	(*MyOutput_) << std::hex << std::setw(4) << 0x00 << " ";
	rxtx_timing[j][k] = 0;
      }
    }
    (*MyOutput_) << std::endl;
  }
  //
  ALCT_phase_analysis(rxtx_timing);
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctTXclockDelay(ALCTtxPhase_);
    thisTMB->SetAlctRXclockDelay(ALCTrxPhase_);
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    thisTMB->SetAlctTXclockDelay(initial_alct_tx_phase);
    thisTMB->SetAlctRXclockDelay(initial_alct_rx_phase);
  }
  thisTMB->WriteRegister(vme_ddd0_adr);  
  //
  thisTMB->SetAlctPatternTrigEnable(initial_alct_pat_trig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetEnableCLCTInputs(initial_enable_clct_inputs);
  thisTMB->WriteRegister(cfeb_inj_adr);
  //
  alct->SetSendEmpty(initial_send_empty);
  alct->WriteConfigurationReg();
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return;
}
//
int ChamberUtilities::RatTmbDelayScan() {
  //
  if (debug_) {
    std::cout << "*******************" << std::endl;
    std::cout << "Find rat_tmb_delay:" << std::endl;
    std::cout << "*******************" << std::endl;
  }
  (*MyOutput_) << "*******************" << std::endl;
  (*MyOutput_) << "Find rat_tmb_delay:" << std::endl;
  (*MyOutput_) << "*******************" << std::endl;
  //
  //Initial delay values:
  thisRAT_->ReadRatTmbDelay();
  int rpc_delay_default = thisRAT_->GetRatTmbDelay();
  //
  // Set up for this test
  //Put RAT into sync mode for test [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                                [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                                 to improve syncing to rising edge of TMB clock
  //                                [2]=loop_tmb
  //                                [3]=free_tx0
  int initial_data = thisTMB->ReadRegister(vme_ratctrl_adr);
  int write_data = initial_data & 0xfffe | 0x0001;
  thisTMB->WriteRegister(vme_ratctrl_adr,write_data);
  //
  //enable RAT input into TMB...
  int read_data = thisTMB->ReadRegister(rpc_inj_adr);
  write_data = read_data | 0x0001;
  thisTMB->WriteRegister(rpc_inj_adr,write_data);
  //
  //these are the arrays of bits we expect from sync mode:
  const int nbits = 19;
  int rpc_rdata_expect[4][nbits];
  bit_to_array(0x2aaaa,rpc_rdata_expect[0],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[1],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[2],nbits);
  bit_to_array(0x2aaaa,rpc_rdata_expect[3],nbits);
  //
  if (debug_>=10) {
    for (int i=0; i<=3; i++) {
      std::cout << "rpc_rdata_expect[" << i << "] = ";
      for (int bit=0; bit<=(nbits-1); bit++) {
        std::cout << rpc_rdata_expect[i][bit] << " ";
      }
      std::cout << std::endl;
    }
  }
  //
  int rpc_bad[16] = {};
  int rpc_data_array[4][nbits];
  //
  for (int pass=0; pass<=100; pass++) { //collect statistics
    //
    if (debug_) {
      if ( (pass % 100) == 0 ) 
	std::cout << "Pass = " << std::dec << pass << std::endl;
    }
    //
    //step through ddd_delay
    for (int ddd_delay=0; ddd_delay<16; ddd_delay++) {
      //
      int count_bad=0;
      //
      thisRAT_->SetRatTmbDelay(ddd_delay);
      thisRAT_->WriteRatTmbDelay();
      //
      // ** read RAT 80MHz demux registers**
      for (int irat=0; irat<=1; irat++) {           //only 2 RPCs exist
	//
	read_data = thisTMB->ReadRegister(rpc_cfg_adr);
	read_data &= 0xf9ff;                        //zero out old RAT bank
	write_data = read_data | (irat << 9);       //select RAT RAM bank
	thisTMB->WriteRegister(rpc_cfg_adr,write_data);
	//
	read_data = thisTMB->ReadRegister(rpc_cfg_adr);
	int rpc_rbxn = (read_data >> 11) & 0x0007;  //RPC MSBS for sync mode
	//
	int rpc_rdata = thisTMB->ReadRegister(rpc_rdata_adr) & 0xffff; //RPC RAM read data for sync mode (LSBS)
	//
	int rpcData = rpc_rdata | (rpc_rbxn << 16);  //pack MS and LSBs into single integer
	//
	bit_to_array(rpcData,rpc_data_array[irat],nbits);
	//
	for (int i=0; i<=(nbits-1); i++) {
	  if (rpc_data_array[irat][i] != rpc_rdata_expect[irat][i]) count_bad += 1;
	}
      }
      if (debug_>10) {
	for (int i=0; i<=1; i++) {
	  (*MyOutput_) << "rpc_data_array[" << i << "] = ";
	  for (int bit=0; bit<=(nbits-1); bit++) {
	    (*MyOutput_) << rpc_data_array[i][bit] << " ";
	  }
	  (*MyOutput_) << std::endl;
	}
      }
      rpc_bad[ddd_delay] += count_bad;
    }
  }
  // ** print out results **
  (*MyOutput_) << "rpc_delay   bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  for (int rpc_delay = 0; rpc_delay <13; rpc_delay++) {
    (*MyOutput_) << "    " << std::hex << rpc_delay 
		 << "           " << std::hex << rpc_bad[rpc_delay] 
	      <<std::endl;
  }
  //
  RatTmbDelay_ = window_analysis(rpc_bad,13);
  //
  (*MyOutput_) << "Best value is rat_tmb_delay = " << std::dec << RatTmbDelay_ << std::endl;

  // Restore initial values:
  thisTMB->WriteRegister(vme_ratctrl_adr,initial_data);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting rat_tmb_delay to measured value..." << std::endl;
    thisRAT_->SetRatTmbDelay(RatTmbDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of rat_tmb_delay..." << std::endl;
    thisRAT_->SetRatTmbDelay(rpc_delay_default);
  }
  thisRAT_->WriteRatTmbDelay();
  //
  return RatTmbDelay_;
}
//
void ChamberUtilities::RpcRatDelayScan() {
  //
  RpcRatDelayScan(0);
  return;
}
//
void ChamberUtilities::RpcRatDelayScan(int rpc) {
  //** Find optimal rpc_rat_clock delay = phasing between RPC[rpc] and RAT **

  //Put RAT into correct mode [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                          [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                           to improve syncing to rising edge of TMB clock
  //                          [2]=loop_tmb
  //                          [3]=free_tx0
  //  int write_data = 0x0002;
  //  thisTMB->WriteRegister(vme_ratctrl_adr,write_data);
  //
  (*MyOutput_) << "Performing RPC" << rpc << "-RAT delay scan..." << std::endl;;
  //
  //  thisRAT_->ReadRatUser1();                           //read initial delay values
  //  int initial_delay = thisRAT_->GetRpcRatDelay(rpc);  //get values into local variable
  //
  int delay;
  //
  int parity_err_ctr[16] = {};
  //
  for (delay = 0; delay<=12; delay++) {                             //steps of 2ns
    //    (*MyOutput_) << "set delay = " << delay 
    //		 << " for RPC " << rpc
    //		 << std::endl;
    thisRAT_->SetRpcRatDelay(rpc,delay);
    thisRAT_->WriteRpcRatDelay();
    thisRAT_->PrintRpcRatDelay();
    //
    thisRAT_->reset_parity_error_counter();
    //
    ::sleep(1);                                                    //accumulate statistics
    //
    thisRAT_->ReadRatUser1();
    parity_err_ctr[delay] = thisRAT_->GetRatRpcParityErrorCounter(rpc);
    //
    //    (*MyOutput_) << "parity error for delay " << delay 
    //		     << " = " << parity_err_ctr[delay]
    //		 << std::endl;
    //
  }
  //
  //  (*MyOutput_) << "Putting inital delay value = " << initial_delay << " back..." << std::endl;
  //  thisRAT_->SetRpcRatDelay(rpc,initial_delay);
  //  thisRAT_->WriteRpcRatDelay();
  //
  // ** print out results **
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << "**** RPC" << rpc << "-RAT delay results ****" << std::endl;
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << " delay    parity counter errors" << std::endl;
  (*MyOutput_) << "-------   ---------------------" << std::endl;
  for (delay = 0; delay <=12; delay++) {
    (*MyOutput_) << "   " << std::hex << delay;
    (*MyOutput_) << "               " << std::hex << parity_err_ctr[delay] 
		 << std::endl;
  }
  //
  RpcRatDelay_[rpc] = window_analysis(parity_err_ctr,13);
  //
  return;
}
//
//----------------------------------------------
// Do all synchronization steps at once
//----------------------------------------------
void ChamberUtilities::FindL1ADelays(){
  //
  // Receive the L1A for the TMB and the ALCT:
  if (FindTmbAndAlctL1aDelay() < 0) return;
  //
  // Receive the L1A for the CFEB:
  if (MeasureTmbLctCableDelay() < 0) return;
  //
  std::cout << "Found TMB, ALCT, and CFEB L1A Receipt Delay Parameters" << std::endl;
  //
  return;
}
//
void ChamberUtilities::FindDAVDelays(){
  //
  // We need to be receiving the L1As correctly at the ALCT and CFEB in order to find the DAV delays
  //
  if (MeasureAlctDavCableDelay() < 0) return;
  //
  if (MeasureCfebDavCableDelay() < 0) return;
  //
  std::cout << "Found ALCT and CFEB Data AVailable Delay Parameters" << std::endl;
  //
  return;
}
//
void ChamberUtilities::FindL1AAndDAVDelays(){
  //
  std::cout << "Measure L1A return and DAV for TMB, ALCT, and CFEB." << std::endl;
  //
  bool initial_use_measured_values = use_measured_values_;
  PropagateMeasuredValues(true);
  //
  // Receive the L1A for the TMB and the ALCT:
  if (FindTmbAndAlctL1aDelay() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  // Since we are getting the L1A for the ALCT, we can determine its DAV timing:
  if (MeasureAlctDavCableDelay() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  // Now receive the L1A for the CFEB:
  if (MeasureTmbLctCableDelay() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  // Since we are getting the L1A for the CFEB, we can determine its DAV timing:
  if (MeasureCfebDavCableDelay() < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  std::cout << "Successfully found L1A for TMB, ALCT, and CFEB." << std::endl;
  std::cout << "Successfully found DAVs for ALCT and CFEB." << std::endl;
  //
  PropagateMeasuredValues(initial_use_measured_values);
  return;
}
//
void ChamberUtilities::Automatic(){
  //
  FindL1AAndDAVDelays();
  //
  return;
}
//
//
//----------------------------------------------
// ALCT-CLCT match timing
//----------------------------------------------
int ChamberUtilities::FindALCTinCLCTMatchWindow() {
  //
  if (debug_) {
    std::cout << "***************************" << std::endl;
    std::cout << "Find match_trig_alct_delay:" << std::endl;
    std::cout << "***************************" << std::endl;
  }
  (*MyOutput_) << "***************************" << std::endl;
  (*MyOutput_) << "Find match_trig_alct_delay:" << std::endl;
  (*MyOutput_) << "***************************" << std::endl;
  //
  const int HistoMin = 0;
  const int HistoMax =14;
  //
  const int test_alct_delay_value  = 7;
  const int test_match_window_size = 15;
  //
  // Set up for this test...
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Get initial values:
  if (debug_) std::cout << "Read TMB initial values" << std::endl;
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_alct_delay_value  = thisTMB->GetAlctVpfDelay();        //0xb2
  int initial_match_window_size = thisTMB->GetAlctMatchWindowSize(); //0xb2
  int initial_mpc_tx_delay      = thisTMB->GetMpcTxDelay();          //0xb2
  //
  const float desired_window_size = (float) initial_match_window_size;
  //
  // desired value should put central value of measured distribution at the following location (counting from 0)
  float desired_value = (desired_window_size*0.5) - 0.5;
  //
  (*MyOutput_) << "With the current CLCT match window width setting of " << desired_window_size << "bx," << std::endl;
  (*MyOutput_) << "the value of the ALCT to be centered in the CLCT match window should end up in bin " 
	       << desired_value << std::endl;
  //
  // Enable this TMB for this test
  if (debug_>=10) std::cout << "Initialize TMB for test" << std::endl;
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetAlctVpfDelay(test_alct_delay_value);
  thisTMB->SetAlctMatchWindowSize(test_match_window_size);
  thisTMB->WriteRegister(tmbtim_adr);
  //
  ::usleep(500000); //time for registers to write...
  //
  if (debug_>=10) {
    std::cout << "Read tmb_trig_adr " << std::hex << tmb_trig_adr
	      << " = 0x" << thisTMB->ReadRegister(tmb_trig_adr) << std::endl;
    std::cout << "Read tmbtim_adr " << std::hex << tmbtim_adr
	      << " = 0x" << thisTMB->ReadRegister(tmbtim_adr) << std::endl;
  }
  //
  ZeroTmbHistograms();
  //
  if (debug_) std::cout << "Going to read TMB " << std::dec << getNumberOfDataReads() << " times" << std::endl;
  for (int i=0; i<getNumberOfDataReads(); i++) {
    if ((i%25) == 0 && debug_) std::cout << "Read TMB " << i << " times" << std::endl;
    thisTMB->TMBRawhits(getPauseBetweenDataReads());
    int value = thisTMB->GetAlctInClctMatchWindow();
    AlctInClctMatchWindowHisto_[value]++;
  }
  //
  if (debug_>=5) std::cout << "Begin histogram analysis" << std::endl;
  float average_value = AverageHistogram(AlctInClctMatchWindowHisto_,HistoMin,HistoMax);
  //
  // Print the data:
  (*MyOutput_) << "Setting match_trig_alct_delay = " << test_alct_delay_value << " gives:" << std::endl;
  if (debug_) std::cout << "Setting match_trig_alct_delay = " << test_alct_delay_value << " gives:" << std::endl;
  //
  PrintHistogram("ALCT in CLCT match window",AlctInClctMatchWindowHisto_,HistoMin,HistoMax,average_value);  
  //
  // amount to shift this distribution is:
  float amount_to_shift = average_value - desired_value;
  int int_amount_to_shift = 0;   //initialize it to zero in case amount_to_shift = 0...
  if (amount_to_shift > 0.) {
    int_amount_to_shift = (int) (amount_to_shift+0.5);
  } else if (amount_to_shift < 0.) {
    int_amount_to_shift = (int) (amount_to_shift-0.5);
  }
  //
  (*MyOutput_) << "The amount to shift the distribution by is " << amount_to_shift 
	       << " (= " << int_amount_to_shift << ")" << std::endl;
  if (debug_) std::cout << "The amount to shift the distribution by is " << amount_to_shift 
			<< " (= " << int_amount_to_shift << ")" << std::endl;
  //
  measured_match_trig_alct_delay_ = test_alct_delay_value - int_amount_to_shift;
  //
  // Determine new value of mpc_tx_delay, based on the prior values 
  // of mpc_tx_delay and match_trig_alct_delay (assuming they had lined LCTs at the MPC)...
  //
  int amount_to_shift_mpc_tx_delay = measured_match_trig_alct_delay_ - initial_alct_delay_value;
  //
  // The mpc_tx_delay compensates for the change in match_trig_alct_delay, so subtract the shift:
  measured_mpc_tx_delay_ = initial_mpc_tx_delay - amount_to_shift_mpc_tx_delay;
  //
  (*MyOutput_) << "Therefore, based on the current settings in the xml file:" << std::endl;
  (*MyOutput_) << "(match_trig_window_size = " << std::dec << desired_window_size      << ")" << std::endl;
  (*MyOutput_) << "(mpc_tx_delay           = " << std::dec << initial_mpc_tx_delay     << ")" << std::endl;
  (*MyOutput_) << "(match_trig_alct_delay  = " << std::dec << initial_alct_delay_value << ")" << std::endl;
  //    
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "Best value is match_trig_alct_delay = " << measured_match_trig_alct_delay_ << std::endl;
  (*MyOutput_) << "Best value is mpc_tx_delay          = " << measured_mpc_tx_delay_          << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  //
  if (debug_) {  
    std::cout << "Therefore, based on the current settings in the xml file:" << std::endl;
    std::cout << "(match_trig_window_size = " << std::dec << desired_window_size      << ")" << std::endl;
    std::cout << "(mpc_tx_delay           = " << std::dec << initial_mpc_tx_delay     << ")" << std::endl;
    std::cout << "(match_trig_alct_delay  = " << std::dec << initial_alct_delay_value << ")" << std::endl;
    std::cout << "Best value is match_trig_alct_delay = " << measured_match_trig_alct_delay_ << std::endl;
    std::cout << "Best value is mpc_tx_delay          = " << measured_mpc_tx_delay_          << std::endl;
  }
  //
  // return to initial values:
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  int return_value;
  //
  if (use_measured_values_) { 
    //
    (*MyOutput_) << "Setting measured values of match_trig_alct_delay and mpc_tx_delay..." << std::endl;
    thisTMB->SetAlctVpfDelay(measured_match_trig_alct_delay_);
    thisTMB->SetMpcTxDelay(measured_mpc_tx_delay_);
    //
    // if using the measured values (e.g., for automatic()), return the smaller of these two, so that a negative
    // value will correctly stop yourself from setting a negative value and blithely continuing...
    return_value = (measured_match_trig_alct_delay_ < measured_mpc_tx_delay_ ?   
		    measured_match_trig_alct_delay_ : measured_mpc_tx_delay_ );  
    //
  } else { 
    //
    (*MyOutput_) << "Reverting back to original values of match_trig_alct_delay and mpc_tx_delay..." << std::endl;
    thisTMB->SetAlctVpfDelay(initial_alct_delay_value); 
    thisTMB->SetMpcTxDelay(initial_mpc_tx_delay);
    //
    //since using the (usable) xml parameters, automatic() can continue with this return value 
    return_value = 1;                                    
    //
  }
  thisTMB->SetAlctMatchWindowSize(initial_match_window_size);
  thisTMB->WriteRegister(tmbtim_adr);
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return return_value;
}
//
//int ChamberUtilities::FindALCTvpf() {
//  //
//  // thisCCB_->setCCBMode(CCB::VMEFPGA);
//  // Not really necessary:
//  // thisTMB->alct_match_window_size_ = 3;
//  //
//  const int MaxTimeBin   = 15;
// //
//  int alct_in_window[MaxTimeBin] = {};
//  //
//  for (int i = 0; i < MaxTimeBin; i++){
//    //
//    std::cout << "ALCT_vpf_delay=" << i << std::endl;
//    //
//    thisTMB->alct_vpf_delay(i);    // loop over this
//    //thisTMB->trgmode(1);         // 
//    thisTMB->ResetCounters();      // reset counters
//    //thisCCB_->startTrigger();     // 2 commands to get trigger going
//    //thisCCB_->bx0();
//    ::sleep(5);                      // accumulate statistics
//    //thisCCB_->stopTrigger();      // stop trigger
//    thisTMB->GetCounters();        // read counter values
//    //
//    //thisTMB->PrintCounters();    // display them to screen
//    //
//    //    thisTMB->PrintCounters(8);
//    //    thisTMB->PrintCounters(10);
//    //
//    alct_in_window[i] = thisTMB->GetCounter(10);
//  }
//  //
//  float RightTimeBin      = 0;
//  float RightTimeBinDenom = 0;
//  //
//  for (int i = 0; i < MaxTimeBin; i++){
//    //
//    (*MyOutput_) << "match_trig_alct_delay " << std::dec << std::setw(5) << i
//		 << " : " << std::setw(10) << alct_in_window[i] << std::endl;
//    //
//    RightTimeBin      += ((float) alct_in_window[i]) * ((float) i) ;
//    RightTimeBinDenom += ((float) alct_in_window[i]) ;
//    //
//  }
//  //
//  if (RightTimeBin > 100) {
//    RightTimeBin /= RightTimeBinDenom ;
//  } else {
//    RightTimeBin = -999.;
//  }
//  //
//  printf("Best Setting is %f \n",RightTimeBin);
//  //
//  printf("\n");
//
//  ALCTvpf_ = (int) (RightTimeBin+0.5);
//
//  (*MyOutput_) << "Best value for match_trig_alct_delay = " << std::dec << ALCTvpf_ << std::endl;
//     
//  return ALCTvpf_ ;
//}
//
//----------------------------------------------
// Winner bits from MPC
//----------------------------------------------
int ChamberUtilities::FindWinner(){
  //
  // Use muons from beam or cosmic rays, or pulsing from TTC
  //
  if ( ! thisMPC ) {
    std::cout << " No MPC defined in XML file " << std::endl ;
    return -1 ;
  }
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  if (debug_) {
    std::cout << "******************" << std::endl;
    std::cout << "Find mpc_rx_delay:" << std::endl;
    std::cout << "******************" << std::endl;
    std::cout << "... using muons from beam, cosmic rays, or pulsing from TTC" << std::endl;
  }
  //
  (*MyOutput_) << "******************" << std::endl;
  (*MyOutput_) << "Find mpc_rx_delay:" << std::endl;
  (*MyOutput_) << "******************" << std::endl;
  (*MyOutput_) << "... using muons from beam, cosmic rays, or pulsing from TTC" << std::endl;
  //
  //
  // Set up for this test...
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_mpc_rx_delay      = thisTMB->GetMpcRxDelay();
  //
  if (debug_>=10) std::cout << "Set mpc_output_enable = 1" << std::endl;
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  ::sleep(1);
  //
  const int delaymin = 0;
  const int delaymax = 10;
  const int histo_size = delaymax - delaymin + 1;
  int number_of_mpc_accepted[histo_size] = {};
  //
  for (int delay_value=delaymin; delay_value<=delaymax; delay_value++) {
    //
    if (debug_) std::cout << "Set mpc_rx_delay = " << std::dec << delay_value;
    //
    thisTMB->SetMpcRxDelay(delay_value);
    thisTMB->WriteRegister(tmb_trig_adr);
    //
    thisTMB->ResetCounters();
    //
    ::sleep(getPauseAtEachSetting());
    //
    thisTMB->GetCounters();
    number_of_mpc_accepted[delay_value] = thisTMB->GetCounter( thisTMB->GetLCTAcceptedByMPCCounterIndex() );
    //
    if (debug_) {
      std::cout << ", number of MPC winner bits accepted = " 
		<< std::dec << number_of_mpc_accepted[delay_value] << std::endl;
    }
    if (debug_>10) thisTMB->PrintCounters();
  }
  //
  float average = AverageHistogram(number_of_mpc_accepted,delaymin,delaymax);
  //
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "Number of Winner bits vs. mpc_rx_delay..." << std::endl;
  PrintHistogram("Number of Winner Bits at TMB",number_of_mpc_accepted,delaymin,delaymax,average);
  //
  MPCdelay_ = RoundOff(average);
  //
  (*MyOutput_) << "Best value is mpc_rx_delay = " << MPCdelay_ << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  //
  if (debug_) std::cout << "Best value is mpc_rx_delay = " << MPCdelay_ << std::endl;
  //
  // return to initial values:
  //
  if (debug_>=10) std::cout << "Set mpc_output_enable back to = " << initial_mpc_output_enable << std::endl;
  //
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting measured value of mpc_rx_delay..." << std::endl;
    thisTMB->SetMpcRxDelay(MPCdelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of mpc_rx_delay..." << std::endl;
    thisTMB->SetMpcRxDelay(initial_mpc_rx_delay);
  }
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return MPCdelay_;
}
//
int ChamberUtilities::FindWinner(int npulses){
  // 
  // Use local pulsing of CFEB, sending the data from the TMB to the MPC by a VME command
  // This is presently not working :(
  //
  std::cout << "FindWinner:  Using local pulsing of CFEB, data sent to MPC from TMB by VME..." << std::endl;
  //
  if ( ! thisMPC ) {
    std::cout << " No MPC defined in XML file " << std::endl ;
    return -1 ;
  }
  //
  // Set up for this test...
  if (debug_) this->RedirectOutput(&std::cout);
  //
  // Get initial values:
  int initial_mpc_output_enable             = thisTMB->GetMpcOutputEnable();          //0x86
  int initial_clct_trig_enable              = thisTMB->GetTmbAllowClct();             //0x86
  int initial_mpc_rx_delay                  = thisTMB->GetMpcRxDelay();               //0x86
  int initial_layer_trig_enable             = thisTMB->GetEnableLayerTrigger();       //0xf0
  int initial_clct_pretrig_enable           = thisTMB->GetClctPatternTrigEnable();    //0x68
  int initial_clct_halfstrip_pretrig_thresh = thisTMB->GetHsPretrigThresh();          //0x70
  int initial_clct_pattern_thresh           = thisTMB->GetMinHitsPattern();           //0x70
  int initial_ignore_ccb_startstop          = thisTMB->GetIgnoreCcbStartStop();       //0x2c
  //
  // Enable this TMB for this test
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(1);
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(6);
  thisTMB->SetMinHitsPattern(6);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetEnableLayerTrigger(0);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(0);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->StartTTC();
  ::sleep(1);
  //
  if (debug_) {
    std::cout << "After enabling, TMB register 0x" << std::hex << tmb_trig_adr 
	      << " = 0x" << thisTMB->ReadRegister(tmb_trig_adr) 
	      << " which has value of mpc_output_enable = " << thisTMB->GetMpcOutputEnable() << std::endl;
  }
  //
  float MpcDelay=0;
  int   MpcDelayN=0;
  //
  float Mpc0Delay=0;
  int   Mpc0DelayN=0;
  //
  float Mpc1Delay=0;
  int   Mpc1DelayN=0;
  //
  const int DelaySize = 15;
  //
  int MPC0Count[DelaySize] = {};
  int MPC1Count[DelaySize] = {};
  //
  //
  for (int i = 0; i < DelaySize; i++){
    //
    int iterations = 0;
    //
    std::cout << "----------> mpc_delay_ =  " << std::dec << i << std::endl;
    //
    thisTMB->SetMpcRxDelay(i);
    thisTMB->WriteRegister(tmb_trig_adr);
    //
    thisTMB->ResetCounters();
    //
    while (iterations < npulses) {
      //
      thisMPC->SoftReset();
      //
      PulseCFEB(-1,0xa);
      //
      if (debug_) {
	//
	//thisMPC->firmwareVersion();
	//(*MyOutput_) << std::endl;
	//
	thisMPC->read_fifos();
	(*MyOutput_) << std::endl;
	//
	thisMPC->read_csr0();
	//
	thisTMB->DataSendMPC();
	thisTMB->GetCounters();
	thisTMB->PrintCounters();
	//
	std::cout << "----------> Accepted at TMB:  MPC0 = " << thisTMB->MPC0Accept() 
		  << ", MPC1 = " << thisTMB->MPC1Accept() << std::endl ;
      }
      //
      if ( (thisTMB->MPC0Accept()+thisTMB->MPC1Accept()) > 0 ) {
	MpcDelay  += i ;    
	MpcDelayN++;
	if ( thisTMB->MPC0Accept() > 0 ) {
	  Mpc0Delay  += i ;    
	  Mpc0DelayN++;
	  MPC0Count[i]++ ;
	}
	if ( thisTMB->MPC1Accept() > 0 ) {
	  Mpc1Delay  += i ;    
	  Mpc1DelayN++;
	  MPC1Count[i]++ ;
	}
      }
      //
      iterations++;
    }
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    (*MyOutput_) << "MPC0 winner delay=" << std::setw(3) << i << " gives " << MPC0Count[i] << std::endl;
  }
  (*MyOutput_) << std::endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    (*MyOutput_) << "MPC1 winner delay=" << std::setw(3) << i << " gives " << MPC1Count[i] << std::endl;
  }
  (*MyOutput_) << std::endl ;
  //
  MpcDelay  /= (MpcDelayN  + 0.0001) ;
  Mpc0Delay /= (Mpc0DelayN + 0.0001) ;
  Mpc1Delay /= (Mpc1DelayN + 0.0001) ;
  //
  (*MyOutput_) << "Correct MPC  setting  : " << MpcDelay << std::endl ;
  (*MyOutput_) << "Correct MPC0 setting  : " << Mpc0Delay << std::endl ;
  (*MyOutput_) << "Correct MPC1 setting  : " << Mpc1Delay << std::endl ;
  //
  MPCdelay_ = (int)(MpcDelay + 0.5);
  //
  // return to initial values:
  thisTMB->SetClctPatternTrigEnable(initial_clct_pretrig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->SetMpcRxDelay(initial_mpc_rx_delay);
  thisTMB->SetTmbAllowClct(initial_clct_trig_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_halfstrip_pretrig_thresh);
  thisTMB->SetMinHitsPattern(initial_clct_pattern_thresh);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetEnableLayerTrigger(initial_layer_trig_enable);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(initial_ignore_ccb_startstop);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  this->RedirectOutput(MyOutput_);
  //
  return MPCdelay_;
}
//
//----------------------------------------------
// DMB parameters
//----------------------------------------------
int ChamberUtilities::MeasureAlctDavCableDelay() {
  //
  if (debug_) {
    std::cout << "**************************" << std::endl;
    std::cout << "Find alct_dav_cable_delay:" << std::endl;
    std::cout << "**************************" << std::endl;
  }
  //
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "Find alct_dav_cable_delay:" << std::endl;
  (*MyOutput_) << "**************************" << std::endl;
  //
  const int DelayMin = 0;
  const int DelayMax = 3;
  float Average[4];                             //2-bit delay value
  //
  // Range over which to analyze the histogram:
  const int HistoMin = ScopeMin_;        
  const int HistoMax = ScopeMax_;
  int Histo[4][5];                              //Total range available:  scope is 5 bins wide
  //
  const int desired_value = 2;                  //we want the scope's average to be this value
  //
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetAlctDavCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // clear histograms
  for (int delay=0; delay<4; delay++) {
    Average[delay] = 0.;
    for (int histo=0; histo<5; histo++) 
      Histo[delay][histo] = 0;
  }
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetAlctDavCableDelay(delay);         //insert the delay value into the DMB cable_delay_ parameter
    thisDMB->setcbldly(thisDMB->GetCableDelay()); //write the value
    ::sleep(1);
    //
    // Get the data:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data:
    Average[delay] = AverageHistogram(AlctDavScopeHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = AlctDavScopeHisto_[bin];
    //
    // Print the data:
    (*MyOutput_) << "Setting alct_dav_cable_delay = " << delay << " gives..." << std::endl;
    if (debug_) std::cout << "Setting alct_dav_cable_delay = " << delay << " gives..." << std::endl;
    PrintHistogram("ALCT DAV Scope",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  AlctDavCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  PrintHistogram("Best ALCT DAV Scope",Histo[AlctDavCableDelay_],HistoMin,HistoMax,Average[AlctDavCableDelay_]);  
  (*MyOutput_) << "Best value is alct_dav_cable_delay = " << AlctDavCableDelay_ << "..." << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  //
  best_average_alct_dav_scope_ = Average[AlctDavCableDelay_];
  //
  if (debug_) std::cout << "Best value is alct_dav_cable_delay = " << AlctDavCableDelay_ << "..." << std::endl;
  //  
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured value of alct_dav_cable_delay..." << std::endl;
    thisDMB->SetAlctDavCableDelay(AlctDavCableDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of alct_dav_cable_delay..." << std::endl;
    thisDMB->SetAlctDavCableDelay(initial_delay_value);
  }
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return AlctDavCableDelay_;
}
//
int ChamberUtilities::MeasureTmbLctCableDelay() {
  //
  if (debug_) {
    std::cout << "*************************" << std::endl;
    std::cout << "Find tmb_lct_cable_delay:" << std::endl;
    std::cout << "*************************" << std::endl;
  }
  //
  (*MyOutput_) << "*************************" << std::endl;
  (*MyOutput_) << "Find tmb_lct_cable_delay:" << std::endl;
  (*MyOutput_) << "*************************" << std::endl;
  //
  const int DelayMin = 0;
  const int DelayMax = 7;                     //restrict the range to reduce the time for scanning...
  float Average[8];                           //3-bit delay value
  //
  // Range over which to analyze the histogram:
  const int HistoMin   = AffToL1aValueMin_;
  const int HistoMax   = AffToL1aValueMax_;
  int Histo[8][256];                          //Total range available:  counter is 8 bits long
  //
  //  int desired_value = 147;                   // this is the value we want the counter to be...
  //  int desired_value = 131;                   // value for xLatency = 1
  //  int desired_value = 115;                   // value for xLatency = 0
  int current_xlatency = thisDMB->GetxLatency();
  int desired_value = 115 + 16*current_xlatency;
  //
  // The ADB_SYNC pulse is 500ns long, and can induce another pulse on the strips...
  // Since the counter counts from the *most recent* AFF, we need to expect this value to be:
  if (UsePulsing_) desired_value -= 550/25;    // to agree with cosmics, this value appears better to be 550ns...
  //
  (*MyOutput_) << "For xLatency = " << current_xlatency 
	       << ", desired value is " << std::dec << desired_value << std::endl;
  if (debug_) std::cout << "For xLatency = " << current_xlatency 
			<< ", desired value is " << std::dec << desired_value << std::endl;
  //
  // Get initial values
  // Enable this TMB to send LCTs to MPC:  
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetTmbLctCableDelay();
  //
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // clear histograms
  for (int delay=0; delay<8; delay++) {
    Average[delay] = 0.;
    for (int histo=0; histo<256; histo++) 
      Histo[delay][histo] = 0;
  }
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Need to set extra latency values into the VME register before you push in
    // the cable_delay into this serial chain
    //
    //    thisDMB->setxfinelatency(thisDMB->GetxFineLatency());
    //    thisDMB->setxlatency(thisDMB->GetxLatency());
    //
    // Set the delay value:
    thisDMB->SetTmbLctCableDelay(delay);        //insert the delay value into the DMB cable_delay_ parameter
    int cable_delay = thisDMB->GetCableDelay(); //get DMB::cable_delay_
    thisDMB->setcbldly(cable_delay);            //write the value
    ::sleep(1);
    //
    // Get the data from this delay value:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data from this delay value:
    Average[delay] = AverageHistogram(AffToL1aValueHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = AffToL1aValueHisto_[bin];
    //
    // Print the data from this delay value:
    (*MyOutput_) << "Setting tmb_lct_cable_delay = " << delay << " gives..." << std::endl;
    if (debug_) std::cout << "Setting tmb_lct_cable_delay = " << delay << " gives..." << std::endl;
    PrintHistogram("AFF to L1A Value",Histo[delay],HistoMin,HistoMax,Average[delay]);  
  }
  //
  // determine which delay value gives the reading we want:
  TmbLctCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "Best value is tmb_lct_cable_delay = " << TmbLctCableDelay_ << "..." << std::endl;
  PrintHistogram("Best AFF to L1A Counter"  ,Histo[TmbLctCableDelay_],HistoMin,HistoMax,Average[TmbLctCableDelay_]);  
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  //
  best_average_aff_to_l1a_counter_ = Average[TmbLctCableDelay_];
  //
  if (debug_) std::cout << "Best value is tmb_lct_cable_delay = " << TmbLctCableDelay_ << "..." << std::endl;
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // Need to set extra latency values into the VME register before you push in 
  // the cable_delay into this serial chain
  //
  //  thisDMB->setxfinelatency(thisDMB->GetxFineLatency());
  //  thisDMB->setxlatency(thisDMB->GetxLatency());
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured values of tmb_lct_cable_delay..." << std::endl;
    thisDMB->SetTmbLctCableDelay(TmbLctCableDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of tmb_lct_cable_delay..." << std::endl;
    thisDMB->SetTmbLctCableDelay(initial_delay_value);
  }
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  float difference = fabs( ((float) desired_value) - best_average_aff_to_l1a_counter_);
  //
  // If our "best" value of tmb_lct_cable_delay gives an average value of the 
  // Active-FEB Flag <-> L1A difference greater than 1 away from the desired value,
  // we are not optimally receiving the L1A in the window for the CFEB...
  //
  if (difference > 0.5) {
    return -999;
  } else {
    return TmbLctCableDelay_;
  }
}
//
int ChamberUtilities::MeasureCfebDavCableDelay() {
  //
  if (debug_) {
    std::cout << "**************************" << std::endl;
    std::cout << "Find cfeb_dav_cable_delay:" << std::endl;
    std::cout << "**************************" << std::endl;
  }
  //
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "Find cfeb_dav_cable_delay:" << std::endl;
  (*MyOutput_) << "**************************" << std::endl;
  //
  const int DelayMin = 0;
  const int DelayMax = 3;
  float Average[4];              //2-bit delay value
  //
  // Range over which to analyze the histogram:
  const int HistoMin = ScopeMin_; 
  const int HistoMax = ScopeMax_;
  int Histo[4][5];               //total range available:  scope is 5 bins wide
  //
  const int desired_value = 2;   //we want the scope's average to be this value
  //
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetCfebDavCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // clear histograms
  for (int delay=0; delay<4; delay++) {
    Average[delay] = 0.;
    for (int histo=0; histo<5; histo++) 
      Histo[delay][histo] = 0;
  }
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetCfebDavCableDelay(delay);         //insert the delay value into the DMB cable_delay_ parameter
    thisDMB->setcbldly(thisDMB->GetCableDelay()); //write the value
    ::sleep(1);
    //
    // Get the data:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data:
    Average[delay] = AverageHistogram(CfebDavScopeHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = CfebDavScopeHisto_[bin];
    //
    // Print the data:
    (*MyOutput_) << "Setting cfeb_dav_cable_delay = " << delay << " gives..." << std::endl;
    if (debug_) std::cout << "Setting cfeb_dav_cable_delay = " << delay << " gives..." << std::endl;
    PrintHistogram("CFEB DAV Scope",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  CfebDavCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "Best value is cfeb_dav_cable_delay = " << CfebDavCableDelay_ << "..." << std::endl;
  if (debug_) std::cout << "Best value is cfeb_dav_cable_delay = " << CfebDavCableDelay_ << "..." << std::endl;  
  PrintHistogram("Best CFEB DAV Scope",Histo[CfebDavCableDelay_],HistoMin,HistoMax,Average[CfebDavCableDelay_]);  
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  //
  best_average_cfeb_dav_scope_ = Average[CfebDavCableDelay_];
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister( tmb_trig_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured value of cfeb_dav_cable_delay..." << std::endl;
    thisDMB->SetCfebDavCableDelay(CfebDavCableDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of cfeb_dav_cable_delay..." << std::endl;
    thisDMB->SetCfebDavCableDelay(initial_delay_value);
  }
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return CfebDavCableDelay_;
}
//
void ChamberUtilities::ReadAllDmbValuesAndScopes() {
  //
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  if (debug_>=10) std::cout << "Initial value of mpc_output_enable = " << initial_mpc_output_enable << std::endl;
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  ::sleep(1);
  //
  if (debug_>=10) {
    std::cout << "After enabling, TMB register 0x" << std::hex << tmb_trig_adr 
	      << " = 0x" << thisTMB->ReadRegister(tmb_trig_adr) 
	      << " which has value of mpc_output_enable = " << thisTMB->GetMpcOutputEnable() << std::endl;
  }
  //
  ZeroDmbHistograms();
  PopulateDmbHistograms();
  //
  AffToL1aScopeAverageValue_ = AverageHistogram(AffToL1aScopeHisto_,ScopeMin_,ScopeMax_);
  CfebDavScopeAverageValue_  = AverageHistogram(CfebDavScopeHisto_ ,ScopeMin_,ScopeMax_);
  TmbDavScopeAverageValue_   = AverageHistogram(TmbDavScopeHisto_  ,ScopeMin_,ScopeMax_);
  AlctDavScopeAverageValue_  = AverageHistogram(AlctDavScopeHisto_ ,ScopeMin_,ScopeMax_);
  //
  AffToL1aAverageValue_ = AverageHistogram(AffToL1aValueHisto_,AffToL1aValueMin_,AffToL1aValueMax_);
  CfebDavAverageValue_  = AverageHistogram(CfebDavValueHisto_ ,CfebDavValueMin_ ,CfebDavValueMax_ );
  TmbDavAverageValue_   = AverageHistogram(TmbDavValueHisto_  ,TmbDavValueMin_  ,TmbDavValueMax_  );
  AlctDavAverageValue_  = AverageHistogram(AlctDavValueHisto_ ,AlctDavValueMin_ ,AlctDavValueMax_ );
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister( tmb_trig_adr);
  //
  return;
}
//
void ChamberUtilities::PrintAllDmbValuesAndScopes() {
  //
  (*MyOutput_) << "Current state of all DMB Counters and Scopes is... " << std::endl;
  //
  PrintHistogram("AFF to L1A Value",AffToL1aValueHisto_,AffToL1aValueMin_,AffToL1aValueMax_,AffToL1aAverageValue_);
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("CFEB DAV Value"  ,CfebDavValueHisto_ ,CfebDavValueMin_ ,CfebDavValueMax_ ,CfebDavAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("TMB DAV Value"   ,TmbDavValueHisto_  ,TmbDavValueMin_  ,TmbDavValueMax_  ,TmbDavAverageValue_  );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("ALCT DAV Value"  ,AlctDavValueHisto_ ,AlctDavValueMin_ ,AlctDavValueMax_ ,AlctDavAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  //
  PrintHistogram("AFF to L1A Scope",AffToL1aScopeHisto_,ScopeMin_,ScopeMax_,AffToL1aScopeAverageValue_);
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("CFEB DAV Scope"  ,CfebDavScopeHisto_ ,ScopeMin_,ScopeMax_,CfebDavScopeAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("TMB DAV Scope"  ,TmbDavScopeHisto_   ,ScopeMin_,ScopeMax_,TmbDavScopeAverageValue_  );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("ALCT DAV Scope"  ,AlctDavScopeHisto_ ,ScopeMin_,ScopeMax_,AlctDavScopeAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  //
  return;
}
//
void ChamberUtilities::SetupRadioactiveTriggerConditions() {
  //
  initial_clct_pretrig_thresh_ = thisTMB->GetHsPretrigThresh();
  initial_clct_pattern_thresh_ = thisTMB->GetMinHitsPattern();
  initial_alct_pretrig_thresh_ = alct->GetPretrigNumberOfLayers();
  initial_alct_pattern_thresh_ = alct->GetPretrigNumberOfPattern();
  //
  thisTMB->SetHsPretrigThresh(1);
  thisTMB->SetMinHitsPattern(1);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  alct->SetPretrigNumberOfLayers(1);
  alct->SetPretrigNumberOfPattern(1);
  alct->WriteConfigurationReg();
  //
  return;
}
//
void ChamberUtilities::ReturnToInitialTriggerConditions() {

  thisTMB->SetHsPretrigThresh(initial_clct_pretrig_thresh_);
  thisTMB->SetMinHitsPattern(initial_clct_pattern_thresh_);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  alct->SetPretrigNumberOfLayers(initial_alct_pretrig_thresh_);
  alct->SetPretrigNumberOfPattern(initial_alct_pattern_thresh_);
  alct->WriteConfigurationReg();
  //
  return;
}
//----------------------------------------------
// L1A accept windows
//----------------------------------------------
int ChamberUtilities::FindTmbAndAlctL1aDelay(){
  //
  if (debug_) {
    std::cout << "**************************" << std::endl;
    std::cout << "Find tmb(alct)_l1a_delay:" << std::endl;
    std::cout << "**************************" << std::endl;
  }
  //
  (*MyOutput_) << "*************************" << std::endl;
  (*MyOutput_) << "Find tmb(alct)_l1a_delay:" << std::endl;
  (*MyOutput_) << "*************************" << std::endl;
  //
  // Set up for this test...
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_tmb_delay             = thisTMB->GetL1aDelay();
  int initial_alct_l1a_delay        = alct->GetWriteL1aDelay();
  //
  // Enable this TMB for this test
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  int tmb_in_l1a_window[255] = {};
  int alct_in_l1a_window[255] = {};
  //
  int tmb_delay_value;
  int tmb_delay_min = getMinTmbL1aDelayValue();
  int tmb_delay_max = getMaxTmbL1aDelayValue();
  //
  int alct_delay_value;
  const int alct_delay_min = getMinAlctL1aDelayValue();
  const int alct_delay_max = getMaxAlctL1aDelayValue();
  //
  int range = ((alct_delay_max-alct_delay_min) > (tmb_delay_max-tmb_delay_min)) ? (alct_delay_max-alct_delay_min) : (tmb_delay_max-tmb_delay_min);
  //
  for (int delay=0; delay<=range; delay++){
    //
    tmb_delay_value  = tmb_delay_min  + delay;
    alct_delay_value = alct_delay_min + delay;
    //
    thisTMB->SetL1aDelay(tmb_delay_value);
    thisTMB->WriteRegister(seq_l1a_adr);
    //
    alct->SetL1aDelay(alct_delay_value);
    alct->WriteConfigurationReg();
    if (debug_>=10) {
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
    }
    //
    thisTMB->ResetCounters();    // reset counters
    //
    ::sleep(getPauseAtEachSetting());                  // accumulate statistics
    //
    thisTMB->GetCounters();      // read counter values
    //
    tmb_in_l1a_window[tmb_delay_value]   = thisTMB->GetCounter( thisTMB->GetL1AInTMBWindowCounterIndex() );
    alct_in_l1a_window[alct_delay_value] = thisTMB->GetCounter( thisTMB->GetALCTRawHitsReadoutCounterIndex()  );
    //
    if (debug_) {
      std::cout << "Set tmb_l1a_delay  = " << std::dec << tmb_delay_value;
      std::cout << ", TMB in L1A window  =  " << std::dec  << tmb_in_l1a_window[tmb_delay_value]  << std::endl;
      //
      std::cout << "Set alct_l1a_delay = " << std::dec << alct_delay_value;
      std::cout << ", ALCT in L1A window =  " << std::dec << alct_in_l1a_window[alct_delay_value] << std::endl;
    }
    //
    if (debug_>=10) thisTMB->PrintCounters();
  }
  //
  // analyze histograms
  //
  float tmb_average = AverageHistogram(tmb_in_l1a_window,tmb_delay_min,tmb_delay_max);
  //
  (*MyOutput_) << "---------------------------------------------" << std::endl;
  (*MyOutput_) << "Number of L1A in TMB window vs. tmb_l1a_delay" << std::endl;
  PrintHistogram("Number of L1A in TMB window",tmb_in_l1a_window,tmb_delay_min,tmb_delay_max,tmb_average);
  //
  TMBL1aTiming_ = RoundOff(tmb_average);
  (*MyOutput_) << " Best value is tmb_l1a_delay = " << TMBL1aTiming_ << std::endl;
  (*MyOutput_) << "---------------------------------------------" << std::endl;
  (*MyOutput_) << std::endl;
  //
  if (debug_) {
    std::cout    << " Best value is tmb_l1a_delay = " << TMBL1aTiming_ << std::endl;
    std::cout    << std::endl;
  }
  //
  //
  float alct_average = AverageHistogram(alct_in_l1a_window,alct_delay_min,alct_delay_max);
  //
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  (*MyOutput_) << "Number of L1A in ALCT window vs. alct_l1a_delay" << std::endl;
  PrintHistogram("Number of L1A in ALCT window",alct_in_l1a_window,alct_delay_min,alct_delay_max,alct_average);
  //
  // The L1A receipt window is truncated in firmware to be an even number of bx wide.  Thus,
  // in order to make sure we are on the LEFT (low) side of the average value, use the following
  // method for rounding off...
  ALCTL1aDelay_ = RoundOffForEvenWindowWidths(alct_average);
  //
  //  ALCTL1aDelay_ = RoundOff(alct_average);
  //
  (*MyOutput_) << " Best value is alct_l1a_delay = " << ALCTL1aDelay_ << std::endl;
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  //
  if (debug_) std::cout << " Best value is alct_l1a_delay = " << ALCTL1aDelay_ << std::endl;
  //
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured value of tmb(alct)_l1a_delay..." << std::endl;
    thisTMB->SetL1aDelay(TMBL1aTiming_);
    alct->SetL1aDelay(ALCTL1aDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of tmb(alct)_l1a_delay..." << std::endl;
    thisTMB->SetL1aDelay(initial_tmb_delay);
    alct->SetL1aDelay(initial_alct_l1a_delay);
  }
  thisTMB->WriteRegister(seq_l1a_adr);
  alct->WriteConfigurationReg();
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  int return_value = (TMBL1aTiming_ < ALCTL1aDelay_ ? TMBL1aTiming_ : ALCTL1aDelay_);
  return return_value;
}
//
//
int ChamberUtilities::FindTMB_L1A_delay(){
  //
  // use the values of min and max set by the accessors
  //
  return FindTMB_L1A_delay(getMinTmbL1aDelayValue(),getMaxTmbL1aDelayValue());
  //
}
//
int ChamberUtilities::FindTMB_L1A_delay(int delay_min, int delay_max){
  //
  if (debug_) {
    std::cout << "*******************" << std::endl;
    std::cout << "Find tmb_l1a_delay:" << std::endl;
    std::cout << "*******************" << std::endl;
  }
  //
  (*MyOutput_) << "*******************" << std::endl;
  (*MyOutput_) << "Find tmb_l1a_delay:" << std::endl;
  (*MyOutput_) << "*******************" << std::endl;
  //
  // Set up for this test...
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_delay             = thisTMB->GetL1aDelay();
  //
  // Enable this TMB for this test
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  int tmb_in_l1a_window[255] = {};
  //
  for (int delay=delay_min; delay<=delay_max; delay++){
    //
    if (debug_) std::cout << "Set tmb_l1a_delay = " << std::dec << delay;
    thisTMB->SetL1aDelay(delay);
    thisTMB->WriteRegister(seq_l1a_adr);
    //
    thisTMB->ResetCounters();    // reset counters
    //
    ::sleep(getPauseAtEachSetting());                  // accumulate statistics
    //
    thisTMB->GetCounters();      // read counter values
    //
    tmb_in_l1a_window[delay] = thisTMB->GetCounter( thisTMB->GetL1AInTMBWindowCounterIndex() );
    //
    if (debug_) std::cout << ", TMB in L1A window =  " << std::dec << tmb_in_l1a_window[delay] << std::endl;
    //
    if (debug_>=10) thisTMB->PrintCounters();
  }
  //
  float average = AverageHistogram(tmb_in_l1a_window,delay_min,delay_max);
  //
  (*MyOutput_) << "---------------------------------------------" << std::endl;
  (*MyOutput_) << "Number of L1A in TMB window vs. tmb_l1a_delay" << std::endl;
  PrintHistogram("Number of L1A in TMB window",tmb_in_l1a_window,delay_min,delay_max,average);
  //
  TMBL1aTiming_ = RoundOff(average);
  (*MyOutput_) << " Best value is tmb_l1a_delay = " << TMBL1aTiming_ << std::endl;
  (*MyOutput_) << "---------------------------------------------" << std::endl;
  //
  std::cout    << " Best value is tmb_l1a_delay = " << TMBL1aTiming_ << std::endl;
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured value of tmb_l1a_delay..." << std::endl;
    thisTMB->SetL1aDelay(TMBL1aTiming_);
  } else {
    (*MyOutput_) << "Reverting back to original value of tmb_l1a_delay..." << std::endl;
    thisTMB->SetL1aDelay(initial_delay);
  }
  thisTMB->WriteRegister(seq_l1a_adr);
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return TMBL1aTiming_ ;
}
//
//
int ChamberUtilities::FindALCT_L1A_delay(){
  //
  // use the values of min and max set by the accessors
  //
  return FindALCT_L1A_delay(getMinAlctL1aDelayValue(),getMaxAlctL1aDelayValue());
  //
}
//
int ChamberUtilities::FindALCT_L1A_delay(int minlimit, int maxlimit){
  //
  if (debug_) {
    std::cout << "********************" << std::endl;
    std::cout << "Find alct_l1a_delay:" << std::endl;
    std::cout << "********************" << std::endl;
  }
  //
  (*MyOutput_) << "********************" << std::endl;
  (*MyOutput_) << "Find alct_l1a_delay:" << std::endl;
  (*MyOutput_) << "********************" << std::endl;
  //
  // Set up for this test...
  // Get initial values:
  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  int initial_alct_l1a_delay    = alct->GetWriteL1aDelay();
  //
  // Enable this TMB for this test
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  int ALCT_l1a_accepted[256] = {};
  //
  for (int delay=minlimit; delay<=maxlimit; delay++) {
    //
    if (debug_) std::cout << "Set alct_l1a_delay = " << std::dec << delay;
    alct->SetL1aDelay(delay);
    alct->WriteConfigurationReg();
    if (debug_>=10) {
      alct->ReadConfigurationReg();
      alct->PrintConfigurationReg();
    }
    //
    thisTMB->ResetCounters();
    //
    ::sleep(getPauseAtEachSetting());
    //
    thisTMB->GetCounters();
    //
    ALCT_l1a_accepted[delay] = thisTMB->GetCounter( thisTMB->GetALCTRawHitsReadoutCounterIndex() );
    //
    if (debug_) std::cout << ", ALCT in L1A window =  " << std::dec << ALCT_l1a_accepted[delay] << std::endl;
    //
    if (debug_>=10) thisTMB->PrintCounters();
  }
  //
  float average = AverageHistogram(ALCT_l1a_accepted,minlimit,maxlimit);
  //
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  (*MyOutput_) << "Number of L1A in ALCT window vs. alct_l1a_delay" << std::endl;
  PrintHistogram("Number of L1A in ALCT window",ALCT_l1a_accepted,minlimit,maxlimit,average);
  //
  // The L1A receipt window is truncated in firmware to be an even number of bx wide.  Thus,
  // in order to make sure we are on the LEFT (low) side of the average value, use the following
  // method for rounding off...
  ALCTL1aDelay_ = RoundOffForEvenWindowWidths(average);
  //
  //  ALCTL1aDelay_ = RoundOff(average);
  //
  (*MyOutput_) << " Best value is alct_l1a_delay = " << ALCTL1aDelay_ << std::endl;
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  //
  if (debug_) std::cout << " Best value is alct_l1a_delay = " << ALCTL1aDelay_ << std::endl;
  //
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting measured value of alct_l1a_delay..." << std::endl;
    alct->SetL1aDelay(ALCTL1aDelay_);
  } else {
    (*MyOutput_) << "Reverting back to original value of alct_l1a_delay..." << std::endl;
    alct->SetL1aDelay(initial_alct_l1a_delay);
  }
  alct->WriteConfigurationReg();
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return ALCTL1aDelay_;
}
//
//
//////////////////////////////////////////////////////////////////////
// scans to check functionality of electronics
//////////////////////////////////////////////////////////////////////
void ChamberUtilities::ALCTChamberScan(){
  //
  if (debug_) {
    std::cout << "******************" << std::endl;
    std::cout << "ALCT chamber scan:" << std::endl;
    std::cout << "******************" << std::endl;
  }
  //
  (*MyOutput_) << "******************" << std::endl;
  (*MyOutput_) << "ALCT chamber scan:" << std::endl;
  (*MyOutput_) << "******************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // read initial values:
  int initial_alct_trigger_mode    = alct->GetTriggerMode();
  int initial_alct_pat_trig_enable = thisTMB->GetAlctPatternTrigEnable();
  //
  // Set up for this test:
  thisTMB->SetAlctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  alct->SetTriggerMode(0);
  alct->WriteConfigurationReg();
  if (debug_>=10) {
    alct->ReadConfigurationReg();
    alct->PrintConfigurationReg();
  }
  //
  thisTMB->StartTTC();
  ::sleep(1);
  //
  int NPulses = 1;
  int chamberResult[MAX_NUM_WIRES_PER_LAYER];
  int chamberResult2[MAX_NUM_WIRES_PER_LAYER];
  int InJected[MAX_NUM_WIRES_PER_LAYER];
  //
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) chamberResult[keyWG] = 0;
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) chamberResult2[keyWG] = 0;
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) InJected[keyWG] = 0;
  //
  for (int Ninject=0; Ninject<NPulses; Ninject++){
    //
    for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) {
      //
      if (debug_) std::cout << "Injecting in WG = " << std::dec << keyWG << std::endl;
      //
      for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct())/6; channel++) {
	  if (channel==keyWG) {
	    alct->SetHotChannelMask(layer,channel,ON);
	  } else {
	    alct->SetHotChannelMask(layer,channel,OFF);
	  }
	}
      }
      //
      alct->WriteHotChannelMask();
      if (debug_>=10) {
	alct->ReadHotChannelMask();	
	alct->PrintHotChannelMask();	
      }
      //
      PulseTestStrips();
      ::usleep(100);
      //
      thisTMB->DecodeALCT();
      if (debug_>=10) {
	thisTMB->PrintALCT();
	std::cout << "ALCT Wordcount = " << std::dec << thisTMB->GetALCTWordCount() << std::endl;
      }
      //
      if (thisTMB->GetAlct0FirstKey() == keyWG && 
	  thisTMB->GetAlct0Quality()  >= 1 ) 
	chamberResult[keyWG]++;
      //
      if (thisTMB->GetAlct1SecondKey()== keyWG && 
	  thisTMB->GetAlct1Quality()  >= 1 ) 
	chamberResult2[keyWG]++;
      //
      InJected[keyWG]++;
    }
  }
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) 
    ALCTWireScan_[keyWG] = chamberResult[keyWG];
  //
  (*MyOutput_) << "Wire" << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << keyWG/100 ;
  (*MyOutput_) << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << ((keyWG/10)%10) ;
  (*MyOutput_) << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << keyWG%10 ;
  (*MyOutput_) << std::endl;
  (*MyOutput_) << "Injected" << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << InJected[keyWG] ;
  (*MyOutput_) << std::endl;
  (*MyOutput_) << "ALCT Chamber Scan Result" << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << chamberResult[keyWG] ;
  (*MyOutput_) << std::endl;
  for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) (*MyOutput_) << chamberResult2[keyWG] ;
  (*MyOutput_) << std::endl;
  //
  if (debug_) {
    std::cout << "Wire" << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << keyWG/100 ;
    std::cout << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << ((keyWG/10)%10) ;
    std::cout << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << keyWG%10 ;
    std::cout << std::endl;
    std::cout << "Injected" << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << InJected[keyWG] ;
    std::cout << std::endl;
    std::cout << "ALCT Chamber Scan Result" << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << chamberResult[keyWG] ;
    std::cout << std::endl;
    for (int keyWG=0; keyWG<alct->GetNumberOfChannelsPerLayer(); keyWG++) std::cout << chamberResult2[keyWG] ;
    std::cout << std::endl;
  }
  //
  // revert back to initial settings
  thisTMB->SetAlctPatternTrigEnable(initial_alct_pat_trig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  alct->SetTriggerMode(initial_alct_trigger_mode);
  alct->WriteConfigurationReg();
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return;
}
//
void ChamberUtilities::CFEBChamberScan(){
  //
  if (debug_) {
    std::cout << "******************" << std::endl;
    std::cout << "CFEB chamber scan:" << std::endl;
    std::cout << "******************" << std::endl;
  }
  //
  (*MyOutput_) << "******************" << std::endl;
  (*MyOutput_) << "CFEB chamber scan:" << std::endl;
  (*MyOutput_) << "******************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Set up for this test...
  // Get initial values:
  int initial_clct_pretrig_enable           = thisTMB->GetClctPatternTrigEnable();    //0x68
  int initial_clct_trig_enable              = thisTMB->GetTmbAllowClct();             //0x86
  int initial_clct_halfstrip_pretrig_thresh = thisTMB->GetHsPretrigThresh();          //0x70
  int initial_clct_pattern_thresh           = thisTMB->GetMinHitsPattern();           //0x70
  int initial_layer_trig_enable             = thisTMB->GetEnableLayerTrigger();       //0xf0
  int initial_ignore_ccb_startstop          = thisTMB->GetIgnoreCcbStartStop();       //0x2c
  //
  // Enable this TMB for this test
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(6);
  thisTMB->SetMinHitsPattern(6);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(0);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(0);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  comparing_with_clct_ = true;
  thisTMB->StartTTC();
  ::sleep(1);
  //
  int MaxStrip = 160;
  int MaxStripWithinCFEB = 32;
  //
  int Muons[MaxStrip];
  int MuonsMaxHits[MaxStrip];
  //
  for (int j=0; j<MaxStrip; j++) {
    Muons[j] = 0;
    MuonsMaxHits[j] = 0;
  }
  //
  int CLCTInputList[5] = {0x1,0x2,0x4,0x8,0x10};
  //
  for (int List=0; List<5; List++){
    //
    for (int Nmuons=0; Nmuons < Npulses_; Nmuons++){
      //
      for (int HalfStrip=0; HalfStrip<MaxStripWithinCFEB; HalfStrip++) {
	//
	if (debug_) 
	  std::cout << "Enabling TMB comparator inputs 0x" << std::hex << CLCTInputList[List] << std::endl;
	//
	PulseCFEB( HalfStrip,CLCTInputList[List]);
	//
	int pulsed_halfstrip = List*MaxStripWithinCFEB + HalfStrip;
	//
	int clct0patternId = thisTMB->GetCLCT0PatternId();
	int clct0nhit = thisTMB->GetCLCT0Nhit();
	int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	//
	if (debug_) {
	  std::cout << " clct0cfeb " << List << std::endl;
	  std::cout << " clct0nhit " << clct0nhit << std::endl;
	  std::cout << " clct0keyHalfStrip " << clct0keyHalfStrip << std::endl;
	  std::cout << std::endl;
	}
	//
	if ( clct0patternId == 10 && clct0keyHalfStrip == pulsed_halfstrip && 
	     clct0nhit == 6       && MuonsMaxHits[pulsed_halfstrip] < clct0nhit   ) 
	  MuonsMaxHits[HalfStrip] = clct0nhit ;

	if ( clct0nhit == 6 && clct0patternId == 10 ) Muons[pulsed_halfstrip]++;
      }
    }      
  }
  //
  // preserve the results:
  for (int i=0;i<5;i++) 
    for (int j=0; j<MaxStripWithinCFEB; j++) {
      int pulsed_halfstrip = i*MaxStripWithinCFEB + j;
      CFEBStripScan_[i][j] = Muons[pulsed_halfstrip];
    }
  //
  // print out the results
  (*MyOutput_) << " Number of Muons seen " << std::endl;
  for (int CFEBs = 0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id="<<CFEBs<< " " ;
    for (int HalfStrip = 0; HalfStrip<MaxStripWithinCFEB; HalfStrip++) {
      int pulsed_halfstrip = CFEBs*MaxStripWithinCFEB + HalfStrip;
      (*MyOutput_) << std::setw(3) << Muons[pulsed_halfstrip] ;
    }
    (*MyOutput_) << std::endl;
  }
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << " Maximum number of hits " << std::endl;
  for (int CFEBs = 0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id="<<CFEBs<< " " ;
    for (int HalfStrip = 0; HalfStrip<MaxStripWithinCFEB; HalfStrip++) {
      int pulsed_halfstrip = CFEBs*MaxStripWithinCFEB + HalfStrip;
      (*MyOutput_) << std::setw(3) << MuonsMaxHits[pulsed_halfstrip] ;
    }
    (*MyOutput_) << std::endl;
  }
  //
  // return to initial values:
  thisTMB->SetClctPatternTrigEnable(initial_clct_pretrig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(initial_clct_trig_enable);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_halfstrip_pretrig_thresh);
  thisTMB->SetMinHitsPattern(initial_clct_pattern_thresh);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(initial_ignore_ccb_startstop);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(initial_layer_trig_enable);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  comparing_with_clct_ = false;
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return;
}
//
void ChamberUtilities::FindDistripHotChannels(){
  //
  if (debug_) {
    std::cout << "**************************" << std::endl;
    std::cout << "Find Distrip Hot Channels:" << std::endl;
    std::cout << "**************************" << std::endl;
  }
  //
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "Find Distrip Hot Channels:" << std::endl;
  (*MyOutput_) << "**************************" << std::endl;
  //
  // if a chamber whose HV is OFF is firing one time per orbit, call it too noisy:
  const float maximum_allowed_rate = 11223;  
  //
  // maximum fraction any one CLCT distrip is allowed to contribute to the rate, if it above, it is deemed HOT:
  const float maximum_contribution = 0.1;
  //
  // number of microseconds we spend on each distrip to determine if it is hot
  const int usec_wait = 500000;
  //
  const int number_of_layers = 6;
  const int number_of_distrips_per_layer = 40;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Set up for this test...
  // Get initial values:
  int initial_clct_pretrig_enable           = thisTMB->GetClctPatternTrigEnable();    //0x68
  int initial_clct_halfstrip_pretrig_thresh = thisTMB->GetHsPretrigThresh();          //0x70
  int initial_layer_trig_enable             = thisTMB->GetEnableLayerTrigger();       //0xf0
  int initial_ignore_ccb_startstop          = thisTMB->GetIgnoreCcbStartStop();       //0x2c
  //
  int initial_hot_channel_mask[number_of_layers][number_of_distrips_per_layer];
  for (int layer=0; layer<number_of_layers; layer++) 
    for (int distrip=0; distrip<number_of_distrips_per_layer; distrip++) 
      initial_hot_channel_mask[layer][distrip] = thisTMB->GetDistripHotChannelMask(layer,distrip);
  //
  int hot_channel_mask[number_of_layers][number_of_distrips_per_layer];
  //
  // Enable this TMB for this test
  // We want CLCT to pretrigger on single 1/2-strips to be able to find which (single) distrip is hot:
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetHsPretrigThresh(1);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetEnableLayerTrigger(0);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  // Start the TMB triggering ourselves:
  thisTMB->SetIgnoreCcbStartStop(0);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->StartTTC();
  ::sleep(1);
  //
  // First establish the baseline:
  //
  //turn all distrips on
  for (int layer=0; layer<number_of_layers; layer++) {
    for (int distrip=0; distrip<number_of_distrips_per_layer; distrip++) {
      hot_channel_mask[layer][distrip] = 1;
      thisTMB->SetDistripHotChannelMask(layer,distrip,hot_channel_mask[layer][distrip]);     
    }
  }
  thisTMB->WriteDistripHotChannelMasks();
  ::usleep(10);
  //
  if (debug_) std::cout << "With all distrips ON..." << std::endl;
  thisTMB->ResetCounters();
  ::usleep(usec_wait);
  thisTMB->GetCounters();
  //
  int clct_pretrigger_counts = thisTMB->GetCounter( thisTMB->GetCLCTPretriggerCounterIndex() );
  if (debug_) std::cout << "---> CLCT pretrigger rate = " << clct_pretrigger_counts << std::endl;
  float clct_pretrigger_rate = ( ((float) clct_pretrigger_counts) / ((float) usec_wait) ) * 1000000.;  // rate in Hz
  //
  if (clct_pretrigger_rate < maximum_allowed_rate) {
    (*MyOutput_) << "CLCT pretrigger rate = " << clct_pretrigger_rate << " less than " << maximum_allowed_rate 
		 << ", so no hot channels necessary for this chamber..." << std::endl;    
    if (debug_) 
      std::cout << "CLCT pretrigger rate = " << clct_pretrigger_rate << " less than " << maximum_allowed_rate 
		<< ", so no hot channels necessary for this chamber..." << std::endl;    
    return;
  }
  //
  // go through each distrip and measure the rate on each one...
  for (int layer_test=0; layer_test<number_of_layers; layer_test++) {
    for (int distrip_test=0; distrip_test<number_of_distrips_per_layer; distrip_test++) {
      //
      if (debug_) 
	std::cout << "HotChannelMask:  Checking (layer,distrip) = " << layer_test << "," << distrip_test << ")" << std::endl;
      //
      int working_hot_channel_mask[number_of_layers][number_of_distrips_per_layer];
      //
      for (int layer=0; layer<number_of_layers; layer++) {
	for (int distrip=0; distrip<number_of_distrips_per_layer; distrip++) {
	  //
	  if (layer==layer_test && distrip==distrip_test) {
	    working_hot_channel_mask[layer][distrip] = 1;
	  } else {
	    working_hot_channel_mask[layer][distrip] = 0;
	  }
	  thisTMB->SetDistripHotChannelMask(layer,distrip,working_hot_channel_mask[layer][distrip]);     
	}
      }
      thisTMB->WriteDistripHotChannelMasks();
      ::usleep(10);
      //
      thisTMB->ResetCounters();
      ::usleep(usec_wait);
      thisTMB->GetCounters();
      //
      clct_pretrigger_counts = thisTMB->GetCounter( thisTMB->GetCLCTPretriggerCounterIndex() );
      if (debug_) std::cout << "---> CLCT pretrigger rate = " << clct_pretrigger_counts << std::endl;
      //
      clct_pretrigger_rate = ( ((float) clct_pretrigger_counts) / ((float) usec_wait) ) * 1000000.;  // rate in Hz
      //
      if (clct_pretrigger_rate > maximum_contribution*maximum_allowed_rate) {
	hot_channel_mask[layer_test][distrip_test] = 0;
	(*MyOutput_) << "Distrip Hot Channel Mask:  Masking off (layer,channel) = (" 
		     << layer_test << "," << distrip_test << ")" << std::endl;
	if (debug_) 
	  std::cout << "Distrip Hot Channel Mask:  Masking off (layer,channel) = (" 
		    << layer_test << "," << distrip_test << ")" << std::endl;
      } else {
	hot_channel_mask[layer_test][distrip_test] = 1;
      }
    }
  }
  //
  // print out the results:
  //
  for (int layer=0; layer<number_of_layers; layer++) {
    char print_hot_channel_mask[number_of_distrips_per_layer/8];
    thisTMB->packCharBuffer(hot_channel_mask[layer],
			    number_of_distrips_per_layer,
			    print_hot_channel_mask);
    //
    int char_counter = number_of_distrips_per_layer/8 - 1;
    //
    (*MyOutput_) << "layer" << std::dec << layer << "_distrip_hot_channel_mask=\"";    
    if (debug_) std::cout << "layer" << std::dec << layer << "_distrip_hot_channel_mask=\"";    
    //
    for (int layer_counter=number_of_distrips_per_layer/8; layer_counter>0; layer_counter--) {
      (*MyOutput_) << std::hex
		   << ((print_hot_channel_mask[char_counter] >> 4) & 0xf) 
		   << (print_hot_channel_mask[char_counter] & 0xf);

      if (debug_) {
	std::cout << std::hex
		  << ((print_hot_channel_mask[char_counter] >> 4) & 0xf) 
		  << (print_hot_channel_mask[char_counter] & 0xf);
      }
      //
      char_counter--;
    }
    (*MyOutput_) << "\"" << std::endl;
    if (debug_) std::cout << "\"" << std::endl;
  }
  //
  // return to initial values:
  thisTMB->SetClctPatternTrigEnable(initial_clct_pretrig_enable);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_halfstrip_pretrig_thresh);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(initial_ignore_ccb_startstop);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(initial_layer_trig_enable);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  for (int layer=0; layer<number_of_layers; layer++) {
    for (int distrip=0; distrip<number_of_distrips_per_layer; distrip++) {
      thisTMB->SetDistripHotChannelMask(layer,distrip,initial_hot_channel_mask[layer][distrip]);     
    }
  }
  thisTMB->WriteDistripHotChannelMasks();
  //
  thisTMB->RedirectOutput(MyOutput_);
  thisDMB->RedirectOutput(MyOutput_);
  thisCCB_->RedirectOutput(MyOutput_);
  thisMPC->RedirectOutput(MyOutput_);
  //
  return;
}
//
//////////////////////////////////////////////////////////////////////
// useful peripheral crate functions
//////////////////////////////////////////////////////////////////////
void ChamberUtilities::InitSystem(){
  //
  std::cout << "Init Chamber " << std::endl ;
  //
  //emuController.configure();               // Init system
  //
  thisCCB_->configure();
  if (thisTMB) thisTMB->configure();
  if (thisDMB) thisDMB->configure();
  if (thisMPC) thisMPC->configure();
  if (alct)    alct->configure();
  //
  //  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
void ChamberUtilities::InitStartSystem(){
  //
  std::cout << "Init System " << std::endl ;
  //
  //emuController.configure();               // Init system
  //
  thisCCB_->configure();
  if (thisTMB) thisTMB->configure();
  if (thisDMB) thisDMB->configure();
  if (thisMPC) thisMPC->configure();
  if (alct)    alct->configure();
  //
  if (thisTMB) thisTMB->StartTTC();
  if (thisTMB) thisTMB->EnableL1aRequest();
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
//
void ChamberUtilities::SetupCoincidencePulsing() {
  //  
  const int default_amplitude = 20;
  const int default_signal    = ADB_SYNC;
  //
  SetupCoincidencePulsing(default_amplitude,default_signal);
  //
  return;
}
//
void ChamberUtilities::SetupCoincidencePulsing(int amplitude) {
  //  
  const int default_signal = ADB_SYNC;
  //
  SetupCoincidencePulsing(amplitude,default_signal);
  //
  return;
}
//
void ChamberUtilities::SetupCoincidencePulsing(int amplitude, int pulse_signal) {
  //
  // Setup to pulse the chamber so that both CLCT and ALCT fire on a single (TTC) command
  //
  // Mask off the distrip channels near both edges of the chamber for pulsing the teststrip:
  //
  // We mask off both because:
  // 1) The distrip nearest the teststrip fires hot when the teststrip is enabled.  
  // 2) The placement of the teststrip relative to which edge depends on the routing of the 
  //    lemo cables, which depends on the chamber type
  // 3) Since the distrip farthest from the teststrip does not have a pulse induced, anyway,
  //    we can mask it off safely
  for (int layer=0; layer<6; layer++) {
    thisTMB->SetDistripHotChannelMask(layer,0,0);
    thisTMB->SetDistripHotChannelMask(layer,39,0);
  }
  thisTMB->WriteDistripHotChannelMasks();
  //
  if (debug_) {
    thisTMB->ReadDistripHotChannelMasks();
    thisTMB->PrintHotChannelMask();
  }
  //
  // Now tell this TMB to send the Active FEB flag to all CFEBs.
  // This is because the DMB counter for the AFF-to-L1A timing is only for CFEB2, 
  // while the pulsing will only fire distrips on CFEB0 or CFEB4
  thisTMB->SetEnableAllCfebsActive(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  //
  // Set up ALCT teststrip pulsing: 
  //
  if (debug_) std::cout << "Setup coincidence pulsing in TMB slot " << thisTMB->slot() << std::endl;
  const int strip_mask = 0x3f;        //pulse all six (layers of) strips
  //
  alct->SetUpPulsing(amplitude,PULSE_LAYERS,strip_mask,pulse_signal);
  ::sleep(1);
  //
  UsePulsing_ = true;
  //
  return;
}
//
void ChamberUtilities::PulseRandomALCT(int delay){
  //
  alct->SetUpRandomALCT();
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips(delay);
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
  printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  //
  (*MyOutput_) << std::endl;
  //
  thisDMB->PrintCounters();
  //
  (*MyOutput_) << std::endl;
  //
}	      
//
void ChamberUtilities::PulseAllWires(){
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips();
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
  printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  //
  (*MyOutput_) << std::endl;
  //
  thisDMB->PrintCounters();
  //
}
//
void ChamberUtilities::PulseTestStrips(int delay){
  //
  //  int slot = thisTMB->slot();
  //
  if ( alct ) {
    //
    if ( beginning == 0 ) {
      //
      thisTMB->DisableCLCTInputs();
      //
      int amplitude = 255;
      int strip_mask = 0xff;
      alct->SetUpPulsing(amplitude,PULSE_AFEBS,strip_mask,ADB_ASYNC);
      //
      beginning = 1;
      //
      PulseTestStrips();
      //
    } else {
      //
      if (debug_>=10) {
	if (alct->GetChamberType().find("ME11")!= std::string::npos) {
	  std::cout << alct->GetChamberType().find("ME11") <<std::endl;
	  std::cout << alct->GetChamberType() <<std::endl;
	  std::cout << "ME11" <<std::endl;
	} else {
	  std::cout << "ME12" <<std::endl;
	}
      }
      //
      thisCCB_->setCCBMode(CCB::VMEFPGA);
      thisCCB_->WriteRegister(0x28,delay);  //4Aug05 DM changed 0x789b to 0x7862
      //
      if (debug_>=10) std::cout << "Setting CCB internal delay to = " << std::dec << delay << std::endl ;
      //
      thisCCB_->ReadRegister(0x28);
      //
      thisCCB_->GenerateAlctAdbASync();	 
      //
    }
    //
  } else {
    if (debug_) std::cout << " No ALCT " << std::endl;
  }  
  //
}
//
void ChamberUtilities::LoadCFEB(int HalfStrip, int CLCTInputs, bool enableL1aEmulator ){
  //
  if ( enableL1aEmulator ) thisTMB->EnableInternalL1aSequencer();
  thisTMB->DisableCLCTInputs();
  thisTMB->SetCLCTPatternTrigger();
  //
  thisDMB->set_comp_thresh(0.1);
  thisDMB->set_dac(0.5,0);
  //
  if (HalfStrip == -1 ) HalfStrip = int(rand()*31./(RAND_MAX+1.0));
  //
  if (debug_) std::cout << " -- Injecting CFEB in halfstrip = " << std::dec << HalfStrip 
			<< " into CLCT inputs = 0x" << std::hex << CLCTInputs << std::endl;
  //
  //thisTMB->DiStripHCMask(HalfStrip/4-1); // counting from 0; //Bad...requests L1a....
  //
  int hp[6];
  //
  if (comparing_with_clct_) {
    //
    // The CLCT key-layer has been moved from layer 3 to layer 2.
    // As a result, for a staggered, non-bending pattern, the key 1/2-strip 
    // determined by the CLCT is changed by 1.
    //
    for (int layer = 0; layer < 6; layer++ )
      hp[layer] = HalfStrip+1;       
    //
  } else{
    //
    for (int layer = 0; layer < 6; layer++ )
      hp[layer] = HalfStrip;       
  }
  //
  // Set the pattern
  //
  thisDMB->trigsetx(hp,CLCTInputs);
  //
  thisTMB->EnableCLCTInputs(CLCTInputs);
  //
  // Inject it (pulse the CFEBs)
  //
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  thisCCB_->WriteRegister(0x28,0x7878);  //4Aug05 DM changed 0x789b to 0x7862
  //
  return;
}
//
void ChamberUtilities::PulseCFEB(int HalfStrip, int CLCTInputs, bool enableL1aEmulator ){
  //
  LoadCFEB(HalfStrip, CLCTInputs, enableL1aEmulator);
  //
  thisDMB->inject(1,0x4f);
  //
  // Decode the TMB CLCTs (0 and 1)
  //
  thisTMB->DecodeCLCT();
  //
  if (debug_>10) {
    thisTMB->PrintCLCT();
    thisDMB->PrintCounters();
  }
  //
  return;
}
//
void ChamberUtilities::InjectMPCData(){
  //
  float MpcDelay=0;
  int   MpcDelayN=0;
  //
  float Mpc0Delay=0;
  int   Mpc0DelayN=0;
  //
  float Mpc1Delay=0;
  int   Mpc1DelayN=0;
  //
  int   DelaySize = 15;
  //
  int   MPC0Count[DelaySize];
  int   MPC1Count[DelaySize];
  //
  for (int i=0; i<DelaySize; i++ ) {
    MPC0Count[i] = 0;
    MPC1Count[i] = 0;
  }
  //
  thisTMB->DisableCLCTInputs();
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  //
  for (int i=0; i<DelaySize; i++) {
    //
    for (int npulse=0; npulse<1; npulse++) {
      //
      std::cout << std::endl ;
      std::cout << "New Run" << std::endl ;
      std::cout << std::endl ;
      //
      thisTMB->mpc_delay(i);
      //
      std::cout << "mpc_delay_ =  " << std::dec << i << std::endl;
      //
      thisMPC->SoftReset();
      thisMPC->init();
      thisMPC->read_fifos();
      //
      thisTMB->InjectMPCData(1,0,0);
      //
      //thisTMB->InjectMPCData(1,0xf7a6a813,0xc27da3b2);
      //
      //thisTMB->FireMPCInjector(1);
      //
      thisCCB_->FireCCBMpcInjector();
      //
      thisMPC->read_fifos();
      //
      thisTMB->ReadRegister(0x86);
      thisTMB->ReadRegister(0x92);
      thisTMB->ReadRegister(0x90);
      //
      if ( thisTMB->MPC0Accept() > 0 ) {
	Mpc0Delay  += i ;    
	Mpc0DelayN++;
	MPC0Count[i]++ ;
      }
      //
      if ( thisTMB->MPC1Accept() > 0 ) {
	Mpc1Delay  += i ;    
	Mpc1DelayN++;
	MPC1Count[i]++ ;
      }
      //
      if( thisTMB->MPC0Accept()+thisTMB->MPC1Accept() > 0 ) {
	MpcDelay  += i ;    
	MpcDelayN++;
      }
      //
    }
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    std::cout << "MPC0 winner delay=" << std::setw(3) << i << " gives " << MPC0Count[i] << std::endl;
  }
  //
  std::cout << std::endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    std::cout << "MPC1 winner delay=" << std::setw(3) << i << " gives " << MPC1Count[i] << std::endl;
  }
  //
  std::cout << std::endl ;
  //
}
//
void ChamberUtilities::PopulateDmbHistograms() {
  //
  for (int i=0; i<getNumberOfDataReads(); i++) {
    //
    if ( ((i+1)%5) == 100 && debug_)  std::cout << "DMB Read " << std::dec << i << " times" << std::endl;
    //
    thisDMB->readtimingCounter();
    thisDMB->readtimingScope();
    //
    int AffToL1aValue = thisDMB->GetL1aLctCounter();
    int CfebDavValue  = thisDMB->GetCfebDavCounter();
    int TmbDavValue   = thisDMB->GetTmbDavCounter();
    int AlctDavValue  = thisDMB->GetAlctDavCounter();
    //
    int AffToL1aScope = thisDMB->GetL1aLctScope();
    int CfebDavScope  = thisDMB->GetCfebDavScope();
    int TmbDavScope   = thisDMB->GetTmbDavScope();
    int AlctDavScope  = thisDMB->GetAlctDavScope();
    //
    // Increment histograms with read values
    //
    AffToL1aValueHisto_[AffToL1aValue]++;
    CfebDavValueHisto_[CfebDavValue]++;
    TmbDavValueHisto_[TmbDavValue]++;
    AlctDavValueHisto_[AlctDavValue]++;
    //
    // Scope value is a 5-bit bitfield:
    for( int i=0; i<5; i++) {
      AffToL1aScopeHisto_[i] += ((AffToL1aScope>>i)&0x1);
      CfebDavScopeHisto_[i]  += ((CfebDavScope >>i)&0x1);
      TmbDavScopeHisto_[i]   += ((TmbDavScope  >>i)&0x1);
      AlctDavScopeHisto_[i]  += ((AlctDavScope >>i)&0x1);
    }
    //
  ::usleep(getPauseBetweenDataReads());
  }
  return;
}
//
/*
extern char cmd[];
extern char sndbuf[];
extern char rcvbuf[];
void settrgsrc(int dword)
      {
	cout << "Input DMB trigger source value in std::hex (0 turns off internal L1A and LCT)" << std::endl;
	cin >> std::hex >> dword >> std::dec;

	cmd[0]=VTX2_USR1;
	sndbuf[0]=37;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR2;
	sndbuf[0]=dword&0x0F; 
	thisDMB->devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR1;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_BYPASS;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
	
	printf("Cal_trg source are Set to %01x (Hex). \n",dword&0x0F);
      }
*/
int ChamberUtilities::AdjustL1aLctDMB(){
  //
  thisDMB->readtimingCounter();
  if ( thisDMB->GetL1aLctCounter() == 0 ) {
    printf(" ****************************** \n");
    printf(" *** You need to enable DMB *** \n");
    printf(" ****************************** \n");
    //return -1;
  }
  //
  int Counter = -1 ;
  //
  for( int l1a=90; l1a<110; l1a++) {
    thisCCB_->SetL1aDelay(l1a);
    ::sleep(2);
    thisDMB->readtimingCounter();
    Counter = thisDMB->GetL1aLctCounter() ;
    printf(" ************ L1a lct counter l1a=%d Counter=%d \n ",l1a,Counter);
    //
    if ( Counter >= 115 && Counter <= 117 ) break ;
    //
  }
  //
  return Counter;
  //
}
//
//////////////////////////////////////////////////////////////////////
// analysis methods
//////////////////////////////////////////////////////////////////////
void ChamberUtilities::ZeroTmbHistograms() {
  //
  if (debug_>=5) std::cout << "Zero TMB histograms" << std::endl;
  //
  // Counter histograms
  for (int i=0; i<15; i++) {
    AlctInClctMatchWindowHisto_[i] = 0;
  }
  //
  return;
}
//
void ChamberUtilities::ZeroDmbHistograms() {
  //
  if (debug_>=5) std::cout << "Zero DMB histograms" << std::endl;
  //
  // scope histograms:
  for (int i=0; i<5; i++) {
    AffToL1aScopeHisto_[i] = 0;
    CfebDavScopeHisto_[i]  = 0;
    TmbDavScopeHisto_[i]   = 0;
    AlctDavScopeHisto_[i]  = 0;
  }
  //
  // Counter histograms
  for (int i=0; i<255; i++) {
    AffToL1aValueHisto_[i] = 0;
    CfebDavValueHisto_[i]  = 0;
    TmbDavValueHisto_[i]   = 0;
    AlctDavValueHisto_[i]  = 0;
  }
  //
  return;
}
//
void ChamberUtilities::ALCT_phase_analysis (int rxtx_timing[13][13]) {
  int i, j, k, p;  
  //
  if (debug_>=5) {
    std::cout << "The array you are working with is:" << std::endl;
    std::cout << std::endl;
    for (j = 0; j < 13; j++) {
      for (k = 0; k < 13; k++) {
	std::cout << std::setw(5) << rxtx_timing[j][k] << " ";
      }
      std::cout << std::endl;
    }
  }
  //
  //now find best position by the non-zero number that is 
  //farthest from zero in both directions
  //
  int nmin_best, ntot_best;
  int nup, ndown, nleft, nright;
  int ntot, nmin;
  int best_element_row = 999;
  int best_element_col = 999;
  nmin_best=1;   // this will require there be at least 1 good delay value on all sides...
  ntot_best=0;
  for (j = 0; j < 13; j++) {           //loops over rows
    for (k = 0; k < 13; k++) {       //loops over columns
      //  
      if ( rxtx_timing[j][k] != 0) {        //find all non-zero matrix elements
	//
	if (debug_>=5) std::cout << "element (row, column) = (" << j << "," << k << ") is good "<< std::endl;
	//
	for (i=j+1;rxtx_timing[i%13][k]!=0 && i<j+13;i++) {   //scans all non-zero element below
	}
	ndown = i-1-j;  //number of non-zero elements below
	//
	for (p=k+1;rxtx_timing[j][p%13]!=0 && p<k+13 ;p++) {  //scans all non-zero elements to the right
	}
	nright = p-1-k;  //number of non-zero elements to the right
	//
	for (i=j-1;rxtx_timing[(13+i)%13][k]!=0 && i>j-13;i--) {   //scans all non-zero elements above
	}
	nup = j-1-i;  //number of non-zero elements above
	//
	for (p=k-1;rxtx_timing[j][(13+p)%13]!=0 && p>k-13;p--) {   //scans all non-zero elements to the left
	}
	nleft = k-1-p;  //number of non-zero elements to the left
	//
	nmin = 100;
	int numbers[] = {nup, ndown, nleft, nright};
	//
	if (debug_>=5) std::cout << " (up,down,left,right) = (";
	for (int d = 0; d < 4; d++) {    // finds the minimum number of non-zero elements in any direction
	  if (debug_>=5) std::cout << numbers[d] << ",";
	  if (nmin > numbers[d]) {
	    nmin = numbers [d];
	  }
	}
	ntot=ndown+nup+nright+nleft;         //finds the total number of non-zero elements in all directions
	//
	if (debug_>=5) std::cout << ")" << " --> nmin = " << nmin << ", ntot = " << ntot << std::endl;
	//
	if (nmin_best <= nmin) {         //finds the array element with the best nmin and ntot value
	  nmin_best = nmin;
	  if (ntot_best < ntot) {
	    ntot_best = ntot;
	    best_element_row = j;
	    best_element_col = k;
	  }
	  //
	  // std::cout << "best element so far is " << best_element_row << best_element_col << std::endl;
	  // std::cout << std::endl;
	  //
	}
      }
    }
  }
  if (debug_>=5) {
    std::cout << "the best values of nmin and ntot are:  " << std::endl;
    std::cout << "nmin_best =  " << nmin_best << "  and ntot_best =  " << ntot_best << std::endl;
  }
  if (debug_) {
    std::cout << "best element is: " << std::endl;
    std::cout << "rx =  " << best_element_row << "    tx =  " << best_element_col << std::endl;
    std::cout << std::endl;
  }
  //
  ALCTrxPhase_ = best_element_row ;
  ALCTtxPhase_ = best_element_col ;
  //
  (*MyOutput_) << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl;
  (*MyOutput_) << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl;
  //
  return;
}	
//
int ChamberUtilities::window_analysis(int * data, const int length) {
  //
  // Determine the best value for the delay setting based on vector of data "data", of length "length"
  //
  // ASSUME:  Data wraps around (such as for phase)
  //
  // ASSUME:  Channel is GOOD if it has fewer counts than:
  const int count_threshold = 10;
  //
  // ASSUME:  We want to exclude windows of width less than the following value:
  const int width_threshold = 2;
  //
  if (debug_>=5) {
    std::cout << "For Window Analysis:" << std::endl;
    std::cout << "  -> Counts > " << std::dec << count_threshold << " considered bad" << std::endl;
  }
  //
  // copy data for wrap-around:
  int twotimeslength = 2 * length;
  int copy[twotimeslength];
  int begin_channel = -1;           
  for (int i=0; i<twotimeslength; i++) {
    copy[i] = data[i % length];
    if (copy[i]>count_threshold && begin_channel<0)   //begin window scan on the first channel of "bad data"
      begin_channel = i;
  }
  if (begin_channel < 0) {
    (*MyOutput_) << "ERROR:  Scan is all 0's"<< std::endl;
    return -999;
  }
  //
  int end_channel = begin_channel+length;
  //
  // Find the windows of "good data", beginning with the first channel of "bad data"
  int window_counter = -1;
  int window_width[length];
  int window_start[length];
  int window_end[length];
  for (int i=0; i<length; i++) {
    window_width[i] = 0;
    window_start[i] = 0;
    window_end[i] = 0;
  }
  for (int delay=begin_channel; delay<end_channel; delay++) {
    if (copy[delay] < count_threshold) {            //Here is a good channel...
      if (copy[delay-1] >= count_threshold)          //and it is a start of a good window
	window_start[++window_counter] = delay;
      window_width[window_counter]++;
    } 
  }

  if (window_counter < 0) {
    (*MyOutput_) << "ERROR:  No windows with counts above count_threshold."<< std::endl;
    return -999;
  }    

  int counter;
  for (counter=0; counter<=window_counter; counter++) {
    window_end[counter] = window_start[counter]+window_width[counter] -1;
    if (debug_>=5) {
      std::cout << "Window = " << std::dec << counter;
      std::cout << " is " << std::dec << window_width[counter] << " channels wide, ";
      std::cout << " from " << std::dec << (window_start[counter] % 13)
		   << " to " << std::dec << (window_end[counter] % 13) << std::endl;    
    }
  }
  if (debug_>=5) 
    std::cout << "  -> window must be at least " << std::dec 
	      << width_threshold << " channels wide" << std::endl;
  //
  //Determine middle of window(s)
  int channel;
  float value;
  float channel_ctr;
  float denominator;
  float average_channel[length];
  //
  int bestValue = -1;
  //
  for (counter=0; counter<=window_counter; counter++) {
    if (window_width[counter] > width_threshold) {                      // exclude narrow width windows
      channel_ctr=0.;
      denominator=0.;
      for (channel=window_start[counter]; channel<=window_end[counter]; channel++) {
	// For average, use unary operation on bits in counter (since 0 = good)
	value = (float) ~(copy[channel] | 0xffff0000);
	channel_ctr += value * (float) channel;
	denominator += value;
      }
      //
      average_channel[counter] = channel_ctr / denominator;
      //
      bestValue = (((int)(average_channel[counter]+0.5)) % 13);
      //
      if (debug_>=5)
	std::cout << "=> BEST DELAY VALUE (window " << std::dec 
		  << counter << ") = " <<  bestValue  << " <=" << std::endl;
    }
  }
  //
  return bestValue;
}
//
int ChamberUtilities::DelayWhichGivesDesiredValue(float * values, const int min_delay, const int max_delay, int desired_value) {
  //
  if (debug_>=5) {
    std::cout << "Find delay which gives closest to " << desired_value << " from following list:" << std::endl;
    for (int i=min_delay; i<=max_delay; i++) 
      std::cout << "value[" << i << "] = " << values[i] << std::endl;
  }
  //
  int best_delay_value = -999; 
  //
  const int number_of_values = max_delay - min_delay + 1;
  float vector_of_differences[number_of_values];
  //
  for (int delay=min_delay; delay<=max_delay; delay++) 
    vector_of_differences[delay] = fabs(values[delay] - (float)(desired_value)); 
  //
  float min_difference=999.;
  //
  // pick index whose vector value is closest to desired value:
  for (int delay=min_delay; delay<=max_delay; delay++) {
    if (vector_of_differences[delay] < min_difference) {
      best_delay_value = delay;
      min_difference = vector_of_differences[delay];
    }
  }
  return best_delay_value;
}
//
void ChamberUtilities::PrintHistogram(std::string label, int * histogram, int min, int max, int average) {
  //
  for (int i=min; i<=max; i++) {
    (*MyOutput_) << std::dec << label << "[" << i << "] = " << histogram[i] << std::endl;
    if (debug_) std::cout << std::dec << label << "[" << i << "] = " << histogram[i] << std::endl;
  }
  //
  (*MyOutput_) << std::dec << label << " average = " << average << " (integer)" << std::endl;
  if (debug_) std::cout << std::dec << label << " average = " << average << " (integer)" << std::endl;
  //
  return;
}
//
void ChamberUtilities::PrintHistogram(std::string label, int * histogram, int min, int max, float average) {
  //
  for (int i=min; i<=max; i++) {
    (*MyOutput_) << std::dec << label << "[" << i << "] = " << histogram[i] << std::endl;
    if (debug_) std::cout << std::dec << label << "[" << i << "] = " << histogram[i] << std::endl;
  }
  //
  (*MyOutput_) << std::dec << label << " average = " << average << " (float)" << std::endl;
  if (debug_) std::cout << std::dec << label << " average = " << average << " (float)" << std::endl;
  //
  return;
}
//
float ChamberUtilities::AverageHistogram(int * histogram, int min_value, int max_value) {
  //
  /// determine the best value by slowly reducing the range over which the average is computed
  //
  float average;
  //
  const int max_width = 7;
  bool first_time = true;
  //
  int width = max_value - min_value + 1;  //starting value for width = input values
  //
  while ( width>=max_width || first_time ) {
    //
    float numer   = 0;
    float denom   = 0;
    average = -999;
    //
    for (int i=min_value; i<=max_value; i++) {
      numer += ((float) histogram[i]) *((float) (i));
      denom += (float) histogram[i];
    }
    if (debug_>=5) {
      std::cout << "Determine average in " << width << " wide window from " << min_value << " to " << max_value << std::endl;
      std::cout << "numerator   = " << numer << std::endl;
      std::cout << "denominator = " << denom << std::endl;
    }
    //
    if (denom > 10) 
      average = numer / denom;
    //
    if (average > 0) {          // Good determination of average, reduce the width and compute again
      //
      width -= 2;
      min_value = ((int) (0.5 + (average - ((float) width)*0.5) ) ) >? min_value;  //maximum of lower value and min_value
      max_value = ((int) (0.5 + (average + ((float) width)*0.5) ) ) <? max_value;  //minimum of lower value and min_value
      //
    } else {                    // Bad determination of average, get out of loop....
      //
      width = 0;
      //
    }
    first_time = false;
    if (debug_>=5) std::cout << "average     = " << average << std::endl;
  }
  //
  return average;;
}
//
//
void ChamberUtilities::bit_to_array(int data, int * array, const int size) {
  int i;
  for (i=0; i<=(size-1); i++) {
    array[i] = (data >> i) & 0x00000001;
  }

  return;
}
//
////////////////////////////////////////////////////////////////
// Following should be deprecated?
////////////////////////////////////////////////////////////////
void ChamberUtilities::ALCTScanDelays(){

  //  unsigned long HCmask[22];
  int CountDelay[20];
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  //  for (int i=0; i< 22; i++) HCmask[i] = 0;
  for (int i=0; i< 20; i++) CountDelay[i] = 0;
  //
  thisTMB->SetALCTPatternTrigger();
  //
  for ( int nloop=0; nloop<20; nloop++){
    for ( int DelaySetting=0; DelaySetting<20; DelaySetting++){
      //
      int keyWG  = int(rand()/(RAND_MAX+0.01)*alct->GetNumberOfChannelsPerLayer());
      //      int ChamberSection = alct->GetNumberOfChannelsInAlct()/6;
      //
      std::cout << std::endl ;
      //
      std::cout << "Injecting at " << std::dec << keyWG << std::endl;
      for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct()/6); channel++) {
	  if (channel==keyWG) {
	    alct->SetHotChannelMask(layer,channel,ON);
	  } else {
	    alct->SetHotChannelMask(layer,channel,OFF);
	  }
	}
      }
      //
      alct->WriteHotChannelMask();
      alct->PrintHotChannelMask();	
      //
      for (int afeb=0; afeb<alct->GetNumberOfAfebs(); afeb++) 
	alct->SetAsicDelay(afeb,DelaySetting);
      alct->WriteAsicDelaysAndPatterns();
      //
      PulseTestStrips();
      printf("Decode ALCT\n");
      thisTMB->DecodeALCT();
      printf("After Decode ALCT\n");
      //
      // Now analyze
      //
      if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	   (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	   (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	   (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondKey() != alct1_bxn) ) {
	//
	alct0_quality     = thisTMB->GetAlct0Quality();
	alct1_quality     = thisTMB->GetAlct1Quality();
	alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	alct0_key         = thisTMB->GetAlct0FirstKey();
	alct1_key         = thisTMB->GetAlct1SecondKey();
	//
	if ( alct0_quality == 3 && alct0_key == keyWG &&
	     alct1_quality == 3 && alct1_key == keyWG ) CountDelay[DelaySetting]++ ;
	//
      }
      //
    }
  }
  //
  for ( int DelaySetting=0; DelaySetting < 20; DelaySetting++ ) std::cout << CountDelay[DelaySetting] << " " ;
  //
  std::cout << std::endl ;
  std::cout << std::endl ;
  //
}
//

  } // namespace emu::pc
 } // namespace emu
