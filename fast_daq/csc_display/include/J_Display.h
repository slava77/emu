#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TCanvas.h"
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

#include "csc_parameters.h"

#ifndef __CINT__  // hide these includes from rootcint
#include <syslog.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
/// -- CMSSW Unpacker --
#include "csc_unpacker.h"
#include "emu/daq/reader/RawDataFile.h"
#include "j_common_data.h"
extern "C"
{
#include "daq_conf.h"
#include "daq_ipc.h"
#include "application.h"
#include "daq_module.h"
#include "csc_event.h"
}
#endif

class J_Display {
        RQ_OBJECT("J_Display");
  private:
        TGMainFrame *fMain;
        //TGCompositeFrame *canFrame, *bFrame, *menuFrame, *histFrame, *dFrame, *chamFrame;
        TGCompositeFrame *canFrame, *bFrame, *menuFrame, *histFrame, *dFrame, *chamFrame, *ctrlFrame, *statusFrame; //v
        TRootEmbeddedCanvas *can1, *can2, *histcan, *nodispcan;
        TPaveLabel *label;
        TCanvas *cmain, *clabel, *chist, *cgraph, *ctrigcan;
        TGTextButton *bWiresStrips, *bAtrig, *bAlctTime, *bClctTime;
        TGMenuBar *menuBar;
        TGPopupMenu *menuConfigure, *menuChambers, *menuRun;
        TGDockableFrame *menuDock;
	//TGNumberEntry *alctChamber, *clctChamber;
	TGNumberEntry *alctChamber, *clctChamber, *selEvent; //v
    TGCheckButton *hackSelect;
	// for display_wires_strips
        TGCompositeFrame *titleFrame, *wiresFrame;
        TGCompositeFrame *midFrame;
        TRootEmbeddedCanvas *titlecan, *lmidcan, *hmidcan, *rmidcan, *wirescan;

	// banner canvas for all displays
        TGCompositeFrame *bannerFrame;
        TRootEmbeddedCanvas *bannercan;
	
//tool bar
	TGToolBar *toolBar;
	TGPictureButton *play, *pause, *rewind, *print;
	TGHSlider *evtSlider; //v
        TGLabel *lblTotalEvents, *lblOpenFile; //v

	//TH1F* ctrighist[NLAYER];
	TH1F* ctrighist[NLAYER];
	TBox* box;
	TLine* line;
	TGaxis* axis;
	TText* text;

  
  public:
	J_Display()
	  {
	    for(int i=0; i<NLAYER; ++i) ctrighist[i]=0;
	    box = new TBox;
	    line = new TLine;
	    axis = new TGaxis;
	    text = new TText;
	  }
	void cbShowEvent_m();
    void rewind_display();
	void layout();
        void normal_layout();
	void handle_menu(int id);
        void handle_chambers_menu(int);
        void handle_run_menu(int);
	void handle_tb(int id);
	void handle_num();
    void handle_hack_mode(bool);
	void request_event(int);
	void request_event_num();
	void update_status_bars();
        void display_wires_alct_time_cc();
        void plot_wires_alct_time_cc();
        void get_polyline_cc(float chx[300], float chy[300], int nch, float xmid, float ymid, float dx, float dy);
        void atrig_wire_geom_cc();
        void atrig_boxit_cc(float y, float x, float dy, float dx, TBox* box);
        void display_atrig_cc();
        void j_plot_atrig_wires();
        void display_wires_strips_cc();
        void display_wires_strips_peaks_cc(); // firman
        void j_plot_wires();
        void j_plot_strips();
        void j_plot_strips_peaks(); // firman
        void scale_y();
        void wires_strips_label();
        void wires_strips_layout();
        void display_sca();
        void plot_samples();
        void scale_sca_y();
        void sca_label();
        void sca_layout();
        void display_ctrig();
        void getsoft();
        void plot_ctrig_strips();
        void ctrig_scale_y();
        void update_banner(string title);
        void ctrig_label();
        void ctrig_layout();
        void display_clct_time();
        void plot_cath_clct_time();
	void Print();
	void CloseWindow();
	void show_menus();
	void show_status_bars();
	void pause_run();

	ClassDef(J_Display,0)
};

