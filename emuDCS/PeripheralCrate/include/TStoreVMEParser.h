#ifndef TStoreVMEParser_h
#define TStoreVMEParser_h

#include <iostream>
#include <fstream>
#include <string>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "Crate.h"
#include "EmuSystem.h"
//
class TStoreVMEParser {
  //
 public:
  //
  TStoreVMEParser(
		  std::vector <std::string > ,
		  EmuSystem * 
		  );//
  //
  inline Crate * crate()const { return crate_; }
  //
  ~TStoreVMEParser();
  //
 private:
  //
  VMEController * controller_;
  Crate * crate_;
  //
};

#endif
