#ifndef _Test_GasGain_h_
#define _Test_GasGain_h_


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


class Test_GasGain: public Test_Generic
{
public:
  Test_GasGain(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
//	void finish();

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);

  TH1F* hist_truncated(TH1F* hist,float ratio);


  static const unsigned int Nbins_wires;
  static const unsigned int Nbins_strips;
  static const unsigned int NStrips_per_CFEB;
  static const unsigned int NLayer;
  static const unsigned int nsegments;
  std::map<std::string, uint32_t> l1a_cntrs;
  //std::map<std::string, test_step> test_steps;
  std::map<int, int> dduL1A;
  std::vector<TH1F*>hist_cluster_sum;
  std::vector<TH1F*>hist_cluster_sum_fine;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  std::map<int, ddu_stats> DDUstats;
  //std::map<int, std::map<std::string, test_step> > htree;

};

#endif
