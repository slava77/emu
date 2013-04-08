
#ifndef __MY_IPC_H__
#define __MY_IPC_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <time.h>
//#include <new.h>
// for semaphores , shared memory and messages
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>


extern int errno;
#define FLAGS 0600|IPC_CREAT|IPC_EXCL

const int MY_STRLEN= 60;
const int MY_BUFSIZ =5*BUFSIZ;

  
class my_msg{
public:
  int msgid; 
  struct message
  {
  long mtype;
  char mtext[MY_BUFSIZ];
  };


 void my_msg_send(char *m, int type);
 int my_msg_receive_sync(char *m, int type);
 int my_msg_receive_async(char *m, int type);
 void my_msg_control(int type);
  
  my_msg(const key_t &k, int type)
  {
  if( (msgid=msgget(k,FLAGS)) ==-1)
    {
    if(errno==EEXIST)
    msgid=msgget(k,0);
    else exit(0);    
    }
 my_msg_control(type); 
  }
 


};


class my_shm{
public:
  
int shmid;
char *shm_buf;
char *shm_cur; 
int sh_size;
int pointer;
//key_t k;
my_shm(key_t k) 
 {
   my_shm_get(k);   
 } 

void my_shm_get(key_t &k);
void my_shm_rm();
int my_scanf(char *s,char *d);
FILE *fopen(const char *path, const char *mode)
{
 pointer=0;
 return ::fopen(path,mode);
}

};


class my_sem{
public:
sembuf p_sembuf;
sembuf v_sembuf;
int semid;
key_t k;
 
my_sem()
 { 
 k=0x200;
 p_sembuf.sem_num=0;
 p_sembuf.sem_op=-1;
 p_sembuf.sem_flg=SEM_UNDO;

 v_sembuf.sem_num=0;
 v_sembuf.sem_op=1;
 v_sembuf.sem_flg=SEM_UNDO;

  if(initsem()<0)
  {perror("initsem"); exit(1);}

 }
int initsem();
int p();
int v();

};


class my_buf{
public:
//const int len=40;  
 int bufsize;
 char **buf;
 int pointer;
 bool full; 
 int number;
 
my_buf(int size): bufsize(size)
 {
   number=0;
   pointer=-1;
   full=false;
   my_buf_init(size); 
 } 

void my_buf_init(int size);
void my_buf_put(char* s);
char* my_buf_get(int n);
void my_buf_reset();
void operator+(char* s);
char* operator[](int n);

};

#endif
