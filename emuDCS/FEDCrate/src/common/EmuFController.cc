//-----------------------------------------------------------------------
// $Id: EmuFController.cc,v 3.6 2007/10/23 17:50:17 gilmore Exp $
// $Log: EmuFController.cc,v $
// Revision 3.6  2007/10/23 17:50:17  gilmore
// EmuFCrateManager fixes for real USC operation
//
// Revision 3.5  2007/10/08 19:20:05  gilmore
// fixes for multi-crate Soap operation
//
// Revision 3.4  2007/09/28 14:17:58  ichiro
// added EmuFCrateManager
//
// Revision 3.3  2007/07/23 05:03:28  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.1.2.4  2007/02/23 14:16:43  ichiro
// add a debug output
//
// Revision 3.1.2.3  2007/02/20 10:20:40  ichiro
// access correct crate/slot
//
// Revision 3.1.2.2  2007/02/15 15:39:27  ichiro
// added DDU sTTS bits setting/reading
//
// Revision 3.1.2.1  2007/02/07 16:57:58  ichiro
// add read/writeTTSBits()
//
// Revision 3.1  2006/09/06 15:43:50  gilmore
// Improved Config and interrupt features.
//
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.3  2006/05/09 19:20:03  gilmore
// Fix for DCC configure function.
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "EmuFController.h"
#include "Crate.h"
#include "DCC.h"
#include "DDU.h"
#include "VMEController.h"
#include "FEDCrateParser.h"

EmuFController::EmuFController()
{
  // clear pointers
  xmlFile_     = 
    "$(XDAQ_ROOT)/emu/emuDCS/FEDCrate/config.xml" ;
  //
}

void EmuFController::init(){
  //
  std::cout << "Init " << std::endl ;
  //
  //-- parse XML file
  //
  std::cout << "---- XML parser ----" << std::endl;
  std::cout << " Here parser " << std::endl;
  FEDCrateParser parser;
  std::cout << " Using file " << xmlFile_ << std::endl ;
  parser.parseFile(xmlFile_.c_str());

  theCrates = parser.crateVector();
  //

/*  Need to add for Soap Init?
  printf(" EmuFController::init:  theController->vmeirq_start_ %d, now irq+pthread_end \n", theController->vmeirq_start_);
  theController->irq_pthread_end(crate);
//  How to set  irqprob=0 ?
  if(theController->vmeirq_start_==1){
    printf(" EmuFController::init: theController->vmeirq_start_ %d, now irq+pthread_start \n", theController->vmeirq_start_);
    theController->irq_pthread_start(crate);
  }
*/
}


void EmuFController::configure() {
  // read the configuration
  //  std::vector<Crate*> myCrates = theSelector.crates();
  vector<Crate *> myCrates = theCrates;
  printf(" myCrates.size() %d \n",myCrates.size());  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    printf(" call mycrate->config %d \n",i);
    myCrates[i]->configure(0);
  }
}


void  EmuFController::enable() {
// JRG, not used?
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    myCrates[i]->enable();
  }
  std::cout << "TAKING DATA" << std::endl;
}


void EmuFController::disable() {
// JRG, not used?
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    myCrates[i]->disable();
  }
}


int EmuFController::irqtest(){
// JRG, not used?
  std::vector<Crate*> myCrates = theSelector.crates();
  int ret,ret2;
  ret=0;
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    ret2=myCrates[i]->vmeController()->thread()->test(0);
    if(ret2!=0&&ret2!=irqlast[i]){
       ret=ret2;
       irqcrate=myCrates[i]->vmeController()->thread()->test(1);
       irqslot=myCrates[i]->vmeController()->thread()->test(2);
       irqstatus=myCrates[i]->vmeController()->thread()->test(3);
    }
    irqlast[i]=ret2;
  }
  return ret;
}


void EmuFController::writeTTSBits(
    unsigned int crate, unsigned int slot, unsigned int bits)
{
  bool useDCC = (slot == 8 || slot == 18);

  cout << "### EmuFController::writeTTSBits on " << crate << " " << slot << endl;

  if (useDCC) {
    DCC *dcc = getDCC(crate, slot);

    dcc->mctrl_fmmset((bits | 0x10) & 0xffff);

  } else { // DDU
    DDU *ddu = getDDU(crate, slot);
    if (ddu == NULL) {
      cout << "### EmuFController::writeTTSBits, getDDU returns NULL, skipping write" << std::endl;
    } else {
      cout << "### EmuFController::writeTTSBits call wr_fmmreg " << std::endl;
      ddu->vmepara_wr_fmmreg((bits | 0xf0e0) & 0xffff);
    }
  }
}


unsigned int EmuFController::readTTSBits(
    unsigned int crate, unsigned int slot)
{
  unsigned int bits;

  bool useDCC = (slot == 8 || slot == 18);

  cout << "### EmuFController::readTTSBits on " << crate << " " << slot << endl;

  if (useDCC) {
    DCC *dcc = getDCC(crate, slot);

    bits = dcc->mctrl_fmmrd();

  } else { // DDU
    DDU *ddu = getDDU(crate, slot);
    if (ddu == NULL) {
      cout << "### EmuFController::readTTSBits, getDDU returns NULL, skipping read" << std::endl;
      bits=0xface;
    } else {
      cout << "### EmuFController::readTTSBits call rd_fmmreg " << std::endl;
      bits = ddu->vmepara_rd_fmmreg();
    }
  }

  return bits & 0xf;
}


DCC *EmuFController::getDCC(int crate, int slot)
{
  theSelector.setCrate(crate);
  theSelector.setSlot(slot);

  std::vector<DCC *> dccs = theSelector.dccs();

  if (dccs.size() > 0) {
    return dccs[0];
  } else {
    return NULL;
  }
}


DDU *EmuFController::getDDU(int crate, int slot)
{
  //  cout << "  getDDU: calling setCrate" << std::endl;
  theSelector.setCrate(crate);
  //  cout << "  getDDU: calling setSlot" << std::endl;
  theSelector.setSlot(slot);

  //  cout << "  getDDU: finished setSlot, select ddus" << std::endl;
  std::vector<DDU *> ddus = theSelector.ddus();

  //  cout << "  getDDU: return, ddus.size=" << ddus.size() << std::endl;
  if (ddus.size() > 0) {
    return ddus[0];
  } else {
    return NULL;
  }
}

// End of file
