//-----------------------------------------------------------------------
// $Id: EmuFController.cc,v 1.1 2006/01/21 19:55:02 gilmore Exp $
// $Log: EmuFController.cc,v $
// Revision 1.1  2006/01/21 19:55:02  gilmore
// *** empty log message ***
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

EmuFController::EmuFController(){
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
    myCrates[i]->configure();
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

