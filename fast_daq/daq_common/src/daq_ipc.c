#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>
#include "daq_ipc.h"
#include "daq_conf.h"

/* int daq_global_ipc_setup() :
 *
 * Create shared memory segment to hold global information about
 * the DAQ. For example, information needed by the monitoring programs.
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author : David Chrisman
 * Date   : 21 Apr 1999
 * Mods   : v1.00 Initial version. 
 *          v1.10 If called by the server then initialize run numbers to -1.
 *
*/

key_t  daq_global_shmem_key;
key_t  evt_buf_shmem_key;
int    evt_buf_shmem_id[NRUN];
/* An array of pointers to the event shared memory data structure */
evt_shmem_type *evt_shmem[NRUN];
/* Variables for accessing the DAQ configuration shared memory */
key_t  daq_conf_shmem_key;
int    daq_conf_shmem_id[NRUN];

/* daq_conf_shmem_type declared in daq_conf.h */
daq_conf_shmem_type *daq_conf_shmem[NRUN];
int    daq_global_shmem_id;
daq_global_shmem_type *daq_global_shmem;
key_t daq_msg_queue_key;
key_t daq_msg_queue_key_tm; /*my ins -- for test manager*/

int   daq_msg_queue_id;
int daq_msg_queue_id_tm; /*my ins -- for test manager*/


int daq_global_ipc_setup()
{
  int i;
  int server = 1;

  /* Create a shared memory buffer for global information */
  int dirmade = mkdir(KEY_FILE, S_IRWXU|S_IRWXG);
  if( dirmade != 0 && errno != EEXIST ){
    perror("daq_global_ipc_setup : mkdir failed\n");
    printf("%d,%d: Please create this directory and try again: %s\n",dirmade, errno, KEY_FILE);
    return -1;
  }
  
  daq_global_shmem_key = ftok(KEY_FILE, DAQ_GLOB_SHMEM_ID_OFFSET);

  if( (int)daq_global_shmem_key < 0  ){
    perror("daq_global_ipc_setup : ftok failed\n");
    return -1;
  }

  daq_global_shmem_id =  shmget(daq_global_shmem_key, 
                                sizeof(daq_global_shmem_type),
                                0660 | IPC_CREAT | IPC_EXCL);
  if( daq_global_shmem_id < 0  ){
    /* Must already exist. Try to attach as client */
    server = 0;
    daq_global_shmem_id =  shmget(daq_global_shmem_key, 
                               sizeof(daq_global_shmem_type),
                               0660);
    if( daq_global_shmem_id < 0  ){
      perror("daq_global_ipc_setup : shmget failed\n");
      return -1;
    }
  }

  daq_global_shmem = (daq_global_shmem_type *)shmat(daq_global_shmem_id, 0, 0 );

  if( (int)daq_global_shmem == -1 ){
    perror("daq_global_ipc_setup : shmat failed\n");
    return -1;
  }

  /* If we are the server then initialize the array */
  if (server == 1) {
    for (i = 0; i < NRUN; i++) {
      daq_global_shmem->run_number[i] = -1;
    }
  }

  return 0;
}

/* int daq_global_ipc_clear() :
 *
 * Clear shared memory segments created in daq_global_ipc_setup().
 * This should be called by the run control program.
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author: David Chrisman
 * Date  : 14 Apr 1999
 * Mods  : v1.00 Initial version.     
 *
*/
int daq_global_ipc_clear()
{
  int rtval;

  rtval = shmctl(daq_global_shmem_id, IPC_RMID, (struct shmid_ds *) 0);

  if( rtval < 0  ){
    perror("daq_global_ipc_clear : shmctl IPC_RMID failed\n");
    return -1;
  }

  return 0;
}

/* int evt_buf_ipc_setup(int irun) :
 *
 * Sets up a shared memory buffer for each run.
 * The server is responsible to mark shared memory segments for removal
 * by calling evt_buf_ipc_clear() before exiting.
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author : David Chrisman
 * Date   : 20 Feb 1999
 * Mods   : v1.00 Initial version. 
 *          v1.10 D. Chrisman. Don't create all ipc structures at once.
 *                             Pass irun as an input variable.
 *
*/
int evt_buf_ipc_setup(int irun)
{

  /* irun can't be too large */

  if (irun >= NRUN) {
    perror("evt_buf_ipc_setup : irun too large\n");
    return -1;
  }

  /* Create a shared memory buffer for run = irun */

  evt_buf_shmem_key = ftok(KEY_FILE, EVT_BUF_SHMEM_ID_OFFSET + irun);

  if( (int)evt_buf_shmem_key < 0  ){
    perror("evt_buf_ipc_setup : ftok failed\n");
    return -1;
  }

  evt_buf_shmem_id[irun] =  shmget(evt_buf_shmem_key, 
                                     sizeof(evt_shmem_type),
                                     0660 | IPC_CREAT | IPC_EXCL);
  if( evt_buf_shmem_id[irun] < 0  ){
    /* Must already exist. Try to attach as client */
    evt_buf_shmem_id[irun] =  shmget(evt_buf_shmem_key, 
                                    sizeof(evt_shmem_type),
                                    0660);
    if( evt_buf_shmem_id[irun] < 0  ){
      perror("evt_buf_ipc_setup : shmget failed\n");
      return -1;
    }
  }

  evt_shmem[irun] = (evt_shmem_type *)shmat( evt_buf_shmem_id[irun], 0, 0 );

  if( (int)evt_shmem[irun] == -1 ){
    perror("evt_buf_ipc_setup : shmat failed\n");
    return -1;
  }

  return 0;
}

