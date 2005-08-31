#include "CrateSetup.h"
#include "Crate.h"
#include <iostream>

CrateSetup::CrateSetup() {
}
 

CrateSetup::~CrateSetup() {
  for(unsigned int i = 0; i < theCrates.size(); ++i) {
    delete theCrates[i];
  }
}


Crate * CrateSetup::crate(unsigned int i) {
  return theCrates[i];
}


void CrateSetup::addCrate(unsigned int i, Crate * crate) {
  if(i >= theCrates.size()) theCrates.resize(i+1);
  theCrates[i] = crate;
}

