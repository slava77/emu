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

};
//===========================
class Rc2Dcs{

public:
  Rc2Dcs();
//-----------
  int turnHV(int isOn, int voltage=0);
//----------- 
  int turnLV(int isOn);
//-----------
  int waitForAcknowledge();

   Rc2DcsMonitorService *monitorService;

};

//============================


#endif
