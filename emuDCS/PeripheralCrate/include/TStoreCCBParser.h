#ifndef TStoreCCBParser_h
#define TStoreCCBParser_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "Crate.h"
#include "EmuSystem.h"
//
class TStoreCCBParser {
  //
 public:
  //
  TStoreCCBParser(std::vector <std::string > ,
		  Crate * 
		  );//
  //
  ~TStoreCCBParser();
  //
 private:
  //
  CCB * ccb_;
  //
};

#endif
