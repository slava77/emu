//-----------------------------------------------------------------------
// $Id: ChamberUtilities.cc,v 1.42 2012/05/09 20:59:00 liu Exp $
// $Log: ChamberUtilities.cc,v $
// Revision 1.42  2012/05/09 20:59:00  liu
// fix missing standard header files
//
// Revision 1.41  2012/01/30 16:46:29  cvuosalo
// Fix compiling warning about uninitiailized variable
//
// Revision 1.40  2011/08/16 11:44:39  rakness
// fix bug at max edge of window to evaluate in determining average
//
// Revision 1.39  2011/08/01 17:13:59  rakness
// automatically handle special region before/after and ME1/1a, ME1/1b, normal chambers separately
//
// Revision 1.38  2011/07/08 13:56:06  rakness
// add a function to measure CFEB-TMB timing on ME1/1a independently
//
// Revision 1.37  2011/02/04 11:27:22  rakness
// update CFEB rx timing scan to handle the special region of 1bx timing shifts
//
// Revision 1.36  2010/09/29 03:44:56  rakness
// first attempt to handle the special CFEB timing region
//
// Revision 1.35  2010/08/19 11:18:27  rakness
// fix bug to pick the correct posneg in CFEBrx scan
//
// Revision 1.34  2010/02/02 15:38:10  rakness
// randomize comparator being pulsed in CFEB-TMB rx test
//
// Revision 1.33  2010/02/01 14:16:29  rakness
// update bad region for ALCT-TMB rx/tx scan
//
// Revision 1.32  2009/11/19 17:04:56  rakness
// change CFEB-TMB rx scan to look at non-straight tracks
//
// Revision 1.31  2009/11/10 10:53:26  rakness
// fix bug picking the correct pipedepth for the updated rx/tx scan
//
// Revision 1.30  2009/11/08 15:09:12  rakness
// keep away from bad region in ALCT-TMB communication scan
//
// Revision 1.29  2009/11/07 15:22:42  rakness
// speed up synchronization scans
//
// Revision 1.28  2009/10/30 11:40:53  rakness
// Update CFEB rx scan for ME1/1 TMB firmware
//
// Revision 1.27  2009/10/28 11:36:25  rakness
// add sleeps after resyncs for BC0 scan
//
// Revision 1.26  2009/10/27 11:07:26  rakness
// 15 Oct 2009 TMB firmware update
//
// Revision 1.25  2009/06/11 12:20:59  rakness
// revamp rx/tx delay scan window algorithm
//
// Revision 1.24  2009/06/11 08:40:10  rakness
// make rx/tx scan faster (10M events checked instead of 40M)
//
// Revision 1.23  2009/05/29 16:53:19  rakness
// some bug fixes for rx/tx scans and downloading firmware
//
// Revision 1.22  2009/05/28 16:36:10  rakness
// update for May 2009 TMB and ALCT firmware versions
//
// Revision 1.21  2009/04/29 17:16:36  rakness
// roll back for Nov 2008 TMB firmware
//
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
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <unistd.h> 
#include <string>
#include <vector>
//
#include "emu/pc/ChamberUtilities.h"
#include "emu/pc/TMB_constants.h"
//
namespace emu {
  namespace pc {

ChamberUtilities::ChamberUtilities(){
  //
  debug_ =  1;     // debug >=  1 = normal output to std::cout
  //                  debug >=  5 = analysis debug output
  //                  debug >= 10 = hardware (registers) debug output
  //
  beginning = 0;
  thisTMB   = 0;
  thisDMB   = 0;
  alct      = 0;
  //
  UsePulsing_ = false;
  //
  Npulses_ = 2;
  comparing_with_clct_ = false;
  me11_pulsing_        = 0;
  //
  use_measured_values_ = false;
  //
  pause_between_data_reads_ = 10000; // default number of microseconds to wait between data reads
  number_of_data_reads_     = 100;    // default number of data reads
  //
  pause_at_each_setting_    = 1;     // default number of seconds to wait at each delay value
  min_alct_l1a_delay_value_ = 134;
  max_alct_l1a_delay_value_ = 170;
  min_tmb_l1a_delay_value_  = 115; 
  max_tmb_l1a_delay_value_  = 150; 
  local_tmb_bxn_offset_     = 3539;
  //
  MyOutput_ = &std::cout ;
  //
  // parameters to determine
  for( int i=0; i<5; i++) {
    CFEBrxPhase_[i]  = -1;
    CFEBrxPosneg_[i] = -1;
    cfeb_rxd_int_delay[i] = -1;
  }
  ALCTtxPhase_       = -1;
  ALCTrxPhase_       = -1;
  ALCTrxPosNeg_      = -1;
  ALCTtxPosNeg_      = -1;
  RatTmbDelay_       = -1;
  for(int i=0; i<2;i++) 
    RpcRatDelay_[i] = -1;
  ALCTvpf_           = -1;
  ALCTvpf            = -1;
  MPCdelay_          = -1;
  AlctDavCableDelay_ = -1;
  TmbLctCableDelay_  = -1;
  CfebDavCableDelay_ = -1;
  TMBL1aTiming_      = -1;
  ALCTL1aDelay_      = -1;
  //
  ALCT_bx0_delay_        = -1;
  match_trig_alct_delay_ = -1;
  tmb_bxn_offset_used_   = -1;
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
  AffToL1aValueMax_= 160;
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
// CFEB-TMB clock phases
//----------------------------------------------
void ChamberUtilities::CFEBTiming(){
  //
  // default with no argument is "normal_scan"
  //
  return CFEBTiming_with_Posnegs(normal_scan);
}
//
void ChamberUtilities::CFEBTiming(CFEBTiming_scanType scanType) {
  //
  return CFEBTiming_with_Posnegs(scanType);
}
//
void ChamberUtilities::CFEBTiming_with_Posnegs(CFEBTiming_scanType scanType) {
  //
  // This code automatically handles the transition into and out of the 
  // "special region," i.e., where the CFEB-TMB communication encounters
  // a bx shift, documented at http://cmsonline.cern.ch/cms-elog/470405
  //
  // To handle this transition, we assume that the values on the userPROM are 
  // the BEFORE values, and the values in the xml file and measured with this 
  // code are the AFTER values 
  //
  // In addition, this code will handle the following cases:
  //  - ME1/1a (CFEBTiming_scanType=me11a_scan) by looking at CFEB4 only
  //  - ME1/1b (CFEBTiming_scanType=me11b_scan) by looking at CFEB0-3 only
  //  - all others (CFEBTiming_scanType=normal_scan) by looking at all 5 CFEB's
  //
  if (debug_) {
    std::cout << "**************************************" << std::endl;
    std::cout << "    Find cfeb[0-4]delay with Posnegs:" << std::endl;
    std::cout << "**************************************" << std::endl;
  }
  (*MyOutput_) << "*************************************" << std::endl;
  (*MyOutput_) << "    Find cfeb[0-4]delay with Posnegs:" << std::endl;
  (*MyOutput_) << "*************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  thisDMB->RedirectOutput(&std::cout);
  thisCCB_->RedirectOutput(&std::cout);
  thisMPC->RedirectOutput(&std::cout);
  //
  // Get initial values, so we can go back to them at the end if we want....  
  // -> Hard Reset to put all the values from the userPROM onto the TMB
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  thisCCB_->hardReset(); 
  thisCCB_->setCCBMode(CCB::DLOG);
  //
  thisTMB->ReadRegister(vme_ddd1_adr);
  thisTMB->ReadRegister(vme_ddd2_adr);
  int initial_cfeb_tof_delay[5] = {thisTMB->GetReadCfeb0TOFDelay(),
				   thisTMB->GetReadCfeb1TOFDelay(),
				   thisTMB->GetReadCfeb2TOFDelay(),
				   thisTMB->GetReadCfeb3TOFDelay(),
				   thisTMB->GetReadCfeb4TOFDelay()};
  //
  thisTMB->ReadRegister(phaser_cfeb0_rxd_adr);
  thisTMB->ReadRegister(phaser_cfeb1_rxd_adr);
  thisTMB->ReadRegister(phaser_cfeb2_rxd_adr);
  thisTMB->ReadRegister(phaser_cfeb3_rxd_adr);
  thisTMB->ReadRegister(phaser_cfeb4_rxd_adr);
  int initial_cfeb_phase[5] = { thisTMB->GetReadCfeb0RxClockDelay(),
			        thisTMB->GetReadCfeb1RxClockDelay(),
			        thisTMB->GetReadCfeb2RxClockDelay(),
			        thisTMB->GetReadCfeb3RxClockDelay(),
			        thisTMB->GetReadCfeb4RxClockDelay() };
  int initial_cfeb_posneg[5]= { thisTMB->GetReadCfeb0RxPosNeg(),
			        thisTMB->GetReadCfeb1RxPosNeg(),
			        thisTMB->GetReadCfeb2RxPosNeg(),
			        thisTMB->GetReadCfeb3RxPosNeg(),
			        thisTMB->GetReadCfeb4RxPosNeg() };
  //
  thisTMB->ReadRegister(seq_trig_en_adr);
  int initial_clct_pretrig_enable           = thisTMB->GetReadClctPatternTrigEnable();    //0x68
  //
  thisTMB->ReadRegister(tmb_trig_adr);
  int initial_clct_trig_enable              = thisTMB->GetReadTmbAllowClct();             //0x86
  //
  thisTMB->ReadRegister(seq_clct_adr);
  int initial_clct_halfstrip_pretrig_thresh = thisTMB->GetReadHsPretrigThresh();          //0x70
  int initial_clct_pattern_thresh           = thisTMB->GetReadMinHitsPattern();           //0x70
  //
  thisTMB->ReadRegister(layer_trg_mode_adr);
  int initial_layer_trig_enable             = thisTMB->GetReadEnableLayerTrigger();       //0xf0
  //
  thisTMB->ReadRegister(ccb_trig_adr);
  int initial_ignore_ccb_startstop          = thisTMB->GetReadIgnoreCcbStartStop();       //0x2c
  //
  // Set up for this test...
  thisTMB->SetClctPatternTrigEnable(1);
  thisTMB->WriteRegister(seq_trig_en_adr);
  //
  thisTMB->SetTmbAllowClct(1);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetHsPretrigThresh(5);
  thisTMB->SetMinHitsPattern(5);
  thisTMB->WriteRegister(seq_clct_adr);
  //
  thisTMB->SetIgnoreCcbStartStop(0);
  thisTMB->WriteRegister(ccb_trig_adr);
  //
  thisTMB->SetEnableLayerTrigger(0);
  thisTMB->WriteRegister(layer_trg_mode_adr);
  //
  int test_cfeb_tof_delay[5] = { thisTMB->GetCfeb0TOFDelay(),
				 thisTMB->GetCfeb1TOFDelay(),
				 thisTMB->GetCfeb2TOFDelay(),
				 thisTMB->GetCfeb3TOFDelay(),
				 thisTMB->GetCfeb4TOFDelay() };
  //
  for (int i=0; i<5; i++) {
    if (initial_cfeb_tof_delay[i] != test_cfeb_tof_delay[i]) {
      (*MyOutput_) << "WARNING: value in userPROM cfeb" << i << "_tof_delay = " << initial_cfeb_tof_delay[i] 
		   << " is not the same as the test value = " << test_cfeb_tof_delay[i] 
		   << std::endl;
      std::cout    << "WARNING: value in userPROM cfeb" << i << "_tof_delay = " << initial_cfeb_tof_delay[i] 
		   << " is not the same as the test value = " << test_cfeb_tof_delay[i] 
		   << std::endl;
    }      
  }
  //
  thisTMB->SetCfeb0TOFDelay(test_cfeb_tof_delay[0]);
  thisTMB->SetCfeb1TOFDelay(test_cfeb_tof_delay[1]);
  thisTMB->SetCfeb2TOFDelay(test_cfeb_tof_delay[2]);
  thisTMB->SetCfeb3TOFDelay(test_cfeb_tof_delay[3]);
  thisTMB->SetCfeb4TOFDelay(test_cfeb_tof_delay[4]);
  thisTMB->WriteRegister(vme_ddd1_adr);
  thisTMB->WriteRegister(vme_ddd2_adr);
  thisTMB->FireDDDStateMachine();
  //
  comparing_with_clct_ = true;
  thisTMB->StartTTC();
  ::sleep(1);
  //
  //
  // Run the scan over all CFEB's regardless of scanType...
  const int MaxTimeDelay=25;
  //
  int Muons[5][2][MaxTimeDelay] = {};
  int NoMuons[5][2][MaxTimeDelay] = {};
  //
  for (int posneg=0; posneg<2; posneg++) {
    //
    // set the same value of posneg for all CFEB's...
    thisTMB->SetCfeb0RxPosNeg(posneg);
    thisTMB->SetCfeb1RxPosNeg(posneg);
    thisTMB->SetCfeb2RxPosNeg(posneg);
    thisTMB->SetCfeb3RxPosNeg(posneg);
    thisTMB->SetCfeb4RxPosNeg(posneg);
    //
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){
      //
      if (debug_) std::cout << "Next event:  posneg=" << std::dec << posneg << ", TimeDelay=" << TimeDelay << std::endl;
      //
      thisTMB->SetCfeb0RxClockDelay(TimeDelay);
      thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
      thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
      //
      thisTMB->SetCfeb1RxClockDelay(TimeDelay);
      thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
      thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
      //
      thisTMB->SetCfeb2RxClockDelay(TimeDelay);
      thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
      thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
      //
      thisTMB->SetCfeb3RxClockDelay(TimeDelay);
      thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
      thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
      //
      thisTMB->SetCfeb4RxClockDelay(TimeDelay);
      thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
      thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
      //
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
	    random_halfstrip[List] = (int) (rand()/(RAND_MAX+0.01)*10);  // random number between 0 and 10
	  	    random_halfstrip[List] += 13;                                 // translate to between 13 and 23
		    //  	    random_halfstrip[List] = 10;                                 // translate to between 13 and 23
	  }
	  //
	  // Now need to map the CFEB numbering into the ME1/1 numbering
	  //
	  int pulsed_halfstrip = -1;
	  //
	  if ( thisTMB->GetTMBFirmwareCompileType() == 0xa ) {   // normal TMB compile type
	    //
	    me11_pulsing_ = 0;
	    //
	    pulsed_halfstrip = List*32 + random_halfstrip[List];
	    //
	  } else if ( thisTMB->GetTMBFirmwareCompileType() == 0xc ) {  // plus endcap ME1/1
	    //
	    me11_pulsing_ = 1;
	    //
	    pulsed_halfstrip = List*32 + random_halfstrip[List];
	    if (List == 4) 
	      pulsed_halfstrip = 159 - random_halfstrip[List];
	    //
	  } else if ( thisTMB->GetTMBFirmwareCompileType() == 0xd ) {  // minus endcap ME1/1
	    //
	    me11_pulsing_ = 1;
	    //
	    pulsed_halfstrip = 127 - (List*32 + random_halfstrip[List]);
	    if (List == 4)
	      pulsed_halfstrip = 128 + random_halfstrip[List];
	  }
	  //
	  PulseCFEB(random_halfstrip[List],CLCTInputList[List]);	
	  //
	  usleep(50);
	  //
	  if (debug_) {
	    std::cout << "CFEB pulsed:" << List      
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
	    std::cout << "TMB:   clct0patternId=" << std::dec << clct0patternId
		      << ", clct0hstp=" << clct0keyHalfStrip      
		      << ", nHits =" << clct0nhit;
	  }
	  //
	  //	int clct1patternId    = thisTMB->GetCLCT1PatternId();
	  //	int clct1nhit         = thisTMB->GetCLCT1Nhit();
	  //	int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	  //
	  if ( clct0patternId <= 3 && clct0keyHalfStrip == pulsed_halfstrip && clct0nhit == 6 ) {
	    Muons[List][posneg][TimeDelay]++;
	    if (debug_) std::cout << " found" << std::endl;
	    last_pulsed_halfstrip[List] = random_halfstrip[List];
	  } else {
	    NoMuons[List][posneg][TimeDelay]+=10;
	    if (debug_) std::cout << " NOT found" << std::endl;
	  }
	  //	if ( clct1nhit == 6 && clct1keyHalfStrip == 16 && clct1cfeb == List ) 
	  //	  Muons[clct1cfeb][TimeDelay]++;
	} // for (CFEBList)
      }  // for (NMuons)
    }  //for (TimeDelay)
  }  //for (posneg)
  //
  //
  // Find the good window for each CFEB and each posneg...
  //
  int windows_per_cfeb[5][2] = {};
  int total_number_of_windows[2] = {};
  //
  // 1) figure out how many "good windows" each CFEB has for each posneg
  for (int posneg=0; posneg<2; posneg++) {
    for (int CFEBs=0; CFEBs<5; CFEBs++) {
      window_analysis(NoMuons[CFEBs][posneg],MaxTimeDelay);
      windows_per_cfeb[CFEBs][posneg] += window_counter + 1;
      total_number_of_windows[posneg] += window_counter + 1;
    }
  }
  //
  // 2) determine the average value for each CFEB for each posneg
  int TimeDelay;
  //
  int MuonsWork[5][2][2*MaxTimeDelay] = {};
  //
  // Build-up the data including wrap-around
  for (int posneg=0; posneg<2; posneg++) {
    for (int CFEBs=0; CFEBs<5; CFEBs++) {
      for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
	MuonsWork[CFEBs][posneg][TimeDelay] = Muons[CFEBs][posneg][TimeDelay] ;
      }
    }
  }
  //
  for (int posneg=0; posneg<2; posneg++) {
    for (int CFEBs=0; CFEBs<5; CFEBs++) {
      TimeDelay=0;
      while (Muons[CFEBs][posneg][TimeDelay]>0) {
	MuonsWork[CFEBs][posneg][TimeDelay+MaxTimeDelay] = Muons[CFEBs][posneg][TimeDelay] ;
	MuonsWork[CFEBs][posneg][TimeDelay] = 0 ;
	TimeDelay++;
      }
    }
  }
  //
  if (debug_ >= 5) {
    for (int posneg=0; posneg<2; posneg++) {    
      std::cout << "TimeDelay Fixed for Delay Wrapping, posneg = " << posneg << std::endl ;
      std::cout << "TimeDelay " ;
      for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) 
	std::cout << std::setw(4) << TimeDelay ;
      std::cout << std::endl;
      for (int CFEBs=0; CFEBs<5; CFEBs++) {
	std::cout << "CFEB Id=" << CFEBs << " " ;
	for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
	  std::cout << std::setw(4) << MuonsWork[CFEBs][posneg][TimeDelay] ;
	}     
	std::cout << std::endl ;
      }   
      std::cout << std::endl ;
    }
  }
  //
  float CFEBMeanN[5][2], CFEBMean[5][2];
  for( int j=0; j<2; j++) {
    for( int i=0; i<5; i++) {
      CFEBMean[i][j]  = 0 ;
      CFEBMeanN[i][j] = 0 ;
    }
  }
  for (int posneg=0; posneg<2; posneg++) {    
    for (int CFEBs=0; CFEBs<5; CFEBs++) {
      for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
	if ( MuonsWork[CFEBs][posneg][TimeDelay] > 0  ) {
	  CFEBMean[CFEBs][posneg]  += ((float) TimeDelay) * ((float) MuonsWork[CFEBs][posneg][TimeDelay])  ; 
	  CFEBMeanN[CFEBs][posneg] += (float) MuonsWork[CFEBs][posneg][TimeDelay] ; 
	}
      }     
    }   
  }
  //
  for (int posneg=0; posneg<2; posneg++) {    
    for( int CFEBs=0; CFEBs<5; CFEBs++) {
      if (CFEBMeanN[CFEBs][posneg] > 0) {
	CFEBMean[CFEBs][posneg] /= CFEBMeanN[CFEBs][posneg] ;
      } else {
	CFEBMean[CFEBs][posneg] = -999;
      }
      if (CFEBMean[CFEBs][posneg] > (MaxTimeDelay-1) ) 
	CFEBMean[CFEBs][posneg] -= MaxTimeDelay ;
      if (debug_ >= 5) std::cout << "posneg " << posneg << ", CFEB" << CFEBs << " mean delay = " << CFEBMean[CFEBs][posneg] ;
    }
    if (debug_ >= 5) std::cout << std::endl;
  }
  //
  // Select the appropriate posneg/rx combinations.
  // The CFEBs we care about depend on which scanType we have...
  //
  // here are the values for "normal_scan"
  int min_cfeb = 0;
  int max_cfeb = 5;
  int optimal_number_of_good_scans = 5;
  //
  if (scanType == ChamberUtilities::me11a_scan) {
    min_cfeb = 4;
    max_cfeb = 5; 
    optimal_number_of_good_scans = 1;
  } else if (scanType == ChamberUtilities::me11b_scan) {
    min_cfeb = 0;
    max_cfeb = 4;
    optimal_number_of_good_scans = 4;
  }
  //
  // pick the posneg value which has 1 window per CFEB
  //
  int good_scan[2] = {};
  int bad_cfeb[2] = {};
  //
  for (int posneg=0; posneg<2; posneg++) {
    //
    bad_cfeb[posneg] = -1;
    //
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      if (windows_per_cfeb[CFEBs][posneg] == 1) {  // if there is one window in this CFEB, it is a good scan for this CFEB
	good_scan[posneg]++;                  // count the number of CFEBs which have a good scan
      } else {
	bad_cfeb[posneg] = CFEBs;             // if there is a bad one, tag it (if there are multiple bad, it will not be selected) 
      }
    }
  }
  //
  int pick_posneg = -1;
  //
  //perfect:  one good window in each CFEB           (posneg=0 or 1, bad_cfeb[posneg]=-1)
  //OK:       one good window in all CFEB's except 1 (posneg=0 or 1, bad_cfeb[posneg]!=-1)
  //bad:      failed scan on > 1 CFEB [1 CFEB on ME11a] (posneg<0)
  //
  for (int posneg=0; posneg<2; posneg++) {
    if (good_scan[posneg] == optimal_number_of_good_scans) {  
      pick_posneg = posneg;
      break;
    } else if ( ( scanType == ChamberUtilities::me11b_scan || scanType == ChamberUtilities::normal_scan ) &&
		(good_scan[posneg] == optimal_number_of_good_scans-1) ){  
      pick_posneg = posneg;
    } 
  }
  //
  (*MyOutput_) << "CFEB TOF delay values for this scan..." << std::endl;
  (*MyOutput_) << " cfeb0_tof_delay = " << std::dec << test_cfeb_tof_delay[0] << std::endl;
  (*MyOutput_) << " cfeb1_tof_delay = " << std::dec << test_cfeb_tof_delay[1] << std::endl;
  (*MyOutput_) << " cfeb2_tof_delay = " << std::dec << test_cfeb_tof_delay[2] << std::endl;
  (*MyOutput_) << " cfeb3_tof_delay = " << std::dec << test_cfeb_tof_delay[3] << std::endl;
  (*MyOutput_) << " cfeb4_tof_delay = " << std::dec << test_cfeb_tof_delay[4] << std::endl;
  //
  // Print number of muons found versus cfeb[0-4]delay for each CFEB for each posneg
  //
  for (int posneg=0; posneg<2; posneg++) {
    (*MyOutput_) << "posneg = " << std::dec << posneg << " has " 
		 << good_scan[posneg] << " CFEBs with one window..." << std::endl;
    std::cout    << "posneg = " << std::dec << posneg << " has " 
		 << good_scan[posneg] << " CFEBs with one window..." << std::endl;
    //
    (*MyOutput_) << "TimeDelay " ;
    std::cout << "TimeDelay " ;
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) {
      (*MyOutput_) << std::setw(4) << TimeDelay ;
      std::cout << std::setw(4) << TimeDelay ;
    }
    (*MyOutput_) << std::endl ;
    std::cout    << std::endl ;
    for (int CFEBs=0; CFEBs<5; CFEBs++) {
      (*MyOutput_) << "CFEB Id=" << CFEBs << " " ;
      std::cout    << "CFEB Id=" << CFEBs << " " ;
      for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
	(*MyOutput_) << std::setw(4) << Muons[CFEBs][posneg][TimeDelay] ;
	std::cout    << std::setw(4) << Muons[CFEBs][posneg][TimeDelay] ;
      }     
      (*MyOutput_) << std::endl ;
      std::cout    << std::endl ;
    }   
    (*MyOutput_) << std::endl ;
    std::cout    << std::endl ;
  }
  //
  if (pick_posneg < 0) {          //bad scan
    //
    (*MyOutput_) << "Not enough good windows for these CFEBs" << std::endl;
    std::cout    << "Not enough good windows for these CFEBs" << std::endl;
    //
  } else if (bad_cfeb[pick_posneg] < 0) {  //OK scan on all CFEB for this posneg
    //
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      CFEBrxPhase_[CFEBs] = RoundOff(CFEBMean[CFEBs][pick_posneg]);
      CFEBrxPosneg_[CFEBs] = pick_posneg;
      (*MyOutput_) << "Best value is cfeb" << CFEBs << "delay = " << CFEBrxPhase_[CFEBs] << std::endl;
      std::cout    << "Best value is cfeb" << CFEBs << "delay = " << CFEBrxPhase_[CFEBs] << std::endl;
    }
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      (*MyOutput_) << "Best value is cfeb" << CFEBs << "posneg = " << CFEBrxPosneg_[CFEBs] << std::endl;
      std::cout    << "Best value is cfeb" << CFEBs << "posneg = " << CFEBrxPosneg_[CFEBs] << std::endl;
    }
  } else {                        //OK scan on 4 out of 5 CFEBs put the value of the 1 to be equal to the rest... 
    //
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      CFEBrxPhase_[CFEBs] = RoundOff(CFEBMean[CFEBs][pick_posneg]);
      CFEBrxPosneg_[CFEBs] = pick_posneg;
    }
    //
    int cfeb_next_to_failed = (bad_cfeb[pick_posneg] + 1) % max_cfeb;
    CFEBrxPhase_[bad_cfeb[pick_posneg]]  = CFEBrxPhase_[cfeb_next_to_failed];
    CFEBrxPosneg_[bad_cfeb[pick_posneg]] = CFEBrxPosneg_[cfeb_next_to_failed];
    //
    (*MyOutput_) << "Scan failed on CFEB " << bad_cfeb[pick_posneg] << ".  Selecting values for this CFEB from CFEB " << cfeb_next_to_failed << std::endl;
    std::cout    << "Scan failed on CFEB " << bad_cfeb[pick_posneg] << ".  Selecting values for this CFEB from CFEB " << cfeb_next_to_failed << std::endl;
    //
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      (*MyOutput_) << "Best value is cfeb" << CFEBs << "delay = " << CFEBrxPhase_[CFEBs] << std::endl;
      std::cout    << "Best value is cfeb" << CFEBs << "delay = " << CFEBrxPhase_[CFEBs] << std::endl;
    }
    for (int CFEBs=min_cfeb; CFEBs<max_cfeb; CFEBs++) {
      (*MyOutput_) << "Best value is cfeb" << CFEBs << "posneg = " << CFEBrxPosneg_[CFEBs] << std::endl;
      std::cout    << "Best value is cfeb" << CFEBs << "posneg = " << CFEBrxPosneg_[CFEBs] << std::endl;
    }
  }
  //
  // Now we have set CFEB TOF, and have measured RX and POSNEG.  Let's see if
  // This combination of parameters is in the "special region"...
  //
  (*MyOutput_) << "Using values before scan... " << std::endl;
  std::cout    << "Using values before scan... " << std::endl;
  bool initial_in_special_region = 
    inSpecialRegion(initial_cfeb_tof_delay[0],initial_cfeb_tof_delay[1],initial_cfeb_tof_delay[2],initial_cfeb_tof_delay[3],initial_cfeb_tof_delay[4],
		    initial_cfeb_phase[0]    ,initial_cfeb_phase[1]    ,initial_cfeb_phase[2]    ,initial_cfeb_phase[3]    ,initial_cfeb_phase[4]    ,
		    initial_cfeb_posneg[0]   ,initial_cfeb_posneg[1]   ,initial_cfeb_posneg[2]   ,initial_cfeb_posneg[3]   ,initial_cfeb_posneg[4]   ,
		    scanType);

  //
  (*MyOutput_) << "Using values after scan... " << std::endl;
  std::cout    << "Using values after scan... " << std::endl;
  bool test_in_special_region = 
    inSpecialRegion(test_cfeb_tof_delay[0],test_cfeb_tof_delay[1],test_cfeb_tof_delay[2],test_cfeb_tof_delay[3],test_cfeb_tof_delay[4],
		    CFEBrxPhase_[0]       ,CFEBrxPhase_[1]       ,CFEBrxPhase_[2]       ,CFEBrxPhase_[3]       ,CFEBrxPhase_[4]       ,
		    CFEBrxPosneg_[0]      ,CFEBrxPosneg_[1]      ,CFEBrxPosneg_[2]      ,CFEBrxPosneg_[3]      ,CFEBrxPosneg_[4]      ,
		    scanType);
  //
  int initial_cfeb_rxd_int_delay[5] = { thisTMB->GetCFEB0RxdIntDelay(),
					thisTMB->GetCFEB1RxdIntDelay(),
					thisTMB->GetCFEB2RxdIntDelay(),
					thisTMB->GetCFEB3RxdIntDelay(),
					thisTMB->GetCFEB4RxdIntDelay() };
  //
  if (initial_in_special_region == false && test_in_special_region == true) {

    (*MyOutput_) << "Special region transition:  OUT-->IN..." << std::endl;
    std::cout    << "Special region transition:  OUT-->IN..." << std::endl;

    for (int i=min_cfeb; i<max_cfeb; i++) {
      //
      cfeb_rxd_int_delay[i] = initial_cfeb_rxd_int_delay[i] - 1;
      //
      (*MyOutput_) << "... cfebN_rxd_int_delay goes from " << initial_cfeb_rxd_int_delay[i]
		   << " -> " << cfeb_rxd_int_delay[i] << std::endl;
      std::cout    << "... cfebN_rxd_int_delay goes from " << initial_cfeb_rxd_int_delay[i]
		   << " -> " << cfeb_rxd_int_delay[i] << std::endl;
      //
      if (cfeb_rxd_int_delay[i] < 0 ) {
  	cfeb_rxd_int_delay[i] = -999;
  	(*MyOutput_) << "WARNING:  cfeb_rxd_int_delay is < 0..." << std::endl;
  	std::cout    << "WARNING:  cfeb_rxd_int_delay is < 0..." << std::endl;
      }
    }
  } else if (initial_in_special_region == true && test_in_special_region == false) {

    (*MyOutput_) << "Special region transition:  IN-->OUT..." << std::endl;
    std::cout    << "Special region transition:  IN-->OUT..." << std::endl;

    for (int i=min_cfeb; i<max_cfeb; i++) {
      //
      cfeb_rxd_int_delay[i] = initial_cfeb_rxd_int_delay[i] + 1;
      //
      (*MyOutput_) << "... cfebN_rxd_int_delay goes from " << initial_cfeb_rxd_int_delay[i]
		   << " -> " << cfeb_rxd_int_delay[i] << std::endl;
      std::cout    << "... cfebN_rxd_int_delay goes from " << initial_cfeb_rxd_int_delay[i]
		   << " -> " << cfeb_rxd_int_delay[i] << std::endl;
    }
  }
  //
  // return to initial values...
  //
  me11_pulsing_ = 0;
  //
  thisCCB_->setCCBMode(CCB::DLOG);
  //
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
    //
    (*MyOutput_) << "Setting cfeb[0-4]delay phases to measured values..." << std::endl;
    //
    thisTMB->SetCfeb0RxClockDelay(CFEBrxPhase_[0]);
    thisTMB->SetCfeb0RxPosNeg(CFEBrxPosneg_[0]);
    thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
    //
    thisTMB->SetCfeb1RxClockDelay(CFEBrxPhase_[1]);
    thisTMB->SetCfeb1RxPosNeg(CFEBrxPosneg_[1]);
    thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
    //
    thisTMB->SetCfeb2RxClockDelay(CFEBrxPhase_[2]);
    thisTMB->SetCfeb2RxPosNeg(CFEBrxPosneg_[2]);
    thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
    //
    thisTMB->SetCfeb3RxClockDelay(CFEBrxPhase_[3]);
    thisTMB->SetCfeb3RxPosNeg(CFEBrxPosneg_[3]);
    thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
    //
    thisTMB->SetCfeb4RxClockDelay(CFEBrxPhase_[4]);
    thisTMB->SetCfeb4RxPosNeg(CFEBrxPosneg_[4]);
    thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
    //
  } else {
    //
    (*MyOutput_) << "Reverting back to original cfeb[0-4]delay phase values..." << std::endl;
    //
    thisTMB->SetCfeb0RxClockDelay(initial_cfeb_phase[0]);
    thisTMB->SetCfeb0RxPosNeg(initial_cfeb_posneg[0]);
    thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
    //
    thisTMB->SetCfeb1RxClockDelay(initial_cfeb_phase[1]);
    thisTMB->SetCfeb1RxPosNeg(initial_cfeb_posneg[1]);
    thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
    //
    thisTMB->SetCfeb2RxClockDelay(initial_cfeb_phase[2]);
    thisTMB->SetCfeb2RxPosNeg(initial_cfeb_posneg[2]);
    thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
    //
    thisTMB->SetCfeb3RxClockDelay(initial_cfeb_phase[3]);
    thisTMB->SetCfeb3RxPosNeg(initial_cfeb_posneg[3]);
    thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
    //
    thisTMB->SetCfeb4RxClockDelay(initial_cfeb_phase[4]);
    thisTMB->SetCfeb4RxPosNeg(initial_cfeb_posneg[4]);
    thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
    //
  }
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
bool ChamberUtilities::inSpecialRegion(int cfeb0tof,int cfeb1tof,int cfeb2tof,int cfeb3tof,int cfeb4tof,
				       int cfeb0rx ,int cfeb1rx ,int cfeb2rx ,int cfeb3rx ,int cfeb4rx ,
				       int cfeb0pn ,int cfeb1pn ,int cfeb2pn ,int cfeb3pn ,int cfeb4pn ,
				       CFEBTiming_scanType scanType) {
  //
  // algorithm to determine if this TMB<->CFEB set is in the special region.  
  // We assume we are using the same value of posneg for all CFEB's 
  // We assume we are using the same value of TOF for all CFEB's 
  // ........ except that ME11a and ME11b can have different posneg and TOF.......
  //
  const int MaxTimeDelay=25;
  //
  float average_rx = -1.;
  if (scanType == ChamberUtilities::normal_scan) {
    average_rx = determine_average_with_wraparound(cfeb0rx,cfeb1rx,cfeb2rx,cfeb3rx,cfeb4rx,MaxTimeDelay);
  } else if (scanType == ChamberUtilities::me11b_scan) {
    average_rx = determine_average_with_wraparound(cfeb0rx,cfeb1rx,cfeb2rx,cfeb3rx,MaxTimeDelay);
  } else if (scanType == ChamberUtilities::me11a_scan) {
    average_rx = cfeb4rx;
  }
  //
  // determined signed_rx with the following algorithm 

  if (scanType == ChamberUtilities::me11a_scan || scanType == ChamberUtilities::normal_scan) {
    if (cfeb4pn == 0) {
      average_rx = -average_rx;
    }
  } else if (scanType == ChamberUtilities::me11b_scan) {
    if (cfeb0pn == 0) {
      average_rx = -average_rx;
    }
  }
  //
  bool in_special_region = false;
  //
  float TOF = -999.;
  if (scanType == ChamberUtilities::me11a_scan || scanType == ChamberUtilities::normal_scan) {
    TOF = (float) cfeb4tof;
  } else if (scanType == ChamberUtilities::me11b_scan) {
    TOF = (float) cfeb0tof;
  }
  //
  float function_value = special_region_function(average_rx);
  //
  if (TOF > function_value) {
    in_special_region = true;
    (*MyOutput_) << "--> Function evaluated at " << average_rx << " = " << function_value 
		 << " is less than TOF (" << TOF 
		 << ") ==> in the special region" << std::endl;

    std::cout    << "--> Function evaluated at " << average_rx << " = " << function_value 
		 << " is less than TOF (" << TOF 
		 << ") ==> in the special region" << std::endl;
  } else {
    (*MyOutput_) << "--> Function evaluated at " << average_rx << " = " << function_value 
		 << " is more than TOF (" << TOF 
		 << ") ==> NOT in the special region" << std::endl;

    std::cout    << "--> Function evaluated at " << average_rx << " = " << function_value 
		 << " is more than TOF (" << TOF 
		 << ") ==> NOT in the special region" << std::endl;
  }
  return in_special_region;
}
//
float ChamberUtilities::determine_average_with_wraparound(int val1, 
							  int val2, 
							  int val3, 
							  int val4, 
							  int val5, 
							  const int max_value ) {
  int vector_of_vals[50] = {};

  // fill the vector with the values we want to average
  for (int i=0; i<max_value; i++) {
    if (i==val1) vector_of_vals[i] += 1;
    if (i==val2) vector_of_vals[i] += 1;
    if (i==val3) vector_of_vals[i] += 1;
    if (i==val4) vector_of_vals[i] += 1;
    if (i==val5) vector_of_vals[i] += 1;
  }
  
  // if a value is near the wrap-around edge (i.e., 0), correct it
  int i=0;
  while (vector_of_vals[i]>0) {
    vector_of_vals[i+max_value]=vector_of_vals[i];
    vector_of_vals[i] = 0;
    i++;
  }
  //
  // determine the mean index value...
  float MeanN = 0.;
  float Mean = 0.;
  //
  for (int index=0; index<2*max_value; index++){ 
    if ( vector_of_vals[index] > 0  ) {
      Mean += ((float) index) * ((float) vector_of_vals[index])  ; 
      MeanN += (float) vector_of_vals[index] ; 
    }
  }     
  //
  Mean /= MeanN;
  //
  float max_to_compare = (float) max_value - 0.1;

  if (Mean > max_to_compare)  
    Mean -= max_value ;
  // 
  return Mean;
}
//
float ChamberUtilities::determine_average_with_wraparound(int val1, 
							  int val2, 
							  int val3, 
							  int val4, 
							  const int max_value ) {
  int vector_of_vals[50] = {};

  // fill the vector with the values we want to average
  for (int i=0; i<max_value; i++) {
    if (i==val1) vector_of_vals[i] += 1;
    if (i==val2) vector_of_vals[i] += 1;
    if (i==val3) vector_of_vals[i] += 1;
    if (i==val4) vector_of_vals[i] += 1;
  }
  
  // if a value is near the wrap-around edge (i.e., 0), correct it
  int i=0;
  while (vector_of_vals[i]>0) {
    vector_of_vals[i+max_value]=vector_of_vals[i];
    vector_of_vals[i] = 0;
    i++;
  }
  //
  // determine the mean index value...
  float MeanN = 0.;
  float Mean = 0.;
  //
  for (int index=0; index<2*max_value; index++){ 
    if ( vector_of_vals[index] > 0  ) {
      Mean += ((float) index) * ((float) vector_of_vals[index])  ; 
      MeanN += (float) vector_of_vals[index] ; 
    }
  }     
  //
  Mean /= MeanN;
  //
  float max_to_compare = (float) max_value - 0.1;

  if (Mean > max_to_compare)  
    Mean -= max_value ;
  // 
  return Mean;
}
//
float ChamberUtilities::special_region_function(float signed_rx) {
  //
  float f = 99999.;
  //
  if (signed_rx > -25. && signed_rx < 2.) {
    f = -0.43 * signed_rx + 5.;
    //
  } else if (signed_rx >= 2. && signed_rx <= 13.) {
    f = 7.;
    //
  } else if (signed_rx > 13. && signed_rx < 25. ) {
    f = 0.4 * signed_rx -5.5;
    //
  }
  //
  return f;
}
//
void ChamberUtilities::CFEBTiming_without_Posnegs(){
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
  int initial_cfeb0_phase = thisTMB->GetCfeb0RxClockDelay();                          //0x112
  int initial_cfeb1_phase = thisTMB->GetCfeb1RxClockDelay();                          //0x114
  int initial_cfeb2_phase = thisTMB->GetCfeb2RxClockDelay();                          //0x116
  int initial_cfeb3_phase = thisTMB->GetCfeb3RxClockDelay();                          //0x118
  int initial_cfeb4_phase = thisTMB->GetCfeb4RxClockDelay();                          //0x11a
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
  int MaxTimeDelay=25;
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
    thisTMB->SetCfeb0RxClockDelay(TimeDelay);
    thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
    //
    thisTMB->SetCfeb1RxClockDelay(TimeDelay);
    thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
    //
    thisTMB->SetCfeb2RxClockDelay(TimeDelay);
    thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
    //
    thisTMB->SetCfeb3RxClockDelay(TimeDelay);
    thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
    //
    thisTMB->SetCfeb4RxClockDelay(TimeDelay);
    thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
    //
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
      MuonsWork[CFEBs][TimeDelay+MaxTimeDelay] = Muons[CFEBs][TimeDelay] ;
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
    if (CFEBMean[CFEBs] > (MaxTimeDelay-1) ) 
      CFEBMean[CFEBs] -= MaxTimeDelay ;
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
    //
    (*MyOutput_) << "Setting cfeb[0-4]delay phases to measured values..." << std::endl;
    //
    thisTMB->SetCfeb0RxClockDelay(CFEBrxPhase_[0]);
    thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
    //
    thisTMB->SetCfeb1RxClockDelay(CFEBrxPhase_[1]);
    thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
    //
    thisTMB->SetCfeb2RxClockDelay(CFEBrxPhase_[2]);
    thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
    //
    thisTMB->SetCfeb3RxClockDelay(CFEBrxPhase_[3]);
    thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
    //
    thisTMB->SetCfeb4RxClockDelay(CFEBrxPhase_[4]);
    thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
    //
    //    thisTMB->SetCFEB0delay(CFEBrxPhase_[0]);
    //    thisTMB->SetCFEB1delay(CFEBrxPhase_[1]);
    //    thisTMB->SetCFEB2delay(CFEBrxPhase_[2]);
    //    thisTMB->SetCFEB3delay(CFEBrxPhase_[3]);
    //    thisTMB->SetCFEB4delay(CFEBrxPhase_[4]);
    //
  } else {
    //
    (*MyOutput_) << "Reverting back to original cfeb[0-4]delay phase values..." << std::endl;
    //
    thisTMB->SetCfeb0RxClockDelay(initial_cfeb0_phase);
    thisTMB->WriteRegister(phaser_cfeb0_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb0_rxd_adr);
    //
    thisTMB->SetCfeb1RxClockDelay(initial_cfeb1_phase);
    thisTMB->WriteRegister(phaser_cfeb1_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb1_rxd_adr);
    //
    thisTMB->SetCfeb2RxClockDelay(initial_cfeb2_phase);
    thisTMB->WriteRegister(phaser_cfeb2_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb2_rxd_adr);
    //
    thisTMB->SetCfeb3RxClockDelay(initial_cfeb3_phase);
    thisTMB->WriteRegister(phaser_cfeb3_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb3_rxd_adr);
    //
    thisTMB->SetCfeb4RxClockDelay(initial_cfeb4_phase);
    thisTMB->WriteRegister(phaser_cfeb4_rxd_adr);
    thisTMB->FirePhaser(phaser_cfeb4_rxd_adr);
    //
    //
    //    thisTMB->SetCFEB0delay(initial_cfeb0_phase);
    //    thisTMB->SetCFEB1delay(initial_cfeb1_phase);
    //    thisTMB->SetCFEB2delay(initial_cfeb2_phase);
    //    thisTMB->SetCFEB3delay(initial_cfeb3_phase);
    //    thisTMB->SetCFEB4delay(initial_cfeb4_phase);
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
//----------------------------------------------
// ALCT-TMB clock phases
//----------------------------------------------
void ChamberUtilities::ALCT_TMB_Loopback() {
  //
  bool initial_use_measured_values = use_measured_values_;
  PropagateMeasuredValues(true);
  //
  // Find a "good enough" value of the alct_rx_clock_delay with alternating 1's and 0's sent from ALCT -> TMB
  int alct_rx_value = Find_alct_rx_with_ALCT_to_TMB_evenodd(10);
  //
  ::usleep(500000);
  //
  // Find a "good enough" value of the alct_tx_clock_delay with alternating 1's and 0's 
  // sent from TMB, correctly latched at ALCT, and then looped back:  TMB -> ALCT -> TMB
  int alct_tx_value = Find_alct_tx_with_TMB_to_ALCT_evenodd(10);
  //
  if (alct_rx_value < 0 || alct_tx_value < 0) {
    PropagateMeasuredValues(initial_use_measured_values);
    return;
  }
  //
  ::usleep(500000);
  //
  // Now measure alct_[rx,tx]_phase_delay with random data...
  ALCT_TMB_TimingUsingRandomLoopback();
  //
  PropagateMeasuredValues(initial_use_measured_values);
  //
  return;
}
//
//
int ChamberUtilities::Find_alct_rx_with_ALCT_to_TMB_evenodd() {
  //
  // default 1000 passes for each rx, wire-pair
  //
  return Find_alct_rx_with_ALCT_to_TMB_evenodd(1000);
}
//
int ChamberUtilities::Find_alct_rx_with_ALCT_to_TMB_evenodd(int number_of_passes) {
  //
  // Find a "good enough" value of the alct_rx_clock_delay with alternating 1's and 0's 
  // sent from ALCT to TMB.  The trick here is that the SEND_EVENODD command sent to the 
  // ALCT sequencer can be interpreted by ALCT even though alct_tx_clock_delay may not 
  // be correct....
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
  int initial_alct_rx_phase      = thisTMB->GetAlctRxClockDelay();
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
  int alct_rx_bad[maximum_number_of_phase_delay_values][28] = {};
  int alct_sync_1st_err_ff[maximum_number_of_phase_delay_values] = {};
  int alct_sync_2nd_err_ff[maximum_number_of_phase_delay_values] = {};
  //
  // expected patterns:
  const int alct_1st_expect = 0xAAAAAAA;	// Teven
  const int alct_2nd_expect = 0x5555555;	// Todd 
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; delay_value++) {
      //
      // Step alct rx clock delay
      if (debug_>=10) std::cout << "Set alct_rx_clock_delay = " << delay_value << std::endl;
      thisTMB->SetAlctRxClockDelay(delay_value);
      thisTMB->WriteRegister(phaser_alct_rxd_adr);
      thisTMB->FirePhaser(phaser_alct_rxd_adr);
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
      alct_sync_1st_err_ff[delay_value] |= thisTMB->GetReadALCTSync1stErrorLatched();
      alct_sync_2nd_err_ff[delay_value] |= thisTMB->GetReadALCTSync2ndErrorLatched();

      if (ipass==0) {
	printf("Teven/Todd: delay_value=%2i ",delay_value);
	printf("rxdata_1st=%8.8X rxdata_2nd=%8.8X ",alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	printf("1st_err/latched=%1i/%1i 2nd_err=%1i/%1i\n",alct_sync_1st_err,alct_sync_1st_err_ff[delay_value],alct_sync_2nd_err,alct_sync_2nd_err_ff[delay_value]);
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
	if ((ibit_1st_expected !=  ibit_1st_received) || (ibit_2nd_expected !=  ibit_2nd_received)) alct_rx_bad[delay_value][ibit]++;
      }	
    }	
    if (debug_) 
      if (ipass%100==0) printf("\t%4i\r",number_of_passes-ipass);
  }
  //
  // sum up badness over all the cable pairs
  int nbad[maximum_number_of_phase_delay_values] = {};
  //
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; delay_value++) 
    for (int ibit=0; ibit<=27; ++ibit) 
      nbad[delay_value] += alct_rx_bad[delay_value][ibit];
  //
  (*MyOutput_) << "--> Summed over all data pairs: " << std::endl;
  std::cout    << "--> Summed over all data pairs: " << std::endl;
  (*MyOutput_) << " alct_rx    bad data count" << std::endl;
  std::cout    << " alct_rx    bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  std::cout << "---------   --------------" << std::endl;
  for (int delay_value = 0; delay_value <maximum_number_of_phase_delay_values; delay_value++) {
    (*MyOutput_) << "    " << std::dec << delay_value << "           " << std::dec << nbad[delay_value] <<std::endl;
    std::cout    << "    " << std::dec << delay_value << "           " << std::dec << nbad[delay_value] <<std::endl;
  }
  //
  ALCTrxPhase_ = window_analysis(nbad,maximum_number_of_phase_delay_values);
  //
  (*MyOutput_) << "alternating 1/0 value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl << std::endl;
  std::cout    << "alternating 1/0 value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl << std::endl;
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
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
    (*MyOutput_) << std::setw(5) << delay_value;	
    std::cout    << std::setw(5) << delay_value;	
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  //
  (*MyOutput_) << "pair      ";
  std::cout    << "pair      ";
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
    (*MyOutput_) << " ----";
    std::cout    << " ----";
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  
  for (int ibit=0; ibit<=27; ++ibit) {
    (*MyOutput_) << std::setw(3) << ibit << "       ";
    std::cout    << std::setw(3) << ibit << "       ";
    for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
      (*MyOutput_) << std::setw(5) << alct_rx_bad[delay_value][ibit];
      std::cout    << std::setw(5) << alct_rx_bad[delay_value][ibit];
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
    thisTMB->SetAlctRxClockDelay(ALCTrxPhase_);
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx_clock_delay value..." << std::endl;
    std::cout    << "Reverting back to original alct_rx_clock_delay value..." << std::endl;
    thisTMB->SetAlctRxClockDelay(initial_alct_rx_phase);
  }
  thisTMB->WriteRegister(phaser_alct_rxd_adr);
  thisTMB->FirePhaser(phaser_alct_rxd_adr);
  //
  return ALCTrxPhase_;
}
//
int ChamberUtilities::Find_alct_tx_with_TMB_to_ALCT_evenodd() {
  //
  // default 1000 passes for each rx, wire-pair
  //
  return Find_alct_tx_with_TMB_to_ALCT_evenodd(1000);
}
//
int ChamberUtilities::Find_alct_tx_with_TMB_to_ALCT_evenodd(int number_of_passes) {
  //
  // Find a "good enough" value of the alct_tx_clock_delay with alternating 1's and 0's 
  // sent from TMB, correctly latched at ALCT, and then looped back:  TMB -> ALCT -> TMB.
  // This is assuming that TMB correctly receives the data, and thus is checking the 
  // latching at the ALCT...
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
  int initial_alct_tx_phase      = thisTMB->GetAlctTxClockDelay();
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
  int alct_rx_bad[maximum_number_of_phase_delay_values][28] = {};
  int alct_sync_1st_err_ff[maximum_number_of_phase_delay_values] = {};
  int alct_sync_2nd_err_ff[maximum_number_of_phase_delay_values] = {};
  //
  // expected patterns:
  const int alct_1st_expect = 0xAA;	//1st-in-time: Teven = 10'b10 1010 1010
  const int alct_2nd_expect = 0x55;	//2nd-in-time: Todd  = 10'b01 0101 0101
  //
  // data to fill the TMB data banks:
  int data_to_loopback_1st[3] = {0,0,alct_1st_expect};   
  int data_to_loopback_2nd[3] = {0,0,alct_2nd_expect};   
  //
  // data which will be coming out of ALCT (if working correctly)
  int alct_1st_expect_fulldataword = alct_1st_expect << 20;
  int alct_2nd_expect_fulldataword = alct_2nd_expect << 20;
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; delay_value++) {
      //
      // Step alct tx clock delay
      thisTMB->SetAlctTxClockDelay(delay_value);
      thisTMB->WriteRegister(phaser_alct_txd_adr);
      thisTMB->FirePhaser(phaser_alct_txd_adr);
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
      alct_sync_1st_err_ff[delay_value] |= thisTMB->GetReadALCTSync1stErrorLatched();
      alct_sync_2nd_err_ff[delay_value] |= thisTMB->GetReadALCTSync2ndErrorLatched();

      if (ipass==0 && debug_) {
	printf("Teven/Todd: delay_value=%2i ",delay_value);
	printf("rxdata_1st=%8.8X rxdata_2nd=%8.8X ",alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	printf("expect_1st=%8.8X expect_2nd=%8.8X ",alct_sync_expect_1st,alct_sync_expect_2nd);
	printf("1st_err/latched=%1i/%1i 2nd_err=%1i/%1i\n",
	       alct_sync_1st_err,alct_sync_1st_err_ff[delay_value],
	       alct_sync_2nd_err,alct_sync_2nd_err_ff[delay_value]);
      }
      //
      // Compare received bits to expected pattern
      if (alct_sync_expect_1st != alct_1st_expect_fulldataword) {
	(*MyOutput_) << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
	std::cout    << "TMB internal error: 1st frame expected =" << std::hex << alct_1st_expect 
		     << " not equal 1st frame in firmware = " << alct_sync_expect_1st << std::endl;
      }
      //
      if (alct_sync_expect_2nd != alct_2nd_expect_fulldataword) {
	(*MyOutput_) << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
	std::cout    << "TMB internal error: 2nd frame expected =" << std::hex << alct_2nd_expect 
		     << " not equal 2nd frame in firmware = " << alct_sync_expect_2nd << std::endl;
      }
      //
      // Compare them bit-by-bit
      for (int ibit=0; ibit<=27; ibit++) {
	int ibit_1st_expected = (alct_1st_expect_fulldataword	>> ibit) & 0x1;
	int ibit_2nd_expected = (alct_2nd_expect_fulldataword	>> ibit) & 0x1;
	int ibit_1st_received = (alct_sync_rxdata_1st	>> ibit) & 0x1;
	int ibit_2nd_received = (alct_sync_rxdata_2nd	>> ibit) & 0x1;
	if ((ibit_1st_expected !=  ibit_1st_received) || (ibit_2nd_expected !=  ibit_2nd_received)) alct_rx_bad[delay_value][ibit]++;
      }	
    }	
    if (debug_) 
      if (ipass%100==0) printf("\t%4i\r",number_of_passes-ipass);
  }
  //
  // sum up badness over all the cable pairs
  int nbad[maximum_number_of_phase_delay_values] = {};
  //
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; delay_value++) 
    for (int ibit=0; ibit<=27; ++ibit) 
      nbad[delay_value] += alct_rx_bad[delay_value][ibit];
  //
  (*MyOutput_) << "--> Summed over all data pairs: " << std::endl;
  std::cout    << "--> Summed over all data pairs: " << std::endl;
  (*MyOutput_) << " alct_tx    bad data count" << std::endl;
  std::cout    << " alct_tx    bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  std::cout    << "---------   --------------" << std::endl;
  for (int delay_value = 0; delay_value <maximum_number_of_phase_delay_values; delay_value++) {
    (*MyOutput_) << "    " << std::dec << delay_value << "           " << std::dec << nbad[delay_value] <<std::endl;
    std::cout    << "    " << std::dec << delay_value << "           " << std::dec << nbad[delay_value] <<std::endl;
  }
  //
  ALCTtxPhase_ = window_analysis(nbad,maximum_number_of_phase_delay_values);
  //
  (*MyOutput_) << "alternating 1/0 value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl << std::endl;
  std::cout    << "alternating 1/0 value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl << std::endl;
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
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
    (*MyOutput_) << std::setw(5) << delay_value;	
    std::cout    << std::setw(5) << delay_value;	
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  //
  (*MyOutput_) << "pair      ";
  std::cout    << "pair      ";
  for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
    (*MyOutput_) << " ----";
    std::cout    << " ----";
  }
  (*MyOutput_) << std::endl;
  std::cout    << std::endl;
  
  for (int ibit=0; ibit<=27; ++ibit) {
    (*MyOutput_) << std::setw(3) << ibit << "       ";
    std::cout    << std::setw(3) << ibit << "       ";
    for (int delay_value=0; delay_value<maximum_number_of_phase_delay_values; ++delay_value) {
      (*MyOutput_) << std::setw(5) << alct_rx_bad[delay_value][ibit];
      std::cout    << std::setw(5) << alct_rx_bad[delay_value][ibit];
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
    thisTMB->SetAlctTxClockDelay(ALCTtxPhase_);
  } else {
    (*MyOutput_) << "Reverting back to original alct_tx_clock_delay value..." << std::endl;
    std::cout    << "Reverting back to original alct_tx_clock_delay value..." << std::endl;
    thisTMB->SetAlctTxClockDelay(initial_alct_tx_phase);
  }
  thisTMB->WriteRegister(phaser_alct_txd_adr);
  thisTMB->FirePhaser(phaser_alct_txd_adr);
  //
  return ALCTtxPhase_;
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
  int initial_alct_tx_phase      = thisTMB->GetAlctTxClockDelay();
  int initial_alct_tx_posneg     = thisTMB->GetAlctTxPosNeg();
  int initial_alct_rx_phase      = thisTMB->GetAlctRxClockDelay();
  int initial_alct_rx_posneg     = thisTMB->GetAlctRxPosNeg();
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
  // Not sure where in the FIFO the data looped back is going to show up...
  // Do a quick scan to look if there is ANY good data here...
  //
  int good_depth = -999;
  int n_pipe_depth = 0;
  //
  int temp_display[2][2][16][maximum_number_of_phase_delay_values][maximum_number_of_phase_delay_values] = {};
  //
  int good_data[2][2][16] = {};
  //
  int min_pipedepth=10;
  int max_pipedepth=16;  
  //
  for (int rx_posneg=0; rx_posneg<2; rx_posneg++) {
    //
    thisTMB->SetAlctRxPosNeg(rx_posneg);
    thisTMB->WriteRegister(phaser_alct_rxd_adr);
    thisTMB->FirePhaser(phaser_alct_rxd_adr);
    //
    for (int tx_posneg=0; tx_posneg<2; tx_posneg++) {
      //
      thisTMB->SetAlctTxPosNeg(tx_posneg);
      thisTMB->WriteRegister(phaser_alct_txd_adr);
      thisTMB->FirePhaser(phaser_alct_txd_adr);
      //
      for (int pipe_depth=min_pipedepth; pipe_depth<max_pipedepth; pipe_depth++) {
	//
	std::cout << "Scanning at (alct_posneg,alct_tx_posneg,pipeline depth) = (" 
		  << rx_posneg << "," << tx_posneg << "," << pipe_depth << ")... ";
	//
	good_data[rx_posneg][tx_posneg][pipe_depth] = 0; 
	//
	for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; rx_value++) {
	  thisTMB->SetAlctRxClockDelay(rx_value);
	  thisTMB->WriteRegister(phaser_alct_rxd_adr);
	  thisTMB->FirePhaser(phaser_alct_rxd_adr);
	  //
	  for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; tx_value++) {
	    thisTMB->SetAlctTxClockDelay(tx_value);
	    thisTMB->WriteRegister(phaser_alct_txd_adr);
	    thisTMB->FirePhaser(phaser_alct_txd_adr);
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
	    // This is the quick scan, so don't wait for a short time for errors to accumulate...
	    //
	    // Read TMB data check flipflops
	    thisTMB->ReadRegister(alct_sync_ctrl_adr);
	    //
	    if ( thisTMB->GetReadALCTSync1stErrorLatched()==0 && thisTMB->GetReadALCTSync2ndErrorLatched()==0 ) {
	      good_data[rx_posneg][tx_posneg][pipe_depth]++; 
	      temp_display[rx_posneg][tx_posneg][pipe_depth][rx_value][tx_value] = pipe_depth;
	    }
	    //
	  }
	}
	std::cout    << "... Number of good spots = " << good_data[rx_posneg][tx_posneg][pipe_depth] << std::endl;
	//
	if (good_data[rx_posneg][tx_posneg][pipe_depth] > 0) {
	  good_depth = pipe_depth;
	  n_pipe_depth++;
	  //
	  (*MyOutput_) << "With (alct_posneg,alct_tx_posneg,pipeline depth) = (" 
		       << rx_posneg << "," << tx_posneg << "," << pipe_depth << ")... ";
	  (*MyOutput_) << "Number of good spots = " << good_data[rx_posneg][tx_posneg][pipe_depth] << std::endl;
	  //
	  std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
	  std::cout    << "         00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24" << std::endl;
	  std::cout    << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
	  //
	  for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; ++rx_value) {
	    //
	    std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
	    //
	    for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; ++tx_value) {
	      //
	      std::cout    << std::dec << std::setw(4) << temp_display[rx_posneg][tx_posneg][pipe_depth][rx_value][tx_value] << " ";
	      //
	    }
	    std::cout    << std::endl;    
	  }
	}
      }
      // focus in on where the good pipedepth is, in order not to waste any time...
      if (good_depth>0) {
        min_pipedepth = (good_depth-2 < 0  ? 0  : good_depth-2);
        max_pipedepth = (good_depth+2 > 16 ? 16 : good_depth+2);
      }
    }
  }
  //
  if (good_depth < 0) {
    std::cout    << "ERROR:  No pipe depths with good data." << std::endl;
    (*MyOutput_) << "ERROR:  No pipe depths with good data." << std::endl;
    ALCTtxPhase_ = -999;
    ALCTtxPosNeg_= -999;
    ALCTrxPhase_ = -999;
    ALCTrxPosNeg_= -999;
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
    thisTMB->SetAlctRxClockDelay(initial_alct_rx_phase);
    thisTMB->SetAlctRxPosNeg(initial_alct_rx_posneg);
    thisTMB->WriteRegister(phaser_alct_rxd_adr);
    thisTMB->FirePhaser(phaser_alct_rxd_adr);
    //
    thisTMB->SetAlctTxClockDelay(initial_alct_tx_phase);
    thisTMB->SetAlctTxPosNeg(initial_alct_tx_posneg);
    thisTMB->WriteRegister(phaser_alct_txd_adr);
    thisTMB->FirePhaser(phaser_alct_txd_adr);
    //
    return -1;
  }
  //
  int best_pipedepth =-1;
  int best_rx_posneg =-1;
  int best_tx_posneg =-1;
  //
  std::cout    << "Picking the combination with the maximum number of good spots..." << std::endl;
  //
  int max_good = 0;
  for (int rx_posneg=0; rx_posneg<2; rx_posneg++) {
    for (int tx_posneg=0; tx_posneg<2; tx_posneg++) {
      for(int pipedepth=0; pipedepth<16; pipedepth++) {
	if (good_data[rx_posneg][tx_posneg][pipedepth]>max_good) {
	  best_pipedepth = pipedepth;
	  best_rx_posneg = rx_posneg;
	  best_tx_posneg = tx_posneg;
	  max_good = good_data[rx_posneg][tx_posneg][pipedepth];
	}
      }
    }
  }
  std::cout    << "Performing a more thorough scan at (rx_posneg,tx_posneg,pipedepth) = (" 
	       << best_rx_posneg << "," << best_tx_posneg << "," << best_pipedepth << ")" << std::endl;
  (*MyOutput_) << "Performing a more thorough scan at (rx_posneg,tx_posneg,pipedepth) = (" 
	       << best_rx_posneg << "," << best_tx_posneg << "," << best_pipedepth << ")" << std::endl;
  //
  ThoroughRxTxScan(best_rx_posneg,best_tx_posneg,best_pipedepth);
  //
  // Analysis of the trigger primitive arrival timing at the SP indicates that the following 
  // set of communication phase parameters to send the BC0 signal to ALCT results in a 
  // non-desirable shift of 1bx...  A perusal of the scans seems to indicate that we might
  // have a comparable sized communication window of good data by just forcing the tx_posneg
  // bit to be 1... Thus.....
  //
  int alct_tof_delay = thisTMB->GetAlctTOFDelay();
  //
  if (ALCTtxPosNeg_ == 0 && ALCTtxPhase_ > 15) {
    //
    int use_this_tx_posneg = 1;
    //
    std::cout    << "The scan returned values (alct_tx_posneg,alct_tx_clock_delay) = (" << ALCTtxPosNeg_ << "," << ALCTtxPhase_ << ")." << std::endl;
    (*MyOutput_) << "The scan returned values (alct_tx_posneg,alct_tx_clock_delay) = (" << ALCTtxPosNeg_ << "," << ALCTtxPhase_ << ")." << std::endl;
    if (alct_tof_delay == 12) {
      //
      std::cout    << "However, since alct_tof_delay is set to " << alct_tof_delay << ", keep this scan..." << std::endl;
      (*MyOutput_) << "However, since alct_tof_delay is set to " << alct_tof_delay << ", keep this scan..." << std::endl;
      //
    } else {
      //
      std::cout    << "In addition, alct_tof_delay is set to " << alct_tof_delay 
		   << ". Since this combination has been deemed to be bad, change alct_tx_posneg to 1 and redo scan..." << std::endl;
      (*MyOutput_) << "In addition, alct_tof_delay is set to " << alct_tof_delay 
		   << ". Since this combination has been deemed to be bad, change alct_tx_posneg to 1 and redo scan..." << std::endl;
      //
      int max_good = 0;
      for (int rx_posneg=0; rx_posneg<2; rx_posneg++) {
	for(int pipedepth=0; pipedepth<16; pipedepth++) {
	  if (good_data[rx_posneg][use_this_tx_posneg][pipedepth]>max_good) {
	    best_pipedepth = pipedepth;
	    best_rx_posneg = rx_posneg;
	    max_good = good_data[rx_posneg][use_this_tx_posneg][pipedepth];
	  }
	}
      }
      //
      std::cout    << "Performing a more thorough scan at (rx_posneg,tx_posneg,pipedepth) = (" 
		   << best_rx_posneg << "," << use_this_tx_posneg << "," << best_pipedepth << ")" << std::endl;
      (*MyOutput_) << "Performing a more thorough scan at (rx_posneg,tx_posneg,pipedepth) = (" 
		   << best_rx_posneg << "," << use_this_tx_posneg << "," << best_pipedepth << ")" << std::endl;
      //    
      ThoroughRxTxScan(best_rx_posneg,use_this_tx_posneg,best_pipedepth);
    }
  }
  //
  //
  (*MyOutput_) << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl;
  (*MyOutput_) << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl;
  (*MyOutput_) << "Best value is alct_posneg = " << std::dec << ALCTrxPosNeg_ << std::endl;
  (*MyOutput_) << "Best value is alct_tx_posneg = " << std::dec << ALCTtxPosNeg_ << std::endl;
  //
  std::cout    << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl;
  std::cout    << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl;
  std::cout    << "Best value is alct_posneg = " << std::dec << ALCTrxPosNeg_ << std::endl;
  std::cout    << "Best value is alct_tx_posneg = " << std::dec << ALCTtxPosNeg_ << std::endl;
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    std::cout    << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctTxClockDelay(ALCTtxPhase_);
    thisTMB->SetAlctTxPosNeg(ALCTtxPosNeg_);
    thisTMB->SetAlctRxClockDelay(ALCTrxPhase_);
    thisTMB->SetAlctRxPosNeg(ALCTrxPosNeg_);
    //
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    std::cout    << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    thisTMB->SetAlctTxClockDelay(initial_alct_tx_phase);
    thisTMB->SetAlctTxPosNeg(initial_alct_tx_posneg);
    thisTMB->SetAlctRxClockDelay(initial_alct_rx_phase);
    thisTMB->SetAlctRxPosNeg(initial_alct_rx_posneg);
  }
  // Set the phase values chosen above...
  thisTMB->WriteRegister(phaser_alct_txd_adr);
  thisTMB->FirePhaser(phaser_alct_txd_adr);
  //
  thisTMB->WriteRegister(phaser_alct_rxd_adr);
  thisTMB->FirePhaser(phaser_alct_rxd_adr);
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
//
void ChamberUtilities::ThoroughRxTxScan(int rx_posneg, int tx_posneg, int pipedepth) {
  //
  thisTMB->SetAlctRxPosNeg(rx_posneg);
  thisTMB->WriteRegister(phaser_alct_rxd_adr);
  thisTMB->FirePhaser(phaser_alct_rxd_adr);
  //
  thisTMB->SetAlctTxPosNeg(tx_posneg);
  thisTMB->WriteRegister(phaser_alct_txd_adr);
  thisTMB->FirePhaser(phaser_alct_txd_adr);
  //
  thisTMB->SetALCTSyncRxDataDelay(pipedepth);
  thisTMB->WriteRegister(alct_sync_ctrl_adr);
  //
  //
  // A display (statistics) array...
  int alct_tx_rx_display[maximum_number_of_phase_delay_values][maximum_number_of_phase_delay_values] = {};
  //
  // An analysis array...
  int alct_tx_rx_analyze[maximum_number_of_phase_delay_values][maximum_number_of_phase_delay_values] = {};
  //
  for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; rx_value++) {
    thisTMB->SetAlctRxClockDelay(rx_value);
    thisTMB->WriteRegister(phaser_alct_rxd_adr);
    thisTMB->FirePhaser(phaser_alct_rxd_adr);
    //
    std::cout << "Scanning tx values at rx = " << rx_value << std::endl;
    //
    for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; tx_value++) {
      thisTMB->SetAlctTxClockDelay(tx_value);
      thisTMB->WriteRegister(phaser_alct_txd_adr);
      thisTMB->FirePhaser(phaser_alct_txd_adr);
      //
      //Set depth where to look for the data
      thisTMB->SetALCTSyncTXRandom(1);
      //
      const int max_loop = 100;
      //
      alct_tx_rx_display[rx_value][tx_value] = max_loop;
      //
      for (int check_loop=0; check_loop<max_loop; check_loop++) {
	//
	// Clear TMB data check flipflops
	thisTMB->SetALCTSyncClearErrors(1);
	//
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// Unclear error flipflops, after this write, the errors are being tallied by TMB firmware
	thisTMB->SetALCTSyncClearErrors(0);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// wait for an error to arrive in order to latch it...
	::usleep(100);
	//
	// Read TMB data check flipflops
	thisTMB->ReadRegister(alct_sync_ctrl_adr);
	//      
	if (thisTMB->GetReadALCTSync1stErrorLatched() == 0 && thisTMB->GetReadALCTSync2ndErrorLatched() == 0) {
	  alct_tx_rx_display[rx_value][tx_value] --;  // count down the number of times it is OK
	} 
	//
	// no sense wasting time on this rx,tx value.  It has badness...
	if (thisTMB->GetReadALCTSync1stErrorLatched() || thisTMB->GetReadALCTSync2ndErrorLatched()) check_loop = max_loop+1;
	//
      }
      //
      // Since the analysis routine requires a non-zero value as a "good" spot and 0 as a "bad" spot,
      // invert what is put in the analysis array compared to the display array...
      alct_tx_rx_analyze[rx_value][tx_value] = max_loop - alct_tx_rx_display[rx_value][tx_value];
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
	printf("Latch OK=%8i delay_value_tx=%2i delay_value_rx=%2i", alct_tx_rx_display[rx_value][tx_value],tx_value,rx_value);
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
  (*MyOutput_) << "         00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24" << std::endl;
  std::cout    << "         00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24" << std::endl;
  (*MyOutput_) << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
  std::cout    << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
  //
  //
  for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; ++rx_value) {
    //
    (*MyOutput_) << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    //
    for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; ++tx_value) {
      //
      (*MyOutput_) << std::dec << std::setw(4) << alct_tx_rx_display[rx_value][tx_value] << " ";
      std::cout    << std::dec << std::setw(4) << alct_tx_rx_display[rx_value][tx_value] << " ";
      //
    }
    (*MyOutput_) << std::endl;    
    std::cout    << std::endl;    
  }
  //
  if (debug_>5) {  
    // Display the analyzed data
    //
    std::cout << "analyzed data..." << std::endl;
    //
    std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
    std::cout    << "         00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24" << std::endl;
    std::cout    << "        ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====" << std::endl; 
    //
    for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; ++rx_value) {
      std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
      //
      for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; ++tx_value) 
	std::cout    << std::dec << std::setw(4) << alct_tx_rx_analyze[rx_value][tx_value] << " ";
      //
      std::cout    << std::endl;    
    }
  }
  //
  // These are the best values:
  //
  ALCTrxPosNeg_ = rx_posneg;
  ALCTtxPosNeg_ = tx_posneg;
  //
  ALCT_phase_analysis(alct_tx_rx_analyze);
  //
  return;
}
//
int ChamberUtilities::ALCT_TMB_TimingUsingErrorCorrectionCode() {
  //
  if (debug_) {
    std::cout << "**********************************************************" << std::endl;
    std::cout << "Find alct_[rx,tx]_phase_delay using Error Correction Code" << std::endl;
    std::cout << "**********************************************************" << std::endl;
  }
  (*MyOutput_) << "**********************************************************" << std::endl;
  (*MyOutput_) << "Find alct_[rx,tx]_phase_delay using Error Correction Code" << std::endl;
  (*MyOutput_) << "**********************************************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_alct_tx_phase      = thisTMB->GetAlctTxClockDelay();
  int initial_alct_rx_phase      = thisTMB->GetAlctRxClockDelay();
  int initial_alct_rx_posneg     = thisTMB->GetAlctRxPosNeg();
  //
  int initial_fire_l1a_oneshot  = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  //
  int initial_ecc_enable          = thisTMB->GetALCTErrorCorrectionCodeEnable();
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
  // Turn on the error correction code in the TMB
  thisTMB->SetALCTErrorCorrectionCodeEnable(1);
  thisTMB->WriteRegister(alct_stat_adr);
  //
  int good_depth = -999;
  //
  // A display (statistics) array...
  int alct_tx_rx_display[maximum_number_of_phase_delay_values][maximum_number_of_phase_delay_values] = {};
  //
  // The analysis array...
  int alct_tx_rx_analyze[maximum_number_of_phase_delay_values][maximum_number_of_phase_delay_values] = {};
  //
  for (int pipe_depth=0; pipe_depth<16; pipe_depth++) {
    //
    std::cout << "pipe_depth = " << pipe_depth << std::endl;
    //
    thisTMB->SetALCTSyncRxDataDelay(pipe_depth);
    thisTMB->SetALCTSyncTXRandom(0);
    thisTMB->WriteRegister(alct_sync_ctrl_adr);
    //
    for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; rx_value++) {
      thisTMB->SetAlctRxClockDelay(rx_value);
      thisTMB->WriteRegister(phaser_alct_rxd_adr);
      thisTMB->FirePhaser(phaser_alct_rxd_adr);
      //
      std::cout << "Scanning tx values at rx = " << rx_value << std::endl;
      //
      for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; tx_value++) {
	thisTMB->SetAlctTxClockDelay(tx_value);
	thisTMB->WriteRegister(phaser_alct_txd_adr);
	thisTMB->FirePhaser(phaser_alct_txd_adr);
	//
	// Put the ALCT into normal running mode to turn off its Linear Feedback Shift Register 
	thisTMB->SetAlctSequencerCommand(NORMAL_MODE);
	thisTMB->WriteRegister(alct_cfg_adr);	
	//
	// Tell ALCT to send 49-bits of random data (+ 7-bit Error Correction Code data)
	thisTMB->SetAlctSequencerCommand(SEND_RANDOM);
	thisTMB->WriteRegister(alct_cfg_adr);	
	//
	// Clear TMB data check flipflops and stop random data from going from TMB
	thisTMB->SetALCTSyncClearErrors(1);
	thisTMB->WriteRegister(alct_sync_ctrl_adr);
	//
	// Unclear error flipflops (start tallying errors), and set depth where to look for the data
  	thisTMB->SetALCTSyncClearErrors(0);
  	thisTMB->WriteRegister(alct_sync_ctrl_adr);
  	//
  	thisTMB->ResetCounters();
  	//
	bool go_quick = false;
	//
	int number_of_latch_errors = 0;
	//
  	for (int check_loop=0; check_loop<10; check_loop++) {
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
  	  number_of_latch_errors += (thisTMB->GetReadALCTSync1stErrorLatched() | thisTMB->GetReadALCTSync2ndErrorLatched());
  	  //
  	  // no sense wasting time on this rx,tx value.  It has badness...
  	  if (thisTMB->GetReadALCTSync1stErrorLatched() || thisTMB->GetReadALCTSync2ndErrorLatched()) go_quick = true;
  	  //
  	  // OK for this read:  read it again, slowly...
  	  if (!thisTMB->GetReadALCTSync1stErrorLatched() && !thisTMB->GetReadALCTSync2ndErrorLatched()) go_quick = false;
	  //
        }
        //
	thisTMB->GetCounters();
	int tx_one_bit_errors           = thisTMB->GetCounter( thisTMB->GetECCTriggerPathOneErrorCounterIndex() );
	int tx_two_bit_errors           = thisTMB->GetCounter( thisTMB->GetECCTriggerPathTwoErrorsCounterIndex() );
	int tx_more_than_two_bit_errors = thisTMB->GetCounter( thisTMB->GetECCTriggerPathMoreThanTwoErrorsCounterIndex() );
	//
	alct_tx_rx_display[rx_value][tx_value] =  tx_one_bit_errors + tx_two_bit_errors + tx_more_than_two_bit_errors ;
	//
	//	std::cout << "(rx,tx) = (" << rx_value << "," << tx_value 
	//		  << ") -> number of errors = " << alct_tx_rx_display[rx_value][tx_value];
	//
        if ( number_of_latch_errors == 0 ) {
	  //	  std::cout << " ----> OK <----" << std::endl;
	  alct_tx_rx_analyze[rx_value][tx_value] = pipe_depth;
	} 
	//	else {
	//	  std::cout << " ***** BAD *****" << std::endl;
	//	}
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
	  printf("Latch OK=%8i depth=%2i delay_value_tx=%2i delay_value_rx=%2i", alct_tx_rx_display[rx_value][tx_value],good_depth,tx_value,rx_value);
	  printf("  read 1st=%8.8X 2nd=%8.8X ", alct_sync_rxdata_1st,alct_sync_rxdata_2nd);
	  printf("expect 1st=%8.8X 2nd=%8.8X\n",alct_sync_expect_1st,alct_sync_expect_2nd);
        }
      }
    }
  }
  //
  //
  // Display the statistics:
  (*MyOutput_) << "Result (tx vs. rx)   tx ----> " << std::endl;
  std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
  (*MyOutput_) << "           00      01      02      03      04      05      06      07      08      09      10      11      12"   << std::endl;
  (*MyOutput_) << "         ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======" << std::endl; 
  std::cout    << "           00      01      02      03      04      05      06      07      08      09      10      11      12"   << std::endl;
  std::cout    << "         ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======" << std::endl; 

  //
  for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; ++rx_value) {
    //
    (*MyOutput_) << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
    //
    for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; ++tx_value) {
      //
      (*MyOutput_) << std::dec << std::setw(7) << alct_tx_rx_display[rx_value][tx_value] << " ";
      std::cout    << std::dec << std::setw(7) << alct_tx_rx_display[rx_value][tx_value] << " ";
      //
    }
    (*MyOutput_) << std::endl;    
    std::cout    << std::endl;    
  }
  //
  if (debug_) {  
    // Display the analyzed data
    //
    std::cout << "analyzed data..." << std::endl;
    //
    std::cout    << "Result (tx vs. rx)   tx ----> " << std::endl;
    std::cout    << "           00      01      02      03      04      05      06      07      08      09      10      11      12" << std::endl;
    std::cout    << "         ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======  ======" << std::endl; 
    //
    for (int rx_value=0; rx_value<maximum_number_of_phase_delay_values; ++rx_value) {
      std::cout    << " rx =" << std::dec << std::setw(2) << rx_value << " " ; 
      //
      for (int tx_value=0; tx_value<maximum_number_of_phase_delay_values; ++tx_value) 
  	std::cout    << std::dec << std::setw(7) << alct_tx_rx_analyze[rx_value][tx_value] << " ";
      //
      std::cout    << std::endl;    
    }
  }
  //
  ALCT_phase_analysis(alct_tx_rx_analyze);
  //
  ALCTrxPosNeg_ = thisTMB->GetAlctRxPosNeg();
  std::cout    << "Best value is alct_rx_posneg = " << ALCTrxPosNeg_ << std::endl;
  (*MyOutput_) << "Best value is alct_rx_posneg = " << ALCTrxPosNeg_ << std::endl;
  //
  if (use_measured_values_) { 
    (*MyOutput_) << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    std::cout    << "Setting alct_rx/tx_clock_delays to measured values..." << std::endl;
    thisTMB->SetAlctTxClockDelay(ALCTtxPhase_);
    thisTMB->SetAlctRxClockDelay(ALCTrxPhase_);
  } else {
    (*MyOutput_) << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    std::cout    << "Reverting back to original alct_rx/tx_clock_delay values..." << std::endl;
    thisTMB->SetAlctTxClockDelay(initial_alct_tx_phase);
    thisTMB->SetAlctRxClockDelay(initial_alct_rx_phase);
    thisTMB->SetAlctRxPosNeg(initial_alct_rx_posneg);
    //
  }
  // Set the phase values chosen above...
  thisTMB->WriteRegister(phaser_alct_txd_adr);
  thisTMB->FirePhaser(phaser_alct_txd_adr);
  //
  thisTMB->WriteRegister(phaser_alct_rxd_adr);
  thisTMB->FirePhaser(phaser_alct_rxd_adr);
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
  thisTMB->SetALCTErrorCorrectionCodeEnable(initial_ecc_enable);
  thisTMB->WriteRegister(alct_stat_adr);
  //
  thisTMB->SetAlctDemuxMode(initial_demux_mode);
  thisTMB->SetAlctRawReadAddress(initial_ALCT_read_address);
  thisTMB->WriteRegister(alctfifo1_adr);
  //
  thisTMB->RedirectOutput(MyOutput_);
  alct->RedirectOutput(MyOutput_);
  //
  return 0;
}
//
//-----------------------------------------------------
// Check all the lines in the TMB-ALCT cables
//-----------------------------------------------------
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
  // expected patterns:
  int alct_1st_expect = 0;
  int alct_2nd_expect = 0;
  //
  for (int ipass=0; ipass<number_of_passes; ipass++) {
    for (int ifirstsecond=0; ifirstsecond<2; ifirstsecond++) {
      for (int itransmit=0; itransmit<28; itransmit++) {
	//    for (int ifirstsecond=0; ifirstsecond<1; ifirstsecond++) {
	//      for (int itransmit=0; itransmit<21; itransmit+=10) {
	//
	// expected patterns:
	alct_1st_expect = (1 << itransmit) * (ifirstsecond==0);
	alct_2nd_expect = (1 << itransmit) * (ifirstsecond==1);
	//
	// Fill data banks with the above data...
	//
	// Since there are so many fewer TMB->ALCT data bits than ALCT->TMB,
	// the following mapping is used to tell ALCT which bits to send the
	// data back on (defined by AlctSequencerCommand(int register_address), below):
	//
	// bank = 0 holds bits 0-9; bank = 1 holds bit 10-19; bank = 2 holds bit 20-27
	int ibit  = itransmit % 10;
	int ibank = itransmit / 10;
	//
	// "1st_phase" holds the stuff received in first 80MHz phase
	int alct_1st_phase[3] = {};
	alct_1st_phase[ibank] = (1 << ibit) * (ifirstsecond==0);
	//
	// "2nd_phase" holds the stuff received in second 80MHz phase
	int alct_2nd_phase[3] = {};
	alct_2nd_phase[ibank] = (1 << ibit) * (ifirstsecond==1);
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
	  thisTMB->SetALCTSyncTxData1st(alct_1st_phase[bank]);
	  thisTMB->WriteRegister(alct_sync_txdata_1st_adr);
	  //
	  // second 80MHz frame
	  thisTMB->SetALCTSyncTxData2nd(alct_2nd_phase[bank]);
	  thisTMB->WriteRegister(alct_sync_txdata_2nd_adr);
	  //
	  //	  std::cout << "pattern = " << std::hex << alct_1st_expect 
	  //		    << ", data in Bank "        << bank
	  //		    << " = "                    << alct_1st_phase[bank]     
	  //		    << " using seq_cmd = "      << register_address 
	  //		    << std::endl; 
	  //	  ::sleep(30);
	  //
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
	if (ipass==0 && debug_>=5) {
	  char dash1[2]={'-','1'};
	  printf("\t%1i %2i tx",ifirstsecond,itransmit);
	  printf("\t1st "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(i==itransmit)*(ifirstsecond==0)]);
	  printf(" 2nd ");  for (int i=0; i<=27; ++i) printf("%1c",dash1[(i==itransmit)*(ifirstsecond==1)]);
	  printf("\n");
	  printf("\t     rx");
	  printf(" 1st "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_rxdata_1st >> i) & 0x1]);
	  printf(" 2nd "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_rxdata_2nd >> i) & 0x1]);
	  printf("\n");
	  printf("\t In TMB");
	  printf(" 1st "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_expect_1st >> i) & 0x1]);
	  printf(" 2nd "); for (int i=0; i<=27; ++i) printf("%1c",dash1[(alct_sync_expect_2nd >> i) & 0x1]);
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
//------------------------------------------
// ALCT->TMB BC0 delays 
//------------------------------------------
int ChamberUtilities::ALCTBC0Scan() {
  //
  // The goal of this scan is to find the alct_bx0_delay value which gives the desired
  // propagation time of the BC0 signal from TMB -> ALCT -> TMB.
  //
  // Thus, Before performing this scan, one should have the following defined:
  // - alct_tof_delay and alct_txd to define the TMB -> ALCT BC0timing
  // - alct_[rx,tx]_clock_delay and alct_[rx,tx]_posneg) to establish good communication between TMB <-> ALCT
  //
  if (debug_) {
    std::cout << "**********************************" << std::endl;
    std::cout << "Scan to align ALCT BC0 back at TMB" << std::endl;
    std::cout << "**********************************" << std::endl;
  }
  (*MyOutput_) << "**********************************" << std::endl;
  (*MyOutput_) << "Scan to align ALCT BC0 back at TMB" << std::endl;
  (*MyOutput_) << "**********************************" << std::endl;
  //
  // send output to std::cout except for the essential information 
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  // Get initial values
  int initial_fire_l1a_one_shot = thisTMB->GetFireL1AOneshot();
  int initial_ignore_ccb_rx     = thisTMB->GetIgnoreCCBRx();
  //
  int initial_mpc_idle_blank     = thisTMB->GetMpcIdleBlank();
  int initial_mpc_sel_ttc_bx0    = thisTMB->GetSelectMpcTtcBx0(); 
  //
  int initial_clct_bx0_delay     = thisTMB->GetClctBx0Delay();    
  int initial_alct_bx0_delay     = thisTMB->GetAlctBx0Delay();
  int initial_alct_bx0_enable    = thisTMB->GetAlctBx0Enable();   
  //
  int initial_alct_txdata_delay  = thisTMB->GetALCTTxDataDelay(); 
  //
  int initial_bxn_offset         = thisTMB->GetBxnOffset();
  //
  //
  if (debug_>=10) {
    std::cout << "Initial values..." << std::endl;
    std::cout << "-> initial_mpc_idle_blank    = " << initial_mpc_idle_blank    << std::endl;
    std::cout << "-> initial_mpc_sel_ttc_bx0   = " << initial_mpc_sel_ttc_bx0   << std::endl;
    //
    std::cout << "-> initial_clct_bx0_delay    = " << initial_clct_bx0_delay    << std::endl;
    std::cout << "-> initial_alct_bx0_delay    = " << initial_alct_bx0_delay    << std::endl;
    std::cout << "-> initial_alct_bx0_enable   = " << initial_alct_bx0_enable   << std::endl;
    //
    std::cout << "-> initial_alct_txdata_delay = " << initial_alct_txdata_delay << std::endl;
    //
    std::cout << "-> initial_bxn_offset        = " << initial_bxn_offset        << std::endl;
  }
  //
  std::cout    << "This scan has the following input parameters... " << std::endl;
  std::cout    << "alct_tof_delay          = 0x" << std::hex << thisTMB->GetAlctTOFDelay() << std::endl; 
  std::cout    << "tmb_to_alct_data_delay  = 0x" << std::hex << thisTMB->GetALCTTxDataDelay() << std::endl;
  (*MyOutput_) << "This scan has the following input parameters... " << std::endl;
  (*MyOutput_) << "alct_tof_delay          = 0x" << std::hex << thisTMB->GetAlctTOFDelay() << std::endl;
  (*MyOutput_) << "tmb_to_alct_data_delay  = 0x" << std::hex << thisTMB->GetALCTTxDataDelay() << std::endl;
  //
  // Set up for this test:
  // turn off the one shot L1A (from TMB)...
  thisTMB->SetFireL1AOneshot(0);
  // turn on the CCB inputs to get BC0 defined from CCB
  thisTMB->SetIgnoreCCBRx(0);
  thisTMB->WriteRegister(ccb_cfg_adr);
  if (debug_>=10) {
    thisTMB->ReadRegister(ccb_cfg_adr);
    thisTMB->PrintTMBRegister(ccb_cfg_adr);
  }
  //
  // MpcIdleBlank = 0 = send BC0 signals to MPC every orbit (1 = blank unless LCT is sent to MPC)
  thisTMB->SetMpcIdleBlank(0);
  // SelectMpcTtcBx0 = 0 = send BC0 to MPC (on LCT0) from TMB (1 = send BC0 from CCB)
  thisTMB->SetSelectMpcTtcBx0(0);
  thisTMB->WriteRegister(tmb_trig_adr);
  if (debug_>=10) {
    thisTMB->ReadRegister(tmb_trig_adr);
    thisTMB->PrintTMBRegister(tmb_trig_adr);
  }
  //
  // AlctBx0Enable = 1 = send BC0 to MPC (on LCT1) from ALCT (0 = copy what it has from the TMB)
  thisTMB->SetAlctBx0Enable(1);
  thisTMB->WriteRegister(bx0_delay_adr);
  if (debug_>=10) {
    thisTMB->ReadRegister(bx0_delay_adr);
    thisTMB->PrintTMBRegister(bx0_delay_adr);
  }
  //
  std::cout    << "Using tmb_bxn_offset = " << std::dec << local_tmb_bxn_offset_ << std::endl;
  (*MyOutput_) << "Using tmb_bxn_offset = " << std::dec << local_tmb_bxn_offset_ << std::endl;
  //
  if (initial_bxn_offset != local_tmb_bxn_offset_) {
    thisTMB->SetBxnOffset(local_tmb_bxn_offset_);
    thisTMB->WriteRegister(seq_offset_adr);
    //
    // Send a BGo "Resync" to make sure that the BC0 offset has taken effect
    thisCCB_->setCCBMode(CCB::VMEFPGA);
    thisCCB_->syncReset(); 
    thisCCB_->setCCBMode(CCB::DLOG);
    //
    // put in a sleep to allow the resync to take hold...
    ::sleep(1);
  }
  //
  // Values for scan over tmb_bxn_offset
  //
  const int minimum_delay_value=0;
  const int maximum_delay_value=16;
  //
  const int number_of_checks_per_value = 100;
  //
  int matched[maximum_delay_value*2] = {};
  //
  std::cout << "Scanning alct_bx0_delay from " << std::dec << minimum_delay_value << " to " << maximum_delay_value << std::endl;
  //
  for (int delay_value=minimum_delay_value; delay_value<maximum_delay_value; delay_value++) {
    //
    thisTMB->SetAlctBx0Delay(delay_value);
    thisTMB->WriteRegister(bx0_delay_adr);
    if (debug_>=10) {
      thisTMB->ReadRegister(bx0_delay_adr);
      thisTMB->PrintTMBRegister(bx0_delay_adr);
    }
    //
    ::usleep(1000);
    //
    for (int j=0; j<number_of_checks_per_value; j++) {
      thisTMB->ReadRegister(bx0_delay_adr);
      int BC0_match = thisTMB->GetReadBx0Match();
      //
      if (debug_>=10 && BC0_match) 
	std::cout << "delay " << thisTMB->GetBxnOffset() << " ---> ALCT*CLCT BC0_match = " << BC0_match << "<-----" << std::endl;
      //
      matched[delay_value] += BC0_match;
    }
  }
  //
  // print out the results...
  //
  float float_average = AverageHistogram(matched,minimum_delay_value,maximum_delay_value);
  ALCT_bx0_delay_     = RoundOff(float_average);
  match_trig_alct_delay_ = ALCT_bx0_delay_;
  tmb_bxn_offset_used_   = local_tmb_bxn_offset_;
  //
  (*MyOutput_) << "------------------------------------------" << std::endl;
  (*MyOutput_) << "ALCT*CLCT BC0 matches vs. alct_bx0_delay" << std::endl;
  for (int delay_value=minimum_delay_value; delay_value<maximum_delay_value; delay_value++) {
    (*MyOutput_) << "alct_bx0_delay[" << std::dec << delay_value << "] = " << matched[delay_value] << std::endl;
    std::cout    << "alct_bx0_delay[" << std::dec << delay_value << "] = " << matched[delay_value] << std::endl;
    //
  }
  (*MyOutput_) << "Best value is alct_bx0_delay = " << ALCT_bx0_delay_ << std::endl;
  std::cout    << "Best value is alct_bx0_delay = " << ALCT_bx0_delay_ << std::endl;
  (*MyOutput_) << "Best value is match_trig_alct_delay = " << match_trig_alct_delay_ << std::endl;
  std::cout    << "Best value is match_trig_alct_delay = " << match_trig_alct_delay_ << std::endl;
  //
  //
  // Return back to the initial conditions...
  thisTMB->SetFireL1AOneshot(initial_fire_l1a_one_shot);
  thisTMB->SetIgnoreCCBRx(initial_ignore_ccb_rx);
  thisTMB->WriteRegister(ccb_cfg_adr);
  //
  thisTMB->SetSelectMpcTtcBx0(initial_mpc_sel_ttc_bx0);
  thisTMB->SetMpcIdleBlank(initial_mpc_idle_blank);
  thisTMB->WriteRegister(tmb_trig_adr);
  //
  thisTMB->SetClctBx0Delay(initial_clct_bx0_delay);
  thisTMB->SetAlctBx0Delay(initial_alct_bx0_delay);
  thisTMB->SetAlctBx0Enable(initial_alct_bx0_enable);
  thisTMB->WriteRegister(bx0_delay_adr);
  //
  thisTMB->SetALCTTxDataDelay(initial_alct_txdata_delay);
  thisTMB->WriteRegister(alct_stat_adr);
  //
  if (use_measured_values_) {
    (*MyOutput_) << "Setting alct_bx0_delay and match_trig_alct_delay to measured value..." << std::endl;
    //
    thisTMB->SetAlctBx0Delay(ALCT_bx0_delay_);
    thisTMB->WriteRegister(bx0_delay_adr);
    //
    thisTMB->SetAlctVpfDelay(match_trig_alct_delay_);
    thisTMB->WriteRegister(tmbtim_adr);
    //
  } else {
    (*MyOutput_) << "Reverting to initial values of alct_bx0_delay and match_trig_alct_delay to measured value..." << std::endl;
    //
    if (initial_bxn_offset != local_tmb_bxn_offset_) {
      //
      thisTMB->SetBxnOffset(initial_bxn_offset);
      thisTMB->WriteRegister(seq_offset_adr);
      //
      // Send a BGo "Resync" to make sure that the BC0 offset has taken effect
      thisCCB_->setCCBMode(CCB::VMEFPGA);
      thisCCB_->syncReset(); 
      thisCCB_->setCCBMode(CCB::DLOG);
      //
      // put in a sleep to allow the resync to take hold...
      ::sleep(1);
    }
  }
  //
  return ALCT_bx0_delay_;
  //
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
void ChamberUtilities::QuickTimingScan() {
  //
  // If only the BC0 synchronization changes ("Time-Of-Flight parameters"), the 
  // following parameters will need to be determined in a "quick" scan
  //
  std::cout << "Quick timing Scan" << std::endl;
  //
  bool initial_use_measured_values = use_measured_values_;
  PropagateMeasuredValues(true);
  //
  // 1) Establish communication between TMB and ALCT, since the ALCT clock has changed
  ALCT_TMB_Loopback();
  //
  // 2) ALCT to TMB delays needed to synchronize the BC0 back at TMB
  ALCTBC0Scan();
  //
  // For the next couple of scans, we need the radioactive trigger to have a large L1A rate from one chamber
  //
  ::sleep(1);   //put in a sleep to recover from the previous scan
  //
  SetupRadioactiveTriggerConditions();
  //
  ::sleep(1);   //put in a sleep to allow the radioactive trigger configuration to take place...
  //
  // 3) Timing between the CLCT pretrigger and the L1A (L1A may be affected by the ALCT 
  // to TMB delays)
  MeasureTmbLctCableDelay();
  //
  // 4) Timing between the L1A at the DMB and the DAV back from the ALCT (Since delay of
  // the L1A to the ALCT has changed)
  MeasureAlctDavCableDelay();
  //
  // Go back to the initial trigger conditions...
  ReturnToInitialTriggerConditions();
  //
  PropagateMeasuredValues(initial_use_measured_values);  
  //
  return;
}
//
//----------------------------------------------
// ALCT-CLCT match timing
//----------------------------------------------
//int ChamberUtilities::FindALCTinCLCTMatchWindow() {
//  //
//  if (debug_) {
//    std::cout << "***************************" << std::endl;
//    std::cout << "Find match_trig_alct_delay:" << std::endl;
//    std::cout << "***************************" << std::endl;
//  }
//  (*MyOutput_) << "***************************" << std::endl;
//  (*MyOutput_) << "Find match_trig_alct_delay:" << std::endl;
//  (*MyOutput_) << "***************************" << std::endl;
//  //
//  const int HistoMin = 0;
//  const int HistoMax =14;
//  //
//  const int test_alct_delay_value  = 7;
//  const int test_match_window_size = 15;
//  //
//  // Set up for this test...
//  // send output to std::cout except for the essential information 
//  thisTMB->RedirectOutput(&std::cout);
//  thisDMB->RedirectOutput(&std::cout);
//  thisCCB_->RedirectOutput(&std::cout);
//  thisMPC->RedirectOutput(&std::cout);
//  //
//  // Get initial values:
//  if (debug_) std::cout << "Read TMB initial values" << std::endl;
//  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
//  int initial_alct_delay_value  = thisTMB->GetAlctVpfDelay();        //0xb2
//  int initial_match_window_size = thisTMB->GetAlctMatchWindowSize(); //0xb2
//  int initial_mpc_tx_delay      = thisTMB->GetMpcTxDelay();          //0xb2
//  //
//  const float desired_window_size = (float) initial_match_window_size;
//  //
//  // desired value should put central value of measured distribution at the following location (counting from 0)
//  float desired_value = (desired_window_size*0.5) - 0.5;
//  //
//  (*MyOutput_) << "With the current CLCT match window width setting of " << desired_window_size << "bx," << std::endl;
//  (*MyOutput_) << "the value of the ALCT to be centered in the CLCT match window should end up in bin " 
//	       << desired_value << std::endl;
//  //
//  // Enable this TMB for this test
//  if (debug_>=10) std::cout << "Initialize TMB for test" << std::endl;
//  thisTMB->SetMpcOutputEnable(1);
//  thisTMB->WriteRegister(tmb_trig_adr);
//  //
//  thisTMB->SetAlctVpfDelay(test_alct_delay_value);
//  thisTMB->SetAlctMatchWindowSize(test_match_window_size);
//  thisTMB->WriteRegister(tmbtim_adr);
//  //
//  ::usleep(500000); //time for registers to write...
//  //
//  if (debug_>=10) {
//    std::cout << "Read tmb_trig_adr " << std::hex << tmb_trig_adr
//	      << " = 0x" << thisTMB->ReadRegister(tmb_trig_adr) << std::endl;
//    std::cout << "Read tmbtim_adr " << std::hex << tmbtim_adr
//	      << " = 0x" << thisTMB->ReadRegister(tmbtim_adr) << std::endl;
//  }
//  //
//  ZeroTmbHistograms();
//  //
//  if (debug_) std::cout << "Going to read TMB " << std::dec << getNumberOfDataReads() << " times" << std::endl;
//  for (int i=0; i<getNumberOfDataReads(); i++) {
//    if ((i%25) == 0 && debug_) std::cout << "Read TMB " << i << " times" << std::endl;
//    thisTMB->TMBRawhits(getPauseBetweenDataReads());
//    int value = thisTMB->GetAlctInClctMatchWindow();
//    AlctInClctMatchWindowHisto_[value]++;
//  }
//  //
//  if (debug_>=5) std::cout << "Begin histogram analysis" << std::endl;
//  float average_value = AverageHistogram(AlctInClctMatchWindowHisto_,HistoMin,HistoMax);
//  //
//  // Print the data:
//  (*MyOutput_) << "Setting match_trig_alct_delay = " << test_alct_delay_value << " gives:" << std::endl;
//  if (debug_) std::cout << "Setting match_trig_alct_delay = " << test_alct_delay_value << " gives:" << std::endl;
//  //
//  PrintHistogram("ALCT in CLCT match window",AlctInClctMatchWindowHisto_,HistoMin,HistoMax,average_value);  
//  //
//  // amount to shift this distribution is:
//  float amount_to_shift = average_value - desired_value;
//  int int_amount_to_shift = 0;   //initialize it to zero in case amount_to_shift = 0...
//  if (amount_to_shift > 0.) {
//    int_amount_to_shift = (int) (amount_to_shift+0.5);
//  } else if (amount_to_shift < 0.) {
//    int_amount_to_shift = (int) (amount_to_shift-0.5);
//  }
//  //
//  (*MyOutput_) << "The amount to shift the distribution by is " << amount_to_shift 
//	       << " (= " << int_amount_to_shift << ")" << std::endl;
//  if (debug_) std::cout << "The amount to shift the distribution by is " << amount_to_shift 
//			<< " (= " << int_amount_to_shift << ")" << std::endl;
//  //
//  measured_match_trig_alct_delay_ = test_alct_delay_value - int_amount_to_shift;
//  //
//  // Determine new value of mpc_tx_delay, based on the prior values 
//  // of mpc_tx_delay and match_trig_alct_delay (assuming they had lined LCTs at the MPC)...
//  //
//  int amount_to_shift_mpc_tx_delay = measured_match_trig_alct_delay_ - initial_alct_delay_value;
//  //
//  // The mpc_tx_delay compensates for the change in match_trig_alct_delay, so subtract the shift:
//  measured_mpc_tx_delay_ = initial_mpc_tx_delay - amount_to_shift_mpc_tx_delay;
//  //
//  (*MyOutput_) << "Therefore, based on the current settings in the xml file:" << std::endl;
//  (*MyOutput_) << "(match_trig_window_size = " << std::dec << desired_window_size      << ")" << std::endl;
//  (*MyOutput_) << "(mpc_tx_delay           = " << std::dec << initial_mpc_tx_delay     << ")" << std::endl;
//  (*MyOutput_) << "(match_trig_alct_delay  = " << std::dec << initial_alct_delay_value << ")" << std::endl;
//  //    
//  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
//  (*MyOutput_) << "Best value is match_trig_alct_delay = " << measured_match_trig_alct_delay_ << std::endl;
//  (*MyOutput_) << "Best value is mpc_tx_delay          = " << measured_mpc_tx_delay_          << std::endl;
//  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
//  //
//  if (debug_) {  
//    std::cout << "Therefore, based on the current settings in the xml file:" << std::endl;
//    std::cout << "(match_trig_window_size = " << std::dec << desired_window_size      << ")" << std::endl;
//    std::cout << "(mpc_tx_delay           = " << std::dec << initial_mpc_tx_delay     << ")" << std::endl;
//    std::cout << "(match_trig_alct_delay  = " << std::dec << initial_alct_delay_value << ")" << std::endl;
//    std::cout << "Best value is match_trig_alct_delay = " << measured_match_trig_alct_delay_ << std::endl;
//    std::cout << "Best value is mpc_tx_delay          = " << measured_mpc_tx_delay_          << std::endl;
//  }
//  //
//  // return to initial values:
//  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
//  thisTMB->WriteRegister(tmb_trig_adr);
//  //
//  int return_value;
//  //
//  if (use_measured_values_) { 
//    //
//    (*MyOutput_) << "Setting measured values of match_trig_alct_delay and mpc_tx_delay..." << std::endl;
//    thisTMB->SetAlctVpfDelay(measured_match_trig_alct_delay_);
//    thisTMB->SetMpcTxDelay(measured_mpc_tx_delay_);
//    //
//    // if using the measured values (e.g., for automatic()), return the smaller of these two, so that a negative
//    // value will correctly stop yourself from setting a negative value and blithely continuing...
//    return_value = (measured_match_trig_alct_delay_ < measured_mpc_tx_delay_ ?   
//		    measured_match_trig_alct_delay_ : measured_mpc_tx_delay_ );  
//    //
//  } else { 
//    //
//    (*MyOutput_) << "Reverting back to original values of match_trig_alct_delay and mpc_tx_delay..." << std::endl;
//    thisTMB->SetAlctVpfDelay(initial_alct_delay_value); 
//    thisTMB->SetMpcTxDelay(initial_mpc_tx_delay);
//    //
//    //since using the (usable) xml parameters, automatic() can continue with this return value 
//    return_value = 1;                                    
//    //
//  }
//  thisTMB->SetAlctMatchWindowSize(initial_match_window_size);
//  thisTMB->WriteRegister(tmbtim_adr);
//  //
//  // send output to std::cout except for the essential information 
//  thisTMB->RedirectOutput(MyOutput_);
//  thisDMB->RedirectOutput(MyOutput_);
//  thisCCB_->RedirectOutput(MyOutput_);
//  thisMPC->RedirectOutput(MyOutput_);
//  //
//  return return_value;
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
  int analyze_up_to = DelayMax;
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
    //
    float difference = (float) desired_value - Average[delay];
    if (difference > 3.) {
      std::cout    << "We've gone past the desired value = " << desired_value << ". Stop this scan." << std::endl;
      (*MyOutput_) << "We've gone past the desired value = " << desired_value << ". Stop this scan." << std::endl;
      analyze_up_to = delay;
      delay = DelayMax+1;
    }
  }
  //
  // determine which delay value gives the reading we want.  Only analyze over the values which have been used...
  TmbLctCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,analyze_up_to,desired_value);
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
  if (fabs(difference) > 2.5) {
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
  //  ALCTL1aDelay_ = RoundOffForEvenWindowWidths(alct_average);
  //
  ALCTL1aDelay_ = RoundOff(alct_average);
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
  //  ALCTL1aDelay_ = RoundOffForEvenWindowWidths(average);
  //
  ALCTL1aDelay_ = RoundOff(average);
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
    for (int layer = 0; layer < 6; layer++ ) {
      // make a very slanted road.  In addition, for ME1/1, need to "unstagger" the strips...
      if (layer == 0) hp[layer] = HalfStrip + 4 + me11_pulsing_;
      if (layer == 1) hp[layer] = HalfStrip + 2;
      if (layer == 2) hp[layer] = HalfStrip     + me11_pulsing_;
      if (layer == 3) hp[layer] = HalfStrip - 2;
      if (layer == 4) hp[layer] = HalfStrip - 4 + me11_pulsing_;
      if (layer == 5) hp[layer] = HalfStrip - 5;
      //
      // The following is needed for NON me11 chambers
      if (me11_pulsing_ == 0) hp[layer]++;
    } 
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
void ChamberUtilities::ALCT_phase_analysis(int rxtx_timing[25][25]) {
  int i, j, k, p;  
  //
  const int max_number_of_values = 25;
  //
  if (debug_>=5) {
    std::cout << "The array you are working with is:" << std::endl;
    for (j = 0; j < max_number_of_values; j++) {
      for (k = 0; k < max_number_of_values; k++) {
	std::cout << std::setw(5) << rxtx_timing[j][k] << " ";
      }
      std::cout << std::endl;
    }
  }
  //
  //now find best position by the non-zero number that is 
  //farthest from zero in both directions
  //
  int nup, ndown, nleft, nright;
  //
  int best_element_row = 999;
  int best_element_col = 999;
  //
  const int min_up_down              = 1;   //the final value must be at least this many values away from the edge up and down
  const int min_left_right           = 1;   //the final value must have at least this many values away from the edge left and right
  int min_up_down_diff    = 999;
  int min_left_right_diff = 999;
  //
  for (j = 0; j < max_number_of_values; j++) {           //loops over rows
    for (k = 0; k < max_number_of_values; k++) {       //loops over columns
      //  
      if ( rxtx_timing[j][k] != 0) {        //find all non-zero matrix elements
	//
	if (debug_>=5) std::cout << "element (row, column) = (" << j << "," << k << ")... ";
	//
	for (i=j+1;rxtx_timing[i%max_number_of_values][k]!=0 && i<j+max_number_of_values;i++) {   //scans all non-zero element below
	}
	ndown = i-1-j;  //number of non-zero elements below
	//
	for (p=k+1;rxtx_timing[j][p%max_number_of_values]!=0 && p<k+max_number_of_values ;p++) {  //scans all non-zero elements to the right
	}
	nright = p-1-k;  //number of non-zero elements to the right
	//
	for (i=j-1;rxtx_timing[(max_number_of_values+i)%max_number_of_values][k]!=0 && i>j-max_number_of_values;i--) {   //scans all non-zero elements above
	}
	nup = j-1-i;  //number of non-zero elements above
	//
	for (p=k-1;rxtx_timing[j][(max_number_of_values+p)%max_number_of_values]!=0 && p>k-max_number_of_values;p--) {   //scans all non-zero elements to the left
	}
	nleft = k-1-p;  //number of non-zero elements to the left
	//
	int up_down_diff    = abs(ndown - nup);
	//
	if (debug_>=5) std::cout << " --> nup = " << nup << ", |up-down| = " << up_down_diff << std::endl;
	//
	if (up_down_diff < min_up_down_diff &&   // the number of elements above and below must be ~equal
	    nup          > min_up_down      &&   // must be a minimum number of spots away from the edge...
	    ndown        > min_up_down      ) {  // must be a minimum number of spots away from the edge...
	  min_up_down_diff = up_down_diff;
	  best_element_row = j;
	}
      }
    }
  }
  //
  if (debug_>=5) std::cout << "best row is " << best_element_row << " with |up-down|="<< min_up_down_diff << "... loop over this row..." << std::endl;
  //
  for (j = best_element_row; j <= best_element_row; j++) {           //loops over rows
    for (k = 0; k < max_number_of_values; k++) {       //loops over columns
      //  
      if ( rxtx_timing[j][k] != 0) {        //find all non-zero matrix elements
	//
	if (debug_>=5) std::cout << "element (row, column) = (" << j << "," << k << ")... ";
	//
	for (i=j+1;rxtx_timing[i%max_number_of_values][k]!=0 && i<j+max_number_of_values;i++) {   //scans all non-zero element below
	}
	ndown = i-1-j;  //number of non-zero elements below
	//
	for (p=k+1;rxtx_timing[j][p%max_number_of_values]!=0 && p<k+max_number_of_values ;p++) {  //scans all non-zero elements to the right
	}
	nright = p-1-k;  //number of non-zero elements to the right
	//
	for (i=j-1;rxtx_timing[(max_number_of_values+i)%max_number_of_values][k]!=0 && i>j-max_number_of_values;i--) {   //scans all non-zero elements above
	}
	nup = j-1-i;  //number of non-zero elements above
	//
	for (p=k-1;rxtx_timing[j][(max_number_of_values+p)%max_number_of_values]!=0 && p>k-max_number_of_values;p--) {   //scans all non-zero elements to the left
	}
	nleft = k-1-p;  //number of non-zero elements to the left
	//
	int left_right_diff = abs(nleft - nright);
	//
	if (debug_>=5) std::cout << " --> nleft = " << nleft << ", |left-right| = " << left_right_diff << std::endl;
	//
	if (left_right_diff < min_left_right_diff &&   // the number of elements to the left and to the right must be ~equal
	    nleft           > min_left_right      &&   // must be a minimum number of spots away from the edge...
	    nright          > min_left_right      ) {  // must be a minimum number of spots away from the edge...
	  min_left_right_diff = left_right_diff;
	  best_element_col = k;
	}
      }
    }
  }
  if (debug_>=5) std::cout << "best column is " << best_element_col << " with |left-right|=" << min_left_right_diff << std::endl;
  //
  ALCTrxPhase_ = best_element_row ;
  ALCTtxPhase_ = best_element_col ;
  //
  if (debug_>=5) {
    std::cout    << "Best value is alct_tx_clock_delay = " << std::dec << ALCTtxPhase_ << std::endl;
    std::cout    << "Best value is alct_rx_clock_delay = " << std::dec << ALCTrxPhase_ << std::endl;
    std::cout << "...due to their following analysis values... " << std::endl;
    std::cout << "|Nup - Ndown|    = " << min_up_down_diff << std::endl;
    std::cout << "|Nleft - Nright| = " << min_left_right_diff << std::endl;
  }
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
  window_counter = -1;
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
      std::cout << " from " << std::dec << (window_start[counter] % length)
		   << " to " << std::dec << (window_end[counter] % length) << std::endl;    
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
      bestValue = (((int)(average_channel[counter]+0.5)) % length);
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
  float average = -999.0;
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
      min_value = std::max(((int) (0.5 + (average - ((float) width)*0.5) ) ), min_value);  //maximum of lower value and min_value
      max_value = std::min(((int) (0.5 + (average + ((float) width)*0.5) ) ), max_value);  //minimum of lower value and min_value
      // min_value = ((int) (0.5 + (average - ((float) width)*0.5) ) ) >? min_value;  //maximum of lower value and min_value
      // max_value = ((int) (0.5 + (average + ((float) width)*0.5) ) ) <? max_value;  //minimum of lower value and min_value
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
