#include<iostream>
#include<string.h>
#include<stdio.h>
#include<libgen.h>

#include "TRootHelpDialog.h" // szs
#include "TGFileDialog.h" // szs

extern "C" {
#include "daq_conf.h"
#include "csc_event.h"
#include "application.h"
}

#include "j_common_data.h"

#ifndef __CINT__
#include "J_Display.h"


using namespace std;

enum CommandIdentifiers
  {

    M_WIRES_STRIPS = 1,
    M_ATRIG,
    M_ALCT_TIME,
    M_CLCT_TIME,
    M_SCA,
    M_CTRIG,
    M_SELECT_FILE,
    M_RUN,
    M_PAUSE,
    M_SELECT_CRATE,
    M_SELECT_CHAMBER
  };

const char *filetypes[] = { "RAW files",    "*.raw",
                            "All files",     "*",
                            0,               0
};

int strips_per_plane, wires_per_plane;
static char buffer[400];
csc_event_type upevt_;
static int alct_chamber = -1;
static int clct_chamber = -1;
static bool firstTime = true;
static bool firstTimeWires = true;
static bool firstTimeSca = true;
static bool firstTimeCtrig = true;
static bool firstTimeN = true;
static bool initial = true;
static int returnValue = 1;
static int activeDisp = 0;
static int init = 1;
static TH1F* hist[NLAYER];
static TH1F* ctrighist[NLAYER];
static TGraph* graph[NLAYER][5];
bool softhalfstrip[2][NSTRIP][NLAYER];
extern int rewind_comm;

J_Display *jd;
TTimer *timer;
extern void cbShowEvent();

void J_Display::cbShowEvent_m()
{
  cbShowEvent();
}

int crate = -1, chamber = -1;

int main(int argc, char **argv)
{
  bool havefile=0;
  if (argc < 2)
    {
      std::cerr <<" Please choose a file from the Select menu " << std::endl;
    }
  
  if ( event_display_global_init(argc, argv) != 0 )
    {
      std::cout << "event_display : Initialization failed\n";
      exit(1);
    }
  
  
  timer = new TTimer();
  
  // theApp modifies argv, so have to save it here
  char filename[500];
  
  if(argc>=2) {
    strcpy(filename, argv[1]); 
    std::cout << "Opening data file: " << argv[1] << std::endl;
    connect_data_file(filename);
    havefile=1;
  }
  
  if (argc > 2)
    {
      crate = strtol(argv[2], NULL, 10);
      chamber = strtol(argv[3], NULL, 10);
      std::cout << "displaying crate: " << crate << " chamber: " << chamber << std::endl;
    }

  TApplication theApp("j_display", &argc, argv);
  jd = new J_Display();
  timer->Connect("Timeout()", "J_Display", jd, "cbShowEvent_m()");

  jd->layout();

  if(havefile){
    // default to wires/strips display
    //jd->handle_menu(M_WIRES_STRIPS);
    jd->handle_menu(M_CTRIG);
  }

  theApp.Run();

  return 0;
}
#endif

