#ifndef _Test_CFEB03_h_
#define _Test_CFEB03_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>
#include "TestCanvas_1h.h"
#include "TestCanvas_6gr1h.h"
#include "TestCanvas_userHisto.h"
#include "Test_Generic.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

// typedef std::map<std::string, bookParams> testParamsCfg;

#define DAC_STEPS 20
/*
// == CFEB SCA cell sample pair (value, count)
typedef struct test_step {
	int active_strip;
        int dac_step;
        int evt_cnt;
	int max_adc;
} test_step;

typedef struct ddu_stats {
	long first_l1a;
	int last_empty;
	long evt_cntr;
	long l1a_cntr;
        int empty_evt_cntr;
        int csc_evt_cntr;
        int strip;
        int dac;

} ddu_stats;

// == CFEB SCA cell sample pair (value, count)
typedef struct dac_step {
        double s;
	double mv;
	double rms;
	double max;
	double max_rms;
	int max_cnt;
        int cnt;
} dac_step;

// == CFEB SCA data structure
typedef struct GainData{
        int Nbins;
        int Nlayers;
        dac_step content[DAC_STEPS][NLAYERS][MAX_STRIPS];
} GainData;
*/
// == CSC->SCA Data structure
typedef std::map<std::string, GainData> cscGainData;


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

	cscCFEBSCAData sdata;
	std::map<std::string, uint32_t> l1a_cntrs;
	std::map<std::string, test_step> test_steps;
	std::map<int, int> dduL1A;
        int currL1A;
        int startL1A;
	int dduID;
	std::map<int, ddu_stats> DDUstats;
        std::map<int, std::map<std::string, test_step> > htree;
	bool fSwitch;
	cscGainData gdata;

};

#endif
