#include <iostream>
#include <unistd.h> // for sleep
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"

using namespace std;

int main(int argc,char **argv){

  // create VME Controller and Crate
  int crateId(0);
  string ipAddr("02:00:00:00:00:07");
  int port(3);
  VMEController *dynatem = new VMEController();
  dynatem->reset();
  dynatem->init(ipAddr,port);
  Crate *crate = new Crate(crateId,dynatem);

  // create CCB
  int ccbSlot(13);
  CCB *ccb = new CCB(crate,ccbSlot);
  //
  //::sleep(1);
  //
  ccb->configure();
  ::sleep(1);
  //
  ccb->firmwareVersion();
  int debugMode(0);
  int jch(6);
  printf("Programming...");
  //
  //int status = ccb->svfLoad(&jch,"../svf/ccb2004_erase.svf",debugMode);
  int status = ccb->svfLoad(&jch,"../svf/ccb2004_020306.svf",debugMode);
  //
  ccb->configure();
  //
  ccb->firmwareVersion();
  //
  if (status >= 0){
    cout << "=== Programming finished"<< endl;
    cout << "=== " << status << " Verify Errors  occured" << endl;
  }
  else{
    cout << "=== Fatal Error. Exiting with " <<  status << endl;
  }
}
