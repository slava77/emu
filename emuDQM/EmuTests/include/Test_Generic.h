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
#include "CondFormats/CSCObjects/interface/CSCMapItem.h"
#include "CondFormats/CSCObjects/interface/CSCCrateMap.h"

#include <sqlite3.h>


#include "ConsumerCanvas.hh"
typedef ConsumerCanvas MonitoringCanvas;

#include "CSCReadoutMappingFromFile.h"
#include "CSCMap.h"

#include "TestCanvas_6gr1h.h"
#include "TestCanvas_1h.h"
#include "TestCanvas_userHisto.h"


#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>
#include <TLinearFitter.h>
#include <TVector.h>

#define NLAYERS 6
#define MAX_STRIPS 80
#define SCA_CELLS 96

// Invalid Value for constants
#define INVAL -999.0


typedef std::map<std::string, TestData2D> TestData;
typedef std::map<std::string, TestData> cscTestData;
typedef std::map<std::string, TH1*> MonHistos;
typedef std::map<std::string, MonHistos> cscMonHistos;
// typedef std::map<std::string, TestCanvas_6gr1h*> TestCanvases;
typedef std::map<std::string, TCanvas*> TestCanvases;
typedef std::map<std::string, TestCanvases> cscTestCanvases;
typedef std::map<std::string, std::string> bookParams;
typedef std::map<std::string, bookParams> testParamsCfg;
typedef std::map<std::string, std::pair<int,int> >CSCtoHWmap;
typedef std::map<std::string, int> ResultsCodes;
typedef std::map<std::string, ResultsCodes> cscResultsCodes;

// Test Limits
typedef struct test_limits
{
  double low0;
  double low1;
  double high0;
  double high1;
} test_limits;


// == CFEB SCA cell sample pair (value, count)
typedef struct sca_sample
{
  double value;
  int cnt;
} sca_sample;

// == CFEB SCA data structure
typedef struct CFEBSCAData
{
  int Nbins;
  int Nlayers;
  sca_sample content[NLAYERS][MAX_STRIPS][SCA_CELLS];
} CFEBSCAData;

// == CSC->SCA Data structure
typedef std::map<std::string, CFEBSCAData> cscCFEBSCAData;

#define DAC_STEPS 20
#define TIME_STEPS 10
#define NSAMPLES 9

// == CFEB SCA cell sample pair (value, count)
typedef struct test_step
{
  int active_strip;
  int dac_step;
  int evt_cnt;
  int max_adc;
} test_step;

typedef struct ddu_stats
{
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
typedef struct dac_step
{
  double s;
  double mv;
  double rms;
  double max;
//        double max_rms;
//        int max_cnt;
  int cnt;
} dac_step;



typedef struct time_sample
{
  int tbin;
  unsigned long value;
} time_sample;

typedef struct pulse_fit
{
  time_sample left;
  time_sample max;
  time_sample right;
} pulse_fit;

// == CFEB SCA data structure
typedef struct GainData
{
  int Nbins;
  int Nlayers;
//	dac_step content[2][2][2][2];
//        pulse_fit fit[2][2][2];
  dac_step content[DAC_STEPS][NLAYERS][MAX_STRIPS][NSAMPLES];
  pulse_fit fit[DAC_STEPS][NLAYERS][MAX_STRIPS];
} GainData;

typedef struct time_step
{
  double left;
  double mv;
  double max;
  double right;
//        double max_rms;
//        int max_cnt;
  int cnt;
  int left_cnt;
  int right_cnt;
} time_step;


typedef struct XtalkData
{
  int Nbins;
  int Nlayers;
  time_step content[TIME_STEPS][NLAYERS][MAX_STRIPS][NSAMPLES];
//        pulse_fit fit[TIME_STEPS][NLAYERS][MAX_STRIPS];
} XtalkData;

typedef std::map<std::string, GainData> cscGainData;
typedef std::map<std::string, XtalkData> cscXtalkData;


// == Base Class for Emu Tests
class Test_Generic
{
public:
  Test_Generic(std::string dfile);
  virtual ~Test_Generic();
  void init();
  virtual void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
  virtual void finish();
  uint32_t getTotalEvents()
  {
    return nTotalEvents;
  }
  void setNumExpectedEvents(uint32_t n)
  {
    nExpectedEvents=n;
  }
  uint32_t getNumExpectedEvents() const
  {
    return nExpectedEvents;
  }

  void setCSCMapFile(std::string filename);
  void setOutDir(std::string outdir)
  {
    outDir=outdir;
  }
  void setRootFile(std::string rootfn)
  {
    rootFile=rootfn;
  }
  void setConfigFile(std::string cfgfile)
  {
    configFile=cfgfile;
    loadTestCfg();
  }
  void setMasksFile(std::string mfile)
  {
    masksFile = mfile;
    loadMasks();
  }
  void setSQLiteDBName(std::string filename)
  {
    SQLiteDB = filename;
  }

  time_sample CalculateCorrectedPulseAmplitude(pulse_fit& fit);

protected:
  std::string getCSCFromMap(int crate, int slot, int& csctype, int& cscposition);
  std::string getCSCTypeLabel(int endcap, int station, int ring );
  int getNumStrips(std::string cscID);
  void saveCSCList();
  void doBinCheck();

  virtual void initCSC(std::string cscID) = 0;
  virtual void bookTestsForCSC(std::string cscID);
  virtual void bookCommonHistos();
  void fillCrateMap(CSCCrateMap* mapobj);
  bool fillCrateMapSQLite(CSCCrateMap* mapobj);
  bool fillCrateMapOracle(CSCCrateMap* mapobj);
  void addCSCtoMap(std::string, int, int);

  int loadTestCfg();
  int loadMasks();
  // virtual void bookTestCanvases(std::string cscID);
  virtual void analyzeCSC(const CSCEventData& data) = 0;
  virtual void finishCSC(std::string cscID) = 0;
  virtual bool checkChannel(TestData& cscdata, std::vector<std::string>& tests, int layer, int strip);


  unsigned long binCheckMask;
  cscTestData tdata;
  cscMonHistos mhistos;
  cscResultsCodes rescodes;
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
  std::string SQLiteDB;
  uint32_t imgW, imgH;

  std::map<std::string, TH2F*> hFormatErrors;
  std::map<std::string, int> tmap; // Map of CSC types for Format Errors histogram
  std::map<std::string, test_limits> tlimits;
  CSCtoHWmap cscmap;
  CSCCrateMap* cratemap;
//  	cscmap1 *map;

  Logger logger;

};

#endif
