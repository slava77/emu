#ifndef DcsDimCommand_h
#define DcsDimCommand_h



#include "dim/dis.hxx" // for server
#include "dim/dic.hxx" // for client
#include <string>
#include <vector>
#include "toolbox/SyncQueue.h"
#include "toolbox/Task.h"
using namespace toolbox;

#include <EmuDcs.h>
#include <DcsCommon.h>




#define MAX_CHAMBER_NUMBER 108

class DcsDimService;
class EmuDcs;

class DcsDimCommand: public DimCommand, public Task{//, public ErrorHandler{

 std::string ipslot;
 std::string ip_address;
 int ccb_slot; 
 int dmb_slot; 
 int tmb_slot; 
 int mpc_slot;

 int lvmb_channels;

 std::string operation_command;

 EmuDcs *EmuDcs_o;

   SyncQueue<std::string> sQueue;

   int db_index;

public:

  int FIRST_UPDATE[MAX_CHAMBER_NUMBER];
  int previous_error_status[MAX_CHAMBER_NUMBER];

  DcsDimService **LV_1_DimBroker_o;
  
  DcsDimService **TEMPERATURE_1_DimBroker_o;
  DcsDimService **COMMAND_1_DimBroker_o;
  DcsDimService **REFERENCE_1_DimBroker_o;
  DimService *RunControlService_o; 


  bool slotsLoading();
  bool nextSlotsLoading(int *i,int *j);
  int commandParse(std::string &command);
  void commandHandler();
  int getDataLV(bool isUpdate=false, bool isDRAFT=0);
  int isError();
  int get_db_index(std::string ip_address, int dmb_slot);
  int get_db_index_by_chamber(std::string chamber);
  std::string get_slot_by_db_index(int db_index_1);
DcsDimCommand(DcsDimService **LV_1_DimBroker_o, DcsDimService **TEMPERATURE_1_DimBroker_o,  
              DcsDimService **COMMAND_1_DimBroker_o,DcsDimService **REFERENCE_1_DimBroker_o, DimService *RunControlService_o,
	      EmuDcs *EmuDcs_o);

// === implemented virtial functions ==================
int svc();
////void warning(const SAXParseException& toCatch);
////void error(const SAXParseException& toCatch);
////void fatalError(const SAXParseException& toCatch);
////void resetErrors();

};

//========================================================================


#endif
