#ifndef Rc2Dcs_h
#define Rc2Dcs_h

#include "EmuDcs.h"

//===========================

class Rc2DcsMonitorService : public DimInfo{
  std::string name;
  void infoHandler();
public:
  bool ACK_RECEIVED;
  Rc2DcsMonitorService(char *name);
  char *getdata;

};
//===========================
class Rc2Dcs{

public:

  char *slow_control;
  char *detector;
  int  isOn_detector;
  float value_to_set;
  int option_to_set;

int TIME_TO_WAIT_AFTER_GETDATA;
 std::vector<std::string> data_vector;

//-----------
  Rc2Dcs();
//-----------
  int turnHV(int isOn, int voltage=0, char *chamber="all");
//----------- 
  int turnLV(int isOn, char *chamber="all");
//-----------
 int turnMRTN(int isOn, char *maraton);
//----------- 
 int turnCRB(int isOn, char *crb);
//-----------
//-----------
 int readHV(char *chamber);
//----------- 
 int readLV(char *chamber);
 int readTEMP(char *chamber);
//----------- 
 int readMRTN(char *maraton);
//----------- 
 int readCRB(char *crb);
 //----------
int readWTH();
int readPT100();
int readGAS();
//----------
 int controlCHIP();
 int controlCHIP(int option, char *chamber);

 int help();

  int waitForAcknowledge(int counter_max=200, int isSilent=1);

  int dataProcessing();
  int sleepProcesing(int time_to_wait);
  int errorProcessing(int ret, int isSilent=1);

  int sendCommand(char *com, char *subcommand);
   Rc2DcsMonitorService *monitorService;

};

//============================


#endif
