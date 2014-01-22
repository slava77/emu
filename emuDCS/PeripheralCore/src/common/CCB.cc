//-----------------------------------------------------------------------
// $Id: CCB.cc,v 3.53 2012/09/30 21:19:42 liu Exp $
// $Log: CCB.cc,v $
// Revision 3.53  2012/09/30 21:19:42  liu
// update for ME11 new electronics
//
// Revision 3.52  2012/09/26 22:14:19  liu
// add new methods and change CCB configure
//
// Revision 3.51  2012/09/13 03:47:59  liu
// update CCB configure()
//
// Revision 3.50  2012/07/12 13:18:52  ahart
//
// Modified to accomodate DCFEB and ODAQMB.
//
// Revision 3.49  2012/07/10 15:25:25  liu
// bug fix in SetExtTrigDelay for STEP
//
// Revision 3.48  2012/07/06 00:49:35  liu
// add functions used by STEP
//
// Revision 3.47  2012/05/09 20:59:00  liu
// fix missing standard header files
//
// Revision 3.46  2012/01/27 21:32:16  liu
// clean Get/SetCCBmode functions
//
// Revision 3.45  2011/11/28 23:44:00  liu
// more log messages
//
// Revision 3.44  2011/11/03 20:52:38  liu
// new ReadConfigBits function
//
// Revision 3.43  2011/10/20 19:24:40  liu
// add signal_csrb2 function and remove obsolete functions
//
// Revision 3.42  2011/07/01 01:42:13  liu
// update
//
// Revision 3.41  2010/08/27 19:51:29  liu
// send PrintTTCRegister output to screen
//
// Revision 3.40  2010/07/28 13:13:49  rakness
// make fine delay computation public
//
// Revision 3.39  2010/02/09 12:12:19  liu
// introduce global run safe configuration check
//
// Revision 3.38  2009/11/17 13:28:46  liu
// fix a bug in TTCrx fine delay with 0 nano sec
//
// Revision 3.37  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 3.36  2008/08/18 12:15:13  rakness
// add FPGA configuration done reads and accessors
//
// Revision 3.35  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.34  2008/08/08 17:56:54  rakness
// fix just introduced bug for checking DLOG mode
//
// Revision 3.33  2008/08/08 11:01:24  rakness
// centralize logging
//
// Revision 3.32  2008/08/06 08:34:33  rakness
// cleanup formatting of check Configuration output
//
// Revision 3.31  2008/08/05 12:29:20  rakness
// make TTCrxFineDelay xml parameter = number of nsec
//
// Revision 3.30  2008/06/12 21:08:55  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.29  2008/05/30 14:54:06  liu
// Change setCCBMode
//
// Revision 3.28  2008/05/30 11:53:19  liu
// update hardreset and I2C
//
// Revision 3.27  2008/05/15 09:55:10  liu
// error messages for Check_Config
//
// Revision 3.26  2008/04/25 10:48:54  liu
// bug fix in CheckConfig
//
// Revision 3.25  2008/04/18 12:13:46  geurts
// fix CCB mode after checkconfig
//
// Revision 3.24  2008/04/11 14:48:48  liu
// add CheckConfig() function
//
// Revision 3.23  2008/03/31 14:41:01  liu
// remove soft_reset from hard_reset sequence
//
// Revision 3.22  2008/03/07 10:26:25  liu
// changed some comments
//
// Revision 3.21  2008/02/29 10:37:24  liu
// comment out read TTCrx registers in configure
//
// Revision 3.20  2008/02/22 13:25:15  liu
// update
//
// Revision 3.19  2008/02/21 09:37:59  liu
// update TTC reset sequence
//
// Revision 3.18  2008/02/18 12:09:19  liu
// new functions for monitoring
//
// Revision 3.17  2008/02/03 12:26:56  liu
// change hard_reset sequence
//
// Revision 3.16  2007/12/27 00:33:52  liu
// update
//
// Revision 3.15  2007/08/27 22:50:57  liu
// update
//
// Revision 3.14  2007/04/02 17:31:39  liu
// fix setCCBMode problem
//
// Revision 3.13  2007/03/28 16:01:38  rakness
// remove l1areset from TMB-MPC test
//
// Revision 3.12  2007/03/26 14:41:39  rakness
// delay after l1aReset to fix TMB-MPC crate timing test
//
// Revision 3.11  2007/03/26 08:15:25  rakness
// Read TTCrx ID in configure
//
// Revision 3.10  2007/03/16 18:06:22  rakness
// put scans+TTCrxID in summary file
//
// Revision 3.9  2007/03/14 11:59:45  rakness
// extract TTCrxID
//
// Revision 3.8  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.7  2006/09/27 16:44:27  mey
// UPdate
//
// Revision 3.6  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.5  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.4  2006/08/07 13:31:29  mey
// Added TTCrx coarse delay
//
// Revision 3.3  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.2  2006/08/03 18:59:13  mey
// Got rid of version number
//
// Revision 3.1  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.51  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.50  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.49  2006/07/19 09:37:59  mey
// Update
//
// Revision 2.48  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.47  2006/07/17 02:34:00  liu
// update
//
// Revision 2.46  2006/07/16 04:15:20  liu
// remove CCB2001 support, code cleanup
//
// Revision 2.45  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.44  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.43  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.42  2006/07/04 09:38:14  mey
// Update
//
// Revision 2.41  2006/06/12 12:47:17  mey
// Update
//
// Revision 2.40  2006/06/09 11:38:32  mey
// Update
//
// Revision 2.39  2006/06/02 07:48:12  mey
// Disable TTCrx hardreset
//
// Revision 2.38  2006/05/23 09:01:21  rakness
// Update
//
// Revision 2.37  2006/05/22 16:20:38  mey
// UPdate
//
// Revision 2.36  2006/05/22 14:07:39  rakness
// Update
//
// Revision 2.35  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.34  2006/05/10 10:24:32  mey
// Update
//
// Revision 2.33  2006/04/28 13:41:17  mey
// Update
//
// Revision 2.32  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.31  2006/04/24 14:57:21  mey
// Update
//
// Revision 2.30  2006/04/18 08:17:29  mey
// UPdate
//
// Revision 2.29  2006/03/10 10:13:54  mey
// Got rid of DCS
//
// Revision 2.28  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.27  2006/02/21 12:44:00  mey
// fixed bug
//
// Revision 2.26  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.25  2006/02/13 19:51:12  mey
// Fix bugs
//
// Revision 2.24  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 2.23  2006/02/02 15:34:23  mey
// Update
//
// Revision 2.22  2006/02/02 14:27:32  mey
// Update
//
// Revision 2.21  2006/01/12 23:44:21  mey
// Update
//
// Revision 2.20  2006/01/12 22:36:23  mey
// UPdate
//
// Revision 2.19  2006/01/12 12:28:24  mey
// UPdate
//
// Revision 2.18  2006/01/12 11:32:26  mey
// Update
//
// Revision 2.17  2006/01/11 13:47:27  mey
// Update
//
// Revision 2.16  2006/01/10 19:46:48  mey
// UPdate
//
// Revision 2.15  2006/01/09 07:17:30  mey
// Update
//
// Revision 2.14  2005/12/20 14:19:58  mey
// Update
//
// Revision 2.13  2005/12/15 14:22:56  mey
// Update
//
// Revision 2.12  2005/11/25 23:42:26  mey
// Update
//
// Revision 2.11  2005/11/21 18:02:50  mey
// Update
//
// Revision 2.10  2005/11/21 17:51:23  mey
// Update
//
// Revision 2.9  2005/11/15 15:37:13  mey
// Update
//
// Revision 2.8  2005/11/07 16:23:39  mey
// Update
//
// Revision 2.7  2005/11/04 10:26:24  mey
// Update
//
// Revision 2.6  2005/11/03 18:24:53  mey
// I2C routines
//
// Revision 2.5  2005/09/28 16:59:57  mey
// Update
//
// Revision 2.4  2005/08/22 07:55:45  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.3  2005/08/11 08:12:42  mey
// Update
//
// Revision 2.2  2005/07/08 10:29:34  geurts
// introduce debug switch to hide debugging messages
//
// Revision 2.1  2005/06/06 11:10:53  geurts
// default power-up mode DLOG. updated for calibration code.
// direct read/write access to registers
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "emu/pc/CCB.h"
#include "emu/pc/VMEController.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
#include <math.h>
#include "emu/pc/Crate.h"
#include <stdlib.h> // for rand and srand
//
//

