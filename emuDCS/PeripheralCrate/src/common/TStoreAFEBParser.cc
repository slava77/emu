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
#include "TStoreAFEBParser.h"
#include "Crate.h"
#include "Chamber.h"
#include "ALCTController.h"
//
TStoreAFEBParser::TStoreAFEBParser(
				 std::vector<std::vector <std::string > > afeb_table,
				 Crate * theCrate,
				 Chamber * theChamber,
				 ALCTController * alct
				 ) {
  //
  //
  // Create the AFEB object
  //
  std::istringstream input;
  //
  std::cout << "AFEB configuration for " << afeb_table.size() << " AFEBs " << std::endl;
  //
  for( unsigned int afeb=0; afeb<afeb_table.size(); afeb++) {
    //
    int number;
    input.str(afeb_table[afeb][AFEB_NUMBER_loc]);
    input >> number;
    //
    int delay;
    input.str(afeb_table[afeb][AFEB_DELAY_loc]);
    input >> delay;
    //
    int threshold;
    input.str(afeb_table[afeb][AFEB_THRESHOLD_loc]);
    input >> threshold;
    //
    alct->SetAsicDelay(number-1,delay);
    alct->SetAfebThreshold(number-1,threshold);
    //
  }
  //
}
    int delay;//
TStoreAFEBParser::~TStoreAFEBParser(){
  //
  //
}
//
