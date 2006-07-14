#ifndef EmuSystem_h
#define EmuSystem_h

#include <vector>
#include <iostream>
#include <fstream>

#include "CrateSetup.h"
class Crate;

class EmuSystem {
  //
 public:
  //
  EmuSystem();
  ~EmuSystem();
  //
  inline Crate * crate(unsigned int i) {
    return myCrateSetup_.crate(i);
  }
  //
  inline std::vector<Crate *> crates() {
    return myCrateSetup_.crates();
  }
  //
  inline void addCrate(unsigned int i, Crate * crate){
    myCrateSetup_.addCrate(i, crate); 
  }
  //
 private:
  //
  CrateSetup myCrateSetup_;
  //
};

#endif
