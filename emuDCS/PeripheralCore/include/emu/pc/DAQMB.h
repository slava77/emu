//-----------------------------------------------------------------------
// $Id: DAQMB.h,v 1.22 2012/12/03 17:00:55 banicz Exp $
// $Log: DAQMB.h,v $
// Revision 1.22  2012/12/03 17:00:55  banicz
// Dan's changes:
// add a function to set and initialize the dcfeb pipeline and fine delays for use after hard resets which wipe these values from the dcfebs.
//
// Revision 1.21  2012/11/26 21:10:02  liu
// add DCFEB pipeline_depth parameter
//
// Revision 1.20  2012/10/16 22:39:07  liu
// read & write FPGA internal registers
//
// Revision 1.19  2012/10/15 16:02:37  liu
// DCFEB firmware loading update
//
// Revision 1.18  2012/10/11 21:26:45  liu
// add DCFEB firmware download and readback
//
// Revision 1.17  2012/10/08 22:04:44  liu
// DCFEB update
//
// Revision 1.16  2012/10/08 02:35:05  liu
// DCFEB update
//
// Revision 1.15  2012/09/30 21:19:42  liu
// update for ME11 new electronics
//
// Revision 1.14  2012/09/05 22:34:46  liu
// introduce HardwareVersion attribute
//
// Revision 1.13  2012/07/10 15:27:57  liu
// add function used by STEP
//
// Revision 1.12  2012/06/20 08:45:00  kkotov
//
// New faster DMB/CFEB EPROM readback routines
//
// Revision 1.11  2012/03/19 22:52:36  liu
// for Khritian Kotov: CFEB firmware fix
//
// Revision 1.10  2012/02/14 09:46:14  liu
// fix FPGA & PROM ID bugs
//
// Revision 1.9  2011/08/26 16:31:57  cvuosalo
// Adding CFEB FPGA check to Expert Tools and DMB Utils pages
//
// Revision 1.8  2010/08/23 11:24:25  rakness
// add DMB Control and FPGA firmware check
//
// Revision 1.7  2010/03/03 20:33:19  liu
// adjust CFEB calib timing by Stan
//
// Revision 1.6  2009/12/18 09:42:48  rakness
// update firmware reloading routine to emulate expert actions
//
// Revision 1.5  2009/10/25 09:54:45  liu
// add a new parameter power_mask for DMB, more counters for CCB
//
// Revision 1.4  2009/08/15 07:56:47  durkin
// fixed several bugs and added new routines
//
// Revision 1.3  2009/08/13 01:48:21  liu
// to skip monitoring if vme access failed
//
// Revision 1.2  2009/03/25 10:19:41  liu
// move header files to include/emu/pc
//
// Revision 1.1  2009/03/25 10:07:42  liu
// move header files to include/emu/pc
//
// Revision 3.33  2008/10/27 08:51:51  gujh
// Put the CFEB PROM firmware readback capability
//    ---- GU, Oct. 27, 2008
//
// Revision 3.32  2008/09/21 18:26:14  liu
// monitoring update
//
// Revision 3.31  2008/08/13 11:30:53  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.30  2008/08/08 11:01:23  rakness
// centralize logging
//
// Revision 3.29  2008/08/06 17:24:50  rakness
// add known_problem parameter to xml file; add time stamp + number of reads to config check output file
//
// Revision 3.28  2008/08/05 08:40:36  rakness
// add minimum number of times to read when checking configuration
//
// Revision 3.27  2008/07/21 08:25:57  rakness
// add mapping into comments for SetKillInput(int)
//
// Revision 3.26  2008/07/16 17:28:36  rakness
// (backwards incompatible!) updates for 3 June 2008 TMB firmware and v3 r10 DMB firmware
//
// Revision 3.25  2008/07/08 10:41:22  rakness
// add getter/setter for kill_chip
//
// Revision 3.24  2008/06/22 14:57:52  liu
// new functions for monitoring
//
// Revision 3.23  2008/06/12 21:08:54  rakness
// add firmware tags for DMB, CFEB, MPC, CCB into xml file; add check firmware button
//
// Revision 3.22  2008/03/30 08:25:15  gujh
// Added the corresponding code for DMB/CFEB fine L1A delay adjustment ---GU, Mar. 30, 2008
//
// Revision 3.21  2008/03/17 08:35:24  rakness
// DAQMB configuration check; turn on chambers before configuration (committed for S. Durkin)
//
// Revision 3.20  2008/02/24 12:48:29  liu
// DMB online counters
//
// Revision 3.19  2007/06/06 14:49:32  gujh
// Added buck_shift_comp_bc and set_comp_thresh_bc
//      ---- June 6, 2007.  GU
//
// Revision 3.18  2007/05/30 16:48:21  rakness
// DMB cable_delay scans/tools
//
// Revision 3.17  2007/04/20 16:07:48  gujh
// Added epromload_broadcast
//     --- Apr. 20, 2007   GU
//
// Revision 3.16  2007/04/12 16:39:49  gujh
// Added buck_shift_ext_bc(strip) function
//     Apr. 12, 2007.  GU
//
// Revision 3.15  2007/03/19 13:35:09  geurts
// make the new cable_delay functions inline
//
// Revision 3.14  2007/03/14 20:41:43  gujh
// Divide the cable_delay into four sections
//       --- Mar. 14, 2007  GU
//
// Revision 3.13  2007/03/13 20:57:13  gujh
// Add function set_rndmtrg_rate(int)
//                  ---- Mar. 13, 2007   GU
//
// Revision 3.12  2006/11/27 15:06:05  mey
// Made chamber(crate)
//
// Revision 3.11  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.10  2006/11/10 16:51:44  mey
// Update
//
// Revision 3.9  2006/10/10 11:10:07  mey
// Update
//
// Revision 3.8  2006/10/05 08:20:06  mey
// UPdate
//
// Revision 3.7  2006/10/05 07:49:41  mey
// UPdate
//
// Revision 3.6  2006/10/02 13:44:58  mey
// Update
//
// Revision 3.5  2006/09/27 16:19:08  mey
// UPdate
//
// Revision 3.4  2006/09/19 08:16:52  mey
// UPdate
//
// Revision 3.3  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.2  2006/09/12 15:50:01  mey
// New software changes to DMB abd CFEB
//
// Revision 3.1  2006/08/02 12:24:55  mey
// Added LctL1aDelay
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.28  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.27  2006/07/18 14:09:33  mey
// Update
//
// Revision 2.26  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.25  2006/06/23 14:21:33  mey
// Moved public members
//
// Revision 2.24  2006/06/20 13:37:45  mey
// Udpate
//
// Revision 2.23  2006/05/31 15:58:46  mey
// Update
//
// Revision 2.22  2006/05/29 08:09:20  mey
// Update
//
// Revision 2.21  2006/05/16 15:54:37  mey
// UPdate
//
// Revision 2.20  2006/04/06 22:23:07  mey
// Update
//
// Revision 2.19  2006/03/17 14:06:15  mey
// Added set test status
//
// Revision 2.18  2006/01/30 09:29:29  mey
// Update
//
// Revision 2.17  2006/01/13 10:06:49  mey
// Got rid of virtuals
//
// Revision 2.16  2006/01/12 23:43:43  mey
// Update
//
// Revision 2.15  2006/01/12 12:27:44  mey
// UPdate
//
// Revision 2.14  2006/01/11 13:25:21  mey
// Update
//
// Revision 2.13  2006/01/10 23:32:09  mey
// Update
//
// Revision 2.12  2005/12/06 17:58:19  mey
// Update
//
// Revision 2.11  2005/10/04 16:02:15  mey
// Added Output
//
// Revision 2.10  2005/09/28 17:11:19  mey
// Update
//
// Revision 2.9  2005/09/28 16:57:30  mey
// Update Tests
//
// Revision 2.8  2005/09/26 07:27:05  mey
// Added BXN toogle routine
//
// Revision 2.7  2005/09/19 07:14:48  mey
// Update
//
// Revision 2.6  2005/09/13 14:46:39  mey
// Get DMB crate id; and DCS
//
// Revision 2.5  2005/09/07 16:18:15  mey
// DMB timing routines
//
// Revision 2.4  2005/09/07 13:54:21  mey
// Included new timing routines from Jianhui
//
// Revision 2.3  2005/08/31 15:12:56  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.2  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.1  2005/06/06 10:05:50  geurts
// calibration-related updates by Alex Tumanov and Jason Gilmore
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
/* New motherboard VTX2 not VTX  so MCTRL,6  not MCTRL,5 */
#ifndef DAQMB_h
#define DAQMB_h
#include <iostream>
#include <vector>
#include <string>
#include "emu/pc/VMEModule.h"
#include "emu/pc/JTAG_constants.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/Chamber.h"
#include "emu/pc/EmuLogger.h"


