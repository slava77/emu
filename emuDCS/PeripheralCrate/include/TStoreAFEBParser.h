#ifndef TStoreAFEBParser_h
#define TStoreAFEBParser_h
//
#include <iostream>
#include <fstream>
#include <vector>
//
#include "Chamber.h"
#include "Crate.h"
//
class TStoreAFEBParser {
  //
 public:
  //
  TStoreAFEBParser(std::vector<std::vector <std::string > >,
		   Crate *,
		   Chamber *,
		   ALCTController *
		  );//
  //
  ~TStoreAFEBParser();
  //
 private:
  //
};

#endif
