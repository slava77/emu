//-----------------------------------------------------------------------
// $Id: CCB.h,v 3.12 2008/06/12 21:08:54 rakness Exp $
// $Log: CCB.h,v $
// Revision 3.12  2008/06/12 21:08:54  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.11  2008/04/11 14:48:47  liu
// add CheckConfig() function
//
// Revision 3.10  2007/08/27 22:50:24  liu
// update
//
// Revision 3.9  2007/04/02 17:31:23  liu
// fix setCCBMode problem
//
// Revision 3.8  2007/03/16 18:06:22  rakness
// put scans+TTCrxID in summary file
//
// Revision 3.7  2007/03/14 11:59:44  rakness
// extract TTCrxID
//
// Revision 3.6  2006/10/30 15:53:19  mey
// Update
//
// Revision 3.5  2006/09/27 16:44:36  mey
// UPdate
//
// Revision 3.4  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.3  2006/08/07 13:31:29  mey
// Added TTCrx coarse delay
//
// Revision 3.2  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.1  2006/08/03 18:59:13  mey
// Got rid of version number
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.25  2006/07/20 14:03:11  mey
// Update
//
// Revision 2.24  2006/07/16 04:14:12  liu
// remove CCB2001 support, code cleanup
//
// Revision 2.23  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.22  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.21  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.20  2006/05/16 15:54:37  mey
// UPdate
//
// Revision 2.19  2006/04/28 13:41:16  mey
// Update
//
// Revision 2.18  2006/03/10 10:42:22  mey
// Left DCS message
//
// Revision 2.17  2006/03/10 10:13:53  mey
// Got rid of DCS
//
// Revision 2.16  2006/02/02 14:27:31  mey
// Update
//
// Revision 2.15  2006/01/13 10:06:44  mey
// Got rid of virtuals
//
// Revision 2.14  2006/01/12 23:43:39  mey
// Update
//
// Revision 2.13  2006/01/12 12:27:38  mey
// UPdate
//
// Revision 2.12  2006/01/12 11:32:02  mey
// Update
//
// Revision 2.11  2006/01/11 13:25:17  mey
// Update
//
// Revision 2.10  2005/12/16 17:50:08  mey
// Update
//
// Revision 2.9  2005/11/25 23:41:39  mey
// Update
//
// Revision 2.8  2005/11/15 15:37:22  mey
// Update
//
// Revision 2.7  2005/11/07 16:23:24  mey
// Update
//
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

class Crate;

class CCB: public VMEModule
{
public:
  void SetL1aDelay(int);
  int  ReadRegister(int);
  void WriteRegister(int,int);
  void firmwareVersion();
  void printFirmwareVersion();
  void HardResetTTCrx();
  CCB(Crate * ,int slot);
  virtual ~CCB();

  /// VMEModule type identifier
  enum CCB2004Mode_t {TTCrqFPGA=0, VMEFPGA=1, DLOG=2};  
  void setCCBMode(CCB2004Mode_t);
  unsigned int boardType() const {return VMEModule::CCB_ENUM;}

  // start routine from VMEModule OK
  void end();

