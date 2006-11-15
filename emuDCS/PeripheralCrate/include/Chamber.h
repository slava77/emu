#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "CFEB.h"

class Chamber {
  //
public:
  //
  Chamber();
  ~Chamber();
  //
  inline void SetTMB(TMB* myTMB)   {thisTMB = myTMB; alct = myTMB->alctController() ;}
  inline TMB* GetTMB(){ return thisTMB; }
  inline void SetDMB(DAQMB* myDMB) {thisDMB = myDMB; }
  inline DAQMB* GetDMB(){ return thisDMB; }
  inline void SetCCB(CCB* myCCB)   {thisCCB_ = myCCB; }
  inline void SetMPC(MPC* myMPC)   {thisMPC = myMPC; }
  inline void SetCrate(Crate* myCrate)   {crate_ = myCrate; }
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
  std::vector <TMB> tmbs_;
  std::vector <CFEB> cfebs_;
  std::vector <DAQMB> dmbs_;
  //
};

#endif
