#ifndef EmuTFPlotter_h
#define EmuTFPlotter_h

#include "emu/dqm/tfanalyzer/EmuTFfiller.h"
#include "emu/dqm/tfanalyzer/EmuTFxmlParsing.h"

#include "emu/dqm/common/EmuMonitoringObject.h"
#include "emu/dqm/common/EmuMonitoringCanvas.h"

#include <string>

// ==  ROOT Section
#include <TROOT.h>
#include <TApplication.h>
#include <TSystem.h>


typedef std::map<std::string, uint32_t> CSCCounters;

class EmuTFPlotter {
private:
	EmuTFfiller *filler;
	std::string HistoFile;
	int unpackedSPcount;
	unsigned long long nevents;

	std::map<std::string, ME_List>         MEs;
	std::map<std::string, MECanvases_List> MECanvases;
	std::map<std::string, CSCCounters>     cscCntrs;

	std::map<std::string,EmuTFxmlParsing::HistAttributes>   histList;
	std::map<std::string,EmuTFxmlParsing::CanvasAttributes> canvasList;
	std::map<std::string,EmuTFxmlParsing::CheckAttributes>  checkList;

	void updateLists(void);

public:
	// All following functions are made for compatibility with emuMonitor package
	void book(void){ return; }
	void setXMLHistosBookingCfgFile(std::string filename); // calling this will "unbook" all the histograms and canvases
	void setXMLCanvasesCfgFile     (std::string filename); // calling this will "unbook" all the histograms and canvases
	void setBinCheckMask(uint32_t mask) { return; }
	void setDDUCheckMask(uint32_t mask) { return; }
	void setCSCMapFile(std::string filename) { return; }
	void setHistoFile (std::string hfile) { HistoFile = hfile; }
	int  getUnpackedDMBCount(void) { return unpackedSPcount; }
	bool isBusy(void) { return false; }

	void processEvent(const char *data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber); //throw (std::exception);
	void validate(void){}
	void saveToROOTFile(std::string outputRootFile);
	void reset();

	std::map<std::string, ME_List >&          getMEs        (void) { updateLists(); return MEs;        }
	std::map<std::string, MECanvases_List >&  getMECanvases (void) { updateLists(); return MECanvases; }
	std::map<std::string, CSCCounters >&      getCSCCounters(void) { return cscCntrs;   }
        TCanvas*				  getCanvas(std::string name);

	EmuTFPlotter(void){ nevents = 0; filler = 0; }
	~EmuTFPlotter(void){ if(filler) delete filler; }

};

#endif
