//----------------------------------------------------------------------
// $Id: VMEController.cc,v 1.1 2005/08/31 18:20:32 gilmore Exp $
// $Log: VMEController.cc,v $
// Revision 1.1  2005/08/31 18:20:32  gilmore
// *** empty log message ***
//
// Revision 1.25  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEController.h"
#include "VMEModule.h"
#include "Crate.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/if_ether.h> 
#include <netinet/in.h> 
#include <netinet/ip.h> 
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <netpacket/packet.h>
#include <errno.h>
#include <string.h>



#include <unistd.h>
#include "eth.h"



#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif

extern char eth[5];
static int uid,uid2;
unsigned char hw_source_addr[ETH_ALEN];
unsigned char hw_dest_addr[ETH_ALEN] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
struct ethhdr ether_header; 

#define MAX_DATA 8990
//#define MAX_DATA 1400
#define VME_CMDS 0x20
#define ACNLG 0x20
#define ACNLG_LOOP 0x60
#define LOOP_CNTRL 0xff
#define ATYPE 2    // VME A24 bit transfer
#define TSIZE 1    // VME 16 bit data transfer
#define TTYPE 0    // VME single transfer mode
#define PACKETOUTDUMP 0   // to print dump set 1

extern int delay_type;

char a_mask[8]={0x00,0x20,0x40,0x50,0x80,0x90,0x00,0x00};
char r_mask=0x00;
char w_mask=0x10;
char ts_mask[4]={0x00,0x04,0x08,0x0c};
char ts_size[4]={1,2,4,8};
char tt_mask[4]={0x00,0x01,0x02,0x03};;
char delay_mask[8]={0,1,2,3,4,5,6,7};
int iprint={0};
unsigned short int header={0};



unsigned short int LRG_read_flag={0};
unsigned short int LRG_read_pnt={0};
unsigned short int LRG_read_flag2={0};

char wbuf[9000];
int nwbuf;
char rbuf[9000];
int nrbuf;




VMEController::VMEController(string ipAddr, int port): 
  theSocket(0), ipAddress_(ipAddr), port_(port), theCurrentModule(0),indian(SWAP)
{
int i;


  int socket = openSocket();
  cout << "VMEController opened socket = " << socket << endl;
}


VMEController::~VMEController(){
  cout << "destructing VMEController .. closing socket " << endl;
   setuid(0);
   closeSocket();
   setuid(uid);
}


void VMEController::start(VMEModule * module) {
  if(theCurrentModule != module) {
    PRINTSTRING(OVAL: start method defined in VMEController.cc is starting )
    end();
    PRINTSTRING(OVAL: starting current module);
    module->start();
    PRINTSTRING(OVAL: current module was started);
    theCurrentModule = module;
  }
}



void VMEController::end() {
  if(theCurrentModule != 0) {
    theCurrentModule->end();
    theCurrentModule = 0;
  }
  assert(plev !=2);
  idevo = 0;
  feuseo = 0;
}


void VMEController::send_last() {
}


int VMEController::openSocket() {

  theSocket = open("/dev/schar", O_RDWR);
        if (theSocket == -1) {
                perror("open");
                return 1;
        }
  	eth_enableblock();   
        get_macaddr();
  return theSocket;
}
int VMEController::eth_read()
{  int err;
int size;
int loopcnt;
 
 loopcnt=0;
 size=0;
 GETMORE: 
 size=read(theSocket,rbuf,nrbuf);
        if(size<0)return size;
        if(size<7){
           if(rbuf[0]==0x03&&loopcnt<10){usleep(1000);loopcnt=loopcnt+1;goto GETMORE;}
        }
   return size;
}


int VMEController::eth_write()
{  char *msg;
  int msg_size;
  int nwritten;
  int i;
   //Creating the packet
     ether_header.h_proto = htons(nwbuf);
     //   ether_header.h_proto = htons(0xfff);

   msg_size = sizeof(ether_header) + nwbuf;
   if((msg = (char *)malloc(msg_size*sizeof(unsigned char))) == NULL){ 
           perror("rp_send: main(): malloc(): No memory available");
           exit(1);
   }
   memcpy(msg, &ether_header, sizeof(ether_header));
   memcpy(msg + sizeof(ether_header), wbuf, nwbuf);   
   // printf("****");for(i=0;i<msg_size;i++)printf("%02x",msg[i]&0xff);printf("\n");
   //   printf("Len : %4d\n",((msg[12]&0xff)<<8)|(msg[13]&0xff));
   nwritten = write(theSocket, (const void *)msg, msg_size);
   free(msg);
   return nwritten; 

}
int VMEController::eth_enableblock(void)
{
  if(ioctl(theSocket,SCHAR_BLOCKON)==-1){
    printf(" error in SCHAR_BLOCKON \n");
  }
  return 0;
}

int VMEController::eth_disableblock(void)
{
  if(ioctl(theSocket,SCHAR_BLOCKOFF)==-1){
    printf(" error in SCHAR_BLOCKOFF \n");
  }
  return 0;
}

