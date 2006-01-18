

#include <DcsDimCommand.h>


extern int CHAMBER_NUMBER;

//===== xdaq stuff (implementing of the virtual functions) ==================================


//=======
///void DcsDimCommand::warning(const SAXParseException& toCatch){};
///void DcsDimCommand::error(const SAXParseException& toCatch){};
///void DcsDimCommand::fatalError(const SAXParseException& toCatch){};
///void DcsDimCommand::resetErrors(){};


bool PUBLISHING_ENABLED=false;

//===========================================================================================

extern bool SIMULATION;

// NORMAL MODE:                      CONFIRMATION_NEEDED = true;
// ALL BOARDS PROGRAMMING MODE:      CONFIRMATION_NEEDED = false

bool CONFIRMATION_NEEDED = false;  
                                    
bool SUSPEND_UPDATE_SERVICE = false;



//=====================================================================
bool DcsDimCommand::nextSlotsLoading(int *i,int *j){

  vector<Crate *> v_crates= EmuDcs_o->crates();
  vector<DAQMB *> v_daqmbs;
  vector<TMB *>   v_tmbs;

#ifdef DCS_PRINTING_0
printf("21 %d %d\n",*i,v_crates.size());
#endif

  if(*i >= v_crates.size()) return false;
  //  printf("21 %d\n",*i);
  ip_address = v_crates[*i]->vmeController()->ipAddress();
  // printf("22\n");
  SIMULATION=false;
  if(ip_address == "137.138.176.246")SIMULATION=true;
  //  if(ip_address == "137.138.102.223")SIMULATION=true;

 

  v_daqmbs=EmuDcs_o->daqmbs(v_crates[*i]);
  v_tmbs=EmuDcs_o->tmbs(v_crates[*i]);

  EmuDcs_o->controlCCB(v_crates[*i]->ccb()); // October 12, 2003

  if(*j >= v_daqmbs.size()) {
       (*j)=-1;
       if(v_daqmbs.size() == 0)printf("the crate without daqmbs, i=%d j=%d d_size=%d c_size=%d\n",*i,*j,v_daqmbs.size(),v_crates.size());
       return true;
  }


  EmuDcs_o->controlDAQMB(v_daqmbs[*j]);
  dmb_slot = v_daqmbs[*j]->slot();


  EmuDcs_o->controlTMB(v_tmbs[*j]);
  tmb_slot = v_tmbs[*j]->slot();

  if(dmb_slot == 22 || dmb_slot == 23 )SIMULATION=true;

    // for simulation use:



  if(SIMULATION){
  char tmp[10];
  sprintf(tmp,"%d",dmb_slot);
  ipslot=ip_address+";"+string(tmp);
    EmuDcs_o->simulationLVStatusControl(ipslot);
  }


  return true;


}
//=====================================================================

