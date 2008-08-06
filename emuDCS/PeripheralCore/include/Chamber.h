#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>
#include <string>

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
  //
  std::vector <TMB> tmbs_;
  std::vector <CFEB> cfebs_;
  std::vector <DAQMB> dmbs_;
  //
};

#endif
