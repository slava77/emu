//----------------------------------------------------------------------
// $Id: VMEController.cc,v 1.2 2006/01/21 19:55:02 gilmore Exp $
// $Log: VMEController.cc,v $
// Revision 1.2  2006/01/21 19:55:02  gilmore
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
#include <errno.h>
#include <string.h>



#include <unistd.h>

#include <pthread.h>
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "vmeIRQ.h"

#define DELAY2 0.016
#define DELAY3 16.384

struct IRQData irqdata[2];

pthread_t threads[2];
int irq_start=0;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif


extern int delay_type;
long OpenBHandle[4][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};




VMEController::VMEController(int Device, int Link): 
  theBHandle(-1), Device_(Device), Link_(Link), theCurrentModule(0),indian(SWAP)
{

CVBoardTypes VMEBoard;
short Lin;
short Dev;
VMEBoard=cvV2718;
long BHandle; 
    cout << "constructing VMEController " << endl;
    Dev=Device;
    Lin=Link;
    if(OpenBHandle[Dev][Lin]==-1){
    if(CAENVME_Init(VMEBoard,Device,Link,&BHandle) !=cvSuccess){
      printf("\n\n Error in Opening CAEN Controller \n");
      exit(1);
    }
    }else{
      BHandle=OpenBHandle[Dev][Lin];
    }
    theBHandle=BHandle;
    OpenBHandle[Dev][Lin]=BHandle;
}


