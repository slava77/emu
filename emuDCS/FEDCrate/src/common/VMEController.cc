//----------------------------------------------------------------------
// $Id: VMEController.cc,v 3.9 2008/08/13 14:20:42 paste Exp $
// $Log: VMEController.cc,v $
// Revision 3.9  2008/08/13 14:20:42  paste
// Massive update removing "using namespace" code and cleaning out stale header files as preparation for RPMs.
//
// Revision 3.8  2008/06/10 13:52:12  gilmore
// improved FED Crate HyperDAQ operability
//
// Revision 3.6  2007/07/23 05:03:30  gilmore
// major structural chages to improve multi-crate functionality
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


#define DELAY2 0.016
#define DELAY3 16.384


#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl; 
#define PRINTSTRING(x) std::cout << #x << std::endl; 
#endif


extern unsigned long vmeadd;
//
// the following variables must be kept global to assure
// no conflict when running EmuFRunControlHyperDAQ and
// EmuFCrateHyperDAQ simultaneously
//

extern int delay_type;
long OpenBHandle[4][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};




VMEController::VMEController(int Device, int Link): 
	theBHandle(-1),
	Device_(Device),
	Link_(Link),
	theCurrentModule(0),
	indian(SWAP)
{
	CVBoardTypes VMEBoard;
	short Lin;
	short Dev;
	VMEBoard=cvV2718;
	int32_t BHandle; 
	std::cout << "constructing VMEController " << std::endl;
	Dev=Device;
	Lin=Link;

	if(OpenBHandle[Dev][Lin]==-1){
		int result = CAENVME_Init(VMEBoard,Device,Link,&BHandle);
		//printf(" result from initializing CAENVME with VMEBoard %08x Device %08x Link %08x BHandle %08x: %08x\n",VMEBoard,Device,Link,BHandle);
		if(result != cvSuccess){
			std::cout << "Error in Opening CAEN Controller " << result << std::endl;
			exit(1);
		}
	}else{
		BHandle=OpenBHandle[Dev][Lin];
	}
	theBHandle=BHandle;
	OpenBHandle[Dev][Lin]=BHandle;
	//printf("--Construction comeplete, address %08x\n",this);
}


VMEController::~VMEController(){
	std::cout << "destructing VMEController .. closing socket " << std::endl;
	CAEN_close();
}
/*
void VMEController::start_thread(long unsigned int runnumber) {
	if (is_thread_started) {
		std::cout << " VMEController: thread alread started, killing previous thread (unsafe)" << std::endl;
		this->kill_thread();
	}
	IRQData data;
	data.crate_number=crateNumber;
	data.Handles.push(theBHandle);
	data.exit=0;
	data.count=0;
	data.count_fmm=0;
	data.last_ddu = 0;
	data.last_errs[0]=0;
	data.last_errs[1]=0;
	data.last_errs[2]=0;
	data.last_count_fmm=0;
	data.last_status=0;
	
	for (int i=0; i<21; i++) {
		data.last_error[i] = 0;
		data.last_fmm_stat[i] = 0;
		data.acc_error[i] = 0;
		data.ddu_count[i] = 0;
		data.previous_problem[i] = 0;
	}
	myThread = new IRQThread(data);
	myThread->start(runnumber);
	is_thread_started = true;
}

void VMEController::end_thread() {
	if (!is_thread_started) {
		std::cout << " VMEController: thread not started, ignoring end request" << std::endl;
		return;
	}
	is_thread_started = false;
	myThread->end();
	delete myThread;
}

void VMEController::kill_thread() {
	if (!is_thread_started) {
		std::cout << " VMEController: thread not started, ignoring kill request" << std::endl;
		return;
	}
	is_thread_started = false;
	myThread->kill();
	delete myThread;
}
*/
void VMEController::setCrate(int number) {
	crateNumber = number;
}

/* void VMEController::start(VMEModule * module) {
	if(theCurrentModule != module) {
		PRINTSTRING(OVAL: start method defined in VMEController.cc is starting )
		end();
		PRINTSTRING(OVAL: starting current module);
		module->start();
		PRINTSTRING(OVAL: current module was started);
		theCurrentModule = module;
	}
} */

void VMEController::start(int slot){
  vmeadd=slot<<19;
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

void VMEController::CAEN_err_reset(void) {
	caen_err = 0;
	return;
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

	//printf(" write: handle %d address %08x data %04x AM %d DW %d \n",theBHandle,Address,*data,AM,DW);
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
//printf("vme_controller with irdwr %d ptr %08x data %04x rcv %08x\n",irdwr,ptr,*data,rcv);
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
   if(delay_type==2)packet_delay= (long int) ((*data)*DELAY2);
   if(delay_type==3)packet_delay= (long int) ((*data)*DELAY3);
   // printf(" packet_delay %d %ld \n",*data,packet_delay);
   udelay(packet_delay);
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


