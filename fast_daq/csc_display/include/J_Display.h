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
        TGPopupMenu *menuConfigure, *menuChamber, *menuRun;
        TGDockableFrame *menuDock;
	//TGNumberEntry *alctChamber, *clctChamber;
	TGNumberEntry *alctChamber, *clctChamber, *selEvent; //v
// for display_wires_strips
        TGCompositeFrame *titleFrame, *wiresFrame;
        TGCompositeFrame *midFrame;
        TRootEmbeddedCanvas *titlecan, *lmidcan, *hmidcan, *rmidcan, *wirescan;
// for display_sca
        TGCompositeFrame *topFrame, *scaMidFrame, *lowFrame; 
        TRootEmbeddedCanvas *topcan, *lscamidcan, *graphcan, *keycan, *lowcan;
//for display_ctrig
        TGCompositeFrame *ctrigtopFrame, *ctrigmidFrame, *ctriglowFrame;
        TRootEmbeddedCanvas *ctrigtopcan, *ctriglowcan, *ctlmidcan, *midcan;
//tool bar
	TGToolBar *toolBar;
	TGPictureButton *play, *pause, *rewind, *print;
	TGHSlider *evtSlider; //v
        TGLabel *lblTotalEvents, *lblOpenFile; //v
  
  public:
	void layout();
        void normal_layout();
	void handle_menu(int id);
        void handle_run_menu(int);
	void handle_tb(int id);
	void handle_num();
        void label_display_cc();
        void plot_wires_alct_time_cc();
//        void plot_atrig_wires_cc();
        void atrig_wire_geom_cc();
//        void atrig_get_polyline_cc(float chx[300], float chy[300], int nch, float xmid, float ymid, float dx, float dy);
        void atrig_boxit_cc(float y, float x, float dy, float dx, TBox* box);
        void display_atrig_cc();
        void wire_geom_cc();
        void get_polyline_cc(float chx[300], float chy[300], int nch, float xmid, float ymid, float dx, float dy);
        void j_plot_atrig_wires();
        void display_wires_strips_cc();
        void j_plot_wires();
        void j_plot_strips();
        void scale_y();
        void wires_strips_layout();
        void wires_strips_label();
        void display_sca();
        void plot_samples();
        void scale_sca_y();
        void sca_label();
        void sca_layout();
        void display_ctrig();
        void getsoft();
        void plot_ctrig_strips();
        void ctrig_scale_y();
        void ctrig_label();
        void ctrig_layout();
        void display_clct_time();
        void plot_cath_clct_time();
	//void select_data_file();
	void request_event(int);
	void request_event_num();
	void show_menus();
	void show_status_bars();
	void update_status_bars();
	void pause_run();
	void Print();
	void cbShowEvent_m();
	void CloseWindow();

	ClassDef(J_Display,0)
};

