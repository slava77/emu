#ifndef TStoreParser_h
#define TStoreParser_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"

class TStoreParser {
  //
 public:
  //
  TStoreParser(std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > ,std::vector < std::vector <std::string > > );//
  void TStoreParser::FindTMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > tmb_table);
  void TStoreParser::FindDMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > dmb_table);
  //
  ~TStoreParser();
  //
private:
  //
  static const int tmb_table_CSCID_loc = 9;
  static const int dmb_table_CSCID_loc = 7;
  static const int csc_table_CSCID_loc = 3;
  //
};

#endif
