#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <iostream>
extern "C"
{
#include "application.h"
#include "daq_module.h"
#include "daq_ipc.h"
#include "daq_conf.h"
}

extern daq_global_shmem_type *daq_global_shmem;

// IMPORTANT: libemudaqreader must be compiled in standalone mode with "make Standalone=yes" 
// and then the resulting *sa_.so file copied to this project lib directory.
#include "emu/daq/reader/RawDataFile.h"
#include "j_common_data.h"

#include "TTimer.h"
using namespace std;


// globals
//FD_event_display *fd_event_display;
//FD_config        *fd_config;
//FD_print         *fd_print;

/* Flags to control the event display (see cdShowEvent() ) */

int need_event;
int need_refresh;
bool change_chamber;
int re_read_peds;
long total_events(0);
unsigned long requested_event;
int disp_wire_strip, wire_strip_active;
int disp_alct_time, alct_time_active;
int disp_clct_time, clct_time_active;
int disp_sca,               sca_active;
int disp_atrig,           atrig_active;
int disp_ctrig,           ctrig_active;
int disp_sci_strips, sci_strips_active;
int disp_sci_wires,   sci_wires_active;

int disp_paused;  /* Equals 1 if display is paused */
int rewind_comm; // set by rewind button, reset by rewind action

/* Pointers to the shared memory */

daq_global_shmem_type *pglobal;
daq_conf_shmem_type   *pconf;
evt_shmem_type        *pevt;

emu::daq::reader::RawDataFile *fd = NULL;
extern long event_num;
int run_num = 0;

extern "C" int read_cable_map(int cable_map_file_id);

extern evt_shmem_type *evt_shmem[NRUN];
extern daq_conf_shmem_type *daq_conf_shmem[NRUN];



char file_name[1000];

int connect_data_file(char *sel_fn)
{
  int imap;
  short buf[20];
  char *path;
  char full_path[500];
  struct stat stats;

  make_status_paused();

  if ((path = getenv("DATADIR")) == NULL)
    {
      syslog(LOG_LOCAL1+LOG_ERR,"Can\'t open data file. DATADIR not set. EE\n");
      std::cout << "Can\'t open data file. Environment variable DATADIR not set" << std::endl;
      return 1;
    }

  if (sel_fn == NULL) return 0;
  sprintf(file_name, "%s", sel_fn);

#ifdef DEBUG
  printf("Open file %s\n", file_name);
#endif

  /* If we have shared memory or a file open we should close it now */
  detach_shmem();
  // RawDataFile
  if (fd != NULL)
    {
      delete fd;
      fd = NULL;
    }

  fd = new emu::daq::reader::RawDataFile (file_name, emu::daq::reader::RawDataFile::DDU);

  scan_data_file(file_name);

  event_num = 0;

  {
#ifdef DEBUG
    printf("Get cable map ID from the data file\n");
#endif
    imap = 4000; // take default cable map
  }
#ifdef DEBUG
  printf("cable_map_file_id = %d\n", imap);
#endif
  read_cable_map(imap);

  requested_event = 0; // start file from beginning

// file opens in paused condition
  make_status_running(); // this is necessary so the Run button shows up
  make_status_paused();
//  	fl_show_object(fd_event_display->help_button); // show rewind

  return 0;

}

extern TTimer *timer;

extern "C" long scan_data_file(const char * fname)
{
  total_events = 0;
  if (fd != NULL)
    {
      fd->open(fname);
      std::cout << "==> Scan data file: " << fname << std::endl;
      while (fd->readNextEvent()) total_events++;
      std::cout << "Total # events: " << total_events << std::endl;
      fd->close();
      fd->open(fname);
    }
  return total_events;

}


