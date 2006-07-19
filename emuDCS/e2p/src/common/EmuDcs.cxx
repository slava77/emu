

//int fun_temp();



#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>  /* sleep function */
#include <signal.h>  /* Usual interrupt handler stuff */
#include <sys/ioctl.h>
#include <errno.h>


#include <string>
#include <vector>


int CHAMBER_NUMBER;

bool SIMULATION=false;

// NORMAL MODE:                      CRATE_CONFIGURED_NEEDED = false;
// ALL BOARDS PROGRAMMING MODE:      CRATE_CONFIGURED_NEEDED = true;
//bool  CRATE_CONFIGURED_NEEDED = false;
bool  CRATE_CONFIGURED_NEEDED = false;



//========================================



/////#include <iostream>
//#include "PeripheralCrateController.h"
//fg #include "VMEControllerdcs.h"
#include "VMEController.h"
///////////#include "DAQMBdcs.h"
//////////#include "CCBdcs.h"
/// #include "FakeCCB.h"
#include "TMB.h"
///////////#include "ALCTController.h"
//#include "TestBeamCrateController.h"
#include "CrateSetup.h"


#include <DcsDimStructures.h>
#include <EmuDcs.h>
#include <DcsDimService.h>
#include <Rc2Dcs.h>

////#include <DcsDimCommand.h>

//  void (*was)(int), catchFunction(int);
int EmuDcs::svc(){


   ::sleep(10);

   //  printf("=============>>>>>>>>>>>========================================================================\n");
  /*
     Rc2Dcs *rc2dcs= new Rc2Dcs();  // corr++ 
     rc2dcs->turnLV(1);   // corr++
  */
    DimClient::sendCommand("LV_1_COMMAND","all;all|power_on"); 

#ifdef DCS_PRINTING_0 
    printf("LV_1_COMMAND is SENT !!  1\n");
#endif

    DimClient::sendCommand("LV_1_COMMAND","all;all|get_data_local_update");
 
#ifdef DCS_PRINTING_0
    printf("LV_1_COMMAND is SENT !!  2\n");
#endif
  while(1){ // indefinite loop (there are no breaks for it)


    //::usleep(100000);//::usleep(1000000); //::sleep(300);//::sleep(100);

#ifdef TESTMODE
::usleep(100000);
#else
//--------------------------------------------
//::sleep(300);

//::usleep(100000);



 char tmp[40];

#ifdef OSUcc 
  ::usleep(READOUT_CYCLE_DELAY);
#else
  ::sleep(READOUT_CYCLE_DELAY);
#endif

  ch_all_counter++;
  for(int i=0;i<CHAMBER_NUMBER;i++){
    
    ch_counters[i]++;
    //    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%d\n",ch_counters[i]);
    if(ch_counters[i] >= READOUT_COUNTER_NUMBER){
      sprintf(tmp,"%s|get_data_local_update",slots[i].c_str());
      DimClient::sendCommand("LV_1_COMMAND",tmp);
      ch_counters[i]=0;
    }
    else {
      sprintf(tmp,"%s|get_data_local",slots[i].c_str());
      DimClient::sendCommand("LV_1_COMMAND",tmp);
    }
  }




//--------------------------------------------
#endif
  /*
#ifdef DCS_PRINTING_0
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!before\n");
#endif
   DimClient::sendCommand("LV_1_COMMAND","all;all|get_data_local");
#ifdef DCS_PRINTING_0
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");   
#endif
  */

    time_t t;
    time(&t);
    char t_c[40];
    char *t_p=(char *)&t_c;
    t_p=ctime(&t);
    printf("----------------------------------------------------------------------------------------------------------------%s\n",t_p);

  }


}

//========================================================================  
EmuDcs::EmuDcs(string *file_to_load_cfeb, string *file_to_load_vme_chip, string *file_to_load_control_chip,
       string *file_to_load_valct288, string *file_to_load_salct288, 
       string *file_to_load_valct384, string *file_to_load_salct384, 
       string *file_to_load_valct672, string *file_to_load_salct672, 
string *file_to_load_tmb)
  : RepeatNumber(1), OPERATION_ACTIVE(false),  Task(""), 
 file_to_load_cfeb(file_to_load_cfeb),file_to_load_vme_chip(file_to_load_vme_chip), 
  file_to_load_control_chip(file_to_load_control_chip),
  file_to_load_valct288(file_to_load_valct288), file_to_load_salct288(file_to_load_salct288), 
  file_to_load_valct384(file_to_load_valct384), file_to_load_salct384(file_to_load_salct384), 
  file_to_load_valct672(file_to_load_valct672), file_to_load_salct672(file_to_load_salct672), 
  file_to_load_tmb(file_to_load_tmb)
{

EmuDcs_launch();
activate();
// sleep(1);
}

