#include"TestCanvas_6gr1h.h"

TestCanvas_6gr1h::TestCanvas_6gr1h (std::string name, std::string title, Int_t Nbinsx, Double_t xlow, Double_t xup, Int_t Nbinsy, Double_t ylow, Double_t yup)
{

  theName  = name.c_str();
  theTitle = title.c_str();

// Set graphs ranges
  theNbinsx = Nbinsx;
  theXlow   = xlow;
  theXup    = xup;

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
// Lines to show performance range on left graphs
  theLowLine = new TLine(theXlow,theLowLimit,theXup,theLowLimit);
  theLowLine->SetLineColor(theColorGreenLight);
  theLowLine->SetLineWidth(theLineWidth);
  theLowLine->SetLineStyle(theLineStyle);
  theHighLine = new TLine(theXlow,theHighLimit,theXup,theHighLimit);
  theHighLine->SetLineColor(theColorGreenLight);
  theHighLine->SetLineWidth(theLineWidth);
  theHighLine->SetLineStyle(theLineStyle);
  theLowLowLine = new TLine(theXlow,theLowLowLimit,theXup,theLowLowLimit);
  theLowLowLine->SetLineColor(theColorRedLight);
  theLowLowLine->SetLineWidth(theLineWidth);
  theLowLowLine->SetLineStyle(theLineStyle);
  theHighHighLine = new TLine(theXlow,theHighHighLimit,theXup,theHighHighLimit);
  theHighHighLine->SetLineColor(theColorRedLight);
  theHighHighLine->SetLineWidth(theLineWidth);
  theHighHighLine->SetLineStyle(theLineStyle);
// Lines to show performance range on right histogram
  theRightLowLine = new TLine(theLowLimit,0.0,theLowLimit,100.0);
  theRightLowLine->SetLineColor(theColorGreenLight);
  theRightLowLine->SetLineWidth(theLineWidth);
  theRightLowLine->SetLineStyle(theLineStyle);
  theRightHighLine = new TLine(theHighLimit,0.0,theHighLimit,100.0);
  theRightHighLine->SetLineColor(theColorGreenLight);
  theRightHighLine->SetLineWidth(theLineWidth);
  theRightHighLine->SetLineStyle(theLineStyle);
  theRightLowLowLine = new TLine(theLowLowLimit,0.0,theLowLowLimit,100.0);
  theRightLowLowLine->SetLineColor(theColorRedLight);
  theRightLowLowLine->SetLineWidth(theLineWidth);
  theRightLowLowLine->SetLineStyle(theLineStyle);
  theRightHighHighLine = new TLine(theHighHighLimit,0.0,theHighHighLimit,100.0);
  theRightHighHighLine->SetLineColor(theColorRedLight);
  theRightHighHighLine->SetLineWidth(theLineWidth);
  theRightHighHighLine->SetLineStyle(theLineStyle);

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
  double fY1  = 0.05;
  double fY2  = 0.93;
  double fYd  = 0.001;
  double fX0  = 0.01;
  double fX1  = 0.03;
  double fX2  = 0.591;

  theLeftPadBackground = new TPad("theLeftPadBackground","theLeftPadBackground",fX0,fY0,fX2,fY2 + fYd);
  theLeftPadBackground->SetFillColor(theColorWhite);
  theLeftPadBackground->SetBorderSize(0);
  theLeftPadBackground->SetRightMargin(0.0);
  theLeftPadBackground->SetLeftMargin((fX1-fX0+0.07*(fX2-fX1))/(fX2-fX0));
  theLeftPadBackground->SetTopMargin(0.2);
  theLeftPadBackground->SetBottomMargin(0.2);

  int fIndexLeftPad;
  std::string fTitleLeftPad;
  for (fIndexLeftPad = 0; fIndexLeftPad < NLAYERS; fIndexLeftPad++)
    {
      fTitleLeftPad = Form("Pad_Layer_%d", fIndexLeftPad + 1);
      theLeftPad[fIndexLeftPad] = new TPad(fTitleLeftPad.c_str(), fTitleLeftPad.c_str(),
                                           fX1, fY1+(NLAYERS - fIndexLeftPad - 1)*(fY2-fY1)/NLAYERS,
                                           fX2, fY1+(NLAYERS - fIndexLeftPad)*(fY2-fY1)/NLAYERS+fYd);
      theLeftPad[fIndexLeftPad]->SetFillColor(theColorWhite);
      theLeftPad[fIndexLeftPad]->SetBorderSize(0);
      theLeftPad[fIndexLeftPad]->SetRightMargin(0.0);
      theLeftPad[fIndexLeftPad]->SetLeftMargin(0.07);
      theLeftPad[fIndexLeftPad]->SetTopMargin(0.05);
      theLeftPad[fIndexLeftPad]->SetBottomMargin(0.0);
    }

  theRightBottomPad = new TPad("theRightBottomPad","theRightBottomPad",.59,.01,.99,.611);
  theRightBottomPad->SetFillColor(theColorWhite);
  theRightBottomPad->SetBorderSize(0);

  theRightTopPad = new TPaveText(.59,.610,.99,fY2+fYd,"brNDC");
  theRightTopPad->SetBorderSize(0);
  theRightTopPad->SetFillColor(theColorWhite);
  theRightTopPad->SetTextAlign(12);
  theRightTopPad->SetTextSize(0.02);
  theRightTopPad->SetTextFont(fTextFont);
  theRightTopPad->AddText("Test:");
  theRightTopPad->AddText("Result:");
  theRightTopPad->AddText("Datafile:");
  theRightTopPad->AddText("Run:");
  theRightTopPad->AddText("Analysis:");
  theRightTopPad->AddText("Number of entries:");
  theRightTopPad->AddText("Out of limits:");

  theRightHisto = new TH1F((name+"_theRightHistogram").c_str(), "theRightHistogram", theNbinsy, theYlow, theYup);
  theRightHisto->SetFillColor(theColorGray);
  theRightHisto->GetXaxis()->CenterTitle(true);
  theRightHisto->GetXaxis()->SetTitleFont(fTextFont);
  theRightHisto->GetXaxis()->SetTitle("Title Y");
  theRightHisto->GetYaxis()->SetTitleFont(fTextFont);
  theRightHisto->GetYaxis()->SetTitle("Entries");

  thePtstatsRightHisto = new TPaveStats(0.7,0.75,0.9,0.9,"brNDC");
  thePtstatsRightHisto->SetName("stats");
  thePtstatsRightHisto->SetBorderSize(2);
  thePtstatsRightHisto->SetFillStyle(0);
  thePtstatsRightHisto->SetTextAlign(12);
  thePtstatsRightHisto->SetOptStat(111110);
  thePtstatsRightHisto->SetOptFit(0);

  theLeftHistoBackground = new TH2F((name+"_background").c_str(), "background", theNbinsx, theXlow, theXup, theNbinsy, theYlow, theYup);
  theLeftHistoBackground->GetXaxis()->SetTitle("Title X");
  theLeftHistoBackground->GetXaxis()->SetTitleFont(fTextFont);
  theLeftHistoBackground->GetXaxis()->CenterTitle(true);
  theLeftHistoBackground->GetXaxis()->SetTitleSize(0.03);
  theLeftHistoBackground->GetXaxis()->SetTitleOffset(3.90);
  theLeftHistoBackground->GetXaxis()->SetLabelSize(0.02);
  theLeftHistoBackground->GetXaxis()->SetLabelOffset(0.16);
  theLeftHistoBackground->GetXaxis()->SetNdivisions(516);
  theLeftHistoBackground->GetYaxis()->SetTitle("Title Y");
  theLeftHistoBackground->GetYaxis()->SetTitleFont(fTextFont);
  theLeftHistoBackground->GetYaxis()->CenterTitle(true);
  theLeftHistoBackground->GetYaxis()->SetTitleSize(0.03);
  theLeftHistoBackground->GetYaxis()->SetTitleOffset(1.85);
  theLeftHistoBackground->GetYaxis()->SetLabelSize(0.0);

  int fIndexLeftHisto;
  std::string fTitleLeftHisto;
  for (fIndexLeftHisto = 0; fIndexLeftHisto < NLAYERS; fIndexLeftHisto++)
    {
      fTitleLeftHisto = Form("Layer_%d", fIndexLeftHisto + 1);
      theLeftHisto[fIndexLeftHisto] = new TH2F((name+"_"+fTitleLeftHisto).c_str(), fTitleLeftHisto.c_str(), theNbinsx, theXlow, theXup, theNbinsy, theYlow, theYup);

      theLeftHisto[fIndexLeftHisto]->GetXaxis()->CenterTitle(true);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetTitle("Title X");
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetTitleFont(fTextFont);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetTitleSize(0.0);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetTitleOffset(0.7);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetNdivisions(516);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetLabelSize(0.0);
      theLeftHisto[fIndexLeftHisto]->GetXaxis()->SetTickLength(0.04);

      theLeftHisto[fIndexLeftHisto]->GetYaxis()->CenterTitle(true);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetTitle(fTitleLeftHisto.c_str());
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetTitleFont(fTextFont);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetTitleSize(0.16);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetTitleOffset(0.20);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetNdivisions(410);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetLabelSize(0.1);
      theLeftHisto[fIndexLeftHisto]->GetYaxis()->SetTickLength(0.02);
    }


  double fMarkerSize = 0.5; ///< Marker size
  int   fSolidCircleMarkerStyle       = 20; ///< Solid circle marker
  int   fEmptyCircleMarkerStyle       = 4;  ///< Empty circle marker
  int   fSolidTriangleUpMarkerStyle   = 22; ///< Solid triangle up marker
  int   fSolidTriangleDownMarkerStyle = 23; ///< Solid triangle down marker
  int   fEmptyTriangleUpMarkerStyle   = 26; ///< Empty triangle up marker (Empty triangle down not implemented)

  for (int fNlayer = 0; fNlayer < NLAYERS; fNlayer++)
    {
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

TestCanvas_6gr1h::~TestCanvas_6gr1h ()
{
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
  delete theRightHisto;
  delete theLeftHistoBackground;
  delete theLeftHisto[0];
  delete theLeftHisto[1];
  delete theLeftHisto[2];
  delete theLeftHisto[3];
  delete theLeftHisto[4];
  delete theLeftHisto[5];
}

const char*  TestCanvas_6gr1h::GetTitle (void)
{

  return theTitle;

}

const char*  TestCanvas_6gr1h::GetName (void)
{

  return theName;

}

void TestCanvas_6gr1h::SetTitle (std::string text)
{

  theTitle = text.c_str();
  theTitlePad->GetLine(0)->SetText(0.0, 0.0, theTitle);

}

void TestCanvas_6gr1h::SetXTitle (std::string text)
{

  theLeftHistoBackground->GetXaxis()->SetTitle(text.c_str());

}

void TestCanvas_6gr1h::SetYTitle (std::string text)
{

  theLeftHistoBackground->GetYaxis()->SetTitle(text.c_str());
  theRightHisto->GetXaxis()->SetTitle(text.c_str());

}

void TestCanvas_6gr1h::AddTextTest(std::string text)
{

  std::string fTest ("Test: ");
  theRightTopPad->GetLine(0)->SetText(0.0, 0.0, (fTest + text).c_str());

}

void TestCanvas_6gr1h::AddTextResult(std::string text)
{

  std::string fResult ("Result: ");
  theRightTopPad->GetLine(1)->SetText(0.0, 0.0, (fResult + text).c_str());

}

void TestCanvas_6gr1h::AddTextDatafile (std::string text)
{

  std::string fDatafile ("Datafile: ");
  theRightTopPad->GetLine(2)->SetText(0.0, 0.0, (fDatafile + text).c_str());

}

void TestCanvas_6gr1h::AddTextRun (std::string text)
{

  std::string fRun ("Run: ");
  theRightTopPad->GetLine(3)->SetText(0.0, 0.0, (fRun + text).c_str());

}

void TestCanvas_6gr1h::AddTextAnalysis (std::string text)
{

  std::string fAnalysis ("Analysis: ");
  theRightTopPad->GetLine(4)->SetText(0.0, 0.0,(fAnalysis + text).c_str());

}

void TestCanvas_6gr1h::AddTextEntries (std::string text)
{

  std::string fEntries ("Number of entries: ");
  theRightTopPad->GetLine(5)->SetText(0.0, 0.0, (fEntries + text).c_str());

}

void TestCanvas_6gr1h::AddTextLimits (std::string text)
{

  std::string fOutOfLimits ("Out of limits: ");
  theRightTopPad->GetLine(6)->SetText(0.0, 0.0, (fOutOfLimits + text).c_str());

}

void TestCanvas_6gr1h::SetLimits (Double_t lowLowLimit,Double_t lowLimit,Double_t highLimit,Double_t highHighLimit)
{

  theLowLimit = lowLimit;
  theLowLowLimit = lowLowLimit;
  theHighLimit = highLimit;
  theHighHighLimit = highHighLimit;

  theLowLine->SetY1(theLowLimit);
  theLowLine->SetY2(theLowLimit);
  theLowLowLine->SetY1(theLowLowLimit);
  theLowLowLine->SetY2(theLowLowLimit);
  theHighLine->SetY1(theHighLimit);
  theHighLine->SetY2(theHighLimit);
  theHighHighLine->SetY1(theHighHighLimit);
  theHighHighLine->SetY2(theHighHighLimit);

  theRightLowLine->SetX1(theLowLimit);
  theRightLowLine->SetX2(theLowLimit);
  theRightLowLowLine->SetX1(theLowLowLimit);
  theRightLowLowLine->SetX2(theLowLowLimit);
  theRightHighLine->SetX1(theHighLimit);
  theRightHighLine->SetX2(theHighLimit);
  theRightHighHighLine->SetX1(theHighHighLimit);
  theRightHighHighLine->SetX2(theHighHighLimit);
}

std::vector<Double_t> TestCanvas_6gr1h::GetLimits()
{
  std::vector<Double_t>limits;
  limits.push_back(theLowLowLimit);
  limits.push_back(theLowLimit);
  limits.push_back(theHighLimit);
  limits.push_back(theHighHighLimit);
  return limits;
}


int TestCanvas_6gr1h::Fill (TestData2D& data, TestData2D& mask)
{
  Double_t fX[NBINS], fY[NBINS];
  int fQualityTest = 1;
  int fNOutOfLimits = 0;

  bool fIsRedSolid    = false;
  bool fIsYellowSolid = false;
  bool fIsEmpty  = false;

  int fNlayer;
  int fNbin;
  for (fNlayer = 0; fNlayer < data.Nlayers; fNlayer++)
    {
      for (fNbin = 0; fNbin < data.Nbins; fNbin++)
        {
          fX[fNbin] = fNbin;
          fY[fNbin] = data.content[fNlayer][fNbin];

          // if (fY[fNbin] == -999.) continue;

          theRightHisto->Fill(fY[fNbin]);

          if (fY[fNbin] <= theHighLimit && fY[fNbin] >= theLowLimit)
            {
              if (mask.content[fNlayer][fNbin] == 0)
                {
                  theGraphGreenSolid[fNlayer]->SetPoint(theGraphGreenSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }
              else
                {
                  if (!fIsEmpty) fIsEmpty = true;
                  theGraphGreenEmpty[fNlayer]->SetPoint(theGraphGreenEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }

            }
          if ((fY[fNbin] > theHighLimit && fY[fNbin] <= theHighHighLimit) || (fY[fNbin] >= theLowLowLimit && fY[fNbin] < theLowLimit))
            {
              if (mask.content[fNlayer][fNbin] == 0)
                {
                  if (!fIsYellowSolid) fIsYellowSolid = true;
                  theGraphYellowSolid[fNlayer]->SetPoint(theGraphYellowSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }
              else
                {
                  if (!fIsEmpty) fIsEmpty = true;
                  theGraphYellowEmpty[fNlayer]->SetPoint(theGraphYellowEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }
            }
          if ((fY[fNbin] > theHighHighLimit && fY[fNbin] <= theYup) || (fY[fNbin] >= theYlow && fY[fNbin] < theLowLowLimit))
            {

              fNOutOfLimits = fNOutOfLimits + 1;
              if (mask.content[fNlayer][fNbin] == 0)
                {
                  if (!fIsRedSolid) fIsRedSolid = true;
                  theGraphRedSolid[fNlayer]->SetPoint(theGraphRedSolid[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }
              else
                {
                  if (!fIsEmpty) fIsEmpty = true;
                  theGraphRedEmpty[fNlayer]->SetPoint(theGraphRedEmpty[fNlayer]->GetN(), fX[fNbin], fY[fNbin]);
                }
            }
          if (fY[fNbin] > theYup)
            {

              fNOutOfLimits = fNOutOfLimits + 1;
              if (mask.content[fNlayer][fNbin] == 0)
                {
                  if (!fIsRedSolid) fIsRedSolid = true;
                  theGraphRedSolidTriangleUp[fNlayer]->SetPoint(theGraphRedSolidTriangleUp[fNlayer]->GetN(), fX[fNbin], theYup);
                }
              else
                {
                  if (!fIsEmpty) fIsEmpty = true;
                  theGraphRedEmptyTriangleUp[fNlayer]->SetPoint(theGraphRedEmptyTriangleUp[fNlayer]->GetN(), fX[fNbin], theYup);
                }
            }
          if (fY[fNbin] < theYlow)
            {

              fNOutOfLimits = fNOutOfLimits + 1;
              if (mask.content[fNlayer][fNbin] == 0)
                {
                  if (!fIsRedSolid) fIsRedSolid = true;
                  theGraphRedSolidTriangleDown[fNlayer]->SetPoint(theGraphRedSolidTriangleDown[fNlayer]->GetN(), fX[fNbin], theYlow);
                }
              else
                {
                  if (!fIsEmpty) fIsEmpty = true;
                  theGraphRedEmptyTriangleDown[fNlayer]->SetPoint(theGraphRedEmptyTriangleDown[fNlayer]->GetN(), fX[fNbin], theYlow);
                }
            }
        }
    }

// Add information (total number of entries and number of enries out of limits) to TextPad
  std::string fsEntries = Form("%.0f",theRightHisto->GetEntries());
  AddTextEntries(fsEntries);
  std::string fsOutOfLimits = Form("%d",fNOutOfLimits);
  AddTextLimits(fsOutOfLimits);

// Add information (Entries, Mean, RMS, Underflow, Overflow) to StatPad of right histogram
  fsEntries = Form("Entries = %.0f",theRightHisto->GetEntries());
  thePtstatsRightHisto->AddText(fsEntries.c_str());
  std::string fsMean = Form("Mean = %f",theRightHisto->GetMean(1));
  thePtstatsRightHisto->AddText(fsMean.c_str());
  std::string fsRMS = Form("RMS = %f",theRightHisto->GetRMS(1));
  thePtstatsRightHisto->AddText(fsRMS.c_str());
  std::string fsUnderflow = Form("Underflow = %.0f",theRightHisto->GetBinContent(0));
  thePtstatsRightHisto->AddText(fsUnderflow.c_str());
  std::string fsOverflow = Form("Overflow = %.0f",theRightHisto->GetBinContent(theNbinsy+1));
  thePtstatsRightHisto->AddText(fsOverflow.c_str());

  theRightLowLine->SetY1(theRightHisto->GetMinimum());
  theRightLowLine->SetY2(theRightHisto->GetMaximum());
  theRightLowLowLine->SetY1(theRightHisto->GetMinimum());
  theRightLowLowLine->SetY2(theRightHisto->GetMaximum());
  theRightHighLine->SetY1(theRightHisto->GetMinimum());
  theRightHighLine->SetY2(theRightHisto->GetMaximum());
  theRightHighHighLine->SetY1(theRightHisto->GetMinimum());
  theRightHighHighLine->SetY2(theRightHisto->GetMaximum());

  int theFillColor = theColorWhite;
  if (!fIsYellowSolid && !fIsRedSolid && !fIsEmpty)
    {
      theFillColor = theColorGreenLight;
      fQualityTest = 1;
    }
  if (fIsYellowSolid)
    {
      theFillColor = theColorYellowLight;
      fQualityTest = 2;
    }
  if (!fIsRedSolid && !fIsYellowSolid && fIsEmpty)
    {
      theFillColor = theColorBlueLight;
      fQualityTest = 3;
    }
  if (fIsRedSolid)
    {
      theFillColor = theColorRedLight;
      fQualityTest = 4;
    }

  theMainCanvas->SetFillColor(theFillColor);

  return fQualityTest;
}

void TestCanvas_6gr1h::Draw (void)
{

// Switch off all default titles and statistics boxes
  theStyle = new TStyle("Style", "Style");
  theStyle->SetOptStat(0);
  theStyle->SetOptTitle(0);
//	theStyle->SetCanvasColor(theFillColor);
//	theStyle->SetFillColor(theColorWhite);
  theStyle->cd();
// 	theMainCanvas->UseCurrentStyle();

  theMainCanvas->cd();
  theTitlePad->Draw();

  theMainCanvas->cd();
  theRightTopPad->Draw();

  theMainCanvas->cd();
  theRightBottomPad->Draw();
  theRightBottomPad->cd();
  theRightHisto->Draw();
  theRightLowLine->Draw();
  theRightLowLowLine->Draw();
  theRightHighLine->Draw();
  theRightHighHighLine->Draw();
  thePtstatsRightHisto->Draw();

  theMainCanvas->cd();
  theLeftPadBackground->Draw();
  theLeftPadBackground->cd();
  theLeftHistoBackground->Draw();

  int fNlayer;
  for (fNlayer = NLAYERS - 1; fNlayer >= 0; fNlayer--)
    {
      theMainCanvas->cd();
      theLeftPad[fNlayer]->Draw();
      theLeftPad[fNlayer]->cd();
      theLeftHisto[fNlayer]->Draw();

      theLowLine->Draw();
      theLowLowLine->Draw();
      theHighLine->Draw();
      theHighHighLine->Draw();

      if (theGraphGreenSolid[fNlayer]->GetN()           > 0) theGraphGreenSolid[fNlayer]->Draw("P");
      if (theGraphYellowSolid[fNlayer]->GetN()          > 0) theGraphYellowSolid[fNlayer]->Draw("P");
      if (theGraphRedSolid[fNlayer]->GetN()             > 0) theGraphRedSolid[fNlayer]->Draw("P");
      if (theGraphRedSolidTriangleUp[fNlayer]->GetN()   > 0) theGraphRedSolidTriangleUp[fNlayer]->Draw("P");
      if (theGraphRedSolidTriangleDown[fNlayer]->GetN() > 0) theGraphRedSolidTriangleDown[fNlayer]->Draw("P");

      if (theGraphGreenEmpty[fNlayer]->GetN()           > 0) theGraphGreenEmpty[fNlayer]->Draw("P");
      if (theGraphYellowEmpty[fNlayer]->GetN()          > 0) theGraphYellowEmpty[fNlayer]->Draw("P");
      if (theGraphRedEmpty[fNlayer]->GetN()             > 0) theGraphRedEmpty[fNlayer]->Draw("P");
      if (theGraphRedEmptyTriangleUp[fNlayer]->GetN()   > 0) theGraphRedEmptyTriangleUp[fNlayer]->Draw("P");
      if (theGraphRedEmptyTriangleDown[fNlayer]->GetN() > 0) theGraphRedEmptyTriangleDown[fNlayer]->Draw("P");
    }

  theMainCanvas->Update();
}

void TestCanvas_6gr1h::SetCanvasSize(uint32_t w, uint32_t h)
{
  if (theMainCanvas) theMainCanvas->SetCanvasSize(w,h);
}

void TestCanvas_6gr1h::SaveAs (std::string file_name)
{
  if (theMainCanvas) theMainCanvas->SaveAs(file_name.c_str());
}

int TestCanvas_6gr1h::Write (std::string newName)
{
  int fNBuffer = 0;
  if (theMainCanvas)
    {
      fNBuffer = theMainCanvas->Write(newName.c_str(),TObject::kOverwrite);
    }
//	theStyle->Write();
  return fNBuffer;
}

int TestCanvas_6gr1h::Write (void)
{
  int fNBuffer = 0;
  if (theMainCanvas)
    {
      fNBuffer = theMainCanvas->Write(theMainCanvas->GetName(),TObject::kOverwrite);
    }
//	theStyle->Write();
  return fNBuffer;
}

TH1* TestCanvas_6gr1h::GetHisto (void)
{

  return theRightHisto;
}
