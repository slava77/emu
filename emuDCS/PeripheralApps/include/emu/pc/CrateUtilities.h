#ifndef CrateUtilities_h
#define CrateUtilities_h

#include <iostream>
#include <fstream>


#include "emu/pc/Crate.h"

namespace emu {
  namespace pc {


class CrateUtilities {
  //
public:
  //
  CrateUtilities();
  virtual ~CrateUtilities();
  //
  inline void SetCrate(Crate * crate) { myCrate_ = crate ; }
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void MpcTMBTest(int number_of_events, int minimum_delay_value, int maximum_delay_value); //"safe window" scan
  void MpcTMBTest(int number_of_events);                                                   //not the "safe window" scan
  inline int  GetMpcTMBTestResult() { return MpcTMBTestResult; }             //result of the non-"safe window" scan
  inline void SetMpcTMBTestResult(int result) { MpcTMBTestResult = result; }
  //
 private:
  //
  bool debug_;
  //
  int MpcTMBTestResult;
  std::ostream * MyOutput_ ;
  //
  Crate * myCrate_;
  //
};

  } // namespace emu::pc
  } // namespace emu
#endif