void VMEController::get_macaddr()
{
FILE *fp;
int itmp;
char mesg[60];
 sprintf(mesg,"/sbin/ifconfig %c%c%c%c > macadd \0",eth[0],eth[1],eth[2],eth[3]);
 system(mesg); 
 fp=fopen("macadd","r");
 fscanf(fp,"eth%1d      Link encap:Ethernet  HWaddr %02x:%02x:%02x:%02x:%02x:%02x  \n",&itmp,&hw_source_addr[0],&hw_source_addr[1],&hw_source_addr[2],&hw_source_addr[3],&hw_source_addr[4],&hw_source_addr[5]);
 fclose(fp);
 printf(" Source      HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n",hw_source_addr[0]&0xff,hw_source_addr[1]&0xff,hw_source_addr[2]&0xff,hw_source_addr[3]&0xff,hw_source_addr[4]&0xff,hw_source_addr[5]&0xff); 
 printf(" Destination HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n",hw_dest_addr[0]&0xff,hw_dest_addr[1]&0xff,hw_dest_addr[2]&0xff,hw_dest_addr[3]&0xff,hw_dest_addr[4]&0xff,hw_dest_addr[5]&0xff); 
   memcpy(ether_header.h_dest, hw_dest_addr, ETH_ALEN);
   memcpy(ether_header.h_source,hw_source_addr, ETH_ALEN);
}

void VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
{
  /* irdwr:   
              0 bufread
              1 bufwrite 
              2 bufread snd  
              3 bufwrite snd 
              4 flush to VME
              5 loop back 
              6 delay
*/

static int nvme;
static int nread=0;
unsigned char *radd_to;
unsigned char *radd_from;
unsigned char *nbytet;
unsigned short int r_nbyte;
unsigned char *r_head0;
unsigned char *r_head1;
unsigned char *r_head2;
unsigned short r_num;
unsigned char *r_datat;
int size,nwrtn;
int i;
unsigned long int ptrt;
static int istrt=0;  
/* initialize */
  if(istrt==0){
    nwbuf=4;
    nvme=0;
    istrt=1;
  }
  /* flush to vme */
  if(irdwr==4){      
    // printf(" flush to vme \n");
    if(nvme==0)return;
    irdwr=3;
    goto Process;
  }
  /*  fill buffer  */
  nvme=nvme+1;
 //  VME command function code
  // wbuf[0]=ACNLG;
  wbuf[0]=0x00;
  wbuf[1]=VME_CMDS;
  // LOOP back to/from Controller
  if(irdwr==5){/* printf(" controller loop back \n"); */ wbuf[0]=ACNLG_LOOP;wbuf[1]=LOOP_CNTRL;irdwr=2;}
  wbuf[nwbuf+0]=0x00;
  // VME Read/Write 
  
  if(irdwr==0||irdwr==2){wbuf[nwbuf+1]=a_mask[ATYPE]|r_mask|ts_mask[TSIZE]|tt_mask[TTYPE];nread=nread+ts_size[TSIZE];}
  if(irdwr==1||irdwr==3){wbuf[nwbuf+1]=a_mask[ATYPE]|w_mask|ts_mask[TSIZE]|tt_mask[TTYPE];} 
  if(irdwr<=3){
    wbuf[nwbuf+2]=0x00;
    ptrt=(unsigned long int)ptr;
    wbuf[nwbuf+3]=(ptrt&0xff0000)>>16;
    wbuf[nwbuf+4]=(ptrt&0xff00)>>8;
    wbuf[nwbuf+5]=(ptrt&0xff);
    wbuf[nwbuf+6]=(*data&0xff00)>>8;
    wbuf[nwbuf+7]=(*data&0xff);
    if(irdwr==1||irdwr==3)nwbuf=nwbuf+8;
    if(irdwr==0||irdwr==2)nwbuf=nwbuf+6;   
  /* check for overflow */
    LRG_read_flag2=0;
    if(nwbuf>MAX_DATA){
      // printf(" nwbuf %d MAX_DATA %d \n",nwbuf,MAX_DATA);
       LRG_read_flag2=1;
       if(irdwr==1)irdwr=3;
       if(irdwr==0)irdwr=2;
       if(LRG_read_flag==0){
         LRG_read_flag=1;    // turn on large read
         LRG_read_pnt=0;
         // printf(" large read flag on \n");
       }
    }
  } 
  // delay
  if(irdwr==6){
    wbuf[nwbuf+0]=delay_mask[delay_type];
    wbuf[nwbuf+1]=0x00;
    if(delay_type<=3){
      wbuf[nwbuf+3]=(*data&0xff);
      wbuf[nwbuf+2]=(*data&0xff00)>>8;
      nwbuf=nwbuf+4;
    }else{
      wbuf[nwbuf+5]=(*data&0xff);
      wbuf[nwbuf+4]=(*data&0xff00)>>8;
      wbuf[nwbuf+3]=(*(data+1)&0xff);
      wbuf[nwbuf+2]=(*(data+1)&0xff00)>>8;
      nwbuf=nwbuf+6;
    }
  } 
  /* write VME commands to vme */
 Process:
  if(irdwr==2||irdwr==3){
    if(nread>0&&wbuf[1]!=0x1f)wbuf[0]=ACNLG;
    wbuf[2]=(nvme&0xff00)>>8;
    wbuf[3]=nvme&0xff;
    if(PACKETOUTDUMP!=0)dump_outpacket(nvme);
    nwrtn=eth_write();
    // printf(" nwrtn %d nwbuf %d \n",nwrtn,nwbuf);
    nwbuf=4;
    nvme=0;
  }
 
 /* read back bytes from vme */
 
  if((irdwr==2||irdwr==3)&&nread>0){
    nrbuf=nread;
    size=eth_read();
    if(size<10){printf(" no data read back \n");system("cat /proc/sys/dev/schar/0");exit(0);}
      radd_to=(unsigned char *)rbuf;
      radd_from=(unsigned char *)rbuf+6;
      nbytet=(unsigned char *)rbuf+12;
      r_nbyte=((nbytet[0]<<8)&0xff00)|(nbytet[1]&0xff);
      r_head0=(unsigned char *)rbuf+14;
      r_head1=(unsigned char *)rbuf+16;
      r_head2=(unsigned char *)rbuf+18;
      r_datat=(unsigned char *)rbuf+20;
      r_num=((r_head2[0]<<8)&0xff00)|(r_head2[1]&0xff);  
      // printf(" size %d \n",size);
      // for(i=0;i<size;i++)printf("%02x",rbuf[i]&0xff);printf("\n");
      if(r_head0[0]!=0x45|r_head0[1]!=0x00){
             // loop back from controller 
             if((r_head0[0]&0xff)==0xc1&&(r_head0[1]&0xff)==0x00)goto ENDL; 
             printf(" HEADER is not stanard so quit %02x%02x \n",
                        r_head0[0]&0xff,r_head0[1]&0xff);
             exit(0);
      }
      // printf(" %d %d %d \n ",nread,r_num,LRG_read_pnt);
      // if(nread!=2*r_num){printf(" nread %d %d %d %d %02x\n",nread,r_num,size,LRG_read_pnt,r_head0[0]&0xff);} 
    for(i=0;i<r_num;i++){rcv[2*i+LRG_read_pnt]=r_datat[2*i+1];rcv[2*i+1+LRG_read_pnt]=r_datat[2*i];}
    if(LRG_read_flag==1)LRG_read_pnt=LRG_read_pnt+2+2*r_num-2;
  ENDL: 
    if(LRG_read_flag2==0){
      LRG_read_flag=0;     // turn off large read
      LRG_read_pnt=0;
      // printf(" large read flag off %d \n",nwbuf);
    }
    nread=0;
  }
}

