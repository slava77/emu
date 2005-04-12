
#include <EmuDcs.h>
#include <DcsDimCommand.h>
//=============================
#include "Singleton.h"
//=============================

int main(int argc, char *argv[]){

//------  xdaq parameters -----------------
  string file_to_load_cfeb;
  string file_to_load_vme_chip;
  string file_to_load_control_chip;

  string file_to_load_valct288;
  string file_to_load_valct384;
  string file_to_load_valct672;

  string file_to_load_salct288;
  string file_to_load_salct384;
  string file_to_load_salct672;

  string file_to_load_tmb;
  int IS_SIMULATION;  
  /*
  string *file_to_load_cfeb_ptr;
  string *file_to_load_vme_chip_ptr;
  string *file_to_load_control_chip_ptr;
  string *file_to_load_valct_ptr;
  string *file_to_load_salct_ptr;
  string *file_to_load_tmb_ptr;
  */
//------------------------------------------------
file_to_load_cfeb=
  "/home/fast/data/daqmb_config/feb_prom/fcntl_v9_r1.svf";
file_to_load_vme_chip=
  "/home/fast/data/daqmb_config/mthb_vprom/vme4_v16_r3.svf";
file_to_load_control_chip=
  "/home/fast/data/daqmb_config/mthb_cprom/cntl4_v35_r1.svf";
file_to_load_valct288=
  "/home/fast/data/daqmb_config/alct_vprom/alct288_virtex.svf";
file_to_load_valct384=
  "/home/fast/data/daqmb_config/alct_vprom/alct384_virtex.svf";
file_to_load_valct672=
  "/home/fast/data/daqmb_config/alct_vprom/alct672_virtex.svf";
file_to_load_tmb=
  "/home/fast/data/daqmb_config/tmb_eprom/tmb2001a_101802.svf";
//--------------------------------------------------

  string *file_to_load_cfeb_ptr = &file_to_load_cfeb;
  string *file_to_load_vme_chip_ptr = &file_to_load_vme_chip;
  string *file_to_load_control_chip_ptr = &file_to_load_control_chip; 

  string *file_to_load_valct288_ptr=&file_to_load_valct288;
  string *file_to_load_valct384_ptr=&file_to_load_valct384;
  string *file_to_load_valct672_ptr=&file_to_load_valct672;

  string *file_to_load_salct288_ptr=&file_to_load_salct288;
  string *file_to_load_salct384_ptr=&file_to_load_salct384;
  string *file_to_load_salct672_ptr=&file_to_load_salct672;

  string *file_to_load_tmb_ptr=&file_to_load_tmb;

   VMEController *vme;
   Crate * crate;
   DAQMB * daqmb;
   TMB *tmb_r;
   CCB *ccb_r;
   MPC *mpc_r;
   int ndaqmbs=2;
   int ntmbs=1;

  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
    printf("crate=%d\n",i);


//===================================================================================   

    //   VMEController *vme = new VMEController( "137.138.102.223",6030);

//------------------
    //fg vme=new VMEControllerdcs( "128.146.39.89",6030);
    vme=new VMEController(0, "128.146.39.89",6030);
   crate = new Crate(i, vme);
   for(int j=0;j<ndaqmbs;j++){
//fg     if(j==0){daqmb = new DAQMBdcs(i,13, 5);}
//fg     else if(j==1){daqmb = new DAQMBdcs(i,23, 5);}
     if(j==0){daqmb = new DAQMB(i,13, 5);}
     else if(j==1){daqmb = new DAQMB(i,23, 5);}
   }
   for(int j=0;j<ntmbs;j++){
      tmb_r = new TMB(0,21);
   }
//fg   ccb_r =  new CCBdcs(0,11 );
   ccb_r =  new CCB(0,11 );
  mpc_r= new MPC(0,20);
//----------------------

//=========================================================================
  crate->vmeController()->closeSocket();
  //crate->vmeController()->openSocket();
  Singleton<CrateSetup>::deleteInstance();

//------------------
//fg  vme=new VMEControllerdcs( "128.146.39.89",6030);
  vme=new VMEController(0, "128.146.39.89",6030);
   crate = new Crate(i, vme);
   for(int j=0;j<ndaqmbs;j++){
//fg     if(j==0){daqmb = new DAQMBdcs(i,13, 5);}
//fg     else if(j==1){daqmb = new DAQMBdcs(i,23, 5);}
     if(j==0){daqmb = new DAQMB(i,13, 5);}
     else if(j==1){daqmb = new DAQMB(i,23, 5);}
   }
   for(int j=0;j<ntmbs;j++){
      tmb_r = new TMB(0,21);
   }
//fg  ccb_r =  new CCBdcs(0,11 );
  ccb_r =  new CCB(0,11 );
  mpc_r= new MPC(0,20);
//----------------------



  } // ncrates
