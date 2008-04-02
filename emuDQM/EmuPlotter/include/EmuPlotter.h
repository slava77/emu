#ifndef EmuPlotter_h
#define EmuPlotter_h

#include <iostream>
#include <string>
#include <signal.h>
#include <map>
#include <string>
#include <iomanip>
#include <set>
#include <sstream>

#include "xdaq.h"
#include "xdata.h"
#include "toolbox.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

// == CMSSW Section
/*  actually calls  emuDQM/CMSSWLibs/FWCore/MessageLogger/interface/MessageLogger.h */
#include "FWCore/MessageLogger/interface/MessageLogger.h"

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

// ==  ROOT Section
#include <TROOT.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TH1.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TString.h>
#include <TCanvas.h>
#include <TKey.h>

// == DDU Bin Examiner
// #include "dduBinExaminer.hh"

#define DEFAULT_IMAGE_FORMAT "png"
#define DEFAULT_CANVAS_WIDTH  800
#define DEFAULT_CANVAS_HEIGHT 600

// using namespace std;

// class MonitorElement: public TH1 {};
// typedef TH1 MonitorElement;
#include "EmuMonitoringObject.h"
#include "EmuMonitoringCanvas.h"
#include "CSCReadoutMappingFromFile.h"
/*
typedef struct CSCCounters {
	uint32_t ALCTcnt;
	uint32_t CLCTcnt;
	uint32_t CFEBcnt;
	uint32_t DMBcnt;	
} CSCTrigCounters;
*/
typedef std::map<std::string, uint32_t> CSCCounters;

///
/// @class EmuPlotter
/// @brief EmuPlotter class
/// 
///

class EmuPlotter {
public:

  EmuPlotter();
  EmuPlotter(Logger logger);
  ~EmuPlotter();

  // === Book Histograms
  void book();
  int loadFromROOTFile(std::string rootfile, bool fReset=true);

