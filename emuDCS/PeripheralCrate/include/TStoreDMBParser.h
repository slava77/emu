#ifndef TStoreDMBParser_h
#define TStoreDMBParser_h
//
#include <iostream>
#include <fstream>
//
#include "DAQMB.h"
#include "Crate.h"
//
class TStoreDMBParser {
  //
 public:
  //
  TStoreDMBParser(
		  std::vector <std::string > ,
		  std::vector<std::vector <std::string > >,
		  Crate *,
		  Chamber *
		  );//
  //
  std::vector<std::vector<std::string> > FindCFEBs(std::vector <std::string>, std::vector < std::vector <std::string > >);
  //
  ~TStoreDMBParser();
  //
 private:
  //
  DAQMB * dmb_;
  //
};

#endif
