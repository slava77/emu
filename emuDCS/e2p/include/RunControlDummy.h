#ifndef _RunControlDummy_h_
#define _RunControlDummy_h_

#include <dic.hxx>

//////#include "xdaq.h"

#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
/////////#include "xoap.h"

// VERSION INFORMATION
#include "PackageInfo.h"

// Package dependencies
#include "toolboxV.h"
////////#include "xoapV.h"
////////#include "xdaqV.h"


#include <EmuDcs.h>
#include <DcsDimCommand.h>





/*
#include "PeripheralCrateController.h"
#include "VMEController.h"
#include "DAQMB.h"
#include "CCB.h"
#include "FakeCCB.h"
#include "TMB.h"
*/
#include "CrateSelector.h" // !!!!!!!


namespace RunControlDummy4
{
    const string package  =  "RunControlDummy4";
    const string versions =  "1.0";
    const string description = "RunControlDummy4: bla";

#define PACKAGE_NAMESPACE RunControlDummy4

    ////#include "RunControlDummyV.i"

#undef PACKAGE_NAMESPACE

}


//===================================================================

// this small class is needed to receive DIM message (we need to receive ACKLOWLEDGE)

class InfoMonitorService : public DimInfo
{
  string name;

	void infoHandler()
	{


	  if(name=="DCS_SERVICE"){
            ACK_RECEIVED = true;
            cout << "DCS_SERVICE received" << endl;
	  }

	}
public :
  bool ACK_RECEIVED;

InfoMonitorService(char *name) : DimInfo(name, -1.0)
{this->name=string(name);}

};
//===================================================================
//===========================================================================================================

class RunControlDummy: public xdaqApplication  
{


//========= share II========
//------  xdaq parameters -----------------

  /*

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

  string *file_to_load_cfeb_ptr;
  string *file_to_load_vme_chip_ptr;
  string *file_to_load_control_chip_ptr;

  string *file_to_load_valct288_ptr;
  string *file_to_load_valct384_ptr;
  string *file_to_load_valct672_ptr;

  string *file_to_load_salct288_ptr;
  string *file_to_load_salct384_ptr;
  string *file_to_load_salct672_ptr;

  string *file_to_load_tmb_ptr;
  */
//-----------------------------------------
  EmuDcs *EmuDcs_i;
//-----------------------------------------
//========= share II========
	
	public:

   CrateSelector theSelector; // it is created in the real run configuration


