/*
 * =====================================================================================
 *
 *       Filename:  ChamberMap.cc
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

ChamberMap::ChamberMap()
{
  bBlank = new TBox(1.0, 0.0, 37, 18);
  bBlank->SetFillColor(0);
  bBlank->SetLineColor(1);
  bBlank->SetLineStyle(1);
}

ChamberMap::~ChamberMap()
{
  delete bBlank;
}

// Transform chamber ID to local canvas coordinates
float ChamberMap::Xmin_local_derived_from_ChamberID(const int side, const int station, const int ring, const int chamber)
{
  float x;

  if ((station == 2 || station == 3 || station == 4) && ring == 1)
    {
      x = (float)((chamber-1)*2);
    }
  else
    {
      x = (float)(chamber-1);
    }
  return x;
}

// Transform chamber ID to local canvas coordinates
float ChamberMap::Xmax_local_derived_from_ChamberID(int side, int station, int ring, int chamber)
{
  float x;

  if ((station == 2 || station == 3 || station == 4) && ring == 1)
    {
      x = (float)((chamber)*2);
    }
  else
    {
      x = (float)(chamber);
    }

  return x;
}

// Transform chamber ID to local canvas coordinates
float ChamberMap::Ymin_local_derived_from_ChamberID(int side, int station, int ring, int chamber)
{
  float y = 0;
  float offset = 0.0;

  if (side == 2)   // "-" side
    {
      offset = 0.0;
      if (station == 4 && ring == 2) y = offset + 0.0;
      if (station == 4 && ring == 1) y = offset + 1.0;
      if (station == 3 && ring == 2) y = offset + 2.0;
      if (station == 3 && ring == 1) y = offset + 3.0;
      if (station == 2 && ring == 2) y = offset + 4.0;
      if (station == 2 && ring == 1) y = offset + 5.0;
      if (station == 1 && ring == 3) y = offset + 6.0;
      if (station == 1 && ring == 2) y = offset + 7.0;
      if (station == 1 && ring == 1) y = offset + 8.0;
    }

  if (side == 1)  // "+" side
    {
      offset = 9.0;
      if (station == 1 && ring == 1) y = offset + 0.0;
      if (station == 1 && ring == 2) y = offset + 1.0;
      if (station == 1 && ring == 3) y = offset + 2.0;
      if (station == 2 && ring == 1) y = offset + 3.0;
      if (station == 2 && ring == 2) y = offset + 4.0;
      if (station == 3 && ring == 1) y = offset + 5.0;
      if (station == 3 && ring == 2) y = offset + 6.0;
      if (station == 4 && ring == 1) y = offset + 7.0;
      if (station == 4 && ring == 2) y = offset + 8.0;
    }

  return y;
}

// Transform chamber ID to local canvas coordinates
float ChamberMap::Ymax_local_derived_from_ChamberID(int side, int station, int ring, int chamber)
{
  float y = 0;
  float offset = 0.0;

  if (side == 2)   // "-" side
    {
      offset = 1.0;
      if (station == 4 && ring == 2) y = offset + 0.0;
      if (station == 4 && ring == 1) y = offset + 1.0;
      if (station == 3 && ring == 2) y = offset + 2.0;
      if (station == 3 && ring == 1) y = offset + 3.0;
      if (station == 2 && ring == 2) y = offset + 4.0;
      if (station == 2 && ring == 1) y = offset + 5.0;
      if (station == 1 && ring == 3) y = offset + 6.0;
      if (station == 1 && ring == 2) y = offset + 7.0;
      if (station == 1 && ring == 1) y = offset + 8.0;
    }

  if (side == 1)  // "+" side
    {
      offset = 10.0;
      if (station == 1 && ring == 1) y = offset + 0.0;
      if (station == 1 && ring == 2) y = offset + 1.0;
      if (station == 1 && ring == 3) y = offset + 2.0;
      if (station == 2 && ring == 1) y = offset + 3.0;
      if (station == 2 && ring == 2) y = offset + 4.0;
      if (station == 3 && ring == 1) y = offset + 5.0;
      if (station == 3 && ring == 2) y = offset + 6.0;
      if (station == 4 && ring == 1) y = offset + 7.0;
      if (station == 4 && ring == 2) y = offset + 8.0;
    }

  return y;
}

// Ring number
int ChamberMap::N_ring(int station)
{
  int n_ring = 0;
  if (station == 1) n_ring = 3;
  if (station == 2) n_ring = 2;
  if (station == 3) n_ring = 2;
  if (station == 4) n_ring = 1;
  return n_ring;
}

// Chamber number
int ChamberMap::N_chamber(int station, int ring)
{
  int n_chambers;
  if (station == 1) n_chambers = 36;
  else
    {
      if (ring == 1) n_chambers = 18;
      else n_chambers = 36;
    }
  return n_chambers;
}

void ChamberMap::draw(TH2*& me)
{

  gStyle->SetPalette(1,0);

  /** VR: Moved this up and made float */
  float HistoMaxValue = me->GetMaximum();
  float HistoMinValue = me->GetMinimum();

  /** Cosmetics... */
  me->GetXaxis()->SetTitle("Chamber");
  me->GetXaxis()->CenterTitle(true);
  me->GetXaxis()->SetLabelSize(0.0);
  me->GetXaxis()->SetTicks("0");
  me->GetXaxis()->SetNdivisions(0);
  me->GetXaxis()->SetTickLength(0.0);

  me->Draw("colz");

  bBlank->Draw("l");

  TBox *b[3][5][4][37];
  TText *tCSC_label[3][5][4][37];

  /** VR: Making it floats and moving up */
  float x_min_chamber, x_max_chamber;
  float y_min_chamber, y_max_chamber;
  float BinContent = 0;
  int fillColor = 0;

  for (int n_side = 1; n_side <= 2; n_side++)
    {
      for (int station = 1; station <= 4; station++)
        {
          for (int n_ring = 1; n_ring <= N_ring(station); n_ring++)
            {
              for (int n_chamber = 1; n_chamber <= N_chamber(station, n_ring); n_chamber++)
                {

                  x_min_chamber = Xmin_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                  x_max_chamber = Xmax_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                  y_min_chamber = Ymin_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                  y_max_chamber = Ymax_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);

                  BinContent = 0;
                  fillColor = 0;

                  /** VR: if the station/ring is an exceptional one (less chambers) we should
                   * correct x coordinates of source. Casts are just to avoid warnings :) */
                  if (station > 1 && n_ring == 1)
                    {
                      BinContent = (float) me->GetBinContent((int) x_max_chamber / 2, (int) y_max_chamber);
                    }
                  else
                    {
                      BinContent = (float) me->GetBinContent((int) x_max_chamber, (int) y_max_chamber);
                    }

                  if (BinContent != 0)
                    {

                      /** VR: color calculation differs for linear and log10 scales though... */
                      if (gPad->GetLogz() == 1)
                        {
                          fillColor = 51 + (int) ((( log10(BinContent) - log10(HistoMaxValue) + 3 ) / 3 ) * 49.0 );
                        }
                      else
                        {
                          fillColor = 51 + (int)(((BinContent - HistoMinValue) / (HistoMaxValue - HistoMinValue)) * 49.0);
                        }

                      /** VR: just to be sure :) */
                      if (fillColor > 100)
                        {
                          fillColor = 100;
                        }
                      if (fillColor < 51 )
                        {
                          fillColor = 51;
                        }
                    }

                  b[n_side][station][n_ring][n_chamber] = new TBox(x_min_chamber + 1, y_min_chamber, x_max_chamber + 1, y_max_chamber);
                  b[n_side][station][n_ring][n_chamber]->SetFillColor(fillColor);
                  b[n_side][station][n_ring][n_chamber]->SetLineColor(1);
                  b[n_side][station][n_ring][n_chamber]->SetLineStyle(2);
                  b[n_side][station][n_ring][n_chamber]->Draw("l");

                  TString ChamberID = Form("%d", n_chamber);
                  tCSC_label[n_side][station][n_ring][n_chamber] = new TText((x_min_chamber + x_max_chamber)/2.0 + 1, (y_min_chamber + y_max_chamber)/2.0, ChamberID);
                  tCSC_label[n_side][station][n_ring][n_chamber]->SetTextAlign(22);
                  tCSC_label[n_side][station][n_ring][n_chamber]->SetTextFont(42);
                  tCSC_label[n_side][station][n_ring][n_chamber]->SetTextSize(0.015);
                  tCSC_label[n_side][station][n_ring][n_chamber]->Draw();
                }
            }
        }
    }

}


