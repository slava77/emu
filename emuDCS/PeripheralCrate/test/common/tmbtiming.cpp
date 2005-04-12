//-----------------------------------------------------------------------
// $Id: tmbtiming.cpp,v 2.0 2005/04/12 08:07:07 geurts Exp $
// $Log: tmbtiming.cpp,v $
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
//! TMB ALCT/CFEB timing utility
/*! \brief TMB-ALCT/CFEB timing
 * \author Frank Geurts
 * \date January 2004
 * 
 * tmbtiming sets up the PeripheralCrate according to the
 * configuration in the XML file. It will pass on the timing
 * parameter to the TMB routines and runs the PHOS4 timing
 * routines on the first TMB defined in the XML-file.
 * For reference the results are written in two files
 * (tmb_alct.dat and tmb_scan.dat).
 *
 */
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

int beginning = 0;

TMB *thisTMB ;
DAQMB* thisDMB ;
CCB* thisCCB ;
ALCTController *alct ;
//
void CFEBTiming();
void PulseCFEB(int HalfStrip = -1, int CLCTInputs = 0x1f );
void CFEBChamberScanning();
void PulseTestStrips();
void ALCTTiming();
void ALCTChamberScanning();
void ALCTSVFLoad();
void TMBL1aTiming();
void FindBestL1aAlct();

