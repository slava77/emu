/********************************************//**
\class CFEBCanvas
\brief A Canvas subdivided into independent graphical areas: 6 graphs, 1 histo and 1 textpad
\details Canvas layout is sketched in the picture below.
\verbatim 
********************************
*            title             *
********************************
*   *      gr1      *          *
*   *****************   text   *
* y *      gr2      *          *
*   ****************************
* t *      gr3      *   * stat *
* i *****************   * box  *
* t *      gr4      *   ********
* l *****************          *
* e *      gr5      *   histo  *
*   *****************          *
*   *      gr6      *          *
*   *    x title    *  y title *
********************************
\endverbatim 
\author Yuriy Pakhotin
\date Thu Jan 10 15:39:13 CET 2008
***********************************************/
#ifndef CFEB_CANVAS_H
#define CFEB_CANVAS_H

#include <iostream>
#include <string.h>

#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TColor.h>
#include <TString.h>
#include <TLine.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TPaveText.h>
#include <TText.h>
#include <TPaveStats.h>

#define NBINS 80
#define NLAYERS 6
#define DEF_WIDTH 1200
#define DEF_HEIGHT 800

#include <CFEBData.h>

class CFEBCanvas {

	int theNbinsx;   ///< Number of bins along X axis of graphs
	Float_t theXlow; ///< Low edge of first bin along X axis of graphs
	Float_t theXup;  ///< Upper edge of last bin along X axis of graphs (not included in last bin)
	
	int theNbinsy;   ///< Number of bins along Y axis of graphs
	Float_t theYlow; ///< Low edge of first bin along Y axis of graphs
	Float_t theYup;  ///< Upper edge of last bin along Y axis of graphs (not included in last bin)

// Color index settings: 
	int theColorWhite;       ///< White
	int theColorGray;        ///< Gray
	int theColorGreenLight;  ///< Light green
	int theColorGreenDark;   ///< Dark green
	int theColorRedLight;    ///< Light red
	int theColorRedDark;     ///< Dark red
	int theColorYellowLight; ///< Light yellow
	int theColorYellowDark;  ///< Dark yellow
	int theColorBlueLight;   ///< Light blue

// Performance range limits settings
//
// *************************************
// *       ---------HighHighLimit Line *
// *       ---------HighLimit Line     *
// * graph                             *
// *       ---------LowLimit Line      *
// *       ---------LowLowLimit Line   *
// *************************************
	Float_t theLowLimit;
	Float_t theLowLowLimit;
	Float_t theHighLimit;
	Float_t theHighHighLimit;

// Lines to show performance range on left graphs
	TLine* theLowLine;
	TLine* theHighLine;
	TLine* theLowLowLine;
	TLine* theHighHighLine;
// Lines to show performance range on right histogram
	TLine* theRightLowLine;
	TLine* theRightHighLine;
	TLine* theRightLowLowLine;
	TLine* theRightHighHighLine;
// Lines settings
	int theLineWidth;
	int theLineStyle;

// Main canvas to draw
	TCanvas *theMainCanvas;

// Pad for canvas' title
	TPaveText *theTitlePad;

// Pad and histogram to make common vertical title for all graphs
	TPad *theLeftPadBackground;
	TH2F *theLeftHistoBackground;

// Pads on the left
	TPad *theLeftPad[NLAYERS];
// Histograms on the left to make axis ranges and titles
	TH2F* theLeftHisto[NLAYERS];

// Text box on the right
	TPaveText *theRightTopPad;

// Pad on the right with summary histogram and statistics box	
	TPad *theRightBottomPad;
	TH1F *theRightHisto;
	TPaveStats *thePtstatsRightHisto;

// Graphs for different layers with solid markers = unmasked channels
	TGraph* theGraphGreenSolid[NLAYERS];
	TGraph* theGraphYellowSolid[NLAYERS];
	TGraph* theGraphRedSolid[NLAYERS];
	TGraph* theGraphRedSolidTriangleUp[NLAYERS];
	TGraph* theGraphRedSolidTriangleDown[NLAYERS];;
// Graphs for different layers with empty markers = masked channels
	TGraph* theGraphGreenEmpty[NLAYERS];
	TGraph* theGraphYellowEmpty[NLAYERS];
	TGraph* theGraphRedEmpty[NLAYERS];
	TGraph* theGraphRedEmptyTriangleUp[NLAYERS];
	TGraph* theGraphRedEmptyTriangleDown[NLAYERS];

  public:
	CFEBCanvas (const char*, const char*, Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
	// CFEBCanvas (Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
	~CFEBCanvas ();
	void SetTitle (std::string);                         ///< Set title of canvas
	void SetXTitle (std::string);                        ///< Set x-title of graphs (see canvas layout above)
	void SetYTitle (std::string);                        ///< Set y-title of graphs (see canvas layout above)
	void SetLimits(Double_t,Double_t,Double_t,Double_t); ///< Set performance range limits
	void AddTextTest (std::string);                      ///< Add test name to text box
	void AddTextResult (std::string);                    ///< Add result name to text box
	void AddTextDatafile (std::string);                  ///< Add datafile name to text box
	void AddTextRun (std::string);                       ///< Add time of run collected to text box
	void AddTextAnalysis (std::string);                  ///< Add time of run analysed to text box
	void AddTextEntries (std::string);                   ///< Add total number of entries
	void AddTextLimits (std::string);                    ///< Add total number of entries out of limits
	void SetCanvasSize(uint32_t, uint32_t);		     ///< Set main canvas size
	/** Fill and analyze data.
	    \n Second input is mask (0 - no mask on channel, 1 - channel masked)
	    \n Output is result of quality test:
	    \n 0 = \b White  = no QTs associated with this canvas
	    \n 1 = \b Green  = no channels masked, "good" result for all channels
	    \n 2 = \b Yellow = no channels masked, some channels are in the "tolerable" zone
	    \n 3 = \b Blue   = some channels are masked, unmasked channels are either "good" or "tolerable"
	    \n 4 = \b Red    = some of unmasked channels give results in the "fail" zone
	*/
	int  Fill (CFEBData&, CFEBData&);                      ///< Fill and analyze data
	void Draw (void);                                    ///< Draw canvas
	void SaveAs (void);                                  ///< Save as canvas_6gr1h.png and canvas_6gr1h.ps
	void SaveAs (std::string name);
	void Write(void);
    
};


#endif
