//-----------------------------------------------------------------------
// $Id: DAQMB.h,v 2.25 2006/06/23 14:21:33 mey Exp $
// $Log: DAQMB.h,v $
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
#include "VMEModule.h"
#include "JTAG_constants.h"
#include "CFEB.h"
#include "EmuModule.h"
/**
 * more documentation available for this class at
 * http://www.physics.ohio-state.edu/~cms/dmb/prog_man.pdf
 * http://www.physics.ohio-state.edu/~cms/dmb/delayset.html
 */


class DAQMB: public VMEModule, public EmuModule
{
public:
  //friend class DAQMBParser;

  // standard routines
  /// construct with all the standard CFEBs and BuckeyeChips
  DAQMB(int newcrate, int slot) ;
  /// construct with a single CFEB, count from zero
  DAQMB(int newcrate, int slot, int cfeb);
  DAQMB(int newcrate, int slot, const std::vector<CFEB> & cfebs);
  //
  virtual ~DAQMB();
  /// from the BOARDTYPE enum
  unsigned int boardType() const {return DMB_ENUM;}
  void end();

  std::vector<CFEB> cfebs() const {return cfebs_;}

// DAQMB constants&resets

  void trigset2(int, int [] );
  void trgfire();
  void load_strip();
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
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  
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
  unsigned long int febpromuser(const CFEB &);
  unsigned long int febpromid(const CFEB &);
  unsigned long int febfpgauser(const CFEB &);
  unsigned long int febfpgaid(const CFEB &);
  unsigned long int mbpromuser(int prom);
  unsigned long int mbpromid(int prom);
  unsigned long int mbfpgauser();
  unsigned long int mbfpgaid();
// DAQMB fifo write and reads
  void wrtfifo(int fifo,int nsndfifo,char* sndfifo);
  void readfifo(int fifo,int nrcvfifo,char* rcvfifo);
// DAQMB load and read flash memory (electronics experts only)
  void buckflash_init();
  void buckflash_load(char *fshift);
  void buckflash_read(char *rshift);
  void buckflash_pflash(); 
  void buckflash_erase(); 
  void preamp_initx();
// DAQMB program proms (electronics experts only)
  void epromload(DEVTYPE devnum,const char *downfile,int writ,char *cbrdnum);
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
  void SetFebDavDelay(int delay){feb_dav_delay_ = delay;}
  void SetTmbDavDelay(int delay){tmb_dav_delay_ = delay;}
  void SetPushDavDelay(int delay){push_dav_delay_ = delay;}
  void SetL1aDavDelay(int delay){l1acc_dav_delay_ = delay;}
  void SetAlctDavDelay(int delay){ALCT_dav_delay_ = delay;}
  void SetCalibrationLctDelay(int delay){calibration_LCT_delay_ = delay;}
  void SetCalibrationL1aDelay(int delay){calibration_l1acc_delay_ = delay;}
  void SetPulseDelay(int delay){pulse_delay_ = delay;}
  void SetInjectDelay(int delay){inject_delay_ = delay;}
  void SetFebClockDelay(int delay){feb_clock_delay_ = delay;}
  void SetCompMode(int mode){comp_mode_ = mode;}
  void SetCompTiming(int delay){comp_timing_ = delay;}
  void SetPreBlockEnd(int delay){pre_block_end_ = delay;}
  void SetCableDelay(int delay){cable_delay_ = delay;}
  void SetCrateId(int id){crate_id_ = id;}
  //
  void SetPulseDac(float value){pul_dac_set_= value;}
  void SetInjectorDac(float value){inj_dac_set_=value;}
  void SetCompThresh(float value){set_comp_thresh_=value;}
  //
  void toggle_pedestal();
  void pulse(int Num_pulse,unsigned int pulse_delay);
  void inject(int Num_pulse,unsigned int pulse_delay);
  void halfset(int icrd,int ipln,int ihalf,int chan[][6][16]);
  void halfset(int icrd,int ipln,int ihalf);
  void trigsetx(int *hp);
  void chan2shift(int chan[][6][16]);

  // RPW stuff from external  really should be CFEB and board class
  // Buckeye shift variables
  int shift_array[5][6][16];
  static const int nchips[5];
  static const int chip_use[5][6];
  static const int layers[6];

  // RPW utilities
  // rtrig
  void toggle_rndmtrg_start();
  void burst_rndmtrg();


  // shifts all buckeyes
  void shift_all(int mode); 
  
  /// sends commands by name
  void executeCommand(std::string command);

  /// Get DAV time
  void readtimingCounter();
  void readtimingScope();

  /// Set cable delay
  void setcbldly(int );
  
  /// 
  void cfeb_vtx_prom(enum DEVTYPE devnum);
  void febpromuser2(const CFEB &,char *cbrdnum);
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
  //

public:
  // unpacks rcvbuf from FPGA operations
  unsigned long int unpack_ibrd() const;
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

 private:
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

  // apparently not used...
  int feb_clock_delay_;
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
  int shift_out[5][36];
  std::ostream * MyOutput_ ;
  int l1a_lct_counter_, cfeb_dav_counter_, tmb_dav_counter_, alct_dav_counter_ ;
  int l1a_lct_scope_, cfeb_dav_scope_, tmb_dav_scope_, alct_dav_scope_, active_dav_scope_ ;
  int  TestStatus_[20];
}; 

#endif