namespace emu {
  namespace pc {


CCB::CCB(Crate * theCrate ,int slot)
: VMEModule(theCrate, slot), 
  EmuLogger(),
  TTC(NO_TTC),
  CLK_INIT_FLAG(0),
  mCCBMode(CCB::DLOG),
  xmlCCBMode(-1),
  BX_Orbit_(924),
  SPS25ns_(0),
  l1aDelay_(0),
  TTCrxID_(-1),
  TTCrxCoarseDelay_(0),
  TTCrxFineDelay_(0),
  l1enabled_(false),
  mDebug(false)
{
  MyOutput_ = &std::cout ;
  //
  hardware_version_=0;
  ReadTTCrxID_ = -1;
  read_firmware_day_   = 9999;
  read_firmware_month_ = 9999;
  read_firmware_year_  = 9999;
  //
  expected_firmware_day_   = 999;
  expected_firmware_month_ = 999;
  expected_firmware_year_  = 999;
  //
  for (int i=0; i<9; i++) {
    read_tmb_cfg_done_[i] = 999;
    read_alct_cfg_done_[i]= 999;
    read_dmb_cfg_done_[i] = 999;
  }
  //
  read_mpc_cfg_done_   = 999;  
  read_ccb_fpga_ready_ = 999;
  read_ttcrx_ready_    = 999;
  read_qpll_lock_      = 999;
  read_eprom_config_ok_= 999;
  //
  (*MyOutput_) << "CCB: in crate=" << this->crate()
	    << " slot=" << this->slot() << std::endl;
 };

CCB::~CCB() {
  (*MyOutput_) << "CCB: removing CCB from crate=" << this->crate() 
       << ", slot=" << this->slot() << std::endl;
}

void CCB::end() {
}

std::ostream & operator<<(std::ostream & os, CCB & ccb) {
  os << std::dec << "l1enabled_ " << ccb.l1enabled_ << std::endl
     << "TTC " << ccb.TTC << std::endl
     << "CLK_INIT_FLAG " << ccb.CLK_INIT_FLAG << std::endl
     << "BX_Orbit_ " << ccb.BX_Orbit_ << std::endl
     << "SPS25ns_ " << ccb.SPS25ns_ << std::endl
     << "TTCrxID_ " << ccb.TTCrxID_ << std::endl
     << "l1aDelay_ " << ccb.l1aDelay_ << std::endl
     << "mDebug " << ccb.mDebug << std::endl
     << "mCCBMode " << ccb.mCCBMode << std::endl
     << "TTCrxCoarseDelay_ " << ccb.TTCrxCoarseDelay_ << std::endl
     << "TTCrxFineDelay_ " << ccb.TTCrxFineDelay_ << std::endl;
  return os;
    }

void CCB::pulse(int Num_pulse,unsigned int pulse_delay, char vme)
{
  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    if (mDebug) (*MyOutput_) << "CCB: NOTE -- switching from DLOG to FPGA mode for pulse" << std::endl; 
  }
  for(int j=0;j<Num_pulse;j++){
    (*MyOutput_) << "Pulsing..."<<std::endl;
    if(pulse_delay>0) theController->sleep_vme(pulse_delay);
    //
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    if(j<(Num_pulse-1)){
      do_vme(VME_WRITE,vme,sndbuf,rcvbuf,LATER);
    }
    else{
      do_vme(VME_WRITE,vme,sndbuf,rcvbuf,NOW);
    }
  }

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    if (mDebug) (*MyOutput_) << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };
}


void CCB::pulse(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x48)
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL0);
}


void CCB::inject(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x4a);
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL1);
}

void CCB::pedestal(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x48;)
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL2);
}