    InfoMonitorService *monitorService;
 
//--------------------------------------------------------------------------------------------	
//===================================================================
// corr++
int RunControlDummy::setup() {

	  printf("DIM Configure()\n");
                monitorService = new InfoMonitorService("DCS_SERVICE");

return 1;

}
//===================================================================
// corr++
int RunControlDummy::DcsDisable() {


          monitorService->ACK_RECEIVED = false; // corr+++
          DimClient::sendCommand("LV_1_COMMAND","DCS_DISABLE");// corr+++

          int counter=0; // corr+++
          while(1){ // corr+++
	    usleep(50000);
            counter++;
            if(monitorService->ACK_RECEIVED)break;
            if(counter==20){printf("DCS does not reply TIME OUT\n"); break;}
	  }

return 1;

}
//===================================================================
// corr++
int RunControlDummy::DcsEnable() {

         DimClient::sendCommand("LV_1_COMMAND","DCS_ENABLE");  // corr+++ 

return 1;

}
//===================================================================


RunControlDummy () // constructor 
	{

	//-----------------------
        soapBindMethod(this, &RunControlDummy::RunConfigure_atStart, "RunConfigure_atStart");
        soapBindMethod(this, &RunControlDummy::RunConfigure_atStop, "RunConfigure_atStop");
        soapBindMethod(this, &RunControlDummy::Dcs_Start, "Dcs_Start");
	//---------------------- 

	}
//--------------------------------------------------------------------------------------------	
	void Configure() throw (xdaqException)
	{


	  setup();
	  DcsDisable();// corr+++

	  /* II
//------------------------------
  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
    printf("crate=%d\n",i);
   
    //   VMEController *vme = new VMEController( "137.138.102.223",6030);
   VMEController *vme = new VMEControllerdcs( "128.146.39.89",6030);

   Crate * crate = new Crate(i, vme);

   int ndaqmbs=1;
   for(int j=0;j<ndaqmbs;j++){
     //     printf("&&&&&&&&&&&&&&&&&&& %d \n",(*(*(*theCrateSetups)[0]->theCrates)[i]->daqmbs)[j]->theSlot);
     //    DAQMB * daqmb = new DAQMB(i,13+j*2, 5);
     DAQMB * daqmb;
     if(j==0){
       int cfeb_number_1=1;
      vector<CFEB> cfebs_vector_1;
      
        for(int i_cfeb=0;i_cfeb < cfeb_number_1; i_cfeb++)
      	cfebs_vector_1.push_back(CFEB(i_cfeb)); // it is for CFEB eprom loading (to kno how many CFEBs to load)

      daqmb = new DAQMBdcs(i,13,cfebs_vector_1);

     }
     else if(j==1){
      daqmb = new DAQMBdcs(i,23);
     }
   }

   int ntmbs=1;
   for(int j=0;j<ntmbs;j++){
     TMB *tmb_r = new TMB(0,12);

   }

  CCB *ccb_r =  new CCBdcs(0,11 );

  MPC *mpc_r = new MPC(0,20);


  } // ncrates
//------------------------------

	  */
	  

//========== II ====================
  int dmb_cr[3][2];
  int tmb_cr[3][2];
    
  
  dmb_cr[0][0]=9;
  dmb_cr[0][1]=19;
  dmb_cr[1][0]=7;
  dmb_cr[1][1]=19;

  tmb_cr[0][0]=8;
  tmb_cr[0][1]=18;
  tmb_cr[1][0]=6;
  tmb_cr[1][1]=18;
  char *ip_addr[]={"10.0.0.11","10.0.0.3"}; // 10.0.0.4 is the bootom crate

  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
    printf("crate=%d\n",i);

    VMEController *vme;

 vme = new VMEControllerdcs( ip_addr[i],6050); // ??????????????????????????????????????????

   Crate * crate = new Crate(i, vme);

   int ndaqmbs=2;
   for(int j=0;j<ndaqmbs;j++){
 
     DAQMB * daqmb;

daqmb = new DAQMBdcs(i,dmb_cr[i][j],5);



   }

   int ntmbs=2;

   for(int j=0;j<ntmbs;j++){
       TMB *tmb_r = new TMB(i,tmb_cr[i][j]);
   }

  CCB *ccb_r =  new CCBdcs(i,13);

  MPC *mpc_r = new MPC(i,12);


  } // ncrates


  printf("================ end tree creation ================\n");

//===============================




  vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    ///////// II    myCrates[i]->vmeController()->openSocket(); // // corr+++
 //--------------            
    //////////    configure(myCrates[i]);
	  cout << "DOING SOME RUN CONFIGURATION VIA D360 ==CONFIGURE==" << endl;
          usleep(1000000); // delay just for testing purposes: that should be replaced by real config.
	  cout << "END OF DOING SOME RUN CONFIGURATION VIA D360 ==CONFIGURE==" << endl;
 //--------------
///////// II    myCrates[i]->vmeController()->closeSocket();  // corr+++
  }

  DcsEnable(); // corr+++


  //========= share II========

  /*
file_to_load_cfeb = "/home/fast/data/daqmb_config/feb_prom/fcntl_v9_r1.svf";
file_to_load_vme_chip="/home/fast/data/daqmb_config/mthb_vprom/vme4_v16_r3.svf";
file_to_load_control_chip="/home/fast/data/daqmb_config/mthb_cprom/cntl4_v35_r1.svf";
file_to_load_valct288="/home/fast/data/daqmb_config/alct_vprom/alct288_virtex.svf";
file_to_load_valct384="/home/fast/data/daqmb_config/alct_vprom/alct384_virtex.svf";
file_to_load_valct672="/home/fast/data/daqmb_config/alct_vprom/alct672_virtex.svf";
file_to_load_tmb="/home/fast/data/daqmb_config/tmb_eprom/tmb2001a_101802.svf";


  file_to_load_cfeb_ptr = &file_to_load_cfeb;
  file_to_load_vme_chip_ptr = &file_to_load_vme_chip;
  file_to_load_control_chip_ptr = &file_to_load_control_chip; 

  file_to_load_valct288_ptr=&file_to_load_valct288;
  file_to_load_valct384_ptr=&file_to_load_valct384;
  file_to_load_valct672_ptr=&file_to_load_valct672;

  file_to_load_salct288_ptr=&file_to_load_salct288;
  file_to_load_salct384_ptr=&file_to_load_salct384;
  file_to_load_salct672_ptr=&file_to_load_salct672;

  file_to_load_tmb_ptr=&file_to_load_tmb;


  EmuDcs_i=new EmuDcs(&file_to_load_cfeb, &file_to_load_vme_chip, &file_to_load_control_chip,
                      &file_to_load_valct288,&file_to_load_salct288,
                      &file_to_load_valct384,&file_to_load_salct384,
                      &file_to_load_valct672,&file_to_load_salct672,
                      &file_to_load_tmb);

  */
  //////  EmuDcs_i=new EmuDcs();

  printf("mmm\n"); 
 //========= share II========



	}
	
