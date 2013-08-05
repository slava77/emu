
#include "J_Display.h"


using namespace std;

static char buffer[400];
static bool firstTime = true;
static bool firstTimeWires = true;
static bool firstTimeSca = true;
static bool firstTimeCtrig = true;
static bool firstTimeN = true;
static bool initial = true;
static int returnValue = 1;
static int activeDisp = 0;
static TH1F* hist[NLAYER];
static TGraph* graph[NLAYER][5];
static bool softhalfstrip[2][NSTRIP][NLAYER];

static const double striphist_x1=0.1;
static const double striphist_x2=0.999;

extern int rewind_comm;
extern emu::daq::reader::RawDataFile *fd;
extern char file_name[1000];

csc_event_type upevt_;
j_common_type j_data;
long event_num;
J_Display *jd;
TTimer *timer;

int crate = -1, chamber = -1;
enum CommandIdentifiers
  {
    M_WIRES_STRIPS = 1,
    M_ATRIG,
    M_ALCT_TIME,
    M_CLCT_TIME,
    M_SCA,
    M_CTRIG,
    M_WIRES_STRIPS_PEAKS, // firman
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

void J_Display::cbShowEvent_m()
{
  int    istat, rtval;
  int    offset;
  short  event_wc;
  static float mean[NSTRIP*NLAYER], rms[NSTRIP*NLAYER];
  static int oneliner = 0;
  //	FL_Coord  x, y, w, h;
  char   message[60];
  unsigned int evnum;
  static int first_time = 1;

  int block_wc;
  // fake header, to satisfy the old unpacker
  short fake_head[] = {
    0x001d, 0x4845, 0x4144, 0x0000, 0x0003, 0x04d2, 0x0000, 0x0000,
    0x0002, 0x0000, 0x5a01, 0x0000, 0x6803, 0x100b, 0x201d,	0x000a,
    0x9a58, 0x102c, 0x0fa0, 0x0fa0, 0x102c, 0x100e, 0x001f, 0x001a,
    0x00c8, 0x001a, 0x0001, 0x0001, 0x001d
  };
  
  if ((need_event && !disp_paused) || (change_chamber))
    {
      // if ((!disp_paused) && change_chamber) change_chamber = false;
      if (change_chamber) change_chamber = false;
      
      // RawDataFile
      do
        {
          if ((fd != NULL && first_time) || rewind_comm)
            {

              if (rewind_comm)
                {
                  this->rewind_display();
                }
              // RawDataFile
              rtval = fd->readNextEvent();
              // std::cout<<"event display rtval next event "<<rtval<<endl;

              upevt_.event_number = ++event_num; // firman
              evnum = event_num; // firman
              // upevt_.event_number = evnum = event_num++;

//			std::cout << "event number 189: " << upevt_.event_number << std::endl;

              // run number
              fake_head[5] = run_num;

              // event number
              fake_head[6] = (evnum << 16) & 0xffff; // MSB first (f@&k)
              fake_head[7] = evnum & 0xffff;
	      
              // chamber type cannot be determined at this point. Could also be different chambers in one file.
              // event display just shows the available number of wiregroups for each event
	      
	      // RawDataFile
	      block_wc = fd->dataLength()/sizeof(short) + 4; // add 4 for DDU2 header, block_wc in front and rear
	      event_wc = block_wc + 200; // add size of header (roughly)
          if( rtval == 0 ) 
            {
              this->rewind_display();
              continue;
            }

	      int i = 1;
	      i += sizeof(fake_head)/sizeof(short); // move index
	      i +=  fd->dataLength() / sizeof(short); // move index
          
              event_wc = i;
            }

          // Unpack the event and release its buffer space
          if (upevt_.event_number >= requested_event)
            {
              std::cout << "==> Calling get_next_event event number/req event "<<upevt_.event_number<<"/"<<requested_event<<endl;
              // std::cout << "==> Calling get_next_event event number/req event "<<event_num<<"/"<<requested_event<<endl;
              // istat = get_next_event(buf, first_time); // unpack only if needed
              istat = get_next_event_cmssw(fd->data(), fd->dataLength(), first_time); // unpack only if needed
              upevt_.event_number = event_num;
              // std::cout << "get next event return status: " << istat << endl;

              if (istat & 2) first_time = 0;
              else
                {
                  // std::cout <<  "starting new event, istat: "<< istat <<"\n";
                  first_time = 1;
                }
            }
          
          else
            istat = 0;

          // RawDataFile
//			if (fd != NULL && first_time) free(buf);
        }
      while (istat & 1); // if required chamber not found, just get the next event.

      if (istat & 4)
        {
          syslog(LOG_LOCAL1+LOG_ERR, "Could not get next event. EE");
          // set timer here
	  //			fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);
          need_event = 1;
          return;
        }
      else
        {
          if (upevt_.event_number < requested_event)
            {

              printf("==> Fast forward to event %d\n", requested_event);

              do
                {
                  // RawDataFile
                  rtval = fd->readNextEvent();
                  upevt_.event_number = evnum = ++event_num;

//					std::cout << "event number 265: " << upevt_.event_number << std::endl;

                  if ( rtval == 0 )
                    {
                      /* If at end of file then set fd back to the beginning */
                      this->rewind_display();
                      break;
                    }
                }
              while (evnum < requested_event - 1);
              
              // set timer here
	      //				fl_set_timer(fd_event_display->show_event_timer, 0.1);
              need_event = 1;
              return;
            }
          else
            {
              need_event = 0;
	      //				subtract_monitor_peds(mean, rms);

              // Copy data from up upevt_ into common blocks used by event display
	      //				unpack_data_();
              unpack_data_cc(); //added 10-30-07
              need_refresh = 1; // firman
            }
        }

    } /* end if need_event */

// show events here
//	j_display_cc();
  if (need_refresh) { // firman
    jd->layout();
    need_refresh = 0; // firman
  }

  disp_wire_strip = wire_strip_active;
  disp_alct_time = alct_time_active;
  disp_clct_time = clct_time_active;
  disp_sca        = sca_active;
  disp_atrig      = atrig_active;
  disp_ctrig      = ctrig_active;
  disp_sci_strips = sci_strips_active;
  disp_sci_wires  = sci_wires_active;
  need_event = 1;
  // set timer here
//	fl_set_timer(fd_event_display->show_event_timer, (double)timer_delay);

}

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
    //jd->handle_menu(M_CLCT_TIME);
    //jd->handle_menu(M_ATRIG);
  }

  theApp.Run();

  return 0;
}

