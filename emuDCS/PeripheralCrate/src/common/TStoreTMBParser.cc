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
#include "TStoreTMBParser.h"
#include "TStore_constants.h"
#include "Chamber.h"
//
TStoreTMBParser::TStoreTMBParser(
				 std::vector <std::string > tmb_table,
				 Crate * theCrate,
				 Chamber * theChamber
				 ) {
  //
  //
  // Create the TMB object
  //
  //
  std::istringstream input;
  input.str(tmb_table[TMB_SLOT_loc]);
  int slot ;
  input >> slot; 
  //
  tmb_ = new TMB(theCrate,theChamber,slot);
  //
  int delay;
  //
  input.str(tmb_table[CFEB0_DELAY_loc]);
  input >> delay; 
  tmb_->SetCFEB0delay(delay);
  //
  input.str(tmb_table[CFEB1_DELAY_loc]);
  input >> delay; 
  tmb_->SetCFEB1delay(delay);
  //
  input.str(tmb_table[CFEB2_DELAY_loc]);
  input >> delay; 
  tmb_->SetCFEB2delay(delay);
  //
  input.str(tmb_table[CFEB3_DELAY_loc]);
  input >> delay; 
  tmb_->SetCFEB3delay(delay);
  //
  input.str(tmb_table[CFEB4_DELAY_loc]);
  input >> delay; 
  tmb_->SetCFEB4delay(delay);
  //
  input.str(tmb_table[ALCT_TX_CLOCK_DELAY_loc]);
  input >> delay; 
  tmb_->SetAlctTXclockDelay(delay);
  //
  input.str(tmb_table[ALCT_RX_CLOCK_DELAY_loc]);
  input >> delay; 
  tmb_->SetAlctRXclockDelay(delay);
  //
  input.str(tmb_table[TMB_L1ADELAY_loc]);
  input >> delay; 
  tmb_->SetL1aDelay(delay);
  //
  input.str(tmb_table[L1A_WINDOW_SIZE_loc]);
  input >> delay; 
  tmb_->SetL1aWindowSize(delay);
  //
  input.str(tmb_table[ALCT_MATCH_WINDOW_SIZE_loc]);
  input >> delay; 
  tmb_->SetAlctMatchWindowSize(delay);
  //
  input.str(tmb_table[ALCT_VPF_DELAY_loc]);
  input >> delay; 
  tmb_->SetAlctVpfDelay(delay);
  //
  input.str(tmb_table[MPC_DELAY_loc]);
  input >> delay; 
  tmb_->SetMpcDelay(delay);
  //
  input.str(tmb_table[FIFO_TBINS_loc]);
  input >> delay; 
  tmb_->SetFifoTbins(delay);
  //
  input.str(tmb_table[FIFO_PRETRIG_loc]);
  input >> delay; 
  tmb_->SetFifoPreTrig(delay);
  //
  input.str(tmb_table[MPC_TX_DELAY_loc]);
  input >> delay; 
  tmb_->SetMpcTXdelay(delay);
  //
  input.str(tmb_table[TRGMODE_loc]);
  input >> delay; 
  tmb_->SetTrgMode(delay);
  //
  input.str(tmb_table[RPC_BXN_OFFSET_loc]);
  input >> delay; 
  tmb_->SetFifoTbins(delay);
  //
  input.str(tmb_table[SHIFT_RPC_loc]);
  input >> delay; 
  tmb_->SetShiftRpc(delay);
  //
  input.str(tmb_table[REQUEST_L1A_loc]);
  input >> delay; 
  tmb_->SetFifoTbins(delay);
  //
  input.str(tmb_table[HS_PRETRIG_THRESH_loc]);
  input >> delay; 
  tmb_->SetHsPretrigThresh(delay);
  //
  input.str(tmb_table[DS_PRETRIG_THRESH_loc]);
  input >> delay; 
  tmb_->SetDsPretrigThresh(delay);
  //
  input.str(tmb_table[MIN_HITS_PATTERN_loc]);
  input >> delay; 
  tmb_->SetMinHitsPattern(delay);
  //
  input.str(tmb_table[DMB_TX_DELAY_loc]);
  input >> delay; 
  tmb_->SetDmbTxDelay(delay);
  //
  input.str(tmb_table[RAT_TMB_DELAY_loc]);
  input >> delay; 
  tmb_->SetRatTmbDelay(delay);
  //
  input.str(tmb_table[RPC0_RAT_DELAY_loc]);
  input >> delay; 
  tmb_->SetRpc0RatDelay(delay);
  //
}
//
TStoreTMBParser::~TStoreTMBParser(){
  //
  //
}
//
