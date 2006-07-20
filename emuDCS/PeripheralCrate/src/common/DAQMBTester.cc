//-----------------------------------------------------------------------
// $Id: DAQMBTester.cc,v 3.0 2006/07/20 21:15:48 geurts Exp $
// $Log: DAQMBTester.cc,v $
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.5  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.4  2005/09/07 17:58:03  mey
// Upade
//
// Revision 2.3  2005/09/06 14:44:25  mey
// Defined output stream
//
// Revision 2.2  2005/06/06 10:10:07  geurts
// typo ...
//
// Revision 2.1  2005/06/06 10:05:51  geurts
// calibration-related updates by Alex Tumanov and Jason Gilmore
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include<iostream>
#include<stdio.h>
#include<unistd.h> // for sleep()
#include "DAQMBTester.h"
#include "CCB.h"
#include "DAQMB.h"
#include "JTAG_constants.h"

#ifndef debugV //silent mode
#define PRINT(x)
#define PRINTSTRING(x)
#else //verbose mode
#define PRINT(x) std::cout << #x << ":\t" << x << std::endl;
#define PRINTSTRING(x) std::cout << #x << std::endl;
#endif


void DAQMBTester::runAllTests() {
  /*  fifo();
  std::cout << "done fifo" << std::endl;
  dump();
  std::cout << "done dump" << std::endl;
  bucktest();
  std::cout << "done bucktest" << std::endl;
  daqmb_init();
  std::cout << "done daqmb_init" << std::endl;
  cfeb_pulse();
  std::cout << "done cfeb_pulse" << std::endl;
  cfeb_inject();
  std::cout << "cfeb_inject" << std::endl;
  halfset(4,4,0);
  std::cout << "done halfset(4,4,0)" << std::endl;
  cfeb_pedestals();  
  std::cout << "done pedestals" << std::endl;*/
  fakeBackPlane_ = false;  

  /*
  lowv_dump();
  std::cout << "done lowv_dump"<<std::endl;
  daqmb_adc_dump();
  std::cout << "done daqmb_adc_dump" << std::endl;
  daqmb_promfpga_dump();
  std::cout << "done daqmb_promfpga_dump" << std::endl;
 
  cfeb_inject();
  std::cout << "done inject half strip 27 on cfeb 2" << std::endl;
  */
}


void DAQMBTester::fifo()
{
  char sndfifo[700];
  char rcvfifo[700];
  
  //initialization of all 600 to 0
  for (int i=0;i<600;i++){
  sndfifo[i]=i;
  rcvfifo[i]=0;
  }

  PRINTSTRING( end of array initialization);

  if(fakeBackPlane_) {
    for(int i=0;i<2;i++){
      ccb_->SoftReset_crate();
      PRINTSTRING( reset CCB backplane);
      sleep(1);
    }
  }

  PRINTSTRING( fakebackplane done...);

  daqmb_->calctrl_fifomrst(); //DAQMB: reset all fifos
  PRINTSTRING( Reset all fifos...);
  daqmb_->wrtfifo(3,600,sndfifo);
  daqmb_->readfifo(3,600,rcvfifo);
  for(int i=0;i<6;i++)printf("%02x",rcvfifo[i]&0xff);printf("\n");
}

/**************** simple constants *****************/

void DAQMBTester::dump()
{
    std::cout<<daqmb_->adcplus(1,6)<< std::endl;     /* readback 1.8 V reference  */
    //std::cout<<std::hex<<daqmb_->febpromuser()<<std::dec<<std::endl; /* readback FEB2 Prom User Word */
}
/**************** buckeye shiftest example *************/

void DAQMBTester::bucktest() // will not be needed in final configuration
{
 daqmb_->buck_shift_test();
}

/**************** initialization example *************/

void DAQMBTester::daqmb_init() // will not be needed in final configuration
{
  
  ccb_->HardReset_crate();    // CCB:reprogram all FPGAs in Crate
  std::cout<<"HardReset_crate" << std::endl;    
  sleep(1);
  ccb_->SoftReset_crate();     // CCB:reset all FPGAs in Crate
  std::cout<<"SoftReset_crate" << std::endl;
  sleep(1);
  //ccb_->rice_clk_setup();   // CCB:setup and start CCB backplane clocks
  //std::cout << "rice_clk_setup " << std::endl; 
  daqmb_->setcrateid(123);   // DAQMB:crate id for data stream no convention yet
  std::cout<<"setcrateid" << std::endl;
  daqmb_->setfebdelay(FEB_DEF_DELAY);  // DAQMB: feb delays
  std::cout<<"setfebdelay" << std::endl;
  daqmb_->setcaldelay(CAL_DEF_DELAY); // DAQMB: cal delays
  std::cout<<"setcaldelay" << std::endl;
  daqmb_->setdavdelay(DAV_DEF_DELAY); // DAQMB: dav delays
  std::cout<<"setdavdelay" << std::endl;
  daqmb_->fxpreblkend(PRE_DEF_DELAY); // DAQMB: pre block end delay
  std::cout<<"setpreblkend" << std::endl;
  daqmb_->calctrl_fifomrst(); //DAQMB: reset all fifos
  std::cout<<"calctrl_fifomrst" << std::endl;
  daqmb_->set_comp_mode(COMPMODE_DEF); //DAQMB: set comparator mode
  std::cout<<"set_comp_mode" << std::endl;
  daqmb_->set_comp_thresh(0.050); //DAQMB: set comparator thresholds to 50 mV
  std::cout<<"set_comp_thresh" << std::endl;
  sleep(1);
// shift buckeyes into normal mode    
  daqmb_->shift_all(NORM_RUN);
  daqmb_->buck_shift(); //DAQMB: shift the buckeyes
  std::cout<<"buck_shift" << std::endl;
  std::cout << "done with daqmb_init()" << std::endl;

}