VMEController::~VMEController(){
  cout << "destructing VMEController .. closing socket " << endl;
   CAEN_close();
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


int VMEController::CAEN_reset(void)
{ 
  /* blank for now, little reason to reset */
  return 0;
}

int VMEController::CAEN_read(unsigned long Address,unsigned short int *data)
{  
int err;
CVAddressModifier AM=cvA24_U_DATA;
CVDataWidth DW=cvD16;
// printf("theBHandle %08x \n",theBHandle);
   err=CAENVME_ReadCycle(theBHandle,Address,data,AM,DW);
   if(err!=0)caen_err=err;
   // printf(" CAENVME read err %d \n",caen_err);
   //printf(" read: address %08x data %04x \n",Address,*data);
   return err;
}


int VMEController::CAEN_write(unsigned long Address,unsigned short int *data)
{
int err;
CVAddressModifier AM=cvA24_U_DATA;
CVDataWidth DW=cvD16;

//printf("theBHandle %08x \n",theBHandle);
//printf(" write: address %08x data %04x \n",Address,*data);
   err=CAENVME_WriteCycle(theBHandle,Address,(char *)data,AM,DW); 
   if(err!=0)caen_err=err;
   // printf(" CAENVME write err %d \n",caen_err);
   return err;
}


void VMEController::CAEN_close(void)
{
    CAENVME_End(theBHandle);
}


void VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
{
static int ird=0;
static long int packet_delay=0;
char rdata[2];

long unsigned int pttr;
  /* irdwr:   
              0 bufread
              1 bufwrite 
              2 bufread snd  
              3 bufwrite snd 
              4 flush to VME
              5 loop back 
              6 delay
*/
 pttr=(long unsigned int)ptr;
 if(irdwr==0){
   CAEN_read(pttr,(unsigned short int *)rdata);
   rcv[ird]=rdata[0];
   ird=ird+1;
   rcv[ird]=rdata[1];
   ird=ird+1;
 }else if(irdwr==1){
   CAEN_write(pttr,data);
 }else if(irdwr==2){
   CAEN_read(pttr,(unsigned short int *)rdata);
   rcv[ird]=rdata[0];
   ird=ird+1;
   rcv[ird]=rdata[1];
   ird=ird+1;
   ird=0;
 }else if(irdwr==3){
   CAEN_write(pttr,data);
 }else if(irdwr==6){
   if(delay_type==2)packet_delay=(long int)((*data)*DELAY2);
   if(delay_type==3)packet_delay=(long int)((*data)*DELAY3);
   udelay(packet_delay);
 }

}


void *VMEController::IRQ_Interrupt_Handler(void *threadarg)
{
struct IRQData *locdata;
long          BHandle;
unsigned long Address;
unsigned char Data[2];
CVAddressModifier AM=cvA24_U_DATA;
CVDataWidth DW=cvD16;
CVIRQLevels IRQLevel=cvIRQ1;
unsigned long mask=0x00000001;

int ierr;
int j;
unsigned int ERR,SYNC,FMM,SLOT,NUM_ERR,NUM_SYNC;
unsigned int status,tmp;
unsigned int last_status[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int irqmap[21]={-1,-1,0,1,2,3,4,-1,5,6,7,8,9,10,11,12,-1,13,14,15,-1};

 locdata=(struct IRQData *)threadarg;
 LOOP:
    BHandle=locdata->Handle;
    CAENVME_IRQEnable(BHandle,mask);

    ierr=CAENVME_IRQWait(BHandle,mask,5000); 

    if(locdata->exit==1)goto ENDR;
    if(ierr!=0)goto LOOP;

    // CAENVME_IRQCheck(BHandle,omask);
    // printf(" IRQ mask %02x \n",omask[0]&0xff);

    Data[0]=0x00;Data[1]=0x00;
    CAENVME_IACKCycle(BHandle,IRQLevel,Data,DW);
    printf(" IRQ vector %02x%02x \n",Data[1]&0xff,Data[0]&0xff);
    ERR=0;SYNC=0;FMM=0;SLOT=0;NUM_ERR=0;
    locdata->count++;
    if(Data[1]&0x80){ERR=1;locdata->errs[0]++;}
    if(Data[1]&0x40){SYNC=1;locdata->errs[1]++;}
    if(Data[1]&0x20){FMM=1;locdata->errs[2]++;}
    SLOT=Data[1]&0x1f;
    NUM_ERR=((Data[0]>>4)&0x0f);
    NUM_SYNC=(Data[0]&0x0f); 
    locdata->count_fmm=NUM_ERR;
    printf(" ERR %d SYNC %d FMM %d SLOT %d NUM_ERR %d NUM_SYNC %d\n",ERR,SYNC,FMM,SLOT,NUM_ERR,NUM_SYNC); 
    locdata->last_ddu=SLOT;
    locdata->last_errs[0]=ERR;
    locdata->last_errs[1]=SYNC;
    locdata->last_errs[2]=FMM;
    locdata->last_count_fmm=NUM_ERR;


    // Address=0x001b4000;
    Address=0x00034000|((SLOT<<19)&0x00f80000);
    printf(" Address %08lx \n",Address);
    Data[0]=0xFF;Data[1]=0xFF;
    CAENVME_ReadCycle(BHandle,Address,Data,AM,DW);
    status=((Data[1]<<8)&0x7f00)|(Data[0]&0xff);
    printf(" CSC_Status %02x%02x \n",Data[1]&0xff,Data[0]&0xff);
    tmp=1;
    for(j=0;j<15;j++){
      if(tmp==(status^last_status[irqmap[SLOT-1]])){
             locdata->last_fiber=j+1;
	     locdata->sums[irqmap[SLOT-1]][j]=locdata->sums[irqmap[SLOT-1]][j]+1;
      }
      tmp=tmp*2;
    }
    last_status[irqmap[SLOT-1]]=status;
    goto LOOP;
 ENDR: 
    pthread_exit(NULL);
}

void VMEController::irq_pthread_start()
{
  int t,i,j,err;
  printf(" about to launch pthreads \n");
      t=0;
      irqdata[t].threadid=t;
      irqdata[t].Handle=VMEController::theBHandle;
      irqdata[t].exit=0;
      for(i=0;i<15;i++){
      for(j=0;j<16;j++)irqdata[t].sums[i][j]=0;
      }
      for(j=0;j<3;j++){irqdata[t].errs[j]=0;irqdata[t].last_errs[j]=0;}
      irqdata[t].count=0;
      irqdata[t].count_fmm=0;
      irqdata[t].last_ddu=0;
      irqdata[t].last_fiber=0;
      irqdata[t].last_count_fmm=0;
      err=pthread_create(&threads[t],NULL,(void *(*)(void *))&IRQ_Interrupt_Handler,(void *)&irqdata[t]);
      if(err){printf(" Error opening thread \n");exit(1);}
    
      irq_start=1;

}

int VMEController::irq_tester(int ival)
{ 
int word;
  if(ival==0)word=irqdata[0].last_ddu;
  if(ival==1)word=irqdata[0].last_errs[0];
  if(ival==2)word=irqdata[0].last_errs[1];
  if(ival==3)word=irqdata[0].last_errs[2];
  if(ival==4)word=irqdata[0].last_count_fmm;
  if(ival==5)word=irqdata[0].last_fiber;
  return word;
}

void VMEController::irq_pthread_info()
{
int irqmap_rev[16]={3,4,5,6,7,9,10,11,12,13,14,15,16,18,19,20};
int i,j;
  printf("VME INTERRUPT INFORMATION \n");
    printf("  Last Interrupt: \n");
    printf("     SLOT %d FIBER %d ERR %d SYNC %d FMM %d  NUM_ERR %d \n",irqdata[0].last_ddu,irqdata[0].last_fiber,irqdata[0].last_errs[0],irqdata[0].last_errs[1],irqdata[0].last_errs[2],irqdata[0].last_count_fmm);  
    printf("  All Interrupts: \n");
    for(i=0;i<15;i++){
      for(j=0;j<16;j++){
	if(irqdata[0].sums[i][j]!=0)printf(" SLOT %d FIBER %d \n",irqmap_rev[i],j); 
      }
    }
}

void VMEController::irq_pthread_end()
{
unsigned long mask=0x00000001;

  CAENVME_IRQDisable(theBHandle,mask);
  if(irq_start==1){
      irqdata[0].exit=1;
      pthread_exit(NULL);
  }
}




int VMEController::udelay(long int itim)
{
  int i,j;
  for(j=0;j<itim;j++){
      for(i=0;i<200;i++);
  }
  return 0; 
}


VMEModule* VMEController::getTheCurrentModule(){
 return theCurrentModule;
}


