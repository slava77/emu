//-----------------------------------------------------------------------
// $Id: Crate.cc,v 2.3 2006/01/12 22:36:34 mey Exp $
// $Log: Crate.cc,v $
// Revision 2.3  2006/01/12 22:36:34  mey
// UPdate
//
// Revision 2.2  2006/01/10 23:32:30  mey
// Update
//
// Revision 2.1  2005/12/20 23:39:37  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "Crate.h"
#include "VMEModule.h"
#include "VMEController.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "DAQMB.h"
#include "TMB.h"
#include "MPC.h"
#include "CCB.h"
#include "DDU.h"
#include "ALCTController.h"
#include "ChamberUtilities.h"

Crate::Crate(int number, VMEController * controller) : 
  theNumber(number),  
  theModules(24),
  theController(controller) 
{
  Singleton<CrateSetup>::instance()->addCrate(number, this);
}


Crate::~Crate() {
  for(unsigned i = 0; i < theModules.size(); ++i) {
    delete theModules[i];
  }
  delete theController;
}


void Crate::addModule(VMEModule * module) {
  theModules[module->slot()] = module;
}


std::vector<ChamberUtilities> Crate::chambers() const {
  //
  std::vector<DAQMB *> dmbVector = daqmbs();
  std::vector<TMB *>   tmbVector = tmbs();
  std::vector<ChamberUtilities>   result;
  //
  for( int i=0; i< dmbVector.size(); i++) {
      for( int j=0; j< tmbVector.size(); j++) {
	//
	if ( (tmbVector[j]->slot()+1) == (dmbVector[i]->slot()) ) {
	  ChamberUtilities chamber ;
	  chamber.SetTMB(tmbVector[j]);
	  chamber.SetDMB(dmbVector[i]);
	  chamber.SetMPC(this->mpc());
	  chamber.SetCCB(this->ccb());
	  result.push_back(chamber);
	}
	//
      }
  }
  //
  return result;
  //
}

std::vector<DAQMB *> Crate::daqmbs() const {
  std::vector<DAQMB *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DAQMB * daqmb = dynamic_cast<DAQMB *>(theModules[i]);
    if(daqmb != 0) result.push_back(daqmb);
  }
  return result;
}


std::vector<TMB *> Crate::tmbs() const {
  std::vector<TMB *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    TMB * tmb = dynamic_cast<TMB *>(theModules[i]);
    if(tmb != 0) result.push_back(tmb);
  }
  return result;
}


CCB * Crate::ccb() const {
  return findBoard<CCB>();
}


MPC * Crate::mpc() const {
  return findBoard<MPC>();
}


DDU * Crate::ddu() const {
  return findBoard<DDU>();
}
//
void Crate::enable() {
  //
  MPC * mpc = this->mpc();
  DDU * ddu = this->ddu();
  CCB * ccb = this->ccb();
  //
  if(mpc) mpc->init();
  if(ddu) ddu->dcntrl_reset();
  ccb->enable();
}
//
void Crate::disable() {
  //
  CCB * ccb = this->ccb();
  ccb->disableL1();
  ccb->disable();
  //::sleep(1);
  std::cout << "data taking disabled " << std::endl;
  //
}
//
void Crate::configure() {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  DDU * ddu = this->ddu();
  //
  ccb->configure();
  //::sleep(1);
  //
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    myTmbs[i]->configure();
  }
  //
  //::sleep(1);
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    ALCTController * alct = myTmbs[i]->alctController();
    if(alct) alct->setup(1);
  }
  //
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->restoreCFEBIdle();
    myDmbs[i]->restoreMotherboardIdle();
    myDmbs[i]->configure();
  }
  //  
  std::cout << "cards " << ccb << " " << mpc << " " << ddu << std::endl;
  if(mpc) mpc->init();
  //::sleep(1);
  //
  if(ddu) ddu->dcntrl_reset();
  //::sleep(2);
  //
}
