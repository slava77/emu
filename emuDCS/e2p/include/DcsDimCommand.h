#ifndef DcsDimCommand_h
#define DcsDimCommand_h

#include <dis.hxx> // for server
#include <dic.hxx> // for client
#include <string>
#include <vector>

#include <EmuDcs.h>
#include <DcsCommon.h>
// XDAQ includes
#include "xdaq.h"
#include "SyncQueue.h"
#include "xdaqExecutive.h"

class DcsDimService;
class EmuDcs;

class DcsDimCommand: public DimCommand, public Task{//, public ErrorHandler{

 string ipslot;
 string ip_address;
 int ccb_slot; 
 int dmb_slot; 
 int tmb_slot; 
 int mpc_slot;
 string operation_command;

 EmuDcs *EmuDcs_o;

   SyncQueue<string> sQueue;

public:



  DcsDimService *LV_1_DimBroker_o;
  
  DcsDimService *TEMPERATURE_1_DimBroker_o;
  DcsDimService *COMMAND_1_DimBroker_o;
  DcsDimService *REFERENCE_1_DimBroker_o;
  DimService *RunControlService_o; 


  bool slotsLoading();
  bool nextSlotsLoading(int *i,int *j);
  int commandParse(string &command);
  void commandHandler();
  int getDataLV();


DcsDimCommand(DcsDimService *LV_1_DimBroker_o, DcsDimService *TEMPERATURE_1_DimBroker_o,  
              DcsDimService *COMMAND_1_DimBroker_o,DcsDimService *REFERENCE_1_DimBroker_o, DimService *RunControlService_o,
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