void CCB::HardReset_crate()
{
  /// Performs a Hard-Reset to all modules in the crate
  /// through a dedicated VME register (not FastControl)
  //
  (*MyOutput_) << "CCB: hard reset" << std::endl;
  //  setCCBMode(CCB::VMEFPGA);
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  do_vme(VME_WRITE,CRATE_HARD_RESET,sndbuf,rcvbuf,NOW);
  // ::sleep(2);
  // setCCBMode(CCB::DLOG);

}
//
void CCB::SoftReset_crate()
{
  /// Reinitializes the FPGAs on DMB, TMB and MPC boards
  /// through a dedicated VME register (not FastControl)
  //
  (*MyOutput_) << "CCB: soft reset" << std::endl;
  // setCCBMode(CCB::VMEFPGA);
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  do_vme(VME_WRITE,CRATE_SOFT_RESET,sndbuf,rcvbuf,NOW);
  //::sleep(1);
  // setCCBMode(CCB::DLOG);
  
}
//
void CCB::EnableL1aCounter()
{
  //
  setCCBMode(CCB::VMEFPGA);
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,enableL1aCounter,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::ResetL1aCounter()
{
  //
  setCCBMode(CCB::VMEFPGA);
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,resetL1aCounter,sndbuf,rcvbuf,NOW);
  //
}
//
int CCB::ReadL1aCounter()
{
  //
  int val=0;
  //
  setCCBMode(CCB::VMEFPGA);
  //
  do_vme(VME_READ,readL1aCounterLSB,sndbuf,rcvbuf,NOW);
  //
  val = ((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff);
  //
  do_vme(VME_READ,readL1aCounterMSB,sndbuf,rcvbuf,NOW);
  //
  val |= ((rcvbuf[0]&0xff)<<24) | ((rcvbuf[1]&0xff)<<16);
  //
  return val;
  //
}
//
void CCB::GenerateAlctAdbASync(){
  //
  (*MyOutput_) << "CCB: GenerateAlctAdbPulseASync" << std::endl;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE, 0x84, sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::GenerateAlctAdbSync(){
   //
   (*MyOutput_) << "CCB: GenerateAlctAdbPulseSync" << std::endl;
   //
   sndbuf[0]=0x00;
   sndbuf[1]=0x00;
   do_vme(VME_WRITE, 0x82, sndbuf,rcvbuf,NOW);
   //
}
//
void CCB::DumpAddress(int address) {
  //
  do_vme(VME_READ,address,sndbuf,rcvbuf,NOW);  
  //
  printf("CCB.Dump %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
}

int CCB::ReadTTCrxReg(int registerAdd)
{
  int pointerRegAddress = (ReadTTCrxID_*2);
  int DataRegAddress = (ReadTTCrxID_*2+1);
  int i;
  // start I2C
  //
  startI2C();
  //
  ReadRegister(CSRA1);
  //
  // write address of the pointer  register
  for( i=6; i>=0; --i) {
    writeI2C(pointerRegAddress>>i);
  }
  //
  writeI2C(0); // write bit 0 (write) = 0
  //
  readI2C();
  //
  WriteRegister(CSRA1,0x04);    
  //
  WriteRegister(CSRA1,0x02);    
  //
  for( i=7; i>=0; --i) {
    writeI2C(registerAdd>>i);
  }
  //
  readI2C() ;
  //
  WriteRegister(CSRA1,0x06);    
  //
  stopI2C();
  //
  // start condition
  startI2C();
  //
  //
  // write address of the data  register (pointer + 1)
  for( i=6; i>=0; --i) {
    writeI2C(DataRegAddress>>i);
  }
  //
  writeI2C(1); // write bit 0 (read) = 1
  //
  readI2C();
  //
  WriteRegister(CSRA1,0x04);    
  //
  WriteRegister(CSRA1,0x02);    
  //
  // read data
  int r;
  int data=0;
  for(i=7; i>=0; --i) {
    data <<= 1;
    r = readI2C();    
    data += (r&0x1);
  }      
  //
  readI2C() ;
  //
  WriteRegister(CSRA1,0x04);    
  //
  WriteRegister(CSRA1,0x04);    
  //  
  // stop condition
  stopI2C();
  //
  return data;
  //
}

void CCB::WriteTTCrxReg(int registerAdd,int value)
{
  int pointerRegAddress = (ReadTTCrxID_*2);
  int DataRegAddress = (ReadTTCrxID_*2+1);
  int i;
  //
  // start I2C
  //
  startI2C();
  //
  ReadRegister(CSRA1);    
  //
  // write address of the pointer  register
  for( i=6; i>=0; --i) {
    writeI2C(pointerRegAddress>>i);
  }
  //
  writeI2C(0); // write bit 0 (write) = 0
  //
  readI2C();
  //
  WriteRegister(CSRA1,0x04);    
  //
  WriteRegister(CSRA1,0x02);    
  //
  for( i=7; i>=0; --i) {
    writeI2C(registerAdd>>i);
  }
  //
  readI2C() ;
  //
  WriteRegister(CSRA1,0x06);    
  //
  stopI2C();
  //
  // start condition
  startI2C();
  //
  //
  // write address of the data  register (pointer + 1)
  for( i=6; i>=0; --i) {
    writeI2C(DataRegAddress>>i);
  }
  //
  writeI2C(0); // write bit 0 (write) = 1
  //
  readI2C();
  //
  WriteRegister(CSRA1,0x04);    
  //
  WriteRegister(CSRA1,0x02);    
  //
  // Write data
  for(i=7; i>=0; --i) {
    writeI2C(value>>i);
  }      
  //
  readI2C() ;
  //
  WriteRegister(CSRA1,0x06);    
  //
  // stop condition
  stopI2C();
  //
  return;
  //
}

void CCB::HardResetTTCrx(){
  //
  // Hard reset TTCrx....
  setCCBMode(CCB::VMEFPGA);
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,TTCrxReset,sndbuf,rcvbuf,NOW);
  //
  ::sleep(1);
  //
  do_vme(VME_READ,CSRB18,sndbuf,rcvbuf,NOW);
  //
  printf("hardResetTTCrx. %x %x \n",rcvbuf[0],rcvbuf[1]);
  //
  if (rcvbuf[0]==0 and rcvbuf[1]==0 ) std::cout << "Failed TTCrxReset" <<std::endl;
  //
}

void CCB::ReadTTCrxID(){
  //
  // Assume Hardreset TTCrx already done
  //
  // Read TTCrx ID number
  do_vme(VME_READ,CSRB18,sndbuf,rcvbuf,NOW);
  //
  //  printf("ReadTTCRxID.%02x%02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
  ReadTTCrxID_ = ( (rcvbuf[0]&0xff) << 8 ) | (rcvbuf[1]&0xff);
  // TTCrxID_ = ReadTTCrxID_;
  //
  (*MyOutput_) << "ReadTTCrx ID = " << std::dec << ReadTTCrxID_ << std::endl;
  //
}
//
int CCB::readI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0c; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //  
  do_vme(VME_READ,CSRA1, sndbuf,rcvbuf,NOW);
  //
  //printf(" %x rcv \n",rcvbuf[1]&0xff);
  //
  return ((rcvbuf[1]>>4)&0x1);
  //
} 
//
void CCB::writeI2C(int data){
  //
  //
  const int i2cBaseData(0x2);
  const int i2cData((data&0x1)<<2);
  const int i2cClkBit(0x1 << 3);
  const int i2cLowInput (i2cBaseData|i2cData);
  const int i2cHighInput(i2cBaseData|i2cData|i2cClkBit);
  //
  sndbuf[0]= 0x00; 
  sndbuf[1]= (i2cBaseData&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cHighInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]= (i2cBaseData&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::start() {
  // send the first signal
  SetupJtag();
  VMEModule::start();
  theController->initDevice(1);
  theController->goToScanLevel();
}
//
void CCB::startI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02 ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::stopI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//

void CCB::WriteRegister(int reg, int value){
  //
  sndbuf[0] = (value>>8)&0xff;
  sndbuf[1] = value&0xff;
  //
  do_vme(VME_WRITE,reg,sndbuf,rcvbuf,NOW);
  //
}

int CCB::ReadRegister(int reg){
  //
  do_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  //printf(" CCB.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}

void CCB::ReadConfigBits()
{
   int readback=ReadRegister(CSRA2) | (ReadRegister(CSRA3)<<16);
  
   read_mpc_cfg_done_= readback & 1;
   readback = readback >>1;
   for (int i=0; i < 9; i++) 
   {
      read_alct_cfg_done_[i] = readback & 1;
      readback = readback >>1;
   }
   for (int i=0; i < 9; i++) 
   {
      read_tmb_cfg_done_[i] = readback & 1;
      readback = readback >>1;
   }
   for (int i=0; i < 9; i++) 
   {
      read_dmb_cfg_done_[i] = readback & 1;
      readback = readback >>1;
   }

   read_ccb_fpga_ready_ = readback & 1;
   read_ttcrx_ready_     = (readback>>1) & 1;
   read_qpll_lock_       = (readback>>2) & 1;
   read_eprom_config_ok_ = (readback>>3) & 1;

   return;
}

void CCB::enableL1() {
  (*MyOutput_) << "CCB: Enable L1A." << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  //sndbuf[0]=(rcvbuf[0]&0xFE);
  sndbuf[0]=(rcvbuf[0]&0xDF);
  //sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF8);
  sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF0);

  if (mDebug) (*MyOutput_) << "CCB: CSRB1=0x" << std::hex 
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2) 
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;

  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = true;

  //enableCLCT();

}


void CCB::disableL1() {
  (*MyOutput_) << "CCB: Disable L1A." << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  sndbuf[0]=rcvbuf[0];
  CCB_CSR1_SAV=rcvbuf[1];
  sndbuf[1]=(rcvbuf[1]|0xf8); // Disable L1ACC from CCB to custom backplane

  //(*MyOutput_) << "disableL1.CCB: CSRB1=0x" << hex 
  //     << setw(2) << int(sndbuf[0]&0xff) << setw(2) <<int(sndbuf[1]&0xff)
  //     << dec << std::endl;
  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = false;
}


void CCB::hardReset() {

  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes and only for the  HardReset_crate function.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    (*MyOutput_) << "CCB: NOTE -- switching from DLOG to FPGA mode for BackPlane HardReset" << std::endl; 
  }
  
  HardReset_crate();
  ::sleep(2);

  syncReset();
  //fg note: *keep* this 1second!
  ::sleep(1);


  // ReadRegister(0x0);

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    (*MyOutput_) << "CCB: NOTE -- switching back to DLOG" << std::endl;
  }  
}


