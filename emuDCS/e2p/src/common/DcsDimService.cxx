

#include <DcsDimService.h>
extern bool CONFIRMATION_NEEDED;
extern bool PUBLISHING_ENABLED;

DcsDimService::DcsDimService(char *name, char *format, void *value, int size) : DimService(name, format, value, size){

      this->value =value; 
      strcpy(this->service_name,name);
}

bool DcsDimService::CONFIRMATION=false;

void DcsDimService::DcsUpdateService(bool isConfirmationNeeded){


  //  printf("service_name=%s+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>>>>>>>>>>>>>\n",service_name);
  if(!PUBLISHING_ENABLED)return;

if(CONFIRMATION_NEEDED) DcsDimService::CONFIRMATION=false;
else DcsDimService::CONFIRMATION = true;

  updateService();
  if(isConfirmationNeeded){

  int step=1000;
  int timeout=3000; // measured in steps
  int time_passed=0;
#ifdef DCS_PRINTING_0
  printf("TIMEOUT STARTS\n");
#endif
    while(1){

      if(DcsDimService::CONFIRMATION)break;
      usleep(step);
      time_passed++;
      if(time_passed >= timeout){
	printf("TIMEOUT: NO CONFIRMATION RECEIVED: PROBABLY THE SERVER IS NOT RUNNING (start_servers command)\n");
	DcsDimService::CONFIRMATION=true;
	PUBLISHING_ENABLED=false;
      }
    }

  }

}
