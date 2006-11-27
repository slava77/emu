#ifndef TStoreCFEBParser_h
#define TStoreCFEBParser_h
//
#include <iostream>
#include <fstream>
#include <vector>
//
#include "Chamber.h"
#include "Crate.h"
//
class TStoreCFEBParser {
  //
 public:
  //
  TStoreCFEBParser(std::vector<std::vector <std::string > >,
		   Crate *,
		   Chamber *,
		   DAQMB *
		  );//
  //
  ~TStoreCFEBParser();
  //
 private:
  //
};

#endif