int main(int argc,char **argv){

  //-- default settings
  char *xmlFile ="tmbTiming.xml";
  int scantime(1);
  bool useScint(false);
  bool doPHOS4cfeb(false);
  bool doPHOS4alct(false);
  bool doTMBScope(false);
  bool doALCTRawhits(false);
  bool doTMBRawhits(false);
  bool doPulseTest(false);
  bool doInteractive(false);
  bool doScopeRead(false);
  bool doScopeReadArm(false);
  bool doForceScopeTrigger(false);
  bool doCFEBTiming(false);
  bool doCFEBChamberScanning(false);
  bool doPrintCounters(false);
  bool doResetCounters(false);
  bool doPulseTestStrips(false);
  bool doALCTTiming(false);
  bool doALCTChamberScanning(false);
  bool doALCTSVFLoad(false);
  bool doReadIDCODE(false);
  bool doLv1TMBTiming(false);
  bool doFindBestL1aALCT(false);
  bool doReadALCTThreshold(false);
  //
  //-- read commandline arguments and xml configuration file
  //
  if (argc>1)
    for (int i=1;i<argc;i++){
      // help: print usage and exit
      if (!strcmp(argv[i],"-h")){
	cout << "Usage: " << argv[0] << " [-alct] [-cfeb] [-scope] [-ALCTRawhits] [-TMBRawhits] [-t (time=" 
	     << scantime << ")] [-f (file="<< xmlFile <<")]"
	     << "[-scint]"<<endl;
	exit(0);
      }
      if (!strcmp(argv[i],"-t")) scantime=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
      if (!strcmp(argv[i],"-scint")) useScint = true;
      if (!strcmp(argv[i],"-alct"))  doPHOS4alct = true;
      if (!strcmp(argv[i],"-cfeb"))  doPHOS4cfeb = true;
      if (!strcmp(argv[i],"-scope")) doTMBScope = true;
      if (!strcmp(argv[i],"-ALCTRawhits")) doALCTRawhits = true;
      if (!strcmp(argv[i],"-TMBRawhits")) doTMBRawhits = true;
      if (!strcmp(argv[i],"-PulseTest")) doPulseTest = true;
      if (!strcmp(argv[i],"-Interactive")) doInteractive = true;
      if (!strcmp(argv[i],"-CFEBTiming")) doCFEBTiming = true;
      if (!strcmp(argv[i],"-CFEBChamberScanning")) doCFEBChamberScanning = true;
      if (!strcmp(argv[i],"-ALCTTiming")) doALCTTiming = true;
  }
  //
  //-- Configure and initialize VME modules
  //
  cout << "Running " << argv[0] << ". time="<< scantime
       <<"s. Config="<<xmlFile;
  if (useScint)
    cout << " scintillator trigger" << endl;
  else
    cout << " Anode trigger" << endl;

  //-- parse XML file
  cout << "---- XML parser ----" << endl;
  PeripheralCrateParser parser;
  parser.parseFile(xmlFile);
    
  //-- Set-up and configure Testbeam 
  cout << " ---- Cosmic Particle Controller ----" << endl;
  TestBeamCrateController tbController;

  //-- Make sure that only one TMB in one crate is configured
  CrateSelector selector = tbController.selector();
  vector<Crate*> crateVector = selector.crates();
  if (crateVector.size() > 1){
    cerr << "Error: only one PeripheralCrate allowed" << endl;
    exit(1);
  }
  vector<TMB*> tmbVector = selector.tmbs(crateVector[0]);
  if (tmbVector.size() > 1){
    cerr << "Error: only one TMB in xml file (" << xmlFile 
	 << ") allowed" << endl;
    exit(1);
  }
  vector<DAQMB*> dmbVector = selector.daqmbs(crateVector[0]);
  if (dmbVector.size() > 1){
    cerr << "Error: only one DMB in xml file ("<< xmlFile
	 << ")allowed." << endl;
    exit(1);
  }

  //-- get pointers to CCB, TMB and DMB
  Crate *thisCrate = crateVector[0];
  thisCCB = thisCrate->ccb();
  thisTMB = tmbVector[0];
  thisDMB = dmbVector[0];
  MPC * thisMPC = thisCrate->mpc();
  DDU * thisDDU = thisCrate->ddu();
  //
  //-- Configure the Cosmic Set-up
  cout << "---- Cosmic ParticConfiguration ----"<<endl;
  if (useScint){
    // use default test beam configurator
    //tbController.configure();
  }
  else{ // set-up the anode trigger
    // do the test beam configuration manually
    //  based on TestBeamController::configure()
    
    cout << "-- Configuring CCB --" << endl;
    //thisCCB->configure();
    //sleep(1);

    cout << "-- Configuring DMB --" << endl;
    //thisDMB->restoreCFEBIdle(); // keep
    //thisDMB->restoreMotherboardIdle(); //keep
    //thisDMB->configure(); //keep
    
    cout << "-- Configuring TMB --" << endl;
    //thisTMB->configure(); //keep
    
    cout << "-- Configuring DMB (cont'd)" << endl;
    //thisDMB->calctrl_fifomrst(); //
    //sleep(1);

    cout << "-- Configuring ALCT --" << endl;
    alct = thisTMB->alctController();
    //if (alct) alct->setup(1);

    cout << "-- Configuring CCB (cont'd)" << endl;
    //thisCCB->l1CathodeScint();

    cout << "-- Configuring MPC --" << endl;
    //if(thisMPC) thisMPC->init();
    //sleep(1);

    cout << "-- Resetting DDU registers (via DCNTRL) " << endl;
    //if(thisDDU) thisDDU->dcntrl_reset();
    //sleep(2);
  }

  
  cout << "---- Test Beam Enable ----"<<endl;


  if (useScint){
    //tbController.enable();
  } else {
    // do the test beam configuration manually (assuming CCB2001)
    cout << "-- Enable CCB -- " << endl;
    //thisCCB->enable();
    cout << "-- Re-init MPC" << endl;
    //if (thisMPC) thisMPC->init();
    //thisCCB->enableCLCT();

    //thisTMB->ResetCounters();

    interactive:

    if (doInteractive) {
       printf("%c7", '\033'); 
       printf("%c8", '\033'); 
       printf("%c[01;36m", '\033');
       cout << "What do you want to do today ?"<< endl;
       printf("%c8", '\033'); 
       printf("%c[0m", '\033');
       int Menu;
       cin >> Menu;
       //
       doTMBScope       = false;
       doALCTRawhits    = false;
       doALCTSVFLoad    = false;
       doTMBRawhits     = false;
       doPulseTest      = false;
       doScopeRead      = false;
       doScopeReadArm   = false;
       doForceScopeTrigger   = false;
       doCFEBTiming          = false;
       doCFEBChamberScanning = false;
       doPrintCounters       = false;
       doResetCounters       = false;
       doPulseTestStrips     = false;
       doALCTChamberScanning = false;
       doReadIDCODE          = false;
       doFindBestL1aALCT     = false;
       doLv1TMBTiming        = false;
       doReadALCTThreshold   = false;
       //
       if ( Menu == 1 ) doTMBScope             = true ;
       if ( Menu == 2 ) doALCTRawhits          = true ;
       if ( Menu == 3 ) doTMBRawhits           = true ;
       if ( Menu == 4 ) doPulseTest            = true ;
       if ( Menu == 5 ) doScopeReadArm         = true ;
       if ( Menu == 6 ) doScopeRead            = true ;
       if ( Menu == 7 ) doForceScopeTrigger    = true ;
       if ( Menu == 8 ) doCFEBTiming           = true ;
       if ( Menu == 9 ) doCFEBChamberScanning  = true ;
       if ( Menu == 10) doPrintCounters        = true ;
       if ( Menu == 11) doResetCounters        = true ;
       if ( Menu == 12) doPulseTestStrips      = true ;
       if ( Menu == 13) doALCTChamberScanning  = true ;
       if ( Menu == 14) doALCTSVFLoad          = true ;
       if ( Menu == 15) doReadIDCODE           = true ;
       if ( Menu == 16) doLv1TMBTiming         = true ;
       if ( Menu == 17) doFindBestL1aALCT      = true ;
       if ( Menu == 18) doReadALCTThreshold    = true ;
    }

    if (doReadALCTThreshold ) {
      long readval;
      printf("Number of afebs %d \n",alct->nAfebs());
      for (int i=0; i<alct->nAfebs(); i++) {
	alct->alct_read_thresh(i,&readval);
	printf("Setting AFEB= %d = %d \n",i,readval);
      }
    }

    if (doFindBestL1aALCT){
       FindBestL1aAlct();
    }

    if (doResetCounters){
       thisTMB->ResetCounters();
    }

    if (doPrintCounters){
       thisTMB->GetCounters();
       thisTMB->PrintCounters();
    }

    if (doALCTChamberScanning){
       ALCTChamberScanning();
    }

    if (doALCTSVFLoad) {
      ALCTSVFLoad();
    }

    // TMB Scope
    if (doTMBScope)
       thisTMB->decode();
    
    //ALCT Rawhits
    
    if (doALCTRawhits)
       thisTMB->ALCTRawhits();
    
    if (doTMBRawhits)
       thisTMB->TMBRawhits();
    
    if (doPulseTest){
       PulseCFEB();
    }
    
    if (doCFEBChamberScanning){
       CFEBChamberScanning();
    }
    
    if (doCFEBTiming){
       CFEBTiming();
    }
    
    if (doALCTTiming){
       ALCTTiming();
    }
    
    if (doPulseTestStrips){
       PulseTestStrips();
       thisTMB->DecodeALCT();
    }
  }

  if (doReadIDCODE) {
    cout << "Read IDCODE" << endl;
    alct->ReadIDCODE();
  }
    
  if (doLv1TMBTiming){
     TMBL1aTiming();
  }

  if (doScopeReadArm){
     cout << "Arm Scope" << endl;
     thisTMB->scope(1,0);
  }

  if (doScopeRead){
     cout << "Readout Scope"<< endl;
     thisTMB->scope(0,1);
  }

  if (doForceScopeTrigger){
    cout << "Force Scope Trigger" << endl;
    thisTMB->ForceScopeTrigger();
  }

  if (doInteractive) goto interactive;

  // identify TMB
  cout << dec; // restore to decimal printing ...
  cout << "Hello, i am TMB in crate "<< thisTMB->crate()
       << " at slot "<< thisTMB->slot() 
       << " version " << thisTMB->version_ << endl;

  // scan ALCT-TMB timing
  if (doPHOS4alct){
    cout << "---- PHOS4 alct timing ----" << endl;
    
    // scan ALCT-TMB rx,tx settings and build histograms
    // UFL cosmic set-up: use 5-10s per scan evt.
    // beam test: 0s should be enough.
    thisTMB->tmb_PHOS4_alct(scantime);
  }

  // scan CFEB-TMB timing.
  if (doPHOS4cfeb){
    cout << "---- PHOS4 cfeb timing ----" << endl;
    thisTMB->tmb_PHOS4_cfeb();
  }

  // done: disable triggering
  cout << "---- Test Beam Disable ----"<<endl;
  if (useScint)
    tbController.disable();
  else {
    thisCCB->disableL1();
    thisCCB->disable();
    sleep(1);
  }
  cout<< "---- done ----" << endl;


  //----------------------------------------------------//

  // maximum scan range on the PHOS4
  int scanMax(25);
  if (thisTMB->version_ == "2004") scanMax=13;

  int array1[scanMax][scanMax],
      array2[scanMax][scanMax],
      array3[scanMax][scanMax];

  //-- Read back the configuration files --//
  // replace this by making the wordcount1 arrays availabe
  // (protected) as members of TMB

  // open file (created by tmb_PHOS4_alct)
  ifstream alctfile("tmb_alct.dat",ios::binary|ios::in);
  if(!alctfile){
    cerr << "Unable to open file tmb_alct.dat. Exiting"<< endl;
    exit(1);
  }

  // get array1
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array1[j][k];
    
  // skip the intermediate line
  char dummy[80];
  alctfile.getline(dummy,80,'\n');

  // get array2 (not used)
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array2[j][k];

  // skip all the intermediate crap
  for (int i=0;i<12;i++)
    alctfile.getline(dummy,80,'\n');

  // get array3
  for (int j=0;j<scanMax;j++)
    for (int k=0;k<scanMax;k++)
      alctfile >> array3[j][k];


  //-- ALCT-TMB timing --//
  cout << "-- ALCT-TMB timing (cf. tmb_alct.dat) --" << endl;

  // 1.determine the tx delay
  int minValidRx(scanMax); // determines a valid tx scan (should be scanMax);
  int validTx(0),firstTx(-1);

  // find acceptable tx regio
  for (int tx=0;tx<scanMax;tx++){
    int validRx(0);
    for (int rx=0;rx<scanMax;rx++){
      if (array1[tx][rx]==0) validRx++;      
    }
    if (validRx >= minValidRx){
      if (firstTx ==-1) firstTx=tx;
      validTx++;
    }
    cout << tx << " " << validRx << " " << firstTx << " " << validTx << endl;
  }

  int alctTx;
  if (firstTx!=-1){
     alctTx = firstTx + validTx/2;
    cout << "tx = " << alctTx << endl;
  } else {
    cout << "unable to determine tx" << endl;
    exit(1);
  }


  // 2. use tx and determine rx delay

  int firstRx(-1), validRx(0);
  for (int rx=0;rx<scanMax;rx++){
    if (array3[alctTx][rx]){
      if (firstRx==-1) firstRx = rx;
      validRx++;
    }
  }

  int alctRx(-1);
  if (firstRx!=-1){
    alctRx = firstRx + validRx/2;
    cout << " rx = " << alctRx << endl;
  } else {
    cout << "unable to determine rx" << endl;
    exit(1);
  }


  //-- CFEB-TMB timing --//
  cout << "-- CFEB-TMB timing (cf. tmb_scan.dat) --" << endl;

  // work in progress ...
  
  
}

