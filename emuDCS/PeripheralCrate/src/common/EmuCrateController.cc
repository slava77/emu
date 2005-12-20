//-----------------------------------------------------------------------
// $Id: EmuCrateController.cc,v 1.2 2005/12/20 14:48:26 mey Exp $
// $Log: EmuCrateController.cc,v $
// Revision 1.2  2005/12/20 14:48:26  mey
// Update
//
// Revision 1.1  2005/12/20 14:37:40  mey
// Update
//
// Revision 2.2  2005/12/20 14:20:04  mey
// Update
//
// Revision 2.1  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.23  2004/06/18 23:52:34  tfcvs
// Introduced code for DCS/PCcontrol crate sharing. (FG)
//  -code additions still commented out, awaiting validation-
//
// Revision 1.22  2004/06/05 19:37:55  tfcvs
// Clean-up (FG)
//
// Revision 1.21  2004/06/03 22:00:39  tfcvs
// Changed the order in which DMB and TMB/ALCT are configured.
// Apparently, a yet-to-be-identified call in the TMB or ALCT class has
// an adverse effect on DAQMB:set_comp_mode(). Although this relocation
// seems to solve the problem it should not be the final solution ...
// Also, removed the repeated DAQMB:calctrl_fifomrst() call, which does not
// affect operation. (JG/FG)
//
// Revision 1.20  2004/06/01 09:52:07  tfcvs
//  enabled CCB in enable(), added CVS-stuff, cleaned up includes (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "EmuCrateController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "CCB.h"
#include "TMB.h"
#include "DDU.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"

EmuCrateController::EmuCrateController(){
  // clear pointers
}

void EmuCrateController::configure() {
  // read the configuration
  std::vector<Crate*> myCrates = theSelector.crates();
  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    configure(myCrates[i]);
  }
}
//
void EmuCrateController::configure(Crate * crate) {
  //
  CCB * ccb = crate->ccb();
  MPC * mpc = crate->mpc();
  DDU * ddu = crate->ddu();
  //
  ccb->configure();
  ::sleep(1);
  //
  std::vector<TMB*> myTmbs = theSelector.tmbs(crate);
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    myTmbs[i]->configure();
  }
  //
  ::sleep(1);
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    ALCTController * alct = myTmbs[i]->alctController();
    if(alct) alct->setup(1);
  }
  //
  std::vector<DAQMB*> myDmbs = theSelector.daqmbs(crate);
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->restoreCFEBIdle();
    myDmbs[i]->restoreMotherboardIdle();
    myDmbs[i]->configure();
  }
  //  
  std::cout << "cards " << ccb << " " << mpc << " " << ddu << std::endl;
  if(mpc) mpc->init();
  ::sleep(1);
  //
  if(ddu) ddu->dcntrl_reset();
  ::sleep(2);
  //
}
//
void  EmuCrateController::enable() {
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    enable(myCrates[i]);
  }
  std::cout << "TAKING DATA" << std::endl;
}
//
void EmuCrateController::enable(Crate * crate) {
  //
  MPC * mpc = crate->mpc();
  DDU * ddu = crate->ddu();
  CCB * ccb = crate->ccb();
  //
  if(mpc) mpc->init();
  if(ddu) ddu->dcntrl_reset();
  ccb->enable();
}
//
void EmuCrateController::disable() {
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    disable(myCrates[i]);
  }
}
//
void EmuCrateController::disable(Crate * crate) {
  CCB * ccb = crate->ccb();
  ccb->disableL1();
  ccb->disable();
  ::sleep(1);
  std::cout << "data taking disabled " << std::endl;
}
//
void EmuCrateController::executeCommand(std::string boardType, std::string command) {
  //
  // to do introduce open/closeSocket calls ... 
  // (or remove open/close alltogether)
  //
  if (boardType=="DAQMB") {
    std::vector<DAQMB*> dmbs(theSelector.daqmbs());
    for(unsigned i = 0; i < dmbs.size(); ++i) {
      dmbs[i]->executeCommand(command);
    }
  }
  //
  else if (boardType=="CCB") {  
    std::vector<Crate *> crates = theSelector.crates();
      for(unsigned i = 0; i < crates.size(); ++i) {
	CCB * ccb = crates[i]->ccb();
	if(ccb == 0) throw("Could not find CCB");
	ccb->executeCommand(command);
      }
  }
  //
  else if (boardType=="TMB") {
    std::vector<TMB*> tmbs = theSelector.tmbs();
    for(unsigned i = 0; i < tmbs.size(); ++i) {
      tmbs[i]->executeCommand(command);
    }
  }
  //
  else if (boardType=="MPC") {
    std::vector<Crate *> crates = theSelector.crates();
    for(unsigned i = 0; i < crates.size(); ++i) {
      MPC * mpc = crates[i]->mpc();
	if(mpc == 0) throw("Could not find MPC");
	mpc->executeCommand(command);
    }
  }
  //
  else {
    throw("Bad Board type!");
  }
}

