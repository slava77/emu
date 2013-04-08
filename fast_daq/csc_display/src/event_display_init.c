#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include "daq_ipc.h"
#include "daq_conf.h"
#include "application.h"
//#include "csc_display_config.h"
#include "event_display_common.h"

extern daq_global_shmem_type *daq_global_shmem;

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
  disp_alct_time = 1;
  disp_clct_time = 1;
  disp_sca        = 1;
  disp_atrig      = 1;
  disp_ctrig      = 1;
  disp_sci_strips = 1;
  disp_sci_wires  = 1;
  disp_paused     = 1;

  wire_strip_active = 1;
  alct_time_active = 0;
  clct_time_active = 0;
  sca_active        = 0;
  atrig_active      = 0;
  ctrig_active      = 0;
  sci_strips_active = 0;
  sci_wires_active  = 0;
  autoscale = 1;

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
