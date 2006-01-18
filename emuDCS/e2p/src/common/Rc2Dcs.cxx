
#include "Rc2Dcs.h"

void Rc2DcsMonitorService::infoHandler(){
  static bool FIRST=true;

  if(FIRST){FIRST=false; return;} //// !!!!!!!!!!!

    if(name=="RC2DCS"){

      getdata=(char *)getData();
      //      printf("string=%s\n",getdata);
      ACK_RECEIVED = true;

      // std::cout << "InfoMonitorService: RC2DCS_Service received" << std::endl;
    }
}

//-----------

Rc2DcsMonitorService::Rc2DcsMonitorService(char *name) : DimInfo(name, -1.0){
   this->name=std::string(name);
   ACK_RECEIVED=false;
   //           printf("string=\n");
}

//-----------

Rc2Dcs::Rc2Dcs(){
   monitorService = new Rc2DcsMonitorService("RC2DCS");
   TIME_TO_WAIT_AFTER_GETDATA=5; // in seconds
   strcpy(dcscontrol_txt,"/home/dcs/bin/dcscontrol_web.txt");
   fp_web=fopen(dcscontrol_txt, "w+");


}

//-----------
int Rc2Dcs::sendCommand(char *com, char *subcommand){

  monitorService->ACK_RECEIVED=false;
  DimClient::sendCommand(com,subcommand);

}
//-----------
int Rc2Dcs::turnHV(int isOn, int voltage, char *chamber){//=0){
    int ret;
    char subcommand[40];

       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");

    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------

    sprintf(subcommand,"HV;%d;%d;%s",isOn,voltage,chamber_sm.c_str());

    sendCommand("RC2DCS_COMMAND",subcommand);
    ret=waitForAcknowledge(300,0);
    //    errorProcessing(ret);
    return 1;
}
//----------- 
int Rc2Dcs::turnLV(int isOn, char *chamber){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------
    sprintf(subcommand,"LV;%d;-1;%s",isOn,chamber_sm.c_str());

    sendCommand("RC2DCS_COMMAND",subcommand);
    ret=waitForAcknowledge(200,0);
    //    errorProcessing(ret);
    return 1;
}

