#define Chamber_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "DDU.h"
#include "ALCTController.h"

class Chamber {
  //
public:
  //
  Chamber();
  Chamber(TMB*);
  virtual ~Chamber();
  //
  inline void SetTMB(TMB* myTMB)   {thisTMB = myTMB; alct = myTMB->alctController() ;}
  inline void SetDMB(DAQMB* myDMB) {thisDMB = myDMB; }
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
