#ifndef _Test_CFEB04_h_
#define _Test_CFEB04_h_


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

// == CFEB SCA cell sample pair (value, count)
typedef struct test_step {
	int active_strip;
        int dac_step;
        int evt_cnt;
	int max_adc;
} test_step;


class Test_CFEB04: public Test_Generic 
{
  public:
	Test_CFEB04(std::string datafile);
	void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

  protected:

	void initCSC(std::string cscID);
	void analyzeCSC(const CSCEventData& data);
	void finishCSC(std::string cscID);

	cscCFEBSCAData sdata;
	std::map<std::string, uint32_t> l1a_cntrs;
	std::map<std::string, test_step> test_steps;
	std::map<int, int> dduL1A;

};

#endif