/* dump specific to A24/1/0 for now */

void VMEController::dump_outpacket(int nvme)
{
int nwbuft,nwbufto,i;
 printf(" Header %02x%02x   #Cmds  %02x%02x \n",wbuf[0]&0xff,wbuf[1]&0xff,wbuf[2]&0xff,wbuf[3]&0xff);
 if(wbuf[1]==VME_CMDS){
    nwbuft=4;
    for(i=0;i<nvme;i++){
      nwbufto=nwbuft;
      if(wbuf[nwbufto]==0){
      if(wbuf[1+nwbufto]==0x54){
	printf(" %d. W %02x%02x %02x%02x%02x%02x %02x%02x \n",i,wbuf[0+nwbuft]&0xff,wbuf[1+nwbuft]&0xff,wbuf[2+nwbuft]&0xff,wbuf[3+nwbuft]&0xff,wbuf[4+nwbuft]&0xff,wbuf[5+nwbuft]&0xff,wbuf[6+nwbuft]&0xff,wbuf[7+nwbuft]&0xff);
      nwbuft=nwbuft+8;}
      if(wbuf[1+nwbufto]==0x44){
         printf(" %d. R %02x%02x %02x%02x%02x%02x  \n",i,wbuf[0+nwbuft]&0xff,wbuf[1+nwbuft]&0xff,wbuf[2+nwbuft]&0xff,wbuf[3+nwbuft]&0xff,wbuf[4+nwbuft]&0xff,wbuf[5+nwbuft]&0xff);
      nwbuft=nwbuft+6;}
      }else{
	if(wbuf[nwbufto]<=3){
	   printf(" %d. D %02x%02x %02x%02x \n",i,wbuf[0+nwbuft]&0xff,wbuf[1+nwbuft]&0xff,wbuf[2+nwbuft]&0xff,wbuf[3+nwbuft]&0xff);
           nwbuft=nwbuft+4;
        }else{
           printf(" %d. D %02x%02x %02x%02x%02x \n",i,wbuf[0+nwbuft]&0xff,wbuf[1+nwbuft]&0xff,wbuf[2+nwbuft]&0xff,wbuf[3+nwbuft]&0xff,wbuf[4+nwbuft]&0xff,wbuf[5+nwbuft]&0xff);
           nwbuft=nwbuft+6;
        }
      }
    }
 }
}




void VMEController::closeSocket() {
#ifndef DUMMY
  close(theSocket);
#endif
  theSocket = 0;
}


VMEModule* VMEController::getTheCurrentModule(){
 return theCurrentModule;
}


