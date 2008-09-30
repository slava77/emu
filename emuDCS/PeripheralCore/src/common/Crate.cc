//-----------------------------------------------------------------------
// $Id: Crate.cc,v 3.46 2008/09/30 14:27:07 liu Exp $
// $Log: Crate.cc,v $
// Revision 3.46  2008/09/30 14:27:07  liu
// read ALCT temperature in monitoring
//
// Revision 3.45  2008/09/21 18:26:15  liu
// monitoring update
//
// Revision 3.44  2008/09/05 17:44:45  liu
// monitoring update
//
// Revision 3.43  2008/08/22 13:05:36  liu
// monitoring update
//
// Revision 3.42  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.41  2008/07/17 11:30:29  liu
// update number of counters
//
// Revision 3.40  2008/06/23 22:28:54  liu
// monitoring update
//
// Revision 3.39  2008/06/22 14:57:52  liu
// new functions for monitoring
//
// Revision 3.38  2008/05/30 11:52:04  liu
// skip dead crate during configuring
//
// Revision 3.37  2008/05/29 11:44:07  liu
// add time-since-last-hard_reset in TMB counters
//
// Revision 3.36  2008/05/28 10:35:31  liu
// DMB counters in jumbo packet
//
// Revision 3.35  2008/05/20 12:04:00  liu
// skip dead crate in monitoring
//
// Revision 3.34  2008/05/20 11:30:20  liu
// TMB counters in jumbo packet
//
// Revision 3.33  2008/05/20 10:46:01  liu
// error handling update
//
// Revision 3.32  2008/04/25 15:35:32  liu
// updates on VMECC
//
// Revision 3.31  2008/04/08 08:44:59  rakness
// remove FIFO clear for DMB fast configure
//
// Revision 3.30  2008/03/27 17:31:21  gujh
// fix the Crate:Configure   --- GU, Rakness
//
// Revision 3.27  2008/02/27 17:02:35  liu
// add Info/Warning packet handling
//
// Revision 3.26  2008/02/27 09:55:25  liu
// update
//
// Revision 3.25  2008/02/24 12:48:30  liu
// DMB online counters
//
// Revision 3.24  2008/02/23 15:25:55  liu
// TMB online counters
//
// Revision 3.23  2008/02/22 13:25:15  liu
// update
//
// Revision 3.22  2008/02/21 09:37:24  liu
// fast config option
//
// Revision 3.21  2008/02/18 12:09:19  liu
// new functions for monitoring
//
// Revision 3.20  2008/02/14 15:06:04  liu
// update
//
// Revision 3.19  2008/01/11 07:40:46  geurts
// removed obsolete include of CrateSetup.h
//
// Revision 3.18  2007/12/27 00:33:54  liu
// update
//
// Revision 3.17  2007/12/25 13:56:19  liu
// update
//
// Revision 3.16  2007/08/27 22:51:36  liu
// update
//
// Revision 3.15  2007/07/17 16:17:22  liu
// remove ChamberUtilities dependence
//
// Revision 3.14  2007/03/14 08:36:35  rakness
// remove RAT from configure
//
// Revision 3.13  2007/02/28 18:35:10  liu
// resolve class dependence
//
// Revision 3.12  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.11  2006/11/13 16:25:31  mey
// Update
//
// Revision 3.10  2006/11/10 16:51:45  mey
// Update
//
// Revision 3.9  2006/10/30 15:56:42  mey
// Update
//
// Revision 3.8  2006/10/20 13:09:44  mey
// UPdate
//
// Revision 3.7  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.6  2006/10/10 14:08:08  mey
// UPdate
//
// Revision 3.5  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.4  2006/10/03 08:15:45  mey
// Update
//
// Revision 3.3  2006/10/03 07:36:02  mey
// UPdate
//
// Revision 3.2  2006/08/01 09:47:38  mey
// Update
//
// Revision 3.1  2006/07/21 07:48:27  mey
// Cleaned up code.Got rid of include Singleton.h
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.16  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.15  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 2.14  2006/07/14 08:11:58  mey
// Got rid of Singleton
//
// Revision 2.13  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.12  2006/06/23 13:53:29  mey
// bug fix
//
// Revision 2.11  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 2.10  2006/06/15 16:38:41  rakness
// multiple slot firmware downloading
//
// Revision 2.9  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.8  2006/03/08 22:53:12  mey
// Update
//
// Revision 2.7  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.6  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.5  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.4  2006/01/18 12:46:48  mey
// Update
//
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
#include <unistd.h> // for sleep
#include <iostream>
#include <sstream>
#include <vector> 
#include <string.h> 
#include "VMEModule.h"
#include "VMEController.h"
#include "DAQMB.h"
#include "TMB.h"
#include "MPC.h"
#include "CCB.h"
#include "RAT.h"
#include "ALCTController.h"
#include "Chamber.h"
#include "VMECC.h"

