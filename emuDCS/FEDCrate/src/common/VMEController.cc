//----------------------------------------------------------------------
// $Id: VMEController.cc,v 3.5 2007/04/27 19:29:44 gilmore Exp $
// $Log: VMEController.cc,v $
// Revision 3.5  2007/04/27 19:29:44  gilmore
// Improved multiple-FED Crate functions, added DDU firmware broadcast and management features
//
// Revision 3.3.2.1  2007/03/19 15:03:23  gilmore
// fixes for VMEser/VMEpara expert functions
//
// Revision 3.3  2006/10/04 03:00:25  gilmore
// Removed some debug lines.
//
// Revision 3.1  2006/08/04 20:28:27  gilmore
// Added Logserver feature to EmuFEDVME Interrupt handler.
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
#include <log4cplus/logger.h>
#include <xdaq/Application.h>


#define DELAY2 0.016
#define DELAY3 16.384


#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif

//
// the following variables must be kept global to assure
// no conflict when running EmuFRunControlHyperDAQ and
// EmuFCrateHyperDAQ simultaneously
//

struct IRQData irqdata[4];
pthread_t threads[4];
int irq_start[4]={0,0,0,0};

extern int delay_type;
long OpenBHandle[4][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};




VMEController::VMEController(int Device, int Link): 
  theBHandle(-1), Device_(Device), Link_(Link),theCurrentModule(0),indian(SWAP),vmeirq_start_(1)
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
// printf(" +++++ CAENVME read sent +++++\n");
   err=CAENVME_ReadCycle(theBHandle,Address,data,AM,DW);
   if(err!=0){
     caen_err=err;
     printf(" CAENVME read err %d \n",caen_err);
   //printf(" read: address %08x data %04x \n",Address,*data);
   }
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
   if(err!=0){
     caen_err=err;
     printf(" CAENVME write err %d \n",caen_err);
   }
// JG, temporary!   usleep(1000);
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
// LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Inside controller");
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
   if(delay_type==2)packet_delay=((*data)*DELAY2);
   if(delay_type==3)packet_delay=((*data)*DELAY3);
   // printf(" packet_delay %d %ld \n",*data,packet_delay);
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
unsigned int ERR,SYNC,FMM,SLOT,NUM_ERR,NUM_SYNC;
unsigned int status;
char buf[300];
//  ,buf0[50],buf1[50],buf2[50],buf3[50],buf4[50];
 locdata=(struct IRQData *)threadarg;
 int crate=locdata->threadid;
 // log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFCrateHyperDAQ");
 log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFEDVME");
 // log4cplus::Logger logger = log4cplus::Logger::getApplicationLogger();
 //  LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Interrupt detected");

 LOOP:
    BHandle=locdata->Handle;
    CAENVME_IRQEnable(BHandle,mask);
    int crate_number = locdata->crate_number;

    ierr=CAENVME_IRQWait(BHandle,mask,5000); 
    // JRG, should show every ~5sec:   printf("IRQ_Int Looped again... \n");
    // printf("CAENVME_IRQWait: %d %d \n",locdata->exit,ierr);
    if(locdata->exit==1)goto ENDR;
    if(ierr!=0)goto LOOP;

    // CAENVME_IRQCheck(BHandle,omask);
    // printf(" IRQ mask %02x \n",omask[0]&0xff);

    Data[0]=0x00;Data[1]=0x00;
    CAENVME_IACKCycle(BHandle,IRQLevel,Data,DW);
    //    printf(" IRQ vector %02x%02x \n",Data[1]&0xff,Data[0]&0xff);
    ERR=0;SYNC=0;FMM=0;SLOT=0;NUM_ERR=0;
    locdata->count++;
    if(Data[1]&0x80)ERR=1;  // DDU Board Error set
    if(Data[1]&0x40)SYNC=1; // DDU Board SyncErr set
    if(Data[1]&0x20)FMM=1;  // FMM Error or SyncErr set
    SLOT=Data[1]&0x1f;
    NUM_ERR=((Data[0]>>4)&0x0f); // # CSCs on DDU with Error set
    NUM_SYNC=(Data[0]&0x0f);     // # CSCs on DDU with SyncErr set
