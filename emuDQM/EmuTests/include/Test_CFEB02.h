#ifndef _Test_CFEB02_h_
#define _Test_CFEB02_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>
#include "CFEBCanvas.h"
#include "EmuBaseTest.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

typedef std::map<std::string, CFEBData> TestData;
typedef std::map<std::string, TestData> cscTestData;
typedef std::map<std::string, TH1*> MonHistos;
typedef std::map<std::string, MonHistos> cscMonHistos;
typedef std::map<std::string, CFEBCanvas*> TestCanvases;
typedef std::map<std::string, TestCanvases> cscTestCanvases;
typedef std::map<std::string, std::string> bookParams;
typedef std::map<std::string, bookParams> testParamsCfg;

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
	~Test_CFEB02();
	void init();
	void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
	void finish();
	uint32_t getTotalEvents() { return nTotalEvents;}
        void setNumExpectedEvents(uint32_t n) { nExpectedEvents=n;}
	uint32_t getNumExpectedEvents() const { return nExpectedEvents;}

	void setCSCMapFile(std::string filename);
	void setOutDir(std::string outdir) {outDir=outdir;} 
	void setRootFile(std::string rootfn) {rootFile=rootfn;}
	void setConfigFile(std::string cfgfile) {configFile=cfgfile; loadTestCfg();}
	void setMasksFile(std::string mfile) {masksFile = mfile;loadMasks();}
	std::string getCSCFromMap(int crate, int slot);	

  private:

	std::string getCSCTypeLabel(int endcap, int station, int ring );
	void saveCSCList();
	void doBinCheck();

	TestData initCSC(std::string cscID);
	void bookMonHistosCSC(std::string cscID);
	int loadTestCfg();
	int loadMasks();
	void bookTestCanvases(std::string cscID);
	void analyzeCSC(const CSCEventData& data);
	void finishCSC(std::string cscID);
	//std::map<std::string, CFEBData>	testsData;
	cscTestData tdata;
	cscMonHistos mhistos;
	cscCFEBSCAData sdata;
  	cscTestCanvases tcnvs;
	testParamsCfg xmlCfg;
	TestData tmasks;
	

	uint32_t nTotalEvents;
	uint32_t nBadEvents;
	std::map<std::string, uint32_t> nCSCEvents;
	std::string dataFile;
	std::string outDir;
        uint32_t nExpectedEvents;

	CSCDCCExaminer bin_checker;
	CSCReadoutMappingFromFile cscMapping;
	std::string cscMapFile;
	std::string testID;
	std::string rootFile;
	std::string configFile;
	std::string masksFile;
	uint32_t imgW, imgH;

};

#endif
