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
#include "TStoreMPCParser.h"
#include "Chamber.h"
#include "TStore_constants.h"
//
TStoreMPCParser::TStoreMPCParser(
				 std::vector <std::string > periph_table,
				 Crate * theCrate
				 ) {
  //
  //
  // Create the MPC object
  //
  std::istringstream input;
  input.str(periph_table[11]);
  int slot ;
  input >> slot; 
  //
  mpc_ = new MPC(theCrate, slot);
  //
  int value;
  //
  input.str(periph_table[SERIALIZERMODE_loc]);
  input >> value;
  mpc_->SetTLK2501TxMode(value);
  //
  input.str(periph_table[TRANSPARENTMODE_loc]);
  input >> value;
  mpc_->SetTransparentMode(value);
  //
  input.str(periph_table[MPC_BOARDID_loc]);
  input >> value;
  mpc_->SetBoardID(value);
  //
}
//
TStoreMPCParser::~TStoreMPCParser(){
  //
  //
}
//
