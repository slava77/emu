#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>
//
#include "RAT.h"
#include "TMB.h"
#include "TMB_constants.h"
//
//RAT::RAT() :
//  EMUjtag()
//{
//}
//
RAT::RAT(TMB * tmb) :
  EMUjtag(tmb)
{
  //
  tmb_ = tmb;
  MyOutput_ = &std::cout ;
  //
};
//
RAT::~RAT() {
  //
  //
}
//
void RAT::rpc_fpga_finished() {
  // Test if the RPC FPGA is finished

  int rpc_done = 0;
  int read_data;
  int counter = 0;
  while (!rpc_done) {
    read_data = tmb_->ReadRegister(rpc_cfg_adr);
    rpc_done = (read_data >> 14) & 0x1;
    counter++;
    if (counter % 1000 == 0) 
      (*MyOutput_) << "Waiting for RPC FPGA to be finished...";
    if (counter > 100000) {
      (*MyOutput_) << "RAT must not be plugged in..." << std::endl;
      (*MyOutput_) << "**********************************" << std::endl;
      (*MyOutput_) << "**** RAT is not going to work ****" << std::endl;
      (*MyOutput_) << "**********************************" << std::endl;
      rpc_done = 1;
    }
  }
  return;
}
//
void RAT::ReadRatIdCode(){
  (*MyOutput_) << "RAT: Read RAT ID Codes" << std::endl;
  //
  rpc_fpga_finished();
  //
  setup_jtag(ChainRat);
  //
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAidCode,
	      RegSizeRatFpga_FPGAidCode);
  rat_idcode_[0] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  //
  //
  ShfIR_ShfDR(ChipLocationRatProm,
	      PROMidCode,
	      RegSizeRatProm_PROMidCode);
  rat_idcode_[1] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  //
  (*MyOutput_) << "RAT FPGA ID code = " << rat_idcode_[0] << std::endl;
  (*MyOutput_) << "RAT PROM ID code = " << rat_idcode_[1] << std::endl;
  //
  return;
}
//
void RAT::ReadRatUserCode(){
  (*MyOutput_) << "RAT: Read RAT User Codes" << std::endl;
  //
  rpc_fpga_finished();
  //
  setup_jtag(ChainRat);
  //
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAuserCode,
	      RegSizeRatFpga_FPGAuserCode);
  rat_usercode_[0] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  //
  //
  ShfIR_ShfDR(ChipLocationRatProm,
	      PROMuserCode,
	      RegSizeRatProm_PROMuserCode);
  rat_usercode_[1] = bits_to_int(GetDRtdo(),GetRegLength(),0);
  //
  (*MyOutput_) << "RAT FPGA User code = " << rat_usercode_[0] << std::endl;
  (*MyOutput_) << "RAT PROM User code = " << rat_usercode_[1] << std::endl;
  //
  return;
}
//
void RAT::ReadRatUser1(){
  (*MyOutput_) << "RAT: Read RAT USER1 JTAG (status register)" << std::endl;
  //
  rpc_fpga_finished();
  //
  //clear user1_value_
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    user1_value_[i] = 0;
  //
  setup_jtag(ChainRat);
  //	       
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAuser1,
	      RegSizeRatFpga_FPGAuser1);
  //
  //Fill user1_value_ with JTAG data...
  rat_user1_length_ = GetRegLength();
  int * user1_pointer = GetDRtdo();
  //
  for (int i=0; i<rat_user1_length_; i++) 
    user1_value_[i] = *(user1_pointer+i);
  //  
  //  
  //Print out the USER1 value from right (first bit out) to left (last bit out):
  char rat_user1[rat_user1_length_/8];
  packCharBuffer(user1_value_,rat_user1_length_,rat_user1);
  //
  (*MyOutput_) << "RAT USER1 = ";
  for (int counter=(rat_user1_length_/8)-1; counter>=0; counter--) 
    (*MyOutput_) << ((rat_user1[counter] >> 4) & 0xf) << (rat_user1[counter] & 0xf);
  (*MyOutput_) << std::endl;
  //
  //  decodeRATUser1();
  //
  return;
}
//
void RAT::decodeRATUser1() {
  // ** parse the bit array from the USER1 data register
  // ** to print it out in a human readable form
  int counter=0;
  int i;
  //
  int rs_begin_array[4];
  for (i=0;i<4;i++) 
    rs_begin_array[i] = user1_value_[counter++];
  int rs_begin = bits_to_int(rs_begin_array,4,0);
  (*MyOutput_) << "Begin marker = " << rs_begin << std::endl;
  //
  int rs_version_array[4];
  for (i=0;i<4;i++) 
    rs_version_array[i] = user1_value_[counter++];
  int rs_version = bits_to_int(rs_version_array,4,0);
  (*MyOutput_) << "Version ID = " << rs_version << std::endl;
  //
  int rs_monthday_array[16];
  for (i=0;i<16;i++) 
    rs_monthday_array[i] = user1_value_[counter++];
  int rs_monthday = bits_to_int(rs_monthday_array,16,0);
  (*MyOutput_) << "Version Month/Day = " << rs_monthday << std::endl;
  //
  int rs_year_array[16];
  for (i=0;i<16;i++) 
    rs_year_array[i] = user1_value_[counter++];
  int rs_year = bits_to_int(rs_year_array,16,0);
  (*MyOutput_) << "Version Year = " << rs_year << std::endl;
  //
  int rs_syncmode = user1_value_[counter++];
  (*MyOutput_) << "1-> 80MHz sync mode = " << rs_syncmode << std::endl;
  //
  int rs_posneg = user1_value_[counter++];
  (*MyOutput_) << "1-> Latch 40MHz RPC data on posedge = " << rs_posneg << std::endl;
  //
  int rs_loop = user1_value_[counter++];
  (*MyOutput_) << "1-> Loopback mode = " << rs_loop << std::endl;
  //
  int rs_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rs_rpc_en_array[i] = user1_value_[counter++];
  int rs_rpc_en = bits_to_int(rs_rpc_en_array,2,0);
  (*MyOutput_) << "RPC driver enabled = " << rs_rpc_en << std::endl;
  //
  int rs_clk_active_array[2];
  for (i=0;i<2;i++) 
    rs_clk_active_array[i] = user1_value_[counter++];
  int rs_clk_active = bits_to_int(rs_clk_active_array,2,0);
  (*MyOutput_) << "RPC direct clock status = " << rs_clk_active << std::endl;
  //
  int rs_locked_tmb = user1_value_[counter++];
  (*MyOutput_) << "TMB DLL locked = " << rs_locked_tmb << std::endl;
  //
  int rs_locked_rpc0 = user1_value_[counter++];
  (*MyOutput_) << "RPC0 DLL locked = " << rs_locked_rpc0 << std::endl;
  //
  int rs_locked_rpc1 = user1_value_[counter++];
  (*MyOutput_) << "RPC1 DLL locked = " << rs_locked_rpc1 << std::endl;
  //
  int rs_locklost_tmb = user1_value_[counter++];
  (*MyOutput_) << "TMB DLL lost lock = " << rs_locklost_tmb << std::endl;
  //
  int rs_locklost_rpc0 = user1_value_[counter++];
  (*MyOutput_) << "RPC0 DLL lost lock = " << rs_locklost_rpc0 << std::endl;
  //
  int rs_locklost_rpc1 = user1_value_[counter++];
  (*MyOutput_) << "RPC1 DLL lost lock = " << rs_locklost_rpc1 << std::endl;
  //
  int rs_txok = user1_value_[counter++];
  (*MyOutput_) << "ALCT TX OK = " << rs_txok << std::endl;
  //
  int rs_rxok = user1_value_[counter++];
  (*MyOutput_) << "ALCT RX OK = " << rs_rxok << std::endl;
  //
  int rs_ntcrit = user1_value_[counter++];
  (*MyOutput_) << "Over Temperature Threshold = " << rs_ntcrit << std::endl;
  //
  int rs_rpc_free = user1_value_[counter++];
  (*MyOutput_) << "rpc_free0 from TMB = " << rs_rpc_free << std::endl;
  //
  int rs_dsn = user1_value_[counter++];
  (*MyOutput_) << "rpc_dsn to TMB = " << rs_dsn << std::endl;
  //
  int rs_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rs_dddoe_wr_array[i] = user1_value_[counter++];
  int rs_dddoe_wr = bits_to_int(rs_dddoe_wr_array,4,0);
  (*MyOutput_) << "DDD status:  output enables = " << rs_dddoe_wr << std::endl;
  //
  int rs_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rs_ddd_wr_array[i] = user1_value_[counter++];
  int rs_ddd_wr = bits_to_int(rs_ddd_wr_array,16,0);
  (*MyOutput_) << "DDD status:  delay values = " << rs_ddd_wr << std::endl;
  //
  int rs_ddd_auto = user1_value_[counter++];
  (*MyOutput_) << "1-> start DDD on power-up = " << rs_ddd_auto << std::endl;
  //
  int rs_ddd_start = user1_value_[counter++];
  (*MyOutput_) << "DDD status: start ddd machine  = " << rs_ddd_start << std::endl;
  //
  int rs_ddd_busy = user1_value_[counter++];
  (*MyOutput_) << "DDD status: state machine busy = " << rs_ddd_busy << std::endl;
  //
  int rs_ddd_verify_ok = user1_value_[counter++];
  (*MyOutput_) << "DDD status: data readback OK = " << rs_ddd_verify_ok << std::endl;
  //
  int rs_rpc0_parity_ok = user1_value_[counter++];
  (*MyOutput_) << "RPC0 parity OK currently = " << rs_rpc0_parity_ok << std::endl;
  //
  int rs_rpc1_parity_ok = user1_value_[counter++];
  (*MyOutput_) << "RPC1 parity OK currently = " << rs_rpc1_parity_ok << std::endl;
  //
  int rs_rpc0_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc0_cnt_perr_array[i] = user1_value_[counter++];
  int rs_rpc0_cnt_perr = bits_to_int(rs_rpc0_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC0 parity error counter = " << rs_rpc0_cnt_perr << std::endl;
  //
  int rs_rpc1_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc1_cnt_perr_array[i] = user1_value_[counter++];
  int rs_rpc1_cnt_perr = bits_to_int(rs_rpc1_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC1 parity error counter = " << rs_rpc1_cnt_perr << std::endl;
  //
  int rs_last_opcode_array[5];
  for (i=0;i<5;i++) 
    rs_last_opcode_array[i] = user1_value_[counter++];
  int rs_last_opcode = bits_to_int(rs_last_opcode_array,5,0);
  (*MyOutput_) << "Last firmware TAP cmd opcode = " << rs_last_opcode << std::endl;
  //
  int rw_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rw_rpc_en_array[i] = user1_value_[counter++];
  int rw_rpc_en = bits_to_int(rw_rpc_en_array,2,0);
  (*MyOutput_) << "rw_rpc_en = " << rw_rpc_en << std::endl;
  //
  int rw_ddd_start = user1_value_[counter++];
  (*MyOutput_) << "rw_ddd_start  = " << rw_ddd_start << std::endl;
  //
  int rw_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rw_ddd_wr_array[i] = user1_value_[counter++];
  int rw_ddd_wr = bits_to_int(rw_ddd_wr_array,16,0);
  (*MyOutput_) << "rw_ddd_wr = " << rw_ddd_wr << std::endl;
  //
  int rw_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rw_dddoe_wr_array[i] = user1_value_[counter++];
  int rw_dddoe_wr = bits_to_int(rw_dddoe_wr_array,4,0);
  (*MyOutput_) << "rw_dddoe_wr = " << rw_dddoe_wr << std::endl;
  //
  int rw_perr_reset = user1_value_[counter++];
  (*MyOutput_) << "rw_perr_reset = " << rw_perr_reset << std::endl;
  //
  int rw_parity_odd = user1_value_[counter++];
  (*MyOutput_) << "rw_parity_odd = " << rw_parity_odd << std::endl;
  //
  int rw_perr_ignore = user1_value_[counter++];
  (*MyOutput_) << "rw_perr_ignore = " << rw_perr_ignore << std::endl;
  //
  int rw_rpc_future_array[6];
  for (i=0;i<6;i++) 
    rw_rpc_future_array[i] = user1_value_[counter++];
  int rw_rpc_future = bits_to_int(rw_rpc_future_array,6,0);
  (*MyOutput_) << "rw_rpc_future = " << rw_rpc_future << std::endl;
  //
  int rs_rpc0_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc0_pdata_array[i] = user1_value_[counter++];
  int rs_rpc0_pdata = bits_to_int(rs_rpc0_pdata_array,19,0);
  (*MyOutput_) << "RPC0 data (includes 16 pad bits + 3bxn) = " << rs_rpc0_pdata << std::endl;
  //
  int rs_rpc1_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc1_pdata_array[i] = user1_value_[counter++];
  int rs_rpc1_pdata = bits_to_int(rs_rpc1_pdata_array,19,0);
  (*MyOutput_) << "RPC1 data (includes 16 pad bits + 3bxn) = " << rs_rpc1_pdata << std::endl;
  //
  int rs_unused_array[29];
  for (i=0;i<29;i++) 
    rs_unused_array[i] = user1_value_[counter++];
  int rs_unused = bits_to_int(rs_unused_array,29,0);
  (*MyOutput_) << "Unused bits = " << rs_unused << std::endl;
  //
  int rs_end_array[4];
  for (i=0;i<4;i++) 
    rs_end_array[i] = user1_value_[counter++];
  int rs_end = bits_to_int(rs_end_array,4,0);
  (*MyOutput_) << "End marker = " << rs_end << std::endl;
  //
  return;
}
//
void RAT::ReadRatUser2(){
  (*MyOutput_) << "RAT: Read RAT USER2 JTAG (control register)" << std::endl;
  //
  rpc_fpga_finished();
  //
  //clear user2_value_
  for (int i=0; i<MAX_NUM_FRAMES; i++) 
    user2_value_[i] = 0;
  //
  setup_jtag(ChainRat);
  //
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAuser2,
	      RegSizeRatFpga_FPGAuser2);
  //
  //Fill user2_ information:
  rat_user2_length_ = GetRegLength();
  int * user2_pointer = GetDRtdo();
  //
  for (int i=0; i<rat_user2_length_; i++) 
    user2_value_[i] = *(user2_pointer+i);
  //
  //  (*MyOutput_) << "user2_value_ = ";
  //  for (int i=rat_user2_length_-1; i>=0; i--) (*MyOutput_) << user2_value_[i];
  //  (*MyOutput_) << std::endl;
  //
  //Put data back into User2 (readout was destructive)
  int rsd[rat_user2_length_];
  //
  for (int i=0; i<rat_user2_length_; i++) 
    rsd[i] = user2_value_[i];
  //
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAuser2,
	      RegSizeRatFpga_FPGAuser2,
	      rsd);
  //
  //Print out the USER2 value from right (first bit out) to left (last bit out):
  char rat_user2[rat_user2_length_/8];
  packCharBuffer(user2_value_,rat_user2_length_,rat_user2);
  //
  (*MyOutput_) << "RAT USER2 = ";
  for (int counter=(rat_user2_length_/8)-1; counter>=0; counter--) 
    (*MyOutput_) << ((rat_user2[counter] >> 4) & 0xf) << (rat_user2[counter] & 0xf);
  (*MyOutput_) << std::endl;
  //
  //  decodeRATUser2();
  //
  return;
}
//
void RAT::decodeRATUser2() {
  // ** parse the bit array from the USER2 data register
  // ** to print it out in a human readable form
  int i;
  int counter = 0;
  //
  int ws_rpc_en_array[2];
  for (i=0;i<2;i++) 
    ws_rpc_en_array[i] = user2_value_[counter++];
  int ws_rpc_en = bits_to_int(ws_rpc_en_array,2,0);
  //
  int ws_ddd_start = user2_value_[counter++];
  //
  int ws_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    ws_ddd_wr_array[i] = user2_value_[counter++];
  int ws_ddd_wr = bits_to_int(ws_ddd_wr_array,16,0);
  //
  int ws_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    ws_dddoe_wr_array[i] = user2_value_[counter++];
  int ws_dddoe_wr = bits_to_int(ws_dddoe_wr_array,4,0);
  //
  int ws_perr_reset = user2_value_[counter++];
  //
  int ws_parity_odd = user2_value_[counter++];
  //
  int ws_perr_ignore = user2_value_[counter++];
  //
  int ws_rpc_future_array[6];
  for (i=0;i<6;i++) 
    ws_rpc_future_array[i] = user2_value_[counter++];
  int ws_rpc_future = bits_to_int(ws_rpc_future_array,6,0);
  //
  (*MyOutput_) << "ws_rpc_en = " << ws_rpc_en << std::endl;
  (*MyOutput_) << "ws_ddd_start  = " << ws_ddd_start << std::endl;
  (*MyOutput_) << "ws_ddd_wr = " << ws_ddd_wr << std::endl;
  (*MyOutput_) << "ws_dddoe_wr = " << ws_dddoe_wr << std::endl;
  (*MyOutput_) << "ws_perr_reset = " << ws_perr_reset << std::endl;
  (*MyOutput_) << "ws_parity_odd = " << ws_parity_odd << std::endl;
  (*MyOutput_) << "ws_perr_ignore = " << ws_perr_ignore << std::endl;
  (*MyOutput_) << "ws_rpc_future = " << ws_rpc_future << std::endl;
  //
  return;
}
//
void RAT::WriteRatUser2_(int * data_in){
  (*MyOutput_) << "RAT: Write RAT USER2 JTAG (control register)" << std::endl;
  //
  rpc_fpga_finished();
  //
  setup_jtag(ChainRat);
  //	       
  ShfIR_ShfDR(ChipLocationRatFpga,
	      FPGAuser2,
	      RegSizeRatFpga_FPGAuser2,
	      data_in);
  //
  return;
}
//
//
void RAT::set_rpcrat_delay(int rpc,int delay) {
  //
  if ( rpc==0 || rpc==1 ) {
    if (delay >= 0 && delay <=12 ) {
      (*MyOutput_) << "Set RPC " << rpc << " delay = " << std::hex << delay << std::endl;
    } else {
      (*MyOutput_) << "only delay values 0-12 allowed" << std::endl;      
      return;
    }
  } else {
    (*MyOutput_) << "RPC " << rpc << " does not exist" << std::endl;
    return;
  }
  //
  ReadRatUser2();               // get the current user2_value_
  //
  delay &= 0x000f;              //strip off extraneous bits
  //
  int lowbit =  2 + rpc*4;      //lowest position of delay bit to set
  int highbit = lowbit+4;       //highest position of delay bit to set
  //
  int rsd[MAX_NUM_FRAMES];
  int bitcount = 0;
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==2) 
      rsd[i] = 0;                            // unstart state machine
    if (i>lowbit && i<=highbit ) {
      rsd[i] = (delay >> bitcount++) & 0x1;  // new delay values
    }
  }
  WriteRatUser2_(rsd);
  //
  // **Check that the DDD state machine went idle...**
  int ddd_busy = 1;
  while (ddd_busy != 0) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;

    if (ddd_busy != 0) {
      (*MyOutput_) << "ERROR: State machine stuck busy" << std::endl;
      ::sleep(1);
    }
  }
  //
  // **Start DDD state machine...**
  rsd[2] = 0x1;                         //keep all the other tdi data the same...
  WriteRatUser2_(rsd);
  //
  // **Check that the DDD state machine went busy...**
  ddd_busy = 0;
  while (ddd_busy != 1) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;
    //
    if (ddd_busy != 1) {
      (*MyOutput_) << "ERROR: State machine did not go busy" << std::endl;
      ::sleep(1);
    }
  }
  //
  // **Unstart DDD state machine again...**
  rsd[2] = 0x0;
  WriteRatUser2_(rsd);
  //
  // **Check that the DDD state machine is not busy...**
  ddd_busy = 1;
  while (ddd_busy != 0) {
    ReadRatUser1();
    ddd_busy = user1_value_[80] & 0x1;
    //
    if (ddd_busy != 0) {
      (*MyOutput_) << "ERROR: State machine is busy" << std::endl;
      ::sleep(1);
    }
  }
  //
  // **Check that the data made it OK...**
  int ddd_verify = user1_value_[81] & 0x1;
  //
  if (ddd_verify != 1) {
    (*MyOutput_) << "ERROR: DDD data not verified" << std::endl;
    ::sleep(1);
  }
  //
  // ** Print out the delay values for the RAT ddd chip: **
  read_rpcrat_delay();
  //
  return;
}
//
void RAT::read_rpcrat_delay(){
  (*MyOutput_) << "RAT: Read RPC-RAT delays" << std::endl;
  //
  ReadRatUser1();             //get the current USER1 values
  //
  int i;
  //
  int delays[16];
  int offset = 62;
  int bit = 0;
  for (i=offset; i<(offset+16); i++) 
    delays[bit++] = user1_value_[i];
  //
  rpc_rat_delay_ = bits_to_int(delays,16,0);
  (*MyOutput_) << "rpc_rat_delay_ = " << std::hex << rpc_rat_delay_ << std::endl;
  //
  int ddd_delay[4];
  for (i=0; i<4; i++) 
    ddd_delay[i] = (rpc_rat_delay_ >> i*4) & 0xf;
  //
  (*MyOutput_) << "RPC0 = " << std::hex << ddd_delay[0] << std::endl;
  (*MyOutput_) << "RPC1 = " << std::hex << ddd_delay[1] << std::endl;
  (*MyOutput_) << "RPC2 = NOT USED" << std::endl;
  (*MyOutput_) << "RPC3 = NOT USED" << std::endl;
  //
  return;
}
//
void RAT::set_rattmb_delay(int delay) {
  //
  if (delay < 0 || delay >15 ) {
    (*MyOutput_) << "only delay values 0-15 allowed" << std::endl;      
    return;
  }
  //
  tmb_->tmb_clk_delays(delay,8);
  //
  return;
}
//
void RAT::read_rattmb_delay() {
  rat_tmb_delay_ = tmb_->tmb_read_delays(8);
  //
  (*MyOutput_) << "RAT-TMB delay = " << std::hex << rat_tmb_delay_ << std::endl;      
  //
  return;
}
//
void RAT::set_perr_ignore(){
  (*MyOutput_) 
    << "RAT: Ignore All 1's and all 1's data words in parity error counter" 
    << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  int rsd[rat_user2_length_];
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==25) 
      rsd[i] = 1;                            // set perr_ignore bit
  }
  WriteRatUser2_(rsd);
  //
  return;
}
//
void RAT::unset_perr_ignore(){
  (*MyOutput_) 
    << "RAT: DO NOT ignore all 1's and all 1's data words in parity error counter" 
    << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  int rsd[rat_user2_length_];
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==25) 
      rsd[i] = 0;                            // set perr_ignore bit
  }
  WriteRatUser2_(rsd);
  //
  return;
}
//
void RAT::use_parity_odd() {
  (*MyOutput_) 
    << "RAT: Use odd parity in parity error counter" 
    << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  int rsd[rat_user2_length_];
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==24) 
      rsd[i] = 1;                            // set parity odd bit
  }
  WriteRatUser2_(rsd);
  //
  return;
}
//
void RAT::use_parity_even() {
  (*MyOutput_) 
    << "RAT: Use even parity in parity error counter" 
    << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  int rsd[rat_user2_length_];
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==24) 
      rsd[i] = 0;                            // set parity even bit
  }
  WriteRatUser2_(rsd);
  //
  return;
}
//
void RAT::ReadRpcParity() {
  (*MyOutput_) 
    << "RAT: Determine parity used in parity error counter" 
    << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  rpc_parity_used_ = user2_value_[24]; 
  //
  return;
}
//
void RAT::reset_parity_error_counter() {
  (*MyOutput_) << "RAT: Reset RPC parity error counter" << std::endl;
  //
  ReadRatUser2();                           // get the current user2_value_
  //
  int rsd[rat_user2_length_];
  //
  // ** Set the new RAT USER2 bits. ** 
  // ** Bits which are unchanged need to be explicitly put back**
  for (int i=0; i<rat_user2_length_; i++) {
    rsd[i]=user2_value_[i];
    if (i==23) 
      rsd[i] = 1;                            // assert parity reset
  }
  WriteRatUser2_(rsd);
  //
  rsd[23] = 0;                               // de-assert parity reset
  WriteRatUser2_(rsd);
  //
  return;
}
//
void RAT::read_rpc_parity_error_counter() {
  (*MyOutput_) << "RAT: Read RPC parity error counter" << std::endl;
  //
  ReadRatUser1();                                   //get the current USER1 value
  //
  int counter_array[2][16];
  for (int rpc=0; rpc<2; rpc++) {
    for (int i=0;i<16;i++) {
      counter_array[rpc][i] = user1_value_[84+16*rpc+i];
      rpc_parity_err_ctr_[rpc] = bits_to_int(counter_array[rpc],16,0);
    }
    (*MyOutput_) << "RPC " << rpc 
		 << " Parity error counter = " << rpc_parity_err_ctr_[rpc] 
		 << std::endl;  
  }
  //
  return;
}
//
void RAT::read_rpc_data() {
  (*MyOutput_) << "RAT: Read RPC data" << std::endl;

  ReadRatUser1();                                   //get the current USER1 value
  //
  int bit;
  int data_array[2][19];
  //
  for (int rpc=0; rpc<2; rpc++) {
    for (bit=0;bit<19;bit++) {
      data_array[rpc][bit] = user1_value_[153+19*rpc+bit];
      rpc_data_[rpc] = bits_to_int(data_array[rpc],19,0);
      rpc_parity_ok_[rpc] = user1_value_[82+rpc];
    }
    //** Count how many ones are in the word... **
    //** Whether the count is even or odd should be correlated with the rpc_parity_ok_[] bit **
    int counter = 0;
    for (bit=0;bit<19;bit++) 
      counter += (rpc_data_[rpc] >> bit) & 0x1;
    //
    (*MyOutput_) << "RPC " << rpc 
		 << " Data = " << rpc_data_[rpc] 
		 << " number of bits = " << counter 
		 << " -> Parity ok = " << rpc_parity_ok_[rpc] 
		 << std::endl;
  }
  //
  return;
}
//
int RAT::ReadRATtempPCB() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x00;   // "local" temperature read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT temperature (PCB)                 = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtempHSink() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x01;   // "remote" temperature read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT temperature (Heat Sink)           = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtCritPCB() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address  
  int command = 0x05;   // "local" temperature critical read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT Critical Temperature (PCB)        = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
//
int RAT::ReadRATtCritHSink() {
  //
  int smb_adr = 0x18;   // gnd, gnd state RAT LM84 chip address
  int command = 0x07;   // "remote" temperature critical read
  int temperature = tmb_->smb_io(smb_adr,command,2);
  //
  (*MyOutput_) << "RAT Critical Temperature (Heat Sink)  = " << std::dec << temperature
	       << " deg C " << std::endl;
  //
  return temperature;
}
