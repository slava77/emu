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
#include "EventFilter/CSCRawToDigi/interface/bitset_append.h"
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

// == DDU Bin Examiner
#include "dduBinExaminer.hh"


#define DEFAULT_IMAGE_FORMAT "png"
#define DEFAULT_CANVAS_WIDTH  800
#define DEFAULT_CANVAS_HEIGHT 600


using namespace std;

// class MonitorElement: public TH1 {};
// typedef TH1 MonitorElement;
#include "EmuMonitoringObject.h"
#include "EmuMonitoringCanvas.h"


class EmuPlotter {
public:

  EmuPlotter();
  EmuPlotter(Logger logger);
  ~EmuPlotter();

  // === Book Histograms
  void book();

  ME_List bookChamber(int chamberID);
  ME_List bookCommon(int nodeNumber);
  ME_List bookDDU(int dduNumber);

  MECanvases_List bookChamberCanvases(int chamberID);
  MECanvases_List bookCommonCanvases(int nodeNumber);
  MECanvases_List bookDDUCanvases(int dduNumber);


  // === Fills the data coming in.  unpacks the data into the METBRawFormat
  //     data structures and calls the appropriate fill() routine below.
  void processEvent(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);
  void processChamber(const CSCEventData& data,int32_t nodeID, int32_t dduID);
  void fillChamberBinCheck();

  void setHistoFile(string hfile) {HistoFile = hfile;};
  void setDDUCheckMask(uint32_t mask) { dduCheckMask = mask;}
  void setBinCheckMask(uint32_t mask) { binCheckMask = mask;}

  void saveHistos() {};
  void saveToROOTFile(std::string filename);
  void saveCanvasesToROOTFile(std::string filename);
  void saveImages(std::string path, 
	std::string format=DEFAULT_IMAGE_FORMAT, 
	int width=DEFAULT_CANVAS_WIDTH, 
	int height=DEFAULT_CANVAS_HEIGHT);
  void saveCanvasImages(std::string path, 
        std::string format=DEFAULT_IMAGE_FORMAT, 
        int width=DEFAULT_CANVAS_WIDTH, 
        int height=DEFAULT_CANVAS_HEIGHT);
  void setXMLHistosBookingCfgFile(string filename) {xmlHistosBookingCfgFile = filename;}
  string GetXMLHistosBookingCfgFile() const {return xmlHistosBookingCfgFile;}
  void setXMLCanvasesCfgFile(string filename) {xmlCanvasesCfgFile = filename;}
  string GetXMLCanvasesCfgFile() const {return xmlCanvasesCfgFile;}
   
  bool isListModified() { return fListModified;};
  bool isBusy() { return fBusy;};
  void setListModified(bool flag) { fListModified = flag;};
  bool isMEvalid(ME_List&, string, EmuMonitoringObject* & );
  map<string, ME_List >  GetMEs() { return MEs;};
  map<string, MECanvases_List >  GetMECanvases() { return MECanvases;};
 
  int getUnpackedDMBCount() const {return unpackedDMBcount;}


protected:

  void setParameters(); 
  int loadXMLBookingInfo(string xmlFile);
  int loadXMLCanvasesInfo(string xmlFile);
  void clearMECollection(ME_List &collection);
  void printMECollection(ME_List &collection);
  void clearCanvasesCollection(MECanvases_List &collection);
  void printCanvasesCollection(MECanvases_List &collection);
  // MonitorElement* createME(DOMNode* MEInfo);
  EmuMonitoringObject* createME(DOMNode* MEInfo);  

  void createHTMLNavigation(std:: string path);
  void createTreeTemplate(std::string path);
  void createTreeEngine(std::string path);
  void createTreePage(std::string path);



private:
  // == list of Monitored Elements 
  // map<string, map<string, MonitorElement*> > MEs;
  map<string, ME_List > MEs;
  map<string, MECanvases_List > MECanvases;

  ME_List dduMEfactory;
  ME_List chamberMEfactory;
  ME_List commonMEfactory;

  MECanvases_List dduCanvasesFactory;
  MECanvases_List chamberCanvasesFactory;
  MECanvases_List commonCanvasesFactory;


  map<string,int> nDMBEvents;
  int unpackedDMBcount;

  uint32_t nEvents;
  uint32_t L1ANumber;
  uint32_t BXN;

  uint32_t dduCheckMask;
  uint32_t binCheckMask;

  bool fListModified;
  string HistoFile;
  bool fSaveHistos;
  int SaveTimer;
  int fStopTimer;
  bool fBusy;

  
  string xmlHistosBookingCfgFile;
  string xmlCanvasesCfgFile;

  Logger logger_;
  dduBinExaminer bin_checker;

};

#endif
