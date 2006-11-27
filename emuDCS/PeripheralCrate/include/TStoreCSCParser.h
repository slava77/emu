#ifndef TStoreCSCParser_h
#define TStoreCSCParser_h

#include <iostream>
#include <fstream>
//
#include "Chamber.h"
//
class TStoreCSCParser {
  //
 public:
  //
  TStoreCSCParser(std::vector <std::string > , 
		  std::vector<std::vector <std::string > >,
		  std::vector<std::vector <std::string > >,
		  std::vector<std::vector <std::string > >,
		  std::vector<std::vector <std::string > >,
		  std::vector<std::vector <std::string > >,
		  Crate * 
		  );//
  //
  std::vector<std::string> TStoreCSCParser::FindTMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > tmb_table);
  std::vector<std::string> TStoreCSCParser::FindDMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > dmb_table);
  //
  ~TStoreCSCParser();
  //
 private:
  //
  Chamber * csc_;
  //
};

#endif