namespace emu {
  namespace pc {

//
/**
 * more documentation available for this class at
 * http://www.physics.ohio-state.edu/~cms/dmb/prog_man.pdf
 * http://www.physics.ohio-state.edu/~cms/dmb/delayset.html
 */

class Crate;

class DAQMB: public VMEModule, public EmuLogger
{
public:
  //friend class DAQMBParser;

  // standard routines
  /// construct with all the standard CFEBs and BuckeyeChips
  explicit DAQMB(Crate * ,Chamber *, int ) ;
  /// construct with a single CFEB, count from zero
  //DAQMB(Crate * , int slot, int cfeb);
  //DAQMB(Crate * , int slot, const std::vector<CFEB> & cfebs);
  //
  virtual ~DAQMB();
  /// from the BOARDTYPE enum
  unsigned int boardType() const {return DMB_ENUM;}
  void end();

  std::vector<CFEB> cfebs() const {return cfebs_;}

// DAQMB constants&resets

  
  friend std::ostream & operator<<(std::ostream & os, DAQMB & daqmb);

  void trigset2(int, int [] );
  void trgfire();
  void enable_cfeb();
  void setcrateid(int dword);
  void setfebdelay(int dword);
  void setcaldelay(int dword);
  void setdavdelay(int dword);
  void fxpreblkend(int dword);
  void settrgsrc(int dword); 
  void calctrl_fifomrst();
  void calctrl_global();
  void restoreCFEBIdle();
  void restoreMotherboardIdle();
  void PrintCounters(int);
  int readfifox_chk(enum DEVTYPE,unsigned int short);
  void wrtfifox(enum DEVTYPE,unsigned short int);
  int readfifox_123chk(enum DEVTYPE devnum);
  void wrtfifo_123(enum DEVTYPE devnum);
  void wrtfifo_toggle(enum DEVTYPE devnum);
  int readfifox_togglechk(enum DEVTYPE devnum);
  //
  void lowv_dump();
  void daqmb_adc_dump();
  void daqmb_promfpga_dump();
  std::string CounterName(int);
  //
  // Tests
  void test3();
  int  test4();
  int  test5();
  int  test6();
  int  test8();
  int  test9();
  int  test10();
  int  test11();
  int  memchk(int);
  int  memchk(DEVTYPE);
  //
  void dmb_readstatus(char [11]);
  void cfebs_readstatus();
  // 
  void SFMWriteProtect();
  void LoadCFEBDelaySFM();
  void LoadDMBIdSFM();
  void LoadCableDelaySFM();
  void ProgramSFM();
  void WriteSFM();
  void PrintCounters();
  //  
  // DAQMB trigger primitives
  //
  void ToogleBXN();

// DAQMB trigger primitives