//========================================================================  
EmuDcs::EmuDcs() : RepeatNumber(1), OPERATION_ACTIVE(false), Task(""){

file_to_load_cfeb = new string("/home/fast/data/daqmb_config/feb_prom/fcntl_v9_r1.svf");
file_to_load_vme_chip=new string("/home/fast/data/daqmb_config/mthb_vprom/vme4_v16_r3.svf");
file_to_load_control_chip=new string("/home/fast/data/daqmb_config/mthb_cprom/cntl4_v35_r1.svf");
file_to_load_valct288=new string("/home/fast/data/daqmb_config/alct_vprom/alct288_virtex.svf");
file_to_load_valct384=new string("/home/fast/data/daqmb_config/alct_vprom/alct384_virtex.svf");
file_to_load_valct672=new string("/home/fast/data/daqmb_config/alct_vprom/alct672_virtex.svf");
file_to_load_tmb=new string("/home/fast/data/daqmb_config/tmb_eprom/tmb2001a_101802.svf");

EmuDcs_launch();
activate();
}
//========================================================================  
void EmuDcs::EmuDcs_launch()
  /*
string *file_to_load_cfeb, string *file_to_load_vme_chip, string *file_to_load_control_chip,
       string *file_to_load_valct288, string *file_to_load_salct288, 
       string *file_to_load_valct384, string *file_to_load_salct384, 
       string *file_to_load_valct672, string *file_to_load_salct672, 
string *file_to_load_tmb) 
: RepeatNumber(1), OPERATION_ACTIVE(false), 
 file_to_load_cfeb(file_to_load_cfeb),file_to_load_vme_chip(file_to_load_vme_chip), 
  file_to_load_control_chip(file_to_load_control_chip),
  file_to_load_valct288(file_to_load_valct288), file_to_load_salct288(file_to_load_salct288), 
  file_to_load_valct384(file_to_load_valct384), file_to_load_salct384(file_to_load_salct384), 
  file_to_load_valct672(file_to_load_valct672), file_to_load_salct672(file_to_load_salct672), 
  file_to_load_tmb(file_to_load_tmb)
  */
{

  alct_c=NULL;

  EmuDcs::RUN_MODE=1;

#ifdef DCS_PRINTING_0
  printf("%s \n %s \n %s \n",this->file_to_load_cfeb->c_str(),this->file_to_load_vme_chip->c_str(),this->file_to_load_control_chip->c_str());
#endif

  number_of_cfebs=5; // temporal parameter: should be taken from DAQMB object 
                     // whenever it is declared there

  db();   

  
  string service_name;
  
  ch_all_counter=0;
  
#ifdef OSUcc
  ///  READOUT_CYCLE_DELAY=CHAMBER_NUMBER*100000*2*1000; // temp for slow 
  READOUT_CYCLE_DELAY=CHAMBER_NUMBER*100000*2-CHAMBER_NUMBER/2;
  READOUT_COUNTER_NUMBER=(5000000/READOUT_CYCLE_DELAY)*100;  // 5sec. * 100 = 500 sec.
#else
  READOUT_CYCLE_DELAY=CHAMBER_NUMBER*2*1000;
  ///  READOUT_CYCLE_DELAY=CHAMBER_NUMBER*2-CHAMBER_NUMBER/2;
  READOUT_COUNTER_NUMBER=500/READOUT_CYCLE_DELAY;
#endif




  for(int i=0;i<CHAMBER_NUMBER;i++){

   ch_counters[i]=0;

    memset(&(LV_1_DimBroker_lv[i]),0,sizeof(LV_1_DimBroker));
    getServiceName(i,"LV_1",service_name);


   LV_1_MonitorService[i]= new DcsDimService((char *)service_name.c_str(),"F:5;F:5;F:5;F:5;F:5;F:5;F:5;F:9;I:2;C:80",  
   (void *)&(LV_1_DimBroker_lv[i]), sizeof(LV_1_DimBroker));


    memset(&(TEMPERATURE_1_DimBroker_tm[i]),0,sizeof(TEMPERATURE_1_DimBroker));
    getServiceName(i,"TEMP_1",service_name);
   //  TEMPERATURE_1_MonitorService= new DcsDimService("TEMP_1","F:5;F:5;F:5;F:5;F:5;I:2;C:80",
   TEMPERATURE_1_MonitorService[i]= new DcsDimService((char *)service_name.c_str(),"F:7;I:2;C:80",
   (void *)&(TEMPERATURE_1_DimBroker_tm[i]), sizeof(TEMPERATURE_1_DimBroker));

    memset(&(COMMAND_1_DimBroker_cm[i]),0,sizeof(COMMAND_1_DimBroker));
    getServiceName(i,"CHIP_1",service_name);
   COMMAND_1_MonitorService[i]= new DcsDimService((char *)service_name.c_str(),"I:3;C:80",
   (void *)&(COMMAND_1_DimBroker_cm[i]), sizeof(COMMAND_1_DimBroker));

    memset(&(REFERENCE_1_DimBroker_rf[i]),0,sizeof(REFERENCE_1_DimBroker));
    getServiceName(i,"RF_1",service_name);
   REFERENCE_1_MonitorService[i]= new DcsDimService((char *)service_name.c_str(),"F:9;I:2;C:80",
   (void *)&(REFERENCE_1_DimBroker_rf[i]), sizeof(REFERENCE_1_DimBroker));

  }


  RunControlService= new DimService("DCS_SERVICE","C:80",
  (void *)&RunControlAck, sizeof(RunControlStructure));
  

#ifdef DCS_PRINTING_0
  printf("start=======\n");
#endif

//======= for fast_final setup ==================================
  /*
  CrateSetup *cr_setup_o= new CrateSetup(); 
  VMEController *vme = new VMEController("128.146.39.89", 6030);
  Crate * crate = new Crate(0, vme); 
  DAQMB * daqmb = new DAQMB(0, 13, 5);
  CCB * ccb =  new CCB(0, 11);
  TMB * tmb = new TMB(0, 21);
  */ 

//======  for dynatem2 ========================================================

  /*
  CrateSetup *cr_setup_o= new CrateSetup(); /////////////////
  VMEController * vme = new VMEController("137.138.102.223", 6030);
  //  VMEController * vme = new VMEController("137.138.102.223", 6030);
  Crate * crate = new Crate(0, vme); 
  DAQMB * daqmb = new DAQMB(0, 7, 5);
  CCB * ccb =  new CCB(0, 13);

  TMB * tmb = new TMB(0, 6);
  */
 //==============================================================
  //--------------
  DcsDimCommand_o = new DcsDimCommand(LV_1_MonitorService,TEMPERATURE_1_MonitorService,
					      COMMAND_1_MonitorService,REFERENCE_1_MonitorService, RunControlService, this );

  DimServer::start("Emu-Dcs Dim Server");
  /*

  while(1){
    ::sleep(10);
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!before\n");
   DimClient::sendCommand("LV_1_COMMAND","all;all|get_data_local");
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");   



  }

  

  signal(SIGALRM,EmuDcs::catchFunction);

  */

  /*
  signal(SIGALRM,EmuDcs::catchFunction);
  itimerval timer_interval;
  timer_interval.it_interval.tv_sec =1;
  timer_interval.it_interval.tv_usec =0;
  setitimer(ITIMER_REAL,&timer_interval,0);
  */


  ///// alarm(5); // temporary solution maybe

}
//=======================================================================================

void EmuDcs::dcs_enable(){

  /////// II vector<Crate *> v_crates= crates();
   /////// II    for(int i=0;i< v_crates.size();i++)v_crates[i]->vmeController()->openSocket();

    soft_start();

}
//=======================================================================================
void EmuDcs::dcs_disable(){

 /////// II vector<Crate *> v_crates= crates();
  /////// II  for(int i=0;i< v_crates.size();i++)v_crates[i]->vmeController()->closeSocket();

    soft_stop(); 

}

//=======================================================================================

void EmuDcs::start(){
  // not used

  /*
  EmuDcs::RUN_MODE=1;

 DimServer::start("start DIM again\n");
alarm(10);
  */

}
//=======================================================================================

void EmuDcs::stop(){

  EmuDcs::RUN_MODE=0;

 DimServer::stop();

  delete DcsDimCommand_o;

  delete LV_1_MonitorService;
  delete TEMPERATURE_1_MonitorService;
  delete COMMAND_1_MonitorService;
  delete REFERENCE_1_MonitorService;
  delete RunControlService;

}
//=======================================================================================

void EmuDcs::soft_start(){

  EmuDcs::RUN_MODE=1;
  //  alarm(10);

}
//=======================================================================================

