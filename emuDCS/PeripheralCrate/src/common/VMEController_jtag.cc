
#ifndef OSUcc

//-----------------------------------------------------------------------
// $Id: VMEController_jtag.cc,v 2.17 2005/12/05 09:00:42 mey Exp $
// $Log: VMEController_jtag.cc,v $
// Revision 2.17  2005/12/05 09:00:42  mey
// Update
//
// Revision 2.16  2005/12/02 18:12:26  mey
// get rid of D360
//
// Revision 2.15  2005/11/30 16:49:32  mey
// Bug DMB firmware load
//
// Revision 2.14  2005/11/30 16:27:42  mey
// Use DMB delay
//
// Revision 2.13  2005/11/30 13:00:03  mey
// DMB firmware loading
//
// Revision 2.12  2005/11/29 10:40:50  mey
// Update
//
// Revision 2.10  2005/11/22 15:15:14  mey
// Update
//
// Revision 2.8  2005/11/15 15:22:49  mey
// Update
//
// Revision 2.7  2005/11/11 13:37:31  mey
// Update
//
// Revision 2.6  2005/11/10 18:25:40  mey
// Update
//
// Revision 2.5  2005/11/09 20:07:23  mey
// Update
//
// Revision 2.4  2005/11/08 06:50:21  mey
// Update
//
// Revision 2.3  2005/11/07 16:23:54  mey
// Update
//
// Revision 2.2  2005/11/07 10:12:15  mey
// Byte swap
//
// Revision 2.1  2005/11/02 16:16:30  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h> // for sleep
#include "VMEController.h"
#include "VMEModule.h"
#include "geom.h"

// file-scope utility functions
void RestoreIdle();
void strcopy12(char *mom,int nfrom, const char *baby, int nbyte);
void strcopy11(char *mom,int nfrom, const char *baby, int nbyte);
void strcopy9(char *mom,int nfrom, const char *baby, int nbyte);
int pows(int n,int m);

char adcbyt[3][8]={
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}
};

char adcreg[5]={0x00,0x0e,0x0d,0x0b,0x07};

char lowvbyt[5][8]={
  {0x89,0x99,0xa1,0xb1,0xc9,0xd9,0xe1,0xf9},
  {0x89,0x91,0xa9,0xb9,0xc1,0xd9,0xe9,0xf1},
  {0x89,0x99,0xa1,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9}
};

char lowvreg[10]={0x10,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x11,0x12};

#define debugV 0

/* register 1-7 special commands 0x10-rs 0x11-w feb power 0x12-r febpower */


