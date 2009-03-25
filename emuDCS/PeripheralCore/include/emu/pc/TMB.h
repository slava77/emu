//-----------------------------------------------------------------------
// $Id: TMB.h,v 1.2 2009/03/25 10:19:41 liu Exp $
// $Log: TMB.h,v $
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.62  2009/03/24 17:41:12  rakness
// Sufficient set of ALCT-TMB Loopback tests to determine alct_rx/tx_phase_delay
//
// Revision 3.61  2009/03/19 13:29:41  rakness
// clean up functionality in writing to userPROMs for TMB and ALCT.  This is to fix bug introduced in TMB v3.80 by using tmb_vme_new
//
// Revision 3.60  2009/03/06 16:45:28  rakness
// add methods for ALCT-TMB loopback
//
// Revision 3.59  2008/12/02 09:21:47  rakness
// set default values to be correct for VME register 0xCC
//
// Revision 3.58  2008/11/28 09:49:27  rakness
// include ME1/1 TMB firmware compilation specification into xml file
//
// Revision 3.57  2008/11/24 17:50:40  rakness
// update for TMB version 18 Nov 2008
//
// Revision 3.56  2008/11/17 08:26:59  rakness
// add unjam TMB
//
// Revision 3.55  2008/11/07 13:00:16  liu
// remove warnings
//
// Revision 3.54  2008/09/30 14:27:07  liu
// read ALCT temperature in monitoring
//
// Revision 3.53  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.52  2008/08/08 14:12:21  liu
// fix Get/Set functions for database
//
// Revision 3.51  2008/08/08 11:01:23  rakness
// centralize logging
//
// Revision 3.50  2008/08/06 17:24:50  rakness
// add known_problem parameter to xml file; add time stamp + number of reads to config check output file
//
// Revision 3.49  2008/08/06 12:06:19  liu
// fix TMB/RAT firmware Hex vs Dec problem
//
// Revision 3.48  2008/08/05 08:40:36  rakness
// add minimum number of times to read when checking configuration
//
// Revision 3.47  2008/07/31 13:33:43  liu
// bug fix in TMB counters
//
// Revision 3.46  2008/07/16 17:28:36  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 3.45  2008/07/04 13:22:14  rakness
// add getter for distrip hot channel mask
//
// Revision 3.44  2008/06/23 14:26:09  rakness
// add getter functions
//
// Revision 3.43  2008/06/12 21:08:54  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.42  2008/05/26 08:24:41  rakness
// for AFEB calibrations:  argument for TMB and ALCT::configure(2) to not write userPROMs; correctly respond to configuration written to broadcast slot
//
// Revision 3.41  2008/05/20 11:30:21  liu
// TMB counters in jumbo packet
//
// Revision 3.40  2008/04/19 14:56:55  rakness
// ALCT database check before loading ALCT firmware
//
// Revision 3.39  2008/02/28 18:36:36  rakness
// make TMB firmware loading robust against all failure modes except power cuts...
//
// Revision 3.38  2008/02/23 15:25:55  liu
// TMB online counters
//
// Revision 3.37  2008/01/18 15:42:16  rakness
// get methods for TMB/RAT on-board voltages and currents
//
// Revision 3.36  2008/01/07 15:08:53  rakness
// add xml parameters:  clct_stagger, clct_blanking, clct_pattern_id_thresh, aff_thresh, min_clct_separation.  Remove xml parameter:  clct_distrip_pretrig_thresh
//
// Revision 3.35  2007/12/06 15:12:39  rakness
// make scan parameters for synchronization configurable from hyperDAQ
//
// Revision 3.34  2007/10/25 17:36:12  rakness
// Add option to enable/disable write to USER JTAG register to allow selective masking of broadcast JTAG commands.  Also enable/disable clocks with explicit write rather than read,write
//
// Revision 3.33  2007/10/24 13:21:30  rakness
// try to clean up and document TMB to MPC methods
//
// Revision 3.32  2007/10/08 15:04:32  rakness
// add ALCT and TMB raw hits button in hyperDAQ
//
// Revision 3.31  2007/09/11 11:30:09  rakness
// document TMB counters
//
// Revision 3.30  2007/08/22 13:39:11  rakness
// add distrip hotchannel mask to xml file
//
// Revision 3.29  2007/08/16 11:40:23  rakness
// add Raw Hits Write Buffer Reset Counter
//
// Revision 3.28  2007/08/13 14:17:22  rakness
// allow synchronization of full trigger/DAQ paths with TTC pulsing
//
// Revision 3.27  2007/08/03 14:35:40  rakness
// begin commenting for doxygen, add hot-channel mask write, add writeregister together with fillregister
//
// Revision 3.26  2007/07/26 13:09:34  rakness
// update CFEB rx scan for CLCT key layer 3 -> 2 change
//
// Revision 3.25  2007/06/21 16:14:02  rakness
// online measurement of ALCT in CLCT matching window
//
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

#include "emu/pc/VMEModule.h"
#include <cstdio>
#include <vector>
#include <string>
#include <bitset>

#include "emu/pc/EMUjtag.h"
#include "emu/pc/EmuLogger.h"

