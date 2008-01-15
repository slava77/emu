#ifndef CFEB_TEST_CANVAS_H
#define CFEB_TEST_CANVAS_H

#include <iostream>
using namespace std;
#include <stdio.h>
#include <math.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TColor.h>
#include <TLine.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TPaveText.h>
#include <TText.h>
#include <TPaveStats.h>
#include <TFile.h>
#include <TSystem.h>

#define NBINS 80
#define NLAYERS 6

struct CFEBData{
	int Nbins;
	int Nlayers;
	double content[6][80];
	int cnts[6][80];
};

class CFEBTestCanvas {
	int ci;   // for color index setting

	Float_t MinValue;
	Float_t MaxValue;
	Float_t LowLimit;
	Float_t LowLowLimit;
	Float_t HighLimit;
	Float_t HighHighLimit;
	
	TCanvas *cnv;

	TPaveText *title_pad;

	TPad *left_pad_background;
	TH2F* left_histo_background;

	TPad *left_pad[NLAYERS];
	TH2F* left_histo[NLAYERS];

	TPaveText *right_pad_top;
	
	TPad *right_pad_bottom;
	TH1F* right_histo;
	TPaveStats* right_histo_ptstats;
	
	TGraph* grGreenSolid[NLAYERS];
	TGraph* grYellowSolid[NLAYERS];
	TGraph* grRedSolid[NLAYERS];
	TGraph* grRedSolidTriangleUp[NLAYERS];
	TGraph* grRedSolidTriangleDown[NLAYERS];
	Int_t nGreenSolid[NLAYERS];
	Int_t nYellowSolid[NLAYERS];
	Int_t nRedSolid[NLAYERS];
	Int_t nRedSolidTriangleUp[NLAYERS];
	Int_t nRedSolidTriangleDown[NLAYERS];
	
	TLine* LowLine;
	TLine* HighLine;
	TLine* LowLowLine;
	TLine* HighHighLine;
	TLine* rightLowLine;
	TLine* rightHighLine;
	TLine* rightLowLowLine;
	TLine* rightHighHighLine;
	