void ALCTSVFLoad(){
  int jch = 3;
  char* filename="/home/fastcosmic/Erase.svf";
  alct->NewSVFLoad(&jch,filename,10);
}

void ALCTChamberScanning(){
   //
   unsigned long HCmask[22];
   //
   int NPulses = 1;
   int chamberResult[64];
   int chamberResult2[64];
   int InJected[64];
   //
   for ( int keyWG=0; keyWG<64; keyWG++) chamberResult[keyWG] = 0;
   for ( int keyWG=0; keyWG<64; keyWG++) chamberResult2[keyWG] = 0;
   for ( int keyWG=0; keyWG<64; keyWG++) InJected[keyWG] = 0;
   //
   for (int Ninject=0; Ninject<NPulses; Ninject++){
      //
      for (int keyWG=0; keyWG<32; keyWG++) {
	 //
	 printf("%c[01;41m", '\033');
	 cout << "Injecting in WG = " << dec << keyWG << endl;
	 printf("%c[0m", '\033'); 
	 //
	 for (int i=0; i<12; i++) {
	    if ( i%2 == 0 ) {
	       if (keyWG<32) {
		  HCmask[i] = 0x1<<keyWG;
	       } else {
		  HCmask[i] = 0x1<<(keyWG-32);
	       }
	    } else {
	       if (keyWG<32) {
		  HCmask[i] = 0x1<<(keyWG) ;
	       } else {
		  HCmask[i] = 0x1<<(keyWG-32);
	       }
	    }
	 }
	 alct->alct_write_hcmask(HCmask);
	 //
	 alct->alct_read_hcmask(HCmask);
	 for (int i=0; i<12; i++ ) {
	    if (i == 0 ) cout << "Layer 0 " ;
	    for(int j=0; j<32; j++ ) 
	       if (((HCmask[i]>>j)&0x1) == 0 ) {
		  cout <<"-";
	       } else {
		  printf("%c[01;44m", '\033');	 
		  cout <<"|";
		  printf("%c[0m", '\033'); 
	       }
	    if ((i+1)%2 == 0 ) {
	       cout << endl;
	       if (i+1<12) cout << "Layer " << (i+1)/2 << " ";
	    }
	 }
	 cout << endl;
	 //
	 PulseTestStrips();
	 //
	 thisTMB->DecodeALCT();
	 //
	 if ( thisTMB->GetAlct0FirstKey()  == keyWG && thisTMB->GetAlct0Quality() == 3 ) 
	    chamberResult[keyWG]++;
	 if ( thisTMB->GetAlct0FirstKey()  == keyWG+32 && thisTMB->GetAlct0Quality() == 3 ) 
	    chamberResult[keyWG+32]++;
	 if ( thisTMB->GetAlct0FirstKey()  == keyWG-32 && thisTMB->GetAlct0Quality() == 3 ) 
	    chamberResult[keyWG-32]++;
	 //
	 if ( thisTMB->GetAlct1SecondKey() == keyWG+32 && thisTMB->GetAlct1Quality() == 3 ) 
	    chamberResult2[keyWG+32]++;
	 if ( thisTMB->GetAlct1SecondKey() == keyWG && thisTMB->GetAlct1Quality() == 3 ) 
	    chamberResult2[keyWG]++;
	 if ( thisTMB->GetAlct1SecondKey() == keyWG-32 && thisTMB->GetAlct1Quality() == 3 ) 
	    chamberResult2[keyWG-32]++;
	 //
	 InJected[keyWG]++;
	 if ( keyWG<32) {
	    InJected[keyWG+32]++;
	 } else {
	    InJected[keyWG-32]++;
	 }
	 //
      }
   }
   cout << endl;
   for (int keyWG=0; keyWG<64; keyWG++) cout << keyWG/10 ;
   cout << endl;
   for (int keyWG=0; keyWG<64; keyWG++) cout << keyWG%10 ;
   cout << endl;
   cout << "InJected" << endl;
   for (int keyWG=0; keyWG<64; keyWG++) cout << InJected[keyWG] ;
   cout << endl;
   cout << "CFEBChamberResult" << endl;
   for (int keyWG=0; keyWG<64; keyWG++) cout << chamberResult[keyWG] ;
   cout << endl;
   for (int keyWG=0; keyWG<64; keyWG++) cout << chamberResult2[keyWG] ;
   cout << endl;
   printf("%c[0m", '\033'); 
}