void CCB::syncReset() {
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;

  int i_ccb=0x03;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  
} 


void CCB::bx0() {
  /// send ccb_bx0 on the backplane:
  (*MyOutput_) << "CCB: BX-zero" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
/* JRG, old way, write to directly to VME adr 0x36, but this
      DOES NOT drive the ccb_cmd bus (no cmd_strobe):
  do_vme(VME_WRITE,0x36,sndbuf,rcvbuf,NOW);
*/

// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
  int i_ccb=0x01;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  
}


void CCB::bxr() {
  (*MyOutput_) << "CCB: BX-reset" << std::endl;
  usleep(10000);

// set ccb_cmd bits (CSRB2[7:2]), EvCntRes (CSR2[1]) and BcntRes (CSR2[0]):
  sndbuf[0]=0x00;
  sndbuf[1]=0xC8;
// send CSRB2 setting on backplane:
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
}


void CCB::reset_bxevt() {
  usleep(10000);

//  Send BCR, ECR and BC0  --GUJH   Mar. 20, 2003
  sndbuf[0]=0x00;
  sndbuf[1]=0x03;
// send CSR2 setting on backplane:
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
  
}

bool CCB::SelfTest() {
  //
  return 0;
  //
}

void CCB::init() {
  //
  //
}
void CCB::PrintTTCrxRegs(){
  //
  (*MyOutput_) << "Register 0 " << std::hex;
  (*MyOutput_) << ReadTTCrxReg(0);
  (*MyOutput_) << " (fine delay)" << std::endl;
  //
  (*MyOutput_) << "Register 1 " ;
  (*MyOutput_) << ReadTTCrxReg(1);
  (*MyOutput_) << " (fine delay)" << std::endl;
      //
  (*MyOutput_) << "Register 2 " ;
  (*MyOutput_) << ReadTTCrxReg(2);
  (*MyOutput_) << " (coarse delay)" <<std::endl;
  //
  (*MyOutput_) << "Register 3 " ;
  (*MyOutput_) << ReadTTCrxReg(3);
  (*MyOutput_) << " (control)" << std::endl << std::endl;
  //
  (*MyOutput_) << "Register 8 " ;
  (*MyOutput_) << ReadTTCrxReg(8);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 9 " ;
  (*MyOutput_) << ReadTTCrxReg(9);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 10 " ;
  (*MyOutput_) << ReadTTCrxReg(10);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 11 " ;
  (*MyOutput_) << ReadTTCrxReg(11);
  (*MyOutput_) << std::endl << std::endl;
  //
  (*MyOutput_) << "Register 16 " ;
  (*MyOutput_) << ReadTTCrxReg(16);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 17 " ;
  (*MyOutput_) << ReadTTCrxReg(17);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 18 " ;
  (*MyOutput_) << ReadTTCrxReg(18);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 19 " ;
  (*MyOutput_) << ReadTTCrxReg(19);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 20 " ;
  (*MyOutput_) << ReadTTCrxReg(20);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 21 " ;
  (*MyOutput_) << ReadTTCrxReg(21);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 22 " ;
  (*MyOutput_) << ReadTTCrxReg(22);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 24 " ;
  (*MyOutput_) << ReadTTCrxReg(24);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 25 " ;
  (*MyOutput_) << ReadTTCrxReg(25);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 26 " ;
  (*MyOutput_) << ReadTTCrxReg(26);
  (*MyOutput_) << std::endl;
      //
  (*MyOutput_) << "Register 27 " ;
  (*MyOutput_) << ReadTTCrxReg(27);
  (*MyOutput_) << std::endl;
  //
  (*MyOutput_) << "Register 28 " ;
  (*MyOutput_) << ReadTTCrxReg(28);
  (*MyOutput_) << std::dec << std::endl;

  std::cout << "PrintTTCrxRegs Using ReadTTCrxID_ "<< ReadTTCrxID_ << std::endl;

  // I2C access could leave the CCB in FPGA mode
  setCCBMode(CCB::DLOG);
  //
}

