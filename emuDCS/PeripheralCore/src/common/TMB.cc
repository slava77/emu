//-----------------------------------------------------------------------
// $Id: TMB.cc,v 3.2 2006/08/08 19:23:08 mey Exp $
// $Log: TMB.cc,v $
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
#include <cstdio>
#include <sstream>

#include "TMB_constants.h"
#include "ALCTController.h"
#include "RAT.h"

// the VME addresses here are defined in
// http://www-collider.physics.ucla.edu/cms/trigger/tmb2001/tmb2001_spec.pdf

TMB::TMB(Crate * theCrate, int slot) :
  VMEModule(theCrate, slot),
  EMUjtag(this),
  ucla_ldev(1),
  cfeb0delay_(7),
  cfeb1delay_(7),
  cfeb2delay_(7),
  cfeb3delay_(7),
  cfeb4delay_(7),
  alct_tx_clock_delay_(0x00),
  alct_rx_clock_delay_(0x0a),
  l1a_window_size_(3),
  l1adelay_(128),
  alct_match_window_size_(3),
  alct_vpf_delay_(4),
  mpc_delay_(7),
  ALCT_input_(1),
  rpc_exists_(0xf),
  fifo_mode_(1),
  fifo_tbins_(7),
  fifo_pretrig_(2),
  alct_clear_(0),
  mpc_tx_delay_(0),
  l1a_offset_(0),
  disableCLCTInputs_(0),
  enableCLCTInputs_(0x1f),
  alctController_(0),
  rat_(0),
  bxn_offset_(0)
{
  //
  //jtag_address = -1;
  //jtag_chain = -1;
  //step_mode = false;
  //
  MyOutput_ = &std::cout ;
  (*MyOutput_) << "TMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
} 


TMB::~TMB() {
  (*MyOutput_) << "destructing ALCTController" << std::endl; 
  delete alctController_; 
  delete rat_;
  (*MyOutput_) << "destructing TMB" << std::endl;
}


