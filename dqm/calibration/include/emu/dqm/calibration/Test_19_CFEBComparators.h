#ifndef _Test_19_CFEBComparators_h_
#define _Test_19_CFEBComparators_h_

#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"

#include "DataFormats/CSCDigi/interface/CSCComparatorDigi.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

#define NMAXSCANC             5
#define NMAXTHRESHC          40 
#define NMAXBUCKETC          31
#define NCFEB_CHIP            6
#define NCFEB                 5
#define MAX_CALIB_POINTSC   256

#define NLAYER 6
#define NSTRIP 112

// Injected pulse in fC is C0+C1*DAC_calibration_pulse
#define INJECT_PULSE_C0  0. 
#define INJECT_PULSE_C1  0.655 // for the new DAC 

typedef struct ddu_stats_cfeb_thresh
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;
  float dac;
  int amp;
  int thresh; 
  int strip;

} ddu_stats_cfeb_thresh;


typedef struct ThresholdScanDataC
{
  int Nbins;
  int Nlayers;
  int content[NMAXSCANC][NLAYER][NSTRIP][MAX_CALIB_POINTSC];
} ThresholdScanDataC;

typedef std::map<std::string, ThresholdScanDataC> cscThresholdScanDataC;


class Test_19_CFEBComparators: public Test_Generic
{
  
public:
  Test_19_CFEBComparators(std::string datafile);
  ~Test_19_CFEBComparators();
  
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  void setTestParams();
  int calc_thresh(int npoints, int* content, float* par, float* chisq);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  
  int nscan;
  
  float  calibration_pulse[NMAXSCANC];
  float  calibration_thresh[NMAXTHRESHC];

  std::map<int, ddu_stats_cfeb_thresh> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  cscThresholdScanDataC tscan_data;
  
  int dmb_tpamps_per_strip;
  int thresh_step;
  int thresh_first;
  int threshs_per_tpamp;
  int events_per_thresh;
  int strip_step;
  int strip_first;
  int dmb_tpamp_first;
  int dmb_tpamp_step;
  int scale_turnoff;
  int range_turnoff;
  int strips_per_run;
  int n_ME11_DCFEBs;
  int nBins;
		  
  
private:
  
};

#endif