void J_Display::rewind_display() {
    printf("==> Rewinding...\n");
    requested_event = 1;
    // RawDataFile
    fd->close();
    // RawDataFile
    fd->open(file_name);
    event_num = 0;
    rewind_comm = 0;
    static int first_time = 1;
}

void J_Display::layout()
{
  int i;

  //	std::cout << "layout, this = " << std::hex << this << " jd = " << jd << std::dec << std::endl;


  if (initial)
    {
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
    case M_WIRES_STRIPS_PEAKS: // firman
      this->display_wires_strips_peaks_cc(); // firman
      break; // firman
    case M_ATRIG:
      this->display_atrig_cc();
      break;
    case M_ALCT_TIME:
      this->display_wires_alct_time_cc();
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

      fMain->SetLayoutManager(new TGVerticalLayout(fMain)); // make frames top-to-bottom

      show_menus();

      // banner
      bannerFrame = new TGCompositeFrame(fMain, 700, 100);
      fMain->AddFrame(bannerFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      bannercan = new TRootEmbeddedCanvas("title canvas", bannerFrame, 700, 70);
      bannerFrame->AddFrame(bannercan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      // body
      canFrame = new TGCompositeFrame(fMain, 700, 400);
      fMain->AddFrame(canFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));

      // plots
      can1 = new TRootEmbeddedCanvas("drawing canvas", canFrame, 700, 400);
      canFrame->AddFrame(can1, new TGLayoutHints(kLHintsTop|kLHintsExpandX));

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
  need_refresh = 1;
}

void J_Display::handle_chambers_menu(int id)
{
  setSelectedChamberID(id);
  need_refresh = 1;
  
  // to refresh the display 
  // rewind_comm = 1;
  requested_event = upevt_.event_number + 1;
  change_chamber = true;
  upevt_.event_number = 0;
  reset_data_file(); // firman ???
  std::cout << "==> Request event: " << requested_event << endl;
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

void J_Display::handle_hack_mode(bool mode)
{
  setHackMode(mode);
  if (mode) printf("Hack mode enabled\n");
  else printf("Hack mode disabled\n");
}

void J_Display::request_event_num()
{

  //  std::cout << "--> Request event: " << selEvent->GetIntNumber() << endl;
  evtSlider->SetPosition(selEvent->GetIntNumber());
  requested_event = selEvent->GetIntNumber();
  // if (ulong( selEvent->GetIntNumber() ) < requested_event)
  if (upevt_.event_number > requested_event)
    {
      reset_data_file();
    }
  std::cout << "==> Request event: " << requested_event << endl;
}

void J_Display::request_event(int pos)
{
  // std::cout << pos << std::endl;

  TGFrame *frm = (TGFrame *) gTQSender;
  if (frm->IsA()->InheritsFrom(TGSlider::Class()))
    {
      selEvent->SetNumber(evtSlider->GetPosition());
      requested_event = evtSlider->GetPosition();
      // if (ulong( evtSlider->GetPosition() ) < requested_event)
      if (upevt_.event_number > requested_event)
        {
          reset_data_file();
        }
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
  if ((ulong( event_num ) >= requested_event)  && !disp_paused)
    {
      // std::cout << "=> Updating " << event_num << std::endl;
      evtSlider->SetPosition(event_num);
      selEvent->SetNumber(event_num);
    }
}


void J_Display::display_wires_alct_time_cc()
{
  this->normal_layout();
  this->update_banner("ALCT Time");
  this->plot_wires_alct_time_cc();
}

void J_Display::plot_wires_alct_time_cc()
{
  int alct_time_tmp;
  float x_coord[2], y_coord[2];
  float x_shift, x_step, y_shift, y_step, y_time_step;
  int color, i, j, ij;

  if(line) delete line; line = new TLine;
  if(text) delete text; text = new TText;

  x_shift = 0.01;
  x_step = 0.85/(NWIRE);
  y_shift = 0.98;
  y_step = 0.7/(NLAYER*upevt_.alct_nbucket);
  y_time_step = (NLAYER+1)*y_step;

  cmain->cd();
  cmain->Clear();
  line->SetLineWidth(4);
  text->SetTextSize(0.040);
  text->SetTextFont(102);

  line->SetLineColor(1);
  for (j=1; j<=NLAYER; j++)
    {
      x_coord[0] = x_shift + x_step;
      x_coord[1] = x_shift + (NWIRE+1)*x_step;
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
      for (ij=1; ij<=NWIRE; ij++)
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
          x_coord[1] = x_shift + (NWIRE+1)*x_step;
          y_coord[0] = y_shift - i*y_time_step - j*y_step;
          y_coord[1] = y_coord[0];
          line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
          if (j==(NLAYER/2+2))
            {
              text->SetTextSize(0.04);
              text->DrawText(x_coord[1]+0.01, y_coord[0], buffer);
            }
          if ((j==NLAYER&&i==upevt_.alct_nbucket))
            {
	      text->SetTextAlign(22);
              text->SetTextSize(0.04);
              sprintf(buffer, "%d", 1);
              text->DrawText(x_coord[0]+0.005, y_coord[0]-0.03, buffer);
              sprintf(buffer, "%d", NWIRE);
              text->DrawText(x_coord[1]-0.005, y_coord[0]-0.03, buffer);
	      text->DrawText((x_coord[1]-x_coord[0])/2, y_coord[0]-0.05, "Wire Group Number");
            }
        }
    }
  line->SetLineColor(2);
  for (i=1; i<=upevt_.alct_nbucket; i++)
    {
      for (j=1; j<=NLAYER; j++)
        {
          for (ij=1; ij<=NWIRE; ij++)
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
  j_data.dx = .10*(48.0/NWIRE);
  for (i=1; i<=11; i+=2)
    {
      j_data.nch[i-1] = NWIRE;
    }
  for (i=2; i<=10; i+=2)
    {
      j_data.nch[i-1] = -NWIRE;
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

void J_Display::atrig_boxit_cc(float y, float x, float dy, float dx, TBox* abox)
{
  float ax[2], ay[2];
  ax[0] = x; //-.5*dx;
  ay[0] = y; //-.5*dy;
  ax[1] = ax[0] + dx;
  ay[1] = ay[0] + dy;

  cmain->cd();
  abox->DrawBox(ax[0], ay[0], ax[1], ay[1]);
  cmain->Update();

  return;
}

void J_Display::display_atrig_cc()
{
  this->normal_layout();
  this->atrig_wire_geom_cc();
  this->update_banner("Anode Trigger");
  this->j_plot_atrig_wires();
  return;
}

void J_Display::j_plot_atrig_wires()
{
  int i, j, ilayer, igroup;
  double dx, dy, x0, y0, x1, y1, x2, y2, why, ex, xk;
  if(box) delete box; box = new TBox;
  if(text) delete text; text = new TText;
  if(line) delete line; line = new TLine;

  cmain->cd();
  cmain->Clear();

  x0 = 0.20;
  y0 = 0.05;
  dx = 0.075;
  dy = 0.8/(NWIRE); //0.0075;

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

  text->SetTextAlign(32);
  text->SetTextSize(.04);
  sprintf(buffer, "Wire Group 1");
  text->DrawText(x0-0.01, y0, buffer);

  sprintf(buffer, "Wire Group %d", NWIRE);
  text->DrawText(x0-0.01, y0 + (NWIRE-1)*dy, buffer);

  line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);

  text->SetTextAlign(11);
  for (i=1; i<=NLAYER; i++)
    {
      for (j=1; j<=NWIRE; j++)
        {
          box->DrawBox(x1, y1, x2, y2);
          y1 += dy;
          y2 += dy;

          if (i==1&&(j==NWIRE||j%10==0))
            {
              line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);
            }

        }

      if (i==1 || i==NLAYER)
        {
          text->SetTextSize(.05);
          sprintf(buffer, "Layer %d", i);
          text->DrawText(x1, y1+.008, buffer);
        }

      x1 = x1 + dx + .01;
      x2 = x1 + dx;
      y1 = y0;
      y2 = y0 + dy;
    }

  text->SetTextSize(0.05);
  text->DrawText(0.03, 0.55, "ANODE");
  text->DrawText(0.03, 0.5, "TRIGGER");
  text->DrawText(0.03, 0.45, "DISPLAY");

  //KEY
  float keybottom = 0.1;
  float keyheight = 0.6;

  text->SetTextAlign(22);
  xk = x0+(NLAYER+1)*(dx+0.01);
  dy*=2;

  text->SetTextSize(0.04);
  text->DrawText(xk+0.5*dx, keybottom + keyheight, "KEY");
  line->DrawLine(xk-0.5*dx, keybottom + keyheight*0.95,xk+1.5*dx, keybottom + keyheight*0.95);

  text->SetTextSize(0.035);
  box->SetFillStyle(1001);
  box->SetFillColor(4);
  box->DrawBox(xk, keybottom + keyheight*0.85, xk+0.25*dx, keybottom + keyheight*0.85+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, keybottom + keyheight*0.85, xk+dx, keybottom + keyheight*0.85+dy);
  text->SetTextColor(4);
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.8, "The wire had");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.75, "a TDC hit");

  box->SetFillStyle(1001);
  box->SetFillColor(3);
  box->DrawBox(xk+0.25*dx, keybottom + keyheight*0.65, xk+0.5*dx, keybottom + keyheight*0.65+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, keybottom + keyheight*0.65, xk+dx, keybottom + keyheight*0.65+dy);
  text->SetTextColor(3);
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.6, "The wire appears");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.55, "in the ALCT");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.5, "raw hits data");

  box->SetFillStyle(1001);
  box->SetFillColor(6);
  box->DrawBox(xk+0.5*dx, keybottom + keyheight*0.4, xk+0.75*dx, keybottom + keyheight*0.4+dy);
  box->SetFillStyle(3002);
  box->DrawBox(xk+0.5*dx, keybottom + keyheight*0.35, xk+0.75*dx, keybottom + keyheight*0.35+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, keybottom + keyheight*0.4, xk+dx, keybottom + keyheight*0.4+dy);
  box->DrawBox(xk, keybottom + keyheight*0.35, xk+dx, keybottom + keyheight*0.35+dy);
  text->SetTextColor(6);
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.3, "The best and 2nd");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.25, "best LCT pattern");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.2, "matched in this event");

  box->SetFillStyle(1001);
  box->SetFillColor(2);
  box->DrawBox(xk+0.75*dx, keybottom + keyheight*0.1, xk+dx, keybottom + keyheight*0.1+dy);
  box->SetFillStyle(0);
  box->SetFillColor(1);
  box->DrawBox(xk, keybottom + keyheight*0.1, xk+dx, keybottom + keyheight*0.1+dy);
  text->SetTextColor(2);
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.05, "The wire was part of");
  text->DrawText(xk+0.5*dx, keybottom + keyheight*0.0, "the TMB choice");

  text->SetTextColor(1);
  text->SetTextAlign(32);
  sprintf(buffer, "ALCT0 quality %d", j_data.alct_q[0]);
  text->DrawText(0.95, 0.85, buffer);
  sprintf(buffer, "ALCT1 quality %d", j_data.alct_q[1]);
  text->DrawText(0.95, 0.8, buffer);

  cmain->Update();

  return;
}

