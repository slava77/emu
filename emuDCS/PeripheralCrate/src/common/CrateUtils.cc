//-----------------------------------------------------------------------
// $Id: CrateUtils.cc,v 3.1 2007/08/28 21:50:58 liu Exp $
// $Log: CrateUtils.cc,v $
// Revision 3.1  2007/08/28 21:50:58  liu
// *** empty log message ***
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 1.4  2006/07/13 15:53:03  mey
// New Parser structure
//
// Revision 1.3  2006/03/23 08:24:58  mey
// Update
//
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
  for( unsigned i=0; i< tmbVector.size(); i++) {
    //
    TMBTester tmp;
    tmp.setTMB(tmbVector[i]);
    tmp.setCCB(MyCCB_);
    tmp.setRAT(tmbVector[i]->getRAT());
    result.push_back(tmp);
    //
  }
  //
  return result;
}

    
