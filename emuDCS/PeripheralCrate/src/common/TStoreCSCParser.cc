//-------------------------------------------
//
//-------------------------------------------
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <sstream>
//
#include "TStoreCSCParser.h"
#include "TStoreTMBParser.h"
#include "TStoreDMBParser.h"
#include "Chamber.h"
#include "TStore_constants.h"
//
TStoreCSCParser::TStoreCSCParser(
				 std::vector <std::string > csc_table,
				 std::vector<std::vector <std::string > > tmb_table,
				 std::vector<std::vector <std::string > > dmb_table,
				 Crate * theCrate
				 ) 
{
  //
  //
  // Create the CSC object
  //
  std::istringstream input;
  //
  csc_ = new Chamber();
  csc_->SetCrate(theCrate);
  //
  std::vector <std::string> myTMB_table = FindTMB(csc_table, tmb_table); 
  TStoreTMBParser(myTMB_table,theCrate,csc_);
  //
  std::vector <std::string> myDMB_table = FindTMB(csc_table, tmb_table); 
  TStoreDMBParser(myDMB_table,theCrate,csc_);
  //
}
//
std::vector<std::string> TStoreCSCParser::FindTMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > tmb_table){
  //
  for( unsigned loop=0; loop<csc.size(); loop++) std::cout << " " << csc[loop];
  //
  for(unsigned int i=0; i<tmb_table.size(); i++){
    //
    if(tmb_table[i][TMB_CSCID_loc] == csc[CSC_CSCID_loc] ) {
      return tmb_table[i];
      //
    }
    //
  }
  //
}
//
std::vector<std::string> TStoreCSCParser::FindDMB(std::vector <std::string> csc, std::vector < std::vector <std::string > > dmb_table){
  //
  for( unsigned loop=0; loop<csc.size(); loop++) std::cout << " " << csc[loop];
  //
  for(unsigned int i=0; i<dmb_table.size(); i++){
    //
    if(dmb_table[i][DMB_CSCID_loc] == csc[CSC_CSCID_loc] ) {
      return dmb_table[i];
    }
    //
  }
  //
}
//
TStoreCSCParser::~TStoreCSCParser(){
  //
  //
}
//
