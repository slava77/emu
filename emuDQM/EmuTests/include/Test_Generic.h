#ifndef _Test_Generic_h_
#define _Test_Generic_h_

#include "version.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include <iomanip>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>


/* Normal calls to CMSSW source tree */
#include "EventFilter/CSCRawToDigi/interface/CSCDCCExaminer.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCALCTHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCALCTTrailer.h"
#include "EventFilter/CSCRawToDigi/interface/CSCAnodeData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCFEBData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCFEBTimeSlice.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCLCTData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUTrailer.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDMBHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDMBTrailer.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBHeader.h"
#include "EventFilter/CSCRawToDigi/interface/CSCTMBTrailer.h"
#include "EventFilter/CSCRawToDigi/src/bitset_append.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"

#include "ConsumerCanvas.hh"
typedef ConsumerCanvas MonitoringCanvas;

#include "CSCReadoutMappingFromFile.h"

#include "TestCanvas_6gr1h.h"
#include "TestCanvas_1h.h"
#include "TestCanvas_userHisto.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

typedef std::map<std::string, TestData2D> TestData;
typedef std::map<std::string, TestData> cscTestData;
typedef std::map<std::string, TH1*> MonHistos;
typedef std::map<std::string, MonHistos> cscMonHistos;
// typedef std::map<std::string, TestCanvas_6gr1h*> TestCanvases;
typedef std::map<std::string, TCanvas*> TestCanvases;
typedef std::map<std::string, TestCanvases> cscTestCanvases;
typedef std::map<std::string, std::string> bookParams;
typedef std::map<std::string, bookParams> testParamsCfg;

// == CFEB SCA cell sample pair (value, count)
typedef struct sca_sample {
        double value;
        int cnt;
} sca_sample;

// == CFEB SCA data structure
typedef struct CFEBSCAData{
        int Nbins;
        int Nlayers;
        sca_sample content[6][80][96];
} CFEBSCAData;

// == CSC->SCA Data structure
typedef std::map<std::string, CFEBSCAData> cscCFEBSCAData;


// == Base Class for Emu Tests
class Test_Generic 
{
  public:
	Test_Generic(std::string dfile);
	virtual ~Test_Generic();
	void init();
	virtual void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
	virtual void finish();
	uint32_t getTotalEvents() { return nTotalEvents;}
        void setNumExpectedEvents(uint32_t n) { nExpectedEvents=n;}
	uint32_t getNumExpectedEvents() const { return nExpectedEvents;}

	void setCSCMapFile(std::string filename);
	void setOutDir(std::string outdir) {outDir=outdir;} 
	void setRootFile(std::string rootfn) {rootFile=rootfn;}
	void setConfigFile(std::string cfgfile) {configFile=cfgfile; loadTestCfg();}
	void setMasksFile(std::string mfile) {masksFile = mfile;loadMasks();}

  protected:
	std::string getCSCFromMap(int crate, int slot, int& csctype, int& cscposition);
	std::string getCSCTypeLabel(int endcap, int station, int ring );
	int getNumStrips(std::string cscID);
	void saveCSCList();
	void doBinCheck();

	virtual void initCSC(std::string cscID) = 0;
	virtual void bookTestsForCSC(std::string cscID);
	virtual void bookCommonHistos();
	int loadTestCfg();
	int loadMasks();
	// virtual void bookTestCanvases(std::string cscID);
	virtual void analyzeCSC(const CSCEventData& data) = 0;
	virtual void finishCSC(std::string cscID) = 0;

	cscTestData tdata;
	cscMonHistos mhistos;
	// MonHistos emuhistos;
	TestCanvases emucnvs;

  	cscTestCanvases tcnvs;
	testParamsCfg xmlCfg;
	TestData tmasks;
	

	uint32_t nTotalEvents;
	uint32_t nBadEvents;
	std::map<std::string, uint32_t> nCSCEvents;
	std::map<std::string, uint32_t> nCSCBadEvents;
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

	std::map<std::string, TH2F*> hFormatErrors;
	std::map<std::string, int> tmap; // Map of CSC types for Format Errors histogram

};

#endif
