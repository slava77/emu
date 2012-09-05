#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>
#include <string>


namespace emu {
  namespace pc {


class TMB;
class DAQMB;
class CCB;
class ALCTController;
class MPC;
class CFEB;
class Crate;

class Chamber {
  //
public:
  //
  Chamber(Crate *);
  ~Chamber();
  //
  void SetTMB(TMB* myTMB);
  inline TMB* GetTMB(){ return thisTMB; }
  //
  inline void SetDMB(DAQMB* myDMB) {thisDMB = myDMB; }
  inline DAQMB* GetDMB(){ return thisDMB; }
  inline void SetCCB(CCB* myCCB)   {thisCCB_ = myCCB; }
  inline void SetMPC(MPC* myMPC)   {thisMPC = myMPC; }
  //
  inline void SetCrate(Crate* myCrate)   {crate_ = myCrate; }
  inline Crate * GetCrate()   {return crate_; }
  //
  inline void SetLabel(std::string label){label_ = label;}
  inline std::string GetLabel(){return label_;}
  //
  inline void SetProblemDescription(std::string problem_description){problem_description_ = problem_description;}
  inline std::string GetProblemDescription(){return problem_description_;}
  //
  /// define the following bitmask for expected configuration problems for this chamber:
  ///  bit[0,1,...7] = ALCT, TMB, CFEB1, CFEB2, CFEB3, CFEB4, CFEB5, DMB
  void SetProblemMask(int problem_mask);
  inline int GetProblemMask(){return problem_mask_;}
  //
  ///From the problem_mask, we expect to see a configuration problem from the ALCT
  inline int GetExpectedConfigProblemALCT()  { return expected_config_problem_alct_ ; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the TMB
  inline int GetExpectedConfigProblemTMB()   { return expected_config_problem_tmb_  ; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the CFEB1
  inline int GetExpectedConfigProblemCFEB1() { return expected_config_problem_cfeb1_; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the CFEB2
  inline int GetExpectedConfigProblemCFEB2() { return expected_config_problem_cfeb2_; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the CFEB3
  inline int GetExpectedConfigProblemCFEB3() { return expected_config_problem_cfeb3_; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the CFEB4
  inline int GetExpectedConfigProblemCFEB4() { return expected_config_problem_cfeb4_; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the CFEB5
  inline int GetExpectedConfigProblemCFEB5() { return expected_config_problem_cfeb5_; }
  //
  ///From the problem_mask, we expect to see a configuration problem from the DMB
  inline int GetExpectedConfigProblemDMB()   { return expected_config_problem_dmb_  ; }
  //
  //
private:
  //
  TMB * thisTMB ;
  DAQMB * thisDMB ;
  CCB * thisCCB_ ;
  ALCTController *alct ;
  MPC * thisMPC;
  Crate * crate_;
  //
  std::string label_;
  std::string problem_description_;
  int problem_mask_;
  int expected_config_problem_alct_ ;
  int expected_config_problem_tmb_  ;
  int expected_config_problem_cfeb1_;
  int expected_config_problem_cfeb2_;
  int expected_config_problem_cfeb3_;
  int expected_config_problem_cfeb4_;
  int expected_config_problem_cfeb5_;
  int expected_config_problem_dmb_  ;
  //
  std::vector <TMB> tmbs_;
  std::vector <CFEB> cfebs_;
  std::vector <DAQMB> dmbs_;
  //
};


  } // namespace emu::pc
  } // namespace emu
#endif
