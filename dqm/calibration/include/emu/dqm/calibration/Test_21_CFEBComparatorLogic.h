#ifndef _Test_21_CFEBComparatorLogic_h_
#define _Test_21_CFEBComparatorLogic_h_


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

class Test_21_CFEBComparatorLogic: public Test_Generic
{
  
public:
  Test_21_CFEBComparatorLogic(std::string datafile);
  ~Test_21_CFEBComparatorLogic();
  
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
  
protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  void setTestParams();
  
  
private:
  TestData halfStrips; //expected a pulse and got a pulse
  TestData halfStripsMultiple; //expected one pulse, got multiple
  TestData halfStripsFake;  //expected no pulse, got some

  //to be loaded from xml at some point
  int events_per_hstrip;
  int hstrips_per_run;
  int hstrip_step;
  int hstrip_first;
  int n_ME11_TMB_DCFEBs;
};

#endif
