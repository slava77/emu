/* daq_ipc.h
 *
 * The event shared memory buffer.
 *
 * Author : David Chrisman
 * Date   : 30 Mar 1999
 * Mods   : v1.00 D. Chrisman. Initial version.
 *          v1.10 D. Chrisman. Add information to struct evt_shmem_type
 *                             which came from Lisa's daq_conf.h file. 
 *          v1.20 L.Gorn+B.Smith. April 19.
 *                             Add the list of constants needed
 *                             for message queue communication
 *                             between the run and readout control
 *                             programs.
 *                            WARNING: because of the C-JAVA interface,
 *                             any change in the values of the "#define"s
 *                             must be emailed to bryan.smith@cern.ch; obviously,
 *                             there needs to be a more automatic way of doing
 *                             this.  Under investigation.
 *          v1.30 B.Smith, June 28.
 *                             Add list of contants needed for
 *                             message queue communication between
 *                             run control and analysis programs.
 *
 */
#ifndef __DAQ_IPC_H__
#define __DAQ_IPC_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "daq_conf.h"
#include "misc.h"  /* logical */

#define NRUN 4                   /* Number of concurrent runs, must range between 1 to 10 */
#define DATA_BUF_LEN 0x800000    /* Length in 16-bit words of data buffer */
#define PTR_BUF_LEN   0x40000    /* Length of the event pointer array */
#define KEY_FILE "/tmp"          /* Directory to use in ftok calls */

/***************************************************************************/
/* Offsets of project id's to be used in calls to ftok. This is to insure
 * that we have unique id's for all ipc structures. 
 */
#define EVT_BUF_SHMEM_ID_OFFSET (char)0
#define DAQ_CONF_SHMEM_ID_OFFSET (char)(NRUN)
#define DAQ_GLOB_SHMEM_ID_OFFSET (char)(NRUN*2)
#define MSG_QUEUE_ID_OFFSET (char)(NRUN*3)

/***************************************************************************/
/* Messages received by readout control */

#define INITIALIZE_READOUT            2
#define      START_READOUT            3
#define      PAUSE_READOUT            5
#define   CONTINUE_READOUT            6
#define       STOP_READOUT            7


/* Messages sent by readout control and-or constants for run state.*/
/*   NOTE: only the "ERROR"s and the "xxxED" are sent back to      */
/*         run control, none of the "xxxING" are sent back.        */

#define READOUT_INITIALIZING            2
#define READOUT_INITIALIZED            12
#define READOUT_INITIALIZATION_ERROR  102

#define READOUT_STARTING                3
#define READOUT_START_ERROR           103

#define READOUT_RUNNING                 4
#define READOUT_RUNNING_ERROR         104

#define READOUT_PAUSING                 5
#define READOUT_PAUSED                 15
#define READOUT_PAUSE_ERROR           105


#define READOUT_CONTINUING              6
#define READOUT_CONTINUE_ERROR        106

#define READOUT_STOPPING                7
#define READOUT_STOPPED                17
#define READOUT_STOP_ERROR            207


#define READOUT_MAX_EVENTS_PAUSE      201
#define READOUT_TRIG_AND_PAUSE        202
#define READOUT_RUNNING_ERROR_PAUSE   203


#define READOUT_RECEIVED_UNEXPECTED_MESSAGE 77777


/* Here are some examples of some message pairs:

run control sends:         allowed readout control answers:
========================================================================
 INITIALIZE_READOUT        READOUT_INITIALIZED  or READOUT_INITIALIZATION_ERROR
      START_READOUT        READOUT_RUNNING      or READOUT_START_ERROR
      PAUSE_READOUT        READOUT_PAUSED       or READOUT_PAUSE_ERROR 
   CONTINUE_READOUT        READOUT_RUNNING      or READOUT_CONTINUE_ERROR
       STOP_READOUT        READOUT_STOPPED      or READOUT_STOP_ERROR

NOTE: there are some special cases, the readout control program may
       send out three messages to the run control program without
       needing a reply:
                        READOUT_MAX_EVENTS_PAUSE   ,
                        READOUT_TRIG_AND_PAUSE     , or
                        READOUT_RUNNING_ERROR_PAUSE.

      Now, at the very same time the readout control is sending out
      a special pause message, the run control might be sending  
      PAUSE_READOUT or a STOP_READOUT.  Clearly, the readout control
      will respond to the STOP_READOUT; however, it is still under
      discussion what the response of the readout control to a 
      PAUSE_READOUT when it is already paused will be.
                                                              B.Smith
 */