int TMB::MPC0Accept(){
  //
  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW);
  //
  return (rcvbuf[0]&0x2)>>1; 
  //
}

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

    tmb_idcode_[device++] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  }
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
int TMB::FirmwareDate(){
  //
  tmb_vme(VME_READ,vme_idreg1_adr,sndbuf,rcvbuf,NOW);
  //
  return (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
}

int TMB::FirmwareYear(){
  //
  tmb_vme(VME_READ,vme_idreg2_adr,sndbuf,rcvbuf,NOW);
  //
  return (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
}


int TMB::FirmwareVersion(){
  //
  tmb_vme(VME_READ,vme_idreg0_adr,sndbuf,rcvbuf,NOW);
  //
  return (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
  //
}

int TMB::FirmwareRevCode(){
  //
  tmb_vme(VME_READ,vme_idreg3_adr,sndbuf,rcvbuf,NOW);
  //
  return (((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff)) ;
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
  (*MyOutput_) << "TMB.StartTTC" << std::endl;
  //
  WriteOutput("TMB.StartTTC");
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x6;
  sndbuf[1] = 0x3;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x1;
  sndbuf[1] = 0x3;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0] = 0x0;
  sndbuf[1] = 0x1;
  tmb_vme(VME_WRITE,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::WriteRegister(int reg, int value){
  //
  sndbuf[0] = (value>>8)&0xff;
  sndbuf[1] = value&0xff;
  //
  tmb_vme(VME_WRITE,reg,sndbuf,rcvbuf,NOW);
  //
}
//
bool TMB::SelfTest() {
  //
return 0;
  //
}
//
void TMB::init() {
}
//
void TMB::configure() {
  //
  ostringstream dump;
  ostringstream dump2;
  ostringstream dump3;
  //
  dump  << "TMB : configure() for TMB in slot = ";
  dump2 << theSlot;
  (*MyOutput_) << dump.str()+dump2.str() << std::endl;
  SendOutput(dump.str()+dump2.str(),"INFO");
  trgmode(5);
  //(*MyOutput_) << "Set TMB CSC ID to Slot_ID/2 = " << theSlot/2 << std::endl;
  load_cscid();
  //(*MyOutput_) << "Resetting counters" << std::endl;
  ResetCounters();
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
  (*MyOutput_) << "Injecting data" << std::endl ;
  //
  for (int evtId(0); evtId<nEvents; ++evtId) {
    //
    ramAdd = (evtId<<8);
    //
    if ( lct0 == 0 ) {
      frame2             = (unsigned short) ((rand()/(RAND_MAX+0.01))*0xffff) ;
      frame1             = (unsigned short) ((rand()/(RAND_MAX+0.01))*0xffff) ;
      frame1            |= (0x1<<15) ;
    } else {
      frame2             = lct0 & 0xffff;
      frame1             = (lct0>>16) & 0xffff;
    }
    //
    unsigned short vpf  = 1;         // 1 bit
    unsigned short qual = rand()%16; // 4 bits
    unsigned short clct = rand()%16; // 4 bits
    unsigned short wire = rand()%128;// 7 bits
    frame1 = ((vpf<<15)&0x8000) + ((qual<<11)&0x7800) +
      ((clct<<7)& 0x780) + wire;
    //
    unsigned short lr     = rand()%2;
    unsigned short halfSt = rand()%256;
    unsigned frame2 = ((lr<<8)&0x100) + halfSt;    
    //
    lct0_ = ((frame1&0xffff)<<16) | (frame2&0xffff) ;
    //
    InjectedLct0.push_back(lct0_);
    //
    printf(" lct0 = %x %x %x\n",frame1,frame2,(unsigned int)lct0_);
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
    if ( lct1 == 0 ) {
      frame2             = (unsigned short) ((rand()/(RAND_MAX+0.01))*0xffff);
      frame1             = (unsigned short) ((rand()/(RAND_MAX+0.01))*0xffff);
      frame1            |= (0x1<<15) ;
    } else {
      frame2             = lct1 & 0xffff;
      frame1             = (lct1>>16) & 0xffff;
    }
    //
    vpf  = 1;         // 1 bit
    qual = rand()%16; // 4 bits
    clct = rand()%16; // 4 bits
    wire = rand()%128;// 7 bits
    frame1 = ((vpf<<15)&0x8000) + ((qual<<11)&0x7800) +
    ((clct<<7)& 0x780) + wire;
    //
    lr     = rand()%2;
    halfSt = rand()%256;
    frame2 = ((lr<<8)&0x100) + halfSt;
    //
    lct1_ = ((frame1&0xffff)<<16) | (frame2&0xffff) ;
    //
    InjectedLct1.push_back(lct1_);
    //
    printf(" lct1 = %x %x %x\n",frame1,frame2,(unsigned int)lct1_);
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
  usleep(100);
  ReadBackMpcRAM(nEvents);
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
  (*MyOutput_) << "Reading back RAM address" << std::endl ;
  //
  for (int evtId(0); evtId<nEvents; ++evtId) {
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
    printf(" %x %x \n",(unsigned int)rlct01,(unsigned int)rlct02);
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
    printf(" %x %x \n",(unsigned int)rlct11,(unsigned int)rlct12);
    //
  }
  //
}
//
void TMB::FireMPCInjector(int nEvents){
  //
  tmb_vme(VME_READ,mpc_inj_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "Fire now" << std::endl;
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
}

void TMB::DecodeALCT(){
   //
  (*MyOutput_) << std::endl;
  (*MyOutput_) << "DecodeALCT.Read " << std::hex << alct_alct0_adr << std::endl ;
  tmb_vme(VME_READ,alct_alct0_adr,sndbuf,rcvbuf,NOW);
  (*MyOutput_) << "DecodeALCT.Done " << std::hex << alct_alct0_adr << std::endl ;
  //
  int data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
  //
  alct0_valid_     = data&0x1;
  alct0_quality_   = (data>>1)&0x3 ;
  alct0_amu_       = (data>>3)&0x1 ;
  alct0_first_key_ = (data>>4)&0x7f;
  alct0_first_bxn_ = (data>>11)&0x3 ;
  //
  (*MyOutput_) << " alct0      = " << data << std::endl;
  (*MyOutput_) << " valid      = " << std::dec<< alct0_valid_ << std::endl;
  (*MyOutput_) << " quality    = " << alct0_quality_ << std::endl ;
  (*MyOutput_) << " amu        = " << alct0_amu_ << std::endl ;
  (*MyOutput_) << " first_key  = " << alct0_first_key_ << std::endl;
  (*MyOutput_) << " first_bxn  = " << alct0_first_bxn_ << std::endl;
  (*MyOutput_) << std::endl;
  //
  tmb_vme(VME_READ,alct_alct1_adr,sndbuf,rcvbuf,NOW);
  //
  data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
  //
  alct1_valid_     = data&0x1;
  alct1_quality_   = (data>>1)&0x3 ;
  alct1_amu_       = (data>>3)&0x1 ;
  alct1_second_key_ = (data>>4)&0x7f;
  alct1_second_bxn_ = (data>>11)&0x3 ;
  //
  (*MyOutput_) << " alct1      = " << data << std::endl ;
  (*MyOutput_) << " valid      = " << alct1_valid_ << std::endl ;
  (*MyOutput_) << " quality    = " << alct1_quality_ << std::endl ;
  (*MyOutput_) << " amu        = " << alct1_amu_ << std::endl ;
  (*MyOutput_) << " second_key = " << alct1_second_key_ << std::endl ;
  (*MyOutput_) << " second_bxn = " << alct1_second_bxn_ << std::endl ;
  //
}
//
void TMB::DecodeCLCT(){
   //
   (*MyOutput_) << std::endl;
   tmb_vme(VME_READ,seq_clctm_adr,sndbuf,rcvbuf2,NOW);
   tmb_vme(VME_READ,seq_clct0_adr,sndbuf,rcvbuf,NOW);
   //
   int data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff | (rcvbuf2[1]&0x1f)<<16 ) ;
   printf(" data             = %8x \n",data);
   CLCT0_cfeb_ =  ((data>>14) & 0x7);
   CLCT0_nhit_ =  ((data>>1)  & 0x7);
   CLCT0_keyHalfStrip_ =  ((data>>9)  & 0x1f) ;
   (*MyOutput_) << "CLCT0.Valid      = " << ((data)     & 0x1)  << std::endl ;
   (*MyOutput_) << "CLCT0.Key HStrip = " << CLCT0_keyHalfStrip_ << std::endl;
   (*MyOutput_) << "CLCT0.Key CFEB   = " << CLCT0_cfeb_  << std::endl ;
   (*MyOutput_) << "CLCT0.Key nhit   = " << CLCT0_nhit_  << std::endl;
   (*MyOutput_) << "CLCT0.Key PatD   = " << ((data>>7)  & 0x1)  << std::endl ;
   (*MyOutput_) << "CLCT0.BXN        = " << ((data>>17) & 0x3)  << std::endl ;
   (*MyOutput_) << std::endl;
   //
   tmb_vme(VME_READ,seq_clct1_adr,sndbuf,rcvbuf,NOW);
   data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff ) | ((rcvbuf2[1]>>5)&0x7)<<16 | (rcvbuf2[0]&0x3)<<19 ;
   printf(" data             = %8x \n",data);
   CLCT1_cfeb_ =  ((data>>14) & 0x7);
   CLCT1_nhit_ =  ((data>>1)  & 0x7);
   CLCT1_keyHalfStrip_ =  ((data>>9)  & 0x1f) ;
   (*MyOutput_) << "CLCT1.Valid      = " << ((data)     & 0x1)  << std::endl;
   (*MyOutput_) << "CLCT1.Key HStrip = " << CLCT1_keyHalfStrip_ << std::endl;
   (*MyOutput_) << "CLCT1.Key CFEB   = " << CLCT1_cfeb_  << std::endl ;
   (*MyOutput_) << "CLCT1.Key nhit   = " << CLCT1_nhit_  << std::endl;
   (*MyOutput_) << "CLCT1.Key PatD   = " << ((data>>7)  & 0x1)  << std::endl ;
   (*MyOutput_) << "CLCT1.BXN        = " << ((data>>17) & 0x3)  << std::endl ;
   (*MyOutput_) << std::endl;
   //
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
  (*MyOutput_) << std::endl;
  //
  if (counter<0)                  (*MyOutput_) << "--------------------------------------------------------" << std::endl;
  if (counter<0)                  (*MyOutput_) << "---              Counters                             --" << std::endl;
  if (counter<0)                  (*MyOutput_) << "--------------------------------------------------------" << std::endl;
  if ((counter<0)||(counter==0))  (*MyOutput_) << CounterName(0)  << FinalCounter[0] <<std::endl ;
  if ((counter<0)||(counter==1))  (*MyOutput_) << CounterName(1)  << FinalCounter[1] <<std::endl ;
  if ((counter<0)||(counter==2))  (*MyOutput_) << CounterName(2)  << FinalCounter[2] <<std::endl ;
  if ((counter<0)||(counter==3))  (*MyOutput_) << CounterName(3)  << FinalCounter[3] <<std::endl ;
  if ((counter<0)||(counter==4))  (*MyOutput_) << CounterName(4)  << FinalCounter[4] <<std::endl ;
  if ((counter<0)||(counter==5))  (*MyOutput_) << CounterName(5)  << FinalCounter[5] <<std::endl ;
  if ((counter<0)||(counter==6))  (*MyOutput_) << CounterName(6)  << FinalCounter[6] <<std::endl ;
  if ((counter<0)||(counter==7))  (*MyOutput_) << CounterName(7)  << FinalCounter[7] <<std::endl ;
  if ((counter<0)||(counter==8))  (*MyOutput_) << CounterName(8)  << FinalCounter[8] <<std::endl ;
  if ((counter<0)||(counter==9))  (*MyOutput_) << CounterName(9)  << FinalCounter[9] <<std::endl ;
  if ((counter<0)||(counter==10)) (*MyOutput_) << CounterName(10) << FinalCounter[10] <<std::endl ;
  if ((counter<0)||(counter==11)) (*MyOutput_) << CounterName(11) << FinalCounter[11] <<std::endl ;
  if ((counter<0)||(counter==12)) (*MyOutput_) << CounterName(12) << FinalCounter[12] <<std::endl ;
  if ((counter<0)||(counter==13)) (*MyOutput_) << CounterName(13) << FinalCounter[13] <<std::endl ;
  if ((counter<0)||(counter==14)) (*MyOutput_) << CounterName(14) << FinalCounter[14] <<std::endl ;
  if ((counter<0)||(counter==15)) (*MyOutput_) << CounterName(15) << FinalCounter[15] <<std::endl ;
  if ((counter<0)||(counter==16)) (*MyOutput_) << CounterName(16) << FinalCounter[16] <<std::endl ;
  if ((counter<0)||(counter==17)) (*MyOutput_) << CounterName(17) << FinalCounter[17] <<std::endl ;
  if ((counter<0)||(counter==18)) (*MyOutput_) << CounterName(18) << FinalCounter[18] <<std::endl ;
  if ((counter<0)||(counter==19)) (*MyOutput_) << CounterName(19) << FinalCounter[19] <<std::endl ;
  if ((counter<0)||(counter==20)) (*MyOutput_) << CounterName(20) << FinalCounter[20] <<std::endl ;
  if ((counter<0)||(counter==21)) (*MyOutput_) << CounterName(21) << FinalCounter[21] <<std::endl ;
  if ((counter<0)||(counter==22)) (*MyOutput_) << CounterName(22) << FinalCounter[22] <<std::endl;
  //
  (*MyOutput_) << std::endl;
   //
}
//
std::string TMB::CounterName(int counter){
  //
  std::string name="Not defined";;
  if( counter == 0 ) name = "ALCT: CRC error                                         ";
  if( counter == 1 ) name = "ALCT: LCT sent to TMB                                   ";
  if( counter == 2 ) name = "ALCT: LCT error (alct debug firmware)                   ";
  if( counter == 3 ) name = "ALCT: L1A readout                                       ";
  if( counter == 4 ) name = "CLCT: Pretrigger                                        ";
  if( counter == 5 ) name = "CLCT: Pretrig but no wbuf available                     ";
  if( counter == 6 ) name = "CLCT: Invalid pattern after drift                       ";
  if( counter == 7 ) name = "CLCT: TMB matching rejected event                       ";
  if( counter == 8 ) name = "TMB:  CLCT or  ALCT or both triggered                   ";
  if( counter == 9 ) name = "TMB:  CLCT or  ALCT or both triggered xmit MPC          ";
  if( counter == 10) name = "TMB:  CLCT and ALCT matched in time                     ";
  if( counter == 11) name = "TMB:  ALCT-only trigger                                 ";
  if( counter == 12) name = "TMB:  CLCT-only trigger                                 ";
  if( counter == 13) name = "TMB:  No trig pulse response                            ";
  if( counter == 14) name = "TMB:  No MPC transmission                               ";
  if( counter == 15) name = "TMB:  No MPC response FF pulse                          ";
  if( counter == 16) name = "TMB:  MPC accepted LCT0                                 ";
  if( counter == 17) name = "TMB:  MPC accepted LCT1                                 ";
  if( counter == 18) name = "L1A:  BUGS, IGNORE (should be L1A received)             ";
  if( counter == 19) name = "L1A:  TMB triggered, TMB in L1A window                  ";
  if( counter == 20) name = "L1A:  L1A received, no TMB in window                    ";
  if( counter == 21) name = "L1A:  TMB triggered, no L1A received                    ";
  if( counter == 22) name = "L1A:  TMB readout                                       ";
  //
  return name;
  //
}
//
void TMB::ResetCounters(){
  //
  unsigned long int adr;
  //unsigned long int rd_data ;
  unsigned long int wr_data;
  //
  // Clear counters
  //
  adr = cnt_ctrl_adr ;
  wr_data= 0x1; //clear
  sndbuf[0] = (wr_data>>8)&0xff;
  sndbuf[1] = (wr_data&0xff);
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  wr_data= 0x0; //unclear
  sndbuf[0] = (wr_data>>8)&0xff;
  sndbuf[1] = (wr_data&0xff);
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);   
  //
}

int TMB::GetCounter(int counterID){
  //
  return FinalCounter[counterID];
  //
}

void TMB::GetCounters(){
  //
  unsigned long int adr;
  unsigned long int rd_data ;
  unsigned long int wr_data;
  //
  // Take snapshot of current counter state
  //
  adr = cnt_ctrl_adr ;
  wr_data= 0x2; //snap
  sndbuf[0] = (wr_data>>8)&0xff;
  sndbuf[1] = (wr_data&0xff);
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  wr_data= 0x0; //unsnap
  sndbuf[0] = (wr_data>>8)&0xff;
  sndbuf[1] = (wr_data&0xff);
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);   
  //
  //
  // Read counters
  //
  for (int counter=0; counter < MaxCounter; counter++){
    adr = cnt_ctrl_adr;
    wr_data= counter << 8 ;
    sndbuf[0] = (wr_data>>8)&0xff;
    sndbuf[1] = (wr_data&0xff);
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    adr = cnt_rdata_adr;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    //
    // Combine lsbs+msbs
    //
    int cnt_lsb, cnt_msb;
      long int cnt_full;
      //
      if( counter%2 ==0 ) {          //even addresses contain counter LSBs
	cnt_lsb = rd_data;
      }
      else                           //odd addresses contain counter MSBs
	{	 
	  cnt_msb  = rd_data;
	  cnt_full = cnt_lsb | (cnt_msb<<16) ;
	  FinalCounter[counter/2] = cnt_full ;     //assembled counter MSB,LSB	 
	}
  }   
  //
  //
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
    printf("Taking %d %x \n",i,(unsigned int)datain[i].to_ulong());
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

int TMB::TMBCRCcalc(std::vector<std::bitset <16> >& TMBData) {
  //
  std::bitset<22> CRC=calCRC22(TMBData);
  (*MyOutput_) << " Test here " << CRC.to_ulong() << std::endl ;
  return CRC.to_ulong();
  //
}

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
   //int alct_wdcnt, alct_busy;
   int rd_data, wr_data, alct_rdata;
   int tmb_state, halt_state;
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



void TMB::DisableL1aRequest(){
  //
  int adr;
  adr = ccb_trig_adr;
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


void TMB::ALCTRawhits(){
  //   
  int adr, alct_wdcnt, alct_busy, rd_data, wr_data, alct_rdata;
  int tmb_state, halt_state;
  std::vector < std::bitset<16> > alct_data;
  
  //Clear RAM address for next event and set sync bit
  /*
  adr = alctfifo1_adr ;
  wr_data  = 0x1; //reset RAM write address
  wr_data |= 0x1000;  // Set sync mode
  sndbuf[0] = (wr_data & 0xff00)>>8 ;
  sndbuf[1] = wr_data & 0x00ff ;
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  wr_data  = 0x0; //unreset
  wr_data |= 0x1000;  // Set sync mode
  sndbuf[0] = (wr_data & 0xff00)>>8 ;
  sndbuf[1] = wr_data & 0x00ff ;
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  */
  //
  ::sleep(2);
  //
  //while ( 1 < 2 ) {
    //
    // Pretrigger halt
    //
  adr = seq_clct_adr ;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  printf(" Start loop tmb_state machine  = %4ld\n",(rd_data>>15)&0x1);
  sndbuf[0] = (rcvbuf[0] & 0x7f) | 0x80 ;
  sndbuf[1] = rcvbuf[1];
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  //
  adr = seqsm_adr ;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  tmb_state     = (rd_data & 0x7);
  //
  adr = seq_clct_adr ;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  halt_state = ((rd_data>>15)&0x1);
  //
  adr = alct_fifo_adr ;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  alct_wdcnt = (rd_data >> 2 ) & 0x1ff;
  alct_busy  = rd_data & 0x0001;
  //
  printf("   word count         = %4ld\n",alct_wdcnt);
  printf("   busy               = %4ld\n",alct_busy);
  printf("   tmb_state machine  = %4ld\n",tmb_state);
  printf("   halt_state         = %4ld\n",halt_state);   
  //
  while((alct_busy) || (tmb_state != 7 )) {
    //
    adr = seqsm_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    tmb_state     = (rd_data & 0x7);
    //
    adr = seq_clct_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    halt_state = ((rd_data>>15)&0x1);
    //
    adr = alct_fifo_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    alct_wdcnt = (rd_data >> 2 ) & 0x1ff;
    alct_busy  = rd_data & 0x0001;
    //
    printf("   word count = %4ld\n",alct_wdcnt);
    printf("   busy       = %4ld\n",alct_busy);
    printf("   tmb_state machine  = %4ld\n",tmb_state);
    printf("   halt_state         = %4ld\n",halt_state);   
    //
  }
  //Write RAM read address to TMB
  for(int i=0;i<alct_wdcnt;i++) {
    //
    adr = alctfifo1_adr ;
    wr_data = (i & 0x7FFF) << 1;
    wr_data |= 0x1000;  // Set sync mode
    wr_data |= 0x0000;  // Disable sync mode
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    
    //Read RAM data from TMB
    adr = alctfifo2_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    alct_rdata = rd_data;
    
    printf("Adr=%4ld, Data=%5lx\n",i,alct_rdata);
    alct_data.push_back((std::bitset<16>)alct_rdata);
  }
  //
  if ( alct_wdcnt > 0 ) {
    
    printf("The size is %d\n",alct_data.size());
	
    int CRC_end  = alct_data.size();
    int CRC_low  = (alct_data[CRC_end-4].to_ulong()) &0x7ff ;
    int CRC_high = (alct_data[CRC_end-3].to_ulong()) &0x7ff ;
    int CRCdata  = (CRC_high<<11) | CRC_low ;
    
    int CRCcalc = TMBCRCcalc(alct_data) ;
    
    printf(" CRC %x \n",CRCcalc);
    printf(" CRC in data stream %x %x %x \n",alct_data[CRC_end-4].to_ulong(),
	   alct_data[CRC_end-3].to_ulong(),CRCdata);
	 
    if ( CRCcalc != CRCdata ) {
      printf("ALCT CRC doesn't agree \n");
    } else {
      printf("ALCT CRC does    agree \n");
	 }
    
  }
  
  alct_data.clear();
  
  //Clear RAM address for next event
  /*
    adr = alctfifo1_adr ;
    wr_data = 0x1; //reset RAM write address
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    wr_data = 0x0; //unreset
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  */
  //
  
  // Pretrigger unhalt
  
  adr = seq_clct_adr ;
  tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  sndbuf[0] = (rcvbuf[0] & 0x7f) ;
  sndbuf[1] = rcvbuf[1];
  tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
  
  //}
} 
//
void TMB::ResetALCTRAMAddress(){
   //
   //Clear RAM address for next event
   //
   int adr, wr_data, rd_data;
   int busy=0;
   //
   tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
   while ( rcvbuf[1]&0x1 != 0 && busy < 20 ){
     (*MyOutput_) << "1.Waiting for busy to clear" <<std::endl;
     tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);
     busy++;
   }
   //
   adr = alctfifo1_adr ;
   wr_data = 0x1; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
   tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
   while ( rcvbuf[1]&0x1 != 0 ){
     (*MyOutput_) << "2.Waiting for busy to clear" <<std::endl;
     tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
   }
   //
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
   tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
   while ( rcvbuf[1]&0x1 != 0 ){
     (*MyOutput_) << "3.Waiting for busy to clear" <<std::endl;
     tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
   }
   //
}

void TMB::ResetRAMAddress(){
   //
   //Clear RAM address for next event
   //
   int adr, wr_data, rd_data;
   //
   adr = dmb_ram_adr ;
   wr_data = 0x2000; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
}

void TMB::TMBRawhits(){
   //
   int adr, dmb_wdcnt, dmb_busy, rd_data, wr_data, dmb_rdata, tmb_state, halt_state, tmb_state_comp;
   std::vector < std::bitset<16> > tmb_data;
   //
   //Clear RAM address for next event
   //
   adr = dmb_ram_adr ;
   wr_data = 0x2000; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
   ::sleep(2);
   //
   // Read forever
   //
   while ( 1 < 2 ) {
      //      
      // Pretrigger halt
      //
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      sndbuf[0] = (rcvbuf[0] & 0x7f) | 0x80 ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      //
      // Check state machine
      //
      adr = seqsm_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      tmb_state     = (rd_data & 0x7);
      tmb_state_comp=  rd_data ;
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      halt_state = ((rd_data>>15)&0x1);
      
      adr = dmb_wdcnt_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      dmb_wdcnt = rd_data & 0x0fff;
      dmb_busy  = (rd_data >> 14 ) & 0x0001;
    
      printf("   word count              = %4ld\n",dmb_wdcnt);
      printf("   busy                    = %4ld\n",dmb_busy);
      printf("   tmb_state machine       = %4ld\n",tmb_state);
      printf("   tmb_state machine comp  = %4ld\n",tmb_state_comp);
      printf("   halt_state              = %4ld\n",halt_state);
      
      while((dmb_busy) || (dmb_wdcnt <= 0) || (tmb_state != 7) ) {
	 
	 adr = seqsm_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 tmb_state     = (rd_data & 0x7);
	 
	 adr = dmb_wdcnt_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_wdcnt = rd_data & 0x0fff;
	 dmb_busy  = (rd_data >> 14 ) & 0x0001;
      
	 printf("   word count              = %4ld\n",dmb_wdcnt);
	 printf("   busy                    = %4ld\n",dmb_busy);
	 printf("   tmb_state machine       = %4ld\n",tmb_state);

      }
      //
      //Write RAM read address to TMB
      //
      for(int i=0;i<dmb_wdcnt;i++) {
	 adr = dmb_ram_adr ;
	 wr_data = (i & 0xFFFF);
	 sndbuf[0] = (wr_data & 0xff00)>>8 ;
	 sndbuf[1] = wr_data & 0x00ff ;
	 tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	 
	 //Read RAM data from TMB
	 adr = dmb_rdata_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_rdata = rd_data;
	 
	 printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
	 tmb_data.push_back((std::bitset<16>)dmb_rdata);
      }

      printf("The size is %d\n",tmb_data.size());

      int CRC_end  = tmb_data.size();
      int CRC_low  = (tmb_data[CRC_end-4].to_ulong()) &0x7ff ;
      int CRC_high = (tmb_data[CRC_end-3].to_ulong()) &0x7ff ;
      int CRCdata  = (CRC_high<<11) | CRC_low ;

      int CRCcalc = TMBCRCcalc(tmb_data) ;

      printf(" CRC %x \n",CRCcalc);
      printf(" CRC in data stream %x %x %x \n",tmb_data[CRC_end-4].to_ulong(),
	     tmb_data[CRC_end-3].to_ulong(),CRCdata);
      
      if ( CRCcalc != CRCdata ) {
	 printf("TMB CRC doesn't agree \n");
      } else {
	 printf("TMB CRC does    agree \n");
      }

      tmb_data.clear();
      //
      //Clear RAM address for next event
      //
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      //
      // Pretrigger unhalt
      //
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      sndbuf[0] = (rcvbuf[0] & 0x7f) ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
            
   }

}

void TMB::OnlyReadTMBRawhits(){
   //
   int adr, dmb_wdcnt, dmb_busy, rd_data, wr_data, dmb_rdata, tmb_state, halt_state, tmb_state_comp;
   std::vector < std::bitset<16> > tmb_data;
      //
      // Check state machine
      //
      adr = seqsm_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      tmb_state     = (rd_data & 0x7);
      tmb_state_comp=  rd_data ;
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      halt_state = ((rd_data>>15)&0x1);
      
      adr = dmb_wdcnt_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      dmb_wdcnt = rd_data & 0x0fff;
      dmb_busy  = (rd_data >> 14 ) & 0x0001;
    
      printf("   word count              = %4ld\n",dmb_wdcnt);
      printf("   busy                    = %4ld\n",dmb_busy);
      printf("   tmb_state machine       = %4ld\n",tmb_state);
      printf("   tmb_state machine comp  = %4ld\n",tmb_state_comp);
      printf("   halt_state              = %4ld\n",halt_state);
      
      while((dmb_busy) || (dmb_wdcnt <= 0) ) {
	 
	 adr = seqsm_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 tmb_state     = (rd_data & 0x7);
	 
	 adr = dmb_wdcnt_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_wdcnt = rd_data & 0x0fff;
	 dmb_busy  = (rd_data >> 14 ) & 0x0001;
      
	 printf("   word count              = %4ld\n",dmb_wdcnt);
	 printf("   busy                    = %4ld\n",dmb_busy);
	 printf("   tmb_state machine       = %4ld\n",tmb_state);

      }
      //
      //Write RAM read address to TMB
      //
      for(int i=0;i<dmb_wdcnt;i++) {
	 adr = dmb_ram_adr ;
	 wr_data = (i & 0xFFFF);
	 sndbuf[0] = (wr_data & 0xff00)>>8 ;
	 sndbuf[1] = wr_data & 0x00ff ;
	 tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	 
	 //Read RAM data from TMB
	 adr = dmb_rdata_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_rdata = rd_data;
	 
	 printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
	 tmb_data.push_back((std::bitset<16>)dmb_rdata);
      }

      printf("The size is %d\n",tmb_data.size());

      int CRC_end  = tmb_data.size();
      int CRC_low  = (tmb_data[CRC_end-4].to_ulong()) &0x7ff ;
      int CRC_high = (tmb_data[CRC_end-3].to_ulong()) &0x7ff ;
      int CRCdata  = (CRC_high<<11) | CRC_low ;

      int CRCcalc = TMBCRCcalc(tmb_data) ;

      printf(" CRC %x \n",CRCcalc);
      printf(" CRC in data stream %x %x %x \n",tmb_data[CRC_end-4].to_ulong(),
	     tmb_data[CRC_end-3].to_ulong(),CRCdata);
      
      if ( CRCcalc != CRCdata ) {
	 printf("TMB CRC doesn't agree \n");
      } else {
	 printf("TMB CRC does    agree \n");
      }

      tmb_data.clear();
      //
      //Clear RAM address for next event
      //
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

}

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


void TMB::fifomode() {

}

void TMB::DataSendMPC(){
  //
  (*MyOutput_) << "TMB LCT data send to MPC" << std::endl;
  //
  tmb_vme(VME_READ,mpc0_frame0_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "MPC0 " << std::endl;
  (*MyOutput_) << "FRAME0 " << std::hex << ((rcvbuf[0]&0xff)<<8 | rcvbuf[1]&0xff) << std::endl ; 
  //
  tmb_vme(VME_READ,mpc0_frame1_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "FRAME1 " << std::hex << ((rcvbuf[0]&0xff)<<8 | rcvbuf[1]&0xff) << std::endl ; 
  //
  tmb_vme(VME_READ,mpc1_frame0_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "MPC1 " << std::endl;
  (*MyOutput_) << "FRAME0 " << std::hex << ((rcvbuf[0]&0xff)<<8 | rcvbuf[1]&0xff) << std::endl ; 
  //
  tmb_vme(VME_READ,mpc1_frame1_adr,sndbuf,rcvbuf,NOW);
  //
  (*MyOutput_) << "FRAME1 " << std::hex << ((rcvbuf[0]&0xff)<<8 | rcvbuf[1]&0xff) << std:: endl ; 
  //
  (*MyOutput_) << std::endl ;
  //
}

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
  printf(" I am in CSC Slot %d \n",theSlot);
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  printf(" read CSC %02x %02x from TMB\n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );

  sndbuf[0]=( (rcvbuf[0]&0x00fe) | ((i>>3)&0x1)) & 0x00ff;
  sndbuf[1]=( (rcvbuf[1]&0x001f) | ((i<<5)&0xe0)) & 0x00ff;
  printf(" write CSC ID %02x %02x to TMB\n",sndbuf[0]&0x00ff,sndbuf[1]&0x00ff);
  tmb_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,NOW);

  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  printf(" read %02x %02x CSC from TMB \n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );
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
  tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
  while ( rcvbuf[1]&0x1 != 0 ){
    (*MyOutput_) << "10.Waiting for busy to clear" <<std::endl;
    tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
  }
  //
  while ( rcvbuf[1]&0x1 != 1 ){
    (*MyOutput_) << "11.Waiting for done" <<std::endl;
    tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);   
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
  start(1);
  do_vme(fcn, vme, snd, rcv, wrt);
}


void TMB::start() {
  //
  (*MyOutput_) << "starting to talk to TMB, device " << ucla_ldev << std::endl;
  //
  // send the first signal
  SetupJtag();
  VMEModule::start();
  theController->initDevice(ucla_ldev, 0);
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
/*
void TMB::jtag_anystate_to_rti() {
  //take JTAG tap from any state to TLR then to RTI

  const int mxbits = 6;           // bits of TMS

  int tms[mxbits];
  int tdi[mxbits];
  int tdo[mxbits];

  int tms_rti[mxbits] = {1, 1, 1, 1, 1, 0}; //Anystate to TLR then RTI
  int tdi_rti[mxbits] = {};

  int iframe = 0;                                     //JTAG frame number

  for (int ibit=0; ibit<mxbits; ibit++) {             // go from any state to RTI
    tms[iframe]=tms_rti[ibit];                        // take tap to RTI
    tdi[iframe++]=tdi_rti[ibit];
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  return;
}
*/
/*
int TMB::bits_to_int(int * bits,
			   int length,
			   int MsbOrLsb) {
  //convert bits array of 1 bit per bye into an integer
  // MsbOrLsb for DACs that take MSB first
  int value;
  int dummy;
  int ibit;

  if (length>32) {
      (*MyOutput_) << "bits_to_int ERROR: Too many bits -> " << length << std::endl;
      (*MyOutput_) << "STOP the program (ctrl-c)...";
      std::cin >> dummy;
  }

  value = 0;
  if (MsbOrLsb == 0) {       // Translate LSB first    
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[ibit]&0x1) << ibit);
  } else {                   // Translate MSB first
    for (ibit=0; ibit<length; ibit++) 
      value |= ((bits[length-ibit-1]&0x1) << ibit);
  }

  return value;
}
*/
/*
void TMB::jtag_ir_dr(int chip_id, 
			   int opcode,
			   int * write_data,
			   int length, 
			   int *read_data) {


  // JTAG instructions and data are loaded for PROM1 first and PROM0 last, i.e.:
  //    JTAG position     Chip ID   Order  Load Name   TMB Name     RAT
  //    -------------     -------   -----  ---------   --------    ------
  //        tdi>0            0                 1        PROM0       FPGA
  //        tdo<1            1                 0        PROM1       PROM
  
  (*MyOutput_) << "TMBTester::JTAG write Instruction register opcode " << std::hex << opcode 
	       << " to JTAG address " << std::hex << jtag_address
	       << " for chip " << std::hex <<chip_id 
	       << std::endl;
  int k;
  int     iframe;                                               //JTAG frame counter
  int tdi[MAX_FRAMES], tms[MAX_FRAMES], tdo[MAX_FRAMES];

  int promIR[8] = {1, 0, 0, 2, 2, 0, 0, 0};     //Values the PROM IR should return.... -> 2 = don't care...

  // ** First JTAG operation writes opcodes to the TAP instruction registers...
  // ** Assume TAP controllers begin in the RTI state ***

  iframe = 0;

  // Put TAP in state ShfIR 
  for (k = 0; k < 4; k++) {
    tms[iframe] = tms_pre_opcode[k];
    tdi[iframe++] = tdi_pre_opcode[k];
  }

  // ** Construct opcode for the selected chip (all but chip_id are BYPASS = all 1's) **
  int idevice, ichip, ibit;
  int bit;

  for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

    ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

    for (ibit=0; ibit<bits_per_opcode[ichip]; ibit++) {    //up to the number of bits in this chip's opcode
      bit = 1;                                             //BYPASS
      if (ichip == chip_id) {                              //this is the chip we want
	bit = (opcode >> ibit) & 0x1;                      //extract bit from opcode
      }
      tms[iframe]=0;                                       // TAP stays in ShfIR
      tdi[iframe++]=bit;                                   // Instruction bit, advance the frame
    }
  }

  // ** Put TAP from ShfIR to Ex1IR ** 
  tms[iframe-1]=1;                                         // Done during the last bit of the opcode

  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_opcode[k];
    tdi[iframe++] = tdi_post_opcode[k];
  }

  //(*MyOutput_) << "Step 1 " << std::endl ;

  if (iframe > MAX_FRAMES) {
    (*MyOutput_) << "do_jtag IR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);   

  // **Check that the TDO shifted out instruction register strings**
  int iopbit;
  if ( jtag_chain == TMB_USER_PROM_CHAIN ||
       jtag_chain == TMB_FPGA_CHAIN ) {
    iframe = 4;                                    //start at first opcode bit (counting from 0)

    for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

      ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

      //      (*MyOutput_) << "Chip " << ichip 
      //		   << " has " << bits_per_opcode[ichip] 
      //		   << " bits per opcode" << std::endl;

      for (ibit=0; ibit<bits_per_opcode[ichip]; ibit++) {    //up to the number of bits in this chip's opcode
	iopbit = tdo[iframe++];                              //current opcode
	if ( iopbit != promIR[ibit] &&
	     promIR[ibit] != 2      ) {
	  (*MyOutput_) << "ERROR in opcode bit" << std::hex << ibit 
		       << ", Read->" << std::hex << iopbit 
		       << ", expected =" << std::hex << promIR[ibit] 
		       << std::endl;
	}	  
      }
    }
    (*MyOutput_) << "JTAG write Instruction Register: ";
    for (k=11; k>=4; k--) {
      (*MyOutput_) << tdo[k] << " ";
    }
    (*MyOutput_) << std::endl;
  }

  // ** Perform the second JTAG operation, which is to read the 
  //    TDO data which ought to be "length" bits long, into the
  //    array "read_data".  Include the possibility to shift in "write_data"
  //    on the tdi line, for destructive ShfDR.

  iframe = 0;

  //(*MyOutput_) << "Step 2 " << std::endl ;

  // Put TAP in state ShfDR from RTI 
  for (k = 0; k < 3; k++) {
    tms[iframe] = tms_pre_read[k];
    tdi[iframe++] = tdi_pre_read[k];
  }

  // ** Set up TMS to shift in the data bits for this chip, BYPASS code for others **
  int offset;

  for (idevice=0; idevice<devices_in_chain; idevice++) {   //loop over all the chips up to the number of chips in this chain

    ichip = devices_in_chain - idevice - 1;                //chip order in chain is reversed

    if (ichip == chip_id) {                                //this is the chip we want
      offset = iframe;                                     //here is the beginning of the data
      for (ibit=0; ibit<length; ibit++) {                  //up to the number of bits specified for this register
	tms[iframe] = 0;                                   // Stay in ShfDR
	tdi[iframe++] = write_data[ibit];                  // Shift in the data for TDI
      }
    } else {                                                 // bypass register is one frame      
      tms[iframe] = 0;                                       // TAP stays in ShfDR
      tdi[iframe++] = 0;                                     // No data goes out to bypass regs
    }
  }

  tms[iframe-1] = 1;                                         // Last opcode, TAP goes to Ex1DR
  
  // ** Put TAP from Ex1IR to RTI **
  for (k = 0; k < 2; k++) {
    tms[iframe] = tms_post_read[k];
    tdi[iframe++] = tdi_post_read[k];
  }

  if (iframe > MAX_FRAMES) {
    (*MyOutput_) << "do_jtag DR ERROR: Too many frames -> " << iframe << std::endl;
    (*MyOutput_) << "STOP the program (ctrl-c)...";
    std::cin >> k;
  }

  jtag_io_byte(iframe,tms,tdi,tdo);

  // ** copy relevant section of tdo to caller's read array **
  for (ibit=0; ibit<length; ibit++) {
    iframe = ibit+offset;
    if (iframe > MAX_FRAMES) {
      (*MyOutput_) << "do_jtag copy ERROR: Too many frames -> " << iframe << std::endl;
      (*MyOutput_) << "STOP the program (ctrl-c)...";
      std::cin >> k;
    }
    read_data[ibit] = tdo[iframe];
    if (jtag_chain == ALCT_MEZZ_USER_CHAIN) read_data[ibit] = tdo[iframe-1];  //ALCT fix (Jonathan)
  }

  return;
}
*/
/*
void TMB::jtag_io_byte(int nframes,
			     int * tms,
			     int * tdi,
			     int * tdo) {

  //	Clocks tck for nframes number of cycles.
  //	Writes nframes of data to tms and tdi on the falling edge of tck.
  //	Reads tdo on the rising clock edge.
  //
  //	Caller passes tms and tdi byte arrays with 1 bit per byte.	
  //	Returned data is also 1 bit per byte. Inefficent,but easy.
  //
  //	tms[]	=	byte's lsb to write to parallel port
  //	tdi[]	=	byte's lsb to write to parallel port
  //	tdo[]	=	bit read back from parallel port, stored in lsb
  //	tck	=	toggled by this routine

  // get current boot register:
  short unsigned int boot_state;
  int dummy = tmb_get_boot_reg(&boot_state);
  boot_state &= 0x7f80;                 //Clear JTAG bits

  //	Set tck,tms,tdi low, select jtag chain, enable VME control of chain

  int tck_bit = 0;                      //TCK low
  int tms_bit = 0;                      //TMS low
  int tdi_bit = 0;                      //TDI low
  int tdo_bit;
  int jtag_en = 1;                      //Boot register sources JTAG

  int sel0 = jtag_chain & 0x1;               //JTAG chain select
  int sel1 = (jtag_chain>>1) & 0x1;
  int sel2 = (jtag_chain>>2) & 0x1;
  int sel3 = (jtag_chain>>3) & 0x1;

  int jtag_word = boot_state;

  jtag_word |= tdi_bit;
  jtag_word |= tms_bit << 1;
  jtag_word |= tck_bit << 2;
  jtag_word |= sel0    << 3;
  jtag_word |= sel1    << 4;
  jtag_word |= sel2    << 5;
  jtag_word |= sel3    << 6;
  jtag_word |= jtag_en << 7;

  dummy = tmb_set_boot_reg(jtag_word);  //write boot register
  
  short unsigned int jtag_in;
  int jtag_out;

  step_mode = false;

  //Loop over input data frames
  int iframe;

  int i;

  if (nframes>0) {                       //no frames to send

    //loop over input data frames...
    for (iframe=0; iframe<nframes; iframe++) { //arrays count from 0
      tdo[iframe]=0;                               //clear tdo

      tck_bit = 0x0 << 2;                          //take TCK low
      tms_bit = (tms[iframe] & 0x1) << 1;                  //TMS bit
      tdi_bit = (tdi[iframe] & 0x1);                       //TDI bit

      jtag_out = jtag_word & 0x7ff8;               //clear old state
      jtag_out |= tck_bit | tms_bit | tdi_bit;

      dummy = tmb_set_boot_reg(jtag_out);    //write boot register
      dummy = tmb_get_boot_reg(&jtag_in);    //read boot register
      //** here only read tdo for step_mode **
      tdo[iframe] = (jtag_in >> 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];

      if (step_mode) {
	  step(iframe,tck_bit,tms_bit,tdi_bit,tdo_bit);
      }

      tck_bit = 0x1 << 2;  //Take TCK high, leave tms,tdi as they were
      jtag_out |= tck_bit | tms_bit | tdi_bit;

      dummy = tmb_set_boot_reg(jtag_out);    //write boot register
      dummy = tmb_get_boot_reg(&jtag_in);    //read boot register
      //** here is the real tdo, read on the rising edge **
      tdo[iframe] = (jtag_in >> 15) & 0x1;         //extract tdo bit, mask lsb
      tdo_bit = tdo[iframe];

      if (step_mode) {
      	step(iframe,tck_bit,tms_bit,tdi_bit,tdo_bit);
      }      
    }
  }
  //** put JTAG bits into an idle state **
  jtag_out &= 0xfffb;     //Take TCK low, leave others as they were
  dummy = tmb_set_boot_reg(jtag_out);        //write boot register

  return;
}
*/
/*
void TMB::step(int frame,
		     int tck, 
		     int tms, 
		     int tdi, 
		     int tdo){

  (*MyOutput_) << "frame = " << std::hex << frame
	       << " tck = " << std::hex << tck
	    << " tms = " << std::hex << tms
	    << " tdi = " << std::hex << tdi
	    << " tdo = " << std::hex << tdo
	    << " pause.... enter any number, then return...";
  int dummy;
  std::cin >> dummy;
  //
  return;
}
*/
/*
void TMB::select_jtag_chain_param(){
  //select:
  //  - how many chips are on this chain
  //  - how many bits per opcode (per chip)

  //Chains currently not supported by this member:
  //    ALCT_SLOW_USER_CHAIN
  //    ALCT_SLOW_PROM_CHAIN
  //    TMB_USER_PROM_CHAIN  

  devices_in_chain = 0;

  if (jtag_chain == TMB_USER_PROM_CHAIN ||
      jtag_chain == TMB_FPGA_CHAIN) {
    devices_in_chain = 2;
    bits_per_opcode[0] = 8;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 0;
    bits_per_opcode[3] = 0;
    bits_per_opcode[4] = 0;

  } else if (jtag_chain == RAT_CHAIN            ||
	     jtag_chain == ALCT_MEZZ_PROM_CHAIN ||
	     jtag_chain == ALCT_MEZZ_USER_CHAIN ) {
    devices_in_chain = 2;
    bits_per_opcode[0] = 5;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 0;
    bits_per_opcode[3] = 0;
    bits_per_opcode[4] = 0;
  } else if (jtag_chain== TMB_MEZZ_FPGA_CHAIN) {
    devices_in_chain = 5;
    bits_per_opcode[0] = 6;
    bits_per_opcode[1] = 8;
    bits_per_opcode[2] = 8;
    bits_per_opcode[3] = 8;
    bits_per_opcode[4] = 8;
  }

  if (devices_in_chain == 0) {
    (*MyOutput_) << "ERROR unsupported JTAG chain " << jtag_chain <<std::endl;
    int dummy = sleep(5);
  } else {
    (*MyOutput_) << "JTAG chain " << std::hex << jtag_chain 
		 << " has " << devices_in_chain << " devices " 
		 << " and " << bits_per_opcode[0] << " "
		 << ", " << bits_per_opcode[1] << " "
		 << ", " << bits_per_opcode[2] << " "
		 << ", " << bits_per_opcode[3] << " "
		 << ", " << bits_per_opcode[4] << " bits per opcode"
		 <<std::endl;
  }

  return;
}
*/
/*
void TMB::set_jtag_chain(int chain) { 
  //
  jtag_chain = chain;
  select_jtag_chain_param();
  //
  return;
}
*/
int TMB::tmb_set_reg(unsigned int vmereg, unsigned short int value )
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=value & 0xff;
   sndbuf[1]=(value >> 8) & 0xff;
   tmb_vme(VME_WRITE, vmereg, sndbuf, rcvbuf, NOW );
   return 0;	
}

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

int TMB::tmb_vme_reg(unsigned int vmereg, unsigned short int* value)
{
   tmb_set_reg(vmereg, *value);
   tmb_get_reg(vmereg, value);
   return 0;
}

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
  return 0;
  //
}

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


std::ostream & operator<<(std::ostream & os, TMB & tmb) {
  os << std::dec << "TMB: crate " << tmb.theCrate_
     << " slot " << tmb.theSlot << "\n"
     << std::hex << "  cfeb delays (hex) " << tmb.cfeb0delay_ << " " 
     << tmb.cfeb1delay_ << " " << tmb.cfeb2delay_ << " " 
     << tmb.cfeb3delay_ << " " << tmb.cfeb4delay_ << "\n"
     << " rx, tx clock delays " << tmb.alct_rx_clock_delay_ 
     << " " << tmb.alct_tx_clock_delay_ << std::endl
     << "alct delay " << tmb.alct_vpf_delay_ 
     << "window size " << tmb.alct_match_window_size_ << std::dec << std::endl;
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

void TMB::tmb_clk_delays(unsigned short int time,int cfeb_id) {
  //
  if(version_ == "2001") {
    old_clk_delays(time, cfeb_id);
  } else {
    new_clk_delays(time, cfeb_id);
  }
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
  /// disable all clocks to cfeb and alct. Should be used when
  /// updating the ALCT firmware
   tmb_vme(VME_READ, vme_step_adr,sndbuf,rcvbuf,NOW);
   sndbuf[0]=rcvbuf[0] & 0x1f;
   sndbuf[1]=rcvbuf[1] & 0xf8;
   tmb_vme(VME_WRITE, vme_step_adr, sndbuf,rcvbuf,NOW);
}

void TMB::enableAllClocks(){
  /// enable all clocks to cfeb and alct. Should be used after
  /// updating the ALCT firmware to get the TMB back in default mode.
   tmb_vme(VME_READ, vme_step_adr,sndbuf,rcvbuf,NOW);
   sndbuf[0]=rcvbuf[0] | 0xe0;
   sndbuf[1]=rcvbuf[1] | 0x07;
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
///////////////////////
// New classes from Greg, ported from TMBTester.cc
//////////////////////
//
std::bitset<64> TMB::dsnRead(int type) {
  //type = 0 = TMB
  //     = 1 = Mezzanine DSN
  //     = 2 = RAT DSN

  std::bitset<64> dsn;

  int offset;
  offset = type*5; 

  int wr_data, rd_data;

  int initial_state;
  // ** need to specifically enable RAT to read back DSN **
  if (type == 2) {  
    initial_state = ReadRegister(vme_ratctrl_adr);  //initial RAT state

    wr_data = initial_state & 0xfffd;    //0=sync_mode, 1=posneg, 2=loop_tmb, 3=free_tx0, 4=dsn enable
    wr_data |= 0x0010;                   //enable the dsn bit

    WriteRegister(vme_ratctrl_adr,wr_data);
  }

  int i;
  int idata;

  // init pulse >480usec
  wr_data = 0x0005; 
  wr_data <<= offset; //send it to correct component
  rd_data = dsnIO(wr_data);

  // ROM Read command = serial 0x33:
  for (i=0; i<=7; i++) {
    idata = (0x33>>i) & 0x1;
    wr_data = (idata<<1) | 0x1; //send "serial write pulse" with "serial SM start"
    wr_data <<= offset; 
    rd_data = dsnIO(wr_data);
  }

  // Read 64 bits of ROM data = 0x3 64 times
  for (i=0; i<=63; i++) {
    wr_data = 0x0003; 
    wr_data <<= offset;
    rd_data = dsnIO(wr_data);

    // pack data into dsn[]
    dsn[i] = (rd_data >> (4+offset)) & 0x1;
  }

  // ** Return the RAT to its initial state **
  if (type == 2) {
    WriteRegister(vme_ratctrl_adr,initial_state);
  }

  return dsn;
}

int TMB::dsnIO(int writeData){
  //Single I/O cycle for Digital Serial Number...
  //called by dsnRead...

  int adr = vme_dsn_adr;
  int readData;

  // write the desired data word:
  WriteRegister(adr,writeData);

  int tmb_busy,mez_busy,rat_busy;
  int busy = 1;
  int nbusy = 1;

  while (busy) {
    readData = ReadRegister(adr);
    
    // check busy on all components:
    tmb_busy = (readData>>3) & 0x1;
    mez_busy = (readData>>8) & 0x1;
    rat_busy = (readData>>13) & 0x1;
    busy = tmb_busy | mez_busy | rat_busy;

    if (nbusy%1000 == 0) {
      (*MyOutput_) << "dsnIO: DSN state machine busy, nbusy = "
                << nbusy << ", readData = " 
		<< std::hex << readData << std::endl;  
    }
    nbusy++;
  }

  // end previous cycle
  WriteRegister(adr,0x0000);

  return readData;
}

void TMB::ADCvoltages(float * voltage){

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

  int adc_dout;                      //Voltage monitor ADC serial data receive
  int adc_sclock;                    //Voltage monitor ADC serial clock
  int adc_din;                       //Voltage monitor ADC serial data transmit
  int adc_cs;                        //Voltage monitor ADC chip select

  int adc_shiftin;
  int iclk;

  int write_data, read_data;
  //
  for (int ich=0; ich<=14; ich++){
    //
    adc_dout = 0;

    adc_din    = 0;
    adc_sclock = 0;
    adc_cs     = 1;

    write_data = 0;
    write_data |= (adc_sclock << 6);  
    write_data |= (adc_din    << 7);  
    write_data |= (adc_cs     << 8);  

    WriteRegister(vme_adc_adr,write_data);

    adc_shiftin = ich << 4;      //d[7:4]=channel, d[3:2]=length, d[1:0]=ldbf,bip
    if (ich >= 14) adc_shiftin = 0;  //don't send channel 14, it is power-down
    
    //put adc_shiftin serially in 11 vme writes
    for (iclk=0; iclk<=11; iclk++){

      if (iclk <= 7) {
	adc_din = (adc_shiftin >> (7-iclk)) & 0x1;
      } else {
	adc_din = 0;
      }
      adc_sclock = 0;
      adc_cs     = 0;

      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      
      WriteRegister(vme_adc_adr,write_data);

      adc_sclock = 1;
      adc_cs     = 0;

      write_data = 0;
      write_data |= (adc_sclock << 6);  
      write_data |= (adc_din    << 7);  
      write_data |= (adc_cs     << 8);  
      
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
    if (ich>=1) {
      voltage[ich-1] = ((float) adc_dout / 4095.)*4.095; //convert adc value to volts
    }
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
  return;
}


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
  //        = 8  = RPC clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)

  if (device==0) data = (ReadRegister(0x18)>>12) & 0xf; 
  if (device==1) data = (ReadRegister(0x1a)>> 0) & 0xf; 
  if (device==2) data = (ReadRegister(0x1a)>> 4) & 0xf; 
  if (device==3) data = (ReadRegister(0x1a)>> 8) & 0xf; 
  if (device==4) data = (ReadRegister(0x1a)>>12) & 0xf; 
  if (device==5) data = (ReadRegister(0x16)>> 4) & 0xf; 
  if (device==6) data = (ReadRegister(0x16)>> 0) & 0xf; 
  if (device==7) data = (ReadRegister(0x16)>> 8) & 0xf; 
  if (device==8) data = (ReadRegister(0x16)>>12) & 0xf; 
  if (device==9) data = (ReadRegister(0x18)>> 0) & 0xf; 
  if (device==10) data =(ReadRegister(0x18)>> 4) & 0xf; 
  if (device==11) data =(ReadRegister(0x18)>> 8) & 0xf; 

  return data;
}

void TMB::new_clk_delays(unsigned short int time,int cfeb_id)
{
  // device = 0  = CFEB 0 Clock
  //        = 1  = CFEB 1 clock
  //        = 2  = CFEB 2 clock
  //        = 3  = CFEB 3 clock
  //        = 4  = CFEB 4 clock
  //        = 5  = ALCT rx clock
  //        = 6  = ALCT tx clock
  //        = 7  = DMB tx clock
  //        = 8  = RPC clock
  //        = 9  = TMB1 clock
  //        = 10 = MPC clock
  //        = 11 = DCC clock (CFEB duty cycle correction)
  //        = 1000 = CFEB [0-5] Clock (all CFEB's)

  //GREG, take these out of here when it goes back in place of new_clk_delays:
  //enum WRT { LATER, NOW };
  //char sndbuf[2],rcvbuf[2];

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
  if ( cfeb_id == 7 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 8 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 9 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 10 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
  } 
  if ( cfeb_id == 11 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,NOW);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,NOW);
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

int TMB::ReadTMBtempPCB() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x00;   // "local" temperature read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;


  return temperature;
}

int TMB::ReadTMBtempFPGA() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x01;   // "remote" temperature read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB temperature IC (FPGA)             = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

int TMB::ReadTMBtCritPCB() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x05;   // "local" temperature critical read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB Critical Temperature IC (PCB)     = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

int TMB::ReadTMBtCritFPGA() {

  int smb_adr = 0x2a;   // float, float state TMB LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  int temperature = smb_io(smb_adr,command,1);

  (*MyOutput_) << "TMB Critical Temperature (FPGA)       = " << std::dec << temperature
	       << " deg C " << std::endl;

  return temperature;
}

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

  //    module = 1 = TMB
  //           = 2 = RAT

  const int adc_adr = vme_adc_adr;

  int smb_data = 0xff;        // null write command

  int write_data,read_data;

  // Current ADC register state:
  int adc_status = ReadRegister(adc_adr);

  // **Step 1 write the command to read the data**

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

  // ** Construct SMBclk and SMBdata **
  //
  //	SMB requires that serial data is stable while clock is high,
  //	so data transitions occur while clock is low,
  //	midway between clock falling edge and rising edge

  int nclks = 115;

  int sda_clock,scl_clock;
  int sda,scl;

  int i2c_clock;
  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle

    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];

    //    (*MyOutput_) << "Before Persistent -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    //    (*MyOutput_) << "After Persistent  -> i2c_clock " << i2c_clock << ", scl = " << scl << " sda_bit = " << sda << std::endl;

    //** Write serial clock and data to TMB VME interface **

    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    WriteRegister(adc_adr,write_data);
  }

  // Current ADC register state:
  adc_status = ReadRegister(adc_adr);

  // **Step 2 read the data**

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

  int d[20];
  int sda_value;

  nclks = 79;

  for (i2c_clock=0; i2c_clock<=nclks; i2c_clock++) {  //200kHz
    sda_clock = (int) i2c_clock/4;                //50 kHz
    scl_clock = (int) ( (i2c_clock+1)/2 );        //50 kHz shifted 1/2 of a 100kHz cycle

    scl = scl_clock & 1;                          // 0 0 1 1 0 0 1 1 0 0 1 1 ....
    sda = sda_bit[sda_clock];

    if (i2c_clock<3) scl=1;                       // START scl stays high
    if (i2c_clock<2) sda=1;                       // START sda transitions low

    if (i2c_clock>nclks-3) scl=1;                // STOP scl stays high
    if (i2c_clock>nclks-2) sda=1;                // STOP sda transitions high

    //** Write serial clock and data to TMB VME interface **
    write_data = adc_status & 0xf9ff;    //clear bits 9 and 10
    write_data |= scl << 9;
    write_data |= sda << 10;
    WriteRegister(adc_adr,write_data);

    //** Read Serial data from TMB VME interface **
    // (read on every cycle to keep clock symmetric)
    read_data = ReadRegister(adc_adr);
    if (scl==1) {
      d[sda_clock] = read_data;
    }
  }

  // pack data into an integer...
  int ishift = 0;
  if (module == 1) ishift = 10;   //data bit from LM84 on TMB
  if (module == 2) ishift = 11;   //data bit from LM84 on RAT

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

  //  (*MyOutput_) << "Temperature = " << std::dec << data << " deg C" << std::endl;

  return data;
}
//
