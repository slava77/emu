#ifndef TMBTester_h
#define TMBTester_h

#include<iostream>
#include<fstream>
#include<stdio.h>
#include <string>
#include "TMB.h"
#include "CCB.h"
#include "TMB_JTAG_constants.h"

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
  inline void setTMBslot(int * slot) {TMBslot = slot;}
  //
  inline int GetResultTestBootRegister() { return ResultTestBootRegister_ ; }
  inline int GetResultTestVMEfpgaDataRegister() { return ResultTestVMEfpgaDataRegister_ ; }
  inline int GetResultTestFirmwareDate() { return ResultTestFirmwareDate_ ; }
  inline int GetResultTestFirmwareType() { return ResultTestFirmwareType_ ; }
  inline int GetResultTestFirmwareVersion() { return ResultTestFirmwareVersion_ ; }
  inline int GetResultTestFirmwareRevCode() { return ResultTestFirmwareRevCode_ ; }
  inline int GetResultTestMezzId() { return ResultTestMezzId_ ; }
  inline int GetResultTestPromId() { return ResultTestPromId_ ; }
  inline int GetResultTestPROMPath() { return ResultTestPROMPath_ ; }
  inline int GetResultTestDSN() { return ResultTestDSN_ ; }
  inline int GetResultTestADC() { return ResultTestADC_ ; }
  inline int GetResultTest3d3444() { return ResultTest3d3444_ ; }
  //
  void reset();
  void readreg4();
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
  //
  bool compareValues(std::string, int, int, bool);
  bool compareValues(std::string, float, float, float);
  void messageOK(std::string,bool);
  //
  // JTAG stuff:
  inline void set_jtag_address(int address){ jtag_address = address;}
  void set_jtag_chain(int);
  void jtag_anystate_to_rti();
  void jtag_ir_dr(int,int,int*,int,int*);
  //
  //To be RAT members:
  void RatTmbDelayScan();
  void ReadRatUser1();
  int ReadRatUser2();
  int read_rat_delay();
  void RpcRatDelayScan(int);
  void set_rat_delay(int,int);
  int read_rpc_parity_error_counter(int);
  int read_rpc_parity_ok(int);
  int read_rpc_data(int);
  void reset_parity_error_counter(int);
 //
  //Not yet working but should eventually be in TMB.cc...
  float tmb_temp(int, int);
  //
  //To be in TMB.cc...
  void SetExtCLCTPatternTrigger();

 protected:
  //

 private: 
  std::ostream * MyOutput_ ;
  TMB * tmb_;
  CCB * ccb_;
  int * TMBslot;

  //JTAG stuff:
  int jtag_address;
  int jtag_chain;
  int devices_in_chain;
  int bits_per_opcode[MAX_NUM_CHIPS];
  int bits_to_int(int*,int,int);
  bool step_mode;
  void select_jtag_chain_param();
  void jtag_io_byte(int,int*,int*,int* );
  void step(int,int,int,int,int);
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
  //
  //functions needed by above tests:
  int dowCRC(std::bitset<64>);
  void bit_to_array(int, int *, const int); 
  void decodeRATUser1(int*);
};

#endif