/****************** pulsing example (precision) *************/

void DAQMBTester::cfeb_pulse()
{   
int chip,ch,schan,brd;
// set dac
  daqmb_->set_cal_dac(1.0,1.0);

// set external pulser for channel 5 on all 6 chips    
  
  schan=5;  
  for(brd=0;brd<5;brd++){
    for(chip=0;chip<6;chip++){
       for(ch=0;ch<16;ch++){
         daqmb_->shift_array[brd][chip][ch]=NORM_RUN;
       }
       daqmb_->shift_array[brd][chip][schan]=EXT_CAP;
    }
  }
  daqmb_->buck_shift();

// set timing
  daqmb_->set_cal_tim_pulse(10);  // 0-22 are valid times

// pulse 1000 times
  if(fakeBackPlane_) {
    daqmb_->pulse(1000,0xff);
  } else {
    std::cout << "pulsing one time" << std::endl;
    ccb_->pulse(1, 0xff);
  }
 
}

/****************** pulsing example (LMS Caps) *************/

void DAQMBTester::cfeb_inject()
{   
// set dac
  daqmb_->set_cal_dac(1.0,1.0);

// shift patterns for half strip 27 on all planes in feb 2    
  for(int i=0;i<6;i++) halfset(2,i,30);
  daqmb_->buck_shift();
  
// set timing

  daqmb_->set_cal_tim_inject(10);  // 0-22 are valid times

// pulse 1000 times
  if(fakeBackPlane_) {
    daqmb_->inject(1000,0xff);
    std::cout << "injecting 1000 times" << std::endl;
  } else {
    std::cout << "injecting one time" << std::endl;
    ccb_->inject(1, 0xff);
  }

}


void  DAQMBTester::halfset(int ifeb,int ipln,int ihalf)
{
  if(ihalf>=0&&ihalf<32){
    int ichan=ihalf/2;
    int iside=ihalf-2*ichan;
    if(iside==0){
      if(ichan-1>=0) daqmb_->shift_array[ifeb][ipln][ichan-1]=MEDIUM_CAP;
      if(ichan-1<0&&ifeb-1>=0) daqmb_->shift_array[ifeb-1][ipln][15]=MEDIUM_CAP;

      if(ichan>=0&&ichan<=15) daqmb_->shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) daqmb_->shift_array[ifeb][ipln][ichan+1]=SMALL_CAP;
      if(ichan+1>15&&ifeb+1<5) daqmb_->shift_array[ifeb+1][ipln][0]=SMALL_CAP;
    }
    if(iside==1){
      if(ichan-1>=0) daqmb_->shift_array[ifeb][ipln][ichan-1]=SMALL_CAP;
      if(ichan-1<0&&ifeb-1>=0) daqmb_->shift_array[ifeb-1][ipln][15]=SMALL_CAP;

      if(ichan>=0&&ichan<=15) daqmb_->shift_array[ifeb][ipln][ichan]=LARGE_CAP;

      if(ichan+1<=15) daqmb_->shift_array[ifeb][ipln][ichan+1]=MEDIUM_CAP;
      if(ichan+1>15&&ifeb+1<5) daqmb_->shift_array[ifeb+1][ipln][0]=MEDIUM_CAP;
    }
  }

} 

/****************** pulsing example pedestals *************/

void DAQMBTester::cfeb_pedestals()
{   

// set pedestals (turn off pulser)
   
 daqmb_->toggle_pedestal();

// pulse 1000 times

  daqmb_->pulse(1000,0xff);
  // for single board  daqmb_->pulse(1000,0xff);
    
// set pedestals (turn on pulser)
   
 daqmb_->toggle_pedestal();

}

/*********** LVMB dump ***************/

/*********** LOADING PROMS ************************/
/*  not for standard users, only for experts */

void DAQMBTester::svfload_feb_prom()
{
 //unsigned short int dword[2];
 //dword[0]=daqmb_->febpromuser(); //get feb2 prom user id
 //char * c=(char *)dword;   // recast dword           
 //daqmb_->epromload(F2PROM,"fcntl_v9_r1.svf",1,c);  // load feb2 prom & old id
}

void DAQMBTester::svfload_prom()
{
 //char *c;
 //daqmb_->epromload(MPROM,"mprom_v9_r1.svf",0,c);  // load mprom
 //daqmb_->epromload(VPROM,"vprom_v9_r1.svf",0,c);  // load vprom
 //daqmb_->epromload(RESET,"reset_v9_r1.svf",0,c);  // load reset
}








