#ifndef _Test_CFEB03_h_
#define _Test_CFEB03_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"
#include "emu/dqm/calibration/CSCxTalk.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>


class Test_CFEB03: public Test_Generic
{
public:
  Test_CFEB03(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
//	void finish();

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);

  cscCFEBSCAData sdata;
  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<std::string, test_step> test_steps;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  std::map<int, ddu_stats> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  cscXtalkData xdata;

  Conv conv;
  double It[64];

};

#endif