void VMEController::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,char *inbuf,char *outbuf,int irdsnd)
{
  char sendline[MAXLINE];
  char cmd2[9000];
  char tmp[4];
  int ppnt,pcmd;
  int idev,i;
  int ncmd2,nbcmd2,nbuf2;
  int n;
  int nleft,ncnt,max;
  /* irdsnd for jtag
          irdsnd = 0 send immediately, no read
          irdsnd = 1 send immediately, read
          irdsnd = 2 send in buffer, no read
  */
  int feuse;
  if(dev==99){
    std::cout << "DEVDO DEVICE 99  - I GUESS THIS MEANS SO SAME" <<std::endl;
    idev = idevo;
    feuse = feuseo;
    if(idev>4 && idev!=11)return;
  } else {
    assert(dev < NUMDEV);
    idev=geo[dev].jchan;
    feuse = geo[dev].femask;
  }
  /****** check we have same old device otherwise we need to initialize */
  bool init=( (idev != idevo) || (feuse != feuseo) );
  idevo=idev;
  feuseo = feuse;

  /* if we are down a level we have to back up */
  if(plev==2&&init==1){
    if(debugV) printf(" about to terminate ! \n");
    scan(TERMINATE,cmd,0,inbuf,2);
  }


/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

  switch(idev) {

  case 5: /* DAC calibration CDAC */
     initDevice(idev, feuse);
     plev=1;
     sendline[0]=cmd[0];
     sendline[1]=cmd[1];
     sendline[2]=cmd[2];
     sendline[3]=cmd[3];
     if(debugV) printf(" sendline[0-4] %02x %02x  \n",sendline[0],sendline[1]);
     writen( sendline, 4);
   break;
   
  case 6: /* TEMP Monitor write/read */   
     initDevice(idev, feuse);
     plev=1;
     sendline[0]=adcreg[cmd[0]];
     sendline[1]=adcbyt[cmd[0]-1][cmd[1]];
     writen( sendline, 2);
     readn(outbuf);
     tmp[0]=outbuf[1];
     outbuf[1]=outbuf[0];
     outbuf[0]=tmp[0];
     return;
   break;

 
 case 7: /* write fifo */
     /* cmd[0]=0 - write single word (inbuf[0]) nbuf times
        cmd[0]=1 - read and check (inbuf[0]) single word nbuf times,
                   number of errors returned to outbuf[0] 
        cmd[0]=2 - write toggle even, toggle odd, or count  nbuf times
        cmd[0]=3 - read and check toggle even, toggle odd or count nbuf times,
                   number of errors returned to outbuf[0]
        cmd[0]=4 - write nbuf individual words from inbuf[0-nbuf]
        cmd[0]=5 - read nbuf individual words into outbuf[0-nbuf] */
     initDevice(idev, feuse);
     plev=1;
     sendline[0]=cmd[0];
     sendline[1]=nbuf&0xff;
     sendline[2]=(nbuf>>8)&0xff;
     sendline[3]=(nbuf>>16)&0xff;
     if(debugV)   printf(" sendline[0-4] %02x %02x %02x %02x \n",sendline[0],sendline[1],sendline[2],sendline[3]);
     n=4;    
     writen( sendline, n);
     if(cmd[0]==0||cmd[0]==1||cmd[0]==2||cmd[0]==3){
       n=2;
       tmp[0]=inbuf[0];
       inbuf[0]=inbuf[1];
       inbuf[1]=tmp[0];
       writen(inbuf,n);
       if(cmd[0]==0||cmd[0]==1||cmd[0]==2||cmd[0]==3){
	 readn(outbuf);
       }
       return;
     }
     if(cmd[0]==4){
       std::cout << "writing individual words  " << nbuf << " to " << theSocket << std::endl;
       n=nbuf;
       strcopy11(sendline,0,inbuf,n);
       writen(sendline, n);
       readn(outbuf);
       return;
     }
     if(cmd[0]==5){
       ncnt=0;
       nleft=nbuf;
       while(nleft>0){
         max=MAXLINE;
         if(max>nleft)max=nleft;
         n=readline(&outbuf[ncnt],max);
         ncnt+=n;
         nleft-=n;
       }
     }
   break;


  case 8: /* LOW VOLTAGE  Monitor write/read */   
     plev=1;
     initDevice(idev, feuse);
     sendline[0]=lowvreg[cmd[0]];
     if(cmd[0]>0&&cmd[0]<8){
     sendline[1]=lowvbyt[cmd[0]-1][cmd[1]];
     }else{
       sendline[1]=cmd[1];
     }
     // printf(" cmd %02x %02x \n",cmd[0],cmd[1]);
     // printf("sendline[0] %02x %02x \n",sendline[0],sendline[1]);
     writen( sendline, 2);
     readn(outbuf);
     tmp[0]=outbuf[1];
     outbuf[1]=outbuf[0];
     outbuf[0]=tmp[0];
     return;
   break;

   case 10: /* buckeye shift flash memory */
     /* cmd 00 initalize program process 
        cmd 01 load in Buckeye pathern
        cmd 02 program flash memory
        cmd 03 read back flash memory
        cmd 04 initialize buckeye */
     plev=1;
     initDevice(idev, feuse);
     sendline[0]=cmd[0];
     n=1;
     writen( sendline, n);
     // printf(" buck flash command %d \n",cmd[0]); 
     if(cmd[0]==0)return;
     if(cmd[0]==1){
       n=295;
       //  printf(" eazjtag: n %d \n",n);
       writen(inbuf, n);
     }
     if(cmd[0]==2)return;
     if(cmd[0]==3){
       std::cout<<" about to read 291 times" << std::endl;
       ncnt=0;
       nleft=291;
       while(nleft>0){
         max=MAXLINE;
         if(max>nleft)max=nleft;
         n=readline(&outbuf[ncnt],max);
         ncnt+=n;
         nleft-=n;
       }
     }
     if(cmd[0]==4)return;
     return;
   break;

   default:
     // for 1-4 and 11
     if(init) {
       initDevice(idev, feuse);
     }
     plev = 2;
   break;

  }

/**********  end initialize ***********************/
/**********  send the JTAG data ************************/ 
if(idev<=4||idev==11){
  if(ncmd>0){
/* stan jtag kludge kludge for serial devices */
    if(geo[dev].nser!=0){

      ppnt=0;
      cmd2[0]=0x00;
      cmd2[1]=0x00;
      int m=geo[dev].nser;
      for(i=0;i<geo[dev].nser;i++){
        int seri = geo[dev].seri[m-i-1];
        if(seri<0) {
          tmp[0]=cmd[0];
          pcmd=geo[-1*seri].kbit;
        }
        else {
	  tmp[0]=geo[seri].kbypass;
          pcmd=geo[seri].kbit;
        }
       //printf(" i %d tmp[0] %04x pcmd %d  \n",i,tmp[0],pcmd);
	//	printf(" cmd[0] %02x \n",cmd[0]);
        for(int k=0;k<pcmd;k++){
          ++ppnt;
          if(((tmp[0]>>k)&0x01)!=0){
            if(ppnt<9){
              int pow2=pows(2,ppnt-1);
              cmd2[0] += pow2;
            //printf(" k cmd %d %02x %d %d \n",k,cmd2[0],ppnt,pow2);
            } 
            if(ppnt>8){
              int pow2=pows(2,ppnt-9);
              cmd2[1] += pow2;
         } 
       }
     }
   }
   ncmd2=ppnt;
   nbcmd2=ncmd2/8+1;
   nbuf2=geo[dev].sxtrbits;
   if(nbuf>0){  
     nbuf2 += nbuf;}
   else{
     nbuf2=0;}
  //int kbit=geo[dev].kbit;
  //int kbybit=geo[dev].kbybit;
  //char kbypass=geo[dev].kbypass;
  // printf(" final ncmd %d cmd %04x %04x \n",ncmd,cmd[1],cmd[0]);
  //   printf(" final nbuf %d nbuf %d \n",nbuf2,nbuf); 
   }
   else
   {
     nbuf2=nbuf;
     ncmd2=ncmd;
     int k=ncmd2/8+1;
     if(k>100)std::cout<<" ****************CATASTROPHE STOP STOP " << std::endl;
     for(i=0;i<k;i++){
      cmd2[i]=cmd[i];
     }
   }
   //printf(" ********** %s dev prev_dev %d %d \n",geo[dev].nam,dev,prev_dev);
/* end stan kludge */
}
}
  switch(idev){
    case 1:   /* jtag feboards */ 
      if(ncmd==-99){scan(-1,cmd,-99,outbuf,irdsnd);break;}    
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
    break;
    case 2: /* jtag motherboard cntrl */ 
      if(ncmd==-99){scan(-1,cmd,-99,outbuf,irdsnd);break;}        
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
    break;
    case 3: /* jtag motherboard prom */
      if(ncmd==-99){scan(-1,cmd,-99,outbuf,irdsnd);break;}   
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0&nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
    break;
    case 4: /* jtag vme-mthrbrd prom */
      if(ncmd==-99){scan(-1,cmd,-99,outbuf,irdsnd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
    break;
    case 11: /* reset vme  prom */
      // printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
      if(ncmd==-99){scan(-1,cmd,-99,outbuf,irdsnd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd,ncmd,outbuf,0);
      }else{
        scan(INSTR_REG,cmd,ncmd,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
    break;

  }
  if(debugV) {
    std::cout << "end of devdo Tbytes " << Tbytes << std::endl;
    for(int ii = 0;  ii < Tbytes; ++ii) {
      std::cout << (int) Tdata[ii] << " ";
    }
    std::cout << std::endl;
  }
/*************  end of senddata **********************/
}


void VMEController::scan(int reg, const char *snd,int cnt,char *rcv,int ird)
{
char tx[70000];
char rcvline[MAXLINE];
int modulus, bytes;
int iextra;
int Temp;
  assert(plev == 2);
 //printf(" reg %d cnt %d snd %02x %02x ir %d \n",reg,cnt,snd[0],snd[1],ird);
  if(cnt==-99){
    tx[0]=0x10;
    tx[1]=snd[0];
    tx[2]=snd[1];
    //    printf(" %d %d \n",tx[2],tx[1]);
    bytes=3;
    goto SPECIAL;
  }
  if(cnt<0){
    tx[0]=0x08;
    bytes=1;
    goto SPECIAL;
  }
  //printf(" debug: entered scan %d ird %d \n",cnt,ird);
  modulus = cnt%8;
  bytes = (modulus == 0) ? cnt/8 : cnt/8 + 1;
  //printf("debug: bytes %d \n",bytes);
  tx[0]=0x00;   /* leave bits 0-1 zero for lvl */
  if(reg==0x02)tx[0]=0x02;
  tx[0]=tx[0]|0x04; /* set shift data bit */
  if(reg==DATA_REG)tx[0]=tx[0]|0x10; /* data or instr */
  if(ird==1)tx[0]=tx[0]|0x20; /* set read bit */
  if(modulus!=0)tx[0]=tx[0]|0x40; /* set mod bit */
  if(bytes>256)tx[0]=tx[0]|0x08; /* set long bit */
  // printf(" tx[0] %02x \n",tx[0]&0xff);
  if(bytes>256){
    iextra=5;
    tx[1]=(cnt>>24)&0xff;
    tx[2]=(cnt>>16)&0xff;
    tx[3]=(cnt>>8)&0xff;
    tx[4]=(cnt&0xff);}
  else{
    iextra=3;
    tx[1]=(cnt>>8)&0xff;
    tx[2]=(cnt&0xff);
    // printf(" cnt %d tx %02x %02x \n",cnt,tx[1]&0xff,tx[2]&0xff);
   }

/* solve the byte swapping problem */
    if(indian==SWAP){
      strcopy11(tx,iextra,snd,bytes);
    }else{
      strcopy12(tx,iextra,snd,bytes);
    }

/* fix last byte problem later */ 

    bytes +=iextra;
 SPECIAL:
    Temp = bytes +Tbytes;

/* ok, pack it */
    // printf(" SCAN: about to send \n");

    if(Temp <= 40000){
      strcopy9(Tdata,Tbytes,tx,bytes);
      Tbytes +=bytes;

      if(ird>0){
        // printf(" debug: immediate Tbytes %d \n",Tbytes);
        writenn(Tdata,Tbytes);
        if(Tbytes>max_buff)max_buff=Tbytes;
        tot_buff=tot_buff+Tbytes;
        Tbytes=0;
      }
    }
    else if (Temp > 40000){
      if(bytes <= 9900){
        strcopy9(Tdata,Tbytes,tx,bytes);
        Tbytes +=bytes;
        writenn(Tdata,Tbytes);
        if(Tbytes>max_buff)max_buff=Tbytes;
        tot_buff=tot_buff+Tbytes;
        Tbytes=0;
      }
      else if (bytes > 9000)
      {
	if(Tbytes>0){
          writenn(Tdata,Tbytes);
          if(Tbytes>max_buff)max_buff=Tbytes;
          tot_buff=tot_buff+Tbytes;
          Tbytes=0;
        } 
        writenn(tx,bytes);
        if(Tbytes>max_buff)max_buff=Tbytes;
        tot_buff=tot_buff+Tbytes;
        Tbytes = 0;
      }
    }
    if(ird==1){              /* a read is coming so get data */;
    if(cnt>10000)sleep(2);
    int nrcvo=0;
      //int nrbyt=cnt/8;
      //if(nrbyt%2==1)nrbyt=nrbyt+1;
      //@@ rick fix from scan_alct
      int nrbyt = (cnt-1)/8+1;
      //  printf(" call readn \n");
      int nrcv = readn(rcvline); 
      // printf(" nrcv %d %d \n",nrcv,nrbyt);
      nrcvo=nrcv;
      strcopy11(rcv,0,rcvline,nrcv); /* endian swapping */
      unsigned short int tmpdata=(((rcv[nrcv-1]&0xff)<<8)&0xff00)|(rcv[nrcv-2]&0xff); /* fix final byte shift anomoly */
      // different in scan(), scan_alct()
      if(cnt%16!=0) {
        if(theCurrentModule->boardType() == VMEModule::DMB_ENUM) {
          tmpdata=(tmpdata>>(16-cnt%16))&0xffff;
        } else {
          if(cnt%16<=8) {
            tmpdata=(tmpdata>>8)&0xffff;
          }
        }
      }

      rcv[nrcv-1]=(tmpdata>>8)&0xff;
      rcv[nrcv-2]=(tmpdata&0xff);
      // for(i=0;i<nrcv;i++)printf(" scan read : i %d rcv %02x \n",i,rcv[i]);
      if(nrbyt-nrcv>4){
	// sleep(1);
      nrcv = readn(rcvline); 
      strcopy11(rcv,nrcvo,rcvline,nrcv); /* endian swapping */
      nrcvo+=nrcv;
      //  printf(" nrcv2 %d %d \n",nrcv,nrbyt);
      } 
    }
}


void VMEController::initDevice(int idev, int feuse) {
  if(debugV)std::cout << "InitDevice " << idev << " " <<(int) feuse << std::endl;
  char c =0;
  unsigned int lev=0x01;
  c=c|lev;
  int chan=idev;
  c=c|(chan<<2);

  char sendline[2];
  //  printf(" scanLevel chan sendline[0] %d %d \n",chan,sendline[0]);
  sendline[0]=c;
  sendline[1]=feuse;
  writen( sendline, 2);
}



void VMEController::goToScanLevel() {
  plev = 2;
}


void VMEController::d360sleep()
{
char sndx[2],rcvx[2];
 scan(-1,sndx,-99,rcvx,0);
} 

void VMEController::RestoreIdle()
{
 char sndx[2],rcvx[2];
 scan(-1,sndx,-1,rcvx,0);
}

void VMEController::InitJTAG(int port)
{
}


void VMEController::CloseJTAG()
{ char sndx[2],rcvx[2];
// printf(" call close jtag \n");
  sndx[0]=0;
  sndx[1]=0;
  devdo((DEVTYPE) 99,-99,sndx,0,sndx,rcvx,2);
}

/*
 * copy 'nbyte' of "baby" to "mom" after skipping "nfrom" bytes
 * 
 */
void strcopy9(char *mom,int nfrom, const char *baby, int nbyte)
{

  mom += nfrom;

  for(int i=0; i<nbyte ;i++)
    {
      /*      printf("i=%d, *mom=0x%02x\n", i,*mom);  */
      *mom++ = *baby++;
    }
}


/*
 * byte swapping
 * 
 */
void strcopy11(char *mom,int nfrom, const char *baby, int nbyte)
{
  bool jodd = (nbyte%2 == 1);

  int jloop=nbyte/2;
  mom += nfrom;
  int j=0;
  for(int i=0; i<jloop ;i++)
    {
      mom[j]  = baby[j+1];
      mom[j+1] = baby[j];
      j=j+2;
    }
  if(jodd) mom[j] = baby[j];
}


/*
 * no byte swapping
 * 
 */
void strcopy12(char *mom,int nfrom, const char *baby, int nbyte)
{
  bool jodd = (nbyte%2 == 1);
  int jloop=nbyte/2;
  mom += nfrom;
  int j=0;
  for(int i=0; i<jloop ;i++)
    {
      mom[j+1]  = baby[j+1];
      mom[j] = baby[j];
      j=j+2;
    }

  if(jodd) mom[j] = baby[j];

}


/*
 * readline (fd, ptr, maxlen)
 *    input  : fd -> socket descripter or file(?)
 *             ptr -> pointor for the buffer
 *             maxlen -> Maximum length of line
 *    output : number of bytes read
 *    
 *   function: Read a line from a "fd" descriptor a byte at a time.
 *             looking for the newline. Store the newline in the buffer,
 *             then follow it with a NULL. ( remove '\n' )
 *
 */

int VMEController::readline(register char *ptr, register int maxlen)
{
  int rc = readn(ptr); 
  // 0 means EOF, no data read, negative means error
  return (rc >= 0) ? rc : -1;
}

int pows(int n,int m)
{int l,i;
l=1;
 for(i=0;i<m;i++)l=l*n;
return l;
}

#else

#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h> 
#include "VMEController.h"
#include "VMEModule.h"
#include "geom.h"
#include "Crate.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h> 
#include <errno.h>
#include <string.h>

#include <unistd.h>

#include "vme_cmd.h"

int pows(int n,int m);

#define debugV 0

/* register 1-7 special commands 0x10-rs 0x11-w feb power 0x12-r febpower */

void VMEController::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
{
  char cmd2[9000];
  char snd[10];
  char tmp[4];
  int kbit,kbybit;
  char kbypass;
  char c;
  int ppnt,pcmd,pow2;
  int idev,i,j,k,m;
  int ncmd2,nbcmd2,nbuf2;
  int n,nt1;
  int nleft,ncnt,max;
  int init;
  int ififo;
  int nbyte;
  unsigned short int ishft,temp;

  /* irdsnd for jtag
          irdsnd = 0 send immediately, no read
          irdsnd = 1 send immediately, read
          irdsnd = 2 send in buffer, no read
  */
  // printf(" ENTERING devdo, dev: %d, ncmd: %d \n",dev, ncmd);
  if(dev!=99){
  idev=geo[dev].jchan;
  }else{
    idev=idevo;
    if(idev>4&idev!=11)return;
  }
  // printf(" enter devdo %d %d \n",dev,idev);

  //printf(" ENTERING devdo, idev idevo dev %d %d %d \n",idev,idevo,dev);
  /****** check we have same old device otherwise we need to initialize */
  init=0;
    if(idev!=idevo){
       init=1;
       //  printf(" idev idevo %d %d plev %d \n",idev,idevo,plev);
    }
    if(idev==1){
      if(dev==1||dev==7)feuse=0x01;
      if(dev==2||dev==8)feuse=0x02;
      if(dev==3||dev==9)feuse=0x04;
      if(dev==4||dev==10)feuse=0x08;
      if(dev==5||dev==11)feuse=0x10;
      if(dev==6||dev==12)feuse=0x1F;
      //  printf(" feuse %d \n",feuse);
      if(feuseo!=feuse)init=1;
      feuseo=feuse;
    }
  idevo=idev;

  //  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo,init);
/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

  switch(idev){

   case 1:  /* JTAG feboard */
  /* send down 1 level */
     if(init==1){ 
      ife=1;
    //  fprintf(fplog," init feboard %d \n",feuse);
      add_i=vmeadd|msk01|msk_i;
      add_d=vmeadd|msk01|msk_d;
      add_dh=vmeadd|msk01|msk_dh;
      add_ds=vmeadd|msk01|msk_ds;
      add_dt=vmeadd|msk01|msk_dt;
      add_rst=vmeadd|msk01|msk_rst;
      add_sw=vmeadd|msk01|msk_sw;
      add_sr=vmeadd|msk01|msk_sr;
      add_r=vmeadd|msk01|msk_r;
      setuse();
     }
   break;

   case 2:   /* JTAG motherboard control */ 
     if(init==1){
      feuse=0xff;
      ife=0; 
      //  fprintf(fplog," init daqmb controller \n");
      add_i=vmeadd|msk02|msk_i;
      add_d=vmeadd|msk02|msk_d;
      add_dh=vmeadd|msk02|msk_dh;
      add_ds=vmeadd|msk02|msk_ds;
      add_dt=vmeadd|msk02|msk_dt;
      add_rst=vmeadd|msk02|msk_rst;
      add_sw=vmeadd|msk02|msk_sw;
      add_sr=vmeadd|msk02|msk_sr;
      add_r=vmeadd|msk02|msk_r;
     }
   break;

   case 3:   /* JTAG motherboard prom */ 
     if(init==1){
      ife=0;
      // printf(" init daqmb prom \n");
      add_i=vmeadd|msk03|msk_i;
      add_d=vmeadd|msk03|msk_d;
      add_dh=vmeadd|msk03|msk_dh;
      add_ds=vmeadd|msk03|msk_ds;
      add_dt=vmeadd|msk03|msk_dt;
      add_rst=vmeadd|msk03|msk_rst;
      add_sw=vmeadd|msk03|msk_sw;
      add_sr=vmeadd|msk03|msk_sr;
      add_r=vmeadd|msk03|msk_r;    
     }
   break;

  case 4:   /* JTAG vme-motherboard prom */
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk04|msk_i;
     add_d=vmeadd|msk04|msk_d;
     add_dh=vmeadd|msk04|msk_dh;
     add_ds=vmeadd|msk04|msk_ds;
     add_dt=vmeadd|msk04|msk_dt;
     add_rst=vmeadd|msk04|msk_rst;
     add_sw=vmeadd|msk04|msk_sw;
     add_sr=vmeadd|msk04|msk_sr;
     add_r=vmeadd|msk04|msk_r;    
    }
   break;

  case 5: /* DAC calibration CDAC */
      //  fprintf(fplog," load cdac \n");
      add_cdac=vmeadd|msk05;
      load_cdac(cmd);
      return;
   break;
   
  case 6: /* TEMP Monitor write/read */       /* motherboard adc */
      add_adcw=vmeadd|msk07|msk_adcw;
      add_adcr=vmeadd|msk07|msk_adcr; 
      add_adcrbb=vmeadd|msk07|msk_adcrbb;
      add_adcrs=vmeadd|msk07|msk_adcrs; 
      add_adcws=vmeadd|msk07|msk_adcws; 
      vme_adc(cmd[0],cmd[1],outbuf);
     return;
   break;

 
 case 7: /* write fifo */     /* set fifo commands */
      ififo=dev-FIFO1+1;
      add_fifo_w00=vmeadd|msk06|msk_fifo_w00; 
      add_fifo_w01=vmeadd|msk06|msk_fifo_w01;
      add_fifo_w10=vmeadd|msk06|msk_fifo_w10; 
      add_fifo_w11=vmeadd|msk06|msk_fifo_w01;
      add_fifo_rln=vmeadd|msk06|msk_fifo_rln; 
      add_fifo_rli=vmeadd|msk06|msk_fifo_rli;
      add_fifo_rhn=vmeadd|msk06|msk_fifo_rhn; 
      add_fifo_rhi=vmeadd|msk06|msk_fifo_rhi;
      add_fifo_ws=vmeadd|msk06|msk_fifo_ws; 
      add_fifo_rs=vmeadd|msk06|msk_fifo_rs;
      add_fifo_i=vmeadd|msk06|msk_fifo_i;
      nbyte=nbuf;
      // printf(" irdsnd %d \n",irdsnd);
      daqmb_fifo(irdsnd,ififo,nbyte,(unsigned short int *)inbuf,(unsigned char *)outbuf);
      break;

 case 8: /* LOW VOLTAGE  Monitor write/read */
      add_loww=vmeadd|msk08|msk_loww;
      add_lowr=vmeadd|msk08|msk_lowr; 
      add_lowwpr=vmeadd|msk08|msk_lowwpr;
      add_lowrpr=vmeadd|msk08|msk_lowrpr;
      add_lowrs=vmeadd|msk08|msk_lowrs; 
      add_lowws=vmeadd|msk08|msk_lowws; 
      lowvolt(cmd[0],cmd[1],outbuf);
   break;

case 10: /* buckeye shift flash memory */
     /* cmd 00 initalize program process 
        cmd 01 load in Buckeye pathern
        cmd 02 program flash memory
        cmd 03 read back flash memory
        cmd 04 initialize buckeye */
 
      add_bucip=vmeadd|msk09|msk_bucip;
      add_bucl=vmeadd|msk09|msk_bucl;
      add_bucf=vmeadd|msk09|msk_bucf;
      add_bucr=vmeadd|msk09|msk_bucr;
      add_buci=vmeadd|msk09|msk_buci;
      add_buce=vmeadd|msk09|msk_buce;
      buckflash(cmd,inbuf,outbuf);
      return;
   break;

   case 11:   /* RESET emergency VME PROM loading */ 
     if(init==1){
       feuse=0x99;
       ife=99;
       add_reset=vmeadd|msk0f;
     }
   break;

  }

/**********  end initialize ***********************/
/**********  send the JTAG data ************************/ 
if(idev<=4||idev==11){
if(ncmd>0){
/* stan jtag kludge kludge for serial devices */
   if(geo[dev].nser!=0){
      ppnt=0;
      cmd2[0]=0x00;
      cmd2[1]=0x00;
      m=geo[dev].nser;
      for(i=0;i<geo[dev].nser;i++){
       if(geo[dev].seri[m-i-1]<0)
         {tmp[0]=cmd[0];pcmd=geo[-1*geo[dev].seri[m-i-1]].kbit;}
       else
	 {tmp[0]=geo[geo[dev].seri[m-i-1]].kbypass;pcmd=geo[geo[dev].seri[m-i-1]].kbit;}
       /*       printf(" i %d tmp[0] %04x pcmd %d  \n",i,tmp[0],pcmd);
		printf(" cmd[0] %02x \n",cmd[0]); */
       for(k=0;k<pcmd;k++){
       ppnt=ppnt+1;
       if(((tmp[0]>>k)&0x01)!=0){
         if(ppnt<9){pow2=pows(2,ppnt-1);cmd2[0]=cmd2[0]+pow2;
	 /*printf(" k cmd %d %02x %d %d \n",k,cmd2[0],ppnt,pow2); */}
         if(ppnt>8){pow2=pows(2,ppnt-9);cmd2[1]=cmd2[1]+pow2;} 
       }
     }
   }
   ncmd2=ppnt;
   nbcmd2=ncmd2/8+1;
   nbuf2=geo[dev].sxtrbits;
   if(nbuf>0){  
     nbuf2=nbuf2+nbuf;}
   else{
     nbuf2=0;}
  kbit=geo[dev].kbit;
  kbybit=geo[dev].kbybit;
  kbypass=geo[dev].kbypass;
  /*   printf(" final ncmd %d cmd %04x %04x \n",ncmd,cmd[1],cmd[0]);
       printf(" final nbuf %d nbuf %d \n",nbuf2,nbuf); */
   }
   else
   {
     nbuf2=nbuf;
     ncmd2=ncmd;
     k=ncmd2/8+1;
     if(k>100)printf(" ****************CATASTROPY STOP STOP ");
     for(i=0;i<k;i++){
      cmd2[i]=cmd[i];
     }
   }
   // printf(" ********** %s dev prev_dev %d %d \n",geo[dev].nam,dev,prev_dev);
/* end stan kludge */
}
}

switch(idev){
    case 1:   /* jtag feboards */ 
      if(ncmd==-99){sleep_vme(cmd);break;}    
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
       }
    break;
    case 2: /* jtag motherboard cntrl */ 
      if(ncmd==-99){sleep_vme(cmd);break;}        
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
       }
    break;
    case 3: /* jtag motherboard prom */
      if(ncmd==-99){sleep_vme(cmd);break;}   
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0&nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }
    break;
    case 4: /* jtag vme-mthrbrd prom */
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }
    break;
    case 11: /* reset vme  prom */
      // printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle_reset();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan_reset(INSTR_REG,cmd,ncmd,outbuf,0);
      }else{
        scan_reset(INSTR_REG,cmd,ncmd,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan_reset(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
    break;

}
}

void VMEController::scan(int reg,const char *snd,int cnt,char *rcv,int ird)
{
int i;
int cnt2;
int npnt;
int byte,bit;
unsigned short int tird[3]={1,1,3};
unsigned short int tmp[2]={0x0000};
unsigned short int *data;
unsigned short int *data2;
unsigned short int *ptr_i;
unsigned short int *ptr_d;
unsigned short int *ptr_dh;
unsigned short int *ptr_ds;
unsigned short int *ptr_dt;
unsigned short int *ptr_r;
 
if(cnt==0)return;

//printf("ENTERING scan:%d\n",reg);
 cnt2=cnt-1;
 // printf(" ****** cnt cnt2 %d %d \n",cnt,cnt2);
 // printf(" reg ird %d %d \n",reg,ird);
 data=(unsigned short int *) snd;

 /* instr */

 if(reg==0){
   add_i=add_i&msk_clr;
   add_i=add_i|(cnt2<<8);
   ptr_i=(unsigned short int*)add_i;
   bit=cnt; 
   // xif(bit>8)*ptr_i=*data;
   // xif(bit<=8)*ptr_i=((*data)>>8);
   // if(bit<=8)*data=((*data)>>8);
   // printf(" 1 VME W: %08x %04x \n",ptr_i,*data);
   vme_controller(tird[ird],ptr_i,data,rcv);
   return;
 }

 /* data */

  if(reg==1){
   byte=cnt/16;
   bit=cnt-byte*16;
   // printf(" bit byte %d %d \n",bit,byte);
   if(byte==0|(byte==1&bit==0)){
     add_d=add_d&msk_clr;
     add_d=add_d|(cnt2<<8);
     ptr_d=(unsigned short int *)add_d; 
     // printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
     // xif(bit>8|byte==1)*ptr_d=*data;
     // xif(bit<=8&byte!=1)*ptr_d=((*data)>>8);
     // if(bit<=8&byte!=1)*data=((*data)>>8);
     vme_controller(tird[ird],ptr_d,data,rcv);
     //  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
     if(ird==1){
       ptr_r=(unsigned short int *)add_r;
       // x*data2=*ptr_r;
       // printf(" R %08x \n",ptr_r);
       vme_controller(2,ptr_r,tmp,rcv);
     }
      return;
   }
  add_dh=add_dh&msk_clr;
  add_dh=add_dh|0x0f00;
  ptr_dh=(unsigned short int *)add_dh;
  // printf(" 3 VME W: %08x %04x \n",ptr_dh,*data);
  vme_controller(1,ptr_dh,data,rcv);
  // x*ptr_dh=*data;
  data=data+1;
  if(ird==1){       
     ptr_r=(unsigned short int *)add_r;
     // printf("3 R %08x \n",ptr_r);
     vme_controller(0,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }
  add_ds=add_ds&msk_clr;
  ptr_ds=(unsigned short int *)add_ds;
  for(i=0;i<byte-1;i++){
    if(i==byte-2&bit==0){
      add_dt=add_dt&msk_clr;
      add_dt=add_dt|0x0f00;
      ptr_dt=(unsigned short int *)add_dt;
      // printf("4 VME W: %08x %04x \n",ptr_dt,*data);
      vme_controller(tird[ird],ptr_dt,data,rcv);
      // x*ptr_dt=*data;
      if(ird==1){
        ptr_r=(unsigned short int *)add_r;
	//  printf("4 R %08x \n",ptr_r);
        vme_controller(2,ptr_r,data,rcv);
        // x*data2=*ptr_r;  
        // printf(" rddata %04x \n",*data2);
      }
      return;
    }else{
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=(unsigned short *)add_ds;
      // printf("5 VME W: %08x %04x \n",ptr_ds,*data);
      vme_controller(1,ptr_ds,data,rcv);
      // x*ptr_ds=*data;
      data=data+1;
      if(ird==1){
        ptr_r=(unsigned short int *)add_r;
        // printf(" R %08x \n",ptr_r);
        vme_controller(0,ptr_r,tmp,rcv);
        // x*data2=*ptr_r; 
	// printf(" rddata %04x \n",*data2);
      }
    }
  }
  cnt2=bit-1;
  add_dt=add_dt&msk_clr;
  add_dt=add_dt|(cnt2<<8);
  ptr_dt=(unsigned short int *)add_dt; 
  // printf("6 VME W: %08x %04x \n",ptr_dt,*data);
  // xif(bit>8)*ptr_dt=*data;
  // xif(bit<=8)*ptr_dt=*data>>8;
  // if(bit<=8)*data=*data>>8;
  vme_controller(tird[ird],ptr_dt,data,rcv);
  if(ird==1){
     ptr_r=(unsigned short int *)add_r;
     // printf(" R %08x \n",ptr_r);
     vme_controller(2,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }

  return;
 }
}

void VMEController::initDevice(int idev, int feuse) {
  if(debugV)cout << "InitDevice " << idev << " " <<(int) feuse << endl;
  //cout << "InitDevice " << idev << " " <<(int) feuse << endl;
 char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1];
unsigned short int *ptrreg;
    vmeadd=0x00000000;

    switch(idev){

    case 1:
      /* feboard */
      break;
      
    case 2:
      /* ALCT Fast Programming JTAG Chain */
      pvme=0x0098;
      break;
      
    case 3:
      /* TMB Mezzanine FPGA +FPGA PROMS JTAG  */
      // fprintf(fplog," tmb mezzanine called \n");
      pvme=0x00b8;
      break;
      
    case 4:
      /* TMB User Proms JTAG CHAIN */
      pvme=0x00d8;
      break;

    case 5:
      /* TMB FPGA User JTAG chain */
      pvme=0x00f8;
      break;
      
    case 6:
      /* ALCT Slow User JTAG Chain */
      pvme=0x0080;
      break;
      
    case 7:
      /* ALCT Slow Programming JTAG Chain
	 ife=50;
	 add_ucla=vmeadd|0x70000|(slot<<19);
	 pvme=0x0088;
    /* ALCT JTAG chain */
      break;
      
    case 8:
      /* ALCT Fast User JTAG chain */
      pvme=0x0090;
    break;
    
    case 9:
      /* ALCT Fast Programming JTAG chain */
      pvme=0x0098;
      break;
    }
}


int pows(int n,int m)
{int l,i;
l=1;
 for(i=0;i<m;i++)l=l*n;
return l;
}


void VMEController::RestoreIdle()
{
 char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr_rst;
 ptr_rst=(unsigned short *)add_rst;
 // printf(" call restore idle %08x %08x \n",ptr_rst,add_rst);
  vme_controller(3,ptr_rst,tmp2,tmp);

}


void VMEController::InitJTAG(int port)
{
}


void VMEController::CloseJTAG()
{ 

}


void VMEController::RestoreIdle_reset()
{
unsigned short int one[1]={0x01};
char tmp[2]={0x00,0x00};
unsigned short int *ptr;
 int i;
 ptr=(unsigned short int *)add_reset;
 for(i=0;i<5;i++){vme_controller(1,ptr,one,tmp);sdly();}
}

void VMEController::load_cdac(const char *snd)
{
 static unsigned short int *ptr;
 unsigned short int data1,data2;
 char tmp[2]={0x00,0x00};
 int n;
 ptr=(unsigned short int *)add_cdac;
 // n = readline(sockfd,val,4);
 data1=((snd[1]&0x0f)<<8)|(snd[0]&0xff);
 data1=(data1<<1)|0xc000;
 data2=((snd[3]&0x0f)<<8)|(snd[2]&0xff);
 data2=(data2<<1)|0x4000; 
 // printf(" cdac address %08x %d \n",ptr,n);
 // printf(" data1 data2 %04x %04x \n",data1,data2);
  vme_controller(1,ptr,&data1,tmp);
 // x*ptr=data1;
 // printf(" one called now the other \n");
  sleep_vme2(100);
  vme_controller(3,ptr,&data2,tmp);
  // x*ptr=data2;
 // printf(" exit cdac routine \n");
}

void VMEController::vme_adc(int ichp,int ichn,char *rcv)
{
 char adcbyt[3][8]={
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}
 };

 char adcreg[5]={0x00,0x0e,0x0d,0x0b,0x07};

 unsigned short int *data;
 unsigned short int *ptr;
 unsigned short int val[2];
 unsigned short int tmp[2]={0x0000,0x0000}; 
 // printf(" enter vme_adc \n");

      val[0]=adcreg[ichp];
      val[1]=adcbyt[ichp-1][ichn];
      if(val[0]==0){ 
         ptr=(unsigned short int *)add_adcrs;
	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       ptr=(unsigned short int *)add_adcws;
       // printf(" select register%08x  %02x \n",ptr,val[0]&0xff);
	 vme_controller(3,ptr,&val[0],rcv);     //*ptr=val[0];  
         ptr=(unsigned short int *)add_adcw;
         // printf(" adc write %08x %02x \n",ptr,val[1]&0xff);
	 vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
      if(val[0]!=0x07){
          ptr=(unsigned short int *)add_adcr;
      }else{
          ptr=(unsigned short int *)add_adcrbb;
      }
      vme_controller(2,ptr,tmp,rcv);     //*data=*ptr;  

      // print(" adc read %08x %02x %02x\n",ptr,rbuf[1]&0xff,rbuf[0]&0xff);
}

void VMEController::buckflash(const char *cmd,const char *inbuf,char *rcv)
{
 unsigned short int *ptr;
 unsigned short int tmp[1];
 unsigned short int *data;
 char *line2;
 int n,m,nleft,i;
 if(cmd[0]==0){
   ptr=(unsigned short int *)add_bucip;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==2){
   ptr=(unsigned short int *)add_bucf;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==4){
   ptr=(unsigned short int *)add_buci; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==5){
   ptr=(unsigned short int *)add_buce; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==1){
   ptr=(unsigned short int *)add_bucl;
   for(i=0;i<294;i++){ 
     line2=(char *)inbuf+i-1;
     // data=(unsigned short int *)line2;
     data[0]=((line2[0]<<8)&0xff00)|(line2[1]&0x00ff);
     vme_controller(1,ptr,data,rcv);    
   }
     line2=(char *)inbuf+294-1;
     //  data=(unsigned short int *)line2;
     data[0]=((line2[0]<<8)&0xff00)|(line2[1]&0x00ff);
     vme_controller(3,ptr,data,rcv);    
   return;
 }
 if(cmd[0]==3){
   //  fprintf(fplog," buckflash about to read \n");
   ptr=(unsigned short int *)add_bucr;
   for(i=0;i<290;i++){
     vme_controller(0,ptr,tmp,rcv);    
   }
   vme_controller(2,ptr,tmp,rcv);    
   return;
 }
}

//

void VMEController::lowvolt(int ichp,int ichn,char *rcv)
{

 char lowvbyt[5][8]={
  {0x89,0x99,0xa1,0xb1,0xc9,0xd9,0xe1,0xf9},
  {0x89,0x91,0xa9,0xb9,0xc1,0xd9,0xe9,0xf1},
  {0x89,0x99,0xa1,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},
  {0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9}
 };

 char lowvreg[10]={0x10,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x11,0x12};
 unsigned short int *data;
 unsigned short int *ptr;
 unsigned short int val[2];
 unsigned short int tmp[2]={0x0000,0x0000}; 
 // printf(" enter vme_adc \n");

      val[0]=lowvreg[ichp];
      if(ichp>0&&ichp<8){
        val[1]=lowvbyt[ichp-1][ichn];
      }else{
        val[1]=ichn;
      }
      if(val[0]==0x10){
         ptr=(unsigned short int *)add_lowrs;
	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       if(val[0]==0x12){ 
         ptr=(unsigned short int *)add_lowrpr;
      	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       if(val[0]==0x11){ 
         ptr=(unsigned short int *)add_lowwpr;
 	 vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
         return;
       } 
       ptr=(unsigned short int *)add_lowws;
       vme_controller(3,ptr,&val[0],rcv);     //*ptr=val[0];   
       ptr=(unsigned short int *)add_loww;
       vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
       ptr=(unsigned short int *)add_lowr; 
       vme_controller(2,ptr,tmp,rcv);     //*data=*ptr;  
}


void  VMEController::scan_reset(int reg,const char *snd, int cnt, char *rcv,int ird)
{
int i,j;
int npnt;
int byte,bit;
unsigned short int x00[1]={0x00};
unsigned short int x01[1]={0x01};
unsigned short int x02[1]={0x02};
unsigned short int x03[1]={0x03};
unsigned short int ival,ival2;
unsigned short int *data;
unsigned short int *ptr;
 if(cnt==0)return;
 ptr=(unsigned short int *)add_reset;
 data=(unsigned short int *) snd;
 // printf("scan_reset %d %d %02x %02x \n",reg,cnt,snd[1]&0xff,snd[0]&0xff);

 /* instr */

 if(reg==0){
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
 }

 /* data */

 if(reg==1){ 
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
 }
 byte=cnt/16;
 bit=cnt-byte*16;
 for(i=0;i<byte;i++){
   for(j=0;j<16;j++){
      ival=*data>>j;
      ival2=ival&0x01;
      if(i!=byte-1|bit!=0|j!=15){
        if(ival2==0){vme_controller(1,ptr,x00,rcv);sdly();}
        if(ival2==1){vme_controller(1,ptr,x02,rcv);sdly();}
      }else{
        if(ival2==0){vme_controller(1,ptr,x01,rcv);sdly();}
        if(ival2==1){vme_controller(1,ptr,x03,rcv);sdly();}
      }
   }
   data=data+1;
 }  
 for(j=0;j<bit;j++){
   ival=*data>>j;
   ival2=ival&0x01;
   if(j<bit-1){
     if(ival2==0){vme_controller(1,ptr,x00,rcv);sdly();}
     if(ival2==1){vme_controller(1,ptr,x02,rcv);sdly();}

   }else{
     if(ival2==0){vme_controller(1,ptr,x01,rcv);sdly();}
     if(ival2==1){vme_controller(1,ptr,x03,rcv);sdly();}
   }
 }
  vme_controller(1,ptr,x01,rcv);sdly();       
  vme_controller(3,ptr,x00,rcv);sdly();       

}

void  VMEController::daqmb_fifo(int irdwr,int ififo,int nbyte,unsigned short int *buf,unsigned char *rcv)
{
 int i;
 unsigned short int *data;
 unsigned short int *ptr;
 unsigned short int tird[4]={1,0,1,0};
 unsigned short int fifo[1];
 unsigned short int tmp[2]={0x0000,0x0000}; 
  /* irdwr=0 read, irdwr=1 write */
   
 if(tird[irdwr]==0){
   // printf("  read %d 16 bit words \n",nbyte/2);
          data=(unsigned short int  *)buf;
          ptr=(unsigned short int *)add_fifo_rli;
          for(i=0;i<nbyte/2-1;i++){
	     vme_controller(0,ptr,data,(char *)rcv); //*data=*ptr;  
             // printf("fifo R %d %08x \n",i,ptr); 
             data=data+1; 
          }	
          vme_controller(2,ptr,data,(char *)rcv); //*data=*ptr;  
          // printf("last fifo R %d %08x %04x \n",i,ptr,*rcv); 
      }
      if(tird[irdwr]==1){
      /* sel fifo */
      if(ififo==1)fifo[0]=0x01;  //fifo1
      if(ififo==2)fifo[0]=0x02;  //fifo2
      if(ififo==3)fifo[0]=0x04;  //fifo3
      if(ififo==4)fifo[0]=0x08;  //fifo4
      if(ififo==5)fifo[0]=0x10;  //fifo5
      if(ififo==6)fifo[0]=0x20;  //fifo6
      if(ififo==7)fifo[0]=0x40;  //fifo7    
      ptr=(unsigned short int *)add_fifo_ws;
      vme_controller(1,ptr,fifo,(char *)rcv);   
      // printf("  write %d 16 bit words \n",nbyte/2);
          data=(unsigned short int *)buf;
          ptr=(unsigned short int *)add_fifo_w00;
          for(i=0;i<nbyte/2-1;i++){
	     vme_controller(1,ptr,data,(char *)rcv);     //*ptr=*data; 
             // printf("     fifo W %d %08x %04x \n",i,ptr,*data); 
             data=data+1; 
          }
          ptr=(unsigned short int *)add_fifo_w01; /* last word bit */
	       //  printf("last fifo W %d %08x %04x \n",i,ptr,*data); 
	  vme_controller(1,ptr,data,(char *)rcv); //*ptr=*data;
          ptr=(unsigned short int *)add_fifo_ws;
	  vme_controller(3,ptr,tmp,(char *)rcv); //*ptr=0x00;
      } 
}


void VMEController::setuse()
{
char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1];
unsigned short int *ptrreg;

 ptrreg=(unsigned short int *) add_sw;
 tmp2[0]=feuse;
 vme_controller(1,ptrreg,tmp2,tmp);
 // x*ptrreg=feuse;
 //  printf(" setuse %08x %04x \n",ptrreg,feuse&0xffff);
}


void VMEController::scan_alct(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  int i, j, k, bit, cnt8;
 const unsigned short int TCK=4;
 const unsigned short int TMS=2;
 unsigned short int ival, d, dd;
 unsigned short int *ptr;
 unsigned char *rcv2, bdata, *data, mytmp[MAXLINE];
 int buff_mode;

 int debug = 0;

 if(cnt<0 || reg<0 || reg>1)return;
 for(int i=0;i<MAXLINE;i++) mytmp[i] = 0;
 ptr=(unsigned short int *)add_ucla;
 rcv2=(unsigned char *)rcv;
 data=(unsigned char *)snd;

// Jinghua Liu:
// If more than 290 bits, can't put them into a single packet,
// need better algorithm later......
 buff_mode = (cnt>290)?3:1;

   if (debug) {
      printf("scan_alct: reg=%d, cnt=%d, ird=%d, Send %02x %02x\n", reg, cnt, ird, snd[0]&0xff, snd[1]&0xff);
   }

   //  reg=0: instruction
   //  reg=1: data
 
   for(i=reg; i<6; i++)
   {
     d=pvme;
     if(i>(reg+1) && i<4) d |=TMS;
     for(j=0;j<3;j++)
     {  
        // each shift needs 3 VME writes, the 2nd one with TCK on:
        dd=d;
        if(j==1) dd |= TCK;
        vme_controller(1,ptr,&dd,rcv);        
     }
   }

  // Loop to shift in/out bits
  bit=0;
  k=0;
  bdata = data[k];
  for(i=0;i<cnt;i++) 
  {
     ival = bdata&0x01;
     bdata >>= 1;
     bit++;
     if(bit==8) 
     { 
        bit=0;
        k++;
        bdata = data[k];
     }

     if(ird){
       // read out one bit, the last argument is just a dummy.
       vme_controller(0,ptr,&dd,rcv);
     }
     
     // data bit to write (ival) is in the lowest bit (TDI)
     d=pvme|ival;
     // at the last shift, we need set TMS=1
     if(i==cnt-1) d |=TMS;
     for(j=0;j<3;j++)
     {
        dd=d;
        if(j==1) dd |= TCK;
        // buff_mode could be either 3 (send and READ!!!) or 1 (buffered):
        vme_controller((j==2)?buff_mode:1,ptr,&dd,(char *)(mytmp+2*i));
     }
  }
  
  // printf("done loop\n");
  // Now put the state machine into idle.
  for(i=0; i<2; i++)
  {
     d=pvme;
     if(i==0) d |=TMS;
     for(j=0;j<3;j++)
     {  
        dd=d;
        if(j==1) dd |= TCK;
        // In the last VME WRITE send the packets. And READ back all data 
        // in the case of fully buffered mode (buff_mode=1).
        vme_controller((i==1 && j==2)?3:1,ptr,&dd,(char *)mytmp);        
     }
  }

  if(ird)
  {
    // combine bits in mytmp[] into bytes in rcv[].
    //  for(i=0; i<cnt*2; i++) printf("%02x ", mytmp[i]&0xff);
    //  printf("\n");
     bit=0; 
     bdata=0; 
     j=0;
    // Use cnt8 instead of cnt, to make sure the bits are in right place.
     cnt8 = 8*((cnt+7)/8);
     for(i=0;i<cnt8;i++)
     {
         if(i<cnt) bdata |= mytmp[2*i+1] & 0x80;
         if(bit==7)
           { rcv2[j++]=bdata;  bit=0;  bdata=0; }
         else
           { bdata >>= 1;     bit++; }
     }
     if (debug) {
        printf("   Output: ");
        for(i=0; i<cnt8/8; i++) printf("%02X ", rcv2[i]);
        printf("\n");
     }
  }
}


void VMEController::RestoreIdle_alct()
{
int k;
unsigned short int d[3];
char tmp[2];
unsigned short int *ptr;
unsigned short int clkon={0x0004};
 ptr=(unsigned short int *)add_ucla;
 // fprintf(fplog," enter restore idle ucla %08x %04x \n",ptr,pvme);
  d[0]=0x0002|pvme;d[1]=0x0002|pvme|clkon;d[2]=0x0002|pvme;
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);
  d[0]=0x0000|pvme;d[1]=0x0000|pvme|clkon;d[2]=0x0000|pvme;
  for(k=0;k<3;k++)vme_controller(3,ptr,d+k,tmp);

}

#endif