void J_Display::layout()
{
  int i;

  //	std::cout << "layout, this = " << std::hex << this << " jd = " << jd << std::dec << std::endl;


  if (initial)
    {
      //		activeDisp = M_WIRES_STRIPS;
      fMain = new TGMainFrame(gClient->GetRoot(), 800, 800);
      fMain->Connect("CloseWindow()", "J_Display", this, "CloseWindow()");
      initial = false;
      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      nodispcan = new TRootEmbeddedCanvas("no displays canvas", fMain, 700, 300);
      fMain->AddFrame(nodispcan, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

      show_status_bars();

    }
  if (firstTime)
    {
      firstTime = false;
      fMain->SetWindowName("CSC Display");
      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();
    }



  switch (activeDisp)
    {
    case M_WIRES_STRIPS:
      this->display_wires_strips_cc();
      break;
    case M_ATRIG:
      this->display_atrig_cc();
      break;
    case M_ALCT_TIME:
      this->plot_wires_alct_time_cc();
      break;
    case M_CLCT_TIME:
      this->display_clct_time();
      break;
    case M_SCA:
      this->display_sca();
      break;
    case M_CTRIG:
      this->display_ctrig();
      break;
    default:
      break;

    }

  update_status_bars();
}

void J_Display::normal_layout()
{



  int i;
  if (firstTimeN)
    {
      firstTimeN = false;
      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      canFrame = new TGCompositeFrame(fMain, 700, 680);
      canFrame->SetLayoutManager(new TGVerticalLayout(canFrame));

      can1 = new TRootEmbeddedCanvas("drawing canvas", canFrame, 700, 605);
      can2 = new TRootEmbeddedCanvas("label canvas", canFrame, 700, 100);

      canFrame->AddFrame(can1, new TGLayoutHints(kLHintsTop|kLHintsExpandX));
      canFrame->AddFrame(can2, new TGLayoutHints(kLHintsTop|kLHintsExpandX));
      fMain->AddFrame(canFrame, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();
      label = new TPaveLabel(.1, .1, .9, .9, "No Active Displays");
      cmain = can1->GetCanvas();
      cmain->cd();
      cmain->Update();
    }
}





void J_Display::handle_menu(int id)
{
  firstTime = true;
  activeDisp = id;
  toolBar->Cleanup();
  fMain->Cleanup();
  firstTimeN = true;
  firstTimeWires = true;
  firstTimeSca = true;
  firstTimeCtrig = true;
  firstTimeN = true;

}

void J_Display::handle_run_menu(int id)
{
  switch (id)
    {
    case M_SELECT_FILE:
      {
        make_status_paused(); 
        std::cout<<" Select a file to Open Please : "<<std::endl;
        
       
	static TString dir(".");
	TGFileInfo fi;
	fi.fFileTypes = filetypes;
	fi.fIniDir    = StrDup(dir);
	//            printf("fIniDir = %s\n", fi.fIniDir);
	new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
	//          printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
	dir = fi.fIniDir;
     
	connect_data_file(fi.fFilename);

      }
      break;
    case M_RUN:
      {
	std::cout << "==> Starting" << std::endl;
	make_status_running();
      }
      break;
    case M_PAUSE:
      {
	std::cout << "==> Pausing" << std::endl;
	make_status_paused();
      }
      break;
    case M_SELECT_CHAMBER:
      cout << "run" << endl;
      returnValue = 0;
      break;     
      
    }
   
   
}

void J_Display::handle_tb(int id)
{
  switch (id)
    {

    case 1:
      make_status_paused();
      break;
    case 2:
      make_status_running();
      break;
    case 3:
      std::cout << "rewind" << endl;
      rewind_comm = 1;
      break;
    case 4:
      make_status_paused(); 
      this->Print();
      break;

    }
}

void J_Display::handle_num()
{

  std::cout << "Num Entry: " << alctChamber->GetIntNumber() << endl;
}


void J_Display::request_event_num()
{

  //  std::cout << "--> Request event: " << selEvent->GetIntNumber() << endl;
  evtSlider->SetPosition(selEvent->GetIntNumber());
  if (selEvent->GetIntNumber() < requested_event)
    {
      reset_data_file();
    }
  requested_event = selEvent->GetIntNumber();
  std::cout << "==> Request event: " << requested_event << endl;
}

void J_Display::request_event(int pos)
{
  // std::cout << pos << std::endl;

  TGFrame *frm = (TGFrame *) gTQSender;
  if (frm->IsA()->InheritsFrom(TGSlider::Class()))
    {
      selEvent->SetNumber(evtSlider->GetPosition());
      if (evtSlider->GetPosition() < requested_event)
        {
          reset_data_file();
        }
      requested_event = evtSlider->GetPosition();
      std::cout << "==> Request event: " << requested_event << endl;
      make_status_running();
    }

}

void J_Display::update_status_bars()
{
  if (event_num > total_events) event_num = total_events;
  lblTotalEvents->SetText(Form("Total Events: %ld", total_events ));
  lblOpenFile->SetText(Form("Opened File: %s", file_name));
  evtSlider->SetRange(1, total_events);
  selEvent->SetLimits(TGNumberFormat::kNELLimitMinMax, 1, total_events);
  if ((event_num >= requested_event)  && !disp_paused)
    {
      // std::cout << "=> Updating " << event_num << std::endl;
      evtSlider->SetPosition(event_num);
      selEvent->SetNumber(event_num);

    }
}


void J_Display::plot_wires_alct_time_cc()
{
  int alct_time_tmp;
  float x_coord[2], y_coord[2];
  float x_shift, x_step, y_shift, y_step, y_time_step;
  int color, i, j, ij;
  int wires_per_plane;

  this->normal_layout();
  sprintf(buffer, "");
  this->label_display_cc();
  TLine *line = new TLine();
  TText *text = new TText(0, 0, buffer);
  x_shift = .01;
  x_step = .008;
  y_shift = .98;
  y_step = .004;
  y_time_step = (6.5)*y_step;

  cmain->cd();
  cmain->Clear();
  line->SetLineWidth(3);
  text->SetTextSize(.020);
  text->SetTextFont(102);

  if (upevt_.chamber_type_id == 11)
    {
      wires_per_plane = 48;
      x_step*=2;
    }
  else if (upevt_.chamber_type_id == 12)
    {
      wires_per_plane = 64;
      x_step*=1.5;
    }
  else if (upevt_.chamber_type_id == 13)
    {
      wires_per_plane = 32;
      x_step*=3;
    }
  else if (upevt_.chamber_type_id == 21)
    {
      wires_per_plane = 112;
    }
  else if (upevt_.chamber_type_id == 22)
    {
      wires_per_plane = 64;
      x_step*=1.5;
    }
  else if (upevt_.chamber_type_id == 31)
    {
      wires_per_plane = 96;
    }
  else if (upevt_.chamber_type_id == 41)
    {
      wires_per_plane = 96;
    }
  else
    {
      wires_per_plane = 64;
      x_step*=1.5;
    }

  line->SetLineColor(1);
  for (j=1; j<=NLAYER; j++)
    {
      x_coord[0] = x_shift + x_step;
      x_coord[1] = x_shift + (wires_per_plane+1)*x_step;
      y_coord[0] = y_shift - 0*y_time_step - j*y_step;
      y_coord[1] = y_coord[0];
      line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
      if (j==(NLAYER/2+2))
        {
          sprintf(buffer, "OR");
          text->DrawText(x_coord[1]+.01, y_coord[0], buffer);
        }
    }

  line->SetLineColor(2);
  for (j=1; j<=NLAYER; j++)
    {
      for (ij=1; ij<=wires_per_plane; ij++)
        {
          alct_time_tmp = upevt_.alct_dump[j-1][ij-1];
          if (alct_time_tmp != 0)
            {
              x_coord[0] = x_shift + ij*x_step;
              x_coord[1] = x_shift + (ij+1)*x_step;
              y_coord[0] = y_shift - 0*y_time_step - j*y_step;
              y_coord[1] = y_coord[0];
              line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
            }
        }
    }

  color = 1;
  std::cout<<"alct_nbucket: "<<upevt_.alct_nbucket<<endl;
  for (i=1; i<=upevt_.alct_nbucket; i++)
    {
      if (color==1)
        {
          line->SetLineColor(4);
          color = 0;
        }
      else
        {
          line->SetLineColor(3);
          color = 1;
        }
      sprintf(buffer, "%d", i);
      for (j=1; j<=NLAYER; j++)
        {
          x_coord[0] = x_shift + x_step;
          x_coord[1] = x_shift + (wires_per_plane+1)*x_step;
          y_coord[0] = y_shift - i*y_time_step - j*y_step;
          y_coord[1] = y_coord[0];
          line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
          if (j==(NLAYER/2+2))
            {
              text->SetTextSize(.02);
              text->DrawText(x_coord[1]+.01, y_coord[0], buffer);
            }
          if ((j==NLAYER&&i==upevt_.alct_nbucket))
            {
              text->SetTextSize(.02);
              sprintf(buffer, "%d", 1);
              text->DrawText(x_coord[0]-.005, y_coord[0]-.02, buffer);
              sprintf(buffer, "%d", wires_per_plane);
              text->DrawText(x_coord[1]-.005, y_coord[0]-.02, buffer);
            }
        }
    }
  line->SetLineColor(2);
  for (i=1; i<=upevt_.alct_nbucket; i++)
    {
      for (j=1; j<=NLAYER; j++)
        {
          for (ij=1; ij<=wires_per_plane; ij++)
            {
              alct_time_tmp = upevt_.alct_dump[j-1][ij-1];
              if ((1&(alct_time_tmp)>>(i-1))==1)
                {
                  x_coord[0] = x_shift + ij*x_step;
                  x_coord[1] = x_shift + (ij+1)*x_step;
                  y_coord[0] = y_shift - i*y_time_step - j*y_step;
                  y_coord[1] = y_coord[0];
                  line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
                }
            }
        }
    }
  cmain->Update();

  delete text;
  delete line;
  return;
}

void J_Display::label_display_cc()
{
  clabel = can2->GetCanvas();
  clabel->cd();
  sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d", upevt_.run_number, upevt_.event_number, upevt_.alct_csc_id, upevt_.clct_csc_id);
  label->SetLabel(buffer);
  label->Draw();
  clabel->Update();
  return;
}

void J_Display::wire_geom_cc()
{
  int i;
  float dy_even, dy_odd, xmid, ymid_index;
  /*	j_data.x0 = .1; //4.93
    dy_even = .04; //.2
    dy_odd = .16; //.8
  */
  if (upevt_.chamber_type_id == 11)
    {
      wires_per_plane = 48;
      strips_per_plane = 80;
    }
  else if (upevt_.chamber_type_id == 12)
    {
      wires_per_plane = 64;
      strips_per_plane = 80;
    }
  else if (upevt_.chamber_type_id == 13)
    {
      wires_per_plane = 32;
      strips_per_plane = 64;
    }
  else if (upevt_.chamber_type_id == 21)
    {
      wires_per_plane = 112;
      strips_per_plane = 80;
    }
  else if (upevt_.chamber_type_id == 22)
    {
      wires_per_plane = 64;
      strips_per_plane = 80;
    }
  else if (upevt_.chamber_type_id == 31)
    {
      wires_per_plane = 96;
      strips_per_plane = 80;
    }
  else if (upevt_.chamber_type_id == 41)
    {
      wires_per_plane = 96;
      strips_per_plane = 80;
    }
  else
    {
      wires_per_plane = 64;
      strips_per_plane = 80;
    }
  /*
    j_data.dx = .024*(48.0/wires_per_plane);

    for(i=1; i<=11; i+=2) {
    j_data.nch[i-1] = wires_per_plane;
    }
    for(i=2; i<=10; i+=2) {
    j_data.nch[i-1] = wires_per_plane;
    }
    ymid_index = 0 +.5*dy_odd;
    for(i=1; i<=11; i++) {
    j_data.ymid[i-1] = ymid_index;
    ymid_index = ymid_index+.5*(dy_even+dy_odd);
    }
    xmid = j_data.x0;
    for(i=1; i<=11; i++) {
    j_data.dy = dy_odd;
    if(i%2 == 0) j_data.dy = dy_even;
    this->get_polyline_cc(j_data.chx[i-1], j_data.chy[i-1], j_data.nch[i-1], xmid, j_data.ymid[i-1], j_data.dx, j_data.dy);
    }*/
  return;
}

void J_Display::get_polyline_cc(float chx[300], float chy[300], int nch, float xmid, float ymid, float dx, float dy)
{
  int i,j;

  float xl, xr, yt, yb, x, y;
  yb = ymid-.5*dy;
  yt=yb+dy;
  if (nch > 0)
    {
      xr = xmid+.5*nch*dx;
      xl = xr-nch*dx;
    }
  else
    {
      xr = xmid-.5*nch*dx;
      xl = xr+nch*dx;
    }
  chx[0] = xl;
  chy[0] = yt;
  chx[1] = xl;
  chy[1] = yb;
  chx[2] = xr;
  chy[2] = yb;
  chx[3] = xr;
  chy[3] = yt;
  chx[4] = xl;
  chy[4] = yt;
  x=xl;
  y=yt;
  if (nch<0&&nch>-20) return;
  if (nch<0)
    {
      j=6;
      for (i=1; i<=(-2)*nch; i+=16)
        {
          chx[j-1] = x;
          chy[j-1] = y;
          j++;
          if (y==yt)
            {
              y=yb;
            }
          else
            {
              y=yt;
            }
          chx[j-1] = x;
          chy[j-1] = y;
          j++;
          x = x+8*dx;
        }
    }
  else
    {
      for (i=1; i<=(2*nch); i+=2)
        {
          j=i+5;
          chx[j-1] = x;
          chy[j-1] = y;
          j++;
          x += dx;
          chx[j-1] = x;
          chy[j-1] = y;
          if (y==yt)
            {
              y=yb;
            }
          else
            {
              y=yt;
            }
        }
    }
  return;
}

void J_Display::atrig_wire_geom_cc()
{
  int i;
  float dy_even, dy_odd, xmid, ymid_index;
  j_data.x0 = 1;
  dy_even = .0025;
  dy_odd = .0141;
  j_data.dx = .10*(48.0/wires_per_plane);
  for (i=1; i<=11; i+=2)
    {
      j_data.nch[i-1] = wires_per_plane;
    }
  for (i=2; i<=10; i+=2)
    {
      j_data.nch[i-1] = -wires_per_plane;
    }
  ymid_index = 0.0 + .5*dy_odd;
  for (i=1; i<=11; i++)
    {
      j_data.ymid[i-1] = ymid_index;
      ymid_index = ymid_index +.5*(dy_even+dy_odd);
    }
  xmid = j_data.x0;
  /*for(i=1; i<=11; i++) {
    j_data.dy = dy_odd;
    if(i%2 == 0) {
    j_data.dy = dy_even;
    }
    this->atrig_get_polyline_cc(j_data.chx[i-1], j_data.chy[i-1], j_data.nch[i-1], xmid, j_data.ymid[i-1], j_data.dx, j_data.dy);
    }

    cmain->Update();*/
  return;
}

void J_Display::atrig_boxit_cc(float y, float x, float dy, float dx, TBox* box)
{
  float ax[2], ay[2];
  ax[0] = x; //-.5*dx;
  ay[0] = y; //-.5*dy;
  ax[1] = ax[0] + dx;
  ay[1] = ay[0] + dy;

  cmain->cd();
  box->DrawBox(ax[0], ay[0], ax[1], ay[1]);
  cmain->Update();

  return;
}

void J_Display::display_atrig_cc()
{
  this->normal_layout();
  this->wire_geom_cc();
  this->atrig_wire_geom_cc();
  this->label_display_cc();
  this->j_plot_atrig_wires();
  return;
}

void J_Display::j_plot_atrig_wires()
{
  int i, j, ilayer, igroup;
  double dx, dy, x0, y0, x1, y1, x2, y2, why, ex, xk;
  TBox *box = new TBox;
  sprintf(buffer, " ");
  TText *text = new TText(0, 0, buffer);
  TLine *line = new TLine;

  cmain->cd();
  cmain->Clear();

  x0 = .20;
  y0 = .05;
  dx = .075;
  dy = .0075;

  x1 = x0;
  y1 = y0;
  x2 = x0 + dx;
  y2 = y0 + dy;
  box->SetFillStyle(1001);
  for (ilayer=1; ilayer<=6; ilayer++)
    {
      j = 2*ilayer - 1;
      for (igroup=1; igroup<=j_data.nch[j-1]; igroup++)
        {
          if (j_data.wires[igroup-1][ilayer-1]!=0)
            {
              ex = x1;
              box->SetFillColor(4);
              box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
          if ((bool)j_data.wiregroup[igroup-1][ilayer-1])
            {
              ex = x1 + .25*dx;
              box->SetFillColor(3);
              box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
          if ((bool)j_data.alct[0][igroup-1][ilayer-1])
            {
              ex = x1 + .5*dx;
              box->SetFillColor(6);
              box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
          if ((bool)j_data.alct[1][igroup-1][ilayer-1])
            {
              ex = x1 + .5*dx;
              box->SetFillStyle(3002);
              box->SetFillColor(6);
              box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
              box->SetFillStyle(1001);
            }
          if ((bool)j_data.tmba[igroup-1][ilayer-1])
            {
              ex = x1 + .75*dx;
              box->SetFillColor(2);
              box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
          y1 += dy;
        }
      x1 = x1 + dx + .01;
      y1 = y0;
    }

  box->SetFillStyle(0);
  box->SetFillColor(1);

  x1 = x0;
  y1 = y0;
  x2 = x0 + dx;
  y2 = y0 + dy;

  text->SetTextFont(42);

  text->SetTextSize(.019);
  sprintf(buffer, "Wire Group 1");
  text->DrawText(x0-1.6*dx, y0, buffer);

  sprintf(buffer, "Wire Group %d", wires_per_plane);
  text->DrawText(x0-1.6*dx, y0 + (wires_per_plane-1)*dy, buffer);

  line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);

  for (i=1; i<=NLAYER; i++)
    {
      for (j=1; j<=wires_per_plane; j++)
        {
          box->DrawBox(x1, y1, x2, y2);
          y1 += dy;
          y2 += dy;

          if (i==1&&(j==wires_per_plane||j%10==0))
            {
              line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);
            }

        }

      if (i==1 || i==NLAYER)
        {
          text->SetTextSize(.025);
          sprintf(buffer, "Layer %d", i);
          text->DrawText(x1, y1+.008, buffer);
        }

      x1 = x1 + dx + .01;
      x2 = x1 + dx;
      y1 = y0;
      y2 = y0 + dy;
    }

  text->SetTextSize(.03);
  text->DrawText(.03, .53, "ANODE");
  text->DrawText(.03, .5, "TRIGGER");
  text->DrawText(.03, .47, "DISPLAY");

  //KEY
  text->SetTextAlign(22);
  xk = x0+(NLAYER+1)*(dx+.01);
  dy*=2;

  text->SetTextSize(.019);
  text->DrawText(xk+.5*dx, .65, "KEY");

  box->SetFillStyle(1001);
  box->SetFillColor(4);
  box->DrawBox(xk, .60, xk+.25*dx, .60+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, .60, xk+dx, .60+dy);
  text->SetTextColor(4);
  text->DrawText(xk+.5*dx, .58, "The wire had");
  text->DrawText(xk+.5*dx, .56, "a TDC hit");

  box->SetFillStyle(1001);
  box->SetFillColor(3);
  box->DrawBox(xk+.25*dx, .50, xk+.5*dx, .50+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, .50, xk+dx, .50+dy);
  text->SetTextColor(3);
  text->DrawText(xk+.5*dx, .48, "The wire appears");
  text->DrawText(xk+.5*dx, .46, "in the ALCT");
  text->DrawText(xk+.5*dx, .44, "raw hits data");

  box->SetFillStyle(1001);
  box->SetFillColor(6);
  box->DrawBox(xk+.5*dx, .40, xk+.75*dx, .40+dy);
  box->SetFillStyle(3002);
  box->DrawBox(xk+.5*dx, .37, xk+.75*dx, .37+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, .40, xk+dx, .40+dy);
  box->DrawBox(xk, .37, xk+dx, .37+dy);
  text->SetTextColor(6);
  text->DrawText(xk+.5*dx, .35, "The best and 2nd");
  text->DrawText(xk+.5*dx, .33, "best LCT pattern");
  text->DrawText(xk+.5*dx, .31, "matched in this event");

  box->SetFillStyle(1001);
  box->SetFillColor(2);
  box->DrawBox(xk+.75*dx, .25, xk+dx, .25+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, .25, xk+dx, .25+dy);
  text->SetTextColor(2);
  text->DrawText(xk+.5*dx, .23, "The wire was part of");
  text->DrawText(xk+.5*dx, .21, "the TMB choice");

  text->SetTextColor(1);
  text->SetTextAlign(32);
  sprintf(buffer, "ALCT0 quality %d", j_data.alct_q[0]);
  text->DrawText(.95, .80, buffer);
  sprintf(buffer, "ALCT1 quality %d", j_data.alct_q[1]);
  text->DrawText(.95, .78, buffer);

  cmain->Update();

  delete text;
  delete box;
  return;
}

void J_Display::display_wires_strips_cc()
{
  this->wires_strips_layout();
  this->wire_geom_cc();
  cmain = wirescan->GetCanvas();
  this->j_plot_wires();
  this->j_plot_strips();
  this->wires_strips_label();
}

void J_Display::j_plot_wires()
{
  int i, j, ilayer, igroup, npts;
  double x0, y0, dx, dy, x1, x2, y1, y2;
  double xmid, ex, why, jay, ch;
  int wghv32[4] = { 12, 22, 22, 22 };
  int wghv64[4] = { 16, 28, 40, 52 };
  int wghv96[4] = { 32, 64, 64, 64 };
  int wghv112[4] = { 44, 80, 80, 80 };
  double x[2], y[2];
  TLine *line = new TLine;
  TText *text = new TText(0, 0, buffer);
  TBox *box = new TBox;

  cmain->cd();
  cmain->Clear();

  box->SetFillStyle(0);
  box->SetLineColor(4);
  line->SetLineColor(4);

  text->SetTextSize(.06);
  text->SetTextAlign(23);
  text->SetTextFont(82);
  text->DrawText(.58, .95, "Strip Number");
  text->SetTextSize(.09);
  text->SetTextFont(102);
  text->DrawText(.5, .75, "Wires");

  x0 = .10;
  y0 = .15;
  dx = .80/(wires_per_plane);
  dy = .50/NLAYER;

  x1 = x0;
  y1 = y0;
  x2 = x1 + dx;
  y2 = y1 + dy;

  line->DrawLine(x1, y0, x1, y0 + NLAYER*(dy+.007)-.007);

  for (i=1; i<=NLAYER; i++)
    {
      for (j=1; j<=wires_per_plane; j++)
        {
          box->DrawBox(x1, y1, x2, y2);
          x1+=dx;
          x2+=dx;
          if (i==1&&(j%8==0))
            {
              line->DrawLine(x1, y0, x1, y0 + NLAYER*(dy+.007)-.007);
            }
        }
      x1 = x0;
      x2 = x1+dx;
      y1 = y1 + dy + .007;
      y2 = y1 + dy;
    }

  line->SetLineColor(7);
  y1 = y0;
  y2 = y0 + NLAYER*(dy+.007) - .007;
  if (wires_per_plane==48) {}
  else
    {
      for (j=0; j<4; j++)
        {
          if (wires_per_plane==32)
            {
              x1 = x0 + wghv32[j]*dx;
            }
          else if (wires_per_plane==64)
            {
              x1 = x0 + wghv64[j]*dx;
            }
          else if (wires_per_plane==96)
            {
              x1 = x0 + wghv96[j]*dx;
            }
          else if (wires_per_plane==112)
            {
              x1 = x0 + wghv112[j]*dx;
            }
          x2 = x1;
          line->DrawLine(x1, y1, x2, y2);
        }
    }

  x1 = x0;
  y1 = y0 + NLAYER*(dy+.007) - .007;
  x2 = x1 + dx;
  y2 = y1 - dy;

  box->SetFillStyle(1001);
  box->SetFillColor(4);
  text->SetTextAlign(33);
  text->SetTextSize(.05);
  text->SetTextFont(82);
  for (ilayer=1; ilayer<=6; ilayer++)
    {
      if (ilayer==1||ilayer==6)
        {
          sprintf(buffer, "Layer %d", ilayer);
          text->DrawText(x1-.01, y1-.02, buffer);
        }
      for (igroup=1; igroup<=wires_per_plane; igroup++)
        {
          if ((ilayer==6)&&(igroup==1||igroup==wires_per_plane))
            {
              text->SetTextSize(.035);
              text->SetTextAlign(13);
              sprintf(buffer, "%d", igroup);
              text->DrawText(x1, y2-.02, buffer);
            }
          if ((bool)j_data.wiregroup[igroup-1][ilayer-1])
            {
              box->DrawBox(x1, y1, x2, y2);
            }
          x1+=dx;
          x2+=dx;
        }
      x1 = x0;
      x2 = x1+dx;
      y1 = y1 - dy - .007;
      y2 = y1 - dy;
    }

  text->SetTextAlign(21);
  text->SetTextSize(.045);
  text->DrawText(.5, .06, "Wire Group Number");

  delete line;
  delete box;
  delete text;
  return;
}

void J_Display::j_plot_strips()
{
  int layer_no, i;
  //if(autoscale>0) {
  this->scale_y();
  //}
  /*else {
    for(layer_no=1; layer_no<=6; layer_no++) {
    j_data.ymin[layer_no-1] = 0;
    j_data.ymax[layer_no-1] = 1000;
    }
    }*/
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.15, "X");
  gStyle->SetLabelSize(.1, "Y");
  for (i=1; i<=NLAYER; i++)
    {
      sprintf(buffer, "hist%d", i-1);
      hist[i-1] = new TH1F(buffer, "", 82, 0, 81);
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      hist[layer_no-1]->SetFillStyle(1001);
      hist[layer_no-1]->SetFillColor(2);
      for (i=1; i<=strips_per_plane; i++)
        {
          if (j_data.strips[i-1][layer_no-1]<0)
            {
              j_data.strips[i-1][layer_no-1] *= .1;
            }
          hist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1]);
	  //         hist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1] - j_data.strips[7][layer_no-1]); // Joe ped subtraction HACK
		
        }
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      (hist[layer_no-1]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no-1], j_data.ymax[layer_no-1]);
      chist->cd(layer_no);
      gPad->Clear();
      hist[layer_no-1]->Draw();
      gPad->Update();
    }


  for (i=1; i<=NLAYER; i++)
    {
      delete hist[i-1];
    }

  return;
}