  void set_comp_mode(int dword);
  void set_comp_thresh_bc(float thresh);
  void set_comp_thresh(float thresh);
  void set_comp_thresh(int, float thresh);
  
  /// DAQMB voltages
  /// see manual, but usually ichp 1 = therm, 2 = DAC, 3 = GND
  /// voltages which range from 0-4.096
  float adcplus(int ichp,int ichn);
  /// voltages which range from +/- 2.048, usually grounds
  float adcminus(int ichp,int ichn);
  float adc16(int ichp,int ichn);
  /// internal function used by the above 
  unsigned int readADC(int ichp, int ichn);

  void set_dac(float volt0,float volt1);
  void trigtest();

// DAQMB Thermometers
  enum Temps {DMBTEMP, CFEB1TEMP, CFEB2TEMP, CFEB3TEMP, CFEB4TEMP,
              CFEB5TEMP, V18REFTEMP, VIRTEXTEMP};
  float readthermx(int feb);

// DAQMB LVMB routines
  float lowv_adc(int ichp,int ichn);
  void lowv_onoff(char c);
  unsigned int lowv_rdpwrreg();
// DAQMB prom readbacks
  unsigned int febpromuser(CFEB &);
  unsigned int febpromid(CFEB &);
  unsigned int febfpgauser(CFEB &);
  unsigned int febfpgaid(CFEB &);
  unsigned int mbpromuser(int prom);
  unsigned int mbpromid(int prom);
  unsigned int mbfpgauser();
  unsigned int mbfpgaid();
  void vmefpgaid();
// DAQMB fifo write and reads
  void wrtfifo(int fifo,int nsndfifo,char* sndfifo);
  void readfifo(int fifo,int nrcvfifo,char* rcvfifo);
// DAQMB load and read flash memory (electronics experts only)
  void buckflash_init();
  void buckflash_load(char *fshift);
  void buckflash_read(char *rshift);
  void buckflash_pflash(); 
  void buckflash_erase();
  int  Fill_BUCK_FLASH_contents(char * buf);
  void jtag_buckflash_engine(int nbuf,char *buf,int n,int t,int d);
  void jtag_buckflash_engine2(int nbuf,char *buf,int n,int *val);
  void buckflash_dump(int nbuf,char *buf);
  void buckflash_load2(int nbytes,char *fshift);
  void buckflash_read2(int nbytes,char *rshift);
  //
  void preamp_initx();
// DAQMB program proms (electronics experts only)
  void epromload_verify(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);
	void epromread(DEVTYPE devnum);
	int  check_eprom_readback(const char *rbkfile, const char *expfile);
  void epromload(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);
  void epromload_broadcast(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum, int ipass);
  void epromloadOld(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);
  void rdbkvirtex(DEVTYPE);
  void rdbkvirtexII();
// DAQMB calibrate
  void set_cal_dac(float volt0,float volt1);
  void buck_shift();
  void buck_shift_out();
  int buck_shift_test();
  //
  void set_cal_tim_pulse(int ntim);
  void set_cal_tim_inject(int ntim);
  //
  inline void SetLVDBMapping(int map) { lvdb_mapping_ = map; }
  inline int GetLVDBMapping() { return lvdb_mapping_; }
  int LVDB_map(int chn);
  
