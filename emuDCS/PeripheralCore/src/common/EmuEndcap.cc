#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <unistd.h> 
#include <string>
#include "emu/pc/Crate.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/EmuEndcap.h"

namespace emu {
  namespace pc {



EmuEndcap::EmuEndcap(){
  theCrates.clear();
}

EmuEndcap::~EmuEndcap() {
  for(unsigned int i = 0; i < theCrates.size(); ++i) {
    delete theCrates[i];
  }
}


void EmuEndcap::addCrate(Crate * crate) {
  theCrates.push_back(crate);
}


void EmuEndcap::addCrate(unsigned int i, Crate * crate) {
  if(i >= theCrates.size()) theCrates.resize(i+1);
  if(theCrates[i]) delete theCrates[i];
  theCrates[i] = crate;
}


void EmuEndcap::setCrate(int crate) {
  theSelectedCrates.clear();
  // -1 means all crates
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
  }
}

void EmuEndcap::setCrate(std::string strCrate) {
  theSelectedCrates.clear();
  
  int crate = -255;
  sscanf(strCrate.c_str(), "%d", &crate);
  std::cout << "Crate in TBB::setCrate = " << crate << std::endl;
  //-1 means all crates
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
  }
}


std::vector<Crate *> EmuEndcap::crates()  
{
  std::vector<Crate *> result;
  if(theSelectedCrates.empty()) {
    for(unsigned icrate = 0; icrate < theCrates.size(); ++icrate) {
         if(theCrates[icrate]->CrateID()!=999){
	     result.push_back(theCrates[icrate]);
         }
      }
  } else {
    for(unsigned icrate = 0; icrate < theCrates.size(); ++icrate) {
      if(theCrates[icrate]->CrateID()!=999) {
         for(unsigned iSelect = 0; iSelect < theSelectedCrates.size(); ++iSelect) {
           if(theCrates[icrate]->CrateID() == theSelectedCrates[iSelect]) 
              result.push_back(theCrates[icrate]);
         }
      }
    }
  }
  return result;
}

std::vector<Crate *> EmuEndcap::broadcast_crate()  {
  std::vector<Crate *> result;
  for(unsigned icrate = 0; icrate < theCrates.size(); ++icrate) {
     if(theCrates[icrate]->CrateID()==999){
        result.push_back(theCrates[icrate]);
     }
  }
  return result;
}


std::vector<DAQMB *> EmuEndcap::daqmbs()  {
  std::vector<DAQMB *> result;
  std::vector<Crate *> myCrates = crates();
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    std::vector<DAQMB *> newCards = daqmbs(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  return result;
}

    
std::vector<DAQMB *> EmuEndcap::daqmbs(Crate * crate)  {
  return crate->daqmbs();
}


std::vector<TMB *> EmuEndcap::tmbs()  {
  std::vector<TMB *> result;
  std::vector<Crate *> myCrates = crates();
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    std::vector<TMB *> newCards = tmbs(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  return result;
}


std::vector<TMB *> EmuEndcap::tmbs(Crate * crate)  {
  return crate->tmbs();
}
} // namespace emu::pc
} // namespace emu
