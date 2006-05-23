//-----------------------------------------------------------------------
// $Id: calibpulse.cpp,v 2.18 2006/05/23 14:52:38 mey Exp $
// $Log: calibpulse.cpp,v $
// Revision 2.18  2006/05/23 14:52:38  mey
// Update
//
// Revision 2.17  2006/04/25 13:25:19  mey
// Update
//
// Revision 2.16  2006/03/27 09:51:43  mey
// UPdate
//
// Revision 2.15  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.14  2006/03/24 14:35:46  mey
// UPdate
//
// Revision 2.13  2006/03/07 09:22:58  mey
// Update
//
// Revision 2.12  2006/02/20 13:31:14  mey
// Update
//
// Revision 2.11  2006/01/13 13:12:13  mey
// UPdate
//
// Revision 2.10  2006/01/11 17:00:24  mey
// Update
//
// Revision 2.9  2006/01/10 23:32:47  mey
// Update
//
// Revision 2.8  2006/01/10 19:47:01  mey
// UPdate
//
// Revision 2.7  2005/12/16 17:49:20  mey
// Update
//
// Revision 2.6  2005/12/10 11:24:22  mey
// Update
//
// Revision 2.5  2005/12/10 09:17:03  mey
// Update
//
// Revision 2.4  2005/12/10 08:49:09  mey
// Update
//
// Revision 2.3  2005/12/10 04:51:38  mey
// Update
//
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
#include "ChamberUtilities.h"
#include <unistd.h> // for sleep

#include "CrateSelector.h"
#include "EmuController.h"

using namespace std;

int main(int argc, char **argv)
{
  char * xmlfile;
  //if (argv[1]) {
  //xmlfile = argv[1];
  //}
  //else {
  //std::cout <<" xml filepath is missing! " << std::endl;
  //std::cout <<" usage: calibpulse.exe filepath " << std::endl;
    //    return 0;
  //}

  bool doPulsealct = false;
  bool doPulsecfeb = false;
  bool doPulseComparator = false ;


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

  EmuController emuController;

  CrateSelector theSelector;
  
  //PeripheralCrateParser parser;
  //parser.parseFile(xmlfile);  

  emuController.SetConfFile(xmlfile);
  emuController.init();
  emuController.configure();

  //
  //TMB *thisTMB(0) ;
  //DAQMB* thisDMB(0) ;
  //CCB* thisCCB(0) ;
  //ALCTController *alct(0) ;
  //
  //vector<TMB*>   tmbVector;
  //vector<DAQMB*> dmbVector;
  //Crate *thisCrate;
  //
  int npulses = 1000;
  //
  vector<Crate*> crateVector = theSelector.crates();
  //
  //thisCrate = crateVector[0];
  //tmbVector = theSelector.tmbs(crateVector[0]);
  //dmbVector = theSelector.daqmbs(crateVector[0]);
  //thisCCB   = thisCrate->ccb();

  CalibDAQ calib;

  if ( doPulsecfeb ) {

    //calib.loadConstants();

    //calib.rateTest();
      
    int nsleep, nstrip, tries, counter =0;
    float dac;
    nsleep = 100000;  
    dac = 1.0;
    
    for (int i=0;i<16;i++) {  
      for (int ntim=0;ntim<20;ntim++) {
	calib.pulseAllDMBs(ntim, i, dac, nsleep);  
	counter++;
	std::cout << "dac = " << dac <<
	  "  strip = " << i <<
	  "  ntim = " << ntim <<
	  "  event  = " << counter << std::endl;
      }
    }
  }
  //
  if ( doPulsealct ) {
    //
    std::cout << "Pulse alct" << std::endl;
    //
    //ChamberUtilities utils = (crateVector[0]->chambers())[0] ;
    //
    //printf("Here1\n");
    //
    //if (tmbVector.size() > 0 && dmbVector.size() > 0 ) {
    //thisTMB = tmbVector[0];
    //cout << thisTMB->slot() << endl;
      //thisDMB = dmbVector[0];
      //alct = thisTMB->alctController();
      //if ( alct ) {
    //
    //utils.SetTMB(thisTMB);
    //utils.SetCCB(thisCCB);
    //utils.SetDMB(thisDMB);
    //
    //printf("Here \n");
    //printf("%x \n",&utils);
    //
    //thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
    //utils.CCBStartTrigger();
    //
    for (int ii=0; ii<npulses; ii++) {
      //
      //calib.pulseRandomWires();
      //
      cout << npulses << endl;
      std::cout << "Pulse..." << std::endl;
      //thisTMB->ResetALCTRAMAddress();
      //utils.PulseRandomALCT();
      //std::cout << "Decode ALCT" << std::endl;
      //thisTMB->DecodeALCT();
      //
      //std::cout << "ALCT WordCount  "  << thisTMB->GetALCTWordCount() <<std::endl;
      //
      //thisDMB->PrintCounters();
      //
      
      //printf(" ii = %d \n",ii) ;
      //
      std::cout << std::endl;
      //}
      //
      //}
    }
    //
  }
  //
  return 0;
  //
}


