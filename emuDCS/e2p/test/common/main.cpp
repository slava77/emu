

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>
#include <time.h>

#include <string.h>

#ifdef Linux
#include <getopt.h>
#endif



#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>  /* sleep function */
#include <signal.h>  /* Usual interrupt handler stuff */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>


///#include <inet.h>


//enum DEVTYPE prev_dev = NONE;
#define FASTDEV_READOUT

bool SIMULATION=false;


using namespace std;

//=============================================================================
//==========================================================
#include <dis.hxx>
#include <string>
#include <vector>


int OPERATION_ACTIVE=false;

#define CFEB_NUMBER 5

typedef struct{
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER];
  float status[CFEB_NUMBER];
}CFEB_LV;

typedef struct{
  float v18;
  float v33;
  float v55;
  float v56;
  float c18;
  float c33;
  float c55;
  float c56;
  float status;
}ALCT_LV;

typedef struct{

CFEB_LV Cfeb_o;
ALCT_LV Alct_o;

}LV_1_data;

typedef struct{

  LV_1_data data;

  // char dynatemTcpIp[80];
 int update_value;
 int setNumber;
 char dynatemTcpIp[80];

}LV_1_DimBroker;




typedef struct{

  //  CFEB_LV Cfeb_o;
  /*
     char dynatemTcpIp[80];
     int setNumber; 
     int update_value;
  */
  /*
  float v33[CFEB_NUMBER];
  float v50[CFEB_NUMBER];
  float v60[CFEB_NUMBER];
  float c33[CFEB_NUMBER];
  float c50[CFEB_NUMBER];
  float c60[CFEB_NUMBER]; 
  
  float v18a;
  float v33a;
  float v55a;
  float v56a;
  float c18a;
  float c33a;
  float c55a;
  float c56a; 
  */

  /// ALCT_LV Alct_o;
 
   LV_1_data data;

  //  char dynatemTcpIp[80];
   int update_value;
     int setNumber;
   char dynatemTcpIp[80];

}LV_1_TEST;






int main_continue(int argc, char *argv[]);
int cfeb_lv(int cfeb_number, CFEB_LV &cfeb_lv_o);
int alct_lv(ALCT_LV &alct_lv_o);
int lv_on_wrap();
int lv_off_wrap();

ALCT_LV alct_lv_o;
CFEB_LV cfeb_lv_o;

//========================================================================

vector<string> d360_ipslot;
vector<int> slot_status;
int current_set;

//===========================================


//===========================================

//========================================

#include <iostream>
#include "PeripheralCrateController.h"

//fg #include "VMEControllerdcs.h"
//fg #include "DAQMBdcs.h"
//fg #include "CCBdcs.h"
//fg #include "FakeCCB.h"
#include "VMEController.h"
#include "DAQMB.h"
#include "CCB.h"
#include "TMB.h"
///////////#include "ALCTController.h"
//#include "TestBeamCrateController.h"
#include "CrateSetup.h"





int main()
{

  CrateSetup *cr_setup_o= new CrateSetup(); /////////////////

  //  exit(0);

  //VMEController * vme = new VMEController("192.168.10.11", 6050);
  //fg VMEController * vme = new VMEControllerdcs("128.97.22.69", 6030);
    VMEController * vme = new VMEController(0, "128.97.22.69", 6030);
  // exit(0);
  Crate * crate = new Crate(0, vme); 
  //  exit(0);
  //DAQMB * daqmb = new DAQMB(0, 12, 5);
  //fg DAQMB * daqmb = new DAQMBdcs(0, 7, 5);
  DAQMB * daqmb = new DAQMB(0, 7, 5);

   //  exit(0);

  //  crate->addModule(daqmb);
   ////////  CCB * ccb =  new FakeCCB(0, 13);
  //fg CCB * ccb =  new CCBdcs(0, 13);
  CCB * ccb =  new CCB(0, 13);

 //  exit(0);

  TMB * tmb = new TMB(0, 6);

  //  exit(0);

  //////////  ALCTController alct(tmb, ALCTController::ME21);

  //////////  TestBeamCrateController controller;

  ///////  cr_setup_o->addCrate(0,crate);         ///////////// ??? number

  daqmb->lowv_adc(3,0);
  ccb->syncReset();
  vme->end();
  daqmb->lowv_adc(3,0);
  ccb->syncReset();
  /*
  PeripheralCrateController *pph= new PeripheralCrateController(); ////////////////////

  //  exit(0);


  vector<Crate *> v_crate_o=pph->crates();
  vector<DAQMB *> v_daqmb_o=pph->daqmbs(v_crate_o[0]);

  int size=v_crate_o.size();
  int size1= v_daqmb_o.size();

  printf("%d %d \n",size,size1); 

  
   pph->loadDAQMB(v_daqmb_o[0]);

   /////  v_daqmb_o[0]->start();

      /// v_daqmb_o[0]->end();
      //  v_daqmb_o[0]->start();
       pph->lowv_dump();
       ////   ccb->start();
        v_daqmb_o[0]->end();

  //////  pph->daqmb=daqmb;

  cout << "configure TMB " << endl;
  //tmb->configure();

  ///tmb->firmwareVersion();
  //tmb->read_delays();
  cout << "setup ALCT" << endl;

  */

  exit(0);      //////////////////////////////
  ////////////  alct.setup(1);

  return 0;
}