//===================================================================================



EmuDcs *EmuDcs_i = new EmuDcs(file_to_load_cfeb_ptr, file_to_load_vme_chip_ptr, file_to_load_control_chip_ptr,
                      file_to_load_valct288_ptr,file_to_load_salct288_ptr,
                      file_to_load_valct384_ptr,file_to_load_salct384_ptr,
                      file_to_load_valct672_ptr,file_to_load_salct672_ptr,
                      file_to_load_tmb_ptr);

 int d=0;
  while(1){
    d++;
    usleep(100000);
       continue;



//----------------------------
     if(d == 200){
       printf("EmuDcs_i->stop()\n");

       //  crate->vmeController()->closeSocket();
  //////  Singleton<CrateSetup>::deleteInstance();

       EmuDcs_i->soft_stop(); 
       ////   delete  EmuDcs_i;    
     }
//----------------------------
//=========================================================================================
     if(d == 400){
       
//------------------
/*
  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
  vme=new VMEController( "128.146.39.89",6030);
   crate = new Crate(i, vme);
   for(int j=0;j<ndaqmbs;j++){
     if(j==0){daqmb = new DAQMB(i,13, 5);}
     else if(j==1){daqmb = new DAQMB(i,23, 5);}
   }
   for(int j=0;j<ntmbs;j++){
      tmb_r = new TMB(0,21);
   }
  ccb_r =  new CCB(0,11 );
  mpc_r= new MPC(0,20);
  }
*/
//----------------------

//----------------------

//  crate->vmeController()->openSocket();
  EmuDcs_i->soft_start();
 

 /*
 EmuDcs_i = new EmuDcs(file_to_load_cfeb_ptr, file_to_load_vme_chip_ptr, file_to_load_control_chip_ptr,
                      file_to_load_valct288_ptr,file_to_load_salct288_ptr,
                      file_to_load_valct384_ptr,file_to_load_salct384_ptr,
                      file_to_load_valct672_ptr,file_to_load_salct672_ptr,
                      file_to_load_tmb_ptr);
  */
//----------------------
     } // if(d == 4){
//----------------------------
  }

  return 1;

}


//=============================





/*


int d=0;
  while(1){
    d++;
    usleep(100000);
//----------------------------
     if(d == 200){
       printf("EmuDcs_i->stop()\n");

  crate->vmeController()->closeSocket();
  Singleton<CrateSetup>::deleteInstance();

       EmuDcs_i->stop(); 
       delete  EmuDcs_i;    
     }
//----------------------------
//=========================================================================================
     if(d == 400){
       
//------------------

  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
  vme=new VMEController( "128.146.39.89",6030);
   crate = new Crate(i, vme);
   for(int j=0;j<ndaqmbs;j++){
     if(j==0){daqmb = new DAQMB(i,13, 5);}
     else if(j==1){daqmb = new DAQMB(i,23, 5);}
   }
   for(int j=0;j<ntmbs;j++){
      tmb_r = new TMB(0,21);
   }
  ccb_r =  new CCB(0,11 );
  mpc_r= new MPC(0,20);
  }

//----------------------

//----------------------
  crate->vmeController()->openSocket();
 EmuDcs_i = new EmuDcs(file_to_load_cfeb_ptr, file_to_load_vme_chip_ptr, file_to_load_control_chip_ptr,
                      file_to_load_valct288_ptr,file_to_load_salct288_ptr,
                      file_to_load_valct384_ptr,file_to_load_salct384_ptr,
                      file_to_load_valct672_ptr,file_to_load_salct672_ptr,
                      file_to_load_tmb_ptr);
//----------------------
     } // if(d == 4){
//----------------------------
    }


*/



//====================================


/*

 int d=0;
  while(1){
    d++;
    usleep(100000);
//----------------------------
     if(d == 200){
       printf("EmuDcs_i->stop()\n");

  crate->vmeController()->closeSocket();
  //////  Singleton<CrateSetup>::deleteInstance();

       EmuDcs_i->soft_stop(); 
       ////   delete  EmuDcs_i;    
     }
//----------------------------
//=========================================================================================
     if(d == 400){
       

  crate->vmeController()->openSocket();
  EmuDcs_i->soft_start();
 

//----------------------
     } // if(d == 4){
//----------------------------
  }


*/