void J_Display::display_wires_strips_cc()
{
  this->wires_strips_layout();
  this->update_banner("Wires & Strips");
  this->j_plot_wires();
  this->j_plot_strips();
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
  if(line) delete line; line = new TLine;
  if(text) delete text; text = new TText;
  if(box) delete box; box = new TBox;

  wirescan->GetCanvas()->cd();
  wirescan->GetCanvas()->Clear();

  box->SetFillStyle(0);
  box->SetLineColor(4);
  line->SetLineColor(4);

  text->SetTextAlign(23);
  text->SetTextSize(0.09);
  text->SetTextFont(102);
  text->DrawText(0.5, 0.95, "Wires");

  text->SetTextAlign(21);
  text->SetTextSize(.055);
  text->DrawText(0.5, 0.1, "Wire Group Number");

  x0 = 0.10;
  y0 = 0.2;
  dx = 0.80/(NWIRE);
  dy = 0.60/NLAYER;

  x1 = x0;
  y1 = y0;
  x2 = x1 + dx;
  y2 = y1 + dy;

  line->DrawLine(x1, y0, x1, y0 + NLAYER*(dy+.007)-.007);

  for (i=1; i<=NLAYER; i++)
    {
      for (j=1; j<=NWIRE; j++)
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
  if (NWIRE==48) {}
  else
    {
      for (j=0; j<4; j++)
        {
          if (NWIRE==32)
            {
              x1 = x0 + wghv32[j]*dx;
            }
          else if (NWIRE==64)
            {
              x1 = x0 + wghv64[j]*dx;
            }
          else if (NWIRE==96)
            {
              x1 = x0 + wghv96[j]*dx;
            }
          else if (NWIRE==112)
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
      for (igroup=1; igroup<=NWIRE; igroup++)
        {
          if ((ilayer==6)&&(igroup==1||igroup==NWIRE))
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

  return;
}

void J_Display::j_plot_strips()
{
  int layer_no, i;
  this->scale_y();
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.15, "X");
  gStyle->SetLabelSize(.1, "Y");
  for (i=1; i<=NLAYER; i++)
    {
      sprintf(buffer, "hist%d", i-1);
      if(hist[i-1]) delete hist[i-1];
      hist[i-1] = new TH1F(buffer, "", NSTRIP+2, 0, NSTRIP+1);
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      hist[layer_no-1]->SetFillStyle(1001);
      hist[layer_no-1]->SetFillColor(2);
      for (i=1; i<=NSTRIP; i++)
        {
          if (j_data.strips[i-1][layer_no-1]<0)
            {
              j_data.strips[i-1][layer_no-1] *= 0.1;
            }
          hist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1]);
        }
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      (hist[layer_no-1]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no-1], j_data.ymax[layer_no-1]);
      (hist[layer_no-1]->GetYaxis())->SetNdivisions(404);
      (hist[layer_no-1]->GetYaxis())->SetLabelSize(0.2);
      chist->cd(layer_no+1);
      gPad->Clear();
      hist[layer_no-1]->Draw();
      gPad->Update();
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
      for (j=1; j<=NSTRIP; j++)
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

void J_Display::wires_strips_layout()
{
  int i;
  if (firstTimeWires)
    {
      firstTimeWires = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      // banner
      bannerFrame = new TGCompositeFrame(fMain, 700, 100);
      fMain->AddFrame(bannerFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      bannercan = new TRootEmbeddedCanvas("title canvas", bannerFrame, 700, 70);
      bannerFrame->AddFrame(bannercan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      // body
      midFrame = new TGCompositeFrame(fMain, 700, 500);
      fMain->AddFrame(midFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      midFrame->SetLayoutManager(new TGHorizontalLayout(midFrame)); // to make side-by-side frames

      // legend
      lmidcan = new TRootEmbeddedCanvas("left canvas", midFrame, 90, 500);
      midFrame->AddFrame(lmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      // strips
      hmidcan = new TRootEmbeddedCanvas("histogram canvas", midFrame, 610, 500);
      midFrame->AddFrame(hmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      // wires
      wiresFrame = new TGCompositeFrame(fMain, 700, 300);
      fMain->AddFrame(wiresFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      wirescan = new TRootEmbeddedCanvas("wires canvas", wiresFrame, 700, 300);
      wiresFrame->AddFrame(wirescan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();

      // divide canvas for individual layer plots
      chist = hmidcan->GetCanvas();
      chist->Divide(1, NLAYER+2, 0, 0);

      // set labels
      wires_strips_label();
    }
}

void J_Display::wires_strips_label()
{
  TCanvas *c;
  if(text) delete text; text = new TText;
  if(axis) delete axis;  axis = new TGaxis;
  if(line) delete line; line = new TLine;

  c = hmidcan->GetCanvas();
  c->cd(NLAYER+2);
  text->SetTextAngle(0);
  
  gPad->SetPad(gPad->GetXlowNDC(),0,gPad->GetXlowNDC()+gPad->GetWNDC(),1); //expand height
  gPad->SetFillStyle(4001); // make transparent
  text->SetTextSize(0.04);
  text->SetTextFont(82);
  text->SetTextAlign(23);
  text->SetTextColor(1);
  text->DrawText(0.5, 0.05, "Strip Number");

  float histstop = 0.87;
  float histsbottom = 0.135;
  line->SetLineColor(1);
  line->SetLineWidth(1);
  line->SetLineStyle(1);
  line->DrawLine(striphist_x1, histsbottom, striphist_x1, histstop);
  line->DrawLine(striphist_x2, histsbottom, striphist_x2, histstop);

  // x-axis at the bottom
  axis->SetLabelSize(0.04);
  axis->SetLabelColor(1);
  axis->DrawAxis(striphist_x1, histsbottom, striphist_x2, histsbottom, 0, NSTRIP+2, 3206);
  
  line->SetLineColor(17);
  line->SetLineStyle(3);
  for(int i=1; i<NCFEB; ++i){
    double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
    double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
    line->DrawLine(cfebedge, histsbottom, cfebedge, histstop);
  }
  if(NCFEB==7){
    int i=4;
    double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
    double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
    line->SetLineColor(17);
    line->SetLineWidth(2);
    line->SetLineStyle(1);
    line->DrawLine(cfebedge, histsbottom-0.02, cfebedge, histstop+0.03);
    c->cd(1);
    text->SetTextSize(0.4);
    text->SetTextFont(82);
    text->SetTextAlign(23);
    text->SetTextColor(1);
    text->DrawText(0.35, 0.4, "ME1/1B");
    text->DrawText(0.80, 0.4, "ME1/1A");
  }
  c->Update();

  c = lmidcan->GetCanvas();
  c->cd();
  c->Clear();
  text->SetTextAlign(21);
  text->SetTextFont(82);
  text->SetTextSize(0.18);
  text->DrawText(0.5, histstop - 0.05, "Layer 1");
  text->DrawText(0.5, histsbottom + 0.05, "Layer 6");
  text->SetTextAngle(90);
  text->SetTextSize(0.25);
  text->SetTextAlign(23);
  text->DrawText(0.6, 0.50, "ADC Counts");
  c->Update();

  return;
}


void J_Display::display_wires_strips_peaks_cc()
{
  this->wires_strips_layout();
  this->update_banner("Wires & Strips (Peaks)");
  this->j_plot_wires();
  this->j_plot_strips_peaks();
}

void J_Display::j_plot_strips_peaks()
{
  int layer_no, i;
  this->scale_y();
  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.15, "X");
  gStyle->SetLabelSize(.1, "Y");
  for (i=1; i<=NLAYER; i++)
    {
      sprintf(buffer, "hist%d", i-1);
      if(hist[i-1]) delete hist[i-1];
      hist[i-1] = new TH1F(buffer, "", NSTRIP+2, 0, NSTRIP+1);
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      hist[layer_no-1]->SetFillStyle(1001);
      hist[layer_no-1]->SetFillColor(2);
      for (i=1; i<=NSTRIP; i++)
        {
          if (j_data.strips_peaks[i-1][layer_no-1]<0)
            {
              j_data.strips_peaks[i-1][layer_no-1] *= 0.1;
            }
          hist[layer_no-1]->Fill((double)i, j_data.strips_peaks[i-1][layer_no-1]);
        }
    }

  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      (hist[layer_no-1]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no-1], j_data.ymax[layer_no-1]);
      (hist[layer_no-1]->GetYaxis())->SetNdivisions(404);
      (hist[layer_no-1]->GetYaxis())->SetLabelSize(0.2);
      chist->cd(layer_no+1);
      gPad->Clear();
      hist[layer_no-1]->Draw();
      gPad->Update();
    }

  return;
}


void J_Display::display_sca()
{
  this->sca_layout();
  this->update_banner("CFEB Time Samples");
  this->plot_samples();
}

void J_Display::plot_samples()
{
  int layer_no, istrip, isample;
  float px[j_data.nsamples], py[j_data.nsamples];
  int lineStyle[5] = {9, 1, 2, 4, 3};
  int lineColor[5] = {2, 3, 1, 6, 4};
  if(text) delete text; text = new TText;

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
          if(graph[layer_no-1][istrip-1]) delete graph[layer_no-1][istrip-1];
          graph[layer_no-1][istrip-1] = new TGraph(j_data.nsamples, px, py);
        }
    }
  
  //draw graphs
  text->SetTextFont(82);
  text->SetTextSize(.13);
  text->SetTextAlign(11);
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      chist->cd(layer_no);
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

void J_Display::sca_layout()
{
  int i;
  if (firstTimeSca)
    {
      firstTimeSca = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      // banner
      bannerFrame = new TGCompositeFrame(fMain, 700, 100);
      fMain->AddFrame(bannerFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      bannercan = new TRootEmbeddedCanvas("title canvas", bannerFrame, 700, 70);
      bannerFrame->AddFrame(bannercan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      // body
      midFrame = new TGCompositeFrame(fMain, 700, 700);
      fMain->AddFrame(midFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      midFrame->SetLayoutManager(new TGHorizontalLayout(midFrame)); // to make side-by-side frames

      // legend
      lmidcan = new TRootEmbeddedCanvas("left canvas", midFrame, 120, 700);
      midFrame->AddFrame(lmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      // plots
      hmidcan = new TRootEmbeddedCanvas("histogram canvas", midFrame, 580, 700);
      midFrame->AddFrame(hmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();

      // divide canvas for individual layer plots
      chist = hmidcan->GetCanvas();
      chist->Divide(1, NLAYER+1, 0, 0);

      sca_label();
    }

}

void J_Display::sca_label()
{
  int layer_no;
  TCanvas *c;
  if(text) delete text; text = new TText;
  if(line) delete line; line = new TLine;
  text->SetTextFont(82);


  c = lmidcan->GetCanvas();
  c->cd();
  c->Clear();
  text->SetTextAlign(23);
  //create key
  float keytop=0.45;
  float keyheight = 0.22;
  float y;
  text->SetTextSize(0.2);
  text->DrawText(0.5, keytop, "Key");
  line->SetLineStyle(1);
  line->SetLineColor(1);
  y=keytop-keyheight*1/6;
  line->DrawLine(0.25, y, 0.75, y);
  text->SetTextAlign(12);
  text->SetTextSize(0.13);
  text->SetTextColor(2);
  y=keytop-keyheight*2/6;
  text->DrawText(0.05, y, "Peak-2");
  line->SetLineColor(2);
  line->SetLineStyle(9);
  line->DrawLine(0.7, y, 0.97, y);
  text->SetTextColor(3);
  y=keytop-keyheight*3/6;
  text->DrawText(0.05, y, "Peak-1");
  line->SetLineColor(3);
  line->SetLineStyle(1);
  line->DrawLine(0.7, y, 0.97, y);
  text->SetTextColor(1);
  y=keytop-keyheight*4/6;
  text->DrawText(0.05, y, "Peak Strip");
  line->SetLineColor(1);
  line->SetLineStyle(2);
  line->DrawLine(0.8, y, 0.97, y);
  text->SetTextColor(6);
  y=keytop-keyheight*5/6;
  text->DrawText(0.05, y, "Peak+1");
  line->SetLineColor(6);
  line->SetLineStyle(4);
  line->DrawLine(0.7, y, 0.97, y);
  text->SetTextColor(4);
  y=keytop-keyheight*6/6;
  text->DrawText(0.05, y, "Peak+2");
  line->SetLineColor(4);
  line->SetLineStyle(3);
  line->DrawLine(0.7, y, 0.97, y);
  c->Update();

  text->SetTextColor(1);
  text->SetTextAlign(22);
  text->SetTextSize(0.2);
  text->DrawText(0.5, 0.91, "Layer 6");
  text->DrawText(0.5, 0.09, "Layer 1");
  text->SetTextAngle(90);
  text->DrawText(0.8, 0.6, "ADC Counts");
  text->SetTextAngle(0);
  c->Update();


  //bottom label
  c = hmidcan->GetCanvas();

  // expand canvas for bottom layer to show x-axis
  c->cd(NLAYER);
  gPad->SetPad(gPad->GetXlowNDC(),gPad->GetYlowNDC()-gPad->GetHNDC()*0.1,gPad->GetXlowNDC()+gPad->GetWNDC(),gPad->GetYlowNDC()+gPad->GetHNDC());
  gPad->SetBottomMargin(0.1);

  // x-axis label
  c->cd(NLAYER+1);
  gPad->SetPad(0,0,1,0.1);
  text->SetTextAngle(0);
  text->SetTextSize(0.3);
  text->SetTextAlign(23);
  text->DrawText(0.47, 0.94, "CFEB Data Time Sample");
  c->Update();

  return;
}

void J_Display::display_ctrig()
{
  this->ctrig_layout();
  this->update_banner("Cathode Trigger Display");
  this->getsoft();
  this->plot_ctrig_strips();
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

  this->ctrig_scale_y();

  gStyle->SetOptStat(0);
  gStyle->SetLabelSize(.25, "X");
  gStyle->SetLabelSize(.2, "Y");
  gStyle->SetNdivisions(505, "Y");

  for (i=1; i<=NLAYER; i++)
    {
      sprintf(buffer, "ctrighist%d", i-1);
      if(ctrighist[i-1]) delete ctrighist[i-1]; 
      ctrighist[i-1] = new TH1F(buffer, "", NSTRIP+2, 0, NSTRIP+1);
    }
  
  //fill histograms
  for (layer_no=1; layer_no<=NLAYER; layer_no++)
    {
      ctrighist[layer_no-1]->SetFillStyle(1001);
      ctrighist[layer_no-1]->SetFillColor(2);
      for (i=1; i<=NSTRIP; i++)
        {
          if (j_data.strips[i-1][layer_no-1]<0)
            {
              j_data.strips[i-1][layer_no-1] *= .1;
            }
          ctrighist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1]);
        }
    }

  dx = (striphist_x2 - striphist_x1)/(ctrighist[0]->GetNbinsX());
  x0 = striphist_x1;

  for (i=1; i<=2*NLAYER; i++)
    {
      ctrigcan->cd(i+1);
      gPad->Clear();

      if (i%2==1)
        { //// Draw CFEB data
	  int index = (i-1)/2;
          (ctrighist[index]->GetYaxis())->SetRangeUser(j_data.ymin[index], j_data.ymax[index]);
          ctrighist[index]->Draw();
        }
      else
        { //// Draw trigger information
	  if(box) delete box;  box = new TBox;
	  float btop = 1.0;
	  float bsize = 0.66;
          for (j=1; j<=NSTRIP; j++)
            {
              x1 = x0 + j*dx;

	      box->SetLineColor(1);

	      // Software halfstrip from CFEB data
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

	      // Comparator halfstrips
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

	      // CLCT key halfstrip
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
        }
      gPad->Update();
    }
  
}

void J_Display::ctrig_scale_y()
{
  int i, layer_no;

  for (layer_no=1; layer_no<=6; layer_no++)
    {
      j_data.ymin[layer_no-1] = 0;
      j_data.ymax[layer_no-1] = 800;
      for (i=1; i<=NSTRIP; i++)
        {
          if (j_data.strips[i-1][layer_no-1]>j_data.ymax[layer_no-1])
            {
              j_data.ymax[layer_no-1] = j_data.strips[i-1][layer_no-1];
            }
        }
      j_data.ymax[layer_no-1] = j_data.ymax[layer_no-1] + .3*(j_data.ymax[layer_no-1] - j_data.ymin[layer_no-1]);
    }

  return;
}

void J_Display::update_banner(string title)
{
  if(text) delete text; text = new TText;
  if(box) delete box; box = new TBox;

  TCanvas *tempcan = bannercan->GetCanvas();
  tempcan->cd();
  tempcan->Clear();
  text->SetTextFont(102);
  text->SetTextAlign(21);

  text->SetTextSize(0.35);
  text->DrawText(0.5, 0.6, title.c_str());

  text->SetTextSize(0.25);
  // sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d", upevt_.run_number, upevt_.event_number, upevt_.alct_csc_id, upevt_.clct_csc_id);
  sprintf(buffer, "Run: %d   Event: %d          ALCT Chamber No:%d    CLCT Chamber No:%d",
                  upevt_.run_number, upevt_.event_number, getSelectedChamberID(), getSelectedChamberID());
  text->DrawText(0.5, 0.1, buffer);
  tempcan->Update();

  return;
}

void J_Display::ctrig_layout()
{
  gStyle->SetFillStyle(0);
  int i;
  if (firstTimeCtrig)
    {
      firstTimeCtrig = false;

      fMain->SetLayoutManager(new TGVerticalLayout(fMain));

      show_menus();

      // banner
      bannerFrame = new TGCompositeFrame(fMain, 700, 100);
      fMain->AddFrame(bannerFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      bannercan = new TRootEmbeddedCanvas("title canvas", bannerFrame, 700, 70);
      bannerFrame->AddFrame(bannercan, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

      // body
      midFrame = new TGCompositeFrame(fMain, 700, 700);
      fMain->AddFrame(midFrame, new TGLayoutHints(kLHintsExpandX,0,0,0,0));
      midFrame->SetLayoutManager(new TGHorizontalLayout(midFrame)); // to make side-by-side frames

      // legend
      lmidcan = new TRootEmbeddedCanvas("left canvas", midFrame, 90, 700);
      midFrame->AddFrame(lmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      // plots
      hmidcan = new TRootEmbeddedCanvas("histogram canvas", midFrame, 610, 700);
      midFrame->AddFrame(hmidcan, new TGLayoutHints(kLHintsExpandY, 0, 0, 0, 0));

      show_status_bars();

      fMain->MapSubwindows();
      fMain->Resize(fMain->GetDefaultSize());
      fMain->MapWindow();

      // Divide canvas for individual layer plots
      ctrigcan = hmidcan->GetCanvas();
      ctrigcan->Divide(1, NLAYER*2 +2, 0, 0);

      // set labels
      ctrig_label();
    }
}

void J_Display::ctrig_label()
{
  if(text) delete text; text = new TText;
  if(box) delete box; box = new TBox;
  if(line) delete line; line = new TLine;
  if(axis) delete axis;  axis = new TGaxis;
  TCanvas* c;

  c = lmidcan->GetCanvas();
  c->cd();
  c->Clear();
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
  c->Update();

  c = ctrigcan->GetCanvas();
  c->cd(2*NLAYER + 2);
  
  gPad->SetPad(gPad->GetXlowNDC(),0,gPad->GetXlowNDC()+gPad->GetWNDC(),1);
  gPad->SetFillStyle(4001);
  text->SetTextSize(0.03);
  text->SetTextFont(82);
  text->SetTextAlign(23);
  text->SetTextColor(1);
  text->DrawText(0.5, 0.05, "Strip Number");

  float histstop = 0.924;
  float histsbottom = 0.094;
  line->SetLineColor(1);
  line->SetLineWidth(1);
  line->SetLineStyle(1);
  line->DrawLine(striphist_x1, histsbottom, striphist_x1, histstop);
  line->DrawLine(striphist_x2, histsbottom, striphist_x2, histstop);

  // x-axis at the bottom
  axis->SetLabelSize(0.04);
  axis->SetLabelColor(1);
  axis->DrawAxis(striphist_x1, histsbottom, striphist_x2, histsbottom, 0, NSTRIP+2, 3206);
  
  line->SetLineColor(17);
  line->SetLineStyle(3);
  for(int i=1; i<NCFEB; ++i){
    double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
    double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
    line->DrawLine(cfebedge, histsbottom, cfebedge, histstop);
  }
  if(NCFEB==7){
    int i=4;
    double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
    double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
    line->SetLineColor(17);
    line->SetLineWidth(2);
    line->SetLineStyle(1);
    line->DrawLine(cfebedge, histsbottom-0.02, cfebedge, histstop+0.03);
    c->cd(1);
    text->SetTextSize(0.4);
    text->SetTextFont(82);
    text->SetTextAlign(23);
    text->SetTextColor(1);
    text->DrawText(0.35, 0.4, "ME1/1B");
    text->DrawText(0.80, 0.4, "ME1/1A");
  }
  c->Update();

  return;
}

void J_Display::display_clct_time()
{
  this->normal_layout();
  this->plot_cath_clct_time();
  this->update_banner("CLCT Time");

}

void J_Display::plot_cath_clct_time()
{
  int clct_time_tmp, shift_result;
  float x_coord[2];
  float y_coord[2];
  float x_shift, x_step, y_shift, y_step, y_time_step;
  int color, i, j, ij, ijk;
  int text_index[NLAYER][NSTRIP/2];
  char t[NLAYER][NSTRIP/2];
  char string[NLAYER/2];
  char temp[100];
  if(line) delete line; line = new TLine;
  if(text) delete text; text = new TText;
  TText *text_draw = new TText;

  x_shift = 0.01;
  x_step = 0.85/(NSTRIP/2);
  y_shift = 0.85;
  y_step = 0.6/(NLAYER*upevt_.clct_nbucket);
  y_time_step = (NLAYER+1)*y_step;

  cmain->cd();
  cmain->Clear();
  line->SetLineWidth(4);
  text->SetTextSize(0.040);
  text->SetTextFont(102);

  for (j=1; j<=NLAYER; j++)
    {
      for (i=1; i<=NSTRIP/2; i++)
        {
          text_index[j-1][i-1] = 0;
        }
    }

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
      sprintf(buffer, "%d", i);
      for (j=1; j<=NLAYER; j++)
        {
          x_coord[0] = x_shift + x_step;
          x_coord[1] = x_shift + (NSTRIP/2+1)*x_step;
          y_coord[0] = y_shift - i*y_time_step - j*y_step;
          y_coord[1] = y_coord[0];
          line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
          if (j==(NLAYER/2+2))
            {
              text->SetTextSize(0.04);
              text->DrawText(x_coord[1]+0.01, y_coord[0], buffer);
            }
          if ((j==NLAYER&&i==upevt_.clct_nbucket))
            {
	      text->SetTextAlign(22);
              text->SetTextSize(0.04);
              sprintf(buffer, "%d", 1);
              text->DrawText(x_coord[0]+0.005, y_coord[0]-0.03, buffer);
              sprintf(buffer, "%d", NSTRIP/2);
              text->DrawText(x_coord[1]-0.005, y_coord[0]-0.03, buffer);
	      text->DrawText((x_coord[1]-x_coord[0])/2, y_coord[0]-0.05, "Distrip Number");
            }
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

  if(text_draw) delete text_draw; text_draw=0;
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
    case M_WIRES_STRIPS_PEAKS: // firman
      sprintf(buffer, "run%d_event%d_id%d_STRIPS.png", upevt_.run_number, upevt_.event_number, activeDisp);
      chist->Print(buffer, "png");
      sprintf(buffer, "run%d_event%d_id%d_WIRES.png", upevt_.run_number, upevt_.event_number, activeDisp);
      cmain->Print(buffer, "png");
      break;
    case M_SCA:
      chist->Print(buffer, "png");
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
  // menu to select mode
  menuConfigure = new TGPopupMenu(gClient->GetRoot());
  menuConfigure->AddEntry("&Wires Strips", M_WIRES_STRIPS);
  menuConfigure->AddEntry("Wires Strips (&Peak)", M_WIRES_STRIPS_PEAKS); // firman
  menuConfigure->AddEntry("At&rig", M_ATRIG);
  menuConfigure->AddEntry("&ALCT time", M_ALCT_TIME);
  menuConfigure->AddEntry("&CLCT time", M_CLCT_TIME);
  menuConfigure->AddEntry("CFEB time &samples", M_SCA);
  menuConfigure->AddEntry("C&trig", M_CTRIG);

  menuConfigure->Connect("Activated(int)", "J_Display", this, "handle_menu(int)");

  // menu to select run mode
  menuRun = new TGPopupMenu(gClient->GetRoot());
  menuRun->AddEntry("&Select File", M_SELECT_FILE);
  menuRun->AddEntry("&Start", M_RUN);
  menuRun->AddEntry("&Pause", M_PAUSE);

  menuRun->Connect("Activated(int)", "J_Display", this, "handle_run_menu(int)");
  
  // menu to select chamber
  menuChambers = new TGPopupMenu(gClient->GetRoot());
  
  // get the chambers data by reading the first event
  std::vector<CSCEventData> chambersData;
  fd->readNextEvent();
  get_chambers_data(&chambersData, fd->data(), fd->dataLength());
  fd->close();
  fd->open(file_name);
  
  // creating entries according to chambers crate ID and DMB ID
  int nChambers = chambersData.size();
  for (int i = 0; i < nChambers; i++)
    {
      // getting chambers crate ID and DMB ID, then calculate chamber ID
      CSCEventData& data = chambersData[i];
      const CSCDMBHeader* dmbHeader = data.dmbHeader();
      int crateID       = dmbHeader->crateID();
      int dmbID         = dmbHeader->dmbID();
      int chamberID     = (((crateID) << 4) + dmbID) & 0xFFF;
      
      // add entry with chamber ID
      char buff1[100];
      sprintf(buff1, "Chamber %d", chamberID);
      const char* buff2 = buff1;
      menuChambers->AddEntry(buff2, chamberID);
      // any handler?
    }
  // add handler to chambers menu entry
  menuChambers->Connect("Activated(int)", "J_Display", this, "handle_chambers_menu(int)");
  
  menuBar = new TGMenuBar(fMain, 700, 20, kHorizontalFrame);
  menuBar->AddPopup("Con&figure", menuConfigure, new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0));
  menuBar->AddPopup("&Chambers", menuChambers, new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0));
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
  
  // select hack mode
  hackSelect = new TGCheckButton(ctrlFrame, "Hack mode");
  hackSelect->Connect("Toggled(bool)", "J_Display", this, "handle_hack_mode(bool)");

  lblTotalEvents = new TGLabel(ctrlFrame, Form("Total Events: %ld", total_events));
  ctrlFrame->AddFrame(lblTotalEvents, new TGLayoutHints(kLHintsCenterY));
  // ctrlFrame->AddFrame(new TGLabel(ctrlFrame, Form("Show Event: %ld", requested_event)), new TGLayoutHints(kLHintsBottom));
  ctrlFrame->AddFrame(hackSelect, new TGLayoutHints(kLHintsRight));
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
