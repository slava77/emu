/********************************************//**
\class TestCanvas_1h
\brief A Canvas subdivided into independent graphical areas: 1 histo and 1 textpad
\details Canvas layout is sketched in the picture below.
\verbatim 
********************************
*            title             *
********************************
*          *  stat  *          *
*          *  box   *   text   *
* y        **********          *
*                   ************
* t                 *          *
* i    HISTOGRAM    *          *
* t                 *          *
* l                 *          *
* e                 *          *
*                   *          *
*                   *          *
*      x title      *          *
********************************
\endverbatim 
\author Yuriy Pakhotin
\date Fri Jan 18 20:37:22 CET 2008
***********************************************/
#ifndef TEST_CANVAS_1H_H
#define TEST_CANVAS_1H_H

#include <iostream>
#include <string.h>

#include <TROOT.h>
#include <TFile.h>
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

#define NBINS 112
#define NLAYERS 6
#define DEF_WIDTH 1200
#define DEF_HEIGHT 800

#include <TestData2D.h>
#include <TestData1D.h>

class TestCanvas_1h: public TCanvas {
	
	const char * theName;
	const char * theTitle;
	
	int theNbinsx;   ///< Number of bins along X axis of graphs
	double theXlow; ///< Low edge of first bin along X axis of graphs
	double theXup;  ///< Upper edge of last bin along X axis of graphs (not included in last bin)
	
	int theNbinsy;   ///< Number of bins along Y axis of graphs
	double theYlow; ///< Low edge of first bin along Y axis of graphs
	double theYup;  ///< Upper edge of last bin along Y axis of graphs (not included in last bin)
	
// Color index settings: 
	int theFillColor;        ///< Canvas fill color
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
	double theLowLimit;
	double theLowLowLimit;
	double theHighLimit;
	double theHighHighLimit;

// Lines to show performance range on summary histogram
	TLine* theSummaryLowLine;
	TLine* theSummaryHighLine;
	TLine* theSummaryLowLowLine;
	TLine* theSummaryHighHighLine;
// Lines settings
	int theLineWidth;
	int theLineStyle;

// Style to draw
	TStyle *theStyle;

// Main canvas to draw
	TCanvas *theMainCanvas;

// Pad for canvas' title
	TPaveText *theTitlePad;

// Pad and summary histogram and statistics box
	TPad *theLeftPadBackground;
	TH1F *theSummaryHisto;
	TPaveStats *thePtstatsSummaryHisto;

// Text box on the right
	TPaveText *theRightTopPad;

// Blank pad on the right
	TPad *theRightBottomPad;

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
	TestCanvas_1h (std::string, std::string, Int_t, Double_t, Double_t);
	~TestCanvas_1h ();
	const char* GetName(void);                           ///< Get name of canvas
	const char* GetTitle(void);                          ///< Get title of canvas
	void SetCanvasSize(uint32_t, uint32_t);              ///< Set canvas size
	void SetTitle (std::string);                         ///< Set title of canvas
	int SetXTitle (std::string);                        ///< Set x-title of graphs (see canvas layout above)
	int SetYTitle (std::string);                        ///< Set y-title of graphs (see canvas layout above)
	void SetLimits(Double_t,Double_t,Double_t,Double_t); ///< Set performance range limits
	void AddTextTest (std::string);                      ///< Add test name to text box
	void AddTextResult (std::string);                    ///< Add result name to text box
	void AddTextDatafile (std::string);                  ///< Add datafile name to text box
	void AddTextRun (std::string);                       ///< Add time of run collected to text box
	void AddTextAnalysis (std::string);                  ///< Add time of run analysed to text box
	void AddTextEntries (std::string);                   ///< Add total number of entries
	void AddTextLimits (std::string);                    ///< Add total number of entries out of limits
	/** Fill and analyze data.
	    \n Second input is mask (0 - no mask on channel, 1 - channel masked)
	    \n Output is result of quality test:
	    \n 0 = \b White  = no QTs associated with this canvas
	    \n 1 = \b Green  = no channels masked, "good" result for all channels
	    \n 2 = \b Yellow = no channels masked, some channels are in the "tolerable" zone
	    \n 3 = \b Blue   = some channels are masked, unmasked channels are either "good" or "tolerable"
	    \n 4 = \b Red    = some of unmasked channels give results in the "fail" zone
	*/
	int  Fill (TestData2D, TestData2D);                  ///< Fill and analyze data
	int  Fill (TestData1D, TestData1D);                  ///< Fill and analyze data
	void Draw (void);                                    ///< Draw canvas
	void SaveAs (std::string);                           ///< Save as image file
	/**  Write this object to the current directory
	     \n The function returns the total number of bytes written to the file.
	     \n It returns 0 if the object cannot be written.
	*/
	int Write (void);                                  ///< Write this object to the current directory
	int Write (std::string);                           ///< Write this object with new name to the current directory
	TH1F* GetHisto(void);
};


#endif