bool DcsDimCommand::slotsLoading(){
  //(string &ip_address, int ccb_slot,int dmb_slot,int tmb_slot,int mpc_slot){



  vector<Crate *> v_crates= EmuDcs_o->crates();
  //  int size_crates=v_crates.size();
  vector<DAQMB *> v_daqmbs;
  vector<TMB *>   v_tmbs;

  bool crate_ok = false;
  //////  bool ccb_ok   = false;
  bool dmb_ok   = false;

  bool tmb_ok   = false; // currently not used
  ///////  bool mpc_ok   = false; // currently not used

  for(int i=0;i< v_crates.size(); i++){
     
    if(ip_address == v_crates[i]->vmeController()->ipAddress() ){
      crate_ok = true;
    }
    else {
      continue;
    }

   v_daqmbs=EmuDcs_o->daqmbs(v_crates[i]);
   v_tmbs=EmuDcs_o->tmbs(v_crates[i]);

   for(int j=0;j<v_daqmbs.size() ;j++){
     if(dmb_slot == v_daqmbs[j]->slot()){
       EmuDcs_o->controlDAQMB(v_daqmbs[j]);
       dmb_ok=true;

       EmuDcs_o->controlTMB(v_tmbs[j]); // j for dmb and tmb should be the same: not checked
       tmb_slot = v_tmbs[j]->slot();    // take the slot from here

 // for simulation use
  char tmp[10];
  sprintf(tmp,"%d",dmb_slot);
  ipslot=ip_address+";"+string(tmp);

     }
   } // j loop

   ////   if(ccb_slot == v_crates[i]->ccb()->slot()){
     EmuDcs_o->controlCCB(v_crates[i]->ccb());
   /////  ccb_ok = true; 
   ////   }

  } // i loop
   
  if(!crate_ok || !dmb_ok){

    cout << "this command is not for me" << endl;
    /*
     cout <<  "ERROR: EmuDcs::slotsAndCrateSelection \n" 
	  << "ip_address, ccb_slot, dmb_slot are not found: \n"
	  << " crate: "<< crate_ok << "\n dmb: "<< dmb_ok  << endl;
     abort();
    */
    return false;

  }
  else {

     SIMULATION=false;
     if(ip_address == "137.138.176.246")SIMULATION=true;
     //    if(ip_address == "137.138.102.223")SIMULATION=true;
     if(dmb_slot == 22 || dmb_slot == 23 )SIMULATION=true;

    // for simulation use:

    if(SIMULATION)
    EmuDcs_o->simulationLVStatusControl(ipslot);

    /*    
    bool found=false;
    for(int i=0;i< EmuDcs_o->d360_ipslot.size();i++ ){
      if(EmuDcs_o->d360_ipslot[i] == ipslot){
         EmuDcs_o->current_set=i;
         found=true;  
      }        
    }
    
    if(!found){
     EmuDcs_o->d360_ipslot.push_back(ipslot);
     EmuDcs_o->slot_status.push_back(0);
     EmuDcs_o->current_set = EmuDcs_o->d360_ipslot.size()-1;
    }
    */

    return true;


  } // else
}



