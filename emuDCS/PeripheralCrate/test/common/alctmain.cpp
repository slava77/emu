//-----------------------------------------------------------------------
// $Id: alctmain.cpp,v 2.0 2005/04/12 08:07:07 geurts Exp $
// $Log: alctmain.cpp,v $
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "VMEController.h"
#include "Crate.h"
#include "TMB.h"
#include "ALCTController.h"

int main()
{

  int crateId=1;
  VMEController * controller = new VMEController(crateId,"128.146.39.89",6030);
  Crate * newCrate = new Crate(crateId, controller);
  TMB * tmb = new TMB(crateId, 6);
  ALCTController alct(tmb, "ME22");
  std::cout << "setup ALCT" << std::endl;
  alct.setup(1);

}


