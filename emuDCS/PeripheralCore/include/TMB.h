//-----------------------------------------------------------------------
// $Id: TMB.h,v 3.17 2006/11/15 16:01:36 mey Exp $
// $Log: TMB.h,v $
// Revision 3.17  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.16  2006/11/10 12:43:07  rakness
// include TMB/ALCT configuration and state machine prints+checks to hyperDAQ
//
// Revision 3.15  2006/11/09 08:47:51  rakness
// add rpc0_raw_delay to xml file
//
// Revision 3.14  2006/11/08 13:53:37  mey
// Update
//
// Revision 3.13  2006/10/30 15:55:43  mey
// Update
//
// Revision 3.12  2006/10/13 15:34:38  rakness
// add mpc_phase
//
// Revision 3.11  2006/10/12 15:56:00  rakness
// cleaned up configuration checking for ALCT/TMB
//
// Revision 3.10  2006/10/10 15:34:58  rakness
// check TMB/ALCT configuration vs xml
//
// Revision 3.9  2006/10/06 12:15:39  rakness
// expand xml file
//
// Revision 3.8  2006/09/24 13:34:37  rakness
// decode configuration registers
//
// Revision 3.7  2006/09/15 07:50:41  rakness
// dump config registers
//
// Revision 3.6  2006/09/08 00:06:11  mey
// UPdate
//
// Revision 3.5  2006/09/06 12:38:10  rakness
// correct time stamp/copy vectors for user prom
//
// Revision 3.4  2006/09/05 10:13:17  rakness
// ALCT configure from prom
//
// Revision 3.3  2006/08/11 16:23:33  rakness
// able to write TMB user prom from configure()
//
// Revision 3.2  2006/08/09 11:57:04  mey
// Got rid of version
//
// Revision 3.1  2006/08/08 19:23:08  mey
// Included Jtag sources
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.48  2006/07/18 14:10:11  mey
// Update
//
// Revision 2.47  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.46  2006/06/23 13:53:29  mey
// bug fix
//
// Revision 2.45  2006/06/20 13:18:16  mey
// Update
//
// Revision 2.44  2006/06/20 12:34:10  mey
// Get Output
//
// Revision 2.43  2006/06/12 12:47:16  mey
// Update
//
// Revision 2.42  2006/05/31 09:10:01  rakness
// Add GetMPCdelay
//
// Revision 2.41  2006/05/24 09:55:03  mey
// Added crate counters
//
// Revision 2.40  2006/04/25 13:25:18  mey
// Update
//
// Revision 2.39  2006/04/11 15:27:42  mey
// Update
//
// Revision 2.38  2006/04/06 08:54:32  mey
// Got rif of friend TMBParser
//
// Revision 2.37  2006/03/28 10:44:21  mey
// Update
//
// Revision 2.36  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.35  2006/03/22 14:36:52  mey
// UPdate
//
// Revision 2.34  2006/03/21 12:27:02  mey
// Update
//
// Revision 2.33  2006/03/20 14:05:59  mey
// UPdate
//
// Revision 2.32  2006/03/20 13:34:46  mey
// Update
//
// Revision 2.31  2006/03/20 09:10:43  mey
// Update
//
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

#include "EMUjtag.h"

class ALCTController;
class TMBParser;
class RAT;
class AnodeChannel;
class Crate;
class Chamber;