//--------------------------------------------------------------------------------------------		
	protected:
SOAPMessage  Dcs_Start(SOAPMessage & message){

  EmuDcs_i=new EmuDcs();

return message;

}

	
//-----------------------------------------------------------  
// this function is simulation of RUN configuration at run start
SOAPMessage  RunConfigure_atStart(SOAPMessage & message)
        {


  DcsDisable();// corr+++ 

  vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
 ///////// II   myCrates[i]->vmeController()->openSocket(); // // corr+++

 //--------------     
    //////    enable(myCrates[i]);       
	  cout << "DOING SOME RUN CONFIGURATION VIA D360 == ENABLE==="  << endl;
          usleep(1000000*20); // delay just for testing purposes: that should be replaced by real config.
           
	  cout << "END OF DOING SOME RUN CONFIGURATION VIA D360 ==ENABLE==" << endl;
 //--------------
  ///////// II  myCrates[i]->vmeController()->closeSocket(); // // corr+++
  }
  cout << "TAKING DATA" << endl;
  DcsEnable(); // corr+++ 

       
          return message;
	}
//===========================================================
// this function is simulation of RUN configuration at run finish
SOAPMessage  RunConfigure_atStop(SOAPMessage & message)
        {



 DcsDisable();// corr+++

  vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
   ///////// II myCrates[i]->vmeController()->openSocket(); // // corr+++

 //--------------   
    //////    disable(myCrates[i]);         
	  cout << "DOING CONFIGURATION AFTER RUN ==DISABLE== VIA D360" << endl;
          usleep(1000000*20); // delay just for testing purposes: that should be replaced by real config.
          
	  cout << "END OF DOING CONFIGURATION AFTER RUN ==DISABLE== VIA D360" << endl;
 //--------------
  ///////// II  myCrates[i]->vmeController()->closeSocket(); // // corr+++
  }

  DcsEnable();// corr+++ 

       
          return message;
	}

};



//=======================================================================================

class RunControlDummySO: public xdaqSO 
{
        public:
	
	// seen as main program for user application
        void init() 
	{
		// blindly
		allocatePluggable("RunControlDummy");	
        }
	
	// Put interface of this into xdaqSO
	xdaqPluggable * create(string name) 
	{
		if ( name == "RunControlDummy" ) return new RunControlDummy();
	}

        void shutdown() 
	{
        }
	
	// Move to xdaqSO
	void allocatePluggable(string name) 
	{
		U16 localHost = xdaq::getHostId();
		int num = xdaq::getNumInstances(name);

		I2O_TID tid;
		U16 host;

		for (int i = 0; i < num; i++)
		{
			tid = xdaq::getTid(name, i); 
			host = xdaq::getHostId(tid);
			if (host == localHost)
			{
					xdaqPluggable * plugin = create(name);
					xdaq::load(plugin);
					break;
			}
		}
	}

        protected:

    
};




#endif
