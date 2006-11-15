#ifndef TStoreParser_h
#define TStoreParser_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "EmuSystem.h"

class TStoreParser {
  //
 public:
  //
  TStoreParser(std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > ,
	       std::vector < std::vector <std::string > > 
	       );
  //
  //
  void ParseTableTMB(Chamber*,std::vector<std::string>);
  void ParseTableDMB(Chamber*,std::vector<std::string>);
  //
  ~TStoreParser();
  //
private:
  //
};

#endif
