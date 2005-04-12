//-----------------------------------------------------------------------
// $Id: CCB.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: CCB.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.21  2004/08/10 18:22:05  tfcvs
// Update
//
// Revision 1.20  2004/08/05 04:19:17  tfcvs
// Update
//
// Revision 1.19  2004/07/20 22:20:33  tfcvs
// - Removed a bug in configure() which prevented hardReset from being called
//   for any non-DLOG mode;
// - Renamed configure(CCB2004Mode_t) to setCCBMode(CCB2004Mode_t) to prevent
//   any confusion
// - setCCBMode and rice_clk_setup() are moved away from public to protected
//
// Revision 1.18  2004/06/05 19:47:05  tfcvs
// Reorganized and debugged TTC control mode for CCB2004 (and 2001).
// CCB2004 mode configurable through XML option: <CCB CCBmode=""> (FG)
//
// Revision 1.17  2004/05/20 17:01:16  tfcvs
// solved a bug, uncovered another one ...
//  - fixed enableL1
//  - initialized l1enabled_
//  - moved all printf to cout and identify class in log message
//
// Revision 1.16  2004/05/18 09:44:14  tfcvs
// added DMB_CFEB_CAL2 address pointer (ccb01/04) and CVS keywords (FG)
//
//-----------------------------------------------------------------------
#ifndef CCB_h
#define CCB_h

#include "VMEModule.h"
#include <string>

class CCBParser;


class CCB: public VMEModule
{
public:
  void firmwareVersion();
  CCB(int newcrate ,int slot, int version = 2001 );
  virtual ~CCB();

  /// VMEModule type identifier
  enum CCB2004Mode_t {TTCrqFPGA=0, VMEFPGA=1, DLOG=2};  
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
  /// mostly for GUIs
  void executeCommand(std::string command);
  friend class CCBParser;

  int TTC; 
  int CLK_INIT_FLAG; 
  int CCB_CSR1_SAV;

  // maybe these should be static, common to all CCBs?
  /// I think this one really belongs in the DDU section...
  int BX_Orbit_;
  int SPS25ns_;
  int l1aDelay_;

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

protected:
  int mCCBMode;
  void setCCBMode(CCB2004Mode_t);
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

  unsigned int CSR1;
  unsigned int CSR2;
  unsigned int CSR5;
  unsigned int RST_CCB_INT_LOG;
  unsigned int CRATE_HARD_RESET;
  unsigned int DMB_CFEB_CAL0;
  unsigned int DMB_CFEB_CAL1;
  unsigned int DMB_CFEB_CAL2;

  bool l1enabled_;
  int mVersion; // CCB version number (2001,2004)


};

#endif