void ALCTTiming(){
   //
   int maxTimeBins(13);
   int selected[13][13];
   int selected2[13][13];
   int selected3[13][13];
   int ALCTWordCount[13][13];
   int j,k;
   int alct0_quality = 0;
   int alct1_quality = 0;
   int alct0_bxn = 0;
   int alct1_bxn = 0;
   int alct0_key = 0;
   int alct1_key = 0;
   //
   unsigned cr[3]  = {0x80fc5fc0, 0x20a0f786, 0x8}; // Configuration for this test L1a_delay=120 L1a_window=0xf
   alct->SetConf(cr,1);
   alct->unpackControlRegister(cr);
   thisTMB->SetALCTPatternTrigger();
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 selected[j][k] = 0;
	 selected2[j][k] = 0;
	 selected3[j][k] = 0;
      }
   }
   //
   unsigned long HCmask[22];
   //
   int keyWG = 16;
   for (int i=0; i<12; i++) {
      if ( i%2 == 0 ) {
	 if (keyWG<32) {
	    HCmask[i] = 0x1<<keyWG;
	 } else {
	    HCmask[i] = 0x1<<(keyWG-32);
	 }
      } else {
	 if (keyWG<32) {
	    HCmask[i] = 0x1<<(keyWG) ;
	 } else {
	    HCmask[i] = 0x1<<(keyWG-32);
	 }
      }
   }
   alct->alct_write_hcmask(HCmask);
   alct->alct_read_hcmask(HCmask);
   //
   for (int i=0; i<12; i++ ) {
      if (i == 0 ) cout << "Layer 0 " ;
      for(int j=0; j<32; j++ ) 
	 if (((HCmask[i]>>j)&0x1) == 0 ) {
	    cout <<"-";
	 } else {
	    printf("%c[01;44m", '\033');	 
	    cout <<"|";
	    printf("%c[0m", '\033'); 
	 }
      if ((i+1)%2 == 0 ) {
	 cout << endl;
	 if (i+1<12) cout << "Layer " << (i+1)/2 << " ";
      }
   }
   cout << endl;
