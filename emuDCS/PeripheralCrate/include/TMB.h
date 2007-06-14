//-----------------------------------------------------------------------
// $Id: TMB.h,v 3.24 2007/06/14 14:47:56 rakness Exp $
// $Log: TMB.h,v $
// Revision 3.24  2007/06/14 14:47:56  rakness
// clean up MPC injection
//
// Revision 3.23  2007/06/12 09:56:56  rakness
// clean TMB Raw Hits
//
// Revision 3.22  2007/06/07 12:57:28  rakness
// update TMB counters
//
// Revision 3.21  2007/05/17 12:52:50  rakness
// ignore_ccb_startstop added to TMB configuration + write configuration to userPROM default
//
// Revision 3.20  2007/04/10 13:31:01  rakness
// add mpc_output_enable, remove rpc2/3
//
// Revision 3.19  2007/03/14 08:59:03  rakness
// make parser dumb
//
// Revision 3.18  2007/01/31 16:49:52  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
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
  void TMBRawhits(int number_of_reads = 1);
  bool OnlyReadTMBRawhits();
  void ForceScopeTrigger();
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
  //
  //
  int  FirmwareDate();
  inline int  GetReadTmbFirmwareDay() { return read_tmb_firmware_day_; }
  inline void SetExpectedTmbFirmwareDay(int day) { tmb_firmware_day_ = ConvertToHexAscii(day); }
  inline int  GetExpectedTmbFirmwareDay() { return tmb_firmware_day_; }
  //
  inline int  GetReadTmbFirmwareMonth() { return read_tmb_firmware_month_; }
  inline void SetExpectedTmbFirmwareMonth(int month) { tmb_firmware_month_ = ConvertToHexAscii(month); }
  inline int  GetExpectedTmbFirmwareMonth() { return tmb_firmware_month_; }
  //
  //
  int  FirmwareYear();
  inline int  GetReadTmbFirmwareYear() { return read_tmb_firmware_year_; }
  inline void SetExpectedTmbFirmwareYear(int year) { tmb_firmware_year_ = ConvertToHexAscii(year); }
  inline int  GetExpectedTmbFirmwareYear() { return tmb_firmware_year_; }
  //
  //
  int  FirmwareVersion();
  inline int  GetReadTmbFirmwareType() { return read_tmb_firmware_type_; }
  inline void SetExpectedTmbFirmwareType(int type) { tmb_firmware_type_ = type; }
  inline int  GetExpectedTmbFirmwareType() { return tmb_firmware_type_; }
  //
  inline int  GetReadTmbFirmwareVersion() { return read_tmb_firmware_version_; }
  inline void SetExpectedTmbFirmwareVersion(int version) { tmb_firmware_version_ = version; }
  inline int  GetExpectedTmbFirmwareVersion() { return tmb_firmware_version_; }
  //
  //
  int  FirmwareRevCode();
  inline int  GetReadTmbFirmwareRevcode() { return read_tmb_firmware_revcode_; }
  inline void SetExpectedTmbFirmwareRevcode(int revcode) { tmb_firmware_revcode_ = revcode; }
  inline int  GetExpectedTmbFirmwareRevcode() { return tmb_firmware_revcode_; }
  //
  inline void SetExpectedRatFirmwareDay(int day) { rat_firmware_day_ = ConvertToHexAscii(day); }
  inline int  GetExpectedRatFirmwareDay() { return rat_firmware_day_; }
  inline void SetExpectedRatFirmwareMonth(int month) { rat_firmware_month_ = ConvertToHexAscii(month); }
  inline int  GetExpectedRatFirmwareMonth() { return rat_firmware_month_; }
  inline void SetExpectedRatFirmwareYear(int year) { rat_firmware_year_ = ConvertToHexAscii(year); }
  inline int  GetExpectedRatFirmwareYear() { return rat_firmware_year_; }
  //
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
  void SetALCTController(ALCTController* a) {alctController_=a;}
  ALCTController * alctController() const {return alctController_;}
  RAT * getRAT() const {return rat_;}
  //      
  int tmb_get_id(struct tmb_id_regs* tmb_id);
  int tmb_set_jtag_src(unsigned short int jtag_src);
  int tmb_get_jtag_src(unsigned short int* jtag_src);
  int tmb_set_jtag_chain(unsigned int jchain);
  //
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
  //
  //void SetVersion(std::string version) {version_ = version;}
  void SetDisableCLCTInputs(int disable){disableCLCTInputs_ = disable;}
  //
  void SetAlct(ALCTController* alct){alctController_ = alct;}
  void SetRat(RAT* rat){rat_ = rat;}
  //
  inline void SetShiftRpc(int shift_rpc){ shift_rpc_ = shift_rpc; }
  inline int  GetShiftRpc(){ return shift_rpc_; }
  //
  //
  ////////////////////////////////////////////////////
  // configuration parameters
  ////////////////////////////////////////////////////
  // ******************************************
  // Enable/configure inputs and injectors:
  // ******************************************
  //----------------------------------------------------------------
  //0X0E = ADR_LOOPBK:  Loop-Back Control Register:
  //----------------------------------------------------------------
  inline void SetAlctInput(int ALCT_input) { ALCT_input_ = ALCT_input; }        
  inline int  GetAlctInput() { return ALCT_input_;}
  //ALCT_input = [0,1] -> ALCT rx [off,on]
  //
  inline void SetEnableAlctTx(int enable_alct_tx) { enable_alct_tx_ = enable_alct_tx; } 
  inline int  GetEnableAlctTx() { return enable_alct_tx_; }
  //enable_alct_tx = [0,1] -> ALCT tx [off,on]
  //
  //----------------------------------------------------------------
  //0X32 = ADR_ALCT_INJ:  ALCT Injector Control:
  //----------------------------------------------------------------
  inline void SetAlctClear(int alct_clear) { alct_clear_ = alct_clear; }                    
  inline int  GetAlctClear() { return alct_clear_; }
  //alct_clear = [0,1] -> [do not blank,blank] ALCT received data
  //
  inline void SetAlctInject(int alct_inject_mux) { alct_inject_mux_ = alct_inject_mux; }              
  inline int  GetAlctInject() { return alct_inject_mux_; }
  //alct_inject_mux = 1 = start ALCT injector state machine
  //
  inline void SetSyncAlctInjectToClctInject(int alct_sync_clct) { alct_sync_clct_ = alct_sync_clct; } 
  inline int  GetSyncAlctInjectToClctInject() { return alct_sync_clct_; }
  //alct_sync_clct = 1 = link ALCT injector with CLCT inject command
  //
  inline void SetAlctInjectorDelay(int alct_inj_delay) { alct_inj_delay_ = alct_inj_delay; }
  inline int  GetAlctInjectorDelay() { return alct_inj_delay_; }
  //
  //----------------------------------------------------------------
  //0X42 = ADR_CFEB_INJ:  CFEB Injector Control:
  //----------------------------------------------------------------
  inline void SetEnableCLCTInputs(int enableCLCTInputs) { enableCLCTInputs_ = enableCLCTInputs; } 
  inline int  GetEnableCLCTInputs() { return enableCLCTInputs_; }
  //enableCLCTInputs = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [disable,enable] CFEB input
  //
  inline void SetSelectCLCTRAM(int cfeb_ram_sel) { cfeb_ram_sel_ = cfeb_ram_sel; }        
  inline int  GetSelectCLCTRAM() { return cfeb_ram_sel_; }
  //cfeb_ram_sel = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [do not select,select] CFEB for RAM read/write
  //
  inline void SetEnableCLCTInject(int cfeb_inj_en_sel) { cfeb_inj_en_sel_ = cfeb_inj_en_sel; }  
  inline int  GetEnableCLCTInject() { return cfeb_inj_en_sel_; }
  //cfeb_inj_en_sel = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [disable,enable] CFEB for injector trigger
  //
  inline void SetStartPatternInjector(int start_pattern_inj) { start_pattern_inj_ = start_pattern_inj; } 
  inline int  GetStartPatternInjector() { return start_pattern_inj_; }
  //start_pattern_inj = 1 = start pattern injector
  //
  //------------------------------------------------------------------
  //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
  //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
  //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
  //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
  //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
  //------------------------------------------------------------------
  inline void SetDistripHotChannelMask(int layer,int distrip,int on_or_off) { hot_channel_mask_[layer][distrip] = on_or_off; } 
  inline int  GetDistripHotChannelMask(int layer,int distrip) { return hot_channel_mask_[layer][distrip]; }
  //layer=[0-5], distrip=[0-39], on_or_off = 0 = disable
  //
  //-----------------------------------------------------------------
  //0XB6 = ADR_RPC_CFG:  RPC Configuration:
  //-----------------------------------------------------------------
  inline void SetRpcExist(int rpc_exists) { rpc_exists_ = rpc_exists; }  
  inline int  GetRpcExist() { return rpc_exists_; }
  //rpc_exists = [0-15]... 4 bit mask, 1 bit per RPC -> each bit [0,1] = RPC [does not,does] exist
  //
  inline void SetRpcReadEnable(int rpc_read_enable) { rpc_read_enable_ = rpc_read_enable; } 
  inline int  GetRpcReadEnable() { return rpc_read_enable_; }
  //rpc_read_enable = 1 = include existing RPCs in DMB readout
  //
  inline void SetRpcBxnOffset(int rpc_bxn_offset) { rpc_bxn_offset_ = rpc_bxn_offset; } 
  inline int  GetRpcBxnOffset() { return rpc_bxn_offset_; }
  //rpc_bxn_offset = [0-15]
  //
  inline void SetRpcSyncBankAddress(int rpc_bank) { rpc_bank_ = rpc_bank; } 
  inline int  GetRpcSyncBankAddress() { return rpc_bank_; }
  //rpc_bank = [0-3] -> RPC bank address, for reading rdata sync mode
  //
  //------------------------------------------------------------------
  //0XBC = ADR_RPC_INJ:  RPC Injector Control
  //------------------------------------------------------------------
  inline void SetEnableRpcInput(int rpc_mask_all) { rpc_mask_all_ = rpc_mask_all; }   
  inline int  GetEnableRpcInput() { return rpc_mask_all_; }
  //rpc_mask_all = [1,0] -> All RPC inputs [on,off]
  //
  inline void SetInjectorMaskRat(int inj_mask_rat) { inj_mask_rat_ = inj_mask_rat; }
  inline int  GetInjectorMaskRat() { return inj_mask_rat_; }
  //inj_mask_rat = 1 = enable RAT for injector fire
  //
  inline void SetInjectorMaskRpc(int inj_mask_rpc) { inj_mask_rpc_ = inj_mask_rpc; }
  inline int  GetInjectorMaskRpc() { return inj_mask_rpc_; }
  //inj_mask_rpc = 1 = enable RPC injector RAM for injector fire
  //
  inline void SetInjectorDelayRat(int inj_delay_rat) { inj_delay_rat_ = inj_delay_rat; }
  inline int  GetInjectorDelayRat() { return inj_delay_rat_; }
  //inj_delay_rat = [0-15] -> CFEB/RPC injectors wait for RAT
  //
  inline void SetRpcInjector(int rpc_inj_sel) {rpc_inj_sel_ = rpc_inj_sel; }
  inline int  GetRpcInjector() {return rpc_inj_sel_; }
  //rpc_inj_sel = 1 = enable injector RAM write 
  //
  inline void SetRpcInjectorWriteDataMSBs(int rpc_inj_wdata) { rpc_inj_wdata_ = rpc_inj_wdata; }
  inline int  GetRpcInjectorWriteDataMSBs() { return rpc_inj_wdata_; }
  //rpc_inj_wdata = [0-7] -> RPC injector write data MSBs
  //
  // ******************************************
  // trigger and signal delays:
  // ******************************************
  //------------------------------------------------------------------
  //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  inline void SetAlctClctPretrigWidth(int alct_pretrig_width) { alct_pretrig_width_ = alct_pretrig_width; }
  inline int  GetAlctClctPretrigWidth() { return alct_pretrig_width_; }
  //alct_pretrig_width = [0-15] = window width for ALCT*CLCT pretrigger (bx)
  //
  inline void SetAlctPretrigDelay(int alct_pretrig_delay) { alct_pretrig_delay_ = alct_pretrig_delay; }
  inline int  GetAlctPretrigDelay() { return alct_pretrig_delay_; }  
  //alct_pretrig_delay = [0-15] = ALCT delay for use in ALCT*CLCT pretrigger (bx)
  //
  inline void SetAlctPatternDelay(int alct_pattern_delay) { alct_pattern_delay_ = alct_pattern_delay; }
  inline int  GetAlctPatternDelay() { return alct_pattern_delay_; }
  //alct_pattern_delay = [0-15] = delay active FEB flag from ALCT (bx)
  //
  inline void SetAdbExternalTrigDelay(int adb_ext_trig_delay) { adb_ext_trig_delay_ = adb_ext_trig_delay; }
  inline int  GetAdbExternalTrigDelay() { return adb_ext_trig_delay_; }
  //adb_ext_trig_delay = [0-15] = delay ADB external trig from CCB (bx)
  //
  //------------------------------------------------------------------
  //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  inline void SetDmbExternalTrigDelay(int dmb_ext_trig_delay) { dmb_ext_trig_delay_ = dmb_ext_trig_delay; }
  inline int  GetDmbExternalTrigDelay() { return dmb_ext_trig_delay_; }
  //dmb_ext_trig_delay = [0-15] = delay external trigger from DMB (bx)
  //
  inline void SetClctExternalTrigDelay(int clct_ext_trig_delay) { clct_ext_trig_delay_ = clct_ext_trig_delay; }
  inline int  GetClctExternalTrigDelay() { return clct_ext_trig_delay_; }
  //clct_ext_trig_delay = [0-15] = delay external trigger from CLCT (bx)
  //
  inline void SetAlctExternalTrigDelay(int alct_ext_trig_delay) { alct_ext_trig_delay_ = alct_ext_trig_delay; }
  inline int  GetAlctExternalTrigDelay() { return alct_ext_trig_delay_; }
  //alct_ext_trig_delay = [0-15] = delay external trigger from ALCT (bx)
  //
  inline void SetLayerTrigDelay(int layer_trig_delay) { layer_trig_delay_ = layer_trig_delay; }
  inline int  GetLayerTrigDelay() { return layer_trig_delay_; }
  //layer_trig_delay = [0-15] = delay layer trigger (bx)
  //
  //------------------------------------------------------------------
  //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
  //------------------------------------------------------------------
  inline void SetL1aDelay(int l1adelay) { l1adelay_ = l1adelay; }
  inline int  GetL1aDelay() { return l1adelay_; }
  //l1adelay = [0-255] (bx)
  //
  inline void SetL1aWindowSize(int l1a_window_size) { l1a_window_size_ = l1a_window_size; }
  inline int  GetL1aWindowSize() { return l1a_window_size_; }
  //l1a_window_size = [0-15] (bx)
  //
  inline void SetInternalL1a(int tmb_l1a_internal) { tmb_l1a_internal_ = tmb_l1a_internal; }
  inline int  GetInternalL1a() { return tmb_l1a_internal_; }
  //tmb_l1a_internal = 1 = generate internal level 1 (overrides external)
  //
  //------------------------------------------------------------------
  //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
  //------------------------------------------------------------------
  inline void SetL1aOffset(int l1a_offset) { l1a_offset_ = l1a_offset; }
  inline int  GetL1aOffset() { return l1a_offset_; }
  //l1a_offset = [0-15]
  //
  inline void SetBxnOffset(int bxn_offset) { bxn_offset_ = bxn_offset; }
  inline int  GetBxnOffset() { return bxn_offset_; }
  //bxn_offset = [0-4095]
  //
  //------------------------------------------------------------------
  //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
  //------------------------------------------------------------------
  inline void SetAlctVpfDelay(int alct_vpf_delay) { alct_vpf_delay_ = alct_vpf_delay; }
  inline int  GetAlctVpfDelay() { return alct_vpf_delay_; }
  //alct_vpf_delay = [0-15] = delay ALCT valid pattern flag to match with CLCT pattern for trigger (bx)
  //
  inline void SetAlctMatchWindowSize(int alct_match_window_size) { alct_match_window_size_ = alct_match_window_size; }
  inline int  GetAlctMatchWindowSize() { return alct_match_window_size_ ; }
  //alct_match_window_size = [0-15] = ALCT/CLCT match window width for trigger (bx)
  //
  inline void SetMpcTXdelay(int mpc_tx_delay) { mpc_tx_delay_ = mpc_tx_delay; }
  inline  int GetMpcTXdelay() { return mpc_tx_delay_; }
  //mpc_tx_delay = [0-15] = delay sending LCT to MPC (bx)
  //
  //------------------------------------------------------------------
  //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
  //------------------------------------------------------------------
  inline void SetRpc0RawDelay(int rpc0_raw_delay) { rpc0_raw_delay_ = rpc0_raw_delay; }
  inline int  GetRpc0RawDelay() { return rpc0_raw_delay_ ; }
  //rpc0_raw_delay = [0-15] = delay RPC data into FIFO (bx)
  //
  inline void SetRpc1RawDelay(int rpc1_raw_delay) { rpc1_raw_delay_ = rpc1_raw_delay; }
  inline int  GetRpc1RawDelay() { return rpc1_raw_delay_ ; }
  //rpc1_raw_delay = [0-15] = delay RPC data into FIFO (bx)
  //
  //
  // ******************************************
  // trigger configuration
  // ******************************************
  inline void SetTrgMode(int trgmode) { trgmode_ = trgmode; }
  inline int  GetTrgMode() { return trgmode_ ; }
  //trgmode = CLCT_trigger                  =  1;
  //          ALCT_trigger                  =  2;
  //          Scintillator_trigger          =  3;
  //          DMB_trigger                   =  4;
  //          ALCT_CLCT_coincidence_trigger =  5;
  //
  //------------------------------------------------------------------
  //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
  //------------------------------------------------------------------
  inline void SetClctPatternTrigEnable(int clct_pat_trig_en) { clct_pat_trig_en_ = clct_pat_trig_en; }
  inline int  GetClctPatternTrigEnable() { return clct_pat_trig_en_; }
  //clct_pat_trig_en = 1 = CLCT Pattern = pretrigger
  //
  inline void SetAlctPatternTrigEnable(int alct_pat_trig_en) { alct_pat_trig_en_ = alct_pat_trig_en; }
  inline int  GetAlctPatternTrigEnable() { return alct_pat_trig_en_; }
  //alct_pat_trig_en = 1 = ALCT Pattern = pretrigger
  //
  inline void SetMatchPatternTrigEnable(int match_pat_trig_en) { match_pat_trig_en_ = match_pat_trig_en; }
  inline int  GetMatchPatternTrigEnable() { return match_pat_trig_en_; }
  //match_pat_trig_en = 1 = ALCT*CLCT Pattern = pretrigger
  //
  inline void SetAdbExtTrigEnable(int adb_ext_trig_en) { adb_ext_trig_en_ = adb_ext_trig_en; }
  inline int  GetAdbExtTrigEnable() { return adb_ext_trig_en_; }
  //adb_ext_trig_en = 1 = allow ADB external triggers from CCB
  //
  inline void SetDmbExtTrigEnable(int dmb_ext_trig_en) { dmb_ext_trig_en_ = dmb_ext_trig_en; }
  inline int  GetDmbExtTrigEnable() { return dmb_ext_trig_en_; }
  //dmb_ext_trig_en = 1 = allow DMB external triggers
  //
  inline void SetClctExtTrigEnable(int clct_ext_trig_en) { clct_ext_trig_en_ = clct_ext_trig_en; }
  inline int  GetClctExtTrigEnable() { return clct_ext_trig_en_; }
  //clct_ext_trig_en = 1 = allow CLCT external triggers (scintillator) from CCB
  //
  inline void SetAlctExtTrigEnable(int alct_ext_trig_en) { alct_ext_trig_en_ = alct_ext_trig_en; }
  inline int  GetAlctExtTrigEnable() { return alct_ext_trig_en_; }
  //alct_ext_trig_en = 1 = allow ALCT external triggers from CCB
  //
  inline void SetVmeExtTrig(int vme_ext_trig) { vme_ext_trig_ = vme_ext_trig; }
  inline int  GetVmeExtTrig() { return vme_ext_trig_; }
  //vme_ext_trig = 1 = Initiate sequencer trigger (write 0 to recover)
  //
  inline void SetExtTrigInject(int ext_trig_inject) { ext_trig_inject_ = ext_trig_inject; }
  inline int  GetExtTrigInject() { return ext_trig_inject_; }
  //ext_trig_inject = 1 = change clct_ext_trig to fire pattern injector
  //
  inline void SetEnableAllCfebsActive(int all_cfeb_active) { all_cfeb_active_ = all_cfeb_active; }
  inline int  GetEnableAllCfebsActive() { return all_cfeb_active_; }
  //all_cfeb_active = 1 = make all CFEBs active when triggered
  //
  inline void SetCfebEnable(int cfebs_enabled) { cfebs_enabled_ = cfebs_enabled; }
  inline int  GetCfebEnable() { return cfebs_enabled_; }
  //cfebs_enabled_ = [0-31] -> normally copied from 0x42.  
  //See TMB documentation before setting these bits...
  //
  void Set_cfeb_enable_source(int value); 
  // value = 42, 68 = VME register which controls the CFEB mask
  // this is the user interface to TMB::SetCfebEnableSource(int), below
  //See TMB documentation before setting this bit.
  //
  inline void SetCfebEnableSource(int cfeb_enable_source) { cfeb_enable_source_ = cfeb_enable_source; }
  inline int  GetCfebEnableSource() { return cfeb_enable_source_; }
  //cfeb_enable_source = [0,1] = [0x68,0x42] is source of cfeb_enable
  //See TMB documentation before setting this bit.
  //
  //------------------------------------------------------------------
  //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
  //------------------------------------------------------------------
  inline void SetTriadPersistence(int triad_persist) { triad_persist_ = triad_persist; }
  inline int  GetTriadPersistence() { return triad_persist_; }
  //triad_persist = [0-15] = triad one-shot perisistence (bx)
  //
  inline void SetHsPretrigThresh(int hs_pretrig_thresh) { hs_pretrig_thresh_ = hs_pretrig_thresh; }
  inline int  GetHsPretrigThresh() { return hs_pretrig_thresh_; }
  //hs_pretrig_thresh = [0-6] = 1/2-strip pretrigger threshold
  //
  inline void SetDsPretrigThresh(int ds_pretrig_thresh) { ds_pretrig_thresh_ = ds_pretrig_thresh; }
  inline int  GetDsPretrigThresh() { return ds_pretrig_thresh_; }
  //ds_pretrig_thresh = [0-6] = di-strip pretrigger threshold
  //
  inline void SetMinHitsPattern(int min_hits_pattern){ min_hits_pattern_ = min_hits_pattern; }
  inline int  GetMinHitsPattern() { return min_hits_pattern_ ;}
  //min_hits_pattern = minimum number of layers needed to match for pattern trigger
  //
  inline void SetDriftDelay(int drift_delay) { drift_delay_ = drift_delay; }
  inline int  GetDriftDelay() { return drift_delay_; }
  //drift_delay = [0-3] = drift delay between TMB pretrigger and CLCT pattern trigger
  //
  inline void SetPretriggerHalt(int pretrigger_halt) { pretrigger_halt_ = pretrigger_halt; }
  inline int  GetPretriggerHalt() { return pretrigger_halt_; }
  //pretrigger_halt = 1 = pretrigger and halt until unhalt arrives
  //
  //------------------------------------------------------------------
  //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
  //------------------------------------------------------------------
  inline void SetFifoMode(int fifo_mode) { fifo_mode_ = fifo_mode; }
  inline int  GetFifoMode() { return fifo_mode_; }
  //fifo_mode = FIFOMODE_NoCfebRaw_FullHeader    = 0
  //            FIFOMODE_AllCfebRaw_FullHeader   = 1;
  //            FIFOMODE_LocalCfebRaw_FullHeader = 2;
  //            FIFOMODE_NoCfebRaw_ShortHeader   = 3;
  //            FIFOMODE_NoCfebRaw_NoHeader      = 4;
  //
  inline void SetFifoTbins(int fifo_tbins) { fifo_tbins_ = fifo_tbins; }
  inline int  GetFifoTbins() { return fifo_tbins_ ; }
  //fifo_tbins = [0-31] -> width of window for CLCT/RPC raw data readout (bx)
  //
  inline void SetFifoPreTrig(int fifo_pretrig) { fifo_pretrig_ = fifo_pretrig; }
  inline int  GetFifoPreTrig() { return fifo_pretrig_; }
  //fifo_pretrig = [0-31] -> number of time bins before TMB pretrigger to begin data readout window (bx)
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  inline void SetTmbSyncErrEnable(int tmb_sync_err_enable) { tmb_sync_err_enable_ = tmb_sync_err_enable; }
  inline int  GetTmbSyncErrEnable() { return tmb_sync_err_enable_; }
  //tmb_sync_err_enable = [0-3]... 2 bit mask, 1 bit per LCT -> each bit [0,1] = [disable,enable] sync_err to MPC
  //
  inline void SetTmbAllowAlct(int tmb_allow_alct) { tmb_allow_alct_ = tmb_allow_alct; }
  inline int  GetTmbAllowAlct() { return tmb_allow_alct_; }
  //tmb_allow_alct = 1 = ALCT only trigger
  //
  inline void SetTmbAllowClct(int tmb_allow_clct) { tmb_allow_clct_ = tmb_allow_clct; }
  inline int  GetTmbAllowClct() { return tmb_allow_clct_; }
  //tmb_allow_clct = 1 = CLCT only trigger
  //
  inline void SetTmbAllowMatch(int tmb_allow_match) { tmb_allow_match_ = tmb_allow_match; }
  inline int  GetTmbAllowMatch() { return tmb_allow_match_; }
  //tmb_allow_match = 1 = ALCT*CLCT trigger
  //
  inline void SetMpcDelay(int mpc_delay) {mpc_delay_= mpc_delay;}
  inline int  GetMPCdelay() { return mpc_delay_; }
  //mpc_delay = [0-15] -> MPC accept bit delay
  //
  inline void SetSelectMpcTtcBx0(int mpc_sel_ttc_bx0) { mpc_sel_ttc_bx0_ = mpc_sel_ttc_bx0; }
  inline int  GetSelectMpcTtcBx0() { return mpc_sel_ttc_bx0_; }
  //mpc_sel_ttc_bx0 = [0,1] -> BX0 for MPC comes from [local,TTC]
  //
  inline void SetMpcIdleBlank(int mpc_idle_blank) { mpc_idle_blank_ = mpc_idle_blank; }
  inline int  GetMpcIdleBlank() { return mpc_idle_blank_; }
  //mpc_idle_blank = 1 = blank MPC data and BX0 except when triggered
  //
  inline void SetMpcOutputEnable(int mpc_output_enable) { mpc_output_enable_ = mpc_output_enable; }
  inline int  GetMpcOutputEnable() { return mpc_output_enable_; }
  //mpc_output_enable = [0,1] = [disable,enable] output (of LCT) to MPC 
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  // Values in this register which are not set by SetTrgMode are:  
  inline void SetClctFlushDelay(int clct_flush_delay) { clct_flush_delay_ = clct_flush_delay; }
  inline int  GetClctFlushDelay() { return clct_flush_delay_; }
  //clct_flush_delay_ = [0-15] = delay to flush the trigger sequencer
  //
  inline void SetClctTurbo(int clct_turbo) { clct_turbo_ = clct_turbo; }
  inline int  GetClctTurbo() { return clct_turbo_; }
  //clct_turbo = 1 = trigger sequencer turbo mode (no raw hits)
  //
  inline void SetRandomLctEnable(int ranlct_enable) { ranlct_enable_ = ranlct_enable; }
  inline int  GetRandomLctEnable() { return ranlct_enable_; }
  //ranlct_enable_ = 1 = enable OSU random LCT generator (see TMB documentation)
  //
  inline void SetWriteBufferRequired(int wrt_buf_required) { wrt_buf_required_ = wrt_buf_required; }
  inline int  GetWriteBufferRequired() { return wrt_buf_required_; }
  //wrt_buf_required_ = 1 = require wr_buffer available to pretrigger
  //
  inline void SetRequireValidClct(int valid_clct_required) { valid_clct_required_ = valid_clct_required; }
  inline int  GetRequireValidClct() { return valid_clct_required_; }
  //valid_clct_required_ = 1 = require valid CLCT after drift delay
  //
  inline void SetL1aAllowMatch(int l1a_allow_match) { l1a_allow_match_ = l1a_allow_match; }
  inline int  GetL1aAllowMatch() { return l1a_allow_match_; }
  //l1a_allow_match = 1 = readout allows tmb trigger pulse in L1A window
  //
  inline void SetL1aAllowNoTmb(int l1a_allow_notmb) { l1a_allow_notmb_ = l1a_allow_notmb; }
  inline int  GetL1aAllowNoTmb() { return l1a_allow_notmb_; }
  //l1a_allow_notmb = 1 = readout allows notmb trigger pulse in L1A window
  //
  inline void SetL1aAllowNoL1a(int l1a_allow_nol1a) { l1a_allow_nol1a_ = l1a_allow_nol1a; }
  inline int  GetL1aAllowNoL1a() { return l1a_allow_nol1a_; }
  //l1a_allow_nol1a = 1 = readout allows tmb trig pulse outside L1A window
  //
  inline void SetL1aAllowAlctOnly(int l1a_allow_alct_only) { l1a_allow_alct_only_ = l1a_allow_alct_only; }
  inline int  GetL1aAllowAlctOnly() { return l1a_allow_alct_only_; }
  //l1a_allow_alct_only = 1 = allow ALCT-only events to readout at L1A
  //
  inline void SetScintillatorVetoClear(int scint_veto_clr) { scint_veto_clr_ =  scint_veto_clr; }
  inline int  GetScintillatorVetoClear() { return scint_veto_clr_; }
  //scint_veto_clr = 1 = clear scintillator veto 
  //
  //---------------------------------------------------------------------
  //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
  //---------------------------------------------------------------------
  inline void SetEnableLayerTrigger(int layer_trigger_en) { layer_trigger_en_ = layer_trigger_en; } 
  inline int  GetEnableLayerTrigger() { return layer_trigger_en_; } 
  //layer_trigger_en = 1 = enable layer trigger mode
  //
  inline void SetLayerTriggerThreshold(int layer_trig_thresh) { layer_trig_thresh_ = layer_trig_thresh; }
  inline int  GetLayerTriggerThreshold() { return layer_trig_thresh_; }
  //layer_trig_thresh = [0-6] = number of layers required for layer trigger
  //
  // ******************************************
  // special configuration registers
  // ******************************************
  //------------------------------------------------------------------
  //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
  //------------------------------------------------------------------
  inline void SetEnableL1aRequestOnAlctExtTrig(int alct_ext_trig_l1aen) { alct_ext_trig_l1aen_ = alct_ext_trig_l1aen; }
  inline int  GetEnableL1aRequestOnAlctExtTrig() { return alct_ext_trig_l1aen_; }
  //alct_ext_trig_l1aen = 1 = request CCB L1A on alct_ext_trig
  //
  inline void SetEnableL1aRequestOnClctExtTrig(int clct_ext_trig_l1aen) { clct_ext_trig_l1aen_ = clct_ext_trig_l1aen; }
  inline int  GetEnableL1aRequestOnClctExtTrig() { return clct_ext_trig_l1aen_; }
  //clct_ext_trig_l1aen = 1 = request CCB L1A on clct_ext_trig
  //
  inline void SetRequestL1a(int request_l1a) { request_l1a_ = request_l1a; }
  inline int  GetRequestL1a() { return request_l1a_; }
  //request_l1a = 1 = request CCB L1A on sequencer trigger
  //
  inline void SetAlctExtTrigVme(int alct_ext_trig_vme) { alct_ext_trig_vme_ = alct_ext_trig_vme; }
  inline int  GetAlctExtTrigVme() { return alct_ext_trig_vme_; }
  //alct_ext_trig_vme = 1 = Fire alct_ext_trig oneshot
  //
  inline void SetClctExtTrigVme(int clct_ext_trig_vme) { clct_ext_trig_vme_ = clct_ext_trig_vme; }
  inline int  GetClctExtTrigVme() { return clct_ext_trig_vme_; }
  //clct_ext_trig_vme = 1 = Fire clct_ext_trig oneshot
  //
  inline void SetExtTrigBoth(int ext_trig_both) { ext_trig_both_ = ext_trig_both; }
  inline int  GetExtTrigBoth() { return ext_trig_both_; }
  //ext_trig_both = 1 = clct_ext_trig fires ALCT + alct_ext_trig fires CLCT
  //
  inline void SetCcbAllowExternalBypass(int ccb_allow_bypass) { ccb_allow_bypass_ = ccb_allow_bypass; }
  inline int  GetCcbAllowExternalBypass() { return ccb_allow_bypass_; }
  //ccb_allow_bypass = 1 = allow clct_exttrig_ccb even though ccb_ignore_rx=1 (address 0x2A)
  //
  inline void SetIgnoreCcbStartStop(int ignore_ccb_startstop) { ignore_ccb_startstop_ = ignore_ccb_startstop; }
  inline int  GetIgnoreCcbStartStop() { return ignore_ccb_startstop_; }
  //ignore_ccb_startstop = 1 = ignore ttc_trig_start, ttc_trig_stop
  //
  inline void SetInternalL1aDelay(int internal_l1a_delay_vme) { internal_l1a_delay_vme_ = internal_l1a_delay_vme; }
  inline int  GetInternalL1aDelay() { return internal_l1a_delay_vme_; }
  //internal_l1a_delay_vme = [0-255]
  //
  //------------------------------------------------------------------
  //0X30 = ADR_ALCT_CFG:  ALCT Configuration
  //------------------------------------------------------------------
  inline void SetEnableAlctExtTrig(int cfg_alct_ext_trig_en) { cfg_alct_ext_trig_en_ = cfg_alct_ext_trig_en; }
  inline int  GetEnableAlctExtTrig() { return cfg_alct_ext_trig_en_; }
  //cfg_alct_ext_trig_en = 1 = enable alct_ext_trig from CCB
  //
  inline void SetEnableAlctExtInject(int cfg_alct_ext_inject_en) { cfg_alct_ext_inject_en_ = cfg_alct_ext_inject_en; }
  inline int  GetEnableAlctExtInject() { return cfg_alct_ext_inject_en_; }
  //cfg_alct_ext_inject_en = 1 = enable alct_ext_inject from CCB
  //
  inline void SetAlctExtTrig(int cfg_alct_ext_trig) { cfg_alct_ext_trig_ = cfg_alct_ext_trig; }
  inline int  GetAlctExtTrig() { return cfg_alct_ext_trig_; }
  //cfg_alct_ext_trig = 1 = assert alct_ext_trig
  //
  inline void SetAlctExtInject(int cfg_alct_ext_inject) { cfg_alct_ext_inject_ = cfg_alct_ext_inject; }
  inline int  GetAlctExtInject() { return cfg_alct_ext_inject_; }
  //cfg_alct_ext_inject = 1 = assert alct_ext_inject
  //
  inline void SetAlctSequencerCommand(int alct_seq_cmd) { alct_seq_cmd_ = alct_seq_cmd; }
  inline int  GetAlctSequencerCommand() { return alct_seq_cmd_; }
  //alct_seq_cmd = [0-7]
  //
  inline void SetEnableAlctUseCcbClock(int alct_clock_en_use_ccb) { alct_clock_en_use_ccb_ = alct_clock_en_use_ccb; }
  inline int  GetEnableAlctUseCcbClock() { return alct_clock_en_use_ccb_; }
  //alct_clock_en_use_ccb = 1 => alct_clock_en_vme = ccb_clock40_enable
  //
  inline void SetAlctClockVme(int alct_clock_en_use_vme) { alct_clock_en_use_vme_ = alct_clock_en_use_vme; }
  inline int  GetAlctClockVme() { return alct_clock_en_use_vme_; }
  //alct_clock_en_use_vme = [0,1] = alct_clock_en scsi signal if GetEnableAlctUseCcbClock() = 0;
  //
  //------------------------------------------------------------------
  //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
  //------------------------------------------------------------------
  inline void SetTmbSlot(int tmb_slot) { tmb_slot_ = tmb_slot; }
  inline int  GetTmbSlot() { return tmb_slot_; }
  //tmb_slot = [2-20] (even) VME slot for this TMB
  //
  inline void SetCscId(int csc_id) { csc_id_ = csc_id; }
  inline int  GetCscId() { return csc_id_; }
  //csc_id = [1-10] index of CSC within this crate (tmb_slot/2) 
  //
  inline void SetRunId(int run_id) { run_id_ = run_id; }
  inline int  GetRunId() { return run_id_; }
  //
  // ******************************************
  // clock phases
  // ******************************************
  //------------------------------------------------------------------
  //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  inline void SetAlctTXclockDelay(int alct_tx_clock_delay) { alct_tx_clock_delay_ = alct_tx_clock_delay; }
  inline int  GetALCTtxPhase() { return alct_tx_clock_delay_; }
  //alct_tx_clock_delay = [0-15] (2ns)
  //
  inline void SetAlctRXclockDelay(int alct_rx_clock_delay) {alct_rx_clock_delay_ = alct_rx_clock_delay;}
  inline int  GetALCTrxPhase() { return alct_rx_clock_delay_; }
  //alct_rx_clock_delay = [0-15] (2ns)
  //
  inline void SetDmbTxDelay(int dmb_tx_delay) { dmb_tx_delay_ = dmb_tx_delay; }
  inline int  GetDmbTxDelay() { return dmb_tx_delay_; }
  //dmb_tx_delay = [0-15] (2ns)
  //
  inline void SetRatTmbDelay(int rat_tmb_delay) { rat_tmb_delay_ = rat_tmb_delay; }
  inline int  GetRatTmbDelay() { return rat_tmb_delay_ ; }
  //rat_tmb_delay = [0-15] (2ns)
  //
  //------------------------------------------------------------------
  //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  inline void SetTmb1Phase(int tmb1_phase) { tmb1_phase_ = tmb1_phase; }
  inline int  GetTmb1Phase() { return tmb1_phase_; }
  //tmb1_phase = [0-15] (2ns)
  //
  inline void SetMpcPhase(int mpc_phase) { mpc_phase_ = mpc_phase; }
  inline int  GetMpcPhase() { return mpc_phase_; }
  //mpc_phase = [0-15] (2ns)
  //
  inline void SetDccPhase(int dcc_phase) { dcc_phase_ = dcc_phase; }
  inline int  GetDccPhase() { return dcc_phase_; }
  //dcc_phase = [0-15] (2ns) (CFEB Duty Cycle Correction)
  //
  inline void SetCFEB0delay(int cfeb0delay) { cfeb0delay_ = cfeb0delay; }
  inline int  GetCFEB0delay() { return cfeb0delay_; }
  //cfeb0delay = [0-15] (2ns)
  //
  //------------------------------------------------------------------
  //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  inline void SetCFEB1delay(int cfeb1delay) { cfeb1delay_ = cfeb1delay; }
  inline int  GetCFEB1delay() { return cfeb1delay_; }
  //cfeb1delay = [0-15] (2ns)
  //
  inline void SetCFEB2delay(int cfeb2delay) { cfeb2delay_ = cfeb2delay; }
  inline int  GetCFEB2delay(){return cfeb2delay_;}
  //cfeb2delay = [0-15] (2ns)
  //
  inline void SetCFEB3delay(int cfeb3delay) { cfeb3delay_ = cfeb3delay; }
  inline int  GetCFEB3delay() { return cfeb3delay_; }
  //cfeb3delay = [0-15] (2ns)
  //
  inline void SetCFEB4delay(int cfeb4delay) { cfeb4delay_ = cfeb4delay; }
  inline int  GetCFEB4delay() { return cfeb4delay_; }
  //cfeb4delay = [0-15] (2ns)
  //
  //------------------------------------------------------------------
  //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
  //------------------------------------------------------------------
  inline void SetRpc0RatDelay(int rpc0_rat_delay) { rpc0_rat_delay_ = rpc0_rat_delay; }
  inline int  GetRpc0RatDelay() { return rpc0_rat_delay_ ; }
  //rpc0_rat_delay = [0-15] (2ns)
  //
  inline void SetRpc1RatDelay(int rpc1_rat_delay) { rpc1_rat_delay_ = rpc1_rat_delay; }
  inline int  GetRpc1RatDelay() { return rpc1_rat_delay_ ; }
  //rpc1_rat_delay = [0-15] (2ns)
  //
  //
  //Return the software value to be written into the register at "address" 
  //The bits are set according to the database values, set by the "Set...(int data)" 
  //methods, above:
  int  FillTMBRegister(unsigned long int address); 
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
  //
  //-- read and program user proms --// 
  void ClockOutPromProgram(int prom,int number_of_addresses);  //prom=[ChipLocationTmbUserPromTMB, ChipLocationTmbUserPromALCT]
  inline int GetClockedOutPromImage(int address) { return clocked_out_prom_image_.at(address); }
  //  inline int SizeOfClockedOutPromImage() { return clocked_out_prom_image_.size(); }
  //
  void OkVmeWrite(char vme_address);                    // allowed to write this address into user prom?
  void ClearVmeWriteVecs();                             
  void SetFillVmeWriteVecs(bool fill_vectors_or_not);   // put the vme information into vectors to put into user prom?
  bool GetFillVmeWriteVecs();
  std::vector<int> GetVecVmeAddress();               // vme write information
  std::vector<int> GetVecDataLsb();                  // vme write information
  std::vector<int> GetVecDataMsb();                  // vme write information
  //
  //
  //-- read groups of TMB registers --//
  void DumpAllRegisters();
  void ReadTMBConfiguration();
  std::vector<unsigned long int> TMBConfigurationRegister;
  void ReadVMEStateMachine();
  void ReadJTAGStateMachine();
  void ReadRawHitsHeader();
  void ReadDDDStateMachine();
  //
  //
  //-- print out read values of groups of TMB registers to screen --//
  void PrintTMBConfiguration();
  void PrintHotChannelMask();
  void PrintVMEStateMachine();
  void PrintJTAGStateMachine();
  void PrintRawHitsHeader();
  void PrintDDDStateMachine();
  //
  void PrintTMBRegister(unsigned long int address);  
  void PrintFirmwareDate();                          
  void PrintBootRegister();                          
  //
  //
  //-- compare read values with expected values --//
  void CheckTMBConfiguration();
  inline int GetTMBConfigurationStatus() { return tmb_configuration_status_; }
  //
  void CheckRawHitsHeader();
  inline int GetRawHitsHeaderStatus() { return raw_hits_header_status_; }
  //
  void CheckVMEStateMachine();
  inline int GetVMEStateMachineStatus() { return vme_state_machine_status_; }
  //
  void CheckJTAGStateMachine();
  inline int GetJTAGStateMachineStatus() { return jtag_state_machine_status_; }
  //
  void CheckDDDStateMachine();
  inline int GetDDDStateMachineStatus() { return ddd_state_machine_status_; }
  //
  //
  //-- integer parsing routines --//
  // insert "value" into the value of the variable pointed at by "data_word" from "lobit" (LSB) to "hibit" (MSB):
  void InsertValueIntoDataWord(int value, int hibit, int lobit, int * data_word);
  //
  // return the value located in "data" which resides from "lobit" to "hibit"
  int ExtractValueFromData(int data, int lobit, int hibit);
  //
  // return a mask of the right size for the specified number of bits
  int makemask(int lo_bit, int hi_bit);
  //
  // convert the argument to its "hex-ascii" value:  i.e.  2007 -> 0x2007
  int ConvertToHexAscii(int value_to_convert);
  //
  //-- register comparison methods --//
  // test if "testval" is equal expected value: "compareval"
  bool compareValues(std::string typeOfTest, int testval, int compareval);    
  //
  // this method allows return to depend on if you want them to be "equalOrNot"
  bool compareValues(std::string typeOfTest, int testval, int compareval, bool equalOrNot);
  //
  //test if "testval" is within a fractional "tolerance" of "compareval"
  bool compareValues(std::string typeOfTest, float testval, float compareval, float tolerance);
  //
  void ReportCheck(std::string check_type, bool status_bit);    //report status of "check_type" to logger, (*MyOutput_) according to "status_bit"
  //
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
  //
  std::ostream * MyOutput_ ;
  bool debug_;
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
  static const int MaxCounter = 0x2f;
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
  //-- TMB data in raw hits VME readout --//
  std::vector< std::bitset<16> > tmb_data_;
  //
  //-- program in user prom --//
  std::vector<int> clocked_out_prom_image_;
  //
  //-- TMB status values --//
  int tmb_configuration_status_;
  int vme_state_machine_status_;
  int jtag_state_machine_status_;
  int ddd_state_machine_status_;
  int raw_hits_header_status_;
  //
  //*******************************************************************
  // Firmware tags:
  //*******************************************************************
  int tmb_firmware_version_;
  int tmb_firmware_revcode_;
  int tmb_firmware_type_;
  int rat_firmware_month_;
  int rat_firmware_day_;
  int rat_firmware_year_;
  //
  int read_tmb_firmware_version_;
  int read_tmb_firmware_revcode_;
  int read_tmb_firmware_type_;
  int read_rat_firmware_month_;
  int read_rat_firmware_day_;
  int read_rat_firmware_year_;
  //
  //
  //*******************************************************************
  // Definitions of TMB VME registers:
  //*******************************************************************
  void DefineTMBConfigurationRegisters_();      //define the registers which define the TMB configuration 
  //                                              (i.e., those that are written into the user PROM)
  void SetTMBRegisterDefaults_();               //set the software write values for TMB registers to default values
  //
  //
  void DecodeTMBRegister_(unsigned long int address, int data);  //parse "data" according to the bit map corresponding to "address"
  void DecodeBootRegister_(int data);                            //parse "data" according to the bit map for the boot register
  //
  //-----------------------------------------------------------------
  //settings which are combinations of registers
  //-----------------------------------------------------------------
  void SetTrgmode_();                  //define TMB registers which are combinations of the database value "trgmode_"
  int trgmode_;     
  //
  int read_trgmode_;
  //
  //-----------------------------------------------------------------
  //0X70000 = ADR_BOOT:  Hardware Bootstrap Register
  //-----------------------------------------------------------------
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
  //-----------------------------------------------------------------
  //0X02 = ADR_IDREG1:  ID Register 1
  //-----------------------------------------------------------------
  int tmb_firmware_month_;
  int tmb_firmware_day_;
  //
  int read_firmware_date_;
  int read_tmb_firmware_month_;
  int read_tmb_firmware_day_;
  //
  //-----------------------------------------------------------------
  //0X04 = ADR_IDREG2:  ID Register 2
  //-----------------------------------------------------------------
  int tmb_firmware_year_;
  //
  int read_tmb_firmware_year_;
  //
  //-----------------------------------------------------------------
  //0X0E = ADR_LOOPBK:  Loop-Back Control Register
  //-----------------------------------------------------------------
  int ALCT_input_;
  int enable_alct_tx_;
  //
  int read_cfeb_oe_;
  int read_alct_loop_;
  int read_ALCT_input_;
  int read_enable_alct_tx_;
  int read_rpc_loop_rat_;
  int read_rpc_loop_tmb_;
  int read_dmb_loop_;
  int read_dmb_oe_;
  int read_gtl_loop_;
  int read_gtl_oe_;
  //
  //------------------------------------------------------------------
  //0X14 = ADR_DDDSM:  3D3444 State Machine Control + DCM Lock Status  
  //------------------------------------------------------------------
  int ddd_state_machine_start_;
  int ddd_state_machine_manual_;
  int ddd_state_machine_latch_;
  int ddd_state_machine_serial_in_;
  int ddd_state_machine_serial_out_;
  int ddd_state_machine_autostart_;
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
  //------------------------------------------------------------------
  //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  int alct_tx_clock_delay_;
  int alct_rx_clock_delay_;
  int dmb_tx_delay_;
  int rat_tmb_delay_;
  //
  int read_alct_tx_clock_delay_;
  int read_alct_rx_clock_delay_;
  int read_dmb_tx_delay_;
  int read_rat_tmb_delay_;
  //
  //------------------------------------------------------------------
  //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  int tmb1_phase_;
  int mpc_phase_;
  int dcc_phase_;
  int cfeb0delay_;
  //
  int read_tmb1_phase_;
  int read_mpc_phase_;
  int read_dcc_phase_;
  int read_cfeb0delay_;
  //
  //------------------------------------------------------------------
  //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  int cfeb1delay_;
  int cfeb2delay_;
  int cfeb3delay_;
  int cfeb4delay_;
  //
  int read_cfeb1delay_;
  int read_cfeb2delay_;
  int read_cfeb3delay_;
  int read_cfeb4delay_;
  //
  //------------------------------------------------------------------
  //0X1E = ADR_RATCTRL:  RAT Module Control
  //------------------------------------------------------------------
  int rpc_sync_;
  int shift_rpc_;
  int rat_dsn_en_;
  //
  int read_rpc_sync_;
  int read_shift_rpc_;
  int read_rat_dsn_en_;
  //
  //------------------------------------------------------------------
  //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
  //------------------------------------------------------------------
  int alct_ext_trig_l1aen_;
  int clct_ext_trig_l1aen_;
  int request_l1a_;
  int alct_ext_trig_vme_;
  int clct_ext_trig_vme_;
  int ext_trig_both_;
  int ccb_allow_bypass_;
  int ignore_ccb_startstop_;
  int internal_l1a_delay_vme_;
  //
  int read_alct_ext_trig_l1aen_;
  int read_clct_ext_trig_l1aen_;
  int read_request_l1a_;
  int read_alct_ext_trig_vme_;
  int read_clct_ext_trig_vme_;
  int read_ext_trig_both_;
  int read_ccb_allow_bypass_;
  int read_ignore_ccb_startstop_;
  int read_internal_l1a_delay_vme_;
  //
  //------------------------------------------------------------------
  //0X30 = ADR_ALCT_CFG:  ALCT Configuration
  //------------------------------------------------------------------
  int cfg_alct_ext_trig_en_;  
  int cfg_alct_ext_inject_en_;
  int cfg_alct_ext_trig_;    
  int cfg_alct_ext_inject_;  
  int alct_seq_cmd_;         
  int alct_clock_en_use_ccb_;
  int alct_clock_en_use_vme_;
  //
  int read_cfg_alct_ext_trig_en_;  
  int read_cfg_alct_ext_inject_en_;
  int read_cfg_alct_ext_trig_;    
  int read_cfg_alct_ext_inject_;  
  int read_alct_seq_cmd_;         
  int read_alct_clock_en_use_ccb_;
  int read_alct_clock_en_use_vme_;
  //
  //------------------------------------------------------------------
  //0X32 = ADR_ALCT_INJ:  ALCT Injector Control
  //------------------------------------------------------------------
  int alct_clear_;
  int alct_inject_mux_;
  int alct_sync_clct_;
  int alct_inj_delay_;
  //
  int read_alct_clear_;
  int read_alct_inject_mux_;
  int read_alct_sync_clct_;
  int read_alct_inj_delay_;
  //
  //------------------------------------------------------------------
  //0X42 = ADR_CFEB_INJ:  CFEB Injector Control
  //------------------------------------------------------------------
  int disableCLCTInputs_;        //remove?
  //
  int enableCLCTInputs_;
  int cfeb_ram_sel_;
  int cfeb_inj_en_sel_;
  int start_pattern_inj_;
  //
  int read_enableCLCTInputs_;
  int read_cfeb_ram_sel_;
  int read_cfeb_inj_en_sel_;
  int read_start_pattern_inj_;
  //
  //------------------------------------------------------------------
  //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
  //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
  //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
  //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
  //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
  //------------------------------------------------------------------
  int GetHotChannelLayerFromMap_(unsigned long int vme_address, int bit_in_register);
  int GetHotChannelDistripFromMap_(unsigned long int vme_address, int bit_in_register);
  //
  int hot_channel_mask_[MAX_NUM_LAYERS][MAX_NUM_DISTRIPS_PER_LAYER];
  //
  int read_hot_channel_mask_[MAX_NUM_LAYERS][MAX_NUM_DISTRIPS_PER_LAYER];
  //
  //------------------------------------------------------------------
  //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
  //------------------------------------------------------------------
  int clct_pat_trig_en_;
  int alct_pat_trig_en_;
  int match_pat_trig_en_;
  int adb_ext_trig_en_;
  int dmb_ext_trig_en_;
  int clct_ext_trig_en_;
  int alct_ext_trig_en_;
  int vme_ext_trig_;
  int ext_trig_inject_;
  int all_cfeb_active_;
  int cfebs_enabled_;
  int cfeb_enable_source_;
  //
  int read_clct_pat_trig_en_;
  int read_alct_pat_trig_en_;
  int read_match_pat_trig_en_;
  int read_adb_ext_trig_en_;
  int read_dmb_ext_trig_en_;
  int read_clct_ext_trig_en_;
  int read_alct_ext_trig_en_;
  int read_vme_ext_trig_;
  int read_ext_trig_inject_;
  int read_all_cfeb_active_;
  int read_cfebs_enabled_;
  int read_cfeb_enable_source_;
  //
  //------------------------------------------------------------------
  //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  int alct_pretrig_width_;
  int alct_pretrig_delay_;
  int alct_pattern_delay_;
  int adb_ext_trig_delay_;
  //
  int read_alct_pretrig_width_;
  int read_alct_pretrig_delay_;
  int read_alct_pattern_delay_;
  int read_adb_ext_trig_delay_;
  //
  //------------------------------------------------------------------
  //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  int dmb_ext_trig_delay_ ;
  int clct_ext_trig_delay_;
  int alct_ext_trig_delay_;
  int layer_trig_delay_   ;
  //
  int read_dmb_ext_trig_delay_ ;
  int read_clct_ext_trig_delay_;
  int read_alct_ext_trig_delay_;
  int read_layer_trig_delay_   ;
  //
  //------------------------------------------------------------------
  //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
  //------------------------------------------------------------------
  int tmb_slot_;
  int csc_id_;
  int run_id_;
  //
  int read_tmb_slot_;
  int read_csc_id_;
  int read_run_id_;
  //
  //------------------------------------------------------------------
  //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
  //------------------------------------------------------------------
  int triad_persist_;
  int hs_pretrig_thresh_;
  int ds_pretrig_thresh_;
  int min_hits_pattern_;
  int drift_delay_;
  int pretrigger_halt_;
  //
  int read_triad_persist_;
  int read_hs_pretrig_thresh_;
  int read_ds_pretrig_thresh_;
  int read_min_hits_pattern_;
  int read_drift_delay_;
  int read_pretrigger_halt_;
  //
  //------------------------------------------------------------------
  //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
  //------------------------------------------------------------------
  int fifo_mode_;
  int fifo_tbins_;
  int fifo_pretrig_;
  //
  int read_fifo_mode_;
  int read_fifo_tbins_;
  int read_fifo_pretrig_;
  //
  //------------------------------------------------------------------
  //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
  //------------------------------------------------------------------
  int l1adelay_;
  int l1a_window_size_;
  int tmb_l1a_internal_;
  //
  int read_l1adelay_;
  int read_l1a_window_size_;
  int read_tmb_l1a_internal_;
  //
  //------------------------------------------------------------------
  //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
  //------------------------------------------------------------------
  int l1a_offset_;
  int bxn_offset_;
  //
  int read_l1a_offset_;
  int read_bxn_offset_;
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  int tmb_sync_err_enable_;
  int tmb_allow_alct_;
  int tmb_allow_clct_;
  int tmb_allow_match_;
  int mpc_delay_;
  int mpc_sel_ttc_bx0_;
  int mpc_idle_blank_;
  int mpc_output_enable_;
  //
  int read_tmb_sync_err_enable_;
  int read_tmb_allow_alct_;
  int read_tmb_allow_clct_;
  int read_tmb_allow_match_;
  int read_mpc_delay_;
  int read_mpc_accept_;
  int read_mpc_reserved_;
  int read_mpc_sel_ttc_bx0_;
  int read_mpc_idle_blank_;
  int read_mpc_output_enable_;
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  int clct_flush_delay_;
  int clct_turbo_;
  int ranlct_enable_;
  int wrt_buf_required_;
  int valid_clct_required_;
  int l1a_allow_match_;
  int l1a_allow_notmb_;
  int l1a_allow_nol1a_;
  int l1a_allow_alct_only_;
  int scint_veto_clr_;
  //
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
  int read_scint_veto_vme_;
  //
  //------------------------------------------------------------------
  //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
  //------------------------------------------------------------------
  int alct_vpf_delay_;
  int alct_match_window_size_;
  int mpc_tx_delay_;
  //
  int read_alct_vpf_delay_;
  int read_alct_match_window_size_;
  int read_mpc_tx_delay_;
  //
  //------------------------------------------------------------------
  //0XB6 = ADR_RPC_CFG:  RPC Configuration
  //------------------------------------------------------------------
  int rpc_exists_;
  int rpc_read_enable_;
  int rpc_bxn_offset_;
  int rpc_bank_;
  //
  int read_rpc_exists_;
  int read_rpc_read_enable_;
  int read_rpc_bxn_offset_;
  int read_rpc_bank_;
  int read_rpc_rbxn_;
  int read_rpc_done_;
  //
  //------------------------------------------------------------------
  //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
  //------------------------------------------------------------------
  int rpc0_raw_delay_;
  int rpc1_raw_delay_;
  //
  int read_rpc0_raw_delay_;
  int read_rpc1_raw_delay_;
  //
  //------------------------------------------------------------------
  //0XBC = ADR_RPC_INJ:  RPC Injector Control
  //------------------------------------------------------------------
  int rpc_mask_all_ ;
  int inj_mask_rat_ ;
  int inj_mask_rpc_ ;
  int inj_delay_rat_;
  int rpc_inj_sel_  ;
  int rpc_inj_wdata_;
  //
  int read_rpc_mask_all_ ;
  int read_inj_mask_rat_ ;
  int read_inj_mask_rpc_ ;
  int read_inj_delay_rat_;
  int read_rpc_inj_sel_  ;
  int read_rpc_inj_wdata_;
  int read_rpc_inj_rdata_;
  //
  //------------------------------------------------------------------
  //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
  //------------------------------------------------------------------
  int jtag_state_machine_start_;
  int jtag_state_machine_sreset_;
  int jtag_state_machine_throttle_;
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
  //------------------------------------------------------------------
  //0XD6 = ADR_JTAGSM1:  JTAG State Machine Word Count
  //------------------------------------------------------------------
  int read_jtag_state_machine_word_count_;
  //
  //------------------------------------------------------------------
  //0XD8 = ADR_JTAGSM2:  JTAG State Machine Checksum
  //------------------------------------------------------------------
  int read_jtag_state_machine_check_sum_;
  int read_jtag_state_machine_tck_fpga_;
  //
  //------------------------------------------------------------------
  //0XDA = ADR_VMESM0:  VME State Machine Control (reads VME PROM)
  //------------------------------------------------------------------
  int vme_state_machine_start_;
  int vme_state_machine_sreset_;
  int vme_state_machine_jtag_auto_; 
  int vme_state_machine_throttle_; 
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
  //------------------------------------------------------------------
  //0XDC = ADR_VMESM1:  VME State Machine Word Count
  //------------------------------------------------------------------
  int read_vme_state_machine_word_count_;
  //
  //------------------------------------------------------------------
  //0XDE = ADR_VMESM2:  VME State Machine Checksum
  //------------------------------------------------------------------
  int read_vme_state_machine_check_sum_;
  int read_vme_state_machine_error_missing_header_start_;
  int read_vme_state_machine_error_missing_header_end_;
  int read_vme_state_machine_error_missing_data_end_marker_;
  int read_vme_state_machine_error_missing_trailer_end_;
  int read_vme_state_machine_error_word_count_overflow_;
  //
  //------------------------------------------------------------------
  //0XE0 = ADR_VMESM3:  Number of VME addresses written by VMESM
  //------------------------------------------------------------------
  int read_vme_state_machine_number_of_vme_writes_;
  //
  //------------------------------------------------------------------
  //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
  //------------------------------------------------------------------
  int rpc0_rat_delay_;
  int rpc1_rat_delay_;
  //
  int read_rpc0_rat_delay_;
  int read_rpc1_rat_delay_;
  //
  //---------------------------------------------------------------------
  //0XEA = ADR_BDSTATUS:  Board Status Summary (copy of raw-hits header)
  //---------------------------------------------------------------------
  int read_bdstatus_ok_;
  int read_vstat_5p0v_;
  int read_vstat_3p3v_;
  int read_vstat_1p8v_;
  int read_vstat_1p5v_;
  int read_nt_crit_;
  int read_vsm_ok_;
  int read_vsm_aborted_;
  int read_vsm_cksum_ok_;
  int read_vsm_wdcnt_ok_;
  int read_jsm_ok_;
  int read_jsm_aborted_;
  int read_jsm_cksum_ok_;
  int read_jsm_wdcnt_ok_;
  int read_jsm_tck_fpga_ok_;
  //
  //---------------------------------------------------------------------
  //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
  //---------------------------------------------------------------------
  int layer_trigger_en_ ;
  int layer_trig_thresh_;
  //
  int read_layer_trigger_en_ ;
  int read_layer_trig_thresh_;
  int read_number_layers_hit_;
  //
};

#endif

