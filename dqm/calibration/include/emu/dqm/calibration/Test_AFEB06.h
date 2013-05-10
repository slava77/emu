#ifndef _Test_AFEB06_h_
#define _Test_AFEB06_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

#define NMAXSCAN            2
#define MAX_CALIB_POINTS   256

// Injected pulse in fC is A0+A1*DAC_calibration_pulse
#define INJECT_PULSE_A0  4.75 
#define INJECT_PULSE_A1  1.15 

// Calibration pulse in mV is A0+A1*DAC_calibration_pulse
#define CALIBRATION_PULSE_A0  17.77 // adjusted with scope measurements, was 18 previously (Madorsky 3/6/07)
#define CALIBRATION_PULSE_A1  4.44  // adjusted with scope measurements, was 4.6 previously (Madorsky 3/6/07)
#define C_DEFAULT  0.25

typedef struct ddu_stats_afeb_thresh
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;
  int pass;
  int thresh; 

} ddu_stats_afeb_thresh;

typedef struct ThresholdScanData
{
  int Nbins;
  int Nlayers;
  int content[NMAXSCAN][NLAYERS][MAX_WIREGROUPS][MAX_CALIB_POINTS];
} ThresholdScanData;

typedef std::map<std::string, ThresholdScanData>    cscThresholdScanData;


class Test_AFEB06: public Test_Generic
{
public:
  Test_AFEB06(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void setTestParams(); // set Test parameters from Config section of parsed xml and test_params map
  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  bool loadAFEBCalibParams(std::string cscID);
  int calc_thresh(int npoints, int* content, float* par, float* chisq);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  
  int num_tpamps;
  int num_thresh;
  int first_thresh;
  int thresh_step;
  int tpamp_first;
  int tpamp_step;
  int ev_per_thresh;
  int pass;

  std::map<int, ddu_stats_afeb_thresh> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  cscThresholdScanData tscan_data;
  cscAFEBCalibParams afeb_cal_params;


};

#endif