void J_Display::scale_y()
{
  int ilayer, j;

  for (ilayer=1; ilayer<=6; ilayer++)
    {
      j_data.ymin[ilayer-1]=0.0;
      j_data.ymax[ilayer-1]=50.0;
      for (j=1; j<=strips_per_plane; j++)
        {
          if (j_data.strips[j-1][ilayer-1]>j_data.ymax[ilayer-1])
            {
              j_data.ymax[ilayer-1] = j_data.strips[j-1][ilayer-1];
            }
        }
      j_data.ymax[ilayer-1] = j_data.ymax[ilayer-1] + .3*(j_data.ymax[ilayer-1]-j_data.ymin[ilayer-1]);
    }
  return;
}

void J_Display::wires_strips_label()
{
  TCanvas *c;
  TText *text;
  sprintf(buffer, " ");
  text = new TText(0,0,buffer);
  text->SetTextAlign(21);
  text->SetTextFont(82);
  text->SetTextSize(.18);

  c = lmidcan->GetCanvas();
  c->cd();
  c->Clear();
  text->DrawText(.5, .90, "Layer 1");
  text->DrawText(.5, .05, "Layer 6");
  text->SetTextAngle(90);
  text->SetTextSize(.25);
  text->SetTextAlign(23);
  text->DrawText(.6, .50, "ADC Counts");
  c->Update();

  c = titlecan->GetCanvas();
  c->cd();
  c->Clear();
  text->SetTextAngle(0);
  text->SetTextSize(.25);
  text->SetTextAlign(23);
  text->SetTextFont(102);
  sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d", upevt_.run_number, upevt_.event_number, upevt_.alct_csc_id, upevt_.clct_csc_id);
  text->DrawText(.5, .9, buffer);
  text->SetTextSize(.35);
  text->DrawText(.58, .30, "Strips");
  c->Update();

  return;
}

