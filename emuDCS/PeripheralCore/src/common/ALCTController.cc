//-----------------------------------------------------------------------
// $Id: ALCTController.cc,v 3.59 2009/04/30 14:23:15 liu Exp $
// $Log: ALCTController.cc,v $
// Revision 3.59  2009/04/30 14:23:15  liu
// fix Get_InvertPulse()
//
// Revision 3.58  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 3.57  2009/03/19 13:29:42  rakness
// clean up functionality in writing to userPROMs for TMB and ALCT.  This is to fix bug introduced in TMB v3.80 by using tmb_vme_new
//
// Revision 3.56  2008/11/18 17:03:18  rakness
// include ALCT PROM readback
//
// Revision 3.55  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.54  2008/08/08 16:18:34  liu
// fix NoSetting values
//
// Revision 3.53  2008/08/08 11:01:24  rakness
// centralize logging
//
// Revision 3.52  2008/08/06 17:24:50  rakness
// add known_problem parameter to xml file; add time stamp + number of reads to config check output file
//
// Revision 3.51  2008/08/05 11:41:48  rakness
// new configuration check page + clean up output
//
// Revision 3.50  2008/08/05 08:40:36  rakness
// add minimum number of times to read when checking configuration
//
// Revision 3.49  2008/07/16 17:28:37  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 3.48  2008/07/04 14:24:29  rakness
// add getters for string values
//
// Revision 3.47  2008/06/12 21:08:55  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.46  2008/05/26 08:24:42  rakness
// for AFEB calibrations:  argument for TMB and ALCT::configure(2) to not write userPROMs; correctly respond to configuration written to broadcast slot
//
// Revision 3.45  2008/04/28 14:29:31  rakness
// Clean up firmware downloading output and synchronization parameter pages
//
// Revision 3.44  2008/04/28 12:08:39  rakness
// different number of PROMs on JTAG chain for ALCT672 compared to ALCT288 and ALCT384
//
// Revision 3.43  2008/04/19 14:56:55  rakness
// ALCT database check before loading ALCT firmware
//
// Revision 3.42  2008/04/09 15:37:24  rakness
// read ALCT fast control FPGA ID
//
// Revision 3.41  2008/01/14 18:17:26  rakness
// correct read of +5.5V_B ADC values on ALCT288
//
// Revision 3.40  2008/01/09 09:45:08  rakness
// modify AFEB mapping and accessors so that user counts from 0 to MaximumUserIndex(), no matter how the AFEBs are physically connected to the ALCT
//
// Revision 3.39  2007/10/29 13:07:20  rakness
// modify ALCT fast control ID decoding for DAQ06 format
//
// Revision 3.38  2007/10/09 11:10:35  rakness
// remove RAT and ALCT inheritance from EMUjtag, i.e., make calls to EMUjtag methods explicitly through TMB
//
// Revision 3.37  2007/07/13 11:57:48  rakness
// add read/accessors to ALCT temperature+on-board voltage
//
// Revision 3.36  2007/05/17 12:52:50  rakness
// ignore_ccb_startstop added to TMB configuration + write configuration to userPROM default
//
// Revision 3.35  2007/04/26 07:43:26  rakness
// AFEB delay chip index count from 0 for unconnected AFEBs in ME1/3,3/1,4/1
//
// Revision 3.34  2007/04/23 09:13:59  rakness
// power on AFEB from xml, pull hardcoding into EMU_JTAG_constants.h
//
// Revision 3.33  2007/04/19 16:09:18  rakness
// add accel pretrig/pattern to ALCT config reg
//
// Revision 3.32  2007/04/05 18:11:52  rakness
// ALCT 576, 192 ID labels
//
// Revision 3.31  2007/03/28 17:46:22  rakness
// xml changes:  add ALCT testpulse, remove TTCrxID
//
// Revision 3.30  2007/03/14 08:59:03  rakness
// make parser dumb
//
// Revision 3.29  2007/01/31 16:49:57  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.28  2006/11/15 10:05:49  rakness
// fix/document testpulse DAC setting routine
//
// Revision 3.27  2006/11/10 12:43:07  rakness
// include TMB/ALCT configuration and state machine prints+checks to hyperDAQ
//
// Revision 3.26  2006/10/21 09:40:46  rakness
// src/common/ALCTController.cc
//
// Revision 3.25  2006/10/19 13:39:31  rakness
// ALCT tags fixed
//
// Revision 3.24  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.23  2006/10/13 15:20:46  rakness
// Update for ME13 and ME31
//
// Revision 3.22  2006/10/12 15:56:02  rakness
// cleaned up configuration checking for ALCT/TMB
//
// Revision 3.21  2006/10/10 15:34:58  rakness
// check TMB/ALCT configuration vs xml
//
// Revision 3.20  2006/10/06 12:15:39  rakness
// expand xml file
//
// Revision 3.19  2006/10/03 08:15:37  mey
// Update
//
// Revision 3.18  2006/10/03 07:36:02  mey
// UPdate
//
// Revision 3.17  2006/09/28 12:52:34  mey
// Update
//
// Revision 3.16  2006/09/24 15:32:55  rakness
// read new alct fast control register
//
// Revision 3.15  2006/09/15 07:50:41  rakness
// dump config registers
//
// Revision 3.14  2006/09/07 15:23:04  rakness
// pull programming back into EMUjtag
//
// Revision 3.13  2006/09/06 12:38:10  rakness
// correct time stamp/copy vectors for user prom
//
// Revision 3.12  2006/09/05 10:13:17  rakness
// ALCT configure from prom
//
// Revision 3.11  2006/08/15 14:16:50  rakness
// add collision mask reg/clean up configure output
//
// Revision 3.10  2006/08/15 08:38:21  mey
// UPdate
//
// Revision 3.9  2006/08/15 08:21:43  mey
// Moved pretrig
//
// Revision 3.8  2006/08/14 16:47:40  mey
// Fixed ME1/3 bug
//
// Revision 3.7  2006/08/14 13:45:12  rakness
// upgrade ALCTnew to accomodate ALCT 192/576
//
// Revision 3.6  2006/08/11 16:23:33  rakness
// able to write TMB user prom from configure()
//
// Revision 3.5  2006/08/08 16:38:27  rakness
// ALCTnew: remove parameters not supported in firmware
//
// Revision 3.4  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.3  2006/08/03 19:00:48  mey
// Trigger on 1/1
//
// Revision 3.2  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.1  2006/07/23 15:42:51  rakness
// index Hot Channel Mask from 0
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.45  2006/07/20 11:07:59  rakness
// make many functions private
//
// Revision 2.44  2006/07/19 18:11:17  rakness
// Error checking on ALCTNEW
//
// Revision 2.43  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.42  2006/07/18 12:21:55  rakness
// ALCT threshold scan with ALCTNEW
//
// Revision 2.41  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.40  2006/07/12 15:06:59  rakness
// option for cleaned up ALCT
//
// Revision 2.39  2006/07/12 12:07:11  mey
// ALCT connectivity
//
// Revision 2.38  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.37  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 2.36  2006/06/22 13:06:14  mey
// Update
//
// Revision 2.35  2006/05/31 11:04:44  mey
// UPdate
//
// Revision 2.34  2006/05/30 07:22:20  mey
// UPdate
//
// Revision 2.33  2006/05/23 09:01:21  rakness
// Update
//
// Revision 2.32  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.31  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.30  2006/05/18 08:35:44  mey
// Update
//
// Revision 2.29  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.28  2006/04/20 15:33:54  mey
// Update
//
// Revision 2.27  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.26  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.25  2006/03/16 19:30:02  mey
// UPdate
//
// Revision 2.24  2006/03/15 16:42:57  mey
// Update
//
// Revision 2.23  2006/03/13 13:23:17  mey
// Update
//
// Revision 2.22  2006/03/10 15:55:28  mey
// Update
//
// Revision 2.21  2006/03/10 10:51:47  mey
// Update
//
// Revision 2.20  2006/03/03 07:59:19  mey
// Update
//
// Revision 2.19  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.18  2006/02/20 13:31:13  mey
// Update
//
// Revision 2.17  2006/02/07 22:49:25  mey
// UPdate
//
// Revision 2.16  2005/12/16 17:49:27  mey
// Update
//
// Revision 2.15  2005/12/10 11:24:14  mey
// Update
//
// Revision 2.14  2005/12/02 18:12:41  mey
// get rid of D360
//
// Revision 2.13  2005/11/25 23:42:17  mey
// Update
//
// Revision 2.12  2005/11/22 15:14:46  mey
// Update
//
// Revision 2.11  2005/11/21 19:47:06  mey
// Update
//
// Revision 2.10  2005/11/21 15:47:59  mey
// Update
//
// Revision 2.9  2005/11/15 15:22:32  mey
// Update
//
// Revision 2.8  2005/11/10 18:25:21  mey
// Update
//
// Revision 2.7  2005/11/09 20:07:13  mey
// Update
//
// Revision 2.6  2005/11/02 16:16:06  mey
// Update for new controller
//
// Revision 2.5  2005/11/02 10:58:20  mey
// Update bxc_offset
//
// Revision 2.4  2005/08/31 15:12:57  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.3  2005/08/15 15:37:48  mey
// Include alct_hotchannel_file
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.33  2004/09/15 21:44:42  tfcvs
// New timing routines
//
// Revision 1.32  2004/09/14 00:27:07  tfcvs
// Update files
//
// Revision 1.31  2004/08/12 18:02:09  tfcvs
// Update
//
// Revision 1.30  2004/06/05 19:34:32  tfcvs
// Reintroduced ALCT pattern file download. File configuration set through
// XML <ALCT alct_pattern_file="">. (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
#include <cstdlib>
//
#include "emu/pc/Crate.h"
#include "emu/pc/Chamber.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/TMB.h"
//
//

