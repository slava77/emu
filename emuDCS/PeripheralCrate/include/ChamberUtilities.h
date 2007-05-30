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
  ////////////////////////////////////////////////
  // determine peripheral crate timing parameters
  ////////////////////////////////////////////////
  // clock phases
  void CFEBTiming();
  void ALCTTiming();
  void RatTmbDelayScan();
  void RpcRatDelayScan();       //rpc=0
  void RpcRatDelayScan(int rpc);
  //
  // ALCT-CLCT match timing
  int  FindALCTvpf();
  //
  // Winner bits from MPC
  int  FindWinner(int number_of_pulses);
  //
  // DMB parameters
  void MeasureAlctDavCableDelay();
  void MeasureTmbLctCableDelay();
  void MeasureCfebDavCableDelay();
  void MeasureCfebCableDelay();
  void ReadAllDmbValuesAndScopes();
  void PrintAllDmbValuesAndScopes();
  //
  // L1A accept windows
  int  TMBL1aTiming(int enable=1);
  int  FindTMB_L1A_delay(int,int);
  int  FindBestL1aAlct();
  int  FindALCT_L1A_delay(int,int);
  //
  // scans to check functionality of electronics
  void CFEBChamberScan();
  void ALCTChamberScan();
  //
  // useful peripheral crate functions
  void InitSystem();
  void InitStartSystem();
  //
  void PulseRandomALCT(int delay = 0x6868);
  void PulseAllWires();
  //
  void PulseTestStrips(int delay = 0x6868);
  void LoadCFEB(int HalfStrip = -1, int CLCTInputs = 0x1f , bool enable=0);
  void PulseCFEB(int HalfStrip = -1, int CLCTInputs = 0x1f , bool enable=0);
  //
  void InjectMPCData() ;
  //
  void ZeroDmbHistograms();
  void PopulateDmbHistograms(int number_of_reads);
  //
  int  AdjustL1aLctDMB();
  void CCBStartTrigger();
  //
  // analysis methods
  void ALCT_phase_analysis(int rxtxtiming[13][13]);
  int window_analysis(int * data, const int length);
  int DelayWhichGivesDesiredValue(float * vector_of_values_with_delay_index, 
				  const int min_delay, const int max_delay, 
				  int desired_value);
  void PrintHistogram(std::string label, int * histogram, int min, int max, int average);
  void PrintHistogram(std::string label, int * histogram, int min, int max, float average);
  float AverageHistogram(int * histogram, int min_value, int max_value);
  void bit_to_array(int data, int * array, const int size) ;
  inline int RoundOff(float RealNumber) { return ( (int) (RealNumber + 0.5) ); }
  //
  //
  // Following should be deprecated?
  void Automatic();
  void ALCTScanDelays();
  //
  // Get and Set peripheral crate instances:
  inline void SetTMB(TMB * myTMB)   {thisTMB = myTMB; alct = myTMB->alctController(); thisRAT_ = myTMB->getRAT();}
  inline TMB * GetTMB() { return thisTMB; }
  //
  inline void SetDMB(DAQMB * myDMB) {thisDMB = myDMB; }
  inline DAQMB * GetDMB() {return thisDMB; }
  //
  inline void SetCCB(CCB * myCCB)   {thisCCB_ = myCCB; }
  inline CCB * GetCCB()   {return thisCCB_;}
  //
  inline void SetMPC(MPC * myMPC)   {thisMPC = myMPC; }
  //
  // Get xml values:
  inline int  GetCFEBrxPhase(int CFEB)           { return thisTMB->GetCFEBrxPhase(CFEB); }
  inline int  GetALCTrxPhase()                   { return thisTMB->GetALCTrxPhase(); }
  inline int  GetALCTtxPhase()                   { return thisTMB->GetALCTtxPhase(); }
  inline int  GetRatTmbDelay()                   { return thisTMB->GetRatTmbDelay() ; }
  inline int  GetRpcRatDelay()                   { return thisTMB->GetRpc0RatDelay(); }
  inline int  GetMPCdelay()                      { return thisTMB->GetMPCdelay(); }
  inline int  GetAlctDavCableDelay_configvalue() { return thisDMB->GetAlctDavCableDelay(); }
  inline int  GetTmbLctCableDelay_configvalue()  { return thisDMB->GetTmbLctCableDelay(); }
  inline int  GetCfebDavCableDelay_configvalue() { return thisDMB->GetCfebDavCableDelay(); }
  inline int  GetCfebCableDelay_configvalue()    { return thisDMB->GetCfebCableDelay(); }
  inline int  GetTMBL1aTiming_configvalue()      { return thisTMB->GetL1aDelay(); }
  inline int  GetALCTvpf_configvalue()           { return thisTMB->GetAlctVpfDelay(); }
  inline int  GetALCTL1aDelay_configvalue()      { return thisTMB->alctController()->GetWriteL1aDelay(); }
  //
  // Get parameters from test summary results (not xml parameters):
  inline int  GetCFEBrxPhaseTest(int CFEB) { return CFEBrxPhase_[CFEB] ; }
  inline int  GetALCTrxPhaseTest()         { return ALCTrxPhase_ ; }
  inline int  GetALCTtxPhaseTest()         { return ALCTtxPhase_ ; }
  inline int  GetRatTmbDelayTest()         { return RatTmbDelay_ ; }
  inline int  GetRpcRatDelayTest()         { return GetRpcRatDelayTest(0); }
  inline int  GetRpcRatDelayTest(int rpc)  { return RpcRatDelay_[rpc] ; }
  inline int  GetALCTvpf()                 { return ALCTvpf_; }
  inline int  GetMPCdelayTest()            { return MPCdelay_ ; }
  inline int  GetAlctDavCableDelayTest()   { return AlctDavCableDelay_; }
  inline int  GetTmbLctCableDelayTest()    { return TmbLctCableDelay_; }
  inline int  GetCfebDavCableDelayTest()   { return CfebDavCableDelay_; }
  inline int  GetCfebCableDelayTest()      { return CfebCableDelay_; }
  inline int  GetTMBL1aTiming()            { return TMBL1aTiming_; }
  inline int  GetBestALCTL1aDelay()        { return BestALCTL1aDelay_; }
  inline int  GetALCTL1aDelay()            { return ALCTL1aDelay_; }
  //
  // Get measured values (not parameters)
  inline int  GetActiveFebFlagToL1aAtDMB() { return AffToL1aAverageValue_; }
  inline int  GetTmbDavValue()             { return TmbDavAverageValue_; }
  inline int  GetCfebDavValue()            { return CfebDavAverageValue_; }
  inline int  GetAlctDavValue()            { return AlctDavAverageValue_; }
  inline int  GetActiveFebFlagToL1aScope() { return AffToL1aScopeAverageValue_; }
  inline int  GetTmbDavScopeValue()        { return TmbDavScopeAverageValue_; }
  inline int  GetCfebDavScopeValue()       { return CfebDavScopeAverageValue_; }
  inline int  GetAlctDavScopeValue()       { return AlctDavScopeAverageValue_; }
  //
  inline int  GetCFEBStripScan(int CFEB, int Strip) { return CFEBStripScan_[CFEB][Strip]; }
  inline int  GetALCTWireScan(int Wire)             { return ALCTWireScan_[Wire]; }
  //
  // Set the parameters from test summary file (not xml):
  inline void SetCFEBrxPhaseTest(int CFEB, int value) { CFEBrxPhase_[CFEB] = value ; }
  inline void SetALCTrxPhaseTest(int value)           { ALCTrxPhase_ = value ; }
  inline void SetALCTtxPhaseTest(int value)           { ALCTtxPhase_ = value ; }
  inline void SetRatTmbDelayTest(int value)           { RatTmbDelay_ = value ; }
  inline void SetRpcRatDelayTest(int rpc, int value)  { RpcRatDelay_[rpc] = value ; }
  inline void SetALCTvpf(int value)                   { ALCTvpf_ = value; }
  inline void SetMPCdelayTest(int value)              { MPCdelay_ = value ; }
  inline void SetTMBL1aTiming(int value)              { TMBL1aTiming_ = value; }
  inline void SetBestALCTL1aDelay(int value)          { BestALCTL1aDelay_ = value; }
  inline void SetALCTL1aDelay(int value)              { ALCTL1aDelay_ = value; }
  //
  inline void SetCFEBStripScan(int CFEB, int Strip, int value) { CFEBStripScan_[CFEB][Strip] = value; }
  inline void SetALCTWireScan(int Wire, int value)             { ALCTWireScan_[Wire] = value; }
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
private:
  //
  std::ostream * MyOutput_ ;
  //
  int Npulses_;
  //
  TMB * thisTMB ;
  DAQMB * thisDMB ;
  CCB * thisCCB_ ;
  ALCTController *alct ;
  MPC * thisMPC;
  RAT * thisRAT_;
  //
  int CFEBrxPhase_[5];
  int ALCTtxPhase_;
  int ALCTrxPhase_;
  int RatTmbDelay_;
  int RpcRatDelay_[2];
  int ALCTvpf_;
  int MPCdelay_;
  int AlctDavCableDelay_;
  int TmbLctCableDelay_;
  int CfebDavCableDelay_;
  int CfebCableDelay_;
  int TMBL1aTiming_;
  int BestALCTL1aDelay_;
  int ALCTL1aDelay_;
  //
  int ScopeMin_;
  int ScopeMax_;
  int AffToL1aValueMin_;
  int AffToL1aValueMax_;
  int CfebDavValueMin_;
  int CfebDavValueMax_;
  int TmbDavValueMin_;
  int TmbDavValueMax_;
  int AlctDavValueMin_;
  int AlctDavValueMax_;
  //
  int AffToL1aAverageValue_;
  int CfebDavAverageValue_;
  int TmbDavAverageValue_;
  int AlctDavAverageValue_;
  int AffToL1aScopeAverageValue_;
  int CfebDavScopeAverageValue_;
  int TmbDavScopeAverageValue_;
  int AlctDavScopeAverageValue_;
  //
  int CFEBStripScan_[5][32];
  int ALCTWireScan_[112];
  //
  int AffToL1aScopeHisto_[5];
  int CfebDavScopeHisto_[5];
  int TmbDavScopeHisto_[5];
  int AlctDavScopeHisto_[5];
  int AffToL1aValueHisto_[255];
  int CfebDavValueHisto_[255];
  int TmbDavValueHisto_[255];
  int AlctDavValueHisto_[255];
  //
};

#endif
