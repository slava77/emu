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
#include "TStoreDMBParser.h"
#include "TStoreCFEBParser.h"
#include "TStore_constants.h"
//
TStoreDMBParser::TStoreDMBParser(
				 std::vector <std::string > dmb_table,
				 std::vector<std::vector <std::string > > cfeb_table,
				 Crate * theCrate,
				 Chamber * theChamber
				 ) {
  //
  //
  // Create the DMB object
  //
  //
  std::istringstream input;
  input.str(dmb_table[DMB_SLOT_loc]);
  int slot ;
  input >> slot; 
  //
  dmb_ = new DAQMB(theCrate,theChamber,slot);
  //
  int delay;
  //
  input.str(dmb_table[FEB_DAV_DELAY_loc]);
  input >> delay; 
  dmb_->SetFebDavDelay(delay);
  //
  input.str(dmb_table[TMB_DAV_DELAY_loc]);
  input >> delay; 
  dmb_->SetTmbDavDelay(delay);
  //
  input.str(dmb_table[PUSH_DAV_DELAY_loc]);
  input >> delay; 
  dmb_->SetPushDavDelay(delay);
  //
  input.str(dmb_table[L1A_DAV_DELAY_loc]);
  input >> delay; 
  dmb_->SetL1aDavDelay(delay);
  //
  input.str(dmb_table[ALCT_DAV_DELAY_loc]);
  input >> delay; 
  dmb_->SetAlctDavDelay(delay);
  //
  input.str(dmb_table[CALIBRATION_LCT_DELAY_loc]);
  input >> delay; 
  dmb_->SetCalibrationLctDelay(delay);
  //
  input.str(dmb_table[CALIBRATION_L1A_DELAY_loc]);
  input >> delay; 
  dmb_->SetCalibrationL1aDelay(delay);
  //
  input.str(dmb_table[PULSE_DELAY_loc]);
  input >> delay; 
  dmb_->SetPulseDelay(delay);
  //
  input.str(dmb_table[INJECT_DELAY_loc]);
  input >> delay; 
  dmb_->SetInjectDelay(delay);
  //
  input.str(dmb_table[PULSE_DAC_SET_loc]);
  input >> delay; 
  dmb_->SetPulseDac(delay);
  //
  input.str(dmb_table[INJECT_DAC_SET_loc]);
  input >> delay; 
  dmb_->SetInjectorDac(delay);
  //
  input.str(dmb_table[COMP_MODE_loc]);
  input >> delay; 
  dmb_->SetCompMode(delay);
  //
  input.str(dmb_table[COMP_TIMING_loc]);
  input >> delay; 
  dmb_->SetCompTiming(delay);
  //
  input.str(dmb_table[PRE_BLOCK_END_loc]);
  input >> delay; 
  dmb_->SetPreBlockEnd(delay);
  //
  input.str(dmb_table[DMB_CRATE_ID_loc]);
  input >> delay; 
  dmb_->SetCrateId(delay);
  //
  input.str(dmb_table[FEB_CLOCK_DELAY_loc]);
  input >> delay; 
  dmb_->SetCfebClkDelay(delay);
  //
  input.str(dmb_table[XLATENCY_loc]);
  input >> delay; 
  dmb_->SetxLatency(delay);
  //
  std::vector<std::vector<std::string> > myCfebs = FindCFEBs(dmb_table, cfeb_table);
  if(myCfebs.size()) TStoreCFEBParser(myCfebs, theCrate, theChamber, dmb_);
  //
}
//
std::vector<std::vector<std::string> > TStoreDMBParser::FindCFEBs(std::vector <std::string> dmb , std::vector < std::vector <std::string > > cfeb_table){
  //
  std::vector<std::vector<std::string> > push;
  //
  int row=0;
  //
  for(unsigned int i=0; i<cfeb_table.size(); i++){
    //
    if(cfeb_table[i][CFEB_CSCID_loc] == dmb[DMB_CSCID_loc] ) {
      push.push_back(std::vector<std::string>());
      push[row] = cfeb_table[i];
      row++;
    }
    //
  }
  //
  return push;
  //
}
//
TStoreDMBParser::~TStoreDMBParser(){
  //
  //
}
//
