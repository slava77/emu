//-----------------------------------------------------------------------
// $Id: injecttest.cpp,v 2.0 2005/06/06 10:01:44 geurts Exp $
// $Log: injecttest.cpp,v $
// Revision 2.0  2005/06/06 10:01:44  geurts
// calibration test routines by Alex Tumanov and Jason Gilmore
//
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "CalibDAQ.h"
#include "PeripheralCrateParser.h"
#include "TestBeamCrateController.h"
#include "MPC.h"
#include <string>
#include <unistd.h> // for sleep

int main(int argc, char **argv)
{
  char * xmlfile;
  if (argv[1]) {
    xmlfile = argv[1];
  }
  else {
    std::cout <<" xml filepath is missing! " << std::endl;
    std::cout <<" usage: injecttest.exe filepath " << std::endl;
    return 0;
  }

  PeripheralCrateParser parser;
  parser.parseFile(xmlfile);  
 
  CCB * ccb = parser.ccbParser().ccb();
   
  TestBeamCrateController tbController;
  tbController.configure();
  tbController.setAllDMBs(15);

  for (int i=0;i<10;i++) {
    ::usleep(1000);
    //::sleep(3);
    ccb->inject(1, 0xff);
    std::cout << "injecting CCB " << i << std::endl;
  }

  return 0;
}


