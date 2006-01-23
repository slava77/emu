#ifndef TMBTester_h
#define TMBTester_h

#include<iostream>
#include<fstream>
#include<stdio.h>
#include <string>
#include "TMB.h"
#include "CCB.h"

class TMBTester {
 public:
  // not responsible for deleting pointers
  TMBTester();
  //
  virtual ~TMBTester();
  //

  inline void setTMB(TMB * tmb) {tmb_ = tmb;}
  inline void setCCB(CCB * ccb) {ccb_ = ccb;}
  inline void setTMBslot(int * slot) {TMBslot = slot;}

  void reset();
  void readreg4();

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
  float tmb_temp(int,int);  //(command, 1=TMB 2=RAT)
  void RatTmbDelayScan();
  void rat_clk_delays(unsigned short int,int);
  void RatUser1JTAG();

  //
  bool compareValues(std::string, int, int, bool);
  bool compareValues(std::string, float, float, float);
  void messageOK(std::string,bool);
  //
  // the following should be in TMB.cc:

 protected:
  //

 private: 
  std::ostream * MyOutput_ ;
  TMB * tmb_;
  CCB * ccb_;
  int * TMBslot;

  //functions needed by above tests:
  int dowCRC(std::bitset<64>);
  void bit_to_array(int, int *, const int); 
  int UserOrBootJTAG(int);
  void vme_jtag_anystate_to_rti(int,int);
  void vme_jtag_write_ir(int,int,int,int);
  void vme_jtag_write_dr(int,int,int,int,
			 int,int);
  void vme_jtag_io_byte(int,int,int,
			unsigned char*,
			unsigned char*,
			unsigned char*,
			int);
  void step(int,int,int,int);
  int select_jtag_chain(int);

  // the following should be in TMB.cc:
};

#endif

