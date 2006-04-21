#include <iostream>
#include <unistd.h> // for sleep
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "DDU.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "CrateSelector.h"

using namespace std;

int main(int argc,char **argv){

  // create VME Controller and Crate
  int crateId(0);
  string ipAddr("02:00:00:00:00:03");
  int port(2);
  VMEController *dynatem = new VMEController(crateId);
  dynatem->init(ipAddr,port);
  Crate *crate = new Crate(crateId,dynatem);

  // create CCB
  int ccbSlot(13);
  CCB *ccb = new CCB(crateId,ccbSlot,2004);
  ccb->configure();
  ::sleep(1);

  // create TMB & ALCT
  int tmbSlot(14);
  string chamberType("ME12");
  TMB *tmb = new TMB(crateId,tmbSlot);
  //
  cout << "Read Register" << endl;
  tmb->ReadRegister(0x4);  
  //
  tmb->version_="2004";
  cout << "Creating ALCT..." << endl;
  ALCTController *alct = new ALCTController(tmb,chamberType);
  cout << "Done..." << endl;

  //for (int i=0;i<42;i++){
  //  alct->delays_[i]=0;
  //  alct->thresholds_[i] = 20;
  //}

  //tmb->SetALCTController(alct);

  //tmb->configure();
  //::sleep(1);
  //alct->setup(1);
  //::sleep(1);

  ALCTIDRegister sc_id, chipID ;

  printf("Reading IDs...") ;

  alct->alct_read_slowcontrol_id(&sc_id) ;
  std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
  alct->alct_fast_read_id(chipID);
  std::cout << " ALCT Fastcontrol ID " << chipID << std::endl;


#if 1
  tmb->disableAllClocks();
  int debugMode(0);
  int jch(3);
  printf("Programming...");
  //int status;
  int status = alct->SVFLoad(&jch,"../svf/alct384rl.svf",debugMode);
  //int status = alct->SVFLoad(&jch,"../svf/alct288fp_rl.svf",debugMode);
  tmb->enableAllClocks();

  if (status >= 0){
    cout << "=== Programming finished"<< endl;
    cout << "=== " << status << " Verify Errors  occured" << endl;
  }
  else{
    cout << "=== Fatal Error. Exiting with " <<  status << endl;
  }

  ::sleep(1);
  ccb->configure();
  ::sleep(1);

  alct->alct_read_slowcontrol_id(&sc_id) ;
  std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
  alct->alct_fast_read_id(chipID);
  std::cout << " ALCT Fastcontrol ID " << chipID << std::endl;


#endif

}



