//-----------------------------------------------------------------------
// $Id: VMEController_jtag.cc,v 2.0 2005/04/12 08:07:06 geurts Exp $
// $Log: VMEController_jtag.cc,v $
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
