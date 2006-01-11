//-----------------------------------------------------------------------
// $Id: CCB.cc,v 2.17 2006/01/11 13:47:27 mey Exp $
// $Log: CCB.cc,v $
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
#include "CCB.h"
#include "VMEController.h"
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>

//
CCB::CCB(int newcrate ,int slot, int version)
: VMEModule(newcrate, slot), 
  l1enabled_(false),
  mVersion(version),
  TTC(NO_TTC),
  CLK_INIT_FLAG(0),
  BX_Orbit_(924),
  SPS25ns_(0),
  TTCrxID_(0x010e),
  l1aDelay_(0),
  mDebug(false)
{
  MyOutput_ = &std::cout ;
  /// initialize VME registers pointers to the default CCB-version 2001
  if (mVersion == 2001){
    CSR1 = 0x00;
    CSR2 = 0x02;
    CSR5 = 0x08; 
    RST_CCB_INT_LOG = 0x28;
    CRATE_HARD_RESET = 0x34;
    DMB_CFEB_CAL0 = 0x48;
    DMB_CFEB_CAL1 = 0x4a;
    DMB_CFEB_CAL2 = 0x4c;
    mCCBMode = CCB::VMEFPGA;
  } else if (mVersion == 2004){
    CSR1 = CSRB1;
    CSR2 = CSRB2;
    CSR5 = CSRB5; 
    // deprecated:
    //RST_CCB_INT_LOG = 0x28; 
    CRATE_HARD_RESET = 0x60;
    DMB_CFEB_CAL0 = 0x8a;
    DMB_CFEB_CAL1 = 0x8c;
    DMB_CFEB_CAL2 = 0x8e;
    mCCBMode = CCB::DLOG;
  } else {
    std::cerr << "CCB: FATAL: unknown CCB version number ("<< mVersion
	      << "). Cannot create: exiting ..." << std::endl;
    exit(0);
  }
    
  std::cout << "CCB: configured for crate=" << this->crate()
	    << " slot=" << this->slot() << " (version=" << mVersion << ")" 
	    << std::endl;
 };

CCB::~CCB() {
  std::cout << "CCB: removing CCB from crate=" << this->crate() 
       << ", slot=" << this->slot() << std::endl;
}

void CCB::end() {
  char rcvx[2];
  char sndx[2];
  //fg 0xff is a JTAG reset ... not sure what it does here.
  do_vme(0xff,CSR1,sndx,rcvx,NOW);
  //2004 do_vme(0xff,CSRB1,sndx,rcvx,NOW);
}

void CCB::pulse(int Num_pulse,unsigned int * delays, char vme) 
{

  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    if (mDebug) std::cout << "CCB: NOTE -- switching from DLOG to FPGA mode for pulse" << std::endl; 
  }

   for(int j=0;j<Num_pulse;j++){
     sndbuf[0]=(delays[j]&0xff00)>>8;
     sndbuf[1]=(delays[j]&0x00ff);
     do_vme(0x03,CSR1,sndbuf,rcvbuf,LATER);
     //2004 do_vme(0x03,CSRB1,sndbuf,rcvbuf,LATER);
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
    if (mDebug) std::cout << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };


}


void CCB::pulse() {
  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    if (mDebug) std::cout << "CCB: NOTE -- switching from DLOG to FPGA mode for pulse" << std::endl; 
  }

 //use 0x48 for pulse (cal0), 0x4a inject (cal1), 0x4c pedestal (cal2):

  sndbuf[0]=0x00;
  sndbuf[1]=0xff;
  //fg ?? what does 0x03 do?? do_vme ignores it!
  do_vme(0x03,CSR1,sndbuf,rcvbuf,LATER);
  //2004 do_vme(0x03,CSRB1,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE,DMB_CFEB_CAL0,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,0x8a,sndbuf,rcvbuf,NOW);

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    if (mDebug) std::cout << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };


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
    if (mDebug) std::cout << "CCB: NOTE -- switching from DLOG to FPGA mode for pulse" << std::endl; 
  }
  unsigned int * delays = new unsigned int[Num_pulse];
  for(int i = 0; i <  Num_pulse; ++i) { 
    delays[i] = pulse_delay;
  }
  pulse( Num_pulse, delays, vme);
  delete[] delays;

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    if (mDebug) std::cout << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };


}