void CCB::configure() {
  //
  SendOutput("CCB : configure()","INFO");
  //
  hard_reset_ccb();
  ::usleep(100000);  // wait 100ms
  // report firmware version
  firmwareVersion();
  printFirmwareVersion();

  setCCBMode(CCB::VMEFPGA);  

  // this line from the old rice_clk_setup(), not sure if it's needed
  SetL1aDelay(l1aDelay_);
  
  //  std::cout << ReadRegister(0x0) << std::endl;
  if(l1enabled_)  
     enableL1();
  else  
     disableL1();
  //  std::cout << ReadRegister(0x0) << std::endl;
  //
  // print out TTCrx registers before reset
  for(int i=0; i<4; i++)
     std::cout << "Register " << i << ":  " << std::hex << ReadTTCrxReg(i) << std::dec << std::endl;
         
  HardResetTTCrx();
  ::usleep(1000);
  // need to read the TTCrxID before TTCrx registers can be touched
  ReadTTCrxID();
  //
  // Download coarse delay to TTCrx
  //
  //PrintTTCrxRegs();
  //std::cout << "write TTCrxCoarseDelay_ = " << TTCrxCoarseDelay_ << " to register 2" << std::endl;
  //
  if(TTCrxCoarseDelay_>0)
  {
     int delay = ((TTCrxCoarseDelay_&0xf)<<4) + (TTCrxCoarseDelay_&0xf);
     WriteTTCrxReg(2,delay);
  }
  //
  // Download fine delay to TTCrx
  //
  if( TTCrxFineDelay_>=0)
  {
     int delay = ConvertNanosecondsToFineDelayUnits(TTCrxFineDelay_&0xff);
     WriteTTCrxReg(0,delay);
     WriteTTCrxReg(1,delay);
  }
  //
  // Enable TTCrx paralle output bus
  //
  WriteTTCrxReg(3,0xB3);  

  // check TTCrx registers
  int rx;
  rx=ReadTTCrxReg(2);
  if(((rx&0xf) != (TTCrxCoarseDelay_&0xf)) || ((rx&0xf0)>>4 != (TTCrxCoarseDelay_&0xf)))  
     std::cout << "ERROR: TTCrx Coarse Delay register readback " << std::hex << (rx&0xff) << std::endl; 

  rx=ReadTTCrxReg(3);
  if((rx&0xff) != 0xB3) 
     std::cout << "ERROR: TTCrx Control register readback " << std::hex << (rx&0xff) << std::endl; 

  // write a special tag to CSRB4
  WriteRegister(CSRB4, 0xCCB0+(TTCrxID_&0xF)); 
  // Set the CCB mode  
  setCCBMode((CCB2004Mode_t)xmlCCBMode);
  //
}

int CCB::CheckConfig(int full_check)
{
  int rx, read_value, expected_value;

  //
  bool config_ok = true;
  //
  std::cout << "CCB: configuration check..." << std::endl;
  // check TTCrx ready and QPLL locked
  ReadConfigBits();
  //
  config_ok &= compareValues("CCB TTCrx",read_ttcrx_ready_,expected_ttcrx_ready_);
  config_ok &= compareValues("CCB QPLL lock",read_qpll_lock_,expected_qpll_lock_);
  //
  // For a safe version of CheckConfig, full_check==0 to avoid reading TTCrx registers 
  // as I2C access would set the CCB in FPGA mode
  //
  if(full_check)
  {
    // check TTCrx Coarse delay
    rx=ReadTTCrxReg(2);
    //
    read_value = (rx&0xf);
    expected_value = (TTCrxCoarseDelay_&0xf);
    config_ok &= compareValues("CCB TTCrxCoarseDelay LSB",read_value,expected_value);
    //
    read_value = (rx&0xf0)>>4;
    config_ok &= compareValues("CCB TTCrxCoarseDelay MSB",read_value,expected_value);
    //
    //  if(((rx&0xf) != (TTCrxCoarseDelay_&0xf)) || ((rx&0xf0)>>4 != (TTCrxCoarseDelay_&0xf)))  
    //  {  std::cout << "CCB_Check_Config: TTCrx Coarse delay inconsistent "
    //               << std::hex << (rx&0xff) << std::dec << std::endl;
    //     return 0;
    //  }
    //
    //
    // check TTCrx Control register
    rx=ReadTTCrxReg(3);
    //
    read_value = (rx&0xff);
    expected_value = 0xB3;
    config_ok &= compareValues("CCB TTCrx Control register",read_value,expected_value);
    //
    //  if((rx&0xff) != 0xB3) 
    //  {  std::cout << "CCB_Check_Config: TTCrx Control register wrong "
    //               << std::hex << (rx&0xff) << std::dec << std::endl;
    //     return 0;
    //  }
    //
    // I2C access could leave the CCB in FPGA mode
    setCCBMode(CCB::DLOG);
    //
  }
  // check CCB in DLOG mode
  rx=ReadRegister(CSRA1);
  std::cout << "CCB CSRA1 value: " << std::hex << (rx & 0xFFFF) << std::dec << std::endl;
  //
  read_value = (rx&0x1);
  expected_value = 1;
  config_ok &= compareValues("CCB DLOG mode",read_value,expected_value);
  //
  //Check component FPGA programming done
  config_ok &= compareValues("CCB FPGA programmed",read_ccb_fpga_ready_,expected_ccb_fpga_ready_);

  // this check is unnecessary, the MPC should be checked in MPC class
  // config_ok &= compareValues("MPC FPGA programmed",read_mpc_cfg_done_  ,expected_mpc_cfg_done_  );

  return (int) config_ok;
}
// 
int CCB::ConvertNanosecondsToFineDelayUnits(int delay_in_nsec) {
  //
  // The Fine Delay register has 240 steps (called "K" counting from 0-239) to 
  // divide up the 24.95nsec-period LHC clock.  Thus, to delay the TTCrxFineDelay 
  // register for a given number of nanoseconds, we use the conversion 
  // 240/24.95 = 9.62 K-steps/nsec.  Then, from Appendix A of the TTCrx manual, 
  // the conversion is given from K to fine-delay register setting:
  //
  const int register_setting[25] = 
    { 14 ,   // K =   0 ->  0 nsec   
      164,   //   =  10 ->  1 nsec
      75 ,   //   =  19 ->  2 nsec
      225,   //   =  29 ->  3 nsec
      136,   //   =  38 ->  4 nsec
      62 ,   //   =  48 ->  5 nsec
      212,   //   =  58 ->  6 nsec
      123,   //   =  67 ->  7 nsec
      33 ,   //   =  77 ->  8 nsec
      199,   //   =  87 ->  9 nsec
      110,   //   =  96 -> 10 nsec
      20 ,   //   = 106 -> 11 nsec
      171,   //   = 115 -> 12 nsec
      81 ,   //   = 125 -> 13 nsec
      7  ,   //   = 135 -> 14 nsec
      158,   //   = 144 -> 15 nsec
      53 ,   //   = 153 -> 16 nsec
      234,   //   = 164 -> 17 nsec
      129,   //   = 173 -> 18 nsec
      55 ,   //   = 183 -> 19 nsec
      206,   //   = 192 -> 20 nsec
      116,   //   = 202 -> 21 nsec
      42 ,   //   = 212 -> 22 nsec
      177,   //   = 221 -> 23 nsec
      103 }; //   = 231 -> 24 nsec
  //
  if (delay_in_nsec >= 0 && delay_in_nsec < 25) {
    //
    return register_setting[delay_in_nsec];
    //
  } else {
    // This is the default setting on power-up.  Note that this does not mean 0 nsec delay...
    return 0;
  }
  //
}
//
void CCB::SetL1aDelay(int l1adelay){
  //
  do_vme(VME_READ,CSRB5,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=l1adelay;  
  do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,NOW);
  //
}

