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
#include "event_display_common.h"
#include "daq_module.h"
#include "daq_ipc.h"
#include "daq_conf.h"
}

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
int autoscale;

float timer_delay;  /* Number of seconds between display updates */
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




/*
 * Input: 0 <= irun < NRUN : Connnect shared ememory for that run
 *
*/
/*
extern "C" int connect_active_run(int irun)
{
	int imap;
	short buf[22];   // The size of the event header

	make_status_paused();

	detach_shmem();

	if (fd != NULL)
	{
		delete fd;
		fd = NULL;
	}

// Attach the event shared memory
	syslog(LOG_LOCAL1+LOG_INFO, "Attach event shared memory. II\n");
	if(evt_buf_ipc_setup(irun) != 0) {
		syslog(LOG_LOCAL1+LOG_ERR, "Attach event shared memory failed. EE\n");
		exit(1);
	}
	pevt = evt_shmem[irun]; // Pointer to the event shared memory

// Attach the DAQ conf shared memory
	syslog(LOG_LOCAL1+LOG_INFO, "Attach DAQ conf shared memory. II\n");
	if(daq_conf_ipc_setup(irun) != 0) {
		syslog(LOG_LOCAL1+LOG_ERR, "Attach DAQ conf shared memory failed. EE\n");
		exit(1);
	}
	pconf = daq_conf_shmem[irun]; // Pointer to the DAQ conf shared memory

// Read the cable map file
	if (fd == NULL)
	{
//		 Get cable map ID from DAQ conf shared memory
		imap = pconf->cable_map_file_id;
		printf("fd = NULL\n");
	}
	else {
		imap = 4000; // default cable map
	}
#ifdef DEBUG
	printf("cable_map_file_id = %d\n", imap);
#endif
	read_cable_map(imap);

	make_status_running();

	return 0;

}
*/
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

  // prepare and display file selector
//	FD_FSELECTOR * fds = fl_get_fselector_fdstruct();
//	fl_set_form_geometry(fds->fselect, 100, 100, 500, 500); // set larger size for longer filenames
//	fl_set_dirlist_sort(FL_RMTIMESORT); // reverse mod. time order, so latest files are on top
//	sel_fn = fl_show_fselector("Select a Data File", full_path, "*.raw", " ");

  // file selector here
//	sel_fn = "/data/csc_00000001_EmuRUI01_Test_12_ALCT_Connectivity_000_091016_153811_UTC.raw";
//	sel_fn = "/data/csc_00000002_EmuRUI00_STEP_12_000_071218_163953_UTC.raw";

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

  event_num = 1;

//	int sret = sscanf(file_name, "/data/csc_%08d", &run_num);
//	if (sret != 1) std::cout << "cannot read run number" << endl;

  /* Read the cable map file */
  /*	if (fd == NULL)
  	{
  #ifdef DEBUG
  		printf("Get cable map ID from shared memory\n");
  #endif
  		imap = pconf->cable_map_file_id;
  	}
  	else
  */
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
//		fl_set_timer(fd_event_display->show_event_timer, (double )timer_delay);
//		fl_show_object(fd_event_display->pause_button);
//		fl_show_object(fd_event_display->print_button);
    }
  else
    {
      if (disp_paused)
        {
          timer->Start(1000, kFALSE);   // 2 seconds
//			fl_resume_timer(fd_event_display->show_event_timer);
//			fl_hide_object(fd_event_display->continue_button);
//			fl_show_object(fd_event_display->pause_button);
//			fl_set_object_lcolor(fd_event_display->status_mess,FL_GREEN);
        }
      else
        {
          timer->Start(1000, kFALSE);   // 2 seconds
//			fl_set_timer(fd_event_display->show_event_timer, (double )timer_delay);
        }
    }

//	fl_set_object_lcolor(fd_event_display->status_mess,FL_GREEN);
//	fl_set_object_label(fd_event_display->status_mess,
//						"Event display running.");

  /* At start of new run we start by displaying all */
  disp_wire_strip = 1;
  disp_alct_time = 1;
  disp_clct_time = 1;
  disp_sca   = 1;
  disp_atrig = 1;
  disp_ctrig = 1;
  need_event = 1;

  disp_paused = 0;

  first = 0;

}

extern "C" void make_status_paused()
{

  if (disp_paused) return;
  /*
    fl_suspend_timer(fd_event_display->show_event_timer);
    fl_hide_object(fd_event_display->pause_button);
    fl_show_object(fd_event_display->continue_button);
    fl_set_object_lcolor(fd_event_display->status_mess,FL_BLUE);
    fl_set_object_label(fd_event_display->status_mess,
    "Event display paused");
  */
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
      event_num = 1;
      rewind_comm = 0;
    }

}


/*
  extern "C" void create_event_display() {
  if(wire_strip_active)
  make_wires_strips_hardcopy_(&autoscale);

  if(sca_active)
  make_sca_hardcopy_();

  if(atrig_active)
  make_atrig_hardcopy_();

  if(ctrig_active)
  make_ctrig_hardcopy_();

  if(sci_strips_active)
  make_scint_strips_hardcopy_();

  if(sci_wires_active)
  make_scint_wires_hardcopy_();

  if(alct_time_active)
  make_alct_time_hardcopy_();

  if(clct_time_active)
  make_clct_time_hardcopy_();

  }
*/

