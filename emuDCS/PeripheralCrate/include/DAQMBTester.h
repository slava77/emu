//-----------------------------------------------------------------------
// $Id: DAQMBTester.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: DAQMBTester.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DAQMBTester_h
#define DAQMBTester_h

class DAQMB;
class CCB;

class DAQMBTester {
public:
  // not responsible for deleting pointers
  DAQMBTester(DAQMB * daqmb, CCB * ccb, bool fakeBackPlane)
  : daqmb_(daqmb), ccb_(ccb), fakeBackPlane_(fakeBackPlane) {}
  ~DAQMBTester() {}

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
  void lowv_dump();
  void daqmb_adc_dump();
  void daqmb_promfpga_dump();

  /// not for standard users, only for experts
  void svfload_feb_prom();
  void svfload_prom();

private:
  DAQMB * daqmb_;
  CCB   * ccb_;
  bool fakeBackPlane_;
};

#endif