namespace emu {
  namespace pc {


Crate::Crate(int CrateID, VMEController * controller) : 
  theCrateID(CrateID),  
  label_("label"),
  alive_(true),
  theModules(28),
  theController(controller)
{
  for(unsigned i=0;i<theModules.size();i++) theModules[i] = 0;
  theChambers.clear();
}


Crate::~Crate() {
  for(unsigned i = 0; i < theModules.size(); ++i) {
    delete theModules[i];
  }
  delete theController;
}


void Crate::addModule(VMEModule * module) {
  if(theModules[module->slot()] != 0 ) delete theModules[module->slot()];
  theModules[module->slot()] = module;
}

void Crate::AddChamber(Chamber * chamber) {
  theChambers.push_back(chamber);
}

#if 0
// commented out to remove dependence on ChamberUtilities. Liu 2007 July 17.
//
std::vector<ChamberUtilities> Crate::chamberUtilsMatch() const {
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
#endif

//
std::vector<Chamber*> Crate::chambers() const {
  //
  return theChambers;
  //
}
//
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

std::vector<ALCTController *> Crate::alcts() const {
  std::vector<ALCTController *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    TMB * tmb = dynamic_cast<TMB *>(theModules[i]);
    if(tmb != 0) {
      ALCTController * alct = tmb->alctController();
      if( alct != 0 ) result.push_back(alct);
    }
  }
  return result;
}


VMECC * Crate::vmecc() const {
  return findBoard<VMECC>();
}

CCB * Crate::ccb() const {
  return findBoard<CCB>();
}

MPC * Crate::mpc() const {
  return findBoard<MPC>();
}

bool Crate::IsAlive() {
  return alive_ && (vmeController()->IsAlive()); 
}

void Crate::enable() {
  //
  MPC * mpc = this->mpc();
  CCB * ccb = this->ccb();
  //
  if(mpc) mpc->init();
  if(ccb) ccb->enable();
}
//
void Crate::disable() {
  //
  CCB * ccb = this->ccb();
  if(ccb) {
    ccb->disableL1();
    ccb->disable();
  //::sleep(1);
    std::cout<< "data taking disabled " << std::endl;
  //
  }
}
//
void Crate::DumpConfiguration() {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  //
  if (ccb) std::cout << (*ccb) << std::endl;
  if (mpc) std::cout << (*mpc) << std::endl;
  //
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    std::cout << "TMB = " << i << std::endl;
    std::cout << (*myTmbs[i]) << std::endl;
    //
    ALCTController * alct = myTmbs[i]->alctController();
    if (alct) {
      std::cout << "ALCTController " << std::endl;
      std::cout << (*alct) << std::endl;
    }
    //
  }
  //
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    std::cout << "DAQMB = " << i << std::endl;
    std::cout << (*myDmbs[i]) << std::endl;
  }
  //
}
//
void Crate::configure(int c) {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  if(!ccb) return;
  std::cout << label_ << " Crate Configuring, Mode: " << c << std::endl; 
  
  std::vector<DAQMB*> myDmbs = this->daqmbs();

  std::cout << " HardReset, then lowv_onoff " << std::endl;
  ccb->hardReset();
  ::sleep(1);
  for (unsigned dmb=0; dmb<myDmbs.size(); dmb++) {
    std::cout << "DMB slot " << myDmbs[dmb]->slot() 
	      << " turn ON chamber..." << std::endl;
    if(!IsAlive())
    {  std::cout << "ERROR: Crate dead, stop!!" << std::endl;
       return;
    }
    myDmbs[dmb]->lowv_onoff(0x3f);
    //
    // The following is not needed, since DMB includes FIFO clear in hard reset
    //    std::cout << "DMB slot " << myDmbs[dmb]->slot() 
    //	      << " call calctrl_fifomrst " << std::endl;
    //    myDmbs[dmb]->calctrl_fifomrst();
  }
  ::sleep(2);
  //
  //theController->init();
  //
  if(!IsAlive())
  {  std::cout << "ERROR: Crate dead, stop!!" << std::endl;
     return;
  }
  ccb->configure();
  //
  if(!IsAlive())
  {  std::cout << "ERROR: Crate dead, stop!!" << std::endl;
     return;
  }
  if(mpc) mpc->configure();
  //

  // c>1: FAST configure, only doing configure for CCB & MPC
  if(c>1) return; 

  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    if (myTmbs[i]->slot()<22){
      //
      if(!IsAlive())
      {  std::cout << "ERROR: Crate dead, stop!!" << std::endl;
         return;
      }

      myTmbs[i]->configure();
      //
      ALCTController * alct = myTmbs[i]->alctController();
      if(alct) {
	std::cout << "alct # =" << i << std::endl;
	alct->configure();
	//
      }
      //
      //      RAT * rat = myTmbs[i]->getRAT();
      //      if(rat) {
      //	//
      //	rat->configure();
      //	//
      //      }
      //
    }
    //
  }
  //
  // std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    if (myDmbs[i]->slot()<22){
      if(!IsAlive())
      {  std::cout << "ERROR: Crate dead, stop!!" << std::endl;
         return;
      }
      myDmbs[i]->restoreCFEBIdle();
      myDmbs[i]->restoreMotherboardIdle();
      myDmbs[i]->configure();
    }
    }
  //  
}
//
void Crate::init() {
  //
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  //
  if(!ccb) return;
  ccb->init();
  //
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    myTmbs[i]->init();
  }
  //
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->init();
  }
  //  
  if(mpc) mpc->init();
  //
}
//
  Chamber * Crate::GetChamber(int slotN)
  {
     for(unsigned i=0; i < theChambers.size(); i++)
     {  if(theChambers[i]->GetTMB()->slot()==slotN || 
           theChambers[i]->GetDMB()->slot()==slotN ) return theChambers[i];
     }
     return (Chamber *) 0x0;
  }

  Chamber * Crate::GetChamber(TMB * tmb)   {  return GetChamber(tmb->slot()); }

  Chamber * Crate::GetChamber(DAQMB * dmb)   {  return GetChamber(dmb->slot()); }

  TMB * Crate::GetTMB(unsigned int slot)
  {
    if(slot < theModules.size() && (slot%2)==0) return dynamic_cast<TMB *>(theModules[slot]);
    else return NULL;
  }

  DAQMB * Crate::GetDAQMB(unsigned int slot)
  {
    if(slot < theModules.size() && (slot%2)==1) return dynamic_cast<DAQMB *>(theModules[slot]);
    else return NULL;
  }

