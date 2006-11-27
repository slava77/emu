#ifndef TStoreALCTParser_h
#define TStoreALCTParser_h

#include <iostream>
#include <fstream>
//
#include "ALCTController.h"
#include "TMB.h"
#include "Crate.h"
#include "Chamber.h"
//
class TStoreALCTParser {
  //
 public:
  //
  TStoreALCTParser(std::vector <std::string > ,
		   std::vector<std::vector <std::string > >,
		   Crate *, Chamber *, TMB * 
		   );//
  //
  //
  ~TStoreALCTParser();
  //
  std::vector<std::vector<std::string> > FindAFEBs(std::vector <std::string> , std::vector < std::vector <std::string > >);
  //
 private:
  //
  ALCTController * alct_;
  //
};

#endif