  // firmware version tags:
  inline void SetExpectedControlFirmwareTag(long int tag) { expected_control_firmware_tag_ = (unsigned long int) tag; }
  inline unsigned long int GetExpectedControlFirmwareTag()  { return expected_control_firmware_tag_; }
  bool CheckControlFirmwareVersion();
  //
  inline void SetExpectedVMEFirmwareTag(int tag) { expected_vme_firmware_tag_ = (unsigned long int) tag; }
  inline int  GetExpectedVMEFirmwareTag() { return expected_vme_firmware_tag_; }
  bool CheckVMEFirmwareVersion();
  //
  inline void SetExpectedCFEBFirmwareTag(int cfeb_index, long int tag) { expected_cfeb_firmware_tag_[cfeb_index] = tag; }
  inline unsigned long int GetExpectedCFEBFirmwareTag(int cfeb_index)  { return expected_cfeb_firmware_tag_[cfeb_index]; }
  bool CheckCFEBFirmwareVersion(CFEB & cfeb_to_check);
  //
  //
  inline void SetFebDavDelay(int delay){feb_dav_delay_ = delay;}
  inline int GetFebDavDelay(){return feb_dav_delay_;}
  //
  inline void SetTmbDavDelay(int delay){tmb_dav_delay_ = delay;}
  inline int GetTmbDavDelay(){return tmb_dav_delay_;}
  //
  inline void SetPushDavDelay(int delay){push_dav_delay_ = delay;}
  inline int GetPushDavDelay(){return push_dav_delay_;}
  //
  inline void SetL1aDavDelay(int delay){l1acc_dav_delay_ = delay;}
  inline int GetL1aDavDelay(){return l1acc_dav_delay_;}
  //
  inline void SetAlctDavDelay(int delay){ALCT_dav_delay_ = delay;}
  inline int GetAlctDavDelay(){return ALCT_dav_delay_;}
  //
  inline void SetCalibrationLctDelay(int delay){calibration_LCT_delay_ = delay;}
  inline int GetCalibrationLctDelay(){return calibration_LCT_delay_;}
  //
  inline void SetCalibrationL1aDelay(int delay){calibration_l1acc_delay_ = delay;}
  inline int GetCalibrationL1aDelay(){return calibration_l1acc_delay_;}
  //
  inline void SetPulseDelay(int delay){pulse_delay_ = delay;}
  inline int GetPulseDelay(){return pulse_delay_;}
  //
  inline void SetInjectDelay(int delay){inject_delay_ = delay;}
  inline int GetInjectDelay(){return inject_delay_;}
  //
  //void SetFebClockDelay(int delay){feb_clock_delay_ = delay;}
  //
  inline void SetCompMode(int mode){comp_mode_ = mode;}
  inline int GetCompMode(){return comp_mode_;}
  //
  inline void SetCompTiming(int delay){comp_timing_ = delay;}
  inline int GetCompTiming(){return comp_timing_;}
  //
  inline void SetPreBlockEnd(int delay){pre_block_end_ = delay;}
  inline int GetPreBlockEnd(){return pre_block_end_ ;}
  //
  //  void SetCableDelay(int delay){cable_delay_ = delay;}
  inline void SetCfebCableDelay(int delay){cable_delay_=(cable_delay_&0xfe)+(delay&0x01);}
  inline void SetTmbLctCableDelay(int delay){cable_delay_=(cable_delay_&0xf1)+((delay<<1)&0x0e);}
  inline void SetCfebDavCableDelay(int delay){cable_delay_=(cable_delay_&0xcf)+((delay<<4)&0x30);}
  inline void SetAlctDavCableDelay(int delay){cable_delay_=(cable_delay_&0x3f)+((delay<<6)&0xc0);}
  //
  inline int GetCableDelay()        { return cable_delay_; }
  inline int GetCfebCableDelay()    { return ((cable_delay_ >> 0) & 0x01); }
  inline int GetTmbLctCableDelay()  { return ((cable_delay_ >> 1) & 0x07); }
  inline int GetCfebDavCableDelay() { return ((cable_delay_ >> 4) & 0x03); }
  inline int GetAlctDavCableDelay() { return ((cable_delay_ >> 6) & 0x03); }
  //
  inline void SetCrateId(int id){crate_id_ = id;}
  //
  inline void SetCfebClkDelay(int delay){killflatclk_ = (killflatclk_&0x3fe0)+(delay&0x1f);}
  inline int GetCfebClkDelay(){return (killflatclk_&0x1f);}
  //
  inline void SetxLatency(int latency){killflatclk_= (killflatclk_&0x3f9f)+(( latency<<5)&0x60);}
  inline int GetxLatency(){return ((killflatclk_>>5)&0x03);}
  //
  inline void SetxFineLatency(int finelatency){killflatclk_ = (killflatclk_&0x3ff)+((finelatency<<10)&0x3c00);}
  inline int GetxFineLatency(){return ((killflatclk_>>10)&0x0f);}
  //
  //Input values for SetKillInput(int killinput):
  //killinput="0" normal mode, no one is disbaled
  //killinput="1" ALCT data are disabled
  //killinput="2" TMB data are disabled
  //killinput="3" CFEB#1 data are disabled
  //killinput="4" CFEB#2 data are disabled
  //killinput="5" CFEB#3 data are disabled
  //killinput="6" CFEB#4 data are disabled
  //killinput="7" CFEB#5 data are disabled
  //
  inline void SetKillInput(int killinput){killflatclk_ = (killflatclk_&0x3c7f)+((killinput<<7)&0x380);}
  inline int GetKillInput(){return ((killflatclk_>>7)&0x07);}

  // The following two are for ODMB.  Cannot share the two methods above with DMB,
  // because these methods might be called before hardware_version_ has been set.
  // Internally, DMB uses killflatclk_, ODMB use kill_input_mask_).
  inline void SetKillInputMask(int killinput) {kill_input_mask_ = killinput;}
  inline int GetKillInputMask() {return kill_input_mask_;}
 
  //
  inline void SetPulseDac(float value){pul_dac_set_= value;}
  inline float GetPulseDac(){return pul_dac_set_;}
  //
  inline void SetInjectorDac(float value){inj_dac_set_=value;}
  inline float GetInjectorDac(){return inj_dac_set_;}
  //
  inline void SetCompThresh(float value){set_comp_thresh_=value;}
  inline float GetCompThresh(){return set_comp_thresh_;}
  //
  inline void SetCompModeCfeb(int cfeb, int value){comp_mode_cfeb_[cfeb]=value;}
  inline void SetCompTimingCfeb(int cfeb, int value){comp_timing_cfeb_[cfeb]=value;}
  inline void SetCompThresholdsCfeb(int cfeb, float value){comp_thresh_cfeb_[cfeb]=value;}
  inline void SetCompClockPhaseCfeb(int cfeb, int value){comp_clk_phase_cfeb_[cfeb]=value;}
  inline void SetADCSampleClockPhaseCfeb(int cfeb, int value){adcsamp_clk_phase_cfeb_[cfeb]=value;}
  inline void SetNSamplesCfeb(int cfeb, int value){nsample_cfeb_[cfeb]=value;}
  inline void SetPreBlockEndCfeb(int cfeb, int value){pre_block_end_cfeb_[cfeb]=value;}
  inline void SetL1aExtraCfeb(int cfeb, int value){L1A_extra_cfeb_[cfeb]=value;}
  //
  inline int GetCompModeCfeb(int cfeb){return comp_mode_cfeb_[cfeb];}
  inline int GetCompTimingCfeb(int cfeb){return comp_timing_cfeb_[cfeb];}
  inline float GetCompThresholdsCfeb(int cfeb){return comp_thresh_cfeb_[cfeb];}
  inline int GetCompClockPhaseCfeb(int cfeb){return comp_clk_phase_cfeb_[cfeb];}
  inline int GetADCSampleClockPhaseCfeb(int cfeb){return adcsamp_clk_phase_cfeb_[cfeb];}
  inline int GetNSamplesCfeb(int cfeb){return nsample_cfeb_[cfeb];}
  //
  void LctL1aDelay(int);
  void LctL1aDelay(int,unsigned);
  void toggle_pedestal();
  void pulse(int Num_pulse,unsigned int pulse_delay);
  void inject(int Num_pulse,unsigned int pulse_delay);
  void halfset(int icrd,int ipln,int ihalf,int chan[][6][16]);
  void halfset(int icrd,int ipln,int ihalf);
  void trigsetx(int *hp,int CFEBInputs=0x1f);
  void chan2shift(int chan[][6][16]);
  void buck_shift_ext_bc(int nstrip);
  void buck_shift_comp_bc(int nstrip);