void CCB::setCCBMode(CCB2004Mode_t mode){
  /// Set the mode of operation for the CCB2004 model
  //  Note: Because I2C access can change the real CCB mode, 
  //        the flag mCCBMode isn't already reliable.
  //        So we have to set the required mode each time.
  char tmpb1[2];

  switch (mode) {
  case TTCrqFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0E;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);
    do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
    tmpb1[0]=rcvbuf[0];
    tmpb1[1]=rcvbuf[1];
    if((tmpb1[1]&0x01)==1) {
       sndbuf[0]=tmpb1[0];
       sndbuf[1]=tmpb1[1] & 0xFE;
       do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    }
    std::cout << "CCB mode set to TTCrqFPGA" << std::endl;
    break;
  case VMEFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0E;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,LATER);
    do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
    tmpb1[0]=rcvbuf[0];
    tmpb1[1]=rcvbuf[1];
    if((tmpb1[1]&0x01)==0) {
       sndbuf[0]=tmpb1[0];
       sndbuf[1]=tmpb1[1] | 0x01;
       do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    }
    std::cout << "CCB mode set to VMEFPGA" << std::endl;
    break;
  case DLOG:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0F;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    std::cout << "CCB mode set to DLOG" << std::endl;
    break;
  default:
    std::cerr << "Warning: unkown CCB2004 operation mode. Using DLOG instead" << std::endl;
    sndbuf[0]=0x00;
    sndbuf[1]=0x0F;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    mode=DLOG;
  }
  mCCBMode=mode;
}

 
void CCB::enable() {
  (*MyOutput_) << "Sync reset, mainly for TMB " << std::endl;
  if(l1enabled_) disableL1();
  syncReset();
  usleep(50000);

  (*MyOutput_) << "BXR and EVTCNTRST" << std::endl;
  reset_bxevt();

  if (SPS25ns_==0) {
    startTrigger();
    usleep(10000);
    (*MyOutput_) << "CCB BC0" << std::endl;

    bx0();
    usleep(10000);
    setCCBMode(CCB::DLOG);
  }

    if (TTC==ALL_TTC)
      enableTTCControl();

  enableL1();
}


void CCB::enableTTCControl() {
  (*MyOutput_) << "CCB: Enable TTC control " << std::endl;

  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  usleep(50000);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]&0xfe);

  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
  (*MyOutput_) << "CCB: CSRB1(read)=0x" << std::hex 
	    << std::setw(2) << int(rcvbuf[0]&0xff) << std::setw(2) 
	    <<int(rcvbuf[1]&0xff) << "changed to CSRB1(set)=0x"
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2)
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;
}