//----------- 
int Rc2Dcs::turnMRTN(int isOn, char *maraton){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    sprintf(subcommand,"MRTN;%d;%s",isOn, maraton);

    sendCommand("RC2DCS_COMMAND",subcommand);
    ret=waitForAcknowledge(200,0);
    //    errorProcessing(ret);
    return 1;
}
//----------- 
int Rc2Dcs::turnCRB(int isOn, char *crb){
    int ret;
    char subcommand[40];
       fclose(fp_web);
      fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string crb_sm(crb);
    pos=crb_sm.find("s");
    if(pos != string::npos)crb_sm.replace(pos,1,"d");
    //------------
    sprintf(subcommand,"CRB;%d;%s",isOn, crb_sm.c_str());

    sendCommand("RC2DCS_COMMAND",subcommand);
    ret=waitForAcknowledge(200,0);
    //    errorProcessing(ret);
    return 1;
}
//========================================================
//========================================================
//========================================================
//----------- 
int Rc2Dcs::controlCHIP(int option, char *chamber){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------
    sprintf(subcommand,"CHIP;%d;%s",option,chamber_sm.c_str());

    sendCommand("RC2DCS_COMMAND",subcommand);
    ret=waitForAcknowledge(200,0);
    //    errorProcessing(ret);
    return 1;
}
//========================================================
//========================================================
//========================================================
//----------- 
//-----------
int Rc2Dcs::readHV(char *chamber){//=0){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------
    sprintf(subcommand,"HVPREREAD;%s",chamber_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"HVREAD;%s",chamber_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();
        
    return 1;
}
//----------- 
int Rc2Dcs::readLV(char *chamber){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------

    sprintf(subcommand,"LVPREREAD;%s",chamber_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"LVREAD;%s",chamber_sm.c_str());
    // monitorService->ACK_RECEIVED1=false;
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    //  printf("m=%s\n",monitorService->getdata);

    dataProcessing();

    return 1;
}

//----------- 
int Rc2Dcs::readTEMP(char *chamber){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string chamber_sm(chamber);
    pos=chamber_sm.find("s");
    if(pos != string::npos)chamber_sm.replace(pos,1,"d");
    //------------
    sprintf(subcommand,"TEMPPREREAD;%s",chamber_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"TEMPREAD;%s",chamber_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 
int Rc2Dcs::readMRTN(char *maraton){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");

    sprintf(subcommand,"MRTNPREREAD;%s",maraton);
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"MRTNREAD;%s",maraton);
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 
int Rc2Dcs::readCRB(char *crb){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");
    //------------
    int pos;
    string crb_sm(crb);
    pos=crb_sm.find("s");
    if(pos != string::npos)crb_sm.replace(pos,1,"d");
    //------------

    sprintf(subcommand,"CRBPREREAD;%s",crb_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"CRBREAD;%s",crb_sm.c_str());
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 
int Rc2Dcs::readWTH(){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");

    sprintf(subcommand,"WTHPREREAD");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"WTHREAD");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 
int Rc2Dcs::readPT100(){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");

    sprintf(subcommand,"PT100PREREAD");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"PT100READ");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 
int Rc2Dcs::readGAS(){
    int ret;
    char subcommand[40];
       fclose(fp_web);
       fp_web=fopen(dcscontrol_txt, "w+");

    sprintf(subcommand,"GASPREREAD");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    sleepProcesing(TIME_TO_WAIT_AFTER_GETDATA);

    sprintf(subcommand,"GASREAD");
    sendCommand("RC2DCS_COMMAND",subcommand);
    if((ret=waitForAcknowledge())<=0)return 0;

    dataProcessing();

    return 1;
}
//----------- 


//-----------
int Rc2Dcs::waitForAcknowledge(int counter_max, int isSilent){


  int counter(0);
  int ret;

  while(true){
  printf("."); fflush(stdout);
    usleep(100000);
    counter++;
      if(monitorService->ACK_RECEIVED) {monitorService->ACK_RECEIVED=false;break;}  
   if(!(counter%50))printf("\n");

    if(counter==counter_max) {
      //   printf("\n");
       std::cerr << "... timing out" << std::endl;
      ret = 0;
      break;
    }
  } // while

     printf("\n");

  if(ret == 0){}
  else if(!strcmp(monitorService->getdata,"ERROR")){
      std::cerr << "ERROR: wrong status" << std::endl;   
      ret= -1;
  }
  else if(!strcmp(monitorService->getdata,"BLOCKED")){
    //// std::cerr << "ERROR: wrong status" << std::endl;   
      ret= -2;
  }
  else if(!strcmp(monitorService->getdata,"NOSUCHDEVICES")){
      ret= -3;
  }
  //------------------------------------------------------------
  else if(!strcmp(monitorService->getdata,"SOMEDEVICEERROR")){
      ret= 2;
  }
  //------------------------------------------------------------
  else ret=1;

  errorProcessing(ret, isSilent);  

    return ret;
}

//==============================================================================================

int Rc2Dcs::errorProcessing(int ret, int isSilent){

  if(ret==0){
    printf("There is no communication to DCS for a few possible reasons\n");
    printf("1) PVSS part of DCS is not running at the moment (maintenance/development)\n");
    printf("2) there is no network connection to PVSS machine\n");

    fprintf(fp_web,"There is no communication to DCS for a few possible reasons\n");
    fprintf(fp_web,"1) PVSS part of DCS is not running at the moment (maintenance/development)\n");
    fprintf(fp_web,"2) there is no network connection to PVSS machine\n");
  }
  else if(ret==-1){
    printf("There is a problem to get the device for a few possible reasons\n");
    printf("1) The device exists but it is not in service\n");
    printf("2) server is not running on a control machine\n");

    fprintf(fp_web,"There is a problem to get the device for a few possible reasons\n");
    fprintf(fp_web,"1) The device exists but it is not in service\n");
    fprintf(fp_web,"2) server is not running on a control machine\n");

  }
  else if(ret==-2){
    printf("A RUN CONTROL ACCESS TO DCS IS TEMPORARILY BLOCKED DUE TO A CRITICAL MAINTENANCE/DEVELOPMENT\n");
    fprintf(fp_web,"A RUN CONTROL ACCESS TO DCS IS TEMPORARILY BLOCKED DUE TO A CRITICAL MAINTENANCE/DEVELOPMENT\n");
  }
  else if(ret==-3){
    printf("ERROR: There are no such device(s)\n");
    fprintf(fp_web,"ERROR: There are no such device(s)\n");
  }


  else if(ret==2){
    printf("The operation is done partly: not all the devices are connected to electronics\n");
    printf("Use a read command to check status of interesting devices\n");

    fprintf(fp_web,"The operation is done partly: not all the devices are connected to electronics\n");
    fprintf(fp_web,"Use a read command to check status of interesting devices\n");
  }

  else{
    if(!isSilent){
      printf("The operation is OK\n");
      fprintf(fp_web,"The operation is OK\n");
    }   
  }

       fflush(fp_web);

  return 1;
 }

//==============================================================================================

int Rc2Dcs::dataProcessing(){

    string data;
    //--------
    data=string(monitorService->getdata);
    int pos=-1;
    int pos_prev=-1;
    string datum;
    data_vector.clear();
    while(1){
      pos_prev=pos;
      pos=data.find(";",pos_prev+1);
      if(pos==string::npos)break;
      datum=data.substr(pos_prev+1,pos-pos_prev-1);
      data_vector.push_back(datum);

    } // while(1)
    //--------

    for(int i=0;i<data_vector.size();i++){
      /*
      if(i==0)printf("service=%s",data_vector[i].c_str());
      else if(i==1)printf(" item=%s\n",data_vector[i].c_str());
      else if(i==2)printf("data: %s",data_vector[i].c_str());
      */
      if(i!=data_vector.size()-1){
        printf(" %s",data_vector[i].c_str());
        fprintf(fp_web," %s",data_vector[i].c_str());
      }
      else {
        printf(" %s\n",data_vector[i].c_str());
        fprintf(fp_web," %s\n",data_vector[i].c_str());
      }

    } // for

       fflush(fp_web);
    return 1;

}


int Rc2Dcs::sleepProcesing(int time_to_wait_in_sec){

  int step=100000;

  for(int i=0;i<time_to_wait_in_sec*1000000/step;i++){
    printf("."); fflush(stdout);
    ::usleep(step);
    if(i!=0 && !(i%50))printf("\n");
  }
     printf("\n");   

  return 1;

}




int Rc2Dcs::help(){


      cout << endl;
      cout << "==============================================" << endl;
      cout << "usage:" << endl;
      cout << "==============================================" << endl;
      cout << "dcscontrol -s slow_control [-d detector] [-t isOn] [-v value|option] " << endl;
      cout << "==============================================" << endl;
      cout << "available 'slow_control' parameters:" << endl;
      cout << "'hv'    (High Voltage)"  << endl;
      cout << "'lv'    (Low Voltage of on-chamber boards)"  << endl;
      cout << "'temp'  (Temperature of on-chamber boards (read-only)) "  << endl;
      cout << "'mrtn'  (Low Voltage of Wiener Maraton Power Supplies)"  << endl;
      cout << "'crb'   (Low Voltage of Peripheral Crate boards)"  << endl;
      cout << "'wth'   (Weather station parameters at SX5 (read-only))"  << endl;
      cout << "'gas'   (Monitoring of Gas components at SX5 (read-only))"  << endl;
      cout << "'pt100' (Monitoring of Gas flows via Chambers(read-only))"  << endl;
      cout << "'chip'  (chip resets for on-csc and Peripheral Crate boards)"  << endl;
      cout << "==============================================" << endl;
      cout << "examples of 'detector' parameters:"  << endl;
      cout << "no parameter  (all relevant detectors for the slow control:"  << endl;
      cout << "               it is applicable for any slow_control "  << endl;
      cout << "'sp3c02'      (a chamber example: StationPlus3Chamber02:"  << endl;
      cout << "               it is applicable for 'hv', 'lv', 'temp' "  << endl;
      cout << "'sp3p1'       (a Peripheral Crate example: StationPlus3Peripheralcrate1:"  << endl;
      cout << "               it is applicable for 'crb' "  << endl;
      cout << "'maraton02'     (a maraton example: Wiener Maraton PS # 02:"  << endl;
      cout << "               it is applicable for 'mrtn' "  << endl;
      cout << "==============================================" << endl;
      cout << "available 'option' parameters:"  << endl;
      cout << "'hr_dmb','hr_alct','hr_cfebs','hr_tmb','hr_all' (hard reset of dmb, alct, etc)"  << endl;
      cout << "               it is applicable for 'chip'"  << endl;
      cout << "==============================================" << endl;
      cout << "examples:" << endl;
      cout << "dcscontrol -s gas                    -> read GAS at SX5" << endl;
      cout << "dcscontrol -s wth                    -> read Weather station parameters at SX5" << endl;
      cout << "dcscontrol -s crb -d sp2p2           -> read LV at StationPlus2PeripheralCrate2" << endl;
      cout << "dcscontrol -s crb -d sp2p2 -t 1      -> turn on LV at StationPlus2PeripheralCrate2" << endl;
      cout << "dcscontrol -s hv -d all -t 1 -v 3600 -> turn on HV at all chambers of slice test and set V=3600" << endl;
      cout << "dcscontrol -s hv -d all -t 0         -> turn off HV at all chambers of slice test" << endl;
      cout << "dcscontrol -s hv -d sp3c01 -t 1      -> turn on HV at StationPlus3Chamber01" << endl;
      cout << "dcscontrol -s hv -d sp3c01           -> read HV data at StationPlus3Chamber01" << endl;
      cout << "" << endl;
      exit(0);

}
//====================================
