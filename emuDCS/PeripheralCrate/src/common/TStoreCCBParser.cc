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
#include "TStoreCCBParser.h"
#include "Chamber.h"
#include "TStore_constants.h"
//
TStoreCCBParser::TStoreCCBParser(
				 std::vector <std::string > periph_table,
				 Crate * theCrate
				 ) {
  //
  //
  // Create the CCB object
  //
  //
  std::istringstream input;
  input.str(periph_table[CCB_SLOT_loc]);
  int slot ;
  input >> slot; 
  //
  ccb_ = new CCB(theCrate,slot);
  //
  int value;
  input.str(periph_table[TTCMODE_loc]);
  input >> value;
  ccb_->SetTTCmode(value);
  //
  input.str(periph_table[CCB_MODE_loc]);
  input >> value;
  ccb_->SetCCBmode(value);
  //
  input.str(periph_table[BX_ORBIT_loc]);
  input >> value;
  ccb_->SetBxOrbit(value);
  //
  input.str(periph_table[SPS25NS_loc]);
  input >> value;
  ccb_->SetSPS25ns(value);
  //
  input.str(periph_table[CCB_L1ADELAY_loc]);
  input >> value;
  ccb_->Setl1adelay(value);
  //
  input.str(periph_table[TTCRXID_loc]);
  input >> value;
  ccb_->SetTTCrxID(value);
  //
  input.str(periph_table[TTCRXCOARSEDELAY_loc]);
  input >> value;
  ccb_->SetTTCrxCoarseDelay(value);
  //
}
//
TStoreCCBParser::~TStoreCCBParser(){
  //
  //
}
//
