#ifndef TStorePeriphParser_h
#define TStorePeriphParser_h

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
class TStorePeriphParser {
  //
 public:
  //
  TStorePeriphParser(std::vector <std::string > ,
		     std::vector < std::vector <std::string > > ,
		     std::vector < std::vector <std::string > > ,
		     std::vector < std::vector <std::string > > ,
		     std::vector < std::vector <std::string > > ,
		     std::vector < std::vector <std::string > > ,
		     std::vector < std::vector <std::string > > ,
		     EmuSystem * 
		     );//
  //
  ~TStorePeriphParser();
  //
 private:
  //
  VMEController * controller_;
  Crate * crate_;
  //
};

#endif
