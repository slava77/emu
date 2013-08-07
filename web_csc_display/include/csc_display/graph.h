#ifndef _CSC_DISPLAY_GRAPH_H_
#define _CSC_DISPLAY_GRAPH_H_

#include <stdio.h>
#include <stdlib.h>
#include "emu/daq/reader/RawDataFile.h"
#include "csc_display/csc_parameters.h"
#include "csc_display/unpacker.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFrame.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TImage.h"
#include "TPaveLabel.h"
#include "TBox.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TLine.h"
#include "TGButton.h"
#include "TText.h"
#include "RQ_OBJECT.h"
#include "TGMenu.h"
#include "TPolyLine.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TGDockableFrame.h"
#include "TGToolBar.h"
#include "TG3DLine.h"
#include "TGNumberEntry.h"
#include "TGFileDialog.h" //v
#include "TGLabel.h" //v
#include "TGSlider.h" //v
#include "TClass.h" //v
#include "TRootHelpDialog.h" // szs
#include "TGFileDialog.h" // szs

void addHeader(TVirtualPad *c, int event_num, int chamberID, char* configStr);

void getWiresAndStripsGraph     (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size, bool peak=false);
void getWiresAndStripsPeakGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);
void getCLCTTimeGraph           (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);
void getALCTTimeGraph           (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);
void getCFEBTimeScaleGraph      (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);
void getCtrigGraph              (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);
void getAtrigGraph              (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size);

// other graphic functions
void wiresStripsLabel(TVirtualPad* c);
void verticalWiresStripsLabel(TVirtualPad* c);
void plotWires(TVirtualPad* c, data_type data, j_common_type j_data);
void plotStrips(TVirtualPad* c, data_type data, j_common_type j_data, TH1F** hist, bool peak);
void scale_y(j_common_type* j_data, bool peak=false);

void plotCFEBTimeScale(TVirtualPad* c, data_type data, j_common_type j_data, TGraph*** graph);
void drawCFEBTimeScaleKeys(TVirtualPad* c);
void drawCFEBTimeScaleHAxis(TVirtualPad* c);
void scale_cfeb_time_sample_y(j_common_type* j_data);

void plotCtrigStrips(TVirtualPad* c, data_type data, j_common_type j_data, TH1F** ctrighist);
void drawCtrigKeys(TVirtualPad* c);
void drawCtrigAxis(TVirtualPad* c);
void getSWHalfStrip(j_common_type j_data, bool softhalfstrip[2][NSTRIP][NLAYER]);
void scale_ctrig_y(j_common_type* j_data);

void plotAtrigWires(TVirtualPad* c, data_type data, j_common_type j_data);
void atrigWireGeomCc(j_common_type* j_data);

// other functions
void getCanvasBuffer(TVirtualPad* c, char** imgBuf, int* size);

// graphical constants
static const double striphist_x1 = 0.1;
static const double striphist_x2 = 0.999;

#endif
