//-----------------------------------------------------------------------
// $Id: RAT.h,v 3.3 2007/01/31 16:49:51 rakness Exp $
// $Log: RAT.h,v $
// Revision 3.3  2007/01/31 16:49:51  rakness
// complete set of TMB/ALCT/RAT xml parameters
//
// Revision 3.2  2006/07/23 14:04:06  rakness
// encapsulate RAT, update configure()
//
// Revision 3.1  2006/07/22 16:12:36  rakness
// clean up RAT/add JTAG checking
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.7  2006/03/21 12:22:45  mey
// UPdate
//
// Revision 1.6  2006/03/20 16:04:19  rakness
// Moved RAT JTAG to EMUjtag
//
// Revision 1.5  2006/03/20 09:05:50  rakness
// move temperature reads to TMB/RAT
//
// Revision 1.4  2006/03/05 18:45:08  mey
// Update
//
// Revision 1.3  2006/02/10 16:40:44  rakness
// RAT delay scans implemented
//
// Revision 1.2  2006/02/03 16:33:52  rakness
// update with much more RAT functionality
//
// Revision 1.1  2006/02/01 18:33:13  mey
// UPdate
//-----------------------------------------------------------------------
#ifndef RAT_h
#define RAT_h

#include <string>
#include "EMUjtag.h"

class TMB;

