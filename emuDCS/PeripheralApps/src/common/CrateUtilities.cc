#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <sstream>
//
#include "CrateUtilities.h"
#include "MPC.h"
#include "TMB.h"
#include "ALCTController.h"
#include "DAQMB.h"
#include "CCB.h"
#include "VMEController.h"
//
using namespace std;
//
CrateUtilities::CrateUtilities() : MpcTMBTestResult(-1), myCrate_(0)
{
  //
  debug_ = false;
  std::cout << "CrateUtilities" << endl ;
  //
  MyOutput_ = &std::cout ;
  //
}
//
//
CrateUtilities::~CrateUtilities(){
  //
  std::cout << "Destructor" << std::endl ;
  //
}
//
void CrateUtilities::DumpTstoreTables(){
  //
  for( unsigned int periph_entry=0; 
       periph_entry<periph_table.size(); periph_entry++){
    std::cout << periph_table[periph_entry] << std::endl;
  }
  //
}
//
void CrateUtilities::CreateTstoreTables(){
  //
  CCB * myCCB = myCrate_->ccb();
  MPC * myMPC = myCrate_->mpc();
  std::vector<TMB *>   myTmbs = myCrate_->tmbs();  
  std::vector<DAQMB *> myDmbs = myCrate_->daqmbs();  
  std::vector<ALCTController *> myAlcts = myCrate_->alcts();  
  std::vector<Chamber *> myCSCs = myCrate_->chambers();
  //
  std::ostringstream periph_output;
  //
  periph_output.str("");
  periph_output << myCCB->GetBxOrbit();
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->GetCCBmode();
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->slot();
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << "2004" ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << "21" ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << "20/06/2004" ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << "v5" ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCrate_->CrateID() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCrate_->GetLabel() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->Getl1adelay() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myMPC->GetBoardID() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myMPC->slot() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  //periph_output << myCrate_->CrateID() ;
  periph_output << "1";
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCrate_->vmeController()->port() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myMPC->GetSerializerMode() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->GetSPS25ns() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myMPC->GetTransparentMode() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->GetTTCmode() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->GetTTCrxCoarseDelay() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCCB->GetTTCrxID() ;
  periph_table.push_back(periph_output.str());
  //
  periph_output.str("");
  periph_output << myCrate_->vmeController()->GetVMEAddress() ;
  periph_table.push_back(periph_output.str());
  //
  std::ostringstream csc_output[myCSCs.size()];
  //
  for(unsigned int csc=0; csc<myCSCs.size(); csc++) {
    //
    int tmbSlot = myCSCs[csc]->GetTMB()->slot();
    int CSCid   = tmbSlot/2;
    if (tmbSlot>12 ) CSCid -=1;
    int cscKey = CSCid;
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << "1" ;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << "20/20/2006" ;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << "v4" ;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << CSCid;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << cscKey;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << myCSCs[csc]->GetLabel() ;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    csc_output[csc] << std::dec << "1" ;
    csc_table[csc].push_back(csc_output[csc].str());
    //
    csc_output[csc].str("");
    //csc_output[csc] << std::dec << myCSCs[csc]->GetCrate()->CrateID();
    csc_output[csc] << std::dec << "1";
    csc_table[csc].push_back(csc_output[csc].str());
    //
  }
  //
  std::ostringstream tmb_output[myTmbs.size()];
  //
  for( unsigned tmb=0; tmb<myTmbs.size(); tmb++) {
    //
    int tmbSlot = myTmbs[tmb]->slot();
    int CSCid   = tmbSlot/2;
    if (tmbSlot>12 ) CSCid -=1;
    int cscKey = CSCid;
    int tmbKey = CSCid;
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetAlctMatchWindowSize();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetALCTrxPhase();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetALCTtxPhase();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetAlctVpfDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetCFEB0delay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetCFEB1delay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetCFEB2delay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetCFEB3delay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetCFEB4delay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << CSCid;
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << cscKey;
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetDmbTxDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetFifoPreTrig();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetFifoTbins();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetHsPretrigThresh();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetL1aDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetL1aWindowSize();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetMinHitsPattern();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetMpcRxDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetMpcTxDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetRatTmbDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetRequestL1a();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetRpc0RatDelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetRpcBxnOffset();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetShiftRpc();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << std::dec << tmbKey ;
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->slot();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << "2004";
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetTrgMode();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
  }
  //
  std::ostringstream dmb_output[myDmbs.size()];
  std::ostringstream cfeb_output;
  int cfeb_line=0;
  //
  for( unsigned dmb=0; dmb<myDmbs.size(); dmb++) {
    //
    int dmbSlot = myDmbs[dmb]->slot();
    int CSCid   = (dmbSlot-1)/2;
    if (dmbSlot>12 ) CSCid -=1;
    int cscKey = CSCid;
    int dmbKey = CSCid;
    int CrateId = 0;
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetAlctDavDelay();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCableDelay();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCalibrationL1aDelay();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCalibrationLctDelay();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCompMode();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCompTiming();
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << CrateId;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << CSCid;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << cscKey;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << dmbKey;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->slot() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCfebClkDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetFebDavDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetInjectDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetInjectorDac() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetL1aDavDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetPreBlockEnd() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetPulseDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetPulseDac() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetPushDavDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetCompThresh() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetFebDavDelay() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    dmb_output[dmb].str("");
    dmb_output[dmb] << myDmbs[dmb]->GetxLatency() ;
    dmb_table[dmb].push_back(dmb_output[dmb].str());
    //
    std::vector<CFEB> myCfebs = myDmbs[dmb]->cfebs();
    //
    for( unsigned int cfeb=0; cfeb<myCfebs.size(); cfeb++) {
      //
      int CfebKey = (dmbKey-1)*5+(cfeb+1);
      //
      cfeb_table.push_back(std::vector<std::string>());
      //
      cfeb_output.str("");
      cfeb_output << CfebKey;
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << myCfebs[cfeb].number();
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << CSCid;
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << dmbKey;
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_line++;
      //
    }
    //
  }
  //
  std::ostringstream alct_output[myAlcts.size()];
  std::ostringstream afeb_output;
  int afeb_line=0;
  //
  for( unsigned alct=0; alct<myAlcts.size(); alct++) {
    //
    int alctSlot = myAlcts[alct]->GetTMB()->slot();
    int CSCid   = (alctSlot)/2;
    if (alctSlot>12 ) CSCid -=1;
    //    int tmbKey = CSCid;
    int alctKey = CSCid;
    //
    alct_output[alct].str("");
    alct_output[alct] << std::dec << CSCid;
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << "pattern_file";
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetAlctWriteAmode();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteBxcOffset();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteCcbEnable();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetChamberType();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << CSCid;
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteExtTrigEnable();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteFifoMode();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteFifoPretrig();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteFifoTbins();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteL1aDelay();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteL1aInternal();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteL1aOffset();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteL1aWindowSize();
    alct_table[alct].push_back(alct_output[alct].str());
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWritePretrigNumberOfPattern();
    alct_table[alct].push_back(alct_output[alct].str());    
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWritePretrigNumberOfLayers();
    alct_table[alct].push_back(alct_output[alct].str());     
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteSendEmpty();
    alct_table[alct].push_back(alct_output[alct].str());    
    //
    alct_output[alct].str("");
    alct_output[alct] << CSCid;
    alct_table[alct].push_back(alct_output[alct].str());    
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteTriggerInfoEnable();
    alct_table[alct].push_back(alct_output[alct].str());    
    //
    alct_output[alct].str("");
    alct_output[alct] << myAlcts[alct]->GetWriteTriggerMode();
    alct_table[alct].push_back(alct_output[alct].str());    
    //
    // Fill AFEB table
    //
    unsigned nAFEBs = myAlcts[alct]->GetNumberOfAfebs();
    //
    for( unsigned afebs=0; afebs<nAFEBs; afebs++){
      //
      int AfebKey = (alctKey-1)*42+(afebs+1);
      //
      afeb_table.push_back(std::vector<std::string>());
      //
      afeb_output.str("");
      afeb_output << myAlcts[alct]->GetWriteAsicDelay(afebs);
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_output.str("");
      afeb_output << AfebKey;
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_output.str("");
      afeb_output << afebs;
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_output.str("");
      afeb_output << myAlcts[alct]->GetAfebThresholdDAC(afebs);
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_output.str("");
      afeb_output << alctKey;
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_output.str("");
      afeb_output << CSCid;
      afeb_table[afeb_line].push_back(afeb_output.str());    
      //
      afeb_line++;
      //
    }
  }
  //
}
//
void CrateUtilities::MpcTMBTest(int Nloop) {
  //
  MpcTMBTest(Nloop, 0, 0);  //default is no scan for "safe window"
  return;
}
//
void CrateUtilities::MpcTMBTest(int Nloop, int min_delay, int max_delay){
  //
  // Allows for scan over "safe window"
  //
  int NFound[255] = {};
  //
  for (int delay=min_delay; delay<=max_delay; delay++) {
    //
    myCrate_->mpc()->SoftReset();
    myCrate_->mpc()->configure();
    //    myCrate_->mpc()->read_csr0();
    //
    if (delay) {
      std::cout << "Delay value = " << delay << std::endl;
      int value = myCrate_->mpc()->ReadRegister(0xAC);
      //  int delay=15;
      value = (value & 0x00ff) | ((delay<<8)&0xff00);
      myCrate_->mpc()->WriteRegister(0xAC,value);
      myCrate_->mpc()->ReadRegister(0xAC);
    }
    //
    myCrate_->mpc()->read_fifos();   //read FIFOB to clear it
    //    myCrate_->mpc()->read_csr0();
    //
    int sequential_events_with_fifob_empty=0;
    //
    std::vector <unsigned long int> InjectedCSCId;
    //
    int nloop;
    for(nloop = 0; nloop<Nloop; nloop++) {
      //
      std::cout << "Begin Event " << nloop << std::endl;
      //
      std::vector <TMB*> myTmbs = myCrate_->tmbs();
      //
      // clear the vectors of input LCTs
      for (unsigned i=0; i<myTmbs.size(); i++) {
	(myTmbs[i]->ResetInjectedLCT());
      }
      //
      // clear the vectors of read out LCTs
      (myCrate_->mpc())->ResetFIFOBLct();
      //
      int NFrames = myTmbs.size();
      if (NFrames > 5 ) NFrames = 5;
      //
      for (unsigned i=0; i<myTmbs.size(); i++) {
	myTmbs[i]->InjectMPCData(NFrames,0,0); //Random Data
	//      myTmbs[i]->ReadBackMpcRAM(NFrames)
      }
      //
      //  myCrate_->ccb()->FireCCBMpcInjector();
      myCrate_->ccb()->injectTMBPattern();
      //
      myCrate_->mpc()->read_fifos();
      //
      if (((myCrate_->mpc())->GetFIFOBLct0()).size() == 0 ) {
	std::cout << "No data in FIFO B for event " << nloop << std::endl;
	sequential_events_with_fifob_empty++;
	//
	if (sequential_events_with_fifob_empty > 9) {
	  std::cout << "Number of events in a row with FIFO-B empty = " 
		    << std::dec << sequential_events_with_fifob_empty << std::endl;
	  std::cout << "...quitting..." << std::endl;
	  break;
	} else {
	  continue;
	}
      } else {
	sequential_events_with_fifob_empty = 0;
      }
      //
      int N_LCTs_found_this_pass = 0;
      //
      for (int frame = 0; frame < NFrames ; frame++ ) {
	//
	unsigned long int MPCLct0= ((myCrate_->mpc())->GetFIFOBLct0())[frame] ;
	unsigned long int MPCLct1= ((myCrate_->mpc())->GetFIFOBLct1())[frame] ;
	unsigned long int MPCLct2= ((myCrate_->mpc())->GetFIFOBLct2())[frame] ;
	//
	// Copy LCT data and sort according to the MPC algorithm...
	//
	std::vector <unsigned long int> SortingLCT;
	//
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  int TMBslot = myTmbs[i]->slot();
	  if (debug_)
	    std::cout << "slot = " << std::dec << TMBslot << "..." << std::endl;
	  if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	    //
	    // MPC algorithm picks the LCT according to:
            // 1) the highest quality
	    // 2) if two LCTs have the same quality, it picks the one from the highest number slot
	    //
	    // Can mimic this by inserting the CSC ID value (bits 12-15 in the LCT)
	    // behind the quality bits and sorting by largest value...
	    //
	    unsigned long int original_lct0_value = (myTmbs[i]->GetInjectedLct0())[frame];
	    //
	    if (debug_)
	      std::cout << "LCT0:  original value = " << std::hex << original_lct0_value << std::endl;
	    //
	    // extract the csc_id from the LCT
	    unsigned long int csc_id = (original_lct0_value & 0x0000f000) >> 12;
	    unsigned long int lct0_sorting_value = 
	      ( (original_lct0_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	      ( (original_lct0_value & 0x07ff0000) >>  4) | //move information over to make space for csc_id
	      ( (csc_id              & 0xf)        << 23);  //insert the csc_id
	    //
	    if (debug_) {
	      std::cout << "csc_id = " << std::hex << csc_id << std::endl;
	      std::cout << "LCT0:  shifted cscid  = " << std::hex << lct0_sorting_value << std::endl;
	    }
	    //
	    SortingLCT.push_back(lct0_sorting_value);
	    //
	    unsigned long int original_lct1_value = (myTmbs[i]->GetInjectedLct1())[frame];
	    //
	    if (debug_)
	      std::cout << "LCT1:  original value = " << std::hex << original_lct1_value << std::endl;
	    //
	    // extract the csc_id from the LCT
	    csc_id = (original_lct1_value & 0x0000f000) >> 12;
	    unsigned long int lct1_sorting_value = 
	      ( (original_lct1_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	      ( (original_lct1_value & 0x07ff0000) >>  4) | //move information over to make space for csc_id
	      ( (csc_id              & 0xf)        << 23);  //insert the csc_id
	    //
	    if (debug_) {
	      std::cout << "csc_id = " << std::hex << csc_id << std::endl;
	      std::cout << "LCT1:  shifted cscid  = " << std::hex << lct1_sorting_value << std::endl;
	    }	    
	    //
	    SortingLCT.push_back(lct1_sorting_value);
	  }
	}
	std::sort(SortingLCT.begin(),SortingLCT.end(), std::greater<int>() );
	//
	// remove slot from sorting vector to print out value:
	std::vector <unsigned long int> InjectedLCT;
	InjectedCSCId.clear();
	//
	for (unsigned vec=0; vec<SortingLCT.size(); vec++) {
	  //
	  unsigned long int original_value = SortingLCT[vec];
	  //
	  // extract the csc_id from the LCT
	  unsigned long int csc_id = (original_value & 0x07800000) >> 23;
	  unsigned long int return_value = 
	    ( (original_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	    ( (original_value & 0x007ff000) <<  4) | //move information over to make space for csc_id
	    ( (csc_id              & 0xf)   << 12);  //insert the csc_id
	    //
	    InjectedLCT.push_back(return_value);
	    InjectedCSCId.push_back(csc_id);
	}
	//
	std::cout << "MPC data in event " << frame << ":" << std::hex 
		  << " " << MPCLct0 << " " << MPCLct1 << " " << MPCLct2 
		  << std::endl ;
	//
	std::cout << "Data sorted in code: ";
	for (unsigned vec=0; vec<InjectedLCT.size(); vec++) 
	  std::cout << std::hex << InjectedLCT[vec] << " "  ;
	std::cout << std::endl ;
	//
	if (debug_) {
	  std::cout << "Data sorted w/cscid: ";
	  for (unsigned vec=0; vec<SortingLCT.size(); vec++) 
	    std::cout << std::hex << SortingLCT[vec] << " "  ;
	  std::cout << std::endl;
	}
	//
	if ( InjectedLCT[0] == MPCLct0 ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 0" << std::endl;
	}
	//
	if ( InjectedLCT[1] == MPCLct1 ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 1" << std::endl;
	}
	//
	if ( InjectedLCT[2] == MPCLct2 ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 2" << std::endl;
	  //
	}
	//
	//	for (unsigned i=0; i<myTmbs.size(); i++) {
	//	  if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct0 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct0 )  {
	//	      std::cout << " LCT0 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct1 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct1 )  {
	//	      std::cout << " LCT1 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct2 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct2 )  {
	//	      std::cout << " LCT2 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	  }
	//	}
	//	std::cout << std::endl;
      }
      //
      std::cout << "N_LCTs found this pass = " << std::dec << N_LCTs_found_this_pass << std::endl;
      std::cout << "N_LCTs found for delay " << std::dec << delay << " = " << NFound[delay] << std::endl;
      //
      if ( NFound[delay] == (nloop+1)*3*NFrames ) {
	//
	if (delay == 0) MpcTMBTestResult = 1;
	//
      } else {
	//
	if (delay == 0) MpcTMBTestResult = 0;
	break;
      }
    }
    //
    if (delay == 0 && MpcTMBTestResult == 1) {
      (*MyOutput_) << "TMB-MPC Crate Test PASS, N_LCT = " << std::dec << NFound[delay] << std::endl;
      std::cout << "TMB-MPC Crate Test PASS, N_LCT = " << std::dec << NFound[delay] << std::endl;
    } else if (delay == 0 && MpcTMBTestResult == 0) {
      (*MyOutput_) << "TMB-MPC Crate Test FAIL " << ", nloop=" << nloop << std::endl;
      std::cout << "TMB-MPC Crate Test FAIL " << ", nloop=" << nloop << std::endl;
    }
    //
    //    std::cout << "Broke out, try next delay..." << std::endl;
  }
  //
  if (min_delay>0 || max_delay>0) {
    (*MyOutput_) << "MPC safe-window scan:" << std::endl;
    for (int delay=min_delay; delay<=max_delay; delay++) {
      (*MyOutput_) << "N_LCT[ " << delay << "] = " << NFound[delay] << std::endl;
    }
  }
  //
  return;
}
