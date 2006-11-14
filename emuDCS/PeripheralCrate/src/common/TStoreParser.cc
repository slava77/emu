//-------------------------------------------
//
//-------------------------------------------
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <map>
//
#include "TStoreParser.h"
#include "Chamber.h"
//
using namespace std;
//
//TStoreParser::TStoreParser(){
//
//std::cout << "TStoreParser" << std::endl;
//
//}
//
TStoreParser::TStoreParser(
			   std::vector < std::vector <std::string > > periph_table,
			   std::vector < std::vector <std::string > > csc_table,
			   std::vector < std::vector <std::string > > tmb_table,
			   std::vector < std::vector <std::string > > dmb_table,
			   std::vector < std::vector <std::string > > alct_table,
			   std::vector < std::vector <std::string > > afeb_table,
			   std::vector < std::vector <std::string > > cfeb_table) {
  //
  std::cout << "CSCs " << csc_table.size() << std::endl;
  //
  std::vector<Chamber> CSCs;
  //
  for(unsigned int i=0; i<csc_table.size(); i++) {
    Chamber csc;
    //
    FindTMB(csc_table[i], tmb_table);
    FindDMB(csc_table[i], dmb_table);
    //
    CSCs.push_back(csc);
    //
  }
  //
  }
//
void TStoreParser::FindTMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > tmb_table){
  //
  std::string cscId = csc[3];
  //
  for( unsigned loop=0; loop<csc.size(); loop++) std::cout << " " << csc[loop];
  //
  for(unsigned int i=0; i<tmb_table.size(); i++){
    //
    if(tmb_table[i][tmb_table_CSCID_loc] == csc[csc_table_CSCID_loc] ) {
      std::cout << "Found TMB!!!" << std::endl;
      for( unsigned loop=0; loop<tmb_table[i].size(); loop++) std::cout << " " << tmb_table[i][loop];
      std::cout << std::endl;
    }
    //
  }
  //
}
//
void TStoreParser::FindDMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > dmb_table){
  //
  std::string cscId = csc[3];
  //
  for( unsigned loop=0; loop<csc.size(); loop++) std::cout << " " << csc[loop];
  //
  for(unsigned int i=0; i<dmb_table.size(); i++){
    //
    if(dmb_table[i][dmb_table_CSCID_loc] == csc[csc_table_CSCID_loc] ) {
      std::cout << "Found DMB!!!" << std::endl;
      for( unsigned loop=0; loop<dmb_table[i].size(); loop++) std::cout << " " << dmb_table[i][loop];
      std::cout << std::endl;
    }
    //
  }
  //
}
//
TStoreParser::~TStoreParser(){
  //
  //
}
//
