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

class Test_CFEB03: public Test_Generic 
{
  public:
	Test_CFEB03(std::string datafile);
	void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

  protected:

	void initCSC(std::string cscID);
	void analyzeCSC(const CSCEventData& data);
	void finishCSC(std::string cscID);

	cscCFEBSCAData sdata;

};

#endif
