//-----------------------------------------------------------------------
// $Id: TestBeamCrateController.cc,v 1.1 2006/01/21 19:55:02 gilmore Exp $
// $Log: TestBeamCrateController.cc,v $
// Revision 1.1  2006/01/21 19:55:02  gilmore
// *** empty log message ***
//
// Revision 2.1  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.23  2004/06/18 23:52:34  tfcvs
// Introduced code for DCS/PCcontrol crate sharing. (FG)
//  -code additions still commented out, awaiting validation-
//
// Revision 1.22  2004/06/05 19:37:55  tfcvs
// Clean-up (FG)
//
// Revision 1.21  2004/06/03 22:00:39  tfcvs
// Changed the order in which DMB and TMB/ALCT are configured.
// Apparently, a yet-to-be-identified call in the TMB or ALCT class has
// an adverse effect on DAQMB:set_comp_mode(). Although this relocation
// seems to solve the problem it should not be the final solution ...
// Also, removed the repeated DAQMB:calctrl_fifomrst() call, which does not
// affect operation. (JG/FG)
//
// Revision 1.20  2004/06/01 09:52:07  tfcvs
//  enabled CCB in enable(), added CVS-stuff, cleaned up includes (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "TestBeamCrateController.h"
#include "Crate.h"
#include "DDU.h"
#include "DCC.h"
#include "VMEController.h"
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


TestBeamCrateController::TestBeamCrateController(){
  // clear pointers
#ifdef USEDCS
  monitorService=0;
  mEmuDcs=0;
#endif
}


void TestBeamCrateController::configure() {
  // read the configuration
#ifdef USEDCS
  DcsSetup();   // DCS contrib
  if (mEmuDcs) DcsDisable(); // DCS contrib
#endif

  std::vector<Crate*> myCrates = theSelector.crates();
  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    configure(myCrates[i]);
  }
#ifdef USEDCS
  if (mEmuDcs) DcsEnable();  // DCS contrib

  // (re-)configure EmuDcs
  if (mEmuDcs) delete mEmuDcs;
  mEmuDcs=new EmuDcs();
#endif

}

void TestBeamCrateController::configureNoDCS() {

  // read the configuration
  std::vector<Crate*> myCrates = theSelector.crates();
  
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    configure(myCrates[i]);
  }

}


void TestBeamCrateController::configure(Crate * crate) {


  std::vector<DDU*> myDDUs = theSelector.ddus(crate);
  for(unsigned i =0; i < myDDUs.size(); ++i) {
    myDDUs[i]->configure();
  }

  ::sleep(1);

  std::vector<DCC*> myDCCs = theSelector.dccs(crate);
  for(unsigned i =0; i < myDCCs.size(); ++i) {
    myDCCs[i]->configure();
  }

}


void  TestBeamCrateController::enable() {
#ifdef USEDCS
  if (mEmuDcs) DcsDisable(); // DCS contrib
#endif
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    enable(myCrates[i]);
  }
#ifdef USEDCS
  if (mEmuDcs) DcsEnable(); // DCS contrib
#endif
  std::cout << "TAKING DATA" << std::endl;
}


void TestBeamCrateController::enable(Crate * crate) {
  //
 
}


void TestBeamCrateController::disable() {
#ifdef USEDCS
  if (mEmuDcs) DcsDisable(); // DCS contrib
#endif
  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    disable(myCrates[i]);
  }
#ifdef USEDCS
  if (mEmuDcs) DcsEnable(); // DCS contrib
#endif
}


void TestBeamCrateController::disable(Crate * crate) {
  
}
 

void TestBeamCrateController::executeCommand(std::string boardType, std::string command) {
  #ifdef debugV
    std::cout << "inside TestBeamCrateController::executeCommand()" << std::endl;
  #endif  

#ifdef USEDCS
    if (mEmuDcs) DcsDisable(); // DCS contrib
#endif

    // to do introduce open/closeSocket calls ... 
    // (or remove open/close alltogether)

    if (boardType=="DDU") {
      std::vector<DDU*> DDUs(theSelector.ddus());
      for(unsigned i = 0; i < DDUs.size(); ++i) {
	DDUs[i]->executeCommand(command);
      }
    }
    
  
    else if (boardType=="DCC") {
      std::vector<DCC*> DCCs = theSelector.dccs();
      for(unsigned i = 0; i < DCCs.size(); ++i) {
	DCCs[i]->executeCommand(command);
      }
    }
 
    else {
      throw("Bad Board type!");
    }

#ifdef USEDCS
    if (mEmuDcs) DcsEnable(); // DCS contrib
#endif
}


#ifdef USEDCS
void TestBeamCrateController::DcsSetup(){

 char* envvar;
 static char envbuf[256];
 envvar = getenv("DIM_DNS_NODE");
 if(!envvar){
  sprintf(envbuf,"DIM_DNS_NODE=%s","pcmsucr2.cern.ch");
  if(putenv(envbuf)){
   printf("error occured\n");
  }  
 }

  std::cout << "TestBeamController: DCS Setup" << std::endl;
  if (monitorService){
    delete monitorService;
    monitorService=0;
  }
  monitorService = new InfoMonitorService("DCS_SERVICE");
}


void TestBeamCrateController::DcsEnable(){
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


void TestBeamCrateController::DcsDisable(){
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
