#ifndef EmuTFPlotter_h
#define EmuTFPlotter_h

#include "EmuTFfiller.h"
#include "EmuMonitoringObject.h"
#include "EmuMonitoringCanvas.h"
#include "EmuTFxmlParsing.h"
#include <string>

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

public:
	// All following functions are made for compatibility with emuMonitor package
	void book(void){ return; }
	void setXMLHistosBookingCfgFile(std::string filename);
	void setXMLCanvasesCfgFile     (std::string filename);
	void setBinCheckMask(uint32_t mask) { return; }
	void setDDUCheckMask(uint32_t mask) { return; }
	void setCSCMapFile(std::string filename) { return; }
	void setHistoFile (std::string hfile) { HistoFile = hfile; }
	int  getUnpackedDMBCount(void) { return unpackedSPcount; }
	bool isBusy(void) { return false; }

	void processEvent(const char *data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber);

	std::map<std::string, ME_List >&          GetMEs        (void) { return MEs;        }
	std::map<std::string, MECanvases_List >&  GetMECanvases (void) { return MECanvases; }
	std::map<std::string, CSCCounters >&      GetCSCCounters(void) { return cscCntrs;   }

	EmuTFPlotter(void);
	~EmuTFPlotter(void){ if(filler) delete filler; }

};

#endif
