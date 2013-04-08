
#include <test_ipc.h>
 
//++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++   
//+++++++++++++++++++++++++++++++
void my_msg::my_msg_send(char *m,int type){
message mes;
mes.mtype=1;
int retv; 

mes.mtype=type;

memcpy(mes.mtext,m,strlen(m));

// my_msg_control();
retv=msgsnd(msgid,&mes,strlen(m),0);
printf("retval++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%d id=%d mes=%s \n",retv,msgid,m); 

} 

//+++++++++++++++++++++++++++++++++
void my_msg::my_msg_control(int type){
msqid_ds stat;
int i,n;
//message mes;
char buf[MY_BUFSIZ];

msgctl(msgid,IPC_STAT,&stat);
 if((n=stat.msg_qnum)>0){
   for(i=0;i<n;i++){
   my_msg_receive_async(buf,type);
   }
 }
}


//+++++++++++++++++++++++++++++++
int my_msg::my_msg_receive_sync(char *m,int type){
message mes;
int status;

mes.mtype=type;

//msgctl(msgid,IPC_STAT,&stat);
 status=msgrcv(msgid,&mes,MY_BUFSIZ,type,0/*IPC_NOWAIT*/);
strcpy(m,mes.mtext);
return status;
}
//+++++++++++++++++++++++++++++++++
int my_msg::my_msg_receive_async(char *m,int type){
message mes;
int status;

mes.mtype=type;

//msgctl(msgid,IPC_STAT,&stat);
 status=msgrcv(msgid,&mes,MY_BUFSIZ,type,IPC_NOWAIT);
strcpy(m,mes.mtext);
return status;
}
//+++++++++++++++++++++++++++++++++




//+++++++++++++++++++++++++++++++++++++++++++++++//
//////////////////////SHARED MEMORY/////////////

int my_shm::my_scanf(char *s, char d[MY_STRLEN])
{
 bool oneatleast=false; 
 int count=0; 
 int countbefor=0; 
 int p; 
 
// d=(char *)malloc(MY_STRLEN); 
 while(1){ 
 p=pointer+countbefor+count; 
 if(*(s+p)=='\0'){pointer=0;return EOF;} 
 if((*(s+p))==' '||(*(s+p))=='\n') 
 { 
  if(oneatleast)
  {  
       memcpy(d,(s+pointer+countbefor),(count));     
       pointer+=(count+countbefor);     
       *(d+count)='\0';
       return count;
  } 
  else
  {
   countbefor++;
  } 
 }  // end of if(whitespace or \n ) 
 else
 {
  oneatleast=true;
  count++; 
 }
         } // end of while
}


void my_shm::my_shm_get(key_t &k)
{
   pointer=0; 
   shm_buf=(char *)malloc(MY_BUFSIZ);
 
// if((shmid=shmget(k,MY_BUFSIZ,FLAGS))<0)
// {printf(" can not get shm segment");exit(1);}   

// if((shm_buf= (char *)shmat(shmid,0,0))==(char *)-1)
//  {printf(" can not attach shm segment");exit(1);}   

}
void my_shm::my_shm_rm()
{
 shmctl(shmid,IPC_RMID,(struct shmid_ds *)0);  

}
//===========++++ +++++++++  ===========  ++++ +++++++++++  ========
//MY_SEM  MY_SEM  MY_SEM  MY_SEM  MY_SEM  MY_SEM  MY_SEM  
/* 

int my_sem::initsem()
{
int status;
semun p;

p.val=1;  

if(  (semid=semget(k,1,FLAGS)) ==-1)
 { if(errno==EEXIST)semid=semget(k,1,0);}
else 
  status=semctl(semid,0,SETVAL,p); //to open the semaphore

if(semid==-1 || status == -1)
{perror("initsem failed"); return -1;}
else 
return semid;

}

int my_sem::p()
{
if(semop(semid,&p_sembuf,1)==-1)
 {
   perror("p(semid) failed");   
 }
else return 0;
}

int my_sem::v()
{
if(semop(semid,&v_sembuf,1)==-1)
 {
   perror("v(semid) failed");    
 }
else return 0;
}
*/
//MY_BUF MY_BUF MY_BUF MY_BUF MY_BUF MY_BUF MY_BUF MY_BUF 


void my_buf::my_buf_init(int size){
  int i;
  printf("   %d\n",sizeof(buf));
  buf=(char **) malloc(size*sizeof(buf)); 
  for(i=0;i<size;i++)
//  *(buf+i)=new char[MY_STRLEN];   
  *(buf+i)=(char *) malloc(5*BUFSIZ); 
} 
//+++++++++++++++++++++++++++++++++++++++++

void my_buf::operator+(char* s){
my_buf_put(s);
}

char* my_buf::operator[](int n){
return (char *)my_buf_get(n);
}


void my_buf::my_buf_put(char *s){
      
 number++;
 if(pointer==(bufsize-1))
   {
   full=true;
   pointer=0;
   } 
  else pointer++;  
   strcpy(*(buf+pointer),s);   
//  printf("  %s %d \n",s,pointer);          
}



char* my_buf::my_buf_get(int n){
   
  if(full)
   {
  if(n>(bufsize-1))return NULL;                
  if(n<=pointer)return buf[pointer-n]; 
  else return buf[bufsize+pointer-n];  
   } 
  else
   {
  if(n>pointer)return NULL;
  return buf[pointer-n];  
   }
}
void my_buf::my_buf_reset(){
   pointer=-1;
   full=false;
}     
//======================================
