#ifndef ChamberUtilities_h
#define ChamberUtilities_h

#include <iostream>
#include <fstream>


#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "RAT.h"
#include "ALCTController.h"

class ChamberUtilities {
  //
public:
  //
  ChamberUtilities();
  virtual ~ChamberUtilities();
  //
  inline void RedirectOutput(std::ostream * Output) { 
    //
    thisTMB->RedirectOutput(Output);
    thisDMB->RedirectOutput(Output);
    thisCCB_->RedirectOutput(Output);
    thisMPC->RedirectOutput(Output);
    MyOutput_ = Output ;
    //
  }
  //
  void CCBStartTrigger();
  void CFEBTiming();
  void PulseCFEB(int HalfStrip = -1, int CLCTInputs = 0x1f , bool enable=0);
  void LoadCFEB(int HalfStrip = -1, int CLCTInputs = 0x1f , bool enable=0);
  void CFEBChamberScan();
  void PulseTestStrips(int delay = 0x6868);
  void ALCTTiming();
  void ALCTScanDelays();
  void ALCTChamberScan();
  void ALCTSVFLoad();
  int  TMBL1aTiming(int enable=1);
  int  FindBestL1aAlct();
  void PulseRandomALCT(int delay = 0x6868);
  void PulseAllWires();
  int  FindWinner(int);
  int  FindALCTvpf();
  int  FindTMB_L1A_delay(int,int);
  int  FindALCT_L1A_delay(int,int);
  void Automatic();
  int  AdjustL1aLctDMB();
  void InitStartSystem();
  void InitSystem();
  void InjectMPCData() ;
  void ALCT_phase_analysis(int rxtxtiming[13][13]);
  void RatTmbDelayScan();
  void RpcRatDelayScan();       //rpc=0
  void RpcRatDelayScan(int rpc);
  //
  // Move to function class ?
  //
  int window_analysis(int * data, const int length) ;
  void bit_to_array(int data, int * array, const int size) ;
  //
  bool UseCosmic ;
  bool UsePulsing ;
  //
  int BestCCBDelaySetting ;
  float CFEBMean[5];
  int TMB_L1a_timing ;
  int Find_ALCT_L1a_delay;
  int ALCT_L1a_delay ;
  int ALCTvpf;
  int beginning;
  //
  inline void SetTMB(TMB* myTMB)   {thisTMB = myTMB; alct = myTMB->alctController(); thisRAT_ = myTMB->getRAT();}
  inline TMB* GetTMB() { return thisTMB; }
  //
  inline void SetDMB(DAQMB* myDMB) {thisDMB = myDMB; }
  inline DAQMB* GetDMB() {return thisDMB; }
  //
  inline void SetCCB(CCB* myCCB)   {thisCCB_ = myCCB; }
  inline CCB* GetCCB()   {return thisCCB_;}
  //
  inline void SetMPC(MPC* myMPC)   {thisMPC = myMPC; }
  //
  inline int  GetALCTrxPhase() { return thisTMB->GetALCTrxPhase(); }
  inline int  GetALCTtxPhase() { return thisTMB->GetALCTtxPhase(); }
  inline int  GetCFEBrxPhase(int CFEB) {return thisTMB->GetCFEBrxPhase(CFEB); }
  inline int  GetMPCdelay()    { return thisTMB->GetMPCdelay(); }
  //
  // The following are test results, not configuration data:
  inline int  GetALCTrxPhaseTest()         { return ALCTrxPhase_ ; }
  inline int  GetALCTtxPhaseTest()         { return ALCTtxPhase_ ; }
  inline int  GetCFEBrxPhaseTest(int CFEB) { return CFEBrxPhase_[CFEB] ; }
  inline int  GetMPCdelayTest()            { return MPCdelay_ ; }
  inline int  GetRatTmbDelayTest()            { return RatTmbDelay_ ; }
  inline int  GetRpcRatDelayTest()         { return GetRpcRatDelayTest(0); }
  inline int  GetRpcRatDelayTest(int rpc)  { return RpcRatDelay_[rpc] ; }
  //
  inline void SetALCTrxPhaseTest(int value) { ALCTrxPhase_ = value ; }
  inline void SetALCTtxPhaseTest(int value) { ALCTtxPhase_ = value ; }
  inline void SetCFEBrxPhaseTest(int CFEB, int value) { CFEBrxPhase_[CFEB] = value ; }
  inline void SetMPCdelayTest(int value)    { MPCdelay_ = value ; }
  inline void  SetRatTmbDelayTest(int value) { RatTmbDelay_ = value ; }
  //
  inline int  GetCFEBStripScan(int CFEB, int Strip){ return CFEBStripScan_[CFEB][Strip]; }
  inline int  GetALCTWireScan(int Wire){ return ALCTWireScan_[Wire]; }
  inline int  GetTMBL1aTiming(){ return TMBL1aTiming_; }
  inline int  GetALCTvpf(){ return ALCTvpf_; }
  inline int  GetBestALCTL1aDelay(){ return BestALCTL1aDelay_; }
  inline int  GetALCTL1aDelay(){ return ALCTL1aDelay_; }
  //
private:
  //
  std::ostream * MyOutput_ ;
  //
  //
  TMB * thisTMB ;
  DAQMB * thisDMB ;
  CCB * thisCCB_ ;
  ALCTController *alct ;
  MPC * thisMPC;
  RAT * thisRAT_;
  //
  int ALCTrxPhase_, ALCTtxPhase_;
  int TMBL1aTiming_;
  int ALCTvpf_;
  int BestALCTL1aDelay_;
  int ALCTL1aDelay_;
  int CFEBrxPhase_[5];
  int MPCdelay_;
  int CFEBStripScan_[5][32];
  int ALCTWireScan_[112];
  int Npulses_;
  int RpcRatDelay_[2];
  int RatTmbDelay_;
  //
};

#endif