  public:
	CFEBTestCanvas (const char*, const char*, Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
	~CFEBTestCanvas ();
	void AddTextTest (const char*);
	void AddTextResult (const char*);
	void AddTextDatafile (const char*);
	void AddTextRun (const char*);
	void AddTextAnalysis (const char*);
	void AddTextEntries (const char*);
	void AddTextLimits (const char*);
	void SetTitle (const char*);
	void SetXTitle (const char*);
	void SetYTitle (const char*);
	void SetLimits(Double_t,Double_t,Double_t,Double_t);
	bool Fill (CFEBData);
	void Draw (void);
	void SaveAs (std::string name="test");
	void Write();
    
};

CFEBTestCanvas::CFEBTestCanvas (const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup) {
	
	MinValue = ylow;
	MaxValue = yup;
	HighLimit     = (MaxValue + MinValue)/2.0 + (MaxValue - MinValue)/6.0;
	HighHighLimit = (MaxValue + MinValue)/2.0 + (MaxValue - MinValue)/3.0;
	LowLimit      = (MaxValue + MinValue)/2.0 - (MaxValue - MinValue)/6.0;
	LowLowLimit   = (MaxValue + MinValue)/2.0 - (MaxValue - MinValue)/3.0;
		
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	
	cnv = new TCanvas(name, title,1200,800);
	cnv->SetFillColor(0);
//	cnv->SetFillColor(2);
	cnv->SetBorderSize(0);
	cnv->SetBorderMode(0);
	cnv->SetFrameFillColor(0);
	
	title_pad = new TPaveText(.02,.944,.98,.98,"brNDC");
	title_pad->SetBorderSize(0);
	title_pad->SetFillColor(0);
	title_pad->SetLineColor(0);
	title_pad->SetTextFont(132);
	title_pad->SetTextSize(0.05);
	title_pad->SetTextAlign(23);
//	title_pad->AddText("CSC ID: ME+2/2/112");
	title_pad->AddText(title);
	
	
	double ry0  = 0.01;
	double rdy1 = 0.182;
	double rdy2 = 0.1456;
	double rdy  = 0.001;
	double rx1  = 0.03;
	double rx2  = 0.591;
	
	left_pad_background = new TPad("left_pad_background","left_pad_background",0.01,ry0,rx2,ry0+rdy1+5.0*rdy2+rdy);
	left_pad_background->SetFillColor(0);
	left_pad_background->SetBorderSize(0);
	left_pad_background->SetBorderMode(0);
	left_pad_background->SetFrameFillColor(0);
	left_pad_background->SetRightMargin(0.2);
	left_pad_background->SetLeftMargin(0.2);
	left_pad_background->SetTopMargin(0.2);
	left_pad_background->SetBottomMargin(0.2);
	
	
	left_pad[5] = new TPad("left_pad[5]","left_pad[5]",rx1,ry0,rx2,ry0+rdy1+rdy);
	left_pad[5]->SetFillColor(0);
	left_pad[5]->SetBorderSize(0);
	left_pad[5]->SetBorderMode(0);
	left_pad[5]->SetFrameFillColor(0);
	left_pad[5]->SetRightMargin(0.01);
	left_pad[5]->SetLeftMargin(0.07);
	left_pad[5]->SetTopMargin(0.05);
	left_pad[5]->SetBottomMargin(0.2);
	
	
	left_pad[4] = new TPad("left_pad[4]","left_pad[4]",rx1,ry0+rdy1,rx2,ry0+rdy1+rdy2+rdy);
	left_pad[4]->SetFillColor(0);
	left_pad[4]->SetBorderSize(0);
	left_pad[4]->SetBorderMode(0);
	left_pad[4]->SetFrameFillColor(0);
	left_pad[4]->SetRightMargin(0.01);
	left_pad[4]->SetLeftMargin(0.07);
	left_pad[4]->SetTopMargin(0.05);
	left_pad[4]->SetBottomMargin(0.0);
	
	
	left_pad[3] = new TPad("left_pad[3]","left_pad[3]",rx1,ry0+rdy1+rdy2,rx2,ry0+rdy1+2.0*rdy2+rdy);
	left_pad[3]->SetFillColor(0);
	left_pad[3]->SetBorderSize(0);
	left_pad[3]->SetBorderMode(0);
	left_pad[3]->SetFrameFillColor(0);
	left_pad[3]->SetRightMargin(0.01);
	left_pad[3]->SetLeftMargin(0.07);
	left_pad[3]->SetTopMargin(0.05);
	left_pad[3]->SetBottomMargin(0.0);
	
	
	left_pad[2] = new TPad("left_pad[2]","left_pad[2]",rx1,ry0+rdy1+2.0*rdy2,rx2,ry0+rdy1+3.0*rdy2+rdy);
	left_pad[2]->SetFillColor(0);
	left_pad[2]->SetBorderSize(0);
	left_pad[2]->SetBorderMode(0);
	left_pad[2]->SetFrameFillColor(0);
	left_pad[2]->SetRightMargin(0.01);
	left_pad[2]->SetLeftMargin(0.07);
	left_pad[2]->SetTopMargin(0.05);
	left_pad[2]->SetBottomMargin(0.0);
	
	
	left_pad[1] = new TPad("left_pad[1]","left_pad[1]",rx1,ry0+rdy1+3.0*rdy2,rx2,ry0+rdy1+4.0*rdy2+rdy);
	left_pad[1]->SetFillColor(0);
	left_pad[1]->SetBorderSize(0);
	left_pad[1]->SetBorderMode(0);
	left_pad[1]->SetFrameFillColor(0);
	left_pad[1]->SetRightMargin(0.01);
	left_pad[1]->SetLeftMargin(0.07);
	left_pad[1]->SetTopMargin(0.05);
	left_pad[1]->SetBottomMargin(0.0);
	
	
	left_pad[0] = new TPad("left_pad[0]","left_pad[0]",rx1,ry0+rdy1+4.0*rdy2,rx2,ry0+rdy1+5.0*rdy2+rdy);
	left_pad[0]->SetFillColor(0);
	left_pad[0]->SetBorderSize(0);
	left_pad[0]->SetBorderMode(0);
	left_pad[0]->SetFrameFillColor(0);
	left_pad[0]->SetRightMargin(0.01);
	left_pad[0]->SetLeftMargin(0.07);
	left_pad[0]->SetTopMargin(0.05);
	left_pad[0]->SetBottomMargin(0.0);
	
	
	right_pad_bottom = new TPad("right_pad_bottom","right_pad_bottom",.59,.01,.99,.611);
	right_pad_bottom->SetFillColor(0);
	right_pad_bottom->SetBorderSize(0);
	right_pad_bottom->SetBorderMode(0);
	right_pad_bottom->SetFrameFillColor(0);
	
	
	right_pad_top = new TPaveText(.59,.610,.99,ry0+rdy1+5.0*rdy2+rdy,"brNDC");
	right_pad_top->SetBorderSize(0);
//	right_pad_top->SetFillStyle(0);
	right_pad_top->SetFillColor(0);
	right_pad_top->SetLineColor(0);
	right_pad_top->SetTextAlign(12);
	right_pad_top->SetTextSize(0.02);
	right_pad_top->SetTextFont(132);
	right_pad_top->AddText("Test:");
	right_pad_top->AddText("Result:");
	right_pad_top->AddText("Datafile:");
	right_pad_top->AddText("Run:");
	right_pad_top->AddText("Analysis:");
	right_pad_top->AddText("Number of entries:");
	right_pad_top->AddText("Out of limits:");
//	right_pad_top->AddText("Test: CFEB02 - pedestals and noise");
// 	right_pad_top->AddText("Result: R01 - overall pedestals");
// 	right_pad_top->AddText("Datafile: test_15_01.results");
// 	right_pad_top->AddText("Run: 2007-12-20 16:44");
// 	right_pad_top->AddText("Analysis: 12/21/07 11:52:45, version 0.1");
// 	right_pad_top->AddText("Number of entries: 80");
// 	right_pad_top->AddText("Out of limits: 0");
	
	right_histo = new TH1F("right_histogram", "right_histogram", nbinsy, ylow, yup);
	right_histo->GetXaxis()->CenterTitle(true);
	right_histo->GetXaxis()->SetTitleFont(132);
//	right_histo->GetXaxis()->SetTitle("Pedestal mean, ADC counts");
	right_histo->GetXaxis()->SetTitle("Title Y");
	right_histo->GetYaxis()->SetTitleFont(132);
	right_histo->GetYaxis()->SetTitle("Entries");
	
	right_histo_ptstats = new TPaveStats(0.6951547,0.7806434,0.8976115,0.8998024,"brNDC");
	right_histo_ptstats->SetName("stats");
	right_histo_ptstats->SetBorderSize(2);
	right_histo_ptstats->SetFillStyle(0);
	right_histo_ptstats->SetTextAlign(12);
	right_histo_ptstats->SetOptStat(1110);
	right_histo_ptstats->SetOptFit(0);
	
	left_histo_background = new TH2F("left_graph_0", "left_graph_0", nbinsx, xlow, xup, nbinsy, ylow, yup);
//	left_histo_background->GetYaxis()->SetTitle("Pedestal mean, ADC counts");
	left_histo_background->GetYaxis()->SetTitle("Title Y");
	left_histo_background->GetYaxis()->SetTitleFont(132);
	left_histo_background->GetYaxis()->CenterTitle(true);
	left_histo_background->GetYaxis()->SetTitleSize(0.03);
	left_histo_background->GetYaxis()->SetTitleOffset(3.90);
	
	left_histo[0] = new TH2F("left_graph_1", "left_graph_1", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[0]->GetXaxis()->SetNdivisions(516);
	left_histo[0]->GetXaxis()->SetLabelSize(0.0);
	left_histo[0]->GetXaxis()->SetTickLength(0.04);
	left_histo[0]->GetYaxis()->SetTitle("Layer 1");
	left_histo[0]->GetYaxis()->SetTitleFont(132);
	left_histo[0]->GetYaxis()->CenterTitle(true);
	left_histo[0]->GetYaxis()->SetNdivisions(410);
	left_histo[0]->GetYaxis()->SetLabelSize(0.1);
	left_histo[0]->GetYaxis()->SetTitleSize(0.16);
	left_histo[0]->GetYaxis()->SetTickLength(0.02);
	left_histo[0]->GetYaxis()->SetTitleOffset(0.20);
	
	left_histo[1] = new TH2F("left_graph_2", "left_graph_2", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[1]->GetXaxis()->SetNdivisions(516);
	left_histo[1]->GetXaxis()->SetLabelSize(0.0);
	left_histo[1]->GetXaxis()->SetTickLength(0.04);
	left_histo[1]->GetYaxis()->SetTitle("Layer 2");
	left_histo[1]->GetYaxis()->SetTitleFont(132);
	left_histo[1]->GetYaxis()->CenterTitle(true);
	left_histo[1]->GetYaxis()->SetNdivisions(410);
	left_histo[1]->GetYaxis()->SetLabelSize(0.1);
	left_histo[1]->GetYaxis()->SetTitleSize(0.16);
	left_histo[1]->GetYaxis()->SetTickLength(0.02);
	left_histo[1]->GetYaxis()->SetTitleOffset(0.20);
	
	left_histo[2] = new TH2F("left_graph_3", "left_graph_3", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[2]->GetXaxis()->SetNdivisions(516);
	left_histo[2]->GetXaxis()->SetLabelSize(0.0);
	left_histo[2]->GetXaxis()->SetTickLength(0.04);
	left_histo[2]->GetYaxis()->SetTitle("Layer 3");
	left_histo[2]->GetYaxis()->SetTitleFont(132);
	left_histo[2]->GetYaxis()->CenterTitle(true);
	left_histo[2]->GetYaxis()->SetNdivisions(410);
	left_histo[2]->GetYaxis()->SetLabelSize(0.1);
	left_histo[2]->GetYaxis()->SetTitleSize(0.16);
	left_histo[2]->GetYaxis()->SetTickLength(0.02);
	left_histo[2]->GetYaxis()->SetTitleOffset(0.20);
	
	left_histo[3] = new TH2F("left_graph_4", "left_graph_4", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[3]->GetXaxis()->SetNdivisions(516);
	left_histo[3]->GetXaxis()->SetLabelSize(0.0);
	left_histo[3]->GetXaxis()->SetTickLength(0.04);
	left_histo[3]->GetYaxis()->SetTitle("Layer 4");
	left_histo[3]->GetYaxis()->SetTitleFont(132);
	left_histo[3]->GetYaxis()->CenterTitle(true);
	left_histo[3]->GetYaxis()->SetNdivisions(410);
	left_histo[3]->GetYaxis()->SetLabelSize(0.1);
	left_histo[3]->GetYaxis()->SetTitleSize(0.16);
	left_histo[3]->GetYaxis()->SetTickLength(0.02);
	left_histo[3]->GetYaxis()->SetTitleOffset(0.20);
	
	left_histo[4] = new TH2F("left_graph_5", "left_graph_5", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[4]->GetXaxis()->SetNdivisions(516);
	left_histo[4]->GetXaxis()->SetLabelSize(0.0);
	left_histo[4]->GetXaxis()->SetTickLength(0.04);
	left_histo[4]->GetYaxis()->SetTitle("Layer 5");
	left_histo[4]->GetYaxis()->SetTitleFont(132);
	left_histo[4]->GetYaxis()->CenterTitle(true);
	left_histo[4]->GetYaxis()->SetNdivisions(410);
	left_histo[4]->GetYaxis()->SetLabelSize(0.1);
	left_histo[4]->GetYaxis()->SetTitleSize(0.16);
	left_histo[4]->GetYaxis()->SetTickLength(0.02);
	left_histo[4]->GetYaxis()->SetTitleOffset(0.20);
	
	left_histo[5] = new TH2F("left_graph_6", "left_graph_6", nbinsx, xlow, xup, nbinsy, ylow, yup);
	left_histo[5]->GetXaxis()->SetTitle("Title X");
	left_histo[5]->GetXaxis()->SetTitleFont(132);
	left_histo[5]->GetXaxis()->CenterTitle(true);
	left_histo[5]->GetXaxis()->SetNdivisions(516);
	left_histo[5]->GetXaxis()->SetLabelSize(0.08);
	left_histo[5]->GetXaxis()->SetTitleSize(0.12);
	left_histo[5]->GetXaxis()->SetTickLength(0.03);
	left_histo[5]->GetXaxis()->SetTitleOffset(0.7);
	left_histo[5]->GetYaxis()->SetTitle("Layer 6");
	left_histo[5]->GetYaxis()->SetTitleFont(132);
	left_histo[5]->GetYaxis()->CenterTitle(true);
	left_histo[5]->GetYaxis()->SetNdivisions(410);
	left_histo[5]->GetYaxis()->SetLabelSize(0.08);
	left_histo[5]->GetYaxis()->SetTitleSize(0.12);
	left_histo[5]->GetYaxis()->SetTickLength(0.02);
	left_histo[5]->GetYaxis()->SetTitleOffset(0.26);

	for(int Nlayer = 0; Nlayer < NLAYERS; Nlayer++) {	
		grGreenSolid[Nlayer] = new TGraph(NLAYERS);
		ci = TColor::GetColor("#003300");
 		grGreenSolid[Nlayer]->SetMarkerColor(ci);
 		grGreenSolid[Nlayer]->SetMarkerStyle(20);
 		grGreenSolid[Nlayer]->SetMarkerSize(0.3);
		
		grYellowSolid[Nlayer] = new TGraph(NLAYERS);
		ci = TColor::GetColor("#ff9900");
		grYellowSolid[Nlayer]->SetMarkerColor(ci);
		grYellowSolid[Nlayer]->SetMarkerStyle(20);
		grYellowSolid[Nlayer]->SetMarkerSize(0.3);

		grRedSolid[Nlayer] = new TGraph(NLAYERS);
		ci = TColor::GetColor("#cc0000");
		grRedSolid[Nlayer]->SetMarkerColor(ci);
		grRedSolid[Nlayer]->SetMarkerStyle(20);
		grRedSolid[Nlayer]->SetMarkerSize(0.3);

		grRedSolidTriangleUp[Nlayer] = new TGraph(NLAYERS);
		ci = TColor::GetColor("#cc0000");
		grRedSolidTriangleUp[Nlayer]->SetMarkerColor(ci);
		grRedSolidTriangleUp[Nlayer]->SetMarkerStyle(22);
		grRedSolidTriangleUp[Nlayer]->SetMarkerSize(0.3);

		grRedSolidTriangleDown[Nlayer] = new TGraph(NLAYERS);
		ci = TColor::GetColor("#cc0000");
		grRedSolidTriangleDown[Nlayer]->SetMarkerColor(ci);
		grRedSolidTriangleDown[Nlayer]->SetMarkerStyle(23);
		grRedSolidTriangleDown[Nlayer]->SetMarkerSize(0.3);
	}
	
	for(int Nlayer = 0; Nlayer < NLAYERS; Nlayer++) {
		nGreenSolid[Nlayer] = 0;
		nYellowSolid[Nlayer] = 0;
		nRedSolid[Nlayer] = 0;
		nRedSolidTriangleUp[Nlayer] = 0;
		nRedSolidTriangleDown[Nlayer] = 0;
	}
	
	ci = TColor::GetColor("#006600");
	LowLine = new TLine(xlow,LowLimit,xup,LowLimit);
	LowLine->SetLineColor(ci);
	LowLine->SetLineWidth(1);
	LowLine->SetLineStyle(7);
	HighLine = new TLine(xlow,HighLimit,xup,HighLimit);
	HighLine->SetLineColor(ci);
	HighLine->SetLineWidth(1);
	HighLine->SetLineStyle(7);
	LowLowLine = new TLine(xlow,LowLowLimit,xup,LowLowLimit);
	LowLowLine->SetLineColor(2);
	LowLowLine->SetLineWidth(1);
	LowLowLine->SetLineStyle(7);
	HighHighLine = new TLine(xlow,HighHighLimit,xup,HighHighLimit);
	HighHighLine->SetLineColor(2);
	HighHighLine->SetLineWidth(1);
	HighHighLine->SetLineStyle(7);
	
	rightLowLine = new TLine(LowLimit,0.0,LowLimit,100.0);
	rightLowLine->SetLineColor(ci);
	rightLowLine->SetLineWidth(1);
	rightLowLine->SetLineStyle(7);
	rightHighLine = new TLine(HighLimit,0.0,HighLimit,100.0);
	rightHighLine->SetLineColor(ci);
	rightHighLine->SetLineWidth(1);
	rightHighLine->SetLineStyle(7);
	rightLowLowLine = new TLine(LowLowLimit,0.0,LowLowLimit,100.0);
	rightLowLowLine->SetLineColor(2);
	rightLowLowLine->SetLineWidth(1);
	rightLowLowLine->SetLineStyle(7);
	rightHighHighLine = new TLine(HighHighLimit,0.0,HighHighLimit,100.0);
	rightHighHighLine->SetLineColor(2);
	rightHighHighLine->SetLineWidth(1);
	rightHighHighLine->SetLineStyle(7);
	

}

CFEBTestCanvas::~CFEBTestCanvas () {
	delete cnv;
 	delete title_pad;
//	delete left_pad_background;
// 	delete left_pad[5];
// 	delete left_pad[4];
// 	delete left_pad[3];
// 	delete left_pad[2];
// 	delete left_pad[1];
// 	delete left_pad[0];
// 	delete right_pad_bottom;
// 	delete right_pad_top;
	delete right_histo;
	delete left_histo_background;
	delete left_histo[0];
	delete left_histo[1];
	delete left_histo[2];
	delete left_histo[3];
	delete left_histo[4];
	delete left_histo[5];
}

void CFEBTestCanvas::AddTextTest (const char* text) {

	right_pad_top->GetLine(0)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextResult (const char* text) {

	right_pad_top->GetLine(1)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextDatafile (const char* text) {

	right_pad_top->GetLine(2)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextRun (const char* text) {

	right_pad_top->GetLine(3)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextAnalysis (const char* text) {

	right_pad_top->GetLine(4)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextEntries (const char* text) {

	right_pad_top->GetLine(5)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::AddTextLimits (const char* text) {

	right_pad_top->GetLine(6)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::SetTitle (const char* text) {
	
	title_pad->GetLine(0)->SetText(0.0, 0.0, text);

}

void CFEBTestCanvas::SetXTitle (const char* text) {
	
	left_histo[5]->GetXaxis()->SetTitle(text);

}

void CFEBTestCanvas::SetYTitle (const char* text) {
	
	left_histo_background->GetYaxis()->SetTitle(text);
	right_histo->GetXaxis()->SetTitle(text);

}

void CFEBTestCanvas::SetLimits (Double_t lowLowLimit,Double_t lowLimit,Double_t highLimit,Double_t highHighLimit) {
	
	LowLimit = lowLimit;
	LowLowLimit = lowLowLimit;
	HighLimit = highLimit;
	HighHighLimit = highHighLimit;
	
	LowLine->SetY1(LowLimit);
	LowLine->SetY2(LowLimit);
	LowLowLine->SetY1(LowLowLimit);
	LowLowLine->SetY2(LowLowLimit);
	HighLine->SetY1(HighLimit);
	HighLine->SetY2(HighLimit);
	HighHighLine->SetY1(HighHighLimit);
	HighHighLine->SetY2(HighHighLimit);
	
	rightLowLine->SetX1(LowLimit);
	rightLowLine->SetX2(LowLimit);
	rightLowLowLine->SetX1(LowLowLimit);
	rightLowLowLine->SetX2(LowLowLimit);
	rightHighLine->SetX1(HighLimit);
	rightHighLine->SetX2(HighLimit);
	rightHighHighLine->SetX1(HighHighLimit);
	rightHighHighLine->SetX2(HighHighLimit);
}

bool CFEBTestCanvas::Fill (CFEBData data) {
	Double_t x[NBINS], y[NBINS];
	bool AllPointsWithinLimits = true;
	int nOutOfLimits = 0;
	for(int Nlayer = 0; Nlayer < data.Nlayers; Nlayer++) {
		nGreenSolid[Nlayer] = 0;
		nYellowSolid[Nlayer] = 0;
		nRedSolid[Nlayer] = 0;
		nRedSolidTriangleUp[Nlayer] = 0;
		nRedSolidTriangleDown[Nlayer] = 0;
		
		for(int Nbin = 0; Nbin < data.Nbins; Nbin++) {
			x[Nbin] = Nbin;
			y[Nbin] = data.content[Nlayer][Nbin];
			
			right_histo->Fill(y[Nbin]);
			
			if(y[Nbin] <= HighLimit && y[Nbin] >= LowLimit) {
				grGreenSolid[Nlayer]->SetPoint(nGreenSolid[Nlayer], x[Nbin], y[Nbin]);
				nGreenSolid[Nlayer] = nGreenSolid[Nlayer] + 1;
			}
			if((y[Nbin] > HighLimit && y[Nbin] <= HighHighLimit) || (y[Nbin] >= LowLowLimit && y[Nbin] < LowLimit)) {
				grYellowSolid[Nlayer]->SetPoint(nYellowSolid[Nlayer], x[Nbin], y[Nbin]);
				nYellowSolid[Nlayer] = nYellowSolid[Nlayer] + 1;
				AllPointsWithinLimits = false;
			}
			if((y[Nbin] > HighHighLimit && y[Nbin] <= MaxValue) || (y[Nbin] >= MinValue && y[Nbin] < LowLowLimit)) {
				grRedSolid[Nlayer]->SetPoint(nRedSolid[Nlayer], x[Nbin], y[Nbin]);
				nRedSolid[Nlayer] = nRedSolid[Nlayer] + 1;
				nOutOfLimits = nOutOfLimits + 1;
				AllPointsWithinLimits = false;
			}
			if(y[Nbin] > MaxValue) {
				grRedSolidTriangleUp[Nlayer]->SetPoint(nRedSolidTriangleUp[Nlayer], x[Nbin], MaxValue);
				nRedSolidTriangleUp[Nlayer] = nRedSolidTriangleUp[Nlayer] + 1;
				nOutOfLimits = nOutOfLimits + 1;
				AllPointsWithinLimits = false;
			}
			if(y[Nbin] < MinValue) {
				grRedSolidTriangleDown[Nlayer]->SetPoint(nRedSolidTriangleDown[Nlayer], x[Nbin], MinValue);
				nRedSolidTriangleDown[Nlayer] = nRedSolidTriangleDown[Nlayer] + 1;
				nOutOfLimits = nOutOfLimits + 1;
				AllPointsWithinLimits = false;
			}
		}
	}
	
	char cEntries[100], cMean[100], cRMS[100];
	sprintf(cEntries,"Entries = %.0f",right_histo->GetEntries());
	sprintf(cMean,"Mean = %f",right_histo->GetMean(1));
	sprintf(cRMS,"RMS = %f",right_histo->GetRMS(1));
	right_histo_ptstats->AddText(cEntries);
	right_histo_ptstats->AddText(cMean);
	right_histo_ptstats->AddText(cRMS);
	
	sprintf(cEntries,"Number of entries: %.0f",right_histo->GetEntries());
	char cOutOfLimits[100];
	sprintf(cOutOfLimits,"Out of limits: %d",nOutOfLimits);
	right_pad_top->GetLine(5)->SetText(0.0, 0.0, cEntries);
	right_pad_top->GetLine(6)->SetText(0.0, 0.0, cOutOfLimits);
	
	rightLowLine->SetY1(right_histo->GetMinimum());
	rightLowLine->SetY2(right_histo->GetMaximum());
	rightLowLowLine->SetY1(right_histo->GetMinimum());
	rightLowLowLine->SetY2(right_histo->GetMaximum());
	rightHighLine->SetY1(right_histo->GetMinimum());
	rightHighLine->SetY2(right_histo->GetMaximum());
	rightHighHighLine->SetY1(right_histo->GetMinimum());
	rightHighHighLine->SetY2(right_histo->GetMaximum());
	
	if(AllPointsWithinLimits) {
		cnv->SetFillColor(8);	// Green
	} else {
		if (nOutOfLimits>0) cnv->SetFillColor(2); // Red 
		else cnv->SetFillColor(5);
	}
	
	return AllPointsWithinLimits;
}

void CFEBTestCanvas::Draw (void) {
	cnv->cd();
	title_pad->Draw();
	
	cnv->cd();
	right_pad_top->Draw();
	
	cnv->cd();
	right_pad_bottom->Draw();
	right_pad_bottom->cd();
	right_histo->Draw();
	rightLowLine->Draw();
	rightLowLowLine->Draw();
	rightHighLine->Draw();
	rightHighHighLine->Draw();
	right_histo_ptstats->Draw();
	
	cnv->cd();
	left_pad_background->Draw();
	left_pad_background->cd();
	left_histo_background->Draw();
	
	for(int Nlayer = NLAYERS - 1; Nlayer >= 0; Nlayer--) {
		cnv->cd();
		left_pad[Nlayer]->Draw();
		left_pad[Nlayer]->cd();
		left_histo[Nlayer]->Draw();
		LowLine->Draw();
		LowLowLine->Draw();
		HighLine->Draw();
		HighHighLine->Draw();
		if(nGreenSolid[Nlayer] > 0) grGreenSolid[Nlayer]->Draw("P");
		if(nYellowSolid[Nlayer] > 0) grYellowSolid[Nlayer]->Draw("P");
		if(nRedSolid[Nlayer] > 0) grRedSolid[Nlayer]->Draw("P");
		if(nRedSolidTriangleUp[Nlayer] > 0) grRedSolidTriangleUp[Nlayer]->Draw("P");
		if(nRedSolidTriangleDown[Nlayer] > 0) grRedSolidTriangleDown[Nlayer]->Draw("P");
	}
}

void CFEBTestCanvas::SaveAs (std::string name) {
	cnv->SaveAs((name +".png").c_str());
//	cnv->SaveAs((name+".ps").c_str());
}

void CFEBTestCanvas::Write () {
	if (cnv) cnv->Write();
}


#endif