namespace emu {
  namespace pc {


ALCTController::ALCTController(TMB * tmb, std::string chamberType) :  EmuLogger() 
{
  tmb_ = tmb;
  //
  std::cout << "Creating ALCTController" << std::endl;
  //
  debug_ = 0;
  //
  alct_configuration_status_ = -1;
  //
  expected_fastcontrol_backward_forward_  = DO_NOT_CARE;
  expected_fastcontrol_negative_positive_ = DO_NOT_CARE;
  //
  SetChamberCharacteristics_(chamberType);
  //
  SetPowerUpTestpulsePowerSwitchReg_();
  SetPowerUpTestpulseAmplitude_();
  SetPowerUpTestpulseGroupMask_();
  SetPowerUpTestpulseStripMask_();
  SetPowerUpAfebThresholds();
  SetPowerUpStandbyRegister_();
  //
  SetPowerUpDelayLineControlReg_();
  SetPowerUpTriggerRegister_();
  SetPowerUpAsicDelays();
  SetPowerUpAsicPatterns();
  SetPowerUpConfigurationReg();
  SetPowerUpHotChannelMask();
  SetPowerUpCollisionPatternMask();
  //
  tmb_->SetCheckJtagWrite(true);
  //
}
//
ALCTController::~ALCTController() {
  //
  //
}
//
std::ostream & operator<<(std::ostream & os, ALCTController & alct) {
  //
  os << std::dec << "ALCTcontroller " << std::endl
     << "Trigger mode " << alct.write_trigger_mode_ << std::endl
     << "Ext trigger enable " << alct.write_ext_trig_enable_ << std::endl
     << "Trigger Info enable " << alct.write_trigger_info_en_ << std::endl
     << "Send empty " << alct.write_send_empty_ << std::endl
     << "L1a internal " << alct.write_l1a_internal_ << std::endl
     << "Fifo tbins " << alct.write_fifo_tbins_ << std::endl
     << "Fifo pretrig " << alct.write_fifo_pretrig_ << std::endl
     << "Fifo write mode " << alct.write_fifo_mode_ << std::endl
     << "L1a delay " << alct.write_l1a_delay_ << std::endl
     << "L1a offset " << alct.write_l1a_offset_ << std::endl
     << "L1a window " << alct.write_l1a_window_ << std::endl
     << "Nph threshold " << alct.write_nph_thresh_ << std::endl
     << "Nph pattern " << alct.write_nph_pattern_ << std::endl
     << "CCB enable " << alct.write_ccb_enable_ << std::endl
     << "Inject mode " << alct.write_inject_ << std::endl
     << "Send empty " << alct.write_send_empty_ << std::endl
     << "Drift delay " << alct.write_drift_delay_ << std::endl
     << "BXC offset " << alct.write_bxc_offset_ << std::endl
     << "Board ID " << alct.write_board_id_ << std::endl
     << "Amode " << alct.write_alct_amode_ << std::endl
     << "Serial Number Select " << alct.write_sn_select_ << std::endl;
  for(int afeb=alct.GetLowestAfebIndex(); afeb<=alct.GetHighestAfebIndex(); afeb++){
    os << "Afeb = " << afeb << " Threshold = " << alct.write_afeb_threshold_[afeb] 
       << " Delay = " << alct.write_asic_delay_[afeb] << std::endl;
  }
  //
  return os;
  //
}
//
///////////////////////////////////////////////////////////////////
// Useful methods to use ALCTController...
///////////////////////////////////////////////////////////////////
void ALCTController::SetUpPulsing(int DAC_pulse_amplitude, 
				  int which_set,
				  int mask,
				  int source){
  //
  //  long int StripMask = 0x3f;
  //  long int PowerUp   = 1 ;
  //
  //  int slot=tmb_->slot();
  //
  (*MyOutput_) << "Set up ALCT (slot " << tmb_->slot() 
	       << ") for pulsing: Amplitude=" << std::dec << DAC_pulse_amplitude << std::endl;
  //
  SetPulseDirection(which_set);
  //
  //  alct_set_test_pulse_powerup(&slot,0);
  SetTestpulsePowerSwitchReg_(OFF);
  WriteTestpulsePowerSwitchReg_();
  PrintTestpulsePowerSwitchReg_();  
  //
  //  usleep(100);   // included into WriteTestpulsePowerSwitchReg();
  //
  //  alct_set_test_pulse_amp(&slot,Amplitude);
  SetTestpulseAmplitude(DAC_pulse_amplitude);
  WriteTestpulseAmplitude_();
  //
  // usleep(100);    // included into WriteTestpulseAmplitude_();
  //
  //  alct_read_test_pulse_stripmask(&slot,&StripMask);
  //  std::cout << " StripMask = " << std::hex << StripMask << std::endl;
  //  ReadTestpulseStripMask_();
  //  PrintTestpulseStripMask_();
  //
  //  if(StripAfeb == 0 ) {
  //    alct_set_test_pulse_stripmask(&slot,0x00);
  //    alct_set_test_pulse_groupmask(&slot,0xff);
  //  }
  //  else if (StripAfeb == 1 ) {
  //    alct_set_test_pulse_stripmask(&slot,stripMask);
  //    alct_set_test_pulse_groupmask(&slot,0x00);
  //  } else {
  //    std::cout << "ALCTcontroller.SetUpPulsing : Don't know this option" <<std::endl;
  //  }
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++)
    SetTestpulseGroupMask_(group,OFF);
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++) 
    SetTestpulseStripMask_(layer,OFF);
  //
  // Choose whether you are pulsing layers with teststrips or AFEBS in groups...
  if (GetPulseDirection()==PULSE_AFEBS) {
    for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
      int off_or_on = (mask >> group) & 0x1;
      SetTestpulseGroupMask_(group,off_or_on);    
    }
  } else if (GetPulseDirection()==PULSE_LAYERS) {
    for (int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      int off_or_on = (mask >> layer) & 0x1;
      SetTestpulseStripMask_(layer,off_or_on);
    }
  } else {
    std::cout << "ALCTcontroller SetUpPulsing : Set " << GetPulseDirection() 
	      << " not available to pulse..." << std::endl;
  }
  WriteTestpulseGroupMask_();
  PrintTestpulseGroupMask_();
  //
  WriteTestpulseStripMask_();
  PrintTestpulseStripMask_();
  //
  //  alct_read_test_pulse_powerup(&slot,&PowerUp);
  //  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  //  ReadTestpulsePowerSwitchReg();
  //  PrintTestpulsePowerSwitchReg();
  //
  //  alct_fire_test_pulse('s');
  SetPulseTriggerSource_(source);
  SetInvertPulse_(OFF);
  WriteTriggerRegister_();
  PrintTriggerRegister_();
  //
  //  usleep(100);  // included into WriteTriggerRegister();
  //
  //  alct_set_test_pulse_powerup(&slot,1);
  SetTestpulsePowerSwitchReg_(ON);
  WriteTestpulsePowerSwitchReg_();
  //
  //  usleep(100); // included into WriteTestpulsePowerSwitchReg();
  //
  //  alct_read_test_pulse_powerup(&slot,&PowerUp);
  //  std::cout << " PowerUp   = " << std::hex << PowerUp << std::dec << std::endl; //11July05 DM added dec
  PrintTestpulsePowerSwitchReg_();  
  //
  return;
}
//
//
void ALCTController::SetUpRandomALCT(){
  //
  //  unsigned long HCmask[22];
  //  unsigned long HCmask2[22];
  //
  //    for (int i=0; i< 22; i++) {
  //    HCmask[i] = 0;
  //    HCmask2[i] = 0;
  //  }
  //
  //  int keyWG  = int(rand()/(RAND_MAX+0.01)*(GetWGNumber())/6/4);
  //  int keyWG2 = (GetWGNumber())/6-keyWG;
  //  int ChamberSection = GetWGNumber()/6;
  int keyWG  = int(rand()/(RAND_MAX+0.01)*(GetNumberOfChannelsInAlct())/6/4);
  int keyWG2 = (GetNumberOfChannelsInAlct())/6-keyWG;
  //
  printf("Injecting at %d and %d\n",keyWG,keyWG2);
  //
  //  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //  //
  //  std::bitset<672> bits(*HCmask) ;
  //  //
  //  for (int i=0;i<672;i++){
  //    if ( i%(GetWGNumber()/6) == keyWG ) bits.set(i);
  //    if ( i%(GetWGNumber()/6) == (GetWGNumber())/6-keyWG ) bits.set(i);
  //  }
  //  //
  //  std::bitset<32> Convert;
  //  //
  //  Convert.reset();
  //  //
  //  for (int i=0;i<(GetWGNumber());i++){
  //    if ( bits.test(i) ) Convert.set(i%32);
  //    if ( i%32 == 31 ) {
  //      HCmask[i/32] = Convert.to_ulong();
  //      Convert.reset();
  //    }
  //  }
  //
  //  alct_write_hcmask(HCmask);
  //  for(int i=0; i<22; i++) std::cout << std::hex << HCmask[i] << std::endl;
  //  alct_read_hcmask(HCmask);
  //  std::cout << std::endl;
  //  for(int i=0; i<22; i++) std::cout << std::hex << HCmask2[i] << std::endl;
    for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      for(int channel=0; channel<GetNumberOfChannelsInAlct()/6; channel++) {
	if (channel==keyWG || channel==keyWG2) {
	  SetHotChannelMask(layer,channel,ON);
	} else {
	  SetHotChannelMask(layer,channel,OFF);
	}
      }
    }
    WriteHotChannelMask();
    //    ReadHotChannelMask();
    PrintHotChannelMask();
  //
  return;
}
//
void ALCTController::configure() {
  //
  this->configure(0); //no argument means write configuration to user PROM
  //
  return;
}
//
void ALCTController::configure(int c) {
  //
  // c = 2 = do not write configuration to userPROM
  //
  tmb_->SetCheckJtagWrite(false);          //in this method we only want to write data...
  //                                       //... the settings will be verified with CheckALCTConfiguration()
  //
  if (c == 2) { 
    SetFillVmeWriteVecs(false);     //do not write configuration to user PROM
  } else {
    SetFillVmeWriteVecs(true);     //write configuration to user PROM
  }
  ClearVmeWriteVecs();
  //
  std::ostringstream dump;
  //
  dump << "ALCT : configuring in slot = " << std::dec << (int) tmb_->slot();
  //
  (*MyOutput_) << dump.str() << std::endl;
  tmb_->SendOutput(dump.str(),"INFO");
  //
  WriteStandbyRegister_();    //turn on AFEBs first to give them time to power on before thresholds and delays are written to them
  //  ReadStandbyRegister_();
  //  PrintStandbyRegister_();
  //
  WriteConfigurationReg();
  //  ReadConfigurationReg();
  //  PrintConfigurationReg();
  //
  WriteHotChannelMask();
  //  ReadHotChannelMask();
  //  PrintHotChannelMask();
  //
  WriteCollisionPatternMask();
  //  ReadCollisionPatternMask();
  //  PrintCollisionPatternMask();
  //
  WriteAfebThresholds();      //set AFEB thresholds and delay after the configuration, hot channel mask, and collision pattern
  //  ReadAfebThresholds();   //are set in order to give the AFEBs time to power on
  //  PrintAfebThresholds();
  //    
  WriteAsicDelaysAndPatterns();
  //  ReadAsicDelaysAndPatterns();
  //  PrintAsicDelays();
  //  PrintAsicPatterns();
  //
  // The following are ALCT registers which should not be changed in the configuration...
  //
  //  WriteTestpulsePowerSwitchReg_();
  //    PrintTestpulsePowerSwitchReg_();
  //
  //  WriteTestpulseAmplitude_();
  //    PrintTestpulseAmplitude_();
  //
  //  WriteTestpulseGroupMask_();
  //    PrintTestpulseGroupMask_();
  //
  //  WriteTestpulseStripMask_();
  //    PrintTestpulseStripMask_();
  //
  //  WriteDelayLineControlReg_();
  //    PrintDelayLineControlReg_();
  //
  //  WriteTriggerRegister_();
  //    PrintTriggerRegister_();
  //
  tmb_->SetCheckJtagWrite(true);                //re-enable the checking of JTAG writes (default)
  //
  // The flag to fill the VME register vector is set => program the user PROM:
  if ( GetFillVmeWriteVecs() ) 
    tmb_->CheckAndProgramProm(ChipLocationTmbUserPromALCT);
  //
  SetFillVmeWriteVecs(false);        //give VME back to the user (default)
  //
  if (tmb_->slot()<22)           //broadcast read will not work, so only check configuration if it is a normal VME slot
    this->CheckALCTConfiguration();
  //
  return;
}
//
void ALCTController::PrintALCTConfiguration() {
  //
  (*MyOutput_) << "ALCT configuration in slot " <<  (int)tmb_->slot() << std::endl;
  //
  PrintSlowControlId();
  //
  PrintFastControlId();
  //
  (*MyOutput_) << "........................ ALCT type = " << std::dec << GetNumberOfChannelsInAlct(); 
  if (GetNumberOfChannelsInAlct() == 288) {
    if (GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE) {
      (*MyOutput_) << "n";
    } else {
      (*MyOutput_) << "p";
    }
    if (GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE) {
      (*MyOutput_) << "b";
    } else {
      (*MyOutput_) << "f";
    }
  }
  (*MyOutput_) << "->" << std::endl;
  //
  (*MyOutput_) << "............ Number of Wire Groups = " << std::dec << GetNumberOfWireGroupsInChamber() << std::endl; 
  (*MyOutput_) << "........ Number of Wires per layer = " << GetNumberOfChannelsPerLayer() << std::endl;
  (*MyOutput_) << ".. Number of groups of delay chips = " << GetNumberOfGroupsOfDelayChips() << std::endl; 
  (*MyOutput_) << "Number of collision pattern groups = " << GetNumberOfCollisionPatternGroups() << std::endl;
  (*MyOutput_) << ".................. Number of AFEBs = " << GetNumberOfAfebs() << std::endl;
  (*MyOutput_) << "...hardware AFEB indices count from " << GetLowestAfebIndex() << " to " << GetHighestAfebIndex() << std::endl;
  (*MyOutput_) << "..while the user indices count from 0 to " << MaximumUserIndex() << std::endl;
  (*MyOutput_) << "which map as 0 -> " << UserIndexToHardwareIndex_(0) << " and " 
	       << MaximumUserIndex() << " -> " << UserIndexToHardwareIndex_(MaximumUserIndex()) << std::endl;
  //
  PrintAfebThresholds();
  //
  PrintStandbyRegister_();
  //
  PrintAsicDelays();
  PrintAsicPatterns();
  //
  PrintConfigurationReg();
  //
  PrintHotChannelMask();
  //
  PrintCollisionPatternMask();
  //
  return;
}
//
void ALCTController::ReadALCTConfiguration() {
  //
  ReadSlowControlId();
  //
  ReadFastControlId();
  //
  ReadAfebThresholds();
  //    
  ReadStandbyRegister_();
  //
  ReadAsicDelaysAndPatterns();
  //
  ReadConfigurationReg();
  //
  ReadHotChannelMask();
  //
  ReadCollisionPatternMask();
  //
  return;
}
//
bool ALCTController::CheckFirmwareDate() {
  //
  bool date_ok = false;
  //
  ReadFastControlId();
  //
  if (GetFastControlRegularMirrorType() == GetExpectedFastControlRegularMirrorType() &&
      GetFastControlAlctType()          == GetExpectedFastControlAlctType()          &&
      GetFastControlYear()              == GetExpectedFastControlYear()              &&
      GetFastControlMonth()             == GetExpectedFastControlMonth()             &&
      GetFastControlDay()               == GetExpectedFastControlDay()               ) {
    //
    // OK to this point... further checks for ME11...
    if ( GetChamberType().find("ME11") != std::string::npos ) {
      //
      if (GetFastControlBackwardForwardType()  == GetExpectedFastControlBackwardForwardType() &&
	  GetFastControlNegativePositiveType() == GetExpectedFastControlNegativePositiveType() ) {
	date_ok = true;  //OK if in here and ME11
      } else {
	date_ok = false; //not OK if didn't pass this check and ME11
      }
    } else {
      //
      date_ok = true;    //OK if in here and not ME11
    }
    //
  } else { 
    date_ok = false;      //didn't pass first checks
  }
  return date_ok;
}
//
void ALCTController::CheckALCTConfiguration() {
  //
  // Default number of times to read the configuration values:
  this->CheckALCTConfiguration(2); 
  //
  return;
}
//
void ALCTController::CheckALCTConfiguration(int max_number_of_reads) {
  //
  // Read the configuration values up to a maximum number of times in order to prevent
  // false failures which are simply bad VME reads.
  //
  bool config_ok = false;
  //
  number_of_configuration_reads_ = 0;
  //
  while (!config_ok && number_of_configuration_reads_ < max_number_of_reads) {
    //
    number_of_configuration_reads_++;
    //
    bool print_errors;
    //
    if (number_of_configuration_reads_ == (max_number_of_reads-1) ) {
      print_errors = true;
      //
    } else {
      print_errors = false;
      //
    }
    //
    config_ok = true;
    //
    ReadALCTConfiguration();  //fill the read values in the software
    //
    config_ok &= compareValues("ALCT Fast Control Firmware Regular/Mirror type",
			       GetFastControlRegularMirrorType(),
			       GetExpectedFastControlRegularMirrorType(),
			       print_errors);
    //
    if (chamber_type_string_ == "ME11") {
      //
      config_ok &= compareValues("ALCT Fast Control Firmware Backward/Forward type",
				 GetFastControlBackwardForwardType(),
				 GetExpectedFastControlBackwardForwardType(),
				 print_errors);
      //
      config_ok &= compareValues("ALCT Fast Control Firmware Negative/Positive type",
				 GetFastControlNegativePositiveType(),
				 GetExpectedFastControlNegativePositiveType(),
				 print_errors);
    }
    config_ok &= compareValues("ALCT Fast Control Firmware ALCT type",
			       GetFastControlAlctType(),
			       GetExpectedFastControlAlctType(),
			       print_errors);
    //
    config_ok &= compareValues("ALCT Fast Control Firmware year",
			       GetFastControlYear(),
			       GetExpectedFastControlYear(),
			       print_errors);
    //
    config_ok &= compareValues("ALCT Fast Control Firmware month",
			       GetFastControlMonth(),
			       GetExpectedFastControlMonth(),
			       print_errors);
    //
    config_ok &= compareValues("ALCT Fast Control Firmware day",
			       GetFastControlDay(),
			       GetExpectedFastControlDay(),
			       print_errors);
    //
    for (int afeb=0; afeb<=MaximumUserIndex(); afeb++) {
      // to compare write and read thresholds, we need to compare an 8-bit dac 
      // with a 10-bit adc, with an offset:
      const float offset = 24.;
      const float slope = 4.;     
      float dac_converted_to_adc = GetAfebThresholdDAC(afeb)*slope + offset;    
      //
      // set the tolerance so the adc is within +/- 3 adc counts of the set value  
      // at low dac values (<~30), this is approximately the precision of the dac/adc
      const float threshold = 6.;
      //
      std::ostringstream tested_value;
      tested_value << "AFEB threshold AnodeChannel Number " << (afeb+1);
      config_ok &= compareValues(tested_value.str(),
				 (float) GetAfebThresholdADC(afeb),
				 dac_converted_to_adc,
				 threshold,
				 print_errors);
    }
    //    
    for (int i=0; i<RegSizeAlctSlowFpga_WRT_STANDBY_REG; i++) {
      std::ostringstream tested_value;
      tested_value << "ALCT Standby Register channel " << i;
      config_ok &= compareValues(tested_value.str(),
				 read_standby_register_[i],
				 write_standby_register_[i],
				 print_errors);
    }
    //
    for (int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      for (int channel=0; channel<GetNumberOfChannelsPerLayer(); channel++) {
	std::ostringstream tested_value;
	tested_value << "ASIC Pattern Layer " << layer << " Channel " << channel;
	config_ok &= compareValues(tested_value.str(),
				   read_asic_pattern_[layer][channel],
				   write_asic_pattern_[layer][channel],
				   print_errors);
      }
    }
    //
    for (int afeb=0; afeb<=MaximumUserIndex(); afeb++) {
      std::ostringstream tested_value;
      tested_value << "AFEB delay AnodeChannel Number " << (afeb+1);
      config_ok &= compareValues(tested_value.str(),GetAsicDelay(afeb),GetWriteAsicDelay(afeb),print_errors);
    }
    //
    config_ok &= compareValues("alct_trig_mode"            ,read_trigger_mode_   ,write_trigger_mode_   ,print_errors);
    config_ok &= compareValues("alct_ext_trig_enable"      ,read_ext_trig_enable_,write_ext_trig_enable_,print_errors);
    config_ok &= compareValues("alct_send_empty"           ,read_send_empty_     ,write_send_empty_     ,print_errors);
    config_ok &= compareValues("alct_inject_mode"          ,read_inject_         ,write_inject_         ,print_errors);
    config_ok &= compareValues("alct_bxn_offset"           ,read_bxc_offset_     ,write_bxc_offset_     ,print_errors);
    config_ok &= compareValues("alct_pretrig_thresh"       ,read_nph_thresh_     ,write_nph_thresh_     ,print_errors);
    config_ok &= compareValues("alct_pattern_thresh"       ,read_nph_pattern_    ,write_nph_pattern_    ,print_errors);
    config_ok &= compareValues("alct_drift_delay"          ,read_drift_delay_    ,write_drift_delay_    ,print_errors);
    config_ok &= compareValues("alct_fifo_tbins"           ,read_fifo_tbins_     ,write_fifo_tbins_     ,print_errors);
    config_ok &= compareValues("alct_fifo_pretrig"         ,read_fifo_pretrig_   ,write_fifo_pretrig_   ,print_errors);
    config_ok &= compareValues("alct_fifo_mode"            ,read_fifo_mode_      ,write_fifo_mode_      ,print_errors);
    config_ok &= compareValues("alct_l1a_delay"            ,read_l1a_delay_      ,write_l1a_delay_      ,print_errors);
    config_ok &= compareValues("alct_l1a_window_size"      ,read_l1a_window_     ,write_l1a_window_     ,print_errors);
    config_ok &= compareValues("alct_L1a_offset"           ,read_l1a_offset_     ,write_l1a_offset_     ,print_errors);
    config_ok &= compareValues("alct_l1a_internal"         ,read_l1a_internal_   ,write_l1a_internal_   ,print_errors);
    config_ok &= compareValues("alct_Board ID (not in xml)",read_board_id_       ,write_board_id_       ,print_errors);
    config_ok &= compareValues("alct_ccb_enable"           ,read_ccb_enable_     ,write_ccb_enable_     ,print_errors);
    config_ok &= compareValues("alct_accel_mode"           ,read_alct_amode_     ,write_alct_amode_     ,print_errors);
    config_ok &= compareValues("alct_trig_info_enable"     ,read_trigger_info_en_,write_trigger_info_en_,print_errors);
    config_ok &= compareValues("alct_config_in_readout",
			       read_config_in_readout_,write_config_in_readout_,print_errors);
    config_ok &= compareValues("alct_sn_select",
			       read_sn_select_,write_sn_select_,print_errors);
    config_ok &= compareValues("alct_accel_pretrig_thresh",
			       read_accelerator_pretrig_thresh_,write_accelerator_pretrig_thresh_,print_errors);
    config_ok &= compareValues("alct_accel_pattern_thresh",
			       read_accelerator_pattern_thresh_,write_accelerator_pattern_thresh_,print_errors);
    //
    for (int layer=0; layer<MAX_NUM_LAYERS; layer++) 
      for (int channel=0; channel<GetNumberOfChannelsPerLayer(); channel++) {
	int index = layer * GetNumberOfChannelsPerLayer() + channel;
	std::ostringstream tested_value;
	tested_value << "ALCT Hot Channel Mask Layer " << layer << " Channel " << channel;
	config_ok &= compareValues(tested_value.str(),read_hot_channel_mask_[index],write_hot_channel_mask_[index],print_errors);
      }
    //
    for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) 
      for (int bitInEnvelope=0; bitInEnvelope<NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP; bitInEnvelope++) {
	int index = group * NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP + bitInEnvelope;
	std::ostringstream tested_value;
	tested_value << "ALCT Collision Pattern Mask Group " << group << " Bit " << bitInEnvelope;
	config_ok &= compareValues(tested_value.str(),
				   read_collision_pattern_mask_reg_[index],
				   write_collision_pattern_mask_reg_[index],
				   print_errors);      
      }
    //
    alct_configuration_status_ = (int) config_ok;
  }
  //
  //  ReportCheck("ALCT configuration check",config_ok);
  //
  return;
}
//
void ALCTController::SetFillVmeWriteVecs(bool fill_vectors_or_not) {
  //
  tmb_->SetALCTFillVmeWriteVecs(fill_vectors_or_not);
  //
  return;
  //
}
//
bool ALCTController::GetFillVmeWriteVecs() {
  //
  return tmb_->GetALCTFillVmeWriteVecs();
  //
}
//
void ALCTController::ClearVmeWriteVecs() {
  //
  tmb_->ClearALCTVmeWriteVecs();
  //
  return;
  //
}
///////////////////////////////////////////////////////////////////
// END:  Useful methods to use ALCTController...
///////////////////////////////////////////////////////////////////
//
///////////////////////
//SLOW CONTROL ID
///////////////////////
void ALCTController::ReadSlowControlId() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ slow control ID " << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_RD_ID_REG,
		    RegSizeAlctSlowFpga_RD_ID_REG);
  //
  tmb_->packCharBuffer(tmb_->GetDRtdo(),
		       tmb_->GetRegLength(),
		       read_slowcontrol_id_);
  //
  //
  tmb_->setup_jtag(ChainAlctSlowMezz);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowMezzProm,
		    PROMidCode,
		    RegSizeAlctSlowMezzFpga_PROMidCode);
  //
  alct_slow_prom_idcode_ = tmb_->bits_to_int(tmb_->GetDRtdo(),tmb_->GetRegLength(),0);
  //
  return;
}
//
void ALCTController::PrintSlowControlId() {
  //
  (*MyOutput_) << chamber_type_string_ 
	       << " Slow Control chip ID = " << std::hex << GetSlowControlChipId()
	       << " version " << GetSlowControlVersionId()
	       << " (day month year) = (" 
	       << GetSlowControlDay()   << " "
	       << GetSlowControlMonth() << " "
	       << GetSlowControlYear()  << ")"
	       << std::dec << std::endl; 
  //
  //  (*MyOutput_) << " PROM ID code = 0x" << std::hex << alct_slow_prom_idcode_ << std::endl;
  //
  return;
}
//
int ALCTController::GetSlowControlChipId() { 
  return (read_slowcontrol_id_[0] & 0xf); 
}
//
int ALCTController::GetSlowControlVersionId() { 
  return ((read_slowcontrol_id_[0]>>4) & 0xf); 
}
//
int ALCTController::GetSlowControlYear() { 
  return ((read_slowcontrol_id_[2]<<8) | read_slowcontrol_id_[1]&0xff); 
}
//
int ALCTController::GetSlowControlDay() { 
  return (read_slowcontrol_id_[3] & 0xff); 
}
//
int ALCTController::GetSlowControlMonth() { 
  return (read_slowcontrol_id_[4] & 0xff); 
}
//
//////////////////////////////////
//TESTPULSE POWERSWITCH REGISTER
//////////////////////////////////
void ALCTController::WriteTestpulsePowerSwitchReg_() {
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE testpulse POWERSWITCH " << write_testpulse_power_setting_ << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_WRT_TESTPULSE_POWERDOWN,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN,
		    &write_testpulse_power_setting_);
  //
  usleep(100);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadTestpulsePowerSwitchReg_();
    tmb_->CompareBitByBit(&write_testpulse_power_setting_,
			  &read_testpulse_power_setting_,
			  RegSizeAlctSlowFpga_WRT_TESTPULSE_POWERDOWN);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulsePowerSwitchReg_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ testpulse POWERSWITCH" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_RD_TESTPULSE_POWERDOWN,
		    RegSizeAlctSlowFpga_RD_TESTPULSE_POWERDOWN);
  //
  read_testpulse_power_setting_ = tmb_->bits_to_int(tmb_->GetDRtdo(),
						    tmb_->GetRegLength(),
						    LSBfirst);
  return;
}
//
void ALCTController::PrintTestpulsePowerSwitchReg_() {
  //
  (*MyOutput_) << "ALCT READ: Testpulse powerswitch = " << std::dec
	       << GetTestpulsePowerSwitchReg_() << std::endl;
  return;
}
//
void ALCTController::SetTestpulsePowerSwitchReg_(int powerswitch) { 
  //
  write_testpulse_power_setting_ = powerswitch & 0x1; 
  return;
}
//
int ALCTController::GetTestpulsePowerSwitchReg_() { 
  //
  return read_testpulse_power_setting_; 
}
//
void ALCTController::SetPowerUpTestpulsePowerSwitchReg_() { 
  //
  SetTestpulsePowerSwitchReg_(testpulse_power_setting_default); 
  return;
}
//
////////////////////////////////
//TESTPULSE AMPLITUDE REGISTER
////////////////////////////////
void ALCTController::WriteTestpulseAmplitude_() {
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE testpulse AMPLITUDE = " 
		 << write_testpulse_amplitude_dacvalue_ << std::endl;
  //
  int dac[RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC] = {};
  //
  // 14 November 2006:  looking at the AFEB testpulse response in 904 and
  // comparing with the FAST site results documented in, e.g.,
  // http://www-hep.phys.cmu.edu/cms/TALKS/teren_01_03/emu_jan_03_teren.html
  // has revealed that:
  //
  // a) this JTAG operation needs 9 bits to shift in the 8-bit DAC value 
  // b) the 9th bit which is shifted in is dummy
  //
  // Since the DAC requires MSB first, the DAC value which we want
  // to set is shifted to the left by 1:
  //
  int dac_value_to_send = write_testpulse_amplitude_dacvalue_ << 1;
  //
  //DAC's require MSB sent first....
  tmb_->int_to_bits(dac_value_to_send,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC,
		    dac,
		    MSBfirst);
  //
  //  for (int i=0; i<RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC; i++) 
  //    (*MyOutput_) << "dac[" << i << "] = " << dac[i] << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //      
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_WRT_TESTPULSE_DAC,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_DAC,
		    dac);
  usleep(100);
  //
  return;
}
//
void ALCTController::PrintTestpulseAmplitude_() {
  //
  (*MyOutput_) << "ALCT: testpulse AMPLITUDE = " << write_testpulse_amplitude_dacvalue_ << std::endl;
  //
  return;
}
//
void ALCTController::SetTestpulseAmplitude(int dacvalue) {
  //
  // 8-bit DAC controls amplitude of analog test pulse sent to AFEBs
  //
  if (dacvalue > 255) {
    (*MyOutput_) << "ALCT: ERROR testpulse amplitude maximum = 255 " << std::endl;
    return;
  } 
  //
  write_testpulse_amplitude_dacvalue_ = dacvalue & 0xff;
  //
  return;
}
//
void ALCTController:: SetPowerUpTestpulseAmplitude_() { 
  //
  SetTestpulseAmplitude(testpulse_amplitude_default); 
  return;
}
//
////////////////////////////////
//TESTPULSE GROUPMASK REGISTER
////////////////////////////////
void ALCTController::WriteTestpulseGroupMask_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE testpulse GROUPMASK" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_WRT_TESTPULSE_GRP,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP,
		    write_testpulse_groupmask_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadTestpulseGroupMask_();
    tmb_->CompareBitByBit(write_testpulse_groupmask_,
			  read_testpulse_groupmask_,
			  RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulseGroupMask_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ testpulse GROUPMASK" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_RD_TESTPULSE_GRP,
		    RegSizeAlctSlowFpga_RD_TESTPULSE_GRP);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctSlowFpga_RD_TESTPULSE_GRP; i++)
    read_testpulse_groupmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTController::PrintTestpulseGroupMask_() {
  //
  int testpulse_groupmask = tmb_->bits_to_int(read_testpulse_groupmask_,
					      RegSizeAlctSlowFpga_RD_TESTPULSE_GRP,
					      LSBfirst);
  //
  (*MyOutput_) << "ALCT READ: Testpulse groupmask = 0x" 
	       << std::hex << testpulse_groupmask << std::endl;
  return;
}
//
void ALCTController::SetTestpulseGroupMask_(int group,  
					    int mask) { 
  //
  // specify which groups of AFEBs are enabled for analog testpulsing
  //
  if (group < 0 || group >= GetNumberOfGroupsOfDelayChips()) {
    (*MyOutput_) << "SetTestpulseGroupMask: ERROR group value must be between 0 and " 
		 << std::dec << GetNumberOfGroupsOfDelayChips()-1 << std::endl;
    return;
  } 
  write_testpulse_groupmask_[group] = mask & 0x1;
  //
  return;
}
//
int ALCTController::GetTestpulseGroupMask_(int group) {
  return read_testpulse_groupmask_[group]; 
}
//
void ALCTController::SetPowerUpTestpulseGroupMask_() {
  //
  for (int group=0; group<RegSizeAlctSlowFpga_WRT_TESTPULSE_GRP; group++)
    write_testpulse_groupmask_[group] = testpulse_groupmask_default;
  return;
}
//
/////////////////////////////////
//TESTPULSE STRIPMASK REGISTER
/////////////////////////////////
void ALCTController::WriteTestpulseStripMask_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE testpulse STRIPMASK" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_WRT_TESTPULSE_STRIP,
		    RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP,
		    write_testpulse_stripmask_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadTestpulseStripMask_();
    tmb_->CompareBitByBit(write_testpulse_stripmask_,
			  read_testpulse_stripmask_,
			  RegSizeAlctSlowFpga_WRT_TESTPULSE_STRIP);
  }
  //
  return;
}
//
void ALCTController::ReadTestpulseStripMask_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ testpulse STRIPMASK" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_RD_TESTPULSE_STRIP,
		    RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP; i++)
    read_testpulse_stripmask_[i] = *(register_pointer+i);
  //
  return;
}
//
void ALCTController::PrintTestpulseStripMask_() {
  //
  int testpulse_stripmask = tmb_->bits_to_int(read_testpulse_stripmask_,
					      RegSizeAlctSlowFpga_RD_TESTPULSE_STRIP,
					      LSBfirst);
  //
  (*MyOutput_) << "ALCT READ: testpulse stripmask = 0x" 
	       << std::hex << testpulse_stripmask << std::endl;
  return;
}
//
void ALCTController::SetTestpulseStripMask_(int layer,
					    int mask) {
  //
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetTestpulseStripMask: ERROR layer number must be between 0 and " 
		 << std::dec << MAX_NUM_LAYERS-1 << std::endl;
    return;
  } 
  //
  write_testpulse_stripmask_[layer] = mask & 0x1;
  //
  return;
}
//
int ALCTController::GetTestpulseStripMask_(int layer) {
  //
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "GetTestpulseStripMask: ERROR layer number must be between 0 and " 
		 << std::dec << MAX_NUM_LAYERS-1 << std::endl;
    return 999;
  } 
  //
  return read_testpulse_stripmask_[layer]; 
}
//
void ALCTController::SetPowerUpTestpulseStripMask_() {
  //
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    write_testpulse_stripmask_[layer] = testpulse_stripmask_default;
  //
  return;
}
//
void ALCTController::Set_PulseDirection(std::string afebs_or_strips) {
  //
  if (afebs_or_strips == "afebs") {
    SetPulseDirection(PULSE_AFEBS);
  } else if (afebs_or_strips == "strips") {
    SetPulseDirection(PULSE_LAYERS);
  }
  return;
}
//
std::string ALCTController::Get_PulseDirection() {
  //
  std::string return_value = "";
  //
  if ( GetPulseDirection() == PULSE_AFEBS ) 
    return_value = "afebs";
  //
  if ( GetPulseDirection() == PULSE_LAYERS )
    return_value = "strips";
  //
  return return_value;
}
//
//////////////////
//AFEB THRESHOLDS
//////////////////
void ALCTController::WriteAfebThresholds() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE afeb THRESHOLDS " << std::endl;
  //
  for (int afebChannel=0; afebChannel<=MaximumUserIndex(); afebChannel++) {
    //
    // Mapping of AFEB channel picks chip through the opcode, using the correct AFEB indexing...
    int opcode = ALCT_SLOW_WRT_THRESH_DAC0 + afeb_dac_chip[UserIndexToHardwareIndex_(afebChannel)];
    //
    // ..... and the DAC channel through TDI, again using the correct AFEB indexing 
    // (N.B. the Get..DAC() method already will access the correct threshold)
    int data_to_send = 
      ( (afeb_dac_channel[UserIndexToHardwareIndex_(afebChannel)]<<8) & 0xf00 ) | GetAfebThresholdDAC(afebChannel) & 0xff;
    if (debug_)
      (*MyOutput_) << "User AFEB" << std::dec << afebChannel 
		   << " writes to hardware AFEB " << UserIndexToHardwareIndex_(afebChannel)
		   << " -> Data to send = " << std::hex << data_to_send 
		   << std::dec << std::endl;
    //
    int vector_of_data[RegSizeAlctSlowFpga_WRT_THRESH_DAC0] = {}; //all DAC's have same size register
    //
    tmb_->int_to_bits(data_to_send,
		      RegSizeAlctSlowFpga_WRT_THRESH_DAC0,
		      vector_of_data,
		      MSBfirst);
    //
    tmb_->setup_jtag(ChainAlctSlowFpga);
    //
    tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		      opcode,
		      RegSizeAlctSlowFpga_WRT_THRESH_DAC0,
		      vector_of_data);
  }
  return;
}
//
void ALCTController::ReadAfebThresholds() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ afeb THRESHOLDS " << std::endl;
  //
  // Fill the vector of read values indexed by the AFEB indexing, accessing the ADC values connected to the right AFEB
  //
  for (int afeb=0; afeb<=MaximumUserIndex(); afeb++)
    read_afeb_threshold_[UserIndexToHardwareIndex_(afeb)] = 
      read_adc_( afeb_adc_chip[UserIndexToHardwareIndex_(afeb)], afeb_adc_channel[UserIndexToHardwareIndex_(afeb)] );
  //
  return;
}
//
void ALCTController::PrintAfebThresholds() {
  //
  for (int afeb=0; afeb<=MaximumUserIndex(); afeb++) 
    (*MyOutput_) << "AFEB " << std::setw(2) << std::dec << afeb
		 << " write threshold DAC = " << std::setw(3) << GetAfebThresholdDAC(afeb)
      //		 << std::endl;
  		 << " -> read threshold ADC = " << GetAfebThresholdADC(afeb)
  		 << " = " << std::setprecision(3) << std::fixed << GetAfebThresholdVolts(afeb) << " V" 
  		 << std::endl;
  return;
}
//
void ALCTController::SetAfebThreshold(int afebChannel, int dacvalue) {
  // 8-bit DAC controls threshold on each AFEB
  //  Voltage = 2.5V * dacvalue/256
  //
  if (dacvalue > 255) {
    (*MyOutput_) << "SetAfebThreshold: ERROR threshold maximum = 255 " << std::endl;
    return;
  } 
  //
  if ( afebChannel<0 || afebChannel>MaximumUserIndex() ) {
    (*MyOutput_) << "SetAfebThreshold: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << MaximumUserIndex()
		 << std::endl;
    return;
  } 
  //
  write_afeb_threshold_[UserIndexToHardwareIndex_(afebChannel)] = dacvalue & 0xff;
  //
  return;
}
//
void ALCTController::ReadAlctTemperatureAndVoltages() {
  //
  if (debug_) 
    (*MyOutput_) << "ALCT: READ ALCT Temperature and on-board voltages... " << std::endl;
  //
  int read_alct_1p8_current_adc = read_adc_(Current1p8_adc_chip,Current1p8_adc_channel);
  read_alct_1p8_current_        = ConvertADCtoCurrent_(read_alct_1p8_current_adc);    
  if (debug_) {
    (*MyOutput_) << "1.8V I ADC = " << std::dec << read_alct_1p8_current_adc << std::endl;
    (*MyOutput_) << "1.8V I     = "             << read_alct_1p8_current_    << std::endl;
  }
  //
  int read_alct_3p3_current_adc = read_adc_(Current3p3_adc_chip,Current3p3_adc_channel);
  read_alct_3p3_current_        = ConvertADCtoCurrent_(read_alct_3p3_current_adc);    
  if (debug_) {
    (*MyOutput_) << "3.3V I ADC = " << std::dec << read_alct_3p3_current_adc << std::endl;
    (*MyOutput_) << "3.3V I     = "             << read_alct_3p3_current_    << std::endl;
  }
  //
  int read_alct_5p5a_current_adc = read_adc_(Current5p5_1_adc_chip,Current5p5_1_adc_channel);
  read_alct_5p5a_current_        = ConvertADCtoCurrent_(read_alct_5p5a_current_adc);
  if (debug_) {
    (*MyOutput_) << "5.5VA I ADC = " << std::dec << read_alct_5p5a_current_adc << std::endl;
    (*MyOutput_) << "5.5VA I     = "             << read_alct_5p5a_current_    << std::endl;
  }
  //
  int read_alct_5p5b_current_adc = read_adc_(Current5p5_2_adc_chip,Current5p5_2_adc_channel);
  read_alct_5p5b_current_         = ConvertADCtoCurrent_(read_alct_5p5b_current_adc);
  if (debug_) {
    (*MyOutput_) << "5.5V2 I ADC = " << std::dec << read_alct_5p5b_current_adc << std::endl;
    (*MyOutput_) << "5.5V2 I     = "             << read_alct_5p5b_current_    << std::endl;
  }
  //
  int read_alct_1p8_voltage_adc = read_adc_(Voltage1p8_adc_chip  ,Voltage1p8_adc_channel  );
  read_alct_1p8_voltage_        = ConvertADCtoVoltage_(read_alct_1p8_voltage_adc);    
  if (debug_) {
    (*MyOutput_) << "1.8V V ADC = " << std::dec << read_alct_1p8_voltage_adc    << std::endl;
    (*MyOutput_) << "1.8V V     = "             << read_alct_1p8_voltage_       << std::endl;
  }
  //
  int read_alct_3p3_voltage_adc = read_adc_(Voltage3p3_adc_chip  ,Voltage3p3_adc_channel  );
  read_alct_3p3_voltage_        = ConvertADCtoVoltage_(read_alct_3p3_voltage_adc);    
  if (debug_) {
    (*MyOutput_) << "3.3V V ADC = " << std::dec << read_alct_3p3_voltage_adc    << std::endl;
    (*MyOutput_) << "3.3V       = "             << read_alct_3p3_voltage_       << std::endl;
  }
  //
  int read_alct_5p5a_voltage_adc = read_adc_(Voltage5p5_1_adc_chip,Voltage5p5_1_adc_channel);
  read_alct_5p5a_voltage_        = ConvertADCtoVoltage_(read_alct_5p5a_voltage_adc);
  if (debug_) {
    (*MyOutput_) << "5.5VA V ADC = " << std::dec << read_alct_5p5a_voltage_adc << std::endl;
    (*MyOutput_) << "5.5V1 V     = "             << read_alct_5p5a_voltage_     << std::endl;
  }
  //
  int read_alct_5p5b_voltage_adc = read_adc_(Voltage5p5_2_adc_chip,Voltage5p5_2_adc_channel);
  read_alct_5p5b_voltage_        = ConvertADCtoVoltage_(read_alct_5p5b_voltage_adc);
  if (debug_) {
    (*MyOutput_) << "5.5V2 V ADC = " << std::dec << read_alct_5p5b_voltage_adc << std::endl;
    (*MyOutput_) << "5.5V2 V     = "             << read_alct_5p5b_voltage_    << std::endl;
  }
  //
  int read_alct_temperature_adc  = read_adc_(Temperature_adc_chip ,Temperature_adc_channel );
  read_alct_temperature_celcius_ = ConvertADCtoTemperature_(read_alct_temperature_adc);
  if (debug_) {
    (*MyOutput_) << "Temp ADC     = " << std::dec << read_alct_temperature_adc << std::endl;
    (*MyOutput_) << "Temp Celcius = " << read_alct_temperature_celcius_ << std::endl;
  }  
  //
  return;
}
//
void ALCTController::PrintAlctTemperature() {
  //
  (*MyOutput_) << "ALCT Temperature =  " << std::setprecision(1) << std::fixed 
	       << GetAlctTemperatureCelcius() << " C" << std::endl;
  return;
}
//
float ALCTController::GetAlct_5p5b_Voltage() { 
  //
  // Since there are so few AFEBs on ALCT288 boards, they are powered from the +5.5V_A line only.
  // Therefore, the +5.5V_B ADC inputs on these boards are shorted to ground, hence, the ADC 
  // values are not useful
  //
  float value_to_return = 99998.;
  //
  if ( GetNumberOfAfebs() > 18 ) 
    value_to_return = read_alct_5p5b_voltage_; 
  //
  return value_to_return;
} 
//
float ALCTController::GetAlct_5p5b_Current() { 
  //
  // Since there are so few AFEBs on ALCT288 boards, they are powered from the +5.5V_A line only.
  // Therefore, the +5.5V_B ADC inputs on these boards are shorted to ground, hence, the ADC 
  // values are not useful
  //
  float value_to_return = 99998.;
  //
  if ( GetNumberOfAfebs() > 18 ) 
    value_to_return = read_alct_5p5b_current_; 
  //
  return value_to_return;
} 
//
int ALCTController::GetAfebThresholdDAC(int afebChannel) { 
  //
  return write_afeb_threshold_[UserIndexToHardwareIndex_(afebChannel)]; 
}
//
int ALCTController::GetAfebThresholdADC(int afebChannel) { 
  //   
  return read_afeb_threshold_[UserIndexToHardwareIndex_(afebChannel)]; 
}
//
float ALCTController::GetAfebThresholdVolts(int afebChannel) {
  //
  return ConvertADCtoVoltage_(GetAfebThresholdADC(afebChannel)); 
}
//
float ALCTController::ConvertADCtoVoltage_(int adc_value) {
  //
  // From the ALCT schematics, the reference voltage for the ADC is +1.225V.
  const float reference_voltage = 1.225;
  //
  // There is a resistor divider so that the maximum value (~5.5V) can 
  // be measured appropriately by the ADC:
  const float r1 = 1.1;
  const float r2 = 4.3;
  //
  // 10 bit adc:
  const float max_adc_value = 1023.;
  //
  // So the ADC value in volts is:
  float volts = (float)adc_value * (reference_voltage / max_adc_value) * ((r1 + r2) / r1); 
  return volts;
}
//
float ALCTController::ConvertADCtoCurrent_(int adc_value) {
  //
  // From the ALCT schematics, the reference voltage for the ADC is +1.225V.
  const float reference_voltage = 1.225;
  //
  // 10 bit adc:
  const float max_adc_value = 1023.;
  //
  // So the ADC value in volts is:
  float volts_into_adc = (float)adc_value * (reference_voltage / max_adc_value);
  //
  // The voltage into the ADC has arrived via an op-amp with a 2kohm resistor in parallel:
  float resistor_in_parallel = 2000.;
  float current_in_parallel  = volts_into_adc / resistor_in_parallel;
  //
  // ... which makes the voltage into the 49.9ohm resistor in front be...
  float resistor_in_front    = 49.9;
  float volts_into_front     = current_in_parallel * resistor_in_front;
  //
  // Finally, the current into the 49.9ohm resistor had gone through a small resistor
  // before going into the op-amp circuitry:
  float small_resistor = 0.01;
  float current = volts_into_front / small_resistor;
  //
  return current;
}
//
float ALCTController::ConvertADCtoTemperature_(int adc_value) {
  //
  // From the ALCT schematics, the reference voltage for the ADC is +1.225V.
  const float reference_voltage = 1.225;
  //
  // 10 bit adc:
  const float max_adc_value = 1023.;
  //
  float volts = (float)adc_value * (reference_voltage / max_adc_value);
  //
  // The device reads temperature with an offset of 25degC = 0.750V and a slope of 0.010V/degC
  float slope  = (1. / 0.010);
  float offset = (25 - 0.750*slope);
  // 
  float temperature = offset + slope*volts;
  return temperature;
}
//
int ALCTController::read_adc_(int chip, int channel) {
  // Pick chip through the opcode:
  int opcode = ALCT_SLOW_RD_THRESH_ADC0 + chip;
  //
  // Specify ADC channel in TDI:
  int data_to_send = (channel&0xf) << 7; // N.B. one more bit than what is specified in ALCT documentation
  //
  int address[RegSizeAlctSlowFpga_RD_THRESH_ADC0] = {}; //all ADC's have same size register
  //
  tmb_->int_to_bits(data_to_send,
		    RegSizeAlctSlowFpga_RD_THRESH_ADC0,
		    address,
		    MSBfirst);
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);
  //
  //
  // Need to ShfDR in two times: 
  //   -> First time to shift in the channel on tdi
  if (debug_) (*MyOutput_) << "Shift in channel value..." << std::endl;
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    opcode,
		    RegSizeAlctSlowFpga_RD_THRESH_ADC0,
		    address);
  //
  // Give adc time to receive address:
  ::usleep(200);
  //
  //   -> Second time to get the data on tdo
  if (debug_) (*MyOutput_) << "Get ADC data..." << std::endl;
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    opcode,
		    RegSizeAlctSlowFpga_RD_THRESH_ADC0,
		    address);
  //
  int adcValue = tmb_->bits_to_int(tmb_->GetDRtdo(),
				   RegSizeAlctSlowFpga_RD_THRESH_ADC0,
				   MSBfirst);
  //
  if (debug_) (*MyOutput_) << "ADC value = 0x" << std::hex << adcValue << std::endl;
  //
  return adcValue;
}
//
void ALCTController::SetPowerUpAfebThresholds() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++) 
    write_afeb_threshold_[afeb] = afeb_threshold_default;
  //
  return;
}
//
////////////////////////
//STANDBY REGISTER
////////////////////////
void ALCTController::WriteStandbyRegister_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE standby register" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);  
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_WRT_STANDBY_REG,
		    RegSizeAlctSlowFpga_WRT_STANDBY_REG,
		    write_standby_register_);
  usleep(100);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadStandbyRegister_();
    tmb_->CompareBitByBit(write_standby_register_,
			  read_standby_register_,
			  RegSizeAlctSlowFpga_WRT_STANDBY_REG);
  }
  //
  return;
}
//
void ALCTController::ReadStandbyRegister_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ Standby Register" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctSlowFpga);  
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctSlowFpga,
		    ALCT_SLOW_RD_STANDBY_REG,
		    RegSizeAlctSlowFpga_RD_STANDBY_REG);
  //
  int * standby_reg_pointer = tmb_->GetDRtdo();
  //
  for (int i=0; i<RegSizeAlctSlowFpga_RD_STANDBY_REG; i++)
    read_standby_register_[i] = *(standby_reg_pointer+i);
  //
  return;
}
//
void ALCTController::PrintStandbyRegister_() {
  //
  const int buffersize = RegSizeAlctSlowFpga_RD_STANDBY_REG/8;
  char tempBuffer[buffersize] = {};
  tmb_->packCharBuffer(read_standby_register_,
		       RegSizeAlctSlowFpga_RD_STANDBY_REG,
		       tempBuffer);
  //
  (*MyOutput_) << "ALCT: Standby Register (right to left)= ";
  for (int i=buffersize; i>=0; i--) {
    if (i == buffersize) {
      (*MyOutput_) << std::hex << (tempBuffer[i] & 0x03) << " ";  //register is 42 bits long
    } else {
      (*MyOutput_) << std::hex << (tempBuffer[i]>>4 & 0xf) 
		   << (tempBuffer[i] & 0xf) << " "; 
    }
  }
  (*MyOutput_) << std::dec <<std::endl;
  //
  return;
}
//
void ALCTController::SetStandbyRegister_(int afebChannel, 
					 int powerswitch) {
  //
  if ( afebChannel<0 || afebChannel>MaximumUserIndex() ) {
    (*MyOutput_) << "Set Standby Register: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 " << " and " << std::dec << MaximumUserIndex()
		 << std::endl;
    return;
  } 
  //
  write_standby_register_[UserIndexToHardwareIndex_(afebChannel)] = powerswitch & 0x1;
  //
  return;
}
//
int ALCTController::GetStandbyRegister_(int afebChannel) {
  //
  return read_standby_register_[UserIndexToHardwareIndex_(afebChannel)]; 
}
//
void ALCTController::SetPowerUpStandbyRegister_() {
  //
  for (int afeb=0; afeb<RegSizeAlctSlowFpga_RD_STANDBY_REG; afeb++) 
    write_standby_register_[afeb] = standby_register_default;
  //
  return;
}
//
//////////////////////////////
// FAST CONTROL ID
//////////////////////////////
void ALCTController::ReadFastControlId() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ fast control ID " << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_ID_REG,
		    RegSizeAlctFastFpga_RD_ID_REG);
  //
  int * fast_control_id_reg_pointer = tmb_->GetDRtdo();
  //
  for (int i=0; i<RegSizeAlctFastFpga_RD_ID_REG; i++)
    read_fastcontrol_id_[i] = *(fast_control_id_reg_pointer+i);
  //
  DecodeFastControlId_();
  //
  ReadFastControlMezzIDCodes();
  //
  return;
}
//
void ALCTController::PrintFastControlId() {
  //
  (*MyOutput_) << chamber_type_string_;
  //
  (*MyOutput_) << ", type "; 
  if ( GetFastControlAlctType() == FIRMWARE_TYPE_192 ) {
    (*MyOutput_) << "192";
  } else if ( GetFastControlAlctType() == FIRMWARE_TYPE_288 ) {
    (*MyOutput_) << "288";
  } else if ( GetFastControlAlctType() == FIRMWARE_TYPE_384 ) {
    (*MyOutput_) << "384";
  } else if ( GetFastControlAlctType() == FIRMWARE_TYPE_576 ) {
    (*MyOutput_) << "576";
  } else if ( GetFastControlAlctType() == FIRMWARE_TYPE_672 ) {
    (*MyOutput_) << "672";
  } else {
    (*MyOutput_) << "unknown";
  }
  // 
  if ( GetFastControlRegularMirrorType() == REGULAR_FIRMWARE_TYPE ) {
    (*MyOutput_) << " [non-mirrored] ";
  } else  if ( GetFastControlRegularMirrorType() == MIRROR_FIRMWARE_TYPE ) {
    (*MyOutput_) << " [mirrored] ";
  } else {
    (*MyOutput_) << " [unknown] ";
  }
  //
  if (chamber_type_string_ == "ME11") {
    if ( GetFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE ) {
      (*MyOutput_) << "backward/";
    } else if ( GetFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE ) {
      (*MyOutput_) << "forward/";
    }
    if ( GetFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
      (*MyOutput_) << "negative ";
    } else if ( GetFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
      (*MyOutput_) << "positive ";
    }
  }
  //
  (*MyOutput_) << " --> version (day month year) = (";
  (*MyOutput_) << std::dec << GetFastControlDay() << " ";
  (*MyOutput_) << std::dec << GetFastControlMonth() << " ";
  (*MyOutput_) << std::dec << GetFastControlYear() << ")" << std::endl;
  //
  // pre-DAQ06 format:
  //(*MyOutput_) << std::hex << GetFastControlDay();
  //(*MyOutput_) << std::hex << GetFastControlMonth();
  //(*MyOutput_) << std::hex << GetFastControlYear() << ")" << std::dec << std::endl; 
  //
  //  (*MyOutput_) << " FPGA, PROM0, PROM1 ID = 0x" 
  //	       << std::hex << GetFastControlFPGAIdCode() << ", "
  //	       << std::hex << alct_prom0_idcode_ << ", "
  //	       << std::hex << alct_prom1_idcode_ << std::endl;
  //
  return;
}
//
void ALCTController::DecodeFastControlId_() {
  // ** Extract the Fast Control ID's software values  **
  // ** from the vector of bits read_fastcontrol_id_[]          **
  //
  int fastcontrol_regular_mirror_bitlo    = fastcontrol_regular_mirror_bitlo_daq06_format   ;
  int fastcontrol_regular_mirror_bithi    = fastcontrol_regular_mirror_bithi_daq06_format   ;
  //
  int fastcontrol_backward_forward_bitlo  = fastcontrol_backward_forward_bitlo_daq06_format ;
  int fastcontrol_backward_forward_bithi  = fastcontrol_backward_forward_bithi_daq06_format ;
  //
  int fastcontrol_negative_positive_bitlo = fastcontrol_negative_positive_bitlo_daq06_format;
  int fastcontrol_negative_positive_bithi = fastcontrol_negative_positive_bithi_daq06_format;
  //
  int fastcontrol_alct_type_bitlo         = fastcontrol_alct_type_bitlo_daq06_format        ;
  int fastcontrol_alct_type_bithi         = fastcontrol_alct_type_bithi_daq06_format        ;
  //
  int fastcontrol_firmware_year_bitlo     = fastcontrol_firmware_year_bitlo_daq06_format    ;
  int fastcontrol_firmware_year_bithi     = fastcontrol_firmware_year_bithi_daq06_format    ;
  //
  int fastcontrol_firmware_day_bitlo      = fastcontrol_firmware_day_bitlo_daq06_format     ;
  int fastcontrol_firmware_day_bithi      = fastcontrol_firmware_day_bithi_daq06_format     ;
  //
  int fastcontrol_firmware_month_bitlo    = fastcontrol_firmware_month_bitlo_daq06_format   ;
  int fastcontrol_firmware_month_bithi    = fastcontrol_firmware_month_bithi_daq06_format   ;
  //
  // pre-DAQ06 format:
  //int fastcontrol_regular_mirror_bitlo    = fastcontrol_regular_mirror_bitlo_predaq06_format   ;
  //int fastcontrol_regular_mirror_bithi    = fastcontrol_regular_mirror_bithi_predaq06_format   ;
  //
  //int fastcontrol_backward_forward_bitlo  = fastcontrol_backward_forward_bitlo_predaq06_format ;
  //int fastcontrol_backward_forward_bithi  = fastcontrol_backward_forward_bithi_predaq06_format ;
  //
  //int fastcontrol_negative_positive_bitlo = fastcontrol_negative_positive_bitlo_predaq06_format;
  //int fastcontrol_negative_positive_bithi = fastcontrol_negative_positive_bithi_predaq06_format;
  //
  //int fastcontrol_alct_type_bitlo         = fastcontrol_alct_type_bitlo_predaq06_format        ;
  //int fastcontrol_alct_type_bithi         = fastcontrol_alct_type_bithi_predaq06_format        ;
  //
  //int fastcontrol_firmware_year_bitlo     = fastcontrol_firmware_year_bitlo_predaq06_format    ;
  //int fastcontrol_firmware_year_bithi     = fastcontrol_firmware_year_bithi_predaq06_format    ;
  //
  //int fastcontrol_firmware_day_bitlo      = fastcontrol_firmware_day_bitlo_predaq06_format     ;
  //int fastcontrol_firmware_day_bithi      = fastcontrol_firmware_day_bithi_predaq06_format     ;
  //
  //int fastcontrol_firmware_month_bitlo    = fastcontrol_firmware_month_bitlo_predaq06_format   ;
  //int fastcontrol_firmware_month_bithi    = fastcontrol_firmware_month_bithi_predaq06_format   ;
  //
  int number_of_bits = fastcontrol_regular_mirror_bithi - fastcontrol_regular_mirror_bitlo + 1;  
  fastcontrol_regular_mirror_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_regular_mirror_bitlo,
						  number_of_bits,
						  LSBfirst);
  //
  number_of_bits = fastcontrol_backward_forward_bithi - fastcontrol_backward_forward_bitlo + 1;  
  fastcontrol_backward_forward_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_backward_forward_bitlo,
						    number_of_bits,
						    LSBfirst);
  //
  number_of_bits = fastcontrol_negative_positive_bithi - fastcontrol_negative_positive_bitlo + 1;  
  fastcontrol_negative_positive_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_negative_positive_bitlo,
						     number_of_bits,
						     LSBfirst);
  //
  number_of_bits = fastcontrol_alct_type_bithi - fastcontrol_alct_type_bitlo + 1;  
  fastcontrol_alct_type_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_alct_type_bitlo,
					     number_of_bits,
					     LSBfirst);
  //
  number_of_bits = fastcontrol_firmware_year_bithi - fastcontrol_firmware_year_bitlo + 1;  
  fastcontrol_firmware_year_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_firmware_year_bitlo,
						 number_of_bits,
						 LSBfirst);
  //
  number_of_bits = fastcontrol_firmware_day_bithi - fastcontrol_firmware_day_bitlo + 1;  
  fastcontrol_firmware_day_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_firmware_day_bitlo,
						number_of_bits,
						LSBfirst);
  //
  number_of_bits = fastcontrol_firmware_month_bithi - fastcontrol_firmware_month_bitlo + 1;  
  fastcontrol_firmware_month_ = tmb_->bits_to_int(read_fastcontrol_id_+fastcontrol_firmware_month_bitlo,
						  number_of_bits,
						  LSBfirst);
  
  return;
}
//
int ALCTController::GetFastControlRegularMirrorType() { 
  return fastcontrol_regular_mirror_; 
}
//
void ALCTController::Set_fastcontrol_backward_forward_type(std::string alct_firmware_backwardForward) {
  //
  if (alct_firmware_backwardForward == "b") 
    SetExpectedFastControlBackwardForwardType(BACKWARD_FIRMWARE_TYPE);
  if (alct_firmware_backwardForward == "f") 
    SetExpectedFastControlBackwardForwardType(FORWARD_FIRMWARE_TYPE);
  //
  return;
}
//
std::string ALCTController::Get_fastcontrol_backward_forward_type() {
  //
  std::string return_value = "";
  //
  if ( GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE )
    return_value = "b";
  //
  if ( GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE )
    return_value = "f"; 
  //
  return return_value;
}
//
//
int ALCTController::GetFastControlBackwardForwardType() { 
  return fastcontrol_backward_forward_; 
}
//
void ALCTController::Set_fastcontrol_negative_positive_type(std::string alct_firmware_negativePositive) {
  if (alct_firmware_negativePositive == "n") 
    SetExpectedFastControlNegativePositiveType(NEGATIVE_FIRMWARE_TYPE);
  if (alct_firmware_negativePositive == "p") 
    SetExpectedFastControlNegativePositiveType(POSITIVE_FIRMWARE_TYPE);
  //
  return;
}
//
std::string ALCTController::Get_fastcontrol_negative_positive_type() {
  //
  std::string return_value = "";
  //
  if ( GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE )
    return_value = "n";
  //
  if ( GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE )
    return_value = "p"; 
  //
  return return_value;
}
//
int ALCTController::GetFastControlNegativePositiveType() { 
  return fastcontrol_negative_positive_; 
}
//
int ALCTController::GetFastControlAlctType() { 
  return fastcontrol_alct_type_; 
}
//
int ALCTController::GetFastControlYear() { 
  //
  return fastcontrol_firmware_year_;
    //((read_fastcontrol_id_[2]<<8) | read_fastcontrol_id_[1]&0xff); 
}
//
int ALCTController::GetFastControlDay() { 
  //
  return fastcontrol_firmware_day_;
    //(read_fastcontrol_id_[3] & 0xff); 
}
//
int ALCTController::GetFastControlMonth() { 
  //
  return fastcontrol_firmware_month_;
    //(read_fastcontrol_id_[4] & 0xff); 
}
//