void CCB::pulse(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x48;)
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL0);
}


void CCB::inject(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x4a);
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL1);
}


void CCB::prgall_bckpln()
{
  /// Performs a Hard-Reset to all modules in the crate
  /// through a dedicated VME register (not FastControl)
  std::cout << "CCB: hard reset" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE,CRATE_HARD_RESET,sndbuf,rcvbuf,NOW);
  theController->end();
}
//
void CCB::reset_bckpln()
{
  /// Reinitializes the FPGAs on DMB, TMB and MPC boards
  /// through a FastControl soft-reset.
  std::cout << "CCB: soft reset" << std::endl;
  int i_ccb=0x1c;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE, CSR2, sndbuf,rcvbuf,NOW);
  sleep(1);
  theController->end();
}
//
void CCB::GenerateAlctAdbASync(){
  //
  std::cout << "CCB: GenerateAlctAdbPulseASync" << std::endl;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  if (mVersion==2001) {
    do_vme(VME_WRITE, 0x40, sndbuf,rcvbuf,NOW);
  }
  else{
    if (mDebug) std::cout << "Writing " << std::endl;
    //do_vme(VME_WRITE, 0x84, sndbuf,rcvbuf,NOW);
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    do_vme(VME_WRITE, 0x84, sndbuf,rcvbuf,NOW);
  }
  //
  theController->end();
  //
}
//
void CCB::GenerateAlctAdbSync(){
   //
   std::cout << "CCB: GenerateAlctAdbPulseSync" << std::endl;
   int i_ccb=0x19;
   sndbuf[0]=0x00;
   sndbuf[1]=(i_ccb<<2)&0xfc;
   do_vme(VME_WRITE, CSR2, sndbuf,rcvbuf,NOW);
   theController->end();
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
//
void CCB::rice_clk_setup()
{
  int l1en=0;
  std::cout << "CCB: Setup rice_clk_setup" << std::endl;

  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    if (mDebug) std::cout << "CCB: NOTE -- switching from DLOG to FPGA mode for setup" << std::endl; 
  }
  
  sndbuf[0]=0x00;
  sndbuf[1]=l1aDelay_;  
  if (mVersion==2001) 
    do_vme(VME_WRITE,CSR5,sndbuf,rcvbuf,LATER);
  if (mVersion==2004) 
    do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,LATER);
  
  if (switchedMode){
    setCCBMode(CCB::DLOG);
    if (mDebug) std::cout << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };
  
  return; // Ignore rest until needed MvdM

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  if (mVersion==2001)
    do_vme(VME_WRITE,RST_CCB_INT_LOG,sndbuf,rcvbuf,LATER);
  //else
  //std::cout << "CCB:  rice_clk_setup: RST_CCB_INT_LOG deprecated" << std::endl;


//JRG, For TMB with CCB clk: 0x0D8 or 0x0C8?  I think 0x048 is best.
  /*  sndbuf[0]=0x00;
  sndbuf[1]=0xd8;
  sndbuf[0]=0x00;
  sndbuf[1]=0xc8; */
//JRG, Ext. clk/L1A via CCB-FP: 0x16c
  sndbuf[0]=0x01;
  sndbuf[1]=0x6c;
//JRG, Use Normal CCB clk: 0x068
  sndbuf[0]=0xfe;  // Entirely disable CCB front panel inputs
  // sndbuf[1]=0x68;
  sndbuf[1]=0x40;    // unmask all CCB L1 sources
  sndbuf[1]=0xc8;    // CCB clock, TMB_bkpln_req  and  VME_L1  control


  std::cout << "TTC " << TTC << " CLK_INIT_FLAG " << CLK_INIT_FLAG << " l1en " << l1en << std::endl;
  std::cout << "SPS " << SPS25ns_ << std::endl;
  //std::cout << "TEMPORARILY DEMANDING TTC = 1 " << std::endl;
  //TTC = 1;
  if (TTC==TTC_CLOCK)sndbuf[1]=0xf2; // TTC clock only, CMD_BUS control via VME
  if (TTC==ALL_TTC)sndbuf[1]=0xf3;    // TTC clock and control