class TMB :  public VMEModule, public EMUjtag {

public:
  //
  friend class ALCTController;
  //friend class TMBParser;
  friend class EMUjtag;
  //
  explicit TMB(Crate * , Chamber *, int );
  virtual ~TMB();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void WriteOutput(std::string);
  //
  /// from the BOARDTYPE enum
  unsigned int boardType() const {return TMB_ENUM;}
  /// ucla_start was always called with a dev and a slot
  void start();
  void end();
  /// ALCTs need to go to lower scan level, whatever that means
  void start(int,int jtagSource=jtagSourceBoot);
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
  //void trgmode_bprsq_dmb();
  void firmwareVersion();
  void setLogicAnalyzerToDataStream(bool yesorno);
  //void tmb_PHOS4_alct(int time=0);
  //void tmb_PHOS4_cfeb();
  void EnableL1aRequest();
  void DisableL1aRequest();
  //
  void EnableCLCTInputs(int CLCInputs );
  void DisableCLCTInputs();
  //
  void DisableALCTInputs();
  void DisableALCTCLCTSync();
  void DisableExternalCCB();
  void EnableInternalL1aEmulator();
  void DisableInternalL1aSequencer();
  void EnableInternalL1aSequencer();
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
  std::string CounterName(int);
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
  //void select_jtag_chain_param();
  //
  //void jtag_ir_dr(int,int,int*,int,int*);
  //void jtag_io_byte(int,int*,int*,int* );
  //int bits_to_int(int*,int,int);
  //void step(int,int,int,int,int);
  //
  void SetALCTController(ALCTController* a) {alctController_=a;}
  ALCTController * alctController() const {return alctController_;}
  RAT * getRAT() const {return rat_;}
  //      
  int tmb_get_id(struct tmb_id_regs* tmb_id);
  int tmb_set_jtag_src(unsigned short int jtag_src);
  int tmb_get_jtag_src(unsigned short int* jtag_src);
  int tmb_set_jtag_chain(unsigned int jchain);
  //void set_jtag_chain(int);
  //void jtag_anystate_to_rti();
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
  inline int  GetCFEBrxPhase(int CFEB) {
    int tmp[5] = { cfeb0delay_, cfeb1delay_, cfeb2delay_, cfeb3delay_, cfeb4delay_ };
    return tmp[CFEB]; 
  }
  inline int  GetMPCdelay() { return mpc_delay_; }
  //
  //inline void SetStepMode(bool stepmode) { step_mode = stepmode ; }
  //
  void SetCFEB0delay(int delay){cfeb0delay_ = delay;}
  int  GetCFEB0delay(){return cfeb0delay_;}
  //
  void SetCFEB1delay(int delay){cfeb1delay_ = delay;}
  int  GetCFEB1delay(){return cfeb1delay_;}
  //
  void SetCFEB2delay(int delay){cfeb2delay_ = delay;}
  int  GetCFEB2delay(){return cfeb2delay_;}
  //
  void SetCFEB3delay(int delay){cfeb3delay_ = delay;}
  int  GetCFEB3delay(){return cfeb3delay_;}
  //
  void SetCFEB4delay(int delay){cfeb4delay_ = delay;}
  int  GetCFEB4delay(){return cfeb4delay_;}
  //
  void SetAlctTXclockDelay(int delay) {alct_tx_clock_delay_ = delay;}
  inline int  GetALCTtxPhase() { return alct_tx_clock_delay_; }
  //
  void SetAlctRXclockDelay(int delay) {alct_rx_clock_delay_ = delay;}
  inline int  GetALCTrxPhase() { return alct_rx_clock_delay_; }
  //
  void SetL1aDelay(int delay) { l1adelay_ = delay; }
  inline int GetL1aDelay() { return l1adelay_; }
  //
  void SetL1aWindowSize(int delay) {l1a_window_size_ = delay;}
  inline int GetL1aWindowSize() {return l1a_window_size_;}
  //
  void SetAlctMatchWindowSize(int size) {alct_match_window_size_ = size;}
  inline int  GetAlctMatchWindowSize()         {return alct_match_window_size_ ;}
  //
  void SetAlctVpfDelay(int delay){alct_vpf_delay_ = delay;}
  int GetAlctVpfDelay(){return alct_vpf_delay_;}
  //
  //void SetVersion(std::string version) {version_ = version;}
  void SetMpcDelay(int delay) {mpc_delay_= delay;}
  void SetAlctInput(int input) {ALCT_input_ = input;}
  void SetRpcExist(int exist) {rpc_exists_ = exist;}
  void SetFifoMode(int mode) {fifo_mode_ = mode;}
  //
  void SetFifoTbins(int tbins) {fifo_tbins_ = tbins;}
  inline int GetFifoTbins() { return fifo_tbins_ ;}
  //
  void SetFifoPreTrig(int pretrig) {fifo_pretrig_ = pretrig;}
  inline int GetFifoPreTrig() {return fifo_pretrig_;}
  //
  void SetAlctClear(int clear){alct_clear_ = clear;}
  //
  void SetMpcTXdelay(int delay){mpc_tx_delay_ = delay;}
  inline int GetMpcTXdelay(){return mpc_tx_delay_ ;}
  //
  void SetL1aOffset(int offset){l1a_offset_ = offset;}
  void SetBxnOffset(int offset){bxn_offset_ = offset;}
  void SetDisableCLCTInputs(int disable){disableCLCTInputs_ = disable;}
  void SetEnableCLCTInputs(int enable){enableCLCTInputs_ = enable;}
  void SetAlct(ALCTController* alct){alctController_ = alct;}
  void SetRat(RAT* rat){rat_ = rat;}
  //
  inline void SetTrgMode(int trgmode){ trgmode_ = trgmode; }
  inline int  GetTrgMode(){ return trgmode_ ; }
  //
  inline void SetRpcBxnOffset(int rpc_bxn_offset){ rpc_bxn_offset_ = rpc_bxn_offset; }
  inline int  GetRpcBxnOffset(){ return rpc_bxn_offset_; }
  //
  inline void SetShiftRpc(int shift_rpc){ shift_rpc_ = shift_rpc; }
  inline int  GetShiftRpc(){ return shift_rpc_; }
  //
  inline void SetRequestL1a(int request_l1a){ request_l1a_ = request_l1a; }
  inline int  GetRequestL1a(){ return request_l1a_; }
  //
  inline void SetHsPretrigThresh(int hs_pretrig_thresh){ hs_pretrig_thresh_ = hs_pretrig_thresh; }
  inline int  GetHsPretrigThresh(){ return hs_pretrig_thresh_; }
  //
  inline void SetDsPretrigThresh(int ds_pretrig_thresh){ ds_pretrig_thresh_ = ds_pretrig_thresh; }
  inline int  GetDsPretrigThresh(){ return ds_pretrig_thresh_; }
  //
  inline void SetMinHitsPattern(int min_hits_pattern){ min_hits_pattern_ = min_hits_pattern; }
  inline int  GetMinHitsPattern(){ return min_hits_pattern_ ;}
  //
  inline void SetDmbTxDelay(int dmb_tx_delay){ dmb_tx_delay_ = dmb_tx_delay; }
  inline int  GetDmbTxDelay(){ return dmb_tx_delay_; }
  //
  inline void SetRatTmbDelay(int rat_tmb_delay){ rat_tmb_delay_ = rat_tmb_delay; }
  inline int  GetRatTmbDelay(){ return rat_tmb_delay_ ; }
  //
  inline void SetRpc0RatDelay(int rpc0_rat_delay){ rpc0_rat_delay_ = rpc0_rat_delay; }
  inline int  GetRpc0RatDelay(){ return rpc0_rat_delay_ ; }
  //
  inline void SetMpcPhase(int mpc_phase){ mpc_phase_ = mpc_phase; }
  //
  inline void SetRpc0RawDelay(int rpc0_raw_delay){ rpc0_raw_delay_ = rpc0_raw_delay; }
  inline int  GetRpc0RawDelay(){ return rpc0_raw_delay_ ; }
  //
  inline std::vector<unsigned long int> GetInjectedLct0() { return InjectedLct0 ; }
  inline std::vector<unsigned long int> GetInjectedLct1() { return InjectedLct1 ; }
  //
  inline void ResetInjectedLCT() {
    InjectedLct0.clear();
    InjectedLct1.clear();
  }
  //
  void ReadTmbIdCodes();
  inline int GetTMBmezzFpgaIdCode() { return tmb_idcode_[0]; }
  inline int GetTMBmezzProm0IdCode() { return tmb_idcode_[1]; }
  inline int GetTMBmezzProm1IdCode() { return tmb_idcode_[2]; }
  inline int GetTMBmezzProm2IdCode() { return tmb_idcode_[3]; }
  inline int GetTMBmezzProm3IdCode() { return tmb_idcode_[4]; }
  inline int GetTMBuserProm0IdCode() { return tmb_idcode_[5]; }
  inline int GetTMBuserProm1IdCode() { return tmb_idcode_[6]; }
  //
  void OkVmeWrite(char vme_address);                    // allowed to write this address into user prom?
  void ClearVmeWriteVecs();                             
  void SetFillVmeWriteVecs(bool fill_vectors_or_not);   // put the vme information into vectors to put into user prom?
  bool GetFillVmeWriteVecs();
  std::vector<int> GetVecVmeAddress();               // vme write information
  std::vector<int> GetVecDataLsb();                  // vme write information
  std::vector<int> GetVecDataMsb();                  // vme write information
  //
  void ClockOutPromProgram(int prom,int number_of_addresses);
  inline int GetClockedOutPromImage(int address) { return clocked_out_prom_image_.at(address); }
  //
  void DumpAllRegisters();
  //
  void ReadTMBConfiguration();
  void PrintTMBConfiguration();
  void CheckTMBConfiguration();
  inline int GetTMBConfigurationStatus() { return tmb_configuration_status_; }
  //
  void ReadVMEStateMachine();
  void PrintVMEStateMachine();
  void CheckVMEStateMachine();
  inline int GetVMEStateMachineStatus() { return vme_state_machine_status_; }
  //
  void ReadJTAGStateMachine();
  void PrintJTAGStateMachine();
  void CheckJTAGStateMachine();
  inline int GetJTAGStateMachineStatus() { return jtag_state_machine_status_; }
  //
  void ReadDDDStateMachine();
  void PrintDDDStateMachine();
  void CheckDDDStateMachine();
  inline int GetDDDStateMachineStatus() { return ddd_state_machine_status_; }
  //
  void ReadRawHitsHeader();
  void PrintRawHitsHeader();
  void CheckRawHitsHeader();
  inline int GetRawHitsHeaderStatus() { return raw_hits_header_status_; }
  //
  //
  int makemask(int lo_bit, int hi_bit);
  void ReportCheck(std::string check_type, bool status_bit);
  bool compareValues(std::string typeOfTest,
		     int read_val,
		     int write_val,
		     bool equalOrNot);
  bool compareValues(std::string typeOfTest,
		     float read_val,
		     float write_val,
		     float tolerance);
  //
public:
  //
  FILE *pfile;
  FILE *pfile2;
  int ucla_ldev;
  //std::string version_;
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
  Chamber * csc_;
  int tmb_idcode_[7];
  //int jtag_address;
  //int jtag_chain;
  int devices_in_chain;
  //bool step_mode;
  //int bits_per_opcode[MAX_NUM_CHIPS];
  std::ostream * MyOutput_ ;
  int alct_tx_clock_delay_;
  int alct_rx_clock_delay_;
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
  int enableCLCTInputs_;
  int mpc_tx_delay_;
  //
  int trgmode_;                  //
  int rpc_bxn_offset_;
  int shift_rpc_;
  int request_l1a_;
  int hs_pretrig_thresh_;
  int ds_pretrig_thresh_;
  int min_hits_pattern_;
  int dmb_tx_delay_;
  int rat_tmb_delay_;
  int rpc0_rat_delay_;
  int mpc_phase_;
  int rpc0_raw_delay_;
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
  std::vector<int> clocked_out_prom_image_;
  //
  int tmb_configuration_status_;
  int vme_state_machine_status_;
  int jtag_state_machine_status_;
  int ddd_state_machine_status_;
  int raw_hits_header_status_;
  //
  // values decoded from the TMB registers:
  void DecodeBootAddress_(int data);
  void DecodeTMBRegister_(unsigned long int address, int data);
  //
  int read_firmware_date_;
  int boot_register_control_jtag_chain_;
  int read_firmware_year_;
  int read_rpc_exists_;
  int read_rpc_read_enable_;
  int read_rpc_bxn_offset_;
  int read_shift_rpc_;
  int read_enable_alct_rx_;
  int read_enable_alct_tx_;
  int read_ALCT_input_;
  int read_alct_ext_trig_l1aen_;
  int read_clct_ext_trig_l1aen_;
  int read_request_l1a_;
  int read_alct_ext_trig_vme_;
  int read_clct_ext_trig_vme_;
  int read_ext_trig_both_;
  int read_ccb_allow_bypass_;
  int read_internal_l1a_delay_vme_;
  int read_fifo_mode_;
  int read_fifo_tbins_;
  int read_fifo_pretrig_;
  int read_clct_pat_trig_en_;
  int read_alct_pat_trig_en_;
  int read_match_pat_trig_en_;
  int read_adb_ext_trig_en_;
  int read_dmb_ext_trig_en_;
  int read_clct_ext_trig_en_;
  int read_alct_ext_trig_en_;
  int read_vme_ext_trig_en_;
  int read_ext_trig_inject_;
  int read_all_cfeb_active_;
  int read_cfebs_enabled_;
  int read_cfeb_enable_source_;
  int read_alct_vpf_delay_;
  int read_alct_match_window_size_;
  int read_mpc_tx_delay_;
  int read_l1a_offset_;
  int read_bxn_offset_;
  int read_triad_persist_;
  int read_hs_pretrig_thresh_;
  int read_ds_pretrig_thresh_;
  int read_min_hits_pattern_;
  int read_drift_delay_;
  int read_pretrigger_halt_;
  int read_seq_readmode_in_dmb_;
  int read_l1adelay_;
  int read_l1a_window_size_;
  int read_tmb_sync_err_enable_;
  int read_tmb_allow_clct_;
  int read_tmb_allow_match_;
  int read_mpc_delay_;
  int read_mpc_sel_ttc_bx0_;
  int read_mpc_idle_blank_;
  int read_tmb1_phase_;
  int read_mpc_phase_;
  int read_dcc_phase_;
  int read_cfeb0delay_;
  int read_cfeb1delay_;
  int read_cfeb2delay_;
  int read_cfeb3delay_;
  int read_cfeb4delay_;
  int read_alct_tx_clock_delay_;
  int read_alct_rx_clock_delay_;
  int read_dmb_tx_delay_;
  int read_rat_tmb_delay_;
  int read_rpc0_rat_delay_;
  int read_rpc1_rat_delay_;
  int read_rpc2_rat_delay_;
  int read_rpc3_rat_delay_;
  int read_enableCLCTInputs_;
  int read_cfeb_ram_sel_;
  int read_cfeb_inj_en_sel_;
  int read_start_pattern_inj_;
  int read_tmb_slot_;
  int read_csc_id_;
  int read_run_id_;
  int read_alct_clear_;
  int read_alct_trig_width_;
  int read_alct_pretrig_delay_;
  int read_alct_pattern_delay_;
  int read_adb_ext_trig_delay_;
  int read_clct_flush_delay_;
  int read_clct_turbo_;
  int read_ranlct_enable_;
  int read_wrt_buf_required_;
  int read_valid_clct_required_;
  int read_l1a_allow_match_;
  int read_l1a_allow_notmb_;
  int read_l1a_allow_nol1a_;
  int read_l1a_allow_alct_only_;
  int read_scint_veto_clr_;
  int read_rpc0_raw_delay_;
  int read_rpc1_raw_delay_;
  int read_rpc2_raw_delay_;
  int read_rpc3_raw_delay_;
  //
  int read_vme_state_machine_start_;
  int read_vme_state_machine_sreset_;
  int read_vme_state_machine_autostart_; 
  int read_vme_state_machine_busy_;
  int read_vme_state_machine_aborted_;
  int read_vme_state_machine_cksum_ok_;
  int read_vme_state_machine_wdcnt_ok_; 
  int read_vme_state_machine_jtag_auto_; 
  int read_vme_state_machine_vme_ready_; 
  int read_vme_state_machine_ok_;
  int read_vme_state_machine_path_ok_; 
  int read_vme_state_machine_throttle_; 
  //
  int read_vme_state_machine_word_count_;
  //
  int read_vme_state_machine_check_sum_;
  int read_vme_state_machine_error_missing_header_start_;
  int read_vme_state_machine_error_missing_header_end_;
  int read_vme_state_machine_error_missing_data_end_marker_;
  int read_vme_state_machine_error_missing_trailer_end_;
  int read_vme_state_machine_error_word_count_overflow_;
  //
  int read_vme_state_machine_number_of_vme_writes_;
  //
  int read_jtag_state_machine_start_;
  int read_jtag_state_machine_sreset_;
  int read_jtag_state_machine_autostart_;
  int read_jtag_state_machine_busy_;
  int read_jtag_state_machine_aborted_;
  int read_jtag_state_machine_cksum_ok_;
  int read_jtag_state_machine_wdcnt_ok_;
  int read_jtag_state_machine_tck_fpga_ok_;
  int read_jtag_state_machine_vme_ready_;
  int read_jtag_state_machine_ok_;
  int read_jtag_state_machine_oe_;
  int read_jtag_state_machine_throttle_;
  //
  int read_jtag_state_machine_word_count_;
  //
  int read_jtag_state_machine_check_sum_;
  int read_jtag_state_machine_tck_fpga_;
  //
  int read_boot_tdi_;
  int read_boot_tms_;
  int read_boot_tck_;
  int read_boot_jtag_chain_select_;
  int read_boot_control_jtag_chain_;
  int read_boot_hard_reset_alct_;
  int read_boot_hard_reset_tmb_;
  int read_boot_allow_hard_reset_alct_;
  int read_boot_allow_VME_;
  int read_boot_enable_mezz_clock_;
  int read_boot_hard_reset_rat_;
  int read_boot_vme_ready_;
  int read_boot_tdo_;
  //
  int read_raw_hits_header_;
  //
  int read_ddd_state_machine_start_;
  int read_ddd_state_machine_manual_;
  int read_ddd_state_machine_latch_;
  int read_ddd_state_machine_serial_in_;
  int read_ddd_state_machine_serial_out_;
  int read_ddd_state_machine_autostart_;
  int read_ddd_state_machine_busy_;
  int read_ddd_state_machine_verify_ok_;
  int read_ddd_state_machine_clock0_lock_;
  int read_ddd_state_machine_clock0d_lock_;
  int read_ddd_state_machine_clock1_lock_;
  int read_ddd_state_machine_clock_alct_lock_;
  int read_ddd_state_machine_clockd_alct_lock_;
  int read_ddd_state_machine_clock_mpc_lock_;
  int read_ddd_state_machine_clock_dcc_lock_;
  int read_ddd_state_machine_clock_rpc_lock_;
  //
  int read_trgmode_;
  int read_CLCTtrigger_setting_;
  int read_ALCTCLCTtrigger_setting_;
  //
};

#endif