//
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
	 cout << "Setting k=" << k << " j="<<j << endl;
	 thisTMB->tmb_clk_delays(k,5) ;
	 thisTMB->tmb_clk_delays(j,6) ;	 
	 thisTMB->ResetALCTRAMAddress();
	 PulseTestStrips();
	 printf("Decode ALCT\n");
	 thisTMB->DecodeALCT();
	 printf("After Decode ALCT\n");
	 //
	 selected[k][j]  = 0;
	 selected2[k][j] = 0;
	 selected3[k][j] = 0;
	 //
	 printf("Check data \n");
	 if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	      (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	      (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	      (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	      (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	      (thisTMB->GetAlct1SecondKey() != alct1_bxn) )
	 {
	   alct0_quality     = thisTMB->GetAlct0Quality();
	   alct1_quality     = thisTMB->GetAlct1Quality();
	   alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	   alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	   alct0_key         = thisTMB->GetAlct0FirstKey();
	   alct1_key         = thisTMB->GetAlct1SecondKey();
	   ALCTWordCount[k][j] = thisTMB->GetALCTWordCount();
	   //
	   if ( alct0_quality == 3 && alct0_key == 16) selected[k][j]++;
	   if ( alct1_quality == 3 && alct1_key == 16) selected[k][j]++;
	   if ( alct0_quality == 3 && alct0_key == 48) selected2[k][j]++;
	   if ( alct1_quality == 3 && alct1_key == 48) selected2[k][j]++;
	   if ( (alct0_quality == 3 && alct0_key == 16) &&
		(alct1_quality == 3 && alct1_key == 16) ) selected3[k][j]++;
	   if ( (alct0_quality == 3 && alct0_key == 48) &&
		(alct1_quality == 3 && alct1_key == 48) ) selected3[k][j]++;
	   if ( (alct0_quality == 3 && alct0_key == 16) &&
		(alct1_quality == 3 && alct1_key == 48) ) selected3[k][j]++;
	   if ( (alct0_quality == 3 && alct0_key == 48) &&
		(alct1_quality == 3 && alct1_key == 16) ) selected3[k][j]++;
	   //
	 }	      
      }
   }
   //
   cout << endl;
   //
   cout << "Selected 1 " << endl;
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "Selected 2 " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected2[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
//
   cout << "Selected 3 " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 cout << selected3[j][k] << " " ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "WordCount " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 printf("%02x ",ALCTWordCount[j][k]&0xff);
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << "Result " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	 if ( selected3[j][k] ) {
	    if(ALCTWordCount[j][k]) printf("%c[01;35m", '\033');	 
	    printf("%04d ",ALCTWordCount[j][k]&0xffff);
	    printf("%c[01;0m", '\033');	 
	 } else {
	    printf("%04d ",0x0);
	 }
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << endl;
   //
}
void FindBestL1aAlct(){
  //
  // Now find the best L1a_delay value for the ALCT
  //
  thisTMB->SetALCTPatternTrigger();
  unsigned long HCmask[22];
  //
  int keyWG = 16;
  for (int i=0; i<12; i++) {
    if ( i%2 == 0 ) {
      if (keyWG<32) {
	HCmask[i] = 0x1<<keyWG;
      } else {
	HCmask[i] = 0x1<<(keyWG-32);
      }
    } else {
      if (keyWG<32) {
	HCmask[i] = 0x1<<(keyWG) ;
      } else {
	HCmask[i] = 0x1<<(keyWG-32);
      }
    }
  }
  alct->alct_write_hcmask(HCmask);
  alct->alct_read_hcmask(HCmask);
  //
  for (int i=0; i<12; i++ ) {
    if (i == 0 ) cout << "Layer 0 " ;
    for(int j=0; j<32; j++ ) 
      if (((HCmask[i]>>j)&0x1) == 0 ) {
	cout <<"-";
      } else {
	printf("%c[01;44m", '\033');	 
	cout <<"|";
	printf("%c[0m", '\033'); 
      }
    if ((i+1)%2 == 0 ) {
      cout << endl;
      if (i+1<12) cout << "Layer " << (i+1)/2 << " ";
    }
  }
  cout << endl;
  //
  //
  int WordCount[200];
  for (int i=0; i<200; i++) WordCount[i] = 0;
  //
  //unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default conf register
  //
  unsigned cr[3];
  //
  alct->GetConf(cr,1);
  //
  int minlimit = 100;
  int maxlimit = 130;
  //
  for (int i=minlimit; i<maxlimit; i++) {
    while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
    cr[1] = (cr[1] & 0xfffff00f) | ((i&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    thisTMB->ResetALCTRAMAddress();
    PulseTestStrips();
    thisTMB->DecodeALCT();
    WordCount[i] = thisTMB->GetALCTWordCount();
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  }
  //
  for (int i=minlimit; i<maxlimit; i++){
    printf(" Value = %d WordCount = %x \n",i,WordCount[i]);
  }
  //
}

void PulseTestStrips(){
   //
   int slot = thisTMB->slot();
   //
   if ( alct ) {
      //
      long int StripMask = 0x3f;
      long int PowerUp = 1 ;
      long int Amplitude = 0xff;
      //
      if ( beginning == 0 ) {
	 //
	 thisTMB->DisableCLCTInputs();
	 //
	 alct->alct_set_test_pulse_amp(&slot,Amplitude);
	 alct->alct_set_test_pulse_stripmask(&slot,StripMask);
	 alct->alct_set_test_pulse_powerup(&slot,PowerUp);
	 //
	 alct->alct_read_test_pulse_stripmask(&slot,&StripMask);
	 cout << " StripMask = " << hex << StripMask << endl;
	 alct->alct_read_test_pulse_powerup(&slot,&PowerUp);
	 cout << " PowerUp   = " << hex << PowerUp << endl;
	 //
	 alct->alct_fire_test_pulse('s');
	 //
	 beginning = 1;
	 PulseTestStrips();
      } else {
	thisCCB->GenerateAlctAdbSync();	 
      }
      //
   } else {
      cout << " No ALCT " << endl;
   }   
   //
}

void PulseCFEB(int HalfStrip, int CLCTInputs ){
   //
   thisTMB->DisableCLCTInputs();
   //
   thisDMB->set_comp_thresh(0.3);
   thisDMB->set_dac(.5,0);
   //
   if (HalfStrip == -1 ) HalfStrip = int(rand()*31./(RAND_MAX+1.0));
   //
   cout << endl;
   cout << " -- Injecting in " << HalfStrip << endl;
   cout << endl;
   //
   int hp[6] = {HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip, HalfStrip};       
   //
   // Set the pattern
   //
   thisDMB->trigsetx(hp);
   //
   thisTMB->EnableCLCTInputs(CLCTInputs);
   //
   // Inject it (pulse the CFEBs)
   //
   thisDMB->inject(1,0x4f);
   //
   // Decode the TMB CLCTs (0 and 1)
   //
   thisTMB->DecodeCLCT();
   //
   cout << endl ;
   //
}

void CFEBChamberScanning(){
   //
   int MaxStrip = 32;
   int Muons[5][MaxStrip];
   int MuonsMaxHits[5][MaxStrip];
   //
   for (int i=0;i<5;i++) 
      for (int j=0; j<MaxStrip; j++) {
	 Muons[i][j] = 0;
	 MuonsMaxHits[i][j] = 0;
      }
   //
   int CLCTInputList[3] = {0x1,0xa,0x14};
   //
   for (int List=0; List<3; List++){
     //
     //
     // Number of pulses = 10
     //
     for (int Nmuons=0; Nmuons<10; Nmuons++){
       //
       for (int Strip=0; Strip<MaxStrip; Strip++) {
	 //
	 printf("Enabling Inputs %x \n",CLCTInputList[List]);
	 //
	 PulseCFEB(Strip,CLCTInputList[List]);
	 //
	 int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	 int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	 int clct0nhit = thisTMB->GetCLCT0Nhit();
	 int clct1nhit = thisTMB->GetCLCT1Nhit();
	 int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	 int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	 //
	 cout << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	 cout << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	 cout << " clct0keyHalfStrip " << clct0keyHalfStrip << " clct1keyHalfStrip " << clct1keyHalfStrip << endl;
	 //
	 if ( clct0keyHalfStrip == Strip && MuonsMaxHits[clct0cfeb][Strip] < clct0nhit ) 
	       MuonsMaxHits[clct0cfeb][Strip] = clct0nhit ;
	 if ( clct1keyHalfStrip == Strip && MuonsMaxHits[clct1cfeb][Strip] < clct1nhit ) 
	   MuonsMaxHits[clct1cfeb][Strip] = clct1nhit ;
	 //
	 if ( clct0nhit == 6 && clct0keyHalfStrip == Strip ) Muons[clct0cfeb][Strip]++;
	 if ( clct1nhit == 6 && clct1keyHalfStrip == Strip ) Muons[clct1cfeb][Strip]++;
	 //
	 cout << endl ;
	 //
	 }
     }      
   }
   //
   cout << endl;
   cout << " Number of Muons seen " << endl;
   //
   for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      cout << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
	 cout << setw(3) << Muons[CFEBs][HalfStrip] ;
      }
      cout << endl;
   }
   //
   cout << endl;
   //
   cout << " Maximum number of hits " << endl;
   //
   for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      cout << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
	 cout << setw(3) << MuonsMaxHits[CFEBs][HalfStrip] ;
      }
      cout << endl;
   }
}

