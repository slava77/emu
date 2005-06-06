//-----------------------------------------------------------------------
// $Id: calibpulse.cpp,v 2.0 2005/06/06 10:01:44 geurts Exp $
// $Log: calibpulse.cpp,v $
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
    std::cout <<" usage: calibpulse.exe filepath " << std::endl;
    //    return 0;
  }

  PeripheralCrateParser parser;
  parser.parseFile(xmlfile);  
 
  CalibDAQ calib;
  calib.loadConstants();
  calib.rateTest();

  /*
  int nsleep, nstrip, tries, counter =0;
  float dac;
  nsleep = 1000;  
  dac = 1.0;
  nstrip = 10;

  for (int i=0;i<1;i++) {  
    for (int ntim=0;ntim<32;ntim++) {
      calib.pulseAllDMBs(ntim, nstrip, dac, nsleep);  
      counter++;
      std::cout << "dac = " << dac <<
	"  strip = " << nstrip <<
        "  ntim = " << ntim <<
	"  event  = " << counter << std::endl;
    }
  }

  */
  return 0;
}


