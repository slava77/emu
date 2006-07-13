#include <iostream>
#include "Crate.h"
#include "CCB.h"
#include "MPC.h"
#include "VMEController.h"
#include "CrateSelector.h"
#include <unistd.h>

using namespace std;

int main(int argc,char **argv){

  // create VME Controller and Crate
  int crateId(0);
  string ipAddr("10.0.0.11");
  int port(6050);
  VMEController *dynatem = new VMEController(crateId);
  dynatem->init(ipAddr,port);
  //Crate *crate = new Crate(crateId,dynatem);

  // create MPC
  MPC *mpc = new MPC(0,12);
  mpc->firmwareVersion();
  mpc->init();

  // create CCb
  CCB *ccb = new CCB(0,13, 2004);
  ccb->firmwareVersion();
  ccb->hardReset();



//load test data into MPC fifo-A
//mpc->injectSP();//inject test patterns into mpc fifoA.  dan. 
char *dataFile="/home/slice/dan/dan_LCT.data";
mpc->injectSP(dataFile);//inject my own test patterns into mpc fifoA.  dan.

//keep on looping and reading out status of FIFOs A,B.
//FIFO-A will empty when  you do a TTC MPC shoot test pattern: L1AR:0xc0
for (int i=0;i<1000;i++){  
  //check FIFO status again:
  sleep(2);
  mpc->read_status();
}




/*
// shoot PRBS 
  mpc->enablePRBS();
  std::cout << " ==> Press key followed by enter to disable >" << flush;
  int dummy;
  std::cin >> dummy;
  mpc->disablePRBS();
*/

}

   
