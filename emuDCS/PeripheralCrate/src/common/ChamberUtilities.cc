//-----------------------------------------------------------------------
// $Id: ChamberUtilities.cc,v 1.23 2006/05/10 10:07:56 mey Exp $
// $Log: ChamberUtilities.cc,v $
// Revision 1.23  2006/05/10 10:07:56  mey
// Update
//
// Revision 1.22  2006/05/10 09:54:20  mey
// Update
//
// Revision 1.21  2006/05/09 14:56:29  mey
// Update
//
// Revision 1.20  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 1.19  2006/04/21 11:50:49  mey
// include/EmuPeripheralCrate.h
//
// Revision 1.18  2006/04/05 22:46:24  mey
// Bug fix
//
// Revision 1.17  2006/03/30 13:55:38  mey
// Update
//
// Revision 1.16  2006/03/28 10:44:21  mey
// Update
//
// Revision 1.15  2006/03/24 16:40:36  mey
// Update
//
// Revision 1.14  2006/03/24 14:35:04  mey
// Update
//
// Revision 1.13  2006/03/23 08:24:58  mey
// Update
//
// Revision 1.12  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 1.11  2006/02/21 12:44:00  mey
// fixed bug
//
// Revision 1.10  2006/02/20 13:31:14  mey
// Update
//
// Revision 1.9  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 1.8  2006/02/15 10:49:13  mey
// Fixed pulsing for ME1
//
// Revision 1.7  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 1.6  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 1.5  2006/01/18 12:46:48  mey
// Update
//
// Revision 1.4  2006/01/16 20:29:06  mey
// Update
//
// Revision 1.3  2006/01/12 22:36:27  mey
// UPdate
//
// Revision 1.2  2006/01/12 11:32:43  mey
// Update
//
// Revision 1.1  2006/01/11 08:54:15  mey
// Update
//
// Revision 1.5  2005/12/16 17:49:39  mey
// Update
//
// Revision 1.4  2005/12/14 08:32:36  mey
// Update
//
// Revision 1.3  2005/12/06 13:30:10  mey
// Update
//
// Revision 1.2  2005/11/30 14:58:02  mey
// Update tests
//
// Revision 1.1  2005/10/28 13:09:04  mey
// Timing class
//
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
//
#include "ChamberUtilities.h"
//
using namespace std;
//
ChamberUtilities::ChamberUtilities(){
  //
  ALCTvpf       = -1;
  TMBL1aTiming_ = -1;
  ALCTrxPhase_  = -1;
  ALCTtxPhase_  = -1;
  BestALCTL1aDelay_ = -1;
  ALCTL1aDelay_     = -1;
  beginning         = 0;
  thisTMB = 0;
  thisDMB = 0;
  //
  for( int i=0; i<5; i++) CFEBrxPhase_[i] = -1;
  UsePulsing = true ;
  UseCosmic = false ;
  for (int i=0;i<5;i++) 
    for (int j=0; j<32; j++) {
      CFEBStripScan_[i][j] = -1;
    }
  for (int i=0;i<112;i++) ALCTWireScan_[i] = -1;
  Npulses_ = 2;
  //
  MyOutput_ = &std::cout ;
  //
  //cout << "ChamberUtilities" << endl ;
  //
}
//
//
ChamberUtilities::~ChamberUtilities(){
  //
  //
}
//
void ChamberUtilities::InitStartSystem(){
  //
  cout << "Init System " << endl ;
  //
  //emuController.configure();               // Init system
  //
  thisCCB_->configure();
  if (thisTMB) thisTMB->configure();
  if (thisDMB) thisDMB->configure();
  if (thisMPC) thisMPC->configure();
  if (alct)    alct->setup(1);
  //
  if (thisTMB) thisTMB->StartTTC();
  if (thisTMB) thisTMB->EnableL1aRequest();
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
//
void ChamberUtilities::InitSystem(){
  //
  cout << "Init Chamber " << endl ;
  //
  //emuController.configure();               // Init system
  //
  thisCCB_->configure();
  if (thisTMB) thisTMB->configure();
  if (thisDMB) thisDMB->configure();
  if (thisMPC) thisMPC->configure();
  if (alct)    alct->setup(1);
  //
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
//
int ChamberUtilities::AdjustL1aLctDMB(){
  //
  thisDMB->readtimingCounter();
  if ( thisDMB->GetL1aLctCounter() == 0 ) {
    printf(" ****************************** \n");
    printf(" *** You need to enable DMB *** \n");
    printf(" ****************************** \n");
    //return -1;
  }
  //
  int Counter = -1 ;
  //
  for( int l1a=90; l1a<110; l1a++) {
    thisCCB_->SetL1aDelay(l1a);
    sleep(2);
    thisDMB->readtimingCounter();
    Counter = thisDMB->GetL1aLctCounter() ;
    printf(" ************ L1a lct counter l1a=%d Counter=%d \n ",l1a,Counter);
    //
    if ( Counter >= 115 && Counter <= 117 ) break ;
    //
  }
  //
  return Counter;
  //
}
//
void ChamberUtilities::CCBStartTrigger(){
  //
  //printf("CCB %x \n",thisCCB_);
  //printf("MPC %x \n",thisMPC);
  //
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  thisCCB_->startTrigger();
  thisCCB_->bc0(); 
  //
}
//
void ChamberUtilities::Automatic(){
  //
  ///////////////////////////////////////////////////////////////// Do AdjustL1aLctDMB()
  //
  //InitStartSystem();
  //
  // Enable all...for DMB....be careful....
  //
  //CCBStartTrigger();
  //
  //BestCCBDelaySetting = AdjustL1aLctDMB();
  //
  //cout << "Best L1a Lct DMB CCB delay "<< BestCCBDelaySetting << endl ;
  //
  //if (BestCCBDelaySetting == -1 ) return ;
  //
  ///////////////////////////////////////////////////////////////// Do CFEB phases
  //
  InitStartSystem();
  //
  // Now set L1a delay
  //
  //thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  CFEBTiming();
  //
  //for( int CFEBs=0; CFEBs<5; CFEBs++ ) printf(" %f ",GetCFEBrxPhase(CFEBs));
  //printf("\n");
  //
  //////////////////////////////////////////////////////////////// Do ALCT phases
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  //thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  //for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  //int ALCT_L1a_delay_pulse = 110;
  //
  //ALCT_L1a_delay_pulse += 5;
  // 
  //unsigned cr[3];
  //alct->GetConf(cr,1);
  //cr[1] = (cr[1] & 0xfffff00f) | ((ALCT_L1a_delay_pulse&0xff)<<4) ;
  //alct->SetConf(cr,1);
  //alct->unpackControlRegister(cr);
  //
  ALCTTiming();
  //
  //printf("ALCT_L1a_delay = %d \n",ALCT_L1a_delay_pulse);
  // 
  //int input;
  //cin >> input ;
  //
  //
  //printf("ALCT_L1a_delay = %d \n",ALCT_L1a_delay_pulse);
  //
  InitStartSystem();
  //
  thisTMB->SetCFEB0delay(CFEBrxPhase_[0]);
  thisTMB->SetCFEB1delay(CFEBrxPhase_[1]);
  thisTMB->SetCFEB2delay(CFEBrxPhase_[2]);
  thisTMB->SetCFEB3delay(CFEBrxPhase_[3]);
  thisTMB->SetCFEB4delay(CFEBrxPhase_[4]);
  //
  CFEBChamberScan();
  //
  InitStartSystem();
  //
  thisTMB->SetAlctTXclockDelay(ALCTtxPhase_);
  thisTMB->SetAlctRXclockDelay(ALCTrxPhase_);
  //
  ALCTChamberScan();
  //
  /*
  ////////////////////////////////////////////////////////////////// Do TMB L1a timing
  //
  // Init System again
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(GetALCTrxPhase(),5) ;
  thisTMB->tmb_clk_delays(GetALCTtxPhase(),6) ;	 
  //
  TMB_L1a_timing = TMBL1aTiming(); // Use pulsing
  //
  // Init System again
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(GetALCTrxPhase(),5) ;
  thisTMB->tmb_clk_delays(GetALCTtxPhase(),6) ;	 
  //
  int TMB_L1a_delay = FindTMB_L1A_delay(100,150); // Use real data
  //
  printf(" TMB_l1a_timing=%d TMB_L1a_delay=%d \n ",TMB_L1a_timing, TMB_L1a_delay);
  //
  //int input;
  //cin >> input;
  //
  ////////////////////////////////////////////////////////////////////// Do ALCT L1a timing
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(GetALCTrxPhase(),5) ;
  thisTMB->tmb_clk_delays(GetALCTtxPhase(),6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  Find_ALCT_L1a_delay = FindBestL1aAlct(); // Use pulsing
  //
  InitStartSystem();
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(GetALCTrxPhase(),5) ;
  thisTMB->tmb_clk_delays(GetALCTtxPhase(),6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  ALCT_L1a_delay = FindALCT_L1A_delay(50,150); // Use real data
  //
  printf("ALCT delay %d %d \n",Find_ALCT_L1a_delay,ALCT_L1a_delay); 
  //
  //int input2;
  //cin >> input2;
  //
  //////////////////////////////////////////////////////////////////////// Get alct vpf
  //
  InitStartSystem();
  //
  // Now set CCB delay
  //
  thisCCB_->SetL1aDelay(BestCCBDelaySetting);
  //
  // Now set new CFEB phases
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) thisTMB->tmb_clk_delays(int(CFEBMean[CFEBs]),CFEBs) ;
  //
  // Now set new ALCT phases
  //
  thisTMB->tmb_clk_delays(GetALCTrxPhase(),5) ;
  thisTMB->tmb_clk_delays(GetALCTtxPhase(),6) ;	 
  //
  // Now set L1a TMB delay
  //
  thisTMB->lvl1_delay(TMB_L1a_delay);
  //
  // Now set L1a ALCT delay
  //
  alct->GetConf(cr,1);
  cr[1] = (cr[1] & 0xfffff00f) | ((ALCT_L1a_delay&0xff)<<4) ;
  alct->SetConf(cr,1);
  alct->unpackControlRegister(cr);
  //
  ALCTvpf = FindALCTvpf(); // Use data for this
  //
  ////////////////////////////////////////////////////////////////////////////////////// Result
  //
  // Result
  // 
  printf("******************** End result ********************\n");
  printf("CFEB phases : \n");
  for( int CFEBs=0; CFEBs<5; CFEBs++) printf("CFEBs%d= %f ",CFEBs,CFEBMean[CFEBs]);
  printf("\n");
  printf(" Best CCB delay  ; \n",BestCCBDelaySetting);      
  printf("ALCT phases    : ");
  printf("ALCT RX=%d TX=%d \n",GetALCTrxPhase(),GetALCTtxPhase());  
  printf("TMB L1a delay  : ");
  printf("TMB_L1a_delay=%d \n",TMB_L1a_delay);
  printf("ALCT L1a delay : ");
  printf("ALCT_L1a_delay=%d ",ALCT_L1a_delay);
  if (ALCT_L1a_delay == 0 ) printf(" *** Failed *** ") ;
  printf("\n");
  printf("ALCTvpf        : ");
  printf("ALCTvpf=%d \n",ALCTvpf);
  //
  */
}
//
void ChamberUtilities::ALCTSVFLoad(){
  int jch = 3;
  char* filename="/home/fastcosmic/Erase.svf";
  alct->NewSVFLoad(&jch,filename,10);
}
//
void ChamberUtilities::ALCTChamberScan(){
   //
   unsigned long HCmask[22];
   unsigned long HCmask2[22];
   //
   int NPulses = 1;
   int chamberResult[112];
   int chamberResult2[112];
   int InJected[112];
   //
   std::cout << " *** New ************* " << std::endl ;
   //
   thisTMB->SetALCTPatternTrigger();
   //
   for ( int keyWG=0; keyWG<112; keyWG++) chamberResult[keyWG] = 0;
   for ( int keyWG=0; keyWG<112; keyWG++) chamberResult2[keyWG] = 0;
   for ( int keyWG=0; keyWG<112; keyWG++) InJected[keyWG] = 0;
   //
   for (int Ninject=0; Ninject<NPulses; Ninject++){
      //
      for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) {
	 //
	std::cout << std::endl;
	printf("%c[01;43m", '\033');
	std::cout << "Injecting in WG = " << dec << keyWG ;
	printf("%c[0m", '\033'); 
	std::cout << endl;
	//
	 for (int i=0; i<22; i++) {
	   HCmask[i] = 0;
	   HCmask2[i] = 0;
	 }
	 //
	 bitset<672> bits(*HCmask) ;
	 //
	 for (int i=0;i<672;i++){
	   if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
	   //if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
	 }
	 //
	 bitset<32> Convert;
	 //
	 Convert.reset();
	 //
	 for (int i=0;i<(alct->GetWGNumber());i++){
	   if ( bits.test(i) ) Convert.set(i%32);
	   if ( i%32 == 31 ) {
	     HCmask[i/32] = Convert.to_ulong();
	     //printf(" Convert %d %d \n",i,Convert.to_ulong());
	     Convert.reset();
	   }
	 }
	 /*
	 for (int i=0; i<12; i++) {
	   //if ( i%2 == 0 ) {
	       if (keyWG<32) {
		 //HCmask[i] = 0x1<<keyWG;
	       } else {
		 //HCmask[i] = 0x1<<(keyWG-32);
	       }
	       //} else {
	       //if (keyWG<32) {
	       // HCmask[i] = 0x1<<(keyWG) ;
	       //} else {
	       // HCmask[i] = 0x1<<(keyWG-32);
	       //}
	       //}
	 }
	 */
	 //for (int i=0; i<22; i++ ) {
	 //printf(" %x %x \n",HCmask[i],HCmask2[i]);
	 //}
	 //
	 alct->alct_write_hcmask(HCmask);
	 alct->alct_read_hcmask(HCmask);
	 /*
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
	 */
	 //
	 printf("\n");
	 for (int i=0; i<(alct->GetWGNumber()); i++) {
	   if ( i%(alct->GetWGNumber()/6) == 0 ) printf("\n");
	   if (bits.test(i)) {
	     printf("|");
	   } else {
	     printf("-");
	   }
	 }
	 //
	 cout << endl;
	 //
	 PulseTestStrips();
	 //
	 thisTMB->DecodeALCT();
	 //
	 printf("Wordcount %x \n",thisTMB->GetALCTWordCount());
	 //
	 if ( thisTMB->GetAlct0FirstKey()  == keyWG && thisTMB->GetAlct0Quality() >= 1 ) 
	    chamberResult[keyWG]++;
	 //
	 if ( thisTMB->GetAlct1SecondKey() == keyWG && thisTMB->GetAlct1Quality() >= 1 ) 
	    chamberResult2[keyWG]++;
	 //
	 InJected[keyWG]++;
	 //
      }
   }
   cout << endl;
   cout << "Wire" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << keyWG/100 ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << ((keyWG/10)%10) ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << keyWG%10 ;
   cout << endl;
   cout << "InJected" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << InJected[keyWG] ;
   cout << endl;
   cout << "ALCTChamberResult" << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << chamberResult[keyWG] ;
   cout << endl;
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) cout << chamberResult2[keyWG] ;
   cout << endl;
   printf("%c[0m", '\033'); 
   //
   for (int keyWG=0; keyWG<(alct->GetWGNumber())/6; keyWG++) ALCTWireScan_[keyWG] = chamberResult[keyWG];
   //
}
//
void ChamberUtilities::ALCTScanDelays(){
  //
  unsigned long HCmask[22];
  int CountDelay[20];
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  for (int i=0; i< 20; i++) CountDelay[i] = 0;
  //
  thisTMB->SetALCTPatternTrigger();
  //
  for ( int nloop=0; nloop<20; nloop++){
    for ( int DelaySetting=0; DelaySetting<20; DelaySetting++){
      //
      int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6);
      int ChamberSection = alct->GetWGNumber()/6;
      //
      cout << endl ;
      cout << "Injecting at " << dec << keyWG << endl;
      //
      for (int i=0; i< 22; i++) HCmask[i] = 0;
      //
      bitset<672> bits(*HCmask) ;
      //
      for (int i=0;i<672;i++){
	if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
      }
      //
      bitset<32> Convert;
      //
      Convert.reset();
      //
      for (int i=0;i<(alct->GetWGNumber());i++){
	if ( bits.test(i) ) Convert.set(i%32);
	if ( i%32 == 31 ) {
	  HCmask[i/32] = Convert.to_ulong();
	  Convert.reset();
	}
      }
      //
      printf("\n");
      //
      alct->alct_write_hcmask(HCmask);
      alct->alct_read_hcmask(HCmask);
      //
      cout << alct->alct_set_delay(-1,DelaySetting) << endl ; // Set all delays
      //
      PulseTestStrips();
      printf("Decode ALCT\n");
      thisTMB->DecodeALCT();
      printf("After Decode ALCT\n");
      //
      // Now analize
      //
      if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	   (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	   (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	   (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	   (thisTMB->GetAlct1SecondKey() != alct1_bxn) ) {
	//
	alct0_quality     = thisTMB->GetAlct0Quality();
	alct1_quality     = thisTMB->GetAlct1Quality();
	alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	alct0_key         = thisTMB->GetAlct0FirstKey();
	alct1_key         = thisTMB->GetAlct1SecondKey();
	//
	if ( alct0_quality == 3 && alct0_key == keyWG &&
	     alct1_quality == 3 && alct1_key == keyWG ) CountDelay[DelaySetting]++ ;
	//
      }
      //
    }
  }
  //
  for ( int DelaySetting=0; DelaySetting < 20; DelaySetting++ ) cout << CountDelay[DelaySetting] << " " ;
  //
  cout << endl ;
  cout << endl ;
  //
}
//
void ChamberUtilities::ALCTTiming(){
  //
  int maxTimeBins(13);
  int ProjectionX[13*2], ProjectionY[13*2];
  int selected[13][13];
  int selected2[13][13];
  int selected3[13][13];
  int ALCTWordCount[13][13];
  int rxtx_timing[13][13];
  int ALCTWordCountWrap[13*2][13*2];
  int ALCTConfDone[13][13];
  int j,k;
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  alct->set_empty(1);
  //
  thisTMB->SetCLCTPatternTrigger();
  thisTMB->DisableCLCTInputs();
  //
  for (j=0;j<maxTimeBins;j++){
    for (k=0;k<maxTimeBins;k++) {
      selected[j][k]  = 0;
      selected2[j][k] = 0;
      selected3[j][k] = 0;
      ALCTWordCount[k][j] = 0;
      ALCTConfDone[k][j] = 0;
    }
  }
  //
  unsigned long HCmask[22];
   //
   for (int i=0; i< 22; i++) HCmask[i] = 0;
   //
   for (j=0;j<maxTimeBins;j++){
      for (k=0;k<maxTimeBins;k++) {
	//
	int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6/4);
	int keyWG2 = (alct->GetWGNumber())/6-keyWG;
	int ChamberSection = alct->GetWGNumber()/6;
	//
	(*MyOutput_) << std::endl;
	(*MyOutput_) << "*******************" << std::endl;
	(*MyOutput_) << "Injecting at " << dec << keyWG << std::endl;
	//
	for (int i=0; i< 22; i++) HCmask[i] = 0;
	//
	bitset<672> bits(*HCmask) ;
	//
	for (int i=0;i<672;i++){
	  if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
	  //if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
	}
	//
	bitset<32> Convert;
	//
	Convert.reset();
	//
	for (int i=0;i<(alct->GetWGNumber());i++){
	  if ( bits.test(i) ) Convert.set(i%32);
	  if ( i%32 == 31 ) {
	    HCmask[i/32] = Convert.to_ulong();
	    //printf(" Convert %d %d \n",i,Convert.to_ulong());
	    Convert.reset();
	  }
	}
	//
	/*
	  for (int i=0; i<(alct->GetWGNumber()/32); i++) {
	  if ( i%2 == 0 ) {
	  if (keyWG<ChamberSection) {
	  HCmask[i] = 0x1<<keyWG;
	  } else {
	  HCmask[i] = 0x1<<(keyWG-ChamberSection);
	  }
	  } else {
	  if (keyWG<ChamberSection) {
	  HCmask[i] = 0x1<<(keyWG) ;
	  } else {
	  HCmask[i] = 0x1<<(keyWG-ChamberSection);
	  }
	  }
	  }
	*/
	//for (int i=0;i<22;i++) {
	//
	//printf(" %02x ",HCmask[i]);
	//
	//if ( i%((alct->GetWGNumber())/8/6+1) == 0 ) printf("\n");
	//
	//}
	//
	(*MyOutput_) << std::endl ;
	//
	alct->alct_write_hcmask(HCmask);
	alct->alct_read_hcmask(HCmask);
	//
	/*
	  for (int i=0; i<(alct->GetWGNumber())/32; i++ ) {
	  if (i == 0 ) cout << "Layer 0 " ;
	  for(int j=0; j<ChamberSection; j++ ) 
	  if (((HCmask[i]>>j)&0x1) == 0 ) {
	  cout <<"-";
	  } else {
	  printf("%c[01;44m", '\033');	 
	  cout <<"|";
	  printf("%c[0m", '\033'); 
	  }
	  if ((i+1)%2 == 0 ) {
	  cout << endl;
	  if (i+1<(alct->GetWGNumber())/32) cout << "Layer " << (i+1)/2 << " ";
	  }
	  }
	  cout << endl;
	*/
	//
	//while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
	//
	(*MyOutput_) << "Setting k=" << k << " j="<<j << endl;
	thisTMB->tmb_clk_delays(k,5) ;
	thisTMB->tmb_clk_delays(j,6) ;	 
	thisTMB->ResetALCTRAMAddress();
	PulseTestStrips();
	::usleep(100);
	//PulseTestStrips();
	//
	(*MyOutput_) << "Decode ALCT" << std::endl ;
	thisTMB->DecodeALCT();
	(*MyOutput_) << "After Decode ALCT" << std::endl;
	//
	int MyWordCount = thisTMB->GetALCTWordCount() ;
	//
	(*MyOutput_) << "Wordcount : " << std::hex<< MyWordCount <<std::endl;
	//
	//selected[k][j]  = 0;
	//selected2[k][j] = 0;
	//selected3[k][j] = 0;
	//
	(*MyOutput_) << "Check data" << std::endl ;
	if ( (thisTMB->GetAlct0Quality()   != alct0_quality) ||
	     (thisTMB->GetAlct1Quality()   != alct1_quality) ||
	     (thisTMB->GetAlct0FirstBxn()  != alct0_bxn) ||
	     (thisTMB->GetAlct1SecondBxn() != alct1_bxn) ||
	     (thisTMB->GetAlct0FirstKey()  != alct0_bxn) ||
	     (thisTMB->GetAlct1SecondKey() != alct1_bxn) || (1==1))
	  {
	    alct0_quality     = thisTMB->GetAlct0Quality();
	    alct1_quality     = thisTMB->GetAlct1Quality();
	    alct0_bxn         = thisTMB->GetAlct0FirstBxn();
	    alct1_bxn         = thisTMB->GetAlct1SecondBxn();
	    alct0_key         = thisTMB->GetAlct0FirstKey();
	    alct1_key         = thisTMB->GetAlct1SecondKey();
	    ALCTWordCount[k][j] = MyWordCount;
	    ALCTConfDone[k][j]  = thisTMB->ReadRegister(0x38) & 0x1 ;
	    //
	    if ( alct0_quality == 3 && alct0_key == keyWG) selected[k][j]++;
	    if ( alct1_quality == 3 && alct1_key == keyWG) selected[k][j]++;
	    if ( alct0_quality == 3 && alct0_key == keyWG2) selected2[k][j]++;
	    if ( alct1_quality == 3 && alct1_key == keyWG2) selected2[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG) &&
		 (alct1_quality == 3 && alct1_key == keyWG) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
		 (alct1_quality == 3 && alct1_key == keyWG2) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG) &&
		 (alct1_quality == 3 && alct1_key == keyWG2) ) selected3[k][j]++;
	    if ( (alct0_quality == 3 && alct0_key == keyWG2) &&
		 (alct1_quality == 3 && alct1_key == keyWG) ) selected3[k][j]++;
	   //
	   /*
	     if ( alct0_quality >= 1 ) selected[k][j]++;
	     if ( alct1_quality >= 1 ) selected[k][j]++;
	     if ( alct0_quality >= 1 ) selected2[k][j]++;
	     if ( alct1_quality >= 1 ) selected2[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	     if ( (alct0_quality >= 1 ) &&
	     (alct1_quality >= 1 ) ) selected3[k][j]++;
	   */
	  }	      
      }
   }
   //
   (*MyOutput_) << "WordCount  (tx vs. rx)   tx ---->" << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_) << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	//if ( ALCTWordCount[j][k] >0 ) printf("%c[01;35m", '\033');	 
	(*MyOutput_) << hex << setw(2) << (ALCTWordCount[j][k]&0xffff) << " ";
	//printf("%c[01;0m", '\033');	 
      }
      (*MyOutput_) << endl;
   }
   //
   (*MyOutput_) << endl;
   (*MyOutput_) << "ConfDone (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_)  << " rx =" << j << ": ";
     for (k=0;k<maxTimeBins;k++) {
	//if ( ALCTConfDone[j][k] >0 ) printf("%c[01;35m", '\033');	 
       (*MyOutput_) << hex << setw(2) << (ALCTConfDone[j][k]&0xffff) << " " ;
       //printf("%c[01;0m", '\033');	 
     }
     (*MyOutput_) << endl;
   }
   //
   (*MyOutput_) << endl;
   //
   (*MyOutput_) << endl;
   //
   (*MyOutput_) << "Selected 1 (tx vs. rx)   tx ----> " << endl;
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_) << " rx =" << j << ": ";
      for (k=0;k<maxTimeBins;k++) {
	 (*MyOutput_) << selected[j][k] << " " ;
      }
      (*MyOutput_) << endl;
   }
   //
   (*MyOutput_) << endl;
   //
   (*MyOutput_) << "Selected 2 (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_) << " rx =" << j << ": ";
     for (k=0;k<maxTimeBins;k++) {
       (*MyOutput_) << selected2[j][k] << " " ;
     }
     (*MyOutput_) << endl;
   }
   //
   (*MyOutput_) << endl;
   //
   (*MyOutput_) << "Selected 3 (tx vs. rx)   tx ----> " << endl;
   //
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_) << " rx =" << j << ": ";
     for (k=0;k<maxTimeBins;k++) {
       (*MyOutput_) << selected3[j][k] << " " ;
     }
     (*MyOutput_) << endl;
   }
   //
   (*MyOutput_) << endl;
   //
   (*MyOutput_) << "Result (tx vs. rx)   tx ----> " << endl;
   (*MyOutput_) << "        00 01 02 03 04 05 06 07 08 09 10 11 12" << endl;
   (*MyOutput_) << "        == == == == == == == == == == == == ==" << endl; 
   //
   // Result
   //
  
   for (j=0;j<maxTimeBins;j++){
     (*MyOutput_) << " rx =" << dec << setw(2) << j << " " ; 
     for (k=0;k<maxTimeBins;k++) {
       if ( ALCTConfDone[j][k] > 0 ) {
	 //if ( ALCTWordCount[j][k] == 0x0c || ALCTWordCount[j][k] == 0x18 ) printf("%c[01;35m", '\033');	 
	 (*MyOutput_) << hex << setw(2) << (ALCTWordCount[j][k]&0xffff) << " ";
	 //printf("%c[01;0m", '\033');
	 rxtx_timing[j][k]=ALCTWordCount[j][k];
       } else {
	 (*MyOutput_) << hex << setw(2) << 0x00 << " ";
	 rxtx_timing[j][k]=0;
       }
     }
     (*MyOutput_) << endl;
   }
   //
   ALCT_phase_analysis(rxtx_timing);
   //
   cout << endl ;
   //
   /*
   // Work it
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       ALCTWordCountWrap[j][k] = 0 ;
     }
   }   
   //
   // Copy old data
   // 
   for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       if ( ALCTConfDone[j][k] > 0 ) {
	 ALCTWordCountWrap[j][k] = ALCTWordCount[j][k] ;
       }
     }
   }
   int nloop = 0;
   //
 LOOP:
   //
   // 1dim Projection
   //
   for (j=0;j<maxTimeBins*2;j++){
     ProjectionX[j] = 0;
     ProjectionY[j] = 0;
   }
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] > 0 ) ProjectionY[j]++;
     }
   }
   //
   for (j=0;j<maxTimeBins*2;j++){
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[k][j] > 0 ) ProjectionX[j]++;
     }
   }
   //
   cout << "ProjectionX " << endl ;
   for (j=0;j<maxTimeBins*2;j++) printf("%02d ",ProjectionX[j]) ;
   printf("\n");
   cout << "ProjectionY " << endl ;
   for (j=0;j<maxTimeBins*2;j++) printf("%02d ",ProjectionY[j]) ;
   printf("\n");
   printf("\n");
   //
   // Kill bad regions
   //
   for (j=0;j<maxTimeBins*2;j++) {
     if (ProjectionX[j] > 11 ) {
       for (k=0;k<maxTimeBins;k++) ALCTWordCountWrap[k][j] = 0;
     }
   }
   //
   // Now do magic
   //
   j = 0;
   while (ProjectionY[j] > 0 ) {
     for (k=0;k<maxTimeBins;k++) {
       ALCTWordCountWrap[j+13][k] = ALCTWordCountWrap[j][k] ;
       ALCTWordCountWrap[j][k]    = 0 ;
     }
     j++;
   }
   //
   j = 0;
   while (ProjectionX[j] > 0 ) {
     for ( k=0; k<maxTimeBins; k++) {
       ALCTWordCountWrap[k][j+13] = ALCTWordCountWrap[k][j] ;
       ALCTWordCountWrap[k][j] = 0 ;
     }
     j++;
   }
   //
   for (int j=0; j<maxTimeBins*2; j++ ){
     printf(" rx = %02d : ",j);   
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] >0 ) printf("%c[01;35m", '\033');	 
       printf("%02x ",(ALCTWordCountWrap[j][k]&0xffff));
       printf("%c[01;0m", '\033');	 
     }
     cout << endl;
   }
   //
   cout << endl;
   //
   nloop++ ;
   //
   //if ( nloop < 2 ) goto LOOP;
   //
   // End Result
   //
   float meanX  = 0;
   float meanXn = 0;
   float meanY  = 0;
   float meanYn = 0;
   //   
   for (int j=0; j<maxTimeBins*2; j++ ){
     printf(" rx = %02d : ",j);   
     for (k=0;k<maxTimeBins*2;k++) {
       if ( ALCTWordCountWrap[j][k] >0 ) printf("%c[01;35m", '\033');	 
       printf("%02x ",(ALCTWordCountWrap[j][k]&0xffff));
       printf("%c[01;0m", '\033');	 
       if ( ALCTWordCountWrap[j][k] >0 ) {
	 meanX += k;
	 meanXn++;
	 meanY += j;
	 meanYn++;
       }       
     }
     cout << endl;
   }
   //
   // Calculate mean
   // 
   meanX /= meanXn+0.0001;
   meanY /= meanYn+0.0001;
   //
   printf(" \n Best Setting TX=%f RX=%f \n",int(meanX)%13 ,int(meanY)%13);
   //
   cout << endl;
   */
}
//
int ChamberUtilities::FindBestL1aAlct(){
  //
  // Now find the best L1a_delay value for the ALCT
  //
  //thisTMB->SetALCTPatternTrigger();
  unsigned long HCmask[22];
  /*
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
  */
  /*
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
  */
  //
  int WordCount[200], DMBCount[200];
  for (int i=0; i<200; i++) {
    WordCount[i] = 0; 
    DMBCount[i]=0;
  }
  //
  //unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default conf register
  //
  unsigned cr[3];
  //
  //alct->set_l1a_internal(1);
  //
  alct->GetConf(cr,1);
  //
  int minlimit = 0;
  int maxlimit = 150;
  //
  for (int l1a=minlimit; l1a<maxlimit; l1a++) {
    //
    //while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
    //
    int keyWG          = int((rand()/(RAND_MAX+0.01))*(alct->GetWGNumber())/6./2.);
    int ChamberSection = alct->GetWGNumber()/6;
    //
    printf("\n");
    printf("-----> Injecting at %d \n",keyWG);
    //
    for (int i=0; i<22; i++) HCmask[i] = 0;
    //
    bitset<672> bits(*HCmask) ;
    //
    for (int i=0;i<672;i++){
      if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
      //if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
    }
    //
    bitset<32> Convert;
    //
    Convert.reset();
    //
    for (int ii=0;ii<(alct->GetWGNumber());ii++){
      if ( bits.test(ii) ) Convert.set(ii%32);
      if ( ii%32 == 31 ) {
	HCmask[ii/32] = Convert.to_ulong();
	Convert.reset();
      }
    }
    //
    alct->alct_write_hcmask(HCmask);
    alct->alct_read_hcmask(HCmask);
    //
    cr[1] = (cr[1] & 0xfffff00f) | ((l1a&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    thisTMB->ResetALCTRAMAddress();
    PulseTestStrips();
    thisTMB->DecodeALCT();
    thisTMB->GetCounters();
    thisTMB->PrintCounters(3);
    WordCount[l1a] = thisTMB->GetALCTWordCount();
    DMBCount[l1a] = thisDMB->GetAlctDavCounter();
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
    //
    thisDMB->PrintCounters();
    //
  }
  //
  for (int i=minlimit; i<maxlimit; i++){
    printf(" Value = %d WordCount = %3d DMBCount = %3d \n",i,WordCount[i],DMBCount[i]);
  }
  //
  float DelayBin  = 0;
  int   DelayBinN = 0;
  int   flag      = 0;
  //
  for (int i=maxlimit; i>minlimit; i--){
    //
    if (flag == 1 && WordCount[i] == 0 ) break;
    //
    if ( WordCount[i]>0 ) {
      //
      //printf("Including %d \n",i);
      //
      flag = 1;
      DelayBin  += i ;
      DelayBinN ++;
    }
    //
  }
  //
  printf(" DelayBin=%f DelaybinN=%d \n",DelayBin,DelayBinN);
  //
  DelayBin /= (DelayBinN+0.0001) ;
  //
  BestALCTL1aDelay_ = DelayBin ;
  //
  return int(DelayBin);
  //
}
//
int ChamberUtilities::FindALCT_L1A_delay(int minlimit, int maxlimit){
  //
  unsigned cr[3];
  //
  alct->GetConf(cr,1);
  //
  int WordCount[200];
  for (int i=0; i<200; i++) WordCount[i] = 0;
  //
  for (int l1a=minlimit; l1a<maxlimit+1; l1a++) {
    //
    cr[1] = (cr[1] & 0xfffff00f) | ((l1a&0xff)<<4) ;
    alct->SetConf(cr,1);
    alct->unpackControlRegister(cr);
    thisTMB->ResetCounters();
    thisTMB->ResetALCTRAMAddress();
    sleep(1);
    thisTMB->GetCounters();
    //
    cout << endl;
    printf("L1a ALCT delay %d : \n",l1a);
    //
    thisTMB->PrintCounters(3);
    thisDMB->PrintCounters();
    thisTMB->DecodeALCT();
    WordCount[l1a] = thisTMB->GetALCTWordCount();
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
    //
  }
  float DelayBin  = 0;
  int   DelayBinN = 0;
  //
  for (int i=maxlimit; i>minlimit; i--){
    //
    if ( WordCount[i]>0 ) {
      DelayBin  += i ;
      DelayBinN ++;
    }
    //
  }
  //
  printf(" DelayBin=%f DelaybinN=%d \n",DelayBin,DelayBinN);
  //
  DelayBin /= (DelayBinN+0.0001) ;
  //
  printf("In.Best L1a ALCT delay %f \n",DelayBin);
  //
  ALCTL1aDelay_ = DelayBin;
  //
  return int(DelayBin);
  //
}
//
void ChamberUtilities::PulseRandomALCT(){
  //
  unsigned long HCmask[22];
  //
  //printf("CCB %x\n",thisCCB_);
  //printf("TMB %x\n",thisTMB);
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //
  int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetWGNumber())/6/4);
  int keyWG2 = (alct->GetWGNumber())/6-keyWG;
  int ChamberSection = alct->GetWGNumber()/6;
  //
  printf("Injecting at %d and %d\n",keyWG,keyWG2);
  //
  for (int i=0; i< 22; i++) HCmask[i] = 0;
  //
  bitset<672> bits(*HCmask) ;
  //
  for (int i=0;i<672;i++){
    if ( i%(alct->GetWGNumber()/6) == keyWG ) bits.set(i);
    if ( i%(alct->GetWGNumber()/6) == (alct->GetWGNumber())/6-keyWG ) bits.set(i);
  }
  //
  bitset<32> Convert;
  //
  Convert.reset();
  //
  for (int i=0;i<(alct->GetWGNumber());i++){
    if ( bits.test(i) ) Convert.set(i%32);
     if ( i%32 == 31 ) {
       HCmask[i/32] = Convert.to_ulong();
       Convert.reset();
     }
  }
  //
  alct->alct_write_hcmask(HCmask);
  alct->alct_read_hcmask(HCmask);
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips();
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
  thisDMB->PrintCounters();
  //
}	      
//
void ChamberUtilities::PulseAllWires(){
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips();
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
  thisDMB->PrintCounters();
  //
}
//
void ChamberUtilities::PulseTestStrips(){
  //
   int slot = thisTMB->slot();
   int TMBtime(1);
   //
   if ( alct ) {
      //
      //thisTMB->ResetALCTRAMAddress();
     //
     //thisTMB->SetCLCTPatternTrigger();
     //
     //unsigned cr[3]  = {0x80fc5fc0, 0x20a0f786, 0x8}; // Configuration for this test L1a_delay=120 L1a_window=0xf
     //
     //alct->SetConf(cr,1);
     //alct->unpackControlRegister(cr);
     //
      long int StripMask = 0x3f;
      long int PowerUp   = 1 ;
      long int Amplitude = 0x3f;
      //
      thisTMB->DisableCLCTInputs();
      //
      if ( beginning == 0 ) {
	//
	printf("Init \n");
	//
	//
	alct->alct_set_test_pulse_amp(&slot,Amplitude);
	//
	alct->alct_read_test_pulse_stripmask(&slot,&StripMask);
	cout << " StripMask = " << hex << StripMask << endl;
	//
	//old alct->alct_set_test_pulse_stripmask(&slot,0x3f);
	//old alct->alct_set_test_pulse_groupmask(&slot,0xff);
	//
	if (alct->GetChamberType().find("ME11")!=string::npos) {
	  alct->alct_set_test_pulse_stripmask(&slot,0x00);
	  alct->alct_set_test_pulse_groupmask(&slot,0xff);
	} else {
	  alct->alct_set_test_pulse_stripmask(&slot,0x3f);
	  alct->alct_set_test_pulse_groupmask(&slot,0xff);
	}
	//
	alct->alct_read_test_pulse_stripmask(&slot,&StripMask);
	cout << " StripMask = " << hex << StripMask << endl;
	//
	alct->alct_read_test_pulse_powerup(&slot,&PowerUp);
	cout << " PowerUp   = " << hex << PowerUp << dec << endl; //11July05 DM added dec
	//
	alct->alct_fire_test_pulse('A');
	//
	alct->alct_set_test_pulse_powerup(&slot,1);
	//
	beginning = 1;
	//
	PulseTestStrips();
	//
      } else {
	//
	if (alct->GetChamberType().find("ME11")!=string::npos) {
	  std::cout << alct->GetChamberType().find("ME11") <<std::endl;
	  std::cout << alct->GetChamberType() <<std::endl;
	  std::cout << "ME11" <<std::endl;
	} else {
	  std::cout << "ME12" <<std::endl;
	}
	  //
	//alct->alct_set_test_pulse_powerup(&slot,PowerUp);
	//alct->alct_set_test_pulse_powerup(&slot,0);
	//
	//printf("CCB %x \n",thisCCB_);
	//printf("MPC %x \n",thisMPC);
	//
	thisCCB_->setCCBMode(CCB::VMEFPGA);
	thisCCB_->WriteRegister(0x28,0x7862);  //4Aug05 DM changed 0x789b to 0x7862
	//July05 changed 0x7878 to 0x789b
	//thisCCB_->WriteRegister(0x20,0x1e71);
	//
	//                                      
	//
	//cout <<"Enter 78 then l1a delay time (in hex)" <<  endl;
	//cin >> hex >>  TMBtime;
	//thisCCB_->WriteRegister(0x28,TMBtime);      //5July05 DM allows you to write in CCB reg value
	                                           //in option 12, but since option 19 calls this also.... 
	//cout <<"TMBtime is " << TMBtime << dec << endl;
	//
	thisCCB_->ReadRegister(0x28);
	//
	//thisCCB_->WriteRegister(0x20,0x01);
	//
	thisCCB_->GenerateAlctAdbASync();	 
	//
	//thisCCB_->setCCBMode(CCB::DLOG);  
	//
	}
      //
   } else {
      cout << " No ALCT " << endl;
   }  
   //
   //thisCCB_->DumpAddress(0x20);
   //
}
//
void ChamberUtilities::LoadCFEB(int HalfStrip, int CLCTInputs, bool enableL1aEmulator ){
  //
  if ( enableL1aEmulator ) thisTMB->EnableInternalL1aSequencer();
  thisTMB->DisableCLCTInputs();
  thisTMB->SetCLCTPatternTrigger();
  //
  thisDMB->set_comp_thresh(0.3);
  thisDMB->set_dac(.5,0);
  //
  if (HalfStrip == -1 ) HalfStrip = int(rand()*31./(RAND_MAX+1.0));
  //
  (*MyOutput_) << endl;
  (*MyOutput_) << " -- Injecting in " << HalfStrip << endl;
  (*MyOutput_) << endl;
  //
  //thisTMB->DiStripHCMask(HalfStrip/4-1); // counting from 0; //Bad...requests L1a....
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
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  thisCCB_->WriteRegister(0x28,0x7878);  //4Aug05 DM changed 0x789b to 0x7862
  //
}
//
void ChamberUtilities::PulseCFEB(int HalfStrip, int CLCTInputs, bool enableL1aEmulator ){
  //
  LoadCFEB(HalfStrip, CLCTInputs, enableL1aEmulator);
  //
  thisDMB->inject(1,0x4f);
  //
  // Decode the TMB CLCTs (0 and 1)
  //
  thisTMB->DecodeCLCT();
  //
  thisDMB->PrintCounters();
  //
  cout << endl ;
  //
}
//
void ChamberUtilities::CFEBChamberScan(){
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
    for (int Nmuons=0; Nmuons < Npulses_; Nmuons++){
      //
      for (int HalfStrip=0; HalfStrip<MaxStrip; HalfStrip++) {
	//
	printf("Enabling Inputs %x \n",CLCTInputList[List]);
	//
	PulseCFEB( HalfStrip,CLCTInputList[List]);
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
	if ( clct0keyHalfStrip == HalfStrip && MuonsMaxHits[clct0cfeb][HalfStrip] < clct0nhit ) 
	  MuonsMaxHits[clct0cfeb][HalfStrip] = clct0nhit ;
	if ( clct1keyHalfStrip == HalfStrip && MuonsMaxHits[clct1cfeb][HalfStrip] < clct1nhit ) 
	  MuonsMaxHits[clct1cfeb][HalfStrip] = clct1nhit ;
	//
	if ( clct0nhit == 6 && clct0keyHalfStrip == HalfStrip ) Muons[clct0cfeb][HalfStrip]++;
	if ( clct1nhit == 6 && clct1keyHalfStrip == HalfStrip ) Muons[clct1cfeb][HalfStrip]++;
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
  for (int i=0;i<5;i++) 
    for (int j=0; j<MaxStrip; j++) {
      CFEBStripScan_[i][j] = Muons[i][j];
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
//
int ChamberUtilities::TMBL1aTiming(int enableInternalL1a){
  //
  int wordcounts[200];
  int TmbDavScope[200];
  int nmuons = 1;
  //
  if(enableInternalL1a) thisTMB->EnableInternalL1aSequencer();
  //
  for (int delay=0;delay<200;delay++) {
    wordcounts[delay] = 0;
    TmbDavScope[delay] = 0;
  }
  //
  int minlimit = 100;
  int maxlimit = 200;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;
  //
  thisTMB->ResetCounters();
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
      if ( UseCosmic ) sleep(2);
      if ( UsePulsing) PulseCFEB(delay%16,0xa);
      wordcounts[delay] += thisTMB->GetWordCount();
      thisTMB->GetCounters();
      thisTMB->PrintCounters(19) ;
      thisTMB->PrintCounters(20) ;
      thisTMB->PrintCounters(21) ;
      //
      TmbDavScope[delay] = thisDMB->GetTmbDavScope();
      //
      printf(" WordCount %d \n",thisTMB->GetWordCount());
    }
  }
  //
  for (int delay=minlimit;delay<maxlimit;delay++){
    if ( wordcounts[delay] > 0 ) {
      RightTimeBin += delay ;
      DataCounter++ ;
    }
    printf("delay = %d wordcount = %d TmbDavScope %d wordcount/nmuons %f \n",delay,wordcounts[delay],TmbDavScope[delay],wordcounts[delay]/(nmuons));
  }
  //
  RightTimeBin /= float(DataCounter) ;
  //
  printf("Right L1a delay setting is %f \n",RightTimeBin);
  //
  TMBL1aTiming_ = RightTimeBin;
  //
  return int(RightTimeBin) ;
  //
}
//
void ChamberUtilities::CFEBTiming(){
  //
  int MaxTimeDelay=13;
  //
  int Muons[5][MaxTimeDelay];
  int MuonsWork[5][2*MaxTimeDelay];
  //
  for(int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      Muons[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for(int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) {
    for(int CFEBs=0; CFEBs<5; CFEBs++) {
      MuonsWork[CFEBs][TimeDelay] = 0;
    }
   }
  //
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){
    //
    std::cout << " Setting TimeDelay to " << TimeDelay << endl;
    //
    thisTMB->tmb_clk_delays(TimeDelay,0) ;
    thisTMB->tmb_clk_delays(TimeDelay,1) ;
    thisTMB->tmb_clk_delays(TimeDelay,2) ;
    thisTMB->tmb_clk_delays(TimeDelay,3) ;
    thisTMB->tmb_clk_delays(TimeDelay,4) ;
    //
    int CLCTInputList[5] = {0x1,0x2,0x4,0x8,0x10};
    //
    for (int List=0; List<5; List++){
      //
      for (int Nmuons=0; Nmuons<2; Nmuons++){
	//
	usleep(50);
	//
	PulseCFEB( 16,CLCTInputList[List]);
	//
	usleep(50);
	//
	thisTMB->DiStripHCMask(16/4-1); // counting from 0;
	//
	std::cout << " TimeDelay " << TimeDelay << " CLCTInput " 
	     << CLCTInputList[List] << " Nmuons " << Nmuons << endl;
	//
	int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	int clct0nhit = thisTMB->GetCLCT0Nhit();
	int clct1nhit = thisTMB->GetCLCT1Nhit();
	int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	//
	std::cout << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	std::cout << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	//
	if ( clct0nhit == 6 && clct0keyHalfStrip == 16 ) Muons[clct0cfeb][TimeDelay]++;
	if ( clct1nhit == 6 && clct1keyHalfStrip == 16 ) Muons[clct1cfeb][TimeDelay]++;
	//
      }
    }
  }
  //
  float CFEBMeanN[5], CFEBMean[5];
  //
  for( int i=0; i<5; i++) {
    CFEBMean[i]  = 0 ;
    CFEBMeanN[i] = 0 ;
  }
  //
  std::cout << endl;
  std::cout << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) std::cout << setw(5) << TimeDelay ;
  std::cout << endl ;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    std::cout << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      std::cout << setw(5) << Muons[CFEBs][TimeDelay] ;
    }     
    std::cout << endl ;
  }   
  //
  std::cout << endl ;
  //
  int TimeDelay;
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      MuonsWork[CFEBs][TimeDelay] = Muons[CFEBs][TimeDelay] ;
    }
  }
  //
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    TimeDelay=0;
    while (Muons[CFEBs][TimeDelay]>0) {
      MuonsWork[CFEBs][TimeDelay+13] = Muons[CFEBs][TimeDelay] ;
      MuonsWork[CFEBs][TimeDelay] = 0 ;
      TimeDelay++;
    }
  }
  //
  std::cout << endl ;
  // 
  std::cout << "TimeDelay Fixed for Delay Wrapping " << endl ;
  std::cout << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) std::cout << setw(5) << TimeDelay ;
  std::cout << endl;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    std::cout << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
      if ( MuonsWork[CFEBs][TimeDelay] > 0  ) {
	CFEBMean[CFEBs]  += TimeDelay  ; 
	CFEBMeanN[CFEBs] += 1 ; 
      }
      std::cout << setw(5) << MuonsWork[CFEBs][TimeDelay] ;
    }     
    std::cout << endl ;
  }   
  //
  std::cout << endl ;
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) {
    CFEBMean[CFEBs] /= CFEBMeanN[CFEBs]+0.0001 ;
    if (CFEBMean[CFEBs] > 12 ) CFEBMean[CFEBs] = (CFEBMean[CFEBs]) - 13 ;
    std::cout << " CFEB = " << CFEBMean[CFEBs] ;
  }
  //
  for( int i=0; i<5; i++) CFEBrxPhase_[i] = CFEBMean[i];
  //
  std::cout << endl ;
  std::cout << endl ;
  //
}
//
void ChamberUtilities::InjectMPCData(){
  //
  float MpcDelay=0;
  int   MpcDelayN=0;
  //
  float Mpc0Delay=0;
  int   Mpc0DelayN=0;
  //
  float Mpc1Delay=0;
  int   Mpc1DelayN=0;
  //
  int   DelaySize = 15;
  //
  int   MPC0Count[DelaySize];
  int   MPC1Count[DelaySize];
  //
  for (int i=0; i<DelaySize; i++ ) {
    MPC0Count[i] = 0;
    MPC1Count[i] = 0;
  }
  //
  thisTMB->DisableCLCTInputs();
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  //
  for (int i=0; i<DelaySize; i++) {
    //
    for (int npulse=0; npulse<1; npulse++) {
      //
      cout << endl ;
      cout << "New Run" << endl ;
      cout << endl ;
      //
      thisTMB->mpc_delay(i);
      //
      cout << "mpc_delay_ =  " << dec << i << endl;
      //
      thisMPC->SoftReset();
      thisMPC->init();
      thisMPC->read_fifos();
      //
      thisTMB->InjectMPCData(1,0,0);
      //
      //thisTMB->InjectMPCData(1,0xf7a6a813,0xc27da3b2);
      //
      //thisTMB->FireMPCInjector(1);
      //
      thisCCB_->FireCCBMpcInjector();
      //
      thisMPC->read_fifos();
      //
      thisTMB->ReadRegister(0x86);
      thisTMB->ReadRegister(0x92);
      thisTMB->ReadRegister(0x90);
      //
      if ( thisTMB->MPC0Accept() > 0 ) {
	Mpc0Delay  += i ;    
	Mpc0DelayN++;
	MPC0Count[i]++ ;
      }
      //
      if ( thisTMB->MPC1Accept() > 0 ) {
	Mpc1Delay  += i ;    
	Mpc1DelayN++;
	MPC1Count[i]++ ;
      }
      //
      if( thisTMB->MPC0Accept()+thisTMB->MPC1Accept() > 0 ) {
	MpcDelay  += i ;    
	MpcDelayN++;
      }
      //
    }
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC0 winner delay=" << setw(3) << i << " gives " << MPC0Count[i] << endl;
  }
  //
  cout << endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC1 winner delay=" << setw(3) << i << " gives " << MPC1Count[i] << endl;
  }
  //
  cout << endl ;
  //
}
//
int ChamberUtilities::FindWinner(int npulses=10){
  //
  if ( ! thisMPC ) {
    cout << " No MPC defined in XML file " << endl ;
    return -1 ;
  }
  //
  thisCCB_->setCCBMode(CCB::VMEFPGA);
  //
  // thisTMB->alct_match_window_size_ = 3;
  // thisTMB->alct_vpf_delay_ = 3;
  //
  float MpcDelay=0;
  int   MpcDelayN=0;
  //
  float Mpc0Delay=0;
  int   Mpc0DelayN=0;
  //
  float Mpc1Delay=0;
  int   Mpc1DelayN=0;
  //
  int   DelaySize = 15;
  //
  int   MPC0Count[DelaySize];
  int   MPC1Count[DelaySize];
  //
  for (int i=0; i<DelaySize; i++ ) {
    MPC0Count[i] = 0;
    MPC1Count[i] = 0;
  }
  //
  for (int i = 0; i < DelaySize; i++){
    //
    //thisTMB->alct_match_window_size_ = i;
    //
    thisTMB->mpc_delay(i);
    //
    //thisTMB->trgmode(1);
    //
    thisTMB->ResetCounters();
    //
    //thisCCB_->startTrigger();
    //thisCCB_->bx0();
    //
    int iterations = 0;
    //
  REPEAT:
    //
    thisMPC->SoftReset();
    thisMPC->init();
    //
    if (UsePulsing) {
      iterations++;
      PulseCFEB(-1,0xa);
    }

    if (UseCosmic)  sleep(2);
    //
    thisTMB->DataSendMPC();
    //
    if (UseCosmic) thisTMB->GetCounters();
    //
    //thisTMB->PrintCounters();
    //
    cout << "mpc_delay_ =  " << dec << i << endl;
    //
    //cout << thisTMB->MPC0Accept() << " " << thisTMB->MPC1Accept() << endl ;
    //
    if (UseCosmic) {
      thisTMB->PrintCounters(8);
      thisTMB->PrintCounters(16);
      thisTMB->PrintCounters(17);
    }
    //
    if (thisTMB->GetCounter(16) || 
	(thisTMB->MPC0Accept()+thisTMB->MPC1Accept()) > 0 ) {
      MpcDelay  += i ;    
      MpcDelayN++;
      if ( thisTMB->MPC0Accept() > 0 ) {
	Mpc0Delay  += i ;    
	Mpc0DelayN++;
	MPC0Count[i]++ ;
      }
      if ( thisTMB->MPC1Accept() > 0 ) {
	Mpc1Delay  += i ;    
	Mpc1DelayN++;
	MPC1Count[i]++ ;
      }
    }
    //
    thisMPC->firmwareVersion();
    //
    //thisMPC->read_fifosA();
    //
    cout << endl;
    //
    thisMPC->read_fifos();
    //
    cout << endl;
    //
    thisMPC->read_csr0();
    //
    if (UsePulsing && iterations < npulses ) goto REPEAT;
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC0 winner delay=" << setw(3) << i << " gives " << MPC0Count[i] << endl;
  }
  //
  cout << endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    cout << "MPC1 winner delay=" << setw(3) << i << " gives " << MPC1Count[i] << endl;
  }
  //
  cout << endl ;
  //
  MpcDelay  /= (MpcDelayN  + 0.0001) ;
  Mpc0Delay /= (Mpc0DelayN + 0.0001) ;
  Mpc1Delay /= (Mpc1DelayN + 0.0001) ;
  //
  cout << "Correct MPC  setting  : " << MpcDelay << endl ;
  cout << "Correct MPC0 setting  : " << Mpc0Delay << endl ;
  cout << "Correct MPC1 setting  : " << Mpc1Delay << endl ;
  //
  cout << endl ;
  //
}
//
int ChamberUtilities::FindALCTvpf(){
  //
  // thisCCB_->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  // thisTMB->alct_match_window_size_ = 3;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;
  //
  int MaxTimeBin   = 8;
  //
  for (int i = 0; i < MaxTimeBin; i++){
    //
    cout << endl << "ALCT_vpf_delay=" << i << endl;
    //
    thisTMB->alct_vpf_delay(i);    // loop over this
    //thisTMB->trgmode(1);         // 
    thisTMB->ResetCounters();      // reset counters
    //thisCCB_->startTrigger();     // 2 commands to get trigger going
    //thisCCB_->bx0();
    sleep(2);                      // accumulate statistics
    //thisCCB_->stopTrigger();      // stop trigger
    thisTMB->GetCounters();        // read counter values
    //
    //thisTMB->PrintCounters();    // display them to screen
    //
    thisTMB->PrintCounters(8);
    thisTMB->PrintCounters(10);
    //
    if ( thisTMB->GetCounter(10) > 0 ) {
      RightTimeBin += i ;
      DataCounter++;
    }
    //
  }
  //
  RightTimeBin /= float(DataCounter) ;
  //
  printf("Best Setting is %f \n",RightTimeBin);
  //
  printf("\n");

  ALCTvpf_ = int(RightTimeBin);
     
  return int(RightTimeBin) ;

}


