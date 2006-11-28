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
#include "TStore_constants.h"
//
#include "TStoreCFEBParser.h"
//
TStoreCFEBParser::TStoreCFEBParser(
				   std::vector<std::vector <std::string > > cfeb_table,
				   Crate * theCrate,
				   Chamber * theChamber,
				   DAQMB * dmb
				   ) 
{
  //
  //
  // Create the CFEB object
  //
  std::istringstream input;
  //
  for( unsigned int cfeb=0; cfeb<cfeb_table.size(); cfeb++) {
    //
    int number;
    input.str(cfeb_table[cfeb][CFEB_NUMBER_loc]);
    input >> number;
    //
    if( number < 5 ) {
      CFEB cfeb(number);
      dmb->cfebs_.push_back(cfeb);
    } else {
      std::cout << "ERROR: CFEB range 0-4" <<std::endl;
    }
    //
  }
  //
}
//
TStoreCFEBParser::~TStoreCFEBParser(){
  //
  //
}
//
