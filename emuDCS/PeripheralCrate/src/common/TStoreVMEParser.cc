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
#include "TStoreVMEParser.h"
#include "Chamber.h"
#include "EmuSystem.h"
#include "TStore_constants.h"
//
TStoreVMEParser::TStoreVMEParser(
				 std::vector <std::string > periph_table,
				 EmuSystem * emuSystem
				 ) {
  //
  //
  // Create the controller object
  //
  std::string VMEaddress = periph_table[VMEADDRESS_loc];
  //
  std::istringstream input;
  input.str(periph_table[PORT_loc]);
  int port ;
  input >> port; 
  //
  input.str(periph_table[PERIPH_CRATE_ID_loc]);
  int CrateID;
  input >> CrateID;
  //
  controller_ = new VMEController(CrateID);
  controller_->SetVMEAddress(VMEaddress);
  controller_->SetPort(port);
  //
  // Create the crate object
  //
  crate_ = new Crate(CrateID,controller_,emuSystem);
  //
  // Now create CSCs
  //
}
//
TStoreVMEParser::~TStoreVMEParser(){
  //
  //
}
//
