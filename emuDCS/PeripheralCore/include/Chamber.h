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
  // define the following bitmask for the chamber's problem mask:
  // bit 0 = ALCT (i.e., least significant bit)
  // bit 1 = TMB
  // bit 2 = CFEB 1
  // bit 3 = CFEB 2
  // bit 4 = CFEB 3
  // bit 5 = CFEB 4
  // bit 6 = CFEB 5
  // bit 7 = DMB
  inline void SetProblemMask(int problem_mask){problem_mask_ = problem_mask;}
  inline int GetProblemMask(){return problem_mask_;}
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
  //
  std::vector <TMB> tmbs_;
  std::vector <CFEB> cfebs_;
  std::vector <DAQMB> dmbs_;
  //
};


  } // namespace emu::pc
  } // namespace emu
#endif
