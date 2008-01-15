#ifndef _Test_CFEB02_h_
#define _Test_CFEB02_h_

#include <map>
#include "CFEBTestCanvas.h"
#include "EmuBaseTest.h"

typedef std::map<std::string, CFEBData> TestData;
typedef std::map<std::string, TestData> cscTestData;
typedef std::map<std::string, TH1*> MonHistos;
typedef std::map<std::string, MonHistos> cscMonHistos;

typedef struct sca_sample {
        double value;
        int cnt;
} sca_sample;

typedef struct CFEBSCAData{
        int Nbins;
        int Nlayers;
        sca_sample content[6][80][96];
} CFEBSCAData;

typedef std::map<std::string, CFEBSCAData> cscCFEBSCAData;

class Test_CFEB02 
{
  public:
	Test_CFEB02(std::string datafile);
	void init();
	void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
	void finish();
	uint32_t getTotalEvents() { return nTotalEvents;}
        void setNumExpectedEvents(uint32_t n) { nExpectedEvents=n;}
	uint32_t getNumExpectedEvents() const { return nExpectedEvents;}

	void setCSCMapFile(std::string filename);
	std::string getCSCFromMap(int crate, int slot);	

  private:

	std::string getCSCTypeLabel(int endcap, int station, int ring );

	TestData initCSC(std::string cscID);
	MonHistos bookMonHistosCSC(std::string cscID);
	void analyzeCSC(const CSCEventData& data);
	//std::map<std::string, CFEBData>	testsData;
	cscTestData tdata;
	cscMonHistos mhistos;
	cscCFEBSCAData sdata;
	uint32_t nTotalEvents;
	uint32_t nBadEvents;
	std::map<std::string, uint32_t> nCSCEvents;
	std::string dataFile;
        uint32_t nExpectedEvents;

	CSCDCCExaminer bin_checker;
	CSCReadoutMappingFromFile cscMapping;
	std::string cscMapFile;

};

#endif