  // RPW stuff from external  really should be CFEB and board class
  // Buckeye shift variables
  // RPW utilities
  // rtrig
  void set_rndmtrg_rate(int);
  void toggle_rndmtrg_start();
  void burst_rndmtrg();


  // shifts all buckeyes
  void shift_all(int mode); 
  
  /// sends commands by name
  void executeCommand(std::string command);

  /// Get DAV time
  void readtimingCounter();
  void readtimingScope();
  char *GetCounters();
  unsigned GetCounter(int );

  /// Set cable delay
  void setcbldly(int );
  
  /// 
  void toggle_caltrg();
  void set_ext_chanx(int chan, int feb=-1);
  void setpulsedelay(int tinj);
  void devdoReset();

  // cable delay routines
  void cbldly_init();
  void cbldly_trig(); 
  void cbldly_phaseA();
  void cbldly_phaseB();
  void cbldly_loadfinedelay();
  void cbldly_programSFM();
  void cbldly_wrtprotectSFM();
  void cbldly_loadmbidSFM();
  void cbldly_loadcfebdlySFM();
  void cbldly_refreshcfebdly();
  //
  void sfm_test_load(char *sndpat);
  void sfm_test_read(char *rcvpat);
  //
  bool SelfTest() ;
  void init() ;
  void configure() ;
  bool checkDAQMBXMLValues();
  inline int GetNumberOfConfigurationReads() { return number_of_configuration_reads_; }
  inline bool GetDAQMBSmokingGunIsOK() { return dmb_smoking_gun_status_; }
  //
  /// Check the configuration of the CFEBs vs. the database... (default not to print out the errors)
  void CheckCFEBsConfiguration(bool print_errors=false);
  //
  /// Get the status of a specific CFEB after having checked it...
  inline bool GetCFEBConfigIsOK(int CFEBIndexCountingFromOne) { return cfeb_config_status_[CFEBIndexCountingFromOne - 1]; }
  /// Get the status of the parameters specific to a CFEB having lost its firmware (after having checked it)...
  inline bool GetSmokingGunIsOK(int CFEBIndexCountingFromOne) { return smoking_gun_status_[CFEBIndexCountingFromOne - 1]; }
  //
  int DCSreadAll(char *data);  
  bool checkvme_fail();

  int cfeb_testjtag_shift(int icfeb,char *out);
	void vtx_cmpfiles(const std::string fileDir, int cbits[]);
  void set_chans_mode(int schan,int mode);
  void set_chans_by4(int schan,int mode);
  void small_configure();
  void testlink(enum DEVTYPE devnum);
  void varytmbdavdelay(int delay);  
  //
  inline void SetHardwareVersion(int version) {hardware_version_ = version;}
  inline int GetHardwareVersion() {return hardware_version_;}

  int ReadRegister(unsigned  address);
  void WriteRegister(unsigned address, int value);
  //
  //
public:
  // unpacks rcvbuf from FPGA operations
  unsigned int unpack_ibrd() const;
  unsigned int unpack_ival() const;


  typedef std::vector<CFEB>::iterator CFEBItr;
  std::vector<CFEB> cfebs_;
  //
  inline int GetL1aLctCounter() { 
    return l1a_lct_counter_ ;
  } 
  inline int GetCfebDavCounter() { 
    return cfeb_dav_counter_ ;
  } 
  inline int GetTmbDavCounter() { 
    return tmb_dav_counter_ ;
  } 
  inline int GetAlctDavCounter() { 
    return alct_dav_counter_ ;
  } 
  //
  inline int GetL1aLctScope() { 
    return l1a_lct_scope_ ;
  } 
  inline int GetCfebDavScope() { 
    return cfeb_dav_scope_ ;
  } 
  inline int GetTmbDavScope() { 
    return tmb_dav_scope_ ;
  } 
  inline int GetAlctDavScope() { 
    return alct_dav_scope_ ;
  } 
  inline int GetActiveDavScope() { 
    return active_dav_scope_ ;
  } 
  //
  inline int GetTestStatus(int test) {
    return TestStatus_[test];
  }
  //
  inline void SetTestStatus(int test, int result) {
    TestStatus_[test] = result;
  }
  //

  int shift_array[7][6][16];

  inline void SetShiftArray(int cfeb, int chip, int chan, int value){
    shift_array_[cfeb][chip][chan] = value;
  }
  //
  inline void SetKillChip(int cfeb, int chip, int value) { kill_chip_[cfeb][chip] = value; }
  inline int  GetKillChip(int cfeb, int chip) { return kill_chip_[cfeb][chip]; }
  //
  inline void SetPowerMask(int value) { power_mask_ = value; }
  inline int GetPowerMask() { return power_mask_; }

  inline int GetFirmwareYear(){ return fwyear_; }
  inline int GetFirmwareMonth(){ return fwmonth_; }
  inline int GetFirmwareDay(){ return fwday_; }
  inline int GetFirmwareVersion(){ return fwvers_; }
  inline int GetFirmwareRevision(){ return fwrv_; }
  inline int GetKillFlatClk(){return killflatclk_;}
  void load_feb_clk_delay();           

