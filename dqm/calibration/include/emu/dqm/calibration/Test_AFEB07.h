#ifndef _Test_AFEB07_h_
#define _Test_AFEB07_h_


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

#define ACTIVETBINS_LIMIT         2 // Reject After Pulsing
#define SLOPE_LOWER_LIMIT       1.5 
#define SLOPE_UPPER_LIMIT       2.5
#define CHISQ_UPPER_LIMIT      80.0 

float cable_del[8][14] = // 6 is the number of different chamber types, 14 is max AFEB group number 
{ 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},   // Dummy
    {3.0, 3.0, 1.6, 1.6, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0}, // ME1.2
    {4.6, 1.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},       // ME1.3
    {1.4, 1.4, 1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4},       // M2.1
    {1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 0, 0},   // ME3.1
    {10.7, 10.7, 7.7, 7.7, 3.0, 3.0, 0, 0, 0, 0, 0, 0, 0, 0}, // ME234.2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 1.4, 0, 0},  // ME4.1
    {3.2, 1.6, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0, 0, 0}   // ME1.1
};

class Test_AFEB07: public Test_Generic
{
public:
  Test_AFEB07(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void setTestParams(); // set Test parameters from Config section of parsed xml and test_params map
  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);

  int getChamberCableIndex(std::string cscID);
  double getChamberDelayOffset(std::string cscID);
  void  fitit(DelayScanData& dsdata, int layer, int wire , int npoints, double *emm, double* bee, double* chisq);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  std::map<int, ddu_stats> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  int fSTEPsetup; // FAST/STEP setup DAQ asynchronous pulsing setup (25ns)
  cscDelayScanData dscan_data;


};

#endif