/*
  TTC=1/2 actually allows TMB_L1Req, VME and TTCrx L1 sources!
  Pawloski:  To REALLY disable TTCrx L1A set bit 0x08 high.
             To disable TMB_L1Req set bit 0x20 high.
*/
  if(CLK_INIT_FLAG==1){
    do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
    //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

    if((rcvbuf[1]&0xf8)!=0xf8) l1en=1;
// only load the new trigger setting NOW if triggers are enabled;
//    otherwise, change the SAV register in software, and the
//    new setting will be loaded when  rice_l1_enable  is called.
//      This applies to TTC=0 and TTC=2 case (L1 trigger change).

    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=((rcvbuf[1]&0xf8)|(sndbuf[1]&0x07));
    if(TTC==ALL_TTC){
      sndbuf[1]=(sndbuf[1]&0xf7);
      CCB_CSR1_SAV=(CCB_CSR1_SAV&0xf7);
    }

    if(TTC==NO_TTC){
      sndbuf[1]=(sndbuf[1]|0x08);
      CCB_CSR1_SAV=(CCB_CSR1_SAV|0x08);
    }

    if((l1en>0)||(TTC==TTC_CLOCK)){
      //fg why???
      std::cout << std::endl;
    }
    else{
      std::cout << "CCB: effective after  rice_l1_enable  call." << std::endl;
      sndbuf[1]=sndbuf[1]|0xf8;
      //JRG: add CCB_CSR1_SAV=sndbuf[1]; ?
    }
  }
  std::cout << "CCB: CSRB1=0x" << std::hex 
       << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2) <<int(sndbuf[1]&0xff)
       << std::dec << std::endl;
  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,LATER);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,LATER);


  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  // JRG, phos4: do_vme(VME_WRITE,0x24,sndbuf,rcvbuf,NOW);
  /* JRG, phos4:  rice_i2c();
  ierr=rice_delay(11,0,0); // manual say initial write required
  for(a=0;a<4;a++){
    for(b=0;b<5;b++){
      ierr=rice_delay(11,a,b);
    }
  }*/

  // XIT:
  CLK_INIT_FLAG=1;
  theController->end();

}

std::bitset<8> CCB::ReadTTCrxReg(const unsigned short registerAdd){
  //
  if (TTCrxID_ == -1) {
    std::cout << "ReadTTCrxReg.No TTCrxID" << std::endl;
    return 0;
  }
  //
  std::bitset<7> pointerRegAddress(TTCrxID_*2);
  std::bitset<7> DataRegAddress(TTCrxID_*2+1);
  std::bitset<8> regAddress(registerAdd);
  //
  //std::cout << " " << pointerRegAddress << " " << DataRegAddress << " " << regAddress << std::endl ;
  //
  // start I2C
  //
  startI2C();
  //
  do_vme(VME_READ,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // write address of the pointer  register
  for( int i(6); i>=0; --i) {
    writeI2C(pointerRegAddress[i]);
  }
  //
  writeI2C(0); // write bit 0 (write) = 0
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  for( int i(7); i>=0; --i) {
    writeI2C(regAddress[i]);
  }
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x06; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  stopI2C();
  //
  // start condition
  startI2C();
  //
  //
  // write address of the data  register (pointer + 1)
  for( int i(6); i>=0; --i) {
    writeI2C(DataRegAddress[i]);
  }
  //
  writeI2C(1); // write bit 0 (read) = 1
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // read data
  int Data;
  std::bitset<8> data;
  for(int i(7); i>=0; --i) {
    Data = readI2C();    
    //std::cout << Data ;
    data.set(i,Data&0x1);
  }      
  //std::cout << std::endl;
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04 ; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //  
  // stop condition
  stopI2C();
  //
  return data;
  //
}
void CCB::HardResetTTCrx(){
  //
  // Go to FPGA mode
  //
  setCCBMode(CCB::VMEFPGA);
  //
  // Hard reset TTCrx....
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,TTCrxReset,sndbuf,rcvbuf,NOW);
  //
}

