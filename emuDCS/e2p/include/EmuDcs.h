#ifndef EmuDcs_h
#define EmuDcs_h

//////#include "PeripheralCrateController.h"
///  emulib4 #include "CrateSelector.h"
//fg #include "VMEControllerdcs.h"
#include "VMEController.h"
#include "DAQMB.h" // 03/31/2005
#include "CCB.h"   // 03/31/2005
///#include "FakeCCB.h"
#include "TMB.h"
#include "MPC.h"
///  emulib4 #include "CrateSetup.h"
#include "ALCTController.h"

#include <DcsDimStructures.h>
#include <DcsDimService.h>
#include <DcsDimCommand.h>
//#include "xdaqExecutive.h"
#include "toolbox/Task.h"
#include "DcsEmuController.h"


#define MAX_CHAMBER_NUMBER 108


class DcsDimCommand;

class EmuDcs : public DcsEmuController, public Task{ //emulib4


public:

  //CrateSelector *theSelectorDcs; //emulib4

int ch_counters[MAX_CHAMBER_NUMBER];
int ch_all_counter;
 int READOUT_CYCLE_DELAY;
 int READOUT_COUNTER_NUMBER;
  
EmuDcs(std::string *file_to_load_cfeb, std::string *file_to_load_vme_chip, std::string *file_to_load_control_chip,
       std::string *file_to_load_valct288, std::string *file_to_load_salct288, 
       std::string *file_to_load_valct384, std::string *file_to_load_salct384, 
       std::string *file_to_load_valct672, std::string *file_to_load_salct672, 
       std::string *file_to_load_tmb);

EmuDcs(DcsEmuController *theEmuController); // emulib4 EmuDcs(CrateSelector &theSelectorDcs); 


 int isError();

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
  int lv_on_wrap(bool IS_SIMULATION_LOCAL, int channels);
  int lv_off_wrap(bool IS_SIMULATION_LOCAL);
  int lowv_status(bool IS_SIMULATION_LOCAL);

  int resetAllBackplaneViaCCB();
  int programAllBackplaneViaCCB();
  int programAll2004();
  int hardReset();

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

  int loadALCT(std::string &command);
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
  
  int db();
  int getServiceName(int index, char *system, std::string &service_name);  


  ///  int commandParse(std::string &command);
  ///  bool slotsLoading(/*std::string &ip_address, int ccb_slot,int dmb_slot,int tmb_slot,int mpc_slot*/);
  

  int controlDAQMB(emu::pc::DAQMB *daqmb);
  int controlCCB(emu::pc::CCB *ccb);
  int controlTMB(emu::pc::TMB *tmb);


  int simulationLVStatusControl(std::string &ipslot);
  static void catchFunction(int);

// ===test stuff below 

  int test();

//================================================

  std::vector<std::string> slots;
  std::vector<std::string> chamber_slots;
  int db_index;

  int number_of_cfebs; // temporal parameter: should be taken from DAQMB object 
                       // whenever it is declared there

  static int RUN_MODE;
  RunControlStructure RunControlAck;
private:

 // ===== xdaq parameters: =========
  int lv_mask_to_set;

  bool OPERATION_ACTIVE; // it is usefull for synchronization of parallel operations 

 int RepeatNumber; // temporary data 

  std::string *file_to_load_cfeb;
  std::string *file_to_load_vme_chip;
  std::string *file_to_load_control_chip;

 std::string *file_to_load_valct288; 
 std::string *file_to_load_salct288; 

  std::string *file_to_load_valct384; 
 std::string *file_to_load_salct384;


  std::string *file_to_load_valct672;  
  std::string *file_to_load_salct672; 

  std::string *file_to_load_tmb;

 //================================

  DcsDimCommand *DcsDimCommand_o;
   
  LV_1_DimBroker LV_1_DimBroker_lv[MAX_CHAMBER_NUMBER];
  TEMPERATURE_1_DimBroker TEMPERATURE_1_DimBroker_tm[MAX_CHAMBER_NUMBER];
  COMMAND_1_DimBroker COMMAND_1_DimBroker_cm[MAX_CHAMBER_NUMBER];
  REFERENCE_1_DimBroker REFERENCE_1_DimBroker_rf[MAX_CHAMBER_NUMBER];
 

  DcsDimService *LV_1_MonitorService[MAX_CHAMBER_NUMBER];
  DcsDimService *TEMPERATURE_1_MonitorService[MAX_CHAMBER_NUMBER];
  DcsDimService *COMMAND_1_MonitorService[MAX_CHAMBER_NUMBER];
  DcsDimService *REFERENCE_1_MonitorService[MAX_CHAMBER_NUMBER];
  DimService *RunControlService;


  emu::pc::DAQMB *daqmb;
  emu::pc::CCB *ccb;
  emu::pc::TMB *tmb;
  emu::pc::ALCTController *alct_c;
  //  VMEController *vme; //  to avoid the call like: daqmb->theController->theCurrentModule
  

 
  char cbrdnum[200]; // 5 // some imported daqmb functions return this parameter


//== for simulation: to keep power staus ==========

std::vector<std::string> d360_ipslot;
std::vector<int> slot_status;
int current_set;


//================================================

// === implemented virtial functions ==================
int svc();

};


#endif
