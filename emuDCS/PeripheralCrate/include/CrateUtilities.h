#ifndef CrateUtilities_h
#define CrateUtilities_h

#include <iostream>
#include <fstream>


#include "CrateSelector.h"
#include "Crate.h"

class CrateUtilities {
  //
public:
  //
  CrateUtilities();
  virtual ~CrateUtilities();
  //
  inline void SetCrate(Crate * crate) { myCrate_ = crate ; }
  void MpcTMBTest(int);
  //
private:
  //
  CrateSelector theSelector;
  Crate * myCrate_;
  //
};

#endif