void ALCTController::SetExpectedFastControlYear(int firmware_year) { 
  //
  expected_fastcontrol_firmware_year_ = firmware_year; 
  //
  // pre-DAQ06 format:
  //expected_fastcontrol_firmware_year_ = tmb_->ConvertToHexAscii(firmware_year); 
  //
  return;
} 
//
void ALCTController::SetExpectedFastControlDay(int firmware_day) { 
  //
  expected_fastcontrol_firmware_day_ = firmware_day; 
  //
  // pre-DAQ06 format:
  //expected_fastcontrol_firmware_day_ = tmb_->ConvertToHexAscii(firmware_day); 
  //
  return;
} 
//
void ALCTController::SetExpectedFastControlMonth(int firmware_month) { 
  //
  expected_fastcontrol_firmware_month_ = firmware_month; 
  //
  // pre-DAQ06 format:
  //expected_fastcontrol_firmware_month_ = tmb_->ConvertToHexAscii(firmware_month); 
  //
  return;
} 
//
void ALCTController::ReadFastControlMezzIDCodes() {
  //
  tmb_->setup_jtag(ChainAlctFastMezz);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastMezzFpga,
		    FPGAidCode,
		    RegSizeAlctFastMezzFpga_FPGAidCode);  
  //
  alct_fpga_idcode_ = (tmb_->bits_to_int(tmb_->GetDRtdo(),tmb_->GetRegLength(),0) ) & 0xfffffff;
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastMezzProm0,
		    PROMidCode,
		    RegSizeAlctFastMezzFpga_PROMidCode);  
  //
  alct_prom0_idcode_ = (tmb_->bits_to_int(tmb_->GetDRtdo(),tmb_->GetRegLength(),0) ) & 0xfffffff;
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastMezzProm1,
		    PROMidCode,
		    RegSizeAlctFastMezzFpga_PROMidCode);  
  //
  alct_prom1_idcode_ = (tmb_->bits_to_int(tmb_->GetDRtdo(),tmb_->GetRegLength(),0) ) & 0xfffffff;
  //
  return;
}
//
////////////////////////////////
// TESTPULSE TRIGGER REGISTER
////////////////////////////////
void ALCTController::WriteTriggerRegister_() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE TRIGGER REGISTER" << std::endl;
  //
  FillTriggerRegister_();
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_TRIG_REG,
		    RegSizeAlctFastFpga_WRT_TRIG_REG,
		    write_trigger_reg_);
  usleep(100);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadTriggerRegister_();
    tmb_->CompareBitByBit(write_trigger_reg_,
			  read_trigger_reg_,
			  RegSizeAlctFastFpga_WRT_TRIG_REG);
  }
  //
  return;
}
//
void ALCTController::ReadTriggerRegister_() {
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_TRIG_REG,
		    RegSizeAlctFastFpga_RD_TRIG_REG);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_TRIG_REG; i++)
    read_trigger_reg_[i] = *(register_pointer+i);
  //
  //Print out control register value in hex
  if (debug_) {
    int scaler_trigger_reg = tmb_->bits_to_int(read_trigger_reg_,
					       RegSizeAlctFastFpga_RD_TRIG_REG,
					       LSBfirst);
    (*MyOutput_) << "ALCT: READ TRIGGER REGISTER = " 
		 << std::hex << scaler_trigger_reg << std::dec << std::endl;
  }
  //
  DecodeTriggerRegister_();
  //
  return;
}
//
void ALCTController::SetPulseTriggerSource_(int source) {
  // Specify which signal will fire the testpulse
  // N.B. The trigger source is a combination of the bits in [0-1] and [2-3]
  //
  write_pulse_trigger_source_ = source & 0xf;
  //
  return;
}
//
void ALCTController::SetInvertPulse_(int mask) {
  // bit = 1 = invert
  //     = 0 = not inverted
  //
  write_invert_pulse_ = mask & 0x1; 
  //
  return;
}  
//
void ALCTController::Set_InvertPulse(std::string invert_pulse) {
  //
  given_invert_pulse_ = invert_pulse;
  if (invert_pulse == "on") {
    SetInvertPulse_(ON);
  } else if (invert_pulse == "off") {
    SetInvertPulse_(OFF);
  }
  return;
}
//
std::string ALCTController::Get_InvertPulse() {
  //
  return given_invert_pulse_;
#if 0
  std::string return_value = "NoSetting";
  //
  if ( GetInvertPulse_() == ON )
    return_value = "on";
  //
  if ( GetInvertPulse_() == OFF )
    return_value = "off";
  //
  return return_value;
#endif
}
//
int ALCTController::GetPulseTriggerSource_() {
  //
  return read_pulse_trigger_source_;
}
//
int ALCTController::GetInvertPulse_() {
  //
  return read_invert_pulse_; 
}  
//
void ALCTController::DecodeTriggerRegister_() {
  // ** Extract the trigger register's software values  **
  // ** from the vector of bits read_trigger_reg_[]          **
  //
  int number_of_bits = trigger_register_source_bithi - trigger_register_source_bitlo + 1;  
  read_pulse_trigger_source_ = tmb_->bits_to_int(read_trigger_reg_+trigger_register_source_bitlo,
						 number_of_bits,
						 LSBfirst);
  //
  number_of_bits = trigger_register_invert_bithi - trigger_register_invert_bitlo + 1;  
  read_invert_pulse_ = tmb_->bits_to_int(read_trigger_reg_+trigger_register_invert_bitlo,
					 number_of_bits,
					 LSBfirst);
  //
  return;
}
//
void ALCTController::FillTriggerRegister_() {
  // ** Project the trigger register's software values  **
  // ** into the vector of bits write_trigger_reg_[]          **
  //
  tmb_->int_to_bits(write_pulse_trigger_source_,
		    trigger_register_source_bithi - trigger_register_source_bitlo + 1,
		    write_trigger_reg_ + trigger_register_source_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_invert_pulse_,
		    trigger_register_invert_bithi - trigger_register_invert_bitlo + 1,
		    write_trigger_reg_ + trigger_register_invert_bitlo,
		    LSBfirst);
  //
  return;
}
//
void ALCTController::PrintTriggerRegister_() {
  //
  (*MyOutput_) << "ALCT Testpulse Trigger Register:" << std::endl;
  (*MyOutput_) << "--------------------------------" << std::endl;
  (*MyOutput_) << " trigger source = ";
  if (read_pulse_trigger_source_ == OFF) (*MyOutput_) << "OFF" << std::endl;
  if (read_pulse_trigger_source_ == SELF) (*MyOutput_) << "SELF" << std::endl;
  if (read_pulse_trigger_source_ == ADB_SYNC) (*MyOutput_) << "ADB_SYNC" << std::endl;
  if (read_pulse_trigger_source_ == ADB_ASYNC) (*MyOutput_) << "ADB_ASYNC" << std::endl;
  if (read_pulse_trigger_source_ == LEMO) (*MyOutput_) << "LEMO" << std::endl;
  //
  (*MyOutput_) << " trigger invert =" << std::dec << read_invert_pulse_ << std::endl;
  //
  return;
}
//
void ALCTController::SetPowerUpTriggerRegister_(){
  //
  SetPulseTriggerSource_(trigger_register_source_default);
  SetInvertPulse_(trigger_register_invert_default);
  //
  return;
}
//
////////////////////////////////
// DELAY LINE CONTROL REGISTER
////////////////////////////////
void ALCTController::WriteDelayLineControlReg_() {
  //
  if ( debug_>4 )
    (*MyOutput_) << "ALCT: WRITE Delay Line CONTROL REGISTER" << std::endl;
  //
  FillDelayLineControlReg_();
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_DELAYLINE_CTRL_REG,
		    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_,
		    write_delay_line_control_reg_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if ( debug_>4 )
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadDelayLineControlReg_();
    tmb_->CompareBitByBit(write_delay_line_control_reg_,
			  read_delay_line_control_reg_,
			  RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_);
  }
  //
  return;
}
//
void ALCTController::ReadDelayLineControlReg_() {
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_DELAYLINE_CTRL_REG,
		    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_; i++)
    read_delay_line_control_reg_[i] = *(register_pointer+i);
  //
  //Print out control register value in hex
  if ( debug_>4 ) {
    int scaler_delay_line_control_reg = tmb_->bits_to_int(read_delay_line_control_reg_,
							  RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_,
							  LSBfirst);
    (*MyOutput_) << "ALCT READ: delay line CONTROL REGISTER = " 
		 << std::hex << scaler_delay_line_control_reg << std::dec << std::endl;
  }
  //
  DecodeDelayLineControlReg_();
  //
  return;
}
//
void ALCTController::SetDelayLineGroupSelect_(int group,
					      int mask){
  // Specify which group of delay chips you are talking to
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  if (group < 0 || group >= GetNumberOfGroupsOfDelayChips()) {
    (*MyOutput_) << "SetDelayLineGroupSelect: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> group " << std::dec << group
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfGroupsOfDelayChips()-1 
		 << std::endl;
    (*MyOutput_) << "Delay Line Group unchanged" << std::endl;
    return;
  }
  write_delay_line_group_select_[group] = ~mask & 0x1;
  //
  return;
}
//
void ALCTController::SetDelayLineSettst_(int mask) {
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  write_delay_line_settst_ = ~mask & 0x1; 
  return;
}  
//
void ALCTController::SetDelayLineReset_(int mask) { 
  // bit = 1 = not enabled
  //     = 0 = enabled
  //
  write_delay_line_reset_ = ~mask & 0x1; 
  return;
}
//
void ALCTController::DecodeDelayLineControlReg_() {
  // ** Extract the delay line control register's software values  **
  // ** from the vector of bits read_delay_line_control_reg_[]     **
  //
  int number_of_bits = delay_line_reset_bithi - delay_line_reset_bitlo + 1;  
  read_delay_line_reset_ = tmb_->bits_to_int(read_delay_line_control_reg_+delay_line_reset_bitlo,
					     number_of_bits,
					     LSBfirst);
  //
  number_of_bits = delay_line_settst_bithi - delay_line_settst_bitlo + 1;  
  read_delay_line_settst_ = tmb_->bits_to_int(read_delay_line_control_reg_+delay_line_settst_bitlo,
					      number_of_bits,
					      LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++)
    read_delay_line_group_select_[index] = read_delay_line_control_reg_[index+delay_line_group_select_bitlo];
  //
  return;
}
//
void ALCTController::FillDelayLineControlReg_() {
  // ** Project the delay line control register's software values  **
  // ** into the vector of bits write_delay_line_control_reg_[]    **
  //
  tmb_->int_to_bits(write_delay_line_reset_,
		    delay_line_reset_bithi-delay_line_reset_bitlo+1,
		    write_delay_line_control_reg_+delay_line_reset_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_delay_line_settst_,
		    delay_line_settst_bithi-delay_line_settst_bitlo+1,
		    write_delay_line_control_reg_+delay_line_settst_bitlo,
		    LSBfirst);
  //
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    write_delay_line_control_reg_[index+delay_line_group_select_bitlo] 
      = write_delay_line_group_select_[index];
  //
  return;
}
//
void ALCTController::PrintDelayLineControlReg_() {
  //
  (*MyOutput_) << "ALCT Delay Line Control Register:" << std::endl;
  (*MyOutput_) << "----------------------------------" << std::endl;
  (*MyOutput_) << "read_delay_line_reset_  = " << std::dec 
	       << read_delay_line_reset_ << std::endl;
  (*MyOutput_) << "read_delay_line_settst_ = " << std::dec 
	       << read_delay_line_settst_ << std::endl;
  for (int index=0; index<GetNumberOfGroupsOfDelayChips(); index++) 
    (*MyOutput_) << "read_delay_line_group_select_[" << std::dec << index 
		 << "] = " << read_delay_line_group_select_[index] << std::endl;
  return;
}
//
void ALCTController::SetPowerUpDelayLineControlReg_(){
  //
  SetDelayLineReset_(delay_line_reset_default);
  SetDelayLineSettst_(delay_line_settst_default);
  for (int group=0; group<7; group++)            // maximum number of groups of delay lines
    write_delay_line_group_select_[group] = ~delay_line_group_select_default & 0x1;
  //
  return;
}
//
////////////////////////////////
// AFEB DELAYS and PATTERNS
////////////////////////////////
void ALCTController::WriteAsicDelaysAndPatterns() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE asic DELAYS and PATTERNS for " 
		 << GetNumberOfGroupsOfDelayChips() << " groups" 
		 << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) 
    WriteAsicDelaysAndPatterns_(group);
  //
  return;
}
//
void ALCTController::WriteAsicDelaysAndPatterns_(int group) {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE asic DELAYS and PATTERNS for group " << group << std::endl;
  //
  // write values to one group of AFEBs at a time...
  //
  SetDelayLineGroupSelect_(group,ON);     
  WriteDelayLineControlReg_();
  //    PrintDelayLineControlReg_();
  //
  FillAsicDelaysAndPatterns_(group);
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_ASIC_DELAY_LINES,
		    RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
		    write_asic_delays_and_patterns_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadAsicDelaysAndPatterns_(group);
    tmb_->CompareBitByBit(write_asic_delays_and_patterns_,           
			  read_asic_delays_and_patterns_,            
			  RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES); 
  }
  //
  SetDelayLineGroupSelect_(group,OFF);     
  WriteDelayLineControlReg_();
  //
  return;
}
//
void ALCTController::ReadAsicDelaysAndPatterns_(int group) {
  //
  //N.B. Before using this method, you should already have selected the right
  // delay line control register values
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS for group " << group << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_ASIC_DELAY_LINES,
		    RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES);

  //The first bit of the read register is junk--the data we want starts at index=1
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=1; i<RegSizeAlctFastFpga_RD_ASIC_DELAY_LINES; i++)  
    read_asic_delays_and_patterns_[i-1] = *(register_pointer+i);
  //
  //The read of the asics delay and pattern register is destructive, so it needs to be reloaded:
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS destructive... writing it back in... " << std::endl;
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_ASIC_DELAY_LINES,
		    RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
		    read_asic_delays_and_patterns_);
  //
  DecodeAsicDelaysAndPatterns_(group);
  //
  return;
}
//
void ALCTController::ReadAsicDelaysAndPatterns() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ asic DELAYS and PATTERNS for " 
		 << GetNumberOfGroupsOfDelayChips() << " groups" 
		 << std::endl;
  //
  SetPowerUpDelayLineControlReg_();         // reset the control register values
  //
  for (int group=0; group<GetNumberOfGroupsOfDelayChips(); group++) {
    //
    // get values from one group of AFEBs at a time...
    SetDelayLineGroupSelect_(group,ON);     
    WriteDelayLineControlReg_();
    //    PrintDelayLineControlReg_();
    //
    ReadAsicDelaysAndPatterns_(group);
    //
    SetDelayLineGroupSelect_(group,OFF);     
    WriteDelayLineControlReg_();
  }
  //
  return;
}
//
void ALCTController::DecodeAsicDelaysAndPatterns_(int group) {
  // ** Extract the ASIC delays and pattern software values for 6 afebs and               **
  // ** corresponding channels from the vector of bits read_asic_delays_and_patterns_[]   **
  //
  //print out values read to screen:
  if (debug_) {
    char values_read[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8];
    tmb_->packCharBuffer(read_asic_delays_and_patterns_,
			 RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
			 values_read);
    (*MyOutput_) << "Read values:  group" << group << "-> asic_delays_and_patterns = ";
    for (int char_counter=RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8-1; char_counter>=0; char_counter--) {
      //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
      (*MyOutput_) << std::hex
		   << ((values_read[char_counter] >> 4) & 0xf) 
		   << (values_read[char_counter] & 0xf) << " ";
    }
    (*MyOutput_) << std::endl;
  }
  //    
  int minimum_afeb_in_group=group*NUMBER_OF_CHIPS_PER_GROUP;
  int maximum_afeb_in_group=(group+1)*NUMBER_OF_CHIPS_PER_GROUP;
  //
  int afeb_counter = 0;
  int wiregroup_counter = 0;
  //
  for (int afeb=minimum_afeb_in_group; afeb<maximum_afeb_in_group; afeb++) {
    //location of delay bits for this afeb:
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;
    //
    int number_of_bits = asic_delay_value_bithi - asic_delay_value_bitlo + 1;
    int delay_value = tmb_->bits_to_int(read_asic_delays_and_patterns_+minimum_delay_bit,
					number_of_bits,
					MSBfirst);
    //
    read_asic_delay_[afeb] = delay_value;
    //
    //location of pattern value bits for this afeb:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      read_asic_pattern_[layer_number][wiregroup_number] = read_asic_delays_and_patterns_[bit];
      //
      wiregroup_counter++;
    }
    afeb_counter++;
  }
  return;
}
//
void ALCTController::FillAsicDelaysAndPatterns_(int group) {
  // ** Project the software values of the delays and patterns  **
  // ** for this group of AFEBs into the vector of bits         **
  // ** write_asic_delays_and_patterns_[]                       **
  //
  int minimum_afeb_in_group=group*NUMBER_OF_CHIPS_PER_GROUP;
  int maximum_afeb_in_group=(group+1)*NUMBER_OF_CHIPS_PER_GROUP;
  //
  int afeb_counter = 0;
  int wiregroup_counter = 0;
  //
  for (int afeb=minimum_afeb_in_group; afeb<maximum_afeb_in_group; afeb++) {
    //location of delay value bits for this afeb:
    int minimum_delay_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_delay_value_bitlo;
    //
    tmb_->int_to_bits(write_asic_delay_[afeb],
		      asic_delay_value_bithi-asic_delay_value_bitlo+1,
		      write_asic_delays_and_patterns_+minimum_delay_bit,
		      MSBfirst);
    //
    //location of pattern value bits for this afeb:
    int minimum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bitlo;
    int maximum_pattern_bit = afeb_counter * NUMBER_OF_ASIC_BITS + asic_pattern_value_bithi;
    //
    for (int bit=minimum_pattern_bit; bit<=maximum_pattern_bit; bit++) {
      // Use the asic-group map to find which layer and channels are in this group:
      int layer_number = GetLayerFromAsicMap_(wiregroup_counter);
      int wiregroup_number = GetChannelFromAsicMap_(group,wiregroup_counter);
      //
      // Specify the order of the bits in asic_delays_and_patterns_[]:
      write_asic_delays_and_patterns_[bit] = write_asic_pattern_[layer_number][wiregroup_number];
      //
      wiregroup_counter++;
    }
    //
    afeb_counter++;
  }
  //print out values written to screen:
  if (debug_) {
    char values_written[RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8];
    tmb_->packCharBuffer(write_asic_delays_and_patterns_,
			 RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES,
			 values_written);
    (*MyOutput_) << "To write:  group" << group << "-> asic_delays_and_patterns = ";
    for (int char_counter=RegSizeAlctFastFpga_WRT_ASIC_DELAY_LINES/8-1; char_counter>=0; char_counter--) {
      //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
      (*MyOutput_) << std::hex
		   << ((values_written[char_counter] >> 4) & 0xf) 
		   << (values_written[char_counter] & 0xf) << " ";
    }
    (*MyOutput_) << std::endl;
  }
  //
  return;
}
//
int ALCTController::GetLayerFromAsicMap_(int asic_index) {
  return asic_layer_map[asic_index];
}
//
int ALCTController::GetChannelFromAsicMap_(int group, 
					   int asic_index) {
  // Each asic map is the same, except shifted by 16 wires per 96-channel group:
  return asic_wiregroup_map[asic_index] + group*NUMBER_OF_LINES_PER_CHIP; 
}
//
void ALCTController::SetAsicDelay(int afebChannel,
				  int delay) {
  if (delay<0 || delay > 15) {
    (*MyOutput_) << "SetAsicDelay: ERROR delay = " << std::dec << delay << ", should be between 0 and 15" << std::endl;
    return;
  } 
  //
  if ( afebChannel<0 || afebChannel>MaximumUserIndex() ) {
    (*MyOutput_) << "SetAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << MaximumUserIndex()
		 << std::endl;
    return;
  } 
  //
  write_asic_delay_[UserIndexToHardwareIndex_(afebChannel)] = delay & 0xf;
  //
  return;

}
//
int ALCTController::GetWriteAsicDelay(int afebChannel) {
  //
  if ( afebChannel<0 || afebChannel>MaximumUserIndex() ) {
    (*MyOutput_) << "GetWriteAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << MaximumUserIndex()
		 << std::endl;
    return 999;
  } 
  //
  return write_asic_delay_[UserIndexToHardwareIndex_(afebChannel)];
  //
}
//
int ALCTController::GetAsicDelay(int afebChannel) {
  //
  if ( afebChannel<0 || afebChannel>MaximumUserIndex() ) {
    (*MyOutput_) << "GetAsicDelay: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << afebChannel
		 << " invalid ... must be between 0 and " << std::dec << MaximumUserIndex() 
		 << std::endl;
    return 999;
  } 
  //
  return read_asic_delay_[UserIndexToHardwareIndex_(afebChannel)];
}
//
void ALCTController::SetPowerUpAsicDelays() {
  //
  for (int afeb=0; afeb<MAX_NUM_AFEBS; afeb++)
      write_asic_delay_[afeb] = asic_delay_value_default;
  //
  return;
}
//
void ALCTController::PrintAsicDelays() {
  //
  (*MyOutput_) << "ASIC delay values:" << std::endl;
  (*MyOutput_) << "AFEB   delay (2ns)" << std::endl;
  (*MyOutput_) << "----   -----------" << std::endl;
  for (int afeb=0; afeb<=MaximumUserIndex(); afeb++)
    (*MyOutput_) << " " << std::dec << afeb << "     " << GetAsicDelay(afeb) << std::endl;
  //
  return;
}
//
void ALCTController::SetAsicPattern(int layer,
				    int channel,
				    int on_or_off) {
  if (layer<0 || layer>=MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetAsicPattern: layer " << std::dec << layer
		 << " invalid ... must be between 0 and " << MAX_NUM_LAYERS-1 
		 << std::endl;
    return;
  } 
  //
  if (channel<0 || channel>=GetNumberOfChannelsPerLayer() ) {
    (*MyOutput_) << "SetAsicPattern: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << " -> channel " << std::dec << channel
		 << " invalid ... must be between 0 and " << GetNumberOfChannelsPerLayer()-1 
		 << std::endl;
    return;
  } 
  write_asic_pattern_[layer][channel] = on_or_off & 0x1;
  //
  return;
}
//
int ALCTController::GetAsicPattern(int layer,
				   int channel) {
  if (layer<0 || layer>=MAX_NUM_LAYERS) {
    (*MyOutput_) << "GetAsicPattern: layer " << std::dec << layer
		 << " invalid ... must be between 0 and " << MAX_NUM_LAYERS-1
		 << std::endl;
    return 999;
  } 
  //
  if (channel<0 || channel>=GetNumberOfChannelsPerLayer() ) {
    (*MyOutput_) << "GetAsicPattern: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << " -> channel " << std::dec << channel
		 << " invalid ... must be between 0 and " << GetNumberOfChannelsPerLayer()-1 
		 << std::endl;
    return 999;
  } 
  //
  return read_asic_pattern_[layer][channel];
}
//
void ALCTController::SetPowerUpAsicPatterns() {
  //
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    for (int channel=0; channel<MAX_NUM_WIRES_PER_LAYER; channel++)
      write_asic_pattern_[layer][channel] = asic_pattern_value_default;
  //
  return;
}
//
void ALCTController::PrintAsicPatterns() {
  //
  char pattern[MAX_NUM_LAYERS][MAX_NUM_WIRES_PER_LAYER/8];
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++)
    tmb_->packCharBuffer(read_asic_pattern_[layer],
			 GetNumberOfChannelsPerLayer(),
			 pattern[layer]);
  //
  (*MyOutput_) << "READ Asic pattern for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
	       << " (from right to left):" << std::endl;
  //
  for (int layer=MAX_NUM_LAYERS-1; layer>=0; layer--) {
    (*MyOutput_) << "Layer " << std::dec << layer << " -> ";    
    //
    for (int char_counter=(GetNumberOfChannelsPerLayer()/8)-1; char_counter>=0; char_counter--) {
      (*MyOutput_) << std::hex
      		   << ((pattern[layer][char_counter] >> 4) & 0xf) 
      		   << (pattern[layer][char_counter] & 0xf) << " ";
    }
      (*MyOutput_) << std::endl;
  }
  //
  return;
}
//
////////////////////////////////
// CONFIGURATION REGISTER
////////////////////////////////
void ALCTController::WriteConfigurationReg() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE Configuration Register" << std::endl;
  //
  FillConfigurationReg_();
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_CONFIG_REG,
		    RegSizeAlctFastFpga_WRT_CONFIG_REG,
		    write_config_reg_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadConfigurationReg();
    tmb_->CompareBitByBit(write_config_reg_,
			  read_config_reg_,
			  RegSizeAlctFastFpga_WRT_CONFIG_REG);
  }
  //
  return;
}
//
void ALCTController::ReadConfigurationReg() {
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_CONFIG_REG,
		    RegSizeAlctFastFpga_RD_CONFIG_REG);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_CONFIG_REG; i++)
    read_config_reg_[i] = *(register_pointer+i);
  //
  if (debug_) {
    //Print out configuration register in hex...
    char configuration_register[RegSizeAlctFastFpga_RD_CONFIG_REG/8];
    tmb_->packCharBuffer(read_config_reg_,
			 RegSizeAlctFastFpga_RD_CONFIG_REG,
			 configuration_register);
    //
    (*MyOutput_) << "ALCT READ: configuration register = 0x";
    for (int counter=RegSizeAlctFastFpga_RD_CONFIG_REG/8; counter>=0; counter--) 
      (*MyOutput_) << std::hex
		   << ((configuration_register[counter] >> 4) & 0xf) 
		   << (configuration_register[counter] & 0xf);
    (*MyOutput_) << std::endl;
  }
  //
  DecodeConfigurationReg_();
  //
  return;
}
//
void ALCTController::PrintConfigurationReg() {
  //
  (*MyOutput_) << "ALCT configuration register:" << std::endl;
  (*MyOutput_) << "----------------------------" << std::endl;
  (*MyOutput_) << "trigger_mode_              = " << std::dec 
		<< GetTriggerMode() << std::endl;                      
  (*MyOutput_) << "ext_trig_enable_           = " << std::dec 
		<< GetExtTrigEnable() << std::endl;                    
  (*MyOutput_) << "send_empty_                = " << std::dec 
		<< GetSendEmpty() << std::endl;
  (*MyOutput_) << "inject_                    = " << std::dec 
		<< GetInjectMode() << std::endl;
  (*MyOutput_) << "bxc_offset_                = " << std::dec 
		<< GetBxcOffset() << std::endl;
  (*MyOutput_) << "nph_thresh_                = " << std::dec 
		<< GetPretrigNumberOfLayers() << std::endl;
  (*MyOutput_) << "nph_pattern_               = " << std::dec 
		<< GetPretrigNumberOfPattern() << std::endl;
  (*MyOutput_) << "drift_delay_               = " << std::dec
		<< GetDriftDelay() << std::endl;
  (*MyOutput_) << "fifo_tbins_                = " << std::dec 
		<< GetFifoTbins() << std::endl;
  (*MyOutput_) << "fifo_pretrig_              = " << std::dec         
		<< GetFifoPretrig() << std::endl;
  (*MyOutput_) << "fifo_mode_                 = " << std::dec
		<< GetFifoMode() << std::endl;
  (*MyOutput_) << "accelerator_pretrig_thresh = " << std::dec
		<< GetAcceleratorPretrigThresh() << std::endl;
  (*MyOutput_) << "l1a_delay_                 = " << std::dec 
		<< GetL1aDelay() << " = 0x" << std::hex
		<< GetL1aDelay() << std::endl;
  (*MyOutput_) << "l1a_window_                = " << std::dec
		<< GetL1aWindowSize() << std::endl;
  (*MyOutput_) << "l1a_offset_                = " << std::dec
		<< GetL1aOffset() << std::endl;
  (*MyOutput_) << "l1a_internal_              = " << std::dec         
		<< GetL1aInternal() << std::endl;
  (*MyOutput_) << "board_id_                  = " << std::dec
		<< GetBoardId() << std::endl;
  (*MyOutput_) << "accelerator_pattern_thresh = " << std::dec
		<< GetAcceleratorPatternThresh() << std::endl;
  (*MyOutput_) << "ccb_enable_                = " << std::dec
		<< GetCcbEnable() << std::endl;
  (*MyOutput_) << "config_in_readout          = " << std::dec
		<< GetConfigInReadout() << std::endl;
  (*MyOutput_) << "alct_amode_                = " << std::dec
		<< GetAlctAmode() << std::endl;
  (*MyOutput_) << "trigger_info_en_           = " << std::dec
		<< GetTriggerInfoEnable() << std::endl;         
  (*MyOutput_) << "sn_select_                 = " << std::dec
		<< GetSnSelect() << std::endl;  
}
//
void ALCTController::SetTriggerMode(int trigger_mode) { 
  //
  write_trigger_mode_ = trigger_mode; 
  return;
}
//
int ALCTController::GetTriggerMode() { 
  //
  return read_trigger_mode_; 
}
//
void ALCTController::SetExtTrigEnable(int ext_trig_enable) { 
  //
  write_ext_trig_enable_ = ext_trig_enable; 
  return;
}
//
int ALCTController::GetExtTrigEnable() { 
  //
  return read_ext_trig_enable_; 
}
//
void ALCTController::SetSendEmpty(int send_empty) { 
  //
  write_send_empty_ = send_empty; 
  return;
}
//
int ALCTController::GetSendEmpty() { 
  //
  return read_send_empty_; 
}
//
void ALCTController::SetInjectMode(int inject) { 
  //
  write_inject_ = inject; 
  return;
}
//
int ALCTController::GetInjectMode() { 
  //
  return read_inject_; 
}
//
void ALCTController::SetBxcOffset(int bxc_offset) { 
  //
  write_bxc_offset_ = bxc_offset; 
  return;
}
//
int ALCTController::GetBxcOffset() { 
  //
  return read_bxc_offset_; 
}
//
void ALCTController::SetPretrigNumberOfLayers(int nph_thresh) { 
  //
  write_nph_thresh_ = nph_thresh; 
  return;
}
//
int ALCTController::GetPretrigNumberOfLayers() { 
  //
  return read_nph_thresh_; 
}
//
void ALCTController::SetPretrigNumberOfPattern(int nph_pattern) { 
  //
  write_nph_pattern_ = nph_pattern; 
  return;
}
//
int ALCTController::GetPretrigNumberOfPattern() { 
  //
  return read_nph_pattern_; 
}
//
void ALCTController::SetDriftDelay(int drift_delay) { 
  //
  write_drift_delay_ = drift_delay; 
  return;
}
//
int ALCTController::GetDriftDelay() { 
  //
  return read_drift_delay_; 
}
//
void ALCTController::SetFifoTbins(int fifo_tbins) { 
  //
  write_fifo_tbins_ = fifo_tbins; 
  return;
}
//
int ALCTController::GetFifoTbins() { 
  //
  return read_fifo_tbins_; 
}
//
void ALCTController::SetFifoPretrig(int fifo_pretrig) { 
  //
  write_fifo_pretrig_ = fifo_pretrig; 
  return;
}
//
int ALCTController::GetFifoPretrig() { 
  //
  return read_fifo_pretrig_; 
}
//
void ALCTController::SetFifoMode(int fifo_mode) { 
  //
  write_fifo_mode_ = fifo_mode; 
  return;
}
//
int ALCTController::GetFifoMode() { 
  //
  return read_fifo_mode_; 
}
//
void ALCTController::SetAcceleratorPretrigThresh(int accelerator_pretrig_thresh) {
  //
  write_accelerator_pretrig_thresh_ = accelerator_pretrig_thresh;
  return;
}
//
int ALCTController::GetAcceleratorPretrigThresh() {
  //
  return read_accelerator_pretrig_thresh_;
}
//
void ALCTController::SetL1aDelay(int l1a_delay) { 
  //
  write_l1a_delay_ = l1a_delay; 
  return;
}
//
int ALCTController::GetL1aDelay() { 
  //
  return read_l1a_delay_; 
}
//
void ALCTController::SetL1aWindowSize(int l1a_window) { 
  //
  write_l1a_window_ = l1a_window; 
  return;
}
//
int ALCTController::GetL1aWindowSize() { 
  //
  return read_l1a_window_; 
}
//
void ALCTController::SetL1aOffset(int l1a_offset) { 
  //
  write_l1a_offset_ = l1a_offset; 
  return;
}
//
int ALCTController::GetL1aOffset() { 
  //
  return read_l1a_offset_; 
}
//
void ALCTController::SetL1aInternal(int l1a_internal) { 
  //
  write_l1a_internal_ = l1a_internal; 
  return;
}
int ALCTController::GetL1aInternal() { 
  //
  return read_l1a_internal_; 
}
//
void ALCTController::SetBoardId(int board_id) { 
  //
  write_board_id_ = board_id; 
  return;
}
//
int ALCTController::GetBoardId() { 
  //
  return read_board_id_; 
}
//
void ALCTController::SetAcceleratorPatternThresh(int accelerator_pattern_thresh) {
  //
  write_accelerator_pattern_thresh_ = accelerator_pattern_thresh;
  return;
}
//
int ALCTController::GetAcceleratorPatternThresh() {
  //
  return read_accelerator_pattern_thresh_;
}
//
void ALCTController::SetCcbEnable(int ccb_enable) { 
  //
  write_ccb_enable_ = ccb_enable; 
  return;
}
//
int ALCTController::GetCcbEnable() { 
  //
  return read_ccb_enable_; 
}
//
void ALCTController::SetConfigInReadout(int config_in_readout) {
  //
  write_config_in_readout_ = config_in_readout;
  return;
}
//
int ALCTController::GetConfigInReadout() {
  //
  return read_config_in_readout_;
}
//
void ALCTController::SetAlctAmode(int alct_amode) { 
  //
  write_alct_amode_ = alct_amode; 
  return;
}
//
int ALCTController::GetAlctAmode() { 
  //
  return read_alct_amode_; 
}
//
void ALCTController::SetTriggerInfoEnable(int trigger_info_en) { 
  //
  write_trigger_info_en_ = trigger_info_en; 
  return;
}
int ALCTController::GetTriggerInfoEnable() { 
  //
  return read_trigger_info_en_; 
}
//
void ALCTController::SetSnSelect(int sn_select) { 
  //
  write_sn_select_ = sn_select; 
  return;
}
int ALCTController::GetSnSelect() { 
  //
  return read_sn_select_; 
}
//
void ALCTController::DecodeConfigurationReg_(){
  // ** Extract the configuration register's software values  **
  // ** from the vector of bits read_config_reg_[]            **
  //
  int number_of_bits = trigger_mode_bithi - trigger_mode_bitlo + 1;
  read_trigger_mode_ = tmb_->bits_to_int(read_config_reg_+trigger_mode_bitlo,
					 number_of_bits,
					 LSBfirst);
  //
  number_of_bits = ext_trig_enable_bithi - ext_trig_enable_bitlo + 1;
  read_ext_trig_enable_ = tmb_->bits_to_int(read_config_reg_+ext_trig_enable_bitlo,
					    number_of_bits,
					    LSBfirst);
  //
  number_of_bits = send_empty_bithi - send_empty_bitlo + 1;
  read_send_empty_ = tmb_->bits_to_int(read_config_reg_+send_empty_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = inject_bithi - inject_bitlo + 1;
  read_inject_ = tmb_->bits_to_int(read_config_reg_+inject_bitlo,
				   number_of_bits,
				   LSBfirst);
  //
  number_of_bits = bxc_offset_bithi - bxc_offset_bitlo + 1;
  read_bxc_offset_ = tmb_->bits_to_int(read_config_reg_+bxc_offset_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = nph_thresh_bithi - nph_thresh_bitlo + 1;
  read_nph_thresh_ = tmb_->bits_to_int(read_config_reg_+nph_thresh_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = nph_pattern_bithi - nph_pattern_bitlo + 1;
  read_nph_pattern_ = tmb_->bits_to_int(read_config_reg_+nph_pattern_bitlo,
					number_of_bits,
					LSBfirst);
  //
  number_of_bits = alct_drift_delay_bithi - alct_drift_delay_bitlo + 1;
  read_drift_delay_ = tmb_->bits_to_int(read_config_reg_+alct_drift_delay_bitlo,
					number_of_bits,
					LSBfirst);
  //
  number_of_bits = alct_fifo_tbins_bithi - alct_fifo_tbins_bitlo + 1;
  read_fifo_tbins_ = tmb_->bits_to_int(read_config_reg_+alct_fifo_tbins_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = alct_fifo_pretrig_bithi - alct_fifo_pretrig_bitlo + 1;
  read_fifo_pretrig_ = tmb_->bits_to_int(read_config_reg_+alct_fifo_pretrig_bitlo,
					 number_of_bits,
					 LSBfirst);
  //
  number_of_bits = alct_fifo_mode_bithi - alct_fifo_mode_bitlo + 1;
  read_fifo_mode_ = tmb_->bits_to_int(read_config_reg_+alct_fifo_mode_bitlo,
				      number_of_bits,
				      LSBfirst);
  //
  number_of_bits = accelerator_pretrig_thresh_bithi - accelerator_pretrig_thresh_bitlo + 1;
  read_accelerator_pretrig_thresh_ = tmb_->bits_to_int(read_config_reg_+accelerator_pretrig_thresh_bitlo,
						       number_of_bits,
						       LSBfirst);
  //
  number_of_bits = l1a_delay_bithi - l1a_delay_bitlo + 1;
  read_l1a_delay_ = tmb_->bits_to_int(read_config_reg_+l1a_delay_bitlo,
				      number_of_bits,
				      LSBfirst);
  //
  number_of_bits = l1a_window_bithi - l1a_window_bitlo + 1;
  read_l1a_window_ = tmb_->bits_to_int(read_config_reg_+l1a_window_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = alct_l1a_offset_bithi - alct_l1a_offset_bitlo + 1;
  read_l1a_offset_ = tmb_->bits_to_int(read_config_reg_+alct_l1a_offset_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = l1a_internal_bithi - l1a_internal_bitlo + 1;
  read_l1a_internal_ = tmb_->bits_to_int(read_config_reg_+l1a_internal_bitlo,
					 number_of_bits,
					 LSBfirst);
  //
  number_of_bits = board_id_bithi - board_id_bitlo + 1;
  read_board_id_ = tmb_->bits_to_int(read_config_reg_+board_id_bitlo,
				     number_of_bits,
				     LSBfirst);
  //
  number_of_bits = accelerator_pattern_thresh_bithi - accelerator_pattern_thresh_bitlo + 1;
  read_accelerator_pattern_thresh_ = tmb_->bits_to_int(read_config_reg_+accelerator_pattern_thresh_bitlo,
						       number_of_bits,
						       LSBfirst);
  //
  number_of_bits = ccb_enable_bithi - ccb_enable_bitlo + 1;
  read_ccb_enable_ = tmb_->bits_to_int(read_config_reg_+ccb_enable_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = config_in_readout_bithi - config_in_readout_bitlo + 1;
  read_config_in_readout_ = tmb_->bits_to_int(read_config_reg_+config_in_readout_bitlo,
					      number_of_bits,
					      LSBfirst);
  //
  number_of_bits = alct_amode_bithi - alct_amode_bitlo + 1;
  read_alct_amode_ = tmb_->bits_to_int(read_config_reg_+alct_amode_bitlo,
				       number_of_bits,
				       LSBfirst);
  //
  number_of_bits = trigger_info_en_bithi - trigger_info_en_bitlo + 1;
  read_trigger_info_en_ = tmb_->bits_to_int(read_config_reg_+trigger_info_en_bitlo,
					    number_of_bits,
					    LSBfirst);
  //
  number_of_bits = sn_select_bithi - sn_select_bitlo + 1;
  read_sn_select_ = tmb_->bits_to_int(read_config_reg_+sn_select_bitlo,
				      number_of_bits,
				      LSBfirst);
  //
  return;
}
//
void ALCTController::FillConfigurationReg_(){
  // ** Project the configuration register's software values  **
  // ** into the vector of bits write_config_reg_[]           **
  //
  tmb_->int_to_bits(write_trigger_mode_,
		    trigger_mode_bithi-trigger_mode_bitlo+1,
		    write_config_reg_+trigger_mode_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_ext_trig_enable_,
		    ext_trig_enable_bithi-ext_trig_enable_bitlo+1,
		    write_config_reg_+ext_trig_enable_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_send_empty_,
		    send_empty_bithi-send_empty_bitlo+1,
		    write_config_reg_+send_empty_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_inject_,
		    inject_bithi-inject_bitlo+1,
		    write_config_reg_+inject_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_bxc_offset_,
		    bxc_offset_bithi-bxc_offset_bitlo+1,
		    write_config_reg_+bxc_offset_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_nph_thresh_,
		    nph_thresh_bithi-nph_thresh_bitlo+1,
		    write_config_reg_+nph_thresh_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_nph_pattern_,
		    nph_pattern_bithi-nph_pattern_bitlo+1,
		    write_config_reg_+nph_pattern_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_drift_delay_,
		    alct_drift_delay_bithi-alct_drift_delay_bitlo+1,
		    write_config_reg_+alct_drift_delay_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_fifo_tbins_,
		    alct_fifo_tbins_bithi-alct_fifo_tbins_bitlo+1,
		    write_config_reg_+alct_fifo_tbins_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_fifo_pretrig_,
		    alct_fifo_pretrig_bithi-alct_fifo_pretrig_bitlo+1,
		    write_config_reg_+alct_fifo_pretrig_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_fifo_mode_,
		    alct_fifo_mode_bithi-alct_fifo_mode_bitlo+1,
		    write_config_reg_+alct_fifo_mode_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_accelerator_pretrig_thresh_,
		    accelerator_pretrig_thresh_bithi-accelerator_pretrig_thresh_bitlo+1,
		    write_config_reg_+accelerator_pretrig_thresh_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_l1a_delay_,
		    l1a_delay_bithi-l1a_delay_bitlo+1,
		    write_config_reg_+l1a_delay_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_l1a_window_,
		    l1a_window_bithi-l1a_window_bitlo+1,
		    write_config_reg_+l1a_window_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_l1a_offset_,
		    alct_l1a_offset_bithi-alct_l1a_offset_bitlo+1,
		    write_config_reg_+alct_l1a_offset_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_l1a_internal_,
		    l1a_internal_bithi-l1a_internal_bitlo+1,
		    write_config_reg_+l1a_internal_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_board_id_,
		    board_id_bithi-board_id_bitlo+1,
		    write_config_reg_+board_id_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_accelerator_pattern_thresh_,
		    accelerator_pattern_thresh_bithi-accelerator_pattern_thresh_bitlo+1,
		    write_config_reg_+accelerator_pattern_thresh_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_ccb_enable_,
		    ccb_enable_bithi-ccb_enable_bitlo+1,
		    write_config_reg_+ccb_enable_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_config_in_readout_,
		    config_in_readout_bithi-config_in_readout_bitlo+1,
		    write_config_reg_+config_in_readout_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_alct_amode_,
		    alct_amode_bithi-alct_amode_bitlo+1,
		    write_config_reg_+alct_amode_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_trigger_info_en_,
		    trigger_info_en_bithi-trigger_info_en_bitlo+1,
		    write_config_reg_+trigger_info_en_bitlo,
		    LSBfirst);
  //
  tmb_->int_to_bits(write_sn_select_,
		    sn_select_bithi-sn_select_bitlo+1,
		    write_config_reg_+sn_select_bitlo,
		    LSBfirst);
  return;
}
//
void ALCTController::SetPowerUpConfigurationReg() {
  //
  // For bits which are not explicitly set by the methods below, set them to 0:
  for (int i=0; i<RegSizeAlctFastFpga_WRT_CONFIG_REG; i++)
    write_config_reg_[i] = 0;
  //
  SetTriggerMode(trigger_mode_default);                      
  SetExtTrigEnable(ext_trig_enable_default);                    
  SetSendEmpty(send_empty_default);                        
  SetInjectMode(inject_default);                       
  SetBxcOffset(bxc_offset_default);                        
  SetPretrigNumberOfLayers(nph_thresh_default);            
  SetPretrigNumberOfPattern(nph_pattern_default);           
  SetDriftDelay(alct_drift_delay_default);                       
  SetFifoTbins(alct_fifo_tbins_default);                       
  SetFifoPretrig(alct_fifo_pretrig_default);                      
  SetFifoMode(alct_fifo_mode_default);                         
  SetAcceleratorPretrigThresh(accelerator_pretrig_thresh_default);
  SetL1aDelay(l1a_delay_default);                       
  SetL1aWindowSize(l1a_window_default);                    
  SetL1aOffset(alct_l1a_offset_default);                        
  SetL1aInternal(l1a_internal_default);                      
  SetBoardId(board_id_default);                          
  SetAcceleratorPatternThresh(accelerator_pattern_thresh_default);
  SetCcbEnable(ccb_enable_default);                        
  SetConfigInReadout(config_in_readout_default);
  SetAlctAmode(alct_amode_default);                        
  SetTriggerInfoEnable(trigger_info_en_default);                
  SetSnSelect(sn_select_default);                         
  //
  return;
}
//
//////////////////////////////
// HOT CHANNEL MASK
//////////////////////////////
void ALCTController::WriteHotChannelMask() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE hot channel mask" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_HOTCHAN_MASK,
		    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_,
		    write_hot_channel_mask_);
  usleep(100);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadHotChannelMask();
    tmb_->CompareBitByBit(write_hot_channel_mask_,
			  read_hot_channel_mask_,
			  RegSizeAlctFastFpga_RD_HOTCHAN_MASK_);
  }
  //
  return;
}
//
void ALCTController::ReadHotChannelMask() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ hot channel mask" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_HOTCHAN_MASK,
		    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_; i++)
    read_hot_channel_mask_[i] = *(register_pointer+i);
  //
  usleep(100);
  //
  //The read of the hot channel mask is destructive, so it needs to be reloaded:
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ Hot Channel Mask destructive... writing it back in... " << std::endl;
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_HOTCHAN_MASK,
		    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_,
		    read_hot_channel_mask_);
  //
  usleep(100);
  //
  return;
}
//
void ALCTController::PrintHotChannelMask() {
  // Print out hot channel mask in hex for each layer 
  // from right (channel 0) to left (number of channels in layer)
  //
  char hot_channel_mask[RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8];
  tmb_->packCharBuffer(read_hot_channel_mask_,
		       RegSizeAlctFastFpga_RD_HOTCHAN_MASK_,
		       hot_channel_mask);
  //
  int char_counter = RegSizeAlctFastFpga_RD_HOTCHAN_MASK_/8 - 1;
  //
  (*MyOutput_) << "ALCT: Hot Channel Mask for ALCT" << std::dec << GetNumberOfChannelsInAlct() 
	       << " (from right to left):" << std::endl;
  //
  for (int layer=5; layer>=0; layer--) {
    (*MyOutput_) << "Layer " << std::dec << layer << " -> ";    
    for (int layer_counter=GetNumberOfChannelsPerLayer()/8; layer_counter>0; layer_counter--) {
      //      (*MyOutput_) << "char_counter " << std::dec << char_counter << " -> ";    
      (*MyOutput_) << std::hex
		   << ((hot_channel_mask[char_counter] >> 4) & 0xf) 
		   << (hot_channel_mask[char_counter] & 0xf) << " ";
      char_counter--;
    }
      (*MyOutput_) << std::endl;
  }
  return;
}
//
void ALCTController::SetHotChannelMask(int layer,
				       int channel,
				       int on_or_off) {
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetHotChannelMask: layer " << layer 
		 << "... must be between 0 and " << MAX_NUM_LAYERS-1 
		 << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  if (channel < 0 || channel >= GetNumberOfChannelsPerLayer()) {
    (*MyOutput_) << "SetHotChannelMask: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << channel 
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfChannelsPerLayer()-1
		 << std::endl;
    (*MyOutput_) << "Hot Channel Mask Unchanged" << std::endl;
    return;
  }
  //
  //index in hot channel mask is determined by layer number and channel number within the layer:
  int index = layer * GetNumberOfChannelsPerLayer() + channel;
  //
  write_hot_channel_mask_[index] = on_or_off;
  //
  return;
}
//
int ALCTController::GetHotChannelMask(int layer,
				      int channel) {
  if (layer < 0 || layer >= MAX_NUM_LAYERS) {
    (*MyOutput_) << "SetHotChannelMask: layer " << layer 
		 << "... must be between 0 and " << MAX_NUM_LAYERS-1 << std::endl;
    return -1;
  }
  if (channel < 0 || channel >= GetNumberOfChannelsPerLayer()) {
    (*MyOutput_) << "SetHotChannelMask: ALCT" << std::dec << GetNumberOfChannelsInAlct() 
		 << "-> channel " << std::dec << channel 
		 << " invalid ... must be between 0 and " << std::dec << GetNumberOfChannelsPerLayer()-1 
		 << std::endl;
    return -1;
  }
  //
  //index in hot channel mask is determined by layer number and channel number within the layer:
  int index = layer * GetNumberOfChannelsPerLayer() + channel;
  //
  return read_hot_channel_mask_[index];
}
//
void ALCTController::SetPowerUpHotChannelMask() {
  //
  for (int channel=0; channel<RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; channel++)
    write_hot_channel_mask_[channel] = alct_hot_channel_mask_default;
  //
  return;
}
//
//////////////////////////////
// COLLISION PATTERN MASK
//////////////////////////////
void ALCTController::WriteCollisionPatternMask() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: WRITE Collision Pattern mask" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_COLLISION_MASK_REG,
		    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_,
		    write_collision_pattern_mask_reg_);
  //
  if ( tmb_->GetCheckJtagWrite() ) {
    //
    if (debug_)
      (*MyOutput_) << "ALCT: Check JTAG write compared with read... " << std::endl;
    //
    ReadCollisionPatternMask();
    tmb_->CompareBitByBit(write_collision_pattern_mask_reg_,
			  read_collision_pattern_mask_reg_,
			  RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_);
  }
  //
  return;
}
//
void ALCTController::ReadCollisionPatternMask() {
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ Collision Pattern mask" << std::endl;
  //
  tmb_->setup_jtag(ChainAlctFastFpga);
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_RD_COLLISION_MASK_REG,
		    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_);
  //
  int * register_pointer = tmb_->GetDRtdo();
  for (int i=0; i<RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_; i++)
    read_collision_pattern_mask_reg_[i] = *(register_pointer+i);
  //
  //The read of the collision register is destructive, so it needs to be reloaded:
  //
  if (debug_)
    (*MyOutput_) << "ALCT: READ Collision Pattern Mask destructive... writing it back in... " << std::endl;
  //
  tmb_->ShfIR_ShfDR(ChipLocationAlctFastFpga,
		    ALCT_FAST_WRT_COLLISION_MASK_REG,
		    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_,
		    read_collision_pattern_mask_reg_);
  //
  return;
}
//
void ALCTController::PrintCollisionPatternMask() {
  // Print out collision pattern mask for each wiregroup 
  // for pattern A and pattern B
  //
  (*MyOutput_) << "READ Collision Pattern Mask for ALCT" << std::dec << GetNumberOfChannelsInAlct() << "..." << std::endl;
  //
  (*MyOutput_) << "Wire Groups 0 to " << GetNumberOfCollisionPatternGroups() << " -> Pattern A:" << std::endl;
  //
  int bit_counter=0;
  //
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    int lo_wire_group_index = group*NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP;
    int hi_wire_group_index = ( (group+1)*NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP ) - 1;
    (*MyOutput_) << "[" << std::dec << std::setw(3) << lo_wire_group_index 
		 << "," << std::dec << std::setw(3) << hi_wire_group_index << "]";
  }
  (*MyOutput_) << std::endl;
  //
  int bit_counter0 = bit_counter++;
  int bit_counter1 = bit_counter++;
  int bit_counter2 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group == 0) {
      (*MyOutput_) << "    ";
    } else { 
      (*MyOutput_) << "    ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2); 
  }
  (*MyOutput_) << std::endl;
  //
  //
  bit_counter0 = bit_counter++;
  bit_counter1 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group == 0) {
      (*MyOutput_) << "    ";
    } else { 
      (*MyOutput_) << "      ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1);
  }
  (*MyOutput_) << std::endl;
  //
  //
  bit_counter0 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group == 0) {
      (*MyOutput_) << "    ";
    } else { 
      (*MyOutput_) << "        ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0);
  }
  (*MyOutput_) << std::endl;
  //
  //
  bit_counter0 = bit_counter++;
  bit_counter1 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group == 0) {
      (*MyOutput_) << "  ";
    } else {
      (*MyOutput_) << "      ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1); 
  }
  (*MyOutput_) << std::endl;
  //
  //
  bit_counter0 = bit_counter++;
  bit_counter1 = bit_counter++;
  bit_counter2 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group != 0) {
      (*MyOutput_) << "    ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2);
  }
  (*MyOutput_) << std::endl;
  //
  //
  bit_counter0 = bit_counter++;
  bit_counter1 = bit_counter++;
  bit_counter2 = bit_counter++;
  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
    if (group != 0) {
      (*MyOutput_) << "    ";
    }
    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2);
  }
  (*MyOutput_) << std::endl;
  //
  //
  //
  // Pattern B is not supported in the low-latency ALCT firmware...  But if it were, here would be its print-out:
  //  (*MyOutput_) << "Wire Groups 0 to " << GetNumberOfCollisionPatternGroups() << " -> Pattern B:" << std::endl;
  //  //
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    int lo_wire_group_index = group*NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP;
  //    int hi_wire_group_index = ( (group+1)*NUMBER_OF_WIREGROUPS_PER_COLLISION_PATTERN_GROUP ) - 1;
  //    (*MyOutput_) << "[" << std::dec << std::setw(3) << lo_wire_group_index 
  //		 << "," << std::dec << std::setw(3) << hi_wire_group_index << "]";
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  bit_counter0 = bit_counter++;
  //  bit_counter1 = bit_counter++;
  //  bit_counter2 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group == 0) {
  //      (*MyOutput_) << "    ";
  //    } else { 
  //      (*MyOutput_) << "    ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2); 
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  //
  //  bit_counter0 = bit_counter++;
  //  bit_counter1 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group == 0) {
  //      (*MyOutput_) << "    ";
  //    } else { 
  //      (*MyOutput_) << "      ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1);
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  //
  //  bit_counter0 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group == 0) {
  //      (*MyOutput_) << "    ";
  //    } else { 
  //      (*MyOutput_) << "        ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0);
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  //
  //  bit_counter0 = bit_counter++;
  //  bit_counter1 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group == 0) {
  //      (*MyOutput_) << "  ";
  //    } else {
  //      (*MyOutput_) << "      ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1); 
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  //
  //  bit_counter0 = bit_counter++;
  //  bit_counter1 = bit_counter++;
  //  bit_counter2 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group != 0) {
  //      (*MyOutput_) << "    ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2);
  //  }
  //  (*MyOutput_) << std::endl;
  //  //
  //  //
  //  bit_counter0 = bit_counter++;
  //  bit_counter1 = bit_counter++;
  //  bit_counter2 = bit_counter++;
  //  for (int group=0; group<GetNumberOfCollisionPatternGroups(); group++) {
  //    if (group != 0) {
  //      (*MyOutput_) << "    ";
  //    }
  //    (*MyOutput_) << GetCollisionPatternMask(group,bit_counter0) << " " << GetCollisionPatternMask(group,bit_counter1) << " " << GetCollisionPatternMask(group,bit_counter2);
  //  }
  //  (*MyOutput_) << std::endl;
  //
  return;
}
//
void ALCTController::SetCollisionPatternMask(int group,
					     int bitInEnvelope,
					     int on_or_off) {
  if ( group < 0 || group >= GetNumberOfCollisionPatternGroups() ) {
    (*MyOutput_) << "SetCollisionPatternMask: ALCT" << std::dec << GetNumberOfChannelsInAlct()  
		 << "-> group " << group
		 << "... must be between 0 and " << GetNumberOfCollisionPatternGroups() 
		 << std::endl;
    (*MyOutput_) << "Collision Pattern Mask Unchanged" << std::endl;
    return;
  }
  if ( bitInEnvelope < 0 || bitInEnvelope >= NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP ) {
    (*MyOutput_) << "SetCollisionPatternMask: ERROR bit " << std::dec << bitInEnvelope 
		 << " invalid ... must be between 0 and " << std::dec << NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP-1
		 << std::endl;
    (*MyOutput_) << "Collision Pattern Mask Unchanged" << std::endl;
    return;
  }
  //
  //index in Collision Pattern mask is determined by group and whether it is the mask for pattern A or B:
  int index = group * NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP + bitInEnvelope;
  //
  write_collision_pattern_mask_reg_[index] = on_or_off & 0x1;
  //
  return;
}
//
int ALCTController::GetCollisionPatternMask(int group,
					    int bitInEnvelope) {
  if ( group < 0 || group >= GetNumberOfCollisionPatternGroups() ) {
    (*MyOutput_) << "GetCollisionPatternMask: ERROR ALCT" << std::dec << GetNumberOfChannelsInAlct()  
		 << "-> group " << group
		 << " invalid ... must be between 0 and " << GetNumberOfCollisionPatternGroups() 
		 << std::endl;
    return -1;
  }
  if ( bitInEnvelope < 0 || bitInEnvelope >= NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP ) {
    (*MyOutput_) << "GetCollisionPatternMask: ERROR bit " << std::dec << bitInEnvelope 
		 << " invalid ... must be between 0 and " << std::dec << NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP-1
		 << std::endl;
    return -1;
  }
  //
  //index in Collision Pattern mask is determined by group:
  int index = group * NUMBER_OF_BITS_IN_COLLISION_MASK_PER_GROUP + bitInEnvelope;
  //
  return read_collision_pattern_mask_reg_[index];
}
//
//
void ALCTController::SetPowerUpCollisionPatternMask() {
  //
  for (int channel=0; channel<RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672; channel++)
    write_collision_pattern_mask_reg_[channel] = collision_pattern_mask_default;
  //
  return;
}
//
//////////////////////////////
// CHAMBER CHARACTERISTICS
//////////////////////////////
void ALCTController::SetChamberCharacteristics_(std::string chamberType) {
  //
  chamber_type_string_ = chamberType;
  NumberOfWireGroupsInChamber_ = 0;
  NumberOfChannelsPerLayer_ = 0;
  //
  (*MyOutput_) << "Chamber is " << chamber_type_string_ << "->" << std::endl;
  //
  if (chamber_type_string_ == "ME11") {
    //
    SetExpectedFastControlRegularMirrorType(REGULAR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME11;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME11);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME11);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME11);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  } else if (chamber_type_string_ == "ME12") {
    //
    SetExpectedFastControlRegularMirrorType(REGULAR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME12;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME12);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME12);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME12);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  } else if (chamber_type_string_ == "ME13") {
    //
    SetExpectedFastControlRegularMirrorType(REGULAR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME13;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME13);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME13);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME13);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  } else if (chamber_type_string_ == "ME21") {
    //
    SetExpectedFastControlRegularMirrorType(REGULAR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME21;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME21);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME21);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME21);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz672);
    //
  } else if (chamber_type_string_ == "ME22") {
    //
    SetExpectedFastControlRegularMirrorType(REGULAR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME22;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME22);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME22);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME22);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  } else if (chamber_type_string_ == "ME31") {
    //
    SetExpectedFastControlRegularMirrorType(MIRROR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME31;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME31);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME31);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME31);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz672);
    //
  } else if (chamber_type_string_ == "ME32") {
    //
    SetExpectedFastControlRegularMirrorType(MIRROR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME32;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME32);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME32);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME32);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  } else if (chamber_type_string_ == "ME41") {
    //
    SetExpectedFastControlRegularMirrorType(MIRROR_FIRMWARE_TYPE);
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME41;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME41);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME41);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME41);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz672);
    //
  } else if (chamber_type_string_ == "ME42") {
    //
    NumberOfWireGroupsInChamber_ = NUMBER_OF_WIRE_GROUPS_ME42;
    SetFastControlAlctType_(FAST_CONTROL_ALCT_TYPE_ME42);
    SetSlowControlAlctType_(SLOW_CONTROL_ALCT_TYPE_ME42);
    SetExpectedFastControlFPGAIdCode(FAST_FPGA_ID_ME42);
    tmb_->SetNumberOfChipsAlctFastMezz(NumberChipsAlctFastMezz288_384);
    //
  }
  //
  if (NumberOfChannelsInAlct_ == 0) {
    //
    (*MyOutput_) << "ALCTController: ERROR Invalid ALCT type " << chamber_type_string_ << std::endl;
    //
  } else {
    //
    //the number of channels per layer depends on the ALCT type, not the physical number of channels in the layer:
    NumberOfChannelsPerLayer_ = GetNumberOfChannelsInAlct() / MAX_NUM_LAYERS;  
    //
    tmb_->SetAlctTypeForProm(GetNumberOfChannelsInAlct());
    //
    (*MyOutput_) << "........................ ALCT type = " << std::dec << GetNumberOfChannelsInAlct() << std::endl; 
    (*MyOutput_) << "............ Number of Wire Groups = " << std::dec << GetNumberOfWireGroupsInChamber() << std::endl; 
    (*MyOutput_) << "........ Number of Wires per layer = " << GetNumberOfChannelsPerLayer() << std::endl;
    (*MyOutput_) << ".. Number of groups of delay chips =  " << GetNumberOfGroupsOfDelayChips() << std::endl; 
    (*MyOutput_) << "Number of collision pattern groups = " << GetNumberOfCollisionPatternGroups() << std::endl;
    (*MyOutput_) << ".................. Number of AFEBs = " << GetNumberOfAfebs() << std::endl;
    (*MyOutput_) << "...hardware AFEB indices count from " << GetLowestAfebIndex() << " to " << GetHighestAfebIndex() << std::endl;
    (*MyOutput_) << "..while the user indices count from 0 to " << MaximumUserIndex() << std::endl;
  }
  //
  return;
}
//
void ALCTController::SetFastControlAlctType_(int type_of_fast_control_alct) {
  //
  if (type_of_fast_control_alct == 192) {
    SetExpectedFastControlAlctType(FIRMWARE_TYPE_192); 
  } else if (type_of_fast_control_alct == 288) {
    SetExpectedFastControlAlctType(FIRMWARE_TYPE_288); 
  } else if (type_of_fast_control_alct == 384) {
    SetExpectedFastControlAlctType(FIRMWARE_TYPE_384); 
  } else if (type_of_fast_control_alct == 576) {
    SetExpectedFastControlAlctType(FIRMWARE_TYPE_576); 
  } else if (type_of_fast_control_alct == 672) {
    SetExpectedFastControlAlctType(FIRMWARE_TYPE_672); 
  } else {
    SetExpectedFastControlAlctType(9999); 
  }
  NumberOfChannelsInAlct_ = type_of_fast_control_alct;
  NumberOfGroupsOfDelayChips_ = 0;
  //
  RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 0;
  RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 0;
  RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 0;
  RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ = 0;
  RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ = 0;
  RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ = 0;
  //
  if (NumberOfChannelsInAlct_ == 192) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_192; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_192;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_192; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_192; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_192; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_192; 
    //
    NumberOfGroupsOfDelayChips_ = NUMBER_OF_GROUPS_OF_DELAY_CHIPS_192;
    //
  } else if (NumberOfChannelsInAlct_ == 288) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_288; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_288;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_288; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_288; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_288; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_288; 
    //
    NumberOfGroupsOfDelayChips_ = NUMBER_OF_GROUPS_OF_DELAY_CHIPS_288;
    //
  } else if (NumberOfChannelsInAlct_ == 384) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_384; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_384;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_384; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_384; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_384; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_384; 
    //
    NumberOfGroupsOfDelayChips_ = NUMBER_OF_GROUPS_OF_DELAY_CHIPS_384;
    //
  } else if (NumberOfChannelsInAlct_ == 576) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_576; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_576;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_576; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_576; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_576; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_576; 
    //
    NumberOfGroupsOfDelayChips_ = NUMBER_OF_GROUPS_OF_DELAY_CHIPS_576;
    //
  } else if (NumberOfChannelsInAlct_ == 672) {
    //
    RegSizeAlctFastFpga_RD_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_RD_HOTCHAN_MASK_672; 
    RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_ = 
      RegSizeAlctFastFpga_WRT_HOTCHAN_MASK_672;
    RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_ = 
      RegSizeAlctFastFpga_RD_COLLISION_MASK_REG_672; 
    RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_ =
      RegSizeAlctFastFpga_WRT_COLLISION_MASK_REG_672; 
    RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_RD_DELAYLINE_CTRL_REG_672; 
    RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_ =
      RegSizeAlctFastFpga_WRT_DELAYLINE_CTRL_REG_672; 
    //
    NumberOfGroupsOfDelayChips_ = NUMBER_OF_GROUPS_OF_DELAY_CHIPS_672;
    //
  }
  //
  NumberOfCollisionPatternGroups_ = type_of_fast_control_alct / MAX_NUM_LAYERS / 8;
  //
  if (NumberOfGroupsOfDelayChips_ == 0) {
    //
    (*MyOutput_) << "ALCTController: ERROR Invalid ALCT type " 
		 << std::dec << type_of_fast_control_alct << std::endl;
    NumberOfChannelsInAlct_ = 0;
  } 
  //
  return;
}
void ALCTController::SetSlowControlAlctType_(int type_of_slow_control_alct) {
  //
  // As far as the slow control FPGA is concerned, the number of AFEBs 
  // is tied to the number of asic chips physically on the board, which 
  // is not necessarily the Fast Control ALCT type, or the number of
  // wire groups in the chamber...:
  //
  NumberOfAFEBs_ = type_of_slow_control_alct / NUMBER_OF_LINES_PER_CHIP;
  //
  // If wanted, we can disable the lowest numbered AFEBs from being controlled by the
  // user by enabling the following lower afeb index:
  lowest_afeb_index_ = GetNumberOfAfebs() - GetNumberOfWireGroupsInChamber() / NUMBER_OF_LINES_PER_CHIP;
  //
  // Here we enable the expert to control these afebs, even though they are not connected
  // to any wires....:
  //lowest_afeb_index_ = 0;
  //
  highest_afeb_index_ = GetNumberOfAfebs() - 1;
  //
  return;
}
//
//
int ALCTController::UserIndexToHardwareIndex_(int index) { 
  //
  int index_to_return = 999;
  //
  // The idea of this method is to allow the user always to access the AFEBs using indices
  // labeled 0 to MaximumUserIndex()-1, regardless of how these AFEBs are connected to the ALCT.  
  //
  // The user needs access to AFEBs in order to
  //  1) power them on or off via the standby register (slow control FPGA)
  //  2) write delay values and patterns to ASICs (fast control FPGA)
  //  3) write threshold DAC values and readback threshold ADC values (slow control FPGA)
  //
  // There are 3 types of ALCT boards:
  //  a) ALCT288 are used for chambers which have 192 (ME1/3) and 288 (ME1/1) wiregroups.
  //  b) ALCT384 are used for chambers which have 384 wiregroups (ME1/2, ME2/2, ME3/2, and ME4/2)
  //  c) ALCT672 are used for chambers which have 576 (ME3/1 and ME4/1) and 672 (ME2/1) wiregroups
  //
  // As such, how the AFEBs are accessed depends on:
  //  - the number of wiregroups in the chamber (defines the number of AFEBs)
  //  - the ALCT type (each ALCT type has a set number of AFEB input connections)
  //  - how the AFEBs are connected to the ALCT
  //
  // On the chambers which have fewer AFEBs than AFEB input connections (i.e., smaller 
  // number of wiregroups than the ALCT type), the lowest 6 AFEBs are not connected to 
  // the ALCT.  See ALCTController::SetSlowControlAlctType_(int).
  // For ME11 chambers, the positive-forward chambers and the negative-backward 
  // chambers are cabled such that the AFEBs labeled 1-18 are connected to the 
  // AFEB inputs on the ALCT from 18-1.
  //
  if (GetChamberType() == "ME11" &&
      ( (GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE && 
	 GetExpectedFastControlBackwardForwardType()  == FORWARD_FIRMWARE_TYPE   ) || 
	(GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE && 
	 GetExpectedFastControlBackwardForwardType()  == BACKWARD_FIRMWARE_TYPE   )  ) ) {
    //
    // invert the order from 0->17 to 17->0
    //
    index_to_return = abs(index - 17);
    //
  } else {
    index_to_return = index + GetLowestAfebIndex();
  } 
  //
  return index_to_return;
}
//
// Methods used to program ALCT prom: 
//
int ALCTController::SVFLoad(int * arg1, const char * arg2, int arg3) { 
  //
  // in concert with the return codes for CheckFirmwareConfiguration():
  // ALCTController::SVFLoad return codes:
  //  >= 0 = number of errors detected from EMUjtag::SVFLoad during the loading of ALCT firmware
  //    -1 = ALCT firmware NOT loaded due to database check failure
  //  < -1 = Number of database errors - 1.  Yell at expert.
  //
  int check_value = CheckFirmwareConfiguration();
  //
  if ( check_value ==  1 ||
       check_value == -1 ) {
    //
    return tmb_->SVFLoad(arg1,arg2,arg3); 
    //
  } 
  //
  check_value = -check_value - 1;
  //
  return check_value;
  //
}
//
void ALCTController::ProgramALCTProms() { 
  //
  int check_value = CheckFirmwareConfiguration();
  //
  if ( check_value ==  1 ||
       check_value == -1 ) {
    //
    tmb_->ProgramALCTProms(); 
    //
  } 
  //
  return;
}
//
int ALCTController::CheckFirmwareConfiguration() {
  //
  // return codes:
  //  -1 = user has entered the keywords in the xml file to eschew the database check.  Caveat emptor.
  //   0 = database check fail.  DO NOT LOAD FIRMWARE, CALL EXPERT.
  //   1 = database check pass.  Load Firmware.
  //  >1 = database failure.  Yell at expert.
  //
  // First get the configuration parameters set in the xml file...
  //
  // These two control VME access to the ALCT:
  std::string xml_VCCIpAddress = tmb_->getCrate()->vmeController()->ipAddress();
  int         xml_tmb_slot     = tmb_->slot();
  //
  // The next 6 are a sanity check on the configuration parameters:
  std::string xml_crateLabel             = tmb_->getCrate()->GetLabel();
  std::string xml_chamberLabel           = tmb_->getChamber()->GetLabel();
  std::string xml_chambertype            = GetChamberType();
  int         xml_backward_forward_type  = GetExpectedFastControlBackwardForwardType();
  int         xml_negative_positive_type = GetExpectedFastControlNegativePositiveType();
  int         xml_regular_mirror_type    = GetExpectedFastControlRegularMirrorType();
  //
  // The following is to enforce that the above firmware is appropriate for the intended FPGA:
  ReadFastControlMezzIDCodes();
  int read_fpga_id = GetFastControlMezzFPGAID();
  //
  // Has the user eschewed the database check?
  if ( xml_crateLabel   == "test_crate"   && 
       xml_chamberLabel == "test_chamber" ) {
    return -1;
  }
  //
  int return_value = 0;
  //
  for (unsigned int config_index=0; config_index<number_of_allowed_firmware_configurations; config_index++) {
    //
    if ( xml_VCCIpAddress           == allowed_firmware_config[config_index].VCCIpAddress           && 
	 xml_tmb_slot               == allowed_firmware_config[config_index].tmb_slot               && 
	 xml_crateLabel             == allowed_firmware_config[config_index].crateLabel             && 
	 xml_chamberLabel           == allowed_firmware_config[config_index].chamberLabel           && 
	 xml_chambertype            == allowed_firmware_config[config_index].chambertype            &&
	 xml_backward_forward_type  == allowed_firmware_config[config_index].backward_forward_type  &&
	 xml_negative_positive_type == allowed_firmware_config[config_index].negative_positive_type &&
	 xml_regular_mirror_type    == allowed_firmware_config[config_index].regular_mirror_type    &&
	 read_fpga_id               == allowed_firmware_config[config_index].fpga_id                ) {
      //
      return_value++;
    }
  }
  //
  if (return_value == 0) {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;    
    std::cout << "!!! ERROR:  Not loading firmware to ALCT with following configuration !!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;    
    std::cout << "VCC IP address    = " << xml_VCCIpAddress << std::endl;
    std::cout << "TMB slot number   = " << std::dec << xml_tmb_slot     << std::endl;
    std::cout << "Crate label       = " << xml_crateLabel   << std::endl;
    std::cout << "Chamber label     = " << xml_chamberLabel << std::endl;
    std::cout << "Chamber type      = " << xml_chambertype  << std::endl;
    std::cout << "Backward/forward  = ";
    if ( xml_backward_forward_type == BACKWARD_FIRMWARE_TYPE ) {
      std::cout << "backward";
    } else if ( xml_backward_forward_type == FORWARD_FIRMWARE_TYPE ) {
      std::cout << "forward";
    } else if ( xml_backward_forward_type == DO_NOT_CARE ) {
      std::cout << "do not care";
    }
    std::cout << std::endl;
    //
    std::cout << "Negative/positive = ";
    if ( xml_negative_positive_type  == NEGATIVE_FIRMWARE_TYPE ) {
      std::cout << "negative ";
    } else if ( xml_negative_positive_type == POSITIVE_FIRMWARE_TYPE ) {
      std::cout << "positive ";
    } else if ( xml_negative_positive_type == DO_NOT_CARE ) {
      std::cout << "do not care";
    }
    std::cout << std::endl;
    //
    std::cout << "Regular/mirror   = ";
    if ( xml_regular_mirror_type == REGULAR_FIRMWARE_TYPE ) {
      std::cout << "non-mirrored";
    } else  if ( xml_regular_mirror_type == MIRROR_FIRMWARE_TYPE ) {
      std::cout << "mirrored";
    } else {
      std::cout << "unknown";
    }
    std::cout << std::endl;
    //
    std::cout << "read FPGA ID   = 0x" << std::hex << read_fpga_id << std::endl;
    //
  } else if (return_value == 1) {
    //
    std::cout << "ALCT firmware database check OK. Will load firmware to ALCT..." << std::endl;
    //
  } else if (return_value > 1) {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;    
    std::cout << "!!! DATABASE ERROR:  Not loading firmware to ALCT  !!!" << std::endl;
    std::cout << "!!!                                                !!!" << std::endl;
    std::cout << "!!!                CONTACT EXPERT                  !!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;    
  }
  //
  //
  return return_value;
  //
}

  } // namespace emu::pc
  } // namespace emu
