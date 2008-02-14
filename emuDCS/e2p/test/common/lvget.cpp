// $Id: lvget.cpp,v 3.1 2008/02/14 16:53:05 geurts Exp $
// $Log: lvget.cpp,v $
// Revision 3.1  2008/02/14 16:53:05  geurts
// updates to e2p for the xdaqr4 environment
//
// Revision 1.16  2004/10/03 01:55:51  tfcvs
// various updates at H2a
//
// Revision 1.15  2004/09/15 21:44:42  tfcvs
// New timing routines
//
// Revision 1.14  2004/09/14 00:27:07  tfcvs
// Update files
//
// Revision 1.13  2004/05/21 10:05:05  tfcvs
// *** empty log message ***
//
// Revision 1.12  2004/05/20 09:52:23  tfcvs
// fixed type in CVS keyword
//
// Revision 1.11  2004/05/20 09:50:32  tfcvs
// Introduced "-f" commandline option to set the configuration file.
// Wait for user input before disabling the TestBeamController. (FG)
//
// ----
#include <iostream>
#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "TestBeamCrateController.h"
#include "TMB.h"
#include "ALCTController.h"
#include "MPC.h"
#include <unistd.h>
#include "PeripheralCrateParser.h"

#include "EmuDcs.h"  // corr++


int main(int argc, char **argv)
{
  char *xmlFile = "configTestBeam2004.xml";

  // process command line arguments
  if (argc>1)
    for (int i=1;i<argc;i++){
      if (!strcmp(argv[i],"-h")){                                                                     
        std::cout << "Usage: " << argv[0] <<"[-f (file="<< xmlFile <<")]" << std::endl;
        exit(0);
      }
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
    }

 
  std::cout << "PeripheralCrate configuration file: " << xmlFile << std::endl;
  PeripheralCrateParser parser;
  parser.parseFile(xmlFile);

  TestBeamCrateController tbController;
  std::cout << "Created TestBeamController" << std::endl;  
  
  std::cout << "Configuring TestBeamController: " << std::endl; 
  tbController.configure();
  std::cout << "Finished TestBeamCrateController configure" << std::endl;

  // Examples of direct access to some modules 
  //DAQMB * daqmb = parser.daqmbParser().daqmb();
  //CCB * ccb = parser.ccbParser().ccb();
  //DAQMBTester tester(daqmb, ccb, false);
  //tester.runAllTests();


  /////  new EmuDcs();   // corr++
  while(1);
  /////Rc2Dcs *rc2dcs= new Rc2Dcs();  // corr++ 
  //// rc2dcs->turnHV(1,500);   // corr++

  // wait for the user to hit return-key before disabling
  {
  std::cout << "Hit Return to continue ..." << std::endl;
  char wait[10];
  std::cin.getline(wait,1);
  }

  std::cout << "Enabling TestBeamCrateController" << std::endl;
  tbController.enable();
  std::cout << "Finished TestBeamCrateController enable!!!!!!!!!!!!!!!!!!" << std::endl;

  // wait for the user to hit return-key before disabling
  {
  std::cout << "Hit Return to continue ..." << std::endl;
  char wait[10];
  std::cin.getline(wait,1);
  }

  tbController.disable();
  std::cout<< "Finished TestBeamCrateController disable" << std::endl;
  
  return 0;
}