void CCB::startTrigger() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "Start Trigger" command on the Fast Control Bus
  (*MyOutput_) << "CCB: Start Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x18;          //cmd[5:0]=0x06 for start_trigger     GUJH
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::injectTMBPattern() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Inject TMB Pattern data
  (*MyOutput_) << "CCB: Start TMB pattern injection to MPC" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=(0x24<<2);          
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::l1aReset(){
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "L1a Reset" command on the Fast Control Bus
  (*MyOutput_) << "CCB: L1a Reset" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=(0x3<<2);          //cmd[5:0]=0x03 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::CLCTexternalTrigger() {
  //
  sndbuf[0]=0x00;
  sndbuf[1]=(0x1A<<2);          //cmd[5:0]=0x03 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::bc0() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "bc0" command on the Fast Control Bus
  (*MyOutput_) << "CCB: bc0 Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x4; 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);

}
//
void CCB::stopTrigger() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "Stop Trigger" command on the Fast Control Bus
  (*MyOutput_) << "CCB: Stop Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x1C;          //cmd[5:0]=0x07 for stop_trigger     GUJH
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::disable() {
  (*MyOutput_) << "CCB: disable" << std::endl;
  disableTTCControl();
  if (SPS25ns_==0) {
    //stop trigger
    stopTrigger();
  }
}
//
void CCB::disableTTCControl() {
  (*MyOutput_) << "CCB: disable TTC control" << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]|0x01);
  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(5000);
}
//
void CCB::executeCommand(std::string command) {
  if(command=="Program Backplane") HardReset_crate();
  if(command=="Reset Backplane")   SoftReset_crate();

  if(command=="Enable L1")        enableL1();
  if(command=="Disable L1")       disableL1();

  if(command=="Hard Reset")       hardReset();
  if(command=="Sync Reset")       syncReset();
  if(command=="Reset BXEVT")      reset_bxevt();
  if(command=="BX0")              bx0();
  if(command=="BXR")              bxr();
}
//
void CCB::FireCCBMpcInjector(){
  //
  //
  // Issue L1Reset
  //
  // l1aReset();
  //
  // Issue L1 Start
  //
  //startTrigger();
  //
  // Issue BC0
  //
  //bc0();
  //
  // Inject TMB Pattern
  //
  injectTMBPattern();
  //
  // Stop Trigger
  //
  //stopTrigger();
  //
}
//
bool CCB::CheckFirmwareDate() {
  //
  bool check_ok;
  //
  // read the firmware version:
  firmwareVersion();
  //
  if ( read_firmware_day_   == GetExpectedFirmwareDay()   &&
       read_firmware_month_ == GetExpectedFirmwareMonth() &&
       read_firmware_year_  == GetExpectedFirmwareYear()  ) {
    check_ok = true;
  } else {
    check_ok = false;
  }
  //
  return check_ok;
}
//
void CCB::printFirmwareVersion() {
  //
  (*MyOutput_) << "CCB: firmware day-month-year: "  
	       << std::dec << read_firmware_day_ 
	       << "-"      << read_firmware_month_ 
	       << "-"      << read_firmware_year_
	       << std::endl;
  //
  return;
}
//
void CCB::firmwareVersion(){
  /// report the firmware version
  do_vme(VME_READ,CSRB17,sndbuf,rcvbuf,NOW);  
  int versionWord = (rcvbuf[0]<<8) + (rcvbuf[1]&0xFF);
  //  std::cout << std::hex << "Word=" << versionWord << std::endl;
  read_firmware_day_   =  versionWord & 0x1F;
  read_firmware_month_ = (versionWord >> 5   ) & 0xF;
  read_firmware_year_  = (versionWord >>(5+4)) + 2000;
  //
  //printFirmwareVersion();
  //
  return;
}

void CCB::signal_csrb2(int cmd){
  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane signals.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
  }
  int i_ccb=cmd & 0x2F;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xFC;
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  if (switchedMode)    setCCBMode(CCB::DLOG);
}

void CCB::hard_reset_alct(){
/// Hard ALCT reset: programming
/// Generate ALCT "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x01; 
// Hard ALCT reset:
// 2004 
  do_vme(VME_WRITE,0x66,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x30  data to write : anything
}

void CCB::hard_reset_dmb(){
/// Hard DMB reset: programming
/// Generate DMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x74; // 0x00;
// Hard DMB reset:
// 2004
  do_vme(VME_WRITE,0x64,sndbuf,rcvbuf,NOW);  // this is the correct DMB Hard Reset // correct: 11/13/2003 base+0x2e  data to write : anything
}

void CCB::hard_reset_tmb(){
/// Hard TMB reset: programming
/// Generate TMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard TMB reset:
// 2004
  do_vme(VME_WRITE,0x62,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x2c  data to write : anything
}

void CCB::hard_reset_mpc(){
/// Hard MPC reset: programming
/// Generate MPC "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard MPC reset:
// 2004
  do_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x32  data to write : anything
}

void CCB::hard_reset_ccb(){
/// Hard reset CCB FPGA by writting CSRA2, data to write : anything
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE,CSRA2,sndbuf,rcvbuf,NOW);
}

void CCB::soft_reset_ccb(){
/// soft reset CCB FPGA by writting CSRA3, data to write : anything
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE,CSRA3,sndbuf,rcvbuf,NOW);
}

void CCB::soft_reset_dmb(){
/// Generate "Soft Reset" 25 ns pulse to DMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,NOW);  //  base+0x6a
}

void CCB::soft_reset_tmb(){
/// Generate "Soft Reset" 25 ns pulse to TMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6c,sndbuf,rcvbuf,NOW); // base+0x7e
}

void CCB::soft_reset_mpc(){
/// Generate "Soft Reset" 25 ns pulse to MPC
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW); // base+0x64
}

void CCB::l1a_and_trig(){
/// Generate "L1ACC" 25 ns pulse and external trigger(if enabled)
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x54,sndbuf,rcvbuf,NOW); // base+0x3c
}

// code used by STEP
//
void CCB::EnableL1aFromVme()
{
	// enable L1A and clct_pretrig from VME command
	unsigned csrb1 = 0x1aed; // also disable all other trigger sources
	WriteRegister(CSRB1, csrb1);
}

void CCB::EnableL1aFromTmbL1aReq()
{
	// enable L1A from TMB L1A request line. 
	// clct_pretrig is not enabled since TMB will generate LCT for CFEBs
	unsigned csrb1 = 0x1edd; // also disable all other trigger sources
	WriteRegister(CSRB1, csrb1);
}

void CCB::EnableL1aFromSyncAdb()
{
	// enable L1A and clct_pretrig from ALCT sync test pulse
	unsigned csrb1 = 0x12fd; // also disable all other trigger sources
	WriteRegister(CSRB1, csrb1);
}

void CCB::EnableL1aFromASyncAdb()
{
	// enable L1A and clct_pretrig from ALCT Async test pulse
	unsigned csrb1 = 0x0afd; // also disable all other trigger sources
	WriteRegister(CSRB1, csrb1);
}

void CCB::EnableL1aFromDmbCfebCalibX()
{
	// enable L1A and clct_pretrig from any of dmb_cfeb_calib signals
	unsigned csrb1 = 0x1af9; // also disable all other trigger sources
	WriteRegister(CSRB1, csrb1);
}

void CCB::DisableL1a()
{
	// kill all triggers
	unsigned csrb1 = 0xffff;
	WriteRegister(CSRB1, csrb1);
}

void CCB::GenerateL1A()
{
	WriteRegister(L1ATrigger, 0); // generate L1A and pretriggers
}

void CCB::GenerateDmbCfebCalib0()
{
	// CFEB test pulse
	WriteRegister(DMB_CFEB_CAL0, 0);
}

void CCB::GenerateDmbCfebCalib1()
{
	// CFEB inject
	WriteRegister(DMB_CFEB_CAL1, 0);
}

void CCB::SetExtTrigDelay(unsigned delay)
{
	unsigned csrb5 = ReadRegister(CSRB5);
	csrb5 &= 0xff;
        csrb5 |= (delay << 8);
	WriteRegister(CSRB5, csrb5);
	
}

