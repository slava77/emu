/* daq_conf.h                         
 *
 * DAQ configuration data structures which are filled by the run_control
 * program. This data is then read by csc_readout.
 *
 * Author : Lisa Gorn      
 * Date   : 30 Mar 1999
 * Mods   : v1.00 L. Gorn. Initial version.
 *          v1.10 D. Chrisman. Seperate out everything having to do with
 *                             the event buffer and put them in daq_ipc.h.
 *          v1.20 L.Gorn+B.Smith. April 19.
 *                             Agree on new definition of "daq_conf_shmem_type"
 *                             so that most of the details are put into files.
 *
 */
#ifndef __DAQ_CONF_H__
#define __DAQ_CONF_H__

#include "misc.h"  /* logical */

/* trigger sources */
#define INTERNAL           0
#define CAMAC_LAM          1
#define DDU2_READY         2

/* trigger window options */
#define DURING_SPILLS      0
#define BETWEEN_SPILLS     1
#define TRIGGER_ANYTIME    2

#define FILE_NAME_LENGTH  80
/* make this user_length match the length in Run_Control_Window.java::
   keyTyped()   */
#define USER_LENGTH    40
#define SITE_LENGTH    20
#define VERSION_LENGTH 10
#define XML_FILE_NAME_LENGTH 300

typedef struct {
/* daq control */
  short version;       /* if version and revision are < 0 then it is a real */
  short revision;      /* versioned buffer, otherwise it should be */
                       /* interpreted according to v1.20 */

    int        run_number;
    int        max_events;
    logical    write_events_to_file;
    char       run_file_name[FILE_NAME_LENGTH];
    logical    real_data;
    logical    trig_and_pause;
 
/* configuration files */
  int module_config_file_id;
  int cable_map_file_id;
  int trig_setup_file_id;
  int afeb_config_file_id;
  int anode_pattern_id;
  int chamber_type_id;
  int anode_hot_mask_id;
  int anode_threshold_id;
  int anode_delay_id;
  int cathode_hot_mask_id;
  int cathode_threshold_id;

/* trigger setup */
    int        trig_source;  
    int        trig_window;
    logical    software_trig;
    logical    software_trig_random;
    float      software_trig_rate;

  char user[USER_LENGTH];
  char site[SITE_LENGTH];

	char xml_file_name[XML_FILE_NAME_LENGTH];
} daq_conf_shmem_type;

#endif  /* included already */


