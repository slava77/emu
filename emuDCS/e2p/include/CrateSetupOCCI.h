#ifndef _CrateSetupOCCI_h_
#define _CrateSetupOCCI_h_

int SIMULATION=false;

////////////#include "xdaq.h"


// VERSION INFORMATION
#include "PackageInfo.h"

// Package dependencies
#include "toolboxV.h"
///////////#include "xoapV.h"
//////////#include "xdaqV.h"

namespace CrateSetupOCCI1
 {
    const string package  =  "CrateSetupOCCI1";
    const string versions =  "2.0";
    const string description = "CrateSetupOCCI1 2: some description";

#define PACKAGE_NAMESPACE CrateSetupOCCI1

#include "CrateSetupOCCIV.i"

#undef PACKAGE_NAMESPACE

}



#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "xoap.h"

/*
#include <scan_subs.h>
#include <calcons2.h>
#include <const.h>
#include <ddu_cpp.h>
#include <eazjtag_cpp.h>
#include <funct_proto_cpp.h>
#include <data_struct.h>
*/


#include <EmuDcs.h>
#include <DcsDimCommand.h>



//#include <occi.h>      //ORACLE OCCI
//using namespace oracle::occi;
//#include <CrateSetupOCCI_gen.h>  
//#include <CrateSetupOCCIRegisterMappings_gen.h>
//#include <CrateSetupOCCIExtension.h>

/////////#include <vsm>
//#include <CrateSetupP.h>
#include <DcsCommon.h>

#include "BSem.h"
#include "StopWatch.hh"

//======================

#include <xercesc/dom/DOM.hpp>


///////////////#include <dom/DOM.hpp>

//---------------------------------------------------------------------
//---------------------------------------------------------------------


  class CrateSetupP_dstore { 
  public:
    int  setup_id; 
    int  reserve2;
  };

//-------------------------------------------------------------


  class CrateP_dstore {
  public:
    int crate_id;
    int setup_id;
    int   theNumber;


};
//-------------------------------------------------------------



  class VMEControllerP_dstore{
  public:
    int controller_id;
    int crate_id;
    string addr;
    int port;

};

//-------------------------------------------------------------


  class MPCP_dstore {
  public:
    int mpc_id;
    int crate_id;
    int    theSlot;
    int    theBaseAddress;
    string reserve;

};

//-------------------------------------------------------------


  class CCBP_dstore {
  public:
    int ccb_id;
    int crate_id;
    int    theSlot;
    int    l1enabled_; 
    int    TTC;
    int    CLK_INIT_FLAG; 
    int    CCB_CSR1_SAV;
    int    useTTCclock_;
    int    BX_Orbit_;
    int    SPS25ns_;
    string Version1;

};

//-------------------------------------------------------------


  class TMBP_dstore {
  public:
    int tmb_id;
    int crate_id;
    int theSlot;

};

//------------------------------------------------------------------------


  class DAQMBP_dstore {
  public:
    int daqmb_id;
    int crate_id;
    int theSlot;
    float pul_dac_set_; 
    float inj_dac_set_;
    float set_comp_thresh_;
    int comp_mode_;
    int comp_timing_;
    int pre_block_end_; 
    int calibration_delay_;
    int feb_dav_delay_;
    int time_samples_;

};

//------------------------------------------------------------------------


 class  CFEBP_dstore{
  public:
   int cfeb_id;
   int daqmb_id;

};

//---------------------------------------------------------------------

 class  BuckeyeChipP_dstore {
  public:
   int reserve1;
   int buckeye_id;
   int cfeb_id;

};


//---------------------------------------------------------------------


//====================================================================================
//====================================================================================
//====================================================================================


class CrateSetupOCCI : public xdaqApplication{
  
   vector<string> CrateSetupNames;
  vector<DAQMBP_dstore> dmbs_vector;
vector<TMBP_dstore> tmbs_vector;

  VMEControllerP_dstore *vme_controller_dstore;
		CrateSetupP_dstore *crate_setup_dstore; 
		CrateP_dstore  *crate_dstore; 
		CCBP_dstore *ccb_dstore; 
		TMBP_dstore *tmb_dstore; 
		MPCP_dstore *mpc_dstore; 
		DAQMBP_dstore *daqmb_dstore; 
		CFEBP_dstore *cfeb_dstore; 
		BuckeyeChipP_dstore *buckeye_dstore; 



		///// occi    Environment *env;
		///// occi    Connection *conn;
    string dataSource;
    string username;
    string password;

  //==========================