// for DMBTest
void CCB::inject_delay_l1a(int l1a_delay)
{
  //  setCCBMode(CCB::VMEFPGA);
  sndbuf[0]=0xDF;
  sndbuf[1]=0xFB;
  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);

  sndbuf[0]=(l1a_delay>>8)&0xff;
  sndbuf[1]=l1a_delay&0xff;
  //  printf(" l1a_delay %d %02x %02x \n",l1a_delay,sndbuf[0]&0xff,sndbuf[1]&0xff);
  do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,NOW);

  usleep(5000);

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE,DMB_CFEB_CAL1,sndbuf,rcvbuf,NOW);
  usleep(5000);
  // setCCBMode(CCB::DLOG);
}

void CCB::enablet() {
  setCCBMode(CCB::VMEFPGA);
  syncReset();
  usleep(50000);
  reset_bxevt();
  startTrigger();
  usleep(10000);
  bx0();
  usleep(10000);
  setCCBMode(CCB::DLOG);
}

int CCB::readDSN(void *data)
{
   char *dsn=(char *)data;
   int iloop=0, error, i, j, dsnstate, rd, c;

   WriteRegister(DSNclear,0);
   udelay(100);
   WriteRegister(DSNreset,0);
   udelay(2000);
   dsnstate=ReadRegister(CSRB9);
   while((dsnstate&5)!=4 && iloop<5)
   {
       udelay(1000*iloop);
       dsnstate=ReadRegister(CSRB9);
       iloop++;
   }
   if((dsnstate&5)!=4) return -1;
   int readcmd=0xF;   
   for(i=0; i<8; i++)
   {
      // WriteRegister(DSNclear,0);
      if(readcmd&1) WriteRegister(DSNwrite1, 0);
      else WriteRegister(DSNwrite0, 0);
      readcmd >>= 1;
      udelay(60);
   }
   error=0;
   for(i=0; i<8; i++)
   {
      rd=0;
      for(j=0; j<8; j++)
      {
         WriteRegister(DSNread, 0);
         udelay(10);
         c=ReadRegister(CSRB9);
         if(c&8)
            rd |= (((c>>1)&1)<<j);        
         else
            error++;
         udelay(5);
      }
      dsn[i]=rd;
   }      
   if(error) std::cout << "Errors in reading DSN: " << error << std::endl;
   return error;
}

int CCB::TestTTC(int testID, int n_loop)
{
   // testID:
   //     0 ----  all tests
   //     1 ----  Write-Read loop with fixed  patterns
   //     2 ----  Write-Read loop with random patterns
   //     3 ----  Write once-Read loop with random patterns
// (*MyOutput_)  
   int error1[4]={0,0,0,0}, error2[4]={0,0,0,0}, error3[4]={0,0,0,0};
   int tin, treg, rin[4],  pattern[8]={0, 0xFF, 0x55, 0xAA, 0x0F, 0xF0, 0x5A, 0xA5};
   if(n_loop>=0)
   {
       HardResetTTCrx();
       ReadTTCrxID();
       srand((unsigned int)time(NULL));
   }
   // test 1
   if(testID==0 || testID==1)
   {
       (*MyOutput_) << "Fixed Pattern Write-Read Test......";
       for(int l=0; l<n_loop; l++)
       {
          for(int k=0; k<8; k++)
          {
             tin=pattern[k];
             for(int i=0; i<4; i++)
             {
                 WriteTTCrxReg(i, tin);
             }
             usleep(100);
             for(int i=0; i<4; i++)
             {
                 treg=ReadTTCrxReg(i);
                 if((treg&0xFF)!=(tin&0xFF)) error1[i]++;
             }
          }
       }
       if((error1[0]+error1[1]+error1[2]+error1[3])==0)
       {  (*MyOutput_) << "PASSED" << std::endl;
       }
       else
       {
          (*MyOutput_) << "FAILED!!!" << std::endl;
          for(int i=0; i<4; i++)
          {
              if(error1[i]) (*MyOutput_) << "------>> Register " << i << " had " << error1[i] << " errors in " << 8*n_loop << " Write-Read." << std::endl;
          }
       }
   }
   // test 2
   if(testID==0 || testID==2)
   {
       (*MyOutput_) << "Random Pattern Write-Read Test......";
       for(int l=0; l<n_loop; l++)
       {
          for(int k=0; k<8; k++)
          {
             for(int i=0; i<4; i++) rin[i] = (int) (255.0 * (rand() / (RAND_MAX + 1.0)));
             for(int i=0; i<4; i++)
             {
                 WriteTTCrxReg(i, rin[i]);
             }
             usleep(100);
             for(int i=0; i<4; i++)
             {
                 treg=ReadTTCrxReg(i);
                 if((treg&0xFF)!=(rin[i]&0xFF)) error2[i]++;
             }
          }
       }
       if((error2[0]+error2[1]+error2[2]+error2[3])==0)
       {  (*MyOutput_) << "PASSED" << std::endl;
       }
       else
       {
          (*MyOutput_) << "FAILED!!!" << std::endl;
          for(int i=0; i<4; i++)
          {
              if(error2[i]) (*MyOutput_) << "------>> Register " << i << " had " << error2[i] << " errors in " << 8*n_loop << " Write-Read." << std::endl;
          }
       }
   }
   // test 3
   if(testID==0 || testID==3)
   {
       (*MyOutput_) << "Random Write-Once-Read-Loop Test......";
       for(int k=0; k<8; k++)
       {
          for(int i=0; i<4; i++) rin[i] = (int) (255.0 * (rand() / (RAND_MAX + 1.0)));
          for(int i=0; i<4; i++)
          {
              WriteTTCrxReg(i, rin[i]);
          }
          usleep(100);
          for(int l=0; l<n_loop; l++)
          {
              for(int i=0; i<4; i++)
              {
                 treg=ReadTTCrxReg(i);
                 if((treg&0xFF)!=(rin[i]&0xFF)) error3[i]++;
              }
              usleep(10);
          }
       }
       if((error3[0]+error3[1]+error3[2]+error3[3])==0)
       {  (*MyOutput_) << "PASSED" << std::endl;
       }
       else
       {
          (*MyOutput_) << "FAILED!!!" << std::endl;
          for(int i=0; i<4; i++)
          {
              if(error3[i]) (*MyOutput_) << "------>> Register " << i << " had " << error3[i] << " errors in " << 8*n_loop << " Read." << std::endl;
          }
       }
   }
   int totalerror=0;
   for(int i=0; i<4; i++) totalerror += (error1[i]+error2[i]+error3[i]);
   return totalerror;   
}

  } // namespace emu::pc
  } // namespace emu
