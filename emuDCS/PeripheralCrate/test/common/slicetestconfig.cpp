//-----------------------------------------------------------------------
// $Id: slicetestconfig.cpp,v 2.0 2005/04/12 08:07:07 geurts Exp $
// $Log: slicetestconfig.cpp,v $
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
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

int main(int argc, char **argv)
{
  char *xmlFile = "configSliceTest.xml";

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
  std::cout << "Created TestBeamController for SliceTest" << std::endl;  
  
  std::cout << "Configuring TestBeamController for SliceTest: " << std::endl; 
  tbController.configure();
  std::cout << "Finished TestBeamCrateController configure for SliceTest" << std::endl;

  // Examples of direct access to some modules 
  //DAQMB * daqmb = parser.daqmbParser().daqmb();
  //CCB * ccb = parser.ccbParser().ccb();
  //DAQMBTester tester(daqmb, ccb, false);
  //tester.runAllTests();

  return 0;
}


