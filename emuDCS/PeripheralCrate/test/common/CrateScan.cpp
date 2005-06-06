//-----------------------------------------------------------------------
// $Id: CrateScan.cpp,v 2.0 2005/06/06 15:13:21 geurts Exp $
// $Log: CrateScan.cpp,v $
// Revision 2.0  2005/06/06 15:13:21  geurts
// utility to quickly scan and check crate integrity (Martin vd Mey)
//
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <string>
#include "PeripheralCrateParser.h"
#include "TestBeamCrateController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "DDU.h"
#include "ALCTController.h"
#include "CrateSelector.h"


using namespace std;

TMB *thisTMB ;
DAQMB* thisDMB ;
CCB* thisCCB ;
ALCTController *alct ;

int main(int argc,char **argv){

  char *xmlFile ="tmbTiming.xml";
  int scantime(1);

  if (argc>1)
    for (int i=1;i<argc;i++){
      // help: print usage and exit                                                                                   
      if (!strcmp(argv[i],"-h")){
        cout << "Usage: " << argv[0] << " [-alct] [-cfeb] [-scope] [-ALCTRawhits] [-TMBRawhits] [-PulseTest] [-Intera\
ctive] [-CFEBTiming] [-CFEBChamberScanning] [-ALCTTiming][-t (time="
             << scantime << ")] [-f (file="<< xmlFile <<")]"
             << "[-scint]"<<endl;
        exit(0);
      }
      if (!strcmp(argv[i],"-t")) scantime=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
    }
  //

  //                                                                                                                  
  //-- Configure and initialize VME modules                                                                           
  //                                                                                                                  
  cout << "CrateScan" << endl;
  //
  //-- parse XML file                                                                                                 
  cout << "---- XML parser ----" << endl;
  PeripheralCrateParser parser;
  parser.parseFile(xmlFile);

  //-- Set-up and configure Testbeam                                                                                  
  cout << " ---- Cosmic Particle Controller ----" << endl;
  TestBeamCrateController tbController;
  //tbController.configure();
  //-- --
  CrateSelector selector = tbController.selector();
  vector<Crate*> crateVector = selector.crates();
  vector<TMB*> tmbVector     = selector.tmbs(crateVector[0]);
  vector<DAQMB*> dmbVector   = selector.daqmbs(crateVector[0]);

  CCB * ccb = crateVector[0]->ccb();

  //if (ccb) ccb->configure();

  printf("Writing...\n");

  for (int i=0; i<tmbVector.size(); i++ ) {
    //tmbVector[i]->configure();
    tmbVector[i]->WriteRegister(0x46,i+10);
  }


  printf("Reading....\n");

  for (int i=0; i<tmbVector.size(); i++ ) {
    tmbVector[i]->ReadRegister(0x46);
  }


  //--- Read TMB 0x00

  for (int i=0; i<tmbVector.size(); i++ ) {
    printf("\n");
    printf("Register 0x46\n");
    tmbVector[i]->ReadRegister(0x46);
    printf("Register 0x0\n");
    tmbVector[i]->ReadRegister(0x0);
    printf("Register 0x10\n");
    tmbVector[i]->ReadRegister(0x10);

    alct = tmbVector[i]->alctController();
    if (alct) {
      int err;
      //alct->setup(1);
      ALCTIDRegister sc_id, chipID ;
      sc_id.chip = 0;
      err = alct->alct_fast_read_id(sc_id) ;
      std::cout <<  " ALCT Fast ID " << sc_id << std::endl;
      err = alct->alct_read_slowcontrol_id(&chipID) ;
      std::cout <<  " ALCT Slowcontrol ID " << chipID << std::endl;

    }
  }

  printf("Reading....\n");

  for (int i=0; i<tmbVector.size(); i++ ) {
    tmbVector[i]->ReadRegister(0x46);
    tmbVector[i]->ReadRegister(0x10);
  }



}