  // get statistics on bad bits from the check_eprom_readback
  inline unsigned int GetNumberOfBadReadbackBits(void) const { return NBBTOT; } 
  inline int GetWordWithBadReadbackBit(unsigned int badbit) const { if( badbit < NBB ) return NBBwrd[badbit]; else return -1; } 
  inline int GetBadReadbackBitPosition(unsigned int badbit) const { if( badbit < NBB ) return NBBbit[badbit]; else return -1; } 
  inline int GetBadReadbackBitType    (unsigned int badbit) const { if( badbit < NBB ) return NBBtyp[badbit]; else return -1; } 

  // code used by STEP
  void trighalfx(int ihalf);
  
  // code for DCFEB
  void write_cfeb_selector(int cfeb_mask);
  int read_cfeb_selector();
  void cfeb_do(int ncmd,void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd); 
  void dcfeb_core(int jfunc, int nbit, void *inbuf, char *outbuf, int option);
  void dcfeb_fpga_call(int inst, unsigned data, char *outbuf);
  std::vector<float> dcfeb_fpga_monitor(CFEB & cfeb);
  void dcfeb_hub(CFEB & cfeb, int jfunc, int nbit, void *inbuf, char *outbuf, int option);
  void dcfeb_sys_reset(CFEB & cfeb);
  unsigned dcfeb_read_status(CFEB & cfeb);
  unsigned dcfeb_write_status(CFEB & cfeb, unsigned status);
  void FADC_SetMask(CFEB & cfeb, short int mask);
  void FADC_Initialize(CFEB & cfeb);
  void FADC_ShiftData(CFEB & cfeb, unsigned bits);
  void Pipeline_Restart(CFEB & cfeb);
  void dcfeb_set_PipelineDepth(CFEB & cfeb, short int depth);
  void set_and_initalize_pipelines_and_fine_delays();
  void Set_NSAMPLE(CFEB & cfeb, int nsample);
  unsigned short dcfeb_fine_delay(CFEB & cfeb, unsigned short delay);
  void dcfeb_set_comp_thresh_bc(float thresh);
  void dcfeb_buck_shift_ext_bc(int nstrip);
  void dcfeb_buck_shift_comp_bc(int nstrip);
  void BuckeyeShift(int chip_mask, char shft_bits[6][6], char *shft_out=NULL);
  void dcfeb_Set_ReadAnyL1a(CFEB & cfeb);
  void dcfeb_Clear_ReadAnyL1a(CFEB & cfeb);

  void dcfeb_loadparam(int paramblock,int nval,unsigned short int  *val);
  void dcfeb_readparam(int paramblock,int nval,unsigned short int  *val);         
  void dcfeb_readfirmware_mcs(CFEB & cfeb, const char *filename);
  void dcfeb_program_virtex6(CFEB & cfeb, const char *mcsfile);
  void dcfeb_program_eprom(CFEB & cfeb, const char *mcsfile, int broadcast=0);
  void dcfeb_configure(CFEB & cfeb);
  void dcfeb_test_dummy(CFEB & cfeb, int test);
  unsigned virtex6_readreg(int reg);
  void virtex6_writereg(int reg, unsigned value);
    
  unsigned dcfeb_readreg_virtex6(CFEB & cfeb,int test);
  void dcfeb_readreg_statusvirtex6(CFEB& cfeb); 
  void dcfeb_readreg_cor0virtex6(CFEB& cfeb);
  void dcfeb_readreg_cor1virtex6(CFEB& cfeb);
  void dcfeb_readreg_idcodevirtex6(CFEB& cfeb);
  void dcfeb_readreg_farvirtex6(CFEB& cfeb);
  void dcfeb_readreg_ctl0virtex6(CFEB& cfeb);
  void dcfeb_readreg_crcvirtex6(CFEB& cfeb);
  void dcfeb_readreg_wbstarvirtex6(CFEB& cfeb);

  void dcfeb_fpga_shutdown();
  int dcfeb_dna(CFEB & cfeb, void *dna);
  std::vector<float> dcfeb_adc(CFEB & cfeb);
  void dcfeb_adc_finedelay(CFEB & cfeb, unsigned short finedelay);
  unsigned dcfeb_startup_status(CFEB & cfeb);
  unsigned dcfeb_qpll_lost_count(CFEB & cfeb);  

  int lvmb_power_state();
  // code for ODMB
  void daqmb_do(int ncmd,void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd, int dev);   
  void dlog_do(int ncmd, void *cmd,int nbuf, void *inbuf,char *outbuf,int irdsnd);
  void odmb_fpga_call(int inst, unsigned data, char *outbuf);
  int DCSread2(char *data);
  int read_cfeb_done();
  int read_qpll_state();
  int read_odmb_id();
  void odmb_save_config();
  void odmb_retrieve_config();

  inline void set_all_chan_norm(int chan[16]) { for(int i=0;i<16;i++)chan[i]=NORM_RUN; }
  inline void set_chan_kill(int ichan,int chan[16]) { chan[ichan]=KILL_CHAN; }
  void chan2shift(int chan[16],unsigned int shft_bits[3]);
  void set_dcfeb_parambuffer(CFEB &cfeb, unsigned short int bufload[34]);
  void autoload_select_readback_wrd(CFEB &cfeb, int ival);
  void autoload_readback_wrd(CFEB &cfeb, char wrd[2]);
  
