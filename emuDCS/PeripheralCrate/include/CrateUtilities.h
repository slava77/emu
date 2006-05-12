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
  inline int  GetMpcTMBTestResult() { return MpcTMBTestResult; }
  inline void SetMpcTMBTestResult(int result) { MpcTMBTestResult = result; }
  void MpcTMBTest(int);
  //
private:
  //
  int MpcTMBTestResult;
  CrateSelector theSelector;
  Crate * myCrate_;
  //
};

#endif
