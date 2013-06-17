#ifndef _Test_16_CFEBConnectivity_h_
#define _Test_16_CFEBConnectivity_h_


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

class Test_16_CFEBConnectivity: public Test_Generic
{
  typedef std::map<int, TestData2D> TimeBinsTestData;
  typedef std::map<std::string, TimeBinsTestData> cscTimeBinsTestData;
  
public:
  Test_16_CFEBConnectivity(std::string datafile);
  ~Test_16_CFEBConnectivity();
  
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  void setTestParams();
  
  int chamberSides;
  int events_per_layer;

  cscTimeBinsTestData adcSum;
  cscTimeBinsTestData adcSum2;
  
private:
  TH1F *min_adc_hist;
  TH1F *max_adc_hist;
  
};

#endif
