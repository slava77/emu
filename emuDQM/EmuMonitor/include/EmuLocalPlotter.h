#ifndef EmuLocalPlotter_h
#define EmuLocalPlotter_h

#include "EmuPlotterInterface.h"
#include "dduBinExaminer.hh"

#include "xdaq.h"
#include "xdata.h"
#include "toolbox.h"

#include <iostream>
#include <fstream>
#include <string>
#include <signal.h>
#include <map>
#include <string>
#include <iomanip>
#include <set>
#include <sstream>

// == CMSSW Section
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
#include "DataFormats/CSCDigi/interface/CSCALCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigi.h"

#include "CSCStripClusterFinder.h"


/*
#include "MuEndEventData.h"
#include "MuEndALCTHeader.h"
#include "MuEndALCTTrailer.h"
#include "MuEndAnodeData.h"
#include "MuEndCFEBData.h"
#include "MuEndCFEBTimeSlice.h"
#include "MuEndCLCTData.h"
#include "MuEndDDUEventData.h"
#include "MuEndDDUHeader.h"
#include "MuEndDDUTrailer.h"
#include "MuEndDMBHeader.h"
#include "MuEndDMBTrailer.h"
#include "MuEndEventData.h"
#include "MuEndTMBData.h"
#include "MuEndTMBHeader.h"
#include "MuEndTMBTrailer.h"
#include "Trigger/L1CSCTrigger/interface/L1MuCSCChamberConstants.h"
#include "Trigger/L1CSCTrigger/interface/L1MuCSCAnodeLCT.h"
#include "Trigger/L1CSCTrigger/interface/L1MuCSCChamberConstants.h"
*/

// ==  ROOT Section
class TH1;
#include <TROOT.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TFile.h>
#include <TRandom.h>
#include <TDirectory.h>
#include <TString.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TPad.h>
#include <TColor.h>
#include <TPaletteAxis.h>
#include <TKey.h>

#include "Task.h"
#include "TaskAttributes.h"
// #include "vxSys.h"

// === Includes from CDF offline DisplayServer
class TConsumerInfo;
#include "ConsumerExport.hh"
#include "TConsumerInfo.hh"
#include "ConsumerCanvas.hh"

#define SLIDES_ID 10000
#define SLIDES    "Slides"
#define DEFAULT_IMAGE_FORMAT "png"
#define DEFAULT_CANVAS_WIDTH  800
#define DEFAULT_CANVAS_HEIGHT 600

using namespace std;

class MuEndEventData;

class ConsumerTask: public Task
{
public:
   ConsumerTask(): Task("ConsumerTask") {};
   int svc() { consExp = new ConsumerExport(9050); cout << "+++ Pretending to create Consumer Server" << endl; return 0;}
   void send(TConsumerInfo *info) { cout << "+++ Pretending to send Histos to Consumer Server" << endl; consExp->send(info);}
   void stop() { cout << "+++ Killing Consumer Server" << endl; delete consExp;}
private:
  ConsumerExport *consExp;
};

class EmuLocalPlotter : public EmuPlotterInterface {
public:

  EmuLocalPlotter();
  EmuLocalPlotter(Logger logger);
  ~EmuLocalPlotter();

  void SetHistoFile(string);
  void SaveHistos();
  void SaveImages(std::string path,
        std::string format=DEFAULT_IMAGE_FORMAT,
        int width=DEFAULT_CANVAS_WIDTH,
        int height=DEFAULT_CANVAS_HEIGHT);
  void SaveImagesFromROOTFile(std::string filename,
	std::string path,
        std::string format=DEFAULT_IMAGE_FORMAT,
        int width=DEFAULT_CANVAS_WIDTH,
        int height=DEFAULT_CANVAS_HEIGHT);
  void SetDDUCheckMask(unsigned int);
  void SetBinCheckMask(unsigned int);
  void SetDDU2004(int);
  // === Book Histograms
  void book();
  map<string, TH1*> book_ddu(int);
  map<string, TH1*> book_chamber(int);
  map<string, TH1*> book_common();
  // map<int, map<string, TH1*> > histos;

  // === Fills the data coming in.  unpacks the data into the METBRawFormat
  //     data structures and calls the appropriate fill() routine below.
  virtual void fill(unsigned char * data, int dataSize, unsigned short errorStat);
  // === Fills histograms from unpacked data.i
  virtual void fill(const CSCEventData & eventData, int);
//TF
//  void fillTF(unsigned char * data, int dataLength);
//TFend

  bool isListModified() { return fListModified;};
  void setListModified(bool flag) { fListModified = flag;};
  TConsumerInfo* getConsumerInfo() { return consinfo; };


  // static void saveproc(int sig);
  virtual void save(const char *filename);
  
  virtual unsigned int bin_check(unsigned char * data, int dataSize);

  // static bool debug;

private:
  void createHTMLNavigation(std:: string path);  
  void createTreeTemplate(std::string path);
  void createTreeEngine(std::string path);
  void createTreePage(std::string path);

  bool fListModified;
  string HistoFile;
  bool fSaveHistos;
  int SaveTimer;
  int fStopTimer;
  unsigned int dduCheckMask;
  unsigned int binCheckMask;
  int ddu2004;
  TConsumerInfo* consinfo;
  int L1ANumber;
  int BXN;
  int nEvents;
//KK
  map<int,int> nDMBEvents;
//KKend
  
  int DMBEvents;
  int ALCTEvents;
  int CLCTEvents;
  int CFEBEvents;
  dduBinExaminer bin_checker;

  Logger logger_;
};

static string histofile = "dqm.root";
static bool check_bin_error = true;

static bool unpack_data = true;
static bool unpack_ddu = true;
static bool unpack_dmb = true;
static bool error_printout = true;
static bool debug_printout = true;
static bool fill_histo = true;
static bool folders = true;



//Draw options of TProfiles
static Width_t   TProfileLineWidth   = 1;
static Color_t   TProfileMarkerColor = 2;
static Style_t   TProfileMarkerStyle = 7;
static Size_t    TProfileMarkerSize  = 0.2;
static Option_t *TProfileDrawOption  = "P";

//Draw options of THisto
static Color_t   THistoFillColor = 48;

#endif
