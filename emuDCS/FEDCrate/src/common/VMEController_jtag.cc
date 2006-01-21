#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h> 
#include "VMEController.h"
#include "VMEModule.h"
#include "geom.h"
#include "geom_def.h"
#include "vme_cmd.h"



int delay_type;
char adcreg[5]={0x00,0x0e,0x0d,0x0b,0x07};
char adcbyt[3][8]={{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}};

unsigned short int tird[3]={1,1,3};

int pows(int n,int m);

char *bufvme;
int cntvme;
int cntvme_rd;

#define debugV 0

/* register 1-7 special commands 0x10-rs 0x11-w feb power 0x12-r febpower */


void VMEController::devdo(enum DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
{
  char cmd2[9000];
  
  char tmp[4];
  int kbit,kbybit;
  char kbypass;
  
  int ppnt,pcmd,pow2;
  int idev,i,k,m;
  int ncmd2,nbcmd2,nbuf2;
  
 
  int init;
  

  unsigned short int ishft,temp;
  unsigned long int vmeaddo;
  static int feuse;  

  /* irdsnd for jtag
          irdsnd = 0 send immediately, no read
          irdsnd = 1 send immediately, read
          irdsnd = 2 send in buffer, no read
  */
  if(dev!=99){
  idev=geo[dev].jchan;
  }else{
    idev=idevo;
    if(idev>4&idev!=12)return;
  }
  // printf(" enter devdo %d %d \n",dev,idev);

  // printf(" idev idevo dev %d %d %d \n",idev,idevo,dev);
  /****** check we have same old device otherwise we need to initialize */
  init=0;
    if(idev!=idevo||vmeadd!=vmeaddo){
       init=1;
    }
  idevo=idev;
  vmeaddo=vmeadd;

  //  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo,init);
/************  JTAG initialize ******************/
/************  immediate instruction nonJTAG ****/

  switch(idev){

   case 1:  /* JTAG */
     if(init==1){ 
      feuse=0xff;
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
     }
   break;

   case 2:   /* JTAG */ 
     if(init==1){
      feuse=0xff;
      ife=0; 
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

   case 3:   /* JTAG */ 
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

  case 4:   /* JTAG */
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

  case 5:   /* JTAG */
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk05|msk_i;
     add_d=vmeadd|msk05|msk_d;
     add_dh=vmeadd|msk05|msk_dh;
     add_ds=vmeadd|msk05|msk_ds;
     add_dt=vmeadd|msk05|msk_dt;
     add_rst=vmeadd|msk05|msk_rst;
     add_sw=vmeadd|msk05|msk_sw;
     add_sr=vmeadd|msk05|msk_sr;
     add_r=vmeadd|msk05|msk_r;    
    }
   break;

  case 6:   /* JTAG */
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk06|msk_i;
     add_d=vmeadd|msk06|msk_d;
     add_dh=vmeadd|msk06|msk_dh;
     add_ds=vmeadd|msk06|msk_ds;
     add_dt=vmeadd|msk06|msk_dt;
     add_rst=vmeadd|msk06|msk_rst;
     add_sw=vmeadd|msk06|msk_sw;
     add_sr=vmeadd|msk06|msk_sr;
     add_r=vmeadd|msk06|msk_r;    
    }
   break;

  case 7:   /* JTAG */
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk07|msk_i;
     add_d=vmeadd|msk07|msk_d;
     add_dh=vmeadd|msk07|msk_dh;
     add_ds=vmeadd|msk07|msk_ds;
     add_dt=vmeadd|msk07|msk_dt;
     add_rst=vmeadd|msk07|msk_rst;
     add_sw=vmeadd|msk07|msk_sw;
     add_sr=vmeadd|msk07|msk_sr;
     add_r=vmeadd|msk07|msk_r;    
    }
   break;

  case 8:   /* JTAG */
    if(init==1){
     feuse=0xff;
     ife=0;
     add_i=vmeadd|msk08|msk_i;
     add_d=vmeadd|msk08|msk_d;
     add_dh=vmeadd|msk08|msk_dh;
     add_ds=vmeadd|msk08|msk_ds;
     add_dt=vmeadd|msk08|msk_dt;
     add_rst=vmeadd|msk08|msk_rst;
     add_sw=vmeadd|msk08|msk_sw;
     add_sr=vmeadd|msk08|msk_sr;
     add_r=vmeadd|msk08|msk_r;    
    }
   break;

  case 9:
     add_vmepara=vmeadd|msk_vmepara;
     vmepara(cmd,inbuf,outbuf);
   break;
 
  case 10: 
     add_vmeser=vmeadd|msk_vmeser;
     vmeser(cmd,inbuf,outbuf);
   break;

  case 11: 
     add_dcc_r=vmeadd|msk00|msk_dcc_r;
     add_dcc_w=vmeadd|msk00|msk_dcc_w;
     dcc(cmd,outbuf); 
   break;

   case 12:   /* RESET emergency VME PROM loading */ 
     if(init==1){
       feuse=0x99;
       ife=99;
       add_reset=vmeadd|msk0f;
     }
   break;

   case 13:
     add_adcr=vmeadd|msk0d|msk_adcr;
     add_adcw=vmeadd|msk0d|msk_adcw;
     add_adcrbb=vmeadd|msk0d|msk_adcrbb;
     add_adcrs=vmeadd|msk0d|msk_adcrs; 
     add_adcws=vmeadd|msk0d|msk_adcws; 
     vme_adc(cmd[0],cmd[1],outbuf);
    break;  
  }

