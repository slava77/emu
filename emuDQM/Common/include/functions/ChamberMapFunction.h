/*
 * =====================================================================================
 *
 *       Filename:  ChamberMapFunction.h
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

#ifndef ChamberMapFunction_h
#define ChamberMapFunction_h

#include "AbstractFunction.h"

class ChamberMapFunction: public AbstractFunction {

  public:

    MonitorElement* calc(FunctionParameters& vme){ 

        MonitorElement* me = dynamic_cast<MonitorElement*>(vme.at(0)->Clone()); 
	
        gStyle->SetPalette(1,0);

        /*
        TH2F *h1 = new TH2F("h1", "CSCs reporting data", 36, 0.0, 36.0, 18, 0.0, 18.0);
        
        // X-axis
        h1->GetXaxis()->SetTitle("Chamber");
        h1->GetXaxis()->CenterTitle(true);
        h1->GetXaxis()->SetLabelSize(0.0);
        h1->GetXaxis()->SetTicks("+-");
        h1->GetXaxis()->SetNdivisions(36);
        h1->GetXaxis()->SetTickLength(0.01);
        
        // Y-axis
        h1->GetYaxis()->SetTitle("Station/Ring");
        h1->GetYaxis()->SetTitleOffset(-1.5);
        h1->GetYaxis()->SetTicks("+-");
        h1->GetYaxis()->CenterTitle(true);
        h1->GetYaxis()->SetLabelSize(0.03);
        h1->GetYaxis()->SetTickLength(0.01);
        h1->GetYaxis()->SetBinLabel( 1,"ME-4/2");
        h1->GetYaxis()->SetBinLabel( 2,"ME-4/1");
        h1->GetYaxis()->SetBinLabel( 3,"ME-3/2");
        h1->GetYaxis()->SetBinLabel( 4,"ME-3/1");
        h1->GetYaxis()->SetBinLabel( 5,"ME-2/2");
        h1->GetYaxis()->SetBinLabel( 6,"ME-2/1");
        h1->GetYaxis()->SetBinLabel( 7,"ME-1/3");
        h1->GetYaxis()->SetBinLabel( 8,"ME-1/2");
        h1->GetYaxis()->SetBinLabel( 9,"ME-1/1");
        h1->GetYaxis()->SetBinLabel(10,"ME+1/1");
        h1->GetYaxis()->SetBinLabel(11,"ME+1/2");
        h1->GetYaxis()->SetBinLabel(12,"ME+1/3");
        h1->GetYaxis()->SetBinLabel(13,"ME+2/1");
        h1->GetYaxis()->SetBinLabel(14,"ME+2/2");
        h1->GetYaxis()->SetBinLabel(15,"ME+3/1");
        h1->GetYaxis()->SetBinLabel(16,"ME+3/2");
        h1->GetYaxis()->SetBinLabel(17,"ME+4/1");
        h1->GetYaxis()->SetBinLabel(18,"ME+4/2");
        h1->SetStats(kFALSE);
        */

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

	TBox *bBlank = new TBox(1.0, 0.0, 37, 18);
	bBlank->SetFillColor(0);
	bBlank->SetLineColor(1);
	bBlank->SetLineStyle(1);
        bBlank->Draw("l");
	
	TBox *b[3][5][4][37];
	TText *tCSC_label[3][5][4][37];

        /** VR: Making it floats and moving up */
        float x_min_chamber, x_max_chamber;
        float y_min_chamber, y_max_chamber;
        float BinContent = 0;
        int fillColor = 0; 

	for(int n_side = 1; n_side <= 2; n_side++) {
          for(int station = 1; station <= 4; station++) {
            for(int n_ring = 1; n_ring <= N_ring(station); n_ring++) {
              for(int n_chamber = 1; n_chamber <= N_chamber(station, n_ring); n_chamber++) {
				
                x_min_chamber = Xmin_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                x_max_chamber = Xmax_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                y_min_chamber = Ymin_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
                y_max_chamber = Ymax_local_derived_from_ChamberID(n_side, station, n_ring, n_chamber);
				
                BinContent = 0;
                fillColor = 0;

                /** VR: if the station/ring is an exceptional one (less chambers) we should
                 * correct x coordinates of source. Casts are just to avoid warnings :) */
                if(station > 1 && n_ring == 1) {
                  BinContent = (float) me->GetBinContent((int) x_max_chamber / 2, (int) y_max_chamber);
                } else {
                  BinContent = (float) me->GetBinContent((int) x_max_chamber, (int) y_max_chamber);
                }

                if(BinContent != 0) {

                  /** VR: color calculation differs for linear and log10 scales though... */
                  if(gPad->GetLogz() == 1) {
                    fillColor = 51 + (int) ((( log10(BinContent) - log10(HistoMaxValue) + 3 ) / 3 ) * 49.0 );
                  } else {
                    fillColor = 51 + (int)(((BinContent - HistoMinValue) / (HistoMaxValue - HistoMinValue)) * 49.0);
                  }

                  /** VR: just to be sure :) */
                  if(fillColor > 100){ fillColor = 100; }
                  if(fillColor < 51 ){ fillColor = 51;  }
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
                tCSC_label[n_side][station][n_ring][n_chamber]->SetTextSize(0.02);
                tCSC_label[n_side][station][n_ring][n_chamber]->Draw();
              }
            }
          }
	}
        
        return NULL;

      };

    private:

    // Transform chamber ID to local canvas coordinates
    float Xmin_local_derived_from_ChamberID(const int side, const int station, const int ring, const int chamber) {
      float x;
	
      if((station == 2 || station == 3 || station == 4) && ring == 1) {
        x = (float)((chamber-1)*2);
      } else {
        x = (float)(chamber-1);
      }
      return x;
    };

    // Transform chamber ID to local canvas coordinates
    float Xmax_local_derived_from_ChamberID(int side, int station, int ring, int chamber) {
      float x;
	
      if((station == 2 || station == 3 || station == 4) && ring == 1) {
        x = (float)((chamber)*2);
      } else {
        x = (float)(chamber);
      }
	
      return x;
    };

    // Transform chamber ID to local canvas coordinates
    float Ymin_local_derived_from_ChamberID(int side, int station, int ring, int chamber) {
      float y;
      float offset = 0.0;
	
      if(side == 2) { // "-" side
        offset = 0.0;
        if(station == 4 && ring == 2) y = offset + 0.0;
        if(station == 4 && ring == 1) y = offset + 1.0;
        if(station == 3 && ring == 2) y = offset + 2.0;
        if(station == 3 && ring == 1) y = offset + 3.0;
        if(station == 2 && ring == 2) y = offset + 4.0;
        if(station == 2 && ring == 1) y = offset + 5.0;
        if(station == 1 && ring == 3) y = offset + 6.0;
        if(station == 1 && ring == 2) y = offset + 7.0;
        if(station == 1 && ring == 1) y = offset + 8.0;
      }

      if(side == 1) {// "+" side
        offset = 9.0;
        if(station == 1 && ring == 1) y = offset + 0.0;
        if(station == 1 && ring == 2) y = offset + 1.0;
        if(station == 1 && ring == 3) y = offset + 2.0;
        if(station == 2 && ring == 1) y = offset + 3.0;
        if(station == 2 && ring == 2) y = offset + 4.0;
        if(station == 3 && ring == 1) y = offset + 5.0;
        if(station == 3 && ring == 2) y = offset + 6.0;
        if(station == 4 && ring == 1) y = offset + 7.0;
        if(station == 4 && ring == 2) y = offset + 8.0;
      }

      return y;
    };

    // Transform chamber ID to local canvas coordinates
    float Ymax_local_derived_from_ChamberID(int side, int station, int ring, int chamber) {
      float y;
      float offset = 0.0;

      if(side == 2) { // "-" side
        offset = 1.0;
        if(station == 4 && ring == 2) y = offset + 0.0;
        if(station == 4 && ring == 1) y = offset + 1.0;
        if(station == 3 && ring == 2) y = offset + 2.0;
        if(station == 3 && ring == 1) y = offset + 3.0;
        if(station == 2 && ring == 2) y = offset + 4.0;
        if(station == 2 && ring == 1) y = offset + 5.0;
        if(station == 1 && ring == 3) y = offset + 6.0;
        if(station == 1 && ring == 2) y = offset + 7.0;
        if(station == 1 && ring == 1) y = offset + 8.0;
      }

      if(side == 1) {// "+" side
        offset = 10.0;
        if(station == 1 && ring == 1) y = offset + 0.0;
        if(station == 1 && ring == 2) y = offset + 1.0;
        if(station == 1 && ring == 3) y = offset + 2.0;
        if(station == 2 && ring == 1) y = offset + 3.0;
        if(station == 2 && ring == 2) y = offset + 4.0;
        if(station == 3 && ring == 1) y = offset + 5.0;
        if(station == 3 && ring == 2) y = offset + 6.0;
        if(station == 4 && ring == 1) y = offset + 7.0;
        if(station == 4 && ring == 2) y = offset + 8.0;
      }

      return y;
    };

    // Ring number
    int N_ring(int station) {
      int n_ring;
      if(station == 1) n_ring = 3;
      if(station == 2) n_ring = 2;
      if(station == 3) n_ring = 2;
      if(station == 4) n_ring = 1;
      return n_ring;
    };

    // Chamber number
    int N_chamber(int station, int ring) {
      int n_chambers;
      if(station == 1) n_chambers = 36;
      else {
        if(ring == 1) n_chambers = 18;
        else n_chambers = 36;
      }
      return n_chambers;
    };
      
};

#endif

