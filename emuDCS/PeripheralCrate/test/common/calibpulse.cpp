//-----------------------------------------------------------------------
// $Id: calibpulse.cpp,v 2.2 2005/12/10 04:47:36 mey Exp $
// $Log: calibpulse.cpp,v $
// Revision 2.2  2005/12/10 04:47:36  mey
// Fix bug
//
// Revision 2.1  2005/09/22 12:59:18  mey
// Alex's changes
//
// Revision 2.0  2005/06/06 10:01:44  geurts
// calibration test routines by Alex Tumanov and Jason Gilmore
//
//
//
//-----------------------------------------------------------------------
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <iomanip>

#include "Crate.h"
#include "DAQMB.h"
#include "DAQMBTester.h"
#include "CCB.h"
#include "TMB.h"
#include "DAQMB.h"
#include "CalibDAQ.h"
#include "PeripheralCrateParser.h"
#include "MPC.h"
#include "CrateUtilities.h"
#include <unistd.h> // for sleep

#include "CrateSelector.h"

using namespace std;

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

  bool doPulsealct = false;
  bool doPulsecfeb = false;


  if (argc>1)
    for (int i=1;i<argc;i++){
      // help: print usage and exit
      if (!strcmp(argv[i],"-h")){
	std::cout << "Usage: " << argv[0] << " [-alct] [-cfeb] " 
	     << ")] [-f (file="<< xmlfile <<")]" 
	     <<std::endl;
	exit(0);
      }
      if (!strcmp(argv[i],"-f"))     xmlfile=argv[++i];
      if (!strcmp(argv[i],"-alct"))  doPulsealct = true;
      if (!strcmp(argv[i],"-cfeb"))  doPulsecfeb = true;
    }

  CrateSelector theSelector;
  
  PeripheralCrateParser parser;
  parser.parseFile(xmlfile);  
  //
  TMB *thisTMB(0) ;
  DAQMB* thisDMB(0) ;
  CCB* thisCCB(0) ;
  ALCTController *alct(0) ;
  //
  vector<TMB*>   tmbVector;
  vector<DAQMB*> dmbVector;
  Crate *thisCrate;
  //
  vector<Crate*> crateVector = theSelector.crates();
  thisCrate = crateVector[0];
  tmbVector = theSelector.tmbs(crateVector[0]);
  dmbVector = theSelector.daqmbs(crateVector[0]);
  thisCCB   = thisCrate->ccb();

  if ( doPulsecfeb ) {

    CalibDAQ calib;
    calib.loadConstants();
  //calib.rateTest();
    
  
    int nsleep, nstrip, tries, counter =0;
    float dac;
    nsleep = 1000;  
    dac = 1.0;
    
    for (int i=1;i<17;i++) {  
      for (int ntim=0;ntim<32;ntim++) {
	calib.pulseAllDMBs(ntim, i, dac, nsleep);  
	counter++;
	std::cout << "dac = " << dac <<
	  "  strip = " << i <<
	  "  ntim = " << ntim <<
	  "  event  = " << counter << std::endl;
      }
    }
  }

  if ( doPulsealct ) {
    //
    std::cout << "Pulse alct" << std::endl;
    //
    CrateUtilities utils;
    //
    if (tmbVector.size() > 0 && dmbVector.size() > 0 ) {
      thisTMB = tmbVector[0];
      cout << thisTMB->slot() << endl;
      thisDMB = dmbVector[0];
      alct = thisTMB->alctController();
      if ( alct ) {
	//
	utils.SetTMB(thisTMB);
	utils.SetCCB(thisCCB);
	utils.SetALCT(alct);
	//
	thisTMB->SetALCTPatternTrigger();
	thisTMB->ResetALCTRAMAddress();
	utils.PulseTestStrips();
	printf("Decode ALCT\n");
	thisTMB->DecodeALCT();
	printf("Decode TMB\n");
	thisTMB->DecodeTMB();
	//
      }
    }
    //
  }

  return 0;
}


