#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "EmuController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "CCB.h"
#include "TMB.h"
#include "MPC.h"
#include "VMEController.h"
#include "DcsEmuController.h"
#ifdef USEDCS
#include "EmuDcs.h"
#include "DcsDimCommand.h"
#endif

#ifdef USEDCS
//--- temporary storage of InfoMonitorService implementation
class InfoMonitorService : public DimInfo{
  std::string name;
  void infoHandler(){
    if(name=="DCS_SERVICE"){
      ACK_RECEIVED = true;
      std::cout << "InfoMonitorService: DCS_Service received" << std::endl;
    }
  }
public:
  bool ACK_RECEIVED;
  InfoMonitorService(char *name) : DimInfo(name, -1.0){
    this->name=std::string(name);
  }
};
//--- end InfoMonitorService implementation 
#endif

//=======================================================
DcsEmuController::DcsEmuController() : EmuController(){
#ifdef USEDCS
  monitorService=0;
  mEmuDcs=0;
#endif
}

//=======================================================

void DcsEmuController::configure_simple(){
 
#ifdef USEDCS
  DcsSetup();   // DCS contrib
  if (mEmuDcs) DcsDisable(); // DCS contrib
#endif 
  // read the configuration
  std::vector<emu::pc::Crate*> myCrates = /*(selector()).*/crates();  // emulib4
     printf("======================================configure_simpl crates=%d\n",myCrates.size());
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    if(myCrates[i]) configure_simple(myCrates[i]);
  }
#ifdef USEDCS
  if (mEmuDcs) DcsEnable();  // DCS contrib

  // (re-)configure EmuDcs
  if (mEmuDcs) delete mEmuDcs;
  printf("emudcs -------------------------------------------------------------------------------------------------------->\n");
  mEmuDcs=new EmuDcs(this); // emulib4
  printf("emudcs -------------------------------------------------------------------------------------------------------->\n");
#endif  
}


//=====================================================
void DcsEmuController::configure_simple(emu::pc::Crate *crate) {
  //
  emu::pc::CCB * ccb = crate->ccb();
  emu::pc::MPC * mpc = crate->mpc();

  printf("======================================configure_simpl\n");
  //
  //theController->init();
  //
  //////////////////ccb->configure();
  //
  ///////////////////if(mpc) mpc->configure();
  //
  std::vector<emu::pc::TMB*> myTmbs = /*(selector()).*/crate->tmbs(); // emulib4
  //  std::vector<TMB*> myTmbs = crate->tmbs();
    printf("======================================configure_simpl tmbs=%d\n",myTmbs.size());
  for(unsigned i =0; i < myTmbs.size(); ++i) {
    if (myTmbs[i]->slot()<22){
     printf("slot tmb======%d\n",myTmbs[i]->slot());
      //
      ///////////////   myTmbs[i]->configure();
      //
      emu::pc::ALCTController * alct = myTmbs[i]->alctController();
      if(alct) {
	/////////////	std::cout << "alct # =" << i << std::endl;
	////////////	alct->configure();
	//
      }
      //
      //      RAT * rat = myTmbs[i]->getRAT();
      //      if(rat) {
      //	//
      //	rat->configure();
      //	//
      //      }
      //
    }
    //
  }
  //
  std::vector<emu::pc::DAQMB*> myDmbs = /*(selector()).*/crate->daqmbs();  // emulib4
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    if (myDmbs[i]->slot()<22){
      printf("slot dmb ======%d\n",myDmbs[i]->slot());
      //////////myDmbs[i]->restoreCFEBIdle();
      //////////myDmbs[i]->restoreMotherboardIdle();
      //////////myDmbs[i]->configure();
    }
    }
  //  
}
//==================================================================
#ifdef USEDCS
void DcsEmuController::DcsSetup(){

 char* envvar;
 static char envbuf[256];
 // envvar = getenv("DIM_DNS_NODE");
 // if(!envvar){
 // sprintf(envbuf,"DIM_DNS_NODE=%s","pcmsucr2.cern.ch");
 // if(putenv(envbuf)){
 //  printf("error occured\n");
 // }  
 // }

  std::cout << "TestBeamController: DCS Setup" << std::endl;
  if (monitorService){
    delete monitorService;
    monitorService=0;
  }
  monitorService = new InfoMonitorService("DCS_SERVICE");
}


void DcsEmuController::DcsEnable(){
  std::cout << "TestBeamController: DCS Enable" << std::endl;
  monitorService->ACK_RECEIVED = false;
  DimClient::sendCommand("LV_1_COMMAND","DCS_ENABLE");

  // wait for max 1s on an acknowledge
  int counter(0);
  while(true){
    usleep(50000);
    counter++;
    if(monitorService->ACK_RECEIVED) break;
    if(counter==20) {
      std::cerr << "TestBeamController: WARNING - DCS does not reply ... timing out" << std::endl;
      break;
    }
  }
}


void DcsEmuController::DcsDisable(){
  std::cout << "TestBeamController: DCS Disable" << std::endl;
  monitorService->ACK_RECEIVED = false;
  DimClient::sendCommand("LV_1_COMMAND","DCS_ENABLE");
  // wait for max 1s on an acknowledge
  int counter(0);
  while(true){
    usleep(50000);
    counter++;
    if(monitorService->ACK_RECEIVED) break;
    if(counter==20) {
      std::cerr << "TestBeamController: WARNING - DCS does not reply ... timing out" << std::endl;
      break;
    }
  } 
}
#endif


