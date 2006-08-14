#include "EmuLocalPlotter.h"
//#include "TrackFinderDataFormat/include/SRSPEventData.h"


using namespace std;

EmuLocalPlotter::EmuLocalPlotter()
: HistoFile(histofile),
  fSaveHistos(false),
  consinfo(new TConsumerInfo("DQM", 1)),
  nEvents(0),
  DMBEvents(0),
  ALCTEvents(0),
  CLCTEvents(0),
  CFEBEvents(0),
  BXN(0),
  L1ANumber(0),
  dduCheckMask(0xFFFFFFFF),
  binCheckMask(0xFFFFFFFF),
  logger_(Logger::getInstance("EmuLocalPlotter"))
{
  // EmuLocalPlotter::debug = false;
//KK	
	bin_checker.output1().hide();
	bin_checker.output2().hide();
	bin_checker.crcALCT(true);
	bin_checker.crcTMB (true);
	bin_checker.crcCFEB(true);
//KK end
  edm::logger = logger_;
}

EmuLocalPlotter::EmuLocalPlotter(Logger logger)
: HistoFile(histofile),
  fSaveHistos(false),
  consinfo(new TConsumerInfo("DQM", 1)),
  nEvents(0),
  DMBEvents(0),
  ALCTEvents(0),
  CLCTEvents(0),
  CFEBEvents(0),
  BXN(0),
  L1ANumber(0),
  dduCheckMask(0xFFFFFFFF),
  binCheckMask(0xFFFFFFFF),
  logger_(logger)
{
  // EmuLocalPlotter::debug = false;
//KK
        bin_checker.output1().hide();
        bin_checker.output2().hide();
        bin_checker.crcALCT(true);
        bin_checker.crcTMB (true);
        bin_checker.crcCFEB(true);
//KK end
  edm::logger = logger_;
}



EmuLocalPlotter::~EmuLocalPlotter() {
	for (map<int, map<string, ConsumerCanvas*> >::iterator itr = canvases.begin(); itr != canvases.end(); ++itr) {
		for (map<string, ConsumerCanvas*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
			delete h_itr->second;
		}
	}
	for (map<int, map<string, TH1*> >::iterator itr = histos.begin(); itr != histos.end(); ++itr) {
		for (map<string, TH1*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
			delete h_itr->second;
		}
	}
	// delete consinfo;
}

void EmuLocalPlotter::SetHistoFile(string name) {
	HistoFile = name;
}

void EmuLocalPlotter::SaveHistos() {
        fSaveHistos = true;
}


void EmuLocalPlotter::SetDDUCheckMask(unsigned int mask) {
        dduCheckMask = mask;
}

void EmuLocalPlotter::SetBinCheckMask(unsigned int mask) {
        binCheckMask = mask;
}

void EmuLocalPlotter::SetDDU2004(int flag) {
        ddu2004 = flag;
}


void EmuLocalPlotter::book() {
	int i, j;
	stringstream stname;
	string hname;
	histos[0] = book_common();
}







