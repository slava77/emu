//-----------------------------------------------------------------------
// $Id: RAT.h,v 1.6 2006/03/20 16:04:19 rakness Exp $
// $Log: RAT.h,v $
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
  RAT();
  ~RAT();
  //
  friend class TMBParser;
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
  void rpc_fpga_finished();
  //
  void ReadRatUser1();
  void decodeRATUser1();
  inline int * GetRatUser1() { return user1_value_ ; }
  //
  void ReadRatIdCode();
  inline int * GetRatIdCode() { return rat_idcode_ ; }
  //
  void ReadRatUserCode();
  inline int * GetRatUserCode() { return rat_usercode_ ; }
  //
  void ReadRatUser2();
  void decodeRATUser2();
  inline int * GetRatUser2() { return user2_value_ ; }
  //
  void set_rpcrat_delay(int,int);     // (rpc,delay)
  void read_rpcrat_delay();
  inline int GetRpcRatDelay() { return rpc_rat_delay_ ; }
  //
  void set_rattmb_delay(int);     
  void read_rattmb_delay();
  inline int GetRatTmbDelay() { return rat_tmb_delay_ ; }
  //
  void reset_parity_error_counter();
  void read_rpc_parity_error_counter();
  inline int GetRpcParityErrorCounter(int rpc) { return rpc_parity_err_ctr_[rpc] ; }
  //
  void read_rpc_data();
  inline int GetRpcData(int rpc) { return rpc_data_[rpc] ; }
  inline int GetRpcParityOK(int rpc) { return rpc_parity_ok_[rpc] ; }
  //
  void set_perr_ignore();
  void unset_perr_ignore();
  //
  void use_parity_odd();
  void use_parity_even();
  void ReadRpcParity();
  inline int GetRpcParityUsed() { return rpc_parity_used_ ; }
  //
  int ReadRATtempPCB();  
  int ReadRATtempHSink();  
  int ReadRATtCritPCB();  
  int ReadRATtCritHSink();    
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
  int user1_value_[MAX_NUM_FRAMES];
  int rat_user1_length_;
  //
  int rat_idcode_[2];
  int rat_usercode_[2];
  //
  void WriteRatUser2_(int*);
  int user2_value_[MAX_NUM_FRAMES];
  int rat_user2_length_;
  //
  int rpc_rat_delay_;
  int rpc_parity_err_ctr_[2];
  int rpc_data_[2];
  int rpc_parity_ok_[2];
  int rpc_parity_used_ ; 
};

#endif

