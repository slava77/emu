#ifndef DAQMBTest_h
#define DAQMBTest_h

class DAQMB;
class CCB;
class HardwareDDU;

class DAQMBTest{
public:
  // not responsible for deleting pointers
  DAQMBTest(DAQMB * daqmb, CCB * ccb, HardwareDDU *ddu)
  : daqmb_(daqmb), ccb_(ccb),ddu_(ddu) {}
  ~DAQMBTest() {}

  void setDAQMB(DAQMB * dmb) {daqmb_ = dmb;}
  void setCCB(CCB * ccb) {ccb_ = ccb;}
  void setDDU(HardwareDDU * ddu){ddu_=ddu;}  
  void test1();
  void test2();
  void test3();
  void test4();
  void test5();
  void test6();
  void test7();
  void test8();
  void test9();
  void test10();
  void test11();
  void test12();
  void test13();
  int memchk(int fifo);
  void prec_dac_set(float vset);
  float chifit_ts(float tql,float tq,float tqh);
  void presq_const(int irw,int brd,int pln,float* old,float* pres_x);
  void prest_const(int irw,int brd,int pln,float* old,float* pres_x);
  void capq_const(int irw,int brd,int pln,float* old,float* pres_x);
  void capt_const(int irw,int brd,int pln,float* old,float* pres_x);
  void halfset(int ifeb,int ipln,int ihalf);

private:
  DAQMB * daqmb_;
  CCB   * ccb_;
  HardwareDDU * ddu_;
};

#endif

