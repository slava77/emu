//-----------------------------------------------------------------------
// $Id: ChamberUtilities.cc,v 3.15 2007/05/30 16:48:27 rakness Exp $
// $Log: ChamberUtilities.cc,v $
// Revision 3.15  2007/05/30 16:48:27  rakness
// DMB cable_delay scans/tools
//
// Revision 3.14  2007/05/16 13:48:38  rakness
// mpc_output_enable=1 in AFF to L1A measurement
//
// Revision 3.13  2007/05/14 17:37:15  rakness
// AFF to L1A timing button
//
// Revision 3.12  2007/03/22 18:34:45  rakness
// weighted average in ALCT-CLCT match, and TMB/ALCT-L1A scans
//
// Revision 3.11  2007/03/14 14:24:50  rakness
// write sync values to summary file
//
// Revision 3.10  2006/10/19 13:01:07  rakness
// RPC-RAT delay scan in hyperDAQ
//
// Revision 3.9  2006/10/19 09:42:03  rakness
// remove old ALCTController
//
// Revision 3.8  2006/10/12 17:52:13  mey
// Update
//
// Revision 3.7  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.6  2006/10/02 13:45:31  mey
// Update
//
// Revision 3.5  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.4  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.3  2006/07/23 15:42:51  rakness
// index Hot Channel Mask from 0
//
// Revision 3.2  2006/07/23 14:04:06  rakness
// encapsulate RAT, update configure()
//
// Revision 3.1  2006/07/22 16:13:02  rakness
// clean up RAT/add JTAG checking
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 1.39  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 1.38  2006/07/19 18:11:17  rakness
// Error checking on ALCTNEW
//
// Revision 1.37  2006/07/18 07:52:30  rakness
// ALCTTiming with ALCTNEW
//
// Revision 1.36  2006/07/14 11:46:31  rakness
// compiler switch possible for ALCTNEW
//
// Revision 1.35  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 1.34  2006/06/23 13:40:26  mey
// Fixed bug
//
// Revision 1.33  2006/06/20 11:35:11  rakness
// output of cfebchamberscan
//
// Revision 1.32  2006/06/20 10:03:11  mey
// Reduced dac setting
//
// Revision 1.31  2006/06/01 16:05:00  rakness
// update
//
// Revision 1.30  2006/05/31 13:10:53  mey
// Update
//
// Revision 1.29  2006/05/29 15:25:36  rakness
// log timing scan output in summary
//
// Revision 1.28  2006/05/18 08:35:44  mey
// Update
//
// Revision 1.27  2006/05/17 14:16:44  mey
// Update
//
// Revision 1.26  2006/05/12 08:03:06  mey
// Update
//
// Revision 1.25  2006/05/11 09:37:31  mey
// Update
//
// Revision 1.24  2006/05/10 10:24:32  mey
// Update
//
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
#include "TMB_constants.h"
//
using namespace std;
//
ChamberUtilities::ChamberUtilities(){
  //
  beginning = 0;
  thisTMB   = 0;
  thisDMB   = 0;
  //
  Npulses_ = 2;
  //
  MyOutput_ = &std::cout ;
  //
  // parameters to determine
  for( int i=0; i<5; i++) 
    CFEBrxPhase_[i] = -1;
  ALCTtxPhase_       = -1;
  ALCTrxPhase_       = -1;
  RatTmbDelay_       = -1;
  for(int i=0; i<2;i++) 
    RpcRatDelay_[i] = -1;
  ALCTvpf_           = -1;
  ALCTvpf            = -1;
  MPCdelay_          = -1;
  AlctDavCableDelay_ = -1;
  TmbLctCableDelay_  = -1;
  CfebDavCableDelay_ = -1;
  CfebCableDelay_    = -1;
  TMBL1aTiming_      = -1;
  BestALCTL1aDelay_  = -1;
  ALCTL1aDelay_      = -1;
  //
  // measured values
  AffToL1aAverageValue_ = -1;
  CfebDavAverageValue_  = -1;
  TmbDavAverageValue_   = -1;
  AlctDavAverageValue_  = -1;
  AffToL1aScopeAverageValue_ = -1;
  CfebDavScopeAverageValue_  = -1;
  TmbDavScopeAverageValue_   = -1;
  AlctDavScopeAverageValue_  = -1;
  //
  for (int i=0;i<5;i++) 
    for (int j=0; j<32; j++) {
      CFEBStripScan_[i][j] = -1;
    }
  for (int i=0;i<112;i++) ALCTWireScan_[i] = -1;
  //
  // values for analyzing DMB
  ScopeMin_        = 0;
  ScopeMax_        = 4;
  AffToL1aValueMin_= 140;
  AffToL1aValueMax_= 155;
  CfebDavValueMin_ = 0;
  CfebDavValueMax_ = 10;
  TmbDavValueMin_  = 0;
  TmbDavValueMax_  = 10;
  AlctDavValueMin_ = 20;
  AlctDavValueMax_ = 30;
  //

  UsePulsing = true ;
  UseCosmic = false ;
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
/////////////////////////////////////////////////////////////////////////////////
// peripheral crate parameter measurements
/////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------
// clock phases
//----------------------------------------------
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
    (*MyOutput_) << " Setting TimeDelay to " << TimeDelay << endl;
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
	(*MyOutput_) << " TimeDelay " << TimeDelay << " CLCTInput " 
	     << CLCTInputList[List] << " Nmuons " << Nmuons << endl;
	//
	int clct0cfeb = thisTMB->GetCLCT0Cfeb();
	int clct1cfeb = thisTMB->GetCLCT1Cfeb();
	int clct0nhit = thisTMB->GetCLCT0Nhit();
	int clct1nhit = thisTMB->GetCLCT1Nhit();
	int clct0keyHalfStrip = thisTMB->GetCLCT0keyHalfStrip();
	int clct1keyHalfStrip = thisTMB->GetCLCT1keyHalfStrip();
	//
	(*MyOutput_) << " clct0cfeb " << clct0cfeb << " clct1cfeb " << clct1cfeb << endl;
	(*MyOutput_) << " clct0nhit " << clct0nhit << " clct1nhit " << clct1nhit << endl;
	//
	if ( clct0nhit == 6 && clct0keyHalfStrip == 16 && clct0cfeb == List ) 
	  Muons[clct0cfeb][TimeDelay]++;
	if ( clct1nhit == 6 && clct1keyHalfStrip == 16 && clct1cfeb == List ) 
	  Muons[clct1cfeb][TimeDelay]++;
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
  (*MyOutput_) << endl;
  (*MyOutput_) << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++) (*MyOutput_) << setw(5) << TimeDelay ;
  (*MyOutput_) << endl ;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<MaxTimeDelay; TimeDelay++){ 
      (*MyOutput_) << setw(5) << Muons[CFEBs][TimeDelay] ;
    }     
    (*MyOutput_) << endl ;
  }   
  //
  (*MyOutput_) << endl ;
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
  (*MyOutput_) << endl ;
  // 
  (*MyOutput_) << "TimeDelay Fixed for Delay Wrapping " << endl ;
  (*MyOutput_) << "TimeDelay " ;
  for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++) (*MyOutput_) << setw(5) << TimeDelay ;
  (*MyOutput_) << endl;
  for (int CFEBs=0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id=" << CFEBs << " " ;
    for (int TimeDelay=0; TimeDelay<2*MaxTimeDelay; TimeDelay++){ 
      if ( MuonsWork[CFEBs][TimeDelay] > 0  ) {
	CFEBMean[CFEBs]  += TimeDelay  ; 
	CFEBMeanN[CFEBs] += 1 ; 
      }
      (*MyOutput_) << setw(5) << MuonsWork[CFEBs][TimeDelay] ;
    }     
    (*MyOutput_) << endl ;
  }   
  //
  (*MyOutput_) << endl ;
  //
  for( int CFEBs=0; CFEBs<5; CFEBs++) {
    CFEBMean[CFEBs] /= CFEBMeanN[CFEBs]+0.0001 ;
    if (CFEBMean[CFEBs] > 12 ) CFEBMean[CFEBs] = (CFEBMean[CFEBs]) - 13 ;
    (*MyOutput_) << " CFEB = " << CFEBMean[CFEBs] ;
  }
  //
  for( int i=0; i<5; i++) CFEBrxPhase_[i] = (int)(CFEBMean[i]+0.5);
  //
  (*MyOutput_) << endl ;
  (*MyOutput_) << endl ;
  //
}
//
void ChamberUtilities::ALCTTiming(){
  //
  int maxTimeBins(13);
  //  int ProjectionX[13*2], ProjectionY[13*2];
  int selected[13][13];
  int selected2[13][13];
  int selected3[13][13];
  int ALCTWordCount[13][13];
  int rxtx_timing[13][13];
  //  int ALCTWordCountWrap[13*2][13*2];
  int ALCTConfDone[13][13];
  int j,k;
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  //
  // set up for this test
  alct->SetSendEmpty(1);
  alct->WriteConfigurationReg();
  alct->PrintConfigurationReg();
  //
  thisTMB->SetCLCTPatternTrigger();
  thisTMB->DisableCLCTInputs();
  //
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
	int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetNumberOfChannelsInAlct())/6/4);
	int keyWG2 = (alct->GetNumberOfChannelsInAlct())/6-keyWG;
	//	int ChamberSection = alct->GetNumberOfChannelsInAlct()/6;
	//
	(*MyOutput_) << std::endl;
	(*MyOutput_) << "*******************" << std::endl;
	(*MyOutput_) << "Injecting at " << dec << keyWG << std::endl;
	//
	for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	  for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct()/6); channel++) {
	    if (channel==keyWG) {
	      alct->SetHotChannelMask(layer,channel,ON);
	    } else {
	      alct->SetHotChannelMask(layer,channel,OFF);
	    }
	  }
	}
	alct->WriteHotChannelMask();
	alct->PrintHotChannelMask();
	//
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
void ChamberUtilities::RatTmbDelayScan() {
  //** Find optimal rpc_clock delay = phasing between RAT board and TMB **
  //
  int ddd_delay;
  int rpc_bad[16] = {};
  //
  //Put RAT into sync mode for test [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                                [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                                 to improve syncing to rising edge of TMB clock
  //                                [2]=loop_tmb
  //                                [3]=free_tx0
  //
  int initial_data = thisTMB->ReadRegister(vme_ratctrl_adr);
  //
  int write_data = initial_data & 0xfffe | 0x0001;
  thisTMB->WriteRegister(vme_ratctrl_adr,write_data);
  //
  //here are the arrays of bits we expect from sync mode:
  const int nbits = 19;
  int rpc_rdata_expect[4][nbits];
  bit_to_array(0x2aaaa,rpc_rdata_expect[0],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[1],nbits);
  bit_to_array(0x55555,rpc_rdata_expect[2],nbits);
  bit_to_array(0x2aaaa,rpc_rdata_expect[3],nbits);
  //
  //  for (i=0; i<=3; i++) {
  //    (*MyOutput_) << "rpc_rdata_expect[" << i << "] = ";
  //    for (int bit=0; bit<=(nbits-1); bit++) {
  //      (*MyOutput_) << rpc_rdata_expect[i][bit] << " ";
  //    }
  //    (*MyOutput_) << std::endl;
  //  }
  //
  //enable RAT input into TMB...
  int read_data = thisTMB->ReadRegister(rpc_inj_adr);
  write_data = read_data | 0x0001;
  thisTMB->WriteRegister(rpc_inj_adr,write_data);
  //
  //Initial delay values:
  //  thisRAT_->ReadRatTmbDelay();
  //  int rpc_delay_default = thisRAT_->GetRatTmbDelay();
  //
  int rpc_rbxn[4],rpc_rdata[4],rpcData[4];
  int rpc_data_array[4][nbits];
  //
  int pass;
  int count_bad;
  //
  (*MyOutput_) << "Performing RAT-TMB delay scan..." << std::endl;;
  //
  //step through ddd_delay
  for (pass=0; pass<=1000; pass++) { //collect statistics
    //
    //    if ( (pass % 100) == 0 ) 
    //      (*MyOutput_) << "Pass = " << std::dec << pass << std::endl;
    //
    for (ddd_delay=0; ddd_delay<16; ddd_delay++) {
      count_bad=0;
      //
      // ** write the delay to the RPC **
      thisRAT_->SetRatTmbDelay(ddd_delay);
      thisRAT_->WriteRatTmbDelay();
      //
      // ** read RAT 80MHz demux registers**
      for (int irat=0; irat<=3; irat++) {
	//
	read_data = thisTMB->ReadRegister(rpc_cfg_adr);
	read_data &= 0xf9ff;                        //zero out old RAT bank
	write_data = read_data | (irat << 9);       //select RAT RAM bank
	thisTMB->WriteRegister(rpc_cfg_adr,write_data);
	//
	read_data = thisTMB->ReadRegister(rpc_cfg_adr);
	rpc_rbxn[irat] = (read_data >> 11) & 0x0007;  //RPC MSBS for sync mode
	//
	rpc_rdata[irat] = thisTMB->ReadRegister(rpc_rdata_adr) & 0xffff; //RPC RAM read data for sync mode (LSBS)
	//
	rpcData[irat] = rpc_rdata[irat] | (rpc_rbxn[irat] << 16);  //pack MS and LSBs into single integer
	//
	bit_to_array(rpcData[irat],rpc_data_array[irat],nbits);
	//
	for (int i=0; i<=(nbits-1); i++) {
	  if (rpc_data_array[irat][i] != rpc_rdata_expect[irat][i]) count_bad += 1;
	}
      }
      //      for (i=0; i<=3; i++) {
      //	(*MyOutput_) << "rpc_data_array[" << i << "] = ";
      //	for (bit=0; bit<=(nbits-1); bit++) {
      //	  (*MyOutput_) << rpc_data_array[i][bit] << " ";
      //	}
      //	(*MyOutput_) << std::endl;
      //      }
      rpc_bad[ddd_delay] += count_bad;
    }
  }
  // Put RPC delay back to initial values:
  //  (*MyOutput_) << "Putting delay values back to " << rpc_delay_default << std::endl;
  //  thisRAT_->SetRatTmbDelay(rpc_delay_default);
  //  thisRAT_->WriteRatTmbDelay();
  //
  // ** Take TMB out of sync mode **
  //  write_data = 0x0002;
  thisTMB->WriteRegister(vme_ratctrl_adr,initial_data);
  //
  // ** print out results **
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "** TMB-RAT delay results *" << std::endl;
  (*MyOutput_) << "**************************" << std::endl;
  (*MyOutput_) << "rpc_delay   bad data count" << std::endl;
  (*MyOutput_) << "---------   --------------" << std::endl;
  for (int rpc_delay = 0; rpc_delay <13; rpc_delay++) {
    (*MyOutput_) << "    " << std::hex << rpc_delay 
		 << "           " << std::hex << rpc_bad[rpc_delay] 
	      <<std::endl;
  }
  //
  RatTmbDelay_ = window_analysis(rpc_bad,13);
  //
  return;
}
//
void ChamberUtilities::RpcRatDelayScan() {
  //
  RpcRatDelayScan(0);
  return;
}
//
void ChamberUtilities::RpcRatDelayScan(int rpc) {
  //** Find optimal rpc_rat_clock delay = phasing between RPC[rpc] and RAT **

  //Put RAT into correct mode [0]=sync-mode -> sends a fixed data pattern to TMB to be analyzed
  //                          [1]=posneg    -> inserts 12.5ns (1/2-cycle) delay in RPC data path
  //                                           to improve syncing to rising edge of TMB clock
  //                          [2]=loop_tmb
  //                          [3]=free_tx0
  //  int write_data = 0x0002;
  //  thisTMB->WriteRegister(vme_ratctrl_adr,write_data);
  //
  (*MyOutput_) << "Performing RPC" << rpc << "-RAT delay scan..." << std::endl;;
  //
  //  thisRAT_->ReadRatUser1();                           //read initial delay values
  //  int initial_delay = thisRAT_->GetRpcRatDelay(rpc);  //get values into local variable
  //
  int delay;
  //
  int parity_err_ctr[16] = {};
  //
  for (delay = 0; delay<=12; delay++) {                             //steps of 2ns
    //    (*MyOutput_) << "set delay = " << delay 
    //		 << " for RPC " << rpc
    //		 << std::endl;
    thisRAT_->SetRpcRatDelay(rpc,delay);
    thisRAT_->WriteRpcRatDelay();
    thisRAT_->PrintRpcRatDelay();
    //
    thisRAT_->reset_parity_error_counter();
    //
    ::sleep(1);                                                    //accumulate statistics
    //
    thisRAT_->ReadRatUser1();
    parity_err_ctr[delay] = thisRAT_->GetRatRpcParityErrorCounter(rpc);
    //
    //    (*MyOutput_) << "parity error for delay " << delay 
    //		     << " = " << parity_err_ctr[delay]
    //		 << std::endl;
    //
  }
  //
  //  (*MyOutput_) << "Putting inital delay value = " << initial_delay << " back..." << std::endl;
  //  thisRAT_->SetRpcRatDelay(rpc,initial_delay);
  //  thisRAT_->WriteRpcRatDelay();
  //
  // ** print out results **
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << "**** RPC" << rpc << "-RAT delay results ****" << std::endl;
  (*MyOutput_) << "********************************" << std::endl;
  (*MyOutput_) << " delay    parity counter errors" << std::endl;
  (*MyOutput_) << "-------   ---------------------" << std::endl;
  for (delay = 0; delay <=12; delay++) {
    (*MyOutput_) << "   " << std::hex << delay;
    (*MyOutput_) << "               " << std::hex << parity_err_ctr[delay] 
		 << std::endl;
  }
  //
  RpcRatDelay_[rpc] = window_analysis(parity_err_ctr,13);
  //
  return;
}
//
//----------------------------------------------
// ALCT-CLCT match timing
//----------------------------------------------
int ChamberUtilities::FindALCTvpf() {
  //
  // thisCCB_->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  // thisTMB->alct_match_window_size_ = 3;
  //
  const int MaxTimeBin   = 15;
  //
  int alct_in_window[MaxTimeBin] = {};
  //
  for (int i = 0; i < MaxTimeBin; i++){
    //
    cout << "ALCT_vpf_delay=" << i << endl;
    //
    thisTMB->alct_vpf_delay(i);    // loop over this
    //thisTMB->trgmode(1);         // 
    thisTMB->ResetCounters();      // reset counters
    //thisCCB_->startTrigger();     // 2 commands to get trigger going
    //thisCCB_->bx0();
    ::sleep(5);                      // accumulate statistics
    //thisCCB_->stopTrigger();      // stop trigger
    thisTMB->GetCounters();        // read counter values
    //
    //thisTMB->PrintCounters();    // display them to screen
    //
    //    thisTMB->PrintCounters(8);
    //    thisTMB->PrintCounters(10);
    //
    alct_in_window[i] = thisTMB->GetCounter(10);
  }
  //
  float RightTimeBin      = 0;
  float RightTimeBinDenom = 0;
  //
  for (int i = 0; i < MaxTimeBin; i++){
    //
    (*MyOutput_) << "match_trig_alct_delay " << std::dec << std::setw(5) << i
		 << " : " << std::setw(10) << alct_in_window[i] << std::endl;
    //
    RightTimeBin      += ((float) alct_in_window[i]) * ((float) i) ;
    RightTimeBinDenom += ((float) alct_in_window[i]) ;
    //
  }
  //
  if (RightTimeBin > 100) {
    RightTimeBin /= RightTimeBinDenom ;
  } else {
    RightTimeBin = -999.;
  }
  //
  printf("Best Setting is %f \n",RightTimeBin);
  //
  printf("\n");

  ALCTvpf_ = (int) (RightTimeBin+0.5);

  (*MyOutput_) << "Best value for match_trig_alct_delay = " << std::dec << ALCTvpf_ << std::endl;
     
  return ALCTvpf_ ;
}
//
//----------------------------------------------
// Winner bits from MPC
//----------------------------------------------
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

    if (UseCosmic)  ::sleep(2);
    //
    thisTMB->DataSendMPC();
    //
    if (UseCosmic) thisTMB->GetCounters();
    //
    //thisTMB->PrintCounters();
    //
    (*MyOutput_) << "mpc_delay_ =  " << dec << i << endl;
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
    (*MyOutput_) << endl;
    //
    thisMPC->read_fifos();
    //
    (*MyOutput_) << endl;
    //
    thisMPC->read_csr0();
    //
    if (UsePulsing && iterations < npulses ) goto REPEAT;
    //
  }
  //
  for (int i=0; i<DelaySize; i++) {
    (*MyOutput_) << "MPC0 winner delay=" << setw(3) << i << " gives " << MPC0Count[i] << endl;
  }
  //
  (*MyOutput_) << endl ;
  //
  for (int i=0; i<DelaySize; i++) {
    (*MyOutput_) << "MPC1 winner delay=" << setw(3) << i << " gives " << MPC1Count[i] << endl;
  }
  //
  (*MyOutput_) << endl ;
  //
  MpcDelay  /= (MpcDelayN  + 0.0001) ;
  Mpc0Delay /= (Mpc0DelayN + 0.0001) ;
  Mpc1Delay /= (Mpc1DelayN + 0.0001) ;
  //
  (*MyOutput_) << "Correct MPC  setting  : " << MpcDelay << endl ;
  (*MyOutput_) << "Correct MPC0 setting  : " << Mpc0Delay << endl ;
  (*MyOutput_) << "Correct MPC1 setting  : " << Mpc1Delay << endl ;
  //
  (*MyOutput_) << endl ;
  //
  MPCdelay_ = (int)(MpcDelay + 0.5);
  //
  return MPCdelay_;
}
//
//----------------------------------------------
// DMB parameters
//----------------------------------------------
void ChamberUtilities::MeasureAlctDavCableDelay() {
  //
  const int DelayMin = 0;
  const int DelayMax = 3;
  const int DelayRange = DelayMax - DelayMin + 1;
  //
  const int HistoMin   = ScopeMin_;
  const int HistoMax   = ScopeMax_;
  //
  const int desired_value = 2;
  //
  // Get initial values:
  int initial_value_of_register = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetAlctDavCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister(tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr));
  //
  float Average[DelayRange];
  int Histo[DelayRange][255];
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetAlctDavCableDelay(delay);         //insert the delay value into the DMB cable_delay_ parameter
    thisDMB->setcbldly(thisDMB->GetCableDelay()); //write the value
    ::sleep(1);
    //
    // Get the data:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data:
    Average[delay] = AverageHistogram(AlctDavScopeHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = AlctDavScopeHisto_[bin];
    //
    // Print the data:
    (*MyOutput_) << "alct_dav_cable_delay = " << delay << std::endl;
    PrintHistogram("ALCT DAV Scope",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  AlctDavCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "--> Best is alct_dav_cable_delay = " << AlctDavCableDelay_ << "..." << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  PrintHistogram("Best ALCT DAV Scope",Histo[AlctDavCableDelay_],HistoMin,HistoMax,Average[AlctDavCableDelay_]);  
  //  
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_value_of_register);
  thisTMB->WriteRegister(tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr));
  //
  thisDMB->SetAlctDavCableDelay(initial_delay_value);
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return;
}
//
void ChamberUtilities::MeasureTmbLctCableDelay() {
  //
  //
  const int DelayMin = 0;
  const int DelayMax = 3;
  const int DelayRange = DelayMax - DelayMin + 1;
  //
  const int HistoMin   = AffToL1aValueMin_;
  const int HistoMax   = AffToL1aValueMax_;
  //
  const int desired_value = 147;
  //
  // Get initial values
  int initial_value_of_register = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetTmbLctCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  int value_to_write = thisTMB->FillTMBRegister(tmb_trig_adr);
  thisTMB->WriteRegister(tmb_trig_adr,value_to_write);
  //
  float Average[DelayRange];
  int Histo[DelayRange][255];
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetTmbLctCableDelay(delay);        //insert the delay value into the DMB cable_delay_ parameter
    int cable_delay = thisDMB->GetCableDelay(); //get DMB::cable_delay_
    thisDMB->setcbldly(cable_delay);            //write the value
    ::sleep(1);
    //
    // Get the data from this delay value:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data from this delay value:
    Average[delay] = AverageHistogram(AffToL1aValueHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = AffToL1aValueHisto_[bin];
    //
    // Print the data from this delay value:
    (*MyOutput_) << "tmb_lct_cable_delay = " << delay << std::endl;
    PrintHistogram("AFF to L1A Value",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  TmbLctCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "--> Best is tmb_lct_cable_delay = " << TmbLctCableDelay_ << "..." << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  PrintHistogram("Best AFF to L1A Value"  ,Histo[TmbLctCableDelay_],HistoMin,HistoMax,Average[TmbLctCableDelay_]);  
  //  
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_value_of_register);
  thisTMB->WriteRegister(tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr));
  //
  thisDMB->SetTmbLctCableDelay(initial_delay_value);
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return;
}
//
void ChamberUtilities::MeasureCfebDavCableDelay() {
  //
  const int DelayMin = 0;
  const int DelayMax = 2;
  const int DelayRange = DelayMax - DelayMin + 1;
  //
  const int HistoMin   = ScopeMin_;
  const int HistoMax   = ScopeMax_;
  //
  const int desired_value = 2;
  //
  // Get initial values:
  int initial_value_of_register = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetCfebDavCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister( tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr) );
  //
  float Average[DelayRange];
  int Histo[DelayRange][255];
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetCfebDavCableDelay(delay);         //insert the delay value into the DMB cable_delay_ parameter
    thisDMB->setcbldly(thisDMB->GetCableDelay()); //write the value
    ::sleep(1);
    //
    // Get the data:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data:
    Average[delay] = AverageHistogram(CfebDavScopeHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = CfebDavScopeHisto_[bin];
    //
    // Print the data:
    (*MyOutput_) << "cfeb_dav_cable_delay = " << delay << std::endl;
    PrintHistogram("CFEB DAV Scope",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  CfebDavCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "--> Best is cfeb_dav_cable_delay = " << CfebDavCableDelay_ << "..." << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  PrintHistogram("Best CFEB DAV Scope",Histo[CfebDavCableDelay_],HistoMin,HistoMax,Average[CfebDavCableDelay_]);  
  //  
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_value_of_register);
  thisTMB->WriteRegister( tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr) );
  //
  thisDMB->SetCfebDavCableDelay(initial_delay_value);
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return;
}
//
void ChamberUtilities::MeasureCfebCableDelay() {
  //
  const int DelayMin = 0;
  const int DelayMax = 1;
  const int DelayRange = DelayMax - DelayMin + 1;
  //
  const int HistoMin   = ScopeMin_;
  const int HistoMax   = ScopeMax_;
  //
  const int desired_value = 2;
  //
  // Get initial values:
  int initial_value_of_register = thisTMB->GetMpcOutputEnable();
  int initial_delay_value = thisDMB->GetCfebCableDelay();
  //
  // Enable this TMB to send LCTs to MPC:
  thisTMB->SetMpcOutputEnable(1);
  thisTMB->WriteRegister( tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr) );
  //
  float Average[DelayRange];
  int Histo[DelayRange][255];
  //
  for (int delay=DelayMin; delay<=DelayMax; delay++) {
    //
    // Set the delay value:
    thisDMB->SetCfebCableDelay(delay);            //insert the delay value into the DMB cable_delay_ parameter
    thisDMB->setcbldly(thisDMB->GetCableDelay()); //write the value
    ::sleep(1);
    //
    // Get the data:
    ZeroDmbHistograms();
    ReadAllDmbValuesAndScopes();
    //
    // Preserve the data:
    Average[delay] = AverageHistogram(CfebDavScopeHisto_,HistoMin,HistoMax);
    for (int bin=HistoMin; bin<=HistoMax; bin++) 
      Histo[delay][bin] = CfebDavScopeHisto_[bin];
    //
    // Print the data:
    (*MyOutput_) << "cfeb_cable_delay = " << delay << std::endl;
    PrintHistogram("CFEB DAV Scope",Histo[delay],HistoMin,HistoMax,Average[delay]);  
    //
  }
  //
  // determine which delay value gives the reading we want:
  CfebCableDelay_ = DelayWhichGivesDesiredValue(Average,DelayMin,DelayMax,desired_value);
  //
  // print the delay value that gives the reading we want:
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  (*MyOutput_) << "--> Best is cfeb_cable_delay = " << CfebCableDelay_ << "..." << std::endl;
  (*MyOutput_) << "-----------------------------------------------------" << std::endl;
  PrintHistogram("Best CFEB DAV Scope",Histo[CfebCableDelay_],HistoMin,HistoMax,Average[CfebCableDelay_]);  
  //  
  // Set the registers back to how they were at beginning...
  thisTMB->SetMpcOutputEnable(initial_value_of_register);
  thisTMB->WriteRegister( tmb_trig_adr,thisTMB->FillTMBRegister(tmb_trig_adr) );
  //
  thisDMB->SetCfebCableDelay(initial_delay_value);
  thisDMB->setcbldly(thisDMB->GetCableDelay());
  //
  return;
}
//
void ChamberUtilities::ReadAllDmbValuesAndScopes() {
  //
  ZeroDmbHistograms();
  //
  const int number_of_reads = 120;
  PopulateDmbHistograms(number_of_reads);
  //
  AffToL1aScopeAverageValue_ = RoundOff( AverageHistogram(AffToL1aScopeHisto_,ScopeMin_,ScopeMax_) );
  CfebDavScopeAverageValue_  = RoundOff( AverageHistogram(CfebDavScopeHisto_ ,ScopeMin_,ScopeMax_) );
  TmbDavScopeAverageValue_   = RoundOff( AverageHistogram(TmbDavScopeHisto_  ,ScopeMin_,ScopeMax_) );
  AlctDavScopeAverageValue_  = RoundOff( AverageHistogram(AlctDavScopeHisto_ ,ScopeMin_,ScopeMax_) );
  //
  AffToL1aAverageValue_ = RoundOff( AverageHistogram(AffToL1aValueHisto_,AffToL1aValueMin_,AffToL1aValueMax_) );
  CfebDavAverageValue_  = RoundOff( AverageHistogram(CfebDavValueHisto_ ,CfebDavValueMin_ ,CfebDavValueMax_ ) );
  TmbDavAverageValue_   = RoundOff( AverageHistogram(TmbDavValueHisto_  ,TmbDavValueMin_  ,TmbDavValueMax_  ) );
  AlctDavAverageValue_  = RoundOff( AverageHistogram(AlctDavValueHisto_ ,AlctDavValueMin_ ,AlctDavValueMax_ ) );
  //
  return;
}
//
void ChamberUtilities::PrintAllDmbValuesAndScopes() {
  //
  PrintHistogram("AFF to L1A Value",AffToL1aValueHisto_,AffToL1aValueMin_,AffToL1aValueMax_,AffToL1aAverageValue_);
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("CFEB DAV Value"  ,CfebDavValueHisto_ ,CfebDavValueMin_ ,CfebDavValueMax_ ,CfebDavAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("TMB DAV Value"   ,TmbDavValueHisto_  ,TmbDavValueMin_  ,TmbDavValueMax_  ,TmbDavAverageValue_  );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("ALCT DAV Value"  ,AlctDavValueHisto_ ,AlctDavValueMin_ ,AlctDavValueMax_ ,AlctDavAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  //
  PrintHistogram("AFF to L1A Scope",AffToL1aScopeHisto_,ScopeMin_,ScopeMax_,AffToL1aScopeAverageValue_);
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("CFEB DAV Scope"  ,CfebDavScopeHisto_ ,ScopeMin_,ScopeMax_,CfebDavScopeAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("TMB DAV Scope"  ,TmbDavScopeHisto_   ,ScopeMin_,ScopeMax_,TmbDavScopeAverageValue_  );
  (*MyOutput_) << "---" << std::endl;
  PrintHistogram("ALCT DAV Scope"  ,AlctDavScopeHisto_ ,ScopeMin_,ScopeMax_,AlctDavScopeAverageValue_ );
  (*MyOutput_) << "---" << std::endl;
  //
  return;
}
//
//----------------------------------------------
// L1A accept windows
//----------------------------------------------
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
      if ( UseCosmic ) ::sleep(2);
      if ( UsePulsing) PulseCFEB(delay%16,0xa);
      wordcounts[delay] += thisTMB->GetWordCount();
      thisTMB->GetCounters();
      //      thisTMB->PrintCounters(19) ;
      //      thisTMB->PrintCounters(20) ;
      //      thisTMB->PrintCounters(21) ;
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
    //printf("delay = %d wordcount = %d TmbDavScope %d wordcount/nmuons %f \n",delay,wordcounts[delay],TmbDavScope[delay],wordcounts[delay]/(nmuons));
  }
  //
  RightTimeBin /= float(DataCounter) ;
  //
  printf("Right L1a delay setting is %f \n",RightTimeBin);
  //
  TMBL1aTiming_ = (int) (RightTimeBin+0.5);
  //
  return TMBL1aTiming_; ;
  //
}
//
//
int ChamberUtilities::FindTMB_L1A_delay(int idelay_min, int idelay_max ){
  //
  //bool useCCB = false; // if using TTC for L1A and start trig, bc0, set to false
  //cout << "Value of useCCB is" << useCCB <<endl;
  //
  //if (useCCB) thisCCB_->setCCBMode(CCB::VMEFPGA);
  // Not really necessary:
  //     thisTMB->alct_match_window_size_ = 3;
  //
  int tmb_in_l1a_window[255] = {};
  //
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
    (*MyOutput_) << "TMB_l1adelay=" << std::dec << i << ":" << endl;
    ::sleep(5);                   // accumulate statistics
    //if (useCCB) thisCCB_->stopTrigger();      // stop trigger
    thisTMB->GetCounters();      // read counter values
    
    //thisTMB->PrintCounters(); // display them to screen
    //    thisTMB->PrintCounters(8);  // display them to screen
    //    thisTMB->PrintCounters(19);
    //    thisTMB->PrintCounters(20);

    tmb_in_l1a_window[i] = thisTMB->GetCounter(19);
  }
  //
  float RightTimeBin      = 0;
  float RightTimeBinDenom = 0;
  int   DataCounter  = 0;
  //
  for (int i = idelay_min; i < idelay_max+1; i++){
    //
    (*MyOutput_) << "tmb_l1a_delay " << std::dec << std::setw(5) << i
		 << " : " << std::setw(10) << tmb_in_l1a_window[i] << std::endl;
    //
    RightTimeBin      += ((float) tmb_in_l1a_window[i]) * ((float) i);
    RightTimeBinDenom += (float) tmb_in_l1a_window[i];
    DataCounter++;
    //
  }
  //
  if (RightTimeBin > 100) {
    RightTimeBin /= RightTimeBinDenom ;
  } else {
    RightTimeBin = -999;
  }
  
  printf("Right L1a delay setting is %f \n",RightTimeBin);

  printf("\n");

  //if (useCCB) thisCCB_->setCCBMode(CCB::DLOG);      // return to "regular" mode for CCB

  TMBL1aTiming_ = (int) (RightTimeBin+0.5);

  (*MyOutput_) << "Best value of tmb_l1a_delay = " << std::dec << std::setw(5) << TMBL1aTiming_ << std::endl;

  return TMBL1aTiming_ ;

}
//
int ChamberUtilities::FindBestL1aAlct(){
  //
  // Now find the best L1a_delay value for the ALCT
  //
  //thisTMB->SetALCTPatternTrigger();
  //
  //  unsigned long HCmask[22];
  int WordCount[200], DMBCount[200];
  for (int i=0; i<200; i++) {
    WordCount[i] = 0; 
    DMBCount[i]=0;
  }
  //  //
  //  //unsigned cr[3]  = {0x80fc5fc0, 0x20a03786, 0x8}; // default conf register
  //  //
  //
  //  unsigned cr[3];
  //  //
  //  alct->GetConf(cr,1);
  //
  int minlimit = 0;
  int maxlimit = 150;
  //
  for (int l1a=minlimit; l1a<maxlimit; l1a++) {
    //
    //while (thisTMB->FmState() == 1 ) printf("Waiting to get out of StopTrigger\n");
    //
    //    int keyWG          = int((rand()/(RAND_MAX+0.01))*(alct->GetWGNumber())/6./2.);
    //    int ChamberSection = alct->GetWGNumber()/6;
    int keyWG          = int((rand()/(RAND_MAX+0.01))*(alct->GetNumberOfChannelsInAlct())/6./2.);
    //    int ChamberSection = alct->GetNumberOfChannelsInAlct()/6;
    //
    printf("\n");
    printf("-----> Injecting at %d \n",keyWG);
    for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
      for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct())/6; channel++) {
	if (channel==keyWG) {
	  alct->SetHotChannelMask(layer,channel,ON);
	} else {
	  alct->SetHotChannelMask(layer,channel,OFF);
	}
      }
    }
    alct->WriteHotChannelMask();
    alct->PrintHotChannelMask();
    //
    alct->SetL1aDelay(l1a);
    alct->WriteConfigurationReg();
    alct->PrintConfigurationReg();
    //
    //
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
  BestALCTL1aDelay_ = (int) (DelayBin+0.5) ;
  //
  return BestALCTL1aDelay_;
  //
}
//
int ChamberUtilities::FindALCT_L1A_delay(int minlimit, int maxlimit){
  //
  int WordCount[200];
  for (int i=0; i<200; i++) WordCount[i] = 0;
  //
  int ALCT_l1a_accepted[256] = {};
  //
  for (int l1a=minlimit; l1a<maxlimit+1; l1a++) {
    //
    alct->SetL1aDelay(l1a);
    alct->WriteConfigurationReg();
    //    alct->ReadConfigurationReg();
    //    alct->PrintConfigurationReg();
    thisTMB->ResetCounters();
    thisTMB->ResetALCTRAMAddress();
    ::sleep(5);                     //collect statistics
    thisTMB->GetCounters();
    //
    cout << endl;
    (*MyOutput_) << "L1a ALCT delay " << std::dec << l1a << std::endl;
    //
    thisTMB->PrintCounters(3);
    thisDMB->PrintCounters();
    //
    ALCT_l1a_accepted[l1a] = thisTMB->GetCounter(3);
    //
    thisTMB->DecodeALCT();
    WordCount[l1a] = thisTMB->GetALCTWordCount();
    if(thisTMB->GetALCTWordCount()==0 and (l1a>0) ) WordCount[l1a-1]=0;  //this makes no sense
    printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
    //
  }
  float DelayBin  = 0;
  //  int   DelayBinN = 0;
  float DelayBinDenom = 0;
  //
  for (int i=minlimit; i<maxlimit+1; i++){
    (*MyOutput_) << "alct_l1a_delay " << std::dec << std::setw(5) << i << ":" 
		 << std::setw(10) << ALCT_l1a_accepted[i] << std::endl;
    //
    //    if ( WordCount[i]>0 ) {
      DelayBin       += ALCT_l1a_accepted[i] * ( (float) i );
      DelayBinDenom  += ALCT_l1a_accepted[i] ;
      //      DelayBinN ++;
      //    }
    //
  }
  //
  //  printf(" DelayBin=%f DelaybinN=%d \n",DelayBin,DelayBinN);
  //
  //  DelayBin /= (DelayBinN+0.0001) ;
  if (DelayBin > 100.) {                //try to prevent all 0's from giving a false value
    DelayBin /= DelayBinDenom ;
  } else {
    DelayBin = -999;
  }
  //
  printf("In.Best L1a ALCT delay %f \n",DelayBin);
  //
  ALCTL1aDelay_ = (int) (DelayBin+0.5);
  //
  (*MyOutput_) << " Best value of alct_l1a_delay = " << ALCTL1aDelay_ << std::endl;
  //
  return ALCTL1aDelay_;
  //
}
//
//
//////////////////////////////////////////////////////////////////////
// scans to check functionality of electronics
//////////////////////////////////////////////////////////////////////
void ChamberUtilities::ALCTChamberScan(){
  //
  // Set up for this test:
  alct->SetTriggerMode(0);
  alct->WriteConfigurationReg();
  //
  int NPulses = 1;
  int chamberResult[MAX_NUM_WIRES_PER_LAYER];
  int chamberResult2[MAX_NUM_WIRES_PER_LAYER];
  int InJected[MAX_NUM_WIRES_PER_LAYER];
  //
  (*MyOutput_) << " *** New ************* " << std::endl ;
  //
  thisTMB->SetALCTPatternTrigger();
  //
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) chamberResult[keyWG] = 0;
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) chamberResult2[keyWG] = 0;
  for ( int keyWG=0; keyWG<MAX_NUM_WIRES_PER_LAYER; keyWG++) InJected[keyWG] = 0;
  //
  for (int Ninject=0; Ninject<NPulses; Ninject++){
    //
    for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) {
      //
      (*MyOutput_) << std::endl;
      printf("%c[01;43m", '\033');
      (*MyOutput_) << "Injecting in WG = " << dec << keyWG ;
      printf("%c[0m", '\033'); 
      (*MyOutput_) << endl;
      //
      for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct())/6; channel++) {
	  if (channel==keyWG) {
	    alct->SetHotChannelMask(layer,channel,ON);
	  } else {
	    alct->SetHotChannelMask(layer,channel,OFF);
	  }
	}
      }
      //
      alct->WriteHotChannelMask();
      alct->PrintHotChannelMask();	
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
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << keyWG/100 ;
  cout << endl;
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << ((keyWG/10)%10) ;
  cout << endl;
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << keyWG%10 ;
  cout << endl;
  cout << "InJected" << endl;
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << InJected[keyWG] ;
  cout << endl;
  cout << "ALCTChamberResult" << endl;
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << chamberResult[keyWG] ;
  cout << endl;
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) cout << chamberResult2[keyWG] ;
  cout << endl;
  printf("%c[0m", '\033'); 
  //
  for (int keyWG=0; keyWG<(alct->GetNumberOfChannelsInAlct())/6; keyWG++) ALCTWireScan_[keyWG] = chamberResult[keyWG];
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
  int CLCTInputList[5] = {0x1,0x2,0x4,0x8,0x10};
  //
  for (int List=0; List<5; List++){
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
  (*MyOutput_) << endl;
  (*MyOutput_) << " Number of Muons seen " << endl;
  //
  for (int CFEBs = 0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id="<<CFEBs<< " " ;
    for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
      (*MyOutput_) << setw(3) << Muons[CFEBs][HalfStrip] ;
    }
    (*MyOutput_) << endl;
  }
  //
  for (int i=0;i<5;i++) 
    for (int j=0; j<MaxStrip; j++) {
      CFEBStripScan_[i][j] = Muons[i][j];
    }
  //
  (*MyOutput_) << endl;
  //
  (*MyOutput_) << " Maximum number of hits " << endl;
  //
  for (int CFEBs = 0; CFEBs<5; CFEBs++) {
    (*MyOutput_) << "CFEB Id="<<CFEBs<< " " ;
    for (int HalfStrip = 0; HalfStrip<MaxStrip; HalfStrip++) {
      (*MyOutput_) << setw(3) << MuonsMaxHits[CFEBs][HalfStrip] ;
    }
    (*MyOutput_) << endl;
  }
}
//
//////////////////////////////////////////////////////////////////////
// useful peripheral crate functions
//////////////////////////////////////////////////////////////////////
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
  if (alct)    alct->configure();
  //
  //  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
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
  if (alct)    alct->configure();
  //
  if (thisTMB) thisTMB->StartTTC();
  if (thisTMB) thisTMB->EnableL1aRequest();
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  //
}
//
void ChamberUtilities::PulseRandomALCT(int delay){
  //
  alct->SetUpRandomALCT();
  //
  thisTMB->ResetALCTRAMAddress();
  PulseTestStrips(delay);
  printf("Decode ALCT\n");
  thisTMB->DecodeALCT();
  //
  printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  //
  (*MyOutput_) << std::endl;
  //
  thisDMB->PrintCounters();
  //
  (*MyOutput_) << std::endl;
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
  printf(" WordCount %d \n",thisTMB->GetALCTWordCount());
  //
  (*MyOutput_) << std::endl;
  //
  thisDMB->PrintCounters();
  //
}
//
void ChamberUtilities::PulseTestStrips(int delay){
  //
  //  int slot = thisTMB->slot();
  //
  if ( alct ) {
    //
    if ( beginning == 0 ) {
      //
      thisTMB->DisableCLCTInputs();
      //
      int amplitude = 255;
      int strip_mask = 0xff;
      alct->SetUpPulsing(amplitude,PULSE_AFEBS,strip_mask,ADB_ASYNC);
      //
      beginning = 1;
      //
      PulseTestStrips();
      //
    } else {
      //
      if (alct->GetChamberType().find("ME11")!=string::npos) {
	(*MyOutput_) << alct->GetChamberType().find("ME11") <<std::endl;
	(*MyOutput_) << alct->GetChamberType() <<std::endl;
	(*MyOutput_) << "ME11" <<std::endl;
      } else {
	(*MyOutput_) << "ME12" <<std::endl;
      }
      //
      thisCCB_->setCCBMode(CCB::VMEFPGA);
      thisCCB_->WriteRegister(0x28,delay);  //4Aug05 DM changed 0x789b to 0x7862
      //
      std::cout << "Setting delay to = " << std::hex << delay << std::endl ;
      //
      thisCCB_->ReadRegister(0x28);
      //
      thisCCB_->GenerateAlctAdbASync();	 
      //
    }
    //
  } else {
    cout << " No ALCT " << endl;
  }  
  //
}
//
void ChamberUtilities::LoadCFEB(int HalfStrip, int CLCTInputs, bool enableL1aEmulator ){
  //
  if ( enableL1aEmulator ) thisTMB->EnableInternalL1aSequencer();
  thisTMB->DisableCLCTInputs();
  thisTMB->SetCLCTPatternTrigger();
  //
  thisDMB->set_comp_thresh(0.1);
  thisDMB->set_dac(0.5,0);
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
  thisDMB->trigsetx(hp,CLCTInputs);
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
void ChamberUtilities::ZeroDmbHistograms() {
  //
  // scope histograms:
  for (int i=0; i<5; i++) {
    AffToL1aScopeHisto_[i] = 0;
    CfebDavScopeHisto_[i]  = 0;
    TmbDavScopeHisto_[i]   = 0;
    AlctDavScopeHisto_[i]  = 0;
  }
  //
  // Counter histograms
  for (int i=0; i<255; i++) {
    AffToL1aValueHisto_[i] = 0;
    CfebDavValueHisto_[i]  = 0;
    TmbDavValueHisto_[i]   = 0;
    AlctDavValueHisto_[i]  = 0;
  }
  //
  return;
}
//
void ChamberUtilities::PopulateDmbHistograms(int number_of_reads) {
  //
  for (int i=0; i<number_of_reads; i++) {
    //
    thisDMB->readtimingCounter();
    thisDMB->readtimingScope();
    //
    int AffToL1aValue = thisDMB->GetL1aLctCounter();
    int CfebDavValue  = thisDMB->GetCfebDavCounter();
    int TmbDavValue   = thisDMB->GetTmbDavCounter();
    int AlctDavValue  = thisDMB->GetAlctDavCounter();
    //
    int AffToL1aScope = thisDMB->GetL1aLctScope();
    int CfebDavScope  = thisDMB->GetCfebDavScope();
    int TmbDavScope   = thisDMB->GetTmbDavScope();
    int AlctDavScope  = thisDMB->GetAlctDavScope();
    //
    // Increment histograms with read values
    //
    AffToL1aValueHisto_[AffToL1aValue]++;
    CfebDavValueHisto_[CfebDavValue]++;
    TmbDavValueHisto_[TmbDavValue]++;
    AlctDavValueHisto_[AlctDavValue]++;
    //
    // Scope value is a 5-bit bitfield:
    for( int i=0; i<5; i++) {
      AffToL1aScopeHisto_[i] += ((AffToL1aScope>>i)&0x1);
      CfebDavScopeHisto_[i]  += ((CfebDavScope >>i)&0x1);
      TmbDavScopeHisto_[i]   += ((TmbDavScope  >>i)&0x1);
      AlctDavScopeHisto_[i]  += ((AlctDavScope >>i)&0x1);
    }
    //
  ::usleep(10000);
  }
  return;
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
    ::sleep(2);
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
  if (!thisCCB_) {
    std::cout << "CCB doesn't exist" << std::endl;
    return;
  }
  thisCCB_->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mod to work.
  thisCCB_->startTrigger();
  thisCCB_->bc0(); 
  //
}
//
//////////////////////////////////////////////////////////////////////
// analysis methods
//////////////////////////////////////////////////////////////////////
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
//
int ChamberUtilities::window_analysis(int * data, const int length) {
  // ** Determine the best value for the delay setting 
  //    based on vector of data "data", of length "length"

  // ASSUME:  Data wraps around (such as for phase)

  // ASSUME:  Channel is GOOD if it has fewer counts than:
  const int count_threshold = 10;

  // ASSUME:  We want to exclude windows of width less than the following value:
  const int width_threshold = 2;

  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  (*MyOutput_) << "For Window Analysis:" << std::endl;
  (*MyOutput_) << "  -> Counts > " << std::dec << count_threshold 
	       << " considered bad" << std::endl;

  // copy data for wrap-around:
  int twotimeslength = 2 * length;
  int copy[twotimeslength];
  int begin_channel = -1;           
  for (int i=0; i<twotimeslength; i++) {
    copy[i] = data[i % length];
    if (copy[i]>count_threshold && begin_channel<0)   //begin window scan on the first channel of "bad data"
      begin_channel = i;
  }
  if (begin_channel < 0) {
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "Scan is all 0's:  Something is wrong... "<< std::endl;
    (*MyOutput_) << "-----------------------------------------------" << std::endl;
    (*MyOutput_) << std::endl;
    return 999;
  }

  int end_channel = begin_channel+length;

  // Find the windows of "good data", beginning with the first channel of "bad data"
  int window_counter = -1;
  int window_width[length];
  int window_start[length];
  int window_end[length];
  for (int i=0; i<length; i++) {
    window_width[i] = 0;
    window_start[i] = 0;
    window_end[i] = 0;
  }
  for (int delay=begin_channel; delay<end_channel; delay++) {
    if (copy[delay] < count_threshold) {            //Here is a good channel...
      if (copy[delay-1] >= count_threshold)          //and it is a start of a good window
	window_start[++window_counter] = delay;
      window_width[window_counter]++;
    } 
  }

  if (window_counter < 0) {
    (*MyOutput_) << std::endl;
    (*MyOutput_) << "No windows with counts above count_threshold.  Something is wrong... "<< std::endl;
    (*MyOutput_) << "-----------------------------------------------" << std::endl;
    (*MyOutput_) << std::endl;
    return 999;
  }    

  int counter;
  for (counter=0; counter<=window_counter; counter++) {
    window_end[counter] = window_start[counter]+window_width[counter] -1;
    (*MyOutput_) << "Window = " << std::dec << counter;
    (*MyOutput_) << " is " << std::dec << window_width[counter] << " channels wide, ";
    (*MyOutput_) << " from " << std::dec << (window_start[counter] % 13)
		 << " to " << std::dec << (window_end[counter] % 13) << std::endl;    
  }

  (*MyOutput_) << "  -> window must be at least " << std::dec << width_threshold 
	       << " channels wide" << std::endl;

  //Determine middle of window(s)
  int channel;
  float value;
  float channel_ctr;
  float denominator;
  float average_channel[length];

  int bestValue = -1;

  for (counter=0; counter<=window_counter; counter++) {
    if (window_width[counter] > width_threshold) {                      // exclude narrow width windows
      channel_ctr=0.;
      denominator=0.;
      for (channel=window_start[counter]; channel<=window_end[counter]; channel++) {
	// For average, use unary operation on bits in counter (since 0 = good)
	value = (float) ~(copy[channel] | 0xffff0000);
	channel_ctr += value * (float) channel;
	denominator += value;
      }
      //
      average_channel[counter] = channel_ctr / denominator;
      //
      bestValue = (((int)(average_channel[counter]+0.5)) % 13);
      //
      (*MyOutput_) << "=> BEST DELAY VALUE (window " << std::dec << counter << ") = "
		   <<  bestValue  << " <=" << std::endl;
    }
  }
  (*MyOutput_) << "-----------------------------------------------" << std::endl;
  (*MyOutput_) << std::endl;
  //
  return bestValue;
}
//
int ChamberUtilities::DelayWhichGivesDesiredValue(float * values, const int min_delay, const int max_delay, int desired_value) {
  //
  //  (*MyOutput_) << "Find delay which gives closest to " << desired_value << " from following list:" << std::endl;
  //  for (int i=min_delay; i<=max_delay; i++) 
  //    (*MyOutput_) << "value[" << i << "] = " << values[i] << std::endl;
  //
  int best_delay_value = -999; 
  //
  const int number_of_values = max_delay - min_delay + 1;
  float vector_of_differences[number_of_values];
  //
  for (int delay=min_delay; delay<=max_delay; delay++) 
    vector_of_differences[delay] = abs(values[delay] - (float)(desired_value)); 
  //
  float min_difference=9999.;
  //
  // pick index whose vector value is closest to desired value:
  for (int delay=min_delay; delay<=max_delay; delay++) {
    if (vector_of_differences[delay] < min_difference) {
      best_delay_value = delay;
      min_difference = vector_of_differences[delay];
    }
  }
  return best_delay_value;
}
//
void ChamberUtilities::PrintHistogram(std::string label, int * histogram, int min, int max, int average) {
  //
  for (int i=min; i<=max; i++) {
    (*MyOutput_) << label << "[" << i << "] = " << histogram[i] << std::endl;
  }
  //
  (*MyOutput_) << label << " average (integer) = " << average << std::endl;
  //
  return;
}
//
void ChamberUtilities::PrintHistogram(std::string label, int * histogram, int min, int max, float average) {
  //
  for (int i=min; i<=max; i++) {
    (*MyOutput_) << label << "[" << i << "] = " << histogram[i] << std::endl;
  }
  //
  (*MyOutput_) << label << " average = " << average << std::endl;
  //
  return;
}
//
float ChamberUtilities::AverageHistogram(int * histogram, int min_value, int max_value) {
  //
  float numer = 0;
  float denom = 0;
  //
  for (int i=min_value; i<=max_value; i++) {
    numer += ((float) histogram[i]) *((float) (i));
    denom += (float) histogram[i];
  }
  //    (*MyOutput_) << "numerator   = " << numer << std::endl;
  //    (*MyOutput_) << "denominator = " << denom << std::endl;
  //
  float average = -999.;
  if (denom > 10) 
    average = numer / denom;
  //
  //    (*MyOutput_) << "average     = " << average << std::endl;
  return average;;
}
//
//
void ChamberUtilities::bit_to_array(int data, int * array, const int size) {
  int i;
  for (i=0; i<=(size-1); i++) {
    array[i] = (data >> i) & 0x00000001;
  }

  return;
}
//
////////////////////////////////////////////////////////////////
// Following should be deprecated?
////////////////////////////////////////////////////////////////
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
void ChamberUtilities::ALCTScanDelays(){

  //  unsigned long HCmask[22];
  int CountDelay[20];
  int alct0_quality = 0;
  int alct1_quality = 0;
  int alct0_bxn = 0;
  int alct1_bxn = 0;
  int alct0_key = 0;
  int alct1_key = 0;
  //
  //  for (int i=0; i< 22; i++) HCmask[i] = 0;
  for (int i=0; i< 20; i++) CountDelay[i] = 0;
  //
  thisTMB->SetALCTPatternTrigger();
  //
  for ( int nloop=0; nloop<20; nloop++){
    for ( int DelaySetting=0; DelaySetting<20; DelaySetting++){
      //
      int keyWG  = int(rand()/(RAND_MAX+0.01)*(alct->GetNumberOfChannelsInAlct())/6);
      //      int ChamberSection = alct->GetNumberOfChannelsInAlct()/6;
      //
      cout << endl ;
      //
      cout << "Injecting at " << dec << keyWG << endl;
      for(int layer=0; layer<MAX_NUM_LAYERS; layer++) {
	for(int channel=0; channel<(alct->GetNumberOfChannelsInAlct()/6); channel++) {
	  if (channel==keyWG) {
	    alct->SetHotChannelMask(layer,channel,ON);
	  } else {
	    alct->SetHotChannelMask(layer,channel,OFF);
	  }
	}
      }
      //
      alct->WriteHotChannelMask();
      alct->PrintHotChannelMask();	
      //
      for (int afeb=0; afeb<alct->GetNumberOfAfebs(); afeb++) 
	alct->SetAsicDelay(afeb,DelaySetting);
      alct->WriteAsicDelaysAndPatterns();
      //
      PulseTestStrips();
      printf("Decode ALCT\n");
      thisTMB->DecodeALCT();
      printf("After Decode ALCT\n");
      //
      // Now analyze
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
