//-----------------------------------------------------------------------
// $Id: TMB.h,v 2.30 2006/03/17 15:51:05 mey Exp $
// $Log: TMB.h,v $
// Revision 2.30  2006/03/17 15:51:05  mey
// New routines
//
// Revision 2.29  2006/03/17 13:57:39  rakness
// Add friend EMUjtag
//
// Revision 2.28  2006/03/05 18:45:08  mey
// Update
//
// Revision 2.27  2006/03/03 07:59:19  mey
// Update
//
// Revision 2.26  2006/02/07 22:49:25  mey
// UPdate
//
// Revision 2.25  2006/02/02 15:34:23  mey
// Update
//
// Revision 2.24  2006/02/02 14:27:31  mey
// Update
//
// Revision 2.23  2006/02/01 18:31:49  mey
// Update
//
// Revision 2.22  2006/01/31 14:42:00  mey
// Update
//
// Revision 2.21  2006/01/23 13:56:52  mey
// Update using Greg's new code
//
// Revision 2.20  2006/01/19 10:03:46  mey
// Update
//
// Revision 2.19  2006/01/18 20:17:49  mey
// Update
//
// Revision 2.18  2006/01/18 12:45:50  mey
// Cleaned up old code
//
// Revision 2.17  2006/01/13 10:07:09  mey
// Got rid of virtuals
//
// Revision 2.16  2006/01/12 23:44:04  mey
// Update
//
// Revision 2.15  2006/01/12 22:35:57  mey
// UPdate
//
// Revision 2.14  2006/01/12 12:28:05  mey
// UPdate
//
// Revision 2.13  2006/01/12 11:32:08  mey
// Update
//
// Revision 2.12  2006/01/11 13:26:26  mey
// Update
//
// Revision 2.11  2005/11/21 17:38:52  mey
// Update
//
// Revision 2.10  2005/10/21 17:13:48  mey
// Update
//
// Revision 2.9  2005/10/05 14:24:29  mey
// Added tests
//
// Revision 2.8  2005/09/28 16:52:51  mey
// Include Output streamer
//
// Revision 2.7  2005/09/06 12:12:12  mey
// Added accessors
//
// Revision 2.6  2005/08/22 07:55:44  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.5  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.4  2005/08/12 14:16:03  mey
// Added pulsing vor TMB-MPC delay
//
// Revision 2.2  2005/07/08 10:33:31  geurts
// allow arbitrary scope trigger channel in TMB::scope()
//
// Revision 2.1  2005/06/06 15:17:17  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef TMB_h
#define TMB_h

#include "VMEModule.h"
#include <cstdio>
#include <vector>
#include <string>
#include <bitset>
#include "AnodeChannel.h"
#include "TMB_JTAG_constants.h"
#include "RAT.h"
#include "EMUjtag.h"

class ALCTController;
class TMBParser;
class RAT;
class EMUjtag;

class TMB : public VMEModule {

public:
  friend class ALCTController;
  friend class TMBParser;
  friend class EMUjtag;
      
