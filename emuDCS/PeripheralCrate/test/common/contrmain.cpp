//-----------------------------------------------------------------------
// $Id: contrmain.cpp,v 2.1 2005/09/22 13:00:18 mey Exp $
// $Log: contrmain.cpp,v $
// Revision 2.1  2005/09/22 13:00:18  mey
// Alex's changes
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "TestBeamCrateController.h"
#include "PeripheralCrateParser.h"
#include "MPC.h"

int main()
{
  PeripheralCrateParser parser;
  parser.parseFile("config.xml");

  MPC * mpc = parser.mpcParser().mpc();
  DAQMB * daqmb = parser.daqmbParser().daqmb();
  CCB * ccb = parser.ccbParser().ccb();

  DAQMBTester tester(daqmb, ccb, true);
  tester.runAllTests();

  /*
  TestBeamCrateController tbController;
  std::cout << "Created TestBeamController" << std::endl;  
  
  tbController.configure();
  std::cout << "Finished TestBeamController configure" << std::endl;

  tbController.enable();
  std::cout<< "Finished TestBeamCrateController enable" << std::endl;


  ccb->pulse();
  std::cout << "Finished pulsing ccb" << std::endl;

     
  tbController.disable();
  std::cout<< "Finished TestBeamCrateController disable" << std::endl;
  */



  return 0;
}