  void pulse(int num_pulse,unsigned int pulse_delay, char vme);
  // these two call pulse for different VME addresses
  void pulse(int num_pulse=1,unsigned int pulse_delay=0xff);
  void inject(int num_pulse,unsigned int pulse_delay);
  void pedestal(int num_pulse,unsigned int pulse_delay);
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void HardReset_crate();
  void SoftReset_crate();
  //
  bool SelfTest() ;
  void init() ;
  void configure() ;
  //
  bool CheckFirmwareDate();
  //
  void FireCCBMpcInjector();
  //
  inline void SetTTCmode(int mode)   {TTC       = mode;}
  inline int  GetTTCmode()   {return TTC;}
  //
  inline void SetCCBmode(int mode)   {mCCBMode  = mode;}
  inline int  GetCCBmode()   {return mCCBMode;}
  //
  inline void SetBxOrbit(int orbit)  {BX_Orbit_ = orbit;}
  inline int  GetBxOrbit()  { return BX_Orbit_ ;}
  //
  inline void SetSPS25ns(int delay)  {SPS25ns_  = delay;}
  inline int  GetSPS25ns()           {return SPS25ns_;}
  //
  inline void Setl1adelay(int delay) {l1aDelay_ = delay;}
  inline int  Getl1adelay() {return l1aDelay_;}
  //
  inline void SetTTCrxID(int ID)     {TTCrxID_  = ID;}
  inline int  GetTTCrxID(){return TTCrxID_;}
  //
  inline void SetReadTTCrxID(int ID) { ReadTTCrxID_ = ID;}
  inline int  GetReadTTCrxID() { return ReadTTCrxID_;}
  //
  inline void SetTTCrxCoarseDelay(int delay) {TTCrxCoarseDelay_  = delay;}
  inline int  GetTTCrxCoarseDelay(){return TTCrxCoarseDelay_;}
  //
  inline void SetTTCrxFineDelay(int delay)     {TTCrxFineDelay_  = delay;}
  //
  inline void SetExpectedFirmwareDay(int day) {expected_firmware_day_ = day;}
  inline int  GetExpectedFirmwareDay() { return expected_firmware_day_;}
  //
  inline void SetExpectedFirmwareMonth(int month) {expected_firmware_month_ = month;}
  inline int  GetExpectedFirmwareMonth() { return expected_firmware_month_;}
  //
  inline void SetExpectedFirmwareYear(int year) {expected_firmware_year_ = year;}
  inline int  GetExpectedFirmwareYear() { return expected_firmware_year_;}
  //
  void PrintTTCrxRegs();
  bool l1Enabled() const {return l1enabled_;}
  void enableL1();
  void disableL1();
  void l1CathodeScint();
  void ccb_vme(char fcn, char vme,const char *snd,char *rcv, int wrt);
  void hardReset();
  void syncReset();
  void reset_bxevt();
  void bx0();
  void bc0();
  void CLCTexternalTrigger();
  void bxr();
  //
  /// uses SPS25ns to decide whether to use TTC triggers
  //
  void enable();
  void enableTTCControl();
  void disableTTCControl();
  void GenerateAlctAdbASync();
  void GenerateAlctAdbSync();
  void startTrigger();
  void stopTrigger();      
  void disable();
  void l1aReset();
  void injectTMBPattern();
  //
  void CCBStartTrigger();
  void ReadTTCrxID();
  std::bitset<8> ReadTTCrxReg(const unsigned short);
  void WriteTTCrxReg(const unsigned short,int);
  int  readI2C();
  void startI2C();
  void stopI2C();
  void writeI2C(int);
  int CheckConfig();
  //
  /// mostly for GUIs
  void executeCommand(std::string command);
  //
  void start();
  //
  int TTC; 
  int CLK_INIT_FLAG; 
  int CCB_CSR1_SAV;
  
  //code used by DCS
  
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

  void EnableL1aCounter();
  void ResetL1aCounter();
  int  ReadL1aCounter();

  void soft_reset_dmb();
  void soft_reset_tmb();
  void soft_reset_mpc();
  void soft_reset_all();
  void l1a_and_trig();
  
  void DumpAddress(int);

  friend std::ostream & operator<<(std::ostream & os, CCB & ccb);

  //code used by DCS

protected:
  int mCCBMode;
  enum TTCMode {NO_TTC=0, TTC_CLOCK=1, ALL_TTC=2};

private:
  std::ostream * MyOutput_ ;
  //
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
  static const unsigned int L1Reset    = 0x50;
  static const unsigned int TTCrxReset = 0x5c;
  //
  static const unsigned int readL1aCounterLSB  = 0x90;
  static const unsigned int readL1aCounterMSB  = 0x92;
  static const unsigned int resetL1aCounter    = 0x94;
  static const unsigned int enableL1aCounter   = 0x96;
  //
  static const unsigned int CRATE_HARD_RESET = 0x60;
  static const unsigned int DMB_CFEB_CAL0 = 0x8a;
  static const unsigned int DMB_CFEB_CAL1 = 0x8c;
  static const unsigned int DMB_CFEB_CAL2 = 0x8e;
  // maybe these should be static, common to all CCBs?
  // I think this one really belongs in the DDU section...
  int BX_Orbit_;
  int SPS25ns_;
  int l1aDelay_;
  //
  int TTCrxID_;
  int ReadTTCrxID_;
  int TTCrxCoarseDelay_;
  int TTCrxFineDelay_;
  //
  bool l1enabled_;
  int mVersion; // CCB version number (2001,2004)
  bool mDebug;
  //
  int read_firmware_day_;
  int read_firmware_month_; 
  int read_firmware_year_;
  //
  int expected_firmware_day_;
  int expected_firmware_month_; 
  int expected_firmware_year_;
};

#endif

