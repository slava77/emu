#include"TestCanvas_1h.h"

TestCanvas_1h::TestCanvas_1h (std::string name, std::string title, Int_t Nbinsy, Double_t ylow, Double_t yup) {

	theName  = name.c_str();
	theTitle = title.c_str();

// Set graphs ranges
	theNbinsx = 10;
	theXlow   = 0;
	theXup    = 10.0;
	
	theNbinsy = Nbinsy;
	theYlow   = ylow;
	theYup    = yup;

// Color index settings	
	theColorWhite         = TColor::GetColor("#ffffff"); ///< White
	theColorGray          = TColor::GetColor("#aaaaaa"); ///< Gray
	theColorGreenLight    = TColor::GetColor("#009900"); ///< Light green
	theColorGreenDark     = TColor::GetColor("#005500"); ///< Dark green
	theColorRedLight      = TColor::GetColor("#ff0000"); ///< Light red
	theColorRedDark       = TColor::GetColor("#cc0000"); ///< Dark red
	theColorYellowLight   = TColor::GetColor("#ffff00"); ///< Light yellow
	theColorYellowDark    = TColor::GetColor("#cccc00"); ///< Dark yellow
	theColorBlueLight     = TColor::GetColor("#0000ff"); ///< Light blue
	
// Default values of performance range limits
	theHighLimit     = (theYup + theYlow)/2.0 + (theYup - theYlow)/6.0;
	theHighHighLimit = (theYup + theYlow)/2.0 + (theYup - theYlow)/3.0;
	theLowLimit      = (theYup + theYlow)/2.0 - (theYup - theYlow)/6.0;
	theLowLowLimit   = (theYup + theYlow)/2.0 - (theYup - theYlow)/3.0;

// Settings for lines
	theLineWidth = 1; ///< Line width = 1 pixel
	theLineStyle = 7; ///< Line style = long dash
// Lines to show performance range on right histogram	
	theSummaryLowLine = new TLine(theLowLimit,0.0,theLowLimit,100.0);
	theSummaryLowLine->SetLineColor(theColorGreenLight);
	theSummaryLowLine->SetLineWidth(theLineWidth);
	theSummaryLowLine->SetLineStyle(theLineStyle);
	theSummaryHighLine = new TLine(theHighLimit,0.0,theHighLimit,100.0);
	theSummaryHighLine->SetLineColor(theColorGreenLight);
	theSummaryHighLine->SetLineWidth(theLineWidth);
	theSummaryHighLine->SetLineStyle(theLineStyle);
	theSummaryLowLowLine = new TLine(theLowLowLimit,0.0,theLowLowLimit,100.0);
	theSummaryLowLowLine->SetLineColor(theColorRedLight);
	theSummaryLowLowLine->SetLineWidth(theLineWidth);
	theSummaryLowLowLine->SetLineStyle(theLineStyle);
	theSummaryHighHighLine = new TLine(theHighHighLimit,0.0,theHighHighLimit,100.0);
	theSummaryHighHighLine->SetLineColor(theColorRedLight);
	theSummaryHighHighLine->SetLineWidth(theLineWidth);
	theSummaryHighHighLine->SetLineStyle(theLineStyle);
	
	theMainCanvas = new TCanvas(theName, theTitle, 1200,800);
	theMainCanvas->SetFillColor(theColorWhite);
	theMainCanvas->SetBorderSize(0);
	
	int fTextFont = 132; ///< Times New Roman
	
	theTitlePad = new TPaveText(.02,.95,.98,.98,"brNDC");
	theTitlePad->SetFillColor(theColorWhite);
	theTitlePad->SetBorderSize(0);
	theTitlePad->SetTextFont(fTextFont);
	theTitlePad->SetTextSize(0.04);
	theTitlePad->SetTextAlign(23);
	theTitlePad->AddText(title.c_str());
	
	double fY0  = 0.01;
//	double fY1  = 0.05;
	double fY2  = 0.93;
	double fYd  = 0.001;
	double fX0  = 0.01;
//	double fX1  = 0.03;
	double fX2  = 0.591;
	
	theLeftPadBackground = new TPad("theLeftPadBackground","theLeftPadBackground",fX0,fY0,fX2,fY2 + fYd);
	theLeftPadBackground->SetFillColor(theColorWhite);
	theLeftPadBackground->SetBorderSize(0);
		
	theRightBottomPad = new TPad("theRightBottomPad","theRightBottomPad",.59,.01,.99,.611);
	theRightBottomPad->SetFillColor(theColorWhite);
	theRightBottomPad->SetBorderSize(0);
	
	theRightTopPad = new TPaveText(.59,.610,.99,fY2+fYd,"brNDC");
	theRightTopPad->SetBorderSize(0);
	theRightTopPad->SetFillColor(theColorWhite);
	theRightTopPad->SetTextAlign(12);
	theRightTopPad->SetTextSize(0.03);
	theRightTopPad->SetTextFont(fTextFont);
	theRightTopPad->AddText("Test:");
	theRightTopPad->AddText("Result:");
	theRightTopPad->AddText("Datafile:");
	theRightTopPad->AddText("Run:");
	theRightTopPad->AddText("Analysis:");
	theRightTopPad->AddText("Number of entries:");
	theRightTopPad->AddText("Out of limits:");
	
	theSummaryHisto = new TH1F(theName, theTitle, theNbinsy, theYlow, theYup);
	theSummaryHisto->SetFillColor(theColorGray);
	theSummaryHisto->GetXaxis()->CenterTitle(true);
	theSummaryHisto->GetXaxis()->SetTitleFont(fTextFont);
	theSummaryHisto->GetXaxis()->SetTitle("Title Y");
	theSummaryHisto->GetYaxis()->SetTitleFont(fTextFont);
	theSummaryHisto->GetYaxis()->SetTitle("Entries");
	
	thePtstatsSummaryHisto = new TPaveStats(0.7,0.75,0.9,0.9,"brNDC");
	thePtstatsSummaryHisto->SetName("stats");
	thePtstatsSummaryHisto->SetBorderSize(2);
	thePtstatsSummaryHisto->SetFillStyle(0);
	thePtstatsSummaryHisto->SetTextAlign(12);
	thePtstatsSummaryHisto->SetOptStat(111110);
	thePtstatsSummaryHisto->SetOptFit(0);
	
	double fMarkerSize = 0.5; ///< Marker size
	int   fSolidCircleMarkerStyle       = 20; ///< Solid circle marker
	int   fEmptyCircleMarkerStyle       = 4;  ///< Empty circle marker
	int   fSolidTriangleUpMarkerStyle   = 22; ///< Solid triangle up marker
	int   fSolidTriangleDownMarkerStyle = 23; ///< Solid triangle down marker
	int   fEmptyTriangleUpMarkerStyle   = 26; ///< Empty triangle up marker (Empty triangle down not implemented)
	
	for(int fNlayer = 0; fNlayer < NLAYERS; fNlayer++) {	
		theGraphGreenSolid[fNlayer] = new TGraph();
 		theGraphGreenSolid[fNlayer]->SetMarkerColor(theColorGreenDark);
 		theGraphGreenSolid[fNlayer]->SetMarkerStyle(fSolidCircleMarkerStyle);
 		theGraphGreenSolid[fNlayer]->SetMarkerSize(fMarkerSize);
		
		theGraphYellowSolid[fNlayer] = new TGraph();
		theGraphYellowSolid[fNlayer]->SetMarkerColor(theColorYellowDark);
		theGraphYellowSolid[fNlayer]->SetMarkerStyle(fSolidCircleMarkerStyle);
		theGraphYellowSolid[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedSolid[fNlayer] = new TGraph();
		theGraphRedSolid[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedSolid[fNlayer]->SetMarkerStyle(fSolidCircleMarkerStyle);
		theGraphRedSolid[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedSolidTriangleUp[fNlayer] = new TGraph();
		theGraphRedSolidTriangleUp[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedSolidTriangleUp[fNlayer]->SetMarkerStyle(fSolidTriangleUpMarkerStyle);
		theGraphRedSolidTriangleUp[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedSolidTriangleDown[fNlayer] = new TGraph();
		theGraphRedSolidTriangleDown[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedSolidTriangleDown[fNlayer]->SetMarkerStyle(fSolidTriangleDownMarkerStyle);
		theGraphRedSolidTriangleDown[fNlayer]->SetMarkerSize(fMarkerSize);
	
		theGraphGreenEmpty[fNlayer] = new TGraph();
 		theGraphGreenEmpty[fNlayer]->SetMarkerColor(theColorGreenDark);
 		theGraphGreenEmpty[fNlayer]->SetMarkerStyle(fEmptyCircleMarkerStyle);
 		theGraphGreenEmpty[fNlayer]->SetMarkerSize(fMarkerSize);
		
		theGraphYellowEmpty[fNlayer] = new TGraph();
		theGraphYellowEmpty[fNlayer]->SetMarkerColor(theColorYellowDark);
		theGraphYellowEmpty[fNlayer]->SetMarkerStyle(fEmptyCircleMarkerStyle);
		theGraphYellowEmpty[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedEmpty[fNlayer] = new TGraph();
		theGraphRedEmpty[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedEmpty[fNlayer]->SetMarkerStyle(fEmptyCircleMarkerStyle);
		theGraphRedEmpty[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedEmptyTriangleUp[fNlayer] = new TGraph();
		theGraphRedEmptyTriangleUp[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedEmptyTriangleUp[fNlayer]->SetMarkerStyle(fEmptyTriangleUpMarkerStyle);
		theGraphRedEmptyTriangleUp[fNlayer]->SetMarkerSize(fMarkerSize);

		theGraphRedEmptyTriangleDown[fNlayer] = new TGraph();
		theGraphRedEmptyTriangleDown[fNlayer]->SetMarkerColor(theColorRedDark);
		theGraphRedEmptyTriangleDown[fNlayer]->SetMarkerStyle(fEmptyTriangleUpMarkerStyle);
		theGraphRedEmptyTriangleDown[fNlayer]->SetMarkerSize(fMarkerSize);
	}
}

TestCanvas_1h::~TestCanvas_1h () {
	delete theMainCanvas;
 	delete theTitlePad;
//	delete theLeftPadBackground;
// 	delete theLeftPad[5];
// 	delete theLeftPad[4];
// 	delete theLeftPad[3];
// 	delete theLeftPad[2];
// 	delete theLeftPad[1];
// 	delete theLeftPad[0];
// 	delete theRightBottomPad;
// 	delete theRightTopPad;
	delete theSummaryHisto;
}

const char*  TestCanvas_1h::GetTitle (void) {
	
	return theTitle;

}

const char*  TestCanvas_1h::GetName (void) {
	
	return theName;

}

void TestCanvas_1h::SetTitle (std::string text) {
	
	theTitle = text.c_str();
	theTitlePad->GetLine(0)->SetText(0.0, 0.0, theTitle);

}

int TestCanvas_1h::SetXTitle (std::string text) {
	
	return 1;

}

int TestCanvas_1h::SetYTitle (std::string text) {
	
	theSummaryHisto->GetXaxis()->SetTitle(text.c_str());
	return 1;

}

void TestCanvas_1h::AddTextTest(std::string text){
	
	std::string fTest ("Test: ");
	theRightTopPad->GetLine(0)->SetText(0.0, 0.0, (fTest + text).c_str());

}

void TestCanvas_1h::AddTextResult(std::string text){
	
	std::string fResult ("Result: ");
	theRightTopPad->GetLine(1)->SetText(0.0, 0.0, (fResult + text).c_str());

}

void TestCanvas_1h::AddTextDatafile (std::string text) {
	
	std::string fDatafile ("Datafile: ");
	theRightTopPad->GetLine(2)->SetText(0.0, 0.0, (fDatafile + text).c_str());

}

void TestCanvas_1h::AddTextRun (std::string text) {
	
	std::string fRun ("Run: ");
	theRightTopPad->GetLine(3)->SetText(0.0, 0.0, (fRun + text).c_str());

}

void TestCanvas_1h::AddTextAnalysis (std::string text) {
	
	std::string fAnalysis ("Analysis: ");
	theRightTopPad->GetLine(4)->SetText(0.0, 0.0,(fAnalysis + text).c_str());

}

void TestCanvas_1h::AddTextEntries (std::string text) {
	
	std::string fEntries ("Number of entries: ");
	theRightTopPad->GetLine(5)->SetText(0.0, 0.0, (fEntries + text).c_str());

}

void TestCanvas_1h::AddTextLimits (std::string text) {

	std::string fOutOfLimits ("Out of limits: ");
	theRightTopPad->GetLine(6)->SetText(0.0, 0.0, (fOutOfLimits + text).c_str());

}

void TestCanvas_1h::SetLimits (Double_t lowLowLimit,Double_t lowLimit,Double_t highLimit,Double_t highHighLimit) {
	
	theLowLimit = lowLimit;
	theLowLowLimit = lowLowLimit;
	theHighLimit = highLimit;
	theHighHighLimit = highHighLimit;
	
	theSummaryLowLine->SetX1(theLowLimit);
	theSummaryLowLine->SetX2(theLowLimit);
	theSummaryLowLowLine->SetX1(theLowLowLimit);
	theSummaryLowLowLine->SetX2(theLowLowLimit);
	theSummaryHighLine->SetX1(theHighLimit);
	theSummaryHighLine->SetX2(theHighLimit);
	theSummaryHighHighLine->SetX1(theHighHighLimit);
	theSummaryHighHighLine->SetX2(theHighHighLimit);
}

int TestCanvas_1h::Fill (TestData1D data, TestData1D mask) {
	return 1;
}

int TestCanvas_1h::Fill (TestData2D data, TestData2D mask) {
	Double_t fX[NBINS], fY[NBINS];
	int fQualityTest = 1;
	int fNOutOfLimits = 0;

	bool fIsRedSolid    = false;
	bool fIsYellowSolid = false;
	bool fIsEmpty  = false;
	
	int fNlayer;
	int fNbin;
	for(fNlayer = 0; fNlayer < data.Nlayers; fNlayer++) {
		for(fNbin = 0; fNbin < data.Nbins; fNbin++) {
			fX[fNbin] = fNbin;
			fY[fNbin] = data.content[fNlayer][fNbin];
			
			theSummaryHisto->Fill(fY[fNbin]);
			
			if(fY[fNbin] <= theHighLimit && fY[fNbin] >= theLowLimit) {
				if(mask.content[fNlayer][fNbin] == 0) {
					theGraphGreenSolid[fNlayer]->SetPoint(theGraphGreenSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				} else {
					if(!fIsEmpty) fIsEmpty = true;
					theGraphGreenEmpty[fNlayer]->SetPoint(theGraphGreenEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				}
				
			}
			if((fY[fNbin] > theHighLimit && fY[fNbin] <= theHighHighLimit) || (fY[fNbin] >= theLowLowLimit && fY[fNbin] < theLowLimit)) {
				if(mask.content[fNlayer][fNbin] == 0) {
					if(!fIsYellowSolid) fIsYellowSolid = true;
					theGraphYellowSolid[fNlayer]->SetPoint(theGraphYellowSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				} else {
					if(!fIsEmpty) fIsEmpty = true;
					theGraphYellowEmpty[fNlayer]->SetPoint(theGraphYellowEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				}
			}
			if((fY[fNbin] > theHighHighLimit && fY[fNbin] <= theYup) || (fY[fNbin] >= theYlow && fY[fNbin] < theLowLowLimit)) {
				
				fNOutOfLimits = fNOutOfLimits + 1;
				if(mask.content[fNlayer][fNbin] == 0) {
					if(!fIsRedSolid) fIsRedSolid = true;
					theGraphRedSolid[fNlayer]->SetPoint(theGraphRedSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				} else {
					if(!fIsEmpty) fIsEmpty = true;
					theGraphRedEmpty[fNlayer]->SetPoint(theGraphRedEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
				}
			}
			if(fY[fNbin] > theYup) {
				
				fNOutOfLimits = fNOutOfLimits + 1;
				if(mask.content[fNlayer][fNbin] == 0) {
					if(!fIsRedSolid) fIsRedSolid = true;
					theGraphRedSolidTriangleUp[fNlayer]->SetPoint(theGraphRedSolidTriangleUp[fNlayer]->GetN(), fX[fNbin], theYup);
				} else {
					if(!fIsEmpty) fIsEmpty = true;
					theGraphRedEmptyTriangleUp[fNlayer]->SetPoint(theGraphRedEmptyTriangleUp[fNlayer]->GetN(), fX[fNbin], theYup);
				}
			}
			if(fY[fNbin] < theYlow) {
				
				fNOutOfLimits = fNOutOfLimits + 1;
				if(mask.content[fNlayer][fNbin] == 0) {
					if(!fIsRedSolid) fIsRedSolid = true;
					theGraphRedSolidTriangleDown[fNlayer]->SetPoint(theGraphRedSolidTriangleDown[fNlayer]->GetN(), fX[fNbin], theYlow);
				} else {
					if(!fIsEmpty) fIsEmpty = true;
					theGraphRedEmptyTriangleDown[fNlayer]->SetPoint(theGraphRedEmptyTriangleDown[fNlayer]->GetN(), fX[fNbin], theYlow);
				}
			}
		}
	}

// Add information (total number of entries and number of enries out of limits) to TextPad
	std::string fsEntries = Form("%.0f",theSummaryHisto->GetEntries());
	AddTextEntries(fsEntries);
	std::string fsOutOfLimits = Form("%d",fNOutOfLimits);
	AddTextLimits(fsOutOfLimits);

// Add information (Entries, Mean, RMS, Underflow, Overflow) to StatPad of summary histogram
	fsEntries = Form("Entries = %.0f",theSummaryHisto->GetEntries());
	thePtstatsSummaryHisto->AddText(fsEntries.c_str());
	std::string fsMean = Form("Mean = %f",theSummaryHisto->GetMean(1));
	thePtstatsSummaryHisto->AddText(fsMean.c_str());
	std::string fsRMS = Form("RMS = %f",theSummaryHisto->GetRMS(1));
	thePtstatsSummaryHisto->AddText(fsRMS.c_str());
	std::string fsUnderflow = Form("Underflow = %.0f",theSummaryHisto->GetBinContent(0));
	thePtstatsSummaryHisto->AddText(fsUnderflow.c_str());
	std::string fsOverflow = Form("Overflow = %.0f",theSummaryHisto->GetBinContent(theNbinsy+1));
	thePtstatsSummaryHisto->AddText(fsOverflow.c_str());
	
	theSummaryLowLine->SetY1(theSummaryHisto->GetMinimum());
	theSummaryLowLine->SetY2(theSummaryHisto->GetMaximum());
	theSummaryLowLowLine->SetY1(theSummaryHisto->GetMinimum());
	theSummaryLowLowLine->SetY2(theSummaryHisto->GetMaximum());
	theSummaryHighLine->SetY1(theSummaryHisto->GetMinimum());
	theSummaryHighLine->SetY2(theSummaryHisto->GetMaximum());
	theSummaryHighHighLine->SetY1(theSummaryHisto->GetMinimum());
	theSummaryHighHighLine->SetY2(theSummaryHisto->GetMaximum());
	
	theFillColor = theColorWhite;
	if(!fIsYellowSolid && !fIsRedSolid && !fIsEmpty) {
		theFillColor = theColorGreenLight;
		fQualityTest = 1;
	}
	if(fIsYellowSolid && !fIsEmpty) {
		theFillColor = theColorYellowLight;
		fQualityTest = 2;
	}
	if(!fIsRedSolid && fIsEmpty){
		theFillColor = theColorBlueLight;
		fQualityTest = 3;
	}
	if(fIsRedSolid){
		theFillColor = theColorRedLight;
		fQualityTest = 4;
	}
	
	theMainCanvas->SetFillColor(theFillColor);

	return fQualityTest;
}

void TestCanvas_1h::UpdateSummary()
{
    if (theSummaryHisto != NULL) {
// Add information (total number of entries and number of enries out of limits) to TextPad
        std::string fsEntries = Form("%.0f",theSummaryHisto->GetEntries());
        AddTextEntries(fsEntries);
//      std::string fsOutOfLimits = Form("%d",fNOutOfLimits);
//      AddTextLimits(fsOutOfLimits);

// Add information (Entries, Mean, RMS, Underflow, Overflow) to StatPad of summary histogram
	thePtstatsSummaryHisto->Clear();
        fsEntries = Form("Entries = %.0f",theSummaryHisto->GetEntries());
        thePtstatsSummaryHisto->AddText(fsEntries.c_str());
        std::string fsMean = Form("Mean = %f",theSummaryHisto->GetMean(1));
        thePtstatsSummaryHisto->AddText(fsMean.c_str());
        std::string fsRMS = Form("RMS = %f",theSummaryHisto->GetRMS(1));
        thePtstatsSummaryHisto->AddText(fsRMS.c_str());
        std::string fsUnderflow = Form("Underflow = %.0f",theSummaryHisto->GetBinContent(0));
        thePtstatsSummaryHisto->AddText(fsUnderflow.c_str());
        std::string fsOverflow = Form("Overflow = %.0f",theSummaryHisto->GetBinContent(theNbinsy+1));
        thePtstatsSummaryHisto->AddText(fsOverflow.c_str());
	theSummaryLowLine->SetY1(theSummaryHisto->GetMinimum());
        theSummaryLowLine->SetY2(theSummaryHisto->GetMaximum());
        theSummaryLowLowLine->SetY1(theSummaryHisto->GetMinimum());
        theSummaryLowLowLine->SetY2(theSummaryHisto->GetMaximum());
        theSummaryHighLine->SetY1(theSummaryHisto->GetMinimum());
        theSummaryHighLine->SetY2(theSummaryHisto->GetMaximum());
        theSummaryHighHighLine->SetY1(theSummaryHisto->GetMinimum());
        theSummaryHighHighLine->SetY2(theSummaryHisto->GetMaximum());	
    }
}

void TestCanvas_1h::SetHistoObject(TH1 *histo) {
	if ((histo != NULL) && (histo != theSummaryHisto)) {
		delete theSummaryHisto;
		theSummaryHisto = histo;
	}
	UpdateSummary();
	
}

void TestCanvas_1h::SetResultCode(int QualityTest) {
	theFillColor = theColorWhite;
	if(QualityTest == 1) {
		theFillColor = theColorGreenLight;
	}
	if(QualityTest == 2) {
		theFillColor = theColorYellowLight;
	}
	if(QualityTest == 3){
		theFillColor = theColorBlueLight;
	}
	if(QualityTest == 4){
		theFillColor = theColorRedLight;
	}
	
	theMainCanvas->SetFillColor(theFillColor);
}

void TestCanvas_1h::Draw (void) {

// Switch off all default titles and statistics boxes		
	theStyle = new TStyle("Style", "Style");
 	theStyle->SetOptStat(0);
 	theStyle->SetOptTitle(0);
 	theStyle->cd();
		
	theMainCanvas->cd();
	theTitlePad->Draw();
	
	theMainCanvas->cd();
	theRightTopPad->Draw();
	
	theMainCanvas->cd();
	theRightBottomPad->Draw();
	
	theMainCanvas->cd();
	theLeftPadBackground->Draw();
	theLeftPadBackground->cd();
	UpdateSummary();
	theSummaryHisto->Draw();
	theSummaryLowLine->Draw();
	theSummaryLowLowLine->Draw();
	theSummaryHighLine->Draw();
	theSummaryHighHighLine->Draw();
	thePtstatsSummaryHisto->Draw();
	
	/* int fNlayer;
	for(fNlayer = NLAYERS - 1; fNlayer >= 0; fNlayer--) {
		theMainCanvas->cd();
		theLeftPad[fNlayer]->Draw();
		theLeftPad[fNlayer]->cd();
		theLeftHisto[fNlayer]->Draw();
		
		theLowLine->Draw();
		theLowLowLine->Draw();
		theHighLine->Draw();
		theHighHighLine->Draw();
		
		if(theGraphGreenSolid[fNlayer]->GetN()           > 0) theGraphGreenSolid[fNlayer]->Draw("P");
		if(theGraphYellowSolid[fNlayer]->GetN()          > 0) theGraphYellowSolid[fNlayer]->Draw("P");
		if(theGraphRedSolid[fNlayer]->GetN()             > 0) theGraphRedSolid[fNlayer]->Draw("P");
		if(theGraphRedSolidTriangleUp[fNlayer]->GetN()   > 0) theGraphRedSolidTriangleUp[fNlayer]->Draw("P");
		if(theGraphRedSolidTriangleDown[fNlayer]->GetN() > 0) theGraphRedSolidTriangleDown[fNlayer]->Draw("P");
		
		if(theGraphGreenEmpty[fNlayer]->GetN()           > 0) theGraphGreenEmpty[fNlayer]->Draw("P");
		if(theGraphYellowEmpty[fNlayer]->GetN()          > 0) theGraphYellowEmpty[fNlayer]->Draw("P");
		if(theGraphRedEmpty[fNlayer]->GetN()             > 0) theGraphRedEmpty[fNlayer]->Draw("P");
		if(theGraphRedEmptyTriangleUp[fNlayer]->GetN()   > 0) theGraphRedEmptyTriangleUp[fNlayer]->Draw("P");
		if(theGraphRedEmptyTriangleDown[fNlayer]->GetN() > 0) theGraphRedEmptyTriangleDown[fNlayer]->Draw("P");
	} */
	
	theMainCanvas->Update();
}

void TestCanvas_1h::SetCanvasSize(uint32_t w, uint32_t h) {
	if (theMainCanvas) theMainCanvas->SetCanvasSize(w,h);
}

void TestCanvas_1h::SaveAs (std::string file_name) {
	if (theMainCanvas) theMainCanvas->SaveAs(file_name.c_str());
}

int TestCanvas_1h::Write (std::string newName) {
	int fNBuffer = 0;
	if(theMainCanvas) {
		fNBuffer = theMainCanvas->Write(newName.c_str(),TObject::kOverwrite);
	}
//	theStyle->Write();
	return fNBuffer;
}

int TestCanvas_1h::Write (void) {
	int fNBuffer = 0;
	if(theMainCanvas) {
		fNBuffer = theMainCanvas->Write(theMainCanvas->GetName(),TObject::kOverwrite);
	}
//	theStyle->Write();
	return fNBuffer;
}

TH1* TestCanvas_1h::GetHisto (void) {

	return theSummaryHisto;
}