namespace emu {
  namespace pc {

class ALCTController;
class TMBParser;
class RAT;
class AnodeChannel;
class Crate;
class Chamber;

class TMB :  public VMEModule, public EMUjtag, public EmuLogger {

public:
  //
  friend class ALCTController;
  //friend class TMBParser;
  friend class EMUjtag;
  //
  explicit TMB(Crate * , Chamber *, int );
  virtual ~TMB();
  //
  //
  void WriteOutput(std::string);
  //
  // from the BOARDTYPE enum
  unsigned int boardType() const {return TMB_ENUM;}
  //
  int  GetWordCount();
  void StartTTC();
  void DumpRegister(int);
  //
  Crate * getCrate();
  Chamber * getChamber();
  //
  //! Write "data" into VME "address"
  void WriteRegister(int address, int data);
  //!return value read from "address"
  int  ReadRegister(int address);
  //! Write data which has been set by Set... methods
  void WriteRegister(int address);
  //
  void DumpAddress(int);
  //
  void DecodeCLCT();
  void PrintCLCT();
  int  GetALCTWordCount();
  void DecodeALCT();
  void PrintALCT();
  //
  //void lctrequestdelay(int dword);
  void clear_i2c();
  void clk_delays(unsigned short int time,int cfeb_id);
  void scope(int scp_arm,int scp_readout, int scp_channel=0x1d);
  void decode();
  //
  //!Extract ALCT raw hits data from VME
  void ALCTRawhits();
  bool ResetALCTRAMAddress();
  bool CheckAlctFIFOBusy(int number_of_checks_before_aborting);
  //
  //!Extract TMB raw hits data from VME
  void TMBRawhits(int microseconds_between_data_reads);
  /// default is 0.1seconds
  void TMBRawhits(); 
  void ResetRAMAddress();
  inline int GetAlctInClctMatchWindow() { return h11_alct_in_clct_match_window_ ; }
  void PrintTMBRawHits();
  //
  //
  void ForceScopeTrigger();
  void init_alct(int choice);
  void load_cscid();
  void DiStripHCMask(int);
  // should have bx window coded in.  See trgmode.
  void lvl1_delay(unsigned short int time);
  void alct_vpf_delay(unsigned short int time);
  void mpc_delay(unsigned short int time);
  void read_delays();
  void reset();
  //void scan_rx_clock();
  void trgmode(int choice);
  void SetALCTPatternTrigger();
  void SetCLCTPatternTrigger();
  //
  //!read the Firmware date from the TMB
  int  FirmwareDate();
  inline int  GetReadTmbFirmwareDay() { return read_tmb_firmware_day_; }
  inline void SetExpectedTmbFirmwareDay(int day) { tmb_firmware_day_ = day; }
  inline int  GetExpectedTmbFirmwareDay() { return tmb_firmware_day_; }
  //
  inline int  GetReadTmbFirmwareMonth() { return read_tmb_firmware_month_; }
  inline void SetExpectedTmbFirmwareMonth(int month) { tmb_firmware_month_ = month; }
  inline int  GetExpectedTmbFirmwareMonth() { return tmb_firmware_month_; }
  //
  //!read the Firmware date from the TMB
  int  FirmwareYear();
  inline int  GetReadTmbFirmwareYear() { return read_tmb_firmware_year_; }
  inline void SetExpectedTmbFirmwareYear(int year) { tmb_firmware_year_ = year; }
  inline int  GetExpectedTmbFirmwareYear() { return tmb_firmware_year_; }
  //
  //!check that the read values are the values expected in the xml file
  bool CheckFirmwareDate();
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
  inline void SetExpectedRatFirmwareDay(int day) { rat_firmware_day_ = day; }
  inline int  GetExpectedRatFirmwareDay() { return rat_firmware_day_; }
  inline void SetExpectedRatFirmwareMonth(int month) { rat_firmware_month_ = month; }
  inline int  GetExpectedRatFirmwareMonth() { return rat_firmware_month_; }
  inline void SetExpectedRatFirmwareYear(int year) { rat_firmware_year_ = year; }
  inline int  GetExpectedRatFirmwareYear() { return rat_firmware_year_; }
  //
  int  PowerComparator();
  //
  // called by TRGMODE, depending on version_
  void setupNewDelayChips();
  void setupOldDelayChips();
  void trgmode_bprsq_alct();
  void trgmode_bprsq_clct(int choice);
  void activecfeb();
  void toggle_l1req();
  //
  void firmwareVersion();
  void setLogicAnalyzerToDataStream(bool yesorno);
  //
  void EnableL1aRequest();
  void DisableL1aRequest();
  //
  void EnableCLCTInputs(int CLCTInputs );
  void DisableCLCTInputs();
  void DisableALCTInputs();
  //
  void DisableALCTCLCTSync();
  //
  void DisableExternalCCB();
  //
  void EnableInternalL1aEmulator();
  void DisableInternalL1aSequencer();
  void EnableInternalL1aSequencer();
  //
  void tmb_clk_delays(unsigned short int time, int cfeb_id);
  //
  int  CCB_command_from_TTC();
  //
  void ExtClctTrigFromCCBonly();
  int ReadTMBtempPCB();  
  int ReadTMBtempFPGA();  
  int ReadTMBtCritPCB();  
  int ReadTMBtCritFPGA();  
  int smb_io(int,int,int);
  //
  // TMB counters
  void ResetCounters();                 /// reset TMB counters
  int * GetCounters();                   /// read TMB counters, fill values in software
  int * NewCounters();                   /// read TMB counters in jumbo packet
  int  GetCounter(int counter);         /// return counter value
  void PrintCounters(int counter=-1);   /// print counter value (-1 means print all)
  std::string CounterName(int counter); /// return counter label
  inline int GetMaxCounter() { return MaxCounter; }
  inline int GetALCTSentToTMBCounterIndex()  { return alct_sent_to_tmb_counter_index_;  }
  inline int GetCLCTPretriggerCounterIndex() { return clct_pretrigger_counter_index_;   }
  inline int GetLCTSentToMPCCounterIndex()   { return lct_sent_to_mpc_counter_index_;   }
  inline int GetL1AInTMBWindowCounterIndex() { return l1a_in_tmb_window_counter_index_; }
  //
  void FireALCTInjector();
  void FireCLCTInjector();
  void ClearALCTInjector();
  void ClearCLCTInjector();
  void ClearScintillatorVeto();
  int  TestArray();
  //
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
  int tmb_get_boot_reg(unsigned short int* value);
  int tmb_set_boot_reg(unsigned short int value);
  int tmb_hard_reset_alct_fpga();
  int tmb_hard_reset_tmb_fpga();
  int tmb_enable_alct_hard_reset(int flag_enable);
  int tmb_enable_vme_commands(int flag_enable);      
  //
  /// mostly for GUI
  void executeCommand(std::string command);      
  friend std::ostream & operator<<(std::ostream & os, TMB & tmb);
  //
  //  int GetCLCT0Cfeb() { return -999; }  //does not exist in TMB firmware anymore
  //  int GetCLCT1Cfeb() { return -999; }  //does not exist in TMB firmware anymore
  //
  int FmState();
  //
  void enableAllClocks();
  void disableAllClocks();
  //
  ////////////////////////
  // The following methods deal with data going from TMB to MPC...
  //
  //!load nEvents worth of MPC data to injector RAM.  The data to load is lct0 and lct1.  If lct0 or lct1 are 0, random data will be generated
  void InjectMPCData(const int nEvents, const unsigned long lct0, const unsigned long lct1);
  //
  //!Read values which will be sent when the injector is fired
  void ReadBackMpcRAM(int nEvents);
  //
  //!Software values of data in TMB injector RAM to MPC
  inline std::vector<unsigned long int> GetInjectedLct0() { return InjectedLct0 ; }
  inline std::vector<unsigned long int> GetInjectedLct1() { return InjectedLct1 ; }
  //
  //!Reset software values of data in TMB injector RAM to MPC
  inline void ResetInjectedLCT() {
    InjectedLct0.clear();
    InjectedLct1.clear();
  }
  //
  //!Send the data in the MPC injector RAM to the MPC
  void FireMPCInjector(int nEvents);
  //
  //!Read back and print the data sent to the MPC
  void DataSendMPC();
  //
  //!Values of "MPC accept" data sent from MPC to TMB 
  int MPC0Accept();
  int MPC1Accept();
  ////////////////////////
  //
  void TriggerTestInjectALCT();
  void TriggerTestInjectCLCT();
  //
  bool SelfTest() ;
  void init() ;
  //
  //!c = 2 = do not write configuration to userPROM
  void configure(int c);
  //
  //!default configure is to write PROMs
  void configure();
  //
  //
  //!device = 0 = TMB, = 1 = mezzanine, = 2 = RAT
  std::bitset<64> dsnRead(int device); 
  //!Read on-board ADCs, then get functions to return values
  void ADCvoltages(float*);
  //
  inline float Get5p0v()     {return v5p0_     ;}
  inline float Get5p0a()     {return a5p0_     ;}
  //
  inline float Get3p3v()     {return v3p3_     ;}
  inline float Get3p3a()     {return a3p3_     ;}
  //
  inline float Get1p5vCore() {return v1p5core_ ;}
  inline float Get1p5aCore() {return a1p5core_ ;}
  //
  inline float Get1p5vTT()   {return v1p5tt_   ;}
  inline float Get1p5aTT()   {return a1p5tt_   ;}
  //
  inline float Get1p0vTT()   {return v1p0_     ;}
  //
  inline float Get1p8vRAT()  {return v1p8rat_  ;}
  inline float Get1p8aRAT()  {return a1p8rat_  ;}	        // if SH921 set 1-2, loop backplane sends 1.500vtt
  //  inline float Get3p3vRAT()  {return v3p3rat_  ;}               // if SH921 set 2-3... otherwise not filled...
  //
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
  // access to TMB VME registers
  ////////////////////////////////////////////////////
  //----------------------------------------------------------------
  //0X70000 = BOOT_REGISTER
  //----------------------------------------------------------------
  inline int GetBootVMEReady() { return read_boot_vme_ready_; }
  //
  //----------------------------------------------------------------
  //0X0E = ADR_LOOPBK:  Loop-Back Control Register:
  //----------------------------------------------------------------
  //!enable_alct_rx = [0,1] -> ALCT rx [off,on]
  inline void SetAlctInput(int enable_alct_rx) { enable_alct_rx_ = enable_alct_rx; }        
  inline int  GetAlctInput() { return enable_alct_rx_;}
  //
  //!enable_alct_tx = [0,1] -> ALCT tx [off,on]
  inline void SetEnableAlctTx(int enable_alct_tx) { enable_alct_tx_ = enable_alct_tx; } 
  inline int  GetEnableAlctTx() { return enable_alct_tx_; }
  //
  //------------------------------------------------------------------
  //0X16 = ADR_DDD0:  3D3444 Chip 0 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  //!alct_tx_clock_delay = [0-15] (2ns)
  inline void SetAlctTXclockDelay(int alct_tx_clock_delay) { alct_tx_clock_delay_ = alct_tx_clock_delay; }
  inline int  GetAlctTXclockDelay() { return alct_tx_clock_delay_; }
  inline int  GetALCTtxPhase() { return alct_tx_clock_delay_; }
  //
  //!alct_rx_clock_delay = [0-15] (2ns)
  inline void SetAlctRXclockDelay(int alct_rx_clock_delay) {alct_rx_clock_delay_ = alct_rx_clock_delay;}
  inline int  GetAlctRXclockDelay() { return alct_rx_clock_delay_; }
  inline int  GetALCTrxPhase() { return alct_rx_clock_delay_; }
  //
  //!dmb_tx_delay = [0-15] (2ns)
  inline void SetDmbTxDelay(int dmb_tx_delay) { dmb_tx_delay_ = dmb_tx_delay; }
  inline int  GetDmbTxDelay() { return dmb_tx_delay_; }
  //
  //!rat_tmb_delay = [0-15] (2ns)
  inline void SetRatTmbDelay(int rat_tmb_delay) { rat_tmb_delay_ = rat_tmb_delay; }
  inline int  GetRatTmbDelay() { return rat_tmb_delay_ ; }
  //
  //------------------------------------------------------------------
  //0X18 = ADR_DDD1:  3D3444 Chip 1 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  //!tmb1_phase = [0-15] (2ns)
  inline void SetTmb1Phase(int tmb1_phase) { tmb1_phase_ = tmb1_phase; }
  inline int  GetTmb1Phase() { return tmb1_phase_; }
  //
  //!mpc_phase = [0-15] (2ns)
  inline void SetMpcPhase(int mpc_phase) { mpc_phase_ = mpc_phase; }
  inline int  GetMpcPhase() { return mpc_phase_; }
  //
  //!dcc_phase = [0-15] (2ns) (CFEB Duty Cycle Correction)
  inline void SetDccPhase(int dcc_phase) { dcc_phase_ = dcc_phase; }
  inline int  GetDccPhase() { return dcc_phase_; }
  //
  //!cfeb0delay = [0-15] (2ns)
  inline void SetCFEB0delay(int cfeb0delay) { cfeb0delay_ = cfeb0delay; }
  inline int  GetCFEB0delay() { return cfeb0delay_; }
  //
  //------------------------------------------------------------------
  //0X1A = ADR_DDD2:  3D3444 Chip 2 Delays, 1 step = 2ns
  //------------------------------------------------------------------
  //!cfeb1delay = [0-15] (2ns)
  inline void SetCFEB1delay(int cfeb1delay) { cfeb1delay_ = cfeb1delay; }
  inline int  GetCFEB1delay() { return cfeb1delay_; }
  //
  //!cfeb2delay = [0-15] (2ns)
  inline void SetCFEB2delay(int cfeb2delay) { cfeb2delay_ = cfeb2delay; }
  inline int  GetCFEB2delay(){return cfeb2delay_;}
  //
  //!cfeb3delay = [0-15] (2ns)
  inline void SetCFEB3delay(int cfeb3delay) { cfeb3delay_ = cfeb3delay; }
  inline int  GetCFEB3delay() { return cfeb3delay_; }
  //
  //!cfeb4delay = [0-15] (2ns)
  inline void SetCFEB4delay(int cfeb4delay) { cfeb4delay_ = cfeb4delay; }
  inline int  GetCFEB4delay() { return cfeb4delay_; }
  //
  //------------------------------------------------------------------
  //0X2A = ADR_CCB_CFG:  CCB Configuration
  //------------------------------------------------------------------
  //!ignore_ccb_rx = 1 = ignore feceived CCB backplane inputs
  inline void SetIgnoreCCBRx(int ignore_ccb_rx) { ignore_ccb_rx_ = ignore_ccb_rx; }
  inline int  GetIgnoreCCBRx() { return ignore_ccb_rx_; }
  //
  //!disable_ccb_tx = 1 = Disable transmitted CCB backplane outputs
  inline void SetDisableCCBTx(int disable_ccb_tx) { disable_ccb_tx_ = disable_ccb_tx; }
  inline int  GetDisableCCBTx() { return disable_ccb_tx_; }
  //
  //!enable_internal_l1a = 1 = Enable internal L1A emulator
  inline void SetEnableInternalL1A(int enable_internal_l1a) { enable_internal_l1a_ = enable_internal_l1a ; }
  inline int  GetEnableInternalL1A() { return enable_internal_l1a_; }
  //
  //!enable_alctclct_status_to_ccb = 1 = Enable ALCT or CLCT status to CCB front panel
  inline void SetEnableALCTorCLCTStatusToCCB(int enable_alctclct_status_to_ccb) { enable_alctclct_status_to_ccb_ = enable_alctclct_status_to_ccb; }
  inline int  GetEnableALCTorCLCTStatusToCCB() { return enable_alctclct_status_to_ccb_; }
  //
  //!enable_alct_status_to_ccb = 1 = Enable ALCT status GTL outputs (requires EnableALCTorCLCTStatusToCCB = 1)
  inline void SetALCTStatusEnableToCCB(int enable_alct_status_to_ccb) { enable_alct_status_to_ccb_ = enable_alct_status_to_ccb; }
  inline int  GetALCTStatusEnableToCCB() { return enable_alct_status_to_ccb_; }
  //
  //!enable_clct_status_to_ccb = 1 = Enable CLCT status GTL outputs (requires EnableALCTorCLCTStatusToCCB = 1)
  inline void SetCLCTStatusEnableToCCB(int enable_clct_status_to_ccb) { enable_clct_status_to_ccb_ = enable_clct_status_to_ccb; }
  inline int  GetCLCTStatusEnableToCCB() { return enable_clct_status_to_ccb_; }
  //
  //!fire_l1a_oneshot_ = 1 = Fire CCB L1-Accept oneshot
  inline void SetFireL1AOneshot(int fire_l1a_oneshot) { fire_l1a_oneshot_ = fire_l1a_oneshot; }
  inline int  GetFireL1AOneshot() { return fire_l1a_oneshot_ ; }
  //
  //------------------------------------------------------------------
  //0X2C = ADR_CCB_TRIG:  CCB Trigger Control
  //------------------------------------------------------------------
  //!alct_ext_trig_l1aen = 1 = request CCB L1A on alct_ext_trig
  inline void SetEnableL1aRequestOnAlctExtTrig(int alct_ext_trig_l1aen) { alct_ext_trig_l1aen_ = alct_ext_trig_l1aen; }
  inline int  GetEnableL1aRequestOnAlctExtTrig() { return alct_ext_trig_l1aen_; }
  //
  //!clct_ext_trig_l1aen = 1 = request CCB L1A on clct_ext_trig
  inline void SetEnableL1aRequestOnClctExtTrig(int clct_ext_trig_l1aen) { clct_ext_trig_l1aen_ = clct_ext_trig_l1aen; }
  inline int  GetEnableL1aRequestOnClctExtTrig() { return clct_ext_trig_l1aen_; }
  //
  //!request_l1a = 1 = request CCB L1A on sequencer trigger
  inline void SetRequestL1a(int request_l1a) { request_l1a_ = request_l1a; }
  inline int  GetRequestL1a() { return request_l1a_; }
  //
  //!alct_ext_trig_vme = 1 = Fire alct_ext_trig oneshot
  inline void SetAlctExtTrigVme(int alct_ext_trig_vme) { alct_ext_trig_vme_ = alct_ext_trig_vme; }
  inline int  GetAlctExtTrigVme() { return alct_ext_trig_vme_; }
  //
  //!clct_ext_trig_vme = 1 = Fire clct_ext_trig oneshot
  inline void SetClctExtTrigVme(int clct_ext_trig_vme) { clct_ext_trig_vme_ = clct_ext_trig_vme; }
  inline int  GetClctExtTrigVme() { return clct_ext_trig_vme_; }
  //
  //!ext_trig_both = 1 = clct_ext_trig fires ALCT + alct_ext_trig fires CLCT
  inline void SetExtTrigBoth(int ext_trig_both) { ext_trig_both_ = ext_trig_both; }
  inline int  GetExtTrigBoth() { return ext_trig_both_; }
  //
  //!ccb_allow_bypass = 1 = allow clct_exttrig_ccb even though ccb_ignore_rx=1 (address 0x2A)
  inline void SetCcbAllowExternalBypass(int ccb_allow_bypass) { ccb_allow_bypass_ = ccb_allow_bypass; }
  inline int  GetCcbAllowExternalBypass() { return ccb_allow_bypass_; }
  //
  //!ignore_ccb_startstop = 1 = ignore ttc_trig_start, ttc_trig_stop
  inline void SetIgnoreCcbStartStop(int ignore_ccb_startstop) { ignore_ccb_startstop_ = ignore_ccb_startstop; }
  inline int  GetIgnoreCcbStartStop() { return ignore_ccb_startstop_; }
  //
  //!internal_l1a_delay_vme = [0-255]
  inline void SetInternalL1aDelay(int internal_l1a_delay_vme) { internal_l1a_delay_vme_ = internal_l1a_delay_vme; }
  inline int  GetInternalL1aDelay() { return internal_l1a_delay_vme_; }
  //
  //------------------------------------------------------------------
  //0X30 = ADR_ALCT_CFG:  ALCT Configuration
  //------------------------------------------------------------------
  //!cfg_alct_ext_trig_en = 1 = enable alct_ext_trig from CCB
  inline void SetEnableAlctExtTrig(int cfg_alct_ext_trig_en) { cfg_alct_ext_trig_en_ = cfg_alct_ext_trig_en; }
  inline int  GetEnableAlctExtTrig() { return cfg_alct_ext_trig_en_; }
  //
  //!cfg_alct_ext_inject_en = 1 = enable alct_ext_inject from CCB
  inline void SetEnableAlctExtInject(int cfg_alct_ext_inject_en) { cfg_alct_ext_inject_en_ = cfg_alct_ext_inject_en; }
  inline int  GetEnableAlctExtInject() { return cfg_alct_ext_inject_en_; }
  //
  //!cfg_alct_ext_trig = 1 = assert alct_ext_trig
  inline void SetAlctExtTrig(int cfg_alct_ext_trig) { cfg_alct_ext_trig_ = cfg_alct_ext_trig; }
  inline int  GetAlctExtTrig() { return cfg_alct_ext_trig_; }
  //
  //!cfg_alct_ext_inject = 1 = assert alct_ext_inject
  inline void SetAlctExtInject(int cfg_alct_ext_inject) { cfg_alct_ext_inject_ = cfg_alct_ext_inject; }
  inline int  GetAlctExtInject() { return cfg_alct_ext_inject_; }
  //
  //!alct_seq_cmd = [0-15]
  inline void SetAlctSequencerCommand(int alct_seq_cmd) { alct_seq_cmd_ = alct_seq_cmd; }
  inline int  GetAlctSequencerCommand() { return alct_seq_cmd_; }
  //
  //!alct_clock_en_use_ccb = 1 => alct_clock_en_vme = ccb_clock40_enable
  inline void SetEnableAlctUseCcbClock(int alct_clock_en_use_ccb) { alct_clock_en_use_ccb_ = alct_clock_en_use_ccb; }
  inline int  GetEnableAlctUseCcbClock() { return alct_clock_en_use_ccb_; }
  //
  //!alct_clock_en_use_vme = [0,1] = alct_clock_en scsi signal if GetEnableAlctUseCcbClock() = 0;
  inline void SetAlctClockVme(int alct_clock_en_use_vme) { alct_clock_en_use_vme_ = alct_clock_en_use_vme; }
  inline int  GetAlctClockVme() { return alct_clock_en_use_vme_; }
  //
  //----------------------------------------------------------------
  //0X32 = ADR_ALCT_INJ:  ALCT Injector Control:
  //----------------------------------------------------------------
  //!alct_clear = [0,1] -> [do not blank,blank] ALCT received data
  inline void SetAlctClear(int alct_clear) { alct_clear_ = alct_clear; }                    
  inline int  GetAlctClear() { return alct_clear_; }
  //
  //!alct_inject_mux = 1 = start ALCT injector state machine
  inline void SetAlctInject(int alct_inject_mux) { alct_inject_mux_ = alct_inject_mux; }              
  inline int  GetAlctInject() { return alct_inject_mux_; }
  //
  //!alct_sync_clct = 1 = link ALCT injector with CLCT inject command
  inline void SetSyncAlctInjectToClctInject(int alct_sync_clct) { alct_sync_clct_ = alct_sync_clct; } 
  inline int  GetSyncAlctInjectToClctInject() { return alct_sync_clct_; }
  //
  inline void SetAlctInjectorDelay(int alct_inj_delay) { alct_inj_delay_ = alct_inj_delay; }
  inline int  GetAlctInjectorDelay() { return alct_inj_delay_; }
  //
  //------------------------------------------------------------------
  //0X3A = ADR_ALCT0_RCD:  ALCT 1st Muon received by TMB
  //------------------------------------------------------------------
  //! valid pattern flag
  inline int GetAlct0Valid()   { return read_alct0_valid_; }
  //
  //! pattern quality
  inline int GetAlct0Quality() { return read_alct0_quality_; }
  //
  //! accelerator muon flag
  inline int GetAlct0Amu()     { return read_alct0_amu_; }
  //
  //! key wire group
  inline int GetAlct0KeyWg()   { return read_alct0_key_wg_; }
  //
  //! OLD WAY of getting key wire group
  inline int GetAlct0FirstKey()   { return GetAlct0KeyWg(); }
  //
  //! bunch crossing number
  inline int GetAlct0Bxn()     { return read_alct0_bxn_; }
  //
  //! OLD WAY of getting bunch crossing number
  inline int GetAlct0FirstBxn()     { return GetAlct0Bxn(); }
  //
  //------------------------------------------------------------------
  //0X3C = ADR_ALCT1_RCD:  ALCT 2nd Muon received by TMB
  //------------------------------------------------------------------
  //! valid pattern flag
  inline int GetAlct1Valid()   { return read_alct1_valid_; }
  //
  //! pattern quality
  inline int GetAlct1Quality() { return read_alct1_quality_; }
  //
  //! accelerator muon flag
  inline int GetAlct1Amu()     { return read_alct1_amu_; }
  //
  //! key wire group
  inline int GetAlct1KeyWg()   { return read_alct1_key_wg_; }
  //
  //! OLD WAY of getting key wire group
  inline int GetAlct1SecondKey()   { return GetAlct1KeyWg(); }
  //
  //! bunch crossing number
  inline int GetAlct1Bxn()     { return read_alct1_bxn_; }
  //
  //! OLD WAY of getting bunch crossing number
  inline int GetAlct1SecondBxn()     { return GetAlct1Bxn(); }
  //
  //----------------------------------------------------------------
  //0X3E = ADR_ALCT_FIFO:  ALCT FIFO RAM Status
  //----------------------------------------------------------------
  //!read_alct_raw_busy = [0,1] = ALCT raw hits FIFO [not busy,busy] writing ALCT data
  inline int  GetReadAlctRawBusy() { return read_alct_raw_busy_; }
  //
  //!read_alct_raw_done = [0,1] = ALCT raw hits [not ready,ready] for VME readout
  inline int  GetReadAlctRawDone() { return read_alct_raw_done_; }
  //
  //!read_alct_raw_word_count = ALCT raw hits word count stored in RAM
  inline int  GetReadAlctRawWordCount() { return read_alct_raw_word_count_; }
  //
  //!read_alct_raw_msbs = ALCT raw hits data most significant bits
  inline int  GetReadAlctRawDataMostSignificantBits() { return read_alct_raw_msbs_; }
  //
  //----------------------------------------------------------------
  //0X42 = ADR_CFEB_INJ:  CFEB Injector Control:
  //----------------------------------------------------------------
  //!enableCLCTInputs = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [disable,enable] CFEB input
  inline void SetEnableCLCTInputs(int enableCLCTInputs) { enableCLCTInputs_ = enableCLCTInputs; } 
  inline int  GetEnableCLCTInputs() { return enableCLCTInputs_; }
  //
  //!cfeb_ram_sel = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [do not select,select] CFEB for RAM read/write
  inline void SetSelectCLCTRAM(int cfeb_ram_sel) { cfeb_ram_sel_ = cfeb_ram_sel; }        
  inline int  GetSelectCLCTRAM() { return cfeb_ram_sel_; }
  //
  //!cfeb_inj_en_sel = [0-31]... 5 bit mask, 1 bit per CFEB -> each bit [0,1] = [disable,enable] CFEB for injector trigger
  inline void SetEnableCLCTInject(int cfeb_inj_en_sel) { cfeb_inj_en_sel_ = cfeb_inj_en_sel; }  
  inline int  GetEnableCLCTInject() { return cfeb_inj_en_sel_; }
  //
  //!start_pattern_inj = 1 = start pattern injector
  inline void SetStartPatternInjector(int start_pattern_inj) { start_pattern_inj_ = start_pattern_inj; } 
  inline int  GetStartPatternInjector() { return start_pattern_inj_; }
  //
  //------------------------------------------------------------------
  //0X4A,4C,4E = ADR_HCM001,HCM023,HCM045 = CFEB0 Hot Channel Masks
  //0X50,52,54 = ADR_HCM101,HCM123,HCM145 = CFEB1 Hot Channel Masks
  //0X56,58,5A = ADR_HCM201,HCM223,HCM245 = CFEB2 Hot Channel Masks
  //0X5C,5E,60 = ADR_HCM301,HCM323,HCM345 = CFEB3 Hot Channel Masks
  //0X62,64,66 = ADR_HCM401,HCM423,HCM445 = CFEB4 Hot Channel Masks
  //------------------------------------------------------------------
  //!layer=[0-5], distrip=[0-39], on_or_off = 0 = disable
  inline void SetDistripHotChannelMask(int layer,int distrip,int on_or_off) { hot_channel_mask_[layer][distrip] = on_or_off; } 
  inline int  GetDistripHotChannelMask(int layer,int distrip) { return hot_channel_mask_[layer][distrip]; }
  //
  //!layer=[0-5], mask=10 hex-characters for the 40 distrips right->left LSB->MSB.  So, to mask off channel 0, mask= fffffffffe
  void SetDistripHotChannelMask(int layer,long long int mask);
  long long int GetDistripHotChannelMask(int layer);
  //
  //!Write registers whose values have been set by SetDistripHotChannelMask(...)
  void WriteDistripHotChannelMasks();
  void ReadDistripHotChannelMasks();
  //
  //------------------------------------------------------------------
  //0X68 = ADR_SEQ_TRIG_EN:  Sequencer Trigger Source Enables
  //------------------------------------------------------------------
  //!clct_pat_trig_en = 1 = CLCT Pattern = pretrigger
  inline void SetClctPatternTrigEnable(int clct_pat_trig_en) { clct_pat_trig_en_ = clct_pat_trig_en; }
  inline int  GetClctPatternTrigEnable() { return clct_pat_trig_en_; }
  //
  //!alct_pat_trig_en = 1 = ALCT Pattern = pretrigger
  inline void SetAlctPatternTrigEnable(int alct_pat_trig_en) { alct_pat_trig_en_ = alct_pat_trig_en; }
  inline int  GetAlctPatternTrigEnable() { return alct_pat_trig_en_; }
  //
  //!match_pat_trig_en = 1 = ALCT*CLCT Pattern = pretrigger
  inline void SetMatchPatternTrigEnable(int match_pat_trig_en) { match_pat_trig_en_ = match_pat_trig_en; }
  inline int  GetMatchPatternTrigEnable() { return match_pat_trig_en_; }
  //
  //!adb_ext_trig_en = 1 = allow ADB external triggers from CCB
  inline void SetAdbExtTrigEnable(int adb_ext_trig_en) { adb_ext_trig_en_ = adb_ext_trig_en; }
  inline int  GetAdbExtTrigEnable() { return adb_ext_trig_en_; }
  //
  //!dmb_ext_trig_en = 1 = allow DMB external triggers
  inline void SetDmbExtTrigEnable(int dmb_ext_trig_en) { dmb_ext_trig_en_ = dmb_ext_trig_en; }
  inline int  GetDmbExtTrigEnable() { return dmb_ext_trig_en_; }
  //
  //!clct_ext_trig_en = 1 = allow CLCT external triggers (scintillator) from CCB
  inline void SetClctExtTrigEnable(int clct_ext_trig_en) { clct_ext_trig_en_ = clct_ext_trig_en; }
  inline int  GetClctExtTrigEnable() { return clct_ext_trig_en_; }
  //
  //!alct_ext_trig_en = 1 = allow ALCT external triggers from CCB
  inline void SetAlctExtTrigEnable(int alct_ext_trig_en) { alct_ext_trig_en_ = alct_ext_trig_en; }
  inline int  GetAlctExtTrigEnable() { return alct_ext_trig_en_; }
  //
  //!vme_ext_trig = 1 = Initiate sequencer trigger (write 0 to recover)
  inline void SetVmeExtTrig(int vme_ext_trig) { vme_ext_trig_ = vme_ext_trig; }
  inline int  GetVmeExtTrig() { return vme_ext_trig_; }
  //
  //!ext_trig_inject = 1 = change clct_ext_trig to fire pattern injector
  inline void SetExtTrigInject(int ext_trig_inject) { ext_trig_inject_ = ext_trig_inject; }
  inline int  GetExtTrigInject() { return ext_trig_inject_; }
  //
  //!all_cfeb_active = 1 = make all CFEBs active when triggered
  inline void SetEnableAllCfebsActive(int all_cfeb_active) { all_cfeb_active_ = all_cfeb_active; }
  inline int  GetEnableAllCfebsActive() { return all_cfeb_active_; }
  //
  //!cfebs_enabled_ = [0-31] -> normally copied from 0x42.  See TMB documentation before setting these bits...
  inline void SetCfebEnable(int cfebs_enabled) { cfebs_enabled_ = cfebs_enabled; }
  inline int  GetCfebEnable() { return cfebs_enabled_; }
  //
  //! value = 42, 68 = VME register which controls the CFEB mask.  See TMB documentation before setting this value.
  void Set_cfeb_enable_source(int value); 
  // this is the user interface to SetCfebEnableSource_(int cfeb_enable_source)...
  //
  inline int  GetCfebEnableSource() { return cfeb_enable_source_; }
  //
  // added for database purpose
  inline int  GetCfebEnableSource_orig() { return cfeb_enable_source_orig; }

