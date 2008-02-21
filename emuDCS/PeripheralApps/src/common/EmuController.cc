#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "EmuController.h"
#include "EmuEndcap.h"
#include "Crate.h"
#include "XMLParser.h"
#include "VMEController.h"

EmuController::EmuController():myEndcap(0)
{
  xmlFile_     =     "" ;
}

void EmuController::init(){
  //
  std::cout << "EmuController Init... " << std::endl ;
  myCrates.clear();
  //
  //-- parse XML file
  //
  XMLParser emuparser;
  std::cout << "----Calling XML parser using file " << xmlFile_ << std::endl ;

  emuparser.parseFile(xmlFile_);
  myEndcap = emuparser.GetEmuEndcap();
  
  if ( myEndcap ) {
    std::cout << "Found EmuEndcap" << std::endl;
  } else {
    std::cout << "EmuController failed to find EmuEndcap" << std::endl;
    return;
  }
  
  myCrates = myEndcap->crates();
  //
  //for(unsigned i = 0; i < myCrates.size(); ++i) {
  //myCrates[i]->init();
  //}
}


void EmuController::configure(int c) 
{
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->configure(c);
  }
}


void  EmuController::enable() 
{
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->enable();
  }
  std::cout << "TAKING DATA" << std::endl;
}


void EmuController::disable() 
{
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->disable();
  }
}

void EmuController::NotInDCS()
{
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) myCrates[i]->vmeController()->SetUseDCS(false);
  }
}
