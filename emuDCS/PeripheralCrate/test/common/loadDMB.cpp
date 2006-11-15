#include <iostream>
#include <unistd.h> // for sleep
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "CrateSelector.h"
#include "EmuSystem.h"

using namespace std;

int main(int argc,char **argv){

  // create VME Controller and Crate
  int crateId(0);
  string ipAddr("02:00:00:00:00:07");
  int port(3);
  VMEController *dynatem = new VMEController(crateId);
  dynatem->init(ipAddr,port);
  //
  dynatem->reset();
  EmuSystem * emuSystem = new EmuSystem();
  Crate *crate = new Crate(crateId,dynatem,emuSystem);
  Chamber * chamber = new Chamber();
  //
  // create CCB
  int ccbSlot(13);
  CCB *ccb = new CCB(crate,ccbSlot);
  ccb->configure();
  ::sleep(1);
  //
  // create DMB
  //
  int dmbSlot(25);
  DAQMB *dmb = new DAQMB(crate,chamber,dmbSlot);
  printf("DMB fpga user id                   : %x ", (int) dmb->mbfpgauser());
  //
  char *outp = "0";
  dmb->epromload(MPROM,"../svf/dmb6cntl_v18_r2.svf",1,outp);  // load mprom
  //
  ccb->configure();
  //
}