  // === Fills the data coming in.  unpacks the data into the METBRawFormat
  //     data structures and calls the appropriate fill() routine below.
  void processEvent(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
  void processChamber(const CSCEventData& data,int32_t nodeID, int32_t dduID);

  void setHistoFile(std::string hfile) {HistoFile = hfile;};

  void setDDUCheckMask(uint32_t mask) { dduCheckMask = mask;}
  uint32_t getDDUCheckMask() const { return dduCheckMask;}
  void setBinCheckMask(uint32_t mask) { binCheckMask = mask;}
  uint32_t getBinCheckMask() const { return binCheckMask;}
  void setXMLHistosBookingCfgFile(std::string filename) {xmlHistosBookingCfgFile = filename;}
  std::string GetXMLHistosBookingCfgFile() const {return xmlHistosBookingCfgFile;}
  void setXMLCanvasesCfgFile(std::string filename) {xmlCanvasesCfgFile = filename;}
  std::string GetXMLCanvasesCfgFile() const {return xmlCanvasesCfgFile;}
  void setCSCMapFile(std::string filename);
  std::string getCSCMapFile() const {return cscMapFile;}

  void saveHistos() {};

  void saveToROOTFile(std::string filename);
  void saveCanvasesToROOTFile(std::string filename);
  void saveImages(std::string path, 
	std::string format=DEFAULT_IMAGE_FORMAT, 
	int width=DEFAULT_CANVAS_WIDTH, 
	int height=DEFAULT_CANVAS_HEIGHT,
	std::string runname="");
  void saveCanvasImages(std::string path, 
        std::string format=DEFAULT_IMAGE_FORMAT, 
        int width=DEFAULT_CANVAS_WIDTH, 
        int height=DEFAULT_CANVAS_HEIGHT,
	std::string runname="");
  int  convertROOTToImages(std::string rootfile, 
	std::string path="", 
	std::string format=DEFAULT_IMAGE_FORMAT,
        int width=DEFAULT_CANVAS_WIDTH,
        int height=DEFAULT_CANVAS_HEIGHT,
        std::string runname="",
	std::string filter="");
  void generateCanvasesListFile(std::string filename="canvases_list.js", std::string imgformat="png");
  void generateLayout(std::string filename, std::string rootfolder);
   
  bool isListModified() { return fListModified;}
  bool isBusy() { return fBusy;};
  void setListModified(bool flag) { fListModified = flag;}

  void setLogLevel(int level) {logger_.setLogLevel(level);}
  void setUnpackingLogLevel(int level) {Logger::getInstance("CSCRawUnpacking").setLogLevel(level);}
  void setUnpackingDebug(bool flag) {/*CSCDDUEventData::setDebug(flag);*/}

  bool isMEvalid(ME_List&, std::string, EmuMonitoringObject* & );
  std::map<std::string, ME_List >&  GetMEs() { return MEs;};
  std::map<std::string, MECanvases_List >&  GetMECanvases() { return MECanvases;};
  std::map<std::string, CSCCounters >& GetCSCCounters() { return cscCntrs;}
 
  uint32_t getUnpackedDMBCount() const {return unpackedDMBcount;}
  uint32_t getTotalEvents() const {return nEvents;}
  uint32_t getTotalUnpackedCSCs() const {return nCSCEvents;}
  uint32_t getGoodEventsCount() const {return nGoodEvents;}
  uint32_t getBadEventsCount() const {return nBadEvents;}
  void cleanup();
  void updateFractionHistos();

protected:

  ME_List bookChamber(int chamberID);
  ME_List bookCommon(int nodeNumber);
  ME_List bookDDU(int dduNumber);

  MECanvases_List bookChamberCanvases(int chamberID);
  MECanvases_List bookCommonCanvases(int nodeNumber);
  MECanvases_List bookDDUCanvases(int dduNumber);

  void init(); 
  void reset();
  void getCSCFromMap(int crate, int slot, int& csctype, int& cscposition);
  int loadXMLBookingInfo(std::string xmlFile);
  int loadXMLCanvasesInfo(std::string xmlFile);
  void clearMECollection(ME_List &collection);
  void printMECollection(ME_List &collection);
  void clearCanvasesCollection(MECanvases_List &collection);
  void printCanvasesCollection(MECanvases_List &collection);
  EmuMonitoringObject* createME(DOMNode* MEInfo);  

  void fillChamberBinCheck(int32_t nodeNumber, bool isEventDenied);

  void createHTMLNavigation(std::string path);
  void createTreeTemplate(std::string path);
  void createTreeEngine(std::string path);
  void createTreePage(std::string path);

  void calcFractionHisto(ME_List MEs, std::string resultHistoName, std::string setHistoName, std::string subSetHistoName);

  std::map<std::string, int> getCSCTypeToBinMap();
  std::string getCSCTypeLabel(int endcap, int station, int ring );

private:
  // == list of Monitored Elements 
  std::map<std::string, ME_List > MEs;
  std::map<std::string, MECanvases_List > MECanvases;

  ME_List dduMEfactory;
  ME_List chamberMEfactory;
  ME_List commonMEfactory;

  MECanvases_List dduCanvasesFactory;
  MECanvases_List chamberCanvasesFactory;
  MECanvases_List commonCanvasesFactory;

  std::map<std::string,uint32_t> nDMBEvents;
  std::map<uint32_t,uint32_t> L1ANumbers;

  uint32_t L1ANumber;
  uint32_t BXN;
  uint32_t evtSize;

  uint32_t unpackedDMBcount;
  uint32_t nEvents;
  uint32_t nGoodEvents;
  uint32_t nBadEvents;
  uint32_t nCSCEvents;

  Logger logger_;

  CSCDCCExaminer bin_checker;

  uint32_t dduCheckMask;
  uint32_t binCheckMask;

  bool fListModified;
  std::string HistoFile;
  bool fSaveHistos;
  int SaveTimer;
  int fStopTimer;
  bool fBusy;
  bool fFirstEvent;
  
  std::string xmlHistosBookingCfgFile;
  std::string xmlCanvasesCfgFile;
  std::string cscMapFile;

  CSCReadoutMappingFromFile cscMapping;
  std::map<std::string, int> tmap;
  std::string eTag;
  std::map<std::string, CSCCounters> cscCntrs; 

};

#endif
