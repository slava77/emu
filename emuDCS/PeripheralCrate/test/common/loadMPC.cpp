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
  string ipAddr("02:00:00:00:00:1a");
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

  // create MPC
  int mpcSlot(12);
  MPC *mpc = new MPC(crate,mpcSlot);
  //
  mpc->firmwareVersion();
  int debugMode(0);
  int jch(6);
  printf("Programming...");
  //
  //int status = mpc->SVFLoad(&jch,"../svf/mpc_erase.svf",debugMode);
  int status = mpc->svfLoad(&jch,"/home/slice/firmware/mpc2004id2_042606.svf",debugMode);
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
