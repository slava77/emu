#include <iostream>
#include <unistd.h> // for sleep
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "DDU.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "CrateSelector.h"

using namespace std;

int main(int argc,char **argv){

  // create VME Controller and Crate
  int crateId(0);
  string ipAddr("02:00:00:00:00:05");
  int port(2);
  VMEController *dynatem = new VMEController(crateId);
  dynatem->init(ipAddr,port);
  Crate *crate = new Crate(crateId,dynatem);

  // create CCB
  int ccbSlot(13);
  CCB *ccb = new CCB(crateId,ccbSlot,2004);
  ccb->configure();
  ::sleep(1);

  // create TMB & ALCT
  int tmbSlot(2);
  string chamberType("ME21");
  TMB *tmb = new TMB(crateId,tmbSlot);
  //
  cout << "Read Register" << endl;
  tmb->ReadRegister(0x4);  
  //
  tmb->version_="2004";
  cout << "Creating ALCT..." << endl;
  ALCTController *alct = new ALCTController(tmb,chamberType);
  cout << "Done..." << endl;

  //for (int i=0;i<42;i++){
  //  alct->delays_[i]=0;
  //  alct->thresholds_[i] = 20;
  //}

  //tmb->SetALCTController(alct);

  //tmb->configure();
  //::sleep(1);
  //alct->setup(1);
  //::sleep(1);

  ALCTIDRegister sc_id, chipID ;

  printf("Reading IDs...") ;

  alct->alct_read_slowcontrol_id(&sc_id) ;
  std::cout <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
  alct->alct_fast_read_id(chipID);
  std::cout << " ALCT Fastcontrol ID " << chipID << std::endl;

#if 1
  tmb->disableAllClocks();
  int debugMode(0);
  int jch(3);
  printf("Programming...");
  //int status;
  //int status = alct->SVFLoad(&jch,"alctcrc384mirror.svf",debugMode);
  int status = alct->SVFLoad(&jch,"../svf/tests/alct672rl_empty_dav.svf",debugMode);
  //int status = alct->SVFLoad(&jch,"../svf/alct672rl.svf",debugMode);
  //--int status = alct->NewSVFLoad(&jch,"alctcrc384mirror.svf",debugMode);
  tmb->enableAllClocks();

  if (status >= 0){
    cout << "=== Programming finished"<< endl;
    cout << "=== " << status << " Verify Errors  occured" << endl;
  }
  else{
    cout << "=== Fatal Error. Exiting with " <<  status << endl;
  }

#endif

}



/*
cfeb_control:

void prg_alct_cb(EZ_Widget *widget, void *data)
{
static char design[10];
int i,txtlen;
int call_fsel;
char *stmp;
GenDATA *dp;
   if(widget){
     dp=(GenDATA *)data;
     call_fsel=1;
     EZ_ConfigureWidget(config_button,
                         EZ_BORDER_TYPE, EZ_BORDER_SUNKEN,
                         EZ_FOREGROUND,"red",0);
     dp->w[0]=config_button;
     stmp=malloc(strlen(dp->config_prepath)+36);
     strcpy(stmp,dp->config_prepath);
     i = EZ_GetWidgetReturnData(widget);
     switch(i){
       case 1:
	 strcat(stmp,"/alct_vprom/*.svf");
	 printf(" stmp %s \n",stmp ) ;
         printf(" about to call reset \n");
	 dp->fsel_funct=ALCTPRGPROM;
         dp->devnum=RESET;
         strcpy(design,"DAQMBV");
         break;
       default:
         call_fsel=0;
         printf("Program ALCT Menu Illegal selection: i=%d\n",i);
         break;
     } 
     dp->s[0]=design;
     if(call_fsel==1){
       printf(" In file selector \n"); 
       EZ_SetFileSelectorInitialPattern(fsel,stmp);
       printf(" Out of file selector \n") ;
       EZ_ActivateWidget(fsel);
       printf(" Out of Widget \n") ;
     }
     else {
       EZ_ConfigureWidget(dp->w[0],
                         EZ_BORDER_TYPE, EZ_BORDER_RAISED,
                         EZ_FOREGROUND,NULL,0);
     }
     free(stmp);
     free(dp->config_downloaded);
     dp->config_downloaded = (char *) malloc(strlen(chngd)+1);
     strcpy(dp->config_downloaded,chngd);
     txtlen=strlen(dp->config_downloaded);
     EZ_ConfigureWidget(DownLoad_file,EZ_LABEL_STRING,dp->config_downloaded,
                           EZ_TEXT_LINE_LENGTH,txtlen,
                           EZ_LABEL_POSITION,EZ_LEFT,0);
   }
}


cfeb_control:

        case ALCTPRGPROM:
           jch = 3;
           SVFdebug = 0 ;
           printf(" Program ALCT \n") ;
           printf(" ALCT.SVFfilename %s \n",buf) ;
           osu_end();
           ret=SVFLoad(dp->tmb_slt, &jch, buf, SVFdebug ) ; 
           osu_start(dp->crate_slt);    
           if (ret >= 0)
           {
	      printf("=== Programming finished\n");
	      printf("=== %d Verify Errors  occured\n", ret);
           }
           else
           {
	      printf("=== Fatal Error. Exiting with %d\n", ret);
           }
           break;


*/
