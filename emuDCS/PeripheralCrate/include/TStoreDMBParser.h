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
  TStoreDMBParser(std::vector <std::string > ,
		  Crate *,
		  Chamber *
		  );//
  //
  ~TStoreDMBParser();
  //
 private:
  //
  DAQMB * dmb_;
  //
};

#endif
