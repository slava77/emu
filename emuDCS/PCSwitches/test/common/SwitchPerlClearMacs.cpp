#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "telnet.h"

void fill_switch_macs(char *thismac);
void parse_mac();
void print_switch_configurations(char *thismac);
void fill_char(char *var,char *line,int first,int length);

typedef struct Mac{
  char *mac;
  char status[8];
}MAC;

typedef struct Sw{
// status info
   char port[5];
   char media[7];
   char link[5];
// interface info
   int rx;
   int rx_error;
   int rx_broad;
   int tx;
   int tx_error;
   int collision;
// mac info
   char vlan[2];
   int nmacs;
   MAC mac[30];
   int nmacs_expected;
   MAC mac_expected[30];
}SW;

SW sw[12];

int main(int argc,char *argv[]){
  char thisip[18];
  if(argc!=2){
    std::cout << " usage: ./SwitchPerlClearMacs <mac address> " << std::endl;
    return 0;
  }
  for(int i=0;i<18;i++)thisip[i]=argv[1][i];

 // initialization
  for(int prt=0;prt<12;prt++){
    sw[prt].nmacs=0;
  }

  fill_switch_macs(thisip);
  print_switch_configurations(thisip);

  return 0;
}


void fill_switch_macs(char *thisip){
    char command[556];
    sprintf(command,"$BUILD_HOME/emu/emuDCS/Switch/bin/switch_telnet.pl %s mac-addr-table > /tmp/switch_mac.dat \n",thisip);
    int ierr=system(command);
    parse_mac();
    ierr=system("rm /tmp/switch_mac.dat");
}

void parse_mac(){
  char line[128];
  FILE *file;
  char mac[19];
  char port[3];
  int prt;
  char status[8];
  int i,j,k,n;
  file=fopen("/tmp/switch_mac.dat","r");
  while(fgets(line,128,file)!=NULL){
    if(line[2]==':'){
      fill_char(mac,line,0,17);
      fill_char(port,line,30,3);
      sscanf(port,"%d",&prt);
      fill_char(sw[prt-1].vlan,line,39,1);
      fill_char(status,line,48,10);
      // printf(" %s %s %s \n",mac,port,status);
      n=sw[prt-1].nmacs;
      if(sw[prt-1].mac[n].mac==NULL)sw[prt-1].mac[n].mac=(char *)malloc(19);
      if(n<30&&prt<13){
        for(k=0;k<19;k++)sw[prt-1].mac[n].mac[k]=mac[k];
        for(k=0;k<8;k++)sw[prt-1].mac[n].status[k]=status[k];
        sw[prt-1].nmacs=n+1;
      }
    }
    line[2]=0;
  }
  fclose(file);
}


void fill_char(char *var,char *line,int first,int length)
{
  int i;
  for(i=first;i<first+length;i++){
    var[i-first]=line[i];
  }
  var[length]='\0';
}

void print_switch_configurations(char *thisip){
  std::cout << " enter print_switch_configurations() " << thisip << std::endl; 
 
  std::ofstream f;
  char fname[14];
  sprintf(fname,"SwitchZero.pl");
  f.open(fname);
 
  // instantiate and login
  f << header2(thisip);
  f << login();

  // interface commands
  f << enable();
  f << configure();
  for(int prt=0;prt<12;prt++){
    char port[5];
    sprintf(port,"0/%d",prt+1);
    int tvlan;
    int n=sw[prt].nmacs;
    for(int i=0;i<n;i++){
      char *cvlan=sw[prt].vlan;
      sscanf(cvlan,"%d",&tvlan);
      char *mac=sw[prt].mac[i].mac;
      // std::cout << swt << " " <<  prt+1 << " " << port <<" " << mac <<" vlan " << tvlan << std::endl;
      f << interface(port);
      f << portsecuritymacaddress(false,port,mac,tvlan);
      f << exitinterface();
    }
  }
  f << exitconfig();
  f << exitenable();
  // show macs
  f << macaddrtable();

  //logout
  f << logout();

  f.close();
}
