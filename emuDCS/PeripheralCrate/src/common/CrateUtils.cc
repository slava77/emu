//-----------------------------------------------------------------------
// $Id: CrateUtils.cc,v 1.1 2006/02/01 18:35:37 mey Exp $
// $Log: CrateUtils.cc,v $
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
    result.push_back(tmp);
    //
  }
  //
  return result;
}

    
