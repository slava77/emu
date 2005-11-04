//-----------------------------------------------------------------------
// $Id: CCB.h,v 2.6 2005/11/04 10:26:13 mey Exp $
// $Log: CCB.h,v $
// Revision 2.6  2005/11/04 10:26:13  mey
// Update
//
// Revision 2.5  2005/11/03 18:24:38  mey
// I2C routines
//
// Revision 2.4  2005/09/28 17:00:05  mey
// Update
//
// Revision 2.3  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.2  2005/07/08 10:29:34  geurts
// introduce debug switch to hide debugging messages
//
// Revision 2.1  2005/06/06 11:10:53  geurts
// default power-up mode DLOG. updated for calibration code.
// direct read/write access to registers
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#ifndef CCB_h
#define CCB_h

#include "VMEModule.h"
#include <string>
#include <bitset>

class CCBParser;


class CCB: public VMEModule
{
public:
  void SetL1aDelay(int);
  int ReadRegister(int);
  void WriteRegister(int,int);
  void firmwareVersion();
  CCB(int newcrate ,int slot, int version = 2004 );
  virtual ~CCB();

  /// VMEModule type identifier
  enum CCB2004Mode_t {TTCrqFPGA=0, VMEFPGA=1, DLOG=2};  
  void setCCBMode(CCB2004Mode_t);
  virtual unsigned int boardType() const {return VMEModule::CCB_ENUM;}

  // start routine from VMEModule OK
  virtual void end();


  virtual void pulse(int num_pulse,unsigned int pulse_delay, char vme);
  // these two call pulse for different VME addresses
  virtual void pulse(int num_pulse,unsigned int pulse_delay);
  virtual void inject(int num_pulse,unsigned int pulse_delay);
  void pulse(int Num_pulse,unsigned int * delays, char vme);
  void pulse();

  virtual void prgall_bckpln();
  virtual void reset_bckpln();

  bool l1Enabled() const {return l1enabled_;}
  void enableL1();
  void disableL1();
  void l1CathodeScint();

  void hardReset();
  void syncReset();
  void reset_bxevt();
  void bx0();
  void bc0();
  void bxr();
  void configure();
  /// uses SPS25ns to decide whether to use TTC triggers
  void enable();
  void enableTTCControl();
  void disableTTCControl();
  void enableCLCT();
  void GenerateAlctAdbASync();
  void GenerateAlctAdbSync();
  void startTrigger();
  void stopTrigger();      
  void disable();
  //
  void ReadTTCrxID();
  std::bitset<8> ReadTTCrxReg(const unsigned short);
  int  readI2C();
  void  startI2C();
  void  stopI2C();
  void writeI2C(int);
  //
  /// mostly for GUIs
  void executeCommand(std::string command);
  friend class CCBParser;

  int TTC; 
  int CLK_INIT_FLAG; 
  int CCB_CSR1_SAV;
  
  
#ifdef USEDCS
  void cmd_source_to_ttcrx();
  void cmd_source_to_vme();
  void sync_reset_csr2();
  void soft_reset_all_csr2();      //+
  void soft_reset_dmb_csr2();      //+
  void soft_reset_tmb_csr2();      //+
  void soft_reset_mpc_csr2();      //+ not used

  void hard_reset_all_csr2(); //+
  void hard_reset_alct_csr2();    //
  void hard_reset_dmb_csr2();     //
  void hard_reset_tmb_csr2();     //
  void hard_reset_mpc_csr2();    //
  void hard_reset_ccb_csr2();    // new function of ccb 2004

  void hard_reset_all();
  void hard_reset_alct();    //+
  void hard_reset_dmb();     //+
  void hard_reset_tmb();     //+
  void hard_reset_mpc();     //+


  void soft_reset_dmb();
  void soft_reset_tmb();
  void soft_reset_mpc();
  void soft_reset_all();

  int set_la1_delay(int delay);  // test stuff
  int get_la1_delay(); // test stuff
#endif

  void DumpAddress(int);


protected:
  int mCCBMode;
  enum TTCMode {NO_TTC=0, TTC_CLOCK=1, ALL_TTC=2};

  void rice_clk_setup();
  
private:
  //-- Control and Status Registers for CCB2004
  //   group A: discrete logic
  static const unsigned int CSRA1  = 0x00;
  static const unsigned int CSRA2  = 0x02;
  static const unsigned int CSRA3  = 0x04;
  //   group B: Xilinx FPGA
  static const unsigned int CSRB1  = 0x20;
  static const unsigned int CSRB2  = 0x22;
  static const unsigned int CSRB3  = 0x24;
  static const unsigned int CSRB4  = 0x26;
  static const unsigned int CSRB5  = 0x28;
  static const unsigned int CSRB6  = 0x2a;
  static const unsigned int CSRB7  = 0x2c;
  static const unsigned int CSRB8  = 0x2e;
  static const unsigned int CSRB9  = 0x30;
  static const unsigned int CSRB10 = 0x32;
  static const unsigned int CSRB11 = 0x34;
  static const unsigned int CSRB12 = 0x36;
  static const unsigned int CSRB13 = 0x38;
  static const unsigned int CSRB14 = 0x3a;
  static const unsigned int CSRB15 = 0x3c;
  static const unsigned int CSRB16 = 0x3e;
  static const unsigned int CSRB17 = 0x40;
  static const unsigned int CSRB18  = 0x42;
  //
  static const unsigned int L1Reset = 0x50;
  static const unsigned int TTCrxReset = 0x5c;

  // maybe these should be static, common to all CCBs?
  // I think this one really belongs in the DDU section...
  int BX_Orbit_;
  int SPS25ns_;
  int l1aDelay_;

  //
  unsigned int CSR1;
  unsigned int CSR2;
  unsigned int CSR5;
  unsigned int RST_CCB_INT_LOG;
  unsigned int CRATE_HARD_RESET;
  unsigned int DMB_CFEB_CAL0;
  unsigned int DMB_CFEB_CAL1;
  unsigned int DMB_CFEB_CAL2;
  int TTCrxID_;

  bool l1enabled_;
  int mVersion; // CCB version number (2001,2004)
  bool mDebug;

};

#endif

