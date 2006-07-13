#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"

class Chamber {
  //
public:
  //
  Chamber();
  Chamber(int,int);
  Chamber(TMB*);
  ~Chamber();
  //
  inline void SetTMB(TMB* myTMB)   {thisTMB = myTMB; alct = myTMB->alctController() ;}
  inline TMB* GetTMB(){ return thisTMB; }
  inline void SetDMB(DAQMB* myDMB) {thisDMB = myDMB; }
  inline DAQMB* GetDMB(){ return thisDMB; }
  inline void SetCCB(CCB* myCCB)   {thisCCB_ = myCCB; }
  inline void SetMPC(MPC* myMPC)   {thisMPC = myMPC; }
  //
private:
  //
  TMB * thisTMB ;
  DAQMB * thisDMB ;
  CCB * thisCCB_ ;
  ALCTController *alct ;
  MPC * thisMPC;
  //
};

#endif
