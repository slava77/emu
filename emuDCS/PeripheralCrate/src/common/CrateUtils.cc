//-----------------------------------------------------------------------
// $Id: CrateUtils.cc,v 1.2 2006/03/21 12:22:46 mey Exp $
// $Log: CrateUtils.cc,v $
// Revision 1.2  2006/03/21 12:22:46  mey
// UPdate
//
// Revision 1.1  2006/02/01 18:35:37  mey
// UPdate
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include<iostream>
#include<stdio.h>
#include<unistd.h> // for sleep()
#include "CrateUtils.h"
#include "CCB.h"
#include "DAQMB.h"
#include "TMB.h"
#include "RAT.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "VMEController.h"
#include "JTAG_constants.h"
#include <string>



CrateUtils::CrateUtils()
{
}

//
CrateUtils::~CrateUtils() { 
}

std::vector<TMBTester> CrateUtils::TMBTests() const {
  //
  std::vector<TMB *>       tmbVector = MyCrate_->tmbs();
  CCB * MyCCB_ = MyCrate_->ccb();
  std::vector<TMBTester>   result;
  //
  for( int i=0; i< tmbVector.size(); i++) {
    //
    TMBTester tmp;
    tmp.setTMB(tmbVector[i]);
    tmp.setCCB(MyCCB_);
    RAT* myRat = new RAT(tmbVector[i]); // Create a RAT only for communication
    tmp.setRAT(myRat);
    result.push_back(tmp);
    //
  }
  //
  return result;
}

    
