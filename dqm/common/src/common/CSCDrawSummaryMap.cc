/*
 * =====================================================================================
 *
 *       Filename:  CSCRenderPlugin_SummaryMap.cc
 *
 *    Description:  Makes a real CSC map out of the dummy histogram. Actually it streches ME(+|-)2/1,
 *    ME(+|-)3/1, ME(+|-)4/1 chambers to the full extent of the diagram. Initial algorithm implementation
 *    was dome by YP and the port to DQM was done by VR.
 *
 *        Version:  1.0
 *        Created:  04/09/2008 04:57:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuriy Pakhotin (YP), pakhotin@ufl.edu; Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#include "emu/dqm/common/CSCDrawMaps.h"
#include <TLine.h>

#define N_TICS    100

SummaryMap::SummaryMap()
{

  h1 = new TH2F("h1", "", 10, -2.5, 2.5, 10, 0.0, 2.0*3.14159);
  h1->GetXaxis()->SetTitle("#eta");
  h1->GetXaxis()->SetTitleOffset(1.2);
  h1->GetXaxis()->CenterTitle(true);
  h1->GetXaxis()->SetLabelSize(0.03);
  h1->GetXaxis()->SetTicks("+-");
  h1->GetXaxis()->SetNdivisions(20510, kTRUE);
  h1->GetYaxis()->SetTitle("#phi");
  h1->GetYaxis()->SetTitleOffset(-1.2);
  h1->GetYaxis()->SetTicks("+-");
  h1->GetYaxis()->SetNdivisions(21010, kTRUE);
  h1->GetYaxis()->CenterTitle(true);
  h1->GetYaxis()->SetLabelSize(0.03);
  h1->SetStats(kFALSE);

  bBlank = new TBox(-2.5, 0.0, 2.5, 2.0*3.14159);
  bBlank->SetFillColor(18);
  bBlank->SetLineColor(1);
  bBlank->SetLineStyle(1);

}

SummaryMap::~SummaryMap()
{
  delete h1;
  delete bBlank;
}

void SummaryMap::drawDetector(TH2* me)
{

  gStyle->SetPalette(1,0);

  h1->Draw();
//  me->Draw();
  bBlank->Draw("l");


  TBox *b[N_TICS][N_TICS];
  TLine *l[N_TICS - 1][2];

  float xd = 5.0 / N_TICS, yd = 1.0 * (2.0 * 3.14159) / N_TICS;

  float xmin, xmax, ymin, ymax;
  unsigned int rep_el = 0, csc_el = 0;

  for (unsigned int x = 0; x < N_TICS; x++)
    {

      xmin = -2.5 + xd * x;
      xmax = xmin + xd;

      if (xmin == -2.5 || xmax == 2.5) continue;
      if (xmin >= -1 && xmax <= 1)     continue;

      for (unsigned int y = 0; y < N_TICS; y++)
        {

          ymin = yd * y;
          ymax = ymin + yd;

          if (me->GetBinContent(x + 1, y + 1) > 0)
            {
              b[x][y] = new TBox(xmin, ymin, xmax, ymax);
              b[x][y]->SetFillColor(8);
              b[x][y]->SetFillStyle(1001);
              b[x][y]->Draw("");
              rep_el++;
            }

          csc_el++;

        }

    }

  for (unsigned int x = 1; x < N_TICS; x++)
    {
      l[x - 1][0] = new TLine(-2.5 + xd * x, 0.0, -2.5 + xd * x, 2.0 * 3.14159);
      l[x - 1][0]->SetLineColor(9);
      l[x - 1][0]->SetLineStyle(1);
      l[x - 1][0]->SetLineWidth(1);
      l[x - 1][0]->Draw();
      l[x - 1][1] = new TLine(-2.5, yd * x, 2.5, yd * x);
      l[x - 1][1]->SetLineColor(9);
      l[x - 1][1]->SetLineStyle(1);
      l[x - 1][1]->SetLineWidth(1);
      l[x - 1][1]->Draw();
    }

  TText *tCanvas_label = new TText(0.0, 2.0 * 3.14159 + 0.5, me->GetTitle());
  tCanvas_label->SetTextAlign(22);
  tCanvas_label->SetTextFont(62);
  tCanvas_label->SetTextSize(0.04);
  tCanvas_label->Draw();

}

void SummaryMap::drawStation(TH2* me, const int station)
{

  CSCAddress adr;

  gStyle->SetPalette(1,0);

//  me->Draw();
  h1->Draw();
  bBlank->Draw("l");

  TBox *b[N_ELEMENTS];
  TLine *l[3456];
  TText *tCSC_label[864];
  TText *tRing_label[6];

  float x_min_chamber = FLT_MAX, x_max_chamber = FLT_MIN;
  float y_min_chamber = FLT_MAX, y_max_chamber = FLT_MIN;

  const CSCAddressBox *box;
  adr.mask.side = adr.mask.ring = adr.mask.chamber  = adr.mask.layer = adr.mask.cfeb = adr.mask.hv = false;
  adr.mask.station = true;
  adr.station = station;

  unsigned int i = 0, p_hw = 0, p_l = 0, p_csc = 0, p_ring = 0;
  while (detector.NextAddressBox(i, box, adr))
    {

      b[p_hw] = new TBox(box->xmin, box->ymin, box->xmax, box->ymax);

      unsigned int x = 1 + (box->adr.side - 1) * 9 + (box->adr.ring - 1) * 3 + (box->adr.hv - 1);
      unsigned int y = 1 + (box->adr.chamber - 1) * 5 + (box->adr.cfeb - 1);

      if (me->GetBinContent(x, y) > 0)
        b[p_hw]->SetFillColor(8);
      else
        b[p_hw]->SetFillColor(18);

      b[p_hw]->SetLineColor(9);
      b[p_hw]->SetLineStyle(1);
      b[p_hw]->Draw("l");
      p_hw++;

      // If this is the last hw element in the chamber - proceed drawing chamber
      if (box->adr.cfeb == detector.NumberOfChamberCFEBs(box->adr.station, box->adr.ring) &&
          box->adr.hv == detector.NumberOfChamberHVs(box->adr.station, box->adr.ring))
        {

          x_max_chamber = box->xmax;
          y_max_chamber = box->ymax;

          l[p_l]     = new TLine(x_min_chamber, y_min_chamber, x_min_chamber, y_max_chamber);
          l[p_l + 1] = new TLine(x_max_chamber, y_min_chamber, x_max_chamber, y_max_chamber);
          l[p_l + 2] = new TLine(x_min_chamber, y_min_chamber, x_max_chamber, y_min_chamber);
          l[p_l + 3] = new TLine(x_min_chamber, y_max_chamber, x_max_chamber, y_max_chamber);
          for (int n_l = 0; n_l < 4; n_l++)
            {
              l[p_l + n_l]->SetLineColor(1);
              l[p_l + n_l]->SetLineStyle(1);
              l[p_l + n_l]->SetLineWidth(1);
              l[p_l + n_l]->Draw();
            }
          p_l += 4;

          TString ChamberID = Form("%d", box->adr.chamber);
          tCSC_label[p_csc] = new TText((x_min_chamber + x_max_chamber) / 2.0, (y_min_chamber + y_max_chamber) / 2.0, ChamberID);
          tCSC_label[p_csc]->SetTextAlign(22);
          tCSC_label[p_csc]->SetTextFont(42);
          tCSC_label[p_csc]->SetTextSize(0.02);
          tCSC_label[p_csc]->Draw();
          p_csc++;

          // Last HW element in Ring? display ring label
          if (box->adr.chamber == detector.NumberOfChambers(box->adr.station, box->adr.ring))
            {
              TString ringID = Form("%d", box->adr.ring);
              tRing_label[p_ring] = new TText((x_min_chamber + x_max_chamber) / 2.0, 2.0 * 3.14159 + 0.1, ringID);
              tRing_label[p_ring]->SetTextAlign(22);
              tRing_label[p_ring]->SetTextFont(62);
              tRing_label[p_ring]->SetTextSize(0.02);
              tRing_label[p_ring]->Draw();
              p_ring++;
            }

        }
      else if (box->adr.cfeb == 1 && box->adr.hv == 1)
        {
          x_min_chamber = box->xmin;
          y_min_chamber = box->ymin;
        }

    }

  TString stationID_minus = Form("ME-%d", station);
  TText *tStation_minus_label = new TText(-1.7, 2.0 * 3.14159 + 0.25, stationID_minus);
  tStation_minus_label->SetTextAlign(22);
  tStation_minus_label->SetTextFont(62);
  tStation_minus_label->SetTextSize(0.02);

  TString stationID_plus = Form("ME+%d", station);
  TText *tStation_plus_label = new TText(1.7, 2.0 * 3.14159 + 0.25, stationID_plus);
  tStation_plus_label->SetTextAlign(22);
  tStation_plus_label->SetTextFont(62);
  tStation_plus_label->SetTextSize(0.02);

  TText *tCanvas_label = new TText(0.0, 2.0 * 3.14159 + 0.5, me->GetTitle());
  tCanvas_label->SetTextAlign(22);
  tCanvas_label->SetTextFont(62);
  tCanvas_label->SetTextSize(0.04);
  tStation_minus_label->Draw();
  tStation_plus_label->Draw();
  tCanvas_label->Draw();

}