//===============================================================================================
int DcsDimCommand::commandParse(string &command){

  
     int pos=command.find("|",0);

     ipslot = command.substr(0,pos);

    int pos2= ipslot.find(";",0);
    int pos1= ipslot.find(";",pos2+1);
    if(pos1 == string::npos){
     dmb_slot=atoi((ipslot.substr(pos2+1, ipslot.size() - pos2 -1)).c_str());
     lvmb_channels=0x3f;
    }
    else{
     dmb_slot=atoi((ipslot.substr(pos2+1, pos1-pos2-1)).c_str());
     lvmb_channels=atoi((ipslot.substr(pos1+1, ipslot.size() - pos1 -1)).c_str());
    }
     ip_address=ipslot.substr(0, pos2);


    operation_command=command.substr(pos+1,command.size()-pos-1);

#ifdef DCS_PRINTING_0
  printf(">>>>>>>>>>>>>>>>>> %d lvmb_channels=%d %s %s\n",dmb_slot,lvmb_channels,ip_address.c_str(),operation_command.c_str());
#endif

     return 1;

}
//===============================================================================================
void DcsDimCommand::commandHandler(){

#ifdef DCS_PRINTING_0
    printf("server++\n");
#endif
    string sub_command=getString();


/* II
  if(EmuDcs::RUN_MODE==0 && sub_command != "DCS_ENABLE"){
    printf("the server is disabled -----> EmuDcs::RUN_MODE==0\n");
    return;

  }
*/

    //    printf("server: %s\n",sub_command.c_str());
    //    exit(0);


    //    static bool FIRST=true;
#ifdef DCS_PRINTING_0
    printf("server1\n");
    printf("server22: %s\n",sub_command.c_str());
#endif
    if(sub_command == "CONFIRMATION"){
        DcsDimService::CONFIRMATION=true;
        return;
    }
    // int pos;

    /// pos=sub_command.find("|",0);
    //-------------------------------------------------------------------------------------------------
    /// string ipslot = sub_command.substr(0,pos);
    ///bool found_slot=false;
#ifdef DCS_PRINTING_0
   printf("server23\n");
#endif

  sQueue.push(sub_command);
#ifdef DCS_PRINTING_0
   printf("server24\n");
#endif
}
//===============================================================================================
int DcsDimCommand::svc(){

  static bool FIRST_CHIP=true;

  string sub_command;

  while(1){ // indefinite loop (there are no breaks for it)

    sub_command = sQueue.pop();

//////// ======= II
    if(EmuDcs::RUN_MODE==0 && sub_command != "DCS_ENABLE")continue; // II
//////// ======= II
#ifdef DCS_PRINTING_0
    printf("sub_command = %s\n",sub_command.c_str());
#endif
    //---------------------------
    if(sub_command == "DCS_ENABLE"){
          EmuDcs_o->dcs_enable();
            strcpy(EmuDcs_o->RunControlAck.ACK,"OK_ENABLE");
            RunControlService_o->updateService();
      continue;
  
    }
    else if (sub_command == "DCS_DISABLE"){
            EmuDcs_o->dcs_disable();
            strcpy(EmuDcs_o->RunControlAck.ACK,"OK_DISABLE");
            RunControlService_o->updateService();
      continue;

    }
    //---------------------------
    commandParse(sub_command);
    int i=0,j=0;    

    bool isALL=false;
    if(ip_address == "all")isALL=true;



    ////    if(operation_command!="program_all_via_ccb"){printf("break;========= %s \n",operation_command.c_str());continue;}
    ////    else printf("program_all_via_ccb\n");

    while(true){
     if(!isALL)  {

       ///     if(ip_address == "137.138.102.223")SIMULATION=true;
       ////     else SIMULATION=false;

      if(!(slotsLoading()))break;  // return
     }
     else{

      if(!(nextSlotsLoading(&i,&j))){
#ifdef DCS_PRINTING_0
	printf("break !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
break;
      }
      if(j==-1){i++;j++;continue;}
      j++;

     }
#ifdef DCS_PRINTING_1
    printf("ip_address.c_str(),dmb_slot      %s %d i=%d j=%d sim=%d\n", ip_address.c_str(),dmb_slot,i,j,SIMULATION);  
#endif
  db_index=get_db_index(ip_address, dmb_slot);
  EmuDcs_o->db_index=db_index;

  //////  if(db_index == 2 || db_index == 3 || db_index == 5)continue;

  printf("oper=%s\n",operation_command.c_str());

  strcpy(((LV_1_DimBroker *)LV_1_DimBroker_o[db_index]->value)->dynatemTcpIp,ip_address.c_str());
  ((LV_1_DimBroker *)LV_1_DimBroker_o[db_index]->value)->setNumber=dmb_slot;

  strcpy(((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_DimBroker_o[db_index]->value)->dynatemTcpIp,ip_address.c_str());
  ((TEMPERATURE_1_DimBroker *)TEMPERATURE_1_DimBroker_o[db_index]->value)->setNumber=dmb_slot;

  strcpy(((COMMAND_1_DimBroker *)COMMAND_1_DimBroker_o[db_index]->value)->dynatemTcpIp,ip_address.c_str());
  ((COMMAND_1_DimBroker *)COMMAND_1_DimBroker_o[db_index]->value)->setNumber=dmb_slot;

  strcpy(((REFERENCE_1_DimBroker *)REFERENCE_1_DimBroker_o[db_index]->value)->dynatemTcpIp,ip_address.c_str());
  ((REFERENCE_1_DimBroker *)REFERENCE_1_DimBroker_o[db_index]->value)->setNumber=dmb_slot;


SUSPEND_UPDATE_SERVICE = false; 


// *************************************************************************************************************
    if(operation_command=="get_data_master"){

      ///     SUSPEND_UPDATE_SERVICE = false;  
#ifdef DCS_PRINTING_0 
      printf("get_data_master!!!!!========\n");
#endif
      getDataLV(true);

      EmuDcs_o->readAllTemperatures();
      TEMPERATURE_1_DimBroker_o[db_index]->DcsUpdateService();

    }
// *************************************************************************************************************
    if(operation_command=="get_data_local_update"){

#ifdef DCS_PRINTING_0
      printf("get_data command update      !!!!!========\n");
#endif

      PUBLISHING_ENABLED=true;
      // for(int i2=0;i2<100;i2++){
       getDataLV(true);
       //printf("++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> i2=%d\n",i2);
       //}
      EmuDcs_o->readAllTemperatures();
      TEMPERATURE_1_DimBroker_o[db_index]->DcsUpdateService();

      if(FIRST_CHIP){
       FIRST_CHIP=true;
       COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();     
      }
    }

// *************************************************************************************************************
    if(operation_command=="get_data_local"){

#ifdef DCS_PRINTING_0
      printf("get_data command local!!!!!========\n");
#endif

      PUBLISHING_ENABLED=true;
      // for(int i2=0;i2<100;i2++){
       getDataLV();
       //printf("++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> i2=%d\n",i2);
       //}
      EmuDcs_o->readAllTemperatures();
      ////////      TEMPERATURE_1_DimBroker_o[db_index]->DcsUpdateService();

    }
// *************************************************************************************************************
    if(operation_command=="get_data"){

#ifdef DCS_PRINTING_0
      printf("get_data command from pvss!!!!!========\n");
#endif

      PUBLISHING_ENABLED=true;


      getDataLV(true);
      
      EmuDcs_o->readAllTemperatures();
      TEMPERATURE_1_DimBroker_o[db_index]->DcsUpdateService();

      if(FIRST_CHIP){
       FIRST_CHIP=true;
       COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();     
      }


    }
// *************************************************************************************************************
    else if(operation_command=="power_on"){
      //  SUSPEND_UPDATE_SERVICE=true;

#ifdef OSUcc
      EmuDcs_o->ch_counters[db_index]=EmuDcs_o->READOUT_COUNTER_NUMBER-100;
#else
      EmuDcs_o->ch_counters[db_index]=EmuDcs_o->READOUT_COUNTER_NUMBER-2;
#endif


#ifdef DCS_PRINTING_0
           printf("lvmb_channels=%d\n", lvmb_channels);
#endif
      
     EmuDcs_o->lv_on_wrap(SIMULATION, lvmb_channels);
      


#ifdef DCS_PRINTING_0
      printf("on on \n");
#endif
      //////    CONFIRMATION=true;

     /*
      printf("on on \n");
      if(!SIMULATION)
      EmuDcs_o->lv_on_wrap();
      else {
           ((LV_1_DimBroker *)LV_1_DimBroker_o->value)->data.Alct_o.status=1; 
           EmuDcs_o->slot_status[EmuDcs_o->current_set]=1;
	   /// printf("file_to_load:::: %s\n",EmuDcs_o->file_to_load_cfeb->c_str());
      }
     */

      /////      get_data();

    }
// *************************************************************************************************************
    else if(operation_command=="power_off"){
      // SUSPEND_UPDATE_SERVICE=true;


#ifdef OSUcc
      EmuDcs_o->ch_counters[db_index]=EmuDcs_o->READOUT_COUNTER_NUMBER-100;
#else
      EmuDcs_o->ch_counters[db_index]=EmuDcs_o->READOUT_COUNTER_NUMBER-2;
#endif


     EmuDcs_o->lv_off_wrap(SIMULATION);
#ifdef DCS_PRINTING_0
      printf("off off \n");
#endif
      //////   CONFIRMATION=true;

     /*
      printf("off off \n");
      if(!SIMULATION)
      EmuDcs_o->lv_off_wrap();
      else {
           ((LV_1_DimBroker *)LV_1_DimBroker_o->value)->data.Alct_o.status=0; 
           EmuDcs_o->slot_status[EmuDcs_o->current_set]=0;
	   /// printf("file_to_load:::: %s\n",EmuDcs_o->file_to_load_cfeb->c_str());
      }
     */
      ///      get_data();

    }

// *************************************************************************************************************
// ************************* Temperatures and reference LV ****************************************************
// *************************************************************************************************************

   else if(operation_command=="read_temperatures"){
      if(!SIMULATION){
        EmuDcs_o->readAllTemperatures();
 
      }
      else{
        printf("read_temperatures SIMULATED\n");
      }
       TEMPERATURE_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
   else if(operation_command=="read_reference_lv"){
      if(!SIMULATION){
        EmuDcs_o->readLV_Reference();
      }
      else{
        printf("read_reference_lv SIMULATED\n");
      }
       REFERENCE_1_DimBroker_o[db_index]->DcsUpdateService(); 
   }
 

// *************************************************************************************************************
// *************************  resetting               **********************************************************
// *************************************************************************************************************
    else if(operation_command=="soft_reset_all"){ // all boards but only in the crate
      if(!SIMULATION){
       EmuDcs_o->resetAllBackplaneViaCCB();
 
      }
      else{
        printf("soft_reset_all SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService(); 
   }
// *************************************************************************************************************
    else if(operation_command=="soft_reset_dmb"){
      if(!SIMULATION){
       EmuDcs_o->resetDAQMB();
       COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
      }
      else{
        printf("soft_reset_dmb SIMULATED\n");
      }
    }

// *************************************************************************************************************
    else if(operation_command=="soft_reset_tmb"){
      if(!SIMULATION){
        EmuDcs_o->resetTMB();
      }
      else{
        printf("soft_reset_tmb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="soft_reset_mpc"){
      if(!SIMULATION){
        EmuDcs_o->resetMPC();
      }
      else{
        printf("soft_reset_mpc SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="int_logic_reset_ccb"){
      if(!SIMULATION){
       EmuDcs_o->reset_int_CCB();
      }
      else{
        printf("int_logic_reset_ccb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="reset_fifo_dmb"){
      if(!SIMULATION){
       EmuDcs_o->resetDAQMB_FIFO();
      }
      else{
        printf(" SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="l1_reset_ccb"){
      if(!SIMULATION){
       EmuDcs_o->ccb_L1_Reset();
      }
      else{
        printf("l1_reset_ccb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="soft_reset_alct"){   // is not possible via CCB
      if(!SIMULATION){

      }
      else{
        printf("soft_reset_alct SIMULATED\n");
      }
    }

// *************************************************************************************************************
// *******************************       programming   *********************************************************
// *************************************************************************************************************

    else if(operation_command=="hard_reset_all"){  // version used in Crate configuration by daq people
      if(!SIMULATION){

	EmuDcs_o->hardReset(); // version used in Crate configuration by daq people
#ifdef DCS_PRINTING_0
	printf("HARD RESET (ALL BOARDS PROGRAMMING) DONE\n");
#endif
      }
      else{
#ifdef DCS_PRINTING_0
        printf("program_all_via_ccb SIMULATED\n");
#endif
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************

    else if(operation_command=="program_all_via_ccb"){
      if(!SIMULATION){

       EmuDcs_o->programAll2004();
#ifdef DCS_PRINTING_0
	printf("CRATE CONFIGURATION(ALL BOARDS PROGRAMMING) DONE\n");
#endif
      }
      else{
#ifdef DCS_PRINTING_0
        printf("program_all_via_ccb SIMULATED\n");
#endif
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************

    else if(operation_command=="program_all_via_dmb"){ // ??? is that programming or soft reset
      if(!SIMULATION){
       EmuDcs_o->resetAllBackplaneViaDMB();
      }
      else{
        printf("program_all_via_dmb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="program_dmb"){  // ?? all chips ?
      if(!SIMULATION){
       EmuDcs_o->programDAQMB();
      }
      else{
        printf("program_dmb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command=="program_tmb"){
      if(!SIMULATION){
       EmuDcs_o->programTMB();
      }
      else{
        printf("program_tmb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="program_mpc"){
      if(!SIMULATION){
       EmuDcs_o->programMPC();
      }
      else{
        printf("program_mpc SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command=="program_alct"){
      if(!SIMULATION){
       EmuDcs_o->programALCT();
      }
      else{
        printf("program_alct SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command=="program_all_cfebs"){
      if(!SIMULATION){
       EmuDcs_o->programAllCFEBs();
      }
      else{
        printf("program_all_cfebs SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


//*************************************************************************************************************
    else if(operation_command=="program_dmb_vme_chip"){
      if(!SIMULATION){
       EmuDcs_o->programDAQMB_VMEChip();
      }
      else{
        printf(" SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************

    else if(operation_command=="program_dmb_ctl_chip"){
      if(!SIMULATION){
       EmuDcs_o->programDAQMB_ControlChip();
      }
      else{
        printf("program_dmb_ctl_chip SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }



// *************************************************************************************************************
// ******************************      Loading               ***************************************************
// *************************************************************************************************************
    else if(operation_command=="load_all_cfebs"){
      if(!SIMULATION){
       EmuDcs_o->loadAllCFEBs();
      }
      else{
        printf("load_all_cfebs SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="load_cfeb1"){
      if(!SIMULATION){
       EmuDcs_o->loadOneCFEB(0);
      }
      else{
        printf("load_cfeb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="load_cfeb2"){
      if(!SIMULATION){
       EmuDcs_o->loadOneCFEB(1);
      }
      else{
        printf("load_cfeb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="load_cfeb3"){
      if(!SIMULATION){
       EmuDcs_o->loadOneCFEB(2);
      }
      else{
        printf("load_cfeb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="load_cfeb4"){
      if(!SIMULATION){
       EmuDcs_o->loadOneCFEB(3);
      }
      else{
        printf("load_cfeb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }
// *************************************************************************************************************
    else if(operation_command=="load_cfeb5"){
      if(!SIMULATION){
       EmuDcs_o->loadOneCFEB(4);
      }
      else{
        printf("load_cfeb SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command.find("load_alct") != string::npos){  
      if(!SIMULATION){
       printf("load_alct\n");
       EmuDcs_o->loadALCT(operation_command);
      }
      else{
        printf("load_alct SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command=="load_tmb"){  
      if(!SIMULATION){
       printf("load_tmb\n");
       EmuDcs_o->loadTMB();
      }
      else{
        printf("load_alct SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }



// *************************************************************************************************************
    else if(operation_command=="load_dmb_vme_chip"){
      if(!SIMULATION){
       EmuDcs_o->loadDAQMB_VMEChip();
      }
      else{
        printf("load_dmb_vme_chip SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }

// *************************************************************************************************************
    else if(operation_command=="load_dmb_vme_chip_emergency"){
      if(!SIMULATION){
       EmuDcs_o->loadDAQMB_VMEChipEmergency();
      }
      else{
        printf("load_dmb_vme_chip SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    else if(operation_command=="load_dmb_ctl_chip"){
      if(!SIMULATION){
       EmuDcs_o->loadDAQMB_ControlChip();
      }
      else{
        printf("load_dmb_ctl_chip SIMULATED\n");
      }
      COMMAND_1_DimBroker_o[db_index]->DcsUpdateService();
    }


// *************************************************************************************************************
    //    printf("1 %s \n",ip_address.c_str());
    /*
    while(1){
     if(CONFIRMATION)break;
     usleep(1000);
    }
    */
    if(!isALL)break;
    //    printf("1\n");
    }   // while(true)

  }  // while(1){ indefinite loop (there are no breaks for it)

}

//======================

DcsDimCommand::DcsDimCommand(DcsDimService **LV_1_DimBroker_o,
DcsDimService **TEMPERATURE_1_DimBroker_o,  DcsDimService **COMMAND_1_DimBroker_o,DcsDimService **REFERENCE_1_DimBroker_o,
DimService *RunControlService_o, EmuDcs *EmuDcs_o):DimCommand("LV_1_COMMAND","C"),Task(""){

#ifdef DCS_PRINTING_0
    printf("start command\n");
#endif
    this->LV_1_DimBroker_o=LV_1_DimBroker_o;


    this->TEMPERATURE_1_DimBroker_o=TEMPERATURE_1_DimBroker_o;
    this->COMMAND_1_DimBroker_o=COMMAND_1_DimBroker_o;
    this->REFERENCE_1_DimBroker_o=REFERENCE_1_DimBroker_o;
    this->RunControlService_o=RunControlService_o;
    this->EmuDcs_o = EmuDcs_o;

    if(CONFIRMATION_NEEDED)DcsDimService::CONFIRMATION = false;
    else DcsDimService::CONFIRMATION = true;

activate();

}
//=================================================================================================
int DcsDimCommand::getDataLV(bool isUpdate){

  static bool FIRST=true;


  ////      printf("get_data!!!!!============================== %d\n", EmuDcs_o->current_set);

  //#ifndef TESTMODE
  //::sleep(3);
  //#endif
              EmuDcs_o->alct_lv();
	      for(int i=1;i<= EmuDcs_o->number_of_cfebs; i++){
               EmuDcs_o->cfeb_lv(i);
	      }


	      //////   printf("cur (1) %d\n", EmuDcs_o->current_set);

      ///////	 if(!SIMULATION){
	   ////////   osu_start(dp->crate_slt);

	   //((LV_1_DimBroker *)LV_1_MonitorService->value)

	  if(EmuDcs_o->lowv_status(SIMULATION))((LV_1_DimBroker *)LV_1_DimBroker_o[db_index]->value)->data.Alct_o.status=
                                                                                      EmuDcs_o->lowv_status(SIMULATION);
          else ((LV_1_DimBroker *)LV_1_DimBroker_o[db_index]->value)->data.Alct_o.status=0;

	  if(FIRST){
           ((LV_1_DimBroker *)LV_1_DimBroker_o[db_index]->value)->data.Alct_o.status=-1;
	   FIRST=false;
	  }

	  ////////  osu_end();
	  ////}
	 /////   else{
	 //////     ((LV_1_DimBroker *)LV_1_DimBroker_o->value)->data.Alct_o.status=EmuDcs_o->slot_status[EmuDcs_o->current_set];                  
	 ///// }


	  //	 	 printf(" **************  status=%f %d %d\n",((LV_1_DimBroker *)LV_1_DimBroker_o->value)->data.Alct_o.status,
	  //                 EmuDcs_o->slot_status[EmuDcs_o->current_set], EmuDcs_o->current_set);
	 ///  printf("cur (2) %d\n", EmuDcs_o->current_set);


#ifdef DCS_PRINTING_0	  
	  printf("SUSPEND_UPDATE_SERVICE=%d ***\n",SUSPEND_UPDATE_SERVICE);
#endif

#ifndef TESTMODE
          if(!SUSPEND_UPDATE_SERVICE){
            if(isUpdate)LV_1_DimBroker_o[db_index]->DcsUpdateService();
           
	  }
#endif

    return 1;

  }
//=================================================================================================

int DcsDimCommand::get_db_index(string ip_address, int dmb_slot){

    char tmp[100];
    int db_index=-1;
      sprintf(tmp,"%s;%d",ip_address.c_str(), dmb_slot);
      for(int i10=0;i10<EmuDcs_o->slots.size();i10++){ 
       if(EmuDcs_o->slots[i10] == string(tmp)){
        db_index=i10; break;
       }
      }
#ifdef DCS_PRINTING_1
      printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ db_index=%d\n",db_index);
#endif
  return db_index;

}
