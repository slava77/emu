#ifndef _Test_PipelineDepth_h_
#define _Test_PipelineDepth_h_

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

class Test_30_PipelineDepth: public Test_Generic
{
public:
  Test_30_PipelineDepth(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

	int PD_min;
	int PD_max;

	int current_PD;
	int current_l1a;

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  void setTestParams();
	void colNormalize(TH2F* hist);

  cscCFEBSCAData sdata;

};

#endif
