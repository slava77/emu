//-----------------------------------------------------------------------
// $Id: ODAQMB.h,v 1.1 2012/07/12 12:59:16 ahart Exp $
// $Log: ODAQMB.h,v $
// Revision 1.1  2012/07/12 12:59:16  ahart
//
// First commit of new files for the DCFEB.
//
// Revision 1.8  2010/08/23 11:24:25  rakness
// add DMB Control and FPGA firmware check
//
// Revision 1.7  2010/03/03 20:33:19  liu
// adjust CFEB calib timing by Stan
//
// Revision 1.6  2009/12/18 09:42:48  rakness
// update firmware reloading routine to emulate expept actions
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
#ifndef ODAQMB_h
#define ODAQMB_h
#include <iostream>
#include <vector>
#include <string>
#include "emu/pc/VMEModule.h"
#include "emu/pc/JTAG_constants.h"
#include "emu/pc/DCFEB.h"
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

class ODAQMB: public VMEModule, public EmuLogger
{
public:

  //friend class ODAQMBParser;

  // standard routines
  /// construct with all the standard CFEBs and BuckeyeChips
  explicit ODAQMB(Crate * ,Chamber *, int ) ;
  /// construct with a single CFEB, count from zero
  //ODAQMB(Crate * , int slot, int cfeb);
  //ODAQMB(Crate * , int slot, const std::vector<CFEB> & cfebs);
  //
  virtual ~ODAQMB();
  /// from the BOARDTYPE enum
  unsigned int boardType() const {return DMB_ENUM;}
  void end();

  std::vector<DCFEB> dcfebs() const {return dcfebs_;}

// ODAQMB constants&resets

  friend std::ostream & operator<<(std::ostream & os, ODAQMB & daqmb);

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
  // ODAQMB trigger primitives
  //
  void ToogleBXN();

// ODAQMB trigger primitives

  void set_comp_mode(int dword);     
  void set_comp_thresh(double dword);
  