void CCB::ReadTTCrxID(){
  //
  // Go to FPGA mode
  //
  setCCBMode(CCB::VMEFPGA);
  //
  // Hardreset TTCrx
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,TTCrxReset,sndbuf,rcvbuf,NOW);
  //
  // Wait some time
  usleep(60);
  //
  // Read TTCrx ID number
  do_vme(VME_READ,CSRB18,sndbuf,rcvbuf,NOW);
  //
  printf("%02x%02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
}
//
int CCB::readI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0c; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
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
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cHighInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]= (i2cBaseData&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::startI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[1]=0x00;
  sndbuf[0]=0x02 ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::stopI2C(){
  //
  sndbuf[1]=0x00;
  sndbuf[0]=0x02;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::enableCLCT(){
   //
   std::cout << "CCB.enableCLCT" <<std::endl;
   //
   if (mVersion==2001) {
      //
      sndbuf[0]=0xff; 
      sndbuf[1]=0xd0;
      do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
      //
   }
   //
   if (mVersion==2004) {
      //
      sndbuf[0]=0xdf; 
      sndbuf[1]=0x10;
      //
      do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
      //
   }
   //
}

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
  printf(" CCB.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}


void CCB::enableL1() {
  std::cout << "CCB: Enable L1A." << std::endl;
  do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  if (mVersion==2001){
    sndbuf[0]=(rcvbuf[0]&0xDF); // JRG: //11011111    GUJH  Mar. 19, 2003
    //  sndbuf[0]=0xdf;  //11011111    GUJH  Mar. 19, 2003
    sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF8);
  } else{
    //sndbuf[0]=(rcvbuf[0]&0xFE);
    sndbuf[0]=(rcvbuf[0]&0xDF);
    //sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF8);
    sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF0);
  }

  if (mDebug) std::cout << "CCB: CSRB1=0x" << std::hex 
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2) 
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;

  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = true;

  //enableCLCT();

}


void CCB::disableL1() {
  std::cout << "CCB: Disable L1A." << std::endl;
  do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  sndbuf[0]=rcvbuf[0];
  CCB_CSR1_SAV=rcvbuf[1];
  sndbuf[1]=(rcvbuf[1]|0xf8); // Disable L1ACC from CCB to custom backplane

  //std::cout << "disableL1.CCB: CSRB1=0x" << hex 
  //     << setw(2) << int(sndbuf[0]&0xff) << setw(2) <<int(sndbuf[1]&0xff)
  //     << dec << std::endl;
  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = false;
}


void CCB::hardReset() {

  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes and only for the  prgall_bckpln function.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    std::cout << "CCB: NOTE -- switching from DLOG to FPGA mode for BackPlane HardReset" << std::endl; 
  }
  
  HardResetTTCrx();
  prgall_bckpln();  
  //fg note: these 10seconds are not necessary for new/old TMB
  //fg sleep(10);

  // sequence of 2nd hard reset with additional delays (Jianhui)
  if  (mVersion==2004){
    sleep(2);
    prgall_bckpln();
    sleep(1); // could go down to ~100msec (Jianhui)
  }

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    std::cout << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };

  reset_bckpln();
  //fg note: this 1second is not necessary
  sleep(1);
  syncReset();
  //fg note: *keep* this 1second!
  sleep(1);
  theController->end();
}


void CCB::syncReset() {
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;

  int i_ccb=0x03;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  theController->end();
} 


void CCB::bx0() {
  /// send ccb_bx0 on the backplane:
  std::cout << "CCB: BX-zero" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
/* JRG, old way, write to directly to VME adr 0x36, but this
      DOES NOT drive the ccb_cmd bus (no cmd_strobe):
  do_vme(VME_WRITE,0x36,sndbuf,rcvbuf,NOW);
*/

// JRG, best way, write to CSR2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
  int i_ccb=0x01;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  theController->end();
}


void CCB::bxr() {
  std::cout << "CCB: BX-reset" << std::endl;
  usleep(10000);

// set ccb_cmd bits (CSR2[7:2]), EvCntRes (CSR2[1]) and BcntRes (CSR2[0]):
  sndbuf[0]=0x00;
  sndbuf[1]=0xC8;
// send CSR2 setting on backplane:
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
}


