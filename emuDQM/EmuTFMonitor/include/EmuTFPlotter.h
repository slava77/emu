#ifndef EmuTFPlotter_h
#define EmuTFPlotter_h

#include "EmuTFfiller.h"
#include "EmuMonitoringObject.h"
#include "EmuMonitoringCanvas.h"
#include "EmuTFxmlParsing.h"
#include <string>

class EmuTFPlotter {
private:
	EmuTFfiller *filler;
	std::string HistoFile;
	int unpackedSPcount;
	unsigned long long nevents;

	std::map<std::string, ME_List>         MEs;
	std::map<std::string, MECanvases_List> MECanvases;
	std::map<std::string, CSCCounters>     cscCntrs;

	std::map<std::string,HistAttributes>   histList;
	std::map<std::string,CanvasAttributes> canvasList;
	std::map<std::string,CheckAttributes>  checkList;

public:
	// All following functions are made for compatibility with emuMonitor package
	void book(void){ return; }
	void setXMLHistosBookingCfgFile(string filename);
	void setXMLCanvasesCfgFile     (string filename);
	void setBinCheckMask(uint32_t mask) { return; }
	void setDDUCheckMask(uint32_t mask) { return; }
	void setCSCMapFile(string filename) { return; }
	void setHistoFile (string hfile) { HistoFile = hfile; }
	int  getUnpackedDMBCount(void) { return unpackedSPcount; }
	bool isBusy(void) { return false; }

	void processEvent(const char *data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber=0);

	std::map<std::string, ME_List >&          GetMEs        (void) { return MEs;        }
	std::map<std::string, MECanvases_List >&  GetMECanvases (void) { return MECanvases; }
	std::map<std::string, CSCCounters >&      GetCSCCounters(void) { return cscCntrs;   }

	EmuTFPlotter(void);
	~EmuTFPlotter(void){ if(filler) delete filler; }

};

#endif