void J_Display::wires_strips_layout()
{
  int i;
  if (firstTimeWires)
    {
      firstTimeWires = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));


      show_menus();

      titleFrame = new TGCompositeFrame(fMain, 700, 100);
      midFrame = new TGCompositeFrame(fMain, 700, 450);
      wiresFrame = new TGCompositeFrame(fMain, 700, 300);

      midFrame->SetLayoutManager(new TGHorizontalLayout(midFrame));

      lmidcan = new TRootEmbeddedCanvas("left canvas", midFrame, 90, 450);
      hmidcan = new TRootEmbeddedCanvas("histogram canvas", midFrame, 610, 450);

      titlecan = new TRootEmbeddedCanvas("title canvas", titleFrame, 700, 70);
      wirescan = new TRootEmbeddedCanvas("wires canvas", wiresFrame, 700, 300);

      midFrame->AddFrame(lmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));
      midFrame->AddFrame(hmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      titleFrame->AddFrame(titlecan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));
      wiresFrame->AddFrame(wirescan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      fMain->AddFrame(titleFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      fMain->AddFrame(midFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      fMain->AddFrame(wiresFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();
      chist = hmidcan->GetCanvas();
      chist->Divide(1, NLAYER, 0, 0);
    }

}

void J_Display::display_sca()
{
  this->sca_layout();
  this->plot_samples();
  this->sca_label();
}

void J_Display::plot_samples()
{
  int layer_no, istrip, isample;
  float px[j_data.nsamples], py[j_data.nsamples];
  int lineStyle[5] = {9, 1, 2, 4, 3};
  int lineColor[5] = {2, 3, 1, 6, 4};
  sprintf(buffer, " ");
  TText *text = new TText(0, 0, buffer);
  this->scale_sca_y();
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.12, "X");
  gStyle->SetLabelSize(.1, "Y");
  //create graph
  if (!j_data.nsamples) return;
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      for (istrip=1; istrip<=5; istrip++)
        {
          for (isample=1; isample<=j_data.nsamples; isample++)
            {
              px[isample-1] = isample;
              py[isample-1] = j_data.sample[istrip-1][layer_no-1][isample-1];
            }
          graph[layer_no-1][istrip-1] = new TGraph(j_data.nsamples, px, py);
        }
    }

  //draw graphs
  text->SetTextFont(82);
  text->SetTextSize(.13);
  text->SetTextAlign(11);
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      cgraph->cd(layer_no);
      gPad->Clear();
      for (istrip=1; istrip<=5; istrip++)
        {
          sprintf(buffer, "CP");
          if (istrip==1)
            {
              sprintf(buffer, "ACP");
              (graph[layer_no-1][0]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no-1], j_data.ymax[layer_no-1]);
            }
          graph[layer_no-1][istrip-1]->SetMarkerStyle(7);
          graph[layer_no-1][istrip-1]->SetMarkerColor(lineColor[istrip-1]);
          graph[layer_no-1][istrip-1]->SetLineStyle(lineStyle[istrip-1]);
          graph[layer_no-1][istrip-1]->SetLineColor(lineColor[istrip-1]);
          graph[layer_no-1][istrip-1]->Draw(buffer);
          gPad->Update();
        }
      sprintf(buffer, "  Peak Strip: %d", j_data.peak_strip[layer_no-1]);
      text->DrawText(.35, .6, buffer);
      gPad->Update();
    }

  //delete graphs
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      for (istrip=1; istrip<=5; istrip++)
        {
          delete graph[layer_no-1][istrip-1];
        }
    }

  return;

}

