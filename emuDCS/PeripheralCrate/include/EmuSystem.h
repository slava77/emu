#ifndef EmuSystem_h
#define EmuSystem_h

#include <vector>
#include <iostream>
#include <fstream>

class CrateSetup;

class EmuSystem {
  //
 public:
  //
  EmuSystem();
  ~EmuSystem();
  //
 private:
  //
  CrateSetup myCrateSetup_();
  //
};

#endif