/**********  end initialize ***********************/
/**********  send the JTAG data ************************/ 
if(idev<=8||idev==12){
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
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 5: /* jtag */
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
    case 6: /* jtag */
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
    case 7: /* jtag */
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
    case 8: /* jtag */
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
   case 12: /* reset vme  prom */
      // printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle_reset();break;}
      if(ncmd>0){
      if(nbuf>0){
	scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan_reset(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
    break;

}
/*************  end of senddata **********************/
}


void VMEController::scan(int reg,const char *snd,int cnt,char *rcv,int ird)
{
int i;
int cnt2;

int byte,bit;
unsigned short int tmp[2]={0x0000};
unsigned short int *data;

unsigned short int *ptr_i;
unsigned short int *ptr_d;
unsigned short int *ptr_dh;
unsigned short int *ptr_ds;
unsigned short int *ptr_dt;
unsigned short int *ptr_r;
 
if(cnt==0)return;


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


void VMEController::initDevice(int idev) {
  if(debugV)cout << "InitDevice " << idev << endl;
    vmeadd=0x00000000;
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


int pows(int n,int m)
{int l,i;
l=1;
 for(i=0;i<m;i++)l=l*n;
return l;
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

void VMEController::sdly()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
 delay_type=2;
 tmp2[0]=50;
 vme_controller(6,ptr,tmp2,tmp);
}


void  VMEController::sleep_vme(const char *outbuf)   // in usecs (min 16 usec)
{

char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
// printf(" outbuf[0-1] %02x %02x \n",outbuf[0]&0xff,outbuf[1]&0xff);
 delay_type=3; 
       tmp2[0]=(outbuf[1]<<8)|outbuf[0];
       tmp2[0]=(unsigned short int)(tmp2[0]/16.0);
       tmp2[0]=tmp2[0]+1;
       vme_controller(6,ptr,tmp2,tmp);
}

void  VMEController::sleep_vme2(unsigned short int time) // time in usec
{
float tmp_time;
unsigned short int itime;
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
 delay_type=3;
       tmp_time=time/16.384;
       itime=(unsigned short int)(tmp_time);
       itime=itime+1;
       tmp2[0]=itime;
       vme_controller(6,ptr,tmp2,tmp);
}

void  VMEController::long_sleep_vme2(float time)   // time in usec
{
float tmp_time;
unsigned long int itime[0];
char tmp[1]={0x00};
unsigned short int *tmp2;
unsigned short int *ptr;
 delay_type=4;
       tmp_time=time/0.004;
       itime[0]=(unsigned long int)(tmp_time+1);
       printf(" time %f tmp_time %f itime %08lx \n",time,tmp_time,itime[0]);
       tmp2=(unsigned short int *)itime;
       vme_controller(6,ptr,tmp2,tmp);
}

void VMEController::handshake_vme()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
       add_control_r=msk_control_r;   
       ptr=(unsigned short int *)add_control_r;
       vme_controller(4,ptr,tmp2,tmp); // flush
       vme_controller(5,ptr,tmp2,tmp); // handshake
}

void VMEController::flush_vme()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
// printf(" flush buffers to VME \n");
       vme_controller(4,ptr,tmp2,tmp); // flush
}

void VMEController::vmeser(const char *cmd,const char *snd,char *rcv)
{
 int i;
 int nt;
 short int seri[16]={2,2,0,0,4,6,0,2,0,2,0,0,4,6,0,2};
 unsigned short int icmd;
 unsigned short int iadr;
 
 unsigned long int add_vmesert;
 unsigned short int *ptr;
 
 char tr;
 unsigned short int tmp[1]={0x0000};
 int nrcv;
/* void VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
           irdwr:   
              0 bufread
              1 bufwrite 
              2 bufread snd  
              3 bufwrite snd 
              4 flush to VME
              5 loop back 
              6 delay
*/
	add_vmesert=add_vmeser+(cmd[0]&0x000f)*4096+4*(cmd[1]&0x000f);
        icmd=cmd[1]&0x000f;  //DDU command
        iadr=cmd[0]&0x000f;  //DDU device
        if((icmd>8&&iadr==4)||(iadr>=8)){  //write, but ignore snd data
	  ptr=(unsigned short int *)add_vmesert;
          vme_controller(3,ptr,tmp,rcv);
	}
	else if(icmd<9&&iadr==4){      //read
          nt=seri[icmd]/2;
          for(i=0;i<nt-1;i++){
	    ptr=(unsigned short int *)add_vmesert;
            vme_controller(0,ptr,tmp,rcv);
          }
	  ptr=(unsigned short int *)add_vmesert;
	  vme_controller(2,ptr,tmp,rcv);

          nrcv=nt*2;
	  //	  printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
          for(i=0;i<nt;i++){
	    tr=rcv[2*i]; rcv[2*i]=rcv[(2*i)+1]; rcv[2*i+1]=tr;
	    //	    printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
	  }
	  //	  printf("\n");

        }
	else if(iadr<4){      //read
          nt=2;
          for(i=0;i<nt-1;i++){
	    ptr=(unsigned short int *)add_vmesert;
            vme_controller(0,ptr,tmp,rcv);
          }
	  ptr=(unsigned short int *)add_vmesert;
	  vme_controller(2,ptr,tmp,rcv);

          nrcv=nt*2;
	  //	  printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
          for(i=0;i<nt;i++){
	    tr=rcv[2*i]; rcv[2*i]=rcv[(2*i)+1]; rcv[2*i+1]=tr;
	    //	    printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
	  }
	  //	  printf("\n");
        }
     return;
}

void VMEController::vmepara(const char *cmd,const char *snd,char *rcv)
{
int i,nt;

unsigned long int add_vmepart;
unsigned short int idev, icmd;
unsigned short int *ptr;
unsigned short int *data;
unsigned short int tmp[1]={0x0000};

 int nrcv;
   icmd=cmd[1]&0x00ff;  //0-127 read, >=128 write
   idev=cmd[0]&0x000f;  //0-7 CMD ignored, >=8 CMD req'd.
   add_vmepart=add_vmepara+(4096*idev)+(4*icmd);

//JRG, added Write case:
   if(icmd>127&&idev>=8){  //Write
     nt=2;
     nt=nt/2; 
     data=(unsigned short int *)snd;
     for(i=0;i<nt-1;i++){
       ptr=(unsigned short int *)add_vmepart;
       vme_controller(1,ptr,data,rcv);
       data=data+1;
     } 
     ptr=(unsigned short int *)add_vmepart;
     vme_controller(3,ptr,data,rcv);
   }
//JRG, end Write case ^^^^

   else{   //Read
     nt=1;
     ptr=(unsigned short int *)add_vmepart;
     vme_controller(2,ptr,tmp,rcv);
     nrcv=nt*2;
     //     printf(" scan vmepar: return from read, nrcv=%d,",nrcv);
     //     for(i=0;i<nrcv;i++)printf(" %02x",rcv[i]&0xff);
     //     printf("\n");
   }
}

void VMEController::dcc(const char *cmd,char *rcv)
{
unsigned long add;
unsigned short int *ptr;
unsigned short int *data;

unsigned short int tcmd;
char c[2];

// n = readline(sockfd,cmd,4); 
// printf(" dcc: cmd %02x %02x %02x %02x \n",cmd[0]&0xff,cmd[1]&0xff,cmd[2]&0xff,cmd[3]&0xff);
 tcmd=cmd[1]<<2;
 if(cmd[0]==0x00){
   c[0]=cmd[3];
   c[1]=cmd[2]; 
   data=(unsigned short int *)c;

   add=add_dcc_w+tcmd;
   ptr=(unsigned short int *)add;
   vme_controller(3,ptr,data,rcv);
   //  printf(" dcc write: ptr %08x data %04x \n",ptr,*data);
   // printf(" about to write \n");
   // *ptr=*data;
 }
 if(cmd[0]==0x01){
   // data2=rcv;
   add=add_dcc_w+tcmd;
   ptr=(unsigned short int *)add;
   vme_controller(2,ptr,data,rcv);
   // *data2=*ptr; 
   //  printf(" dcc read: ptr %08x data %04x \n",ptr,*data2);
   // n = writen(sockfd,rcv,2);
 }
}

void VMEController::vme_adc(int ichp,int ichn,char *rcv)
{
 
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
      // ptr=(unsigned short int *)add_adcws;
       // printf(" select register%08x  %02x \n",ptr,val[0]&0xff);
      //  vme_controller(3,ptr,&val[0],rcv);     //*ptr=val[0];  
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