void CCB::l1CathodeScint() {
/*
   idly is the delay in nanoseconds
   add and add2 are defined in table 9 of VME ccb manual
   for instance  DMB1 has add=0x20 add2=0x00
                 DMB4 has add=0x30 add2=0x02
*/

// Use CCB front panel L1 to generate L1ACC,
  //sndbuf[0]=0xff;  
  sndbuf[0]=0xdf; // disable all but bit 13 (L1AC enabled unconditionally)
  sndbuf[1]=0x70; // enable both TTCrx and FrontPannel L1A

  //JG, reads current CSR1 status:
  do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  std::cout << "CCB: (l1cathodescint) " << std::hex << (int)rcvbuf[0] << "-" <<  (int)rcvbuf[1] << std::dec << std::endl;

  // make sure TTCrx is/remains the source for commands (bit 0=0)
  // CCB2001: make sure CCB quarts osc. is selected (b1=b2=0)
  // CCB2004: bit1-2 are not used.
  CCB_CSR1_SAV=(sndbuf[1]&0xf8);


  if((rcvbuf[1]&0xf8)!=0xf8){    //skip CCB read, replace with (CCB_L1_EN>0)?
// only load the new trigger setting NOW if triggers are enabled;
//    otherwise, change the SAV register in software, and the
//    new setting will be loaded when  rice_l1_enable  is called.
    sndbuf[1]=rcvbuf[1];
  }
  else{
//JRG define sndbuf[1]=unchanged_L1_bits
    std::cout << "CCB:  effective after  rice_l1_enable  call."<< std::endl;
    //fg this operation doesn't seem to be very usefull:
    sndbuf[1]=rcvbuf[1]&0xff;
  }

  std::cout << "CCB: CSRB1=0x" << std::hex 
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2)
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;
  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);

// J. Gu tuned CCB L1 delay to allow for TCB-L1 passthrough (77 was ~78 or 79)
  std::cout << "CCB: set L1ADelay (CSRB5) to " << l1aDelay_ << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=l1aDelay_;
  do_vme(VME_WRITE,CSR5,sndbuf,rcvbuf,LATER);
  //2004 do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,LATER);
}


void CCB::reset_bxevt() {
  usleep(10000);

//  Send BCR, ECR and BC0  --GUJH   Mar. 20, 2003
  sndbuf[0]=0x00;
  sndbuf[1]=0x03;
// send CSR2 setting on backplane:
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
  theController->end();
}

void CCB::SelfTest() {
  //
  //
}

void CCB::init() {
  //
  //
}

void CCB::configure() {
  //
  if (mVersion==2004){
    // Set the CCB mode  
    setCCBMode((CCB2004Mode_t)mCCBMode);
    // report firmware version
    firmwareVersion();
  }
  else 
    std::cerr << "Error: Unknown CCB version ("<< mVersion << "). Unable to configure."<< std::endl;

  hardReset();
  rice_clk_setup();
  disableL1();
}
void CCB::SetL1aDelay(int l1adelay){
  //
  if (mVersion==2001) 
    do_vme(VME_READ,CSR5,sndbuf,rcvbuf,NOW);
  if (mVersion==2004) 
    do_vme(VME_READ,CSRB5,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=l1adelay;  
  if (mVersion==2001) 
    do_vme(VME_WRITE,CSR5,sndbuf,rcvbuf,NOW);
  if (mVersion==2004) 
    do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,NOW);
  //
}

void CCB::setCCBMode(CCB2004Mode_t mode){
  /// Set the mode of operation for the CCB2004 model
  switch (mode) {
  case TTCrqFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    break;
  case VMEFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    sndbuf[0]=0x00;
    sndbuf[1]=0x01;
    do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    break;
  case DLOG:
    sndbuf[0]=0x00;
    sndbuf[1]=0x01;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    break;
  default:
    std::cerr << "Warning: unkown CCB2004 operation mode. Using DLOG instead" << std::endl;
    sndbuf[0]=0x00;
    sndbuf[1]=0x01;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
  }
}

 
void CCB::enable() {
  std::cout << "Sync reset, mainly for TMB " << std::endl;
  if(l1enabled_) disableL1();
  syncReset();
  usleep(50000);

  std::cout << "BXR and EVTCNTRST" << std::endl;
  reset_bxevt();

  if (SPS25ns_==0) {
    startTrigger();
    usleep(10000);
    std::cout << "CCB BC0" << std::endl;

    bx0();
    usleep(10000);
  }

  if (mVersion==2001)
    enableTTCControl();
  else if (mVersion==2004)
    if (TTC==ALL_TTC)
      enableTTCControl();

  enableL1();
}


void CCB::enableTTCControl() {
  std::cout << "CCB: Enable TTC control " << std::endl;
  if (mVersion==2001) TTC = ALL_TTC;

  do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  usleep(50000);
  if (mVersion==2001){
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]|0x03);
  }
  else{  // version 2004 flipped the bit ..
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]&0xfe);
  }

  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
  std::cout << "CCB: CSRB1(read)=0x" << std::hex 
	    << std::setw(2) << int(rcvbuf[0]&0xff) << std::setw(2) 
	    <<int(rcvbuf[1]&0xff) << "changed to CSRB1(set)=0x"
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2)
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;
}


