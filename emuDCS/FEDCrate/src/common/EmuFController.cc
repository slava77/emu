//-----------------------------------------------------------------------
// $Id: EmuFController.cc,v 1.3 2006/05/09 19:20:03 gilmore Exp $
// $Log: EmuFController.cc,v $
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
  //
}

void EmuFController::configure() {
  // read the configuration
  std::vector<Crate*> myCrates = theSelector.crates();
  printf(" myCrates.size() %d \n",myCrates.size());  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    printf(" call mycrate->config %d \n",i);
    myCrates[i]->configure(i);
  }
}
//
//
void  EmuFController::enable() {
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    myCrates[i]->enable();
  }
  std::cout << "TAKING DATA" << std::endl;
}
//
//
void EmuFController::disable() {
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    myCrates[i]->disable();
  }
}
//
//
int EmuFController::irqtest(){  
  std::vector<Crate*> myCrates = theSelector.crates();
  int ret,ret2;
  ret=0;
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    ret2=myCrates[i]->irqtest(i,0);
    if(ret2!=0&&ret2!=irqlast[i]){
       ret=ret2;
       irqcrate=myCrates[i]->irqtest(i,1);
       irqslot=myCrates[i]->irqtest(i,2);
       irqstatus=myCrates[i]->irqtest(i,3);
    }
    irqlast[i]=ret2;
  }
  return ret;
}