void J_Display::scale_sca_y()
{
  int isample, layer_no, trace;

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      j_data.ymin[layer_no-1] = 0;
      j_data.ymax[layer_no-1] = 10.0;
      for (trace=1; trace<=5; trace++)
        {
          for (isample=1; isample<=j_data.nsamples; isample++)
            {
              if (j_data.sample[trace-1][layer_no-1][isample-1]>j_data.ymax[layer_no-1])
                {
                  j_data.ymax[layer_no-1] = j_data.sample[trace-1][layer_no-1][isample-1];
                }
            }
        }
      j_data.ymax[layer_no-1] = j_data.ymax[layer_no-1] + .1*(j_data.ymax[layer_no-1]-j_data.ymin[layer_no-1]);
    }
  return;
}

void J_Display::sca_label()
{
  int layer_no;
  TCanvas *tcan;
  sprintf(buffer, " ");
  TText *text = new TText(0,0,buffer);
  TLine *line = new TLine;
  text->SetTextFont(82);

  tcan = keycan->GetCanvas();
  tcan->cd();
  tcan->Clear();
  text->SetTextSize(.15);
  text->SetTextAlign(23);
  text->DrawText(.5, .95, "Switched-");
  text->DrawText(.5, .90, "Capacitor");
  text->DrawText(.5, .85, "Array");
  text->DrawText(.5, .80, "Display");
  //create key
  text->SetTextSize(.12);
  text->DrawText(.5, .3, "Key");
  line->SetLineStyle(1);
  line->SetLineColor(1);
  line->DrawLine(.25, .28, .75, .28);
  text->SetTextAlign(12);
  text->SetTextSize(.10);
  text->SetTextColor(2);
  text->DrawText(.05, .25, "Peak-2");
  line->SetLineColor(2);
  line->SetLineStyle(9);
  line->DrawLine(.5, .25, .9, .25);
  text->SetTextColor(3);
  text->DrawText(.05, .22, "Peak-1");
  line->SetLineColor(3);
  line->SetLineStyle(1);
  line->DrawLine(.5, .22, .9, .22);
  text->SetTextColor(1);
  text->DrawText(.05, .19, "Peak Strip");
  line->SetLineColor(1);
  line->SetLineStyle(2);
  line->DrawLine(.65, .19, .9, .19);
  text->SetTextColor(6);
  text->DrawText(.05, .16, "Peak+1");
  line->SetLineColor(6);
  line->SetLineStyle(4);
  line->DrawLine(.5, .16, .9, .16);
  text->SetTextColor(4);
  text->DrawText(.05, .13, "Peak+2");
  line->SetLineColor(4);
  line->SetLineStyle(3);
  line->DrawLine(.5, .13, .9, .13);
  tcan->Update();

  //left label
  tcan = lscamidcan->GetCanvas();
  tcan->cd();
  tcan->Clear();
  text->SetTextColor(1);
  text->SetTextAlign(22);
  text->SetTextSize(.2);
  text->DrawText(.5, .91, "Layer 6");
  text->DrawText(.5, .09, "Layer 1");
  text->SetTextAngle(90);
  text->DrawText(.8, .5, "ADC Counts");
  text->SetTextAngle(0);
  tcan->Update();

  //bottom label
  tcan = lowcan->GetCanvas();
  tcan->cd();
  tcan->Clear();
  text->SetTextSize(.3);
  text->SetTextAlign(23);
  text->DrawText(.47, .94, "SCA Sample Number");
  tcan->Update();

  //top label
  tcan = topcan->GetCanvas();
  tcan->cd();
  tcan->Clear();
  text->SetTextAlign(22);
  text->SetTextSize(.15);
  text->SetTextFont(102);
  sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d", upevt_.run_number, upevt_.event_number, upevt_.alct_csc_id, upevt_.clct_csc_id);
  text->DrawText(.5, .5, buffer);
  tcan->Update();

  return;
}

