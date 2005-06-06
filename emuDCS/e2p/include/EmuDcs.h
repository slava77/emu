#ifndef EmuDcs_h
#define EmuDcs_h

#include "PeripheralCrateController.h"

//fg #include "VMEControllerdcs.h"
#include "VMEController.h"
#include "DAQMB.h" // 03/31/2005
#include "CCB.h"   // 03/31/2005
///#include "FakeCCB.h"
#include "TMB.h"
#include "MPC.h"
#include "CrateSetup.h"
#include "ALCTController.h"

#include <DcsDimStructures.h>
#include <DcsDimService.h>
#include <DcsDimCommand.h>
#include "xdaqExecutive.h"
using namespace std;

class DcsDimCommand;

class EmuDcs : public PeripheralCrateController, public Task{

public:
  
EmuDcs(string *file_to_load_cfeb, string *file_to_load_vme_chip, string *file_to_load_control_chip,
       string *file_to_load_valct288, string *file_to_load_salct288, 
       string *file_to_load_valct384, string *file_to_load_salct384, 
       string *file_to_load_valct672, string *file_to_load_salct672, 
       string *file_to_load_tmb);

EmuDcs();

void EmuDcs_launch();

 void dcs_enable();
 void dcs_disable();
  void stop();
  void start();
  void soft_stop();
  void soft_start();

  int safeExit();

  int cfeb_lv(int cfeb_number);
  int alct_lv();
  int lv_on_wrap(bool IS_SIMULATION_LOCAL);
  int lv_off_wrap(bool IS_SIMULATION_LOCAL);
  int lowv_status(bool IS_SIMULATION_LOCAL);

  int resetAllBackplaneViaCCB();
  int programAllBackplaneViaCCB();
  int programAll2004();
  int resetAllBackplaneViaDMB();
  int readAllTemperatures();
  int lvSet(); 
  int programAllCFEBs();
  int loadAllCFEBs();
  int loadOneCFEB(int number);
 
  int programALCT();
  int programDAQMB();
  int programTMB();
  int programMPC();

  int loadALCT(string &command);
  int loadTMB();
 
  int programDAQMB_VMEChip();
  int loadDAQMB_VMEChip();
  int loadDAQMB_VMEChipEmergency();

  int programDAQMB_ControlChip();
  int loadDAQMB_ControlChip();

  int resetDAQMB();
  int resetDAQMB_FIFO();
  int resetTMB();
  int resetMPC();
  int resetALCT();
  int reset_int_CCB();

  int ccb_L1_Reset();

  int readLV_Reference();


  ///  int commandParse(string &command);
  ///  bool slotsLoading(/*string &ip_address, int ccb_slot,int dmb_slot,int tmb_slot,int mpc_slot*/);
  

  int controlDAQMB(DAQMB *daqmb);
  int controlCCB(CCB *ccb);
  int controlTMB(TMB *tmb);


  int simulationLVStatusControl(string &ipslot);
  static void catchFunction(int);

// ===test stuff below 

  int test();

//================================================

  int number_of_cfebs; // temporal parameter: should be taken from DAQMB object 
                       // whenever it is declared there

  static int RUN_MODE;
  RunControlStructure RunControlAck;
private:

 // ===== xdaq parameters: =========
  int lv_mask_to_set;
  string *file_to_load_cfeb;
  string *file_to_load_vme_chip;
  string *file_to_load_control_chip;
  string *file_to_load_valct288; 
  string *file_to_load_valct384; 
  string *file_to_load_valct672; 
  string *file_to_load_salct288;
  string *file_to_load_salct384;
  string *file_to_load_salct672; 
  string *file_to_load_tmb;

 //================================

  DcsDimCommand *DcsDimCommand_o;
   
  LV_1_DimBroker LV_1_DimBroker_lv;
  TEMPERATURE_1_DimBroker TEMPERATURE_1_DimBroker_tm;
  COMMAND_1_DimBroker COMMAND_1_DimBroker_cm;
  REFERENCE_1_DimBroker REFERENCE_1_DimBroker_rf;
 

  DcsDimService *LV_1_MonitorService;
  DcsDimService *TEMPERATURE_1_MonitorService;
  DcsDimService *COMMAND_1_MonitorService;
  DcsDimService *REFERENCE_1_MonitorService;
  DimService *RunControlService;


  DAQMB *daqmb;
  CCB *ccb;
  TMB *tmb;
  ALCTController *alct_c;
  //  VMEController *vme; //  to avoid the call like: daqmb->theController->theCurrentModule
  

  int RepeatNumber; // temporary data 
  char cbrdnum[200]; // 5 // some imported daqmb functions return this parameter
  bool OPERATION_ACTIVE; // it is usefull for synchronization of parallel operations 

//== for simulation: to keep power staus ==========

vector<string> d360_ipslot;
vector<int> slot_status;
int current_set;


//================================================

// === implemented virtial functions ==================
int svc();

};


#endif