void Crate::MonitorCCB(int cycle, char * buf) 
{
  buf[0]=0;
  
  CCB * ccb = this->ccb();
  MPC * mpc = this->mpc();
  //
  if(ccb==NULL || mpc==NULL || !IsAlive()) return;
  ccb->read_later(0x0);
  ccb->read_later(0x2);
  ccb->read_later(0x4);
  ccb->read_later(0x36);
  ccb->read_later(0x38);
  ccb->read_later(0x3a);
  ccb->read_later(0x3c);
  ccb->read_later(0x3e);
  ccb->read_later(0x44);
  ccb->read_later(0x46);
  ccb->read_later(0x48);
  mpc->read_later(0x0);
  mpc->read_later(0xB8);
  mpc->read_later(0xCA);
  int rb=mpc->read_now(0xCC, buf+2);
  if(rb>0)  buf[0]=rb;
}

void Crate::MonitorTMB(int cycle, char * buf) 
{
  int TOTAL_TMB_COUNTERS=49;
  int * countbuf, *buf4;
  short *buf2;
 
  
  buf2=(short *)buf;
  buf4=(int *)buf;
  for(int i=0; i<= TOTAL_TMB_COUNTERS*9; i++) buf4[i]=0;
  vmeController()->SetUseDelay(true);
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    if(IsAlive())
    {  countbuf=myTmbs[i]->NewCounters();
       if(countbuf) memcpy(buf+4+i*4*TOTAL_TMB_COUNTERS, countbuf, 4*TOTAL_TMB_COUNTERS);
    }
  }  
  *buf2 = TOTAL_TMB_COUNTERS*2*9;
  return;
}

void Crate::MonitorDMB(int cycle, char * buf) 
{
  int TOTAL_DMB_COUNTERS=12; // aligned at 4 bytes (integer)
  char * countbuf;
  short *buf2;

  buf2=(short *)buf;
  *buf2 = 0;
  vmeController()->SetUseDelay(true);
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    if(IsAlive())
    {  countbuf=myDmbs[i]->GetCounters();
       if(countbuf) memcpy(buf+4+i*TOTAL_DMB_COUNTERS, countbuf, TOTAL_DMB_COUNTERS);
    }
  }
  *buf2 = (TOTAL_DMB_COUNTERS/2)*9;
  return;
}

void Crate::MonitorDCS(int cycle, char * buf) 
{
  int rn, TOTAL_DCS_COUNTERS=48; // aligned at 4 bytes (integer)
  short *buf2;

  buf2=(short *)buf;
  *buf2 = 0;
  for(int i=0; i<= TOTAL_DCS_COUNTERS*9; i++) buf2[i]=0;
  vmeController()->SetUseDelay(true);
  std::vector<DAQMB*> myDmbs = this->daqmbs();
  std::vector<TMB*> myTmbs = this->tmbs();
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    if(IsAlive())
    {  
        rn=myDmbs[i]->DCSreadAll(buf+4+i*2*TOTAL_DCS_COUNTERS);
       // if ( rn<0 ) error condition
        rn=myTmbs[i]->DCSreadAll(buf+4+i*2*TOTAL_DCS_COUNTERS+46*2);
    }
  }
  *buf2 = (TOTAL_DCS_COUNTERS)*9;
  return;
}

} // namespace emu::pc
} // namespace emu
