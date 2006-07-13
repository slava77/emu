//-----------------------------------------------------------------------
// $Id: EmuConfig.cpp,v 1.3 2006/07/13 15:46:38 mey Exp $
// $Log: EmuConfig.cpp,v $
// Revision 1.3  2006/07/13 15:46:38  mey
// New Parser strurture
//
// Revision 1.2  2006/02/01 21:03:23  mey
// Got rid of TestBeam
//
// Revision 1.1  2006/02/01 19:45:02  mey
// UPdate
//
// Revision 2.1  2005/06/15 13:54:46  geurts
// Changed default run behavior.
// testbeamcontrol runs a full Configure and Enable. Use -i option for
// interactive use, including the possibility to execute Disable
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#include <iostream>
#include "VMEController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "EmuController.h"
#include "TMB.h"
#include "ALCTController.h"
#include "MPC.h"
#include <unistd.h>
#include "PeripheralCrateParser.h"

int main(int argc, char **argv)
{
  char *xmlFile = "config.xml";
  bool doInteractive(false);

  // process command line arguments
  if (argc>1)
    for (int i=1;i<argc;i++){
      if (!strcmp(argv[i],"-h")){                                                                     
        std::cout << "Usage: " << argv[0] <<"[-f (file="<< xmlFile <<")]" << std::endl;
        exit(0);
      }
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
      if (!strcmp(argv[i],"-i")) doInteractive=true;
    }

 
  std::cout << "PeripheralCrate configuration file: " << xmlFile << std::endl;
  //PeripheralCrateParser parser;
  //parser.parseFile(xmlFile);

  //TestBeamCrateController tbController;
  EmuController emuController;
  emuController.SetConfFile(xmlFile);
  emuController.init();
  std::cout << "Created EmuController" << std::endl;  
  
  std::cout << "Configuring EmuController: " << std::endl; 
  emuController.configure();
  std::cout << "EmuCrateController configured." << std::endl;

  // Examples of direct access to some modules 
  //DAQMB * daqmb = parser.daqmbParser().daqmb();
  //CCB * ccb = parser.ccbParser().ccb();
  //DAQMBTester tester(daqmb, ccb, false);
  //tester.runAllTests();


  // wait for the user to hit return-key before disabling
  if (doInteractive){
    std::cout << "Hit Return to continue ..." << std::endl;
    char wait[10];
    std::cin.getline(wait,1);
  }

  //std::cout << "Enabling EmuController" << std::endl;
  //emuController.enable();
  //std::cout << "EmuController enabled." << std::endl;

  // wait for the user to hit return-key before disabling
  //if (doInteractive){
  //std::cout << "Hit Return to continue ..." << std::endl;
  //char wait[10];
  //std::cin.getline(wait,1);
  
    // only do a disable in the interactive mode
    //emuController.disable();
    //std::cout<< "Finished EmuController disable" << std::endl;
  //}
  
  return 0;
}


