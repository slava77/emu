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
#include "TStoreALCTParser.h"
#include "TStoreAFEBParser.h"
#include "TStore_constants.h"
#include "Chamber.h"
//
TStoreALCTParser::TStoreALCTParser(
				   std::vector <std::string > alct_table,
				   std::vector<std::vector <std::string > >afeb_table,
				   Crate * theCrate,
				   Chamber * theChamber,
				   TMB * theTMB
				   ) {
  //
  //
  // Create the ALCT object
  //
  std::istringstream input;
  //
  std::string chamberType = alct_table[CHAMBER_TYPE_loc];
  //
  alct_ = new ALCTController(theTMB, chamberType);
  //
  int value;
  input.str(alct_table[TRIG_MODE_loc]);
  input >> value;
  alct_->SetTriggerMode(value);
  //
  input.str(alct_table[EXT_TRIG_EN_loc]);
  input >> value;
  alct_->SetExtTrigEnable(value);
  //
  input.str(alct_table[TRIG_INFO_EN_loc]);
  input >> value;
  alct_->SetTriggerInfoEnable(value);
  //
  input.str(alct_table[L1A_INTERNAL_loc]);
  input >> value;
  alct_->SetTriggerInfoEnable(value);
  //
  input.str(alct_table[ALCT_FIFO_TBINS_loc]);
  input >> value;
  alct_->SetFifoTbins(value);
  //
  input.str(alct_table[ALCT_FIFO_PRETRIG_loc]);
  input >> value;
  alct_->SetFifoPretrig(value);
  //
  input.str(alct_table[L1A_DELAY_loc]);
  input >> value;
  alct_->SetL1aDelay(value);
  //
  input.str(alct_table[L1A_OFFSET_loc]);
  input >> value;
  alct_->SetL1aDelay(value);
  //
  input.str(alct_table[L1A_WINDOW_loc]);
  input >> value;
  alct_->SetL1aDelay(value);
  //
  input.str(alct_table[NPH_THRESH_loc]);
  input >> value;
  alct_->SetPretrigNumberOfLayers(value);
  //
  input.str(alct_table[NPH_PATTERN_loc]);
  input >> value;
  alct_->SetPretrigNumberOfPattern(value);
  //
  input.str(alct_table[CCB_ENABLE_loc]);
  input >> value;
  alct_->SetCcbEnable(value);
  //
  input.str(alct_table[SEND_EMPTY_loc]);
  input >> value;
  alct_->SetSendEmpty(value);
  //
  input.str(alct_table[AMODE_loc]);
  input >> value;
  alct_->SetAlctAmode(value);
  //
  input.str(alct_table[BXC_OFFSET_loc]);
  input >> value;
  alct_->SetBxcOffset(value);
  //
  std::vector<std::vector<std::string> > myAfebs = FindAFEBs(alct_table, afeb_table);
  if(myAfebs.size()) TStoreAFEBParser(myAfebs, theCrate, theChamber, alct_);
  //
}
//
std::vector<std::vector<std::string> > TStoreALCTParser::FindAFEBs(std::vector <std::string> alct , std::vector < std::vector <std::string > > afeb_table){
  //
  std::vector<std::vector<std::string> > push;
  //
  int row=0;
  //
  for(unsigned int i=0; i<afeb_table.size(); i++){
    //
    if(afeb_table[i][AFEB_CSCID_loc] == alct[ALCT_CSCID_loc] ) {
      //
      push.push_back(std::vector<std::string>());
      push[row] = afeb_table[i];
      row++;
      //
    }
    //
  }
  //
  return push;
  //
}
//
TStoreALCTParser::~TStoreALCTParser(){
  //
  //
}
//
