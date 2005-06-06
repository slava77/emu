
#include "Rc2Dcs.h"

void Rc2DcsMonitorService::infoHandler(){
    if(name=="RC2DCS"){
      ACK_RECEIVED = true;
      // std::cout << "InfoMonitorService: RC2DCS_Service received" << std::endl;
    }
}

Rc2DcsMonitorService::Rc2DcsMonitorService(char *name) : DimInfo(name, -1.0){
    this->name=std::string(name);
}

//-----------

Rc2Dcs::Rc2Dcs(){
   monitorService = new Rc2DcsMonitorService("RC2DCS");
}


//-----------
int Rc2Dcs::turnHV(int isOn, int voltage){//=0){
    int ret;
    char subcommand[40];
    sprintf(subcommand,"HV;%d;%d",isOn,voltage);


    DimClient::sendCommand("RC2DCS_COMMAND",subcommand);
    if(!(ret=waitForAcknowledge()))return 0;
    return 1;
}
//----------- 
int Rc2Dcs::turnLV(int isOn){
    int ret;
    char subcommand[40];
    sprintf(subcommand,"LV;%d;-1",isOn);

    DimClient::sendCommand("RC2DCS_COMMAND",subcommand);
    if(!(ret=waitForAcknowledge()))return 0;
    return 1;
}
//-----------
int Rc2Dcs::waitForAcknowledge(){
  int counter(0);
  while(true){
    usleep(50000);
    counter++;
    if(monitorService->ACK_RECEIVED) break;
    if(counter==20) {
      std::cerr << "TestBeamController: WARNING - DCS does not reply ... timing out" << std::endl;
      return 0;
    }
  } 

    return 1;
}


//====================================