void CCB::startTrigger() {
  /// Send "Start Trigger" command on the Fast Control Bus
  std::cout << "CCB: Start Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x18;          //cmd[5:0]=0x06 for start_trigger     GUJH
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}

void CCB::bc0() {
  /// Send "bc0" command on the Fast Control Bus
  std::cout << "CCB: bc0 Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x4; 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);

}


void CCB::stopTrigger() {
  /// Send "Stop Trigger" command on the Fast Control Bus
  std::cout << "CCB: Stop Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x1C;          //cmd[5:0]=0x07 for stop_trigger     GUJH
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}


void CCB::disable() {
  std::cout << "CCB: disable" << std::endl;
  disableTTCControl();
  if (SPS25ns_==0) {
    //stop trigger
    stopTrigger();
  }
}


void CCB::disableTTCControl() {
  std::cout << "CCB: disable TTC control" << std::endl;
  do_vme(VME_READ,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
  if (mVersion==2001){
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]&0xfe);
    TTC=1;
  }else {// mVersion==2004
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]|0x01);
  }
  do_vme(VME_WRITE,CSR1,sndbuf,rcvbuf,NOW);
  //2004 do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(5000);
}


void CCB::executeCommand(std::string command) {
  if(command=="Program Backplane") prgall_bckpln();
  if(command=="Reset Backplane")   reset_bckpln();

  if(command=="Enable L1")        enableL1();
  if(command=="Disable L1")       disableL1();
  if(command=="L1 Cathode Scint") l1CathodeScint();

  if(command=="Hard Reset")       hardReset();
  if(command=="Sync Reset")       syncReset();
  if(command=="Reset BXEVT")      reset_bxevt();
  if(command=="BX0")              bx0();
  if(command=="BXR")              bxr();
}


void CCB::firmwareVersion(){
  /// report the firmware version
  do_vme(VME_READ,CSRB17,sndbuf,rcvbuf,NOW);  
  int versionWord = (rcvbuf[0]<<8) + (rcvbuf[1]&0xFF);
  int day   =  versionWord & 0x1F;
  int month = (versionWord >> 5   ) & 0xF;
  int year  = (versionWord >>(5+4)) + 2000;
  (*MyOutput_) << "CCB: firmware date: " 
       << day << "-" << month << "-" << year << std::endl;
}

#ifdef USEDCS
//fg NOTE: CSR1 (2001) vs CSRB1(2004) bit  incompatibilities ...
void CCB::cmd_source_to_ttcrx(){
// 2004
  sndbuf[0]=0x00; // ??????????
  sndbuf[1]=0;  // ??????????
  do_vme(VME_WRITE,0x20,sndbuf,rcvbuf,1);
}