  string DpSetName_;
  string DpSetValue_;
  string DpGetName_;
  string DpSubscribeName_;
  string Callback_;

  vector<string> par_names;
  vector<string> par_values; 
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
//------------------------------------------------
  string databaseUrl_;
  string typeId_;
  string dbVendor_;
  string configSpaceDefinitionFile_;
  unsigned long databaseTimeout_;
  bool dataReady_;
  bool timeoutInUse_;
  DOMDocument* doc;
  unsigned int serverTid_;
  unsigned int localTid_;
  SOAPMessage databaseReply_;
  //  list< DBCaller* > callerList_;
  unsigned long holyCounter_;
  BSem mutex_;



  EmuDcs *EmuDcs_i;
//-----------------------------------------


 DOMNode *rootNode;


 protected:
  I2O_TID PvssInterfaceTid_;

 public:
//========================================================================================================================
  void Configure()  throw (xdaqException) {
 printf(" Configure %d %d %s %s\n",serverTid_,localTid_,databaseUrl_.c_str(),dbVendor_.c_str());
  
 ///// serverTid_ = xdaq::getTid("XDAQDataStore",0);
  localTid_ = tid();  

  

  printf("Configure  %d %d %s %s time_out=%ld\n",serverTid_,localTid_,databaseUrl_.c_str(),dbVendor_.c_str(),databaseTimeout_);

  /*
   
  //-----------------------------------------------------------

  string boot = string("/home/fast/Z/");
  vsmBase c(boot);

  //-----------------------------------------------------------
 
  
   vsmVector<CrateSetupPV> *theCrateSetups;
   c.create_container((vsmVector<CrateSetupPV> *&)theCrateSetups,"theCrateSetups");

   CrateSetupPV  *cratesetup_ov = new CrateSetupPV();
   theCrateSetups->append_objectTree(cratesetup_ov);
  
  
  //-----------------------------------------------------------
   VMEControllerPV *vme_c_1;
   ///    if(SIMULATION)
    vme_c_1 = new VMEControllerPV("137.138.102.223",6030);
    ///    else
    ///    vme_c_1 = new VMEControllerPV("128.146.39.89",6030);

   

   CratePV *crate_1 = new CratePV(0, vme_c_1);

   crate_1->mpc = new  MPCPV(21,101);
   crate_1->ccb = new CCBPV(11,1,1,1,1,1,1,1);

   (*theCrateSetups)[0]->theCrates->append_objectTree(crate_1);
  
  
  //-----------------------------------------------------------
   
   DAQMBPV *daqmb_11 = new DAQMBPV(13,0,0,0,0,0,0,0,0,0);
   (*(*theCrateSetups)[0]->theCrates)[0]->daqmbs->append_objectTree(daqmb_11);

   //-----------------------------------------------------------
 
   CFEBPV *cfeb = new CFEBPV();
   (*(*(*theCrateSetups)[0]->theCrates)[0]->daqmbs)[0]->cfebs->append_objectTree(cfeb);   
    
   //-----------------------------------------------------------
   

   BuckeyeChipPV *buckeye = new BuckeyeChipPV(10);
   
   (*(*(*(*theCrateSetups)[0]->theCrates)[0]->daqmbs)[0]->cfebs)[0]->buckeyeChips->append_objectTree(buckeye); 

   //-----------------------------------------------------------

   TMBPV *tmb_1 = new TMBPV(12);
   (*(*theCrateSetups)[0]->theCrates)[0]->tmbs->append_objectTree(tmb_1);


 //========================================================================================================
 
  
 //#############################################

  int VSM_READ=1;

  if(VSM_READ){
  int ncrates=(*theCrateSetups)[0]->theCrates->getting_size();

  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
    printf("crate=%d\n",i);
 
    cout << "=="<< (*(*theCrateSetups)[0]->theCrates)[i]->theController->addr <<"==" <<endl;

   VMEController *vme = new VMEController( (*(*theCrateSetups)[0]->theCrates)[i]->theController->addr,
                                            (*(*theCrateSetups)[0]->theCrates)[i]->theController->port);

   Crate * crate = new Crate((*(*theCrateSetups)[0]->theCrates)[i]->theNumber+i, vme);

   //    int ndaqmbs=1;
   int ndaqmbs=(*(*theCrateSetups)[0]->theCrates)[i]->daqmbs->getting_size();
   for(int j=0;j<ndaqmbs;j++){
     printf("&&&&&&&&&&&&&&&&&&& %d \n",(*(*(*theCrateSetups)[0]->theCrates)[i]->daqmbs)[j]->theSlot);
    DAQMB * daqmb = new DAQMB((*(*theCrateSetups)[0]->theCrates)[i]->theNumber, 
                              (*(*(*theCrateSetups)[0]->theCrates)[i]->daqmbs)[j]->theSlot, 5);
   }


   int ntmbs=(*(*theCrateSetups)[0]->theCrates)[i]->tmbs->getting_size();
   for(int j=0;j<ntmbs;j++){
     TMB *tmb_r = new TMB((*(*theCrateSetups)[0]->theCrates)[i]->theNumber, 
                          (*(*(*theCrateSetups)[0]->theCrates)[i]->tmbs)[j]->theSlot);

   }

   if(vsmUtils::isValid("(*(*theCrateSetups)[0]->theCrates)[i]->ccb->theSlot"))
  CCB *ccb_r =  new CCB((*(*theCrateSetups)[0]->theCrates)[i]->theNumber, 
                          (*(*theCrateSetups)[0]->theCrates)[i]->ccb->theSlot );

   if(vsmUtils::isValid("(*(*theCrateSetups)[0]->theCrates)[i]->mpc->theSlot"));
  MPC *mpc_r = new MPC((*(*theCrateSetups)[0]->theCrates)[i]->theNumber, 
                          (*(*theCrateSetups)[0]->theCrates)[i]->mpc->theSlot );
   

  } // for(int i=0;i<ncrates;i++){

  } // if(VSM_READ){

 */

 
    ///    vme_c_1 = new VMEControllerPV("128.146.39.89",6030);
  
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
  /*
  
  dmb_cr[0][0]=7;
  dmb_cr[0][1]=19;
  dmb_cr[1][0]=5;
  dmb_cr[1][1]=17;

  tmb_cr[0][0]=6;
  tmb_cr[0][1]=18;
  tmb_cr[1][0]=4;
  tmb_cr[1][1]=16;
  char *ip_addr[]={"10.0.0.3","10.0.0.4"};
  */

  for(int i=0;i<1;i++){ // ncrates !!!!!!!!!!!!!!!!1
    printf("crate=%d\n",i);

    VMEController *vme;

    //   VMEController *vme = new VMEController( "137.138.102.223",6030);
    //    VMEController *vme = new VMEControllerdcs( "128.146.39.89",6030); // ??????????????????????????????????????????


 vme = new VMEControllerdcs( ip_addr[i],6050); // ??????????????????????????????????????????

   Crate * crate = new Crate(i, vme);

   int ndaqmbs=2;
   for(int j=0;j<ndaqmbs;j++){
     //     printf("&&&&&&&&&&&&&&&&&&& %d \n",(*(*(*theCrateSetups)[0]->theCrates)[i]->daqmbs)[j]->theSlot);
     //    DAQMB * daqmb = new DAQMB(i,13+j*2, 5);
     DAQMB * daqmb;
     /*
     if(j==0){
       
       int cfeb_number_1=5;
      vector<CFEB> cfebs_vector_1;
      
        for(int i_cfeb=0;i_cfeb < cfeb_number_1; i_cfeb++)
      	cfebs_vector_1.push_back(CFEB(i_cfeb)); // it is for CFEB eprom loading (to kno how many CFEBs to load)

	daqmb = new DAQMBdcs(i,5,cfebs_vector_1); //??????????????

       */

daqmb = new DAQMBdcs(i,dmb_cr[i][j],5);

/*
     if(j==0 && i==0){
       daqmb = new DAQMBdcs(i,7,5);  // 7, 5 
     }
     else if(j==1){ 
       daqmb = new DAQMBdcs(i,19,5); // 19,17  ???????????????
     }
*/

   }

   int ntmbs=2;

   for(int j=0;j<ntmbs;j++){
       TMB *tmb_r = new TMB(i,tmb_cr[i][j]);
     /*
     if(j==0)
       TMB *tmb_r = new TMB(0,6); //  6,4 ???????????????
     else if(j==1)
       TMB *tmb_r = new TMB(0,18); // 18,16   ???????????????
     */
   }

  CCB *ccb_r =  new CCBdcs(i,13);

  MPC *mpc_r = new MPC(i,12);


  } // ncrates

  }

//=======================================================================================================