int ChamberUtilities::FindTMB_L1A_delay( int idelay_min, int idelay_max ){
  //
  //bool useCCB = false; // if using TTC for L1A and start trig, bc0, set to false
  //cout << "Value of useCCB is" << useCCB <<endl;
  //
  //if (useCCB) thisCCB_->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  //     thisTMB->alct_match_window_size_ = 3;
  //
  float RightTimeBin = 0;
  int   DataCounter  = 0;

  for (int i = idelay_min; i < idelay_max+1; i++){
    
    //thisTMB->l1adelay_ = i;// loop over this
    //
    thisTMB->lvl1_delay(i);
    //
    // thisTMB->trgmode(1);         // 
    //
    thisTMB->ResetCounters();    // reset counters
    //if (useCCB) thisCCB_->startTrigger();     // 2 commands to get trigger going
    //if (useCCB) thisCCB_->bx0();
    cout << endl << "TMB_l1adelay=" << i << ":" << endl;
    sleep(1);                   // accumulate statistics
    //if (useCCB) thisCCB_->stopTrigger();      // stop trigger
    thisTMB->GetCounters();      // read counter values
    
    //thisTMB->PrintCounters(); // display them to screen
    thisTMB->PrintCounters(8);  // display them to screen
    thisTMB->PrintCounters(19);
    thisTMB->PrintCounters(20);

    if ( thisTMB->GetCounter(19) > 0) {
      RightTimeBin += i;
      DataCounter++;
    }

  }

  RightTimeBin /= float(DataCounter) ;
  
  printf("Right L1a delay setting is %f \n",RightTimeBin);

  printf("\n");

  //if (useCCB) thisCCB_->setCCBMode(CCB::DLOG);      // return to "regular" mode for CCB

  TMBL1aTiming_ = RightTimeBin;

  return int(RightTimeBin) ;

}
//
/*
extern char cmd[];
extern char sndbuf[];
extern char rcvbuf[];

void settrgsrc(int dword)
      {
	cout << "Input DMB trigger source value in hex (0 turns off internal L1A and LCT)" << endl;
	cin >> hex >> dword >> dec;

	cmd[0]=VTX2_USR1;
	sndbuf[0]=37;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR2;
	sndbuf[0]=dword&0x0F; 
	thisDMB->devdo(MCTRL,6,cmd,4,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_USR1;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,0);
	cmd[0]=VTX2_BYPASS;
	sndbuf[0]=0;
	thisDMB->devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,2);
	
	printf("Cal_trg source are Set to %01x (Hex). \n",dword&0x0F);
      }
*/