/* Additional "msg queue control constants" needed for the java - c interface:*/
#define NO_MESSAGE_WAITING                 88888
#define TIMEOUT_WAITING_FOR_MESSAGE_ERROR  99999
#define MICROSEC_SLEEP_TILL_UPDATE         30000

/* should be at least one "spill" wide, i.e. about a 12 second wait. */
#define MAX_UPDATES_UNTIL_TIMEOUT             400

#define  FIRST_RUN_CONTROL   0 
#define SECOND_RUN_CONTROL   1 
#define  THIRD_RUN_CONTROL   2 
#define FOURTH_RUN_CONTROL   3 

#define    FROM_RUN_CTRL_TO_READOUT_CTRL 1
#define    FROM_READOUT_CTRL_TO_RUN_CTRL 2
#define SPECIAL_READOUT_CTRL_TO_RUN_CTRL 3

#define FROM_RUN_CTRL_TO_MONITOR 100 /* used for inter-JAVA JVM communication */

#define FROM_RUN_CTRL_TO_ANALYSIS  200

#define ANALYSIS_PAUSE                   1
#define ANALYSIS_CONTINUE                2
#define ANALYSIS_CLEAR_HISTOGRAMS        3
#define ANALYSIS_UPDATE_ONLINE_PEDESTALS 4

/***************************************************************************/
/* Variables for accessing the event shared memory */
key_t  evt_buf_shmem_key;
int    evt_buf_shmem_id[NRUN];

typedef struct
{
   int        run_number;
   int        event_number;
   int        spill_number;
   int        run_state;         
   int        i_first_event;
   int        i_last_event;
   int        i_first_event_of_spill;
   int        i_first_unwritten_event;
   int        ptr_buffer_length;
   int        data_buffer_length;
   short int  *data_buffer_top;
   short int  *loc_event[PTR_BUF_LEN];
   short int  cirbuf[DATA_BUF_LEN];

} evt_shmem_type ;

/* An array of pointers to the event shared memory data structure */
evt_shmem_type *evt_shmem[NRUN];

/***************************************************************************/
/* Variables for accessing the DAQ configuration shared memory */
key_t  daq_conf_shmem_key;
int    daq_conf_shmem_id[NRUN];

/* daq_conf_shmem_type declared in daq_conf.h */
daq_conf_shmem_type *daq_conf_shmem[NRUN];

/***************************************************************************/
/* Variables for accessing the DAQ global shared memory */
key_t  daq_global_shmem_key;
int    daq_global_shmem_id;

typedef struct
{
   int run_number[NRUN];
   int in_spill;

}daq_global_shmem_type ;

daq_global_shmem_type *daq_global_shmem;

/***************************************************************************/
/* Variables for accessing the message queue */
#define MAX_SEND_SIZE 80

struct mymsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};


key_t daq_msg_queue_key;
int   daq_msg_queue_id;

/***************************************************************************/
/* function declarations */

/*
int daq_global_ipc_setup();
int daq_global_ipc_clear();

int evt_buf_ipc_setup(int irun);
int evt_buf_ipc_clear(int irun);

int daq_conf_ipc_setup(int irun);
int daq_conf_ipc_clear(int irun);

int create_msg_queue();
int remove_msg_queue();
int send_message(long type, int msg, int irun);
int read_message(long type, int *msg, int *irun);
*/
// my_ins
int mysem_initsem();
int mysem_p();
int mysem_v();

#endif  /* included already */

