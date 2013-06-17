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

#define HIST_1PLANE_LOW_LIMIT0        0.
#define HIST_2PLANE_LOW_LIMIT0        0.
#define HIST_3PLANE_LOW_LIMIT0        0.
#define HIST_4PLANE_LOW_LIMIT0        0.
#define HIST_5PLANE_LOW_LIMIT0        0.
#define HIST_6PLANE_LOW_LIMIT0        0.

#define HIST_1PLANE_HIGH_LIMIT0     200.
#define HIST_2PLANE_HIGH_LIMIT0      27.
#define HIST_3PLANE_HIGH_LIMIT0      27.
#define HIST_4PLANE_HIGH_LIMIT0      20.
#define HIST_5PLANE_HIGH_LIMIT0      13.5
#define HIST_6PLANE_HIGH_LIMIT0      10.5

#define HIST_1PLANE_LOW_LIMIT1        0.
#define HIST_2PLANE_LOW_LIMIT1        0.
#define HIST_3PLANE_LOW_LIMIT1        0.
#define HIST_4PLANE_LOW_LIMIT1        0.
#define HIST_5PLANE_LOW_LIMIT1        0.
#define HIST_6PLANE_LOW_LIMIT1        0.

#define HIST_1PLANE_HIGH_LIMIT1   ((source_type) ? 20000. : 2000.)
#define HIST_2PLANE_HIGH_LIMIT1   ((source_type) ? 200.   : 20.)
#define HIST_3PLANE_HIGH_LIMIT1   ((source_type) ? 20.    : 10.)
#define HIST_4PLANE_HIGH_LIMIT1   ((source_type) ? 16.    : 8.)
#define HIST_5PLANE_HIGH_LIMIT1   10.
#define HIST_6PLANE_HIGH_LIMIT1   ((source_type) ? 8.     : 5.)

#define MAX_SCALER              1000000.

typedef struct ddu_stats_afeb
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;

} ddu_stats_afeb;


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
  bool loadThresholdParams(std::string dfile);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  

  std::map<int, ddu_stats_afeb> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;

  unsigned short alct_valid_patt[2];     /* ALCT valid pattern flag          */
  unsigned short alct_patt_quality[2];   /* ALCT pattern quality (0-3)       */
  unsigned short alct_accel_muon[2];     /* ALCT accelerator muon            */
  unsigned short alct_wire_group[2];     /* ALCT Wire-Gang ID (0-111)        */
  unsigned short alct_full_bxn;          /* ALCT Full Bunch Crossing Number  */
  	 
  int first, plane_threshold, pattern_threshold;
  float all_time[6];
  int threshold_limit[6];
  int src_status; // Status of radioactive source for this run, 
  int nwires;
  int source_type;

};

#endif