void ChamberUtilities::ALCT_phase_analysis (int rxtx_timing[13][13]) {
  int i, j, k, p;  
  //
  /* 
  cout << "The array you are working with is:" << endl;
  cout << endl;
	for (j = 0; j < 13; j++) {
		for (k = 0; k < 13; k++) {
		cout << rxtx_timing[j][k] << " ";
				}
			cout << endl;
		cout << endl;
		}
  */
  //
//now find best position by the non-zero number that is 
//farthest from zero in both directions
  
  int nmin_best, ntot_best;
  int nup, ndown, nleft, nright;
  int ntot, nmin;
  int best_element_row, best_element_col;
  nmin_best=0;
  ntot_best=0;
  for (j = 0; j < 13; j++) {           //loops over rows
    for (k = 0; k < 13; k++) {       //loops over columns
      
      if ( rxtx_timing[j][k] != 0) {        //find all non-zero matrix elements
	//			  cout << "element (row column) " << j << k << " is  good "<< endl;
	//
	for (i=j+1;rxtx_timing[i%13][k]!=0 && i<j+13;i++) {   //scans all non-zero element below
	}
	ndown = i-1-j;  //number of non-zero elements below
	//			  cout << "ndown = " << ndown << "  ";
	for (p=k+1;rxtx_timing[j][p%13]!=0 && p<k+13 ;p++) {  //scans all non-zero elements to the right
	}
	nright = p-1-k;  //number of non-zero elements to the right
	//			  cout << "nright = " << nright << "  ";		       
	for (i=j-1;rxtx_timing[(13+i)%13][k]!=0 && i>j-13;i--) {   //scans all non-zero elements above
	}
	nup = j-1-i;  //number of non-zero elements above
	//			  cout << "nup = " << nup << "  ";
	for (p=k-1;rxtx_timing[j][(13+p)%13]!=0 && p>k-13;p--) {   //scans all non-zero elements to the left
	}
	nleft = k-1-p;  //number of non-zero elements to the left
	//			  cout << "nleft = " << nleft << endl;
	nmin = 100;
	int numbers[] = {nup, ndown, nleft, nright};
	
	for (int d = 0; d < 4; d++) {    // finds the minimum number of non-zero elements in any direction
	  if (nmin > numbers[d]) {
	    nmin = numbers [d];
	  }
	}
	//
	// 				cout << "nmin =  " << nmin << endl;
	ntot=ndown+nup+nright+nleft;         //finds the total number of non-zero elements in all directions
	//				cout << "ntot =  " << ntot << endl;
	//				cout << endl;
	//
	if (nmin_best < nmin) {         //finds the array element with the best nmin and ntot value
	  nmin_best = nmin;
	  if (ntot_best < ntot) {
	    ntot_best = ntot;
	  }
	  best_element_row = j;
	  best_element_col = k;
	  //
	  // cout << "best element so far is " << best_element_row << best_element_col << endl;
	  // cout << endl;
	  //
	}
      }
    }
  }
  (*MyOutput_) << endl;
  (*MyOutput_) << "the best values of nmin and ntot are:  " << endl;
  (*MyOutput_) << "nmin =  " << nmin_best << "  and ntot =  " << ntot_best << endl;
  (*MyOutput_) << endl;
  (*MyOutput_) << "best element is: " << endl;
  (*MyOutput_) << "rx =  " << best_element_row << "    tx =  " << best_element_col << endl;
  (*MyOutput_) << endl;
  //
  ALCTrxPhase_ = best_element_row ;
  ALCTtxPhase_ = best_element_col ;
  //
}	