void J_Display::sca_layout()
{
  int i;
  if (firstTimeSca)
    {
      firstTimeSca = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      topFrame = new TGCompositeFrame(fMain, 800, 100);
      scaMidFrame = new TGCompositeFrame(fMain, 800, 650);
      lowFrame = new TGCompositeFrame(fMain, 800, 70);

      scaMidFrame->SetLayoutManager(new TGHorizontalLayout(scaMidFrame));

      lscamidcan = new TRootEmbeddedCanvas("left canvas", scaMidFrame, 90, 650);
      graphcan = new TRootEmbeddedCanvas("graph canvas", scaMidFrame, 550, 650);
      keycan = new TRootEmbeddedCanvas("key canvas", scaMidFrame, 160, 650);

      topcan = new TRootEmbeddedCanvas("top canvas", topFrame, 800, 100);
      lowcan = new TRootEmbeddedCanvas("lower canvas", lowFrame, 800, 40);

      scaMidFrame->AddFrame(lscamidcan, new TGLayoutHints(kLHintsExpandY));
      scaMidFrame->AddFrame(graphcan, new TGLayoutHints(kLHintsExpandY));
      scaMidFrame->AddFrame(keycan, new TGLayoutHints(kLHintsExpandY));

      topFrame->AddFrame(topcan, new TGLayoutHints(kLHintsExpandY|kLHintsExpandX));
      lowFrame->AddFrame(lowcan, new TGLayoutHints(kLHintsExpandY|kLHintsExpandX));

      fMain->AddFrame(topFrame, new TGLayoutHints(kLHintsExpandX));
      fMain->AddFrame(scaMidFrame, new TGLayoutHints(kLHintsExpandX));
      fMain->AddFrame(lowFrame, new TGLayoutHints(kLHintsExpandX));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();
      cgraph = graphcan->GetCanvas();
      cgraph->Divide(1, NLAYER, 0, 0);
    }

}

void J_Display::display_ctrig()
{
  this->ctrig_layout();
  this->wire_geom_cc();
  this->getsoft();
  this->plot_ctrig_strips();
  this->ctrig_label();
}

void J_Display::getsoft()
{
  int ilayer, istrip;
  const int comparator_threshold = 50;

  for (ilayer=1; ilayer<=NLAYER; ilayer++)
    {
      for (istrip=1; istrip<=NSTRIP; istrip++)
        {
          softhalfstrip[0][istrip-1][ilayer-1] = false;
          softhalfstrip[1][istrip-1][ilayer-1] = false;
          if (j_data.strips[istrip-1][ilayer-1]>comparator_threshold)
            {
              if (istrip>1 && istrip<NSTRIP)
                {
                  if ((j_data.strips[istrip-1][ilayer-1] > j_data.strips[istrip-2][ilayer-1]) && (j_data.strips[istrip-1][ilayer-1] > j_data.strips[istrip][ilayer-1]))
                    { 
                      if (j_data.strips[istrip-2][ilayer-1] > j_data.strips[istrip][ilayer-1])
                        {
                          softhalfstrip[0][istrip-1][ilayer-1] = true;
                        }
                      else
                        {
                          softhalfstrip[1][istrip-1][ilayer-1] = true;
                        }
                    }
                }
              else if (istrip==1)
                {
                  if (j_data.strips[istrip-1][ilayer-1]>j_data.strips[istrip][ilayer])
                    {
                      softhalfstrip[1][istrip-1][ilayer-1] = true;
                    }
                }
              else if (istrip==NSTRIP)
                {
                  if (j_data.strips[istrip-1][ilayer-1]>j_data.strips[istrip-2][ilayer-1])
                    {
                      softhalfstrip[1][istrip-1][ilayer-1] = true;
                    }
                }
            }
        }
    }

  return;
}

