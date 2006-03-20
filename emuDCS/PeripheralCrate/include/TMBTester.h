#ifndef TMBTester_h
#define TMBTester_h

#include<iostream>
#include<fstream>
#include<stdio.h>
#include <string>
#include "TMB.h"
#include "CCB.h"
#include "RAT.h"
#include "EMUjtag.h"

class TMBTester {
 public:
  // not responsible for deleting pointers
  TMBTester();
  //
  virtual ~TMBTester();
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  inline void setTMB(TMB * tmb) {tmb_ = tmb;}
  inline void setCCB(CCB * ccb) {ccb_ = ccb;}
  inline void setRAT(RAT * rat) {rat_ = rat;}
  inline void setJTAG(EMUjtag * jtag) {EMUjtag_ = jtag;}
  //
  inline int GetResultTestBootRegister() { return ResultTestBootRegister_ ; }
  inline int SetResultTestBootRegister(int result) { ResultTestBootRegister_ = result ; }
  //
  inline int GetResultTestVMEfpgaDataRegister() { return ResultTestVMEfpgaDataRegister_ ; }
  inline int SetResultTestVMEfpgaDataRegister(int result) { ResultTestVMEfpgaDataRegister_ = result ; }
  //
  inline int GetResultTestFirmwareDate() { return ResultTestFirmwareDate_ ; }
  inline int SetResultTestFirmwareDate(int result) { return ResultTestFirmwareDate_ = result ; }
  //
  inline int GetResultTestFirmwareType() { return ResultTestFirmwareType_ ; }
  inline int SetResultTestFirmwareType(int result) { ResultTestFirmwareType_ = result ; }
  //
  inline int GetResultTestFirmwareVersion() { return ResultTestFirmwareVersion_ ; }
  inline int SetResultTestFirmwareVersion(int result) { ResultTestFirmwareVersion_ = result ; }
  //
  inline int GetResultTestFirmwareRevCode() { return ResultTestFirmwareRevCode_ ; }
  inline int SetResultTestFirmwareRevCode(int result) { ResultTestFirmwareRevCode_ = result ; }
  //
  inline int GetResultTestMezzId() { return ResultTestMezzId_ ; }
  inline int SetResultTestMezzId(int result) { ResultTestMezzId_ = result ; }
  //
  inline int GetResultTestPromId() { return ResultTestPromId_ ; }
  inline int SetResultTestPromId(int result) { ResultTestPromId_ = result ; }
  //
  inline int GetResultTestPROMPath() { return ResultTestPROMPath_ ; }
  inline int SetResultTestPROMPath(int result) { ResultTestPROMPath_ = result ; }
  //
  inline int GetResultTestDSN() { return ResultTestDSN_ ; }
  inline int SetResultTestDSN(int result) { ResultTestDSN_ = result ; }
  //
  inline int GetResultTestADC() { return ResultTestADC_ ; }
  inline int SetResultTestADC(int result) { ResultTestADC_ = result ; }
  //
  inline int GetResultTest3d3444() { return ResultTest3d3444_ ; }
  inline int SetResultTest3d3444(int result) { ResultTest3d3444_ = result ; }
  //
  inline int GetResultTestALCTtxrx() { return ResultTestALCTtxrx_ ; }
  inline int SetResultTestALCTtxrx(int result) { ResultTestALCTtxrx_ = result ; }
  //
  inline int GetResultTestRATtemper() { return ResultTestRATtemper_ ; }
  inline int SetResultTestRATtemper(int result) { ResultTestRATtemper_ = result; }
  //
  inline int GetResultTestRATidCodes() { return ResultTestRATidCodes_ ; }
  inline int SetResultTestRATidCodes(int result) { ResultTestRATidCodes_ = result; }
  //
  inline int GetResultTestRATuserCodes() { return ResultTestRATuserCodes_ ; }
  inline int SetResultTestRATuserCodes(int result) { ResultTestRATuserCodes_ = result; }
  //
  void reset();
  //
  bool runAllTests();
  bool testFirmwareDate();
  bool testFirmwareType();
  bool testFirmwareVersion();
  bool testFirmwareSlot();
  bool testFirmwareRevCode();
  bool testBootRegister();
  bool testHardReset();
  bool testVMEfpgaDataRegister();
  bool testJTAGchain();    // check user && boot
  bool testJTAGchain(int); // user=0, boot=1
  bool testMezzId();
  bool testPROMid();
  bool testPROMpath();
  bool testDSN(); // check TMB && mezzanine && RAT
  bool testDSN(int); // TMB=0, mezzanine=1, RAT=2
  bool testADC();
  bool test3d3444();
  bool testALCTtxrx();      
  bool testRATtemper();      
  bool testRATidCodes();
  bool testRATuserCodes();
  //
  bool compareValues(std::string, int, int, bool);
  bool compareValues(std::string, float, float, float);
  void messageOK(std::string,bool);
  //
  // Maybe should be in CrateUtilities.cc?
  void RatTmbDelayScan();
  void RpcRatDelayScan(int);
  void window_analysis(int *,const int);
  void computeBER(int);
  //
  // Should be in RAT.cc (after smb_io(int,int,int) moves to TMB.cc)
  void rpc_fpga_finished();
  void ReadRatUser1();
  void decodeRATUser1();
  inline int * GetRatUser1() { return user1_value_ ; }
  void ReadRatIdCode();
  inline int * GetRatIdCode() { return rat_idcode_ ; }
  void ReadRatUserCode();
  inline int * GetRatUserCode() { return rat_usercode_ ; }
  void ReadRatUser2();
  void decodeRATUser2();
  inline int * GetRatUser2() { return user2_value_ ; }
  void set_rpcrat_delay(int,int);     // (rpc,delay)
  void read_rpcrat_delay();
  inline int GetRpcRatDelay() { return rpc_rat_delay_ ; }
  void set_rattmb_delay(int);     
  void read_rattmb_delay();
  inline int GetRatTmbDelay() { return rat_tmb_delay_ ; }
  void reset_parity_error_counter();
  void read_rpc_parity_error_counter();
  inline int GetRpcParityErrorCounter(int rpc) { return rpc_parity_err_ctr_[rpc] ; }
  void read_rpc_data();
  inline int GetRpcData(int rpc) { return rpc_data_[rpc] ; }
  inline int GetRpcParityOK(int rpc) { return rpc_parity_ok_[rpc] ; }
  void set_perr_ignore();
  void unset_perr_ignore();
  void use_parity_odd();
  void use_parity_even();
  void ReadRpcParity();
  inline int GetRpcParityUsed() { return rpc_parity_used_ ; }
  //
  //Some useful routines....
  void bit_to_array(int,int *,const int);
  void RpcComputeParity(int);
  //
  //HACK until TMB has correct chip:
  void jtag_src_boot_reg();
  //
 protected:
  // Should be in RAT.cc (after smb_io(int,int,int) moves to TMB.cc)
  int rat_tmb_delay_;
  //
 private: 
  std::ostream * MyOutput_ ;
  TMB * tmb_;
  CCB * ccb_;
  RAT * rat_;
  EMUjtag * EMUjtag_;
  //
  int ResultTestBootRegister_ ;
  int ResultTestVMEfpgaDataRegister_ ;
  int ResultTestFirmwareDate_;
  int ResultTestFirmwareType_;
  int ResultTestFirmwareVersion_;
  int ResultTestFirmwareSlot_;
  int ResultTestFirmwareRevCode_;
  int ResultTestMezzId_;
  int ResultTestPromId_;
  int ResultTestPROMPath_;
  int ResultTestDSN_;
  int ResultTestADC_;
  int ResultTest3d3444_;
  int ResultTestALCTtxrx_;
  int ResultTestRATtemper_;
  int ResultTestRATidCodes_;
  int ResultTestRATuserCodes_;
  //
  //functions needed by above tests:
  int dowCRC(std::bitset<64>);
  //
  // Should be in RAT.cc (after smb_io(int,int,int) moves to TMB.cc)
  int user1_value_[MAX_FRAMES];
  int rat_user1_length_;
  int rat_idcode_[2];
  int rat_usercode_[2];
  void WriteRatUser2_(int*);
  int user2_value_[MAX_FRAMES];
  int rat_user2_length_;
  int rpc_rat_delay_;
  int rpc_parity_err_ctr_[2];
  int rpc_data_[2];
  int rpc_parity_ok_[2];
  int rpc_parity_used_ ; 
  //
};

#endif