  void Enable()
  throw (xdaqException) { 

    /* fsm
   string typeId;
   DOM_Node rootNode;
  DOM_Document replyDoc = sendRequest( "CRATESETUPP_DSTORE", "SETUP_ID", typeId, rootNode ); 
  string empty_s = string("");
  DOM2OBJECTconvert(rootNode,empty_s); ///  to complete ***************************************************************
    */
    /*
  string *file_to_load_cfeb_ptr = &file_to_load_cfeb;
  string *file_to_load_vme_chip_ptr = &file_to_load_vme_chip;
  string *file_to_load_control_chip_ptr = &file_to_load_control_chip; 

  string *file_to_load_valct_ptr=&file_to_load_valct;
  string *file_to_load_salct_ptr=&file_to_load_salct;
  string *file_to_load_tmb_ptr=&file_to_load_tmb;
    */
  printf("%s \n",file_to_load_control_chip_ptr->c_str());
    

  EmuDcs_i=new EmuDcs(file_to_load_cfeb_ptr, file_to_load_vme_chip_ptr, file_to_load_control_chip_ptr,
                      file_to_load_valct288_ptr,file_to_load_salct288_ptr,
                      file_to_load_valct384_ptr,file_to_load_salct384_ptr,
                      file_to_load_valct672_ptr,file_to_load_salct672_ptr,
                      file_to_load_tmb_ptr);

  printf("mmm\n"); 
  
  }
  