/* int evt_buf_ipc_clear(int irun) :
 *
 * Clear shared memory segments created in evt_buf_ipc_setup().
 * This should be called by the run control program.
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author: David Chrisman
 * Date  : 20 Feb 1999
 * Mods  : v1.00 Initial version.     
 *         v1.10 D. Chrisman. Don't clear all ipc structures at once.
 *                            Pass irun as an input variable.
 *
*/
int evt_buf_ipc_clear(int irun)
{
  int rtval;

  /* irun can't be too large */

  if (irun >= NRUN) {
    perror("evt_buf_ipc_clear : irun too large\n");
    return -1;
  }

  rtval = shmctl(evt_buf_shmem_id[irun], IPC_RMID, (struct shmid_ds *) 0);

  if( rtval < 0  ){
    perror("evt_buf_ipc_clear : shmctl IPC_RMID failed\n");
    return -1;
  }

  return 0;
}

/* int daq_conf_ipc_setup(int irun) :
 *
 * Sets up a shared memory buffer for global configuration data, like
 * lists of modules to be read out and cable maps. One is created for each 
 * run. The server is responsible to mark shared memory segments for removal
 * by calling daq_conf_ipc_clear().
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author: David Chrisman
 * Date  : 20 Feb 1999
 * Mods  : v1.00 Initial version. 
 *         v1.10 D. Chrisman. Don't create all ipc structures at once.
 *                            Pass irun as an input variable.
 *
*/
int daq_conf_ipc_setup(int irun)
  {

	  /* irun can't be too large */

	  if (irun >= NRUN) 
	  {
		  perror("daq_conf_ipc_setup : irun too large\n");
		  return -1;
	  }

	  daq_conf_shmem_key = ftok(KEY_FILE, DAQ_CONF_SHMEM_ID_OFFSET + irun+5);
 // +5 is to prevent using already created segment with different length   
	  if( (int)daq_conf_shmem_key < 0  )
	  {
		  perror("daq_conf_ipc_setup : ftok failed\n");
		  return -1;
	  }

	  printf("size: %d, key: %d\n",sizeof(daq_conf_shmem_type),
			 daq_conf_shmem_key);

	  daq_conf_shmem_id[irun] =  shmget(daq_conf_shmem_key, 
										sizeof(daq_conf_shmem_type),
										0660 | IPC_CREAT | IPC_EXCL);
	  if( daq_conf_shmem_id[irun] < 0  )
	  {
		  switch (errno)
		  {
		  case EINVAL:
			  printf ("shmget1 error: EINVAL\n"); fflush(stdout); break;
		  case EEXIST:
			  printf ("shmget1 error: EEXIST\n"); fflush(stdout); break;
		  case ENOSPC:
			  printf ("shmget1 error: ENOSPC\n"); fflush(stdout); break;
		  case ENOENT:
			  printf ("shmget1 error: ENOENT\n"); fflush(stdout); break;
		  case EACCES:
			  printf ("shmget1 error: EACCES\n"); fflush(stdout); break;
		  case ENOMEM:
			  printf ("shmget1 error: ENOMEM\n"); fflush(stdout); break;
		  }
		  /* Try to attach as client */
		  daq_conf_shmem_id[irun] =  shmget(daq_conf_shmem_key, 
											sizeof(daq_conf_shmem_type),
											0660);
		  if( daq_conf_shmem_id[irun] < 0  )
		  {
			  switch (errno)
			  {
			  case EINVAL:
				  printf ("shmget2 error: EINVAL\n"); fflush(stdout); break;
			  case EEXIST:
				  printf ("shmget2 error: EEXIST\n"); fflush(stdout); break;
			  case ENOSPC:
				  printf ("shmget2 error: ENOSPC\n"); fflush(stdout); break;
			  case ENOENT:
				  printf ("shmget2 error: ENOENT\n"); fflush(stdout); break;
			  case EACCES:
				  printf ("shmget2 error: EACCES\n"); fflush(stdout); break;
			  case ENOMEM:
				  printf ("shmget2 error: ENOMEM\n"); fflush(stdout); break;
			  }
			  perror("daq_conf_ipc_setup : shmget failed\n");
			  return -1;
		  }

	  }
  
	  daq_conf_shmem[irun] = 
		  (daq_conf_shmem_type *)shmat( daq_conf_shmem_id[irun], 0, 0 );

	  if( (int)daq_conf_shmem[irun] == -1 )
	  {
		  perror("daq_conf_ipc_setup : shmat failed\n");
		  return -1;
	  }

	  return 0;
  }

