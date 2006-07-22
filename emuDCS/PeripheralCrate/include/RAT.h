//-----------------------------------------------------------------------
// $Id: RAT.h,v 3.1 2006/07/22 16:12:36 rakness Exp $
// $Log: RAT.h,v $
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
  friend class TMBParser;
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  //
  ////////////////////////////////////////////////
  // Useful methods to use the RAT class...
  ////////////////////////////////////////////////
  void configure();
  //
  // Things to set from database:
  void SetRpcRatDelay(int RPC,     // set Write values -> RPC   = [0-1]
		      int delay);  //                     delay = [0-12] (2ns steps)
  int  GetRpcRatDelay(int RPC);    // get Read values -> RPC = [0-1]
  //
  void PrintRpcRatDelay();       // print out Read values
  void WriteRpcRatDelay();       // send Write values to RAT
  //
  void set_rattmb_delay(int delay);   // delay = [0-12] (2ns steps)     
  void read_rattmb_delay();
  inline int GetRatTmbDelay() { return rat_tmb_delay_ ; }
  //
  //
  void reset_parity_error_counter();
  int GetRatRpcParityErrorCounter(int RPC);  // get Read values -> RPC = [0-1]
  //
  int ReadRATtempPCB();  
  int ReadRATtempHSink();  
  int ReadRATtCritPCB();  
  int ReadRATtCritHSink();    
  //
  // the following should become private very soon:
  void SetRatParityOdd(int on_or_off);         // on_or_off = ON or OFF
  void SetRatParityErrorIgnore(int on_or_off); // on_or_off = ON or OFF
  //    ON = Ignore data words with all 1's in parity error counter
  //   OFF = Count these data words in the parity error counter 
  //
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
  //
  /////////////////////////////////////
  // User1 Register (read register):
  /////////////////////////////////////
  // The following are copies (read-only) of the User2 control register:
  inline int GetRatRpcEnable() { return read_rs_rpc_en_; }               
  inline int GetRat3dStart() { return read_rs_ddd_start_; }            
  inline int GetRat3dOutputEnable() { return read_rs_dddoe_wr_; }             
  inline int GetRatParityErrorReset() { return read_rw_perr_reset_; }    
  inline int GetRatParityOdd() { return read_rw_parity_odd_; }           
  inline int GetRatParityErrorIgnore() { return read_rw_perr_ignore_; }  
  inline int GetRatRpcFuture() { return read_rw_rpc_future_; }               
  //
  // Other information available from RAT User1 register:
  inline int GetRatBegin() { return read_rs_begin_; }
  inline int GetRatVersion() { return read_rs_version_; }
  inline int GetRatMonthDay() { return read_rs_monthday_; }             
  inline int GetRatYear() { return read_rs_year_; }                 
  inline int GetRatSyndMode() { return read_rs_syncmode_; }             
  inline int GetRatPosNeg() { return read_rs_posneg_; }               
  inline int GetRatLoop() { return read_rs_loop_; }                 
  inline int GetRatClockActive() { return read_rs_clk_active_; }           
  inline int GetRatLockedTmb() { return read_rs_locked_tmb_; }           
  inline int GetRatLockedRpc0() { return read_rs_locked_rpc0_; }          
  inline int GetRatLockedRpc1() { return read_rs_locked_rpc1_; }          
  inline int GetRatLockLostTmb() { return read_rs_locklost_tmb_; }         
  inline int GetRatLockLostRpc0() { return read_rs_locklost_rpc0_; }        
  inline int GetRatLockLostRpc1() { return read_rs_locklost_rpc1_; }        
  inline int GetRatTxOk() { return read_rs_txok_; }                 
  inline int GetRatRxOk() { return read_rs_rxok_; }                 
  inline int GetRatCriticalTempBit() { return read_rs_ntcrit_; }               
  inline int GetRatRpcFree() { return read_rs_rpc_free_; }             
  inline int GetRatDsn() { return read_rs_dsn_; }                  
  inline int GetRat3dAuto() { return read_rs_ddd_auto_; }             
  inline int GetRat3dBusy() { return read_rs_ddd_busy_; }             
  inline int GetRat3dVerifyOk() { return read_rs_ddd_verify_ok_; }        
  inline int GetRatLastOpcode() { return read_rs_last_opcode_; }         
  inline int GetRatUnused() { return read_rs_unused_; }
  inline int GetRatEnd() { return read_rs_end_; }
  //
  int GetRatRpcPdata(int RPC);    // RPC=[0-1] -> get last set of data latched by RAT to check for parity
  int GetRatRpcParityOk(int RPC); // RPC=[0-1] -> is parity OK for last set of data latched by RAT?
  //
  //
  void ReadRatUser1();
  void PrintRatUser1();
  //
  //
protected:
  //
  int rat_tmb_delay_;
  //
private:
  //
  std::ostream * MyOutput_ ;
  TMB * tmb_ ;
  //
  int rat_idcode_[2];
  int rat_usercode_[2];
  //
  void rpc_fpga_finished_();
  //
  /////////////////////////////////////
  // USER1 Register:
  /////////////////////////////////////
  int user1_value_[MAX_NUM_FRAMES];
  int rat_user1_length_;
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
  int read_rpc_rat_delay_[4];
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
  void SetRatRpcEnable_(int enable);
  void SetRat3dStart_(int on_or_off);           // on_or_off = ON or OFF
  void SetRat3dOutputEnable_(int enable);   
  void SetRatParityErrorReset_(int on_or_off);  // on_or_off = ON or OFF
  void SetRatRpcFuture_(int dummy);             // unused
  //
  void SetPowerUpUser2Register_();
  //
  void FillRatUser2_();
  void WriteRatUser2_();
  //
  int user2_value_[MAX_NUM_FRAMES];
  int rat_user2_length_;
  //
  int write_rw_rpc_en_;              
  int write_rw_ddd_start_;           
  int write_rw_ddd_wr_;              
  int write_rpc_rat_delay_[4];
  int write_rw_dddoe_wr_;            
  int write_rw_perr_reset_;          
  int write_rw_parity_odd_;          
  int write_rw_perr_ignore_;         
  int write_rw_rpc_future_;   
  //
  int rpc_parity_err_ctr_[2];
  int rpc_data_[2];
  int rpc_parity_ok_[2];
  int rpc_parity_used_ ; 
};

#endif

