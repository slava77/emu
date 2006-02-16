#ifndef EmuSPMonitor_h
#define EmuSPMonitor_h
#include <map>
#include <list>
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

class EmuSPMonitor {
private:
	TH1F hTF_LCTs, hTF_CLCTs, hTF_ALCTs, hTF_CORLCTs, hTF_Eff, hTF_L1As, hTF_ModeProblem;
	TH1F hTF_nTracks, hTF_bxTrack, hTF_bxFirstTrack, hTF_bxSecondTrack, hTF_bxThirdTrack;
	TH1F hTF_BXA, hTF_SEQ, hTF_bxaEff;
	TH2F hTF_L1AvsBX, hTF_VP;
	TH1F hTF_CSCBX[16];
	TCanvas cnv1, cnv2, cnv3, cnv4, cnv5, cnv6;

	TCanvas *Quality[11], *StripID[11], *ClctPattern[11], *WireGroup[11], *BX[11], *Bits[11];
	TH1F *hTF_Quality[11], *hTF_StripID[11], *hTF_halfStripID[11], *hTF_diStripID[11], *hTF_ClctPattern[11], *hTF_WireGroup[11], *hTF_BX[11];
	TH1F *hTF_bc0[11], *hTF_bx0[11], *hTF_lr[11], *hTF_vp[11];

	TH1F hTF_TrkEta, hTF_TrkPhi, hTF_Pt;
	TCanvas *TrackGeom, *TrackPt;

	int refreshCnt, cntCSC[16], seqAmbigious;

	std::list<TCanvas*> canvases;
	std::map<const char*,TH1F*> histograms;

public:
	std::list<TCanvas*> getListOfCanvases(void);
	void fill(const unsigned short* data);
	void bookCanvas(TCanvas* &canvas, const char* name, const char* title, int cscId/*, ...*/) const ;
	void book2Dhist(TH1F* &hist, const char* name, const char* title, int nBins, int max, int min, int cscId /*, ...*/) const ;
	bool bookCSC(int csc);

	EmuSPMonitor(void);
	~EmuSPMonitor(void){}
};

#endif