 public:
//==========================================================================================================
  CrateSetupOCCI() : mutex_(BSem::FULL)  {

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


 exportParam ( "file_to_load_cfeb" , file_to_load_cfeb  );
 exportParam ( "file_to_load_vme_chip" , file_to_load_vme_chip  );
 exportParam ( "file_to_load_control_chip" , file_to_load_control_chip  );

 exportParam ( "file_to_load_valct288" , file_to_load_valct288  );
 exportParam ( "file_to_load_valct384" , file_to_load_valct384  );
 exportParam ( "file_to_load_valct672" , file_to_load_valct672  );

 exportParam ( "file_to_load_salct288" , file_to_load_salct288  );
 exportParam ( "file_to_load_salct384" , file_to_load_salct384  );
 exportParam ( "file_to_load_salct672" , file_to_load_salct672  );

 exportParam ( "file_to_load_tmb" , file_to_load_tmb  );
 exportParam ( "IS_SIMULATION" , IS_SIMULATION  );

 soapBindMethod(this, &CrateSetupOCCI::DCS_ENABLE, "DCS_ENABLE");
 soapBindMethod(this, &CrateSetupOCCI::DCS_DISABLE, "DCS_DISABLE");

 printf("params export \n");
  }


//============================================================================
        SOAPMessage  DCS_ENABLE(SOAPMessage & message)
        {
           
	  
		cout << " DCS is responding " << endl;
		
		I2O_TID client_ = 0;

	
		SOAPMessage reply;

		SOAPEnvelope envelope = reply.getSOAPPart().getEnvelope();

		SOAPBody b = envelope.getBody();
	      
		SOAPName responseName = envelope.createName("OKDCS","","");
		
		SOAPBodyElement e = b.addBodyElement ( responseName );


          DimClient::sendCommand("LV_1_COMMAND","DCS_ENABLE");

		/*	
                SOAPName counterName = envelope.createName("Counter", "", "");
                SOAPElement counterElement = e.addChildElement(counterName);
                xdaqInteger cv (counter_);
                counterElement.addTextNode(cv.toString());
		*/
		return reply;
	
	}


//============================================================================
//============================================================================
        SOAPMessage  DCS_DISABLE(SOAPMessage & message)
        {
           
	  
		cout << " DCS is responding " << endl;
		
		I2O_TID client_ = 0;

	
		SOAPMessage reply;

		SOAPEnvelope envelope = reply.getSOAPPart().getEnvelope();

		SOAPBody b = envelope.getBody();
	      
		SOAPName responseName = envelope.createName("OKDCS","","");
		
		SOAPBodyElement e = b.addBodyElement ( responseName );

          DimClient::sendCommand("LV_1_COMMAND","DCS_DISABLE");
		/*	
                SOAPName counterName = envelope.createName("Counter", "", "");
                SOAPElement counterElement = e.addChildElement(counterName);
                xdaqInteger cv (counter_);
                counterElement.addTextNode(cv.toString());
		*/
		return reply;
	
	}
};


//============================================================================
class CrateSetupOCCISO: public xdaqSO 
{
        public:
	
	// seen as main program for user application
        void init() 
	{
		// blindly
	  printf("init\n");
	  allocatePluggable("CrateSetupOCCI");

        }
	
	// Put interface of this into xdaqSO
	xdaqPluggable * create(string name) 
	{
		if ( name == "CrateSetupOCCI" ) return new CrateSetupOCCI();

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
