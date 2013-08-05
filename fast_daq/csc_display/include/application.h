/** Header file generated with fdesign on Sun May  2 11:07:15 1999.**/

#ifndef __APPLICATION_H__  
#define __APPLICATION_H__  

#include "daq_ipc.h"
//#include "forms.h"
//#include "csc_display_config.h"
//#include "print.h"
//#include "event_display.h"


#define DEFAULT_TIMER_DELAY 2

/* Pointers to the forms */

//extern FD_event_display *fd_event_display;
//extern FD_config        *fd_config;
//extern FD_print         *fd_print;

//Flags to control the event display (see cdShowEvent() )

extern int need_event;
extern int need_refresh; // firman
extern bool change_chamber; // firman
extern int re_read_peds;
extern long total_events; //v
extern long event_num; //v
extern unsigned long requested_event;
extern int disp_wire_strip, wire_strip_active;
extern int disp_alct_time, alct_time_active;
extern int disp_clct_time, clct_time_active;
extern int disp_sca,               sca_active;
extern int disp_atrig,           atrig_active;
extern int disp_ctrig,           ctrig_active;
extern int disp_sci_strips, sci_strips_active;
extern int disp_sci_wires,   sci_wires_active;
extern char file_name[1000]; //v

extern float timer_delay;  /* Number of seconds between display updates */
extern int disp_paused;  /* Equals 1 of display is paused */

// Pointers to the shared memory

extern daq_global_shmem_type *pglobal;
extern daq_conf_shmem_type   *pconf;
extern evt_shmem_type        *pevt;
 
extern int run_num;

/* Function prototypes */

int event_display_global_init(int argc, char *argv[]);
void detach_shmem();
long scan_data_file(const char* fname); //v
void make_status_running();
void make_status_paused();
int connect_active_run(int irun);
int connect_data_file(char * fn);
void create_event_display();
void reset_data_file(); //v

#endif /* __APPLICATION_H__ */