  /// ODAQMB voltages
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

// ODAQMB Thermometers
  enum Temps {DMBTEMP, CFEB1TEMP, CFEB2TEMP, CFEB3TEMP, CFEB4TEMP,
              CFEB5TEMP, V18REFTEMP, VIRTEXTEMP};
  float readthermx(int feb);

// ODAQMB LVMB routines
  float lowv_adc(int ichp,int ichn);
  void lowv_onoff(char c);
  unsigned int lowv_rdpwrreg(); 
// ODAQMB prom readbacks
  unsigned long int mbpromuser(int prom);
  unsigned long int mbpromid(int prom);
  unsigned long int mbfpgauser();
  unsigned long int mbfpgaid();
  void vmefpgaid();
// ODAQMB fifo write and reads
  void wrtfifo(int fifo,int nsndfifo,char* sndfifo);
  void readfifo(int fifo,int nrcvfifo,char* rcvfifo);
// ODAQMB load and read flash memory (electronics experts only)
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
// ODAQMB program proms (electronics experts only)
  void epromload_verify(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum); //xcfeb
  void epromload(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);  //xcfeb
  void epromload_broadcast(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum, int ipass); //xcfeb
  void epromloadOld(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);
  void rdbkvirtex(DEVTYPE);
  void rdbkvirtexII();
  void programvirtex6(DEVTYPE,const char *mcsfile);  //xcfeb
// ODAQMB calibrate
  void set_cal_dac(float volt0,float volt1);
  void buck_shift();
  void buck_shift_out();
  int buck_shift_test();
  //
  void set_cal_tim_pulse(int ntim);
  void set_cal_tim_inject(int ntim);
  //
  // firmware version tags:
  inline void SetExpectedControlFirmwareTag(long int tag) { expected_control_firmware_tag_ = (unsigned long int) tag; }
  inline unsigned long int GetExpectedControlFirmwareTag()  { return expected_control_firmware_tag_; }
  bool CheckControlFirmwareVersion();
  //
  inline void SetExpectedVMEFirmwareTag(int tag) { expected_vme_firmware_tag_ = (unsigned long int) tag; }
  inline int  GetExpectedVMEFirmwareTag() { return expected_vme_firmware_tag_; }
  bool CheckVMEFirmwareVersion();
  //
  inline void SetExpectedCFEBFirmwareTag(int cfeb_index, long int tag) { expected_cfeb_firmware_tag_[cfeb_index] = tag; } //xcfeb
  inline unsigned long int GetExpectedCFEBFirmwareTag(int cfeb_index)  { return expected_cfeb_firmware_tag_[cfeb_index]; } //xcfeb
  bool CheckCFEBFirmwareVersion(const DCFEB & cfeb_to_check); //xcfeb
  //
  //
  void SetFebDavDelay(int delay){feb_dav_delay_ = delay;}
  inline int GetFebDavDelay(){return feb_dav_delay_;}
  //
  void SetTmbDavDelay(int delay){tmb_dav_delay_ = delay;}
  inline int GetTmbDavDelay(){return tmb_dav_delay_;}
  //
  void SetPushDavDelay(int delay){push_dav_delay_ = delay;}
  inline int GetPushDavDelay(){return push_dav_delay_;}
  //
  void SetL1aDavDelay(int delay){l1acc_dav_delay_ = delay;}
  inline int GetL1aDavDelay(){return l1acc_dav_delay_;}
  //
  void SetAlctDavDelay(int delay){ALCT_dav_delay_ = delay;}
  inline int GetAlctDavDelay(){return ALCT_dav_delay_;}
  //
  void SetCalibrationLctDelay(int delay){calibration_LCT_delay_ = delay;}
  inline int GetCalibrationLctDelay(){return calibration_LCT_delay_;}
  //
  void SetCalibrationL1aDelay(int delay){calibration_l1acc_delay_ = delay;}
  inline int GetCalibrationL1aDelay(){return calibration_l1acc_delay_;}
  //
  void SetPulseDelay(int delay){pulse_delay_ = delay;}
  inline int GetPulseDelay(){return pulse_delay_;}
  //
  void SetInjectDelay(int delay){inject_delay_ = delay;}
  inline int GetInjectDelay(){return inject_delay_;}
  //
  //void SetFebClockDelay(int delay){feb_clock_delay_ = delay;}
  //
  void SetCompMode(int mode){comp_mode_ = mode;}
  inline int GetCompMode(){return comp_mode_;}
  //
  void SetCompTiming(int delay){comp_timing_ = delay;}
  inline int GetCompTiming(){return comp_timing_;}
  //
  void SetPreBlockEnd(int delay){pre_block_end_ = delay;}
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
  void SetCrateId(int id){crate_id_ = id;}
  //
  void SetCfebClkDelay(int delay){killflatclk_ = (killflatclk_&0x3fe0)+(delay&0x1f);}
  inline int GetCfebClkDelay(){return (killflatclk_&0x1f);}
  //
  void SetxLatency(int latency){killflatclk_= (killflatclk_&0x3f9f)+(( latency<<5)&0x60);}
  inline int GetxLatency(){return ((killflatclk_>>5)&0x03);}
  //
  void SetxFineLatency(int finelatency){killflatclk_ = (killflatclk_&0x3ff)+((finelatency<<10)&0x3c00);}
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
  void SetKillInput(int killinput){killflatclk_ = (killflatclk_&0x3c7f)+((killinput<<7)&0x380);}
  inline int GetKillInput(){return ((killflatclk_>>7)&0x07);}
  //
  void SetPulseDac(float value){pul_dac_set_= value;}
  inline float GetPulseDac(){return pul_dac_set_;}
  //
  void SetInjectorDac(float value){inj_dac_set_=value;}
  inline float GetInjectorDac(){return inj_dac_set_;}
  //
  void SetCompThresh(float value){set_comp_thresh_=value;} //xcfeb
  inline float GetCompThresh(){return set_comp_thresh_;} //xcfeb
  //
  void SetCompModeCfeb(int cfeb, int value){comp_mode_cfeb_[cfeb]=value;}  //xcfeb
  void SetCompTimingCfeb(int cfeb, int value){comp_timing_cfeb_[cfeb]=value;} //xcfeb
  void SetCompThresholdsCfeb(int cfeb, float value){comp_thresh_cfeb_[cfeb]=value;} //xcfeb
  void SetPreBlockEndCfeb(int cfeb, int value){pre_block_end_cfeb_[cfeb]=value;} 
  void SetL1aExtraCfeb(int cfeb, int value){L1A_extra_cfeb_[cfeb]=value;} 
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
  void cfeb_vtx_prom(enum DEVTYPE devnum); 
  void febpromuser2(const DCFEB &,char *cbrdnum);
  void toggle_caltrg();
  void set_ext_chanx(int chan);
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
  bool checkODAQMBXMLValues();
  inline int GetNumberOfConfigurationReads() { return number_of_configuration_reads_; }
  inline bool GetODAQMBSmokingGunIsOK() { return dmb_smoking_gun_status_; }
  //
  ///Check the configuration of the CFEBs vs. the database...
  void CheckCFEBsConfiguration(bool print_errors); 
  /// Check the configuration of the CFEBs vs. the database... (default not to print out the errors)
  void CheckCFEBsConfiguration();
  //
  /// Get the status of a specific CFEB after having checked it...
  inline bool GetCFEBConfigIsOK(int CFEBIndexCountingFromOne) { return cfeb_config_status_[CFEBIndexCountingFromOne - 1]; }
  /// Get the status of the parameters specific to a CFEB having lost its firmware (after having checked it)...
  inline bool GetSmokingGunIsOK(int CFEBIndexCountingFromOne) { return smoking_gun_status_[CFEBIndexCountingFromOne - 1]; }
  //
  int DCSreadAll(char *data);  
  bool checkvme_fail();

  int cfeb_testjtag_shift(int icfeb,char *out);
  int vtx_cmpfiles(char *fname,int *cbits); 
  void set_chans_mode(int schan,int mode);
  void set_chans_by4(int schan,int mode);
  void small_configure(); //xcfeb
  void testlink(enum DEVTYPE devnum);
  void varytmbdavdelay(int delay);  

  //
  //
public:
  // unpacks rcvbuf from FPGA operations
  unsigned long int unpack_ibrd() const;
  unsigned int unpack_ival() const;


  typedef std::vector<DCFEB>::iterator DCFEBItr;
  std::vector<DCFEB> dcfebs_;
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
            
 private:
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
  int NSAMPLE;
  unsigned short int pdata[6][16][100];
 // for DMBTest
#define MAX_MSG_SIZE 1024 
 public:
  // int NSAMPLE;
  inline int getNSample(){return NSAMPLE;}
  inline void setNSample(int nsamp){NSAMPLE=nsamp;}
  inline unsigned short int getpdata(int chip,int chan,int samp){return pdata[chip][chan][samp];}
  void set_comp_thresh_cfeb_(float word); //DMBTest
  void set_xlatency_(int dword); // DMBTest
  bool checkDAQMBXMLValues();

};

  } // namespace emu::pc
  } // namespace emu
#endif