  TMB(int newcrate, int slot);
  virtual ~TMB();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  /// from the BOARDTYPE enum
  unsigned int boardType() const {return TMB_ENUM;}
  /// ucla_start was always called with a dev and a slot
  void start();
  void end();
  /// ALCTs need to go to lower scan level, whatever that means
  void start(int idev);
  /// does start(1)
  void tmb_vme(char fcn, char vme, const char *snd,char *rcv, int wrt);      
  int  GetWordCount();
  int  GetALCTWordCount();
  void StartTTC();
  int  ReadRegister(int);
  void DumpRegister(int);
  void WriteRegister(int,int);
  void DecodeCLCT();
  void DecodeALCT();
  void DumpAddress(int);
  //void lctrequestdelay(int dword);
  void clear_i2c();
  void clk_delays(unsigned short int time,int cfeb_id);
  void scope(int scp_arm,int scp_readout, int scp_channel=0x1d);
  void decode();
  void ALCTRawhits();
  void TMBRawhits();
  void ForceScopeTrigger();
  void OnlyReadTMBRawhits();
  void fifomode();
  void init_alct(int choice);
  void load_cscid();
  void DiStripHCMask(int);
  void PrintCounters(int counter=-1);
  // should have bx window coded in.  See trgmode.
  void lvl1_delay(unsigned short int time);
  void alct_vpf_delay(unsigned short int time);
  void mpc_delay(unsigned short int time);
  void optimize();
  void read_delays();
  void reset();
  //void scan_rx_clock();
  void trgmode(int choice);
  void ResetCounters();
  void SetALCTPatternTrigger();
  void SetCLCTPatternTrigger();
  int  FirmwareDate();
  int  FirmwareYear();
  int  FirmwareVersion();
  int  FirmwareRevCode();
  int  PowerComparator();
  /// called by TRGMODE, depending on version_
  void setupNewDelayChips();
  void setupOldDelayChips();
  void trgmode_bprsq_alct();
  void trgmode_bprsq_clct(int choice);
  void activecfeb();
  void toggle_l1req();
  void trgmode_bprsq_dmb();
  void firmwareVersion();
  void setLogicAnalyzerToDataStream(bool yesorno);
  //void tmb_PHOS4_alct(int time=0);
  //void tmb_PHOS4_cfeb();
  void EnableCLCTInputs(int CLCInputs );
  void EnableL1aRequest();
  void DisableCLCTInputs();
  void DisableALCTInputs();
  void DisableALCTCLCTSync();
  void DisableExternalCCB();
  void EnableInternalL1aEmulator();
  void DisableInternalL1aSequencer();
  void tmb_clk_delays(unsigned short int time, int cfeb_id);
  //
  int  CCB_command_from_TTC();
  void DataSendMPC();
  void InjectMPCData(const int nEvents, const unsigned long lct0, const unsigned long lct1);
  //
  void ExtClctTrigFromCCBonly();
  int ReadTMBtempPCB();  
  int ReadTMBtempFPGA();  
  int ReadTMBtCritPCB();  
  int ReadTMBtCritFPGA();  
  int smb_io(int,int,int);
  //
  void ResetRAMAddress();
  void ResetALCTRAMAddress();
  int  GetCounter(int);
  void GetCounters();
  void FireALCTInjector();
  void FireCLCTInjector();
  void ClearALCTInjector();
  void ClearCLCTInjector();
  void ClearScintillatorVeto();
  int  TestArray();
  int  TMBCRCcalc(std::vector< std::bitset<16> >& datain );
  std::bitset<22> calCRC22(const std::vector< std::bitset<16> >& datain);
  std::bitset<22> nextCRC22_D16(const std::bitset<16>& D, const std::bitset<22>& C);
  //
  void select_jtag_chain_param();
  //
  void jtag_ir_dr(int,int,int*,int,int*);
  void jtag_io_byte(int,int*,int*,int* );
  int bits_to_int(int*,int,int);
  void step(int,int,int,int,int);
  //
  void SetALCTController(ALCTController* a) {alctController_=a;}
  ALCTController * alctController() const {return alctController_;}
  RAT * getRAT() const {return rat_;}
  //      
  int tmb_get_id(struct tmb_id_regs* tmb_id);
  int tmb_set_jtag_src(unsigned short int jtag_src);
  int tmb_get_jtag_src(unsigned short int* jtag_src);
  int tmb_set_jtag_chain(unsigned int jchain);
  void set_jtag_chain(int);
  void jtag_anystate_to_rti();
  int tmb_set_reg(unsigned int vmereg, unsigned short int value );
  int tmb_get_reg(unsigned int vmereg, unsigned short int* value );
  int tmb_vme_reg(unsigned int vmereg, unsigned short int* value);
  int tmb_jtag_io(unsigned char tms, unsigned char tdi, unsigned char* tdo);
  int tmb_get_boot_reg(unsigned short int* value);
  int tmb_set_boot_reg(unsigned short int value);
  int tmb_hard_reset_alct_fpga();
  int tmb_hard_reset_tmb_fpga();
  int tmb_enable_alct_hard_reset(int flag_enable);
  int tmb_enable_vme_commands(int flag_enable);      
  /// mostly for GUI
  void executeCommand(std::string command);      
  friend std::ostream & operator<<(std::ostream & os, TMB & tmb);
      //
  int GetCLCT0Cfeb() { return CLCT0_cfeb_; }
  int GetCLCT1Cfeb() { return CLCT1_cfeb_; }
  int GetCLCT0Nhit() { return CLCT0_nhit_; }
  int GetCLCT1Nhit() { return CLCT1_nhit_; }
  int GetCLCT0keyHalfStrip() { return CLCT0_keyHalfStrip_; }
  int GetCLCT1keyHalfStrip() { return CLCT1_keyHalfStrip_; }
  //
  int FmState();
  //
  int GetAlct0Valid()    { return alct0_valid_; }
  int GetAlct0Quality()  { return alct0_quality_; }
  int GetAlct0Amu()      { return alct0_amu_; }
  int GetAlct0FirstKey() { return alct0_first_key_; }
  int GetAlct0FirstBxn() { return alct0_first_bxn_; }
  //
  int GetAlct1Valid()     { return alct1_valid_; }
  int GetAlct1Quality()   { return alct1_quality_; }
  int GetAlct1Amu()       { return alct1_amu_; }
  int GetAlct1SecondKey() { return alct1_second_key_; }
  int GetAlct1SecondBxn() { return alct1_second_bxn_; }
  //
  void enableAllClocks();
  void disableAllClocks();
  void FireMPCInjector(int);
  void ReadBackMpcRAM(int);
  //
  void TriggerTestInjectALCT();
  void TriggerTestInjectCLCT();
  //
  int MPC0Accept();
  int MPC1Accept();
  //
  bool SelfTest() ;
  void init() ;
  void configure() ;
  //
  std::bitset<64> dsnRead(int); // TMB=0, mezzanine=1, RAT=2
  void ADCvoltages(float*);
  int tmb_read_delays(int);
  //
  inline int  GetALCTrxPhase() { return alct_rx_clock_delay_; }
  inline int  GetALCTtxPhase() { return alct_tx_clock_delay_; }
  inline int  GetCFEBrxPhase(int CFEB) {
    int tmp[5] = { cfeb0delay_, cfeb1delay_, cfeb2delay_, cfeb3delay_, cfeb4delay_ };
    return tmp[CFEB]; 
  }
  //
  inline void SetStepMode(bool stepmode) { step_mode = stepmode ; }
  //
  inline std::vector<unsigned long int> GetInjectedLct0() { return InjectedLct0 ; }
  inline std::vector<unsigned long int> GetInjectedLct1() { return InjectedLct1 ; }
  //
  inline void ResetInjectedLCT() {
    InjectedLct0.clear();
    InjectedLct1.clear();
  }
  //
public:
  //
  FILE *pfile;
  FILE *pfile2;
  int ucla_ldev;
  std::string version_;
  //
protected:
  /// for PHOS4 chips, v2001
  void old_clk_delays(unsigned short int time, int cfeb_id);
  /// for DDD chips, v2004
  void new_clk_delays(unsigned short int time, int cfeb_id);
  void new_clk_delays_preGreg(unsigned short int time, int cfeb_id);
  //
  int dsnIO(int);
  //
  ALCTController * alctController_;
  RAT * rat_;
  //
private:
  //
  int jtag_address;
  int jtag_chain;
  int devices_in_chain;
  bool step_mode;
  int bits_per_opcode[MAX_NUM_CHIPS];
  std::ostream * MyOutput_ ;
  int alct_tx_clock_delay_;
  int alct_rx_clock_delay_;
  int trigMB_dav_delay_;
  int ALCT_dav_delay_; 
  int push_dav_delay_;
  int l1acc_dav_delay_;
  int cfeb0delay_;
  int cfeb1delay_;
  int cfeb2delay_;
  int cfeb3delay_;
  int cfeb4delay_;
  int l1a_offset_;
  int bxn_offset_;
  int l1a_window_size_;
  int l1adelay_;
  int alct_match_window_size_;
  int alct_vpf_delay_;
  int mpc_delay_;
  int ALCT_input_;
  int rpc_exists_;
  int fifo_mode_;
  int fifo_tbins_;
  int fifo_pretrig_;
  int alct_clear_;
  int disableCLCTInputs_;
  int mpc_tx_delay_;
  //
  std::vector<unsigned long int> InjectedLct0;
  std::vector<unsigned long int> InjectedLct1;
  unsigned long lct0_, lct1_;
  int CLCT0_cfeb_;
  int CLCT1_cfeb_;
  int CLCT0_nhit_;
  int CLCT1_nhit_;
  int CLCT0_keyHalfStrip_;
  int CLCT1_keyHalfStrip_;
  static const int MaxCounter = 0x2d;
  long int FinalCounter[MaxCounter];
  //
  int alct0_valid_;
  int alct0_quality_;
  int alct0_amu_;
      int alct0_first_key_;
  int alct0_first_bxn_;
  //
  int alct1_valid_;
  int alct1_quality_;
  int alct1_amu_;
  int alct1_second_key_;
  int alct1_second_bxn_;
  //
};

#endif

