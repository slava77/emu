#ifndef _CSCDrawMaps_h
#define _CSCDrawMaps_h

#include <math.h>
#include <string>
#include <TH1.h>
#include <TH2.h>
#include <TBox.h>
#include <TText.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "DQM/CSCMonitorModule/interface/CSCDetector.h"


/**
 * @class ChamberMap
 * @brief Class that draws CSC Map diagram
 */
class ChamberMap {
  public:
    ChamberMap();
    ~ChamberMap();
    void draw(TH2*& me);
  private:
    float Xmin_local_derived_from_ChamberID(const int side, const int station, const int ring, const int chamber);
    float Xmax_local_derived_from_ChamberID(int side, int station, int ring, int chamber);
    float Ymin_local_derived_from_ChamberID(int side, int station, int ring, int chamber);
    float Ymax_local_derived_from_ChamberID(int side, int station, int ring, int chamber);
    int N_ring(int station);
    int N_chamber(int station, int ring);
    TBox *bBlank;
};

/**
 * @class SummaryMap
 * @brief Class that draws CSC Summary Map diagram
 */
class SummaryMap {
  public:
    SummaryMap();
    ~SummaryMap();
    void drawDetector(TH2* me);
    void drawStation(TH2* me, const int station);
  private:
    CSCDetector detector;
    TH2F *h1;
    TBox *bBlank;
};

#endif