/* int daq_conf_ipc_clear(int irun) :
 *
 * Remove shared memory segments created in daq_conf_ipc_setup().
 * This should be called by the server.
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author: David Chrisman
 * Date  : 20 Feb 1999
 * Mods  : v1.00 Initial version.     
 *         v1.10 D. Chrisman. Don't clear all ipc structures at once.
 *                            Pass irun as an input variable.
 *
*/
int daq_conf_ipc_clear(int irun)
{
  int rtval = 0;

  /* irun can't be too large */

  if (irun >= NRUN) {
    perror("daq_conf_ipc_clear : irun too large\n");
    return -1;
  }

  rtval = shmctl(daq_conf_shmem_id[irun], IPC_RMID, (struct shmid_ds *) 0);

  if( rtval < 0  ){
    perror("daq_conf_ipc_clear : shmctl IPC_RMID failed \n");
    return -1;
  }

  return rtval;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* int create_msg_queue() :
 *
 * Create a message queue to be used for sending messages between 
 * the run control program and the CSC readout program.
 *
 * Sets global variable daq_msg_queue_id (see daq_ipc.h).
 *
 * return: ==  0 -> ok
 *       : == -1 -> error
 *
 * Author: David Chrisman
 * Date  : 30 Mar 1999
 * Mods  : v1.00 Initial version.     
 *
*/
int create_msg_queue()
{

  /* Create unique key via call to ftok() */

  daq_msg_queue_key = ftok(KEY_FILE, MSG_QUEUE_ID_OFFSET);

  if( (int)daq_msg_queue_key < 0  ){
    perror("create_msg_queue : ftok failed\n");
    return -1;
  }

  /* Open the queue - create if necessary */

  daq_msg_queue_id = msgget(daq_msg_queue_key, IPC_CREAT | 0660 | IPC_EXCL);

  if(daq_msg_queue_id < 0 ) {
    /* Try to open as client */
    daq_msg_queue_id = msgget(daq_msg_queue_key, 0660);
    if(daq_msg_queue_id < 0 ) {
      perror("create_msg_queue : msgget failed\n");
      return -1;
    }
  }

  return 0;

}
//+++++++++++++++++++++++++++ my ins +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int create_msg_queue_tm()
{

  /* Create unique key via call to ftok() */

  daq_msg_queue_key_tm = ftok(KEY_FILE, MSG_QUEUE_ID_OFFSET_TM);

  if( (int)daq_msg_queue_key_tm < 0  ){
    perror("create_msg_queue_tm : ftok failed\n");
    return -1;
  }

  /* Open the queue - create if necessary */

  daq_msg_queue_id_tm = msgget(daq_msg_queue_key_tm, IPC_CREAT | 0660 | IPC_EXCL);

  if(daq_msg_queue_id_tm < 0 ) {
    /* Try to open as client */
    daq_msg_queue_id_tm = msgget(daq_msg_queue_key_tm, 0660);
    if(daq_msg_queue_id_tm < 0 ) {
      perror("create_msg_queue_tm : msgget failed\n");
      return -1;
    }
  }

  return 0;

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* int send_message() : 
 *
 * Send a message to the queue.
 * 
 * input : type = 1 for messages between run_control and CSC_readout
 *       : msg  = START, STOP, PAUSE or CONTUNUE (defined in daq_ipc.h)
 *       : 0 <= irun <= NRUN
 *
 * return: ==       0 -> ok
 *       : ==  EAGAIN -> message queue if full (no message written)
 *       : ==      -1 -> error
 *
 * Author: David Chrisman
 * Date  : 13 Apr 1999
 * Mods  : v1.00 Initial version.     
 *       : v1.10 D. Chrisman, No longer pass pointer qbuf in arguement list.
 *       : v1.20 D. Chrisman, If the message queue is full, then the message 
 *                            is not written to the queue, and EAGAIN is 
 *                            returned.
 *       : v1.30 D. Chrisman, No longer pass qid in arguement list.
 *
*/
int send_message(long type, int msg, int irun)
{
  struct mymsgbuf qbuf;
  //create_msg_queue();

  qbuf.mtype = type;
  sprintf(qbuf.mtext, "%d %d", msg, irun);

  if((msgsnd(daq_msg_queue_id, (struct msgbuf *)&qbuf, strlen(qbuf.mtext)+1, IPC_NOWAIT)) == -1) {
    if(errno == EAGAIN) {
      return errno;
    }
    else {
      perror("send_message : msgsnd failed\n");
      return -1;     
    }
  }
  
  return 0;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//====== my_ins 

int send_message_tm(long type, int msg, int irun)
{
  struct mymsgbuf qbuf;
  //create_msg_queue();

  qbuf.mtype = type;
  sprintf(qbuf.mtext, "%d %d", msg, irun);

  if((msgsnd(daq_msg_queue_id_tm, (struct msgbuf *)&qbuf, strlen(qbuf.mtext)+1, IPC_NOWAIT)) == -1) {
    if(errno == EAGAIN) {
      return errno;
    }
    else {
      perror("send_message : msgsnd failed\n");
      return -1;     
    }
  }
  
  return 0;

}

//====== my_ins 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* int read_message() : 
 *
 * Read a message from the queue.
 *
 * input : type = 1 for messages between run_control and CSC_readout
 *
 * return: == 0      -> ok
 *       : == ENOMSG -> No message waiting on the queue.
 *       : == -1     -> error
 *       : msg  = START, STOP, PAUSE or CONTUNUE (defined in daq_ipc.h)
 *       : 0 <= irun <= NRUN
 *
 * Author: David Chrisman
 * Date  : 13 Apr 1999
 * Mods  : v1.00 Initial version.     
 *       : v1.10 D. Chrisman, No longer pass pointer qbuf in arguement list.
 *       : v1.20 D. Chrisman, No longer pass qid in arguement list.
 *
*/
int read_message(long type, int *msg, int *irun)
{

  struct mymsgbuf qbuf;

//create_msg_queue();
  qbuf.mtype = type;

  if((msgrcv(daq_msg_queue_id, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, type, IPC_NOWAIT)) == -1) {
    if(errno == ENOMSG) { 
      return errno;
    }
    else {
      perror("read_message : msgrcv failed\n");
      return -1;
    }
  } 

  sscanf(qbuf.mtext, "%d %d", msg, irun);

  return 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//======= my_ins 
int read_message_tm(long type, int *msg, int *irun)
{

  struct mymsgbuf qbuf;

//create_msg_queue_tm();
  qbuf.mtype = type;

  if((msgrcv(daq_msg_queue_id_tm, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, type, IPC_NOWAIT)) == -1) {
    if(errno == ENOMSG) { 
      return errno;
    }
    else {
      perror("read_message : msgrcv failed\n");
      return -1;
    }
  } 

  sscanf(qbuf.mtext, "%d %d", msg, irun);

  return 0;
}

/* int remove_msg_queue() : 
 *
 * Remove the message queue.      
 *
 * return: == 0  -> ok
 *       : == -1 -> error                            
 *
 * Author: David Chrisman
 * Date  : 30 Mar 1999
 * Mods  : v1.00 Initial version.     
 *
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int remove_msg_queue()
{

  if((msgctl(daq_msg_queue_id, IPC_RMID, 0)) == -1) {
    perror("remove_msg_queue : msgctl failed\n");
    return -1;
  }

  return 0;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// my ins
int remove_msg_queue_tm()
{

  if((msgctl(daq_msg_queue_id_tm, IPC_RMID, 0)) == -1) {
    perror("remove_msg_queue_tm : msgctl failed\n");
    return -1;
  }

  return 0; 

}
//========================================
// my_ins

#define MY_STRLEN  60
#define MY_BUFSIZ 10*BUFSIZ

 typedef  struct
  {
  long mtype;
  char mtext[MY_BUFSIZ];
  }message;
//======================
message mes;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void my_msg_send(char *m, int type){


int retv;
mes.mtype=type;

memcpy(mes.mtext,m,strlen(m));

// my_msg_control();
retv=msgsnd(daq_msg_queue_id,&mes,strlen(m),0);
//printf("retval++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%d id=%d mes=%s \n",retv,msgid,m); 

} 

//+++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++
int my_msg_receive_sync(char *m, int type){

  //msqid_ds stat;
int status;
mes.mtype=type;

//msgctl(msgid,IPC_STAT,&stat);
 status=msgrcv(daq_msg_queue_id,&mes,MY_BUFSIZ,type,0/*IPC_NOWAIT*/);
strcpy(m,mes.mtext);
return status;
}
//+++++++++++++++++++++++++++++++++
int my_msg_receive_async(char *m, int type){
  //msqid_ds stat;
int status;
mes.mtype=type;

//msgctl(msgid,IPC_STAT,&stat);
 status=msgrcv(daq_msg_queue_id,&mes,MY_BUFSIZ,type,IPC_NOWAIT);
strcpy(m,mes.mtext);
return status;
}
//+++++++++++++++++++++++++++++++++











































