//-----------------------------------------------------------------------
// $Id: DAQMB.h,v 2.1 2005/06/06 10:05:50 geurts Exp $
// $Log: DAQMB.h,v $
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
/**
 * more documentation available for this class at
 * http://www.physics.ohio-state.edu/~cms/dmb/prog_man.pdf
 * http://www.physics.ohio-state.edu/~cms/dmb/delayset.html
 */


class DAQMB: public VMEModule
{
public:
  friend class DAQMBParser;

// standard routines
  /// construct with all the standard CFEBs and BuckeyeChips
  DAQMB(int newcrate, int slot);
  /// construct with a single CFEB, count from zero
  DAQMB(int newcrate, int slot, int cfeb);
  DAQMB(int newcrate, int slot, const std::vector<CFEB> & cfebs);
  virtual ~DAQMB();
  /// from the BOARDTYPE enum
  virtual unsigned int boardType() const {return DMB_ENUM;}
  virtual void end();

  void configure();

  std::vector<CFEB> cfebs() const {return cfebs_;}

// DAQMB constants&resets
  void load_strip();
  void setcrateid(int dword);
  void setfebdelay(int dword);
  void setcaldelay(int dword);
  void setdavdelay(int dword);
  void fxpreblkend(int dword);
  void calctrl_fifomrst();
  void calctrl_global();
  void restoreCFEBIdle();
  void restoreMotherboardIdle();

// DAQMB trigger primitives
  void set_comp_mode(int dword);
  void set_comp_thresh(float thresh);

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
  void DAQMB::trigtest();

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
  void preamp_initx();
// DAQMB program proms (electronics experts only)
  void epromload(DEVTYPE devnum,char *downfile,int writ,char *cbrdnum);
  void rdbkvirtex(DEVTYPE);
  void rdbkvirtexII();
// DAQMB calibrate
  void set_cal_dac(float volt0,float volt1);
  void buck_shift();
  int buck_shift_test();

  void set_cal_tim_pulse(int ntim);
  void set_cal_tim_inject(int ntim);


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
  // shifts all buckeyes
  void shift_all(int mode); 
  
  /// sends commands by name
  void executeCommand(std::string command);

#ifdef USEDCS
  /// DCS additions
  void cfeb_vtx_prom(enum DEVTYPE devnum);
  void devdoReset();
#endif

public:
  // unpacks rcvbuf from FPGA operations
  unsigned long int unpack_ibrd() const;
  unsigned int unpack_ival() const;

  /// CFEB Data Available delay adjustment (25ns per step); 
  int feb_dav_delay_;
  /// TMB Data Available delay adjustment (25ns per step); 
  int tmb_dav_delay_; 
  /// Accept window adjustment (25ns per step); 
  int push_dav_delay_;
  /// L1ACC latency relative to CLCT (25ns per step).   
  int l1acc_dav_delay_;
  int ALCT_dav_delay_;

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



  typedef std::vector<CFEB>::iterator CFEBItr;
  std::vector<CFEB> cfebs_;
}; 

#endif

