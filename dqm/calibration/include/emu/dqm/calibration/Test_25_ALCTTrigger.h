#ifndef _Test_25_ALCTTrigger_h_
#define _Test_25_ALCTTrigger_h_


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

typedef struct ddu_stats_afeb
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;

} ddu_stats_afeb;

struct LimitData { 
    int events[6];
    int time[6];
};

typedef std::map<std::string, LimitData> EventLimits;

class Test_25_ALCTTrigger: public Test_Generic
{
public:
  Test_25_ALCTTrigger(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  bool loadThresholdParams(std::string dfile, std::string cscID, int crateID, int DMBslot);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  
  std::string dataFileName;
  EventLimits threshold_limit;

  std::map<int, ddu_stats_afeb> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;

  unsigned short alct_valid_patt[2];     /* ALCT valid pattern flag          */
  unsigned short alct_patt_quality[2];   /* ALCT pattern quality (0-3)       */
  unsigned short alct_accel_muon[2];     /* ALCT accelerator muon            */
  unsigned short alct_wire_group[2];     /* ALCT Wire-Gang ID (0-111)        */
  unsigned short alct_full_bxn;          /* ALCT Full Bunch Crossing Number  */
  	 
  int first, plane_threshold, pattern_threshold;
  int nwires;

};

#endif
