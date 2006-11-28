#ifndef Chamber_h
#define Chamber_h

#include <iostream>
#include <vector>
#include <string>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "CFEB.h"
#include "Crate.h"

class Chamber {
  //
public:
  //
  Chamber(Crate *);
  ~Chamber();
  //
  inline void SetTMB(TMB* myTMB)   {thisTMB = myTMB; alct = myTMB->alctController() ;}
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
  //
  std::vector <TMB> tmbs_;
  std::vector <CFEB> cfebs_;
  std::vector <DAQMB> dmbs_;
  //
};

#endif
