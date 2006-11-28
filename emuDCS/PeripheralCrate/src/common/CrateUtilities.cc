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
CrateUtilities::CrateUtilities() : myCrate_(0), MpcTMBTestResult(-1)
{
  //
  std::cout << "CrateUtilities" << endl ;
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
  periph_output << myCrate_->CrateID() ;
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
    csc_output[csc] << std::dec << myCSCs[csc]->GetCrate()->CrateID();
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
    tmb_output[tmb] << myTmbs[tmb]->GetDsPretrigThresh();
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
    tmb_output[tmb] << myTmbs[tmb]->GetMPCdelay();
    tmb_table[tmb].push_back(tmb_output[tmb].str());
    //
    tmb_output[tmb].str("");
    tmb_output[tmb] << myTmbs[tmb]->GetMpcTXdelay();
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
      cfeb_table.push_back(std::vector<std::string>());
      //
      cfeb_output.str("");
      cfeb_output << cfeb+(100*dmb);
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << myCfebs[cfeb].number();
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << "1";
      cfeb_table[cfeb_line].push_back(cfeb_output.str());    
      //
      cfeb_output.str("");
      cfeb_output << myDmbs[dmb]->slot();
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
    int tmbKey = CSCid;
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
    int nAFEBs = myAlcts[alct]->GetNumberOfAfebs();
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
      afeb_output << afebs+(100*alct);
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
void CrateUtilities::MpcTMBTest(int Nloop){
  //
  int NFound = 0;
  //
  for(int nloop = 0; nloop<Nloop; nloop++) {
  //
  myCrate_->mpc()->SoftReset();
  myCrate_->mpc()->configure();
  myCrate_->mpc()->read_csr0();
  myCrate_->mpc()->read_fifos();
  myCrate_->mpc()->read_csr0();
  //
  std::vector <TMB*> myTmbs = myCrate_->tmbs();
  //
  for (int i=0; i<myTmbs.size(); i++) {
    //
    (myTmbs[i]->ResetInjectedLCT());
    //
  }
  //
  (myCrate_->mpc())->ResetFIFOBLct();
  //
  for (int i=0; i<myTmbs.size(); i++) {
    //
    myTmbs[i]->InjectMPCData(myTmbs.size(),0,0); //Random Data
    //
  }
  //
  myCrate_->ccb()->FireCCBMpcInjector();
  //
  myCrate_->mpc()->read_fifos();
  //
  if (((myCrate_->mpc())->GetFIFOBLct0()).size() == 0 ) {
    std::cout << "No data" << std::endl;
    return;
  }
  //
  int NInjected = 0;
  //
  int NFrames = myTmbs.size();
  if (NFrames > 5 ) NFrames = 5;
  //
  for( int frame = 0; frame < NFrames ; frame++ ) {
    //
    std::cout << " Frame " << frame << " " << std::hex << std::setw(8) <<
      " " << ((myCrate_->mpc())->GetFIFOBLct0())[frame] <<
      " " << ((myCrate_->mpc())->GetFIFOBLct1())[frame] <<
      " " << ((myCrate_->mpc())->GetFIFOBLct2())[frame] 
	      << std::endl ;
    //
    unsigned long int MPCLct0= ((myCrate_->mpc())->GetFIFOBLct0())[frame] ;
    unsigned long int MPCLct1= ((myCrate_->mpc())->GetFIFOBLct1())[frame] ;
    unsigned long int MPCLct2= ((myCrate_->mpc())->GetFIFOBLct2())[frame] ;
    //
    // Copy LCT data and sort...
    //
    std::vector <unsigned long int> InjectedLCT;
    //
    for (int i=0; i<myTmbs.size(); i++) {
      if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	InjectedLCT.push_back((myTmbs[i]->GetInjectedLct0())[frame]);
	InjectedLCT.push_back((myTmbs[i]->GetInjectedLct1())[frame]);
      }
    }
    std::sort(InjectedLCT.begin(),InjectedLCT.end(), std::greater<int>() );
    //
    std::cout << "Sorted : " ;
    for ( int vec=0; vec<InjectedLCT.size(); vec++) std::cout << InjectedLCT[vec] << " "  ;
    //
    std::cout << std::endl ;
    //
    for (int i=0; i<myTmbs.size(); i++) {
      if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct0 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct0 )  {
	  std::cout << "Found0 " << std::endl;
	  NFound++;
	}
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct1 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct1 )  {
	  std::cout << "Found1 " << std::endl;
	  NFound++;
	}
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct2 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct2 )  {
	  std::cout << "Found2 " << std::endl;
	  NFound++;
	}
      }
    }
  }
  //
  std::cout << "NFound " << std::dec << NFound%15 << " " << NFound << std::endl;
  if ( NFound == (nloop+1)*3*NFrames ) {
    std::cout << "Passed" << std::endl;
    MpcTMBTestResult = 1;
  } else {
    std::cout << "Failed " << " nloop=" << nloop << std::endl;
    MpcTMBTestResult = 0;
    break;
  }
  //
}
}