void CCB::cmd_source_to_vme(){
/// this function should be executed before any *_csr2 function 
// 2004
  sndbuf[0]=0x00; // ??????????
  sndbuf[1]=1;  // ??????????
  do_vme(VME_WRITE,0x20,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_all_csr2(){
/// reload all fpga-s from eprom (CCB specification)
// Hard reset ALL:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x34, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x34,sndbuf,rcvbuf,1);
//
// JRG, best way, write to CSR2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:

// 2004
  int i_ccb=0x04;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/12/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 4
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::sync_reset_csr2(){
/// reset L1 readout buffers and resynchronize optical links (CCB specification) ???

// Generate ccb_L1_Reset (sync reset):
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// 2004
  int i_ccb=0x03;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 3
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}
//
void CCB::soft_reset_all_csr2(){
/// initialize the FPGA-s on dmb,tmb,mpc (CCB specification)

// Soft reset ALL:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x3c, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x3c,sndbuf,rcvbuf,1);
//
//
// JRG, best way, write to CSR2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
// 2004
  int i_ccb=0x1c;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 1c
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::soft_reset_dmb_csr2(){
/// initialize the FPGA-s on dmb (CCB specification)

// Soft reset DMBs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x6a, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x6a,sndbuf,rcvbuf,1);
//
//
// JRG, best way, write to CSR2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:

// 2004
  int i_ccb=0x1d;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 1d
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::soft_reset_tmb_csr2(){
/// initialize the FPGA-s on tmb (CCB specification)

// Soft reset TMBs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x7e, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x7e,sndbuf,rcvbuf,1);
//
//
// JRG, best way, write to CSR2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
// 2004
  int i_ccb=0x1e;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 1e
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::soft_reset_mpc_csr2(){
/// initialize the FPGA-s on mpc (CCB specification)

// Soft reset MPCs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;

// 2004
  int i_ccb=0x1f;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 1f
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}




void CCB::hard_reset_dmb_csr2(){
/// initialize the FPGA-s on dmb (CCB specification)
// Hard reset DMBs:
// 2004
  int i_ccb=0x12;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 12
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_tmb_csr2(){
/// initialize the FPGA on TMB (CCB specification)
// Hard reset TMBs:
// 2004
  int i_ccb=0x10;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 10
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_alct_csr2(){
/// initialize the FPGA-s on ALCT (CCB specification)
// Hard reset ALCT:
// 2004
  int i_ccb=0x11;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 11
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_ccb_csr2(){
/// initialize the FPGA-s on CCB (CCB specification)
// Hard reset CCB:
// 2004
  int i_ccb=0x0f;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; //
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_mpc_csr2(){
/// initialize the FPGA on MPC (CCB specification)
// Hard reset MPC:
// 2004
  int i_ccb=0x13;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSR2[7...2]) ; command code = 13
  //do_vme(VME_WRITE,0x22,sndbuf,rcvbuf,1);
  do_vme(VME_WRITE,CSR2,sndbuf,rcvbuf,1);
}



void CCB::hard_reset_alct(){
/// Hard ALCT reset: programming
/// Generate ALCT "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x01; 
// Hard ALCT reset:
// 2004 
  do_vme(VME_WRITE,0x66,sndbuf,rcvbuf,1); // correct: 11/13/2003 base+0x30  data to write : anything
}



void CCB::hard_reset_dmb(){
/// Hard DMB reset: programming
/// Generate DMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x74; // 0x00;
// Hard DMB reset:
// 2004
  do_vme(VME_WRITE,0x64,sndbuf,rcvbuf,1);  // this is the correct DMB Hard Reset // correct: 11/13/2003 base+0x2e  data to write : anything
}



void CCB::hard_reset_tmb(){
/// Hard TMB reset: programming
/// Generate TMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard TMB reset:
// 2004
  do_vme(VME_WRITE,0x62,sndbuf,rcvbuf,1); // correct: 11/13/2003 base+0x2c  data to write : anything
}



void CCB::hard_reset_mpc(){
/// Hard MPC reset: programming
/// Generate MPC "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard MPC reset:
// 2004
  do_vme(VME_WRITE,0x68,sndbuf,rcvbuf,1); // correct: 11/13/2003 base+0x32  data to write : anything
}



void CCB::hard_reset_all(){
/// Generate "Hard Reset" 400ns pulse to all modules in crate
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  // 2004
  do_vme(VME_WRITE,0x60,sndbuf,rcvbuf,1);
}



void CCB::soft_reset_dmb(){
/// Generate "Soft Reset" 25 ns pulse to DMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,1);  //  base+0x6a
}



void CCB::soft_reset_tmb(){
/// Generate "Soft Reset" 25 ns pulse to TMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6c,sndbuf,rcvbuf,1); // base+0x7e
}



void CCB::soft_reset_mpc(){
/// Generate "Soft Reset" 25 ns pulse to MPC
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x70,sndbuf,rcvbuf,1); // base+0x64
}



void CCB::soft_reset_all(){
/// Generate "Soft Reset" 25 ns pulse to all modules in crate!!!
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6a,sndbuf,rcvbuf,1); // base+0x3c
}



//======= test stuff below ===================
int CCB::set_la1_delay(int delay){
  sndbuf[0]=0x00;
  sndbuf[1]=(char)delay;
  do_vme(VME_WRITE,0x08,sndbuf,rcvbuf,0);
  return 1;
}

int CCB::get_la1_delay(){
  do_vme(VME_READ,0x08,sndbuf,rcvbuf,0);
  std::cout << "rcvbuf " << std::hex << std::setw(2) 
	    << int(rcvbuf[0]&0xff) << " " << int(rcvbuf[1]&0xff)
	    << std::dec << std::endl;
  return 1;
}
#endif
