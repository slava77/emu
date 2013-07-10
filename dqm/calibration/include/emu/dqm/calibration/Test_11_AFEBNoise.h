#ifndef _Test_11_AFEBNoise_h_
#define _Test_11_AFEBNoise_h_

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

class Test_11_AFEBNoise: public Test_Generic
{
public:
  Test_11_AFEBNoise(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  void setTestParams();
  
  int num_wires_hit[NLAYERS];
  int last_wire[NLAYERS];
  
  int duration_ms;
  std::string voltageStr;

};

#endif
