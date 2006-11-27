//-------------------------------------------
//
//-------------------------------------------
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <set>
//
#include "TStoreParser.h"
#include "TStorePeriphParser.h"
#include "Chamber.h"
#include "EmuSystem.h"
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
  // Create EmuSystem
  //
  std::cout << "Creating EmuSystem" << std::endl;
  //
  EmuSystem * emuSystem = new EmuSystem();
  //
  std::cout << "Done Creating EmuSystem" << std::endl;
  //
  for(unsigned int crate=0; crate<periph_table.size(); crate++) {
    //
    TStorePeriphParser(
		       periph_table.at(crate),
		       csc_table,
		       tmb_table,
		       dmb_table,
		       alct_table,
		       afeb_table,
		       cfeb_table,
		       emuSystem
		       );
    //
  }
  //
}
//
void ParseTableTMB(Chamber* csc,std::vector<std::string>){
  //
  //
}
//
void ParseTableDMB(Chamber* csc,std::vector<std::string>){
  //
  //
}
//
//
TStoreParser::~TStoreParser(){
  //
  //
}
//
