#ifndef ChamberUtilities_h
#define ChamberUtilities_h

#include <iostream>
#include <fstream>


#include "emu/pc/DAQMB.h"
#include "emu/pc/TMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/MPC.h"
#include "emu/pc/RAT.h"
#include "emu/pc/ALCTController.h"


namespace emu {
  namespace pc {

/**
   Functions specific to a chamber which may use methods from several different core classes  
 */
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
  //
  ///When measuring synchronization parameters, choose to propagate the measured values from one step to the next (or not)
  inline void PropagateMeasuredValues(bool true_or_false) { use_measured_values_ = true_or_false; }
  //
  ///Perform multiple synchronization steps sequentially:
  void FindL1ADelays();
  void FindDAVDelays();
  void FindL1AAndDAVDelays();
  void Automatic();    // should be deprecated, since its name is not descriptive at all
  //
  /// When BC0 synchronization parameters change, perform this quick scan:
  void QuickTimingScan();
  //
  // clock phases...
  ///  CFEB -> TMB communication delays
  enum CFEBTiming_scanType {normal_scan, me11b_scan, me11a_scan};  
  void CFEBTiming();                           //default is normal_scan
  void CFEBTiming(CFEBTiming_scanType);
  void CFEBTiming_with_Posnegs(CFEBTiming_scanType);
  void CFEBTiming_without_Posnegs();
  //
  /// RAT -> TMB communication delays
  int  RatTmbDelayScan();
  void RpcRatDelayScan();       //rpc=0
  void RpcRatDelayScan(int rpc);
  //
  /// Determine all the timing parameters needed to establish ALCT<->TMB communication
  void ALCT_TMB_Loopback();
  //
  /// Find a "good enough" value of the alct_rx_clock_delay with alternating 1's and 0's sent from ALCT to TMB. 
  int  Find_alct_rx_with_ALCT_to_TMB_evenodd(int number_of_passes);
  //
  /// Find a "good enough" value of the alct_tx_clock_delay with alternating 1's and 0's looped back from TMB -> ALCT -> TMB.
  int  Find_alct_tx_with_TMB_to_ALCT_evenodd(int number_of_passes);
  //
  /// Check all the conductors in the cable with walking 1's looped back from TMB -> ALCT -> TMB
  int  TMB_to_ALCT_walking_ones(int number_of_passes);
  //
  /// same as above, but with "default number of passes...
  int  Find_alct_rx_with_ALCT_to_TMB_evenodd();
  int  Find_alct_tx_with_TMB_to_ALCT_evenodd();
  int  TMB_to_ALCT_walking_ones();
  //
  /// Thoroughly check the ALCT<->TMB communication by looping back random data from TMB -> ALCT -> TMB
  int ALCT_TMB_TimingUsingRandomLoopback();
  int ALCT_TMB_TimingUsingErrorCorrectionCode();
  void ThoroughRxTxScan(int rx_posneg, int tx_posneg, int pipe_depth);
  //
  // Define the timing of the BC0 from TMB -> ALCT -> TMB
  int ALCTBC0Scan();
  //
  inline void setLocalTmbBxnOffset(int value) { local_tmb_bxn_offset_ = value; }
  inline int getLocalTmbBxnOffset() { return local_tmb_bxn_offset_; }
  //
  //-----
  /// number of microseconds to wait between data reads
  inline void setPauseBetweenDataReads(int pause) { pause_between_data_reads_ = pause; }
  inline int getPauseBetweenDataReads() { return pause_between_data_reads_; }
  //
  /// number of times to read DMB scope/counter
  inline void setNumberOfDataReads(int number) { number_of_data_reads_ = number; }
  inline int getNumberOfDataReads() { return number_of_data_reads_; }
  //
  // ALCT-CLCT match timing
  //  int FindALCTinCLCTMatchWindow();
  //
  // DMB parameters
  int MeasureAlctDavCableDelay();
  int MeasureTmbLctCableDelay();
  int MeasureCfebDavCableDelay();
  //
  //
  //-----
  /// number of seconds to wait (sec) at each delay value setting
  inline void setPauseAtEachSetting(int pause) { pause_at_each_setting_ = pause; }
  inline int getPauseAtEachSetting() { return pause_at_each_setting_; }
  //
  /// Winner bits from MPC -> TMB
  int FindWinner();                          /// Use cosmic rays/pulsing from TTC
  int FindWinner(int number_of_pulses);      /// Use local pulsing of CFEB, data sent to MPC from TMB by VME command
  //
  // L1A accept windows
  inline void setMinAlctL1aDelayValue(int value) { min_alct_l1a_delay_value_ = value; }
  inline int getMinAlctL1aDelayValue() { return min_alct_l1a_delay_value_; }
  //
  inline void setMaxAlctL1aDelayValue(int value) { max_alct_l1a_delay_value_ = value; }
  inline int getMaxAlctL1aDelayValue() { return max_alct_l1a_delay_value_; }
  //
  inline void setMinTmbL1aDelayValue(int value) { min_tmb_l1a_delay_value_ = value; }
  inline int getMinTmbL1aDelayValue() { return min_tmb_l1a_delay_value_; }
  //
  inline void setMaxTmbL1aDelayValue(int value) { max_tmb_l1a_delay_value_ = value; }
  inline int getMaxTmbL1aDelayValue() { return max_tmb_l1a_delay_value_; }
  //
  int FindTmbAndAlctL1aDelay();
  int FindTMB_L1A_delay();
  int FindALCT_L1A_delay();
  //
  int  FindTMB_L1A_delay(int min_delay, int max_delay);  //set the min/max values by arguments... should go away...
  int  FindALCT_L1A_delay(int min_delay, int max_delay); //set the min/max values by arguments... should go away...
  //
  // scans to check functionality of electronics
  void ALCTChamberScan();  
  void CFEBChamberScan();
  void FindDistripHotChannels();
  //
  // useful peripheral crate functions
  void SetupRadioactiveTriggerConditions();
  void ReturnToInitialTriggerConditions();
  //
  void InitSystem();
  void InitStartSystem();
  //
  /// Setup to pulse the chamber so that both CLCT and ALCT fire on the ADB_SYNC command
  void SetupCoincidencePulsing();
  void SetupCoincidencePulsing(int amplitude);
  void SetupCoincidencePulsing(int amplitude, int pulse_signal);
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
  void PopulateDmbHistograms();
  void ReadAllDmbValuesAndScopes();
  void PrintAllDmbValuesAndScopes();
  //
  int  AdjustL1aLctDMB();
  //
  // analysis methods
  void ZeroTmbHistograms();
  void ZeroDmbHistograms();
  void ALCT_phase_analysis(int array_to_analyze[25][25]);
  int window_analysis(int * data, const int length);
  int window_counter;
  int DelayWhichGivesDesiredValue(float * vector_of_values_with_delay_index, 
				  const int min_delay, const int max_delay, 
				  int desired_value);
  void PrintHistogram(std::string label, int * histogram, int min, int max, int average);
  void PrintHistogram(std::string label, int * histogram, int min, int max, float average);
  float AverageHistogram(int * histogram, int min_value, int max_value);
  void bit_to_array(int data, int * array, const int size) ;
  inline int RoundOff(float RealNumber) { return ( (int) (RealNumber + 0.5) ); }
  inline int RoundOffForEvenWindowWidths(float RealNumber) { return ((int) RealNumber); }
  bool inSpecialRegion(int cfeb0tof,int cfeb1tof,int cfeb2tof,int cfeb3tof,int cfeb4tof,
		       int cfeb0rx ,int cfeb1rx ,int cfeb2rx ,int cfeb3rx ,int cfeb4rx ,
		       int cfeb0pn ,int cfeb1pn ,int cfeb2pn ,int cfeb3pn ,int cfeb4pn ,
		       CFEBTiming_scanType scanType);
  float determine_average_with_wraparound(int val1, int val2, int val3, int val4, int val5, const int max_val);
  float determine_average_with_wraparound(int val1, int val2, int val3, int val4, const int max_val);
  float special_region_function(float signed_rx);
  //
  //
  // Following should be deprecated?
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
  inline int  GetCfebRxClockDelay(int CFEB)      { return thisTMB->GetCfebRxClockDelay(CFEB); }
  inline int  GetAlctRxClockDelay()              { return thisTMB->GetAlctRxClockDelay(); }
  inline int  GetAlctTxClockDelay()              { return thisTMB->GetAlctTxClockDelay(); }
  inline int  GetAlctRxPosNeg()                  { return thisTMB->GetAlctRxPosNeg(); }
  inline int  GetAlctTxPosNeg()                  { return thisTMB->GetAlctTxPosNeg(); }
  inline int  GetRatTmbDelay()                   { return thisTMB->GetRatTmbDelay() ; }
  inline int  GetRpcRatDelay()                   { return thisTMB->GetRpc0RatDelay(); }
  inline int  GetMPCdelay()                      { return thisTMB->GetMpcRxDelay(); }
  inline int  GetMPCTxDelay_configvalue()        { return thisTMB->GetMpcTxDelay(); }
  inline int  GetAlctDavCableDelay_configvalue() { return thisDMB->GetAlctDavCableDelay(); }
  inline int  GetTmbLctCableDelay_configvalue()  { return thisDMB->GetTmbLctCableDelay(); }
  inline int  GetCfebDavCableDelay_configvalue() { return thisDMB->GetCfebDavCableDelay(); }
  inline int  GetCfebCableDelay_configvalue()    { return thisDMB->GetCfebCableDelay(); }
  inline int  GetTMBL1aTiming_configvalue()      { return thisTMB->GetL1aDelay(); }
  inline int  GetALCTvpf_configvalue()           { return thisTMB->GetAlctVpfDelay(); }
  inline int  GetALCTL1aDelay_configvalue()      { return thisTMB->alctController()->GetWriteL1aDelay(); }
  inline int  GetAlctBx0Delay()                  { return thisTMB->GetAlctBx0Delay(); }
  //
  // Get parameters from test summary results (not xml parameters):
  inline int  GetCFEBrxPhaseTest(int CFEB) { return CFEBrxPhase_[CFEB] ; }
  inline int  GetCFEBrxPosnegTest(int CFEB){ return CFEBrxPosneg_[CFEB] ; }
  inline int  GetCFEBrxdIntDelayTest(int CFEB) { return cfeb_rxd_int_delay[CFEB] ; }
  inline int  GetALCTrxPhaseTest()         { return ALCTrxPhase_ ; }
  inline int  GetALCTtxPhaseTest()         { return ALCTtxPhase_ ; }
  inline int  GetAlctRxPosNegTest()        { return ALCTrxPosNeg_ ; }
  inline int  GetAlctTxPosNegTest()        { return ALCTtxPosNeg_ ; }
  inline int  GetRatTmbDelayTest()         { return RatTmbDelay_ ; }
  inline int  GetRpcRatDelayTest()         { return GetRpcRatDelayTest(0); }
  inline int  GetRpcRatDelayTest(int rpc)  { return RpcRatDelay_[rpc] ; }
  inline int  GetMatchTrigAlctDelayTest()  { return match_trig_alct_delay_; }
  inline int  GetTmbBxnOffsetTest()        { return tmb_bxn_offset_used_; }
  inline int  GetMpcRxDelayTest()          { return MPCdelay_ ; }
  inline int  GetAlctDavCableDelayTest()   { return AlctDavCableDelay_; }
  inline int  GetTmbLctCableDelayTest()    { return TmbLctCableDelay_; }
  inline int  GetCfebDavCableDelayTest()   { return CfebDavCableDelay_; }
  inline int  GetTmbL1aDelayTest()         { return TMBL1aTiming_; }
  inline int  GetAlctL1aDelayTest()        { return ALCTL1aDelay_; }
  inline int  GetAlctBx0DelayTest()        { return ALCT_bx0_delay_; }
  //
  // Get measured values (not parameters)
  inline float GetActiveFebFlagToL1aAtDMB() { return AffToL1aAverageValue_; }
  inline float GetTmbDavValue()             { return TmbDavAverageValue_; }
  inline float GetCfebDavValue()            { return CfebDavAverageValue_; }
  inline float GetAlctDavValue()            { return AlctDavAverageValue_; }
  inline float GetActiveFebFlagToL1aScope() { return AffToL1aScopeAverageValue_; }
  inline float GetTmbDavScopeValue()        { return TmbDavScopeAverageValue_; }
  inline float GetCfebDavScopeValue()       { return CfebDavScopeAverageValue_; }
  inline float GetAlctDavScopeValue()       { return AlctDavScopeAverageValue_; }
  //
  inline float GetBestAverageAFFtoL1A()     { return best_average_aff_to_l1a_counter_; } 
  inline float GetBestAverageALCTDAVScope() { return best_average_alct_dav_scope_; } 
  inline float GetBestAverageCFEBDAVScope() { return best_average_cfeb_dav_scope_; } 
  //
  inline int  GetCFEBStripScan(int CFEB, int Strip) { return CFEBStripScan_[CFEB][Strip]; }
  inline int  GetALCTWireScan(int Wire)             { return ALCTWireScan_[Wire]; }
  //
  // Set the parameters from test summary file (not xml):
  inline void SetCFEBrxPhaseTest(int CFEB, int value) { CFEBrxPhase_[CFEB] = value ; }
  inline void SetALCTrxPhaseTest(int value)           { ALCTrxPhase_ = value ; }
  inline void SetALCTtxPhaseTest(int value)           { ALCTtxPhase_ = value ; }
  inline void SetAlctRxPosNegTest(int value)          { ALCTrxPosNeg_ = value ; }
  inline void SetAlctTxPosNegTest(int value)          { ALCTtxPosNeg_ = value ; }
  inline void SetRatTmbDelayTest(int value)           { RatTmbDelay_ = value ; }
  inline void SetRpcRatDelayTest(int rpc, int value)  { RpcRatDelay_[rpc] = value ; }
  inline void SetMatchTrigAlctDelayTest(int value)    { match_trig_alct_delay_ = value; }
  inline void SetMpcRxDelayTest(int value)            { MPCdelay_ = value; }
  inline void SetAlctDavCableDelayTest(int value)     { AlctDavCableDelay_ = value; }
  inline void SetTmbLctCableDelayTest(int value)      { TmbLctCableDelay_ = value; }
  inline void SetCfebDavCableDelayTest(int value)     { CfebDavCableDelay_ = value; }
  inline void SetTmbL1aDelayTest(int value)           { TMBL1aTiming_ = value; }
  inline void SetAlctL1aDelayTest(int value)          { ALCTL1aDelay_ = value; }
  //
  inline void SetCFEBStripScan(int CFEB, int Strip, int value) { CFEBStripScan_[CFEB][Strip] = value; }
  inline void SetALCTWireScan(int Wire, int value)             { ALCTWireScan_[Wire] = value; }
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
  int debug_;
  //
  bool use_measured_values_;
  int min_alct_l1a_delay_value_;
  int max_alct_l1a_delay_value_;
  int min_tmb_l1a_delay_value_; 
  int max_tmb_l1a_delay_value_; 
  int pause_at_each_setting_;
  int pause_between_data_reads_;
  int number_of_data_reads_;
  //
  int Npulses_;
  bool comparing_with_clct_;
  int me11_pulsing_;
  //
  bool UsePulsing_;
  //
  TMB * thisTMB ;
  DAQMB * thisDMB ;
  CCB * thisCCB_ ;
  ALCTController *alct ;
  MPC * thisMPC;
  RAT * thisRAT_;
  //
  int CFEBrxPhase_[5];
  int CFEBrxPosneg_[5];
  int cfeb_rxd_int_delay[5];
  int ALCTtxPhase_;
  int ALCTrxPhase_;
  int ALCTrxPosNeg_;
  int ALCTtxPosNeg_;
  int RatTmbDelay_;
  int RpcRatDelay_[2];
  int ALCTvpf_;
  int match_trig_alct_delay_;
  int MPCdelay_;
  int AlctDavCableDelay_;
  int TmbLctCableDelay_;
  int CfebDavCableDelay_;
  int TMBL1aTiming_;
  int BestALCTL1aDelay_;
  int ALCTL1aDelay_;
  //
  int local_tmb_bxn_offset_;
  int ALCT_bx0_delay_;
  int tmb_bxn_offset_used_;
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
  float AffToL1aAverageValue_;
  float CfebDavAverageValue_;
  float TmbDavAverageValue_;
  float AlctDavAverageValue_;
  float AffToL1aScopeAverageValue_;
  float CfebDavScopeAverageValue_;
  float TmbDavScopeAverageValue_;
  float AlctDavScopeAverageValue_;
  //
  float best_average_aff_to_l1a_counter_;
  float best_average_alct_dav_scope_;
  float best_average_cfeb_dav_scope_;
  //
  int CFEBStripScan_[5][32];
  int ALCTWireScan_[112];
  //
  int AlctInClctMatchWindowHisto_[16];
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
  int initial_clct_pretrig_thresh_;
  int initial_clct_pattern_thresh_;
  int initial_alct_pretrig_thresh_;
  int initial_alct_pattern_thresh_;
};

  } // namespace emu::pc
  } // namespace emu
#endif