extern "C" void make_status_running()
{
  static int first = 1;

  std::cout << "==> Switching to running status" << std::endl;

  /* Start the event display timer. Events will be displayed every
   * timer_delay seconds.
   */

  if (first)
    {
      timer->Start(1000, kFALSE);   // 2 seconds
    }
  else
    {
      if (disp_paused)
        {
          timer->Start(1000, kFALSE);   // 2 seconds
        }
      else
        {
          timer->Start(1000, kFALSE);   // 2 seconds
        }
    }

  /* At start of new run we start by displaying all */
  disp_wire_strip = 1;
  disp_alct_time = 1;
  disp_clct_time = 1;
  disp_sca   = 1;
  disp_atrig = 1;
  disp_ctrig = 1;
  need_event = 1;
  change_chamber = false;
  need_refresh = 1; // firman

  disp_paused = 0;

  first = 0;

}

extern "C" void make_status_paused()
{
  disp_paused = 1;
}

extern "C" void detach_shmem()
{
  if (pconf != NULL) {
    if(shmdt(pconf) != 0) {
      syslog(LOG_LOCAL1+LOG_ERR,
	     "Failed to detach DAQ conf shared memory. EE\n");
      perror("shmdt");
    }
    pconf = NULL;
  }

  if (pevt != NULL) {
    if(shmdt(pevt) != 0) {
      syslog(LOG_LOCAL1+LOG_ERR,
	     "Failed to detach event shared memory. EE\n");
      perror("shmdt");
    }
    pevt = NULL;
  }
}

extern "C" void reset_data_file()
{
  if (fd != NULL)
    {
      std::cout << "==> Resetting data file " << file_name << std::endl;
      fd->close();
      // RawDataFile
      fd->open(file_name);
      event_num = 0;
      rewind_comm = 0;
    }

}

int event_display_global_init(int argc, char *argv[])
{
  char *print_command;

  /* Start message logging */
  openlog(argv[0], LOG_NDELAY, LOG_LOCAL1);
  syslog(LOG_LOCAL1|LOG_NOTICE, "Event display program starting. NN\n");


  /* Attach the DAQ global shared memory */
  syslog(LOG_LOCAL1|LOG_INFO, "Attach DAQ global shared memory. II\n");
  if(daq_global_ipc_setup() != 0) {
    syslog(LOG_LOCAL1|LOG_ERR, "Attach DAQ global shared memory failed. EE\n");
    exit(1);
  }
  pglobal = daq_global_shmem;

  /* Get list of active runs from DAQ global shared memory */
  //	get_run_list(fd_event_display->file_menu, 0);

  /* Set the default print command */
  print_command = getenv("PRINT_CMD");
  //	fl_set_input(fd_print->print_command, print_command);

  /* Hide the pause and continue buttons */
  //	fl_hide_object(fd_event_display->pause_button);
  //	fl_hide_object(fd_event_display->continue_button);
  //	fl_hide_object(fd_event_display->print_button);

  /*
   * Initialize more event display stuff
   */
  re_read_peds = 0;

  disp_wire_strip = 1;
  disp_alct_time  = 1;
  disp_clct_time  = 1;
  disp_sca        = 1;
  disp_atrig      = 1;
  disp_ctrig      = 1;
  disp_sci_strips = 1;
  disp_sci_wires  = 1;
  disp_paused     = 1;
  need_refresh    = 1; // firman

  change_chamber    = false; // firman
  wire_strip_active = 1;
  alct_time_active  = 0;
  clct_time_active  = 0;
  sca_active        = 0;
  atrig_active      = 0;
  ctrig_active      = 0;
  sci_strips_active = 0;
  sci_wires_active  = 0;

  pevt  = NULL;
  pconf = NULL;

  //  fd = -1;

  //	timer_delay = fl_get_counter_value(fd_config->timer_delay);

  //	init_display_();
  //  wire_geom_();
  //	wire_geom_3_();
  //	atrig_wire_geom_();
  //	scint_geom_();


  /*
   *atrig_wire_geom_();
   *read_clct_patterns_();
   *read_alct_patterns_();
   */

#ifdef DEBUG
  printf("EventDisplay : event_display_global_init finished\n");
#endif

  return 0;
}
