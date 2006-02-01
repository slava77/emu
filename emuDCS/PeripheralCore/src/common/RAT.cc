#include "RAT.h"
#include "TMB_constants.h"
#include "TMB_JTAG_constants.h"
//
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>

//
RAT::RAT(TMB * tmb) :
  tmb_(tmb)
{
  //
  MyOutput_ = &std::cout ;
  //
};
//
RAT::~RAT() {
  //
  //
}
//
void RAT::ReadRatUser1(){
  //
  (*MyOutput_) << "TMBTester: Read RAT USER1 JTAG (status register)" << std::endl;
  //
  int dummy;
  int read_data;
  int opcode,chip_id;
  //
  // First make sure the RPC FPGA is finished:
  read_data = tmb_->ReadRegister(rpc_cfg_adr);
  int rpc_done = (read_data >> 14) & 0x1;
  (*MyOutput_) << "RPC done = " << rpc_done << std::endl;

  //tmb_->set_jtag_address(TMB_ADR_BOOT);    
  tmb_->set_jtag_chain(RAT_CHAIN);         

  // **Take the chosen chain's TAP to RTI (via the chosen address)**
  tmb_->jtag_anystate_to_rti();    
  //
  const int register_length = 32;  
  int data[register_length];
  //
  int idcode[2];
  int write_data[MAX_FRAMES] = {};   //create fat 0 for writing to data registers
  //
  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_IDCODE;                  
    } else { 
      opcode = PROM_IDCODE;                  
    }
    tmb_->jtag_ir_dr(chip_id,opcode,write_data,register_length,data);
    //
    idcode[chip_id] = tmb_->bits_to_int(data,32,0);
  }
  //
  int usercode[2];
  for (chip_id=0; chip_id<=1; chip_id++){
    if (chip_id == 0) {
      opcode = VTX2_USERCODE;                  
    } else { 
      opcode = PROM_USERCODE;                  
    }
    tmb_->jtag_ir_dr(chip_id,opcode,write_data,register_length,data);
    //
    usercode[chip_id] = tmb_->bits_to_int(data,32,0);
  }
  //
  chip_id=0;
  opcode = VTX2_USR1;
  int user1_value[rat_user1_length];
  //
  tmb_->jtag_ir_dr(chip_id,opcode,write_data,rat_user1_length,user1_value);
  //
  int bit_array[7][32];
  int first,second;
  //
  int counter=0;
  for (first=0; first<7; first++) {
    for (second=0; second<32; second++) {
      bit_array[first][second] = user1_value[counter++];
    }
  }
  int rat_user1[7];
  //
  for (first=0; first<7; first++) 
    rat_user1[first] = tmb_->bits_to_int(bit_array[first],32,0);
  //
  (*MyOutput_) << "RAT PROM ID code = " << idcode[0] << std::endl;
  (*MyOutput_) << "RAT FPGA ID code = " << idcode[1] << std::endl;
  (*MyOutput_) << "RAT PROM User code = " << usercode[0] << std::endl;
  (*MyOutput_) << "RAT FPGA User code = " << usercode[1] << std::endl;
  //
  (*MyOutput_) << "RAT USER1 = ";
  for (first=0; first<7; first++) 
    (*MyOutput_) << rat_user1[first];
  (*MyOutput_) << std::endl;
  //
  decodeRATUser1(user1_value);
  //
  return;
}
//
void RAT::decodeRATUser1(int * data) {

  // ** parse the bit array from the USER1 data register
  // ** to print it out in a human readable form
  int counter=0;
  int i;

  int rs_begin_array[4];
  for (i=0;i<4;i++) 
    rs_begin_array[i] = data[counter++];
  int rs_begin = tmb_->bits_to_int(rs_begin_array,4,0);
  (*MyOutput_) << "Begin marker = " << rs_begin << std::endl;

  int rs_version_array[4];
  for (i=0;i<4;i++) 
    rs_version_array[i] = data[counter++];
  int rs_version = tmb_->bits_to_int(rs_version_array,4,0);
  (*MyOutput_) << "Version ID = " << rs_version << std::endl;

  int rs_monthday_array[16];
  for (i=0;i<16;i++) 
    rs_monthday_array[i] = data[counter++];
  int rs_monthday = tmb_->bits_to_int(rs_monthday_array,16,0);
  (*MyOutput_) << "Version Month/Day = " << rs_monthday << std::endl;

  int rs_year_array[16];
  for (i=0;i<16;i++) 
    rs_year_array[i] = data[counter++];
  int rs_year = tmb_->bits_to_int(rs_year_array,16,0);
  (*MyOutput_) << "Version Year = " << rs_year << std::endl;

  int rs_syncmode = data[counter++];
  (*MyOutput_) << "1-> 80MHz sync mode = " << rs_syncmode << std::endl;

  int rs_posneg = data[counter++];
  (*MyOutput_) << "1-> Latch 40MHz RPC data on posedge = " << rs_posneg << std::endl;

  int rs_loop = data[counter++];
  (*MyOutput_) << "1-> Loopback mode = " << rs_loop << std::endl;

  int rs_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rs_rpc_en_array[i] = data[counter++];
  int rs_rpc_en = tmb_->bits_to_int(rs_rpc_en_array,2,0);
  (*MyOutput_) << "RPC driver enabled = " << rs_rpc_en << std::endl;

  int rs_clk_active_array[2];
  for (i=0;i<2;i++) 
    rs_clk_active_array[i] = data[counter++];
  int rs_clk_active = tmb_->bits_to_int(rs_clk_active_array,2,0);
  (*MyOutput_) << "RPC direct clock status = " << rs_clk_active << std::endl;

  int rs_locked_tmb = data[counter++];
  (*MyOutput_) << "TMB DLL locked = " << rs_locked_tmb << std::endl;

  int rs_locked_rpc0 = data[counter++];
  (*MyOutput_) << "RPC0 DLL locked = " << rs_locked_rpc0 << std::endl;

  int rs_locked_rpc1 = data[counter++];
  (*MyOutput_) << "RPC1 DLL locked = " << rs_locked_rpc1 << std::endl;

  int rs_locklost_tmb = data[counter++];
  (*MyOutput_) << "TMB DLL lost lock = " << rs_locklost_tmb << std::endl;

  int rs_locklost_rpc0 = data[counter++];
  (*MyOutput_) << "RPC0 DLL lost lock = " << rs_locklost_rpc0 << std::endl;

  int rs_locklost_rpc1 = data[counter++];
  (*MyOutput_) << "RPC1 DLL lost lock = " << rs_locklost_rpc1 << std::endl;

  int rs_txok = data[counter++];
  (*MyOutput_) << "ALCT TX OK = " << rs_txok << std::endl;

  int rs_rxok = data[counter++];
  (*MyOutput_) << "ALCT RX OK = " << rs_rxok << std::endl;

  int rs_ntcrit = data[counter++];
  (*MyOutput_) << "Over Temperature Threshold = " << rs_ntcrit << std::endl;

  int rs_rpc_free = data[counter++];
  (*MyOutput_) << "rpc_free0 from TMB = " << rs_rpc_free << std::endl;

  int rs_dsn = data[counter++];
  (*MyOutput_) << "rpc_dsn to TMB = " << rs_dsn << std::endl;

  int rs_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rs_dddoe_wr_array[i] = data[counter++];
  int rs_dddoe_wr = tmb_->bits_to_int(rs_dddoe_wr_array,4,0);
  (*MyOutput_) << "DDD status:  output enables = " << rs_dddoe_wr << std::endl;

  int rs_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rs_ddd_wr_array[i] = data[counter++];
  int rs_ddd_wr = tmb_->bits_to_int(rs_ddd_wr_array,16,0);
  (*MyOutput_) << "DDD status:  delay values = " << rs_ddd_wr << std::endl;

  int rs_ddd_auto = data[counter++];
  (*MyOutput_) << "1-> start DDD on power-up = " << rs_ddd_auto << std::endl;

  int rs_ddd_start = data[counter++];
  (*MyOutput_) << "DDD status: start ddd machine  = " << rs_ddd_start << std::endl;

  int rs_ddd_busy = data[counter++];
  (*MyOutput_) << "DDD status: state machine busy = " << rs_ddd_busy << std::endl;

  int rs_ddd_verify_ok = data[counter++];
  (*MyOutput_) << "DDD status: data readback OK = " << rs_ddd_verify_ok << std::endl;

  int rs_rpc0_parity_ok = data[counter++];
  (*MyOutput_) << "RPC0 parity OK currently = " << rs_rpc0_parity_ok << std::endl;

  int rs_rpc1_parity_ok = data[counter++];
  (*MyOutput_) << "RPC1 parity OK currently = " << rs_rpc1_parity_ok << std::endl;

  int rs_rpc0_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc0_cnt_perr_array[i] = data[counter++];
  int rs_rpc0_cnt_perr = tmb_->bits_to_int(rs_rpc0_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC0 parity error counter = " << rs_rpc0_cnt_perr << std::endl;

  int rs_rpc1_cnt_perr_array[16];
  for (i=0;i<16;i++) 
    rs_rpc1_cnt_perr_array[i] = data[counter++];
  int rs_rpc1_cnt_perr = tmb_->bits_to_int(rs_rpc1_cnt_perr_array,16,0);
  (*MyOutput_) << "RPC1 parity error counter = " << rs_rpc1_cnt_perr << std::endl;

  int rs_last_opcode_array[5];
  for (i=0;i<5;i++) 
    rs_last_opcode_array[i] = data[counter++];
  int rs_last_opcode = tmb_->bits_to_int(rs_last_opcode_array,5,0);
  (*MyOutput_) << "Last firmware TAP cmd opcode = " << rs_last_opcode << std::endl;


  int rw_rpc_en_array[2];
  for (i=0;i<2;i++) 
    rw_rpc_en_array[i] = data[counter++];
  int rw_rpc_en = tmb_->bits_to_int(rw_rpc_en_array,2,0);
  (*MyOutput_) << "rw_rpc_en = " << rw_rpc_en << std::endl;

  int rw_ddd_start = data[counter++];
  (*MyOutput_) << "rw_ddd_start  = " << rw_ddd_start << std::endl;

  int rw_ddd_wr_array[16];
  for (i=0;i<16;i++) 
    rw_ddd_wr_array[i] = data[counter++];
  int rw_ddd_wr = tmb_->bits_to_int(rw_ddd_wr_array,16,0);
  (*MyOutput_) << "rw_ddd_wr = " << rw_ddd_wr << std::endl;

  int rw_dddoe_wr_array[4];
  for (i=0;i<4;i++) 
    rw_dddoe_wr_array[i] = data[counter++];
  int rw_dddoe_wr = tmb_->bits_to_int(rw_dddoe_wr_array,4,0);
  (*MyOutput_) << "rw_dddoe_wr = " << rw_dddoe_wr << std::endl;

  int rw_perr_reset = data[counter++];
  (*MyOutput_) << "rw_perr_reset = " << rw_perr_reset << std::endl;

  int rw_parity_odd = data[counter++];
  (*MyOutput_) << "rw_parity_odd = " << rw_parity_odd << std::endl;

  int rw_perr_ignore = data[counter++];
  (*MyOutput_) << "rw_perr_ignore = " << rw_perr_ignore << std::endl;

  int rw_rpc_future_array[6];
  for (i=0;i<6;i++) 
    rw_rpc_future_array[i] = data[counter++];
  int rw_rpc_future = tmb_->bits_to_int(rw_rpc_future_array,6,0);
  (*MyOutput_) << "rw_rpc_future = " << rw_rpc_future << std::endl;


  int rs_rpc0_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc0_pdata_array[i] = data[counter++];
  int rs_rpc0_pdata = tmb_->bits_to_int(rs_rpc0_pdata_array,19,0);
  (*MyOutput_) << "RPC0 data (includes 16 pad bits + 3bxn) = " << rs_rpc0_pdata << std::endl;

  int rs_rpc1_pdata_array[19];
  for (i=0;i<19;i++) 
    rs_rpc1_pdata_array[i] = data[counter++];
  int rs_rpc1_pdata = tmb_->bits_to_int(rs_rpc1_pdata_array,19,0);
  (*MyOutput_) << "RPC1 data (includes 16 pad bits + 3bxn) = " << rs_rpc1_pdata << std::endl;

  int rs_unused_array[29];
  for (i=0;i<29;i++) 
    rs_unused_array[i] = data[counter++];
  int rs_unused = tmb_->bits_to_int(rs_unused_array,29,0);
  (*MyOutput_) << "Unused bits = " << rs_unused << std::endl;

  int rs_end_array[4];
  for (i=0;i<4;i++) 
    rs_end_array[i] = data[counter++];
  int rs_end = tmb_->bits_to_int(rs_end_array,4,0);
  (*MyOutput_) << "End marker = " << rs_end << std::endl;

return;
}
//
