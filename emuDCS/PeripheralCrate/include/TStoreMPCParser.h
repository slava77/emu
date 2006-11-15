#ifndef TStoreMPCParser_h
#define TStoreMPCParser_h

#include <iostream>
#include <fstream>
//
#include "MPC.h"
//
class TStoreMPCParser {
  //
 public:
  //
  TStoreMPCParser(std::vector <std::string > ,
		  Crate * 
		  );//
  //
  ~TStoreMPCParser();
  //
 private:
  //
  MPC * mpc_;
  //
};

#endif
