//-----------------------------------------------------------------------
// $Id: DAQMBTester.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: DAQMBTester.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.2  2005/09/07 17:59:21  mey
// Update
//
// Revision 2.1  2005/09/06 14:44:43  mey
// Defined output stream
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DAQMBTester_h
#define DAQMBTester_h

class DAQMB;
class CCB;

#include<iostream>
#include<fstream>
#include<stdio.h>

class DAQMBTester {
public:
  // not responsible for deleting pointers
  DAQMBTester(DAQMB * daqmb, CCB * ccb, bool fakeBackPlane)
    : daqmb_(daqmb), ccb_(ccb), fakeBackPlane_(fakeBackPlane)
  {
    MyOutput_ = & std::cout ;
  }
  //
  ~DAQMBTester() {}
  //
  void setDAQMB(DAQMB * dmb) {daqmb_ = dmb;}
  void setCCB(CCB * ccb) {ccb_ = ccb;}

  void runAllTests();

  void fifo();
  void dump();
  void bucktest();
  void daqmb_init();
  void cfeb_pulse();
  void cfeb_inject();
  void halfset(int ifeb,int ipln,int ihalf);
  void cfeb_pedestals();

  /// not for standard users, only for experts
  void svfload_feb_prom();
  void svfload_prom();
  //
  // Redirect Output
  //
  inline void RedirectOutput(std::ostream * Output) { MyOutput_ = Output ; }
  //
private:
  std::ostream * MyOutput_ ;
  DAQMB * daqmb_;
  CCB   * ccb_;
  bool fakeBackPlane_;
};

#endif