  // various delays in ODMB
  inline void odmb_set_LCT_L1A_delay(int delay) { WriteRegister(LCT_L1A_DLY, delay&0x3F); }  // 6 bits
  inline void odmb_set_TMB_delay(int delay) { WriteRegister(TMB_DLY, delay&0x3F); }  // 6 bits
  inline void odmb_set_Push_delay(int delay) { WriteRegister(PUSH_DLY, delay&0x3F); }  // 6 bits
  inline void odmb_set_ALCT_delay(int delay) { WriteRegister(ALCT_DLY, delay&0x3F); }  // 6 bits
  inline void odmb_set_Inj_delay(int delay) { WriteRegister(INJ_DLY, delay&0x1F); }  // 5 bits
  inline void odmb_set_Ext_delay(int delay) { WriteRegister(EXT_DLY, delay&0x1F); }  // 5 bits
  inline void odmb_set_Cal_delay(int delay) { WriteRegister(CAL_DLY, delay&0xF); }   // 4 bits
  inline int odmb_read_LCT_L1A_delay() { return ReadRegister(LCT_L1A_DLY) & 0x3F; }  // 6 bits
  inline int odmb_read_TMB_delay() { return ReadRegister(TMB_DLY) & 0x3F; }  // 6 bits
  inline int odmb_read_Push_delay() { return ReadRegister(PUSH_DLY) & 0x3F; }  // 6 bits
  inline int odmb_read_ALCT_delay() { return ReadRegister(ALCT_DLY) & 0x3F; }  // 6 bits
  inline int odmb_read_Inj_delay() { return ReadRegister(INJ_DLY) & 0x1F; }  // 5 bits
  inline int odmb_read_Ext_delay() { return ReadRegister(EXT_DLY) & 0x1F; }  // 5 bits
  inline int odmb_read_Cal_delay() { return ReadRegister(CAL_DLY) & 0xF; }   // 4 bits

  // kill input from boards (CFEBs, TMB, ALCT) mask; multiple boards can be killed
  inline void odmb_set_kill_mask(int kill) { WriteRegister(ODMB_KILL, kill); }
  inline int odmb_read_kill_mask() { return ReadRegister(ODMB_KILL); } 

  inline int odmb_firmware_version() { return ReadRegister(read_FW_VERSION); }  

  // this one uses discrete logic interface
  std::vector<float> odmb_fpga_sysmon();
  // this one uses ODMB Device-7
  std::vector<float> odmb_fpga_adc();


  void odmb_loadparam(int paramblock,int nval,unsigned short int  *val);
  void odmb_readparam(int paramblock,int nval,unsigned short int  *val);         

  void odmb_readfirmware_mcs(const char *filename);
  void odmb_program_eprom(const char *mcsfile);
  void odmb_program_virtex6(const char *mcsfile);
  bool odmb_program_eprom_poll(const char *mcsfile);
  

 private:

  // DCFEB BPI-->EPROM access rountines
  void dcfeb_XPROM_do(unsigned short command);
  unsigned short dcfeb_bpi_status();
  unsigned dcfeb_bpi_readtimer();
  void dcfeb_bpi_reset();
  void dcfeb_bpi_disable();
  void dcfeb_bpi_enable();

  inline void dcfebprom_noop() { dcfeb_XPROM_do(XPROM_NoOp); }
  inline void dcfebprom_lock() { dcfeb_XPROM_do(XPROM_Block_Lock); }
  inline void dcfebprom_unlock() { dcfeb_XPROM_do(XPROM_Block_UnLock); }
  inline void dcfebprom_timerstart() { dcfeb_XPROM_do(XPROM_Timer_Start); } 
  inline void dcfebprom_timerstop() { dcfeb_XPROM_do(XPROM_Timer_Stop); } 
  inline void dcfebprom_timerreset() { dcfeb_XPROM_do(XPROM_Timer_Reset); } 
  inline void dcfebprom_clearstatus() { dcfeb_XPROM_do(XPROM_Clear_Status); }
  void dcfebprom_multi(int cnt, unsigned short *manbuf);
  void dcfebprom_unlockerase(); 
  void dcfebprom_loadaddress(unsigned short uaddress, unsigned short laddress);
  void dcfebprom_bufferprogram(unsigned nwords, unsigned short *prm_dat);
  void dcfebprom_read(unsigned nwords, unsigned short *pdata);

  // ODMB BPI-->EPROM access rountines
  void odmb_XPROM_do(unsigned short command);
  unsigned short odmb_bpi_status();
  unsigned odmb_bpi_readtimer();
  void odmb_bpi_reset();
  void odmb_bpi_disable();
  void odmb_bpi_enable();

  inline void odmbeprom_noop() { odmb_XPROM_do(XPROM_NoOp); }
  inline void odmbeprom_lock() { odmb_XPROM_do(XPROM_Block_Lock); }
  inline void odmbeprom_unlock() { odmb_XPROM_do(XPROM_Block_UnLock); }
  inline void odmbeprom_timerstart() { odmb_XPROM_do(XPROM_Timer_Start); } 
  inline void odmbeprom_timerstop() { odmb_XPROM_do(XPROM_Timer_Stop); } 
  inline void odmbeprom_timerreset() { odmb_XPROM_do(XPROM_Timer_Reset); } 
  inline void odmbeprom_clearstatus() { odmb_XPROM_do(XPROM_Clear_Status); }
  void odmbeprom_multi(int cnt, unsigned short *manbuf);
  void odmbeprom_unlockerase(); 
  void odmbeprom_loadaddress(unsigned short uaddress, unsigned short laddress);
  void odmbeprom_bufferprogram(unsigned nwords, unsigned short *prm_dat);
  void odmbeprom_read(unsigned nwords, unsigned short *pdata);
  bool odmbeprom_cmd_fifo_empty(unsigned int poll_interval = 100 /*us*/);
  bool odmbeprom_pec_ready(unsigned int poll_interval = 500 /*us*/);
            