  //
  //------------------------------------------------------------------
  //0X6A = ADR_SEQ_TRIG_DLY0:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  //!alct_pretrig_width = [0-15] = window width for ALCT*CLCT pretrigger (bx)
  inline void SetAlctClctPretrigWidth(int alct_pretrig_width) { alct_pretrig_width_ = alct_pretrig_width; }
  inline int  GetAlctClctPretrigWidth() { return alct_pretrig_width_; }
  //
  //!alct_pretrig_delay = [0-15] = ALCT delay for use in ALCT*CLCT pretrigger (bx)
  inline void SetAlctPretrigDelay(int alct_pretrig_delay) { alct_pretrig_delay_ = alct_pretrig_delay; }
  inline int  GetAlctPretrigDelay() { return alct_pretrig_delay_; }  
  //
  //!alct_pattern_delay = [0-15] = delay active FEB flag from ALCT (bx)
  inline void SetAlctPatternDelay(int alct_pattern_delay) { alct_pattern_delay_ = alct_pattern_delay; }
  inline int  GetAlctPatternDelay() { return alct_pattern_delay_; }
  //
  //!adb_ext_trig_delay = [0-15] = delay ADB external trig from CCB (bx)
  inline void SetAdbExternalTrigDelay(int adb_ext_trig_delay) { adb_ext_trig_delay_ = adb_ext_trig_delay; }
  inline int  GetAdbExternalTrigDelay() { return adb_ext_trig_delay_; }
  //
  //------------------------------------------------------------------
  //0X6C = ADR_SEQ_TRIG_DLY1:  Sequencer Trigger Source Delays
  //------------------------------------------------------------------
  //!dmb_ext_trig_delay = [0-15] = delay external trigger from DMB (bx)
  inline void SetDmbExternalTrigDelay(int dmb_ext_trig_delay) { dmb_ext_trig_delay_ = dmb_ext_trig_delay; }
  inline int  GetDmbExternalTrigDelay() { return dmb_ext_trig_delay_; }
  //
  //!clct_ext_trig_delay = [0-15] = delay external trigger from CLCT (bx)
  inline void SetClctExternalTrigDelay(int clct_ext_trig_delay) { clct_ext_trig_delay_ = clct_ext_trig_delay; }
  inline int  GetClctExternalTrigDelay() { return clct_ext_trig_delay_; }
  //
  //!alct_ext_trig_delay = [0-15] = delay external trigger from ALCT (bx)
  inline void SetAlctExternalTrigDelay(int alct_ext_trig_delay) { alct_ext_trig_delay_ = alct_ext_trig_delay; }
  inline int  GetAlctExternalTrigDelay() { return alct_ext_trig_delay_; }
  //
  //------------------------------------------------------------------
  //0X6E = ADR_SEQ_ID:  Sequencer Board + CSC Ids
  //------------------------------------------------------------------
  //!tmb_slot = [2-20] (even) VME slot for this TMB
  inline void SetTmbSlot(int tmb_slot) { tmb_slot_ = tmb_slot; }
  inline int  GetTmbSlot() { return tmb_slot_; }
  //
  //!csc_id = [1-10] index of CSC within this crate (tmb_slot/2) 
  inline void SetCscId(int csc_id) { csc_id_ = csc_id; }
  inline int  GetCscId() { return csc_id_; }
  //
  inline void SetRunId(int run_id) { run_id_ = run_id; }
  inline int  GetRunId() { return run_id_; }
  //
  //------------------------------------------------------------------
  //0X70 = ADR_SEQ_CLCT:  Sequencer CLCT configuration
  //------------------------------------------------------------------
  //!triad_persist = [0-15] = triad one-shot perisistence (bx)
  inline void SetTriadPersistence(int triad_persist) { triad_persist_ = triad_persist; }
  inline int  GetTriadPersistence() { return triad_persist_; }
  //
  //!hit_thresh = [0-6] = 1/2-strip pretrigger threshold
  inline void SetHsPretrigThresh(int hit_thresh) { hit_thresh_ = hit_thresh; }
  inline int  GetHsPretrigThresh() { return hit_thresh_; }
  //
  //!aff_thresh = [0-6] = minimum number of hits needed on CLCT pretrigger pattern to send Active FEB Flag to DMB
  inline void SetActiveFebFlagThresh(int aff_thresh) { aff_thresh_ = aff_thresh; } 
  inline int  GetActiveFebFlagThresh() { return aff_thresh_; } 
  inline int  GetReadActiveFebFlagThresh() { return read_aff_thresh_; } 
  //
  //!min_hits_pattern = minimum number of layers needed to match for pattern trigger
  inline void SetMinHitsPattern(int min_hits_pattern){ min_hits_pattern_ = min_hits_pattern; }
  inline int  GetMinHitsPattern() { return min_hits_pattern_ ;}
  //
  //!drift_delay = [0-3] = drift delay between TMB pretrigger and CLCT pattern trigger
  inline void SetDriftDelay(int drift_delay) { drift_delay_ = drift_delay; }
  inline int  GetDriftDelay() { return drift_delay_; }
  //
  //!pretrigger_halt = 1 = pretrigger and halt until unhalt arrives
  inline void SetPretriggerHalt(int pretrigger_halt) { pretrigger_halt_ = pretrigger_halt; }
  inline int  GetPretriggerHalt() { return pretrigger_halt_; }
  inline int  GetReadPretriggerHalt() { return read_pretrigger_halt_; }
  //
  //------------------------------------------------------------------
  //0X72 = ADR_SEQ_FIFO:  Sequencer FIFO configuration
  //------------------------------------------------------------------
  //fifo_mode = FIFOMODE_NoCfebRaw_FullHeader    = 0
  //            FIFOMODE_AllCfebRaw_FullHeader   = 1;
  //            FIFOMODE_LocalCfebRaw_FullHeader = 2;
  //            FIFOMODE_NoCfebRaw_ShortHeader   = 3;
  //            FIFOMODE_NoCfebRaw_NoHeader      = 4;
  inline void SetFifoMode(int fifo_mode) { fifo_mode_ = fifo_mode; }
  inline int  GetFifoMode() { return fifo_mode_; }
  //
  //!fifo_tbins = [0-31] -> width of window for CLCT/RPC raw data readout (bx)
  inline void SetFifoTbins(int fifo_tbins) { fifo_tbins_ = fifo_tbins; }
  inline int  GetFifoTbins() { return fifo_tbins_ ; }
  //
  //!fifo_pretrig = [0-31] -> number of time bins before TMB pretrigger to begin data readout window (bx)
  inline void SetFifoPreTrig(int fifo_pretrig) { fifo_pretrig_ = fifo_pretrig; }
  inline int  GetFifoPreTrig() { return fifo_pretrig_; }
  //
  //!fifo_no_raw_hits = [0-31] -> number of time bins before TMB pretrigger to begin data readout window (bx)
  inline void SetFifoNoRawHits(int fifo_no_raw_hits) { fifo_no_raw_hits_ = fifo_no_raw_hits; }
  inline int  GetFifoNoRawHits() { return fifo_no_raw_hits_; }
  //
  //------------------------------------------------------------------
  //0X74 = ADR_SEQ_L1A:  Sequencer L1A configuration
  //------------------------------------------------------------------
  //!l1adelay = [0-255] (bx)
  inline void SetL1aDelay(int l1adelay) { l1adelay_ = l1adelay; }
  inline int  GetL1aDelay() { return l1adelay_; }
  //
  //!l1a_window_size = [0-15] (bx)
  inline void SetL1aWindowSize(int l1a_window_size) { l1a_window_size_ = l1a_window_size; }
  inline int  GetL1aWindowSize() { return l1a_window_size_; }
  //
  //!tmb_l1a_internal = 1 = generate internal level 1 (overrides external)
  inline void SetInternalL1a(int tmb_l1a_internal) { tmb_l1a_internal_ = tmb_l1a_internal; }
  inline int  GetInternalL1a() { return tmb_l1a_internal_; }
  //
  //------------------------------------------------------------------
  //0X76 = ADR_SEQ_OFFSET:  Sequencer Counter Offsets
  //------------------------------------------------------------------
  //!l1a_offset = [0-15]
  inline void SetL1aOffset(int l1a_offset) { l1a_offset_ = l1a_offset; }
  inline int  GetL1aOffset() { return l1a_offset_; }
  //
  //!bxn_offset = [0-4095]
  inline void SetBxnOffset(int bxn_offset) { bxn_offset_ = bxn_offset; }
  inline int  GetBxnOffset() { return bxn_offset_; }
  //
  //------------------------------------------------------------------
  //0X78 = ADR_SEQ_CLCT0:  Sequencer Latched CLCT0
  //------------------------------------------------------------------
  //! CLCT0 valid pattern flag
  inline int GetCLCT0Valid() { return read_CLCT0_valid_; }
  //
  //! CLCT0 number of hits on pattern = [0-6]
  inline int GetCLCT0Nhit() { return read_CLCT0_nhit_; }
  //
  //! CLCT0 pattern ID = [0-10]
  inline int GetCLCT0PatternId() { return read_CLCT0_pattern_; }
  //
  //! CLCT0 key 1/2-strip = [0-159]
  inline int GetCLCT0keyHalfStrip() { return read_CLCT0_keyHalfStrip_; }
  //
  //
  //------------------------------------------------------------------
  //0X7A = ADR_SEQ_CLCT1:  Sequencer Latched CLCT1
  //------------------------------------------------------------------
  //! CLCT1 valid pattern flag
  inline int GetCLCT1Valid() { return read_CLCT1_valid_; }
  //
  //! CLCT1 number of hits on pattern = [0-6]
  inline int GetCLCT1Nhit() { return read_CLCT1_nhit_; }
  //
  //! CLCT1 pattern ID = [0-10]
  inline int GetCLCT1PatternId() { return read_CLCT1_pattern_; }
  //
  //! CLCT1 key 1/2-strip = [0-159]
  inline int GetCLCT1keyHalfStrip() { return read_CLCT1_keyHalfStrip_; }
  //
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  //!tmb_sync_err_enable = [0-3]... 2 bit mask, 1 bit per LCT -> each bit [0,1] = [disable,enable] sync_err to MPC
  inline void SetTmbSyncErrEnable(int tmb_sync_err_enable) { tmb_sync_err_enable_ = tmb_sync_err_enable; }
  inline int  GetTmbSyncErrEnable() { return tmb_sync_err_enable_; }
  //
  //!tmb_allow_alct = 1 = ALCT only trigger
  inline void SetTmbAllowAlct(int tmb_allow_alct) { tmb_allow_alct_ = tmb_allow_alct; }
  inline int  GetTmbAllowAlct() { return tmb_allow_alct_; }
  //
  //!tmb_allow_clct = 1 = CLCT only trigger
  inline void SetTmbAllowClct(int tmb_allow_clct) { tmb_allow_clct_ = tmb_allow_clct; }
  inline int  GetTmbAllowClct() { return tmb_allow_clct_; }
  //
  //!tmb_allow_match = 1 = ALCT*CLCT trigger
  inline void SetTmbAllowMatch(int tmb_allow_match) { tmb_allow_match_ = tmb_allow_match; }
  inline int  GetTmbAllowMatch() { return tmb_allow_match_; }
  //
  //!mpc_rx_delay = [0-15] -> MPC accept bit delay
  inline void SetMpcRxDelay(int mpc_rx_delay) {mpc_rx_delay_= mpc_rx_delay;}
  inline int  GetMpcRxDelay() { return mpc_rx_delay_; }
  //
  //!mpc_sel_ttc_bx0 = [0,1] -> BX0 for MPC comes from [local,TTC]
  inline void SetSelectMpcTtcBx0(int mpc_sel_ttc_bx0) { mpc_sel_ttc_bx0_ = mpc_sel_ttc_bx0; }
  inline int  GetSelectMpcTtcBx0() { return mpc_sel_ttc_bx0_; }
  //
  //!mpc_idle_blank = 1 = blank MPC data and BX0 except when triggered
  inline void SetMpcIdleBlank(int mpc_idle_blank) { mpc_idle_blank_ = mpc_idle_blank; }
  inline int  GetMpcIdleBlank() { return mpc_idle_blank_; }
  //
  //!mpc_output_enable = [0,1] = [disable,enable] output (of LCT) to MPC 
  inline void SetMpcOutputEnable(int mpc_output_enable) { mpc_output_enable_ = mpc_output_enable; }
  inline int  GetMpcOutputEnable() { return mpc_output_enable_; }
  //
  //------------------------------------------------------------------
  //0XA8 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM control
  //------------------------------------------------------------------
  //!alct_raw_reset_ = 1 = Reset ALCT raw hits FIFO controller  
  inline void SetAlctRawReset(int alct_raw_reset) { alct_raw_reset_ = alct_raw_reset; }
  inline int  GetAlctRawReset() { return alct_raw_reset_; }
  //
  //!alct_raw_read_address_ = ALCT raw hits RAM read address or demux word  
  inline void SetAlctRawReadAddress(int alct_raw_read_address) { alct_raw_read_address_ = alct_raw_read_address; }
  inline int  GetAlctRawReadAddress() { return alct_raw_read_address_; }
  //
  //!alct_demux_mode_ = [0,1] = ADR_ALCTFIFO2 has [RAM,demux] data
  inline void SetAlctDemuxMode(int alct_demux_mode) { alct_demux_mode_ = alct_demux_mode; }
  inline int  GetAlctDemuxMode() { return alct_demux_mode_; }
  //
  //
  //------------------------------------------------------------------
  //0XAA = ADR_ALCTFIFO2:  ALCT Raw Hits RAM control
  //------------------------------------------------------------------
  //!If alct_demux_mode = 0, read_alct_raw_lsbs = ALCT raw hits data least significant bits
  //!If alct_demux_mode = 1, read_alct_raw_lsbs = alct_1st_vme[14:1],alct_1st_vme[28:15],alct_2nd_vme[14:1],alct_2nd_vme[28:15]
  inline int  GetReadAlctRawDataLeastSignificantBits() { if (alct_demux_mode_ == 0) {return read_alct_raw_lsbs_;} else {return -999;} }
  inline int  GetReadAlctDemuxData() { if (alct_demux_mode_ == 1) {return read_alct_raw_lsbs_;} else {return -999;} }
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  //!clct_flush_delay_ = [0-15] = delay to flush the trigger sequencer  
  inline void SetClctFlushDelay(int clct_flush_delay) { clct_flush_delay_ = clct_flush_delay; }
  inline int  GetClctFlushDelay() { return clct_flush_delay_; }
  //
  //!wr_buffer_autoclear_ = 1 = enable frozen buffer auto clear
  inline void SetWriteBufferAutoclear(int wr_buffer_autoclear) { wr_buffer_autoclear_ = wr_buffer_autoclear; }
  inline int  GetWriteBufferAutoclear() { return wr_buffer_autoclear_; }
  //
  //!clct_write_continuous_enable = 1 = allow continuous header buffer writing for invalid triggers
  inline void SetClctWriteContinuousEnable(int clct_write_continuous_enable) { clct_write_continuous_enable_ = clct_write_continuous_enable; }
  inline int  GetClctWriteContinuousEnable() { return clct_write_continuous_enable_; }
  //
  //!wrt_buf_required_ = 1 = require wr_buffer available to pretrigger
  inline void SetWriteBufferRequired(int wrt_buf_required) { wrt_buf_required_ = wrt_buf_required; }
  inline int  GetWriteBufferRequired() { return wrt_buf_required_; }
  //
  //!valid_clct_required_ = 1 = require valid CLCT after drift delay
  inline void SetRequireValidClct(int valid_clct_required) { valid_clct_required_ = valid_clct_required; }
  inline int  GetRequireValidClct() { return valid_clct_required_; }
  //
  //!l1a_allow_match = 1 = readout allows tmb trigger pulse in L1A window
  inline void SetL1aAllowMatch(int l1a_allow_match) { l1a_allow_match_ = l1a_allow_match; }
  inline int  GetL1aAllowMatch() { return l1a_allow_match_; }
  //
  //!l1a_allow_notmb = 1 = readout allows notmb trigger pulse in L1A window
  inline void SetL1aAllowNoTmb(int l1a_allow_notmb) { l1a_allow_notmb_ = l1a_allow_notmb; }
  inline int  GetL1aAllowNoTmb() { return l1a_allow_notmb_; }
  //
  //!l1a_allow_nol1a = 1 = readout allows tmb trig pulse outside L1A window
  inline void SetL1aAllowNoL1a(int l1a_allow_nol1a) { l1a_allow_nol1a_ = l1a_allow_nol1a; }
  inline int  GetL1aAllowNoL1a() { return l1a_allow_nol1a_; }
  //
  //!l1a_allow_alct_only = 1 = allow ALCT-only events to readout at L1A
  inline void SetL1aAllowAlctOnly(int l1a_allow_alct_only) { l1a_allow_alct_only_ = l1a_allow_alct_only; }
  inline int  GetL1aAllowAlctOnly() { return l1a_allow_alct_only_; }
  //
  //!scint_veto_clr = 1 = clear scintillator veto 
  inline void SetScintillatorVetoClear(int scint_veto_clr) { scint_veto_clr_ =  scint_veto_clr; }
  inline int  GetScintillatorVetoClear() { return scint_veto_clr_; }
  //
  //------------------------------------------------------------------
  //0XAE = ADR_SEQSM:  Sequencer Machine State
  //------------------------------------------------------------------
  //!clct_state_machine_ = [0-7] = CLCT Trigger Machine state
  inline int  GetReadClctMachineState() { return read_clct_state_machine_; }
  //
  //!tmb_match_state_machine_ = [0-7] = TMB Match Machine state -- deprecated
  //inline int  GetReadTmbMatchMachineState() { return read_tmb_match_state_machine_; }
  //
  //!readout_state_machine_ = [0-31] = Readout Machine state
  inline int  GetReadReadoutMachineState() { return read_readout_state_machine_; }
  //
  //!buffer_queue_full_ = 1 = Buffer queue full
  inline int  GetReadBufferQueueFull() { return read_buffer_queue_full_; }
  //
  //!buffer_queue_empty_ = 1 = Buffer queue empty
  inline int  GetReadBufferQueueEmpty() { return read_buffer_queue_empty_; }
  //
  //!buffer_queue_overflow_ = 1 = Buffer queue overflow
  inline int  GetReadBufferQueueOverflow() { return read_buffer_queue_overflow_; }
  //
  //!buffer_queue_underflow_ = 1 = Buffer queue underflow
  inline int  GetReadBufferQueueUnderflow() { return read_buffer_queue_underflow_; }
  //
  //------------------------------------------------------------------
  //0XB2 = ADR_TMBTIM:  TMB Timing for ALCT*CLCT Coincidence
  //------------------------------------------------------------------
  //!alct_vpf_delay = [0-15] = delay ALCT valid pattern flag to match with CLCT pattern for trigger (bx)
  inline void SetAlctVpfDelay(int alct_vpf_delay) { alct_vpf_delay_ = alct_vpf_delay; }
  inline int  GetAlctVpfDelay() { return alct_vpf_delay_; }
  //
  //!alct_match_window_size = [0-15] = ALCT/CLCT match window width for trigger (bx)
  inline void SetAlctMatchWindowSize(int alct_match_window_size) { alct_match_window_size_ = alct_match_window_size; }
  inline int  GetAlctMatchWindowSize() { return alct_match_window_size_ ; }
  //
  //!mpc_tx_delay = [0-15] = delay sending LCT to MPC (bx)
  inline void SetMpcTxDelay(int mpc_tx_delay) { mpc_tx_delay_ = mpc_tx_delay; }
  inline  int GetMpcTxDelay() { return mpc_tx_delay_; }
  //
  //-----------------------------------------------------------------
  //0XB6 = ADR_RPC_CFG:  RPC Configuration:
  //-----------------------------------------------------------------
  //!rpc_exists = [0-15]... 4 bit mask, 1 bit per RPC -> each bit [0,1] = RPC [does not,does] exist
  inline void SetRpcExist(int rpc_exists) { rpc_exists_ = rpc_exists; }  
  inline int  GetRpcExist() { return rpc_exists_; }
  //
  //!rpc_read_enable = 1 = include existing RPCs in DMB readout
  inline void SetRpcReadEnable(int rpc_read_enable) { rpc_read_enable_ = rpc_read_enable; } 
  inline int  GetRpcReadEnable() { return rpc_read_enable_; }
  //
  //!rpc_bxn_offset = [0-15]
  inline void SetRpcBxnOffset(int rpc_bxn_offset) { rpc_bxn_offset_ = rpc_bxn_offset; } 
  inline int  GetRpcBxnOffset() { return rpc_bxn_offset_; }
  //
  //!rpc_bank = [0-3] -> RPC bank address, for reading rdata sync mode
  inline void SetRpcSyncBankAddress(int rpc_bank) { rpc_bank_ = rpc_bank; } 
  inline int  GetRpcSyncBankAddress() { return rpc_bank_; }
  //
  //------------------------------------------------------------------
  //0XBA = ADR_RPC_RAW_DELAY:  RPC Raw Hits Data Delay
  //------------------------------------------------------------------
  //!rpc0_raw_delay = [0-15] = delay RPC data into FIFO (bx)
  inline void SetRpc0RawDelay(int rpc0_raw_delay) { rpc0_raw_delay_ = rpc0_raw_delay; }
  inline int  GetRpc0RawDelay() { return rpc0_raw_delay_ ; }
  //
  //!rpc1_raw_delay = [0-15] = delay RPC data into FIFO (bx)
  inline void SetRpc1RawDelay(int rpc1_raw_delay) { rpc1_raw_delay_ = rpc1_raw_delay; }
  inline int  GetRpc1RawDelay() { return rpc1_raw_delay_ ; }
  //
  //
  //!to be deprecated
  inline void SetTrgMode(int trgmode) { trgmode_ = trgmode; }
  inline int  GetTrgMode() { return trgmode_ ; }
  //trgmode = CLCT_trigger                  =  1;
  //          ALCT_trigger                  =  2;
  //          Scintillator_trigger          =  3;
  //          DMB_trigger                   =  4;
  //          ALCT_CLCT_coincidence_trigger =  5;
  //
  //------------------------------------------------------------------
  //0XBC = ADR_RPC_INJ:  RPC Injector Control
  //------------------------------------------------------------------
  //!rpc_mask_all = [1,0] -> All RPC inputs [on,off]
  inline void SetEnableRpcInput(int rpc_mask_all) { rpc_mask_all_ = rpc_mask_all; }   
  inline int  GetEnableRpcInput() { return rpc_mask_all_; }
  //
  //!inj_mask_rat = 1 = enable RAT for injector fire
  inline void SetInjectorMaskRat(int inj_mask_rat) { inj_mask_rat_ = inj_mask_rat; }
  inline int  GetInjectorMaskRat() { return inj_mask_rat_; }
  //
  //!inj_mask_rpc = 1 = enable RPC injector RAM for injector fire
  inline void SetInjectorMaskRpc(int inj_mask_rpc) { inj_mask_rpc_ = inj_mask_rpc; }
  inline int  GetInjectorMaskRpc() { return inj_mask_rpc_; }
  //
  //!inj_delay_rat = [0-15] -> CFEB/RPC injectors wait for RAT
  inline void SetInjectorDelayRat(int inj_delay_rat) { inj_delay_rat_ = inj_delay_rat; }
  inline int  GetInjectorDelayRat() { return inj_delay_rat_; }
  //
  //!rpc_inj_sel = 1 = enable injector RAM write 
  inline void SetRpcInjector(int rpc_inj_sel) {rpc_inj_sel_ = rpc_inj_sel; }
  inline int  GetRpcInjector() {return rpc_inj_sel_; }
  //
  //!rpc_inj_wdata = [0-7] -> RPC injector write data MSBs
  inline void SetRpcInjectorWriteDataMSBs(int rpc_inj_wdata) { rpc_inj_wdata_ = rpc_inj_wdata; }
  inline int  GetRpcInjectorWriteDataMSBs() { return rpc_inj_wdata_; }
  //
  //------------------------------------------------------------------
  //0XC4 = ADR_RPC_TBINS:  RPC FIFO Time Bins
  //------------------------------------------------------------------
  //!fifo_tbins_rpc = Number of RPC FIFO time bins to read out
  inline void SetFifoTbinsRpc(int fifo_tbins_rpc) { fifo_tbins_rpc_ = fifo_tbins_rpc; }
  inline int  GetFifoTbinsRpc() { return fifo_tbins_rpc_; }
  //
  //!fifo_pretrig_rpc = Number of RPC FIFO time bins before pretrigger
  inline void SetFifoPretrigRpc(int fifo_pretrig_rpc) { fifo_pretrig_rpc_ = fifo_pretrig_rpc; }
  inline int  GetFifoPretrigRpc() { return fifo_pretrig_rpc_; }
  //
  //!rpc_decouple = 1/0 = independent RPC tbins/copy CFEB tbins
  inline void SetRpcDecoupleTbins(int rpc_decouple) { rpc_decouple_ = rpc_decouple; }
  inline int  GetRpcDecoupleTbins() { return rpc_decouple_; }
  //
  //------------------------------------------------------------------
  //0XCA = ADR_BX0_DELAY:  BX0 to MPC delays
  //------------------------------------------------------------------
  //!alct_bx0_delay = ALCT bx0 delay to MPC transmitter
  inline void SetAlctBx0Delay(int alct_bx0_delay) { alct_bx0_delay_ = alct_bx0_delay; }
  inline int  GetAlctBx0Delay() { return alct_bx0_delay_; }
  //
  //!clct_bx0_delay = CLCT bx0 delay to MPC transmitter
  inline void SetClctBx0Delay(int clct_bx0_delay) { clct_bx0_delay_ = clct_bx0_delay; }
  inline int  GetClctBx0Delay() { return clct_bx0_delay_; }
  //
  //!alct_bx0_enable = 1/0 = enable ALCT BX0/use CLCT BX0 for ALCT 
  inline void SetAlctBx0Enable(int alct_bx0_enable) { alct_bx0_enable_ = alct_bx0_enable; }
  inline int  GetAlctBx0Enable() { return alct_bx0_enable_; }
  //
  //-----------------------------------------------------------------------------
  //0XCC = ADR_NON_TRIG_RO:  Non-Triggering Event Enables + ME1/1A(1B) reversal 
  //-----------------------------------------------------------------------------
  //!tmb_allow_alct_nontrig_readout = 1/0 = do/don't allow ALCT-only non-triggering readout
  inline void SetAllowAlctNontrigReadout(int tmb_allow_alct_nontrig_readout) { tmb_allow_alct_nontrig_readout_ = tmb_allow_alct_nontrig_readout; } 
  inline int  GetAllowAlctNontrigReadout() { return tmb_allow_alct_nontrig_readout_; } 
  inline int  GetReadAllowAlctNontrigReadout() { return read_tmb_allow_alct_nontrig_readout_; } 
  //
  //!tmb_allow_clct_nontrig_readout = 1/0 = do/don't allow CLCT-only non-triggering readout
  inline void SetAllowClctNontrigReadout(int tmb_allow_clct_nontrig_readout) { tmb_allow_clct_nontrig_readout_ = tmb_allow_clct_nontrig_readout; } 
  inline int  GetAllowClctNontrigReadout() { return tmb_allow_clct_nontrig_readout_; } 
  inline int  GetReadAllowClctNontrigReadout() { return read_tmb_allow_clct_nontrig_readout_; } 
  //
  //!tmb_allow_match_nontrig_readout = 1/0 = do/don't allow ALCT*CLCT non-triggering readout
  inline void SetAllowMatchNontrigReadout(int tmb_allow_match_nontrig_readout) { tmb_allow_match_nontrig_readout_ = tmb_allow_match_nontrig_readout; } 
  inline int  GetAllowMatchNontrigReadout() { return tmb_allow_match_nontrig_readout_; } 
  inline int  GetReadAllowMatchNontrigReadout() { return read_tmb_allow_match_nontrig_readout_; } 
  //
  //!mpc_block_me1a = 1/0 = do/don't block ME1A LCT's from going to MPC (still queue data for readout)
  inline void SetBlockME1aToMPC(int mpc_block_me1a) { mpc_block_me1a_ = mpc_block_me1a; } 
  inline int  GetBlockME1aToMPC() { return mpc_block_me1a_; } 
  inline int  GetReadBlockME1aToMPC() { return read_mpc_block_me1a_; } 
  //
  //!clct_pretrigger_counter_non_me11 = 1/0 = do/don't allow CLCT pretrigger Counters 6 and 7 to count non-ME1A/B.  If 1-> they should equal counter 5
  inline void SetClctPretriggerCounterME11(int clct_pretrigger_counter_non_me11) { clct_pretrigger_counter_non_me11_ = clct_pretrigger_counter_non_me11; } 
  inline int  GetClctPretriggerCounterME11() { return clct_pretrigger_counter_non_me11_; } 
  inline int  GetReadClctPretriggerCounterME11() { return read_clct_pretrigger_counter_non_me11_; } 
  //
  //!csc_me11 = 1/0 = TMB firmware compile type is/isn't for ME1/1 (setting based on tmb_firmware_compile_type)
  inline int  GetCSCME11() { return csc_me11_; } 
  inline int  GetReadCSCME11() { return read_csc_me11_; } 
  //
  //!clct_stagger = 1/0 = do/don't stagger strip layers (setting based on tmb_firmware_compile_type)
  inline int  GetClctStagger() { return clct_stagger_; } 
  inline int  GetReadClctStagger() { return read_clct_stagger_; } 
  //
  //!reverse_stagger = 1/0 = do/don't reverse and stagger strips (setting based on tmb_firmware_compile_type)
  inline int  GetReverseStagger() { return reverse_stagger_; } 
  inline int  GetReadReverseStagger() { return read_reverse_stagger_; } 
  //
  //!reverse_me1a = 1/0 = do/don't reverse me1a 1/2-strips (setting based on tmb_firmware_compile_type)
  inline int  GetReverseME1a() { return reverse_me1a_; } 
  inline int  GetReadReverseME1a() { return read_reverse_me1a_; } 
  //
  //!reverse_me1b = 1/0 = do/don't reverse me1b 1/2-strips (setting based on tmb_firmware_compile_type)
  inline int  GetReverseMe1b() { return reverse_me1b_; } 
  inline int  GetReadReverseMe1b() { return read_reverse_me1b_; } 
  //
  // Although the following are read-only bits, we set it in the xml file to define what TMB firmware type to expect...
  // The software Setters and Getters do not necessarily correspond to the actual bits which are read from address 0xCC.  
  //!tmb_firmware_compile_type = 0xa,0xb,0xc,0xd = type of firmware specifying the 1/2-strip ordering and ME1/1 CFEB arrangement.
  void SetTMBFirmwareCompileType(int tmb_firmware_compile_type);  
  inline int GetTMBFirmwareCompileType() { return tmb_firmware_compile_type_; } 
  inline int GetReadTMBFirmwareCompileType() { return read_tmb_firmware_compile_type_; }
  //
  //------------------------------------------------------------------
  //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
  //------------------------------------------------------------------
  inline void SetJtagDisableWriteToAdr10(int jtag_disable_write_to_adr10) { jtag_disable_write_to_adr10_ = jtag_disable_write_to_adr10; }
  inline int  GetJtagDisableWriteToAdr10() { return jtag_disable_write_to_adr10_; }
  //
  //------------------------------------------------------------------
  //0XE6 = ADR_DDDR0:  RAT 3D3444 RPC Delays, 1 step = 2ns
  //------------------------------------------------------------------
  //!rpc0_rat_delay = [0-15] (2ns)
  inline void SetRpc0RatDelay(int rpc0_rat_delay) { rpc0_rat_delay_ = rpc0_rat_delay; }
  inline int  GetRpc0RatDelay() { return rpc0_rat_delay_ ; }
  //
  //!rpc1_rat_delay = [0-15] (2ns)
  inline void SetRpc1RatDelay(int rpc1_rat_delay) { rpc1_rat_delay_ = rpc1_rat_delay; }
  inline int  GetRpc1RatDelay() { return rpc1_rat_delay_ ; }
  //
  //---------------------------------------------------------------------
  //0XF0 = ADR_LAYER_TRIG:  Layer-Trigger Mode
  //---------------------------------------------------------------------
  //!layer_trigger_en = 1 = enable layer trigger mode
  inline void SetEnableLayerTrigger(int layer_trigger_en) { layer_trigger_en_ = layer_trigger_en; } 
  inline int  GetEnableLayerTrigger() { return layer_trigger_en_; } 
  //
  //!layer_trig_thresh = [0-6] = number of layers required for layer trigger
  inline void SetLayerTriggerThreshold(int layer_trig_thresh) { layer_trig_thresh_ = layer_trig_thresh; }
  inline int  GetLayerTriggerThreshold() { return layer_trig_thresh_; }
  //
  //!clct_throttle = [0-255] = CLCT pretrigger rate throttle
  inline void SetClctThrottle(int clct_throttle) { clct_throttle_ = clct_throttle; }
  inline int  GetClctThrottle() { return clct_throttle_; }
  //
  //---------------------------------------------------------------------
  //0XF4 = ADR_TEMP0:  Pattern finder Pretrigger
  //---------------------------------------------------------------------
  //!clct_blanking = 1 = blank CLCT output if no valid pattern flag
  inline void SetClctBlanking(int clct_blanking) { clct_blanking_ = clct_blanking; } 
  inline int  GetClctBlanking() { return clct_blanking_; } 
  inline int  GetReadClctBlanking() { return read_clct_blanking_; } 
  //
  //!clct_pattern_id_thresh = [0-8] = minimum pattern ID value for CLCT pretrigger
  inline void SetClctPatternIdThresh(int clct_pattern_id_thresh) { clct_pattern_id_thresh_ = clct_pattern_id_thresh; } 
  inline int  GetClctPatternIdThresh() { return clct_pattern_id_thresh_; } 
  inline int  GetReadClctPatternIdThresh() { return read_clct_pattern_id_thresh_; } 
  //
  //!clct_pattern_id_thresh_postdrift = [0-8] = minimum pattern ID value for CLCT post drift-delay
  inline void SetClctPatternIdThreshPostDrift(int clct_pattern_id_thresh_postdrift) { clct_pattern_id_thresh_postdrift_ = clct_pattern_id_thresh_postdrift; } 
  inline int  GetClctPatternIdThreshPostDrift() { return clct_pattern_id_thresh_postdrift_; } 
  inline int  GetReadClctPatternIdThreshPostDrift() { return read_clct_pattern_id_thresh_postdrift_; } 
  //
  //!adjacent_cfeb_distance = [0-31] = Distance from key on CFEBn to CFEBn+1 to set Active FEB Flag on CFEBn+1 for DMB
  //... setting to 5 enables hs0,1,2,3,4 and hs31,30,29,28,27
  inline void SetAdjacentCfebDistance(int adjacent_cfeb_distance) { adjacent_cfeb_distance_ = adjacent_cfeb_distance; } 
  inline int  GetAdjacentCfebDistance() { return adjacent_cfeb_distance_; } 
  inline int  GetReadAdjacentCfebDistance() { return read_adjacent_cfeb_distance_; } 
  //
  //---------------------------------------------------------------------
  //0XF6 = ADR_TEMP1:  CLCT separation
  //---------------------------------------------------------------------
  //!min_clct_separation = [0-255] = minimum 1/2-strip separation between two CLCTs
  inline void SetMinClctSeparation(int min_clct_separation) { min_clct_separation_ = min_clct_separation; } 
  inline int  GetMinClctSeparation() { return min_clct_separation_; } 
  inline int  GetReadMinClctSeparation() { return read_min_clct_separation_; } 
  //
  //---------------------------------------------------------------------
  //0XFC = ADR_CCB_STAT1:  CCB Status Register (cont. from 0x2E)
  //---------------------------------------------------------------------
  //!read_ccb_ttcrx_lock_never = 1 = TTCrx lock never achieved
  inline int GetReadTTCrxLockNever() { return read_ccb_ttcrx_lock_never_; }
  //
  //!read_ccb_ttcrx_lost_ever = 1 = TTCrx lock lost at least once
  inline int GetReadTTCrxLostEver() { return read_ccb_ttcrx_lost_ever_; }
  //
  //!read_ccb_qpll_lock_never = 1 = QPLL lock never achieved
  inline int GetReadQPLLLockNever() { return read_ccb_qpll_lock_never_; }
  //
  //!read_ccb_qpll_lost_ever = 1 = QPLL lock lost at least once
  inline int GetReadQPLLLostEver() { return read_ccb_qpll_lost_ever_; }
  //
  //---------------------------------------------------------------------
  //0X104 = ADR_ALCT_SYNC_CTRL:  ALCT Sync Mode Control
  //---------------------------------------------------------------------
  //!alct_sync_rxdata_dly = sync mode delay pointer to valid data
  inline void SetALCTSyncRxDataDelay(int alct_sync_rxdata_dly) { alct_sync_rxdata_dly_ = alct_sync_rxdata_dly; }
  inline int  GetALCTSyncRxDataDelay() { return alct_sync_rxdata_dly_; }
  inline int  GetReadALCTSyncRxDataDelay() { return read_alct_sync_rxdata_dly_; }
  //
  //!alct_sync_tx_random = 1 = TMB transmits random data to ALCT
  inline void SetALCTSyncTXRandom(int alct_sync_tx_random) { alct_sync_tx_random_ = alct_sync_tx_random; }
  inline int  GetALCTSyncTXRandom() { return alct_sync_tx_random_; }
  inline int  GetReadALCTSyncTXRandom() { return read_alct_sync_tx_random_; }
  //
  //!alct_sync_clear_errors = 1 = ALCT sync mode clear rng errors FF's
  inline void SetALCTSyncClearErrors(int alct_sync_clear_errors) { alct_sync_clear_errors_ = alct_sync_clear_errors; }
  inline int  GetALCTSyncClearErrors() { return alct_sync_clear_errors_; }
  inline int  GetReadALCTSyncClearErrors() { return read_alct_sync_clear_errors_; }
  // 
  //!alct_sync_1st_error = 1st in time match OK, ALCT-to-TMB
  inline int GetReadALCTSync1stError() { return read_alct_sync_1st_error_; }
  //
  //!alct_sync_2nd_error = 2nd in time match OK, ALCT-to-TMB
  inline int GetReadALCTSync2ndError() { return read_alct_sync_2nd_error_; }
  //
  //!alct_sync_1st_error_latched = 1st in time match OK, ALCT-to-TMB, latched
  inline int GetReadALCTSync1stErrorLatched() { return read_alct_sync_1st_error_latched_; }
  //
  //!alct_sync_2nd_error_latched = 2nd in time match OK, ALCT-to-TMB, latched
  inline int GetReadALCTSync2ndErrorLatched() { return read_alct_sync_2nd_error_latched_; }
  //
  //---------------------------------------------------------------------
  //0X106 = ADR_ALCT_SYNC_TXDATA_1ST:  ALCT Sync Mode Transmit Data 1st
  //---------------------------------------------------------------------
  //!alct_sync_txdata_1st = sync mode data to send for loopback 1st in time
  inline void SetALCTSyncTxData1st(int alct_sync_txdata_1st) { alct_sync_txdata_1st_ = alct_sync_txdata_1st; }
  inline int  GetALCTSyncTxData1st() { return alct_sync_txdata_1st_; }
  inline int  GetReadALCTSyncTxData1st() { return read_alct_sync_txdata_1st_; }
  //
  //---------------------------------------------------------------------
  //0X108 = ADR_ALCT_SYNC_TXDATA_2ND:  ALCT Sync Mode Transmit Data 2nd
  //---------------------------------------------------------------------
  //!alct_sync_txdata_2nd = sync mode data to send for loopback 2nd in time
  inline void SetALCTSyncTxData2nd(int alct_sync_txdata_2nd) { alct_sync_txdata_2nd_ = alct_sync_txdata_2nd; }
  inline int  GetALCTSyncTxData2nd() { return alct_sync_txdata_2nd_; }
  inline int  GetReadALCTSyncTxData2nd() { return read_alct_sync_txdata_2nd_; }
  //
  //
  // **********************************************************************************
  //
  //!Return the software value to be written into the register at "address", whose values have been set by the "Set...(int data)" methods
  int  FillTMBRegister(unsigned long int address); 
  //
  void UnjamFPGA();
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
  bool OkTMBVmeWrite(unsigned vme_address);                    // allowed to write this address into user prom?
  //
  // put the vme information into vectors to put into user prom?
  inline void SetTMBFillVmeWriteVecs(bool fill_vectors_or_not)  { tmb_fill_write_vme_vectors_ = fill_vectors_or_not; }
  inline bool GetTMBFillVmeWriteVecs() { return tmb_fill_write_vme_vectors_; }
  //
  // access to vectors of information to go into the userPROM
  inline std::vector<int> GetTMBVecVmeAddress() { return tmb_write_vme_address_; }   
  inline std::vector<int> GetTMBVecDataLsb() { return tmb_write_data_lsb_; }         
  inline std::vector<int> GetTMBVecDataMsb() { return tmb_write_data_msb_; }         
  //
  // clear vectors of information to go into the userPROM
  void ClearTMBVmeWriteVecs();                            
  //
  //---------------------------------------------------------------------
  // The following would be better out of VMEController... 
  // Leave them there now because EMUjtag uses "scan" to do its VME commands
  //
  // allow EMUjtag to tell VMEController to fill up the vector of addresses/commands
  void SetALCTOkVMEWriteAddress(bool address_ok);
  //
  // put the vme information into vectors to put into user prom?
  void SetALCTFillVmeWriteVecs(bool fill_vectors_or_not);
  bool GetALCTFillVmeWriteVecs();
  //
  // access to vectors of information to go into the userPROM
  std::vector<int> GetALCTVecVmeAddress();
  std::vector<int> GetALCTVecDataLsb();
  std::vector<int> GetALCTVecDataMsb();
  //
  // clear vectors of information to go into the userPROM
  void ClearALCTVmeWriteVecs();                            
  //---------------------------------------------------------------------
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
  void CheckTMBConfiguration(int maximum_number_of_reads);
  void CheckTMBConfiguration();                               // this method calls the above method with a fixed maximum number of reads
  inline int GetTMBConfigurationStatus() { return tmb_configuration_status_; }
  inline int GetNumberOfConfigurationReads() { return number_of_configuration_reads_; }
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
  /// insert "value" into the value of the variable pointed at by "data_word" from "lobit" (LSB) to "hibit" (MSB):
  void InsertValueIntoDataWord(int value, int hibit, int lobit, int * data_word);
  //
  int ExtractValueFromData(int data, int lobit, int hibit); /// return the value located in "data" which resides from "lobit" to "hibit"
  //
  int makemask(int lo_bit, int hi_bit); /// return a mask of the right size for the specified number of bits
  //
  int ConvertToHexAscii(int value_to_convert); /// convert the argument to its "hex-ascii" value:  i.e.  2007 -> 0x2007
  //
  int DCSreadAll(char *data); /// read out all DCS values (Temperatures and LVs)
  //
  FILE *pfile;
  int ucla_ldev;
  //std::string version_;
  //
protected:
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
  // VME access methods which should never be called except from within TMB...
  // ucla_start was always called with a dev and a slot
  void start();
  void end();
  // ALCTs need to go to lower scan level, whatever that means
  void start(int,int jtagSource=jtagSourceBoot);
  // does start(1)
  void tmb_vme(char fcn, char vme, const char *snd,char *rcv, int wrt);      
  void tmb_vme_new(char fcn, unsigned vme, unsigned short data, char *rcv, int when);