class RAT : public EMUjtag
{
public:
  //
  RAT(TMB * );
  //RAT();
  ~RAT();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  //
  //---------------------------------
  // Parameters in the database...
  //---------------------------------
  // RPC-RAT phase:
  //---------------------------------
  void SetRpcRatDelay(int RPC,     // set Write values -> RPC   = [0-1]
		      int delay);  //                     delay = [0-12] (2ns steps)
  int  GetRpcRatDelay(int RPC);    // get Read values -> RPC = [0-1]
  //
  void PrintRpcRatDelay();       // print out Read values
  //
  void WriteRpcRatDelay();       // send Write values to RAT
  inline void ReadRpcRatDelay() { ReadRatUser1(); }
  //
  //
  //---------------------------------
  // RAT-TMB phase:
  //---------------------------------
  void SetRatTmbDelay(int delay);   // delay = [0-15] (2ns steps)     
  int GetRatTmbDelay();
  //
  void PrintRatTmbDelay();       // print out Read values
  //
  void WriteRatTmbDelay();
  void ReadRatTmbDelay();
  //
  //
  ////////////////////////////////////////////////
  // Useful methods to use the RAT class...
  ////////////////////////////////////////////////
  void CheckRATConfiguration();
  inline int GetRATConfigurationStatus() { return rat_configuration_status_; }
  //
  void configure();
  //
  void reset_parity_error_counter();
  //
  int ReadRATtempPCB();  
  int ReadRATtempHSink();  
  int ReadRATtCritPCB();  
  int ReadRATtCritHSink();    
  //
  ///////////////////////////////////
  // user and ID codes...
  ///////////////////////////////////
  void ReadRatIdCode();
  int GetRatIdCode(int device);    //device = [ChipLocationRatFpga,ChipLocationRatProm]
  //
  void ReadRatUserCode();
  int GetRatUserCode(int device);  //device = [ChipLocationRatFpga,ChipLocationRatProm]
  //
  inline int GetReadRatFirmwareDay()   { return read_rat_firmware_day_;   }
  inline int GetReadRatFirmwareMonth() { return read_rat_firmware_month_; }
  inline int GetReadRatFirmwareYear()  { return read_rat_firmware_year_;  }
  //
  /////////////////////////////////////
  // User1 Register (read register):
  /////////////////////////////////////
  int GetRatRpcParityErrorCounter(int RPC);  // get Read values -> RPC = [0-1]
  int GetRatRpcPdata(int RPC);    // RPC=[0-1] -> get last set of data latched by RAT to check for parity
  int GetRatRpcParityOk(int RPC); // RPC=[0-1] -> is parity OK for last set of data latched by RAT?
  int GetRatTxOk();
  int GetRatRxOk();
  int GetRatCriticalTempBit();  
  int GetRatParityOdd();  
  //
  void ReadRatUser1();
  void PrintRatUser1();
  //
  //
protected:
  //
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  int rat_configuration_status_;
  int rat_idcode_[2];
  int rat_usercode_[2];
  //
  int read_rat_firmware_day_;
  int read_rat_firmware_month_;
  int read_rat_firmware_year_;
  //
  void rpc_fpga_finished_();
  //
  int read_rpc_rat_delay_[4];
  int write_rpc_rat_delay_[4];
  int read_rat_tmb_delay_;
  int write_rat_tmb_delay_;
  //
  /////////////////////////////////////
  // USER1 Register (read register):
  /////////////////////////////////////
  int user1_value_[MAX_NUM_FRAMES];
  int rat_user1_length_;
  //
  int GetRat3dBusy_();
  int GetRat3dVerifyOk_();
  //
  void decodeRATUser1_();
  int read_rs_begin_;                
  int read_rs_version_;              
  int read_rs_monthday_;             
  int read_rs_year_;                 
  int read_rs_syncmode_;             
  int read_rs_posneg_;               
  int read_rs_loop_;                 
  int read_rs_rpc_en_;               
  int read_rs_clk_active_;           
  int read_rs_locked_tmb_;           
  int read_rs_locked_rpc0_;          
  int read_rs_locked_rpc1_;          
  int read_rs_locklost_tmb_;         
  int read_rs_locklost_rpc0_;        
  int read_rs_locklost_rpc1_;        
  int read_rs_txok_;                 
  int read_rs_rxok_;                 
  int read_rs_ntcrit_;               
  int read_rs_rpc_free_;             
  int read_rs_dsn_;                  
  int read_rs_dddoe_wr_;             
  int read_rs_ddd_wr_;               
  int read_rs_ddd_auto_;             
  int read_rs_ddd_start_;            
  int read_rs_ddd_busy_;             
  int read_rs_ddd_verify_ok_;        
  int read_rs_rpc0_parity_ok_;       
  int read_rs_rpc1_parity_ok_;       
  int read_rs_rpc0_cnt_perr_;
  int read_rs_rpc1_cnt_perr_;
  int read_rs_last_opcode_;         
  int read_rw_rpc_en_;              
  int read_rw_ddd_start_;           
  int read_rw_ddd_wr_;              
  int read_rw_dddoe_wr_;            
  int read_rw_perr_reset_;          
  int read_rw_parity_odd_;          
  int read_rw_perr_ignore_;         
  int read_rw_rpc_future_;   
  int read_rs_rpc0_pdata_;   
  int read_rs_rpc1_pdata_;           
  int read_rs_unused_;
  int read_rs_end_;
  //
  /////////////////////////////////////////////////////////////////////////////////////////////
  // USER2 Register (control register) - Only write to this register (reading is destructive)
  /////////////////////////////////////////////////////////////////////////////////////////////
  int user2_value_[MAX_NUM_FRAMES];
  int rat_user2_length_;
  //
  void SetPowerUpUser2Register_();
  //
  void SetRatParityOdd_(int on_or_off);         // on_or_off = ON or OFF
  void SetRatParityErrorIgnore_(int on_or_off); // on_or_off = ON or OFF
  //                           ON = Ignore data words with all 1's in parity error counter
  //                          OFF = Count these data words in the parity error counter 
  void SetRatRpcEnable_(int enable);
  void SetRat3dStart_(int on_or_off);           // on_or_off = ON or OFF
  void SetRat3dOutputEnable_(int enable);   
  void SetRatParityErrorReset_(int on_or_off);  // on_or_off = ON or OFF
  void SetRatRpcFuture_(int dummy);             // unused
  //
  void WriteRatUser2_();
  //
  void FillRatUser2_();
  int write_rw_rpc_en_;              
  int write_rw_ddd_start_;           
  int write_rw_ddd_wr_;              
  int write_rw_dddoe_wr_;            
  int write_rw_perr_reset_;          
  int write_rw_parity_odd_;          
  int write_rw_perr_ignore_;         
  int write_rw_rpc_future_;   
  //
};

#endif

