#ifndef TMBTester_h
#define TMBTester_h

#include<iostream>
#include<fstream>
#include<stdio.h>
#include <string>
#include <bitset>

class TMB;
class CCB;
class RAT;

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
};

#endif

