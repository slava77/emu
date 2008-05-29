//-----------------------------------------------------------------------
// $Id: TMB.cc,v 3.63 2008/05/29 11:36:06 liu Exp $
// $Log: TMB.cc,v $
// Revision 3.63  2008/05/29 11:36:06  liu
// add time-since-last-hard_reset in TMB counters
//
// Revision 3.62  2008/05/26 08:24:43  rakness
// for AFEB calibrations:  argument for TMB and ALCT::configure(2) to not write userPROMs; correctly respond to configuration written to broadcast slot
//
// Revision 3.61  2008/05/20 11:30:20  liu
// TMB counters in jumbo packet
//
// Revision 3.60  2008/05/12 10:23:16  rakness
// return control to VME bus after checking TMB PROM/FPGA IDs
//
// Revision 3.59  2008/04/19 14:56:55  rakness
// ALCT database check before loading ALCT firmware
//
// Revision 3.58  2008/02/28 18:36:36  rakness
// make TMB firmware loading robust against all failure modes except power cuts...
//
// Revision 3.57  2008/02/23 15:25:54  liu
// TMB online counters
//
// Revision 3.56  2008/01/18 15:42:15  rakness
// get methods for TMB/RAT on-board voltages and currents
//
// Revision 3.55  2008/01/07 15:08:55  rakness
// add xml parameters:  clct_stagger, clct_blanking, clct_pattern_id_thresh, aff_thresh, min_clct_separation.  Remove xml parameter:  clct_distrip_pretrig_thresh
//
// Revision 3.54  2007/12/06 15:12:41  rakness
// make scan parameters for synchronization configurable from hyperDAQ
//
// Revision 3.53  2007/11/07 08:54:57  rakness
// make the csc_id which is injected into the MPC injector RAM always be the correct one for this TMB
//
// Revision 3.52  2007/10/25 17:36:12  rakness
// Add option to enable/disable write to USER JTAG register to allow selective masking of broadcast JTAG commands.  Also enable/disable clocks with explicit write rather than read,write
//
// Revision 3.51  2007/10/24 13:21:31  rakness
// try to clean up and document TMB to MPC methods
//
// Revision 3.50  2007/10/08 15:04:36  rakness
// add ALCT and TMB raw hits button in hyperDAQ
//
// Revision 3.49  2007/08/27 11:28:34  geurts
// explicitly set an integer constant to long long int by suffixing it with LL
//
// Revision 3.48  2007/08/22 13:39:11  rakness
// add distrip hotchannel mask to xml file
//
// Revision 3.47  2007/08/16 11:40:23  rakness
// add Raw Hits Write Buffer Reset Counter
//
// Revision 3.46  2007/08/15 12:40:56  rakness
// determine sync parameters w/1 button, clean up output, control level of cout with debug_
//
// Revision 3.45  2007/08/06 14:20:08  rakness
// pulse teststrips and measure ALCT in CLCT match window
//
// Revision 3.44  2007/08/03 14:35:40  rakness
// begin commenting for doxygen, add hot-channel mask write, add writeregister together with fillregister
//
// Revision 3.43  2007/08/01 11:40:38  rakness
// reorder sleep in TMB raw hits to maximize chances of good data readout
//
// Revision 3.42  2007/07/26 13:09:32  rakness
// update CFEB rx scan for CLCT key layer 3 -> 2 change
//
// Revision 3.41  2007/07/24 11:15:35  rakness
// more bits checked in TMB-MPC test. Sorting algorithm based only on data which is passed
//
// Revision 3.40  2007/07/20 15:13:00  rakness
// improve emulation of MPC and TMB in TMB-MPC crate test
//
// Revision 3.39  2007/06/26 14:39:14  rakness
// fix cfeb_enable_expected bug
//
// Revision 3.38  2007/06/22 12:28:11  rakness
// fix checking of register 0x68 based on which registers are enabled
//
// Revision 3.37  2007/06/21 16:14:03  rakness
// online measurement of ALCT in CLCT matching window
//
// Revision 3.36  2007/06/14 14:47:55  rakness
// clean up MPC injection
//
// Revision 3.35  2007/06/12 09:56:57  rakness
// clean TMB Raw Hits
//
// Revision 3.34  2007/06/07 12:57:28  rakness
// update TMB counters
//
// Revision 3.33  2007/05/17 12:52:50  rakness
// ignore_ccb_startstop added to TMB configuration + write configuration to userPROM default
//
// Revision 3.32  2007/04/10 13:31:01  rakness
// add mpc_output_enable, remove rpc2/3
//
// Revision 3.30  2007/03/21 12:56:51  rakness
// update labels of TMB counters
//
// Revision 3.29  2007/03/14 08:59:03  rakness
// make parser dumb
//
// Revision 3.28  2007/03/08 03:14:20  liu
// include math.h
//
// Revision 3.27  2007/01/31 16:50:06  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.26  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.25  2006/11/10 12:43:07  rakness
// include TMB/ALCT configuration and state machine prints+checks to hyperDAQ
//
// Revision 3.24  2006/11/09 08:47:51  rakness
// add rpc0_raw_delay to xml file
//
// Revision 3.23  2006/10/21 17:40:58  mey
// Got rid of last commit
//
// Revision 3.21  2006/10/14 10:09:08  mey
// UPdate
//
// Revision 3.20  2006/10/13 15:34:39  rakness
// add mpc_phase
//
// Revision 3.19  2006/10/12 15:56:02  rakness
// cleaned up configuration checking for ALCT/TMB
//
// Revision 3.18  2006/10/10 15:34:58  rakness
// check TMB/ALCT configuration vs xml
//
// Revision 3.17  2006/10/06 12:15:40  rakness
// expand xml file
//
// Revision 3.16  2006/09/28 12:52:35  mey
// Update
//
// Revision 3.15  2006/09/24 13:34:39  rakness
// decode configuration registers
//
// Revision 3.14  2006/09/15 07:50:41  rakness
// dump config registers
//
// Revision 3.13  2006/09/14 11:47:57  mey
// update
//
// Revision 3.12  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.11  2006/09/08 00:06:32  mey
// UPdate
//
// Revision 3.10  2006/09/07 15:23:05  rakness
// pull programming back into EMUjtag
//
// Revision 3.9  2006/09/06 12:38:11  rakness
// correct time stamp/copy vectors for user prom
//
// Revision 3.8  2006/09/05 10:13:17  rakness
// ALCT configure from prom
//
// Revision 3.7  2006/08/11 16:23:33  rakness
// able to write TMB user prom from configure()
//
// Revision 3.6  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.5  2006/08/09 11:57:04  mey
// Got rid of version
//
// Revision 3.4  2006/08/09 09:39:47  mey
// Moved TMB_trgmode to TMB.cc
//
// Revision 3.3  2006/08/08 19:40:00  mey
// Fixed bug
//
// Revision 3.2  2006/08/08 19:23:08  mey
// Included Jtag sources
//
// Revision 3.1  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.75  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.74  2006/07/18 14:12:47  mey
// Update
//
// Revision 2.73  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.72  2006/07/12 12:07:11  mey
// ALCT connectivity
//
// Revision 2.71  2006/07/11 13:23:15  mey
// Update
//
// Revision 2.70  2006/07/11 13:02:42  mey
// fixed bug
//
// Revision 2.69  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.68  2006/06/22 13:06:14  mey
// Update
//
// Revision 2.67  2006/06/20 13:18:17  mey
// Update
//
// Revision 2.66  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.65  2006/06/12 12:47:17  mey
// Update
//
// Revision 2.64  2006/05/24 09:55:03  mey
// Added crate counters
//
// Revision 2.63  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.62  2006/05/10 10:24:32  mey
// Update
//
// Revision 2.61  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.60  2006/04/25 13:25:19  mey
// Update
//
// Revision 2.59  2006/04/11 15:27:42  mey
// Update
//
// Revision 2.58  2006/04/06 22:23:08  mey
// Update
//
// Revision 2.57  2006/03/28 10:44:21  mey
// Update
//
// Revision 2.56  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.55  2006/03/22 14:36:52  mey
// UPdate
//
// Revision 2.54  2006/03/21 12:27:02  mey
// Update
//
// Revision 2.53  2006/03/20 13:34:40  mey
// Update
//
// Revision 2.52  2006/03/20 09:10:43  mey
// Update
//
// Revision 2.51  2006/03/17 15:51:04  mey
// New routines
//
// Revision 2.50  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.49  2006/03/09 22:30:16  mey
// Jinghua's updates
//
// Revision 2.48  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.47  2006/03/05 18:45:08  mey
// Update
//
// Revision 2.46  2006/03/03 07:59:20  mey
// Update
//
// Revision 2.45  2006/02/06 14:09:07  mey
// Fixed bug
//
// Revision 2.44  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 2.43  2006/02/02 14:27:32  mey
// Update
//
// Revision 2.42  2006/02/01 18:31:50  mey
// Update
//
// Revision 2.41  2006/02/01 13:30:55  mey
// Fixed ADC readout
//
// Revision 2.40  2006/01/31 14:42:14  mey
// Update
//
// Revision 2.39  2006/01/31 08:52:13  mey
// Update
//
// Revision 2.38  2006/01/23 15:00:10  mey
// Update
//
// Revision 2.37  2006/01/23 13:56:53  mey
// Update using Greg's new code
//
// Revision 2.36  2006/01/20 09:34:30  mey
// Got rid of LATER
//
// Revision 2.35  2006/01/18 12:45:44  mey
// Cleaned up old code
//
// Revision 2.34  2006/01/14 22:25:08  mey
// UPdate
//
// Revision 2.33  2006/01/12 23:44:47  mey
// Update
//
// Revision 2.32  2006/01/12 22:36:09  mey
// UPdate
//
// Revision 2.31  2006/01/12 12:28:29  mey
// UPdate
//
// Revision 2.30  2006/01/12 12:14:53  mey
// Update
//
// Revision 2.29  2006/01/12 11:48:12  mey
// Update
//
// Revision 2.28  2006/01/12 11:32:30  mey
// Update
//
// Revision 2.27  2006/01/11 16:58:17  mey
// Update
//
// Revision 2.26  2006/01/11 13:47:51  mey
// Update
//
// Revision 2.25  2006/01/09 07:17:37  mey
// Update
//
// Revision 2.24  2005/12/15 14:25:09  mey
// Update
//
// Revision 2.23  2005/12/05 18:11:17  mey
// UPdate
//
// Revision 2.22  2005/11/30 16:26:07  mey
// Redirect output
//
// Revision 2.21  2005/11/25 14:45:07  mey
// UPdate
//
// Revision 2.20  2005/11/21 18:08:38  mey
// UPdate
//
// Revision 2.19  2005/11/21 17:38:34  mey
// Update
//
// Revision 2.18  2005/11/07 10:12:05  mey
// Byte swap
//
// Revision 2.17  2005/10/06 14:48:32  mey
// Added tmb trigger test
//
// Revision 2.16  2005/10/05 14:24:19  mey
// Added tests
//
// Revision 2.15  2005/10/04 16:01:17  mey
// Update
//
// Revision 2.14  2005/09/28 16:52:40  mey
// Include Output streamer
//
// Revision 2.13  2005/09/15 08:13:48  mey
// CSC id update
//
// Revision 2.12  2005/09/06 12:11:32  mey
// Added accessors
//
// Revision 2.11  2005/08/31 15:12:58  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.10  2005/08/23 15:49:54  mey
// Update MPC injector for random LCT patterns
//
// Revision 2.9  2005/08/22 16:58:32  mey
// Fixed bug in TMB-MPC injector
//
// Revision 2.8  2005/08/22 16:38:27  mey
// Added TMB-MPC injector
//
// Revision 2.7  2005/08/22 07:55:45  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.6  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.5  2005/08/15 15:37:57  mey
// Include alct_hotchannel_file
//
// Revision 2.4  2005/08/12 14:16:03  mey
// Added pulsing vor TMB-MPC delay
//
// Revision 2.2  2005/07/08 10:33:32  geurts
// allow arbitrary scope trigger channel in TMB::scope()
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "TMB.h"
#include "JTAG_constants.h"
#include "VMEController.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <sstream>
#include<math.h>

#include "TMB_constants.h"
#include "ALCTController.h"
#include "RAT.h"
#include "Chamber.h"

// the VME addresses here are defined in
// http://www-collider.physics.ucla.edu/cms/trigger/tmb2001/tmb2001_spec.pdf

TMB::TMB(Crate * theCrate, Chamber * theChamber, int slot) :
  VMEModule(theCrate, slot),
  EMUjtag(this),
  alctController_(0),
  rat_(0),
  csc_(theChamber)
{
  //
  debug_ = false;
  //
  ucla_ldev = 1;
  //
  theChamber->SetTMB(this);
  //
  std::cout << "Inside TMB" << std::endl;
  //
  MyOutput_ = &std::cout ;
  (*MyOutput_) << "TMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
  //
  //
  SetTMBRegisterDefaults_();
  DefineTMBConfigurationRegisters_();
  //
  tmb_configuration_status_  = -1;
  vme_state_machine_status_  = -1;
  jtag_state_machine_status_ = -1;
  ddd_state_machine_status_  = -1;
  raw_hits_header_status_    = -1;
} 


TMB::~TMB() {
  (*MyOutput_) << "destructing ALCTController" << std::endl; 
  delete alctController_; 
  delete rat_;
  (*MyOutput_) << "destructing TMB" << std::endl;
}
//
Crate * TMB::getCrate() {
  //
  return theCrate_;
}
//
Chamber * TMB::getChamber() {
  //
  return csc_;
}
//
int TMB::MPC0Accept(){
  //
  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW);
  //
  return (rcvbuf[0]&0x2)>>1; 
  //
}
//
int TMB::MPC1Accept(){
  //
  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW);
  //
  return (rcvbuf[0]&0x4)>>2; 
  //
}
//
void TMB::DumpRegister(int reg){
  //
  int value = ReadRegister(reg);
  //
  (*MyOutput_) << " TMB.reg=" 
	       << std::hex << reg << " " 
	       << (rcvbuf[0]&0xff) << " " 
	       << (rcvbuf[1]&0xff) << " " 
	       << (value&0xffff) << std::endl ;
  //
}
//
int TMB::ReadRegister(int reg){
  //
  tmb_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  DecodeTMBRegister_(reg,value);
  //
  return value;
  //
}
//
void TMB::ReadTmbIdCodes() {
  //Get ID codes for the following devices:
  //tmb_idcode_[0] = TMB Mezz FPGA IDCode
  //           [1] = TMB Mezz PROM 0 IDCode
  //           [2] = TMB Mezz PROM 1 IDCode
  //           [3] = TMB Mezz PROM 2 IDCode
  //           [4] = TMB Mezz PROM 3 IDCode
  //           [5] = TMB User PROM 0 IDCode
  //           [6] = TMB User PROM 1 IDCode
  //
  int device;
  for (device=0; device<7; device++) 
    tmb_idcode_[device] = 0;
  //
  device = 0;
  //
  setup_jtag(ChainTmbMezz);
  //
  ShfIR_ShfDR(ChipLocationTmbMezzFpga,
	      FPGAidCode,
	      RegSizeTmbMezzFpga_FPGAidCode);
  tmb_idcode_[device++] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  //
  int chip_location;
  for (chip_location=1; chip_location<=4; chip_location++){
    ShfIR_ShfDR(chip_location,
		PROMidCode,
		RegSizeTmbMezzProm_PROMidCode);
    //
    tmb_idcode_[device++] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  }
  //
  short unsigned int BootReg;
  tmb_get_boot_reg(&BootReg);
  BootReg &= 0xff7f;                    // Give JTAG chain to the FPGA to configure ALCT on hard reset
  BootReg &= 0xf7ff;                    // Allow FPGA access to the VME register
  tmb_set_boot_reg(BootReg);
  //
  //
  setup_jtag(ChainTmbUser);
  //
  for (chip_location=0; chip_location<=1; chip_location++){
    ShfIR_ShfDR(chip_location,
		PROMidCode,
		RegSizeTmbUserProm_PROMidCode);
    tmb_idcode_[device++] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  }
  //
  return;
}
//
int TMB::ConvertToHexAscii(int value_to_convert) { 
  //
  // convert the argument to its "hex-ascii" value:  i.e.  2007 -> 0x2007
  //
  //  std::cout << "value_to_convert = " << std::dec << value_to_convert << std::endl;
  //
  int hex_ascii_value = 0;
  int reduced_value = value_to_convert;
  int number_of_bits_to_shift = 0;
  //
  while (reduced_value) {
    //    std::cout << "reduced_value = " << std::dec << reduced_value << std::endl;
    // here is the 1's digit:
    int ones_digit = reduced_value % 10;
    //
    //    std::cout << "ones_digit = " << std::dec << ones_digit << std::endl;
    //
    // insert ones_digit into the next most significant hex digit:
    hex_ascii_value |= (ones_digit & 0xf) << number_of_bits_to_shift;
    //    std::cout << "hex_ascii_value = " << std::hex << hex_ascii_value << std::endl;
    number_of_bits_to_shift += 4;
    //
    // remove the ones digit:
    reduced_value /= 10;
  }
  //
  //  std::cout << "hex_ascii_value to return = " << std::hex << hex_ascii_value << std::endl;
  //
  return hex_ascii_value;
}
//
int TMB::FirmwareDate(){
  //
  tmb_vme(VME_READ,vme_idreg1_adr,sndbuf,rcvbuf,NOW);
  //
  int data = (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
  read_tmb_firmware_day_   = data & 0xff;
  read_tmb_firmware_month_ = ((data >> 8) & 0xff);
  //
  return data;
  //
}

int TMB::FirmwareYear(){
  //
  tmb_vme(VME_READ,vme_idreg2_adr,sndbuf,rcvbuf,NOW);
  //
  int data = (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
  read_tmb_firmware_year_ = data;
  //
  return data;
  //
}


int TMB::FirmwareVersion(){
  //
  tmb_vme(VME_READ,vme_idreg0_adr,sndbuf,rcvbuf,NOW);
  //
  int data = (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
  read_tmb_firmware_type_    = data & 0xf;
  read_tmb_firmware_version_ = (data >> 4) & 0xf;
  //
  return data;
  //
}

int TMB::FirmwareRevCode(){
  //
  tmb_vme(VME_READ,vme_idreg3_adr,sndbuf,rcvbuf,NOW);
  //
  int data = (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
  read_tmb_firmware_revcode_ = (data>>12) & 0x000f ;
  //
  return data;
  //
}
//
int TMB::PowerComparator(){
  //
  tmb_vme(VME_READ,vme_adc_adr,sndbuf,rcvbuf,NOW);
  //
  return (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
}
//
int TMB::CCB_command_from_TTC(){
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x0;
  tmb_vme(VME_READ,ccb_stat_adr,sndbuf,rcvbuf,NOW);
  //
  return (rcvbuf[1]&0xff);
  //
}
//
void TMB::WriteOutput(std::string output){
  //
  (*MyOutput_) << output << std::endl ;
  //
  //LOG4CPLUS_INFO(getApplicationLogger(), output);
  //
}
//
void TMB::StartTTC(){
  //
  WriteOutput("TMB:  StartTriggers");
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;      // Disconnect CCB backplane
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x6;      // TTC command to generate (Trig Start--needed if ignore_ccb_startstop=0 or
  //                       for old versions of TMB firmware)
  sndbuf[1] = 0x3;      // Disconnect CCB backplane + Assert internal CCB command Broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;      // Disconnect CCB backplane + Un-assert internal CCB cmd broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x3;      // TTC command to generate (Resync)
  sndbuf[1] = 0x3;      // Disconnect CCB backplane + Assert internal CCB command Broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;      // Disconnect CCB backplane + Un-assert internal CCB cmd broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x1;      // TTC command to generate (BC0)
  sndbuf[1] = 0x3;      // Disconnect CCB backplane + Assert internal CCB command Broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;      // Disconnect CCB backplane + Un-assert internal CCB command Broadcast strobe
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x0;      // Connect CCB backplane
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::WriteRegister(int reg, int value){
  //
  sndbuf[0] = (value>>8)&0xff;
  sndbuf[1] = value&0xff;
  //
  tmb_vme(VME_WRITE,reg,sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::WriteRegister(int address) {
  //
  int data_to_write = FillTMBRegister(address);
  //
  if (data_to_write < 0x10000) {
    //
    WriteRegister(address,data_to_write);
    //
  } else {
    //
    (*MyOutput_) << "TMB: ERROR in WriteRegister, data word too big to write" << std::endl;
  }
  return;
}
//
bool TMB::SelfTest() {
  //
  return 0;
}
//
void TMB::init() {
  //
  return;
}
//
void TMB::configure() {
  //
  this->configure(0); //no argument means write configuration to user PROM
  //
  return;
}
//
void TMB::configure(int c) {
  //
  // c = 2 = do not write configuration to userPROM
  //
  if (c == 2) { 
    SetFillVmeWriteVecs(false);     //do not write configuration to user PROM
  } else {
    SetFillVmeWriteVecs(true);     //write configuration to user PROM
  }
  ClearVmeWriteVecs();
  //
  ostringstream dump;
  ostringstream dump2;
  //
  dump  << "TMB : configuring in slot = ";
  dump2 << theSlot;
  (*MyOutput_) << dump.str()+dump2.str() << std::endl;
  SendOutput(dump.str()+dump2.str(),"INFO");
  //
  for (unsigned int index=0; index<TMBConfigurationRegister.size(); index++) {
    //
    unsigned long int address = TMBConfigurationRegister.at(index);
    //
    //don't write to the user FPGA JTAG address, since it is used for ALCT configuration:
    if ( address == vme_usr_jtag_adr ) continue;
    //
    WriteRegister(address);
    //
  }
  //
  // When configuring with VME, the 3d3444 state machine needs to be 
  // started and stopped appropriately in order for the delay values 
  // to be set correctly:
  if ( !GetFillVmeWriteVecs() ) {
    WriteRegister(vme_dddsm_adr,0x20);
    WriteRegister(vme_dddsm_adr,0x21);
    WriteRegister(vme_dddsm_adr,0x20);
  }
  //
  // The flag to fill the VME register vector is set => program the user PROM:
  if ( GetFillVmeWriteVecs() )      
    CheckAndProgramProm(ChipLocationTmbUserPromTMB);
  //
  SetFillVmeWriteVecs(false);    //give VME back to the user (default)
  //
  if (this->slot()<22)           //broadcast read will not work, so only check configuration if it is a normal VME slot
    CheckTMBConfiguration();  
  //
}
//
void TMB::SetTrgmode_() {  
  //
  // To be deprecated.  Replaced by explicitly setting each pretrigger/trigger bit
  //
  // set the combinations of bits for 
  // register 0x68 = ADR_SEQ_TRIG_EN  and
  // register 0x86 = ADR_TMB_TRIG    
  // according to the setting of trgmode_
  //
  // clear the settings on the pattern trigger enable (0x68):
  clct_pat_trig_en_  = 0;
  alct_pat_trig_en_  = 0;
  match_pat_trig_en_ = 0;
  adb_ext_trig_en_   = 0;
  dmb_ext_trig_en_   = 0;
  clct_ext_trig_en_  = 0;
  alct_ext_trig_en_  = 0;
  vme_ext_trig_   = 0;
  //
  // clear the settings on 0x86:
  tmb_allow_clct_    = 0;
  tmb_allow_alct_    = 0;
  tmb_allow_match_   = 1;  //always set the TMB to allow matched 
  //
  if ( trgmode_ == CLCT_trigger ) { 
    //
    clct_pat_trig_en_ = 1;
    tmb_allow_clct_ = 1;
    //
  } else if ( trgmode_ == ALCT_trigger ) { 
    //
    alct_pat_trig_en_ = 1;
    tmb_allow_alct_ = 1;
    //
  } else if ( trgmode_ == Scintillator_trigger ) { 
    //
    clct_ext_trig_en_ = 1;
    //
  } else if ( trgmode_ == DMB_trigger ) { 
    //
    dmb_ext_trig_en_ = 1;
    //
  } else if ( trgmode_ == ALCT_CLCT_coincidence_trigger ) { 
    //
    match_pat_trig_en_ = 1;
    //
  } 
  //
  return;
}
void TMB::clear_i2c() {
  //
  (*MyOutput_) << "Done so unstart state machine" << std::endl ;
  sndbuf[0]=0x8c;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::InjectMPCData(const int nEvents, const unsigned long lct0, const unsigned long lct1){
  //
  unsigned short frame1, frame2, ramAdd;
  //
  (*MyOutput_) << "TMB:  Inject " << nEvents << " events with 2 muons into MPC data to slot " << this->slot() << std::endl ;
  //
  for (int evtId(0); evtId<nEvents; ++evtId) {
    //
    ramAdd = (evtId<<8);
    //
    unsigned short vpf      = 1;         
    unsigned short sync_err = 0;
    unsigned short qual1;
    unsigned short qual2;
    unsigned short BC0 = 0;  //this value should be equal for the same event in all slots...
    ReadRegister(seq_id_adr);
    unsigned short csc_id  = (unsigned short) GetCscId();
    //
    if ( lct0 == 0 ) {  // random LCT
      //
      qual1 = 0;
      while (qual1 < 1)                // ensure that one random muon has quality > 0
	qual1= rand()%16;              
      unsigned short clct   = rand()%16; 
      unsigned short wire   = rand()%128;
      unsigned short bxn0   = rand()%2;
      unsigned short lr     = rand()%2;
      unsigned short halfSt = rand()%256;
      //
      frame1 = 
	((vpf   &  0x1) << 15) + 
	((qual1 &  0xf) << 11) + 
	((clct  &  0xf) <<  7) + 
	((wire  & 0x7f) <<  0) ;
      //
      frame2 = 
	((csc_id   &  0xf) << 12) +
	((BC0      &  0x1) << 11) +
	((bxn0     &  0x1) << 10) +
	((sync_err &  0x1) <<  9) +
	((lr       &  0x1) <<  8) + 
	((halfSt   & 0xff) <<  0);    
      //
    } else {
      // insert the csc_id specific for this TMB (otherwise the user has to specify...)
      frame2 = ( ((lct0>> 0) & 0x0fff) | (csc_id & 0xf) << 12 ) ;
      frame1 = (lct0>>16) & 0xffff;
    }
    //
    lct0_ = ((frame1&0xffff)<<16) | (frame2&0xffff) ;
    //
    InjectedLct0.push_back(lct0_);
    //
    if (debug_) printf("TMB lct0 = %x %x %x\n",frame1,frame2,(unsigned int)lct0_);
    //
    sndbuf[0] = (frame1>>8)&0xff ;
    sndbuf[1] = (frame1)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_wdata_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = ((ramAdd+1)>>8)&0xff ; // Assert write enable
    sndbuf[1] = ((ramAdd+1))&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW );
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (frame2>>8)&0xff ;
    sndbuf[1] = (frame2)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_wdata_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = ((ramAdd+2)>>8)&0xff ;  // Assert write enable
    sndbuf[1] = (ramAdd+2)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW );
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    if ( lct1 == 0 ) {    //random LCT
      //
      qual2 = 15;
      while (qual2 >= qual1)           // ensure that quality for LCT1 is always less than quality for LCT0
	qual2 = rand()%16;            
      unsigned short clct   = rand()%16; 
      unsigned short wire   = rand()%128;
      unsigned short bxn0   = rand()%2;
      unsigned short lr     = rand()%2;
      unsigned short halfSt = rand()%256;
      //
      frame1 = 
	((vpf   &  0x1) << 15) + 
	((qual2 &  0xf) << 11) + 
	((clct  &  0xf) <<  7) + 
	((wire  & 0x7f) <<  0) ;
      //
      frame2 = 
	((csc_id   &  0xf) << 12) +
	((BC0      &  0x1) << 11) +
	((bxn0     &  0x1) << 10) +
	((sync_err &  0x1) <<  9) +
	((lr       &  0x1) <<  8) + 
	((halfSt   & 0xff) <<  0);    
      //

    } else {
      // insert the csc_id specific for this TMB (otherwise the user has to specify...)
      frame2 = ( ((lct1>> 0) & 0x0fff) | (csc_id & 0xf) << 12 ) ;
      frame1 = (lct1 >> 16) & 0xffff;
    }
    //
    lct1_ = ((frame1&0xffff)<<16) | (frame2&0xffff) ;
    //
    InjectedLct1.push_back(lct1_);
    //
    if (debug_) printf("TMB lct1 = %x %x %x\n",frame1,frame2,(unsigned int)lct1_);
    //
    sndbuf[0] = (frame1>>8)&0xff ;
    sndbuf[1] = (frame1)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_wdata_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = ((ramAdd+4)>>8)&0xff ;
    sndbuf[1] = ((ramAdd+4))&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW );        // Assert write enable
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (frame2>>8)&0xff ;
    sndbuf[1] = (frame2)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_wdata_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = (ramAdd>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    sndbuf[0] = ((ramAdd+8)>>8)&0xff ;
    sndbuf[1] = (ramAdd+8)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW );      // Assert write enable
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
  }
  //
  // Read back RAM address
  //
  //  usleep(100);
  //  ReadBackMpcRAM(nEvents);
  usleep(100);
  //
  // Now fire MPC injector
  //
  //FireMPCInjector(nEvents);
  //
}
//
void TMB::ReadBackMpcRAM(int nEvents){
  //
  unsigned short ramAdd;
  //
  (*MyOutput_) << "TMB:  Read MPC injector RAM for " << std::dec << nEvents << " events" << std::endl ;
  //
  for (int evtId(0); evtId<nEvents; ++evtId) {
    //
    (*MyOutput_) << "Event " << std::dec << evtId << "..." << std::endl;
    //
    ramAdd = (evtId<<8);
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff | (0x1<<4) ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    tmb_vme(VME_READ,mpc_ram_rdata_adr,sndbuf,rcvbuf,NOW);
    unsigned long int rlct01 = ((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff) ;
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff | (0x1<<5) ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    tmb_vme(VME_READ,mpc_ram_rdata_adr,sndbuf,rcvbuf,NOW);
    unsigned long int rlct02 = ((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff) ;
    //
    unsigned long int rlct0 = ( ((rlct01 & 0xffff) << 16) | ((rlct02 & 0xffff) << 0) );
    (*MyOutput_) << "LCT0 = " << std::hex << rlct0 << std::endl;
    //
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff | (0x1<<6) ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    tmb_vme(VME_READ,mpc_ram_rdata_adr,sndbuf,rcvbuf,NOW);
    unsigned long int rlct11 = ((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff) ;
    //
    sndbuf[0] = ((ramAdd)>>8)&0xff ;
    sndbuf[1] = (ramAdd)&0xff | (0x1<<7) ;
    tmb_vme(VME_WRITE,mpc_ram_adr,sndbuf,rcvbuf,NOW);
    //
    tmb_vme(VME_READ,mpc_ram_rdata_adr,sndbuf,rcvbuf,NOW);
    unsigned long int rlct12 = ((rcvbuf[0]&0xff)<<8) | ((rcvbuf[1]&0xff)) ;
    //
    unsigned long int rlct1 = ( ((rlct11 & 0xffff) << 16) | ((rlct12 & 0xffff) << 0) );
    (*MyOutput_) << "LCT1 = " << std::hex << rlct1 << std::endl;
    //
  }
  //
}
//
void TMB::FireMPCInjector(int nEvents){
  //
  tmb_vme(VME_READ,mpc_inj_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "TMB: Fire MPC injector" << std::endl;
  //
  sndbuf[0] = rcvbuf[0] & 0xfe ; // Unfire injector
  sndbuf[1] = nEvents & 0xff;
  tmb_vme(VME_WRITE,mpc_inj_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = rcvbuf[0] & 0xfe | 0x1 ; // Fire injector
  sndbuf[1] = nEvents & 0xff;
  tmb_vme(VME_WRITE,mpc_inj_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = rcvbuf[0] & 0xfe ; // UnFire injector
  sndbuf[1] = nEvents & 0xff;
  tmb_vme(VME_WRITE,mpc_inj_adr,sndbuf,rcvbuf,NOW);
  //
  return;
}
//
void TMB::DataSendMPC(){
  //
  (*MyOutput_) << "TMB: data sent to MPC..." << std::endl;
  //
  int mpc0frame0 = ReadRegister(mpc0_frame0_adr);
  (*MyOutput_) << "LCT0 FRAME0 " << std::hex << mpc0frame0 << std::endl ; 
  //
  int mpc0frame1 = ReadRegister(mpc0_frame1_adr);
  (*MyOutput_) << "LCT0 FRAME1 " << std::hex << mpc0frame1 << std::endl ; 
  //
  int mpc1frame0 = ReadRegister(mpc1_frame0_adr);
  (*MyOutput_) << "LCT1 FRAME0 " << std::hex << mpc1frame0 << std::endl ; 
  //
  int mpc1frame1 = ReadRegister(mpc1_frame1_adr);
  (*MyOutput_) << "LCT1 FRAME1 " << std::hex << mpc1frame1 << std::endl ; 
  //
  return;
}
//
void TMB::DecodeALCT(){
  //
  tmb_vme(VME_READ,alct_alct0_adr,sndbuf,rcvbuf,NOW);
  //
  alct0_data_ = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
  alct0_valid_     = ((alct0_data_ >>  0) &  0x1);
  alct0_quality_   = ((alct0_data_ >>  1) &  0x3);
  alct0_amu_       = ((alct0_data_ >>  3) &  0x1);
  alct0_first_key_ = ((alct0_data_ >>  4) & 0x7f);
  alct0_first_bxn_ = ((alct0_data_ >> 11) &  0x3);
  //
  tmb_vme(VME_READ,alct_alct1_adr,sndbuf,rcvbuf,NOW);
  //
  alct1_data_ = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
  alct1_valid_      = ((alct1_data_ >>  0) &  0x1);
  alct1_quality_    = ((alct1_data_ >>  1) &  0x3);
  alct1_amu_        = ((alct1_data_ >>  3) &  0x1);
  alct1_second_key_ = ((alct1_data_ >>  4) & 0x7f);
  alct1_second_bxn_ = ((alct1_data_ >> 11) &  0x3);
  //
  //  PrintALCT();
  //
  return;
}
//
void TMB::PrintALCT() {
  //
  (*MyOutput_) << "----------------------"                                << std::endl;
  (*MyOutput_) << " ALCT0.data  = 0x"     << std::hex << alct0_data_      << std::endl;
  (*MyOutput_) << "----------------------"                                << std::endl;
  (*MyOutput_) << " ALCT0.valid     = 0x" << std::hex << alct0_valid_     << std::endl;
  (*MyOutput_) << " ALCT0.quality   = "   << std::dec << alct0_quality_   << std::endl;
  (*MyOutput_) << " ALCT0.amu       = 0x" << std::hex << alct0_amu_       << std::endl;
  (*MyOutput_) << " ALCT0.key WG    = "   << std::dec << alct0_first_key_ << std::endl;
  (*MyOutput_) << " ALCT0.bxn       = 0x" << std::hex << alct0_first_bxn_ << std::endl;
  //
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "----------------------"                                << std::endl;
  (*MyOutput_) << " ALCT1.data  = 0x"     << std::hex << alct1_data_      << std::endl;
  (*MyOutput_) << "----------------------"                                << std::endl;
  (*MyOutput_) << " ALCT1.valid     = 0x" << std::hex << alct1_valid_     << std::endl;
  (*MyOutput_) << " ALCT1.quality   = "   << std::dec << alct1_quality_   << std::endl;
  (*MyOutput_) << " ALCT1.amu       = 0x" << std::hex << alct1_amu_       << std::endl;
  (*MyOutput_) << " ALCT1.key WG    = "   << std::dec << alct1_second_key_ << std::endl;
  (*MyOutput_) << " ALCT1.bxn       = 0x" << std::hex << alct1_second_bxn_ << std::endl;
  //
  return;
}
//
void TMB::DecodeCLCT(){
   //
   tmb_vme(VME_READ,seq_clctm_adr,sndbuf,rcvbuf2,NOW);
   tmb_vme(VME_READ,seq_clct0_adr,sndbuf,rcvbuf,NOW);
   //
   CLCT0_data_ = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff | (rcvbuf2[1]&0x1f)<<16 ) ;
   CLCT0_valid_        = ((CLCT0_data_>> 0) &  0x1);
   CLCT0_nhit_         = ((CLCT0_data_>> 1) &  0x7);
   CLCT0_pattern_      = ((CLCT0_data_>> 4) &  0xf);
   CLCT0_bend_         = ((CLCT0_data_>> 8) &  0x1);
   CLCT0_keyHalfStrip_ = ((CLCT0_data_>> 9) & 0x1f);
   CLCT0_cfeb_         = ((CLCT0_data_>>14) &  0x7);
   CLCT0_BXN_          = ((CLCT0_data_>>17) &  0x3);
   CLCT0_sync_err_     = ((CLCT0_data_>>19) &  0x1);
   CLCT0_bx0_local_    = ((CLCT0_data_>>20) &  0x1);
   //
   tmb_vme(VME_READ,seq_clct1_adr,sndbuf,rcvbuf,NOW);
   CLCT1_data_ = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff ) | ((rcvbuf2[1]>>5)&0x7)<<16 | (rcvbuf2[0]&0x3)<<19 ;
   //
   CLCT1_valid_        = ((CLCT1_data_>> 0) &  0x1);
   CLCT1_nhit_         = ((CLCT1_data_>> 1) &  0x7);
   CLCT1_pattern_      = ((CLCT1_data_>> 4) &  0xf);
   CLCT1_bend_         = ((CLCT1_data_>> 8) &  0x1);
   CLCT1_keyHalfStrip_ = ((CLCT1_data_>> 9) & 0x1f);
   CLCT1_cfeb_         = ((CLCT1_data_>>14) &  0x7);
   CLCT1_BXN_          = ((CLCT1_data_>>17) &  0x3);
   CLCT1_sync_err_     = ((CLCT1_data_>>19) &  0x1);
   CLCT1_bx0_local_    = ((CLCT1_data_>>20) &  0x1);
   //
   //   PrintCLCT();
   //
   return;
}
//
void TMB::PrintCLCT() {
  //
  std::cout << "CLCT0 data = 0x"       << std::hex << CLCT0_data_          << std::endl;
  std::cout << "CLCT1 data = 0x"       << std::hex << CLCT1_data_          << std::endl;
  //
  (*MyOutput_) << "----------------------"                                   << std::endl;
  (*MyOutput_) << "CLCT0 data = 0x"       << std::hex << CLCT0_data_          << std::endl;
  (*MyOutput_) << "----------------------"                                   << std::endl;
  (*MyOutput_) << "CLCT0.Valid      = 0x" << std::hex << CLCT0_valid_        << std::endl;
  (*MyOutput_) << "CLCT0.Nhits      = 0x" << std::hex << CLCT0_nhit_         << std::endl;
  (*MyOutput_) << "CLCT0.pattern    = 0x" << std::hex << CLCT0_pattern_      << std::endl;
  (*MyOutput_) << "CLCT0.bend       = 0x" << std::hex << CLCT0_bend_         << std::endl;
  (*MyOutput_) << "CLCT0.Key HStrip = "   << std::dec << CLCT0_keyHalfStrip_ << std::endl;
  (*MyOutput_) << "CLCT0.Key CFEB   = "   << std::dec << CLCT0_cfeb_         << std::endl;
  (*MyOutput_) << "CLCT0.BXN        = 0x" << std::hex << CLCT0_BXN_          << std::endl;
  (*MyOutput_) << "CLCT0.sync err   = 0x" << std::hex << CLCT0_sync_err_     << std::endl;
  (*MyOutput_) << "CLCT0.BX0 local  = 0x" << std::hex << CLCT0_bx0_local_    << std::endl;
  //
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "----------------------"                                   << std::endl;
  (*MyOutput_) << "CLCT1 data = 0x"       << std::hex << CLCT1_data_         << std::endl;
  (*MyOutput_) << "----------------------"                                   << std::endl;
  (*MyOutput_) << "CLCT1.Valid      = 0x" << std::hex << CLCT1_valid_        << std::endl;
  (*MyOutput_) << "CLCT1.Nhits      = 0x" << std::hex << CLCT1_nhit_         << std::endl;
  (*MyOutput_) << "CLCT1.pattern    = 0x" << std::hex << CLCT1_pattern_      << std::endl;
  (*MyOutput_) << "CLCT1.bend       = 0x" << std::hex << CLCT1_bend_         << std::endl;
  (*MyOutput_) << "CLCT1.Key HStrip = "   << std::dec << CLCT1_keyHalfStrip_ << std::endl;
  (*MyOutput_) << "CLCT1.Key CFEB   = "   << std::dec << CLCT1_cfeb_         << std::endl;
  (*MyOutput_) << "CLCT1.BXN        = 0x" << std::hex << CLCT1_BXN_          << std::endl;
  (*MyOutput_) << "CLCT1.sync err   = 0x" << std::hex << CLCT1_sync_err_     << std::endl;
  (*MyOutput_) << "CLCT1.BX0 local  = 0x" << std::hex << CLCT1_bx0_local_    << std::endl;
  //
  return;
}
//
int TMB::FmState(){
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  int fm_state = (rcvbuf[1]&0xf0)>>4;
  //
  return fm_state;
  //
}
//
void TMB::PrintCounters(int counter){
  //
  // if (counter < 0) { print all counters }
  //
  if (counter<0)                  (*MyOutput_) << "--------------------------------------------------------" << std::endl;
  if (counter<0)                  (*MyOutput_) << "---              Counters                             --" << std::endl;
  if (counter<0)                  (*MyOutput_) << "--------------------------------------------------------" << std::endl;
  if (counter<0) {
    for (int i=0; i <= (MaxCounter-1)/2; i++) 
      (*MyOutput_) << CounterName(i)  << FinalCounter[i] <<std::endl ;
  } else {
    (*MyOutput_) << CounterName(counter)  << FinalCounter[counter] <<std::endl ;
  }
  //
}
//
std::string TMB::CounterName(int counter){
  //
  std::string name="Not defined";;
  if( counter == 0 ) name = "ALCT: CRC error                                         ";
  if( counter == 1 ) name = "ALCT: LCT sent to TMB                                   ";
  if( counter == 2 ) name = "ALCT: LCT received data error                           ";
  if( counter == 3 ) name = "ALCT: L1A readout                                       ";
  if( counter == 4 ) name = "CLCT: Pretrigger                                        ";
  if( counter == 5 ) name = "CLCT: Pretrig but no wbuf available                     ";
  if( counter == 6 ) name = "CLCT: Invalid pattern after drift                       ";
  if( counter == 7 ) name = "CLCT: TMB matching rejected event                       ";
  if( counter == 8 ) name = "TMB:  CLCT,ALCT,or both trigger                         ";
  if( counter == 9 ) name = "TMB:  CLCT,ALCT,or both trigger, trig allowed, xmit MPC ";
  if( counter == 10) name = "TMB:  CLCT and ALCT matched in time                     ";
  if( counter == 11) name = "TMB:  ALCT-only trigger                                 ";
  if( counter == 12) name = "TMB:  CLCT-only trigger                                 ";
  if( counter == 13) name = "TMB:  No trig pulse response (TMB internal logic check) ";
  if( counter == 14) name = "TMB:  No MPC transmission (TMB internal logic check)    ";
  if( counter == 15) name = "TMB:  No MPC response FF pulse (TMB internal logic ck)  ";
  if( counter == 16) name = "TMB:  MPC accepted LCT0                                 ";
  if( counter == 17) name = "TMB:  MPC accepted LCT1                                 ";
  if( counter == 18) name = "L1A:  L1A received                                      ";
  if( counter == 19) name = "L1A:  TMB triggered, TMB in L1A window                  ";
  if( counter == 20) name = "L1A:  L1A received, no TMB in window                    ";
  if( counter == 21) name = "L1A:  TMB triggered, no L1A received                    ";
  if( counter == 22) name = "L1A:  TMB readout                                       ";
  if( counter == 23) name = "CLCT: Triad skipped                                     ";
  if( counter == 24) name = "TMB:  Raw Hits Buffer Reset due to overflow             ";
  if( counter == 25) name = "TMB:  No ALCT in trigger                                ";
  if( counter == 26) name = "TMB:  One ALCT in trigger                               ";
  if( counter == 27) name = "TMB:  One CLCT in trigger                               ";
  if( counter == 28) name = "TMB:  Two ALCTs in trigger                              ";
  if( counter == 29) name = "TMB:  Two CLCTs in trigger                              ";
  if( counter == 30) name = "TMB:  ALCT0 copied to ALCT1 to make 2nd LCT             ";
  if( counter == 31) name = "TMB:  CLCT0 copied to CLCT1 to make 2nd LCT             ";
  if( counter == 32) name = "TMB:  LCT1 has higher quality than LCT0 (ranking error) ";
  //
  // The following are not accessible via VME
  //  if( counter == 33) name = "HDR:  Pretrigger counter                                ";
  //  if( counter == 34) name = "HDR:  CLCT counter                                      ";
  //  if( counter == 35) name = "HDR:  TMB trigger counter                               ";
  //  if( counter == 36) name = "HDR:  ALCTs received counter                            ";
  //  if( counter == 37) name = "HDR:  L1As received counter (12 bits)                   ";
  //  if( counter == 38) name = "HDR:  Readout counter (12 bits)                         ";
  //  if( counter == 39) name = "HDR:  BC0 counter                                       ";
  //
  return name;
}
//
void TMB::ResetCounters(){
  //
  for(int i=0;i<MaxCounter;i++) FinalCounter[i]=0;

  // Clear counters
  //
  WriteRegister(cnt_ctrl_adr,0x1);
  WriteRegister(cnt_ctrl_adr,0x0);
  //
  return;
}
//
int TMB::GetCounter(int counterID){
  //
  return FinalCounter[counterID];
}
//
int * TMB::GetCounters(){
  //
  // Take snapshot of current counter state
  //
  WriteRegister(cnt_ctrl_adr,0x2); //snap
  WriteRegister(cnt_ctrl_adr,0x0); //unsnap
  //
  // Extract counter data whose picture has been taken
  //
  for (int counter=0; counter <= MaxCounter; counter++){
    //
    int counter_address = counter << 8 ;
    WriteRegister(cnt_ctrl_adr,counter_address);
    //
    int rd_data = ReadRegister(cnt_rdata_adr);
    //
    // Combine lsbs+msbs
    //
    int cnt_lsb, cnt_msb;
    long int cnt_full;
    //
    if( counter%2 == 0 ) {          //even addresses contain counter LSBs
      cnt_lsb = rd_data;
      // (*MyOutput_) << "counter " << counter << ", LSB = " << cnt_lsb ;
    } else {	                     //odd addresses contain counter MSBs
      cnt_msb  = rd_data;
      // (*MyOutput_) << ", MSB = " << cnt_msb << std::endl;
      cnt_full = cnt_lsb | (cnt_msb<<16) ;
      FinalCounter[counter/2] = cnt_full ;     //assembled counter MSB,LSB	 
    }
  }   
  //
  return (int *)FinalCounter;
}
//
int * TMB::NewCounters(){
  //
  // Take snapshot of current counter state
  //
  write_later(cnt_ctrl_adr,0x2); //snap
  vme_delay(0x20);
  write_later(cnt_ctrl_adr,0x0); //unsnap
  //
  // Extract counter data whose picture has been taken
  //
  for (int counter=0; counter <= MaxCounter; counter++){
    //
    int counter_address = counter << 8 ;
    write_later(cnt_ctrl_adr,counter_address);
    //
    read_later(cnt_rdata_adr);
  }   
  // time since last hard_reset (in seconds)
  read_now(0xE8, (char *)FinalCounter);
  //
  return (int *)FinalCounter;
}

void TMB::old_clk_delays(unsigned short int time,int cfeb_id)
{
 // for old PHOS4 chips
int iloop;
 iloop=0;
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,NOW);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,NOW);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,NOW);
  }
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,NOW);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,NOW);
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  // send delay to dynatem
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,NOW);
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);


LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
}

// JMT uncomment to see scope output on screen as well as file
//#define fprintf(fp, fmt, s...) { fprintf(fp, fmt, ## s); printf(fmt, ## s); }
  
void TMB::scope(int scp_arm,int scp_readout, int scp_channel) {

  unsigned int nchannels = 128;
  unsigned int nrams     = nchannels/16;
  unsigned int nbits     = nchannels*16;

  static const char* scope_tag[128] = {"ch00 sequencer pretrig    ",
				       "ch01 active_feb_flag      ",
				       "ch02 any_cfeb_hit         ",
				       "ch03 any_cfeb_hsds        ",
				       "ch04 wr_buf_busy          ",
				       "ch05 wr_buf_ready         ",
				       "ch06 clct_ext_trig_os     ",
				       "ch07 alct_active_feb      ",
				       "ch08 alct_pretrig_win     ",
				       "ch09 first_really_valid   ",
				       "ch10 clct_sm==xtmb        ",
				       "ch11 tmb_discard          ",
				       "ch12 discard_nobuf        ",
				       "ch13 discard_invp         ",
				       "ch14 discard_tmbreject    ",
				       "ch15 0(no dmb readout)    ",
				       "ch16 first_nhit[0]        ",
				       "ch17 first_nhit[1]        ",
				       "ch18 first_nhit[2]        ",
				       "ch19 first_pat[3]hsds     ",
				       "ch20 second_nhit[0]       ",
				       "ch21 second_nhit[1]       ",
				       "ch22 second_nhit[2]       ",
				       "ch23 second_pat[3]hsds    ",
				       "ch24 latch_clct0          ",
				       "ch25 latch_clct1          ",
				       "ch26 alct_1st_valid       ",
				       "ch27 alct_2nd_valid       ",
				       "ch28 alct_vpf_tp          ",
				       "ch29 clct_vpf_tp          ",
				       "ch30 clct_window_tp       ",
				       "ch31 0(no dmb readout)    ",
				       "ch32 sequencer pretrig    ",
				       "ch33 mpc_frame_ff         ",
				       "ch34 mpc_response_ff      ",
				       "ch35 mpc_accept_tp[0]     ",
				       "ch36 mpc_accept_tp[1]     ",
				       "ch37 l1a_pulse_dsp        ",
				       "ch38 l1a_window_dsp       ",
				       "ch39 dmb_dav_mux          ",
				       "ch40 dmb_busy             ",
				       "ch41 hs_thresh[0]         ",
				       "ch42 hs_thresh[1]         ",
				       "ch43 hs_thresh[2]         ",
				       "ch44 ds_thresh[0]         ",
				       "ch45 ds_thresh[1]         ",
				       "ch46 ds_thresh[2]         ",
				       "ch47 0(no dmb readout)    ",
				       "ch48 sequencer pretrig    ",
				       "ch49 valid_clct_required  ",
				       "ch50 buf_nbusy[0]         ",
				       "ch51 buf_nbusy[1]         ",
				       "ch52 buf_nbusy[2]         ",
				       "ch53 buf_nbusy[3]         ",
				       "ch54 0                    ",
				       "ch55 0                    ",
				       "ch56 0                    ",
				       "ch57 0                    ",
				       "ch58 0                    ",
				       "ch59 l1a_rx_counter[0]    ",
				       "ch60 l1a_rx_counter[1]    ",
				       "ch61 l1a_rx_counter[2]    ",
				       "ch62 l1a_rx_counter[3]    ",
				       "ch63 0(no dmb readout)    ",
				       "ch64 sequencer pretrig    ",
				       "ch65 bxn_counter[ 0]      ",
				       "ch66 bxn_counter[ 1]      ",
				       "ch67 bxn_counter[ 2]      ",
				       "ch68 bxn_counter[ 3]      ",
				       "ch69 bxn_counter[ 4]      ",
				       "ch70 bxn_counter[ 5]      ",
				       "ch71 bxn_counter[ 6]      ",
				       "ch72 bxn_counter[ 7]      ",
				       "ch73 bxn_counter[ 8]      ",
				       "ch74 bxn_counter[ 9]      ",
				       "ch75 bxn_counter[10]      ",
				       "ch76 bxn_counter[11]      ",
				       "ch77 0                    ",
				       "ch78 0                    ",
				       "ch79 0(no dmb readout)    ",
				       "ch80 dmb seq_wdata[0]     ",
				       "ch81 dmb seq_wdata[1]     ",
				       "ch82 dmb seq_wdata[2]     ",
				       "ch83 dmb seq_wdata[3]     ",
				       "ch84 dmb seq_wdata[4]     ",
				       "ch85 dmb seq_wdata[5]     ",
				       "ch86 dmb seq_wdata[6]     ",
				       "ch87 dmb seq_wdata[7]     ",
				       "ch88 dmb seq_wdata[8]     ",
				       "ch89 dmb seq_wdata[9]     ",
				       "ch90 dmb seq_wdata[10]    ",
				       "ch91 dmb seq_wdata[11]    ",
				       "ch92 dmb seq_wdata[12]    ",
				       "ch93 dmb seq_wdata[13]    ",
				       "ch94 dmb seq_wdata[14]    ",
				       "ch95 dmb seq_wdata[15]    ",
				       "ch96  rpc0_bxn[0]         ",
				       "ch97  rpc0_bxn[1]         ",
				       "ch98  rpc0_bxn[2]         ",
				       "ch99  rpc1_bxn[0]         ",
				       "ch100 rpc1_bxn[1]         ",
				       "ch101 rpc1_bxn[2]         ",
				       "ch102 rpc2bxn[0]          ",
				       "ch103 rpc2bxn[1]          ",
				       "ch104 rpc2bxn[2]          ",
				       "ch105 rpc3bxn[0]          ",
				       "ch106 rpc3bxn[1]          ",
				       "ch107 rpc3bxn[2]          ",
				       "ch108 0                   ",
				       "ch109 0                   ",
				       "ch110 0                   ",
				       "ch111 0(no dmb readout)   ",
				       "ch112 rpc0_nhits[0]       ",
				       "ch113 rpc0_nhits[1]       ",
				       "ch114 rpc0_nhits[2]       ",
				       "ch115 rpc0_nhits[3]       ",
				       "ch116 rpc1_nhits[0]       ",
				       "ch117 rpc1_nhits[1]       ",
				       "ch118 rpc1_nhits[2]       ",
				       "ch119 rpc1_nhits[3]       ",
				       "ch120 rpc2_nhits[0]       ",
				       "ch121 rpc2_nhits[1]       ",
				       "ch122 rpc2_nhits[2]       ",
				       "ch123 rpc2_nhits[3]       ",
				       "ch124 rpc3_nhits[0]       ",
				       "ch125 rpc3_nhits[1]       ",
				       "ch126 rpc3_nhits[2]       ",
				       "ch127 rpc3_nhits[3](no d) "
  } ;

  unsigned long int scope_ram[256][nrams];
  int scp_raw_data[nbits];
  char *scope_ch[256];
  int scope_raw_decode = 1;

  unsigned long int runstop;
  //unsigned long int force_trig;
  //unsigned long int ch_source;
  unsigned long int ram_sel;
  unsigned long int Auto;
  //unsigned long int nowrite;
  //unsigned long int ready;
  //unsigned long int trig_done;

  unsigned long int adr;
  unsigned long int rd_data = 0;
  unsigned long int wr_data;
  //unsigned long int status;
  unsigned long int i;
  unsigned long int itbin;
  unsigned long int ich;
  unsigned long int ibit;
  unsigned long int iram;
  //unsigned long int idigit;
  unsigned long int iline;
  unsigned long int iadr;

  unsigned long int first_nhit[256];
  unsigned long int second_nhit[256];
  unsigned long int hs_thresh[256] ;
  unsigned long int ds_thresh[256] ;
  unsigned long int bxn[256] ;
  unsigned long int dmb[256] ;
  unsigned long int l1a_rx_cnt[256] ;
  unsigned long int buf_nbusy[256] ;

  unsigned long int	rpc0_bxn[256] ;
  unsigned long int	rpc1_bxn[256] ;
  unsigned long int	rpc2_bxn[256] ;
  unsigned long int	rpc3_bxn[256] ;
  unsigned long int	rpc0_nhits[256] ;
  unsigned long int	rpc1_nhits[256] ;
  unsigned long int	rpc2_nhits[256] ;
  unsigned long int	rpc3_nhits[256] ;


  rd_data = 0; //temp

  /**** scp_arm ****************************************************/
  if(scp_arm) {

    //Get current scope control state, exit if in Auto mode
    adr = scp_ctrl_adr;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    Auto = ((rcvbuf[0]&0xff) << 8) & (rcvbuf[1]&0xff);
    Auto = (Auto >> 5) & 0x1;
    if(Auto == 1) return;

    //Reset scope state to idle
    runstop = 0;
    ram_sel = 0;

    adr = scp_ctrl_adr ;
    wr_data = (Auto << 5) | (ram_sel << 2) | runstop;
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    //status = tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    //printf("Scope status %04x",rd_data);

    //Select channel
    sndbuf[0] = 0x00;
    sndbuf[1] = scp_channel&0xff;
    tmb_vme(VME_WRITE,0xce,sndbuf,rcvbuf,NOW);

    //Arm scope for triggering, send scope state to wait_trig
    runstop = 1;
    adr = scp_ctrl_adr ;
    wr_data = (Auto << 5) | (ram_sel << 2) | runstop;
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    return;
  }

  /**** scp_readout *******************************************************/

  //Exit if in Auto mode
  if(Auto == 1) return;

  //Wait for scope to trigger
  if(scp_readout) {
     for(i=1;i<100;i++) {                              //Give it time to store 255 words
      adr = scp_ctrl_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);              //read scope status
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      printf("Scope status %04x\n",(unsigned int)rd_data);
      if((rd_data & 0x0080) != 0) goto TRIGGERED;                    //triggered and done
      printf("Waiting for scope to trigger %ld\n",i);
     }
     (*MyOutput_) << "Scope never triggered" << std::endl;
     //
     SendOutput("Scope never triggered");
     //
     goto END;
     
     //Read back embedded scope data
  TRIGGERED:
      printf("Scope triggered\n");

	for(itbin=0;itbin<256;itbin++) {                    //loop over ram addresses
	  for(iram=0;iram<nrams;iram++) {
	    adr = scp_ctrl_adr ;
	    wr_data = 0x0000 | (itbin << 8) | (iram << 2) | (Auto << 5) ;             //write scope ram address
	    sndbuf[0] = (wr_data & 0xff00)>>8 ;
	    sndbuf[1] = wr_data & 0x00ff ;
	    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	    
	    adr = scp_rdata_adr ;
	    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);              //read scope data at this address
	    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	    scope_ram[itbin][iram] = rd_data;                  //store 16 parallel in local array
	    //printf(" 1 %3d %1d %4x %4x \n", itbin,iram,scope_ram[itbin][iram] ) ;
	  }
	}
	goto DISPLAY;
	
	//for(itbin=0;itbin<256;itbin++) {               //loop over ram addresses
	//for(iram=0;iram<4;iram++) {
	//  printf(" 2 %3d %1d %4x %4x \n", itbin,iram,scope_ram[itbin][iram] ) ;
	//}
	//}

  }

  /**** scp_raw_decode *************************************************/

  //Exit if not Auto mode
  if(Auto == 1) return;

  //load scope_ram from raw-hits format readout
  if(scope_raw_decode) {
    iline = 0;
    for(iram=0;iram<nrams;iram++){
      for(iadr=0;iadr<256;iadr++) {
	itbin = ram_sel*256 + iadr;
	scope_ram[itbin][iram] = scp_raw_data[iline];
	iline++;
      }
    }
  }
  goto DISPLAY;


  /**** Display *********************************************************/

  //Construct waveform
 DISPLAY:
  //
  (*MyOutput_) << "Displaying " << std::endl;
  //
  //if (!pfile) pfile = fopen("tmb_scope.txt","w");
  //
  for(ich=0;ich<128;ich++) {                      //loop over 128 scope channels
    //
    iram=ich/16;
    (*MyOutput_) << scope_tag[ich] ;
    //
    for(itbin=0;itbin<256;itbin++) {                      //256 time bins per channel
      //
      ibit = ((scope_ram[itbin][iram]) >> (ich%16) ) & 1; //logic levels vs tbin for this chan	      
      if(ibit == 0) scope_ch[itbin] = "_";       //display symbol for logic 0
      if(ibit == 1) scope_ch[itbin] = "-";       //display symbol for logic 1
      (*MyOutput_) << scope_ch[itbin];
      //
      // Construct integer for special channel groups
      //
      if (ich == 16) first_nhit[itbin]=ibit ;
      if (ich == 17) first_nhit[itbin]=first_nhit[itbin] | (ibit<<1);
      if (ich == 18) first_nhit[itbin]=first_nhit[itbin] | (ibit<<2);
      //
      if (ich == 20) second_nhit[itbin]=ibit;
      if (ich == 21) second_nhit[itbin]=second_nhit[itbin] | (ibit<<1);
      if (ich == 22) second_nhit[itbin]=second_nhit[itbin] | (ibit<<2);
      //
      if (ich == 41) hs_thresh[itbin]=ibit ;
      if (ich == 42) hs_thresh[itbin]=hs_thresh[itbin] | (ibit<<1) ;
      if (ich == 43) hs_thresh[itbin]=hs_thresh[itbin] | (ibit<<2) ;
      //
      if (ich == 44) ds_thresh[itbin]=ibit;
      if (ich == 45) ds_thresh[itbin]=ds_thresh[itbin] | (ibit<<1);
      if (ich == 46) ds_thresh[itbin]=ds_thresh[itbin] | (ibit<<2);
      //
      if (ich == 50) buf_nbusy[itbin]=ibit ;
      if (ich == 51) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<1) ;
      if (ich == 52) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<2) ;
      if (ich == 53) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<3) ;
      //									  //
      if (ich == 59) l1a_rx_cnt[itbin]=ibit;
      if (ich == 60) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<1);
      if (ich == 61) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<2);
      if (ich == 62) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<3);
      //
      if (ich == 65) bxn[itbin]=ibit ;
      if (ich >= 66 && ich <= 76) bxn[itbin]=bxn[itbin] | (ibit<<(ich-65)) ;
      // 
      if (ich == 80) dmb[itbin] = ibit ;
      if (ich >= 81 && ich <= 95) dmb[itbin]=dmb[itbin] | (ibit<<(ich-80)) ;
      //
      if (ich == 96) rpc0_bxn[itbin]=ibit ;
      if (ich == 97) rpc0_bxn[itbin]=rpc0_bxn[itbin] | (ibit << 1);
      if (ich == 98) rpc0_bxn[itbin]=rpc0_bxn[itbin] | (ibit << 2);
      //
      if (ich == 99 )rpc1_bxn[itbin]=ibit;
      if (ich == 100)rpc1_bxn[itbin]=rpc1_bxn[itbin] | (ibit << 1);
      if (ich == 101)rpc1_bxn[itbin]=rpc1_bxn[itbin] | (ibit << 2);
      //
      if (ich == 102)rpc2_bxn[itbin]=ibit;
      if (ich == 103)rpc2_bxn[itbin]=rpc2_bxn[itbin] | (ibit << 1);
      if (ich == 104)rpc2_bxn[itbin]=rpc2_bxn[itbin] | (ibit << 2);
      //
      if (ich == 105)rpc3_bxn[itbin]=ibit;
      if (ich == 106)rpc3_bxn[itbin]=rpc3_bxn[itbin] | (ibit << 1);
      if (ich == 107)rpc3_bxn[itbin]=rpc3_bxn[itbin] | (ibit << 2);
	//
      if(ich == 112)rpc0_nhits[itbin]=ibit;
      if(ich == 113)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 1);
      if(ich == 114)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 2);
      if(ich == 115)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 3);
      //
      if(ich == 116)rpc1_nhits[itbin]=ibit;
      if(ich == 117)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 1);
      if(ich == 118)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 2);
      if(ich == 119)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 3);
      //
      if(ich == 120)rpc2_nhits[itbin]=ibit;
      if(ich == 121)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 1);
      if(ich == 122)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 2);
      if(ich == 123)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 3);
      //
      if(ich == 124)rpc3_nhits[itbin]=ibit;
      if(ich == 125)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 1);
      if(ich == 126)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 2);
      if(ich == 127)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 3);
      //
    }  //close itbin
    //
    if (ich == 18) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << first_nhit[itbin] ;
      }
    }
    if (ich == 22) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << second_nhit[itbin] ;
      }
    }
    if (ich == 43) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << hs_thresh[itbin] ;
      }
    }
    if (ich == 46) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ds_thresh[itbin] ;
      }
    }
    if (ich == 53) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << buf_nbusy[itbin] ;
      }
    }
    if (ich == 62) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((l1a_rx_cnt[itbin]>>8)&0xf) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((l1a_rx_cnt[itbin]>>4)&0xf) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((l1a_rx_cnt[itbin])&0xf) ;
      }
    }
    if (ich == 76) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((bxn[itbin] >> 8) & 0xf) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((bxn[itbin] >> 4) & 0xf) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((bxn[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 95) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((dmb[itbin] >> 12) & 0xf ) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((dmb[itbin] >> 8) & 0xf ) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << std::hex << ((dmb[itbin] >> 4) & 0xf) ;
      }
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((dmb[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 98 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc0_bxn[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 101 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
        (*MyOutput_) << ((rpc1_bxn[itbin]) & 0xf ) ;
      }
    }
    //      
    if (ich == 104 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc2_bxn[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 107 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc3_bxn[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 115 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc0_nhits[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 119 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc1_nhits[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 123 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc2_nhits[itbin]) & 0xf ) ;
      }
    }
    //
    if (ich == 127 ) {
      (*MyOutput_) << std::endl;
      (*MyOutput_) << scope_tag[ich] ;
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	(*MyOutput_) << ((rpc3_nhits[itbin]) & 0xf ) ;
      }
    }
    //
    (*MyOutput_) << std::endl;
    //
  }    
  // JMT close the file so it actually gets all flushed
  //if (pfile) fclose(pfile);
  //
  END:
    return;
} //scope

std::bitset<22> TMB::calCRC22(const std::vector< std::bitset<16> >& datain){
  std::bitset<22> CRC;
  CRC.reset();
  for(unsigned int i=0;i<datain.size()-4;i++){
    //    printf("Taking %d %x \n",i,(unsigned int)datain[i].to_ulong());
    CRC=nextCRC22_D16(datain[i],CRC);
  }
  return CRC;
}


std::bitset<22> TMB::nextCRC22_D16(const std::bitset<16>& D, 
				       const std::bitset<22>& C){
  std::bitset<22> NewCRC;
  
  NewCRC[ 0] = D[ 0] ^ C[ 6];
  NewCRC[ 1] = D[ 1] ^ D[ 0] ^ C[ 6] ^ C[ 7];
  NewCRC[ 2] = D[ 2] ^ D[ 1] ^ C[ 7] ^ C[ 8];
  NewCRC[ 3] = D[ 3] ^ D[ 2] ^ C[ 8] ^ C[ 9];
  NewCRC[ 4] = D[ 4] ^ D[ 3] ^ C[ 9] ^ C[10];
  NewCRC[ 5] = D[ 5] ^ D[ 4] ^ C[10] ^ C[11];
  NewCRC[ 6] = D[ 6] ^ D[ 5] ^ C[11] ^ C[12];
  NewCRC[ 7] = D[ 7] ^ D[ 6] ^ C[12] ^ C[13];
  NewCRC[ 8] = D[ 8] ^ D[ 7] ^ C[13] ^ C[14];
  NewCRC[ 9] = D[ 9] ^ D[ 8] ^ C[14] ^ C[15];
  NewCRC[10] = D[10] ^ D[ 9] ^ C[15] ^ C[16];
  NewCRC[11] = D[11] ^ D[10] ^ C[16] ^ C[17];
  NewCRC[12] = D[12] ^ D[11] ^ C[17] ^ C[18];
  NewCRC[13] = D[13] ^ D[12] ^ C[18] ^ C[19];
  NewCRC[14] = D[14] ^ D[13] ^ C[19] ^ C[20];
  NewCRC[15] = D[15] ^ D[14] ^ C[20] ^ C[21];
  NewCRC[16] = D[15] ^ C[ 0] ^ C[21];
  NewCRC[17] = C[ 1];
  NewCRC[18] = C[ 2];
  NewCRC[19] = C[ 3];
  NewCRC[20] = C[ 4];
  NewCRC[21] = C[ 5];

  return NewCRC;
}
//
int TMB::TestArray(){
  (*MyOutput_) << "In TestArray" << std::endl;
  int data[] = {      
    0x6b0c 
    ,0x13e7 
    ,0x1291 
    ,0x135f 
    ,0x5ba 
    ,0x13 
    ,0x3ec 
    ,0x12e9 
    ,0x402d 
    ,0x157 
    ,0x515 
    ,0x29d1 
    ,0x5125 
    ,0x1660 
    ,0x3aa5 
    ,0x1700 
    ,0x243f 
    ,0x5d 
    ,0x21 
    ,0xbb9 
    ,0x0 
    ,0x53 
    ,0xcff 
    ,0x23 
    ,0x24 
    ,0x38ca 
    ,0x6e0b 
    ,0x0 
    ,0x0 
    ,0x0 
    ,0x0 
    ,0x0 
    ,0x0 
    ,0x100 
    ,0x100 
    ,0x100 
    ,0x100 
    ,0x100 
    ,0x100 
    ,0x200 
    ,0x200 
    ,0x200 
    ,0x200 
    ,0x200 
    ,0x200 
    ,0x300 
    ,0x300 
    ,0x300 
    ,0x300 
    ,0x300 
    ,0x300 
    ,0x400 
    ,0x400 
    ,0x400 
    ,0x400 
    ,0x400 
    ,0x400 
    ,0x500 
    ,0x500 
    ,0x500 
    ,0x500 
    ,0x500 
    ,0x500 
    ,0x600 
    ,0x600 
    ,0x600 
    ,0x600 
    ,0x600 
    ,0x600 
    ,0x1000 
    ,0x1000 
    ,0x1000 
    ,0x1000 
    ,0x1000 
    ,0x1000 
    ,0x1100 
    ,0x1100 
    ,0x1100 
    ,0x1100 
    ,0x1100 
    ,0x1100 
    ,0x1200 
    ,0x1200 
    ,0x1200 
    ,0x1200 
    ,0x1200 
    ,0x1200 
    ,0x1300 
    ,0x1300 
    ,0x1300 
    ,0x1300 
    ,0x1300 
    ,0x1300 
    ,0x1400 
    ,0x1400 
    ,0x1400 
    ,0x1400 
    ,0x1400 
    ,0x1400 
    ,0x1500 
    ,0x1500 
    ,0x1500 
    ,0x1500 
    ,0x1500 
    ,0x1500 
    ,0x1600 
    ,0x1600 
    ,0x1600 
    ,0x1600 
    ,0x1600 
    ,0x1600 
    ,0x2000 
    ,0x2000 
    ,0x2000 
    ,0x2000 
    ,0x2000 
    ,0x2000 
    ,0x2100 
    ,0x2180 
    ,0x2100 
    ,0x2100 
    ,0x2100 
    ,0x2100 
    ,0x2280 
    ,0x2280 
    ,0x2280 
    ,0x2200 
    ,0x2200 
    ,0x2200 
    ,0x2300 
    ,0x2300 
    ,0x2380 
    ,0x2300 
    ,0x2300 
    ,0x2300 
    ,0x2400 
    ,0x2400 
    ,0x2400 
    ,0x2400 
    ,0x2400 
    ,0x2400 
    ,0x2500 
    ,0x2500 
    ,0x2500 
    ,0x2500 
    ,0x2500 
    ,0x2500 
    ,0x2600 
    ,0x2600 
    ,0x2600 
    ,0x2600 
    ,0x2600 
    ,0x2600 
    ,0x3000 
    ,0x3000 
    ,0x3000 
    ,0x3000 
    ,0x3000 
    ,0x3000 
    ,0x3100 
    ,0x3100 
    ,0x3100 
    ,0x3101 
    ,0x3100 
    ,0x3100 
    ,0x3200 
    ,0x3200 
    ,0x3200 
    ,0x3200 
    ,0x3200 
    ,0x3200 
    ,0x3300 
    ,0x3300 
    ,0x3300 
    ,0x3301 
    ,0x3300 
    ,0x3302 
    ,0x3400 
    ,0x3400 
    ,0x3400 
    ,0x3400 
    ,0x3401 
    ,0x3400 
    ,0x3500 
    ,0x3500 
    ,0x3500 
    ,0x3500 
    ,0x3500 
    ,0x3500 
    ,0x3600 
    ,0x3600 
    ,0x3600 
    ,0x3600 
    ,0x3601 
    ,0x3600 
    ,0x4000 
    ,0x4000 
    ,0x4000 
    ,0x4000 
    ,0x4000 
    ,0x4000 
    ,0x4100 
    ,0x4100 
    ,0x4100 
    ,0x4100 
    ,0x4100 
    ,0x4100 
    ,0x4200 
    ,0x4200 
    ,0x4200 
    ,0x4200 
    ,0x4200 
    ,0x4200 
    ,0x4300 
    ,0x4300 
    ,0x4300 
    ,0x4300 
    ,0x4300 
    ,0x4300 
    ,0x4400 
    ,0x4400 
    ,0x4400 
    ,0x4400 
    ,0x4400 
    ,0x4400 
    ,0x4500 
    ,0x4500 
    ,0x4500 
    ,0x4500 
    ,0x4500 
    ,0x4500 
    ,0x4600 
    ,0x4600 
    ,0x4600 
    ,0x4600 
    ,0x4600 
    ,0x4600 
    ,0x6b04 
    ,0xff 
    ,0xf7 
    ,0x1ff 
    ,0xf7 
    ,0x2ff 
    ,0xf7 
    ,0x3ff 
    ,0xf7 
    ,0x4ff 
    ,0xf7 
    ,0x5ff 
    ,0xf7 
    ,0x6ff 
    ,0xf7 
    ,0x10ff 
    ,0x17ff 
    ,0x11ff 
    ,0x17ff 
    ,0x12ff 
    ,0x17ff 
    ,0x13ff 
    ,0x17ff 
    ,0x14ff 
    ,0x17ff 
    ,0x15ff 
    ,0x17ff 
    ,0x16ff 
    ,0x17ff 
    ,0x20ff 
    ,0x20f7 
    ,0x21ff 
    ,0x20f7 
    ,0x22ff 
    ,0x20f7 
    ,0x23ff 
    ,0x20f7 
    ,0x24ff 
    ,0x20f7 
    ,0x25ff 
    ,0x20f7 
    ,0x26ff 
    ,0x20f7 
    ,0x30ff 
    ,0x37ff 
    ,0x31ff 
    ,0x37ff 
    ,0x32ff 
    ,0x37ff 
    ,0x33ff 
    ,0x37ff 
    ,0x34ff 
    ,0x37ff 
    ,0x35ff 
    ,0x37ff 
    ,0x36ff 
    ,0x37ff 
    ,0x6e04 
    ,0x6e0c 
    ,0xdf1a 
    ,0xdb92 
    ,0xde0f 
    ,-1
  };
  std::vector < std::bitset<16> > alct_data;
  for (int i=0; data[i] != -1; i++ ) {
    alct_data.push_back((std::bitset<16>)data[i]);
  }
  int CRC_end  = alct_data.size();
  printf(" Size = %d \n",CRC_end);
  int CRC_low  = (alct_data[CRC_end-3].to_ulong()) &0x7ff ;
  int CRC_high = (alct_data[CRC_end-2].to_ulong()) &0x7ff ;
  int CRCdata  = (CRC_high<<11) | CRC_low ;
  printf(" CRC in data stream %x %x %x \n",(unsigned int)alct_data[CRC_end-3].to_ulong(),
	 (unsigned int)alct_data[CRC_end-2].to_ulong(),
	 CRCdata);
  printf(" CRC %x \n",TMBCRCcalc(alct_data));
  return 0;
}
//
int TMB::TMBCRCcalc(std::vector<std::bitset <16> >& TMBData) {
  //
  std::bitset<22> CRC=calCRC22(TMBData);
  //  (*MyOutput_) << " Test here " << CRC.to_ulong() << std::endl ;
  return CRC.to_ulong();
  //
}
//
void TMB::FireALCTInjector(){
  //
  tmb_vme(VME_READ, alct_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =   rcvbuf[0]&0xff ;
  sndbuf[1] =  (rcvbuf[1]&0xff) | 0x2 ;
  tmb_vme(VME_WRITE, alct_inj_adr, sndbuf,rcvbuf,NOW);
  //
}

void TMB::ClearALCTInjector(){
  //
  tmb_vme(VME_READ, alct_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =  (rcvbuf[0]&0xff) ;
  sndbuf[1] =  (rcvbuf[1]&0xfd) ;
  tmb_vme(VME_WRITE, alct_inj_adr, sndbuf,rcvbuf,NOW);
  //
}

void TMB::ClearScintillatorVeto(){
  //
  tmb_vme(VME_READ, seqmod_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0xef) | (0x1<<4);
  sndbuf[1] =  rcvbuf[1]&0xff ;
  tmb_vme(VME_WRITE, seqmod_adr, sndbuf,rcvbuf,NOW);
  //
}

void TMB::FireCLCTInjector(){
  //
  tmb_vme(VME_READ, cfeb_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =  (rcvbuf[0]&0x7f) | (0x80) ;
  sndbuf[1] =  (rcvbuf[1]&0xff);
  tmb_vme(VME_WRITE, cfeb_inj_adr, sndbuf,rcvbuf,NOW);
  //
}

void TMB::ClearCLCTInjector(){
  //
  tmb_vme(VME_READ, cfeb_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =  (rcvbuf[0]&0x7f) ;
  sndbuf[1] =  (rcvbuf[1]&0xff);
  tmb_vme(VME_WRITE, cfeb_inj_adr, sndbuf,rcvbuf,NOW);
  //
}



void TMB::DisableALCTInputs(){
  //
  tmb_vme(VME_READ, alct_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =  (rcvbuf[0]&0xff) ;
  sndbuf[1] =  (rcvbuf[1]&0xfe) | (0x1);
  tmb_vme(VME_WRITE, alct_inj_adr, sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::DisableALCTCLCTSync(){
  //
  tmb_vme(VME_READ, alct_inj_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =  (rcvbuf[0]&0xff) ;
  sndbuf[1] =  (rcvbuf[1]&0xfb) ;
  tmb_vme(VME_WRITE, alct_inj_adr, sndbuf,rcvbuf,NOW);
  //
}


//
void TMB::DisableCLCTInputs(){
  //
  tmb_vme(VME_READ,cfeb_inj_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0xff);
  sndbuf[1] = (rcvbuf[1]&0xe0) ;
  tmb_vme(VME_WRITE,cfeb_inj_adr,sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::DisableExternalCCB(){
  //
  tmb_vme(VME_READ,ccb_cfg_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0xff);
  sndbuf[1] = (rcvbuf[1]&0xfc) | (0x3);
  tmb_vme(VME_WRITE,ccb_cfg_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::EnableInternalL1aEmulator(){
  //
  tmb_vme(VME_READ,ccb_cfg_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0xff);
  sndbuf[1] = (rcvbuf[1]&0xfb) | (0x4);
  tmb_vme(VME_WRITE,ccb_cfg_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::DisableInternalL1aSequencer(){
  //
  tmb_vme(VME_READ,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0x0f);
  sndbuf[1] = (rcvbuf[1]&0xff) ;
  tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::EnableInternalL1aSequencer(){
  //
  tmb_vme(VME_READ,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0]&0x0f) | 0x10;
  sndbuf[1] = (rcvbuf[1]&0xff) ;
  tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::EnableCLCTInputs(int CLCTInputs = 0x1f){
//
   int adr;
   int rd_data;
   adr = cfeb_inj_adr ;
   tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
   rd_data   = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
   sndbuf[0] = rcvbuf[0];
   sndbuf[1] = (rcvbuf[1] & 0xe0) | CLCTInputs ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
//
}

void TMB::EnableL1aRequest(){
  //
  int adr;
  adr = ccb_trig_adr;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  //printf(" Current %x %x \n",rcvbuf[0], rcvbuf[1]);
  //
  sndbuf[0] = rcvbuf[0];
  sndbuf[1] = 4;
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  //
  //tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  //printf(" Setting to %x %x \n",rcvbuf[0], rcvbuf[1]);
  //
}
//
void TMB::DisableL1aRequest(){
  //
  int adr = ccb_trig_adr;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  //printf(" Current %x %x \n",rcvbuf[0], rcvbuf[1]);
  //
  sndbuf[0] = rcvbuf[0];
  sndbuf[1] = 0;
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  //
  //tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  //printf(" Setting to %x %x \n",rcvbuf[0], rcvbuf[1]);
  //
}
//
////////////////////////////////////////////////////
// ALCT and TMB data from VME reads
////////////////////////////////////////////////////
void TMB::TMBRawhits() {
  TMBRawhits(100000);
  return;
}
//
void TMB::TMBRawhits(int microseconds_between_data_reads){
  //
  bool read_ok = false;
  //
  int max_number_of_times = 10;  //prevent going into an infinite loop
  int number_of_reads = 0;
  //
  while (!read_ok && (number_of_reads<max_number_of_times) ) {
    //
    number_of_reads++;
    //
    //pretrigger and halt until next unhalt arrives:
    SetPretriggerHalt(1);
    WriteRegister(seq_clct_adr);
    ::usleep(microseconds_between_data_reads);   // Give the chamber time to trigger on and read an event
    //
    // Attempt to read the data:
    ResetRAMAddress();
    read_ok = ReadTMBRawhits_();   //check to see that pretrigger made a L1A -> read out...
    //
    // Pretrigger unhalt, go back to normal data taking:
    SetPretriggerHalt(0);
    WriteRegister(seq_clct_adr);
    ::usleep(10000);
  }
  //
  if (number_of_reads >= max_number_of_times) 
    (*MyOutput_) << "TMB read " << std::dec << number_of_reads << " times with no data..." << std::endl;
  //
  return;
}
//
void TMB::ResetRAMAddress(){
  //
  //Clear RAM address for next event
  //
  WriteRegister(dmb_ram_adr,0x2000); //reset RAM write address
  WriteRegister(dmb_ram_adr,0x0000); //unreset
  //
  return;
}
//
bool TMB::ReadTMBRawhits_(){
  //
  tmb_data_.clear();
  //
  // Check state machine, is it halted while we extract the data?
  //
  ReadRegister(seq_clct_adr);
  int halt_state = GetReadPretriggerHalt();
  if (debug_) (*MyOutput_) << "TMB halt_state before read RAM = " << halt_state << std::endl;
  //
  if (!halt_state) {
    (*MyOutput_) << "ERROR:  TMB not halted.  halt_state before read RAM = " << halt_state << std::endl;
    return false;
  }
  //
  int dmb_busy  = 1;
  dmb_wordcount_ = 0;
  //
  // Probe the RAM to see if there is data.  If there is nothing after a few attempts, try again...
  //
  const int max_number_of_tries = 5;
  int number_of_tries = 0;
  //
  while( ((dmb_busy) || (dmb_wordcount_ <= 0)) && (number_of_tries < max_number_of_tries) ) {
    //
    number_of_tries++;
    //
    int rd_data = ReadRegister(dmb_wdcnt_adr);
    dmb_wordcount_   = rd_data & 0x0fff;
    dmb_busy    = (rd_data>>14) & 0x1;  
    //
    //(*MyOutput_) << "Try to get TMB data through VME " << std::dec << number_of_tries << " times" << std::endl;
    //(*MyOutput_) << "DMB busy       = " << dmb_busy << std::endl;
    //(*MyOutput_) << "DMB word count = " << std::dec << dmb_wordcount_ << std::endl;
  }
  //
  if (number_of_tries >= max_number_of_tries) {
    if (debug_) (*MyOutput_) << "DMB word count=0 or DMB busy "<< std::dec << number_of_tries << " times..." << std::endl;
    return false;
  } else {
    if (debug_) (*MyOutput_) << "Got TMB data on the " << std::dec << number_of_tries << " time" << std::endl;
  }
  // Get the data:
  for(int i=0; i<dmb_wordcount_; i++) {
    //
    //Write RAM read address
    int address = (i & 0xFFFF);
    WriteRegister(dmb_ram_adr,address);
    //    
    //Read RAM data
    int dmb_rdata = ReadRegister(dmb_rdata_adr);
    //
    tmb_data_.push_back((std::bitset<16>) dmb_rdata);
    if (debug_) (*MyOutput_) << "Address = " << std::dec << i << ", data = " << std::hex << dmb_rdata << std::endl;
  }
  //
  if (debug_) (*MyOutput_) << "Total number of words read = " << std::dec << tmb_data_.size() << std::endl;
  //
  // Determine the CRC to see if we've extracted the data correctly, among other possibilities
  int CRC_end  = tmb_data_.size();
  int CRC_low  = (tmb_data_[CRC_end-4].to_ulong()) &0x7ff;
  int CRC_high = (tmb_data_[CRC_end-3].to_ulong()) &0x7ff;
  //
  int CRCdata  = (CRC_high<<11) | CRC_low;
  int CRCcalc = TMBCRCcalc(tmb_data_);
  //
  int dataOK = compareValues("TMB CRC", CRCcalc, CRCdata);
  //
  if (dataOK) {
    if (debug_) (*MyOutput_) << "CRC OK..." << std::endl;
    DecodeTMBRawHits_();
  } else {
    (*MyOutput_) << "CRC not OK..." << std::endl;
  }
  //
  return dataOK;
}
//
void TMB::ALCTRawhits() {
  //   
  bool read_ok = false;
  //
  int max_number_of_times = 10;  //prevent going into an infinite loop
  int number_of_reads = 0;
  //
  while (!read_ok && (number_of_reads<max_number_of_times) ) {
    //
    number_of_reads++;
    //
    //pretrigger and halt until next unhalt arrives:
    //    if (debug_) std::cout << "TMB:  Halt pretrigger" << std::endl;
    //    SetPretriggerHalt(1);
    //    WriteRegister(seq_clct_adr);
    //    ::sleep(100000);   // Give the chamber time to trigger on and read an event
    //
    // Attempt to read the data:
    read_ok = ReadALCTRawhits_();   //check to see that pretrigger made a L1A -> read out...
    //
    // Pretrigger unhalt, go back to normal data taking:
    //    if (debug_) std::cout << "TMB:  Unhalt pretrigger" << std::endl;
    //    SetPretriggerHalt(0);
    //    WriteRegister(seq_clct_adr);
    //    ::usleep(10000);
  }
  //
  if (number_of_reads >= max_number_of_times) 
    (*MyOutput_) << "TMB read ALCT " << std::dec << number_of_reads << " times with no data..." << std::endl;
  //
  return;
}
//
bool TMB::ReadALCTRawhits_() {
  //
  const int max_number_of_waits_for_busy = 20;
  //
  ReadRegister(seq_clct_adr);
  int halt_state = GetReadPretriggerHalt();
  if (debug_) std::cout << "TMB:  TMB halt_state before read ALCT raw hits RAM = " << halt_state << std::endl;
  //
  //  if (!halt_state) {
  //    std::cout << "ERROR:  TMB not halted.  halt_state before read ALCT raw hits RAM = " << halt_state << std::endl;
  //    return false;
  //  }
  //
  if (!CheckAlctFIFOBusy(max_number_of_waits_for_busy)) {
    std::cout << "TMB:  At beginning of ReadALCTRawHits(),  ALCT FIFO Busy " << max_number_of_waits_for_busy << " times... " << std::endl;
    std::cout << "..... aborting" << std::endl;
    return false;
  }
  //
  int data = ReadRegister(alct_fifo_adr);
  if (debug_) (*MyOutput_) << "Register 0x3E -> Read=0x" << std::hex << data << std::endl;
  //
  int alct_wdcnt = GetReadAlctRawWordCount();
  if (debug_) (*MyOutput_) << "Number of raw words =" << std::dec << alct_wdcnt << std::endl;
  //
  if (alct_wdcnt == 0) {
    if (debug_) std::cout << "Try again" << std::endl;
    return false;
  }
  //
  for(int i=0;i<alct_wdcnt;i++) {
    //
    //Write RAM read address to TMB
    SetAlctRawSync(0);
    SetAlctDemuxMode(0);         //set to enable alctfifo2 address to contain raw hits
    SetAlctRawReadAddress(i);
    WriteRegister(alctfifo1_adr);
    int data = ReadRegister(alctfifo1_adr);
    //    if (debug_) (*MyOutput_) << "Register 0xA2 -> Read=" << std::hex << data << std::endl;
    //
    //Read RAM data from TMB...
    //
    // Least Significant Bits:
    data = ReadRegister(alctfifo2_adr);
    long int alct_rdata = (GetReadAlctRawDataLeastSignificantBits() & 0xffff);
    //    if (debug_) (*MyOutput_) << "Register 0xA4 -> Read=" << std::hex << data << std::endl;
    //
    // Add on the most significant bits:
    data = ReadRegister(alct_fifo_adr);
    //    if (debug_) (*MyOutput_) << "Register 0x3E -> Read=" << std::hex << data << std::endl;
    alct_rdata |= ( (GetReadAlctRawDataMostSignificantBits()&0x3) << 16 );
    //
    (*MyOutput_) << "Adr=" << std::dec << std::setw(4) << i 
		 << ", Data=0x" << std::hex 
		 << ((alct_rdata>>16)&0xf) 
		 << ((alct_rdata>>12)&0xf) 
		 << ((alct_rdata>> 8)&0xf) 
		 << ((alct_rdata>> 4)&0xf) 
		 << ((alct_rdata>> 0)&0xf)
		 << std::endl;
    if (debug_) 
      std::cout << "Adr=" << std::dec << std::setw(4) << i 
		<< ", Data=" << std::hex 
		<< ((alct_rdata>>16)&0xf) 
		<< ((alct_rdata>>12)&0xf) 
		<< ((alct_rdata>> 8)&0xf) 
		<< ((alct_rdata>> 4)&0xf) 
		<< ((alct_rdata>> 0)&0xf)
		<< std::endl;
    //
    //alct_data.push_back((std::bitset<16>)alct_rdata);
  }
  //
  //  if ( alct_wdcnt > 0 ) {
  //    
  //    printf("The size is %d\n",alct_data.size());
  //	
  //    int CRC_end  = alct_data.size();
  //    int CRC_low  = (alct_data[CRC_end-4].to_ulong()) &0x7ff ;
  //    int CRC_high = (alct_data[CRC_end-3].to_ulong()) &0x7ff ;
  //    int CRCdata  = (CRC_high<<11) | CRC_low ;
  //    
  //    int CRCcalc = TMBCRCcalc(alct_data) ;
  //    
  //    printf(" CRC %x \n",CRCcalc);
  //    printf(" CRC in data stream %lx %lx %x \n",alct_data[CRC_end-4].to_ulong(),
  //	   alct_data[CRC_end-3].to_ulong(),CRCdata);
  //	 
  //    if ( CRCcalc != CRCdata ) {
  //      printf("ALCT CRC doesn't agree \n");
  //    } else {
  //      printf("ALCT CRC does    agree \n");
  //    }
  //
  //}
  //
  //  alct_data.clear();
  //
  return true;
}
//
bool TMB::ResetALCTRAMAddress(){
  //
  //Clear RAM address
  //
  if (debug_) std::cout << "Reset ALCTRAMAddress" << std::endl;
  //
  const int maximum_number_of_allowed_busies = 20;
  //
  if (!CheckAlctFIFOBusy(maximum_number_of_allowed_busies)) {
    std::cout << "TMB:  At beginning of ResetALCTRAMAddress(),  ALCT FIFO Busy " << maximum_number_of_allowed_busies << " times... " << std::endl;
    std::cout << "..... aborting" << std::endl;
    return false;
  }
  //
  //reset ALCT raw hits FIFO controller:
  SetAlctRawReset(1);
  WriteRegister(alctfifo1_adr);
  //
  if (!CheckAlctFIFOBusy(maximum_number_of_allowed_busies)) {
    std::cout << "TMB:  After resetting ALCT FIFO controller,  ALCT FIFO Busy " << maximum_number_of_allowed_busies << " times... " << std::endl;
    std::cout << "..... aborting" << std::endl;
    return false;
  }
  //
  //unreset ALCT raw hits FIFO controller:
  SetAlctRawReset(0);
  WriteRegister(alctfifo1_adr);
  //
  if (!CheckAlctFIFOBusy(maximum_number_of_allowed_busies)) {
    std::cout << "TMB:  After unresetting ALCT FIFO controller,  ALCT FIFO Busy " << maximum_number_of_allowed_busies << " times... " << std::endl;
    std::cout << "..... aborting" << std::endl;
    return false;
  }
  //
  return true;
}
//
//
bool TMB::CheckAlctFIFOBusy(int number_of_checks_before_aborting) {
  //
  int number_of_checks = 0;
  //
  int data = ReadRegister(alct_fifo_adr);
  if (debug_) std::cout << "CheckAlctFIFOBusy:  TMB register 0x3E = " << std::hex << data << std::endl;
  //
  while ( GetReadAlctRawBusy() != 0 && 
	  GetReadAlctRawDone() != 1 && 
	  number_of_checks < number_of_checks_before_aborting ){
    //
    if (debug_) std::cout << "TMB:  ALCT raw hits FIFO busy writing ALCT data... " << number_of_checks << " times" << std::endl;
    //
    data = ReadRegister(alct_fifo_adr);
    if (debug_) std::cout << "CheckAlctFIFOBusy:  TMB register 0x3E = " << std::hex << data << std::endl;
    //
    number_of_checks++;
  }
  //
  if (number_of_checks < number_of_checks_before_aborting) {
    if (debug_) std::cout << "CheckAlctFIFOBusy:  Not busy, continuing..." << std::endl;
    return true;
  } else {
    if (debug_) std::cout << "CheckAlctFIFOBusy:  BUSY, should abort..." << std::endl;
    return false;
  }
}
//
//
void TMB::DecodeTMBRawHits_() {
  //
  for (int word_count=0; word_count<dmb_wordcount_; word_count++) 
    DecodeTMBRawHitWord_(word_count);
  //
  return;
}
//
void TMB::DecodeTMBRawHitWord_(int address) {
  //
  int data = tmb_data_[address].to_ulong();
  //
  if (address == 0) {
    h0_beginning_of_cathode_ = ExtractValueFromData(data ,h0_beginning_of_cathode_lo_bit ,h0_beginning_of_cathode_hi_bit );
    h0_marker_6_             = ExtractValueFromData(data ,h0_marker_6_lo_bit             ,h0_marker_6_hi_bit             );
    //
  } else if (address == 1) { 
    h1_nTbins_per_cfeb_ = ExtractValueFromData(data ,h1_nTbins_per_cfeb_lo_bit ,h1_nTbins_per_cfeb_hi_bit );
    h1_cfebs_read_      = ExtractValueFromData(data ,h1_cfebs_read_lo_bit      ,h1_cfebs_read_hi_bit      );
    h1_fifo_mode_       = ExtractValueFromData(data ,h1_fifo_mode_lo_bit       ,h1_fifo_mode_hi_bit       );
    //
  } else if (address == 2) { 
    h2_l1a_counter_ = ExtractValueFromData(data ,h2_l1a_counter_lo_bit ,h2_l1a_counter_hi_bit );
    h2_csc_id_      = ExtractValueFromData(data ,h2_csc_id_lo_bit      ,h2_csc_id_hi_bit      );
    h2_board_id_    = ExtractValueFromData(data ,h2_board_id_lo_bit    ,h2_board_id_hi_bit    );
    h2_l1a_type_    = ExtractValueFromData(data ,h2_l1a_type_lo_bit    ,h2_l1a_type_hi_bit    );
    //
  } else if (address == 3) { 
    h3_bxn_counter_   = ExtractValueFromData(data ,h3_bxn_counter_lo_bit   ,h3_bxn_counter_hi_bit   );
    h3_record_type_   = ExtractValueFromData(data ,h3_record_type_lo_bit   ,h3_record_type_hi_bit   );
    h3_scope_in_data_ = ExtractValueFromData(data ,h3_scope_in_data_lo_bit ,h3_scope_in_data_hi_bit );
    //
  } else if (address == 4) { 
    h4_nheader_words_   = ExtractValueFromData(data ,h4_nheader_words_lo_bit   ,h4_nheader_words_hi_bit   );
    h4_nCFEBs_read_     = ExtractValueFromData(data ,h4_nCFEBs_read_lo_bit     ,h4_nCFEBs_read_hi_bit     );
    h4_has_buffer_data_ = ExtractValueFromData(data ,h4_has_buffer_data_lo_bit ,h4_has_buffer_data_hi_bit );
    h4_fifo_pretrig_    = ExtractValueFromData(data ,h4_fifo_pretrig_lo_bit    ,h4_fifo_pretrig_hi_bit    );
    //
  } else if (address == 5) { 
    h5_l1a_at_pretrig_                   = ExtractValueFromData(data ,h5_l1a_at_pretrig_lo_bit                   ,h5_l1a_at_pretrig_hi_bit                   );
    h5_trigger_source_vector_            = ExtractValueFromData(data ,h5_trigger_source_vector_lo_bit            ,h5_trigger_source_vector_hi_bit            );
    h5_trigger_source_halfstrip_distrip_ = ExtractValueFromData(data ,h5_trigger_source_halfstrip_distrip_lo_bit ,h5_trigger_source_halfstrip_distrip_hi_bit );
    //
  } else if (address == 6) { 
    h6_aff_to_dmb_  = ExtractValueFromData(data ,h6_aff_to_dmb_lo_bit  ,h6_aff_to_dmb_hi_bit  );
    h6_cfeb_exists_ = ExtractValueFromData(data ,h6_cfeb_exists_lo_bit ,h6_cfeb_exists_hi_bit );
    h6_run_info_    = ExtractValueFromData(data ,h6_run_info_lo_bit    ,h6_run_info_hi_bit    );
    //
  } else if (address == 7) { 
    h7_bxn_at_clct_pretrig_ = ExtractValueFromData(data ,h7_bxn_at_clct_pretrig_lo_bit ,h7_bxn_at_clct_pretrig_hi_bit );
    h7_sync_err_            = ExtractValueFromData(data ,h7_sync_err_lo_bit            ,h7_sync_err_hi_bit            );
    //
  } else if (address == 8) { 
    h8_clct0_lsbs_ = ExtractValueFromData(data ,h8_clct0_lsbs_lo_bit ,h8_clct0_lsbs_hi_bit );
    //
  } else if (address == 9) { 
    h9_clct1_lsbs_ = ExtractValueFromData(data ,h9_clct1_lsbs_lo_bit ,h9_clct1_lsbs_hi_bit );
    //
  } else if (address == 10) { 
    h10_clct0_msbs_            = ExtractValueFromData(data ,h10_clct0_msbs_lo_bit            ,h10_clct0_msbs_hi_bit            );
    h10_clct1_msbs_            = ExtractValueFromData(data ,h10_clct1_msbs_lo_bit            ,h10_clct1_msbs_hi_bit            );
    h10_clct0_invalid_pattern_ = ExtractValueFromData(data ,h10_clct0_invalid_pattern_lo_bit ,h10_clct0_invalid_pattern_hi_bit );
    //
  } else if (address == 11) {
    h11_alct_clct_match_           = ExtractValueFromData(data ,h11_alct_clct_match_lo_bit           ,h11_alct_clct_match_hi_bit           );
    h11_alct_trig_only_            = ExtractValueFromData(data ,h11_alct_trig_only_lo_bit            ,h11_alct_trig_only_hi_bit            );
    h11_clct_trig_only_            = ExtractValueFromData(data ,h11_clct_trig_only_lo_bit            ,h11_clct_trig_only_hi_bit            );
    h11_clct0_alct_bxn_diff_       = ExtractValueFromData(data ,h11_clct0_alct_bxn_diff_lo_bit       ,h11_clct0_alct_bxn_diff_hi_bit       );
    h11_clct1_alct_bxn_diff_       = ExtractValueFromData(data ,h11_clct1_alct_bxn_diff_lo_bit       ,h11_clct1_alct_bxn_diff_hi_bit       );
    h11_alct_in_clct_match_window_ = ExtractValueFromData(data ,h11_alct_in_clct_match_window_lo_bit ,h11_alct_in_clct_match_window_hi_bit );
    h11_triad_persistence_         = ExtractValueFromData(data ,h11_triad_persistence_lo_bit         ,h11_triad_persistence_hi_bit         );
    //
  } else if (address == 12) { 
    h12_mpc0_frame0_lsbs_ = ExtractValueFromData(data ,h12_mpc0_frame0_lsbs_lo_bit ,h12_mpc0_frame0_lsbs_hi_bit );
    //
  } else if (address == 13) { 
    h13_mpc0_frame1_lsbs_ = ExtractValueFromData(data ,h13_mpc0_frame1_lsbs_lo_bit ,h13_mpc0_frame1_lsbs_hi_bit );
    //
  } else if (address == 14) { 
    h14_mpc1_frame0_lsbs_ = ExtractValueFromData(data ,h14_mpc1_frame0_lsbs_lo_bit ,h14_mpc1_frame0_lsbs_hi_bit );
    //
  } else if (address == 15) { 
    h15_mpc1_frame1_lsbs_ = ExtractValueFromData(data ,h15_mpc1_frame1_lsbs_lo_bit ,h15_mpc1_frame1_lsbs_hi_bit );
    //
  } else if (address == 16) { 
    h16_mpc0_frame0_msbs_              = ExtractValueFromData(data ,h16_mpc0_frame0_msbs_lo_bit              ,h16_mpc0_frame0_msbs_hi_bit              );
    h16_mpc0_frame1_msbs_              = ExtractValueFromData(data ,h16_mpc0_frame1_msbs_lo_bit              ,h16_mpc0_frame1_msbs_hi_bit              );
    h16_mpc1_frame0_msbs_              = ExtractValueFromData(data ,h16_mpc1_frame0_msbs_lo_bit              ,h16_mpc1_frame0_msbs_hi_bit              );
    h16_mpc1_frame1_msbs_              = ExtractValueFromData(data ,h16_mpc1_frame1_msbs_lo_bit              ,h16_mpc1_frame1_msbs_hi_bit              );
    h16_mpc_accept_                    = ExtractValueFromData(data ,h16_mpc_accept_lo_bit                    ,h16_mpc_accept_hi_bit                    );
    h16_clct_halfstrip_pretrig_thresh_ = ExtractValueFromData(data ,h16_clct_halfstrip_pretrig_thresh_lo_bit ,h16_clct_halfstrip_pretrig_thresh_hi_bit );
    h16_clct_distrip_pretrig_thresh_   = ExtractValueFromData(data ,h16_clct_distrip_pretrig_thresh_lo_bit   ,h16_clct_distrip_pretrig_thresh_hi_bit   );
    //
  } else if (address == 17) { 
    h17_write_buffer_ready_     = ExtractValueFromData(data ,h17_write_buffer_ready_lo_bit     ,h17_write_buffer_ready_hi_bit     );
    h17_pretrig_tbin_           = ExtractValueFromData(data ,h17_pretrig_tbin_lo_bit           ,h17_pretrig_tbin_hi_bit           );
    h17_write_buffer_address_   = ExtractValueFromData(data ,h17_write_buffer_address_lo_bit   ,h17_write_buffer_address_hi_bit   );
    h17_pretrig_no_free_buffer_ = ExtractValueFromData(data ,h17_pretrig_no_free_buffer_lo_bit ,h17_pretrig_no_free_buffer_hi_bit );
    h17_buffer_full_            = ExtractValueFromData(data ,h17_buffer_full_lo_bit            ,h17_buffer_full_hi_bit            );
    h17_buffer_almost_full_     = ExtractValueFromData(data ,h17_buffer_almost_full_lo_bit     ,h17_buffer_almost_full_hi_bit     );
    h17_buffer_half_full_       = ExtractValueFromData(data ,h17_buffer_half_full_lo_bit       ,h17_buffer_half_full_hi_bit       );
    h17_buffer_empty_           = ExtractValueFromData(data ,h17_buffer_empty_lo_bit           ,h17_buffer_empty_hi_bit           );
    //
  } else if (address == 18) { 
    h18_nbuf_busy_          = ExtractValueFromData(data ,h18_nbuf_busy_lo_bit          ,h18_nbuf_busy_hi_bit          );
    h18_buf_busy_           = ExtractValueFromData(data ,h18_buf_busy_lo_bit           ,h18_buf_busy_hi_bit           );
    h18_l1a_stack_overflow_ = ExtractValueFromData(data ,h18_l1a_stack_overflow_lo_bit ,h18_l1a_stack_overflow_hi_bit );
    //
  } else if (address == 19) { 
    h19_tmb_trig_pulse_         = ExtractValueFromData(data ,h19_tmb_trig_pulse_lo_bit         ,h19_tmb_trig_pulse_hi_bit         );
    h19_tmb_alct_only_          = ExtractValueFromData(data ,h19_tmb_alct_only_lo_bit          ,h19_tmb_alct_only_hi_bit          );
    h19_tmb_clct_only_          = ExtractValueFromData(data ,h19_tmb_clct_only_lo_bit          ,h19_tmb_clct_only_hi_bit          );
    h19_tmb_match_              = ExtractValueFromData(data ,h19_tmb_match_lo_bit              ,h19_tmb_match_hi_bit              );
    h19_write_buffer_ready_     = ExtractValueFromData(data ,h19_write_buffer_ready_lo_bit     ,h19_write_buffer_ready_hi_bit     );
    h19_write_buffer_available_ = ExtractValueFromData(data ,h19_write_buffer_available_lo_bit ,h19_write_buffer_available_hi_bit );
    h19_write_tbin_address_     = ExtractValueFromData(data ,h19_write_tbin_address_lo_bit     ,h19_write_tbin_address_hi_bit     );
    h19_write_buffer_address_   = ExtractValueFromData(data ,h19_write_buffer_address_lo_bit   ,h19_write_buffer_address_hi_bit   );
    //
  } else if (address == 20) { 
    h20_discard_no_write_buf_available_ = ExtractValueFromData(data ,h20_discard_no_write_buf_available_lo_bit ,h20_discard_no_write_buf_available_hi_bit );
    h20_discard_invalid_pattern_        = ExtractValueFromData(data ,h20_discard_invalid_pattern_lo_bit        ,h20_discard_invalid_pattern_hi_bit        );
    h20_discard_tmb_reject_             = ExtractValueFromData(data ,h20_discard_tmb_reject_lo_bit             ,h20_discard_tmb_reject_hi_bit             );
    h20_timeout_no_tmb_trig_pulse_      = ExtractValueFromData(data ,h20_timeout_no_tmb_trig_pulse_lo_bit      ,h20_timeout_no_tmb_trig_pulse_hi_bit      );
    h20_timeout_no_mpc_frame_           = ExtractValueFromData(data ,h20_timeout_no_mpc_frame_lo_bit           ,h20_timeout_no_mpc_frame_hi_bit           );
    h20_timeout_no_mpc_response_        = ExtractValueFromData(data ,h20_timeout_no_mpc_response_lo_bit        ,h20_timeout_no_mpc_response_hi_bit        );
    //
  } else if (address == 21) { 
    h21_match_trig_alct_delay_   = ExtractValueFromData(data ,h21_match_trig_alct_delay_lo_bit   ,h21_match_trig_alct_delay_hi_bit   );
    h21_match_trig_window_width_ = ExtractValueFromData(data ,h21_match_trig_window_width_lo_bit ,h21_match_trig_window_width_hi_bit );
    h21_mpc_tx_delay_            = ExtractValueFromData(data ,h21_mpc_tx_delay_lo_bit            ,h21_mpc_tx_delay_hi_bit            );
    //
  } else if (address == 22) {
    h22_rpc_exist_       = ExtractValueFromData(data ,h22_rpc_exist_lo_bit       ,h22_rpc_exist_hi_bit       );
    h22_rpc_list_        = ExtractValueFromData(data ,h22_rpc_list_lo_bit        ,h22_rpc_list_hi_bit        );
    h22_nrpc_            = ExtractValueFromData(data ,h22_nrpc_lo_bit            ,h22_nrpc_hi_bit            );
    h22_rpc_read_enable_ = ExtractValueFromData(data ,h22_rpc_read_enable_lo_bit ,h22_rpc_read_enable_hi_bit );
    h22_nlayers_hit_     = ExtractValueFromData(data ,h22_nlayers_hit_lo_bit     ,h22_nlayers_hit_hi_bit     );
    h22_l1a_in_window_   = ExtractValueFromData(data ,h22_l1a_in_window_lo_bit   ,h22_l1a_in_window_hi_bit   );
    //
  } else if (address == 23) { 
    h23_board_status_ = ExtractValueFromData(data ,h23_board_status_lo_bit ,h23_board_status_hi_bit );
    //
  } else if (address == 24) { 
    h24_time_since_hard_reset_ = ExtractValueFromData(data ,h24_time_since_hard_reset_lo_bit ,h24_time_since_hard_reset_hi_bit );
    //
  } else if (address == 25) { 
    h25_firmware_version_date_code_ = ExtractValueFromData(data ,h25_firmware_version_date_code_lo_bit ,h25_firmware_version_date_code_hi_bit );
    //
  }
  //
  return;
}
//
void TMB::PrintTMBRawHits() {
  //
  (*MyOutput_) << "Header 0:" << std::endl;
  (*MyOutput_) << "-> beginning of cathode record marker = " << h0_beginning_of_cathode_ << std::endl;
  (*MyOutput_) << "-> marker 6                           = " << h0_marker_6_             << std::endl;
  //
  (*MyOutput_) << "Header 1:" << std::endl;
  (*MyOutput_) << "-> number of time bins per CFEB in dump                = " << h1_nTbins_per_cfeb_ << std::endl;
  (*MyOutput_) << "-> CFEBs read out for this event                       = " << h1_cfebs_read_      << std::endl;
  (*MyOutput_) << "-> fifo mode                                           = " << h1_fifo_mode_       << std::endl;
  //
  (*MyOutput_) << "Header 2:" << std::endl;
  (*MyOutput_) << "-> L1A received and pushed on L1A stack                = " << h2_l1a_counter_ << std::endl;
  (*MyOutput_) << "-> Chamber ID number (= slot/2 or slot/2-1 if slot>12) = " << h2_csc_id_      << std::endl;
  (*MyOutput_) << "-> module ID number (= VME slot)                       = " << h2_board_id_    << std::endl;
  (*MyOutput_) << "-> L1A pop type mode                                   = " << h2_l1a_type_;
  if (h2_l1a_type_ == 0) {
    (*MyOutput_) << " = Normal CLCT trigger with buffer data and L1A window match" << std::endl;
  } else if (h2_l1a_type_ == 1) {
    (*MyOutput_) << " = ALCT-only trigger, no data buffers" << std::endl;
  } else if (h2_l1a_type_ == 2) {
    (*MyOutput_) << " = L1A-only, no matching TMB trigger, no buffer data" << std::endl;
  } else if (h2_l1a_type_ == 3) {
    (*MyOutput_) << " = TMB triggered, no L1A-window match, event has buffer data" << std::endl;
  }
  //
  (*MyOutput_) << "Header 3:" << std::endl;  
  (*MyOutput_) << "-> Bunch-crossing number pushed on L1A stack on L1A arrival = " << h3_bxn_counter_ << std::endl;
  (*MyOutput_) << "-> Record type = " << h3_record_type_;
  if (h3_record_type_ == 0 ) {
    (*MyOutput_) << " = No rawhits, full header" << std::endl;
  } else if (h3_record_type_ == 1 ) {
    (*MyOutput_) << " = Full rawhits, full header" << std::endl;
  } else if (h3_record_type_ == 2 ) {
    (*MyOutput_) << " = Local rawhits, full header" << std::endl;
  } else if (h3_record_type_ == 3 ) {
    (*MyOutput_) << " = No rawhits, short header (no buffer available at pretrigger)" << std::endl;
  }
  (*MyOutput_) << "-> internal logic analyzer scope data included in readout = 0x " << std::hex << h3_scope_in_data_ << std::endl;
  //
  (*MyOutput_) << "Header 4:" << std::endl;  
  (*MyOutput_) << "-> Number of header words                        = 0x " << std::hex << h4_nheader_words_   << std::endl;
  (*MyOutput_) << "-> Number of CFEBs readout                       = 0x " << std::hex << h4_nCFEBs_read_     << std::endl;
  (*MyOutput_) << "-> Number of CFEBs readout                       = 0x " << std::hex << h4_has_buffer_data_ << std::endl;
  (*MyOutput_) << "-> Number time bins in readout before pretrigger = 0x " << std::hex << h4_fifo_pretrig_    << std::endl;
  //
  (*MyOutput_) << "Header 5:" << std::endl;  
  (*MyOutput_) << "-> L1A number at CLCT pretrigger       = 0x " << std::hex << h5_l1a_at_pretrig_                   << std::endl;
  (*MyOutput_) << "-> trigger source vector               = 0x " << std::hex << h5_trigger_source_vector_            << std::endl;
  (*MyOutput_) << "-> trigger source halfstrip or distrip = 0x " << std::hex << h5_trigger_source_halfstrip_distrip_ << std::endl;
  //
  (*MyOutput_) << "Header 6:" << std::endl;  
  (*MyOutput_) << "-> Active CFEB list sent to DMB = 0x " << std::hex << h6_aff_to_dmb_  << std::endl;
  (*MyOutput_) << "-> List of instantiated CFEBs   = 0x " << std::hex << h6_cfeb_exists_ << std::endl;
  (*MyOutput_) << "-> Run info                     = 0x " << std::hex << h6_run_info_    << std::endl;
  //
  (*MyOutput_) << "Header 7:" << std::endl;  
  (*MyOutput_) << "-> bunch crossing number at CLCT pretrigger    = 0x " << std::hex << h7_bxn_at_clct_pretrig_ << std::endl;
  (*MyOutput_) << "-> bunch crossing number synchronization error = 0x " << std::hex << h7_sync_err_            << std::endl;
  //
  (*MyOutput_) << "Header 8:" << std::endl;  
  (*MyOutput_) << "-> CLCT0 pattern trigger (after drift) LSBS = 0x " << std::hex << h8_clct0_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 9:" << std::endl;  
  (*MyOutput_) << "-> CLCT1 pattern trigger (after drift) LSBS = 0x " << std::hex << h9_clct1_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 10:" << std::endl;  
  (*MyOutput_) << "-> CLCT0 pattern trigger (after drift) MSBS = 0x " << std::hex << h10_clct0_msbs_            << std::endl;
  (*MyOutput_) << "-> CLCT1 pattern trigger (after drift) MSBS = 0x " << std::hex << h10_clct1_msbs_            << std::endl;
  (*MyOutput_) << "-> CLCT0 had invalid pattern after drift    = 0x " << std::hex << h10_clct0_invalid_pattern_ << std::endl;
  //
  (*MyOutput_) << "Header 11:" << std::endl;  
  (*MyOutput_) << "-> ALCT and CLCT matched in time         = 0x " << std::hex << h11_alct_clct_match_           << std::endl;
  (*MyOutput_) << "-> ALCT trigger only                     = 0x " << std::hex << h11_alct_trig_only_            << std::endl;
  (*MyOutput_) << "-> CLCT trigger only                     = 0x " << std::hex << h11_clct_trig_only_            << std::endl;
  (*MyOutput_) << "-> ALCT-CLCT0 bunch crossing difference  = 0x " << std::hex << h11_clct0_alct_bxn_diff_       << std::endl;
  (*MyOutput_) << "-> ALCT-CLCT1 bunch crossing difference  = 0x " << std::hex << h11_clct1_alct_bxn_diff_       << std::endl;
  (*MyOutput_) << "-> Location of ALCT in CLCT match window = 0x " << std::hex << h11_alct_in_clct_match_window_ << std::endl;
  (*MyOutput_) << "-> triad persistence                     = 0x " << std::hex << h11_triad_persistence_         << std::endl;
  //
  (*MyOutput_) << "Header 12:" << std::endl;  
  (*MyOutput_) << "-> MPC muon0 frame 0 LSBs = 0x " << std::hex << h12_mpc0_frame0_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 13:" << std::endl;  
  (*MyOutput_) << "-> MPC muon0 frame 1 LSBs = 0x " << std::hex << h13_mpc0_frame1_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 14:" << std::endl;  
  (*MyOutput_) << "-> MPC muon1 frame 0 LSBs = 0x " << std::hex << h14_mpc1_frame0_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 15:" << std::endl;  
  (*MyOutput_) << "-> MPC muon1 frame 1 LSBs = 0x " << std::hex << h15_mpc1_frame1_lsbs_ << std::endl;
  //
  (*MyOutput_) << "Header 16:" << std::endl;  
  (*MyOutput_) << "-> MPC muon0 frame 0 MSBs              = 0x " << std::hex << h16_mpc0_frame0_msbs_              << std::endl;
  (*MyOutput_) << "-> MPC muon0 frame 1 MSBs              = 0x " << std::hex << h16_mpc0_frame1_msbs_              << std::endl;
  (*MyOutput_) << "-> MPC muon1 frame 0 MSBs              = 0x " << std::hex << h16_mpc1_frame0_msbs_              << std::endl;
  (*MyOutput_) << "-> MPC muon1 frame 1 MSBs              = 0x " << std::hex << h16_mpc1_frame1_msbs_              << std::endl;
  (*MyOutput_) << "-> MPC muon accept response            = 0x " << std::hex << h16_mpc_accept_                    << std::endl;
  (*MyOutput_) << "-> CLCT halfstrip pretrigger threshold = 0x " << std::hex << h16_clct_halfstrip_pretrig_thresh_ << std::endl;
  (*MyOutput_) << "-> CLCT distrip pretrigger threshold   = 0x " << std::hex << h16_clct_distrip_pretrig_thresh_   << std::endl;
  //
  (*MyOutput_) << "Header 17:" << std::endl;  
  (*MyOutput_) << "-> Write buffer is ready           = 0x " << std::hex << h17_write_buffer_ready_     << std::endl;
  (*MyOutput_) << "-> Tbin address for pretrig        = 0x " << std::hex << h17_pretrig_tbin_           << std::endl;
  (*MyOutput_) << "-> write buffer address            = 0x " << std::hex << h17_write_buffer_address_   << std::endl;
  (*MyOutput_) << "-> pretrig arrived, no buffer free = 0x " << std::hex << h17_pretrig_no_free_buffer_ << std::endl;
  (*MyOutput_) << "-> buffer full                     = 0x " << std::hex << h17_buffer_full_            << std::endl;
  (*MyOutput_) << "-> buffer almost full              = 0x " << std::hex << h17_buffer_almost_full_     << std::endl;
  (*MyOutput_) << "-> buffer half full                = 0x " << std::hex << h17_buffer_half_full_       << std::endl;
  (*MyOutput_) << "-> buffer empty                    = 0x " << std::hex << h17_buffer_empty_           << std::endl;
  //
  (*MyOutput_) << "Header 18:" << std::hex << std::endl;  
  (*MyOutput_) << "-> Number of buffers busy = 0x " << std::hex << h18_nbuf_busy_          << std::endl;
  (*MyOutput_) << "-> List of busy buffers   = 0x " << std::hex << h18_buf_busy_           << std::endl;
  (*MyOutput_) << "-> L1A stack overflow     = 0x " << std::hex << h18_l1a_stack_overflow_ << std::endl;
  //
  (*MyOutput_) << "Header 19:" << std::endl;  
  (*MyOutput_) << "-> TMB response                                             = 0x " << std::hex << h19_tmb_trig_pulse_         << std::endl;
  (*MyOutput_) << "-> Only ALCT triggered                                      = 0x " << std::hex << h19_tmb_alct_only_          << std::endl;
  (*MyOutput_) << "-> Only CLCT triggered                                      = 0x " << std::hex << h19_tmb_clct_only_          << std::endl;
  (*MyOutput_) << "-> ALCT*CLCT triggered                                      = 0x " << std::hex << h19_tmb_match_              << std::endl;
  (*MyOutput_) << "-> Write buffer ready at pretrig                            = 0x " << std::hex << h19_write_buffer_ready_     << std::endl;
  (*MyOutput_) << "-> write buffer either (ready -or- not required) at pretrig = 0x " << std::hex << h19_write_buffer_available_ << std::endl;
  (*MyOutput_) << "-> Tbin address at pretrig                                  = 0x " << std::hex << h19_write_tbin_address_     << std::endl;
  (*MyOutput_) << "-> Address of write buffer at pretrig                       = 0x " << std::hex << h19_write_buffer_address_   << std::endl;
  //
  (*MyOutput_) << "Header 20:" << std::endl;  
  (*MyOutput_) << "-> pretrig but no write buffer available = 0x " << std::hex << h20_discard_no_write_buf_available_ << std::endl;
  (*MyOutput_) << "-> invalid pattern after drift           = 0x " << std::hex << h20_discard_invalid_pattern_        << std::endl;
  (*MyOutput_) << "-> TMB rejected event                    = 0x " << std::hex << h20_discard_tmb_reject_             << std::endl;
  (*MyOutput_) << "-> timeout with no TMB trig pulse        = 0x " << std::hex << h20_timeout_no_tmb_trig_pulse_      << std::endl;
  (*MyOutput_) << "-> timeout with no mpc_frame_ff          = 0x " << std::hex << h20_timeout_no_mpc_frame_           << std::endl;
  (*MyOutput_) << "-> timeout with no mpc_response_ff       = 0x " << std::hex << h20_timeout_no_mpc_response_        << std::endl;
  //
  (*MyOutput_) << "Header 21:" << std::endl;  
  (*MyOutput_) << "-> setting of ALCT delay for match window = 0x " << std::hex << h21_match_trig_alct_delay_   << std::endl;
  (*MyOutput_) << "-> setting of match window width          = 0x " << std::hex << h21_match_trig_window_width_ << std::endl;
  (*MyOutput_) << "-> setting of MPC transmit delay          = 0x " << std::hex << h21_mpc_tx_delay_            << std::endl;
  //
  (*MyOutput_) << "Header 22:" << std::endl;  
  (*MyOutput_) << "-> RPCs connected to this TMB            = 0x " << std::hex << h22_rpc_exist_       << std::endl;
  (*MyOutput_) << "-> RPCs included in readout              = 0x " << std::hex << h22_rpc_list_        << std::endl;
  (*MyOutput_) << "-> Number of RPCs in readout             = 0x " << std::hex << h22_nrpc_            << std::endl;
  (*MyOutput_) << "-> RPC readout enabled                   = 0x " << std::hex << h22_rpc_read_enable_ << std::endl;
  (*MyOutput_) << "-> Number of layers hit on layer trigger = 0x " << std::hex << h22_nlayers_hit_     << std::endl;
  (*MyOutput_) << "-> Position of L1A in window             = 0x " << std::hex << h22_l1a_in_window_   << std::endl;
  //
  (*MyOutput_) << "Header 23:" << std::endl;  
  (*MyOutput_) << "-> Board status = 0x " << std::hex << h23_board_status_ << std::endl;
  //
  (*MyOutput_) << "Header 24:" << std::endl;  
  (*MyOutput_) << "-> seconds since last hard reset = 0x " << std::hex << h24_time_since_hard_reset_ << std::endl;
  //
  (*MyOutput_) << "Header 25:" << std::endl;  
  (*MyOutput_) << "-> Firmware version date code = 0x " << std::hex << h25_firmware_version_date_code_ << std::endl;
//
  return;
}
//
void TMB::decode() {
  //
  unsigned long int base_adr;
  //unsigned long int boot_adr;
  unsigned long int adr;
  unsigned long int wr_data;
  unsigned long int rd_data;

  //  static const unsigned long int tmb_global_slot     = 26;
  //  static const unsigned long int tmb_brcst_slot      = 27;
  //  static const unsigned long int tmb_boot_adr        = 0x070000;
  static const unsigned long int islot               = 6;

  //
  unsigned long int firmware_type;
  unsigned long int firmware_series;
  char *firmware_name;

  int rdscope;      //Boolean vars
  int scp_arm;
  int scp_readout;
  int disp_dump;

  unsigned long int clct0_vme;
  unsigned long int clct1_vme;
  unsigned long int clct0_msbs;
  unsigned long int clct1_msbs;

  unsigned long int clct0_vpf;
  unsigned long int clct0_pat;
  unsigned long int clct0_bend;
  unsigned long int clct0_key;
  unsigned long int clct0_cfeb;
  unsigned long int clct0_bxn;
  unsigned long int clct0_nhit;
  unsigned long int clct0_hsds;
  unsigned long int clct0_sync;
  unsigned long int clct0_bx0;

  unsigned long int clct1_vpf;
  unsigned long int clct1_pat;
  unsigned long int clct1_bend;
  unsigned long int clct1_key;
  unsigned long int clct1_cfeb;
  unsigned long int clct1_bxn;
  unsigned long int clct1_nhit;
  unsigned long int clct1_hsds;
  unsigned long int clct1_sync;
  unsigned long int clct1_bx0;

  unsigned long int dmb_wdcnt;
  //  unsigned long int dmb_adr;
  unsigned long int dmb_rdata;
  unsigned long int dmb_busy;

  char ians;
  unsigned long int i;
  int iloop;
  int nloop;
  //unsigned int kill_loop2 = 0;

  rd_data = 0; //temp
  ians = 'n'; //temp

  /*** Open VME Interface ***/


  //start(1) ;
  //boot_adr = base_adr|0x070000;

  pfile = fopen("tmb_scope.txt","w");

  //pfile2 = fopen("TMBOUT.dat","w");

  /*** Get firmware type code ***/

  adr = base_adr;
  tmb_vme(VME_READ,0x0,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  firmware_type = rd_data & 0xF;
  firmware_series = (rd_data >> 4) & 0xF;

  firmware_name = "TMB2001 type unknown ... beware";
  if((firmware_type == 0xC) && (firmware_series == 0xE)) firmware_name = "TMB2001E";

  printf(" TMB firmware %s\n Slot %02ld Adr %06lx\n",firmware_name,islot,base_adr);

  /*** Inject ALCT+CLCT then readout raw hits ***/

  int kill_loop=0;
  nloop = 10;

  //Single cycle or loop
  printf(" Loop? (y/n): ");
  scanf("%c",&ians);
  iloop = false;
  if((ians == 'y') || (ians == 'Y')) {
    iloop = true;
    printf(" How many events? (1-100): ");
    scanf("%d",&nloop);
  }
  //else kill_loop = nloop;

  //ians = 'n';
  //printf("Display cathode dump on screen? (y/n): ");
  //scanf("%c",&ians);
  //if((ians == 'y') || (ians == 'Y')) disp_dump = true;
  //else disp_dump = false;

  disp_dump = false ;

  rdscope = true;
  //
  //if(iloop) rdscope = false;
  //
  printf("\n");
  //
  while( ((kill_loop++) <= nloop) )  {
    //
    while(FmState() == 1 ) {
      printf("%c7", '\033');
      printf("%c[01;37m",'\033');
      printf("%c8", '\033');
      printf("Waiting to get out of StopTrigger\n");
      printf("%c8", '\033');
    }
    printf("%c[0m", '\033');
    //
    printf("Event Number: %d\n",kill_loop);
    //
    //Wait for PHOS4 to initialize
    /*
    unsigned long int  phos4_lock = 0;
    while(!((phos4_lock != 0) || ((kill_loop2++) == 10000))) {
      adr = vme_phos4sm_adr ;
      status = tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      phos4_lock = rd_data & 0x8000;
      if(phos4_lock == 0) printf("Waiting for PHOS4 lock");
    }
    */
    //
    //Clear DMB RAM write-address
    //
    adr = dmb_ram_adr ;
    wr_data = 0x2000;   //reset RAM write address
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    wr_data = 0x0000;   //unreset
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

    //Arm scope trigger
    scp_arm = true;
    scp_readout = false;
    if(rdscope) scope(scp_arm,scp_readout);


    //Get latched CLCT words
    adr = seq_clct0_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct0_vme = rd_data;

    adr = seq_clct1_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct1_vme = rd_data;

    adr = seq_clctm_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct0_msbs = rd_data & 0x001F;
    clct1_msbs = (rd_data >> 5) & 0x001F;

    clct0_vme = clct0_vme | (clct0_msbs << 16);
    clct1_vme = clct1_vme | (clct1_msbs << 16);

    //if(!iloop) {
    //if(kill_loop == 0) {

    fprintf(pfile,"\n\n*********************************************\n");
    fprintf(pfile,"* Event Number: %d                          *\n",kill_loop);
    fprintf(pfile,"*********************************************\n");
    fprintf(pfile,"clct0_vme = %6.6lx\n",clct0_vme);
    fprintf(pfile,"clct1_vme = %6.6lx\n",clct1_vme);
    fprintf(pfile,"clctm_vme = %6.6lx\n",rd_data);

    //Decompose readout CLCT
    clct0_vpf   = (clct0_vme >>  0) & 0x0001;   //Valid pattern flag
    clct0_nhit  = (clct0_vme >>  1) & 0x0007;   //Hits on pattern
    clct0_pat   = (clct0_vme >>  4) & 0x0007;   //Pattern number
    clct0_hsds  = (clct0_vme >>  7) & 0x0001;   //1=1/2-strip, 0=distrip
    clct0_bend  = (clct0_vme >>  8) & 0x0001;   //Bend direction
    clct0_key   = (clct0_vme >>  9) & 0x001F;   //First 1/2-strip ID number
    clct0_cfeb  = (clct0_vme >> 14) & 0x0007;   //First CFEB ID
    clct0_bxn   = (clct0_vme >> 17) & 0x0003;   //Bunch crossing number
    clct0_sync  = (clct0_vme >> 19) & 0x0001;   //Sync error
    clct0_bx0   = (clct0_vme >> 20) & 0x0001;   //Bx0

    clct1_vpf   = (clct1_vme >>  0) & 0x0001;   //Valid pattern flag
    clct1_nhit  = (clct1_vme >>  1) & 0x0007;   //Hits on pattern
    clct1_pat   = (clct1_vme >>  4) & 0x0007;   //Pattern number
    clct1_hsds  = (clct1_vme >>  7) & 0x0001;   //1=1/2-strip, 0=distrip
    clct1_bend  = (clct1_vme >>  8) & 0x0001;   //Bend direction
    clct1_key   = (clct1_vme >>  9) & 0x001F;   //First 1/2-strip ID number
    clct1_cfeb  = (clct1_vme >> 14) & 0x0007;   //First CFEB ID
    clct1_bxn   = (clct1_vme >> 17) & 0x0003;   //Bunch crossing number
    clct1_sync  = (clct1_vme >> 19) & 0x0001;   //Sync error
    clct1_bx0   = (clct1_vme >> 20) & 0x0001;   //Bx0

    //if(!iloop) {
    //if(kill_loop == 0) {

    fprintf(pfile,"CLCT0: vpf= %4ld nhit= %4ld pat= %4ld hsds= %4ld bend= %4ld key= %4ld cfeb= %4ld\n",
	    clct0_vpf,clct0_nhit,clct0_pat,clct0_hsds,clct0_bend,clct0_key,clct0_cfeb);
    
    fprintf(pfile,"CLCT1: vpf= %4ld nhit= %4ld pat= %4ld hsds= %4ld bend= %4ld key= %4ld cfeb= %4ld\n",
	    clct1_vpf,clct1_nhit,clct1_pat,clct1_hsds,clct1_bend,clct1_key,clct1_cfeb);
    //}
        
    
    /***** Readout raw hits *****/
    
    //Read back embedded scope data
    scp_arm = false;
    scp_readout = true;
    if(rdscope) scope(scp_arm,scp_readout);

    //Get DMB RAM word count and busy bit
    adr = dmb_wdcnt_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    dmb_wdcnt = rd_data & 0x0FFF;
    dmb_busy = (rd_data >> 14) & 0x0001;

    printf("   word count = %4ld\n",dmb_wdcnt);
    printf("   busy       = %4ld\n",dmb_busy);

    if((dmb_busy) || (dmb_wdcnt <= 0)) {
      if(dmb_busy)       printf("Can not read RAM: dmb reports busy\n");
      if(dmb_wdcnt <= 0) printf("Can not read RAM: dmb reports word count <=0\n");
    } else {
      //Write RAM read address to TMB
      for(i=0;i<dmb_wdcnt;i++) {
        adr = dmb_ram_adr ;
        wr_data = i & 0xFFFF;
        sndbuf[0] = (wr_data & 0xff00)>>8 ;
        sndbuf[1] = wr_data & 0x00ff ;
        tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

        //Read RAM data from TMB
        adr = dmb_rdata_adr ;
        tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
        rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
        dmb_rdata = rd_data;
        adr = dmb_wdcnt_adr ;
        tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read msbs
        rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
        dmb_rdata = dmb_rdata | ((rd_data << 2) & 0x30000);     //rdata msbs

        if(disp_dump) printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
        fprintf(pfile,"Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
      }

      //Clear RAM address for next event
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0000; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    }

  } //ends while(iloop) {

  /***** Exit: Close VME Interface *****/
  theController->end() ;
  //fclose(pfile);

  //fclose(pfile2);
  //return 0;

} //main
//
void TMB::init_alct(int choice)
{
 //start(1);
 if ( choice ==1 ){
   printf(" Closing ALCT-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x59;
 }
 if ( choice ==2 ){
   printf(" Opening ALCT-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x58;
 }
 tmb_vme(VME_WRITE,0x32,sndbuf,rcvbuf,NOW);
 if ( choice ==1 ){
   printf(" Closing CFEB-TMB Input \n") ;
   sndbuf[0]=0x7c;
   sndbuf[1]=0x00;
 }
 if ( choice ==2 ){
   printf(" Opening CFEB-TMB Input \n") ;
   sndbuf[0]=0x7c;
   sndbuf[1]=0x1f;
 }
 tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
 if ( choice ==1 ){
   printf(" Closing CCB-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x3b;
 }
 if ( choice ==2 ){
   printf(" Opening CCB-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x38;
 }
 tmb_vme(VME_WRITE,0x2a,sndbuf,rcvbuf,NOW);
 end() ;
}


void TMB::load_cscid()
{
  int i;
  if ( theSlot <=12) {
    i=(theSlot/2)&0x000f;
  } else {
    i=(theSlot/2-1)&0x000f;
  }
  //  printf(" I am in CSC Slot %d \n",theSlot);
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  //  printf(" read CSC %02x %02x from TMB\n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );

  sndbuf[0]=( (rcvbuf[0]&0x00fe) | ((i>>3)&0x1)) & 0x00ff;
  sndbuf[1]=( (rcvbuf[1]&0x001f) | ((i<<5)&0xe0)) & 0x00ff;
  //  printf(" write CSC ID %02x %02x to TMB\n",sndbuf[0]&0x00ff,sndbuf[1]&0x00ff);
  tmb_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,NOW);

  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  //  printf(" read %02x %02x CSC from TMB \n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );
}


void TMB::lvl1_delay(unsigned short int time)
{
  tmb_vme(VME_READ,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0]=  (rcvbuf[0]&0xff);
  sndbuf[1]=  (time&0x00ff);
  tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW);
}

void TMB::alct_vpf_delay(unsigned short int time)
{
  tmb_vme(VME_READ,tmbtim_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0]=  rcvbuf[0];
  sndbuf[1]=  (time&0xf) | (rcvbuf[1]&0xf0);
  tmb_vme(VME_WRITE,tmbtim_adr,sndbuf,rcvbuf,NOW);
}

void TMB::mpc_delay(unsigned short int time)
{
   // Read Trigger conf
   tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
   //
   printf("*** Inside.MPC delay %d \n", time);
   //
   printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
   sndbuf[0] = (rcvbuf[0] & 0xfe | (time & 0x8)>>3) & 0xff;
   sndbuf[1] = (rcvbuf[1] & 0x1f | (time & 0x7)<<5) & 0xff;
   printf("Setting address 0x86 to %x %x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
   tmb_vme(VME_WRITE,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Write Trigger conf
   //
}
//
int TMB::GetWordCount(){
  //
  tmb_vme(VME_READ,dmb_wdcnt_adr,sndbuf,rcvbuf,NOW);
  //
  return ( rcvbuf[1]&0xff | (rcvbuf[0]&0xf)>>8);
  //
}
//
void TMB::SetALCTPatternTrigger(){
  //
  tmb_vme(VME_READ, seq_trig_en_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =   rcvbuf[0]&0xff ;
  sndbuf[1] =  (rcvbuf[1]&0xff) | (0x2) ;
  tmb_vme(VME_WRITE, seq_trig_en_adr, sndbuf,rcvbuf,NOW);
  //
}
//
void TMB::SetCLCTPatternTrigger(){
  //
  tmb_vme(VME_READ, seq_trig_en_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] =   rcvbuf[0]&0xff ;
  sndbuf[1] =  (rcvbuf[1]&0xff) | (0x1) ;
  tmb_vme(VME_WRITE, seq_trig_en_adr, sndbuf,rcvbuf,NOW);
  //
}
//
int TMB::GetALCTWordCount(){
  //
  const int maximum_number_of_allowed_busies = 20;
  //
  if (!CheckAlctFIFOBusy(maximum_number_of_allowed_busies)) {
    std::cout << "TMB:  At beginning of GetALCTWordCount(),  ALCT FIFO Busy " << maximum_number_of_allowed_busies << " times... " << std::endl;
    std::cout << "..... aborting" << std::endl;
    return 0;
  }
  //
  tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);
  //
  return ( (rcvbuf[1]>>2)&0x3f | (rcvbuf[0]&0x1f)>>6);
  //
}
//
void TMB::optimize()
{
  //
  char rcvbuf_read[2];
  char rcvbuf_sav[2];
  char rcvbuf_sav2[2];
  //
  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  int ncounts; // ncounts_sum ;
  //
  int i,j,k,l,m ;
  FILE *pfile3;
  //
  pfile3 = fopen("Optimize.txt","w");
  //
  //start(1) ;
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0xac,sndbuf,rcvbuf,NOW);
  //
  rcvbuf_sav2[0] = rcvbuf[0] ;
  rcvbuf_sav2[1] = rcvbuf[1] ;
  //
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0x70,sndbuf,rcvbuf,NOW);
  //
  rcvbuf_sav[0] = rcvbuf[0] ;
  rcvbuf_sav[1] = rcvbuf[1] ;
  printf("rcvbuf 0: %x  1: %x\n",rcvbuf_sav[0],rcvbuf_sav[1]);
  //
  //for (i=1;i<9;i++) {
  i = 5 ;                                                                                   
  sndbuf[0] = rcvbuf[0]  ;
  if ( i>5 ) sndbuf[1] = (rcvbuf[1]&0xf0) | (i-4)&0xf  ;
  else sndbuf[1] = (rcvbuf[1]&0xf0) | 0x1  ;
  tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf_read,1);
  //for (j=1;j<7;j++) {
  //for (k=1;k<7;k++) {
  //for (l=0;l<4;l++) {
  j=4;
  k=4;                                                                                                
  l=3;
  ncounts = 0;
  sndbuf[0] = (rcvbuf[0]&0x83) | (i&0xf) | ((j&0x7)<<4) ;
  sndbuf[1] = (rcvbuf[1]&0x80) | ((k&0x7)<<2) | ((l&3)<<5) ;
  //tmb_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW);
  // Loop over events
  for (m=0; m<20;  m++) {
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0x78,sndbuf,rcvbuf_read,1);
  nhits     = (rcvbuf_read[1] & 0x0e)>>1 ;
  pattern   = (rcvbuf_read[1] & 0x70)>>4 ;
  striptype = (rcvbuf_read[1] & 0x80)>>7 ;
  cfebid    = (rcvbuf_read[0] & 0xc0)>>6 ;
  if ( nhits > 4 && striptype == 1 ) ncounts++ ;
  }
  printf ( " i %d j %d k %d l %d ncounts %d \n",i,j,k,l,ncounts ) ;
  fprintf(pfile3,"i %d j %d k %d l %d ncounts %d \n",i,j,k,l,ncounts ) ;
  //}
  //}
    //}
  //}

  sndbuf[0] = rcvbuf_sav[0] ;
  sndbuf[1] = rcvbuf_sav[1] ;
  tmb_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW);
  sndbuf[0] = rcvbuf_sav2[0] ;
  sndbuf[1] = rcvbuf_sav2[1] ;
  tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf,NOW);
  end() ;
  fclose(pfile3);
}

void TMB::ForceScopeTrigger(){
  //
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0] | 0x2 ;
  sndbuf[1]=rcvbuf[1];
  tmb_vme(VME_WRITE,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0] ;
  sndbuf[1]=rcvbuf[1];
  tmb_vme(VME_WRITE,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::read_delays()
{
  printf(" read delay registers \n");
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,NOW);
  printf(" delay register is: %02x%02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
}


void TMB::reset() {
  sndbuf[0]=0x41;
  sndbuf[1]=0x90;
  tmb_vme(VME_READ,0x70000,sndbuf,rcvbuf,NOW);
  printf("Bootstrap %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff ) ;
}
//
/*
void TMB::scan_rx_clock() 
{
  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  long int i;
  int j, time_setting ;
  long int ncounts,ncounts_sum ;
  long int cfeb[5][25];
  int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  char rcvbuf_old2[2];
  int time_set;
  int word_count[25];
  int pattern_quality[25];
  int alct_counts[25];

  goto ALCT;

  // CFEB part

  pfile = fopen("tmb_cfeb_scan.txt","w") ;

  //start(1) ;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x42,sndbuf,rcvbuf_old,1);
  printf("read %x%x \n",rcvbuf_old[0],rcvbuf_old[1]) ;
  for (cfeb_loop=0;cfeb_loop<5;cfeb_loop++) {
    printf("-------------------------------------------\n") ;
    printf("             CFEB %d \n",cfeb_loop) ;
    printf("-------------------------------------------\n") ;
    printf("\n") ;
    fprintf(pfile,"\n") ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"             CFEB %d \n",cfeb_loop) ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"\n") ;    
    time_setting = 0;
    ncounts_sum  = 0 ;
    printf("Masking cfeb %d \n",cfeb_loop) ;
    sndbuf[0] = rcvbuf_old[0];
    sndbuf[1] = (rcvbuf_old[1]&0xe0) | (1 << cfeb_loop ) ;
    printf("Setting %x%x \n",sndbuf[0],sndbuf[1]) ;
    tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
    for (j=0; j<25; j++ ) {
      tmb_clk_delays(j,cfeb_loop) ;
      ncounts = 0 ;
      for (i=0; i<100;  i++) {
	sndbuf[0] = 0x0 ;
	sndbuf[1] = 0x0 ;  
	tmb_vme(VME_READ,0x78,sndbuf,rcvbuf,NOW);
	nhits     = (rcvbuf[1] & 0x0e)>>1 ;
	pattern   = (rcvbuf[1] & 0x70)>>4 ;
	striptype = (rcvbuf[1] & 0x80)>>7 ;
	cfebid    = (rcvbuf[0] & 0xc0)>>6 ;
	if ( nhits > 4 && striptype == 1 ) ncounts++ ;
      }
      time_setting = time_setting + j*ncounts ;
      ncounts_sum  = ncounts_sum + ncounts ; 
      printf ( " time %d ncounts %d %d %d \n ",j,ncounts,time_setting,ncounts_sum ) ;
      cfeb[cfeb_loop][j] = ncounts ;
    }

    printf(" %d %d \n",time_setting,ncounts_sum) ;
    
    time_set = time_setting/ncounts_sum ;

    printf ( " Setting time to %d \n ",time_set) ;
    fprintf ( pfile, " Setting time to %d \n ",time_set) ;
    tmb_clk_delays(time_set,cfeb_loop) ;

    for(j=0;j<25;j++)
      printf("CFEB: %d, timebin %d, counts %ld\n",cfeb_loop,j,cfeb[cfeb_loop][j]);

    //
    //for (j=0;j<50;j++) {
    //for (i=0;i<25;i++) {
    //if ( cfeb[cfeb_loop][i] > 50-j ) {
    //  printf ("*") ;
    //  fprintf (pfile,"*") ;
    //} else {
    //  printf ("_") ;
    //  fprintf (pfile,"_") ;
    //}
    //}
    //printf("\n") ;
    //fprintf(pfile,"\n") ;
    //}
    //

  }
  //
  // Setting old values...
  //
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = rcvbuf_old[1];
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
  theController->end() ;
  fclose(pfile) ;

  //ALCT part

  return ;

 ALCT:

  start(1) ;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  //tmb_vme(VME_READ,0x2A,sndbuf,rcvbuf_old,1);
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x1 ;  //Disconnect CCB
  //tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  //tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<25;j++){
    tmb_clk_delays(j,5) ;
    sndbuf[0] = rcvbuf_old2[0];
    sndbuf[1] = (rcvbuf_old2[1]&0xfe) | 0x1 ;
    tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
    sndbuf[0] = rcvbuf_old2[0];
    sndbuf[1] = (rcvbuf_old2[1]&0xfe) | 0x0 ;
    tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
    sndbuf[0] = 0;
    sndbuf[1] = 0;
    tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW); 
    word_count[j] = ((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc) ;
    printf( " Setting %d word_count %d \n ",j,word_count[j] ) ;
    alct_counts[j] = 0 ;
    for (i=0;i<100;i++) {
      sndbuf[0] = 0;
      sndbuf[1] = 0;
      tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW); 
      pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
      if ( pattern_quality[j] == 3 ) (alct_counts[j])++ ;
    }
  }
  sndbuf[0] = rcvbuf_old[0] ;
  sndbuf[1] = rcvbuf_old[1] ;  //Old CCB status
  //tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  for (j=0;j<25;j++) printf(" %d ",word_count[j]);
  printf("\n") ;
  for (j=0;j<25;j++) printf(" %d  ",alct_counts[j]);
  printf("\n") ;
  endDevice() ;

}

*/
/*
void TMB::activecfeb()
{

  tmb_vme(VME_READ,0x68,sndbuf,rcvbuf,NOW);
//  printf("\n   current TMB adr 0x68 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  sndbuf[1]=rcvbuf[1];
  if((rcvbuf[0]&0x02)==0){
    sndbuf[0]=(rcvbuf[0]|0x02);
    printf("   set to ON!\n");
  }
  else{
    sndbuf[0]=(rcvbuf[0]&0xfd);
    printf("   set to OFF!\n");
  }

  printf("     send to TMB adr 0x68: 0x%02x%02x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  tmb_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW);


  tmb_vme(VME_READ,0x72,sndbuf,rcvbuf,NOW);
  printf("\n   current TMB adr 0x72 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
// [12:8] = #FIFO time bins before pretrigger
// [7:3] =  #FIFO time bins to read out
// [2:0] =  FIFO mode:
//           0=NoDump/1=FullDump/2=LocalDump/3=NoDumpShortHdr/4=NoDumpNoHdr

}
*/
void TMB::DumpAddress(int address){
  //
  tmb_vme(VME_READ,address,sndbuf,rcvbuf,NOW);
  //
  printf(" TMB.Dump %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
}


void TMB::toggle_l1req()
{

  tmb_vme(VME_READ,0x2c,sndbuf,rcvbuf,NOW);
//  printf("\n   current TMB adr 0x2c setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  sndbuf[0]=rcvbuf[0];
  if((rcvbuf[1]&0x07)==0){
    sndbuf[1]=(rcvbuf[1]|0x04);
    printf("   set TMB sequencer L1req to ON!\n");
  }
  else{
    sndbuf[1]=(rcvbuf[1]&0xf8);
    printf("   set all TMB L1req to OFF!\n");
  }

  printf("     send to TMB adr 0x2c: 0x%02x%02x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  tmb_vme(VME_WRITE,0x2c,sndbuf,rcvbuf,NOW);


  tmb_vme(VME_READ,0x72,sndbuf,rcvbuf,NOW);
  printf("\n   current TMB adr 0x72 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
// [12:8] = #FIFO time bins before pretrigger
// [7:3] =  #FIFO time bins to read out
// [2:0] =  FIFO mode:
//           0=NoDump/1=FullDump/2=LocalDump/3=NoDumpShortHdr/4=NoDumpNoHdr

}

/*
void TMB::trgmode_bprsq_dmb()
{
  sndbuf[0]=0x00;
  sndbuf[1]=0x50;
  tmb_vme(VME_WRITE,0x32,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x7c;
  sndbuf[1]=0x1f;
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x00;
  sndbuf[1]=0x0f;  //0b
  tmb_vme(VME_WRITE,0x86,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x00;
  sndbuf[1]=0x08; //b9
  tmb_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW); 
}
*/



void TMB::firmwareVersion()
{
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,vme_idreg0_adr,sndbuf,rcvbuf,NOW);
  printf(" TMB Version %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,vme_idreg1_adr,sndbuf,rcvbuf,NOW);
  printf(" TMB month/date %02x/%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,vme_idreg2_adr,sndbuf,rcvbuf,NOW);
  printf(" TMB year %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,vme_idreg3_adr,sndbuf,rcvbuf,NOW);
  printf(" TMB revision %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
}


void TMB::setLogicAnalyzerToDataStream(bool yesorno) {
  //Enable/Disable Logic Analyzer in data stream
  sndbuf[0]=0x00;
  sndbuf[1] = yesorno ? 0x38 : 0x18;
  tmb_vme(VME_WRITE,scp_ctrl_adr,sndbuf,rcvbuf,NOW); // Scope Readout
}



void TMB::tmb_vme(char fcn, char vme,
                  const char *snd,char *rcv, int wrt) {
  OkVmeWrite(vme);  
  start(1);
  do_vme(fcn, vme, snd, rcv, wrt);
}


void TMB::start() {
  //
  //(*MyOutput_) << "starting to talk to TMB, device " << ucla_ldev << std::endl;
  //
  // send the first signal
  SetupJtag();
  VMEModule::start();
  theController->initDevice(ucla_ldev);
  theController->goToScanLevel();
}


void TMB::start(int idev,int JtagSource) {
  //
  VMEModule::SetJtagSource(JtagSource);
  //
  if(idev != ucla_ldev) {
    endDevice();
    ucla_ldev = idev;
  }
  //theController->start(this);
  start();
  //
}


void TMB::end() {
#ifdef debugV
(*MyOutput_) << "Ending TMB device " << ucla_ldev << std::endl;
#endif
char rcvx[2];
char sndx[2];
  if(ucla_ldev==1){
    tmb_vme(0xff,0x00,sndx,rcvx,1);
    theController->release_plev();
  }else{
    theController->send_last();
  }
  VMEModule::end();
}


int TMB::tmb_get_id(struct tmb_id_regs* tmb_id)
{
  unsigned short int value;

  tmb_get_reg(vme_idreg0_adr, &value);
  tmb_id->fw_type = value & 0x000f;
  tmb_id->fw_version = (value >> 4) & 0x000f;
  tmb_id->brd_geo_addr = (value >> 8) & 0x000f;
  
  tmb_get_reg(vme_idreg1_adr, &value);
  tmb_id->fw_day = value & 0x00ff;
  tmb_id->fw_month = (value >> 8) & 0x00ff;

  tmb_get_reg(vme_idreg2_adr, &value);
  tmb_id->fw_year = value & 0xffff;

  tmb_get_reg(vme_idreg3_adr, &value);
  tmb_id->fpga_type = value & 0xffff;

  return 0;
}

int TMB::tmb_set_jtag_src(unsigned short int jtag_src)
{ 
  unsigned short int value;

  tmb_get_boot_reg(&value);
  //printf("0x%04x\n", value);
  value = (value & ~TMB_JTAG_SRC) | ((jtag_src & 0x01) << 7); // JTAG Source selection bit at position 7
  //printf("0x%04x\n", value);
  // tmb_set_boot_reg(value);

  return 0;
} 

int TMB::tmb_get_jtag_src(unsigned short int* jtag_src)
{
  unsigned short int value = 0;

  tmb_get_boot_reg(&value);
  *jtag_src = ((value & TMB_JTAG_SRC) > 0) ? JTAG_HARD_SRC : JTAG_SOFT_SRC; 
  
  return 0;
}

int TMB::tmb_set_jtag_chain(unsigned int jchain)
{
   unsigned short int chain;
   unsigned short int jtag_src = 0;
   
   tmb_get_jtag_src(&jtag_src);

   if (jtag_src == JTAG_SOFT_SRC)
     {
	tmb_get_reg(vme_usr_jtag_adr, &chain);
	chain = (chain & 0xff0f) | ((jchain & 0x0f) << 4);
	tmb_set_reg(vme_usr_jtag_adr, chain);
     } 
   else
     {
	tmb_get_boot_reg(&chain);
	chain = (chain & 0xff87) | ( (jchain & 0x0f) << 3);
        tmb_set_boot_reg(chain);
     }

   return 0;
}
//
int TMB::tmb_set_reg(unsigned int vmereg, unsigned short int value )
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=value & 0xff;
   sndbuf[1]=(value >> 8) & 0xff;
   tmb_vme(VME_WRITE, vmereg, sndbuf, rcvbuf, NOW );
   return 0;	
}
//
int TMB::tmb_get_reg(unsigned int vmereg, unsigned short int* value )
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=0;
   sndbuf[1]=0;
   tmb_vme(VME_READ, vmereg, sndbuf, rcvbuf, NOW );
   *value = (rcvbuf[1]&0xff) | (rcvbuf[0]<<8);
   return 0;
}  
//
int TMB::tmb_vme_reg(unsigned int vmereg, unsigned short int* value)
{
   tmb_set_reg(vmereg, *value);
   tmb_get_reg(vmereg, value);
   return 0;
}
//
int TMB::tmb_jtag_io(unsigned char tms, unsigned char tdi, unsigned char* tdo)
{
  unsigned short int jtag_src = 0;
  unsigned short int value = 0;
  tmb_get_jtag_src(&jtag_src);
  if (jtag_src == JTAG_SOFT_SRC)
    {
	tmb_get_reg(vme_usr_jtag_adr, &value);

        if (tdi > 0) 
          { value |= SOFT_TDI;}
	else 
	  { value &= ~SOFT_TDI & 0xffff;}; 

        if (tms > 0)
	  { value |= SOFT_TMS;}
	else 
	  { value &= ~SOFT_TMS & 0xffff;};

	tmb_set_reg(vme_usr_jtag_adr, value);

	value |= SOFT_TCK;
        tmb_set_reg(vme_usr_jtag_adr, value);

	tmb_get_reg(vme_usr_jtag_adr, &value);
	*tdo = ((value & SOFT_TDO) > 0) ? 1 : 0;
	
	value &= ~SOFT_TCK & 0xffff;
        tmb_set_reg(vme_usr_jtag_adr, value);
    }
  else
    {
        tmb_get_boot_reg(&value);
	if (tdi > 0) 
          { value |= HARD_TDI;}
        else 
          { value &= ~HARD_TDI & 0xffff;} 

        if (tms > 0)
          { value |= HARD_TMS;}
        else 
          { value &= ~HARD_TMS & 0xffff;}

        tmb_set_boot_reg(value);

        value |= HARD_TCK;
        tmb_set_boot_reg(value);

        tmb_get_boot_reg(&value);
        *tdo = ((value & HARD_TDO) > 0) ? 1 : 0;
        
        value &= ~HARD_TCK & 0xffff;
        tmb_set_boot_reg(value);
    };
  return 0;
}
//
int TMB::tmb_get_boot_reg(unsigned short int* value)
{
  //char sndbuf[2];
  //char rcvbuf[2];
  //
  tmb_vme(VME_READ | VME_BOOT_REG, 0x70000, sndbuf, rcvbuf, NOW ); // Send read request
  //
  //tmb_vme(VME_READ, 4, sndbuf, rcvbuf, NOW );
  //tmb_vme(VME_READ, 4, sndbuf, rcvbuf, NOW );
  //tmb_vme(VME_READ, 4, sndbuf, rcvbuf, NOW );
  //
  *value = (rcvbuf[1]&0xff) | (rcvbuf[0]<<8);
  //printf("get_boot.%02x %02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
  DecodeBootRegister_(*value);
  //
  return 0;
  //
}
//
int TMB::tmb_set_boot_reg(unsigned short int value)
{
  //char sndbuf[2];
  //char rcvbuf[2];
  //
  sndbuf[0]=(value >> 8) & 0xff;
  sndbuf[1]=value & 0xff;
  //
  //printf(" Here1. %x %x \n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  //
  //tmb_vme(VME_WRITE, 0x16,sndbuf,rcvbuf, NOW);
  //
  //tmb_vme(VME_READ,  0x16,sndbuf,rcvbuf, NOW);
  //
  //sndbuf[0]=value & 0xff;
  //sndbuf[1]=(value >> 8) & 0xff;
  //
  //printf(" Here2. %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
  //printf("\n");
  //
  tmb_vme(VME_WRITE | VME_BOOT_REG, 0, sndbuf, rcvbuf, NOW );
  //
  return 0;   
}
//
int TMB::tmb_hard_reset_alct_fpga()
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  value &= ~ALCT_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  value |= ALCT_HARD_RESET;
  tmb_set_boot_reg(value);
  value &= ~ALCT_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  return 0;
}
//
int TMB::tmb_hard_reset_tmb_fpga()
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  value &= ~TMB_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  value |= TMB_HARD_RESET;
  tmb_set_boot_reg(value);
  value &= ~TMB_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  return 0;
}
//
int TMB::tmb_enable_alct_hard_reset(int flag_enable)
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  if(flag_enable>0)
    { value |= TMB_ENABLE_ALCT_RESET;}
  else 
    { value &= ~TMB_ENABLE_ALCT_RESET & 0xffff; }
  tmb_set_boot_reg(value);
  return 0;
}
//
int TMB::tmb_enable_vme_commands(int flag_enable)
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  if(flag_enable>0) 
    { value |= TMB_ENABLE_VME;}
  else
    { value &= ~TMB_ENABLE_VME & 0xffff;} 
  tmb_set_boot_reg(value);
  return 0;
}
//
std::ostream & operator<<(std::ostream & os, TMB & tmb) {

  os << std::dec << "TMB: crate " << tmb.theCrate_
     << " slot " << tmb.theSlot << std::endl
     << std::hex 
     << "  cfeb delays (hex) " << tmb.cfeb0delay_ << " " 
     << tmb.cfeb1delay_ << " " << tmb.cfeb2delay_ << " " 
     << tmb.cfeb3delay_ << " " << tmb.cfeb4delay_ << std::endl
     << " rx, tx clock delays " << tmb.alct_rx_clock_delay_ 
     << " " << tmb.alct_tx_clock_delay_ << std::endl
     << "l1a window size " << tmb.l1a_window_size_ << std::endl
     << "l1a delay " << tmb.l1adelay_ << std::endl
     << "match window size " << tmb.alct_match_window_size_ << std::endl
     << "alct vpf delay " << tmb.alct_vpf_delay_  << std::endl
     << "mpc delay  " << tmb.mpc_delay_ << std::endl
     << "ALCT_input " << tmb.ALCT_input_ << std::endl
     << "rpc_exists " << tmb.rpc_exists_ << std::endl
     << "fifo_mode  " << tmb.fifo_mode_ << std::endl
     << "fifo_tbins " << tmb.fifo_tbins_ << std::endl
     << "fifo_pretrig " << tmb.fifo_pretrig_ << std::endl
     << "alct_clear " << tmb.alct_clear_ << std::endl
     << "mpc_tx_delay " << tmb.mpc_tx_delay_ << std::endl
     << "l1a_offset " << tmb.l1a_offset_ << std::endl
     << "disableCLCTInputs " << tmb.disableCLCTInputs_ << std::endl
     << "enableCLCTInputs " << tmb.enableCLCTInputs_ << std::endl
     << "alctController_ " << tmb.alctController_ << std::endl
     << "rat_ " << tmb.rat_ << std::endl
     << "bxn_offset_ " << tmb.bxn_offset_ << std::endl
     << "trgmode_ " << tmb.trgmode_ << std::endl
     << "rpc_bxn_offset_ " << tmb.rpc_bxn_offset_ << std::endl
     << "shift_rpc_ " << tmb.shift_rpc_ << std::endl
     << "request_l1a_ " << tmb.request_l1a_ << std::endl
     << "hs_pretrig_thresh_ " << tmb.hs_pretrig_thresh_ << std::endl
    //     << "ds_pretrig_thresh_ " << tmb.ds_pretrig_thresh_ << std::endl
     << "min_hits_pattern_ " << tmb.min_hits_pattern_ << std::endl
     << "dmb_tx_delay_ " << tmb.dmb_tx_delay_ << std::endl
     << "rat_tmb_delay_ " << tmb.rat_tmb_delay_ << std::endl
     << "rpc0_rat_delay_ " << tmb.rpc0_rat_delay_ << std::endl
     << "mpc_phase_ " << tmb.mpc_phase_ << std::endl
     << std::dec << std::endl;
  return os;
}

/*
void TMB::tmb_PHOS4_alct(int time) 
{
  ////fg a more elegant solution is on its way
  int maxTimeBins(25);
  //
  if (version_=="2004")
    maxTimeBins=13;

  //int nhits;
  //int pattern ;
  //int striptype ;
  //int cfebid ;
  //long int i;
  int j,k; //time_setting ;
  //long int ncounts,ncounts_sum ;
  //long int cfeb[5][maxTimeBins];
  //int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  char rcvbuf_old2[2];
  //int time_set;
  int word_count1[maxTimeBins][maxTimeBins];
  int word_count2[maxTimeBins][maxTimeBins];
  int word_count3[maxTimeBins][maxTimeBins];
  int word_count4[maxTimeBins][maxTimeBins];
  int envelope[maxTimeBins][maxTimeBins];
  
  //int pattern_quality[maxTimeBins];
  int alct_counts[maxTimeBins][maxTimeBins];
  start(1);
  //ALCT part
  (*MyOutput_) << "first call in PHOs4 alct " << std::endl;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x2A,sndbuf,rcvbuf_old,1);
  (*MyOutput_) << "write in PHOS4" << std::endl;
  //
  while(FmState() == 1 ) {
    printf("%c7", '\033');
    printf("%c[01;37m",'\033');
    printf("%c8", '\033');
    printf("Waiting to get out of StopTrigger\n");
    printf("%c8", '\033');
  }
  printf("%c[0m", '\033');
  //     
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x1 ;  //Disconnect CCB
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  (*MyOutput_) << "going well " << std::endl;  
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       //
       while(FmState() == 1 ) {
	 printf("%c7", '\033');
	 printf("%c[01;37m",'\033');
	 printf("%c8", '\033');
	 printf("Waiting to get out of StopTrigger\n");
	 printf("%c8", '\033');
       }
       printf("%c[0m", '\033');
       //
	tmb_clk_delays(k,5) ;
	tmb_clk_delays(j,6) ;
	sndbuf[0] = 0;
	sndbuf[1] = 0x1 ;
	tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
	sndbuf[0] = 0;
	sndbuf[1] = 0x0 ;
	tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
	sndbuf[0] = 0;
	sndbuf[1] = 0;
	tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW); 
	word_count1[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
	sndbuf[0] = 0;
	sndbuf[1] = 0;
	//sleep(time); //fg wait <time> to collect significant statistics (not relevant)
	tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
	word_count2[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
	
	printf( " Setting %d word_count %d \n ",j,word_count1[j][k]-word_count2[j][k] ) ;
	alct_counts[j][k] = 0 ;
	//
	//for (i=0;i<100;i++) {
	//sndbuf[0] = 0;	
	//sndbuf[1] = 0;
	//tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW); 
	//pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
	//if ( pattern_quality[j] == 3 ) (alct_counts[j][k])++ ;
	//}
	//
     }
  }
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x0 ;  //Connect CCB
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       //
       while(FmState() == 1 ) {
	 printf("%c7", '\033');
	 printf("%c[01;37m",'\033');
	 printf("%c8", '\033');
	 printf("Waiting to get out of StopTrigger\n");
	 printf("%c8", '\033');
       }
       printf("%c[0m", '\033');
       //
       tmb_clk_delays(k,5) ;
       tmb_clk_delays(j,6) ;
       sndbuf[0] = 0;
       sndbuf[1] = 0x1 ;
       tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
       sndbuf[0] = 0;
       sndbuf[1] = 0x0 ;
       tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
       sndbuf[0] = 0;
       sndbuf[1] = 0;
       tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
       word_count3[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
       sndbuf[0] = 0;
       sndbuf[1] = 0;
       ::sleep(time); //fg wait <time> to collect significant statistics
       tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
       word_count4[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
       //       
	printf( " Setting %d word_count %d \n ",j,word_count3[j][k]-word_count4[j][k] ) ;
	alct_counts[j][k] = 0 ;
	//
	//for (i=0;i<100;i++) {
	//sndbuf[0] = 0;
	//sndbuf[1] = 0;
	//tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW);
	//pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
	//if ( pattern_quality[j] == 3 ) (alct_counts[j][k])++ ;
	//}
	//
     }
  }
//
  pfile = fopen("tmb_alct.txt","w") ;
  //
  sndbuf[0] = rcvbuf_old[0] ;
  sndbuf[1] = rcvbuf_old[1] ;  //Old CCB status
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	printf(" %04d ",abs(word_count1[j][k]-word_count2[j][k]));
	fprintf(pfile," %04d ",abs(word_count1[j][k]-word_count2[j][k]));
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
//
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	printf(" %04d ",abs(word_count3[j][k]-word_count4[j][k]));
	fprintf(pfile," %04d ",abs(word_count3[j][k]-word_count4[j][k]));
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
  //
  printf(" Envelope \n") ;
  printf("\n");
  printf("|-----------------> higher bits \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("\\/ \n") ;
  printf("lower bits \n") ;
  printf("\n");
  fprintf(pfile," Envelope \n") ;
  fprintf(pfile,"\n");
  fprintf(pfile,"|-----------------> higher bits \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"\\/ \n") ;
  fprintf(pfile,"lower bits \n") ;
  fprintf(pfile,"\n");
//
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	if ( word_count1[j][k] - word_count2[j][k] != 0 ) envelope[j][k] = 0 ;
	if ( word_count1[j][k] - word_count2[j][k] == 0 ) envelope[j][k] = abs(word_count3[j][k]-word_count4[j][k]) ;
	printf(" %04d ",envelope[j][k]);
	fprintf(pfile," %04d ",envelope[j][k]);
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
//   
  printf("\n") ;
  fclose(pfile);
//
}
*/

/*
void TMB::tmb_PHOS4_cfeb() 
{
  //fg a more elegant solution is on its way
  int maxTimeBins(25);
  if (version_=="2004")
    maxTimeBins=13;

  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  long int i;
  int j, time_setting ; //k
  long int ncounts,ncounts_sum ;
  long int cfeb[5][maxTimeBins];
  int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  //char rcvbuf_old2[2];
  int time_set;
  //int word_count1[maxTimeBins][maxTimeBins];
  //int word_count2[maxTimeBins][maxTimeBins];
  //int word_count3[maxTimeBins][maxTimeBins];
  //int word_count4[maxTimeBins][maxTimeBins];
  //int envelope[maxTimeBins][maxTimeBins];

  //int pattern_quality[maxTimeBins];
  //int alct_counts[maxTimeBins][maxTimeBins];


  // CFEB part

  pfile = fopen("tmb_cfeb_scan.txt","w") ;
  //start(1);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x42,sndbuf,rcvbuf_old,1);
  printf("read %x%x \n",rcvbuf_old[0],rcvbuf_old[1]) ;
  for (cfeb_loop=0;cfeb_loop<1;cfeb_loop++) {
    printf("-------------------------------------------\n") ;
    printf("             CFEB %d \n",cfeb_loop) ;
    printf("-------------------------------------------\n") ;
    printf("\n") ;
    fprintf(pfile,"\n") ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"             CFEB %d \n",cfeb_loop) ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"\n") ;    
    time_setting = 0;
    ncounts_sum  = 0 ;
    printf("Masking cfeb %d \n",cfeb_loop) ;
    sndbuf[0] = rcvbuf_old[0];
    sndbuf[1] = (rcvbuf_old[1]&0xe0) | (1 << cfeb_loop ) ;
    printf("Setting %x%x \n",sndbuf[0],sndbuf[1]) ;
    tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
    for (j=0; j<maxTimeBins; j++ ) {
      tmb_clk_delays(j,cfeb_loop) ;
      ncounts = 0 ;
      for (i=0; i<10000;  i++) {
	sndbuf[0] = 0x0 ;
	sndbuf[1] = 0x0 ;  
	tmb_vme(VME_READ,0x78,sndbuf,rcvbuf,NOW);
	nhits     = (rcvbuf[1] & 0x0e)>>1 ;
	pattern   = (rcvbuf[1] & 0x70)>>4 ;
	striptype = (rcvbuf[1] & 0x80)>>7 ;
	cfebid    = (rcvbuf[0] & 0xc0)>>6 ;
	if ( nhits > 4 && striptype == 1 ) ncounts++ ;
      }
      time_setting = time_setting + j*ncounts ;
      ncounts_sum  = ncounts_sum + ncounts ; 
      printf ( " time %d ncounts %d %d %d \n ",j,ncounts,time_setting,ncounts_sum ) ;
      cfeb[cfeb_loop][j] = ncounts ;
    }

    printf(" %d %d \n",time_setting,ncounts_sum) ;
    
    time_set = time_setting/ncounts_sum ;

    printf ( " Setting time to %d \n ",time_set) ;
    fprintf ( pfile, " Setting time to %d \n ",time_set) ;
    tmb_clk_delays(time_set,cfeb_loop) ;

    for(j=0;j<maxTimeBins;j++)
      fprintf(pfile,"CFEB: %d, timebin %d, counts %ld\n",cfeb_loop,j,cfeb[cfeb_loop][j]);

    //
    //for (j=0;j<50;j++) {
    //for (i=0;i<25;i++) {
    //if ( cfeb[cfeb_loop][i] > 50-j ) {
    //  printf ("*") ;
    //  fprintf (pfile,"*") ;
    //} else {
    //  printf ("_") ;
    //  fprintf (pfile,"_") ;
    //}
    //}
    //printf("\n") ;
    //fprintf(pfile,"\n") ;
    //}
    //

  }
  //
  // Setting old values...
  //
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = rcvbuf_old[1];
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
  fclose(pfile) ;


  return ;

}
*/

//choice = 1 CLCT
//choice = 2 ALCT
//choice = 3 SCINT
//choice = 4 DMB
//choice = 5 CLCT+ALCT

//cable = 1 blue
//cable = 2 black

void TMB::trgmode(int choice)
{
  //int ierr;

  // Read address back
  tmb_vme(VME_READ, rpc_cfg_adr ,sndbuf,rcvbuf, NOW); 
  //  sndbuf[0]=rcvbuf[0];
  //  sndbuf[1]=rcvbuf[1] & 0xf0 | rpc_exists_ & 0x0f ;
  sndbuf[0]=rcvbuf[0] & 0xfe | ( (rpc_bxn_offset_ >> 3) & 0x1);
  sndbuf[1]=rcvbuf[1] & 0x10 | ( (rpc_bxn_offset_ & 0x07) << 5 ) | rpc_exists_ & 0x0f ;
  // Change address
  tmb_vme(VME_WRITE, rpc_cfg_adr ,sndbuf,rcvbuf, NOW); 

  // Read address back
  tmb_vme(VME_READ, vme_ratctrl_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=(rcvbuf[1] & 0xfd) | ((shift_rpc_ & 0x1) << 1);
  // Change address
  tmb_vme(VME_WRITE, vme_ratctrl_adr ,sndbuf,rcvbuf, NOW); 

  // Read address back
  tmb_vme(VME_READ, vme_loopbk_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1] & 0xfb | (ALCT_input_ << 2) ;
  //  printf(" ALCT_input %d \n",ALCT_input_);
  //  printf(" Setting ALCT input to %x %x \n",sndbuf[0],sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE, vme_loopbk_adr ,sndbuf,rcvbuf, NOW); 

  // Read address back
  tmb_vme(VME_READ, ccb_trig_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  //  sndbuf[1]=0;
  sndbuf[1]=(rcvbuf[1] & 0xfb) | ((request_l1a_ & 0x1) << 2);
  //  printf(" Disabling L1a_request %x %x \n",sndbuf[0],sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE, ccb_trig_adr ,sndbuf,rcvbuf, NOW); 


  // Read address back
  tmb_vme(VME_READ, seq_fifo_adr ,sndbuf,rcvbuf, NOW);
  //  printf(" ^^^ fifo_pretrig_ %d \n",fifo_pretrig_);
  //  printf(" ^^^ fifo_mode_ %d \n",fifo_mode_);
  //  printf(" ^^^ fifo_tbins_ %d \n",fifo_tbins_);
  sndbuf[0]= (fifo_pretrig_ & 0x1f) ;
  sndbuf[1]= (fifo_mode_ & 0x7) | ((fifo_tbins_ & 0x1f) << 3); 
  //  printf(" -------------- Reading 0x72 %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //  printf(" -------------- Writing 0x72 %x %x \n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  // Change address
  tmb_vme(VME_WRITE, seq_fifo_adr ,sndbuf,rcvbuf, NOW); 
  
  //disable CCB input during VME programming
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,  ccb_cfg_adr,sndbuf,rcvbuf, NOW); //Read CCB Config
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=(rcvbuf[1] | 0x01);
  //Write back with ignore CCB
  tmb_vme(VME_WRITE, ccb_cfg_adr,sndbuf,rcvbuf, NOW); 

  //sndbuf[0]=0x03;
  //sndbuf[1]=0x9C;
  //tmb_vme(VME_WRITE, lhc_cycle_adr,sndbuf,rcvbuf,NOW); // Maximum bxn
  //sndbuf[0]=0x7c;
  //sndbuf[1]=0x1f;
  //tmb_vme(VME_WRITE, cfeb_inj_adr,sndbuf,rcvbuf,NOW); //Mask CFEBs
  //sndbuf[0]=0x01;
  //sndbuf[1]=0x1b;
  //
  // Read address back
  tmb_vme(VME_READ,seq_trig_en_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=(rcvbuf[0]&0xff);
  //
  if ( choice == CLCT_trigger ){
    sndbuf[1]=(rcvbuf[1]&0x10)|0x01;
  }
  if ( choice == ALCT_trigger ){
    sndbuf[1]=(rcvbuf[1]&0x10)|0x02; 
  }
  if ( choice == Scintillator_trigger ){
    sndbuf[1]=(rcvbuf[1]&0x10)|0x20; 
  }
  if ( choice == DMB_trigger ){
    sndbuf[1]=(rcvbuf[1]&0x10)|0x10; 
  }
  if ( choice == ALCT_CLCT_coincidence_trigger ){
    sndbuf[1]=(rcvbuf[1]&0x10)|0x04; 
  }
  //
  ostringstream dump;
  //dump << "TMB. triggermode " << std::hex << (int) (sndbuf[0]&0xff) << " " <<  (int) (sndbuf[1]&0xff) ;
  //
  //  (*MyOutput_) << dump.str() <<std::endl;
  SendOutput(dump.str(),"INFO");
  //printf("TRGMODE %x %x %x" , seq_trig_en_adr, sndbuf[0], sndbuf[1]); 
  tmb_vme(VME_WRITE,seq_trig_en_adr,sndbuf,rcvbuf,NOW); // Sequencer Trigger Source

  //ALCT match window size and pulse delay settings ...
  // Read address back
  tmb_vme(VME_READ,tmbtim_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0] = (rcvbuf[0] & 0xf0) | (mpc_tx_delay_ & 0xf);
  sndbuf[1] = alct_match_window_size_ * 16 + alct_vpf_delay_;
  //printf("TRGMODE %x %x %x \n" , 0xb2, sndbuf[0], sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE,tmbtim_adr,sndbuf,rcvbuf,NOW); // ALCT delay
  //
  // l1a and bxn offsets 
  //
  // No read address back necessary, since these values fill the register
  //printf(" **********  Setting bxn to %d l1a to %d \n",bxn_offset_,l1a_offset_);
  sndbuf[0] = (bxn_offset_>>4)&0xff ;
  sndbuf[1] = (l1a_offset_&0xf) | ((bxn_offset_&0xf)<<4);
  tmb_vme(VME_WRITE,seq_offset_adr,sndbuf,rcvbuf,NOW); // Sequencer Counter Offset
  //
  sndbuf[0]=0x21;
  sndbuf[1]=0x41;
  //tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf,NOW); // Trigger Modifier

  
  //Pattern Thresholds (di/half/valid)
  //sndbuf[0]=0x4d;  // 3/3/3
  //sndbuf[1]=0xb5;
  //sndbuf[0]=0x45;  // 3/3/1
  //sndbuf[1]=0xb5;
  //sndbuf[0]=0x53;   // 7/4/4
  //sndbuf[1]=0xc5;
  //sndbuf[0]=0x47;   // 7/4/1
  //sndbuf[1]=0xc5;
  //sndbuf[0]=0x52;   // 4/4/4
  //sndbuf[1]=0x45;
  sndbuf[0]=0x46;   // 4/4/1 this is the best overall on 28-june-03
  sndbuf[1]=0x45;
  //sndbuf[0]=0x4F;   // 7/3/3 new
  //sndbuf[1]=0xD5;
  //sndbuf[0]=0x4D;   // 7/2/2 new
  //sndbuf[1]=0xA5;
  //sndbuf[0]=0x53;   // 7/2/4 new
  //sndbuf[1]=0xA5;
  //
  // Read address back
  tmb_vme(VME_READ,seq_clct_adr,sndbuf,rcvbuf,NOW); 
  sndbuf[0] = (rcvbuf[0] & 0xe0) |
    ( (min_hits_pattern_  & 0x7) << 2); 
  //    | ( (ds_pretrig_thresh_ & 0x6) >> 1);
  sndbuf[1] = (rcvbuf[1] & 0x0f) |
    ( (hs_pretrig_thresh_ & 0x7) << 4);
  //    | ( (ds_pretrig_thresh_ & 0x1) << 7)
  //  printf("TRGMODE %x %x %x" , seq_clct_adr, sndbuf[0], sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE,seq_clct_adr,sndbuf,rcvbuf,NOW); // Sequencer CLCT Conf.
  //
  if ( alct_clear_ == 1 ) {
    DisableALCTInputs();
  }

  setLogicAnalyzerToDataStream(false);
  
  //
  
  //  std::cout << " Setting slot depending settings on TMB slot .... " << theSlot <<std::endl;
  
  //L1A window size and pulse delay Chamber 1
  // Read address back
  tmb_vme(VME_READ,seq_l1a_adr,sndbuf,rcvbuf,NOW); // L1A delay
  sndbuf[0]=(rcvbuf[0] & 0xf0) | l1a_window_size_ & 0x0f;
  sndbuf[1]=l1adelay_ & 0xff;
  //  printf("TRGMODE %x %x %x" , seq_l1a_adr, sndbuf[0], sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW); // L1A delay
  
  // Only allow matched
  //tmb_vme(VME_READ, tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  //
  //sndbuf[0] = rcvbuf[0] & (0xff);
  //sndbuf[1] = rcvbuf[1] & (0xf3);
  //
  //tmb_vme(VME_WRITE,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  
  // Read Trigger conf
  //
  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  //
  //  printf("**********MPC delay %d \n",mpc_delay_);
  //
  //  printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  sndbuf[0] = (rcvbuf[0] & 0xfe | (mpc_delay_ & 0x8)>>3) & 0xff;
  sndbuf[1] = (rcvbuf[1] & 0x1f | (mpc_delay_ & 0x7)<<5) & 0xff;
  //
  if ( choice == ALCT_CLCT_coincidence_trigger ) {
    sndbuf[1] = (sndbuf[1]&0xf3) | (0x10); 
  } else {
    sndbuf[1] = (sndbuf[1]&0xf3) | (0x18); 
  }
  //
  //  printf("Setting address 0x86 to %x %x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  tmb_vme(VME_WRITE,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Write Trigger conf
  
  usleep(200);
  
  //(*MyOutput_) << "Setting up delay chips for TMB version " << version_ << std::endl;
  //if(version_ == "2004") {
  setupNewDelayChips();
  //} else 
  //setupOldDelayChips();
  //}
  
  //enable CCB input after VME programming
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,ccb_cfg_adr,sndbuf,rcvbuf,NOW); //Read CCB Config
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=(rcvbuf[1] & 0xfe);
  //
  //printf("TRGMODE %x %x %x" , 0x2a, sndbuf[0], sndbuf[1]);
  //
  tmb_vme(VME_WRITE,ccb_cfg_adr,sndbuf,rcvbuf,NOW);  //Write back, enable CCB
  //
  //tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  //
  //printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
  usleep(200);
  //
  //std::cout <<"enable CLCT Input " << enableCLCTInputs_ << std::endl;
  //
  EnableCLCTInputs(enableCLCTInputs_);
  //
  if ( disableCLCTInputs_ ) DisableCLCTInputs();
  //
  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  //  printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
}


void TMB::setupNewDelayChips() {
  //   std::cout << "setting up new TMB delay chips" << std::endl;
   sndbuf[0]=((cfeb0delay_<<4)&0xF0);   //DCC (CFEB duty cycle correction) unused
   sndbuf[1]=((mpc_phase_<<4)&0xF0);                      //TMB1, MPC clock unused
   tmb_vme(0x02,0x18,sndbuf,rcvbuf,1);  // CFEB0 DDD setting
   sndbuf[0]=((cfeb4delay_<<4)&0xF0)|(cfeb3delay_&0x0F);
   sndbuf[1]=((cfeb2delay_<<4)&0xF0)|(cfeb1delay_&0x0F);
   sndbuf[0]=((cfeb0delay_<<4)&0xF0);
   sndbuf[1]=0x00;
   //   tmb_vme(0x02,0x18,sndbuf,rcvbuf,1);  // CFEB0 DDD setting
   sndbuf[0]=((cfeb4delay_<<4)&0xF0)|(cfeb3delay_&0x0F);
   sndbuf[1]=((cfeb2delay_<<4)&0xF0)|(cfeb1delay_&0x0F);
   tmb_vme(0x02,0x1A,sndbuf,rcvbuf,1);  // CFEB1-4 DDD setting
   //
   //   sndbuf[0]=0x05;
   sndbuf[0]=((rat_tmb_delay_<<4)&0xF0)|(dmb_tx_delay_&0x0F);
   sndbuf[1]=((alct_rx_clock_delay_<<4)&0xF0)|(alct_tx_clock_delay_&0x0F);
   tmb_vme(0x02,0x16,sndbuf,rcvbuf,1);
   //
   sndbuf[0]=0x00;                     // RPC1, RPC2, RPC3 unused
   sndbuf[1]=(rpc0_rat_delay_&0x0F);
   tmb_vme(0x02,0xE6,sndbuf,rcvbuf,1);
   //
   sndbuf[0]=0x00;                     // RPC1, RPC2, RPC3 unused
   sndbuf[1]=(rpc0_raw_delay_&0x0F);
   tmb_vme(0x02,0xBA,sndbuf,rcvbuf,1);
   //
   sndbuf[0]=0x00;
   sndbuf[1]=0x20;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
   sndbuf[0]=0x00;
   sndbuf[1]=0x21;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
   sndbuf[0]=0x00;
   sndbuf[1]=0x20;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
}


void TMB::setupOldDelayChips() {
   sndbuf[0]=cfeb0delay_;
   sndbuf[1]=0x00;
   printf("TRGMODE %x %x %x" , 0x1a, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,NOW);

   sndbuf[0]=cfeb2delay_;
   sndbuf[1]=cfeb1delay_;
   printf("TRGMODE %x %x %x" , 0x1c, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,NOW);

   sndbuf[0]=cfeb4delay_;
   sndbuf[1]=cfeb3delay_;
   printf("TRGMODE %x %x %x" , 0x1e, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,NOW);

   //sndbuf[0]=0x00;  // Orig BeamTest setting
   //sndbuf[1]=0x0a;
   sndbuf[0] = alct_rx_clock_delay_;
   sndbuf[1] = alct_tx_clock_delay_;
   printf("TRGMODE %x %x %x" , 0x16, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,NOW);

   sndbuf[0]=0x00;
   sndbuf[1]=0x07;
   printf("TRGMODE %x %x %x" , 0x18, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x18,sndbuf,rcvbuf,NOW);
   sndbuf[0]=0x88;
   sndbuf[1]=0x77;
   tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW); // PHOS4 state machine
   sndbuf[0]=0x88;
   sndbuf[1]=0x33;
   tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW); // PHOS4 state machine
   ::sleep(5);
}

void TMB::tmb_clk_delays(unsigned short int time,int cfeb_id) {
  //
  //if(version_ == "2001") {
  //old_clk_delays(time, cfeb_id);
  //} else {
  new_clk_delays(time, cfeb_id);
  //}
  //
}

void TMB::DiStripHCMask(int DiStrip) {
  //
  // Only enables specific Distrp channels
  //
  printf("Enabling DiStrip %d \n",DiStrip);
  //
  sndbuf[0] = (0x1<<(DiStrip+1))&0xff;
  sndbuf[1] = (0x1<<(DiStrip))&0xff;
  //
  printf(" %x %x \n",sndbuf[0],sndbuf[1]);
  //
  tmb_vme(VME_WRITE, hcm001_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm023_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm045_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_WRITE, hcm101_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm123_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm145_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_WRITE, hcm201_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm223_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm245_adr,sndbuf,rcvbuf,NOW);
  /*
  tmb_vme(VME_WRITE, hcm301_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm323_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm345_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm401_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm423_adr,sndbuf,rcvbuf,NOW);
  tmb_vme(VME_WRITE, hcm445_adr,sndbuf,rcvbuf,NOW);
  */
}


void TMB::new_clk_delays_preGreg(unsigned short int time,int cfeb_id)
{
  //int ierr;
int iloop;
 iloop=0;
 //printf(" here write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 1000 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[1]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    sndbuf[0]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,NOW);
  } 

  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x21;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    //
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
    printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    //
  }

  printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);

  while((rcvbuf[1]&0x40)!=0x00){
    iloop++;
    if(iloop>10){
      printf(" tmb_clk_delays: loop count exceeded so quit \n");
      return;
    }
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  }
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1]&0xfe;
  //
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  //
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  }
  //
  if((rcvbuf[1]&0x80)!=0x80){
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    printf(" tmb_clk_delays: something is wrong. Can NOT be verified \n");
    return;
  }

  /* removed for new TMB delay chip
int ierr;
int iloop;
 iloop=0;
  //start(1); 
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,LATER);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  // send delay to dynatem 
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,LATER); 
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);

 
LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
  */
}


void TMB::executeCommand(std::string command) {
  //if(command=="PHOS4 ALCT")  tmb_PHOS4_alct();
  //if(command=="PHOS4 CFEB")  tmb_PHOS4_cfeb(); 
  //if(command=="Scan RX Clock") scan_rx_clock();
  if(command=="Show Firmware Version") firmwareVersion();
}


void TMB::disableAllClocks(){
  /// Disable all clocks to cfeb and alct. Should be used when updating the ALCT firmware
  //
  // Reading in broadcast mode does not work.  Comment the next few lines out...
  //  tmb_vme(VME_READ, vme_step_adr,sndbuf,rcvbuf,NOW);  
  //  sndbuf[0]=rcvbuf[0] & 0x1f;
  //  sndbuf[1]=rcvbuf[1] & 0xf8;
  sndbuf[0]= 0x1f;
  sndbuf[1]= 0xf8;
  tmb_vme(VME_WRITE, vme_step_adr, sndbuf,rcvbuf,NOW);
}

void TMB::enableAllClocks(){
  /// Dnable all clocks to cfeb and alct. Should be used after updating the ALCT firmware to get the TMB back in default mode.
  //
  // Reading in broadcast mode does not work.  Comment the next few lines out...
  //  tmb_vme(VME_READ, vme_step_adr,sndbuf,rcvbuf,NOW);   
  //  sndbuf[0]=rcvbuf[0] | 0xe0;
  //  sndbuf[1]=rcvbuf[1] | 0x07;
  sndbuf[0] = 0xe0;
  sndbuf[1] = 0x07;
  tmb_vme(VME_WRITE, vme_step_adr, sndbuf,rcvbuf,NOW);
}

void TMB::TriggerTestInjectALCT(){
  //
  // Turn off CCB backplane inputs, turn on L1A emulator
  /*
  adr = ccb_cfg_adr;
  wr_data ='003D'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  DisableExternalCCB();
  EnableInternalL1aEmulator();
  //
  // Enable sequencer trigger, set internal l1a delay
  /*
  adr = ccb_trig_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data =rd_data.and.'FF00'x;
  wr_data =wr_data.or.'0004'x;
  //	wr_data = wr_data.or.ishft(114,8);
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  tmb_vme(VME_READ, ccb_trig_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] = rcvbuf[0];
  sndbuf[1] = 0x4;
  tmb_vme(VME_WRITE, ccb_trig_adr, sndbuf,rcvbuf,NOW);
  //
  // Turn off ALCT cable inputs, disable synchronized alct+clct triggers
  /*
  adr = alct_inj_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'0000'x;
  wr_data = wr_data.or. '0001'x;
  wr_data = wr_data.or.ishft(alct_injector_delay,3); //post-rat firmware;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  DisableALCTInputs();
  DisableALCTCLCTSync();
  //
  // Turn off CLCT cable inputs
  /*
  adr = cfeb_inj_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'FFE0'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  DisableCLCTInputs();
  //
  // Turn off internal level 1 accept for sequencer
  /*
  adr = seq_l1a_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'0FFF'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  DisableInternalL1aSequencer();
  //
  // Select ALCT pattern trigger
  /*
  adr = seq_trig_en_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  rd_data=rd_data.and.'FF00'x;
  wr_data = rd_data.or.'0002'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  SetALCTPatternTrigger();
  //
  // Set start_trigger state then bx0 for FMM
  /*
  ttc_cmd=6;
  adr = base_adr+ccb_cmd_adr;
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  //
  /*
  wr_data='0003'x.or.ishft(ttc_cmd,8);
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  ttc_cmd=1;
  wr_data='0003'x.or.ishft(ttc_cmd,8);
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  //
  StartTTC();
  //
  // Arm scope trigger
  /*
  scp_arm=.true.;
  scp_readout=.false.;
  scp_raw_decode=.false.;
  scp_silent=.false.;
  if (rdscope)
    scope128(base_adr,scp_ctrl_adr,scp_rdata_adr,
	     scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_raw_data);
  */
  //
  scope(1,0,0);
  //
  // Clear previous inject
  /*
  adr=alct_inj_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data=rd_data.and.'FFFD'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  ClearALCTInjector();
  //  
  // Fire ALCT injector
  /*
    wr_data=wr_data.or.'0002'x;	//Fire ALCT inject
    status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  FireALCTInjector();
  //
  // Clear previous inject
  /*
  wr_data=rd_data.and.'FFFD'x ;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  ClearALCTInjector();
  //
  
  // Check scintillator veto is set
  /*
  adr=base_adr+seqmod_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  scint_veto=ishft(rd_data,-13).and.1;
  if(scint_veto.ne.1)pause 'scint veto failed to set';
  */
  //
  tmb_vme(VME_READ, seqmod_adr, sndbuf,rcvbuf,NOW);
  int scint_veto = ((rcvbuf[0]&0xff)>>5&0x1);
  if(scint_veto != 1) {
    (*MyOutput_) << "scint veto failed to set" << std::endl;
    return;
  }
  //
  // Clear scintillator veto
  /*
  wr_data=rd_data.or.ishft(1,12);
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  ClearScintillatorVeto();
  /*
  wr_data=wr_data.xor.ishft(1,12);
  status = vme_write(%ref(adr),%ref(wr_data));
  status = vme_read (%ref(adr),%ref(rd_data));
  scint_veto=ishft(rd_data,-12).and.1;
  if(scint_veto.ne.0)pause 'scint veto failed to clear';
  */
  tmb_vme(VME_READ, seqmod_adr, sndbuf,rcvbuf,NOW);
  scint_veto = ((sndbuf[0]&0xff)>>5)&0x1;
  if(scint_veto != 0) {
    (*MyOutput_) << "scint veto failed to clear" << std::endl;
  }
  //
  // Read back embedded scope data
  /*
  scp_arm=.false.;
  scp_readout=.true.;
  scp_raw_decode=.false.;
  scp_silent=.false.;
  */
  scope(0,1);
  /*
    if (rdscope)
    scope128(base_adr,scp_ctrl_adr,scp_rdata_adr,
    scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_raw_data);
  */
}

void TMB::TriggerTestInjectCLCT(){
  //
  // Turn off CCB backplane inputs, turn on L1A emulator
  /*
    adr = ccb_cfg_adr;
    wr_data ='003D'x;
    status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  DisableExternalCCB();
  EnableInternalL1aEmulator();
  //
  // Enable sequencer trigger, turn off dmb trigger, set internal l1a delay
  //
  /*
    adr = ccb_trig_adr+base_adr
    wr_data ='0004'x
    wr_data = wr_data.or.ishft(114,8)
    status = vme_write(%ref(adr),%ref(wr_data))
  */
  tmb_vme(VME_READ, ccb_trig_adr, sndbuf,rcvbuf,NOW);
  sndbuf[0] = rcvbuf[0];
  sndbuf[1] = 0x4;
  tmb_vme(VME_WRITE, ccb_trig_adr, sndbuf,rcvbuf,NOW);
  //
  // Turn off ALCT cable inputs, disable synchronized alct+clct triggers
  /*
  adr = alct_inj_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'0000'x;
  wr_data = wr_data.or. '0001'x;
  wr_data = wr_data.or.ishft(alct_injector_delay,3); //post-rat firmware;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  DisableALCTInputs();
  DisableALCTCLCTSync();
  //
  // Turn off CLCT cable inputs
  /*
  adr = cfeb_inj_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'FFE0'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  DisableCLCTInputs();
  //
  // Turn off internal level 1 accept for sequencer
  /*
  adr = seq_l1a_adr+base_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  wr_data = rd_data.and.'0FFF'x;
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  //
  DisableInternalL1aSequencer();
  //
  // Select pattern trigger
  //
  SetCLCTPatternTrigger();
  //
  // Set start_trigger state then bx0 for FMM
  /*
  ttc_cmd=6;
  adr = base_adr+ccb_cmd_adr;
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  //
  /*
  wr_data='0003'x.or.ishft(ttc_cmd,8);
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  ttc_cmd=1;
  wr_data='0003'x.or.ishft(ttc_cmd,8);
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  /*
  wr_data='0001'x;
  status= vme_write (%ref(adr),%ref(wr_data));
  */
  //
  StartTTC();
  //
  // Arm scope trigger
  /*
  scp_arm=.true.;
  scp_readout=.false.;
  scp_raw_decode=.false.;
  scp_silent=.false.;
  if (rdscope)
    scope128(base_adr,scp_ctrl_adr,scp_rdata_adr,
	     scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_raw_data);
  */
  //
  scope(1,0,0);
  //
  // Clear previous CLCT inject
  /*
    adr=cfeb_inj_adr+base_adr
    status = vme_read (%ref(adr),%ref(rd_data))
    wr_data=rd_data.and.'7FFF'x	
    status = vme_write(%ref(adr),%ref(wr_data))
  */
  //
  ClearCLCTInjector();
  //  
  // Fire CLCT injector
  /*
    wr_data=wr_data.or.'8000'x
    status = vme_write(%ref(adr),%ref(wr_data))
  */
  FireCLCTInjector();
  //

  // Clear previous inject
  /*
    adr=cfeb_inj_adr+base_adr
    status = vme_read (%ref(adr),%ref(rd_data))
    wr_data=rd_data.and.'7FFF'x	
    status = vme_write(%ref(adr),%ref(wr_data))
  */
  //
  ClearCLCTInjector();
  //  
  // Check scintillator veto is set
  /*
  adr=base_adr+seqmod_adr;
  status = vme_read (%ref(adr),%ref(rd_data));
  scint_veto=ishft(rd_data,-13).and.1;
  if(scint_veto.ne.1)pause 'scint veto failed to set';
  */
  //
  tmb_vme(VME_READ, seqmod_adr, sndbuf,rcvbuf,NOW);
  int scint_veto = ((rcvbuf[0]&0xff)>>5&0x1);
  if(scint_veto != 1) {
    (*MyOutput_) << "scint veto failed to set" << std::endl;
    return;
  }
  //
  // Clear scintillator veto
  /*
    wr_data=rd_data.or.ishft(1,12);
  status = vme_write(%ref(adr),%ref(wr_data));
  */
  ClearScintillatorVeto();
  /*
  wr_data=wr_data.xor.ishft(1,12);
  status = vme_write(%ref(adr),%ref(wr_data));
  status = vme_read (%ref(adr),%ref(rd_data));
  scint_veto=ishft(rd_data,-12).and.1;
  if(scint_veto.ne.0)pause 'scint veto failed to clear';
  */
  tmb_vme(VME_READ, seqmod_adr, sndbuf,rcvbuf,NOW);
  scint_veto = ((sndbuf[0]&0xff)>>5)&0x1;
  if(scint_veto != 0) {
    (*MyOutput_) << "scint veto failed to clear" << std::endl;
  }
  //
  // Read back embedded scope data
  /*
    scp_arm=.false.;
    scp_readout=.true.;
    scp_raw_decode=.false.;
    scp_silent=.false.;
  */
  scope(0,1);
  /*
    if (rdscope)
    scope128(base_adr,scp_ctrl_adr,scp_rdata_adr,
    scp_arm,scp_readout,scp_raw_decode,scp_silent,scp_raw_data);
  */
}
//
//
int TMB::tmb_read_delays(int device) {

  int data;

  // device = 0  = CFEB 0 Clock
  //        = 1  = CFEB 1 clock
  //        = 2  = CFEB 2 clock
  //        = 3  = CFEB 3 clock
  //        = 4  = CFEB 4 clock
  //        = 5  = ALCT rx clock
  //        = 6  = ALCT tx clock
  //        = 7  = DMB tx clock
  //        = 8  = RAT clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)
  //        = 12 = RPC 0 clock
  //        = 13 = RPC 1 clock

  if (device==0) data = (ReadRegister(vme_ddd1_adr)>>12) & 0xf; 
  if (device==1) data = (ReadRegister(vme_ddd2_adr)>> 0) & 0xf; 
  if (device==2) data = (ReadRegister(vme_ddd2_adr)>> 4) & 0xf; 
  if (device==3) data = (ReadRegister(vme_ddd2_adr)>> 8) & 0xf; 
  if (device==4) data = (ReadRegister(vme_ddd2_adr)>>12) & 0xf; 
  if (device==5) data = (ReadRegister(vme_ddd0_adr)>> 4) & 0xf; 
  if (device==6) data = (ReadRegister(vme_ddd0_adr)>> 0) & 0xf; 
  if (device==7) data = (ReadRegister(vme_ddd0_adr)>> 8) & 0xf; 
  if (device==8) data = (ReadRegister(vme_ddd0_adr)>>12) & 0xf; 
  if (device==9) data = (ReadRegister(vme_ddd1_adr)>> 0) & 0xf; 
  if (device==10) data =(ReadRegister(vme_ddd1_adr)>> 4) & 0xf; 
  if (device==11) data =(ReadRegister(vme_ddd1_adr)>> 8) & 0xf; 
  if (device==12) data =(ReadRegister(rat_3d_delays_adr)>> 0) & 0xf; 
  if (device==13) data =(ReadRegister(rat_3d_delays_adr)>> 4) & 0xf; 

  return data;
}

void TMB::new_clk_delays(unsigned short int time,int device)
{
  // device = 0  = CFEB 0 Clock
  //        = 1  = CFEB 1 clock
  //        = 2  = CFEB 2 clock
  //        = 3  = CFEB 3 clock
  //        = 4  = CFEB 4 clock
  //        = 5  = ALCT rx clock
  //        = 6  = ALCT tx clock
  //        = 7  = DMB tx clock
  //        = 8  = RAT clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)
  //        = 12 = RPC 0 clock
  //        = 13 = RPC 1 clock
  //        = 1000 = CFEB [0-5] Clock (all CFEB's)

  //int ierr;
int iloop;
 iloop=0;
 //printf(" here write to delay registers \n");
  if ( device == 0 ) {
    tmb_vme(0x01,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 1 ) {
    tmb_vme(0x01,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 2 ) {
    tmb_vme(0x01,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 3 ) {
    tmb_vme(0x01,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 4 ) {
    tmb_vme(0x01,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 5 ) {
    tmb_vme(0x01,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 6 ) {
    tmb_vme(0x01,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 7 ) {
    tmb_vme(0x01,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 8 ) {
    tmb_vme(0x01,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd0_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 9 ) {
    tmb_vme(0x01,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 10 ) {
    tmb_vme(0x01,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 11 ) {
    tmb_vme(0x01,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 12 ) {
    tmb_vme(0x01,rat_3d_delays_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,rat_3d_delays_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 13 ) {
    tmb_vme(0x01,rat_3d_delays_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,rat_3d_delays_adr,sndbuf,rcvbuf,NOW);
  } 
  if ( device == 1000 ) {
    tmb_vme(0x01,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,vme_ddd1_adr,sndbuf,rcvbuf,NOW);
    sndbuf[1]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    sndbuf[0]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    tmb_vme(0x02,vme_ddd2_adr,sndbuf,rcvbuf,NOW);
  } 

  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //while((rcvbuf[1]>>6)&(0x1)){
  //(*MyOutput_) << "1.Waiting"<<std::endl;
  //tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //}
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x21;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //while((rcvbuf[1]>>6)&(0x1)){
  //(*MyOutput_) << "2.Waiting"<<std::endl;
  //tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //}
  //
  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //while((rcvbuf[1]>>6)&(0x1)){
  //(*MyOutput_) << "3.Waiting"<<std::endl;
  //tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  //}
  //
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    //
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
    //printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    //
  }

  //printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);

  while((rcvbuf[1]&0x40)!=0x00){
    iloop++;
    if(iloop>10){
      printf(" tmb_clk_delays: loop count exceeded so quit \n");
      return;
    }
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,NOW);
  }
  //
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1]&0xfe;
  //
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  while ( ((rcvbuf[1]>>6)&(0x1)) ){
    (*MyOutput_) << "4.Waiting"<<std::endl;
    tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  }
  //
  if((rcvbuf[1]&0x80)!=0x80){
    printf(" *** check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
    printf(" tmb_clk_delays: something is wrong. Can NOT be verified \n");
    ::sleep(100);
  }

  /* removed for new TMB delay chip
int ierr;
int iloop;
 iloop=0;
  //start(1); 
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,LATER);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  // send delay to dynatem 
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,LATER); 
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);

 
LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
  */
}

void TMB::ExtClctTrigFromCCBonly() {

  (*MyOutput_) << "Enable CLCT external trigger from TTC (through CCB)" << std::endl;

  int data = ReadRegister(seq_trig_en_adr);

  (*MyOutput_) << "TMB Sequencer trigger source before = " << std::hex << data << std::endl;

  data = 0x0020;                                 //allow CLCT external triggers from CCB

  WriteRegister(seq_trig_en_adr,data);

  data = ReadRegister(seq_trig_en_adr);

  (*MyOutput_) << "TMB Sequencer trigger source after = " << std::hex << data << std::endl;

  return;
}
/////////////////////////////////////////////////////////////////////
// hot channel masks
/////////////////////////////////////////////////////////////////////
void TMB::SetDistripHotChannelMask(int layer,long long int mask) {
  //
  // mask=10-hex characters for the 40 distrips right->left LSB->MSB.  
  // So, to mask off channel 0, mask= 0xfffffffffe
  //
  if (debug_) 
    std::cout << "TMB: Setting Distrip hot channel mask for Layer " << std::dec << layer 
	      << " (LSB->MSB right->left): " << std::hex << mask << std::endl;
  //
  mask &= 0xffffffffffLL;  //clean mask before processing
  //
  for (int channel=39; channel>=0; channel--) {
    int on_or_off = (mask >> channel) & 0x1;
    //
    if (debug_) {
      std::cout << on_or_off;
      if ((channel%4) == 0) std::cout << " ";
    }
    //
    SetDistripHotChannelMask(layer,channel,on_or_off);
  }
  if (debug_) std::cout << std::endl;
  //
  return;
}
//
void TMB::WriteDistripHotChannelMasks(){
  //
  WriteRegister(hcm001_adr);
  WriteRegister(hcm023_adr); 
  WriteRegister(hcm045_adr); 
  WriteRegister(hcm101_adr); 
  WriteRegister(hcm123_adr); 
  WriteRegister(hcm145_adr); 
  WriteRegister(hcm201_adr); 
  WriteRegister(hcm223_adr); 
  WriteRegister(hcm245_adr); 
  WriteRegister(hcm301_adr); 
  WriteRegister(hcm323_adr); 
  WriteRegister(hcm345_adr); 
  WriteRegister(hcm401_adr); 
  WriteRegister(hcm423_adr); 
  WriteRegister(hcm445_adr); 
  //
  return;
}
//
void TMB::ReadDistripHotChannelMasks(){
  //
  ReadRegister(hcm001_adr);
  ReadRegister(hcm023_adr); 
  ReadRegister(hcm045_adr); 
  ReadRegister(hcm101_adr); 
  ReadRegister(hcm123_adr); 
  ReadRegister(hcm145_adr); 
  ReadRegister(hcm201_adr); 
  ReadRegister(hcm223_adr); 
  ReadRegister(hcm245_adr); 
  ReadRegister(hcm301_adr); 
  ReadRegister(hcm323_adr); 
  ReadRegister(hcm345_adr); 
  ReadRegister(hcm401_adr); 
  ReadRegister(hcm423_adr); 
  ReadRegister(hcm445_adr); 
  //
  return;
}
//
////////////////////////////////////////////////////////
// Digital Serial Numbers
////////////////////////////////////////////////////////
std::bitset<64> TMB::dsnRead(int type) {
  //type = 0 = TMB
  //     = 1 = Mezzanine DSN
  //     = 2 = RAT DSN
  //
  std::bitset<64> dsn;
  //
  int offset;
  offset = type*5; 
  //
  int wr_data, rd_data;
  //
  int initial_state;
  // ** need to specifically enable RAT to read back DSN **
  if (type == 2) {  
    initial_state = ReadRegister(vme_ratctrl_adr);  //initial RAT state
    //
    wr_data = initial_state & 0xfffd;    //0=sync_mode, 1=posneg, 2=loop_tmb, 3=free_tx0, 4=dsn enable
    wr_data |= 0x0010;                   //enable the dsn bit
    //
    WriteRegister(vme_ratctrl_adr,wr_data);
  }
  //
  // init pulse >480usec
  wr_data = 0x0005; 
  wr_data <<= offset; //send it to correct component
  rd_data = dsnIO(wr_data);
  //
  // ROM Read command = serial 0x33:
  for (int i=0; i<=7; i++) {
    int idata = (0x33>>i) & 0x1;
    wr_data = (idata<<1) | 0x1; //send "serial write pulse" with "serial SM start"
    wr_data <<= offset; 
    rd_data = dsnIO(wr_data);
  }
  //
  // Read 64 bits of ROM data = 0x3 64 times
  for (int i=0; i<=63; i++) {
    wr_data = 0x0003; 
    wr_data <<= offset;
    rd_data = dsnIO(wr_data);
    //
    // pack data into dsn[]
    dsn[i] = (rd_data >> (4+offset)) & 0x1;
  }
  //
  // ** Return the RAT to its initial state **
  if (type == 2) 
    WriteRegister(vme_ratctrl_adr,initial_state);
  //
  return dsn;
}
//
int TMB::dsnIO(int writeData){
  //Single I/O cycle for Digital Serial Number...
  //called by dsnRead...
  //
  int readData;
  //
  // write the desired data word:
  WriteRegister(vme_dsn_adr,writeData);
  //
  int tmb_busy,mez_busy,rat_busy;
  int busy = 1;
  int nbusy = 1;
  //
  while (busy) {
    readData = ReadRegister(vme_dsn_adr);
    //
    // check busy on all components:
    tmb_busy = (readData>>3) & 0x1;
    mez_busy = (readData>>8) & 0x1;
    rat_busy = (readData>>13) & 0x1;
    busy = tmb_busy | mez_busy | rat_busy;
    //
    if (nbusy%1000 == 0) {
      (*MyOutput_) << "dsnIO: DSN state machine busy, nbusy = "
                << nbusy << ", readData = " 
		<< std::hex << readData << std::endl;  
    }
    nbusy++;
  }
  //
  // end previous cycle
  WriteRegister(vme_dsn_adr,0x0000);
  //
  return readData;
}
//
////////////////////////////////////////////////////////////////////////////
// read on-board Voltages and temperatures
////////////////////////////////////////////////////////////////////////////
void TMB::ADCvoltages(float * voltage){
  //
  //Read the ADC of the voltage values ->
  //voltage[0] = +5.0V TMB
  //       [1] = +3.3V TMB
  //       [2] = +1.5V core
  //       [3] = +1.5V TT
  //       [4] = +1.0V TT
  //       [5] = +5.0V Current (A) TMB
  //       [6] = +3.3V Current (A) TMB
  //       [7] = +1.5V core Current (A) TMB
  //       [8] = +1.5V TT Current (A) TMB
  //       [9] = if SH921 set 1-2, +1.8V RAT current (A)
  //           = if SH921 set 2-3, +3.3V RAT
  //      [10] = +1.8V RAT core
  //      [11] = reference Voltage * 0.5
  //      [12] = ground (0V)
  //      [13] = reference voltage (= ADC maximized)
  //
  int adc_dout;                      //Voltage monitor ADC serial data receive
  int adc_sclock;                    //Voltage monitor ADC serial clock
  int adc_din;                       //Voltage monitor ADC serial data transmit
  int adc_cs;                        //Voltage monitor ADC chip select
  //
  int adc_shiftin;
  int iclk;
  //
  int write_data, read_data;
  //
  for (int ich=0; ich<=14; ich++){
    //
    adc_dout = 0;
    //
    adc_din    = 0;
    adc_sclock = 0;
    adc_cs     = 1;
    //
    write_data = 0;
    write_data |= (adc_sclock << 6);  
    write_data |= (adc_din    << 7);  
    write_data |= (adc_cs     << 8);  
    //
    WriteRegister(vme_adc_adr,write_data);
    //
    adc_shiftin = ich << 4;      //d[7:4]=channel, d[3:2]=length, d[1:0]=ldbf,bip
    if (ich >= 14) adc_shiftin = 0;  //don't send channel 14, it is power-down
    //
    //put adc_shiftin serially in 11 vme writes
    for (iclk=0; iclk<=11; iclk++){
      //
      if (iclk <= 7) {
	adc_din = (adc_shiftin >> (7-iclk)) & 0x1;
      } else {
	adc_din = 0;
      }
      adc_sclock = 0;
      adc_cs     = 0;
      //
      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      //
      WriteRegister(vme_adc_adr,write_data);
      //
      adc_sclock = 1;
      adc_cs     = 0;
      //
      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      //
      WriteRegister(vme_adc_adr,write_data);
      //
      usleep(100);
      //
      read_data = (PowerComparator() >> 5) & 0x1;
      //
      //pack output into adc_dout
      adc_dout |= (read_data << (11-iclk));
    }
    //
    adc_din    = 0;
    adc_sclock = 0;
    adc_cs     = 1;
    //
    write_data = 0;
    write_data |= (adc_sclock << 6);  
    write_data |= (adc_din    << 7);  
    write_data |= (adc_cs     << 8);  
    //
    WriteRegister(vme_adc_adr,write_data);
    //
    if (ich>=1) 
      voltage[ich-1] = ((float) adc_dout / 4095.)*4.095; //convert adc value to volts
    //
  }
  //
  voltage[0] *= 2.0;                      // 1V/2V
  voltage[5] /= 0.2;                      // 200mV/Amp
  voltage[6] /= 0.2;                      // 200mV/Amp
  voltage[7] /= 0.2;                      // 200mV/Amp
  voltage[8] /= 0.2;                      // 200mV/Amp
  voltage[9] /= 0.2;                      // 200mV/Amp if SH921 set 1-2, else comment out line
  //
  v5p0_	    = voltage[0];	      
  v3p3_	    = voltage[1];
  v1p5core_ = voltage[2];
  v1p5tt_   = voltage[3];
  v1p0_	    = voltage[4];
  a5p0_	    = voltage[5];	      
  a3p3_	    = voltage[6];
  a1p5core_ = voltage[7];
  a1p5tt_   = voltage[8];
  a1p8rat_  = voltage[9];	        // if SH921 set 1-2, loop backplane sends 1.500vtt
  //v3p3rat_= voltage[9];	// if SH921 set 2-3
  v1p8rat_  = voltage[10];
  vref2_    = voltage[11];
  vzero_    = voltage[12];
  vref_     = voltage[13];
  //
  return;
}
//
int TMB::ReadTMBtempPCB() {
  //
  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x00;   // "local" temperature read
  int temperature = smb_io(smb_adr,command,1);
  //
  (*MyOutput_) << "TMB temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int TMB::ReadTMBtempFPGA() {
  //
  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x01;   // "remote" temperature read
  int temperature = smb_io(smb_adr,command,1);
  //
  (*MyOutput_) << "TMB temperature IC (FPGA)             = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int TMB::ReadTMBtCritPCB() {
  //
  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x05;   // "local" temperature critical read
  int temperature = smb_io(smb_adr,command,1);
  //
  (*MyOutput_) << "TMB Critical Temperature IC (PCB)     = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int TMB::ReadTMBtCritFPGA() {
  //
  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  int temperature = smb_io(smb_adr,command,1);
  //
  (*MyOutput_) << "TMB Critical Temperature (FPGA)       = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int TMB::smb_io(int smb_adr, int cmd, int module) {
  //	Generates SMB serial clock and data streams to TMB LM84 chip
  //
  //   -> Returns temperature values in Celcius <-
  //
  //    smb_adr = 0x2a = float, float state TMB LM84 chip address
  //            = 0x18 = gnd, gnd state RAT LM84 chip address
  //
  //    cmd = 0x00 = local temperature command
  //        = 0x01 = remote temperature command
  //        = 0x05 = local tcrit command  
  //        = 0x07 = remote tcrit command  
  //
  //    module = 1 = TMB
  //           = 2 = RAT
  //
  const int adc_adr = vme_adc_adr;
  //
  int smb_data = 0xff;        // null write command
  //
  int write_data,read_data;
  //
  // Current ADC register state:
  int adc_status = ReadRegister(adc_adr);
  //
  // **Step 1 write the command to read the data**
  //
  // ** initialize SMB data stream **
  int sda_bit[29];
  sda_bit[0] = 0;                                // Start
  sda_bit[1] = (smb_adr >> 6) & 1;               // A6
  sda_bit[2] = (smb_adr >> 5) & 1;               // A5
  sda_bit[3] = (smb_adr >> 4) & 1;               // A4
  sda_bit[4] = (smb_adr >> 3) & 1;               // A3
  sda_bit[5] = (smb_adr >> 2) & 1;               // A2
  sda_bit[6] = (smb_adr >> 1) & 1;               // A1
  sda_bit[7] = (smb_adr >> 0) & 1;               // A0
  sda_bit[8] = 0;                                // 0 = write command register                           
  sda_bit[9] = 1;                                // ACK
  sda_bit[10]= (cmd     >> 7) & 1;               // C7 
  sda_bit[11]= (cmd     >> 6) & 1;               // C6 
  sda_bit[12]= (cmd     >> 5) & 1;               // C5 
  sda_bit[13]= (cmd     >> 4) & 1;               // C4 
  sda_bit[14]= (cmd     >> 3) & 1;               // C3 
  sda_bit[15]= (cmd     >> 2) & 1;               // C2 
  sda_bit[16]= (cmd     >> 1) & 1;               // C1 
  sda_bit[17]= (cmd     >> 0) & 1;               // C0 
  sda_bit[18]= 1;                                // ACK
  sda_bit[19]= (smb_data>> 7) & 1;               // D7 write data register
  sda_bit[20]= (smb_data>> 6) & 1;               // D6 write data register
  sda_bit[21]= (smb_data>> 5) & 1;               // D5 write data register
  sda_bit[22]= (smb_data>> 4) & 1;               // D4 write data register
  sda_bit[23]= (smb_data>> 3) & 1;               // D3 write data register
  sda_bit[24]= (smb_data>> 2) & 1;               // D2 write data register
  sda_bit[25]= (smb_data>> 1) & 1;               // D1 write data register
  sda_bit[26]= (smb_data>> 0) & 1;               // D0 write data register
  sda_bit[27]= 1;                                // ACK
  sda_bit[28]= 0;                                // Stop
  //
  // ** Construct SMBclk and SMBdata **
  //
  //	SMB requires that serial data is stable while clock is high,
  //	so data transitions occur while clock is low,
  //	midway between clock falling edge and rising edge
  //
  int nclks = 115;
  //
  int sda_clock,scl_clock;
  int sda,scl;
  //
  int i2c_clock;
  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle
    //
    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];
    //
    //    (*MyOutput_) << "Before Persistent -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;
    //
    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low
    //
    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high
    //
    //    (*MyOutput_) << "After Persistent  -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;
    //
    //** Write serial clock and data to TMB VME interface **
    //
    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    WriteRegister(adc_adr,write_data);
  }
  //
  // Current ADC register state:
  adc_status = ReadRegister(adc_adr);
  //
  // **Step 2 read the data**
  //
  // ** initialize SMB data stream **
  sda_bit[0] = 0;                                // Start
  sda_bit[1] = (smb_adr >> 6) & 1;               // A6
  sda_bit[2] = (smb_adr >> 5) & 1;               // A5
  sda_bit[3] = (smb_adr >> 4) & 1;               // A4
  sda_bit[4] = (smb_adr >> 3) & 1;               // A3
  sda_bit[5] = (smb_adr >> 2) & 1;               // A2
  sda_bit[6] = (smb_adr >> 1) & 1;               // A1
  sda_bit[7] = (smb_adr >> 0) & 1;               // A0
  sda_bit[8] = 1;                                // 1 = read data register                           
  sda_bit[9] = 1;                                // ACK
  sda_bit[10]= 1;                                // D7 read from LM84, 1=z output from fpga
  sda_bit[11]= 1;                                // D6
  sda_bit[12]= 1;                                // D5
  sda_bit[13]= 1;                                // D4
  sda_bit[14]= 1;                                // D3
  sda_bit[15]= 1;                                // D2
  sda_bit[16]= 1;                                // D1
  sda_bit[17]= 1;                                // D0
  sda_bit[18]= 1;                                // ACK
  sda_bit[19]= 0;                                // Stop
  //
  int d[20];
  int sda_value;
  //
  nclks = 79;
  //
  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle
    //
    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];
    //
    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low
    //
    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high
    //
    //** Write serial clock and data to TMB VME interface **
    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    WriteRegister(adc_adr,write_data);
    //
    //** Read Serial data from TMB VME interface **
    // (read on every cycle to keep clock symmetric)
    read_data = ReadRegister(adc_adr);
    if (scl==1) 
      d[sda_clock] = read_data;
  }
  //
  // pack data into an integer...
  int ishift = 0;
  if (module == 1) ishift = 10;   //data bit from LM84 on TMB
  if (module == 2) ishift = 11;   //data bit from LM84 on RAT
  //
  int i;
  int data = 0;
  for (i=0; i<=31; i++) {
    if (i<=7) {
      sda_value = (d[17-i]>>ishift) & 0x1;
      data |= sda_value<<i;          //d[7:0]
    } else {
      data |= sda_value<<i;          //sign extend if bit 7 indicates negative value      
    }
  }
  //
  //  (*MyOutput_) << "Temperature = " << std::dec << data << " deg C" << std::endl;
  //
  return data;
}
//
////////////////////////////////////////////////////////////////////////////
// write/read userPROMs
////////////////////////////////////////////////////////////////////////////
void TMB::ClockOutPromProgram(int prom,
			      int number_of_addresses) {
  //
  clocked_out_prom_image_.clear();    
  //
  int enabledProm = prom;
  int disabledProm = (enabledProm + 1) % 2;
  //
  (*MyOutput_) << "TMB:  Clock out 0x" << std::hex << number_of_addresses 
	       << " addresses from user PROM " << enabledProm << "... " << std::endl;
  //
  int prom_clk[2];
  int prom_oe[2];
  int prom_nce[2];
  //
  prom_clk[enabledProm]=0;    
  prom_oe[enabledProm] =1;     //enable this prom in vme register
  prom_nce[enabledProm]=0;
  //
  prom_clk[disabledProm]=0;    
  prom_oe[disabledProm] =0;    //disable this prom in vme register
  prom_nce[disabledProm]=1;
  //
  int prom_src=1;
  //
  int write_data = 
    (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
    (prom_nce[1]<<13) |        //PROM 1 /chip_enable
    (prom_oe[1] <<12) |        //PROM 1 output enable
    (prom_clk[1]<<11) |        //PROM 1 clock
    (prom_nce[0]<<10) |        //PROM 0 /chip_enable
    (prom_oe[0] << 9) |        //PROM 0 output enable
    (prom_clk[0]<< 8);         //PROM 0 clock
  //
  WriteRegister(vme_prom_adr,write_data);
  //
  // **Read the data from the selected PROM **
  for (int prom_adr=0; prom_adr<number_of_addresses; prom_adr++) {
    //
    clocked_out_prom_image_.push_back((int) (ReadRegister(vme_prom_adr) & 0xff));
    //    (*MyOutput_) << "VME address " << std::hex << vme_prom_adr
    //		 << ", read prom " << enabledProm 
    //    		 << ", address " << prom_adr 
    //    		 << ", data = " << clocked_out_prom_image_.at(prom_adr) 
    //    		 << std::endl;
    //
    // ** Toggle the clock to advance the address **
    prom_clk[enabledProm]=1;
    write_data = 
      (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
      (prom_nce[1]<<13) |        //PROM 1 /chip_enable
      (prom_oe[1] <<12) |        //PROM 1 output enable
      (prom_clk[1]<<11) |        //PROM 1 clock
      (prom_nce[0]<<10) |        //PROM 0 /chip_enable
      (prom_oe[0] << 9) |        //PROM 0 output enable
      (prom_clk[0]<< 8);         //PROM 0 clock
    WriteRegister(vme_prom_adr,write_data);
    //
    prom_clk[enabledProm]=0;
    write_data = 
      (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
      (prom_nce[1]<<13) |        //PROM 1 /chip_enable
      (prom_oe[1] <<12) |        //PROM 1 output enable
      (prom_clk[1]<<11) |        //PROM 1 clock
      (prom_nce[0]<<10) |        //PROM 0 /chip_enable
      (prom_oe[0] << 9) |        //PROM 0 output enable
      (prom_clk[0]<< 8);         //PROM 0 clock
    WriteRegister(vme_prom_adr,write_data);  
  }
  //
  // ** Turn PROMs off **
  prom_clk[enabledProm]=0;
  prom_oe[enabledProm] =0;    //disable this prom in VME register
  prom_nce[enabledProm]=1;
  //
  prom_src=0;
  //
  write_data = 
    (prom_src   <<14) |        //0=on-board led, 1=enabled PROM
    (prom_nce[1]<<13) |        //PROM 1 /chip_enable
    (prom_oe[1] <<12) |        //PROM 1 output enable
    (prom_clk[1]<<11) |        //PROM 1 clock
    (prom_nce[0]<<10) |        //PROM 0 /chip_enable
    (prom_oe[0] << 9) |        //PROM 0 output enable
    (prom_clk[0]<< 8);         //PROM 0 clock
  
  WriteRegister(vme_prom_adr,write_data);
  //
  return;
}
//
void TMB::ClearVmeWriteVecs() {
  //
  theController->Clear_VmeWriteVecs();
  //
  return;
}
//
void TMB::SetFillVmeWriteVecs(bool fill_vectors_or_not) {
  //
  theController->Set_FillVmeWriteVecs(fill_vectors_or_not);
  //
  return;
}
//
bool TMB::GetFillVmeWriteVecs() {
  //
  return theController->Get_FillVmeWriteVecs();
  //
}
//
std::vector<int> TMB::GetVecVmeAddress() { 
  //
  return theController->Get_VecVmeAddress(); 
  //
}
//
std::vector<int> TMB::GetVecDataLsb() { 
  //
  return theController->Get_VecDataLsb(); 
  //
}
std::vector<int> TMB::GetVecDataMsb() { 
  //
  return theController->Get_VecDataMsb(); 
  //
}
//
void TMB::OkVmeWrite(char vme) {
  //
  theController->Set_OkVmeWriteAddress(false);
  //
  // Allow writes from user prom only to specific VME addresses:
  for (unsigned int index=0; index<TMBConfigurationRegister.size(); index++) {
    //
    if ( vme == (char) (TMBConfigurationRegister.at(index) & 0xff) ) {
      theController->Set_OkVmeWriteAddress(true);      
      break;
    }
    //
  } 
  //
  return;
}
//
////////////////////////////////////////////////////////////////////////////
// Read TMB registers
////////////////////////////////////////////////////////////////////////////
void TMB::DumpAllRegisters() {
  //
  (*MyOutput_) << "TMB register dump:" << std::endl;
  (*MyOutput_) << "addr  value" << std::endl;
  (*MyOutput_) << "----  -----" << std::endl;
  for (int register_address=0; register_address <= LARGEST_VME_ADDRESS; register_address+=2) {
    //
    int register_value = ReadRegister(register_address);
    (*MyOutput_) << " " << std::hex  
		 << ( (register_address >> 4) & 0xf ) 
		 << ( (register_address >> 0) & 0xf ) 
		 << "   " << std::hex
		 << ( (register_value   >>12) & 0xf )
		 << ( (register_value   >> 8) & 0xf )
		 << ( (register_value   >> 4) & 0xf )
		 << ( (register_value   >> 0) & 0xf ) << std::endl;
  }
  //
  return;
}
//
void TMB::ReadTMBConfiguration() {
  //
  FirmwareDate();
  FirmwareYear();
  FirmwareVersion();
  FirmwareRevCode();
  //
  unsigned short int BootData;
  tmb_get_boot_reg(&BootData);
  //
  //  (*MyOutput_) << "addr    data" << std::endl;
  //  (*MyOutput_) << "====   ======" << std::endl;
  //
  for (unsigned int index=0; index<TMBConfigurationRegister.size(); index++) {
    //
    unsigned long int VMEregister = TMBConfigurationRegister.at(index);
    //
    if (VMEregister != vme_usr_jtag_adr) {     // skip the user jtag register
      //
      //      (*MyOutput_) << "0x" << std::hex
      //		   << ((VMEregister >> 4) & 0xf) 
      //		   << ((VMEregister >> 0) & 0xf);
      //      (*MyOutput_) << "   ";
      //
      //      int config_data = 
      ReadRegister(VMEregister);
      //
      //      (*MyOutput_) << "0x" << std::hex  
      //		   << ((config_data >>12) & 0xf) 
      //		   << ((config_data >> 8) & 0xf) 
      //		   << ((config_data >> 4) & 0xf) 
      //		   << ((config_data >> 0) & 0xf);
      //      (*MyOutput_) << std::endl;		
    }
  }
  //
  //
  return;
}
//
void TMB::ReadVMEStateMachine() {
  //
  ReadRegister(vme_sm_ctrl_adr);
  ReadRegister(vme_sm_wdcnt_adr);
  ReadRegister(vme_sm_cksum_adr);
  ReadRegister(num_vme_sm_adr_adr);
  //
  return;
}
//
void TMB::ReadJTAGStateMachine() {
  //
  ReadRegister(jtag_sm_ctrl_adr);
  ReadRegister(jtag_sm_cksum_adr);
  ReadRegister(jtag_sm_wdcnt_adr);
  //
  unsigned short int BootData;
  tmb_get_boot_reg(&BootData);
  //
  return;
}
//
void TMB::ReadRawHitsHeader() {
  //
  ReadRegister(tmb_stat_adr);
  //
  return;
}
//
void TMB::ReadDDDStateMachine() {
  //
  ReadRegister(vme_dddsm_adr);
  //
  return;
}
//
//
////////////////////////////////////////////////////////////////////////////
// TMB configuration register definitions and defaults
////////////////////////////////////////////////////////////////////////////
void TMB::DefineTMBConfigurationRegisters_(){ 
  //
  TMBConfigurationRegister.clear();
  //
  // Register used for ALCT configuration... 
  // N.B.  Do not write to this register during TMB configuration.  
  //       Included in TMBConfigurationRegister vector to allow writing of
  //       its data to the user prom.
  TMBConfigurationRegister.push_back(vme_usr_jtag_adr);     //0x10 ALCT JTAG address
  //
  //
  // Registers used for TMB configuration....
  //
  // Enable/configure inputs and injectors:
  TMBConfigurationRegister.push_back(vme_loopbk_adr);       //0x0e enable ALCT LVDS rx/tx
  TMBConfigurationRegister.push_back(alct_inj_adr  );       //0x32 mask ALCT
  TMBConfigurationRegister.push_back(cfeb_inj_adr  );       //0x42 enable CFEB inputs
  TMBConfigurationRegister.push_back(rpc_cfg_adr   );       //0xB6 enable RPC 
  TMBConfigurationRegister.push_back(rpc_inj_adr   );       //0xBC mask RPC
  //
  // trigger and signal delays:
  TMBConfigurationRegister.push_back(seq_trig_dly0_adr);    //0x6A ALCT*CLCT pretrigger source delays
  TMBConfigurationRegister.push_back(seq_trig_dly1_adr);    //0x6C External and Layer-Trigger source delays
  TMBConfigurationRegister.push_back(seq_l1a_adr      );    //0x74 L1A accept window width/delay
  TMBConfigurationRegister.push_back(seq_offset_adr   );    //0x76 L1A, TMB BXN offsets
  TMBConfigurationRegister.push_back(tmbtim_adr       );    //0xB2 ALCT*CLCT trigger coincidence timing, MPC tx delay 
  TMBConfigurationRegister.push_back(rpc_raw_delay_adr);    //0xBA RPC Raw Hits delay
  //
  // trigger configuration:
  TMBConfigurationRegister.push_back(seq_trig_en_adr        );   //0x68 sequencer trigger source enables
  TMBConfigurationRegister.push_back(seq_clct_adr           );   //0x70 CLCT sequencer configuration
  TMBConfigurationRegister.push_back(seq_fifo_adr           );   //0x72 sequencer fifo configuration
  TMBConfigurationRegister.push_back(tmb_trig_adr           );   //0x86 TMB trigger configuration/MPC accept, delays
  TMBConfigurationRegister.push_back(seqmod_adr             );   //0xAC sequencer Trigger modifiers
  TMBConfigurationRegister.push_back(layer_trg_mode_adr     );   //0xF0 Layer-Trigger mode
  TMBConfigurationRegister.push_back(pattern_find_pretrg_adr);   //0xF4 CLCT pattern-finder operation 
  TMBConfigurationRegister.push_back(clct_separation_adr    );   //0xF6 CLCT separation 
  //
  // special modifiers:
  TMBConfigurationRegister.push_back(ccb_trig_adr);         //0x2c configure request l1a from CCB 
  TMBConfigurationRegister.push_back(alct_cfg_adr);         //0x30 configure ALCT
  TMBConfigurationRegister.push_back(seq_id_adr  );         //0x6E board, csc ID 
  //
  // clock phases:
  TMBConfigurationRegister.push_back(vme_ddd0_adr     );    //0x16 phases: RAT/TMB, DMBtx, ALCTrx, ALCTtx
  TMBConfigurationRegister.push_back(vme_ddd1_adr     );    //0x18 phases: CFEB0, DCC, MPC, TMB1
  TMBConfigurationRegister.push_back(vme_ddd2_adr     );    //0x1a phases: CFEB4, CFEB3, CFEB2, CFEB1
  TMBConfigurationRegister.push_back(rat_3d_delays_adr);    //0xE6 phases: RPC1/RAT, RPC0/RAT 
  //
  // hot channel masks:
  TMBConfigurationRegister.push_back(hcm001_adr);  //0x4A distrip hot channel mask CFEB 0 layers 0,1 
  TMBConfigurationRegister.push_back(hcm023_adr);  //0x4C distrip hot channel mask CFEB 0 layers 2,3 
  TMBConfigurationRegister.push_back(hcm045_adr);  //0x4E distrip hot channel mask CFEB 0 layers 4,5 
  TMBConfigurationRegister.push_back(hcm101_adr);  //0x50 distrip hot channel mask CFEB 1 layers 0,1 
  TMBConfigurationRegister.push_back(hcm123_adr);  //0x52 distrip hot channel mask CFEB 1 layers 2,3 
  TMBConfigurationRegister.push_back(hcm145_adr);  //0x54 distrip hot channel mask CFEB 1 layers 4,5 
  TMBConfigurationRegister.push_back(hcm201_adr);  //0x56 distrip hot channel mask CFEB 2 layers 0,1 
  TMBConfigurationRegister.push_back(hcm223_adr);  //0x58 distrip hot channel mask CFEB 2 layers 2,3 
  TMBConfigurationRegister.push_back(hcm245_adr);  //0x5A distrip hot channel mask CFEB 2 layers 4,5 
  TMBConfigurationRegister.push_back(hcm301_adr);  //0x5C distrip hot channel mask CFEB 3 layers 0,1 
  TMBConfigurationRegister.push_back(hcm323_adr);  //0x5E distrip hot channel mask CFEB 3 layers 2,3 
  TMBConfigurationRegister.push_back(hcm345_adr);  //0x60 distrip hot channel mask CFEB 3 layers 4,5 
  TMBConfigurationRegister.push_back(hcm401_adr);  //0x62 distrip hot channel mask CFEB 4 layers 0,1 
  TMBConfigurationRegister.push_back(hcm423_adr);  //0x64 distrip hot channel mask CFEB 4 layers 2,3 
  TMBConfigurationRegister.push_back(hcm445_adr);  //0x66 distrip hot channel mask CFEB 4 layers 4,5 
  //
  // Removed from configuration list as per UCLA request December 2006...
  //*****  TMBConfigurationRegister.push_back(vme_ratctrl_adr);      //0x1e RAT control 
  //
  return;
}
//
void TMB::SetTMBRegisterDefaults_() {
  //
  // Set the default write values to the configuration registers:
  //
  trgmode_ = CLCT_trigger;  
  //
  //-----------------------------------------------------------------
  //0X0E = ADR_LOOPBK:  Loop-Back Control Register
  //-----------------------------------------------------------------
  ALCT_input_     = ALCT_input_default    ;
  enable_alct_tx_ = enable_alct_tx_default;
  //
  //------------------------------------------------------------------
  //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  alct_tx_clock_delay_ = alct_tx_clock_delay_default;
  alct_rx_clock_delay_ = alct_rx_clock_delay_default;
  dmb_tx_delay_        = dmb_tx_delay_default       ;
  rat_tmb_delay_       = rat_tmb_delay_default      ;
  //
  //------------------------------------------------------------------
  //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  tmb1_phase_ = tmb1_phase_default;  
  mpc_phase_  = mpc_phase_default ;
  dcc_phase_  = dcc_phase_default ;
  cfeb0delay_ = cfeb0delay_default;
  //
  //------------------------------------------------------------------
  //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  cfeb1delay_ = cfeb1delay_default;
  cfeb2delay_ = cfeb2delay_default;
  cfeb3delay_ = cfeb3delay_default;
  cfeb4delay_ = cfeb4delay_default;
  //
  //------------------------------------------------------------------
  //0X1E = ADR_RATCTRL:  RAT Module Control
  //------------------------------------------------------------------
  rpc_sync_   = rpc_sync_default  ;
  shift_rpc_  = shift_rpc_default ;
  rat_dsn_en_ = rat_dsn_en_default;
  //
  //------------------------------------------------------------------
  //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
  //------------------------------------------------------------------
  alct_ext_trig_l1aen_    = alct_ext_trig_l1aen_default   ;
  clct_ext_trig_l1aen_    = clct_ext_trig_l1aen_default   ;
  request_l1a_            = request_l1a_default           ;
  alct_ext_trig_vme_      = alct_ext_trig_vme_default     ;         
  clct_ext_trig_vme_      = clct_ext_trig_vme_default     ;         
  ext_trig_both_          = ext_trig_both_default         ;         
  ccb_allow_bypass_       = ccb_allow_bypass_default      ;         
  ignore_ccb_startstop_   = ignore_ccb_startstop_default  ;         
  internal_l1a_delay_vme_ = internal_l1a_delay_vme_default;         
  //
  //------------------------------------------------------------------
  //0X30 = ADR_ALCT_CFG:  ALCT Configuration
  //------------------------------------------------------------------
  cfg_alct_ext_trig_en_   = cfg_alct_ext_trig_en_default  ;  
  cfg_alct_ext_inject_en_ = cfg_alct_ext_inject_en_default;
  cfg_alct_ext_trig_      = cfg_alct_ext_trig_default     ;    
  cfg_alct_ext_inject_    = cfg_alct_ext_inject_default   ;  
  alct_seq_cmd_           = alct_seq_cmd_default          ;         
  alct_clock_en_use_ccb_  = alct_clock_en_use_ccb_default ;
  alct_clock_en_use_vme_  = alct_clock_en_use_vme_default ;
  //
  //------------------------------------------------------------------
  //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
  //------------------------------------------------------------------
  alct_clear_      = alct_clear_default     ;
  alct_inject_mux_ = alct_inject_mux_default;
  alct_sync_clct_  = alct_sync_clct_default ;
  alct_inj_delay_  = alct_inj_delay_default ;
  //
  //------------------------------------------------------------------
  //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
  //------------------------------------------------------------------
  disableCLCTInputs_ = 0;                         //remove this?
  enableCLCTInputs_  = enableCLCTInputs_default ;
  cfeb_ram_sel_      = cfeb_ram_sel_default     ;
  cfeb_inj_en_sel_   = cfeb_inj_en_sel_default  ;
  start_pattern_inj_ = start_pattern_inj_default;
  //
  //------------------------------------------------------------------
  //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
  //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
  //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
  //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
  //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
  //------------------------------------------------------------------
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++) 
    for (int distrip=0; distrip<MAX_NUM_DISTRIPS_PER_LAYER; distrip++)
      hot_channel_mask_[layer][distrip] = hot_channel_mask_default;
  //
  //
  //------------------------------------------------------------------
  //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
  //------------------------------------------------------------------
  clct_pat_trig_en_   = clct_pat_trig_en_default  ;
  alct_pat_trig_en_   = alct_pat_trig_en_default  ;
  match_pat_trig_en_  = match_pat_trig_en_default ;
  adb_ext_trig_en_    = adb_ext_trig_en_default   ;  
  dmb_ext_trig_en_    = dmb_ext_trig_en_default   ;  
  clct_ext_trig_en_   = clct_ext_trig_en_default  ; 
  alct_ext_trig_en_   = alct_ext_trig_en_default  ; 
  vme_ext_trig_       = vme_ext_trig_default      ;  
  ext_trig_inject_    = ext_trig_inject_default   ;  
  all_cfeb_active_    = all_cfeb_active_default   ;  
  cfebs_enabled_      = cfebs_enabled_default     ;  
  cfeb_enable_source_ = cfeb_enable_source_default;
  //
  //------------------------------------------------------------------
  //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  alct_pretrig_width_ = alct_pretrig_width_default;
  alct_pretrig_delay_ = alct_pretrig_delay_default;
  alct_pattern_delay_ = alct_pattern_delay_default;
  adb_ext_trig_delay_ = adb_ext_trig_delay_default;
  //
  //------------------------------------------------------------------
  //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  dmb_ext_trig_delay_  = dmb_ext_trig_delay_default ;
  clct_ext_trig_delay_ = clct_ext_trig_delay_default;
  alct_ext_trig_delay_ = alct_ext_trig_delay_default;
  layer_trig_delay_    = layer_trig_delay_default   ;
  //
  //------------------------------------------------------------------
  //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
  //------------------------------------------------------------------
  tmb_slot_ = this->slot();     //the TMB was instantiated with this value from the parser
  csc_id_   = tmb_slot_/2;      //the index of the CSC served by this peripheral crate
  if (tmb_slot_ > 11 ) csc_id_--;
  run_id_   = run_id_default;
  //
  //------------------------------------------------------------------
  //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
  //------------------------------------------------------------------
  triad_persist_     = triad_persist_default    ;
  hs_pretrig_thresh_ = hs_pretrig_thresh_default;
  //ds_pretrig_thresh_ = ds_pretrig_thresh_default;
  min_hits_pattern_  = min_hits_pattern_default ;
  drift_delay_       = drift_delay_default      ;
  pretrigger_halt_   = pretrigger_halt_default  ;
  //
  //------------------------------------------------------------------
  //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
  //------------------------------------------------------------------
  fifo_mode_    = fifo_mode_default   ;
  fifo_tbins_   = fifo_tbins_default  ;
  fifo_pretrig_ = fifo_pretrig_default;
  //
  //------------------------------------------------------------------
  //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
  //------------------------------------------------------------------
  l1adelay_         = l1adelay_default        ;
  l1a_window_size_  = l1a_window_size_default ;
  tmb_l1a_internal_ = tmb_l1a_internal_default;
  //
  //------------------------------------------------------------------
  //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
  //------------------------------------------------------------------
  l1a_offset_ = l1a_offset_default;
  bxn_offset_ = bxn_offset_default;
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  tmb_sync_err_enable_ = tmb_sync_err_enable_default;
  tmb_allow_alct_      = tmb_allow_alct_default     ;
  tmb_allow_clct_      = tmb_allow_clct_default     ;
  tmb_allow_match_     = tmb_allow_match_default    ;
  mpc_delay_           = mpc_delay_default          ;
  mpc_sel_ttc_bx0_     = mpc_sel_ttc_bx0_default    ;
  mpc_idle_blank_      = mpc_idle_blank_default     ;
  mpc_output_enable_   = mpc_output_enable_default  ;
  //
  //------------------------------------------------------------------
  //0XA2 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM control
  //------------------------------------------------------------------
  alct_raw_reset_        = alct_raw_reset_default       ;
  alct_raw_read_address_ = alct_raw_read_address_default;
  alct_raw_sync_     = alct_raw_sync_default    ;
  alct_demux_mode_       = alct_demux_mode_default      ;
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  clct_flush_delay_    = clct_flush_delay_default   ;
  clct_turbo_          = clct_turbo_default         ;
  ranlct_enable_       = ranlct_enable_default      ;
  wrt_buf_required_    = wrt_buf_required_default   ;
  valid_clct_required_ = valid_clct_required_default;
  l1a_allow_match_     = l1a_allow_match_default    ;
  l1a_allow_notmb_     = l1a_allow_notmb_default    ;
  l1a_allow_nol1a_     = l1a_allow_nol1a_default    ;
  l1a_allow_alct_only_ = l1a_allow_alct_only_default;
  scint_veto_clr_      = scint_veto_clr_default     ;
  //
  //------------------------------------------------------------------
  //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
  //------------------------------------------------------------------
  alct_vpf_delay_         = alct_vpf_delay_default        ;
  alct_match_window_size_ = alct_match_window_size_default;
  mpc_tx_delay_           = mpc_tx_delay_default          ;
  //
  //------------------------------------------------------------------
  //0XB6 = ADR_RPC_CFG:  RPC Configuration
  //------------------------------------------------------------------
  rpc_exists_      = rpc_exists_default     ;
  rpc_read_enable_ = rpc_read_enable_default;
  rpc_bxn_offset_  = rpc_bxn_offset_default ;
  rpc_bank_        = rpc_bank_default       ;
  //
  //------------------------------------------------------------------
  //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
  //------------------------------------------------------------------
  rpc0_raw_delay_ = rpc0_raw_delay_default;
  rpc1_raw_delay_ = rpc1_raw_delay_default;
  //
  //------------------------------------------------------------------
  //0XBC = ADR_RPC_INJ:  RPC Injector Control
  //------------------------------------------------------------------
  rpc_mask_all_  = rpc_mask_all_default ; 
  inj_mask_rat_  = inj_mask_rat_default ;
  inj_mask_rpc_  = inj_mask_rpc_default ;
  inj_delay_rat_ = inj_delay_rat_default;
  rpc_inj_sel_   = rpc_inj_sel_default  ;
  rpc_inj_wdata_ = rpc_inj_wdata_default;
  //
  //------------------------------------------------------------------
  //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
  //------------------------------------------------------------------
  jtag_disable_write_to_adr10_ = jtag_disable_write_to_adr10_default;
  //
  //------------------------------------------------------------------
  //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
  //------------------------------------------------------------------
  rpc0_rat_delay_ = rpc0_rat_delay_default;
  rpc1_rat_delay_ = rpc1_rat_delay_default;
  //
  //---------------------------------------------------------------------
  //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
  //---------------------------------------------------------------------
  layer_trigger_en_  = layer_trigger_en_default ; 
  layer_trig_thresh_ = layer_trig_thresh_default;
  //
  //---------------------------------------------------------------------
  //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
  //---------------------------------------------------------------------
  clct_blanking_          = clct_blanking_default         ; 
  clct_stagger_           = clct_stagger_default          ; 
  clct_pattern_id_thresh_ = clct_pattern_id_thresh_default; 
  aff_thresh_             = aff_thresh_default            ; 
  //
  //---------------------------------------------------------------------
  //0XF6 = ADR_TEMP1:  CLCT separation
  //---------------------------------------------------------------------
  clct_separation_src_              = clct_separation_src_default             ; 
  clct_separation_ram_write_enable_ = clct_separation_ram_write_enable_default; 
  clct_separation_ram_adr_          = clct_separation_ram_adr_default         ; 
  min_clct_separation_              = min_clct_separation_default             ; 
  //
  return;
}
//
//
////////////////////////////////////////////////////////////////////////////////////////
// Decode registers read from TMB
////////////////////////////////////////////////////////////////////////////////////////
void TMB::DecodeTMBRegister_(unsigned long int address, int data) {
  //
  if ( address == vme_loopbk_adr ) {
    //------------------------------------------------------------------
    //0X0E = ADR_LOOPBK:  Loop-Back Control Register  
    //------------------------------------------------------------------
    read_cfeb_oe_        = ExtractValueFromData(data,cfeb_oe_bitlo       ,cfeb_oe_bithi       );
    read_alct_loop_      = ExtractValueFromData(data,alct_loop_bitlo     ,alct_loop_bithi     );
    read_ALCT_input_     = ExtractValueFromData(data,ALCT_input_bitlo    ,ALCT_input_bithi    );
    read_enable_alct_tx_ = ExtractValueFromData(data,enable_alct_tx_bitlo,enable_alct_tx_bithi);
    read_rpc_loop_rat_   = ExtractValueFromData(data,rpc_loop_rat_bitlo  ,rpc_loop_rat_bithi  );
    read_rpc_loop_tmb_   = ExtractValueFromData(data,rpc_loop_tmb_bitlo  ,rpc_loop_tmb_bithi  );
    read_dmb_loop_       = ExtractValueFromData(data,dmb_loop_bitlo      ,dmb_loop_bithi      );
    read_dmb_oe_         = ExtractValueFromData(data,dmb_oe_bitlo        ,dmb_oe_bithi        );
    read_gtl_loop_       = ExtractValueFromData(data,gtl_loop_bitlo      ,gtl_loop_bithi      );
    read_gtl_oe_         = ExtractValueFromData(data,gtl_oe_bitlo        ,gtl_oe_bithi        );
    //
  } else if ( address == vme_dddsm_adr ) {    
    //------------------------------------------------------------------
    //0X14 = ADR_DDDSM:  3D3444 State Machine Control + DCM Lock Status  
    //------------------------------------------------------------------
    read_ddd_state_machine_start_            = ExtractValueFromData(data,ddd_state_machine_start_bitlo     ,ddd_state_machine_start_bithi     );
    read_ddd_state_machine_manual_           = ExtractValueFromData(data,ddd_state_machine_manual_bitlo    ,ddd_state_machine_manual_bithi    );
    read_ddd_state_machine_latch_            = ExtractValueFromData(data,ddd_state_machine_latch_bitlo     ,ddd_state_machine_latch_bithi     );
    read_ddd_state_machine_serial_in_        = ExtractValueFromData(data,ddd_state_machine_serial_in_bitlo ,ddd_state_machine_serial_in_bithi );
    read_ddd_state_machine_serial_out_       = ExtractValueFromData(data,ddd_state_machine_serial_out_bitlo,ddd_state_machine_serial_out_bithi);
    read_ddd_state_machine_autostart_        = ExtractValueFromData(data,ddd_state_machine_autostart_bitlo ,ddd_state_machine_autostart_bithi );
    read_ddd_state_machine_busy_             = ExtractValueFromData(data,ddd_state_machine_busy_bitlo      ,ddd_state_machine_busy_bithi      );
    read_ddd_state_machine_verify_ok_        = ExtractValueFromData(data,ddd_state_machine_verify_ok_bitlo ,ddd_state_machine_verify_ok_bithi );
    read_ddd_state_machine_clock0_lock_      = ExtractValueFromData(data,ddd_state_machine_clock0_lock_bitlo,
								    ddd_state_machine_clock0_lock_bithi);
    read_ddd_state_machine_clock0d_lock_     = ExtractValueFromData(data,ddd_state_machine_clock0d_lock_bitlo,
								    ddd_state_machine_clock0d_lock_bithi);
    read_ddd_state_machine_clock1_lock_      = ExtractValueFromData(data,ddd_state_machine_clock1_lock_bitlo,
								    ddd_state_machine_clock1_lock_bithi);
    read_ddd_state_machine_clock_alct_lock_  = ExtractValueFromData(data,ddd_state_machine_clock_alct_lock_bitlo,
								    ddd_state_machine_clock_alct_lock_bithi);
    read_ddd_state_machine_clockd_alct_lock_ = ExtractValueFromData(data,ddd_state_machine_clockd_alct_lock_bitlo,
								    ddd_state_machine_clockd_alct_lock_bithi);
    read_ddd_state_machine_clock_mpc_lock_   = ExtractValueFromData(data,ddd_state_machine_clock_mpc_lock_bitlo,
								    ddd_state_machine_clock_mpc_lock_bithi);
    read_ddd_state_machine_clock_dcc_lock_   = ExtractValueFromData(data,ddd_state_machine_clock_dcc_lock_bitlo,
								    ddd_state_machine_clock_dcc_lock_bithi);
    read_ddd_state_machine_clock_rpc_lock_   = ExtractValueFromData(data,ddd_state_machine_clock_rpc_lock_bitlo,
								    ddd_state_machine_clock_rpc_lock_bithi);
    //
  } else if ( address == vme_ddd0_adr ) {
    //------------------------------------------------------------------
    //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    read_alct_tx_clock_delay_ = ExtractValueFromData(data,alct_tx_clock_delay_bitlo,alct_tx_clock_delay_bithi);
    read_alct_rx_clock_delay_ = ExtractValueFromData(data,alct_rx_clock_delay_bitlo,alct_rx_clock_delay_bithi);
    read_dmb_tx_delay_        = ExtractValueFromData(data,dmb_tx_delay_bitlo       ,dmb_tx_delay_bithi       );
    read_rat_tmb_delay_       = ExtractValueFromData(data,rat_tmb_delay_bitlo      ,rat_tmb_delay_bithi      );
    //
  } else if ( address == vme_ddd1_adr ) {
    //------------------------------------------------------------------
    //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    read_tmb1_phase_ = ExtractValueFromData(data,tmb1_phase_bitlo,tmb1_phase_bithi);
    read_mpc_phase_  = ExtractValueFromData(data,mpc_phase_bitlo ,mpc_phase_bithi );
    read_dcc_phase_  = ExtractValueFromData(data,dcc_phase_bitlo ,dcc_phase_bithi );
    read_cfeb0delay_ = ExtractValueFromData(data,cfeb0delay_bitlo,cfeb0delay_bithi);
    //
  } else if ( address == vme_ddd2_adr ) {
    //------------------------------------------------------------------
    //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    read_cfeb1delay_ = ExtractValueFromData(data,cfeb1delay_bitlo,cfeb1delay_bithi);
    read_cfeb2delay_ = ExtractValueFromData(data,cfeb2delay_bitlo,cfeb2delay_bithi);
    read_cfeb3delay_ = ExtractValueFromData(data,cfeb3delay_bitlo,cfeb3delay_bithi);
    read_cfeb4delay_ = ExtractValueFromData(data,cfeb4delay_bitlo,cfeb4delay_bithi);
    //
  } else if ( address == vme_ratctrl_adr ) {
    //------------------------------------------------------------------
    //0X1E = ADR_RATCTRL:  RAT Module Control
    //------------------------------------------------------------------
    read_rpc_sync_   = ExtractValueFromData(data,rpc_sync_bitlo  ,rpc_sync_bithi  );
    read_shift_rpc_  = ExtractValueFromData(data,shift_rpc_bitlo ,shift_rpc_bithi );
    read_rat_dsn_en_ = ExtractValueFromData(data,rat_dsn_en_bitlo,rat_dsn_en_bithi);
    //
  } else if ( address == ccb_trig_adr ) {
    //------------------------------------------------------------------
    //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
    //------------------------------------------------------------------
    read_alct_ext_trig_l1aen_    = ExtractValueFromData(data,alct_ext_trig_l1aen_bitlo   ,alct_ext_trig_l1aen_bithi   );
    read_clct_ext_trig_l1aen_    = ExtractValueFromData(data,clct_ext_trig_l1aen_bitlo   ,clct_ext_trig_l1aen_bithi   );
    read_request_l1a_            = ExtractValueFromData(data,request_l1a_bitlo           ,request_l1a_bithi           );
    read_alct_ext_trig_vme_      = ExtractValueFromData(data,alct_ext_trig_vme_bitlo     ,alct_ext_trig_vme_bithi     );
    read_clct_ext_trig_vme_      = ExtractValueFromData(data,clct_ext_trig_vme_bitlo     ,clct_ext_trig_vme_bithi     );
    read_ext_trig_both_          = ExtractValueFromData(data,ext_trig_both_bitlo         ,ext_trig_both_bithi         );
    read_ccb_allow_bypass_       = ExtractValueFromData(data,ccb_allow_bypass_bitlo      ,ccb_allow_bypass_bithi      );
    read_ignore_ccb_startstop_   = ExtractValueFromData(data,ignore_ccb_startstop_bitlo  ,ignore_ccb_startstop_bithi  );
    read_internal_l1a_delay_vme_ = ExtractValueFromData(data,internal_l1a_delay_vme_bitlo,internal_l1a_delay_vme_bithi);
    //
  } else if ( address == alct_cfg_adr ) {
    //------------------------------------------------------------------
    //0X30 = ADR_ALCT_CFG:  ALCT Configuration
    //------------------------------------------------------------------
    read_cfg_alct_ext_trig_en_   = ExtractValueFromData(data,cfg_alct_ext_trig_en_bitlo  ,cfg_alct_ext_trig_en_bithi  ); 
    read_cfg_alct_ext_inject_en_ = ExtractValueFromData(data,cfg_alct_ext_inject_en_bitlo,cfg_alct_ext_inject_en_bithi); 
    read_cfg_alct_ext_trig_      = ExtractValueFromData(data,cfg_alct_ext_trig_bitlo     ,cfg_alct_ext_trig_bithi     ); 
    read_cfg_alct_ext_inject_    = ExtractValueFromData(data,cfg_alct_ext_inject_bitlo   ,cfg_alct_ext_inject_bithi   ); 
    read_alct_seq_cmd_           = ExtractValueFromData(data,alct_seq_cmd_bitlo          ,alct_seq_cmd_bithi          ); 
    read_alct_clock_en_use_ccb_  = ExtractValueFromData(data,alct_clock_en_use_ccb_bitlo ,alct_clock_en_use_ccb_bithi ); 
    read_alct_clock_en_use_vme_  = ExtractValueFromData(data,alct_clock_en_use_vme_bitlo ,alct_clock_en_use_vme_bithi ); 
    //
  } else if ( address == alct_inj_adr ) {
    //------------------------------------------------------------------
    //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
    //------------------------------------------------------------------
    read_alct_clear_      = ExtractValueFromData(data,alct_clear_bitlo     ,alct_clear_bithi     );
    read_alct_inject_mux_ = ExtractValueFromData(data,alct_inject_mux_bitlo,alct_inject_mux_bithi);
    read_alct_sync_clct_  = ExtractValueFromData(data,alct_sync_clct_bitlo ,alct_sync_clct_bithi );
    read_alct_inj_delay_  = ExtractValueFromData(data,alct_inj_delay_bitlo ,alct_inj_delay_bithi );
    //
  } else if ( address == alct_fifo_adr ) {
    //------------------------------------------------------------------
    //0X3E = ADR_ALCT_FIFO:  ALCT FIFO RAM Status
    //------------------------------------------------------------------
    read_alct_raw_busy_       = ExtractValueFromData(data,alct_raw_busy_bitlo      ,alct_raw_busy_bithi      );
    read_alct_raw_done_       = ExtractValueFromData(data,alct_raw_done_bitlo      ,alct_raw_done_bithi      );
    read_alct_raw_word_count_ = ExtractValueFromData(data,alct_raw_word_count_bitlo,alct_raw_word_count_bithi);
    read_alct_raw_msbs_       = ExtractValueFromData(data,alct_raw_msbs_bitlo      ,alct_raw_msbs_bithi      );
    //
  } else if ( address == cfeb_inj_adr ) {
    //------------------------------------------------------------------
    //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
    //------------------------------------------------------------------
    read_enableCLCTInputs_  = ExtractValueFromData(data,enableCLCTInputs_bitlo ,enableCLCTInputs_bithi );
    read_cfeb_ram_sel_      = ExtractValueFromData(data,cfeb_ram_sel_bitlo     ,cfeb_ram_sel_bithi     );
    read_cfeb_inj_en_sel_   = ExtractValueFromData(data,cfeb_inj_en_sel_bitlo  ,cfeb_inj_en_sel_bithi  );
    read_start_pattern_inj_ = ExtractValueFromData(data,start_pattern_inj_bitlo,start_pattern_inj_bithi);
    //
  } else if ( address == hcm001_adr || address == hcm023_adr || address == hcm045_adr ||
	      address == hcm101_adr || address == hcm123_adr || address == hcm145_adr ||
	      address == hcm201_adr || address == hcm223_adr || address == hcm245_adr ||
	      address == hcm301_adr || address == hcm323_adr || address == hcm345_adr ||
	      address == hcm401_adr || address == hcm423_adr || address == hcm445_adr ) {
    //------------------------------------------------------------------
    //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
    //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
    //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
    //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
    //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
    //------------------------------------------------------------------
    for (int bit_in_register=0; bit_in_register<16; bit_in_register++) {
      //
      // get the layer and distrip channels covered by this register
      int layer   = GetHotChannelLayerFromMap_(address,bit_in_register);
      int distrip = GetHotChannelDistripFromMap_(address,bit_in_register);
      //
      read_hot_channel_mask_[layer][distrip] = ExtractValueFromData(data,bit_in_register,bit_in_register);
    }
    //
  } else if ( address == seq_trig_en_adr ) {
    //------------------------------------------------------------------
    //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
    //------------------------------------------------------------------
    read_clct_pat_trig_en_   = ExtractValueFromData(data,clct_pat_trig_en_bitlo  ,clct_pat_trig_en_bithi  );
    read_alct_pat_trig_en_   = ExtractValueFromData(data,alct_pat_trig_en_bitlo  ,alct_pat_trig_en_bithi  );
    read_match_pat_trig_en_  = ExtractValueFromData(data,match_pat_trig_en_bitlo ,match_pat_trig_en_bithi );
    read_adb_ext_trig_en_    = ExtractValueFromData(data,adb_ext_trig_en_bitlo   ,adb_ext_trig_en_bithi   );
    read_dmb_ext_trig_en_    = ExtractValueFromData(data,dmb_ext_trig_en_bitlo   ,dmb_ext_trig_en_bithi   );
    read_clct_ext_trig_en_   = ExtractValueFromData(data,clct_ext_trig_en_bitlo  ,clct_ext_trig_en_bithi  );
    read_alct_ext_trig_en_   = ExtractValueFromData(data,alct_ext_trig_en_bitlo  ,alct_ext_trig_en_bithi  );
    read_vme_ext_trig_       = ExtractValueFromData(data,vme_ext_trig_bitlo      ,vme_ext_trig_bithi      );
    read_ext_trig_inject_    = ExtractValueFromData(data,ext_trig_inject_bitlo   ,ext_trig_inject_bithi   );
    read_all_cfeb_active_    = ExtractValueFromData(data,all_cfeb_active_bitlo   ,all_cfeb_active_bithi   );
    read_cfebs_enabled_      = ExtractValueFromData(data,cfebs_enabled_bitlo     ,cfebs_enabled_bithi     );
    read_cfeb_enable_source_ = ExtractValueFromData(data,cfeb_enable_source_bitlo,cfeb_enable_source_bithi);
    //    
  } else if ( address == seq_trig_dly0_adr ) {
    //------------------------------------------------------------------
    //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    read_alct_pretrig_width_ = ExtractValueFromData(data,alct_pretrig_width_bitlo,alct_pretrig_width_bithi);
    read_alct_pretrig_delay_ = ExtractValueFromData(data,alct_pretrig_delay_bitlo,alct_pretrig_delay_bithi);
    read_alct_pattern_delay_ = ExtractValueFromData(data,alct_pattern_delay_bitlo,alct_pattern_delay_bithi);
    read_adb_ext_trig_delay_ = ExtractValueFromData(data,adb_ext_trig_delay_bitlo,adb_ext_trig_delay_bithi);
    //
  } else if ( address == seq_trig_dly1_adr ) {
    //------------------------------------------------------------------
    //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    read_dmb_ext_trig_delay_    =  ExtractValueFromData(data,dmb_ext_trig_delay_bitlo ,dmb_ext_trig_delay_bithi );
    read_clct_ext_trig_delay_   =  ExtractValueFromData(data,clct_ext_trig_delay_bitlo,clct_ext_trig_delay_bithi);
    read_alct_ext_trig_delay_   =  ExtractValueFromData(data,alct_ext_trig_delay_bitlo,alct_ext_trig_delay_bithi);
    read_layer_trig_delay_      =  ExtractValueFromData(data,layer_trig_delay_bitlo   ,layer_trig_delay_bithi   );
    //
  } else if ( address == seq_id_adr ) {
    //------------------------------------------------------------------
    //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
    //------------------------------------------------------------------
    read_tmb_slot_ = ExtractValueFromData(data,tmb_slot_bitlo,tmb_slot_bithi);
    read_csc_id_   = ExtractValueFromData(data,csc_id_bitlo  ,csc_id_bithi  );
    read_run_id_   = ExtractValueFromData(data,run_id_bitlo  ,run_id_bithi  );
    //
  } else if ( address == seq_clct_adr ) {
    //------------------------------------------------------------------
    //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
    //------------------------------------------------------------------
    read_triad_persist_     = ExtractValueFromData(data,triad_persist_bitlo    ,triad_persist_bithi    );
    read_hs_pretrig_thresh_ = ExtractValueFromData(data,hs_pretrig_thresh_bitlo,hs_pretrig_thresh_bithi);
    //read_ds_pretrig_thresh_ = ExtractValueFromData(data,ds_pretrig_thresh_bitlo,ds_pretrig_thresh_bithi);
    read_min_hits_pattern_  = ExtractValueFromData(data,min_hits_pattern_bitlo ,min_hits_pattern_bithi );
    read_drift_delay_       = ExtractValueFromData(data,drift_delay_bitlo      ,drift_delay_bithi      );
    read_pretrigger_halt_   = ExtractValueFromData(data,pretrigger_halt_bitlo  ,pretrigger_halt_bithi  );
    //
  } else if ( address == seq_fifo_adr ) {
    //------------------------------------------------------------------
    //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
    //------------------------------------------------------------------
    read_fifo_mode_    = ExtractValueFromData(data,fifo_mode_bitlo   ,fifo_mode_bithi   );
    read_fifo_tbins_   = ExtractValueFromData(data,fifo_tbins_bitlo  ,fifo_tbins_bithi  );
    read_fifo_pretrig_ = ExtractValueFromData(data,fifo_pretrig_bitlo,fifo_pretrig_bithi);
    //
  } else if ( address == seq_l1a_adr ) {
    //------------------------------------------------------------------
    //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
    //------------------------------------------------------------------
    read_l1adelay_         = ExtractValueFromData(data,l1adelay_bitlo        ,l1adelay_bithi        );
    read_l1a_window_size_  = ExtractValueFromData(data,l1a_window_size_bitlo ,l1a_window_size_bithi );
    read_tmb_l1a_internal_ = ExtractValueFromData(data,tmb_l1a_internal_bitlo,tmb_l1a_internal_bithi);
    //
  } else if ( address == seq_offset_adr ) {
    //------------------------------------------------------------------
    //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
    //------------------------------------------------------------------
    read_l1a_offset_ = ExtractValueFromData(data,l1a_offset_bitlo,l1a_offset_bithi);
    read_bxn_offset_ = ExtractValueFromData(data,bxn_offset_bitlo,bxn_offset_bithi);
    //    
  } else if ( address == tmb_trig_adr ) {
    //------------------------------------------------------------------
    //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
    //------------------------------------------------------------------
    read_tmb_sync_err_enable_ = ExtractValueFromData(data,tmb_sync_err_enable_bitlo,tmb_sync_err_enable_bithi);
    read_tmb_allow_alct_      = ExtractValueFromData(data,tmb_allow_alct_bitlo     ,tmb_allow_alct_bithi     );
    read_tmb_allow_clct_      = ExtractValueFromData(data,tmb_allow_clct_bitlo     ,tmb_allow_clct_bithi     );
    read_tmb_allow_match_     = ExtractValueFromData(data,tmb_allow_match_bitlo    ,tmb_allow_match_bithi    );
    read_mpc_delay_           = ExtractValueFromData(data,mpc_delay_bitlo          ,mpc_delay_bithi          );
    read_mpc_accept_          = ExtractValueFromData(data,mpc_accept_bitlo         ,mpc_accept_bithi         );
    read_mpc_reserved_        = ExtractValueFromData(data,mpc_reserved_bitlo       ,mpc_reserved_bithi       );
    read_mpc_sel_ttc_bx0_     = ExtractValueFromData(data,mpc_sel_ttc_bx0_bitlo    ,mpc_sel_ttc_bx0_bithi    );
    read_mpc_idle_blank_      = ExtractValueFromData(data,mpc_idle_blank_bitlo     ,mpc_idle_blank_bithi     );
    read_mpc_output_enable_   = ExtractValueFromData(data,mpc_output_enable_bitlo  ,mpc_output_enable_bithi  );
    //
  } else if ( address == alctfifo1_adr ) {
    //------------------------------------------------------------------
    //0XA2 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM control 
    //------------------------------------------------------------------
    read_alct_raw_reset_        = ExtractValueFromData(data,alct_raw_reset_bitlo       ,alct_raw_reset_bithi       );
    read_alct_raw_read_address_ = ExtractValueFromData(data,alct_raw_read_address_bitlo,alct_raw_read_address_bithi);
    read_alct_raw_sync_         = ExtractValueFromData(data,alct_raw_sync_bitlo        ,alct_raw_sync_bithi        );
    read_alct_demux_mode_       = ExtractValueFromData(data,alct_demux_mode_bitlo      ,alct_demux_mode_bithi      );
    //
  } else if ( address == alctfifo2_adr ) {
    //------------------------------------------------------------------
    //0XA4 = ADR_ALCTFIFO2:  ALCT Raw Hits RAM data 
    //------------------------------------------------------------------
    read_alct_raw_lsbs_ = ExtractValueFromData(data,alct_raw_lsbs_bitlo,alct_raw_lsbs_bithi);
    //
  } else if ( address == seqmod_adr ) {
    //------------------------------------------------------------------
    //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
    //------------------------------------------------------------------
    read_clct_flush_delay_    = ExtractValueFromData(data,clct_flush_delay_bitlo   ,clct_flush_delay_bithi   );
    read_clct_turbo_          = ExtractValueFromData(data,clct_turbo_bitlo         ,clct_turbo_bithi         );
    read_ranlct_enable_       = ExtractValueFromData(data,ranlct_enable_bitlo      ,ranlct_enable_bithi      );
    read_wrt_buf_required_    = ExtractValueFromData(data,wrt_buf_required_bitlo   ,wrt_buf_required_bithi   );
    read_valid_clct_required_ = ExtractValueFromData(data,valid_clct_required_bitlo,valid_clct_required_bithi);
    read_l1a_allow_match_     = ExtractValueFromData(data,l1a_allow_match_bitlo    ,l1a_allow_match_bithi    );
    read_l1a_allow_notmb_     = ExtractValueFromData(data,l1a_allow_notmb_bitlo    ,l1a_allow_notmb_bithi    );
    read_l1a_allow_nol1a_     = ExtractValueFromData(data,l1a_allow_nol1a_bitlo    ,l1a_allow_nol1a_bithi    );
    read_l1a_allow_alct_only_ = ExtractValueFromData(data,l1a_allow_alct_only_bitlo,l1a_allow_alct_only_bithi);
    read_scint_veto_clr_      = ExtractValueFromData(data,scint_veto_clr_bitlo     ,scint_veto_clr_bithi     );
    read_scint_veto_vme_      = ExtractValueFromData(data,scint_veto_vme_bitlo     ,scint_veto_vme_bithi     );
    //
  } else if ( address == seqsm_adr ) {
    //------------------------------------------------------------------
    //0XAE = ADR_SEQSM:  Sequencer Machine State
    //------------------------------------------------------------------
    read_clct_state_machine_      = ExtractValueFromData(data,clct_state_machine_bitlo     ,clct_state_machine_bithi     );
    read_tmb_match_state_machine_ = ExtractValueFromData(data,tmb_match_state_machine_bitlo,tmb_match_state_machine_bithi);
    read_readout_state_machine_   = ExtractValueFromData(data,readout_state_machine_bitlo  ,readout_state_machine_bithi  );
    read_readout_stack_full_      = ExtractValueFromData(data,readout_stack_full_bitlo     ,readout_stack_full_bithi     );
    read_readout_stack_empty_     = ExtractValueFromData(data,readout_stack_empty_bitlo    ,readout_stack_empty_bithi    );
    read_readout_stack_overflow_  = ExtractValueFromData(data,readout_stack_overflow_bitlo ,readout_stack_overflow_bithi );
    read_readout_stack_underflow_ = ExtractValueFromData(data,readout_stack_underflow_bitlo,readout_stack_underflow_bithi);
    //
  } else if ( address == tmbtim_adr ) {
    //------------------------------------------------------------------
    //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
    //------------------------------------------------------------------
    read_alct_vpf_delay_         = ExtractValueFromData(data,alct_vpf_delay_bitlo        ,alct_vpf_delay_bithi        );
    read_alct_match_window_size_ = ExtractValueFromData(data,alct_match_window_size_bitlo,alct_match_window_size_bithi);
    read_mpc_tx_delay_           = ExtractValueFromData(data,mpc_tx_delay_bitlo          ,mpc_tx_delay_bithi          );
    //
  } else if ( address == rpc_cfg_adr ) {
    //------------------------------------------------------------------
    //0XB6 = ADR_RPC_CFG:  RPC Configuration
    //------------------------------------------------------------------
    read_rpc_exists_      = ExtractValueFromData(data,rpc_exists_bitlo     ,rpc_exists_bithi     );
    read_rpc_read_enable_ = ExtractValueFromData(data,rpc_read_enable_bitlo,rpc_read_enable_bithi);
    read_rpc_bxn_offset_  = ExtractValueFromData(data,rpc_bxn_offset_bitlo ,rpc_bxn_offset_bithi );
    read_rpc_bank_        = ExtractValueFromData(data,rpc_bank_bitlo       ,rpc_bank_bithi       );
    read_rpc_rbxn_        = ExtractValueFromData(data,rpc_rbxn_bitlo       ,rpc_rbxn_bithi       );
    read_rpc_done_        = ExtractValueFromData(data,rpc_done_bitlo       ,rpc_done_bithi       );
    //
  } else if ( address == rpc_raw_delay_adr ) {
    //------------------------------------------------------------------
    //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
    //------------------------------------------------------------------
    read_rpc0_raw_delay_ = ExtractValueFromData(data,rpc0_raw_delay_bitlo,rpc0_raw_delay_bithi);
    read_rpc1_raw_delay_ = ExtractValueFromData(data,rpc1_raw_delay_bitlo,rpc1_raw_delay_bithi);
    //
  } else if ( address == rpc_inj_adr ) {
    //------------------------------------------------------------------
    //0XBC = ADR_RPC_INJ:  RPC Injector Control
    //------------------------------------------------------------------
    read_rpc_mask_all_  = ExtractValueFromData(data,rpc_mask_all_bitlo ,rpc_mask_all_bithi );  
    read_inj_mask_rat_  = ExtractValueFromData(data,inj_mask_rat_bitlo ,inj_mask_rat_bithi );  
    read_inj_mask_rpc_  = ExtractValueFromData(data,inj_mask_rpc_bitlo ,inj_mask_rpc_bithi );  
    read_inj_delay_rat_ = ExtractValueFromData(data,inj_delay_rat_bitlo,inj_delay_rat_bithi); 
    read_rpc_inj_sel_   = ExtractValueFromData(data,rpc_inj_sel_bitlo  ,rpc_inj_sel_bithi  );   
    read_rpc_inj_wdata_ = ExtractValueFromData(data,rpc_inj_wdata_bitlo,rpc_inj_wdata_bithi); 
    read_rpc_inj_rdata_ = ExtractValueFromData(data,rpc_inj_rdata_bitlo,rpc_inj_rdata_bithi); 
    //
  } else if ( address == jtag_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
    //------------------------------------------------------------------
    read_jtag_state_machine_start_       = ExtractValueFromData(data,jtag_state_machine_start_bitlo      ,jtag_state_machine_start_bithi      );
    read_jtag_state_machine_sreset_      = ExtractValueFromData(data,jtag_state_machine_sreset_bitlo     ,jtag_state_machine_sreset_bithi     );
    read_jtag_state_machine_autostart_   = ExtractValueFromData(data,jtag_state_machine_autostart_bitlo  ,jtag_state_machine_autostart_bithi  );
    read_jtag_state_machine_busy_        = ExtractValueFromData(data,jtag_state_machine_busy_bitlo       ,jtag_state_machine_busy_bithi       );
    read_jtag_state_machine_aborted_     = ExtractValueFromData(data,jtag_state_machine_aborted_bitlo    ,jtag_state_machine_aborted_bithi    );
    read_jtag_state_machine_cksum_ok_    = ExtractValueFromData(data,jtag_state_machine_cksum_ok_bitlo   ,jtag_state_machine_cksum_ok_bithi   );
    read_jtag_state_machine_wdcnt_ok_    = ExtractValueFromData(data,jtag_state_machine_wdcnt_ok_bitlo   ,jtag_state_machine_wdcnt_ok_bithi   );
    read_jtag_state_machine_tck_fpga_ok_ = ExtractValueFromData(data,jtag_state_machine_tck_fpga_ok_bitlo,jtag_state_machine_tck_fpga_ok_bithi);
    read_jtag_state_machine_vme_ready_   = ExtractValueFromData(data,jtag_state_machine_vme_ready_bitlo  ,jtag_state_machine_vme_ready_bithi  );
    read_jtag_state_machine_ok_          = ExtractValueFromData(data,jtag_state_machine_ok_bitlo         ,jtag_state_machine_ok_bithi         );
    read_jtag_state_machine_oe_          = ExtractValueFromData(data,jtag_state_machine_oe_bitlo         ,jtag_state_machine_oe_bithi         );
    read_jtag_disable_write_to_adr10_    = ExtractValueFromData(data,jtag_disable_write_to_adr10_bitlo   ,jtag_disable_write_to_adr10_bithi   );
    read_jtag_state_machine_throttle_    = ExtractValueFromData(data,jtag_state_machine_throttle_bitlo   ,jtag_state_machine_throttle_bithi   );
    //
  } else if ( address == jtag_sm_wdcnt_adr ) {    
    //------------------------------------------------------------------
    //0XD6 = ADR_JTAGSM1:  JTAG State Machine Word Count
    //------------------------------------------------------------------
    read_jtag_state_machine_word_count_ = ExtractValueFromData(data,jtag_state_machine_word_count_bitlo,jtag_state_machine_word_count_bithi);
    //
  } else if ( address == jtag_sm_cksum_adr ) {
    //------------------------------------------------------------------
    //0XD8 = ADR_JTAGSM2:  JTAG State Machine Checksum
    //------------------------------------------------------------------
    read_jtag_state_machine_check_sum_ = ExtractValueFromData(data,jtag_state_machine_check_sum_bitlo,jtag_state_machine_check_sum_bithi);
    read_jtag_state_machine_tck_fpga_  = ExtractValueFromData(data,jtag_state_machine_tck_fpga_bitlo ,jtag_state_machine_tck_fpga_bithi );
    //
  } else if ( address == vme_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XDA = ADR_VMESM0:  VME State Machine Control (reads VME PROM)
    //------------------------------------------------------------------
    read_vme_state_machine_start_     = ExtractValueFromData(data,vme_state_machine_start_bitlo    ,vme_state_machine_start_bithi    );
    read_vme_state_machine_sreset_    = ExtractValueFromData(data,vme_state_machine_sreset_bitlo   ,vme_state_machine_sreset_bithi   );
    read_vme_state_machine_autostart_ = ExtractValueFromData(data,vme_state_machine_autostart_bitlo,vme_state_machine_autostart_bithi);
    read_vme_state_machine_busy_      = ExtractValueFromData(data,vme_state_machine_busy_bitlo     ,vme_state_machine_busy_bithi     );
    read_vme_state_machine_aborted_   = ExtractValueFromData(data,vme_state_machine_aborted_bitlo  ,vme_state_machine_aborted_bithi  );
    read_vme_state_machine_cksum_ok_  = ExtractValueFromData(data,vme_state_machine_cksum_ok_bitlo ,vme_state_machine_cksum_ok_bithi );
    read_vme_state_machine_wdcnt_ok_  = ExtractValueFromData(data,vme_state_machine_wdcnt_ok_bitlo ,vme_state_machine_wdcnt_ok_bithi );
    read_vme_state_machine_jtag_auto_ = ExtractValueFromData(data,vme_state_machine_jtag_auto_bitlo,vme_state_machine_jtag_auto_bithi);
    read_vme_state_machine_vme_ready_ = ExtractValueFromData(data,vme_state_machine_vme_ready_bitlo,vme_state_machine_vme_ready_bithi);
    read_vme_state_machine_ok_        = ExtractValueFromData(data,vme_state_machine_ok_bitlo       ,vme_state_machine_ok_bithi       );
    read_vme_state_machine_path_ok_   = ExtractValueFromData(data,vme_state_machine_path_ok_bitlo  ,vme_state_machine_path_ok_bithi  );
    read_vme_state_machine_throttle_  = ExtractValueFromData(data,vme_state_machine_throttle_bitlo ,vme_state_machine_throttle_bithi );
    //
  } else if ( address == vme_sm_wdcnt_adr ) {
    //------------------------------------------------------------------
    //0XDC = ADR_VMESM1:  VME State Machine Word Count
    //------------------------------------------------------------------
    read_vme_state_machine_word_count_ = ExtractValueFromData(data,vme_state_machine_word_count_bitlo,vme_state_machine_word_count_bithi);
    //
  } else if ( address == vme_sm_cksum_adr ) {
    //------------------------------------------------------------------
    //0XDE = ADR_VMESM2:  VME State Machine Checksum
    //------------------------------------------------------------------
    read_vme_state_machine_check_sum_                     = ExtractValueFromData(data,vme_state_machine_check_sum_bitlo,
										 vme_state_machine_check_sum_bithi);
    read_vme_state_machine_error_missing_header_start_    = ExtractValueFromData(data,vme_state_machine_error_missing_header_start_bitlo,
										 vme_state_machine_error_missing_header_start_bithi);
    read_vme_state_machine_error_missing_header_end_      = ExtractValueFromData(data,vme_state_machine_error_missing_header_end_bitlo,
										 vme_state_machine_error_missing_header_end_bithi);
    read_vme_state_machine_error_missing_data_end_marker_ = ExtractValueFromData(data,vme_state_machine_error_missing_data_end_marker_bitlo,
										 vme_state_machine_error_missing_data_end_marker_bithi);
    read_vme_state_machine_error_missing_trailer_end_     = ExtractValueFromData(data,vme_state_machine_error_missing_trailer_end_bitlo,
										 vme_state_machine_error_missing_trailer_end_bithi);
    read_vme_state_machine_error_word_count_overflow_     = ExtractValueFromData(data,vme_state_machine_error_word_count_overflow_bitlo,
										 vme_state_machine_error_word_count_overflow_bithi);
    //
  } else if ( address == num_vme_sm_adr_adr ) {
    //------------------------------------------------------------------
    //0XE0 = ADR_VMESM3:  Number of VME addresses written by VMESM
    //------------------------------------------------------------------
    read_vme_state_machine_number_of_vme_writes_ = ExtractValueFromData(data,vme_state_machine_number_of_vme_writes_bitlo,
									vme_state_machine_number_of_vme_writes_bithi);
    //
  } else if ( address == rat_3d_delays_adr ) {
    //------------------------------------------------------------------
    //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
    //------------------------------------------------------------------
    read_rpc0_rat_delay_ = ExtractValueFromData(data,rpc0_rat_delay_bitlo,rpc0_rat_delay_bithi);
    read_rpc1_rat_delay_ = ExtractValueFromData(data,rpc1_rat_delay_bitlo,rpc1_rat_delay_bithi);
    //
  } else if ( address == tmb_stat_adr ) {    
    //---------------------------------------------------------------------
    //0XEA = ADR_BDSTATUS:  Board Status Summary (copy of raw-hits header)
    //---------------------------------------------------------------------
    read_bdstatus_ok_     = ExtractValueFromData(data,bdstatus_ok_bitlo    ,bdstatus_ok_bithi    );
    read_vstat_5p0v_      = ExtractValueFromData(data,vstat_5p0v_bitlo     ,vstat_5p0v_bithi     );
    read_vstat_3p3v_      = ExtractValueFromData(data,vstat_3p3v_bitlo     ,vstat_3p3v_bithi     );
    read_vstat_1p8v_      = ExtractValueFromData(data,vstat_1p8v_bitlo     ,vstat_1p8v_bithi     );
    read_vstat_1p5v_      = ExtractValueFromData(data,vstat_1p5v_bitlo     ,vstat_1p5v_bithi     );
    read_nt_crit_         = ExtractValueFromData(data,nt_crit_bitlo        ,nt_crit_bithi        );
    read_vsm_ok_          = ExtractValueFromData(data,vsm_ok_bitlo         ,vsm_ok_bithi         );
    read_vsm_aborted_     = ExtractValueFromData(data,vsm_aborted_bitlo    ,vsm_aborted_bithi    );
    read_vsm_cksum_ok_    = ExtractValueFromData(data,vsm_cksum_ok_bitlo   ,vsm_cksum_ok_bithi   );
    read_vsm_wdcnt_ok_    = ExtractValueFromData(data,vsm_wdcnt_ok_bitlo   ,vsm_wdcnt_ok_bithi   );
    read_jsm_ok_          = ExtractValueFromData(data,jsm_ok_bitlo         ,jsm_ok_bithi         );
    read_jsm_aborted_     = ExtractValueFromData(data,jsm_aborted_bitlo    ,jsm_aborted_bithi    );
    read_jsm_cksum_ok_    = ExtractValueFromData(data,jsm_cksum_ok_bitlo   ,jsm_cksum_ok_bithi   );
    read_jsm_wdcnt_ok_    = ExtractValueFromData(data,jsm_wdcnt_ok_bitlo   ,jsm_wdcnt_ok_bithi   );
    read_jsm_tck_fpga_ok_ = ExtractValueFromData(data,jsm_tck_fpga_ok_bitlo,jsm_tck_fpga_ok_bithi);
    //
  } else if ( address == layer_trg_mode_adr ) {    
    //---------------------------------------------------------------------
    //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
    //---------------------------------------------------------------------
    read_layer_trigger_en_   = ExtractValueFromData(data,layer_trigger_en_bitlo ,layer_trigger_en_bithi );
    read_layer_trig_thresh_  = ExtractValueFromData(data,layer_trig_thresh_bitlo,layer_trig_thresh_bithi);
    read_number_layers_hit_  = ExtractValueFromData(data,number_layers_hit_bitlo,number_layers_hit_bithi);
    //
  } else if ( address == pattern_find_pretrg_adr ) {    
    //---------------------------------------------------------------------
    //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
    //---------------------------------------------------------------------
    read_clct_blanking_          = ExtractValueFromData(data,clct_blanking_bitlo         ,clct_blanking_bithi         );
    read_clct_stagger_           = ExtractValueFromData(data,clct_stagger_bitlo          ,clct_stagger_bithi          );
    read_clct_pattern_id_thresh_ = ExtractValueFromData(data,clct_pattern_id_thresh_bitlo,clct_pattern_id_thresh_bithi);
    read_aff_thresh_             = ExtractValueFromData(data,aff_thresh_bitlo            ,aff_thresh_bithi            );
    //
  } else if ( address == clct_separation_adr ) {    
    //---------------------------------------------------------------------
    //0XF6 = ADR_TEMP1:  CLCT separation
    //---------------------------------------------------------------------
    read_clct_separation_src_              = ExtractValueFromData(data,clct_separation_src_bitlo             ,clct_separation_src_bithi             );
    read_clct_separation_ram_write_enable_ = ExtractValueFromData(data,clct_separation_ram_write_enable_bitlo,clct_separation_ram_write_enable_bithi);
    read_clct_separation_ram_adr_          = ExtractValueFromData(data,clct_separation_ram_adr_bitlo         ,clct_separation_ram_adr_bithi         );
    read_min_clct_separation_              = ExtractValueFromData(data,min_clct_separation_bitlo             ,min_clct_separation_bithi             );
    //
  } 
  //
  // combinations of bits which say which trgmode_ we are using....
  //
  int read_CLCTtrigger_setting     = read_tmb_allow_clct_  & read_clct_pat_trig_en_;
  int read_ALCTCLCTtrigger_setting = read_tmb_allow_match_ & read_match_pat_trig_en_;
  //
  if (read_CLCTtrigger_setting) 
    read_trgmode_ = CLCT_trigger;
  if (read_ALCTCLCTtrigger_setting) 
    read_trgmode_ = ALCT_CLCT_coincidence_trigger;
  //
  return;
}
//
void TMB::DecodeBootRegister_(int data) {
  //------------------------------------------------------------------
  //0X70000 = ADR_BOOT:  Hardware Bootstrap Register
  //------------------------------------------------------------------
  read_boot_tdi_                     = ExtractValueFromData(data,boot_tdi_bitlo                    ,boot_tdi_bithi                    );
  read_boot_tms_                     = ExtractValueFromData(data,boot_tms_bitlo                    ,boot_tms_bithi                    );
  read_boot_tck_                     = ExtractValueFromData(data,boot_tck_bitlo                    ,boot_tck_bithi                    );
  read_boot_jtag_chain_select_       = ExtractValueFromData(data,boot_jtag_chain_select_bitlo      ,boot_jtag_chain_select_bithi      );
  read_boot_control_jtag_chain_      = ExtractValueFromData(data,boot_control_jtag_chain_bitlo     ,boot_control_jtag_chain_bithi     );
  read_boot_hard_reset_alct_         = ExtractValueFromData(data,boot_hard_reset_alct_bitlo        ,boot_hard_reset_alct_bithi        );
  read_boot_hard_reset_tmb_          = ExtractValueFromData(data,boot_hard_reset_tmb_bitlo         ,boot_hard_reset_tmb_bithi         );
  read_boot_disable_hard_reset_alct_ = ExtractValueFromData(data,boot_disable_hard_reset_alct_bitlo,boot_disable_hard_reset_alct_bithi);
  read_boot_disable_VME_             = ExtractValueFromData(data,boot_disable_VME_bitlo            ,boot_disable_VME_bithi            );
  read_boot_disable_mezz_clock_      = ExtractValueFromData(data,boot_disable_mezz_clock_bitlo     ,boot_disable_mezz_clock_bithi     );
  read_boot_hard_reset_rat_          = ExtractValueFromData(data,boot_hard_reset_rat_bitlo         ,boot_hard_reset_rat_bithi         );
  read_boot_vme_ready_               = ExtractValueFromData(data,boot_vme_ready_bitlo              ,boot_vme_ready_bithi              );
  read_boot_tdo_                     = ExtractValueFromData(data,boot_tdo_bitlo                    ,boot_tdo_bithi                    );
  //
  return;
}
//
////////////////////////////////////////////////////////////////////////////////////////
// Print read values to screen
////////////////////////////////////////////////////////////////////////////////////////
void TMB::PrintTMBConfiguration() {
  //
  (*MyOutput_) << "TMB READ configuration in slot = " << (int) slot() << std::endl;
  //
  PrintFirmwareDate();
  PrintBootRegister();
  //
  for (unsigned int index=0; index<TMBConfigurationRegister.size(); index++) {
    //
    unsigned long int VMEregister = TMBConfigurationRegister.at(index);
    //
    if (VMEregister != vme_usr_jtag_adr &&      // skip the user jtag register
	VMEregister != hcm001_adr && VMEregister != hcm023_adr && VMEregister != hcm045_adr &&   //skip the hot channel
	VMEregister != hcm101_adr && VMEregister != hcm123_adr && VMEregister != hcm145_adr &&   //mask registers... 
	VMEregister != hcm201_adr && VMEregister != hcm223_adr && VMEregister != hcm245_adr &&   //(print hot channel mask
	VMEregister != hcm301_adr && VMEregister != hcm323_adr && VMEregister != hcm345_adr &&   //out in a different way)
	VMEregister != hcm401_adr && VMEregister != hcm423_adr && VMEregister != hcm445_adr )
      PrintTMBRegister(VMEregister);
  }
  //
  PrintHotChannelMask();
  //
  return;
}
//
void TMB::PrintHotChannelMask() {
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  (*MyOutput_) << "TMB Hot Channel Mask (from right to left)" << std::endl;
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  //
  for (int layer=5; layer>=0; layer--) {
    char hot_channel_mask[MAX_NUM_DISTRIPS_PER_LAYER/8];
    packCharBuffer(read_hot_channel_mask_[layer],
		   MAX_NUM_DISTRIPS_PER_LAYER,
		   hot_channel_mask);
    //
    int char_counter = MAX_NUM_DISTRIPS_PER_LAYER/8 - 1;
    //
    (*MyOutput_) << "Layer " << std::dec << layer << " -> ";    
    for (int layer_counter=MAX_NUM_DISTRIPS_PER_LAYER/8; layer_counter>0; layer_counter--) {
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
void TMB::PrintVMEStateMachine() {
  //
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  (*MyOutput_) << "TMB VME state machine status in slot = " << (int) slot() << std::endl;
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  //
  PrintTMBRegister(vme_sm_ctrl_adr);
  PrintTMBRegister(vme_sm_wdcnt_adr);
  PrintTMBRegister(vme_sm_cksum_adr);
  PrintTMBRegister(num_vme_sm_adr_adr);
  //
  return;
}
//
void TMB::PrintJTAGStateMachine() {
  //
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  (*MyOutput_) << "TMB JTAG state machine status in slot = " << (int) slot() << std::endl;
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  //
  PrintTMBRegister(jtag_sm_ctrl_adr);
  PrintTMBRegister(jtag_sm_cksum_adr);
  PrintTMBRegister(jtag_sm_wdcnt_adr);
  PrintBootRegister();
  //
  return;
}
//
void TMB::PrintRawHitsHeader() {
  //
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  (*MyOutput_) << "TMB raw hits header in slot " << std::dec << (int) slot() << std::endl;
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  //
  PrintTMBRegister(tmb_stat_adr);
  //
  return;
}
//
void TMB::PrintDDDStateMachine() {
  //
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  (*MyOutput_) << "TMB DDD state machine status in slot = " << (int) slot() << std::endl;
  (*MyOutput_) << "-------------------------------------------" << std::endl;
  //
  PrintTMBRegister(vme_dddsm_adr);
  //
  return;
}
//
void TMB::PrintTMBRegister(unsigned long int address) {
  //
  (*MyOutput_) << std::hex << "0x" << ( (address>>4) & 0xf) << ( (address>>0) & 0xf);
  //
  if ( address == vme_loopbk_adr ) {
    //-----------------------------------------------------------------
    //0X0E = ADR_LOOPBK:  Loop-Back Control Register
    //-----------------------------------------------------------------
    (*MyOutput_) << " ->Loopback register:" << std::endl;
    (*MyOutput_) << "    CFEB output enable                    = " << std::hex << read_cfeb_oe_        << std::endl;
    (*MyOutput_) << "    ALCT loop-back                        = " << std::hex << read_alct_loop_      << std::endl;
    (*MyOutput_) << "    ALCT enable LVDS rx                   = " << std::hex << read_ALCT_input_     << std::endl;
    (*MyOutput_) << "    ALCT enable LVDS tx                   = " << std::hex << read_enable_alct_tx_ << std::endl;
    (*MyOutput_) << "    RPC FPGA enters loop-back mode        = " << std::hex << read_rpc_loop_rat_   << std::endl;
    (*MyOutput_) << "    TMBs RAT backplane ICs loop-back mode = " << std::hex << read_rpc_loop_tmb_   << std::endl;
    (*MyOutput_) << "    DMB loop-back                         = " << std::hex << read_dmb_loop_       << std::endl;
    (*MyOutput_) << "    DMB driver not enable                 = " << std::hex << read_dmb_oe_         << std::endl;
    (*MyOutput_) << "    GTL loop-back                         = " << std::hex << read_gtl_loop_       << std::endl;
    (*MyOutput_) << "    GTL outputs not enable                = " << std::hex << read_gtl_oe_         << std::endl;
    //
  } else if ( address == vme_dddsm_adr ) {
    //------------------------------------------------------------------
    //0X14 = ADR_DDDSM:  3D3444 State Machine Control + DCM Lock Status  
    //------------------------------------------------------------------
    (*MyOutput_) << " ->TMB DDD State Machine register:" << std::endl;
    (*MyOutput_) << "    start VME        = " << std::hex << read_ddd_state_machine_start_           << std::endl;
    (*MyOutput_) << "    manual clock mode= " << std::hex << read_ddd_state_machine_manual_           << std::endl;
    (*MyOutput_) << "    address latch    = " << std::hex << read_ddd_state_machine_latch_            << std::endl;
    (*MyOutput_) << "    serial in        = " << std::hex << read_ddd_state_machine_serial_in_        << std::endl;
    (*MyOutput_) << "    serial out       = " << std::hex << read_ddd_state_machine_serial_out_       << std::endl;
    (*MyOutput_) << "    auto start       = " << std::hex << read_ddd_state_machine_autostart_        << std::endl;
    (*MyOutput_) << "    busy             = " << std::hex << read_ddd_state_machine_busy_             << std::endl;
    (*MyOutput_) << "    verify OK        = " << std::hex << read_ddd_state_machine_verify_ok_        << std::endl;
    (*MyOutput_) << "    clock 0 DCM lock = " << std::hex << read_ddd_state_machine_clock0_lock_      << std::endl;
    (*MyOutput_) << "    clock 0d DCM lock= " << std::hex << read_ddd_state_machine_clock0d_lock_     << std::endl;
    (*MyOutput_) << "    clock 1 DCM lock = " << std::hex << read_ddd_state_machine_clock1_lock_      << std::endl;
    (*MyOutput_) << "    clock ALCT lock  = " << std::hex << read_ddd_state_machine_clock_alct_lock_  << std::endl;
    (*MyOutput_) << "    clock ALCTd lock = " << std::hex << read_ddd_state_machine_clockd_alct_lock_ << std::endl;
    (*MyOutput_) << "    clock MPC lock   = " << std::hex << read_ddd_state_machine_clock_mpc_lock_   << std::endl;
    (*MyOutput_) << "    clock DCC lock   = " << std::hex << read_ddd_state_machine_clock_dcc_lock_   << std::endl;
    (*MyOutput_) << "    clock RPC lock   = " << std::hex << read_ddd_state_machine_clock_rpc_lock_   << std::endl;
    //
  } else if ( address == vme_ddd0_adr ) {    
    //------------------------------------------------------------------
    //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Clock phase delay register:" << std::endl;
    (*MyOutput_) << "    ALCT tx phase = " << std::dec << read_alct_tx_clock_delay_ << std::endl;
    (*MyOutput_) << "    ALCT rx phase = " << std::dec << read_alct_rx_clock_delay_ << std::endl;
    (*MyOutput_) << "    DMB tx phase  = " << std::dec << read_dmb_tx_delay_        << std::endl;
    (*MyOutput_) << "    RAT-TMB phase = " << std::dec << read_rat_tmb_delay_       << std::endl;
    //
  } else if ( address == vme_ddd1_adr ) {    
    //------------------------------------------------------------------
    //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Clock phase delay register:" << std::endl;
    (*MyOutput_) << "    TMB 1 phase                            = " << std::dec << read_tmb1_phase_ << std::endl;
    (*MyOutput_) << "    MPC phase                              = " << std::dec << read_mpc_phase_  << std::endl;
    (*MyOutput_) << "    DCC (cfeb duty cycle correction) phase = " << std::dec << read_dcc_phase_  << std::endl;
    (*MyOutput_) << "    CFEB 0 phase                           = " << std::dec << read_cfeb0delay_ << std::endl;
    //
  } else if ( address == vme_ddd2_adr ) {    
    //------------------------------------------------------------------
    //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Clock phase delay register:" << std::endl;
    (*MyOutput_) << "    CFEB 1 phase = " << std::dec << read_cfeb1delay_ << std::endl;
    (*MyOutput_) << "    CFEB 2 phase = " << std::dec << read_cfeb2delay_ << std::endl;
    (*MyOutput_) << "    CFEB 3 phase = " << std::dec << read_cfeb3delay_ << std::endl;
    (*MyOutput_) << "    CFEB 4 phase = " << std::dec << read_cfeb4delay_ << std::endl;
    //
  } else if ( address == vme_ratctrl_adr ) {
    //------------------------------------------------------------------
    //0X1E = ADR_RATCTRL:  RAT Module Control
    //------------------------------------------------------------------
    (*MyOutput_) << " ->RAT control register:" << std::endl;
    (*MyOutput_) << "    RPC 80MHz sync mode = " << std::hex << read_rpc_sync_   << std::endl;
    (*MyOutput_) << "    RPC shift 1/2 cycle = " << std::hex << read_shift_rpc_  << std::endl;
    (*MyOutput_) << "    enable RAT DSN read = " << std::hex << read_rat_dsn_en_ << std::endl;
    //
  } else if ( address == ccb_trig_adr ) {
    //------------------------------------------------------------------
    //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
    //------------------------------------------------------------------
    (*MyOutput_) << " ->CCB trigger configuration register:" << std::endl;
    (*MyOutput_) << "    Request CCB L1a on ALCT external trig = " << std::hex << read_alct_ext_trig_l1aen_    << std::endl;
    (*MyOutput_) << "    Request CCB L1a on CLCT external trig = " << std::hex << read_clct_ext_trig_l1aen_    << std::endl;
    (*MyOutput_) << "    Request CCB L1a on sequencer trigger  = " << std::hex << read_request_l1a_            << std::endl;
    (*MyOutput_) << "    Fire ALCT external trig one-shot      = " << std::hex << read_alct_ext_trig_vme_      << std::endl;
    (*MyOutput_) << "    Fire CLCT external trig one-shot      = " << std::hex << read_clct_ext_trig_vme_      << std::endl;
    (*MyOutput_) << "    CLCText fire ALCT + ALCText fire CLCT = " << std::hex << read_ext_trig_both_          << std::endl;
    (*MyOutput_) << "    allow CLCTextCCB when ccb_ignore_rx=1 = " << std::hex << read_ccb_allow_bypass_       << std::endl;
    (*MyOutput_) << "    Ignore CCB trig start/stop            = " << std::hex << read_ignore_ccb_startstop_   << std::endl;
    (*MyOutput_) << "    Internal L1A delay (VME)              = " << std::dec << read_internal_l1a_delay_vme_ << std::endl;
    //
  } else if ( address == alct_cfg_adr ) {
    //------------------------------------------------------------------
    //0X30 = ADR_ALCT_CFG:  ALCT Configuration
    //------------------------------------------------------------------
    (*MyOutput_) << " ->ALCT configuration register:" << std::endl;
    (*MyOutput_) << "    Enable alct_ext_trig from CCB             = " << std::hex << read_cfg_alct_ext_trig_en_   << std::endl;  
    (*MyOutput_) << "    Enable alct_ext_inject from CCB           = " << std::hex << read_cfg_alct_ext_inject_en_ << std::endl;
    (*MyOutput_) << "    Assert alct_ext_trig                      = " << std::hex << read_cfg_alct_ext_trig_      << std::endl;    
    (*MyOutput_) << "    Assert alct_ext_inject                    = " << std::hex << read_cfg_alct_ext_inject_    << std::endl;  
    (*MyOutput_) << "    ALCT sequencer command                   =0x" << std::hex << read_alct_seq_cmd_           << std::endl;         
    (*MyOutput_) << "    alct_clock_en_vme=ccb_clock40_enable      = " << std::hex << read_alct_clock_en_use_ccb_  << std::endl;
    (*MyOutput_) << "    set alct_clock_en scsi signal if above=0  = " << std::hex << read_alct_clock_en_use_vme_  << std::endl;
    //
  } else if ( address == alct_inj_adr ) {
    //------------------------------------------------------------------
    //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
    //------------------------------------------------------------------
    (*MyOutput_) << " ->ALCT injector control register:" << std::endl;
    (*MyOutput_) << "    Blank ALCT received data          = " << std::hex << read_alct_clear_      << std::endl;
    (*MyOutput_) << "    Start ALCT injector state machine = " << std::hex << read_alct_inject_mux_ << std::endl;
    (*MyOutput_) << "    Link ALCT inject with CLCT inject = " << std::hex << read_alct_sync_clct_  << std::endl;
    (*MyOutput_) << "    ALCT injector delay               = " << std::hex << read_alct_inj_delay_  << std::endl;
    //
  } else if ( address == cfeb_inj_adr ) {
    //------------------------------------------------------------------
    //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
    //------------------------------------------------------------------
    (*MyOutput_) << " ->CFEB injector control register:" << std::endl;
    (*MyOutput_) << "    CFEB enable mask               = 0x" << std::hex << read_enableCLCTInputs_  << std::endl;
    (*MyOutput_) << "    select CFEB for RAM read/write = 0x" << std::hex << read_cfeb_ram_sel_      << std::endl;
    (*MyOutput_) << "    CFEB enable injector mask      = 0x" << std::hex << read_cfeb_inj_en_sel_   << std::endl;
    (*MyOutput_) << "    start pattern injector         = "   << std::hex << read_start_pattern_inj_ << std::endl;
    //
  } else if ( address == seq_trig_en_adr ) {
    //------------------------------------------------------------------
    //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer Trigger Source Enable register:" << std::endl;
    (*MyOutput_) << "    CLCT pretrigger                 = "   << std::hex << read_clct_pat_trig_en_   << std::endl;
    (*MyOutput_) << "    ALCT pretrigger                 = "   << std::hex << read_alct_pat_trig_en_   << std::endl;
    (*MyOutput_) << "    ALCT*CLCT pretrigger            = "   << std::hex << read_match_pat_trig_en_  << std::endl;
    (*MyOutput_) << "    ADB external pretrigger         = "   << std::hex << read_adb_ext_trig_en_    << std::endl;
    (*MyOutput_) << "    DMB external pretrigger         = "   << std::hex << read_dmb_ext_trig_en_    << std::endl;
    (*MyOutput_) << "    CLCT ext (scint) pretrigger     = "   << std::hex << read_clct_ext_trig_en_   << std::endl;
    (*MyOutput_) << "    ALCT ext pretrigger             = "   << std::hex << read_alct_ext_trig_en_   << std::endl;
    (*MyOutput_) << "    VME pretrigger                  = "   << std::hex << read_vme_ext_trig_       << std::endl;
    (*MyOutput_) << "    CLCT ext trig fires pattern inj = "   << std::hex << read_ext_trig_inject_    << std::endl;
    (*MyOutput_) << "    all CFEBs active                = "   << std::hex << read_all_cfeb_active_    << std::endl;
    (*MyOutput_) << "    CFEBs enabled                   = 0x" << std::hex << read_cfebs_enabled_      << std::endl;
    (*MyOutput_) << "    enable CFEBS through VME 0x42   = "   << std::hex << read_cfeb_enable_source_ << std::endl;
    //
  } else if ( address == seq_trig_dly0_adr ) {
    //------------------------------------------------------------------
    //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer Trigger Source Delays register:" << std::endl;
    (*MyOutput_) << "    ALCT*CLCT pretrigger match window width    = " << std::dec << read_alct_pretrig_width_ << std::endl;
    (*MyOutput_) << "    ALCT*CLCT Pretrigger ALCT delay            = " << std::dec << read_alct_pretrig_delay_ << std::endl;
    (*MyOutput_) << "    Active FEB Flag delay                      = " << std::dec << read_alct_pattern_delay_ << std::endl;
    (*MyOutput_) << "    ADB External trigger delay                 = " << std::dec << read_adb_ext_trig_delay_ << std::endl;
    //
  } else if ( address == seq_trig_dly1_adr ) {
    //------------------------------------------------------------------
    //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer Trigger Source Delays register:" << std::endl;
    (*MyOutput_) << "    Delay dmb_ext_trig from DMB          = " << std::dec << read_dmb_ext_trig_delay_  << std::endl;
    (*MyOutput_) << "    Delay clct_ext_trig (scint) from CCB = " << std::dec << read_clct_ext_trig_delay_ << std::endl;
    (*MyOutput_) << "    Delay alct_ext_trig from CCB         = " << std::dec << read_alct_ext_trig_delay_ << std::endl;
    (*MyOutput_) << "    Layer trigger delay                  = " << std::dec << read_layer_trig_delay_    << std::endl;
    //
  } else if ( address == seq_id_adr ) {
    //------------------------------------------------------------------
    //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
    //------------------------------------------------------------------
    (*MyOutput_) << " ->ID register:" << std::endl;
    (*MyOutput_) << "    TMB slot = " << std::dec << read_tmb_slot_ << std::endl;
    (*MyOutput_) << "    CSC ID   = " << std::dec << read_csc_id_   << std::endl;
    (*MyOutput_) << "    Run ID   = " << std::dec << read_run_id_   << std::endl;
    //
  } else if ( address == seq_clct_adr ) {
    //------------------------------------------------------------------
    //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer CLCT configuration register:" << std::endl;
    (*MyOutput_) << "    Triad 1-shot persistence          = 0x" << std::hex << read_triad_persist_     << std::endl;
    (*MyOutput_) << "    1/2-strip pretrigger thresh       = "   << std::dec << read_hs_pretrig_thresh_ << std::endl;
    //(*MyOutput_) << "    di-strip pretrigger thresh        = "   << std::dec << read_ds_pretrig_thresh_ << std::endl;
    (*MyOutput_) << "    min pattern hits for valid pattern= "   << std::dec << read_min_hits_pattern_  << std::endl;
    (*MyOutput_) << "    drift delay                       = "   << std::dec << read_drift_delay_       << std::endl;
    (*MyOutput_) << "    pretrigger then halt until unhalt = "   << std::hex << read_pretrigger_halt_   << std::endl;
    //
  } else if ( address == seq_fifo_adr ) {
    //------------------------------------------------------------------
    //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer FIFO configuration register:" << std::endl;
    (*MyOutput_) << "   FIFO mode                           = " << std::dec << read_fifo_mode_ << " = ";
    if (read_fifo_mode_ == FIFOMODE_NoCfebRaw_FullHeader) {
      (*MyOutput_) << "no CFEB raw hits, full header";
    } else if (read_fifo_mode_ == FIFOMODE_AllCfebRaw_FullHeader) {
      (*MyOutput_) << "all CFEB raw hits, full header";
    } else if (read_fifo_mode_ == FIFOMODE_LocalCfebRaw_FullHeader) {
      (*MyOutput_) << "local CFEB raw hits, full header";
    } else if (read_fifo_mode_ == FIFOMODE_NoCfebRaw_ShortHeader) {
      (*MyOutput_) << "no CFEB raw hits, short header";
    } else if (read_fifo_mode_ == FIFOMODE_NoCfebRaw_NoHeader) {
      (*MyOutput_) << "no CFEB raw hits, no header";
    } else {
      (*MyOutput_) << "unknown fifo_mode...";
    }
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "   Number FIFO time bins read out       = " << std::dec << read_fifo_tbins_   << std::endl;
    (*MyOutput_) << "   Number FIFO time bins before pretrig = " << std::dec << read_fifo_pretrig_ << std::endl;
    //
  } else if ( address == seq_l1a_adr ) {
    //------------------------------------------------------------------
    //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer L1A configuration register:" << std::endl;
    (*MyOutput_) << "    L1a delay from pretrig status output = " << std::dec << read_l1adelay_         << std::endl;
    (*MyOutput_) << "    L1a accept window width              = " << std::dec << read_l1a_window_size_  << std::endl;
    (*MyOutput_) << "    Generate internal L1a                = " << std::dec << read_tmb_l1a_internal_ << std::endl;
    //
  } else if ( address == seq_offset_adr ) {
    //------------------------------------------------------------------
    //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer counter offset register:" << std::endl;
    (*MyOutput_) << "    L1a Counter Preset value = " << std::dec << read_l1a_offset_ << std::endl;
    (*MyOutput_) << "    BXN offset at reset      = " << std::dec << read_bxn_offset_ << std::endl;
    //
  } else if ( address == tmb_trig_adr ) {    
    //------------------------------------------------------------------
    //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
    //------------------------------------------------------------------
    (*MyOutput_) << " ->TMB trigger configuration/MPC accept register:" << std::endl;
    (*MyOutput_) << "    Allow sync_err to MPC for either muon        = " << std::hex << read_tmb_sync_err_enable_ << std::endl;
    (*MyOutput_) << "    Allow ALCT only trigger                      = " << std::hex << read_tmb_allow_alct_      << std::endl;
    (*MyOutput_) << "    Allow CLCT only trigger                      = " << std::hex << read_tmb_allow_clct_      << std::endl;
    (*MyOutput_) << "    Allow ALCT*CLCT trigger                      = " << std::hex << read_tmb_allow_match_     << std::endl;
    (*MyOutput_) << "    MPC accept response delay                    = " << std::dec << read_mpc_delay_           << std::endl;
    (*MyOutput_) << "    MPC accept latched after delay               = " << std::dec << read_mpc_accept_          << std::endl;
    (*MyOutput_) << "    MPC reserved latched after delay             = " << std::dec << read_mpc_reserved_        << std::endl;
    (*MyOutput_) << "    MPC gets bx0 from TTC                        = " << std::dec << read_mpc_sel_ttc_bx0_     << std::endl;
    (*MyOutput_) << "    blank MPC data and bx0 except when triggered = " << std::hex << read_mpc_idle_blank_      << std::endl;
    (*MyOutput_) << "    enable outputs to MPC                        = " << std::hex << read_mpc_output_enable_   << std::endl;
    //
  } else if ( address == seqmod_adr ) {
    //------------------------------------------------------------------
    //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
    //------------------------------------------------------------------
    (*MyOutput_) << " ->Sequencer trigger modifier register:" << std::endl;
    (*MyOutput_) << "    Trigger sequencer flush state timer              = " << std::dec << read_clct_flush_delay_    << std::endl;
    (*MyOutput_) << "    Disable raw hits (turbo mode)                    = " << std::hex << read_clct_turbo_          << std::endl;
    (*MyOutput_) << "    Enable OSU random LCT generator                  = " << std::hex << read_ranlct_enable_       << std::endl;
    (*MyOutput_) << "    Require wr_buffer available to pretrigger        = " << std::hex << read_wrt_buf_required_    << std::endl;
    (*MyOutput_) << "    Require valid CLCT after drift delay             = " << std::hex << read_valid_clct_required_ << std::endl;
    (*MyOutput_) << "    Readout allows tmb trig pulse in L1a window      = " << std::hex << read_l1a_allow_match_     << std::endl;
    (*MyOutput_) << "    Readout allows notmb trig pulse in L1a window    = " << std::hex << read_l1a_allow_notmb_     << std::endl;
    (*MyOutput_) << "    Readout allows TMB trig pulse outside L1a window = " << std::hex << read_l1a_allow_nol1a_     << std::endl;
    (*MyOutput_) << "    Allow ALCT-only events to readout at L1a         = " << std::hex << read_l1a_allow_alct_only_ << std::endl;
    (*MyOutput_) << "    Clear scintillator veto FF                       = " << std::hex << read_scint_veto_clr_      << std::endl;
    (*MyOutput_) << "    Scintillator veto FF state                       = " << std::hex << read_scint_veto_vme_      << std::endl;
    //
  } else if ( address == tmbtim_adr ) {
    //------------------------------------------------------------------
    //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
    //------------------------------------------------------------------
    (*MyOutput_) << " ->TMB timing for ALCT*CLCT coincidence register:" << std::endl;
    (*MyOutput_) << "    ALCT*CLCT trigger ALCT (Valid Pattern Flag) delay = " << std::dec << read_alct_vpf_delay_         << std::endl;
    (*MyOutput_) << "    ALCT*CLCT trigger match window size               = " << std::dec << read_alct_match_window_size_ << std::endl;
    (*MyOutput_) << "    MPC transmit delay                                = " << std::dec << read_mpc_tx_delay_           << std::endl;
    //
  } else if ( address == rpc_cfg_adr ) {
    //------------------------------------------------------------------
    //0XB6 = ADR_RPC_CFG:  RPC Configuration
    //------------------------------------------------------------------
    (*MyOutput_) << " ->RPC configuration register:" << std::endl;
    (*MyOutput_) << "    RPC exists                                     = 0x" << std::hex << read_rpc_exists_      << std::endl;
    (*MyOutput_) << "    RPC read enable                                = "   << std::hex << read_rpc_read_enable_ << std::endl;
    (*MyOutput_) << "    RPC BXN offset                                 = "   << std::dec << read_rpc_bxn_offset_  << std::endl;
    (*MyOutput_) << "    RPC bank address (for reading rdata sync mode) = "   << std::dec << read_rpc_bank_        << std::endl;
    (*MyOutput_) << "    RPC rdata [18:16] msbs for sync mode           = "   << std::dec << read_rpc_rbxn_        << std::endl;
    (*MyOutput_) << "    RPC FPGA reports configuration done            = "   << std::dec << read_rpc_done_        << std::endl;
    //
  } else if ( address == rpc_raw_delay_adr ) {
    //------------------------------------------------------------------
    //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
    //------------------------------------------------------------------
    (*MyOutput_) << " ->RPC raw hits delay register:" << std::endl;
    (*MyOutput_) << "    RPC0 raw hits delay = " << std::dec << read_rpc0_raw_delay_ << std::endl;
    (*MyOutput_) << "    RPC1 raw hits delay = " << std::dec << read_rpc1_raw_delay_ << std::endl;
    //
  } else if ( address == rpc_inj_adr ) {
    //------------------------------------------------------------------
    //0XBC = ADR_RPC_INJ:  RPC Injector Control
    //------------------------------------------------------------------
    (*MyOutput_) << " ->RPC injector control register:" << std::endl;
    (*MyOutput_) << "    Enable RPC inputs to RAT             = "   << std::hex << read_rpc_mask_all_  << std::endl; 
    (*MyOutput_) << "    Enable RAT for injector fire         = "   << std::hex << read_inj_mask_rat_  << std::endl; 
    (*MyOutput_) << "    Enable RPC inj RAM for injector fire = "   << std::hex << read_inj_mask_rpc_  << std::endl; 
    (*MyOutput_) << "    CFEB/RPC injectors wait for RAT      = "   << std::dec << read_inj_delay_rat_ << std::endl; 
    (*MyOutput_) << "    Enable injector RAM write            = "   << std::hex << read_rpc_inj_sel_   << std::endl; 
    (*MyOutput_) << "    RPC injector write data MSBs         = 0x" << std::hex << read_rpc_inj_wdata_ << std::endl; 
    (*MyOutput_) << "    RPC injector read data MSBs          = 0x" << std::hex << read_rpc_inj_rdata_ << std::endl; 
    //
  } else if ( address == jtag_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
    //------------------------------------------------------------------
    (*MyOutput_) << " ->JTAG State Machine Control register:" << std::endl;
    (*MyOutput_) << "    prom start vme   = "   << std::hex << read_jtag_state_machine_start_       << std::endl;
    (*MyOutput_) << "    sreset           = "   << std::hex << read_jtag_state_machine_sreset_      << std::endl;
    (*MyOutput_) << "    autostart        = "   << std::hex << read_jtag_state_machine_autostart_   << std::endl;
    (*MyOutput_) << "    busy             = "   << std::hex << read_jtag_state_machine_busy_        << std::endl;
    (*MyOutput_) << "    aborted          = "   << std::hex << read_jtag_state_machine_aborted_     << std::endl;
    (*MyOutput_) << "    check sum OK     = "   << std::hex << read_jtag_state_machine_cksum_ok_    << std::endl;
    (*MyOutput_) << "    word count OK    = "   << std::hex << read_jtag_state_machine_wdcnt_ok_    << std::endl;
    (*MyOutput_) << "    tck FPGA OK      = "   << std::hex << read_jtag_state_machine_tck_fpga_ok_ << std::endl;
    (*MyOutput_) << "    VME ready        = "   << std::hex << read_jtag_state_machine_vme_ready_   << std::endl;
    (*MyOutput_) << "    state machine OK = "   << std::hex << read_jtag_state_machine_ok_          << std::endl;
    (*MyOutput_) << "    throttle         = 0x" << std::hex << read_jtag_state_machine_throttle_    << std::endl;
    (*MyOutput_) << "    disable wrt to 10= "   << std::hex << read_jtag_disable_write_to_adr10_   << std::endl;
    (*MyOutput_) << "    jtag oe          = "   << std::hex << read_jtag_state_machine_oe_          << std::endl;
    //
  } else if ( address == jtag_sm_wdcnt_adr ) {
    //------------------------------------------------------------------
    //0XD6 = ADR_JTAGSM1:  JTAG State Machine Word Count
    //------------------------------------------------------------------
    (*MyOutput_) << " ->JTAG State Machine Word Count register:" << std::endl;
    (*MyOutput_) << "    word count = 0x" << std::hex << read_jtag_state_machine_word_count_ << std::endl;
    //
  } else if ( address == jtag_sm_cksum_adr ) {
    //------------------------------------------------------------------
    //0XD8 = ADR_JTAGSM2:  JTAG State Machine Checksum
    //------------------------------------------------------------------
    (*MyOutput_) << " ->JTAG State Machine Checksum register:" << std::endl;
    (*MyOutput_) << "    check sum  = 0x" << std::hex << read_jtag_state_machine_check_sum_ << std::endl;
    (*MyOutput_) << "    tck_fpga   = 0x" << std::hex << read_jtag_state_machine_tck_fpga_ << std::endl;
    //
  } else if ( address == vme_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XDA = ADR_VMESM0:  VME State Machine Control (reads VME PROM)
    //------------------------------------------------------------------
    (*MyOutput_) << " ->VME State Machine Control register:" << std::endl;
    (*MyOutput_) << "    start            = "   << std::hex << read_vme_state_machine_start_     << std::endl;
    (*MyOutput_) << "    sreset           = "   << std::hex << read_vme_state_machine_sreset_    << std::endl;
    (*MyOutput_) << "    autostart        = "   << std::hex << read_vme_state_machine_autostart_ << std::endl;
    (*MyOutput_) << "    busy             = "   << std::hex << read_vme_state_machine_busy_      << std::endl;
    (*MyOutput_) << "    aborted          = "   << std::hex << read_vme_state_machine_aborted_   << std::endl;
    (*MyOutput_) << "    check sum OK     = "   << std::hex << read_vme_state_machine_cksum_ok_  << std::endl;
    (*MyOutput_) << "    word count OK    = "   << std::hex << read_vme_state_machine_wdcnt_ok_  << std::endl;
    (*MyOutput_) << "    JTAG auto        = "   << std::hex << read_vme_state_machine_jtag_auto_ << std::endl;
    (*MyOutput_) << "    VME ready        = "   << std::hex << read_vme_state_machine_vme_ready_ << std::endl;
    (*MyOutput_) << "    state machine OK = "   << std::hex << read_vme_state_machine_ok_        << std::endl;
    (*MyOutput_) << "    path OK          = "   << std::hex << read_vme_state_machine_path_ok_   << std::endl;
    (*MyOutput_) << "    throttle         = 0x" << std::hex << read_vme_state_machine_throttle_  << std::endl;
    //
  } else if ( address == vme_sm_wdcnt_adr ) {
    //------------------------------------------------------------------
    //0XDC = ADR_VMESM1:  VME State Machine Word Count
    //------------------------------------------------------------------
    (*MyOutput_) << " ->VME State Machine Word Count register:" << std::endl;
    (*MyOutput_) << "    word count = 0x" << std::hex << read_vme_state_machine_word_count_ << std::endl;
    //
  } else if ( address == vme_sm_cksum_adr ) {
    //------------------------------------------------------------------
    //0XDE = ADR_VMESM2:  VME State Machine Checksum
    //------------------------------------------------------------------
    (*MyOutput_) << " ->VME State Machine Checksum register:" << std::endl;
    (*MyOutput_) << "    check sum               = 0x" << std::hex << read_vme_state_machine_check_sum_                     << std::endl;
    (*MyOutput_) << "    missing header start    = "   << std::hex << read_vme_state_machine_error_missing_header_start_    << std::endl;
    (*MyOutput_) << "    missing header end      = "   << std::hex << read_vme_state_machine_error_missing_header_end_      << std::endl;
    (*MyOutput_) << "    missing data end marker = "   << std::hex << read_vme_state_machine_error_missing_data_end_marker_ << std::endl;
    (*MyOutput_) << "    missing trailer end     = "   << std::hex << read_vme_state_machine_error_missing_trailer_end_     << std::endl;
    (*MyOutput_) << "    word count overflow     = "   << std::hex << read_vme_state_machine_error_word_count_overflow_     << std::endl;
    //
  } else if ( address == num_vme_sm_adr_adr ) {
    //------------------------------------------------------------------
    //0XE0 = ADR_VMESM3:  Number of VME addresses written by VMESM
    //------------------------------------------------------------------
    (*MyOutput_) << " ->VME State Machine Address Count register:" << std::endl;
    (*MyOutput_) << "    Number of VME writes = 0x" << std::hex << read_vme_state_machine_number_of_vme_writes_ << std::endl;
    //
  } else if ( address == rat_3d_delays_adr ) {
    //------------------------------------------------------------------
    //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
    //------------------------------------------------------------------
    (*MyOutput_) << " ->RPC/RAT phase delay register:" << std::endl;
    (*MyOutput_) << "    RPC0/RAT rx phase = " << std::dec << read_rpc0_rat_delay_ << std::endl;
    (*MyOutput_) << "    RPC1/RAT rx phase = " << std::dec << read_rpc1_rat_delay_ << std::endl;
    //
  } else if ( address == tmb_stat_adr ) {
    //---------------------------------------------------------------------
    //0XEA = ADR_BDSTATUS:  Board Status Summary (copy of raw-hits header)
    //---------------------------------------------------------------------
    (*MyOutput_) << " ->Board Status Summary register:" << std::endl;
    (*MyOutput_) << "    board status OK                 = " << std::hex << read_bdstatus_ok_     << std::endl;
    (*MyOutput_) << "    5.0V critical OK                = " << std::hex << read_vstat_5p0v_      << std::endl;
    (*MyOutput_) << "    3.3V critical OK                = " << std::hex << read_vstat_3p3v_      << std::endl;
    (*MyOutput_) << "    1.8V critical OK                = " << std::hex << read_vstat_1p8v_      << std::endl;
    (*MyOutput_) << "    1.5V critical OK                = " << std::hex << read_vstat_1p5v_      << std::endl;
    (*MyOutput_) << "    Temperature not critical        = " << std::hex << read_nt_crit_         << std::endl;
    (*MyOutput_) << "    VME state machine OK            = " << std::hex << read_vsm_ok_          << std::endl;
    (*MyOutput_) << "    VME state machine aborted       = " << std::hex << read_vsm_aborted_     << std::endl;
    (*MyOutput_) << "    VME state machine checksum OK   = " << std::hex << read_vsm_cksum_ok_    << std::endl;
    (*MyOutput_) << "    VME state machine wordcount OK  = " << std::hex << read_vsm_wdcnt_ok_    << std::endl;
    (*MyOutput_) << "    JTAG state machine OK           = " << std::hex << read_jsm_ok_          << std::endl;
    (*MyOutput_) << "    JTAG state machine aborted      = " << std::hex << read_jsm_aborted_     << std::endl;
    (*MyOutput_) << "    JTAG state machine check sum OK = " << std::hex << read_jsm_cksum_ok_    << std::endl;
    (*MyOutput_) << "    JTAG state machine wordcount OK = " << std::hex << read_jsm_wdcnt_ok_    << std::endl;
    (*MyOutput_) << "    JTAG state machine FPGA TCK OK  = " << std::hex << read_jsm_tck_fpga_ok_ << std::endl;
    //
  } else if ( address == layer_trg_mode_adr ) {
    //---------------------------------------------------------------------
    //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
    //---------------------------------------------------------------------
    (*MyOutput_) << " ->Layer-trigger mode register:" << std::endl;
    (*MyOutput_) << "    Enable layer trigger mode = " << std::dec << read_layer_trigger_en_  << std::endl; 
    (*MyOutput_) << "    Layer trigger threshold   = " << std::dec << read_layer_trig_thresh_ << std::endl; 
    (*MyOutput_) << "    Number of layers hit      = " << std::dec << read_number_layers_hit_ << std::endl; 
    //
  } else if ( address == pattern_find_pretrg_adr ) {
    //---------------------------------------------------------------------
    //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
    //---------------------------------------------------------------------
    (*MyOutput_) << " ->CLCT Pattern Finder pretrigger configuration register:" << std::endl;
    (*MyOutput_) << "    Blank CLCT output if no valid pattern flag                = " << std::dec << read_clct_blanking_           << std::endl; 
    (*MyOutput_) << "    Stagger CLCT layers                                       = " << std::dec << read_clct_stagger_            << std::endl; 
    (*MyOutput_) << "    Minimum pattern ID value for CLCT pretrig                 = " << std::dec << read_clct_pattern_id_thresh_  << std::endl; 
    (*MyOutput_) << "    Minimum layers in pattern to send Active FEB Flag to DMB  = " << std::dec << read_aff_thresh_              << std::endl; 
    //
  } else if ( address == clct_separation_adr ) {
    //---------------------------------------------------------------------
    //0XF6 = ADR_TEMP1:  CLCT separation
    //---------------------------------------------------------------------
    (*MyOutput_) << " ->CLCT Separation register:" << std::endl;
    (*MyOutput_) << "    CLCT separation source = VME      = " << std::dec << read_clct_separation_src_               << std::endl; 
    (*MyOutput_) << "    CLCT separation RAM write enable  = " << std::dec << read_clct_separation_ram_write_enable_  << std::endl; 
    (*MyOutput_) << "    CLCT separation RAM address       = " << std::dec << read_clct_separation_ram_adr_           << std::endl; 
    (*MyOutput_) << "    Minimum 1/2-strip CLCT separation = " << std::dec << read_min_clct_separation_               << std::endl; 
    //
  } else {
    //
    (*MyOutput_) << " -> Unable to decode register: PLEASE DEFINE" << std::endl;
    //
  }
  return;
}
//
void TMB::PrintFirmwareDate() {
  //
  (*MyOutput_) << "-> TMB Firmware date: " << std::hex 
	       << ((GetReadTmbFirmwareMonth() >>  4) & 0xf)
	       << ((GetReadTmbFirmwareMonth() >>  0) & 0xf)
	       << ((GetReadTmbFirmwareDay()   >>  4) & 0xf)
	       << ((GetReadTmbFirmwareDay()   >>  0) & 0xf)
	       << ((GetReadTmbFirmwareYear()  >> 12) & 0xf)
	       << ((GetReadTmbFirmwareYear()  >>  8) & 0xf)
	       << ((GetReadTmbFirmwareYear()  >>  4) & 0xf)
	       << ((GetReadTmbFirmwareYear()  >>  0) & 0xf) 
	       << std::endl;
  (*MyOutput_) << "-> TMB Firmware type   : " << std::hex << GetReadTmbFirmwareType()    << std::endl;
  (*MyOutput_) << "-> TMB Firmware version: " << std::hex << GetReadTmbFirmwareVersion() << std::endl;
  (*MyOutput_) << "-> TMB Firmware RevCode: " << std::hex << GetReadTmbFirmwareRevcode() << std::endl;
  //
  return;
}
//
void TMB::PrintBootRegister() {
  //
  //-----------------------------------------------------------------
  //0X70000 = ADR_BOOT:  Hardware Bootstrap Register
  //-----------------------------------------------------------------
  (*MyOutput_) << " boot register tdi                     = "  << std::hex << read_boot_tdi_                     << std::endl;
  (*MyOutput_) << " boot register tms                     = "  << std::hex << read_boot_tms_                     << std::endl;
  (*MyOutput_) << " boot register tck                     = "  << std::hex << read_boot_tck_                     << std::endl;
  (*MyOutput_) << " boot register jtag chain select       = 0x"<< std::hex << read_boot_jtag_chain_select_       << std::endl;
  (*MyOutput_) << " boot register control JTAG chain      = "  << std::hex << read_boot_control_jtag_chain_      << std::endl;
  (*MyOutput_) << " boot register hard reset ALCT         = "  << std::hex << read_boot_hard_reset_alct_         << std::endl;
  (*MyOutput_) << " boot register hard reset TMB          = "  << std::hex << read_boot_hard_reset_tmb_          << std::endl;
  (*MyOutput_) << " boot register disable hard reset ALCT = "  << std::hex << read_boot_disable_hard_reset_alct_ << std::endl;
  (*MyOutput_) << " boot register disable VME             = "  << std::hex << read_boot_disable_VME_             << std::endl;
  (*MyOutput_) << " boot register disable TMB mezz clock  = "  << std::hex << read_boot_disable_mezz_clock_      << std::endl;
  (*MyOutput_) << " boot register hard reset RAT          = "  << std::hex << read_boot_hard_reset_rat_          << std::endl;
  (*MyOutput_) << " boot register VME ready               = "  << std::hex << read_boot_vme_ready_               << std::endl;
  (*MyOutput_) << " boot register tdo                     = "  << std::hex << read_boot_tdo_                     << std::endl;
  //
  return;
}
//
//
////////////////////////////////////////////////////////////////////////////////////////
// Fill register value with values extracted from database
////////////////////////////////////////////////////////////////////////////////////////
void TMB::Set_cfeb_enable_source(int value) {
  //
  // decode TMB VME register into the bit which the VME register expects
  if (value == 42) {
    SetCfebEnableSource_(1);     
  } else if (value == 68) {
    SetCfebEnableSource_(0);     
  }
  return;
}
//
int TMB::FillTMBRegister(unsigned long int address) {
  //
  // Default to write 0's into bits not specified
  int data_word = 0;
  //
  if ( address == vme_loopbk_adr ) {
    //------------------------------------------------------------------
    //0X0E = ADR_LOOPBK:  Loop-Back Control Register  
    //------------------------------------------------------------------
    InsertValueIntoDataWord(ALCT_input_    ,ALCT_input_bithi    ,ALCT_input_bitlo    ,&data_word);
    InsertValueIntoDataWord(enable_alct_tx_,enable_alct_tx_bithi,enable_alct_tx_bitlo,&data_word);
    //
  } else if ( address == vme_dddsm_adr ) {    
    //------------------------------------------------------------------
    //0X14 = ADR_DDDSM:  3D3444 State Machine Control + DCM Lock Status  
    //------------------------------------------------------------------
    InsertValueIntoDataWord(ddd_state_machine_start_     ,ddd_state_machine_start_bithi     ,ddd_state_machine_start_bitlo     ,&data_word);
    InsertValueIntoDataWord(ddd_state_machine_manual_    ,ddd_state_machine_manual_bithi    ,ddd_state_machine_manual_bitlo    ,&data_word);
    InsertValueIntoDataWord(ddd_state_machine_latch_     ,ddd_state_machine_latch_bithi     ,ddd_state_machine_latch_bitlo     ,&data_word);
    InsertValueIntoDataWord(ddd_state_machine_serial_in_ ,ddd_state_machine_serial_in_bithi ,ddd_state_machine_serial_in_bitlo ,&data_word);
    InsertValueIntoDataWord(ddd_state_machine_serial_out_,ddd_state_machine_serial_out_bithi,ddd_state_machine_serial_out_bitlo,&data_word);
    InsertValueIntoDataWord(ddd_state_machine_autostart_ ,ddd_state_machine_autostart_bithi ,ddd_state_machine_autostart_bitlo ,&data_word);
    //
  } else if ( address == vme_ddd0_adr ) {
    //------------------------------------------------------------------
    //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_tx_clock_delay_,alct_tx_clock_delay_bithi,alct_tx_clock_delay_bitlo,&data_word);
    InsertValueIntoDataWord(alct_rx_clock_delay_,alct_rx_clock_delay_bithi,alct_rx_clock_delay_bitlo,&data_word);
    InsertValueIntoDataWord(dmb_tx_delay_       ,dmb_tx_delay_bithi       ,dmb_tx_delay_bitlo       ,&data_word); 
    InsertValueIntoDataWord(rat_tmb_delay_      ,rat_tmb_delay_bithi      ,rat_tmb_delay_bitlo      ,&data_word);
    //
  } else if ( address == vme_ddd1_adr ) {
    //------------------------------------------------------------------
    //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    InsertValueIntoDataWord(tmb1_phase_,tmb1_phase_bithi,tmb1_phase_bitlo,&data_word);
    InsertValueIntoDataWord(mpc_phase_ ,mpc_phase_bithi ,mpc_phase_bitlo ,&data_word);
    InsertValueIntoDataWord(dcc_phase_ ,dcc_phase_bithi ,dcc_phase_bitlo ,&data_word);
    InsertValueIntoDataWord(cfeb0delay_,cfeb0delay_bithi,cfeb0delay_bitlo,&data_word);
    //
  } else if ( address == vme_ddd2_adr ) {
    //------------------------------------------------------------------
    //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
    //------------------------------------------------------------------
    InsertValueIntoDataWord(cfeb1delay_,cfeb1delay_bithi,cfeb1delay_bitlo,&data_word);
    InsertValueIntoDataWord(cfeb2delay_,cfeb2delay_bithi,cfeb2delay_bitlo,&data_word);
    InsertValueIntoDataWord(cfeb3delay_,cfeb3delay_bithi,cfeb3delay_bitlo,&data_word);
    InsertValueIntoDataWord(cfeb4delay_,cfeb4delay_bithi,cfeb4delay_bitlo,&data_word);
    //
  } else if ( address == vme_ratctrl_adr ) {
    //------------------------------------------------------------------
    //0X1E = ADR_RATCTRL:  RAT Module Control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(rpc_sync_  ,rpc_sync_bithi  ,rpc_sync_bitlo  ,&data_word);
    InsertValueIntoDataWord(shift_rpc_ ,shift_rpc_bithi ,shift_rpc_bitlo ,&data_word);
    InsertValueIntoDataWord(rat_dsn_en_,rat_dsn_en_bithi,rat_dsn_en_bitlo,&data_word);
    //
  } else if ( address == ccb_trig_adr ) {
    //------------------------------------------------------------------
    //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_ext_trig_l1aen_   ,alct_ext_trig_l1aen_bithi   ,alct_ext_trig_l1aen_bitlo   ,&data_word);
    InsertValueIntoDataWord(clct_ext_trig_l1aen_   ,clct_ext_trig_l1aen_bithi   ,clct_ext_trig_l1aen_bitlo   ,&data_word);
    InsertValueIntoDataWord(request_l1a_           ,request_l1a_bithi           ,request_l1a_bitlo           ,&data_word);
    InsertValueIntoDataWord(alct_ext_trig_vme_     ,alct_ext_trig_vme_bithi     ,alct_ext_trig_vme_bitlo     ,&data_word);
    InsertValueIntoDataWord(clct_ext_trig_vme_     ,clct_ext_trig_vme_bithi     ,clct_ext_trig_vme_bitlo     ,&data_word);
    InsertValueIntoDataWord(ext_trig_both_         ,ext_trig_both_bithi         ,ext_trig_both_bitlo         ,&data_word);
    InsertValueIntoDataWord(ccb_allow_bypass_      ,ccb_allow_bypass_bithi      ,ccb_allow_bypass_bitlo      ,&data_word);
    InsertValueIntoDataWord(ignore_ccb_startstop_  ,ignore_ccb_startstop_bithi  ,ignore_ccb_startstop_bitlo  ,&data_word);
    InsertValueIntoDataWord(internal_l1a_delay_vme_,internal_l1a_delay_vme_bithi,internal_l1a_delay_vme_bitlo,&data_word);
    //
  } else if ( address == alct_cfg_adr ) {
    //------------------------------------------------------------------
    //0X30 = ADR_ALCT_CFG:  ALCT Configuration
    //------------------------------------------------------------------
    InsertValueIntoDataWord(cfg_alct_ext_trig_en_  ,cfg_alct_ext_trig_en_bithi  ,cfg_alct_ext_trig_en_bitlo  ,&data_word);
    InsertValueIntoDataWord(cfg_alct_ext_inject_en_,cfg_alct_ext_inject_en_bithi,cfg_alct_ext_inject_en_bitlo,&data_word);
    InsertValueIntoDataWord(cfg_alct_ext_trig_     ,cfg_alct_ext_trig_bithi     ,cfg_alct_ext_trig_bitlo     ,&data_word);
    InsertValueIntoDataWord(cfg_alct_ext_inject_   ,cfg_alct_ext_inject_bithi   ,cfg_alct_ext_inject_bitlo   ,&data_word);
    InsertValueIntoDataWord(alct_seq_cmd_          ,alct_seq_cmd_bithi          ,alct_seq_cmd_bitlo          ,&data_word);
    InsertValueIntoDataWord(alct_clock_en_use_ccb_ ,alct_clock_en_use_ccb_bithi ,alct_clock_en_use_ccb_bitlo ,&data_word);
    InsertValueIntoDataWord(alct_clock_en_use_vme_ ,alct_clock_en_use_vme_bithi ,alct_clock_en_use_vme_bitlo ,&data_word);
    //
  } else if ( address == alct_inj_adr ) {
    //------------------------------------------------------------------
    //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_clear_     ,alct_clear_bithi     ,alct_clear_bitlo     ,&data_word);
    InsertValueIntoDataWord(alct_inject_mux_,alct_inject_mux_bithi,alct_inject_mux_bitlo,&data_word);
    InsertValueIntoDataWord(alct_sync_clct_ ,alct_sync_clct_bithi ,alct_sync_clct_bitlo ,&data_word);
    InsertValueIntoDataWord(alct_inj_delay_ ,alct_inj_delay_bithi ,alct_inj_delay_bitlo ,&data_word);
    //
  } else if ( address == cfeb_inj_adr ) {
    //------------------------------------------------------------------
    //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(enableCLCTInputs_ ,enableCLCTInputs_bithi ,enableCLCTInputs_bitlo ,&data_word);
    InsertValueIntoDataWord(cfeb_ram_sel_     ,cfeb_ram_sel_bithi     ,cfeb_ram_sel_bitlo     ,&data_word);
    InsertValueIntoDataWord(cfeb_inj_en_sel_  ,cfeb_inj_en_sel_bithi  ,cfeb_inj_en_sel_bitlo  ,&data_word);
    InsertValueIntoDataWord(start_pattern_inj_,start_pattern_inj_bithi,start_pattern_inj_bitlo,&data_word);
    //
  } else if ( address == hcm001_adr || address == hcm023_adr || address == hcm045_adr ||
	      address == hcm101_adr || address == hcm123_adr || address == hcm145_adr ||
	      address == hcm201_adr || address == hcm223_adr || address == hcm245_adr ||
	      address == hcm301_adr || address == hcm323_adr || address == hcm345_adr ||
	      address == hcm401_adr || address == hcm423_adr || address == hcm445_adr ) {
    //------------------------------------------------------------------
    //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
    //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
    //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
    //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
    //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
    //------------------------------------------------------------------
    for (int bit_in_register=0; bit_in_register<16; bit_in_register++) {
      //
      // get the layer and distrip channels covered by this register
      int layer   = GetHotChannelLayerFromMap_(address,bit_in_register);
      int distrip = GetHotChannelDistripFromMap_(address,bit_in_register);
      //
      InsertValueIntoDataWord(hot_channel_mask_[layer][distrip],bit_in_register,bit_in_register,&data_word);
    }
    //
  } else if ( address == seq_trig_en_adr ) {
    //------------------------------------------------------------------
    //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
    //------------------------------------------------------------------
    InsertValueIntoDataWord(clct_pat_trig_en_  ,clct_pat_trig_en_bithi  ,clct_pat_trig_en_bitlo  ,&data_word);
    InsertValueIntoDataWord(alct_pat_trig_en_  ,alct_pat_trig_en_bithi  ,alct_pat_trig_en_bitlo  ,&data_word);
    InsertValueIntoDataWord(match_pat_trig_en_ ,match_pat_trig_en_bithi ,match_pat_trig_en_bitlo ,&data_word);
    InsertValueIntoDataWord(adb_ext_trig_en_   ,adb_ext_trig_en_bithi   ,adb_ext_trig_en_bitlo   ,&data_word);
    InsertValueIntoDataWord(dmb_ext_trig_en_   ,dmb_ext_trig_en_bithi   ,dmb_ext_trig_en_bitlo   ,&data_word);
    InsertValueIntoDataWord(clct_ext_trig_en_  ,clct_ext_trig_en_bithi  ,clct_ext_trig_en_bitlo  ,&data_word);
    InsertValueIntoDataWord(alct_ext_trig_en_  ,alct_ext_trig_en_bithi  ,alct_ext_trig_en_bitlo  ,&data_word);
    InsertValueIntoDataWord(vme_ext_trig_      ,vme_ext_trig_bithi      ,vme_ext_trig_bitlo      ,&data_word);
    InsertValueIntoDataWord(ext_trig_inject_   ,ext_trig_inject_bithi   ,ext_trig_inject_bitlo   ,&data_word);
    InsertValueIntoDataWord(all_cfeb_active_   ,all_cfeb_active_bithi   ,all_cfeb_active_bitlo   ,&data_word);
    InsertValueIntoDataWord(cfebs_enabled_     ,cfebs_enabled_bithi     ,cfebs_enabled_bitlo     ,&data_word);
    InsertValueIntoDataWord(cfeb_enable_source_,cfeb_enable_source_bithi,cfeb_enable_source_bitlo,&data_word);
    //    
  } else if ( address == seq_trig_dly0_adr ) {
    //------------------------------------------------------------------
    //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_pretrig_width_,alct_pretrig_width_bithi,alct_pretrig_width_bitlo,&data_word);
    InsertValueIntoDataWord(alct_pretrig_delay_,alct_pretrig_delay_bithi,alct_pretrig_delay_bitlo,&data_word);
    InsertValueIntoDataWord(alct_pattern_delay_,alct_pattern_delay_bithi,alct_pattern_delay_bitlo,&data_word);
    InsertValueIntoDataWord(adb_ext_trig_delay_,adb_ext_trig_delay_bithi,adb_ext_trig_delay_bitlo,&data_word);
    //
  } else if ( address == seq_trig_dly1_adr ) {
    //------------------------------------------------------------------
    //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
    //------------------------------------------------------------------
    InsertValueIntoDataWord(dmb_ext_trig_delay_ ,dmb_ext_trig_delay_bithi ,dmb_ext_trig_delay_bitlo ,&data_word);
    InsertValueIntoDataWord(clct_ext_trig_delay_,clct_ext_trig_delay_bithi,clct_ext_trig_delay_bitlo,&data_word);
    InsertValueIntoDataWord(alct_ext_trig_delay_,alct_ext_trig_delay_bithi,alct_ext_trig_delay_bitlo,&data_word);
    InsertValueIntoDataWord(layer_trig_delay_   ,layer_trig_delay_bithi   ,layer_trig_delay_bitlo   ,&data_word);
    //
  } else if ( address == seq_id_adr ) {
    //------------------------------------------------------------------
    //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
    //------------------------------------------------------------------
    InsertValueIntoDataWord(tmb_slot_,tmb_slot_bithi,tmb_slot_bitlo,&data_word);
    InsertValueIntoDataWord(csc_id_  ,csc_id_bithi  ,csc_id_bitlo  ,&data_word);
    InsertValueIntoDataWord(run_id_  ,run_id_bithi  ,run_id_bitlo  ,&data_word);
    //
  } else if ( address == seq_clct_adr ) {
    //------------------------------------------------------------------
    //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
    //------------------------------------------------------------------
    InsertValueIntoDataWord(triad_persist_    ,triad_persist_bithi    ,triad_persist_bitlo    ,&data_word);
    InsertValueIntoDataWord(hs_pretrig_thresh_,hs_pretrig_thresh_bithi,hs_pretrig_thresh_bitlo,&data_word);
    //InsertValueIntoDataWord(ds_pretrig_thresh_,ds_pretrig_thresh_bithi,ds_pretrig_thresh_bitlo,&data_word);
    InsertValueIntoDataWord(min_hits_pattern_ ,min_hits_pattern_bithi ,min_hits_pattern_bitlo ,&data_word);
    InsertValueIntoDataWord(drift_delay_      ,drift_delay_bithi      ,drift_delay_bitlo      ,&data_word);
    InsertValueIntoDataWord(pretrigger_halt_  ,pretrigger_halt_bithi  ,pretrigger_halt_bitlo  ,&data_word);
    //
  } else if ( address == seq_fifo_adr ) {
    //------------------------------------------------------------------
    //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
    //------------------------------------------------------------------
    InsertValueIntoDataWord(fifo_mode_   ,fifo_mode_bithi   ,fifo_mode_bitlo   ,&data_word);
    InsertValueIntoDataWord(fifo_tbins_  ,fifo_tbins_bithi  ,fifo_tbins_bitlo  ,&data_word);
    InsertValueIntoDataWord(fifo_pretrig_,fifo_pretrig_bithi,fifo_pretrig_bitlo,&data_word);
    //
  } else if ( address == seq_l1a_adr ) {
    //------------------------------------------------------------------
    //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
    //------------------------------------------------------------------
    InsertValueIntoDataWord(l1adelay_        ,l1adelay_bithi        ,l1adelay_bitlo        ,&data_word);
    InsertValueIntoDataWord(l1a_window_size_ ,l1a_window_size_bithi ,l1a_window_size_bitlo ,&data_word);
    InsertValueIntoDataWord(tmb_l1a_internal_,tmb_l1a_internal_bithi,tmb_l1a_internal_bitlo,&data_word);
    //
  } else if ( address == seq_offset_adr ) {
    //------------------------------------------------------------------
    //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
    //------------------------------------------------------------------
    InsertValueIntoDataWord(l1a_offset_,l1a_offset_bithi,l1a_offset_bitlo,&data_word);
    InsertValueIntoDataWord(bxn_offset_,bxn_offset_bithi,bxn_offset_bitlo,&data_word);
    //    
  } else if ( address == tmb_trig_adr ) {
    //------------------------------------------------------------------
    //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
    //------------------------------------------------------------------
    InsertValueIntoDataWord(tmb_sync_err_enable_,tmb_sync_err_enable_bithi,tmb_sync_err_enable_bitlo,&data_word);
    InsertValueIntoDataWord(tmb_allow_alct_     ,tmb_allow_alct_bithi     ,tmb_allow_alct_bitlo     ,&data_word);
    InsertValueIntoDataWord(tmb_allow_clct_     ,tmb_allow_clct_bithi     ,tmb_allow_clct_bitlo     ,&data_word);
    InsertValueIntoDataWord(tmb_allow_match_    ,tmb_allow_match_bithi    ,tmb_allow_match_bitlo    ,&data_word);
    InsertValueIntoDataWord(mpc_delay_          ,mpc_delay_bithi          ,mpc_delay_bitlo          ,&data_word);
    InsertValueIntoDataWord(mpc_sel_ttc_bx0_    ,mpc_sel_ttc_bx0_bithi    ,mpc_sel_ttc_bx0_bitlo    ,&data_word);
    InsertValueIntoDataWord(mpc_idle_blank_     ,mpc_idle_blank_bithi     ,mpc_idle_blank_bitlo     ,&data_word);
    InsertValueIntoDataWord(mpc_output_enable_  ,mpc_output_enable_bithi  ,mpc_output_enable_bitlo  ,&data_word);
    //
  } else if ( address == alctfifo1_adr ) {
    //------------------------------------------------------------------
    //0XA2 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_raw_reset_       ,alct_raw_reset_bithi       ,alct_raw_reset_bitlo       ,&data_word);
    InsertValueIntoDataWord(alct_raw_read_address_,alct_raw_read_address_bithi,alct_raw_read_address_bitlo,&data_word);
    InsertValueIntoDataWord(alct_raw_sync_        ,alct_raw_sync_bithi        ,alct_raw_sync_bitlo        ,&data_word);
    InsertValueIntoDataWord(alct_demux_mode_      ,alct_demux_mode_bithi      ,alct_demux_mode_bitlo      ,&data_word);
    //
  } else if ( address == seqmod_adr ) {
    //------------------------------------------------------------------
    //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
    //------------------------------------------------------------------
    InsertValueIntoDataWord(clct_flush_delay_   ,clct_flush_delay_bithi   ,clct_flush_delay_bitlo   ,&data_word);
    InsertValueIntoDataWord(clct_turbo_         ,clct_turbo_bithi         ,clct_turbo_bitlo         ,&data_word);
    InsertValueIntoDataWord(ranlct_enable_      ,ranlct_enable_bithi      ,ranlct_enable_bitlo      ,&data_word);
    InsertValueIntoDataWord(wrt_buf_required_   ,wrt_buf_required_bithi   ,wrt_buf_required_bitlo   ,&data_word);
    InsertValueIntoDataWord(valid_clct_required_,valid_clct_required_bithi,valid_clct_required_bitlo,&data_word);
    InsertValueIntoDataWord(l1a_allow_match_    ,l1a_allow_match_bithi    ,l1a_allow_match_bitlo    ,&data_word);
    InsertValueIntoDataWord(l1a_allow_notmb_    ,l1a_allow_notmb_bithi    ,l1a_allow_notmb_bitlo    ,&data_word);
    InsertValueIntoDataWord(l1a_allow_nol1a_    ,l1a_allow_nol1a_bithi    ,l1a_allow_nol1a_bitlo    ,&data_word);
    InsertValueIntoDataWord(l1a_allow_alct_only_,l1a_allow_alct_only_bithi,l1a_allow_alct_only_bitlo,&data_word);
    InsertValueIntoDataWord(scint_veto_clr_     ,scint_veto_clr_bithi     ,scint_veto_clr_bitlo     ,&data_word);
    //
  } else if ( address == tmbtim_adr ) {
    //------------------------------------------------------------------
    //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
    //------------------------------------------------------------------
    InsertValueIntoDataWord(alct_vpf_delay_        ,alct_vpf_delay_bithi        ,alct_vpf_delay_bitlo        ,&data_word);
    InsertValueIntoDataWord(alct_match_window_size_,alct_match_window_size_bithi,alct_match_window_size_bitlo,&data_word);
    InsertValueIntoDataWord(mpc_tx_delay_          ,mpc_tx_delay_bithi          ,mpc_tx_delay_bitlo          ,&data_word);
    //
  } else if ( address == rpc_cfg_adr ) {
    //------------------------------------------------------------------
    //0XB6 = ADR_RPC_CFG:  RPC Configuration
    //------------------------------------------------------------------
    InsertValueIntoDataWord(rpc_exists_     ,rpc_exists_bithi     ,rpc_exists_bitlo     ,&data_word);
    InsertValueIntoDataWord(rpc_read_enable_,rpc_read_enable_bithi,rpc_read_enable_bitlo,&data_word);
    InsertValueIntoDataWord(rpc_bxn_offset_ ,rpc_bxn_offset_bithi ,rpc_bxn_offset_bitlo ,&data_word);
    InsertValueIntoDataWord(rpc_bank_       ,rpc_bank_bithi       ,rpc_bank_bitlo       ,&data_word);
    //
  } else if ( address == rpc_raw_delay_adr ) {
    //------------------------------------------------------------------
    //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
    //------------------------------------------------------------------
    InsertValueIntoDataWord(rpc0_raw_delay_,rpc0_raw_delay_bithi,rpc0_raw_delay_bitlo,&data_word);
    InsertValueIntoDataWord(rpc1_raw_delay_,rpc1_raw_delay_bithi,rpc1_raw_delay_bitlo,&data_word);
    //
  } else if ( address == rpc_inj_adr ) {
    //------------------------------------------------------------------
    //0XBC = ADR_RPC_INJ:  RPC Injector Control
    //------------------------------------------------------------------
    InsertValueIntoDataWord(rpc_mask_all_ ,rpc_mask_all_bithi ,rpc_mask_all_bitlo ,&data_word); 
    InsertValueIntoDataWord(inj_mask_rat_ ,inj_mask_rat_bithi ,inj_mask_rat_bitlo ,&data_word); 
    InsertValueIntoDataWord(inj_mask_rpc_ ,inj_mask_rpc_bithi ,inj_mask_rpc_bitlo ,&data_word); 
    InsertValueIntoDataWord(inj_delay_rat_,inj_delay_rat_bithi,inj_delay_rat_bitlo,&data_word); 
    InsertValueIntoDataWord(rpc_inj_sel_  ,rpc_inj_sel_bithi  ,rpc_inj_sel_bitlo  ,&data_word); 
    InsertValueIntoDataWord(rpc_inj_wdata_,rpc_inj_wdata_bithi,rpc_inj_wdata_bitlo,&data_word); 
    //
  } else if ( address == jtag_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
    //------------------------------------------------------------------
    InsertValueIntoDataWord(jtag_state_machine_start_   ,jtag_state_machine_start_bithi   ,jtag_state_machine_start_bitlo   ,&data_word);
    InsertValueIntoDataWord(jtag_state_machine_sreset_  ,jtag_state_machine_sreset_bithi  ,jtag_state_machine_sreset_bitlo  ,&data_word);
    InsertValueIntoDataWord(jtag_disable_write_to_adr10_,jtag_disable_write_to_adr10_bithi,jtag_disable_write_to_adr10_bitlo,&data_word);
    InsertValueIntoDataWord(jtag_state_machine_throttle_,jtag_state_machine_throttle_bithi,jtag_state_machine_throttle_bitlo,&data_word);
    //
  } else if ( address == vme_sm_ctrl_adr ) {
    //------------------------------------------------------------------
    //0XDA = ADR_VMESM0:  VME State Machine Control (reads VME PROM)
    //------------------------------------------------------------------
    InsertValueIntoDataWord(vme_state_machine_start_    ,vme_state_machine_start_bithi    ,vme_state_machine_start_bitlo    ,&data_word);
    InsertValueIntoDataWord(vme_state_machine_sreset_   ,vme_state_machine_sreset_bithi   ,vme_state_machine_sreset_bitlo   ,&data_word);
    InsertValueIntoDataWord(vme_state_machine_jtag_auto_,vme_state_machine_jtag_auto_bithi,vme_state_machine_jtag_auto_bitlo,&data_word);
    InsertValueIntoDataWord(vme_state_machine_throttle_ ,vme_state_machine_throttle_bithi ,vme_state_machine_throttle_bitlo ,&data_word);
    //
  } else if ( address == rat_3d_delays_adr ) {
    //------------------------------------------------------------------
    //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
    //------------------------------------------------------------------
    InsertValueIntoDataWord(rpc0_rat_delay_,rpc0_rat_delay_bithi,rpc0_rat_delay_bitlo,&data_word);
    InsertValueIntoDataWord(rpc1_rat_delay_,rpc1_rat_delay_bithi,rpc1_rat_delay_bitlo,&data_word);
    //
  } else if ( address == layer_trg_mode_adr ) {
    //---------------------------------------------------------------------
    //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
    //---------------------------------------------------------------------
    InsertValueIntoDataWord(layer_trigger_en_ ,layer_trigger_en_bithi ,layer_trigger_en_bitlo ,&data_word); 
    InsertValueIntoDataWord(layer_trig_thresh_,layer_trig_thresh_bithi,layer_trig_thresh_bitlo,&data_word); 
    //
  } else if ( address == pattern_find_pretrg_adr ) {
    //---------------------------------------------------------------------
    //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
    //---------------------------------------------------------------------
    InsertValueIntoDataWord(clct_blanking_         ,clct_blanking_bithi         ,clct_blanking_bitlo         ,&data_word);
    InsertValueIntoDataWord(clct_stagger_          ,clct_stagger_bithi          ,clct_stagger_bitlo          ,&data_word);
    InsertValueIntoDataWord(clct_pattern_id_thresh_,clct_pattern_id_thresh_bithi,clct_pattern_id_thresh_bitlo,&data_word);
    InsertValueIntoDataWord(aff_thresh_            ,aff_thresh_bithi            ,aff_thresh_bitlo            ,&data_word);
    //
  } else if ( address == clct_separation_adr ) {
    //---------------------------------------------------------------------
    //0XF6 = ADR_TEMP1:  CLCT separation
    //---------------------------------------------------------------------
    InsertValueIntoDataWord(clct_separation_src_             ,clct_separation_src_bithi             ,clct_separation_src_bitlo             ,&data_word);
    InsertValueIntoDataWord(clct_separation_ram_write_enable_,clct_separation_ram_write_enable_bithi,clct_separation_ram_write_enable_bitlo,&data_word);
    InsertValueIntoDataWord(clct_separation_ram_adr_         ,clct_separation_ram_adr_bithi         ,clct_separation_ram_adr_bitlo         ,&data_word);
    InsertValueIntoDataWord(min_clct_separation_             ,min_clct_separation_bithi             ,min_clct_separation_bitlo             ,&data_word);
    //
  } else {
    //
    (*MyOutput_) << "TMB: ERROR in FillTMBRegister, VME address = " << address << " not supported to be filled" << std::endl;
    data_word = 0xf0000;  //make data word too big to fit in TMB register
    //
  }
  //
  return data_word;
}
//
//
////////////////////////////////////////////////////////////////////////////////////////
// Check read values versus expected values 
////////////////////////////////////////////////////////////////////////////////////////
void TMB::CheckTMBConfiguration() {
  //
  bool config_ok = true;
  //
  ReadTMBConfiguration();    // fill the read values in the software
  //
  // Check if user has forced CFEB enable bits in register 0x68 to be copied from 0x42
  // If yes => expected value of address 0x68 = write value of address 0x42 
  // If no  => expected value of address 0x68 = write value of address 0x68 
  //
  int cfebs_enabled_expected;
  //
  if (GetCfebEnableSource() == 1) {
    cfebs_enabled_expected = enableCLCTInputs_;
  } else {
    cfebs_enabled_expected = cfebs_enabled_; 
  }
  //
  //-----------------------------------------------------------------
  // firmware information
  //-----------------------------------------------------------------
  config_ok &= compareValues("TMB Firmware Day"    ,GetReadTmbFirmwareDay()    ,GetExpectedTmbFirmwareDay()    );
  config_ok &= compareValues("TMB Firmware Month"  ,GetReadTmbFirmwareMonth()  ,GetExpectedTmbFirmwareMonth()  );
  config_ok &= compareValues("TMB Firmware Year"   ,GetReadTmbFirmwareYear()   ,GetExpectedTmbFirmwareYear()   );
  config_ok &= compareValues("TMB Firmware Type"   ,GetReadTmbFirmwareType()   ,GetExpectedTmbFirmwareType()   );
  config_ok &= compareValues("TMB Firmware Version",GetReadTmbFirmwareVersion(),GetExpectedTmbFirmwareVersion());
  config_ok &= compareValues("TMB Firmware Revcode",GetReadTmbFirmwareRevcode(),GetExpectedTmbFirmwareRevcode());
  //
  //-----------------------------------------------------------------
  //0X70000 = ADR_BOOT:  Hardware Bootstrap Register
  //-----------------------------------------------------------------
  config_ok &= compareValues("Boot register control JTAG chain",read_boot_control_jtag_chain_,boot_control_jtag_chain_expected);
  //
  //-----------------------------------------------------------------
  //0X0E = ADR_LOOPBK:  Loop-Back Control Register
  //-----------------------------------------------------------------
  config_ok &= compareValues("enable_alct_tx",read_enable_alct_tx_,enable_alct_tx_);
  config_ok &= compareValues("ALCT_input"    ,read_ALCT_input_    ,ALCT_input_    );
  //
  //------------------------------------------------------------------
  //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB-ALCT rx phase",read_alct_rx_clock_delay_,alct_rx_clock_delay_);
  config_ok &= compareValues("TMB-ALCT tx phase",read_alct_tx_clock_delay_,alct_tx_clock_delay_);
  config_ok &= compareValues("DMB tx phase"     ,read_dmb_tx_delay_       ,dmb_tx_delay_       );
  config_ok &= compareValues("TMB-RAT phase"    ,read_rat_tmb_delay_      ,rat_tmb_delay_      );
  //
  //------------------------------------------------------------------
  //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB1 phase"      ,read_tmb1_phase_,tmb1_phase_);
  config_ok &= compareValues("MPC phase"       ,read_mpc_phase_ ,mpc_phase_ );
  config_ok &= compareValues("DCC phase"       ,read_dcc_phase_ ,dcc_phase_ );
  config_ok &= compareValues("TMB-CFEB 0 phase",read_cfeb0delay_,cfeb0delay_);
  //
  //------------------------------------------------------------------
  //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB-CFEB 1 phase",read_cfeb1delay_,cfeb1delay_);
  config_ok &= compareValues("TMB-CFEB 2 phase",read_cfeb2delay_,cfeb2delay_);
  config_ok &= compareValues("TMB-CFEB 3 phase",read_cfeb3delay_,cfeb3delay_);
  config_ok &= compareValues("TMB-CFEB 4 phase",read_cfeb4delay_,cfeb4delay_);
  //
  //------------------------------------------------------------------
  //0X1E = ADR_RATCTRL:  RAT Module Control
  //------------------------------------------------------------------
  //  config_ok &= compareValues("Shift RPC 1/2 phase"             ,read_shift_rpc_ ,shift_rpc_ );
  //  config_ok &= compareValues("RPC sync"                        ,read_rpc_sync_  ,rpc_sync_  );
  //  config_ok &= compareValues("RAT digital serial number enable",read_rat_dsn_en_,rat_dsn_en_);
  //
  //------------------------------------------------------------------
  //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
  //------------------------------------------------------------------
  config_ok &= compareValues("Request CCB L1a on ALCT ext trig"     ,read_alct_ext_trig_l1aen_   ,alct_ext_trig_l1aen_   );
  config_ok &= compareValues("Request CCB L1a on CLCT ext trig"     ,read_clct_ext_trig_l1aen_   ,clct_ext_trig_l1aen_   );
  config_ok &= compareValues("Request CCB L1a on sequencer trigger" ,read_request_l1a_           ,request_l1a_           );
  config_ok &= compareValues("Fire ALCT ext trig one-shot"          ,read_alct_ext_trig_vme_     ,alct_ext_trig_vme_     );
  config_ok &= compareValues("Fire CLCT ext trig one-shot"          ,read_clct_ext_trig_vme_     ,clct_ext_trig_vme_     );
  config_ok &= compareValues("CLCText fire ALCT + ALCText fire CLCT",read_ext_trig_both_         ,ext_trig_both_         );
  config_ok &= compareValues("allow CLCTextCCB when ccb_ignore_rx=1",read_ccb_allow_bypass_      ,ccb_allow_bypass_      );
  config_ok &= compareValues("Ignore CCB trig start/stop"           ,read_ignore_ccb_startstop_  ,ignore_ccb_startstop_  );
  config_ok &= compareValues("Internal L1A delay (VME)"             ,read_internal_l1a_delay_vme_,internal_l1a_delay_vme_);
  //
  //------------------------------------------------------------------
  //0X30 = ADR_ALCT_CFG:  ALCT Configuration
  //------------------------------------------------------------------
  config_ok &= compareValues("Enable alct_ext_trig from CCB"           ,read_cfg_alct_ext_trig_en_  ,cfg_alct_ext_trig_en_  );
  config_ok &= compareValues("Enable alct_ext_inject from CCB"         ,read_cfg_alct_ext_inject_en_,cfg_alct_ext_inject_en_);
  config_ok &= compareValues("Assert alct_ext_trig"  		       ,read_cfg_alct_ext_trig_     ,cfg_alct_ext_trig_     ); 
  config_ok &= compareValues("Assert alct_ext_inject"		       ,read_cfg_alct_ext_inject_   ,cfg_alct_ext_inject_   );
  config_ok &= compareValues("ALCT sequencer command" 		       ,read_alct_seq_cmd_          ,alct_seq_cmd_          );
  config_ok &= compareValues("alct_clock_en_vme=ccb_clock40_enable"    ,read_alct_clock_en_use_ccb_ ,alct_clock_en_use_ccb_ );
  config_ok &= compareValues("set alct_clock_en scsi signal if above=0",read_alct_clock_en_use_vme_ ,alct_clock_en_use_vme_ );
  //
  //------------------------------------------------------------------
  //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
  //------------------------------------------------------------------
  config_ok &= compareValues("alct_clear"                       ,read_alct_clear_     ,alct_clear_     );
  config_ok &= compareValues("start ALCT injector state machine",read_alct_inject_mux_,alct_inject_mux_);
  config_ok &= compareValues("sync ALCT injector with CLCT"     ,read_alct_sync_clct_ ,alct_sync_clct_ );
  config_ok &= compareValues("ALCT injector delay"              ,read_alct_inj_delay_ ,alct_inj_delay_ );
  //
  //------------------------------------------------------------------
  //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
  //------------------------------------------------------------------
  config_ok &= compareValues("Reg 42:  Enable CFEB n to trigger" ,read_enableCLCTInputs_ ,enableCLCTInputs_ );
  config_ok &= compareValues("Select CFEB n for RAM read/write"  ,read_cfeb_ram_sel_     ,cfeb_ram_sel_     );
  config_ok &= compareValues("Enable CFEB n for injector trigger",read_cfeb_inj_en_sel_  ,cfeb_inj_en_sel_  ); 
  config_ok &= compareValues("Start CLCT pattern injector"       ,read_start_pattern_inj_,start_pattern_inj_);
  //
  //------------------------------------------------------------------
  //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
  //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
  //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
  //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
  //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
  //------------------------------------------------------------------
  for (int layer=0; layer<MAX_NUM_LAYERS; layer++) {
    for (int distrip=0; distrip<MAX_NUM_DISTRIPS_PER_LAYER; distrip++) {
      std::ostringstream hotmasklabel;
      hotmasklabel << "CLCT Hot Channel Mask Layer " << layer << " Distrip " << distrip; 
      config_ok &= compareValues(hotmasklabel.str(),
				 read_hot_channel_mask_[layer][distrip],
				 hot_channel_mask_[layer][distrip] );
    }
  }
  //
  //------------------------------------------------------------------
  //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
  //N.B. See TMB documentation first before controlling CFEBs through this register...
  //------------------------------------------------------------------
  config_ok &= compareValues("Allow CLCT pretrigger"                   ,read_clct_pat_trig_en_  ,clct_pat_trig_en_     );
  config_ok &= compareValues("Allow ALCT pretrigger"                   ,read_alct_pat_trig_en_  ,alct_pat_trig_en_     );
  config_ok &= compareValues("Allow ALCT*CLCT pretrigger"              ,read_match_pat_trig_en_ ,match_pat_trig_en_    );
  config_ok &= compareValues("Allow ADB ext trig (CCB) for pretrigger" ,read_adb_ext_trig_en_   ,adb_ext_trig_en_      );
  config_ok &= compareValues("Allow DMB ext trig for pretrigger"       ,read_dmb_ext_trig_en_   ,dmb_ext_trig_en_      );
  config_ok &= compareValues("Allow CLCT ext trig (scint) for pretrig" ,read_clct_ext_trig_en_  ,clct_ext_trig_en_     );
  config_ok &= compareValues("Allow ALCT ext trig (CCB) for pretrigger",read_alct_ext_trig_en_  ,alct_ext_trig_en_     );
  config_ok &= compareValues("Initiate sequencer trigger from VME"     ,read_vme_ext_trig_      ,vme_ext_trig_         );
  config_ok &= compareValues("Make clct_ext_trig fire pattern injector",read_ext_trig_inject_   ,ext_trig_inject_      );
  config_ok &= compareValues("Make all CFEBs active when triggered"    ,read_all_cfeb_active_   ,all_cfeb_active_      );
  config_ok &= compareValues("Reg 0x68:  Enable CFEB n to trigger"     ,read_cfebs_enabled_     ,cfebs_enabled_expected);
  config_ok &= compareValues("CFEB enable mask from register 0x42"     ,read_cfeb_enable_source_,cfeb_enable_source_   );
  //
  //------------------------------------------------------------------
  //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  config_ok &= compareValues("ALCT*CLCT pretrigger match window width",read_alct_pretrig_width_,alct_pretrig_width_);
  config_ok &= compareValues("ALCT*CLCT pretrigger ALCT delay"        ,read_alct_pretrig_delay_,alct_pretrig_delay_);
  config_ok &= compareValues("Active FEB Flag delay"                  ,read_alct_pattern_delay_,alct_pattern_delay_);
  config_ok &= compareValues("Delay adb_ext_trig from CCB"            ,read_adb_ext_trig_delay_,adb_ext_trig_delay_);
  //
  //------------------------------------------------------------------
  //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  config_ok &= compareValues("Delay dmb_ext_trig from DMB"         ,read_dmb_ext_trig_delay_  ,dmb_ext_trig_delay_  );
  config_ok &= compareValues("Delay clct_ext_trig (scint) from CCB",read_clct_ext_trig_delay_ ,clct_ext_trig_delay_ );
  config_ok &= compareValues("Delay alct_ext_trig from CCB"        ,read_alct_ext_trig_delay_ ,alct_ext_trig_delay_ );
  config_ok &= compareValues("layer trigger delay"                 ,read_layer_trig_delay_    ,layer_trig_delay_    );
  //
  //------------------------------------------------------------------
  //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB slot",read_tmb_slot_,tmb_slot_);
  config_ok &= compareValues("CSC ID"  ,read_csc_id_  ,csc_id_  );
  config_ok &= compareValues("Run ID"  ,read_run_id_  ,run_id_  );
  //
  //------------------------------------------------------------------
  //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
  //------------------------------------------------------------------
  config_ok &= compareValues("Triad persistence"               ,read_triad_persist_    ,triad_persist_    );
  config_ok &= compareValues("CLCT 1/2-strip pretrig threshold",read_hs_pretrig_thresh_,hs_pretrig_thresh_);
  //config_ok &= compareValues("CLCT di-strip pretrig threshold" ,read_ds_pretrig_thresh_,ds_pretrig_thresh_);
  config_ok &= compareValues("CLCT pattern threshold"          ,read_min_hits_pattern_ ,min_hits_pattern_ );
  config_ok &= compareValues("CLCT Drift Delay"                ,read_drift_delay_      ,drift_delay_      );
  //
  //------------------------------------------------------------------
  //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB fifo mode"   ,read_fifo_mode_   ,fifo_mode_   );
  config_ok &= compareValues("TMB fifo Tbins"  ,read_fifo_tbins_  ,fifo_tbins_  );
  config_ok &= compareValues("TMB fifo pretrig",read_fifo_pretrig_,fifo_pretrig_);
  //
  //------------------------------------------------------------------
  //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB L1a delay"                             ,read_l1adelay_        ,l1adelay_        );
  config_ok &= compareValues("TMB L1a window size"                       ,read_l1a_window_size_ ,l1a_window_size_ );
  config_ok &= compareValues("Generate internal L1a (overrides external)",read_tmb_l1a_internal_,tmb_l1a_internal_);
  //
  //------------------------------------------------------------------
  //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
  //------------------------------------------------------------------
  config_ok &= compareValues("TMB L1a counter offset"   ,read_l1a_offset_,l1a_offset_);
  config_ok &= compareValues("TMB Bunch Crossing offset",read_bxn_offset_,bxn_offset_);
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  config_ok &= compareValues("Allow sync err to MPC for each muon"         ,read_tmb_sync_err_enable_,tmb_sync_err_enable_);
  config_ok &= compareValues("Allow ALCT-only trigger"                     ,read_tmb_allow_alct_     ,tmb_allow_alct_     );
  config_ok &= compareValues("Allow CLCT-only trigger"                     ,read_tmb_allow_clct_     ,tmb_allow_clct_     );
  config_ok &= compareValues("Allow ALCT*CLCT trigger"                     ,read_tmb_allow_match_    ,tmb_allow_match_    );
  config_ok &= compareValues("MPC rx delay"                                ,read_mpc_delay_          ,mpc_delay_          );
  config_ok &= compareValues("MPC gets TTC BX0"                            ,read_mpc_sel_ttc_bx0_    ,mpc_sel_ttc_bx0_    );
  config_ok &= compareValues("Blank MPC data and BX0 except when triggered",read_mpc_idle_blank_     ,mpc_idle_blank_     );
  config_ok &= compareValues("Enable output to MPC"                        ,read_mpc_output_enable_  ,mpc_output_enable_  );
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  config_ok &= compareValues("Trigger seq flush state timer"                  ,read_clct_flush_delay_   ,clct_flush_delay_   );
  config_ok &= compareValues("Disable raw hits (turbo mode)"                  ,read_clct_turbo_         ,clct_turbo_         );
  config_ok &= compareValues("Enable OSU random LCT generator"                ,read_ranlct_enable_      ,ranlct_enable_      );
  config_ok &= compareValues("Require wr_buffer available to pretrigger"      ,read_wrt_buf_required_   ,wrt_buf_required_   );
  config_ok &= compareValues("Require valid CLCT after drift delay"           ,read_valid_clct_required_,valid_clct_required_);
  config_ok &= compareValues("Allow tmb trig pulse in L1a window"             ,read_l1a_allow_match_    ,l1a_allow_match_    );
  config_ok &= compareValues("Allow no TMB trig pulse in L1a window"          ,read_l1a_allow_notmb_    ,l1a_allow_notmb_    );
  config_ok &= compareValues("Allow readout TMB trig pulse outside L1a window",read_l1a_allow_nol1a_    ,l1a_allow_nol1a_    );
  config_ok &= compareValues("Allow ALCT-only events to readout at L1a"       ,read_l1a_allow_alct_only_,l1a_allow_alct_only_);
  config_ok &= compareValues("Clear scintillator veto"                        ,read_scint_veto_clr_     ,scint_veto_clr_     );
  //
  //------------------------------------------------------------------
  //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
  //------------------------------------------------------------------
  config_ok &= compareValues("ALCT*CLCT trigger ALCT (vpf) delay" ,read_alct_vpf_delay_        ,alct_vpf_delay_        );
  config_ok &= compareValues("ALCT*CLCT trigger match window size",read_alct_match_window_size_,alct_match_window_size_);
  config_ok &= compareValues("MPC tx delay"                       ,read_mpc_tx_delay_          ,mpc_tx_delay_          );
  //
  //------------------------------------------------------------------
  //0XB6 = ADR_RPC_CFG:  RPC Configuration
  //------------------------------------------------------------------
  config_ok &= compareValues("RPC exists"                              ,read_rpc_exists_     ,rpc_exists_     );
  config_ok &= compareValues("Include existing RPCs in DMB readout"    ,read_rpc_read_enable_,rpc_read_enable_);
  config_ok &= compareValues("RPC Bunch Crossing offset"               ,read_rpc_bxn_offset_ ,rpc_bxn_offset_ );
  config_ok &= compareValues("RPC Bank address (for reading sync mode)",read_rpc_bank_       ,rpc_bank_       );
  //
  //------------------------------------------------------------------
  //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
  //------------------------------------------------------------------
  config_ok &= compareValues("RPC0 raw hits delay",read_rpc0_raw_delay_,rpc0_raw_delay_);
  config_ok &= compareValues("RPC1 raw hits delay",read_rpc1_raw_delay_,rpc1_raw_delay_);
  //
  //------------------------------------------------------------------
  //0XBC = ADR_RPC_INJ:  RPC Injector Control
  //------------------------------------------------------------------
  config_ok &= compareValues("Enable RPC inputs to RAT"            ,read_rpc_mask_all_ ,rpc_mask_all_ );
  config_ok &= compareValues("Enable RAT for injector fire"        ,read_inj_mask_rat_ ,inj_mask_rat_ ); 
  config_ok &= compareValues("Enable RPC inj RAM for injector fire",read_inj_mask_rpc_ ,inj_mask_rpc_ ); 
  config_ok &= compareValues("CFEB/RPC injectors wait for RAT"     ,read_inj_delay_rat_,inj_delay_rat_); 
  config_ok &= compareValues("Enable injector RAM write"           ,read_rpc_inj_sel_  ,rpc_inj_sel_  ); 
  //
  //------------------------------------------------------------------
  //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
  //------------------------------------------------------------------
  config_ok &= compareValues("RAT-RPC0 phase",read_rpc0_rat_delay_,rpc0_rat_delay_);
  config_ok &= compareValues("RAT-RPC1 phase",read_rpc1_rat_delay_,rpc1_rat_delay_);
  //
  //---------------------------------------------------------------------
  //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
  //---------------------------------------------------------------------
  config_ok &= compareValues("Enable layer trigger mode",read_layer_trigger_en_ ,layer_trigger_en_ ); 
  config_ok &= compareValues("Layer trigger threshold"  ,read_layer_trig_thresh_,layer_trig_thresh_); 
  //
  //---------------------------------------------------------------------
  //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
  //---------------------------------------------------------------------
  config_ok &= compareValues("CLCT blanking mode"               ,read_clct_blanking_         ,clct_blanking_         );
  config_ok &= compareValues("CLCT stagger mode"                ,read_clct_stagger_          ,clct_stagger_          );
  config_ok &= compareValues("CLCT pattern ID threshold"        ,read_clct_pattern_id_thresh_,clct_pattern_id_thresh_);
  config_ok &= compareValues("Active FEB Flag pattern threshold",read_aff_thresh_            ,aff_thresh_            );
  //
  //---------------------------------------------------------------------
  //0XF6 = ADR_TEMP1:  CLCT separation
  //---------------------------------------------------------------------
  config_ok &= compareValues("CLCT separation source is VME",read_clct_separation_src_,clct_separation_src_);
  config_ok &= compareValues("Minimum CLCT separation"      ,read_min_clct_separation_,min_clct_separation_);
  //
  //
  ReportCheck("TMB configuration check",config_ok);
  //
  tmb_configuration_status_ = (int) config_ok;
  //
  return;
}
//
void TMB::CheckRawHitsHeader() {
  //
  bool config_ok = true;
  //
  ReadRawHitsHeader();
  //
  config_ok &= compareValues("TMB Raw hits header:board status OK"                ,read_bdstatus_ok_    ,bdstatus_ok_expected    );
  config_ok &= compareValues("TMB Raw hits header:5.0V critical OK"               ,read_vstat_5p0v_     ,vstat_5p0v_expected     );
  config_ok &= compareValues("TMB Raw hits header:3.3V critical OK"               ,read_vstat_3p3v_     ,vstat_3p3v_expected     );
  config_ok &= compareValues("TMB Raw hits header:1.8V critical OK"               ,read_vstat_1p8v_     ,vstat_1p8v_expected     );
  config_ok &= compareValues("TMB Raw hits header:1.5V critical OK"               ,read_vstat_1p5v_     ,vstat_1p5v_expected     );
  config_ok &= compareValues("TMB Raw hits header:Temperature not critical"       ,read_nt_crit_        ,nt_crit_expected        );
  config_ok &= compareValues("TMB Raw hits header:VME state machine OK"           ,read_vsm_ok_         ,vsm_ok_expected         );
  config_ok &= compareValues("TMB Raw hits header:VME state machine aborted"      ,read_vsm_aborted_    ,vsm_aborted_expected    );
  config_ok &= compareValues("TMB Raw hits header:VME state machine checksum OK"  ,read_vsm_cksum_ok_   ,vsm_cksum_ok_expected   );
  config_ok &= compareValues("TMB Raw hits header:VME state machine wordcount OK" ,read_vsm_wdcnt_ok_   ,vsm_wdcnt_ok_expected   );
  config_ok &= compareValues("TMB Raw hits header:JTAG state machine OK"          ,read_jsm_ok_         ,jsm_ok_expected         );
  config_ok &= compareValues("TMB Raw hits header:JTAG state machine aborted"     ,read_jsm_aborted_    ,jsm_aborted_expected    );
  config_ok &= compareValues("TMB Raw hits header:JTAG state machine checksum OK" ,read_jsm_cksum_ok_   ,jsm_cksum_ok_expected   );
  config_ok &= compareValues("TMB Raw hits header:JTAG state machine wordcount OK",read_jsm_wdcnt_ok_   ,jsm_wdcnt_ok_expected   );
  config_ok &= compareValues("TMB Raw hits header:JTAG state machine FPGA TCK OK" ,read_jsm_tck_fpga_ok_,jsm_tck_fpga_ok_expected);
  //
  ReportCheck("TMB Raw hits header",config_ok);
  //
  raw_hits_header_status_ = (int) config_ok;
  //
  return;
}
//
void TMB::CheckVMEStateMachine() {
  //
  bool config_ok = true;
  //
  ReadVMEStateMachine();
  //
  config_ok &= compareValues("VME state machine autostart"              ,read_vme_state_machine_autostart_,vme_state_machine_autostart_expected);
  config_ok &= compareValues("VME state machine aborted"                ,read_vme_state_machine_aborted_  ,vme_state_machine_aborted_expected);
  config_ok &= compareValues("VME state machine check sum OK"           ,read_vme_state_machine_cksum_ok_ ,vme_state_machine_cksum_ok_expected);
  config_ok &= compareValues("VME state machine word count OK"          ,read_vme_state_machine_wdcnt_ok_ ,vme_state_machine_wdcnt_ok_expected);
  config_ok &= compareValues("VME state machine VME ready"              ,read_vme_state_machine_vme_ready_,vme_state_machine_vme_ready_expected);
  config_ok &= compareValues("VME state machine OK"                     ,read_vme_state_machine_ok_       ,vme_state_machine_ok_expected);
  config_ok &= compareValues("VME state machine path OK"                ,read_vme_state_machine_path_ok_  ,vme_state_machine_path_ok_expected);
  config_ok &= compareValues("VME state machine missing header start"   ,read_vme_state_machine_error_missing_header_start_   
			     ,vme_state_machine_error_missing_header_start_expected   );
  config_ok &= compareValues("VME state machine missing header end"     ,read_vme_state_machine_error_missing_header_end_     
			     ,vme_state_machine_error_missing_header_end_expected     );
  config_ok &= compareValues("VME state machine missing data end marker",read_vme_state_machine_error_missing_data_end_marker_
			     ,vme_state_machine_error_missing_data_end_marker_expected);
  config_ok &= compareValues("VME state machine missing trailer end"    ,read_vme_state_machine_error_missing_trailer_end_    
			     ,vme_state_machine_error_missing_trailer_end_expected    );
  config_ok &= compareValues("VME state machine word count overflow"    ,read_vme_state_machine_error_word_count_overflow_    
			     ,vme_state_machine_error_word_count_overflow_expected    );
  //
  ReportCheck("VME state machine check",config_ok);
  //
  vme_state_machine_status_ = (int) config_ok;
  //
  return;
}
//
void TMB::CheckJTAGStateMachine() {
  //
  bool config_ok = true;
  //
  ReadJTAGStateMachine();
  //
  config_ok &= compareValues("JTAG state machine autostart"         ,read_jtag_state_machine_autostart_  ,jtag_state_machine_autostart_expected  );
  config_ok &= compareValues("JTAG state machine aborted"           ,read_jtag_state_machine_aborted_    ,jtag_state_machine_aborted_expected    );
  config_ok &= compareValues("JTAG state machine check sum OK"      ,read_jtag_state_machine_cksum_ok_   ,jtag_state_machine_cksum_ok_expected   );
  config_ok &= compareValues("JTAG state machine word count OK"     ,read_jtag_state_machine_wdcnt_ok_   ,jtag_state_machine_wdcnt_ok_expected   );
  config_ok &= compareValues("JTAG state machine tck FPGA OK"       ,read_jtag_state_machine_tck_fpga_ok_,jtag_state_machine_tck_fpga_ok_expected);
  config_ok &= compareValues("JTAG state machine VME ready"         ,read_jtag_state_machine_vme_ready_  ,jtag_state_machine_vme_ready_expected  );
  config_ok &= compareValues("JTAG state machine OK"                ,read_jtag_state_machine_ok_         ,jtag_state_machine_ok_expected         );
  config_ok &= compareValues("Boot Register FPGA control JTAG chain",read_boot_control_jtag_chain_       ,boot_control_jtag_chain_expected       );
  //
  ReportCheck("JTAG state machine check",config_ok);
  //
  jtag_state_machine_status_ = (int) config_ok;
  //
  return;
}
//
void TMB::CheckDDDStateMachine() {
  //
  bool config_ok = true;
  //
  ReadDDDStateMachine();
  //
  config_ok &= compareValues("DDD state machine busy"             ,read_ddd_state_machine_busy_          ,ddd_state_machine_busy_expected        );
  config_ok &= compareValues("DDD state machine data verify OK"   ,read_ddd_state_machine_verify_ok_     ,ddd_state_machine_verify_ok_expected   );
  config_ok &= compareValues("DDD state machine clock 0 DCM lock" ,read_ddd_state_machine_clock0_lock_   ,ddd_state_machine_clock0_lock_expected );
  config_ok &= compareValues("DDD state machine clock 0d DCM lock",read_ddd_state_machine_clock0d_lock_  ,ddd_state_machine_clock0d_lock_expected);
  config_ok &= compareValues("DDD state machine clock 1 DCM lock" ,read_ddd_state_machine_clock1_lock_   ,ddd_state_machine_clock1_lock_expected );
  config_ok &= compareValues("DDD state machine clock ALCT lock"  ,read_ddd_state_machine_clock_alct_lock_
			     ,ddd_state_machine_clock_alct_lock_expected );
  config_ok &= compareValues("DDD state machine clock ALCTd lock" ,read_ddd_state_machine_clockd_alct_lock_
			     ,ddd_state_machine_clockd_alct_lock_expected);
  config_ok &= compareValues("DDD state machine clock MPC lock"   ,read_ddd_state_machine_clock_mpc_lock_
			     ,ddd_state_machine_clock_mpc_lock_expected  );
  config_ok &= compareValues("DDD state machine clock DCC lock"   ,read_ddd_state_machine_clock_dcc_lock_
			     ,ddd_state_machine_clock_dcc_lock_expected  );
  //
  // expected value of RPC lock check depends on if RPC exists...
  //
  config_ok &= compareValues("DDD state machine clock RPC lock"   ,read_ddd_state_machine_clock_rpc_lock_,(rpc_exists_ & 0x1)                    );
  //
  ReportCheck("TMB DDD state machine check",config_ok);
  //
  ddd_state_machine_status_ = (int) config_ok;
  //
  return;
}
//
////////////////////////////////////////////////////////////////////////////////////////
// integer parsing routines 
////////////////////////////////////////////////////////////////////////////////////////
void TMB::InsertValueIntoDataWord(int value, int hibit, int lobit, int * data_word) {
  //
  // insert "value" into the value of the variable pointed at by "data_word" from "lobit" (LSB) to "hibit" (MSB)
  //
  const int NUMBER_OF_BITS = 16;
  int data_vector[NUMBER_OF_BITS] = {};
  //
  //  (*MyOutput_) << "Put " << value << " into " << (*data_word) 
  //	       << " from bits " << lobit << " to " << hibit << std::endl;
  //
  // fill vector of bits data_vector[] with what is in the address pointed at by data_word:
  int_to_bits( (*data_word),
	       NUMBER_OF_BITS,
	       data_vector,
	       LSBfirst );
  //
  //  (*MyOutput_) << "Before adding " << value << "..." << std::endl;
  //  for (int i=0; i<NUMBER_OF_BITS; i++) 
  //    (*MyOutput_) << "data_vector[" << i << "] =" << data_vector[i] << std::endl;
  //
  // insert "value" into "data_vector" beginning at index "lobit" and finishing at vector "hibit"
  int size_of_value = hibit - lobit + 1;
  int * pointer_to_place_in_data_vector = data_vector + lobit;
  int_to_bits(value,
	      size_of_value,
	      pointer_to_place_in_data_vector,
	      LSBfirst);
  //
  //  (*MyOutput_) << "After adding " << value << "..." << std::endl;
  //  for (int i=0; i<NUMBER_OF_BITS; i++) 
  //    (*MyOutput_) << "data_vector[" << i << "] =" << data_vector[i] << std::endl;
  //
  // pack vector of bits into integer to be written to VME register:
  (*data_word) = bits_to_int(data_vector,
			     NUMBER_OF_BITS,
			     LSBfirst);
  //
  //  (*MyOutput_) << "... gives " << std::hex << (*data_word) << std::endl;
  //
  return;
}
//
int TMB::ExtractValueFromData(int data, int bitlo, int bithi) {
  //
  // return the value located in "data" which resides from "bitlo" to "bithi"
  //
  int value = 0;
  //
  value = (data >> bitlo) & makemask(bitlo,bithi);
  //
  return value;
}
//
int TMB::makemask(int bitlo, int bithi) {
  //
  // return a mask of the right size for the specified number of bits
  //
  int mask = 0;
  for (int bit=bitlo; bit<=bithi; bit++) {
    mask <<= 1;
    mask |= 1;
  }
  return mask;
}
//
/////////////////////////////////////////////////////////////////////
// register comparison methods
/////////////////////////////////////////////////////////////////////
bool TMB::compareValues(std::string TypeOfTest, 
			int testval, 
			int compareval) {
  //
  //Default is that you want "testval" to equal "compareval"...
  //
  return compareValues(TypeOfTest,testval,compareval,true);
  //
}
//
bool TMB::compareValues(std::string TypeOfTest, 
			int testval, 
			int compareval,
			bool equal) {
  //
  // test if "testval" is equivalent to the expected value: "compareval"
  // return depends on if you wanted them to be "equal"
  //
  //(*MyOutput_) << "compareValues:  " << TypeOfTest << " -> ";
  //
  if (equal) {
    if (testval == compareval) {
      //      (*MyOutput_) << "PASS = 0x" << std::hex << compareval << std::endl;
      return true;
    } else {
      std::ostringstream dump;
      dump << "compareValues:  FAIL! ";
      dump << TypeOfTest 
	   << " -> expected value = 0x" << std::hex << compareval
	   << ", returned value = 0x" << std:: hex << testval
	   << std::endl;
      //
      (*MyOutput_) << "compareValues:  FAIL! ";
      (*MyOutput_) << TypeOfTest 
		   << " -> expected value = 0x" << std::hex << compareval
		   << ", returned value = 0x" << std:: hex << testval
		   << std::endl;
      //
      SendOutput(dump.str(),"ERROR");
      return false;
    }
  } else {
    if (testval != compareval) {
      //      (*MyOutput_) << "PASS -> 0x" << std::hex << testval 
      //                   << " not equal to 0x" <<std::hex << compareval 
      //		   << std::endl;
      return true;
    } else {
      std::ostringstream dump;
      dump << "compareValues:  FAIL! ";
      dump << TypeOfTest 
	   << " expected = returned = 0x" << std::hex << testval
	   << std::endl;
      //
      (*MyOutput_) << "compareValues:  FAIL! ";
      (*MyOutput_) << TypeOfTest 
		   << " expected = returned = 0x" << std::hex << testval
		   << std::endl;
      //
      SendOutput(dump.str(),"ERROR");
      return false;
    }
  }
}
//
bool TMB::compareValues(std::string TypeOfTest, 
			float testval, 
			float compareval,
			float tolerance) {
  //
  // test if "testval" is within "tolerance" of "compareval"...
  //
  //  (*MyOutput_) << "compareValues tolerance:  " << TypeOfTest << " -> ";
  //
  float err = (testval - compareval)/compareval;
  //
  float fractolerance = tolerance*compareval;
  //
  if (fabs(err)>tolerance) {
      std::ostringstream dump;
      dump << "compareValues:  FAIL! ";
      dump << TypeOfTest 
	   << " expected = " << compareval 
	   << ", returned = " << testval
	   << " outside of tolerance "<< fractolerance
	   << std::endl;
      //
      (*MyOutput_) << "compareValues:  FAIL! ";
      (*MyOutput_) << TypeOfTest 
		   << " expected = " << compareval 
		   << ", returned = " << testval
		   << " outside of tolerance "<< fractolerance
		   << std::endl;
      //
      SendOutput(dump.str(),"ERROR");
      return false;
  } else {
    //      (*MyOutput_) << "PASS!" << std::endl;
    //      (*MyOutput_) << TypeOfTest 
    //		<< " value = " << testval
    //		<< " within "<< fractolerance
    //		<< " of " << compareval
    //		<< std::endl;
      return true;
  }
  //
}
//
void TMB::ReportCheck(std::string check_type, bool status_bit) {
  //
  std::ostringstream dump;
  dump << "slot " << (int) slot() << ": " << check_type << " -> ";
  (*MyOutput_) << "slot " << (int) slot() << ": " << check_type << " -> ";
  //
  if ( status_bit ) {
    dump << "OK" << std::endl;
    (*MyOutput_) << "OK" << std::endl;
    SendOutput(dump.str(),"INFO");
  } else {
    dump << "FAIL <-" << std::endl;
    (*MyOutput_) << "FAIL <-" << std::endl;
    SendOutput(dump.str(),"ERROR");
  }
  //
  return;
}
//
/////////////////////////////////////////////////////////////////////
// hot channel masks
/////////////////////////////////////////////////////////////////////
int TMB::GetHotChannelLayerFromMap_(unsigned long int vme_address, int bit_in_register) {
  //
  if (vme_address == hcm001_adr) {
    return cfeb0_layer01_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm023_adr) {
    return cfeb0_layer23_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm045_adr) {
    return cfeb0_layer45_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm101_adr) {
    return cfeb1_layer01_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm123_adr) {
    return cfeb1_layer23_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm145_adr) {
    return cfeb1_layer45_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm201_adr) {
    return cfeb2_layer01_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm223_adr) {
    return cfeb2_layer23_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm245_adr) {
    return cfeb2_layer45_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm301_adr) {
    return cfeb3_layer01_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm323_adr) {
    return cfeb3_layer23_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm345_adr) {
    return cfeb3_layer45_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm401_adr) {
    return cfeb4_layer01_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm423_adr) {
    return cfeb4_layer23_hotchannelmask_layer_map[bit_in_register];
    //
  } else if (vme_address == hcm445_adr) {
    return cfeb4_layer45_hotchannelmask_layer_map[bit_in_register];
    //
  } 
  return -999;
}
//
int TMB::GetHotChannelDistripFromMap_(unsigned long int vme_address, int bit_in_register) {
  //
  if (vme_address == hcm001_adr) {
    return cfeb0_layer01_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm023_adr) {
    return cfeb0_layer23_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm045_adr) {
    return cfeb0_layer45_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm101_adr) {
    return cfeb1_layer01_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm123_adr) {
    return cfeb1_layer23_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm145_adr) {
    return cfeb1_layer45_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm201_adr) {
    return cfeb2_layer01_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm223_adr) {
    return cfeb2_layer23_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm245_adr) {
    return cfeb2_layer45_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm301_adr) {
    return cfeb3_layer01_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm323_adr) {
    return cfeb3_layer23_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm345_adr) {
    return cfeb3_layer45_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm401_adr) {
    return cfeb4_layer01_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm423_adr) {
    return cfeb4_layer23_hotchannelmask_distrip_map[bit_in_register];
    //
  } else if (vme_address == hcm445_adr) {
    return cfeb4_layer45_hotchannelmask_distrip_map[bit_in_register];
    //
  } 
  return -999;
}
//