void TMBL1aTiming(){
   //
   int wordcounts[200];
   int nmuons = 1;
   //
   for (int delay=0;delay<200;delay++) wordcounts[delay] = 0;
   //
   int minlimit = 110;
   int maxlimit = 140;
   //
   for( int delay=minlimit; delay<maxlimit; delay++){
     //
     printf("delay %d\n",delay);
     //
      thisTMB->lvl1_delay(delay);
      //
      for (int Nmuons=0; Nmuons<nmuons; Nmuons++){
	while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
	thisTMB->ResetRAMAddress();
	PulseCFEB(16,0xa);
	wordcounts[delay] += thisTMB->GetWordCount();
	//printf(" WordCount %d \n",thisTMB->GetWordCount());
      }
   }
   //
   for (int delay=minlimit;delay<maxlimit;delay++){
      printf("delay = %d wordcount = %d wordcount/nmuons %d \n",delay,wordcounts[delay],wordcounts[delay]/(nmuons));
   }
   //
}

void CFEBTiming(){
   //
   int MaxTimeDelay=13;
   //
   int Muons[5][MaxTimeDelay];
   //
   for(int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) {
      for(int CFEBs=0; CFEBs<5; CFEBs++) {
	 Muons[CFEBs][TimeDelay] = 0;
      }
   }
   //
   for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){
      //
      cout << " Setting TimeDelay to " << TimeDelay << endl;
      //
      thisTMB->tmb_clk_delays(TimeDelay,0) ;
      thisTMB->tmb_clk_delays(TimeDelay,1) ;
      thisTMB->tmb_clk_delays(TimeDelay,2) ;
      thisTMB->tmb_clk_delays(TimeDelay,3) ;
      thisTMB->tmb_clk_delays(TimeDelay,4) ;
      //
      int CLCTInputList[3] = {0x1,0xa,0x14};
      //
      for (int List=0; List<3; List++){
	 //
	 for (int Nmuons=0; Nmuons<10; Nmuons++){
	    //
	    PulseCFEB(16,CLCTInputList[List]);
	    //
	    cout << " TimeDelay " << TimeDelay << " CLCTInput " 
		 << CLCTInputList[List] << " Nmuons " << Nmuons << endl;
	    //
	    int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	    int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	    int clct0nhit = thisTMB->GetCLCT0Nhit();
	    int clct1nhit = thisTMB->GetCLCT1Nhit();
	    int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	    int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	    //
	    cout << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	    cout << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	    //
	    if ( clct0nhit == 6 && clct0keyHalfStrip == 16 ) Muons[clct0cfeb][TimeDelay]++;
	    if ( clct1nhit == 6 && clct1keyHalfStrip == 16 ) Muons[clct1cfeb][TimeDelay]++;
	    //
	 }
      }
   }
   //
   cout << endl;
   for (int CFEBs=0; CFEBs<5; CFEBs++) {
      cout << "CFEB Id=" << CFEBs << " " ;
      for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) 
	 cout << " " << Muons[CFEBs][TimeDelay]  ;
      cout << endl;
   }
   //
   cout << endl;
   //
}