  bool debug_;
  //
  int number_of_configuration_reads_;
  //
  // on-board voltages and currents
  float v5p0_     ;	      
  float v3p3_     ;
  float v1p5core_ ;
  float v1p5tt_   ;
  float v1p0_	;
  float a5p0_	;	      
  float a3p3_	;
  float a1p5core_ ;
  float a1p5tt_   ;
  float a1p8rat_  ;	        // if SH921 set 1-2, loop backplane sends 1.500vtt
  float v3p3rat_  ;               // if SH921 set 2-3... otherwise not filled...
  float v1p8rat_  ;
  float vref2_    ;
  float vzero_    ;
  float vref_     ;
  //
  std::vector<unsigned long int> InjectedLct0;
  std::vector<unsigned long int> InjectedLct1;
  unsigned long lct0_, lct1_;
  //
  int CLCT0_data_;
  int CLCT1_data_;
  //
  int ALCT0_data_;
  int ALCT1_data_;
  //
  // The following is actually the MaxCounter in TMB + 1 (i.e., they count from 0)
  static const int MaxCounter = 64;
  long int FinalCounter[MaxCounter+2];
  int alct_sent_to_tmb_counter_index_;
  int clct_pretrigger_counter_index_;
  int lct_sent_to_mpc_counter_index_; 
  int l1a_in_tmb_window_counter_index_; 
  //
  //-- TMB and ALCT data in raw hits VME readout --//
  std::vector< std::bitset<16> > tmb_data_;
  int dmb_wordcount_;
  bool ReadTMBRawhits_();
  void DecodeTMBRawHits_();
  void DecodeTMBRawHitWord_(int address);
  //
  bool ReadALCTRawhits_();
  //  std::vector< std::bitset<18> > alct_data_;
  //
  //-- program in user prom --//
  std::vector<int> clocked_out_prom_image_;
  std::vector<int> tmb_write_vme_address_;
  std::vector<int> tmb_write_data_lsb_;
  std::vector<int> tmb_write_data_msb_;
  //
  //-- controls for which registers to allow writing into the userPROM --//
  bool tmb_ok_vme_write_;
  bool tmb_fill_write_vme_vectors_;
  //
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
  int read_boot_disable_hard_reset_alct_;
  int read_boot_disable_VME_;
  int read_boot_disable_mezz_clock_;
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
  int enable_alct_rx_;
  int enable_alct_tx_;
  //
  int read_cfeb_oe_;
  int read_alct_loop_;
  int read_enable_alct_rx_;
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
  //0X2A = ADR_CCB_CFG:  CCB Configuration
  //------------------------------------------------------------------
  int ignore_ccb_rx_                ;
  int disable_ccb_tx_               ;
  int enable_internal_l1a_          ;
  int enable_alctclct_status_to_ccb_;
  int enable_alct_status_to_ccb_    ;
  int enable_clct_status_to_ccb_    ;
  int fire_l1a_oneshot_             ;
  //
  int read_ignore_ccb_rx_                ;
  int read_disable_ccb_tx_               ;
  int read_enable_internal_l1a_          ;
  int read_enable_alctclct_status_to_ccb_;
  int read_enable_alct_status_to_ccb_    ;
  int read_enable_clct_status_to_ccb_    ;
  int read_fire_l1a_oneshot_             ;
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
  //0X3A = ADR_ALCT0_RCD:  ALCT 1st Muon received by TMB
  //------------------------------------------------------------------
  int read_alct0_valid_  ;
  int read_alct0_quality_;
  int read_alct0_amu_    ;
  int read_alct0_key_wg_ ;
  int read_alct0_bxn_    ;
  //
  //------------------------------------------------------------------
  //0X3C = ADR_ALCT1_RCD:  ALCT 2nd Muon received by TMB
  //------------------------------------------------------------------
  int read_alct1_valid_  ;
  int read_alct1_quality_;
  int read_alct1_amu_    ;
  int read_alct1_key_wg_ ;
  int read_alct1_bxn_    ;
  //
  //------------------------------------------------------------------
  //0X3E = ADR_ALCT_INJ:  ALCT FIFO RAM Status
  //------------------------------------------------------------------
  int read_alct_raw_busy_;
  int read_alct_raw_done_;
  int read_alct_raw_word_count_;
  int read_alct_raw_msbs_;
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
  int cfeb_enable_source_orig;
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
  inline void SetCfebEnableSource_(int cfeb_enable_source) { cfeb_enable_source_ = cfeb_enable_source; }
  //cfeb_enable_source = [0,1] = [0x68,0x42] is source of cfeb_enable
  //See TMB documentation before setting this bit.
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
  //
  int read_dmb_ext_trig_delay_ ;
  int read_clct_ext_trig_delay_;
  int read_alct_ext_trig_delay_;
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
  int hit_thresh_;
  int aff_thresh_;
  int min_hits_pattern_;
  int drift_delay_;
  int pretrigger_halt_;
  //
  int read_triad_persist_;
  int read_hit_thresh_;
  int read_aff_thresh_;
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
  int fifo_no_raw_hits_;
  //
  int read_fifo_mode_;
  int read_fifo_tbins_;
  int read_fifo_pretrig_;
  int read_fifo_no_raw_hits_;
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
  //0X78 = ADR_SEQ_CLCT0:  Sequencer Latched CLCT0
  //------------------------------------------------------------------
  int read_CLCT0_valid_       ;
  int read_CLCT0_nhit_        ;
  int read_CLCT0_pattern_     ;
  int read_CLCT0_keyHalfStrip_;
  //
  //------------------------------------------------------------------
  //0X7A = ADR_SEQ_CLCT1:  Sequencer Latched CLCT1
  //------------------------------------------------------------------
  int read_CLCT1_valid_       ;
  int read_CLCT1_nhit_        ;
  int read_CLCT1_pattern_     ;
  int read_CLCT1_keyHalfStrip_;
  //
  //------------------------------------------------------------------
  //0X86 = ADR_TMB_TRIG:  TMB Trigger configuration/MPC accept
  //------------------------------------------------------------------
  int tmb_sync_err_enable_;
  int tmb_allow_alct_;
  int tmb_allow_clct_;
  int tmb_allow_match_;
  int mpc_rx_delay_;
  int mpc_sel_ttc_bx0_;
  int mpc_idle_blank_;
  int mpc_output_enable_;
  //
  int read_tmb_sync_err_enable_;
  int read_tmb_allow_alct_;
  int read_tmb_allow_clct_;
  int read_tmb_allow_match_;
  int read_mpc_rx_delay_;
  int read_mpc_accept_;
  int read_mpc_reserved_;
  int read_mpc_sel_ttc_bx0_;
  int read_mpc_idle_blank_;
  int read_mpc_output_enable_;
  //
  //------------------------------------------------------------------
  //0X98 = ADR_SCP_CTRL:  Scope control
  //------------------------------------------------------------------
  int scope_in_readout_;
  //
  int read_scope_in_readout_;
  //
  //------------------------------------------------------------------
  //0XA8 = ADR_ALCTFIFO1:  ALCT Raw Hits RAM Control
  //------------------------------------------------------------------
  int alct_raw_reset_;
  int alct_raw_read_address_;
  int alct_demux_mode_;
  //
  int read_alct_raw_reset_;
  int read_alct_raw_read_address_;
  int read_alct_demux_mode_;
  //
  //------------------------------------------------------------------
  //0XAA = ADR_ALCTFIFO2:  ALCT Raw Hits RAM Control
  //------------------------------------------------------------------
  int read_alct_raw_lsbs_;
  //
  //------------------------------------------------------------------
  //0XAC = ADR_SEQMOD:  Sequencer Trigger Modifiers
  //------------------------------------------------------------------
  int clct_flush_delay_;
  int wr_buffer_autoclear_;
  int clct_write_continuous_enable_;
  int wrt_buf_required_;
  int valid_clct_required_;
  int l1a_allow_match_;
  int l1a_allow_notmb_;
  int l1a_allow_nol1a_;
  int l1a_allow_alct_only_;
  int scint_veto_clr_;
  //
  int read_clct_flush_delay_;
  int read_wr_buffer_autoclear_;
  int read_clct_write_continuous_enable_;
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
  //0XAE = ADR_SEQSM:  Sequencer Machine State
  //------------------------------------------------------------------
  int read_clct_state_machine_;      
  int read_readout_state_machine_;   
  int read_buffer_queue_full_;      
  int read_buffer_queue_empty_;    
  int read_buffer_queue_overflow_;  
  int read_buffer_queue_underflow_; 
  //
  //------------------------------------------------------------------
  //0XB0 = ADR_SEQCLCTM:  Sequencer CLCT (Most significant bits)
  //------------------------------------------------------------------
  int read_CLCT_BXN_;
  int read_CLCT_sync_err_;
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
  //0XC4 = ADR_RPC_TBINS:  RPC FIFO Time Bins
  //------------------------------------------------------------------
  int fifo_tbins_rpc_    ;
  int fifo_pretrig_rpc_  ;
  int rpc_decouple_;
  //
  int read_fifo_tbins_rpc_    ;
  int read_fifo_pretrig_rpc_  ;
  int read_rpc_decouple_;
  //
  //------------------------------------------------------------------
  //0XCA = ADR_BX0_DELAY:  BX0 to MPC delays
  //------------------------------------------------------------------
  int alct_bx0_delay_ ;
  int clct_bx0_delay_ ;
  int alct_bx0_enable_;
  //
  int read_alct_bx0_delay_ ;
  int read_clct_bx0_delay_ ;
  int read_alct_bx0_enable_;
  //
  //-----------------------------------------------------------------------------
  //0XCC = ADR_NON_TRIG_RO:  Non-Triggering Event Enables + ME1/1A(1B) reversal 
  //-----------------------------------------------------------------------------
  int tmb_allow_alct_nontrig_readout_  ;
  int tmb_allow_clct_nontrig_readout_  ;
  int tmb_allow_match_nontrig_readout_ ;
  int mpc_block_me1a_                  ;
  int clct_pretrigger_counter_non_me11_;
  int csc_me11_                        ;
  int clct_stagger_                    ;
  int reverse_stagger_                 ;
  int reverse_me1a_                    ;
  int reverse_me1b_                    ;
  int tmb_firmware_compile_type_       ;
  //
  int read_tmb_allow_alct_nontrig_readout_  ;
  int read_tmb_allow_clct_nontrig_readout_  ;
  int read_tmb_allow_match_nontrig_readout_ ;
  int read_mpc_block_me1a_                  ;
  int read_clct_pretrigger_counter_non_me11_;
  int read_csc_me11_                        ;
  int read_clct_stagger_                    ;
  int read_reverse_stagger_                 ;
  int read_reverse_me1a_                    ;
  int read_reverse_me1b_                    ;
  int read_tmb_firmware_compile_type_       ;
  int expected_tmb_firmware_compile_type_   ; //this is the value we expect to readback from the hardware
  //
  //------------------------------------------------------------------
  //0XD4 = ADR_JTAGSM0:  JTAG State Machine Control (reads JTAG PROM)
  //------------------------------------------------------------------
  int jtag_state_machine_start_;
  int jtag_state_machine_sreset_;
  int jtag_disable_write_to_adr10_;
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
  int read_jtag_disable_write_to_adr10_;
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
  int clct_throttle_;
  //
  int read_layer_trigger_en_ ;
  int read_layer_trig_thresh_;
  int read_number_layers_hit_;
  int read_clct_throttle_;
  //
  //---------------------------------------------------------------------
  //0XF4 = ADR_TEMP0:  Pattern Finder Pretrigger
  //---------------------------------------------------------------------
  int clct_blanking_;
  int clct_pattern_id_thresh_;
  int clct_pattern_id_thresh_postdrift_;
  int adjacent_cfeb_distance_;
  //
  int read_clct_blanking_;
  int read_clct_pattern_id_thresh_;
  int read_clct_pattern_id_thresh_postdrift_;
  int read_adjacent_cfeb_distance_;
  //
  //---------------------------------------------------------------------
  //0XF6 = ADR_TEMP1:  CLCT separation
  //---------------------------------------------------------------------
  int clct_separation_src_;
  int clct_separation_ram_write_enable_;
  int clct_separation_ram_adr_;
  int min_clct_separation_;
  //
  int read_clct_separation_src_;
  int read_clct_separation_ram_write_enable_;
  int read_clct_separation_ram_adr_;
  int read_min_clct_separation_;
  //
  //---------------------------------------------------------------------
  //0XFC = ADR_CCB_STAT1:  CCB Status Register (cont. from 0x2E)
  //---------------------------------------------------------------------
  int read_ccb_ttcrx_lock_never_;
  int read_ccb_ttcrx_lost_ever_ ;
  int read_ccb_qpll_lock_never_ ;
  int read_ccb_qpll_lost_ever_  ;
  //
  //---------------------------------------------------------------------
  //0X104 = ADR_ALCT_SYNC_CTRL:  ALCT Sync Mode Control
  //---------------------------------------------------------------------
  int alct_sync_rxdata_dly_       ;
  int alct_sync_tx_random_        ;
  int alct_sync_clear_errors_     ;
  //
  int read_alct_sync_rxdata_dly_       ;
  int read_alct_sync_tx_random_        ;
  int read_alct_sync_clear_errors_     ;
  int read_alct_sync_1st_error_        ;
  int read_alct_sync_2nd_error_        ;
  int read_alct_sync_1st_error_latched_;
  int read_alct_sync_2nd_error_latched_;
  //
  //---------------------------------------------------------------------
  //0X106 = ADR_ALCT_SYNC_TXDATA_1ST:  ALCT Sync Mode Transmit Data 1st
  //---------------------------------------------------------------------
  int alct_sync_txdata_1st_;
  //
  int read_alct_sync_txdata_1st_;
  //
  //---------------------------------------------------------------------
  //0X108 = ADR_ALCT_SYNC_TXDATA_2ND:  ALCT Sync Mode Transmit Data 2nd
  //---------------------------------------------------------------------
  int alct_sync_txdata_2nd_;
  //
  int read_alct_sync_txdata_2nd_;
  //
  //
  //*******************************************************************
  // TMB Raw Hits header words
  //*******************************************************************
  //-----------
  // header 0
  //-----------
  int h0_beginning_of_cathode_ ;
  int h0_marker_6_             ;
  //
  //-----------
  // header 1
  //-----------
  int h1_nTbins_per_cfeb_ ;
  int h1_cfebs_read_      ;
  //
  // fifo mode
  // 0 = no raw hits, full header (if buffer was available at pretrigger)
  // 1 = all 5 CFEBs raw hits, full header (if buffer was available at pretrigger)
  // 2 = Local raw hits, full header (if buffer was available at pretrigger)
  // 3 = no raw hits, short header
  // 4 = no raw hits, no header
  int h1_fifo_mode_       ;
  //
  //-----------
  // header 2
  //-----------
  int h2_l1a_counter_ ;
  int h2_csc_id_      ;
  int h2_board_id_    ;
  //
  // L1A pop type mode:  
  // 0 = Normal CLCT trigger with buffer data and L1A window match
  // 1 = ALCT-only trigger, no data buffers
  // 2 = L1A-only, no matching TMB trigger, no buffer data
  // 3 = TMB triggered, no L1A-window match, event has buffer data
  int h2_l1a_type_    ;
  //
  //-----------
  // header 3
  //-----------
  int h3_bxn_counter_   ;
  //
  // record type: 
  //   0 = rawhits no   , header full
  //   1 = rawhits full , header full
  //   2 = rawhits local, header full
  //   3 = rawhits no   , header short (no buffer was available at pretrigger)
  int h3_record_type_   ;
  int h3_scope_in_data_ ;
  //
  //-----------
  // header 4
  //-----------
  int h4_nheader_words_   ;
  int h4_nCFEBs_read_     ;
  int h4_has_buffer_data_ ;
  int h4_fifo_pretrig_    ;
  //
  //-----------
  // header 5
  //-----------
  int h5_l1a_at_pretrig_                   ;
  int h5_trigger_source_vector_            ;
  int h5_trigger_source_halfstrip_distrip_ ;
  //
  //-----------
  // header 6
  //-----------
  int h6_aff_to_dmb_  ;
  int h6_cfeb_exists_ ;
  int h6_run_info_    ;
  //
  //-----------
  // header 7
  //-----------
  int h7_bxn_at_clct_pretrig_ ;
  int h7_sync_err_            ;
  //
  //-----------
  // header 8
  //-----------
  int h8_clct0_lsbs_ ;
  //
  //-----------
  // header 9
  //-----------
  int h9_clct1_lsbs_ ;
  //
  //-----------
  // header 10
  //-----------
  int h10_clct0_msbs_            ;
  int h10_clct1_msbs_            ;
  int h10_clct0_invalid_pattern_ ;
  //
  //-----------
  // header 11
  //-----------
  int h11_alct_clct_match_           ;
  int h11_alct_trig_only_            ;
  int h11_clct_trig_only_            ;
  int h11_clct0_alct_bxn_diff_       ;
  int h11_clct1_alct_bxn_diff_       ;
  int h11_alct_in_clct_match_window_ ;
  int h11_triad_persistence_         ;
  //
  //-----------
  // header 12
  //-----------
  int h12_mpc0_frame0_lsbs_ ;
  //
  //-----------
  // header 13
  //-----------
  int h13_mpc0_frame1_lsbs_ ;
  //
  //-----------
  // header 14
  //-----------
  int h14_mpc1_frame0_lsbs_ ;
  //
  //-----------
  // header 15
  //-----------
  int h15_mpc1_frame1_lsbs_ ;
  //
  //-----------
  // header 16
  //-----------
  int h16_mpc0_frame0_msbs_              ;
  int h16_mpc0_frame1_msbs_              ;
  int h16_mpc1_frame0_msbs_              ;
  int h16_mpc1_frame1_msbs_              ;
  int h16_mpc_accept_                    ;
  int h16_clct_halfstrip_pretrig_thresh_ ;
  int h16_clct_distrip_pretrig_thresh_   ;
  //
  //-----------
  // header 17
  //-----------
  int h17_write_buffer_ready_     ;
  int h17_pretrig_tbin_           ;
  int h17_write_buffer_address_   ;
  int h17_pretrig_no_free_buffer_ ;
  int h17_buffer_full_            ;
  int h17_buffer_almost_full_     ;
  int h17_buffer_half_full_       ;
  int h17_buffer_empty_           ;
  //
  //-----------
  // header 18
  //-----------
  int h18_nbuf_busy_          ;
  int h18_buf_busy_           ;
  int h18_l1a_stack_overflow_ ;
  //
  //-----------
  // header 19
  //-----------
  int h19_tmb_trig_pulse_         ;
  int h19_tmb_alct_only_          ;
  int h19_tmb_clct_only_          ;
  int h19_tmb_match_              ;
  int h19_write_buffer_ready_     ;
  int h19_write_buffer_available_ ;
  int h19_write_tbin_address_     ;
  int h19_write_buffer_address_   ;
  //
  //-----------
  // header 20
  //-----------
  int h20_discard_no_write_buf_available_ ;
  int h20_discard_invalid_pattern_        ;
  int h20_discard_tmb_reject_             ;
  int h20_timeout_no_tmb_trig_pulse_      ;
  int h20_timeout_no_mpc_frame_           ;
  int h20_timeout_no_mpc_response_        ;
  //
  //-----------
  // header 21
  //-----------
  int h21_match_trig_alct_delay_   ;
  int h21_match_trig_window_width_ ;
  int h21_mpc_tx_delay_            ;
  //
  //-----------
  // header 22
  //-----------
  int h22_rpc_exist_       ;
  int h22_rpc_list_        ;
  int h22_nrpc_            ;
  int h22_rpc_read_enable_ ;
  int h22_nlayers_hit_     ;
  int h22_l1a_in_window_   ;
  //
  //-----------
  // header 23
  //-----------
  int h23_board_status_ ;
  //
  //-----------
  // header 24
  //-----------
  int h24_time_since_hard_reset_ ;
  //
  //-----------
  // header 25
  //-----------
  int h25_firmware_version_date_code_ ;
  //
};

  } // namespace emu::pc
} // namespace emu

#endif

 