void J_Display::plot_ctrig_strips()
{
  int i, layer_no, j;
  double x0, x1, dx;
  TBox *box = new TBox;
  TGaxis *axis = new TGaxis;
  TLine *line = new TLine;

  this->ctrig_scale_y();

  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.25, "X");
  gStyle->SetLabelSize(.2, "Y");
  gStyle->SetNdivisions(505, "Y");

  for (i=1; i<=NLAYER; i++)
    {
      sprintf(buffer, "ctrighist%d", i-1);
      ctrighist[i-1] = new TH1F(buffer, "", 82, 0, 81);
    }

  //fill histograms
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      ctrighist[layer_no-1]->SetFillStyle(1001);
      ctrighist[layer_no-1]->SetFillColor(2);
      for (i=1; i<=strips_per_plane; i++)
        {
          if (j_data.strips[i-1][layer_no-1]<0)
            {
              j_data.strips[i-1][layer_no-1] *= .1;
            }
          ctrighist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1]);
	  //   ctrighist[layer_no-1]->Fill((double)i, j_data.sample[i-1][layer_no-1][2] - j_data.sample[i-1][layer_no-1][7]); // Joe : manual ped removal HACK
       

        }
    }

  x0 = .100;
  dx = 0.879/strips_per_plane;
  for (i=1; i<=2*NLAYER; i++)
    {
      ctrigcan->cd(i);
      gPad->Clear();
      
      if (i%2==1)
        {
          (ctrighist[(i-1)/2]->GetYaxis())->SetRangeUser(j_data.ymin[(i-1)/2], j_data.ymax[(i-1)/2]);
          //(ctrighist[(i-1)/2]->GetYaxis())->SetRangeUser(0, 1000);
          ctrighist[(i-1)/2]->Draw();
        }
      else
        {


	  float btop = 1.0;
	  float bsize = 0.66;
          for (j=1; j<=strips_per_plane; j++)
            {
              x1 = x0 + j*dx;
              box->SetLineColor(1);
	      
              if (softhalfstrip[0][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(4);
                  box->DrawBox(x1, btop-bsize*0/3, x1+dx/2.0, btop - bsize*1/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
              if (softhalfstrip[1][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(4);
                  box->DrawBox(x1+dx/2.0, btop-bsize*0/3, x1+dx, btop - bsize*1/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }

              if (j_data.halfstrip[0][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(6);
                  box->DrawBox(x1, btop-bsize*1/3, x1+dx/2.0, btop - bsize*2/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
              if (j_data.halfstrip[1][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(6);
                  box->DrawBox(x1+dx/2.0, btop-bsize*1/3, x1+dx, btop - bsize*2/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }

              if (j_data.clct[0][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(3);
                  box->DrawBox(x1, btop-bsize*2/3, x1+dx/2.0, btop - bsize*3/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
              if (j_data.clct[1][j-1][(i/2)-1])
                {
                  box->SetFillStyle(1001);
                  box->SetFillColor(3);
                  box->DrawBox(x1+dx/2.0, btop-bsize*2/3, x1+dx, btop - bsize*3/3);
                  box->SetFillStyle(0);
                  box->SetFillColor(1);
                  box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }

            }
          if (i==12)
            {
              line->DrawLine(.1, .18, .1, 1);
              line->DrawLine(.9999, .18, .9999, 1);
              axis->SetLabelSize(.25);
              axis->DrawAxis(.1, .18, 1, .18, 0, 81);
            }
          else
            {
              line->DrawLine(.1, 0, .1, 1);
              line->DrawLine(.9999, 0, .9999, 1);

            }
        }

      gPad->Update();
    }

  //delete histograms
  for (layer_no=0; layer_no<NLAYER; layer_no++)
    {
      delete ctrighist[layer_no];
    }
  delete box;
  delete axis;
}

void J_Display::ctrig_scale_y()
{
  int i, layer_no;

  for (layer_no=1; layer_no<=6; layer_no++)
    {
      j_data.ymin[layer_no-1] = 0;
      j_data.ymax[layer_no-1] = 10;
      for (i=1; i<=strips_per_plane; i++)
        {
          if (j_data.strips[i-1][layer_no-1]>j_data.ymax[layer_no-1])
            {
              j_data.ymax[layer_no-1] = j_data.strips[i-1][layer_no-1];
            }
        }
      j_data.ymax[layer_no-1] = j_data.ymax[layer_no-1] + .3*(j_data.ymax[layer_no-1] - j_data.ymin[layer_no-1]);
      //		j_data.ymax[layer_no-1] *=2; not necessary??
    }

  return;
}

void J_Display::ctrig_label()
{
  sprintf(buffer, " ");
  TText *text = new TText(0, 0, buffer);
  TBox *box = new TBox;
  TLine *line = new TLine;

  TCanvas *tempcan = ctrigtopcan->GetCanvas();
  tempcan->cd();
  tempcan->Clear();
  text->SetTextFont(102);
  text->SetTextSize(.25);
  text->SetTextAlign(21);
  sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d", upevt_.run_number, upevt_.event_number, upevt_.alct_csc_id, upevt_.clct_csc_id);
  text->DrawText(.5, .5, buffer);
  text->SetTextSize(.25);
  text->DrawText(.5, .1, "Cathode Trigger Display");
  tempcan->Update();

  tempcan = ctlmidcan->GetCanvas();
  tempcan->cd();
  tempcan->Clear();
  text->SetTextFont(82);
  text->SetTextSize(.2);
  text->SetTextAlign(22);
  text->DrawText(.5, .9, "Layer 1");
  text->SetTextAngle(90);
  text->DrawText(.5, .6, "ADC Counts");
  text->SetTextAngle(0);
  text->SetTextSize(.19);
  text->DrawText(.5, .45, "Key");
  line->SetLineColor(1);
  line->DrawLine(.2, .44, .8, .44);
  text->SetTextSize(.14);
  text->SetTextAlign(11);
  text->SetTextFont(102);

  float btop=0.43;
  float bsize=0.08;
  //blue software halfstrip
  btop=0.43;
  box->SetFillStyle(1001);
  box->SetFillColor(4);
  box->DrawBox(.1, btop-bsize*1/3, .15, btop-bsize*0/3);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(.1, btop-bsize, .15, btop);
  text->SetTextColor(4);
  text->DrawText(.2, .39, "Software");
  text->DrawText(.2, .37, "halfstrip");
  //pink hardware halfstrip
  btop=0.33;
  box->SetFillStyle(1001);
  box->SetFillColor(6);
  box->DrawBox(.1, btop-bsize*2/3, .15, btop-bsize*1/3);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(.1, btop-bsize, .15, btop);
  text->SetTextColor(6);
  text->DrawText(.2, .29, "Hardware");
  text->DrawText(.2, .27, "halfstrip");
  //green	key halfstrip
  btop=0.23;
  box->SetFillStyle(1001);
  box->SetFillColor(3);
  box->DrawBox(.1, btop-bsize*3/3, .15, btop-bsize*2/3);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(.1, btop-bsize, .15, btop);
  text->SetTextColor(1);
  text->DrawText(.2, .19, "Key");
  text->DrawText(.2, .17, "halfstrip");
  tempcan->Update();

  tempcan = ctriglowcan->GetCanvas();
  tempcan->cd();
  tempcan->Clear();
  text->SetTextSize(.3);
  text->SetTextFont(82);
  text->SetTextAlign(23);
  text->SetTextColor(1);
  text->DrawText(.55, .92, "Strip Number");
  tempcan->Update();


  delete text;
  delete line;
  delete box;

  return;
}

void J_Display::ctrig_layout()
{
  int i;
  if (firstTimeCtrig)
    {
      firstTimeCtrig = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      ctrigtopFrame = new TGCompositeFrame(fMain, 700, 100);
      ctrigmidFrame = new TGCompositeFrame(fMain, 700, 700);
      ctriglowFrame = new TGCompositeFrame(fMain, 700, 100);

      ctrigmidFrame->SetLayoutManager(new TGHorizontalLayout(ctrigmidFrame));

      ctlmidcan = new TRootEmbeddedCanvas("left canvas", ctrigmidFrame, 90, 700);
      midcan = new TRootEmbeddedCanvas("middle canvas", ctrigmidFrame, 610, 700);

      ctrigtopcan = new TRootEmbeddedCanvas("title canvas", ctrigtopFrame, 700, 70);
      ctriglowcan = new TRootEmbeddedCanvas("wires canvas", ctriglowFrame, 700, 40);

      ctrigmidFrame->AddFrame(ctlmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));
      ctrigmidFrame->AddFrame(midcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      ctrigtopFrame->AddFrame(ctrigtopcan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));
      ctriglowFrame->AddFrame(ctriglowcan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      fMain->AddFrame(ctrigtopFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      fMain->AddFrame(ctrigmidFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      fMain->AddFrame(ctriglowFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();
      ctrigcan = midcan->GetCanvas();
      ctrigcan->Divide(1, NLAYER*2, 0, 0);
    }

}

void J_Display::display_clct_time()
{
  this->normal_layout();
  this->wire_geom_cc();
  this->plot_cath_clct_time();
  this->label_display_cc();

}

void J_Display::plot_cath_clct_time()
{
  int clct_time_tmp, shift_result;
  float x_coord[2];
  float y_coord[2];
  float x_shift, x_step, y_shift, y_step, y_time_step;
  int color, i, j, ij, ijk;
  int text_index[NLAYER][NSTRIP/2];
  char text[NLAYER];
  char t[NLAYER][NSTRIP/2];
  char string[NLAYER/2];
  char temp[100];
  TLine *line = new TLine;
  sprintf(buffer, " ");
  TText *text_draw = new TText(0, 0, buffer);

  cmain->cd();
  cmain->Clear();

  for (i=0; i<NLAYER; i++)
    {
      text[i] = ' ';
    }
  for (j=1; j<=NLAYER; j++)
    {
      for (i=1; i<=NSTRIP/2; i++)
        {
          text_index[j-1][i-1] = 0;
        }
    }

  x_shift = .1;
  x_step = .019;
  y_shift = .85;
  y_step = .005;
  y_time_step = (6+1)*y_step;

  line->SetLineWidth(3);

  color = 1;
  for (i=1; i<=upevt_.clct_nbucket; i++)
    {
      if (color==1)
        {
          line->SetLineColor(4);
          color = 0;
        }
      else
        {
          line->SetLineColor(3);
          color = 1;
        }
      for (j=1; j<=NLAYER; j++)
        {
          x_coord[0] = x_shift + x_step;
          x_coord[1] = x_shift + (NSTRIP/2+1)*x_step;
          y_coord[0] = y_shift - i*y_time_step - j*y_step;
          y_coord[1] = y_coord[0];
          line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
        }
    }

  line->SetLineColor(2);
  std::cout<<"clct_nbucket: "<<upevt_.clct_nbucket<<endl;
  for (i=1; i<=upevt_.clct_nbucket; i++)
    {
      for (j=1; j<=NLAYER; j++)
        {
          for (ij=1; ij<=(NSTRIP); ij++)
            {
              clct_time_tmp = upevt_.clct_dump_strips[j-1][ij-1];
              if ((1&clct_time_tmp>>(i-1))==1)
                {
                  x_coord[0] = x_shift + ij/2*x_step;
                  x_coord[1] = x_shift + (ij/2+1)*x_step;
                  y_coord[0] = y_shift - i*y_time_step - j*y_step;
                  y_coord[1] = y_coord[0];
                  line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
                  text_index[j-1][ij/2-1]++;
                }
            }
        }
    }

  for (j=1; j<=NLAYER; j++)
    {
      for (i=1; i<=NSTRIP/2; i++)
        {
          t[j-1][i-1] = '*';
          switch (text_index[j-1][i-1])
            {
            case 1:
              t[j-1][i-1] = '1';
              break;
            case 2:
              t[j-1][i-1] = '2';
              break;
            case 3:
              t[j-1][i-1] = '3';
              break;
            case 4:
              t[j-1][i-1] = '4';
              break;
            case 5:
              t[j-1][i-1] = '5';
              break;
            case 6:
              t[j-1][i-1] = '6';
              break;
            case 7:
              t[j-1][i-1] = '7';
              break;
            case 8:
              t[j-1][i-1] = '8';
              break;
            case 9:
              t[j-1][i-1] = '9';
              break;
            default:
              t[j-1][i-1] = '*';
            }
        }
    }

  sprintf(buffer, "");
  double dy = .018;
  double dx, x0;
  x0 = .1;
  dx = .022;
  text_draw->SetTextFont(82);
  text_draw->SetTextSize(.031);
  for (i=1; i<=NLAYER; i++)
    {
      for (j=1; j<=NSTRIP/2; j++)
        {
          sprintf(buffer, "%c", t[i-1][j-1]);
          text_draw->DrawText(x_shift + j*x_step, .95-i*dy, buffer);
        }
    }

  cmain->Update();

  delete line;
  delete text_draw;
  return;
}

void J_Display::Print()
{

  sprintf(buffer, "run%d_event%d_id%d.png", upevt_.run_number, upevt_.event_number, activeDisp);
  switch (activeDisp)
    {

    case M_ATRIG:
    case M_ALCT_TIME:
    case M_CLCT_TIME:
      cmain->Print(buffer, "png");
      break;
    case M_WIRES_STRIPS:
      sprintf(buffer, "run%d_event%d_id%d_STRIPS.png", upevt_.run_number, upevt_.event_number, activeDisp);
      chist->Print(buffer, "png");
      sprintf(buffer, "run%d_event%d_id%d_WIRES.png", upevt_.run_number, upevt_.event_number, activeDisp);
      cmain->Print(buffer, "png");
      break;
    case M_SCA:
      cgraph->Print(buffer, "png");
      break;
    case M_CTRIG:
      ctrigcan->Print(buffer, "png");
      break;
    default:
      std::cout<<"No canvas to print."<<endl;
    }

  return;
}

void J_Display::CloseWindow()
{
  gApplication->Terminate();
}

void J_Display::show_menus()
{

  menuConfigure = new TGPopupMenu(gClient->GetRoot());
  menuConfigure->AddEntry("&Wires Strips", M_WIRES_STRIPS);
  menuConfigure->AddEntry("At&rig", M_ATRIG);
  menuConfigure->AddEntry("&Alct time", M_ALCT_TIME);
  menuConfigure->AddEntry("&Clct time", M_CLCT_TIME);
  menuConfigure->AddEntry("&SCA", M_SCA);
  menuConfigure->AddEntry("C&trig", M_CTRIG);

  menuConfigure->Connect("Activated(int)", "J_Display", this, "handle_menu(int)");

  menuRun = new TGPopupMenu(gClient->GetRoot());
  menuRun->AddEntry("&Select File", M_SELECT_FILE);
  menuRun->AddEntry("&Start", M_RUN);
  menuRun->AddEntry("&Pause", M_PAUSE);

  menuRun->Connect("Activated(int)", "J_Display", this, "handle_run_menu(int)");

  menuBar = new TGMenuBar(fMain, 700, 20, kHorizontalFrame);
  menuBar->AddPopup("Con&figure", menuConfigure, new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0));
  menuBar->AddPopup("&Run", menuRun, new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0));

  fMain->AddFrame(menuBar, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

  char *xpms[4] = { "$ICONLOCATION/pause.xpm", "$ICONLOCATION/play.xpm", "$ICONLOCATION/rewind.xpm", "$ICONLOCATION/print.xpm"};
  char *tips[4] = {"Pause", "Play", "Rewind", "Print to File"};
  ToolBarData_t t[4];

  toolBar = new TGToolBar(fMain, 700, 40);

  for (int i=0; i<4; i++)
    {
      t[i].fPixmap = xpms[i];
      t[i].fTipText = tips[i];
      t[i].fStayDown = kFALSE;
      t[i].fId = i+1;
      t[i].fButton = NULL;

      toolBar->AddButton(fMain, &t[i], 0);
    }

  toolBar->Connect("Clicked(int)", "J_Display", this, "handle_tb(int)");

  TGHorizontal3DLine *lh = new TGHorizontal3DLine(fMain);
  fMain->AddFrame(lh, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

  fMain->AddFrame(toolBar, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

}

void J_Display::show_status_bars()
{
  chamFrame = new TGCompositeFrame(fMain, 700, 50);
  chamFrame->SetLayoutManager(new TGHorizontalLayout(chamFrame));

  alctChamber = new TGNumberEntry(chamFrame, TGNumberFormat::kNESInteger);
  clctChamber = new TGNumberEntry(chamFrame, TGNumberFormat::kNESInteger);
  alctChamber->Connect("ValueChanged(long)", "J_Display", this, "handle_num()");
  // chamFrame->AddFrame(alctChamber, new TGLayoutHints(kLHintsLeft));
  fMain->AddFrame(chamFrame, new TGLayoutHints(kLHintsBottom|kLHintsExpandX));

  ctrlFrame = new TGCompositeFrame(fMain, 700, 50);
  ctrlFrame->SetLayoutManager(new TGHorizontalLayout(ctrlFrame));

  evtSlider = new TGHSlider(ctrlFrame, 400); //, kSlider1 | kScaleBoth);
  evtSlider->SetRange(1, total_events);
  evtSlider->SetPosition(event_num);
  // evtSlider->Connect("PositionChanged(Int_t)", "J_Display", this, "request_event(Int_t)");
  evtSlider->Connect("Pressed()", "J_Display", this, "pause_run()");
  evtSlider->Connect("Released()", "J_Display", this, "request_event(Int_t)");

  selEvent = new TGNumberEntry(ctrlFrame, TGNumberFormat::kNESInteger);
  selEvent->SetLimits(TGNumberFormat::kNELLimitMinMax, 1, total_events);
  selEvent->SetNumber(event_num);
  // selEvent->SetButtonToNum(false);
  selEvent->Connect("ValueSet(Long_t)", "J_Display", this, "request_event_num()");

  lblTotalEvents = new TGLabel(ctrlFrame, Form("Total Events: %ld", total_events));
  ctrlFrame->AddFrame(lblTotalEvents, new TGLayoutHints(kLHintsCenterY));
  // ctrlFrame->AddFrame(new TGLabel(ctrlFrame, Form("Show Event: %ld", requested_event)), new TGLayoutHints(kLHintsBottom));
  ctrlFrame->AddFrame(evtSlider, new TGLayoutHints(kLHintsRight|kLHintsExpandX));
  ctrlFrame->AddFrame(selEvent );


  fMain->AddFrame(ctrlFrame, new TGLayoutHints(kLHintsBottom|kLHintsExpandX));

  statusFrame = new TGCompositeFrame(fMain, 700, 50);
  statusFrame->SetLayoutManager(new TGHorizontalLayout(statusFrame));

  lblOpenFile = new TGLabel(statusFrame, Form("Opened File: %s", file_name));
  statusFrame->AddFrame(lblOpenFile);
  fMain->AddFrame(statusFrame, new TGLayoutHints(kLHintsBottom|kLHintsExpandX));

}


void J_Display::pause_run()
{
  make_status_paused();
}