  //
  int shift_array_[7][6][16];
  static const int nchips[7];
  static const int chip_use[7][6];
  static const int layers[6];
  char febstat_[7][4];
  //
  int kill_chip_[7][6];
  //
  int number_of_configuration_reads_; 
  int fwyear_, fwmonth_, fwday_, fwvers_, fwrv_;

  Chamber * csc_;
  /// CFEB Data Available delay adjustment (25ns per step); 
  int feb_dav_delay_;
  /// TMB Data Available delay adjustment (25ns per step); 
  int tmb_dav_delay_; 
  /// Accept window adjustment (25ns per step); 
  int push_dav_delay_;
  /// L1ACC latency relative to CLCT (25ns per step).   
  int l1acc_dav_delay_;

  /// Calibration LCT delay setting (25ns per step);
  int calibration_LCT_delay_;
  /// Calibration L1ACC delay setting (25ns per step); 
  int calibration_l1acc_delay_;
  /// Buckeye PULSE delay setting (6.25ns per step);  
  int pulse_delay_;
  /// Buckeye INJECT delay setting (6.25ns per step).
  int inject_delay_;
  /// in volts
  float pul_dac_set_; 
  /// in volts
  float inj_dac_set_; 
  /// in volts, comparator threshold
  float set_comp_thresh_;
  /// Comparator timing setting
  int comp_timing_;
  /// Comparator working mode setting
  int comp_mode_;
  /// Will adjust the number of time samples before the pulse, 
  /// used as pedestals.
  int pre_block_end_;
  int cable_delay_;
  int crate_id_;
  int toogle_bxn_;
  int ALCT_dav_delay_;
  //
  int CableDelay_, CrateID_, CfebClkDelay_, XLatency_, XFineLatency_,KillInput_,killflatclk_;
  //
  int cfeb_clk_delay_;
  int xlatency_, xfinelatency_,killinput_;
  int comp_mode_cfeb_[7];
  int comp_timing_cfeb_[7];
  int pre_block_end_cfeb_[7];
  int L1A_extra_cfeb_[7];
  float comp_thresh_cfeb_[7];
  int comp_clk_phase_cfeb_[7];
  int adcsamp_clk_phase_cfeb_[7];
  int nsample_cfeb_[7];
  //
  int shift_out[7][36];
  int l1a_lct_counter_, cfeb_dav_counter_, tmb_dav_counter_, alct_dav_counter_ ;
  int l1a_lct_scope_, cfeb_dav_scope_, tmb_dav_scope_, alct_dav_scope_, active_dav_scope_ ;
  int  TestStatus_[20];
  unsigned char FinalCounter[10];
  //
  unsigned long int expected_control_firmware_tag_;
  int       expected_vme_firmware_tag_;
  int       expected_firmware_revision_;
  unsigned long int expected_cfeb_firmware_tag_[7];
  //
  bool cfeb_config_status_[7];
  bool smoking_gun_status_[7];
  bool dmb_smoking_gun_status_;
  //
  int failed_checkvme_;
  int power_mask_;
  // statistics from check_eprom_readback on bad bits: typ=0 1->0 typ=1 0->1
  unsigned int NBB,NBBwrd[20],NBBbit[20],NBBtyp[20],NBBTOT;

  int hardware_version_;
  int kill_input_mask_;
  int lvdb_mapping_;
  
  // VME registers defined in ODMB for direct access
  // Liu May 29, 2013. ODMB firmware version 0.0
  //
  static const unsigned ODMB_CTRL = 0x3000;
  static const unsigned DCFEB_CTRL = 0x3010;
  static const unsigned DCFEB_DONE = 0x3120;
  static const unsigned ODMB_QPLL = 0x3124;
  
  static const unsigned LCT_L1A_DLY = 0x4000;
  static const unsigned TMB_DLY = 0x4004;
  static const unsigned PUSH_DLY = 0x4008;
  static const unsigned ALCT_DLY = 0x400C;
  static const unsigned INJ_DLY = 0x4010;
  static const unsigned EXT_DLY = 0x4014;
  static const unsigned CAL_DLY = 0x4018;
  static const unsigned ODMB_KILL = 0x401C;
  static const unsigned ODMB_CRATEID = 0x4020;

  static const unsigned read_ODMB_ID = 0x4100;
  static const unsigned read_FW_VERSION = 0x4200;
  static const unsigned read_FW_BUILD = 0x4300;

  static const unsigned ODMB_Save_Config = 0x6000;
  static const unsigned ODMB_Retrieve_Config = 0x6004;
  static const unsigned BPI_Reset = 0x6020;
  static const unsigned BPI_Disable = 0x6024;
  static const unsigned BPI_Enable = 0x6028;
  static const unsigned BPI_Write = 0x602C;
  static const unsigned BPI_Read = 0x6030;
  static const unsigned BPI_Read_n = 0x6034;
  static const unsigned BPI_Status = 0x6038;
  static const unsigned BPI_Timer_l = 0x603C;
  static const unsigned BPI_Timer_h = 0x6040;
  static const unsigned FPGA_ADC_BASE = 0x7000;

  // The following registers are for CFEB & LVMB access, also valid on DMB
  static const unsigned WRITE_CFEB_SELECTOR=0x1020;
  static const unsigned READ_CFEB_SELECTOR=0x1024;
  static const unsigned reset_CFEB_JTAG = 0x1018;

  static const unsigned ADC_CTRL_BYTE = 0x8000;
  static const unsigned ADC_READ = 0x8004;
  static const unsigned set_ADC_SELECT = 0x8020;
  static const unsigned read_ADC_SELECT = 0x8024;
  static const unsigned set_POWER_MASK = 0x8010;
  static const unsigned read_POWER_MASK = 0x8014;
  
}; 

  } // namespace emu::pc
  } // namespace emu
#endif