// JRG, need Crate number here:
    sprintf(buf," ** EmuFEDVME: Interrupt detected for Crate/Slot %d/%d, 0x%02x%02x ** ",crate_number,SLOT,Data[1]&0xff,Data[0]&0xff);
    printf("%s\n",buf);
    //    printf(" ** EmuFEDVME: Interrupt detected for Crate/Slot %d/%d, 0x%02x%02x ** \n",crate,SLOT,Data[1]&0xff,Data[0]&0xff);
    // orig.  LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Interrupt detected");
    //    LOG4CPLUS_INFO(logger, " EmuFEDVME: Interrupt detected");
    LOG4CPLUS_ERROR(logger,buf);
    if(Data[1]&0xe0){
      printf(" ** EmuFEDVME %d/%d: ",crate,SLOT);
      sprintf(buf," ** EmuFEDVME %d/%d: ",crate,SLOT);
      if(ERR){
	printf("  DDU has Error");
	strcat(buf,"  DDU has Error");
      }
      if(SYNC){
	printf("  DDU Lost Sync");
	strcat(buf,"  DDU Lost Sync");
      }
      if(ERR){
	printf("  FMM Reset requested");
	strcat(buf,"  FMM Reset requested");
      }
      printf("\n");
      //      LOG4CPLUS_ERROR(logger,buf << endl);
      LOG4CPLUS_ERROR(logger,buf);
    }
    locdata->count_fmm=NUM_ERR;
    //    printf(" ERR %d SYNC %d FMM %d SLOT %d NUM_ERR %d NUM_SYNC %d\n",ERR,SYNC,FMM,SLOT,NUM_ERR,NUM_SYNC); 
    locdata->last_ddu=SLOT;
    locdata->last_errs[0]=ERR;
    locdata->last_errs[1]=SYNC;
    locdata->last_errs[2]=FMM;
    locdata->last_count_fmm=NUM_ERR;


    // Address=0x001b4000;
    Address=0x00034000|((SLOT<<19)&0x00f80000);
    Data[0]=0xFF;
    Data[1]=0xFF;
// JRG, get CSC_Stat: 16-bit word for which CSCs have a problem
    CAENVME_ReadCycle(BHandle,Address,Data,AM,DW);
    status=((Data[1]<<8)&0xff00)|(Data[0]&0xff);
    printf(" ** EmuFEDVME %d/%d:   %d CSCs w/Error, %d w/SyncErr. CSCtroubleFlags 0x%02x%02x\n",crate,SLOT,NUM_ERR,NUM_SYNC,Data[1],Data[0]);
    sprintf(buf," ** EmuFEDVME %d/%d:   %d CSCs w/Error, %d w/SyncErr. CSCtroubleFlags 0x%02x%02x\n",crate,SLOT,NUM_ERR,NUM_SYNC,Data[1],Data[0]);
    //    printf(" CSC_Status %02x%02x \n",Data[1]&0xff,Data[0]&0xff);
    LOG4CPLUS_INFO(logger,buf << endl);
    locdata->last_status=status;
    goto LOOP;
 ENDR: 
    printf(" IRQ_Int call pthread_exit \n");
    pthread_exit(NULL);
}


void VMEController::irq_pthread_start(int crate)
{
  int t,j,err;
  printf(" about to launch pthreads \n");
      t=crate;
      if(irq_start[t]==1)return;
      irqdata[t].threadid=t;
      irqdata[t].Handle=VMEController::theBHandle;
      irqdata[t].exit=0;
      for(j=0;j<3;j++){irqdata[t].last_errs[j]=0;}
      irqdata[t].count=0;
      irqdata[t].count_fmm=0;
      irqdata[t].last_ddu=0;
      irqdata[t].last_status=0;
      irqdata[t].last_count_fmm=0;
      irqdata[t].crate_number=crate;
      err=pthread_create(&threads[t],NULL,(void *(*)(void *))&IRQ_Interrupt_Handler,(void *)&irqdata[t]);
      if(err){printf(" Error opening thread \n");exit(1);}
    
      irq_start[t]=1;

}

int VMEController::irq_tester(int crate,int ival)
{ 
int word;
  if(ival==0)word=irqdata[crate].count;
  if(ival==1)word=irqdata[crate].threadid;
  if(ival==2)word=irqdata[crate].last_ddu;
  if(ival==3)word=irqdata[crate].last_status;
  if(ival==4)word=irqdata[crate].last_errs[0];
  if(ival==5)word=irqdata[crate].last_errs[1];
  if(ival==6)word=irqdata[crate].last_errs[2];
  if(ival==7)word=irqdata[crate].last_count_fmm;
  // if(ival==0)printf(" irq_tester %d %d %d \n",crate,ival,word);
  return word;
}

void VMEController::irq_pthread_info(int crate)
{;
  printf("VME INTERRUPT INFORMATION \n");
    printf("  Last Interrupt: \n");
    printf("  CRATE %d  SLOT %d FIBER %04x ERR %d SYNC %d FMM %d  NUM_ERR %d \n",crate,irqdata[crate].last_ddu,irqdata[crate].last_status,irqdata[crate].last_errs[crate],irqdata[crate].last_errs[1],irqdata[crate].last_errs[2],irqdata[crate].last_count_fmm);  

}


void VMEController::irq_pthread_end(int crate)
{
unsigned long mask=0x00000001;
  CAENVME_IRQDisable(theBHandle,mask);
  if(irq_start[crate]==1){
    printf(" about to end pthreads \n");
    irqdata[crate].exit=1;
    irq_start[crate]=0;
    //   pthread_exit(NULL);
    pthread_cancel(threads[crate]);
    printf("Interrupt handler sleeping for 6 seconds.... \n");
    usleep(6000000);
    //    sleep(6);
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


