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
  string ipAddr("02:00:00:00:00:07");
  int port(2);
  VMEController *dynatem = new VMEController(crateId);
  dynatem->init(ipAddr,port);
  Crate *crate = new Crate(crateId,dynatem);

  // create CCB
  int ccbSlot(13);
  CCB *ccb = new CCB(crateId,ccbSlot,2004);
  ccb->configure();
  ::sleep(1);

  // create MPC
  int mpcSlot(12);
  MPC *mpc = new MPC(crateId,mpcSlot);
  //
  mpc->firmwareVersion();
  int debugMode(0);
  int jch(6);
  printf("Programming...");
  //
  //int status = mpc->SVFLoad(&jch,"../svf/mpc_erase.svf",debugMode);
  int status = mpc->SVFLoad(&jch,"../svf/mpc2004id2_042606.svf",debugMode);
  //
  ccb->configure();
  //
  mpc->firmwareVersion();
  //
  if (status >= 0){
    cout << "=== Programming finished"<< endl;
    cout << "=== " << status << " Verify Errors  occured" << endl;
  }
  else{
    cout << "=== Fatal Error. Exiting with " <<  status << endl;
  }
}
