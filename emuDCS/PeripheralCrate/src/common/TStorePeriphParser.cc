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
#include "TStorePeriphParser.h"
#include "TStoreVMEParser.h"
#include "TStoreCCBParser.h"
#include "TStoreMPCParser.h"
#include "TStoreCSCParser.h"
#include "EmuSystem.h"
#include "Crate.h"
#include "TStore_constants.h"
//
TStorePeriphParser::TStorePeriphParser(
				       std::vector <std::string > periph_table,
				       std::vector < std::vector <std::string > > csc_table,
				       std::vector < std::vector <std::string > > tmb_table,
				       std::vector < std::vector <std::string > > dmb_table,
				       std::vector < std::vector <std::string > > alct_table,
				       std::vector < std::vector <std::string > > afeb_table,
				       std::vector < std::vector <std::string > > cfeb_table,
				       EmuSystem * emuSystem
				       ) {
  //
  //
  // Create the controller/crate objects
  //
  TStoreVMEParser vmeParser(periph_table,emuSystem);
  //
  Crate * crate = vmeParser.crate();
  //
  TStoreCCBParser(periph_table,crate);
  TStoreMPCParser(periph_table,crate);
  //
  // Now create CSCs
  //
  for (unsigned int CSCs=0; CSCs<csc_table.size(); CSCs++){
    //
    std::vector <std::string> table = csc_table[CSCs];
    TStoreCSCParser(table,tmb_table,dmb_table,crate);
    //
  }
  //
}
//
TStorePeriphParser::~TStorePeriphParser(){
  //
  //
}
//