void EmuDcs::soft_stop(){

  EmuDcs::RUN_MODE=0;


}
//=======================================================================================
int EmuDcs::RUN_MODE=1;
//=======================================================================================

void EmuDcs::catchFunction(int){

  static int number_of_hard_resets=0;

  
  number_of_hard_resets++;
#ifdef DCS_PRINTING_0
  printf("catch\n");
#endif
  /*
  printf("+++++++=======CRATE\n");
  if(CRATE_CONFIGURED_NEEDED){
  printf("+++++++=======CRATE1\n");
       DimClient::sendCommand("LV_1_COMMAND","all;all|program_all_via_ccb");
  printf("+++++++=======CRATE2\n");
  }
  */
  /////  if(!(EmuDcs::RUN_MODE))return; 

  if(CRATE_CONFIGURED_NEEDED){
#ifdef DCS_PRINTING_0
  printf("+++++++=======CRATE1\n");
#endif
       DimClient::sendCommand("LV_1_COMMAND","all;all|program_all_via_ccb");
  //       DimClient::sendCommand("LV_1_COMMAND","all;all|program_dmb");
  // DimClient::sendCommand("LV_1_COMMAND","all;all|load_alct384");
  //   DimClient::sendCommand("LV_1_COMMAND","all;all|load_all_cfebs");

#ifdef DCS_PRINTING_0
  printf("+++++++=======CRATE2\n");
#endif

  if(number_of_hard_resets==2)CRATE_CONFIGURED_NEEDED=false;
  }
  else{
#ifdef DCS_PRINTING_0
    printf("number_of_hard_resets=%d\n",number_of_hard_resets);
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!before\n");
#endif
   DimClient::sendCommand("LV_1_COMMAND","all;all|get_data_local");
#ifdef DCS_PRINTING_0
    printf("LV_1_COMMAND is SENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
  }

  
  if(number_of_hard_resets<=2)
  alarm(5);
  else alarm(5);

}


//=======================================================================================

int EmuDcs::safeExit(){
  // signal(SIGALRM,this->catchFunction);
  //fg  DAQMB *daqmb_loc = dynamic_cast<DAQMB *>(((VMEControllerdcs *) daqmb->getTheController())->getTheCurrentModule()); //
  //DAQMB *daqmb_loc = dynamic_cast<DAQMB *>(((VMEController *) daqmb->getTheController())->getTheCurrentModule()); //
  //if(daqmb_loc != 0) daqmb->getTheController()->end();

  return 1;

}

//========================================================================  

int EmuDcs::cfeb_lv(int cfeb_number){

 

  if(SIMULATION){

 float frand_1;
 float frand_2;
 float frand_3;

 float factor1=10;

    if(slot_status[current_set]){
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);

  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v33[cfeb_number-1] = 3.3+ frand_2 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c33[cfeb_number-1] = 0.5+ frand_3;
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v50[cfeb_number-1] = 5.0+ frand_2 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c50[cfeb_number-1] = 0.6+ frand_3;
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v60[cfeb_number-1] = 6.0+ frand_2 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c60[cfeb_number-1] = 0.7+ frand_3;

    }
    else{
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v33[cfeb_number-1] = 0 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v50[cfeb_number-1] = 0 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v60[cfeb_number-1] = 0 ;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c33[cfeb_number-1] = 0;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c50[cfeb_number-1] = 0;
  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c60[cfeb_number-1] = 0;
    }

  }
  else{

  OPERATION_ACTIVE=true;

  float adcval[3];

  /*
  float adcval1;
  float adcval2;
  float adcval3;
  */

  float values[3];
  for(int i=0;i<3;i++)values[i]=0;

  ///////	osu_start(dp->crate_slt);

	for(int j=0;j<RepeatNumber;j++){

	 if(cfeb_number==1){
  adcval[0]=daqmb->lowv_adc(3,3); 
#ifdef DCS_PRINTING_1 
printf("CFEB1 3.3V    %.3f V \n",adcval[0]/1000); 
#endif 
  adcval[1]= daqmb->lowv_adc(3,4); 
#ifdef DCS_PRINTING_1 
printf("CFEB1 5.0V    %.3f V \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(3,5); 
#ifdef DCS_PRINTING_1 
printf("CFEB1 6.0V    %.3f V \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==2){
  adcval[0]=daqmb->lowv_adc(3,6); 
#ifdef DCS_PRINTING_1 
printf("CFEB2 3.3V    %.3f V \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(3,7); 
#ifdef DCS_PRINTING_1 
printf("CFEB2 5.0V    %.3f V \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(4,0); 
#ifdef DCS_PRINTING_1 
printf("CFEB2 6.0V    %.3f V \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==3){
  adcval[0]=daqmb->lowv_adc(4,1); 
#ifdef DCS_PRINTING_1 
printf("CFEB3 3.3V    %.3f V \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(4,2); 
#ifdef DCS_PRINTING_1 
printf("CFEB3 5.0V    %.3f V \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(4,3); 
#ifdef DCS_PRINTING_1 
printf("CFEB3 6.0V    %.3f V \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==4){
  adcval[0]=daqmb->lowv_adc(4,4);
#ifdef DCS_PRINTING_1 
printf("CFEB4 3.3V    %.3f V \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(4,5);
#ifdef DCS_PRINTING_1 
printf("CFEB4 5.0V    %.3f V \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(4,6);
#ifdef DCS_PRINTING_1 
printf("CFEB4 6.0V    %.3f V \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==5){
  adcval[0]=daqmb->lowv_adc(4,7);
#ifdef DCS_PRINTING_1 
printf("CFEB5 3.3V    %.3f V \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(5,0);
#ifdef DCS_PRINTING_1 
printf("CFEB5 5.0V    %.3f V \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(5,1);
#ifdef DCS_PRINTING_1 
printf("CFEB5 6.0V    %.3f V \n",adcval[2]/1000); 
#endif
	 }


	 for(int i1=0;i1<3;i1++){
	   if(adcval[i1]!=0)values[i1]=adcval[i1]/1000;
	   //	   if(adcval2!=0)values[1]=adcval2/1000;
	   //	   if(adcval3!=0)values[2]=adcval3/1000;          
	 }
         int i2=0;
	 for(int i1=0;i1<3;i1++){
	   if(values[i1]==0)break;
           i2++;
	 }
         if(i2==3)break;

	} // j

   ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v33[cfeb_number-1] =values[0];//adcval1/1000;
   ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v50[cfeb_number-1] =values[1];//adcval2/1000;
   ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.v60[cfeb_number-1] =values[2];//adcval3/1000;
	
 //==========================================

  for(int i=0;i<3;i++)values[i]=0;

	for(int j=0;j<RepeatNumber;j++){

	 if(cfeb_number==1){
  adcval[0]=daqmb->lowv_adc(1,0);
#ifdef DCS_PRINTING_1 
printf("CFEB1 OCM 3   %.3f A \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(1,1);
#ifdef DCS_PRINTING_1 
printf("CFEB1 OCM 5   %.3f A \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(1,2);
#ifdef DCS_PRINTING_1 
printf("CFEB1 OCM 6   %.3f A \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==2){
  adcval[0]=daqmb->lowv_adc(1,3);
#ifdef DCS_PRINTING_1 
printf("CFEB2 OCM 3   %.3f A \n",adcval[0]/1000); 
#endif 
  adcval[1]=daqmb->lowv_adc(1,4);
#ifdef DCS_PRINTING_1 
printf("CFEB2 OCM 5   %.3f A \n",adcval[1]/1000); 
#endif 
  adcval[2]=daqmb->lowv_adc(1,5);
#ifdef DCS_PRINTING_1 
printf("CFEB2 OCM 6   %.3f A \n",adcval[2]/1000); 
#endif 
	 }
	 else if(cfeb_number==3){
  adcval[0]=daqmb->lowv_adc(1,6);
#ifdef DCS_PRINTING_1 
printf("CFEB3 OCM 3   %.3f A \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(1,7);
#ifdef DCS_PRINTING_1 
printf("CFEB3 OCM 5   %.3f A \n",adcval[1]/1000); 
#endif
  adcval[2]=daqmb->lowv_adc(2,0);
#ifdef DCS_PRINTING_1 
printf("CFEB3 OCM 6   %.3f A \n",adcval[2]/1000); 
#endif 
	 }
	 else if(cfeb_number==4){
  adcval[0]=daqmb->lowv_adc(2,1);
#ifdef DCS_PRINTING_1 
printf("CFEB4 OCM 3   %.3f A \n",adcval[0]/1000); 
#endif 
  adcval[1]=daqmb->lowv_adc(2,2);
#ifdef DCS_PRINTING_1 
printf("CFEB4 OCM 5   %.3f A \n",adcval[1]/1000); 
#endif 
  adcval[2]=daqmb->lowv_adc(2,3);
#ifdef DCS_PRINTING_1 
printf("CFEB4 OCM 6   %.3f A \n",adcval[2]/1000); 
#endif
	 }
	 else if(cfeb_number==5){
  adcval[0]=daqmb->lowv_adc(2,4);
#ifdef DCS_PRINTING_1 
printf("CFEB5 OCM 3   %.3f A \n",adcval[0]/1000); 
#endif
  adcval[1]=daqmb->lowv_adc(2,5);
#ifdef DCS_PRINTING_1 
printf("CFEB5 OCM 5   %.3f A \n",adcval[1]/1000); 
#endif 
  adcval[2]=daqmb->lowv_adc(2,6);
#ifdef DCS_PRINTING_1 
printf("CFEB5 OCM 6   %.3f A \n",adcval[2]/1000); 
#endif
	 }

	 for(int i1=0;i1<3;i1++){
	   if(adcval[i1]!=0)values[i1]=adcval[i1]/1000;
	   //	   if(adcval2!=0)values[1]=adcval2/1000;
	   //	   if(adcval3!=0)values[2]=adcval3/1000;          
	 }
         int i2=0;
	 for(int i1=0;i1<3;i1++){
	   if(values[i1]==0)break;
           i2++;
	 }
         if(i2==3)break;

	} // j



	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c33[cfeb_number-1] =values[0];//adcval1/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c50[cfeb_number-1] =values[1];//adcval2/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.c60[cfeb_number-1] =values[2];//adcval3/1000;
	
	  /////////	 osu_end();


	 OPERATION_ACTIVE=false;

  } // else if(SIMULATION)
	 return 1;

}

//===============================================

int EmuDcs::alct_lv(){


  //  cout << "VMEController opened socket ================================== " << daqmb->theController->theSocket << endl;
  //  printf("test1=%d\n",daqmb->theController->test1);


  // printf(" simulation=%d \n",SIMULATION);

  if(SIMULATION){

    //    printf("daqmb->test=%d ***************************************************************\n",daqmb->test);

 float frand_1;
 float frand_2;
 float frand_3;

 float factor1=10;

    if(slot_status[current_set]){
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v18 =1.8+ frand_2;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c18 =0.3+frand_3;
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v33 =3.3+ frand_2;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c33 =0.5+frand_3;
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v55 =5.5+ frand_2;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c55 =0.6+frand_3;
 frand_1 = 1+(int) (20.0*rand()/(RAND_MAX+1.0)); //(rand() % 10);
 frand_2 = 1./(factor1+frand_1);
 frand_3 = 1./(20+frand_1);
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v56 =5.6+ frand_2;	
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c56 =0.6+frand_3;

    }
    else{
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v18 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v33 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v55 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v56 =0;	
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c18 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c33 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c55 =0;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c56 =0;
    }

  }
  else{

    //    printf("daqmb->test=%d *************************************************************** real \n",daqmb->test);
  OPERATION_ACTIVE=true;

 float adcval[4];
 /*
  float adcval1;
  float adcval2;
  float adcval3;
  float adcval4;
 */


  float values[4];
  for(int i=0;i<4;i++)values[i]=0;

	for(int j=0;j<RepeatNumber;j++){

	  ///////	osu_start(dp->crate_slt);

  adcval[0]=daqmb->lowv_adc(5,3); 
#ifdef DCS_PRINTING_1
printf("ALCT 1.8V     %.3f V \n",adcval[0]/1000);
#endif
  adcval[1]=daqmb->lowv_adc(5,2); 
#ifdef DCS_PRINTING_1
printf("ALCT 3.3V     %.3f V \n",adcval[1]/1000);
#endif
  adcval[2]=daqmb->lowv_adc(5,5); 
#ifdef DCS_PRINTING_1
printf("ALCT 5.5V A   %.3f V \n",adcval[2]/1000);
#endif
  adcval[3]=daqmb->lowv_adc(5,4); 
#ifdef DCS_PRINTING_1
printf("ALCT 5.5V B   %.3f V \n",adcval[3]/1000);
#endif

	 for(int i=0;i<4;i++){
	   if(adcval[i]!=0)values[i]=adcval[i]/1000;
	   //	   if(adcval2!=0)values[1]=adcval2/1000;
	   //	   if(adcval3!=0)values[2]=adcval3/1000;  
	   //	   if(adcval3!=0)values[3]=adcval4/1000;          
	 }
         int i2=0;
	 for(int i=0;i<4;i++){
	   if(values[i]==0)break;
           i2++;
	 }
         if(i2==4)break;

	} //j 
	

	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v18 =values[0];//adcval1/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v33 =values[1];//adcval2/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v55 =values[2];//adcval3/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.v56 =values[3];//adcval4/1000;	
     

//-----------------------------------------
  for(int i=0;i<4;i++)values[i]=0;

	for(int j=0;j<RepeatNumber;j++){

  adcval[0]=daqmb->lowv_adc(3,0); 
#ifdef DCS_PRINTING_1
printf("ALCT OCM 1.8  %.3f A \n",adcval[0]/1000);
#endif

  adcval[1]=daqmb->lowv_adc(2,7); 
#ifdef DCS_PRINTING_1
printf("ALCT OCM 3.3  %.3f A \n",adcval[1]/1000);
#endif

  adcval[2]=daqmb->lowv_adc(3,2); 
#ifdef DCS_PRINTING_1
printf("ALCT OCM 5A   %.3f A \n",adcval[2]/1000);
#endif

  adcval[3]=daqmb->lowv_adc(3,1); 
#ifdef DCS_PRINTING_1
printf("ALCT OCM 5B   %.3f A \n",adcval[3]/1000);
#endif

	 for(int i=0;i<4;i++){
	   if(adcval[i]!=0)values[i]=adcval[i]/1000;
	   //	   if(adcval2!=0)values[1]=adcval2/1000;
	   //	   if(adcval3!=0)values[2]=adcval3/1000;  
	   //	   if(adcval3!=0)values[3]=adcval4/1000;          
	 }
         int i2=0;
	 for(int i=0;i<4;i++){
	   if(values[i]==0)break;
           i2++;
	 }
         if(i2==4)break;

	} // j


	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c18 =values[0];//adcval1/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c33 =values[1];//adcval2/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c55 =values[2];//adcval3/1000;
	  ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.c56 =values[3];//adcval4/1000;	
	  /////////	 osu_end();

	 ///////	 memcpy((ALCT_LV *) &(((LV_1_DimBroker *)Command::LV_1_DimBroker_o->value)->data.Alct_o),(ALCT_LV *)Command::alct->value, sizeof(ALCT_LV) );

	 OPERATION_ACTIVE=false;

  } // else if(SIMULATION)

 return 1;

}

//=====================================================================

int EmuDcs::lv_on_wrap(bool IS_SIMULATION_LOCAL, int channels){

  OPERATION_ACTIVE=true;

#ifdef DCS_PRINTING_0
  printf("lv_on_wrap()\n");
#endif
 
  /////////  osu_start(dp->crate_slt);
  if(!IS_SIMULATION_LOCAL){
   for (int i=0;i< 10;i++){
     /*if(!lowv_status(IS_SIMULATION_LOCAL))*/daqmb->lowv_onoff(channels);//  0x3f commented out because of wrong work LVMB
     if(lowv_status(IS_SIMULATION_LOCAL)) break;
   }
  }
  else{
#ifdef DCS_PRINTING_0
      printf("on  SIMULATION\n");
#endif
    slot_status[current_set]=1;
  }

  ////////  osu_end();

  for(int i=0;i<number_of_cfebs ;i++){
    if(channels & (1 << i))
   ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.status[i]=1;
    else ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.status[i]=0;
  }
    if(channels & (1 << 5))
           ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.status=1;
    else
            ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.status=0;

	 OPERATION_ACTIVE=false;

  return 1;

}

//=====================================================================

int EmuDcs::lv_off_wrap(bool IS_SIMULATION_LOCAL){

  OPERATION_ACTIVE=true;

#ifdef DCS_PRINTING_0
  printf("lv_off_wrap()\n");
#endif

  if(!IS_SIMULATION_LOCAL){
   for (int i=0;i< 10;i++){
     /*if(lowv_status(IS_SIMULATION_LOCAL))*/daqmb->lowv_onoff(0x0);// commented out because of wrong work LVMB
   if(!lowv_status(IS_SIMULATION_LOCAL))break;
   }
  }
  else{
      printf("off  SIMULATION\n");
   slot_status[current_set]=0;
  }
#ifdef DCS_PRINTING_0
      printf("off  \n");
#endif
            for(int i=0;i<number_of_cfebs ;i++)
           ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Cfeb_o.status[i]=0;
           ((LV_1_DimBroker *)LV_1_MonitorService[db_index]->value)->data.Alct_o.status=0; 


	 OPERATION_ACTIVE=false;
  return 1;

}

//=====================================================================

int EmuDcs::lowv_status(bool IS_SIMULATION_LOCAL){

  if(!IS_SIMULATION_LOCAL){
   if((daqmb->lowv_rdpwrreg()&0x00ff) ==0xc0)return 0;
   else return (daqmb->lowv_rdpwrreg() & 0x3f);
  }
  else{
   return slot_status[current_set];
  }
}

//=====================================================================

/*
bool EmuDcs::slotsLoading(){
  //(string &ip_address, int ccb_slot,int dmb_slot,int tmb_slot,int mpc_slot){



  vector<Crate *> v_crates=crates();
  //  int size_crates=v_crates.size();
  vector<DAQMB *> v_daqmbs;

  bool crate_ok = false;
  //////  bool ccb_ok   = false;
  bool dmb_ok   = false;

  bool tmb_ok   = false; // currently not used
  ///////  bool mpc_ok   = false; // currently not used

  for(int i=0;i< v_crates.size(); i++){
     
    if(ip_address == v_crates[i]->vmeController()->ipAddress() ){
      crate_ok = true;
    }
    else {
      continue;
    }

   v_daqmbs=daqmbs(v_crates[i]);
   for(int j=0;j<v_daqmbs.size() ;j++){
     if(dmb_slot == v_daqmbs[j]->slot()){
       controlDAQMB(v_daqmbs[j]);
       dmb_ok=true;
     }
   } // j loop

   ////   if(ccb_slot == v_crates[i]->ccb()->slot()){
     controlCCB(v_crates[i]->ccb());
   /////  ccb_ok = true; 
   ////   }

  } // i loop
   
  if(!crate_ok || !dmb_ok){

    cout << "this command is not for me" << endl;
 
    return false;

  }
  else {

    // for simulation use:

    bool found=false;
    for(int i=0;i< d360_ipslot.size();i++ ){
      if(d360_ipslot[i] == ipslot){
         current_set=i;
         found=true;  
      }
        
    }
    
    if(!found){
     d360_ipslot.push_back(ipslot);
     slot_status.push_back(0);
     current_set = d360_ipslot.size()-1;
    }
     return true;


  } // else
}

//=====================================================================

int EmuDcs::commandParse(string &command){

  
     int pos=command.find("|",0);
    //-------------------------------------------------------------------------------------------------
     ipslot = command.substr(0,pos);

    int pos2= ipslot.find(";",0);
    int pos1= ipslot.find(";",pos2+1);
    if(pos1 == string::npos){
     dmb_slot=atoi((ipslot.substr(pos2+1, ipslot.size() - pos2 -1)).c_str());
    }
    else{
     dmb_slot=atoi((ipslot.substr(pos2+1, pos1-pos2-1)).c_str());
     tmb_slot=atoi((ipslot.substr(pos1+1, ipslot.size() - pos1 -1)).c_str());
    }
     ip_address=ipslot.substr(0, pos2);


    operation_command=command.substr(pos+1,command.size()-pos-1);

    ////    printf("%d %s %s\n",dmb_slot,ip_address.c_str(),operation_command.c_str());

     return 1;

}

*/

//=====================================================================
int EmuDcs::resetAllBackplaneViaCCB(){

  //  ccb->reset_bckpln();
   ccb->soft_reset_all_csr2();
  ////  03/31/2005 ((CCBdcs *)ccb)-> soft_reset_all_csr2();
  return 1;
 
}
//=====================================================================
int EmuDcs::programAllBackplaneViaCCB(){

  //  ccb->prgall_bckpln();

  ///////////  03/31/2005 ((CCBdcs *)ccb)->hard_reset_all_csr2();
  return 1;

}
//=====================================================================
int EmuDcs::hardReset(){

  ccb->hardReset(); // version used in Crate configuration by daq people

  return 1;

}

//=====================================================================
int EmuDcs::programAll2004(){

  //  ccb->prgall_bckpln();

  ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
  ccb->hard_reset_all();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);

  return 1;

}
//=====================================================================
int EmuDcs::resetAllBackplaneViaDMB(){

  daqmb->calctrl_global();
  return 1;

}
//=====================================================================
int EmuDcs::readAllTemperatures(){


  if(!SIMULATION){
  //   printf("Reading all temperatures\n");
  //      printf("Reading Mother board temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_daq  = (daqmb->readthermx(0)-32.)*5./9.;
  //     printf("Reading FEB1 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb1 = (daqmb->readthermx(1)-32.)*5./9.;
  //    printf("Reading FEB2 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb2 = (daqmb->readthermx(2)-32.)*5./9.;
  //   printf("Reading FEB3 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb3 = (daqmb->readthermx(3)-32.)*5./9.;
  //    printf("Reading FEB4 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb4 = (daqmb->readthermx(4)-32.)*5./9.;
  //      printf("Reading FEB5 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb5 = (daqmb->readthermx(5)-32.)*5./9.;
  //         printf("Reading Virtex temperature\n");
  //         daqmb->readthermx(7);


	float readtemp;
#ifdef DCS_PRINTING_1
	//        printf("alct_temperature>>\n");
#endif
        int tmb_slot=tmb->slot();
#ifdef DCS_PRINTING_1
	//        printf("alct_temperature>>\n");
#endif
	alct_c->alct_read_temp(&tmb_slot, &readtemp);
#ifdef DCS_PRINTING_1
	//        printf("alct_temperature>>\n");
#endif
        ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_alct = readtemp;
#ifdef DCS_PRINTING_1
        printf("alct_temperature = %f\n",readtemp);
#endif

  }
  else{

         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_daq  = 20.;
  //     printf("Reading FEB1 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb1 = 21.;
  //    printf("Reading FEB2 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb2 = 22.;
  //   printf("Reading FEB3 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb3 = 23.;
  //    printf("Reading FEB4 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb4 = 24.;
  //      printf("Reading FEB5 temperature\n");
         ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_feb5 = 25.;
  //         printf("Reading Virtex temperature\n");
  //         daqmb->readthermx(7);
        ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_MonitorService[db_index]->value)->t_alct = 26.;



  }

  return 1;

}
//=====================================================================
int EmuDcs::lvSet(){

  daqmb->lowv_onoff(lv_mask_to_set);
  return 1;

}
//=====================================================================
int EmuDcs::programAllCFEBs(){

  /* doe snot work even in daqmb: 
         daqmb->cfeb_vtx_prom(F1PROM);
         daqmb->cfeb_vtx_prom(F2PROM);
         daqmb->cfeb_vtx_prom(F3PROM);
         daqmb->cfeb_vtx_prom(F4PROM);
         daqmb->cfeb_vtx_prom(F5PROM);
  */

  /////////  03/31/2005 ((CCBdcs *)ccb)->hard_reset_dmb();
 ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
   ccb->hard_reset_dmb();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);
#ifdef DCS_PRINTING_0
	printf("HARD RESET DMB DONE\n");
#endif

  return 1;

}
//=====================================================================
int EmuDcs::loadAllCFEBs(){


DEVTYPE devnum;

// check the numbering (from 1? ) !!!!!!!!!!!
 for(int cfeb=0; cfeb < daqmb->cfebs().size(); cfeb++ ){
  
         if(cfeb==0)
         devnum=F1PROM;
         else if(cfeb==1)
         devnum=F2PROM;
         else if(cfeb==2)
         devnum=F3PROM;
         else if(cfeb==3)
         devnum=F4PROM;
         else if(cfeb==4)
         devnum=F5PROM;
  

	unsigned long ibrd=daqmb->febpromuser((daqmb->cfebs())[cfeb]);
        printf(" download svf file::: %s\n",(char *)file_to_load_cfeb->c_str());
        cbrdnum[0]=ibrd        & 0xff;
        cbrdnum[1]=(ibrd >> 8) & 0xff;
        cbrdnum[2]=(ibrd >> 16)& 0xff;
        cbrdnum[3]=(ibrd >> 24)& 0xff;

        if(devnum==F1PROM)printf("%dl\n",cbrdnum);

	// ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)
        daqmb->epromload(devnum,(char *)file_to_load_cfeb->c_str(),1,cbrdnum);  ///// !!!!!!! cbrdnum must be declared in somewhere
        ///if(cfeb == 0)break;
        printf(" file has been downloaded \n");
 }
  return 1;

}
//=====================================================================
int EmuDcs::loadOneCFEB(int cfeb){


DEVTYPE devnum;

  
         if(cfeb==0)
         devnum=F1PROM;
         else if(cfeb==1)
         devnum=F2PROM;
         else if(cfeb==2)
         devnum=F3PROM;
         else if(cfeb==3)
         devnum=F4PROM;
         else if(cfeb==4)
         devnum=F5PROM;
  
	unsigned long ibrd=daqmb->febpromuser((daqmb->cfebs())[cfeb]);
        printf(" download svf file::: %s\n",(char *)file_to_load_cfeb->c_str());
        cbrdnum[0]=ibrd        & 0xff;
        cbrdnum[1]=(ibrd >> 8) & 0xff;
        cbrdnum[2]=(ibrd >> 16)& 0xff;
        cbrdnum[3]=(ibrd >> 24)& 0xff;

        strcpy(cbrdnum,"abcd");
        if(devnum==F1PROM)printf("%s\n",cbrdnum);

	// ibrd=(rcvbuf[0]&0xff)|((rcvbuf[1]&0xff)<<8)|((rcvbuf[2]&0xff)<<16)|((rcvbuf[3]&0xff)<<24)
        daqmb->epromload(devnum,(char *)file_to_load_cfeb->c_str(),1,cbrdnum);  ///// !!!!!!! cbrdnum must be declared in somewhere
        printf(" file has been downloaded \n");
 
  return 1;

}
//=====================================================================
int EmuDcs::programALCT(){

  ////////// 03/31/2005  ((CCBdcs *)ccb)->hard_reset_alct();
 ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
   ccb->hard_reset_alct();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);
#ifdef DCS_PRINTING_0
	printf("HARD RESET ALCT DONE\n");
#endif

  return 1;

}
//=====================================================================
int EmuDcs::programDAQMB(){

  ///////// 03/31/2005 ((CCBdcs *)ccb)->hard_reset_dmb();

 ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
   ccb->hard_reset_dmb();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);
#ifdef DCS_PRINTING_0
	printf("HARD RESET DMB DONE\n");
#endif

  return 1;

}
//=====================================================================
int EmuDcs::programTMB(){

  //////// 03/31/2005  ((CCBdcs *)ccb)->hard_reset_tmb();

 ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
    ccb->hard_reset_tmb();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);
#ifdef DCS_PRINTING_0
	printf("HARD RESET TMB DONE\n");
#endif


  return 1;

}
//=====================================================================
int EmuDcs::programMPC(){

  ////////// 03/31/2005  ((CCBdcs *)ccb)->hard_reset_mpc();

 ccb->setCCBMode(CCB::VMEFPGA);
  ::usleep(100000);
  //ccb->setCCBMode(CCB::DLOG);
   ccb->hard_reset_mpc();
  ::sleep(1);
   ccb->setCCBMode(CCB::DLOG);
#ifdef DCS_PRINTING_0
	printf("HARD RESET MPC DONE\n");
#endif

  return 1;

}
//=====================================================================
int EmuDcs::loadALCT(string &command){
  /*
     ALCTController::CHAMBER ch_type = ALCTController::ME22;
     if(alct_c == NULL)  alct_c = new ALCTController(tmb,ch_type);
     else {delete alct_c; alct_c = new ALCTController(tmb,ch_type);}
  */

  	int db = 0;
  	int jchan = 3; // virtext ALCT chip
        string *file_to_load_valct;
	if(command=="load_alct288")file_to_load_valct=file_to_load_valct288;
	else if(command=="load_alct384")file_to_load_valct=file_to_load_valct384;
	else if(command=="load_alct672")file_to_load_valct=file_to_load_valct672;

 alct_c->SVFLoad(&jchan, ( char *)(file_to_load_valct)->c_str(), 1 );
 
  return 1;

}
//=====================================================================
int EmuDcs::loadTMB(){

  	int db = 0;
  	int jchan = 5; // virtext ALCT chip
 alct_c->SVFLoad(&jchan, ( char *)(file_to_load_tmb)->c_str(), 1 );
 
  return 1;

}
//=====================================================================
int EmuDcs::programDAQMB_VMEChip(){

 
 printf(" programming mthrbrd VME VTX is not implimented: use program_all via backplane signal\n");

  return 1;

}
//=====================================================================
int EmuDcs::loadDAQMB_VMEChip(){


   DEVTYPE devnum=VPROM;
   ////    strcpy(design,"DAQMBV");

    printf(" download svf file n"); 
    daqmb->epromload(devnum,(char *)file_to_load_vme_chip->c_str(),1,cbrdnum);
    printf(" file has been downloaded \n");

  return 1;

}
//=====================================================================
int EmuDcs::loadDAQMB_VMEChipEmergency(){

  /////////////  03/31/2005((DAQMBdcs *)daqmb)->devdoReset();


  DEVTYPE devnum;

         devnum=RESET;
	 ////    strcpy(design,"DAQMBV");

    printf(" download svf file n"); 
    daqmb->epromload(devnum,(char *)file_to_load_vme_chip->c_str(),1,cbrdnum);
    printf(" file has been downloaded \n");


  return 1;


}
//=====================================================================

int EmuDcs::programDAQMB_ControlChip(){

 printf(" programming mthrbrd CTRL VTX is not implimented: use program_all via backplane signal\n");
  return 1;

}
//=====================================================================
int EmuDcs::loadDAQMB_ControlChip(){

  // /home/fast/data/daqmb_config/mthb_cprom/cntl4_v35_r1.svf
   DEVTYPE devnum=MPROM;
   //         strcpy(design,"DAQMBC");


    printf(" download svf file: %s\n",file_to_load_control_chip->c_str()); 
    daqmb->epromload(devnum,(char *)file_to_load_control_chip->c_str(),1,cbrdnum);
    printf(" file has been downloaded \n");


  return 1;

}

//=====================================================================
int EmuDcs::resetDAQMB_FIFO(){

  // waht is that: hard of soft reset?

       printf("Reseting fifio's\n");     
       daqmb->calctrl_fifomrst();    
  return 1;

}

//=====================================================================

int EmuDcs::resetDAQMB(){

  //////// 03/31/2005  ((CCBdcs *)ccb)->soft_reset_dmb_csr2();

  return 1;

}
//=====================================================================
int EmuDcs::resetTMB(){
  ////////// 03/31/2005 ((CCBdcs *)ccb)->soft_reset_tmb_csr2();
  return 1;

}
//=====================================================================
int EmuDcs::resetMPC(){
  ////////// 03/31/2005  ((CCBdcs *)ccb)->soft_reset_mpc_csr2();
  return 1;

}
//=====================================================================
int EmuDcs::resetALCT(){

  // it is not possible via ccb // how to do that ?
  return 1;

}

//=====================================================================
int EmuDcs::reset_int_CCB(){
  // ccb->ccb_int_reset(); not fount in the manual of ccb 2004
  return 1;

}

//=====================================================================
int EmuDcs::ccb_L1_Reset(){
  ///////////// 03/31/2005  ((CCBdcs *)ccb)->sync_reset_csr2();
  return 1;

}
//=====================================================================

int EmuDcs::readLV_Reference(){

#ifdef DCS_PRINTING_1
         printf("Reading 1.8V Chip 1\n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_18_chip1=daqmb->adcplus(1,6);
#ifdef DCS_PRINTING_1
        printf("Reading 1.8V Chip 2\n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_18_chip2=daqmb->adcplus(2,6);
#ifdef DCS_PRINTING_1
         printf("Reading 1.8V Chip 3\n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_18_chip3 = daqmb->adcminus(3,6);
#ifdef DCS_PRINTING_1
         printf("Reading DMB GND\n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_dmb_gnd = daqmb->adcminus(3,0);
#ifdef DCS_PRINTING_1
         printf("Reading CFEB1 GND \n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_cfeb1 =daqmb->adcminus(3,1);
#ifdef DCS_PRINTING_1
         printf("Reading CFEB2 GND \n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_cfeb2 =daqmb->adcminus(3,2);
#ifdef DCS_PRINTING_1
         printf("Reading CFEB3 GND \n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_cfeb3 =daqmb->adcminus(3,3);
#ifdef DCS_PRINTING_1
         printf("Reading CFEB4 GND \n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_cfeb4 =daqmb->adcminus(3,4);
#ifdef DCS_PRINTING_1
         printf("Reading CFEB5 GND \n");
#endif
       ((REFERENCE_1_DimBroker *)REFERENCE_1_MonitorService[db_index]->value)->lv_ref_cfeb5 =daqmb->adcminus(3,5);

  return 1;

}
//=====================================================================


int EmuDcs::controlDAQMB(DAQMB *daqmb){        /////////////////////
    this->daqmb = daqmb;  
    return 1;
}
//=====================================================================
int EmuDcs::controlCCB(CCB *ccb){        /////////////////////
    this->ccb = ccb; 
    return 1; 
}
//=====================================================================
int EmuDcs::controlTMB(TMB *tmb){        /////////////////////
    this->tmb = tmb;

    //     ALCTController::CHAMBER ch_type = ALCTController::ME22;

    if(alct_c == NULL)  alct_c = new ALCTController(tmb,"ME22");//ch_type);
    else {delete alct_c; alct_c = new ALCTController(tmb,"ME22");}//ch_type);}

    return 1;
}
//=====================================================================

int EmuDcs::simulationLVStatusControl(string &ipslot){

    bool found=false;
    for(int i=0;i< d360_ipslot.size();i++ ){
      if(d360_ipslot[i] == ipslot){
         current_set=i;
         found=true;  
      }        
    }
    
    if(!found){
     d360_ipslot.push_back(ipslot);
     slot_status.push_back(0);
     current_set = d360_ipslot.size()-1;
    }
     return true;


    return 1;
}

//=====================================================================

int EmuDcs::db(){
  
  vector<Crate *> v_crates= crates();
  vector<DAQMB *> v_daqmbs;
  vector<TMB *>   v_tmbs;

  char tmp[100];

#ifdef DCS_PRINTING_1
  ///printf("21 %d %d\n",*i,v_crates.size());
#endif

  CHAMBER_NUMBER=0;

  for(int i=0;i<v_crates.size();i++){

   v_daqmbs=daqmbs(v_crates[i]);

      for(int j=0;j<v_daqmbs.size();j++){

	sprintf(tmp,"%s;%d",(v_crates[i]->vmeController()->ipAddress()).c_str(),v_daqmbs[j]->slot()); 
	slots.push_back(string(tmp));
	CHAMBER_NUMBER++;
      	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++slots[%d]=%s\n",j,tmp);

      }
  }

  return 1;

}
//=====================================================================
int EmuDcs::getServiceName(int index, char *system, string &service_name){

  
      int pos, pos_prev=0; 

      //   for(int i=0;i<slots.size();i++){
      int i=index;

           service_name=string(system);
	   
           pos_prev=slots[i].find(";",0)+1;
           service_name+=string("_")+slots[i].substr(0,pos_prev-1);

            for(int j=0;j<1;j++){
        	pos=slots[i].find(";",pos_prev);
                if(pos == string::npos)pos=slots[i].size();
                service_name+=string("_")+slots[i].substr(pos_prev,pos-pos_prev);
                pos_prev=pos+1;
	    } // for j
	   
	    //pos_prev=slots[i].find(".",0)+1;


            pos_prev=0;
#ifdef OSUcc
            for(int j=0;j<5;j++){
        	pos=service_name.find(":",pos_prev);
#else
            for(int j=0;j<3;j++){
                pos=service_name.find(".",pos_prev);
#endif

                if(pos==string::npos){


		}
                else{
		  ///		  printf("pos=%d\n",pos);
		}
                //if(pos == string::npos)pos=slots[i].size();
                //tmp_str+=string("_")+slots[i].substr(pos_prev,pos-pos_prev);
                service_name.replace(pos,1,"_");
                pos_prev=pos+1;
	    } // for j
	    //	 } // for i


	    //	    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++service_name=%s\n",service_name.c_str());

  return 1;

}


//=================== test stuff below ================================

int EmuDcs::test(){

  
  printf("int EmuDcs::test(){\n");
  /*
  ccb->set_la1_delay(30);
  ccb->get_la1_delay();

  loadDAQMB_ControlChip();
  //loadDAQMB_VMEChip();
  // loadAllCFEBs();

  

  // loadOneCFEB(0);

  */

  //  return 1;

  printf("22\n");
  // ALCTController::CHAMBER ch_type = ALCTController::ME22;
 printf("221 %ld\n",tmb);
 ALCTController *alct_r = new ALCTController(tmb,"ME22");//)ch_type);
 printf("222 %ld\n",alct_r);
  	int db = 0;
 
  	int jchan = 3; // virtext ALCT chip


	float readtemp;
        int tmb_slot=tmb->slot();
	alct_r->alct_read_temp(&tmb_slot, &readtemp);
        printf("temperature = %f\n",readtemp);

	//////     alct_r->SVFLoad(&jchan,"/home/fast/data/daqmb_config/alct_vprom/alct384_virtex_erase.svf" , 0 );
        alct_r->SVFLoad(&jchan,"/home/fast/data/daqmb_config/alct_vprom/alct384_virtex_erase.svf" , 1 );
	// *** alct_r->SVFLoad(&jchan,"/home/fast/data/daqmb_config/alct_vprom/alct384_virtex.svf" , 1 );
     /////  	int jchan = 1; // slow control ALCT chip
     /////     alct_r->SVFLoad(&jchan,"/home/fast/data/daqmb_config/alct_vprom/??????" , 0 );
  	    jchan = 5; // TMB prom
	    //// alct_r->SVFLoad(&jchan,"/data/home/fast/data/daqmb_config/tmb_vprom/tmb_erase.svf" , 0 );
	    ////alct_r->SVFLoad(&jchan,"/data/home/fast/data/daqmb_config/tmb_vprom/tmb2001a_101802.svf" , 0 );

	    //alct_r->SVFLoad(int *jch, char *fn, int db );


	    //	    vector<int> *ss = new vector<int>(5);




  return 1;


}
