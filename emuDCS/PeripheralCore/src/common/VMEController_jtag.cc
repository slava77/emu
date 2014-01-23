#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h> 
#include "emu/pc/VMEController.h"
#include "emu/pc/VMEModule.h"
#include "emu/pc/geom.h"
#include "emu/pc/Crate.h"
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

#include "emu/pc/vme_cmd.h"

#define debugV 0
#define MAX_DATA 7800

/* register 1-7 special commands 0x10-rs 0x11-w feb power 0x12-r febpower */


namespace emu {
  namespace pc {

void VMEController::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
{
  char cmd2[9000];
  char tmp[4];
  int kbit,kbybit;
  char kbypass;
  int ppnt,pcmd,pow2;
  int idev,i,k,m;
  int ncmd2=0,nbcmd2,nbuf2=0;
  int init;
  int ififo;
  int nbyte;
  unsigned short int ishft,temp;

  //std::cout << "devdo.vmeadd="<<std::hex<<vmeadd<<std::endl;

  /* irdsnd for jtag
          irdsnd = 0 send immediately, no read
          irdsnd = 1 send immediately, read
          irdsnd = 2 send in buffer, no read
  */
  if(dev!=99){
  idev=geo[dev].jchan;
  }else{
    idev=idevo;
    if(idev>4&idev!=11)return;
  }
  // printf(" enter devdo %d %d \n",dev,idev);
  if (DEBUG) {
      printf("devdo: dev=%d, idev=%d, ncmd=%d, nbuf=%d, irdsnd=%d, Cmd %02x %02x\n", 
       dev, idev, ncmd, nbuf, irdsnd, cmd[0]&0xff, cmd[1]&0xff);
   }

  //printf(" ENTERING devdo, idev idevo dev %d %d %d \n",idev,idevo,dev);
  /****** check we have same old device otherwise we need to initialize */
  init=0;
  //
  if(idev!=idevo or vmeadd != vmeaddo ){
    init=1;
  }
  //
  if(idev==1){
    if(dev==1||dev==7||dev==36)feuse=0x01;
    if(dev==2||dev==8||dev==37)feuse=0x02;
    if(dev==3||dev==9||dev==38)feuse=0x04;
    if(dev==4||dev==10||dev==39)feuse=0x08;
    if(dev==5||dev==11||dev==40)feuse=0x10;
    if(dev==6||dev==12)feuse=0x1F;
    if(dev==41)feuse=0x20;
    if(dev==42)feuse=0x40;
    if(dev==43)feuse=0x7F;
    if(feuseo!=feuse)init=1;
    feuseo=feuse;
  }
  //
  idevo=idev;
  vmeaddo = vmeadd;
  //
  //  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo,init);
/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

  //std::cout << "init=" << init << " idev=" << idev << std::endl;

  switch(idev){

   case 1:  /* JTAG feboard */
     /* send down 1 level */
     ife=1;
     add_i=vmeadd|msk01|msk_i;
     add_d=vmeadd|msk01|msk_d;
     add_dh=vmeadd|msk01|msk_dh;
     add_ds=vmeadd|msk01|msk_ds;
     add_dt=vmeadd|msk01|msk_dt;
     add_rst=vmeadd|msk01|msk_rst;
     add_sw=vmeadd|msk01|msk_sw;
     add_sr=vmeadd|msk01|msk_sr;
     add_r=vmeadd|msk01|msk_r;
     if(init==1){        
       setuse();
     }
     break;
     
   case 2:   /* JTAG motherboard control */ 
     //if(init==1){
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
      //}
   break;

   case 3:   /* JTAG motherboard prom */ 
     //if(init==1){
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
      //}
   break;

  case 4:   /* JTAG vme-motherboard prom */
    //if(init==1){
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
     //}
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

  case 9:  // DAQMB VME FPGA register access
    add_vmefpga=vmeadd|msk00;
    if (cmd[0]==0x01) {  //read
      add_vmefpga=add_vmefpga+((cmd[1]<<2)&0xfc);
      unsigned int ptr=add_vmefpga;
      unsigned short int testdata[2]; 
      testdata[0]=0; testdata[1]=0;
      vme_controller(2,ptr,testdata,outbuf);
    }
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
    buckflash(cmd,nbuf,inbuf,outbuf);
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

  //std::cout << "devdo.add_r="<<std::hex<<add_r<<std::endl;
  //std::cout << "devdo.add_i="<<std::hex<<add_i<<std::endl;

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
           if(ppnt<9){pow2=1<<(ppnt-1);cmd2[0]=cmd2[0]+pow2;
	   /*printf(" k cmd %d %02x %d %d \n",k,cmd2[0],ppnt,pow2); */}
           if(ppnt>8){pow2=1<<(ppnt-9);cmd2[1]=cmd2[1]+pow2;} 
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
     temp=((outbuf[2*(nbuf2/16)+1]<<8)&0xff00)|(outbuf[2*(nbuf2/16)]&0xff);
     temp=(temp>>ishft);
     outbuf[2*(nbuf2/16)+1]=(temp&0xff00)>>8;
     outbuf[2*(nbuf2/16)]=temp&0x00ff;
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
     temp=((outbuf[2*(nbuf2/16)+1]<<8)&0xff00)|(outbuf[2*(nbuf2/16)]&0xff);
     temp=(temp>>ishft);
     outbuf[2*(nbuf2/16)+1]=(temp&0xff00)>>8;
     outbuf[2*(nbuf2/16)]=temp&0x00ff;
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
     temp=((outbuf[2*(nbuf2/16)+1]<<8)&0xff00)|(outbuf[2*(nbuf2/16)]&0xff);
     temp=(temp>>ishft);
     outbuf[2*(nbuf2/16)+1]=(temp&0xff00)>>8;
     outbuf[2*(nbuf2/16)]=temp&0x00ff;
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
     temp=((outbuf[2*(nbuf2/16)+1]<<8)&0xff00)|(outbuf[2*(nbuf2/16)]&0xff);
     temp=(temp>>ishft);
     outbuf[2*(nbuf2/16)+1]=(temp&0xff00)>>8;
     outbuf[2*(nbuf2/16)]=temp&0x00ff;
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

    // needed for new DCFEB promloading
void VMEController::SendRUNTESTClks(unsigned long int nclks){
  unsigned int ptr_ds;
  unsigned short int data[2]={0x000,0x0000};
  char rcv[8];  
  unsigned long int words;
  float timetune={1.000000001}; // fraction of timer
  float timer;
  int sleeptime;
  int bit;
  int big,pkt,left;
  int maxsize,numbigpackets,leftoverpacket;
  int cnt2;
  unsigned long int finalbits,finalbits2;
  unsigned long int nclks2=nclks;
  if(nclks==100000)nclks2=5000;
  if(nclks==1000000)nclks2=50000;
  words=nclks2/16;
  maxsize=MAX_DATA/4;   // maximum number of words one can transfer                              
  numbigpackets=words/maxsize;
  leftoverpacket=words-numbigpackets*maxsize;
  bit=nclks2-numbigpackets*maxsize*16-leftoverpacket*16;
  // printf(" nclks2 %ld nwords %ld numbigpackets %d leftoverpacket %d bit %d \n",nclks2,words,numbigpackets,leftoverpacket,bit);                                                                   
    finalbits=0;
    finalbits2=0;
    for(big=0;big<numbigpackets;big++){
      for(pkt=0;pkt<maxsize;pkt++){
	add_ds=add_ds&msk_clr;
	add_ds=add_ds|0x0f00;
	ptr_ds=add_ds;
	if(pkt<maxsize-1){
          vme_controller(1,ptr_ds,data,rcv);
          finalbits2=finalbits2+16;
	}else{
	  vme_controller(3,ptr_ds,data,rcv);
	  timer=maxsize*16*timetune;
          sleeptime=(int)timer;
          finalbits2=finalbits2+16; 
	  finalbits=finalbits+maxsize*16;
	  //  printf(" 0 VME W: %08x %04x \n",ptr_ds,*data);
	  // printf(" runtest data sent so sleep %d \n",sleeptime);fflush(stdout);
	  usleep(sleeptime);
	}
      }
    }
    for(left=0;left<leftoverpacket;left++){
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=add_ds;
      finalbits2=finalbits2+16;
      if(left<leftoverpacket-1){
	vme_controller(1,ptr_ds,data,rcv);
      }else{
	if(bit==0)vme_controller(3,ptr_ds,data,rcv);
	if(bit!=0)vme_controller(1,ptr_ds,data,rcv);
	timer=leftoverpacket*16*timetune;
        sleeptime=(int)timer;
	finalbits=finalbits+leftoverpacket*16;
        //	printf(" 1 VME W: %08x %04x \n",ptr_ds,*data); 
      }
    }
    if(leftoverpacket==0)sleeptime=10;
    if(bit!=0){
      cnt2=bit-1;
      finalbits=finalbits+bit;
      finalbits2=finalbits2+bit;
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|(cnt2<<8);
      ptr_ds=add_ds;
      vme_controller(3,ptr_ds,data,rcv); // last one
      // printf("2 VME W: %08x %04x \n",ptr_ds,*data); 
    }
    if(nclks2!=finalbits2||nclks2!=finalbits)printf(" problem in sendRUNTESTclks %ld %ld %ld \n",nclks2,finalbits,finalbits2);
    printf(" runtest data sent total clks %ld sleep %d \n",finalbits2,sleeptime);fflush(stdout);
    usleep(sleeptime);
    //  printf(" out if sendRUNTESTclks \n");
}

void VMEController::scan(int reg,const char *snd,int cnt,char *rcv,int ird)
{
int i;
int cnt2;
int byte,bit;
unsigned short int tird[3]={3,1,3};
unsigned short int tmp[2]={0x0000};
unsigned short int *data;
unsigned int ptr_i;
unsigned int ptr_d;
unsigned int ptr_dh;
unsigned int ptr_ds;
unsigned int ptr_dt;
unsigned int ptr_r;
 
if(cnt==0)return;

   if (DEBUG) {
      printf("scan: reg=%d, cnt=%d, ird=%d, Send %02x %02x\n", reg, cnt, ird, snd[0]&0xff, snd[1]&0xff);
   }

 cnt2=cnt-1;
 data=(unsigned short int *) snd;

 /* instr */

 //std::cout << "scan.vmeadd="<<std::hex<<vmeadd<<std::endl;
 //std::cout << "scan.add_r="<<std::hex<<add_r<<std::endl;
 //std::cout << "scan.add_i="<<std::hex<<add_i<<std::endl;

 if(reg==0){
   add_i=add_i&msk_clr;
   add_i=add_i|(cnt2<<8);
   ptr_i=add_i;
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
   if(byte==0||(byte==1&&bit==0)){
     add_d=add_d&msk_clr;
     add_d=add_d|(cnt2<<8);
     ptr_d=add_d; 
     // printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
     // xif(bit>8|byte==1)*ptr_d=*data;
     // xif(bit<=8&byte!=1)*ptr_d=((*data)>>8);
     // if(bit<=8&byte!=1)*data=((*data)>>8);
     vme_controller(tird[ird],ptr_d,data,rcv);
     //  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
     if(ird==1){
       ptr_r=add_r;
       // x*data2=*ptr_r;
       // printf(" R %08x \n",ptr_r);
       vme_controller(2,ptr_r,tmp,rcv);
     }
     return;
   }
   add_dh=add_dh&msk_clr;
   add_dh=add_dh|0x0f00;
   ptr_dh=add_dh;
   // printf(" 3 VME W: %08x %04x \n",ptr_dh,*data);
  vme_controller(1,ptr_dh,data,rcv);
  // x*ptr_dh=*data;
  data=data+1;
  if(ird==1){       
     ptr_r=add_r;
     // printf("3 R %08x \n",ptr_r);
     vme_controller(0,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }
  add_ds=add_ds&msk_clr;
  ptr_ds=add_ds;
  for(i=0;i<byte-1;i++){
    if(i==(byte-2)&&bit==0){
      add_dt=add_dt&msk_clr;
      add_dt=add_dt|0x0f00;
      ptr_dt=add_dt;
      // printf("4 VME W: %08x %04x \n",ptr_dt,*data);
      vme_controller(tird[ird],ptr_dt,data,rcv);
      // x*ptr_dt=*data;
      if(ird==1){
        ptr_r=add_r;
	//  printf("4 R %08x \n",ptr_r);
        vme_controller(2,ptr_r,data,rcv);
        // x*data2=*ptr_r;  
        // printf(" rddata %04x \n",*data2);
      }
      return;
    }else{
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=add_ds;
      // printf("5 VME W: %08x %04x \n",ptr_ds,*data);
      vme_controller(1,ptr_ds,data,rcv);
      // x*ptr_ds=*data;
      data=data+1;
      if(ird==1){
        ptr_r=add_r;
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
  ptr_dt=add_dt; 
  // printf("6 VME W: %08x %04x \n",ptr_dt,*data);
  // xif(bit>8)*ptr_dt=*data;
  // xif(bit<=8)*ptr_dt=*data>>8;
  // if(bit<=8)*data=*data>>8;
  vme_controller(tird[ird],ptr_dt,data,rcv);
  if(ird==1){
     ptr_r=add_r;
     // printf(" R %08x \n",ptr_r);
     vme_controller(2,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }

  return;
 }
}

void VMEController::scan_dmb_headtail(int reg,const char *snd,int cnt,char *rcv,int ird,int headtail, int when)
{
int i;
int cnt2;
int byte,bit;
int tird[2]={0, 2};
int tiwt[2]={1, 3};
unsigned short int tmp[2]={0x0000};
unsigned short int *data;
unsigned int ptr_i;
unsigned int ptr_dh;
unsigned int ptr_ds;
unsigned int ptr_dt;
unsigned int ptr_r;
 
 if(cnt==0)return;
 if(when!=0) when=1;
 if(ird==1 && reg==1) tiwt[1]=1;  // if READ is needed, then WRITEs are all buffered 

   if (DEBUG) {
      printf("scan: reg=%d, cnt=%d, ird=%d, when=%d, Send %02x %02x\n", reg, cnt, ird, when, snd[0]&0xff, snd[1]&0xff);
        }

 cnt2=cnt-1;
 data=(unsigned short int *) snd;

 /* instr */

 //std::cout << "scan.vmeadd="<<std::hex<<vmeadd<<std::endl;
 //std::cout << "scan.add_r="<<std::hex<<add_r<<std::endl;
 //std::cout << "scan.add_i="<<std::hex<<add_i<<std::endl;

 if(reg==0){
   add_i=add_i&msk_clr;
   add_i=add_i|(cnt2<<8);
   ptr_i=add_i;
   bit=cnt; 
   // xif(bit>8)ptr_i=*data;
   // xif(bit<=8)ptr_i=((*data)>>8);
   // if(bit<=8)*data=((*data)>>8);
   // printf(" 1 VME W: %08x %04x \n",ptr_i,*data);
   vme_controller(tiwt[when],ptr_i,data,rcv);
   return;
 }

 /* data */

  if(reg==1){
   byte=cnt/16;
   bit=cnt-byte*16;
   // printf(" bit byte %d %d %d %d \n",bit,byte,ird,when);
   if(byte==0||(byte==1&&bit==0)){
     add_ds=add_ds&msk_clr;
     add_ds=add_ds|(cnt2<<8);
     ptr_ds=add_ds; 
     // printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
     // xif(bit>8|byte==1)ptr_d=*data;
     // xif(bit<=8&byte!=1)ptr_d=((*data)>>8);
     // if(bit<=8&byte!=1)*data=((*data)>>8);
     vme_controller(tiwt[when],ptr_ds,data,rcv);
     //  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
     if(ird==1){
       ptr_r=add_r;
       // x*data2=ptr_r;
       // printf(" R %08x \n",ptr_r);
       vme_controller(tird[when],ptr_r,tmp,rcv);
     }
     return;
   }
   if(headtail==3){
     add_dh=add_dh&msk_clr;
     add_dh=add_dh|0x0f00;
     ptr_dh=add_dh;
     vme_controller(1,ptr_dh,data,rcv);
   }else{
     add_ds=add_ds&msk_clr;
     add_ds=add_ds|0x0f00;
     ptr_ds=add_ds;
     vme_controller(1,ptr_ds,data,rcv);
   }
  // x*ptr_dh=*data;
  data=data+1;
  if(ird==1){       
     ptr_r=add_r;
     // printf("3 R %08x \n",ptr_r);
     vme_controller(0,ptr_r,tmp,rcv);
     // x*data2=ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }
  add_ds=add_ds&msk_clr;
  ptr_ds=add_ds;
  for(i=0;i<byte-1;i++){
    if(i==(byte-2)&&bit==0){
      if(headtail==1){
        add_dt=add_dt&msk_clr;
        add_dt=add_dt|0x0f00;
        ptr_dt=add_dt;
        vme_controller(tiwt[when],ptr_dt,data,rcv);
      }else{
        add_ds=add_ds&msk_clr;
        add_ds=add_ds|0x0f00;
        ptr_ds=add_ds;
        vme_controller(tiwt[when],ptr_ds,data,rcv);
      }
      // x*ptr_dt=*data;
      if(ird==1){
        ptr_r=add_r;
	//  printf("4 R %08x \n",ptr_r);
        vme_controller(tird[when],ptr_r,data,rcv);
        // x*data2=ptr_r;  
        // printf(" rddata %04x \n",*data2);
      }
      return;
    }else{
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=add_ds;
      // printf("5 VME W: %08x %04x \n",ptr_ds,*data);
      vme_controller(1,ptr_ds,data,rcv);
      // x*ptr_ds=*data;
      data=data+1;
      if(ird==1){
        ptr_r=add_r;
        // printf(" R %08x \n",ptr_r);
        vme_controller(0,ptr_r,tmp,rcv);
        // x*data2=ptr_r; 
	// printf(" rddata %04x \n",*data2);
      }
    }
  }
  cnt2=bit-1;
  if(headtail==1){
    add_dt=add_dt&msk_clr;
    add_dt=add_dt|(cnt2<<8);
    ptr_dt=add_dt; 
    vme_controller(tiwt[when],ptr_dt,data,rcv);
  }else{
    add_ds=add_ds&msk_clr;
    add_ds=add_ds|(cnt2<<8);
    ptr_ds=add_ds; 
    vme_controller(tiwt[when],ptr_ds,data,rcv);
  }
  if(ird==1){
     ptr_r=add_r;
     // printf(" R %08x \n",ptr_r);
     vme_controller(tird[when],ptr_r,tmp,rcv);
     // x*data2=ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }

  return;
 }
}

void VMEController::initDevice(int idev) {
  //if(debugV)cout << "InitDevice " << idev << " " <<(int) feuse << endl;
  //cout << "InitDevice " << idev << " " <<(int) feuse << endl;
  
  //    vmeadd=0x00000000;
  
  switch(idev){
    
  case 1:
    pvme=0x0;
    /* default device like MPC or CCB (no chain)*/
    break;
    
  case 2:
    // ALCT Fast Programming JTAG Chain--done through the user JTAG register
    pvme=0x0098;
    break;
    
  case 3:
    // TMB Mezzanine FPGA +FPGA PROMS JTAG--done through the Bootstrap register
    // During TMB firmware downloads, we want to prevent a known improper 
    // firmware download from blocking the VME bus.  Therefore, we disable the 
    // TMB FPGA from controlling the VME bus (boot reg[11]=1)
    pvme=0x08b8;
    break;
    
  case 4:
    // TMB User Proms JTAG CHAIN--done through the user JTAG
    pvme=0x00d8;
    break;
    
  case 5:
    // TMB FPGA User JTAG chain--relic from old code--keep anyway--do it through the user JTAG register
    pvme=0x00f8;
    break;
    
  case 6:
    // ALCT Slow User JTAG Chain--done through the user JTAG
    pvme=0x0080;
    break;
    
  case 7:
    // ALCT Slow Programming JTAG Chain--done through the user JTAG
    pvme=0x0088;
    break;
    
  case 8:
    // ALCT Fast User JTAG chain--done through the user JTAG
    pvme=0x0090;
    break;
    
  case 9:
    // ALCT Fast Programming JTAG chain--done through the user JTAG
    pvme=0x0098;
    break;
    
  case 10:
    // RAT FPGA+PROMS--done through the user JTAG
    pvme=0x00e8;
    break;
    //
  }
}


void VMEController::RestoreIdle()
{
 char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1]={0x0000};
unsigned int ptr_rst;
 ptr_rst=add_rst;
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
unsigned int ptr;
 int i;
 ptr=add_reset;
 for(i=0;i<5;i++){vme_controller(3,ptr,one,tmp);sdly();}
}

void VMEController::load_cdac(const char *snd)
{
 unsigned int ptr;
 unsigned short int data1,data2;
 char tmp[2]={0x00,0x00};

 ptr=add_cdac;
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
  sleep_vme(100);
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

 unsigned int ptr;
 unsigned short int val[2];
 unsigned short int tmp[2]={0x0000,0x0000}; 
 // printf(" enter vme_adc \n");

      val[0]=adcreg[ichp];
      val[1]=adcbyt[ichp-1][ichn];
      if(val[0]==0){ 
         ptr=add_adcrs;
	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       ptr=add_adcws;
       // printf(" select register%08x  %02x \n",ptr,val[0]&0xff);
	 vme_controller(3,ptr,&val[0],rcv);     //*ptr=val[0];  
         ptr=add_adcw;
         // printf(" adc write %08x %02x \n",ptr,val[1]&0xff);
	 vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
      if(val[0]!=0x07){
          ptr=add_adcr;
      }else{
          ptr=add_adcrbb;
      }
      vme_controller(2,ptr,tmp,rcv);     //*data=*ptr;  

      // print(" adc read %08x %02x %02x\n",ptr,rbuf[1]&0xff,rbuf[0]&0xff);
}

void VMEController::buckflash(const char *cmd,int nbuf,const char *inbuf,char *rcv)
{
 unsigned int ptr;
 unsigned short int tmp[1];
 unsigned short int data[2];
 char *line2;
 int i;
 //printf(" entering buckflash cmd %d  \n",cmd[0);

 if(cmd[0]==0){
   ptr=add_bucip;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==2){
   ptr=add_bucf;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==4){
   ptr=add_buci; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==5){
   ptr=add_buce; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==1){
   ptr=add_bucl;
   for(i=0;i<(nbuf/8)-1;i++){ 
     line2=(char *)inbuf+i-1;
     // data=(unsigned short int *)line2;
     // data[0]=((line2[0]<<8)&0xff00)|(line2[1]&0x00ff);
     data[0]=line2[1]&0x00ff;
     //printf(" TOVME %d %04x \n",i,data[0]);
     vme_controller(1,ptr,data,rcv);    
   }
     line2=(char *)inbuf+(nbuf/8)-1;
     //  data=(unsigned short int *)line2;
     // data[0]=((line2[0]<<8)&0xff00)|(line2[1]&0x00ff);
     data[0]=line2[1]&0x00ff;
     //printf(" TOVME %d %04x \n",(nbuf/8)-1,data[0]);
     vme_controller(3,ptr,data,rcv);    
   return;
 }
 if(cmd[0]==3){
   //  fprintf(fplog," buckflash about to read \n");
   ptr=add_bucr;
   for(i=0;i<nbuf/8;i++){
     vme_controller(0,ptr,tmp,rcv);    
   }
   vme_controller(2,ptr,tmp,rcv);    
   return;
 }
}

/*
void VMEController::buckflash(const char *cmd,const char *inbuf,char *rcv)
{
 unsigned int ptr;
 unsigned short int tmp[1];
 unsigned short int *data;
 char *line2;
 int n,m,nleft,i;
 if(cmd[0]==0){
   ptr=add_bucip;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==2){
   ptr=add_bucf;
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1]);
   tmp[0]=cmd[1];
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==4){
   ptr=add_buci; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==5){
   ptr=add_buce; 
   // fprintf(fplog," VME W: %08x %04x \n",ptr,cmd[1])
   tmp[0]=cmd[1]; 
   vme_controller(3,ptr,tmp,rcv);    // *ptr=cmd[1];
   return;
 }
 if(cmd[0]==1){
   ptr=add_bucl;
   for(i=0;i<294;i++){ 
     line2=(char *)inbuf+i-1;
     printf("%d\n",line2);
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
   ptr=add_bucr;
   for(i=0;i<290;i++){
     vme_controller(0,ptr,tmp,rcv);    
   }
   vme_controller(2,ptr,tmp,rcv);    
   return;
 }
}

*/

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
 unsigned int ptr;
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
         ptr=add_lowrs;
	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       if(val[0]==0x12){ 
         ptr=add_lowrpr;
      	 vme_controller(2,ptr,tmp,rcv);     //*ptr=*data;  
         return;
       }
       if(val[0]==0x11){ 
         ptr=add_lowwpr;
 	 vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
         return;
       } 
       ptr=add_lowws;
       vme_controller(3,ptr,&val[0],rcv);     //*ptr=val[0];   
       ptr=add_loww;
       vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
       ptr=add_lowr; 
       vme_controller(2,ptr,tmp,rcv);     //*data=*ptr;  
}


void  VMEController::scan_reset(int reg,const char *snd, int cnt, char *rcv,int ird)
{
int i,j;
int byte,bit;
unsigned short int x00[1]={0x00};
unsigned short int x01[1]={0x01};
unsigned short int x02[1]={0x02};
unsigned short int x03[1]={0x03};
unsigned short int ival,ival2;
unsigned short int *data;
unsigned int ptr;

 if(cnt==0)return;
 ptr=add_reset;
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

void  VMEController::scan_reset_headtail(int reg,const char *snd, int cnt,char *rcv,int headtail,int ird)
{
int i,j;
int byte,bit;
unsigned short int x00[1]={0x00};
unsigned short int x01[1]={0x01};
unsigned short int x02[1]={0x02};
unsigned short int x03[1]={0x03};
unsigned short int ival,ival2;
unsigned short int *data;
unsigned int ptr;
 if(headtail==3)x01[1]=0x01;
 if(headtail==3)x03[1]=0x03;
 if(headtail==0)x01[1]=0x00;
 if(headtail==0)x03[1]=0x02;
 if(headtail==3)x01[1]=0x01;
 if(headtail==3)x03[1]=0x03;
 if(cnt==0)return;
 ptr=add_reset;
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
 unsigned int ptr;
 unsigned short int tird[4]={1,0,1,0};
 unsigned short int fifo[1];
 unsigned short int tmp[2]={0x0000,0x0000}; 
  /* irdwr=0 read, irdwr=1 write */
   
 if(tird[irdwr]==0){
   // printf("  read %d 16 bit words \n",nbyte/2);
          data=(unsigned short int  *)buf;
          ptr=add_fifo_rli;
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
      ptr=add_fifo_ws;
      vme_controller(1,ptr,fifo,(char *)rcv);   
      // printf("  write %d 16 bit words \n",nbyte/2);
          data=(unsigned short int *)buf;
          ptr=add_fifo_w00;
          for(i=0;i<nbyte/2-1;i++){
	     vme_controller(1,ptr,data,(char *)rcv);     //*ptr=*data; 
             // printf("     fifo W %d %08x %04x \n",i,ptr,*data); 
             data=data+1; 
          }
          ptr=add_fifo_w01; /* last word bit */
	       //  printf("last fifo W %d %08x %04x \n",i,ptr,*data); 
	  vme_controller(1,ptr,data,(char *)rcv); //*ptr=*data;
          ptr=add_fifo_ws;
	  vme_controller(3,ptr,tmp,(char *)rcv); //*ptr=0x00;
      } 
}


void VMEController::setuse()
{
char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1];
unsigned int ptr;

 ptr = add_sw;
 tmp2[0]=feuse;
// for (int iloop=0;iloop<30;iloop++) sdly();
 usleep(1000);
 vme_controller(3,ptr,tmp2,tmp);
// for (int iloop=0;iloop<30;iloop++) sdly();
 usleep(1000);
 // x*ptrreg=feuse;
 //printf(" setuse %08x %04x \n",ptrreg,feuse&0xffff);
}

void VMEController::scan_jtag(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  int i, j, k, bit, cnt8;
  const unsigned short int TCK=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  unsigned short int ival, d, dd;
  unsigned int ptr;
  unsigned char *rcv2, bdata, *data, mytmp[MAXLINE];
  int buff_mode;
  
 if(cnt<0 || reg<0 || reg>1)return;
 for(int i=0;i<MAXLINE;i++) mytmp[i] = 0;
 ptr=add_ucla;
 rcv2=(unsigned char *)rcv;
 data=(unsigned char *)snd;
 
 //cout << "add_ucla = " << add_ucla << std::endl;
 //cout << "TCK= " << TCK << std::endl;
 //cout << "TMS= " << TMS << std::endl;

// Jinghua Liu on Aug-15-2006: 
// With the updated vme_controller() fucntion, it's now safe to
// buffer all VME commands, including cross jumbo packet READs.
 buff_mode = 1;

 //
   if (DEBUG) {
      printf("scan_jtag: reg=%d, cnt=%d, ird=%d, Send %02x %02x\n", reg, cnt, ird, snd[0]&0xff, snd[1]&0xff);
   }

   //  reg=0: instruction
   //  reg=1: data
   //
   for(i=reg; i<4; i++)
   {
     d=pvme;
     if(i<2) d |=TMS;
     for(j=0;j<3;j++)
     {  
        // each shift needs 3 VME writes, the 2nd one with TCK on:
        dd=d;
        if(j==1) dd |= TCK;
	//std::cout << " dd = " << dd << std::endl;
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
     d=pvme|(ival<<TDI_);
     // at the last shift, we need set TMS=1
     if(i==cnt-1) d |=TMS;
     for(j=0;j<3;j++)
     {
        dd=d;
        if(j==1) dd |= TCK;
        // buff_mode could be either 3 (send and READ!!!) or 1 (buffered):
	//std::cout << " dd = " << dd << std::endl;
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
    // for(i=0; i<cnt*2; i++) printf("%02x ", mytmp[i]&0xff);
    //  printf("\n");
     bit=0; 
     bdata=0; 
     j=0;
    // Use cnt8 instead of cnt, to make sure the bits are in right place.
     cnt8 = 8*((cnt+7)/8);
     for(i=0;i<cnt8;i++)
     {
       if(i<cnt) bdata |= ((mytmp[2*i+1] & (1<<TDO_))<<(7-TDO_));
  
       if(bit==7)
	 { rcv2[j++]=bdata;  bit=0;  bdata=0; }
       else
	 { bdata >>= 1;     bit++; }
     }
     if (DEBUG) {
       printf("scan_jtag output: ");
       for(i=0; i<cnt8/8; i++) printf("%02X ", rcv2[i]);
       printf("\n");
     }
  }
}

void VMEController::RestoreIdle_jtag()
{
// LIU, Aug 27, 2011: using vme_controller() instead of VME_controller()

  int k;
  unsigned short int d[3];
  char tmp[2];
  unsigned int ptr;
  unsigned short int clkon=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  ptr=add_ucla;
  // fprintf(fplog," enter restore idle ucla %08x %04x \n",ptr,pvme);
  //
  // std::cout << "ptr="<<ptr<< " pvme= " <<pvme<< std::endl;
  //
  d[0]=TMS|pvme;d[1]=TMS|pvme|clkon;d[2]=TMS|pvme;
  
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  d[0]=0x0000|pvme;d[1]=0x0000|pvme|clkon;d[2]=0x0000|pvme;
  for(k=0;k<3;k++)vme_controller((k==2)?3:1,ptr,d+k,tmp);
  //
}


void VMEController::RestoreReset_jtag()
{
  int k;
  unsigned short int d[3];
  char tmp[2];
  unsigned int ptr;
  unsigned short int clkon=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  ptr=add_ucla;
  // fprintf(fplog," enter restore idle ucla %08x %04x \n",ptr,pvme);
  d[0]=TMS|pvme;d[1]=TMS|pvme|clkon;d[2]=TMS|pvme;
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller(1,ptr,d+k,tmp);
  for(k=0;k<3;k++)vme_controller((k==2)?3:1,ptr,d+k,tmp);
  //d[0]=0x0000|pvme;d[1]=0x0000|pvme|clkon;d[2]=0x0000|pvme;
  //for(k=0;k<3;k++)vme_controller((k==2)?3:1,ptr,d+k,tmp);
  //
}

void VMEController::CycleIdle_jtag(int cycles)
{
  int i, k;
  unsigned short int d[3];
  char tmp[2];
  unsigned int ptr;
  unsigned short int clkon=(1<<TCK_);
  // const unsigned short int TMS=(1<<TMS_);
  ptr=add_ucla;
  if(cycles>1) std::cout << "Cycle JTAG IDLE (TMS=0) " << cycles << " times." << std::endl;
  //
  // std::cout << "ptr="<<ptr<< " pvme= " <<pvme<< std::endl;
  //
  d[0]=pvme;d[1]=pvme|clkon;d[2]=pvme;
  for(i=1;i<=cycles; i++)  
  {
     for(k=0;k<3;k++)vme_controller((k==2 && i==cycles)?3:1,ptr,d+k,tmp);
  }
  //
}

void VMEController::goToScanLevel(){
}

void VMEController::release_plev(){
}

/* register 1-7 special commands 0x10-rs 0x11-w feb power 0x12-r febpower */

void VMEController::new_devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
{
  char cmd2[9000];
  char tmp[4];
  int kbit,kbybit;
  char kbypass;
  int ppnt,pcmd,pow2;
  int idev,i,k,m;
  int ncmd2=0,nbcmd2,nbuf2=0;
  int init;
  int ififo;
  int nbyte;
  unsigned short int ishft,temp;

  //std::cout << "devdo.vmeadd="<<std::hex<<vmeadd<<std::endl;

  /* irdsnd for jtag
          irdsnd = 0 no read, later
          irdsnd = 1 no read, now
          irdsnd = 2    read, later
          irdsnd = 3    read, now
  */
  if (DEBUG) {
      printf("devdo: dev=%d, ncmd=%d, nbuf=%d, irdsnd=%d, Cmd %02x %02x\n", 
       dev, ncmd, nbuf, irdsnd, cmd[0]&0xff, cmd[1]&0xff);
   }
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
  //
  if(idev!=idevo or vmeadd != vmeaddo ){
    init=1;
  }
  //
  if(idev==1){
    if(dev==1||dev==7)feuse=0x01;
    if(dev==2||dev==8)feuse=0x02;
    if(dev==3||dev==9)feuse=0x04;
    if(dev==4||dev==10)feuse=0x08;
    if(dev==5||dev==11)feuse=0x10;
    if(dev==6||dev==12)feuse=0x1F;
    if(feuseo!=feuse)init=1;
    feuseo=feuse;
  }
  //
  idevo=idev;
  vmeaddo = vmeadd;
  //
  //  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo,init);
/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

  //std::cout << "init=" << init << " idev=" << idev << std::endl;

  switch(idev){

   case 1:  /* JTAG feboard */
     /* send down 1 level */
     ife=1;
     add_i=vmeadd|msk01|msk_i;
     add_d=vmeadd|msk01|msk_d;
     add_dh=vmeadd|msk01|msk_dh;
     add_ds=vmeadd|msk01|msk_ds;
     add_dt=vmeadd|msk01|msk_dt;
     add_rst=vmeadd|msk01|msk_rst;
     add_sw=vmeadd|msk01|msk_sw;
     add_sr=vmeadd|msk01|msk_sr;
     add_r=vmeadd|msk01|msk_r;
     if(init==1){        
       setuse();
     }
     break;
     
   case 2:   /* JTAG motherboard control */ 
     //if(init==1){
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
      //}
   break;

   case 3:   /* JTAG motherboard prom */ 
     //if(init==1){
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
      //}
   break;

  case 4:   /* JTAG vme-motherboard prom */
    //if(init==1){
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
     //}
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

  case 9:  // DAQMB VME FPGA register access
    add_vmefpga=vmeadd|msk00;
    if (cmd[0]==0x01) {  //read
      add_vmefpga=add_vmefpga+((cmd[1]<<2)&0xfc);
      unsigned int testadd=add_vmefpga;
      unsigned int ptr=testadd;
      unsigned short int testdata[2]; 
      testdata[0]=0; testdata[1]=0;
      vme_controller(2,ptr,testdata,outbuf);
    }
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
    buckflash(cmd,nbuf,inbuf,outbuf);
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

  //std::cout << "devdo.add_r="<<std::hex<<add_r<<std::endl;
  //std::cout << "devdo.add_i="<<std::hex<<add_i<<std::endl;

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
           if(ppnt<9){pow2=1<<(ppnt-1);cmd2[0]=cmd2[0]+pow2;
	   /*printf(" k cmd %d %02x %d %d \n",k,cmd2[0],ppnt,pow2); */}
           if(ppnt>8){pow2=1<<(ppnt-9);cmd2[1]=cmd2[1]+pow2;} 
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

 if(ncmd<0) RestoreIdle();
 if(idev!=11)
 { 
   if(ncmd>0) scan_dmb(INSTR_REG,cmd2,ncmd2,outbuf,0,(nbuf>0)?0:(irdsnd&1));
//   if(ncmd>0) scan_dmb(INSTR_REG,cmd2,ncmd2,outbuf,0,(irdsnd&1));
//   if(ncmd>0 && nbuf>0) sleep_vme(200); 
   if(nbuf>0) scan_dmb(DATA_REG,inbuf,nbuf2,outbuf,(irdsnd>>1)&1,irdsnd&1);
   if((irdsnd&1)==1 && nbuf2%16!=0)
   {
     ishft=16-nbuf2%16;
     temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
     temp=(temp>>ishft);
     outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
     outbuf[nbuf2/8]=temp&0x00ff;
   }
 }
 else  /* reset vme  prom */
 { 
   // printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
   if(ncmd>0) scan_reset(INSTR_REG,cmd,ncmd,outbuf,irdsnd);
   if(nbuf>0) scan_reset(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
 }
}
}

void VMEController::scan_dmb(int reg,const char *snd,int cnt,char *rcv,int ird, int when)
{
int i;
int cnt2;
int byte,bit;
int tird[2]={0, 2};
int tiwt[2]={1, 3};
unsigned short int tmp[2]={0x0000};
unsigned short int *data;
unsigned int ptr_i;
unsigned int ptr_d;
unsigned int ptr_dh;
unsigned int ptr_ds;
unsigned int ptr_dt;
unsigned int ptr_r;
 
 if(cnt==0)return;
 if(when!=0) when=1;
 if(ird==1 && reg==1) tiwt[1]=1;  // if READ is needed, then WRITEs are all buffered 

   if (DEBUG) {
      printf("scan: reg=%d, cnt=%d, ird=%d, when=%d, Send %02x %02x\n", reg, cnt, ird, when, snd[0]&0xff, snd[1]&0xff);
   }

 cnt2=cnt-1;
 data=(unsigned short int *) snd;

 /* instr */

 //std::cout << "scan.vmeadd="<<std::hex<<vmeadd<<std::endl;
 //std::cout << "scan.add_r="<<std::hex<<add_r<<std::endl;
 //std::cout << "scan.add_i="<<std::hex<<add_i<<std::endl;

 if(reg==0){
   add_i=add_i&msk_clr;
   add_i=add_i|(cnt2<<8);
   ptr_i=add_i;
   bit=cnt; 
   // xif(bit>8)*ptr_i=*data;
   // xif(bit<=8)*ptr_i=((*data)>>8);
   // if(bit<=8)*data=((*data)>>8);
   // printf(" 1 VME W: %08x %04x \n",ptr_i,*data);
   vme_controller(tiwt[when],ptr_i,data,rcv);
   return;
 }

 /* data */

  if(reg==1){
   byte=cnt/16;
   bit=cnt-byte*16;
   // printf(" bit byte %d %d \n",bit,byte);
   if(byte==0||(byte==1&&bit==0)){
     add_d=add_d&msk_clr;
     add_d=add_d|(cnt2<<8);
     ptr_d=add_d; 
     // printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
     // xif(bit>8|byte==1)*ptr_d=*data;
     // xif(bit<=8&byte!=1)*ptr_d=((*data)>>8);
     // if(bit<=8&byte!=1)*data=((*data)>>8);
     vme_controller(tiwt[when],ptr_d,data,rcv);
     //  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
     if(ird==1){
       ptr_r=add_r;
       // x*data2=*ptr_r;
       // printf(" R %08x \n",ptr_r);
       vme_controller(tird[when],ptr_r,tmp,rcv);
     }
     return;
   }
   add_dh=add_dh&msk_clr;
   add_dh=add_dh|0x0f00;
   ptr_dh=add_dh;
   // printf(" 3 VME W: %08x %04x \n",ptr_dh,*data);
  vme_controller(1,ptr_dh,data,rcv);
  // x*ptr_dh=*data;
  data=data+1;
  if(ird==1){       
     ptr_r=add_r;
     // printf("3 R %08x \n",ptr_r);
     vme_controller(0,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }
  add_ds=add_ds&msk_clr;
  ptr_ds=add_ds;
  for(i=0;i<byte-1;i++){
    if(i==(byte-2)&&bit==0){
      add_dt=add_dt&msk_clr;
      add_dt=add_dt|0x0f00;
      ptr_dt=add_dt;
      // printf("4 VME W: %08x %04x \n",ptr_dt,*data);
      vme_controller(tiwt[when],ptr_dt,data,rcv);
      // x*ptr_dt=*data;
      if(ird==1){
        ptr_r=add_r;
	//  printf("4 R %08x \n",ptr_r);
        vme_controller(tird[when],ptr_r,data,rcv);
        // x*data2=*ptr_r;  
        // printf(" rddata %04x \n",*data2);
      }
      return;
    }else{
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=add_ds;
      // printf("5 VME W: %08x %04x \n",ptr_ds,*data);
      vme_controller(1,ptr_ds,data,rcv);
      // x*ptr_ds=*data;
      data=data+1;
      if(ird==1){
        ptr_r=add_r;
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
  ptr_dt=add_dt; 
  // printf("6 VME W: %08x %04x \n",ptr_dt,*data);
  // xif(bit>8)*ptr_dt=*data;
  // xif(bit<=8)*ptr_dt=*data>>8;
  // if(bit<=8)*data=*data>>8;
  vme_controller(tiwt[when],ptr_dt,data,rcv);
  if(ird==1){
     ptr_r=add_r;
     // printf(" R %08x \n",ptr_r);
     vme_controller(tird[when],ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }

  return;
 }
}

void VMEController::shift_bits(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  int i, j, k, bit, cnt8;
  const unsigned short int TCK=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  unsigned short int ival, d, dd;
  unsigned int ptr;
  unsigned char *rcv2, bdata, *data, mytmp[MAXLINE];
  
  if(cnt<0 || reg<0 || reg>1)return;
  for(int i=0;i<MAXLINE;i++) mytmp[i] = 0;
  ptr=add_ucla;
  rcv2=(unsigned char *)rcv;
  data=(unsigned char *)snd;
 
// Jinghua Liu on Aug-15-2006: 
// With the updated vme_controller() fucntion, it's now safe to
// buffer all VME commands, including cross jumbo packet READs.

 //
   if (DEBUG) {
      printf("shift_bits: exit=%d, cnt=%d, ird=%d, Send %02x %02x\n", reg, cnt, ird, snd[0]&0xff, snd[1]&0xff);
   }

   //  reg=1: go to EXIT1-DR or EXIT1-IR at the last bit
   //  reg=0: stay in SHIFT-DR or SHIFT-IR

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
     d=pvme|(ival<<TDI_);
     // if reg==1, at the last shift, we need set TMS=1
     if(reg==1 && i==cnt-1) d |=TMS;
     for(j=0;j<3;j++)
     {
        dd=d;
        if(j==1) dd |= TCK;
        vme_controller((j==2 && i==cnt-1)?3:1,ptr,&dd,(char *)mytmp);
     }
  }
  
  if(ird)
  {
    // combine bits in mytmp[] into bytes in rcv[].
    // for(i=0; i<cnt*2; i++) printf("%02x ", mytmp[i]&0xff);
    //  printf("\n");
     bit=0; 
     bdata=0; 
     j=0;
    // Use cnt8 instead of cnt, to make sure the bits are in right place.
     cnt8 = 8*((cnt+7)/8);
     for(i=0;i<cnt8;i++)
     {
       if(i<cnt) bdata |= ((mytmp[2*i+1] & (1<<TDO_))<<(7-TDO_));
  
       if(bit==7)
	 { rcv2[j++]=bdata;  bit=0;  bdata=0; }
       else
	 { bdata >>= 1;     bit++; }
     }
     if (DEBUG) {
       printf("scan_jtag output: ");
       for(i=0; i<cnt8/8; i++) printf("%02X ", rcv2[i]);
       printf("\n");
     }
  }
}

void VMEController::shift_state(int cnt, int mask)
{
  int i, j, bdata;
  const unsigned short int TCK=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  unsigned short int ival, d, dd;
  unsigned int ptr;
  unsigned char mytmp[1000];
  
  if(cnt<0 || cnt>32)return;
  ptr=add_ucla;
 
  if (DEBUG) {
      printf("shift_state: cnt=%d, tms=%08x\n", cnt, mask);
  }

  // Loop to shift in/out bits
  bdata = mask;
  for(i=0;i<cnt;i++) 
  {
     ival = bdata&0x01;
     bdata >>= 1;
     d=pvme;
     if(ival) d |=TMS;
     for(j=0;j<3;j++)
     {
        dd=d;
        if(j==1) dd |= TCK;
        vme_controller((j==2 && i==cnt-1)?3:1,ptr,&dd,(char *)(mytmp));
     }
  }
  
}

void VMEController::scan_word(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  int i, j, k, bit, cnt8;
  const unsigned short int TCK=(1<<TCK_);
  const unsigned short int TMS=(1<<TMS_);
  unsigned short int ival, d, dd;
  unsigned int ptr;
  unsigned char *rcv2, bdata, *data, mytmp[MAXLINE];
  int buff_mode;
  
 if(cnt<0 || reg<0 || reg>1)return;
 for(int i=0;i<MAXLINE;i++) mytmp[i] = 0;
 ptr=add_ucla;
 rcv2=(unsigned char *)rcv;
 data=(unsigned char *)snd;
 
 //cout << "add_ucla = " << add_ucla << std::endl;
 //cout << "TCK= " << TCK << std::endl;
 //cout << "TMS= " << TMS << std::endl;

// Jinghua Liu on Aug-15-2006: 
// With the updated vme_controller() fucntion, it's now safe to
// buffer all VME commands, including cross jumbo packet READs.
 buff_mode = 1;

 //
   if (DEBUG) {
      printf("scan_jtag: reg=%d, cnt=%d, ird=%d, Send %02x %02x\n", reg, cnt, ird, snd[0]&0xff, snd[1]&0xff);
      // for(int i=0; i<32; i++) printf("%02X ",data[i]&0xff);
      // printf("\n");
   }

   //  reg=0: instruction
   //  reg=1: data
   //
   for(i=reg; i<4; i++)
   {
     d=pvme;
     if(i<2) d |=TMS;
     for(j=0;j<3;j++)
     {  
        // each shift needs 3 VME writes, the 2nd one with TCK on:
        dd=d;
        if(j==1) dd |= TCK;
	//std::cout << " dd = " << dd << std::endl;
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
     d=pvme|(ival<<TDI_);
     // at the last shift, we need set TMS=1
     if(i==cnt-1 || (i%32)==31) d |=TMS;
     for(j=0;j<3;j++)
     {
        dd=d;
        if(j==1) dd |= TCK;
        // buff_mode could be either 3 (send and READ!!!) or 1 (buffered):
	//std::cout << " dd = " << dd << std::endl;
        vme_controller((j==2)?buff_mode:1,ptr,&dd,(char *)(mytmp+2*i));
     }

     // after every word (32 bits), need go through exit->update->idle
     if((i%32)==31 && i<(cnt-1))
     {
        // std::cout << "extra step after bit " << i << std::endl;
        for(int m=0; m<5; m++)
        {
           d=pvme;
           if(m==0 || m==2) d |=TMS;
           for(j=0;j<3;j++)
           {  
               // each shift needs 3 VME writes, the 2nd one with TCK on:
              dd=d;
              if(j==1) dd |= TCK;
	      //std::cout << " dd = " << dd << std::endl;
              vme_controller(1,ptr,&dd,rcv);        
           }
        }
     }
     // std::cout << i << " bits done" << std::endl;        
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
    // for(i=0; i<cnt*2; i++) printf("%02x ", mytmp[i]&0xff);
    //  printf("\n");
     bit=0; 
     bdata=0; 
     j=0;
    // Use cnt8 instead of cnt, to make sure the bits are in right place.
     cnt8 = 8*((cnt+7)/8);
     for(i=0;i<cnt8;i++)
     {
       if(i<cnt) bdata |= ((mytmp[2*i+1] & (1<<TDO_))<<(7-TDO_));
  
       if(bit==7)
	 { rcv2[j++]=bdata;  bit=0;  bdata=0; }
       else
	 { bdata >>= 1;     bit++; }
     }
     if (DEBUG) {
       printf("scan_jtag output: ");
       for(i=0; i<cnt8/8; i++) printf("%02X ", rcv2[i]);
       printf("\n");
     }
  }
}

void VMEController::DCFEBEPROM_read(DEVTYPE dv,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int ird,int snd,int init)
{
  if(init==1){
    if(dv==36)feuse=0x01;
    if(dv==37)feuse=0x02;
    if(dv==38)feuse=0x04;
    if(dv==39)feuse=0x08;
    if(dv==40)feuse=0x10;
    if(dv==41)feuse=0x20;
    if(dv==42)feuse=0x40;
    if(dv==43)feuse=0x1F;
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
    scan_dmb(INSTR_REG,cmd,ncmd,outbuf,0,0);
    scan_dmb(DATA_REG,inbuf,nbuf,outbuf,ird,snd);
 }

} // namespace emu::pc  
} // namespace emu  
