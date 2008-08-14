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

void fill_expected_mac_table();
int compare(char *a,char *b,int begin,int length);
void dump_expected_macs(int ioff);
void print_switch_configurations(int swt);

  // switch stuff
 int swadd;
 int swadd2;
 int init;

 typedef struct side{
  char *name;
  int nswitch;
  int nport;
  int slot;
 }SIDE;

 SIDE side[33];

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

SW sw[4][12];
SW old[4][12];
char *ip[8];

//pc stuff
typedef struct pc_eth{
  long int rx_bytes;
  long int rx;
  long int rx_err;
  long int rx_drop;
  long int rx_fifo;
  long int rx_frame;
  long int rx_compressed;
  long int rx_multicast;
  long int tx_bytes;
  long int tx;
  long int tx_err;
  long int tx_drop;
  long int tx_fifo;
  long int tx_colls;
  long int tx_carrier;
  long int tx_compressed;
}PC_ETH;

PC_ETH eth[2];
PC_ETH ethold[2];

typedef struct pc_stats{
  char machine[12];
  char eth[5];
}PC_STATS;

PC_STATS pc[2];
SIDE plus[33]={
  {"csc-pc1    ",4,6,0},
  {"csc-dcs-pc1",4,8,0},
  {"csc-pc1b   ",4,7,0},
  {"vme+1/1    ",3,2,0},			
  {"vme+1/2    ",1,7,0},			
  {"vme+1/3    ",1,8,0},			
  {"vme+1/4    ",2,7,0},			
  {"vme+1/5    ",2,8,0},	
  {"vme+1/6    ",3,7,0},			
  {"vme+1/7    ",3,8,0},			
  {"vme+1/8    ",1,1,0},			
  {"vme+1/9    ",1,2,0},			
  {"vme+1/10   ",2,1,0},			
  {"vme+1/11   ",2,2,0},			
  {"vme+1/12   ",3,1,0},			
  {"vme+2/1    ",3,3,0},			
  {"vme+2/2    ",1,9,0},			
  {"vme+2/3    ",2,9,0},			
  {"vme+2/4    ",3,9,0},			
  {"vme+2/5    ",1,3,0},			
  {"vme+2/6    ",2,3,0},			
  {"vme+3/1    ",2,4,0},			
  {"vme+3/2    ",3,4,0},			
  {"vme+3/3    ",1,10,0},			
  {"vme+3/4    ",2,10,0},			
  {"vme+3/5    ",3,10,0},			
  {"vme+3/6    ",1,4,0},			
  {"vme+4/1    ",1,5,0},			
  {"vme+4/2    ",2,5,0},			
  {"vme+4/3    ",3,5,0},			
  {"vme+4/4    ",1,11,0},			
  {"vme+4/5    ",2,11,0},			
  {"vme+4/6    ",3,11,0}
};	

SIDE minus[33]={
  {"csc-pc2    ",7,6,0},
  {"csc-dcs-pc2",7,8,0},
  {"csc-pc2b   ",7,7,0},
  {"vme-1/1    ",10,2,0},			
  {"vme-1/2    ",8,7,0},			
  {"vme-1/3    ",8,8,0},			
  {"vme-1/4    ",9,7,0},			
  {"vme-1/5    ",9,8,0},			
  {"vme-1/6    ",10,7,0},			
  {"vme-1/7    ",10,8,0},			
  {"vme-1/8    ",8,1,0},			
  {"vme-1/9    ",8,2,0},			
  {"vme-1/10   ",9,1,0},			
  {"vme-1/11   ",9,2,0},			
  {"vme-1/12   ",10,1,0},			
  {"vme-2/1    ",10,3,0},			
  {"vme-2/2    ",8,9,0},			
  {"vme-2/3    ",9,9,0},			
  {"vme-2/4    ",10,9,0},			
  {"vme-2/5    ",8,3,0},			
  {"vme-2/6    ",9,3,0},			
  {"vme-3/1    ",9,4,0},			
  {"vme-3/2    ",10,4,0},			
  {"vme-3/3    ",8,10,0},			
  {"vme-3/4    ",9,10,0},			
  {"vme-3/5    ",10,10,0},			
  {"vme-3/6    ",8,4,0},			
  {"vme-4/1    ",8,5,0},			
  {"vme-4/2    ",9,5,0},			
  {"vme-4/3    ",10,5,0},			
  {"vme-4/4    ",8,11,0},			
  {"vme-4/5    ",9,11,0},			
  {"vme-4/6    ",10,11,0}
};


char* tip[8]={
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20"
};

/* 
//cern switch addresses
char* tip[8]={
 "192.168.10.101","192.168.10.102",
 "192.168.10.103","192.168.10.104",
 "192.168.10.107","192.168.10.108",
 "192.168.10.109","192.168.10.110"
};
*/
  PC_STATS pc_minus[2]={
   {"cmspc00","eth1"},
   {"cmspc00","eth1"}
   };
  PC_STATS pc_plus[2]={
   {"cmspc00","eth1"},
   {"cmspc00","eth1"}
   };

/*
PC_STATS pc_plus[2]={
 {"csc-pc1","eth2"},
  {"csc-dcs-pc1","eth2"}
};

PC_STATS pc_minus[2]={
  {"csc-pc2","eth2"},
  {"csc-dcs-pc2","eth2"}
};
*/

int sidevlan[4][12]; 

int plusvlan[4][12]={
  {1,1,1,1,1,1,2,2,2,2,2,2},
  {1,1,1,1,1,1,2,2,2,2,2,2},
  {1,1,1,1,1,1,2,2,2,2,2,2},
  {1,1,1,1,1,1,1,1,1,1,1,1}
};

int minusvlan[4][12]={
  {1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,2,2,2,2,2,2},
  {1,1,1,1,1,1,2,2,2,2,2,2},
  {1,1,1,1,1,1,2,2,2,2,2,2}
};

int main(){

  char *Side;

  Side="plus";
  if(Side=="plus"){
     for(int i=0;i<33;i++)side[i]=plus[i];
     for(int i=0;i<4;i++){
       for(int j=0;j<12;j++)sidevlan[i][j]=plusvlan[i][j];
     }
     swadd=0;swadd2=0;
     for(int i=0;i<2;i++)pc[i]=pc_plus[i];
  }
  fill_expected_mac_table();
  print_switch_configurations(1);
  print_switch_configurations(2);
  print_switch_configurations(3);
  print_switch_configurations(4);

  Side="minus";
  if(Side=="minus"){
     for(int i=0;i<33;i++)side[i]=minus[i];
     for(int i=0;i<4;i++){
       for(int j=0;j<12;j++)sidevlan[i][j]=minusvlan[i][j];
     }

     swadd=4;swadd2=6;
     for(int i=0;i<2;i++)pc[i]=pc_minus[i];
  }

  fill_expected_mac_table();
  print_switch_configurations(1);
  print_switch_configurations(2);
  print_switch_configurations(3);
  print_switch_configurations(4);

  return 0;
}

void fill_expected_mac_table(){

  typedef struct expected_macs{
    char *name;
    char *mac;
  }EXPECTED_MACS;
#include "../../include/expected_macs_plus.h"
#include "../../include/expected_macs_minus.h"
  EXPECTED_MACS em[37];  
  if(swadd==0)for(int i=0;i<37;i++)em[i]=expplus[i];
  if(swadd==4)for(int i=0;i<37;i++)em[i]=expminus[i];
   
  // initialization
  for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      sw[swt][prt].nmacs_expected=0;
    }
  }
  for(int i=0;i<33;i++){
    int swt=side[i].nswitch-1-swadd2;
    int prt=side[i].nport-1;
    int n=sw[swt][prt].nmacs_expected;
    sw[swt][prt].mac_expected[n].mac=em[i].mac;
    n=n+1;
    sw[swt][prt].nmacs_expected=n;
  }
  // plus side first
  if(swadd==0){ 
    // switch pointers
    int n=sw[3][1].nmacs_expected;
    sw[3][1].mac_expected[n].mac=em[33].mac;
    n=n+1;
    sw[3][1].nmacs_expected=n;
    n=sw[3][2].nmacs_expected;
    sw[3][2].mac_expected[n].mac=em[34].mac;
    n=n+1;
    sw[3][2].nmacs_expected=n;
    n=sw[3][3].nmacs_expected;
    sw[3][3].mac_expected[n].mac=em[35].mac;
    n=n+1;
    sw[3][3].nmacs_expected=n;
    
    int tprt[2]={5,11};
    for(int swt=0;swt<3;swt++){
      // fill in pc macs
      for(int i=0;i<2;i++){
        int n=sw[swt][tprt[i]].nmacs_expected; 
        sw[swt][tprt[i]].mac_expected[n].mac=em[36].mac;  // switch 4
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[0].mac;  // pc 1
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[1].mac;  // pc 2
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[2].mac;  // pc 1b                                                      
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
 
      }
    // mac pointers to switchs 1-3
      for(int prt=0;prt<5;prt++){
        int n=sw[3][swt+1].nmacs_expected; 
        sw[3][swt+1].mac_expected[n].mac=sw[swt][prt].mac_expected[0].mac;  
        n=n+1;
        sw[3][swt+1].nmacs_expected=n;
      }
      for(int prt=6;prt<11;prt++){
        int n=sw[3][swt+8].nmacs_expected; 
        sw[3][swt+8].mac_expected[n].mac=sw[swt][prt].mac_expected[0].mac;  
        n=n+1;
        sw[3][swt+8].nmacs_expected=n;
      }

    }
    dump_expected_macs(0);   
  } 
  // minus side next  
  if(swadd==4){ 
    // switch pointers
    int n=sw[0][1].nmacs_expected;
    sw[0][1].mac_expected[n].mac=em[34].mac;
    n=n+1;
    sw[0][1].nmacs_expected=n;
    n=sw[0][2].nmacs_expected;
    sw[0][2].mac_expected[n].mac=em[35].mac;
    n=n+1;
    sw[0][2].nmacs_expected=n;
    n=sw[0][3].nmacs_expected;
    sw[0][3].mac_expected[n].mac=em[36].mac;
    n=n+1;
    sw[0][3].nmacs_expected=n; 

    int tprt[2]={5,11};
    for(int swt=1;swt<4;swt++){
      // fill in pc macs
      for(int i=0;i<2;i++){
        int n=sw[swt][tprt[i]].nmacs_expected; 
        sw[swt][tprt[i]].mac_expected[n].mac=em[33].mac;  // switch 4
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[0].mac;  // pc 1
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[1].mac;  // pc 2
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
        sw[swt][tprt[i]].mac_expected[n].mac=em[2].mac;  // pc 2b                                                      
        n=n+1;
        sw[swt][tprt[i]].nmacs_expected=n;
      }
    // mac pointers to switchs 1-3
      for(int prt=0;prt<5;prt++){
        int n=sw[0][swt].nmacs_expected; 
        sw[0][swt].mac_expected[n].mac=sw[swt][prt].mac_expected[0].mac;  
        n=n+1;
        sw[0][swt].nmacs_expected=n;
      }
      for(int prt=6;prt<11;prt++){
        int n=sw[0][swt+7].nmacs_expected; 
        sw[0][swt+7].mac_expected[n].mac=sw[swt][prt].mac_expected[0].mac;  
        n=n+1;
        sw[0][swt+7].nmacs_expected=n;
      }

    }
    dump_expected_macs(6);   
  } 
   
} 

int compare(char *a,char *b,int begin,int length){
  int j=0;
  for(int i=begin;i<begin+length;i++){
    if(a[i]==b[i])j=j+1;
  }
  return j;
}

void dump_expected_macs(int ioff){
  std::cout << " Expected Switch Addresses " << std::endl;
  for(int swt=0;swt<4;swt++){
    std::cout << "Switch" <<swt+1+ioff<<std::endl;
    for(int prt=0;prt<12;prt++){
      int n=sw[swt][prt].nmacs_expected;
      for(int m=0;m<n;m++){
	//std::cout << swt+1 << " " << prt+1 << " " << sw[swt][prt].nmacs_expected << std::endl;
	std::cout << swt+1 << " " << prt+1 << " " << sw[swt][prt].mac_expected[m].mac << std::endl;
      }
    }
  }
}


void print_switch_configurations(int swt)
{
  std::cout << " enter print_switch_configurations() " << std::endl; 

  std::ofstream f;
  char fname[11];
  sprintf(fname,"Switch%d.pl",swt+swadd2);
  f.open(fname);

  // instantiate and login
  f << header();
  f << login();

  // vlan commands
  f << enable();
  f << vlandatabase();
  f << vlanid(2);
  f << vlanname(2,"top");
  f << vlanname(1,"bottom");
  f << vlanmakestatic(2);
  f << exitvlan();
  f << exitenable();

  // configure commands
  f << enable();
  f << configure();
  f << logginghost();
  f << loggingsyslog();
  f << spanningtree(false);
  f << portsecurity(true);
  f << snmptrapslinkstatus(false);
  f << snmpservertrapslinkmode(false);
  f << mtu(10240);
  // f << portchanneladminmodeall(false);
  // f << portchannellinktrapall(false);
  f << stormcontrolbroadcast(false);
  f << stormcontrolflowcontrol(false);
  f << exitconfig();
  f << exitenable();

  // interface commands
  f << enable();
  f << configure();
  for(int prt=0;prt<12;prt++){
    char port[5];
    sprintf(port,"0/%d",prt+1);
    int tvlan=sidevlan[swt-1][prt];
    int n=sw[swt-1][prt].nmacs_expected;
    for(int i=0;i<n;i++){
      char *mac=sw[swt-1][prt].mac_expected[i].mac;
      f << interface(port);
      // f << adminmode(true,port);
      f << portlacpmode(false,port);
      f << autonegotiate(true,port);
      // f << speed(port,1000);
      f << vlan(port,tvlan);
      f << vlaningressfilter(false,port);
      f << vlanparticipation(port,"exclude",1);
      f << vlanparticipation(port,"include",tvlan);
      f << vlanacceptframeall(true,port);
      f << vlanpriority(port,7);
      // f << portchannelstaticenable(true,port);
      f << portsecuritymaxdynamic(port,20);
      f << portsecuritymaxsstatic(port,10);
      // std::cout << swt << " " <<  prt+1 << " " << port <<" " << mac <<" vlan " << tvlan << std::endl;
      f << portsecuritymacaddress(true,port,mac,tvlan);
      f << exitinterface();
    }
  }
  f << exitconfig();
  f << exitenable();

  //logout
  f << logout();

  f.close();
}
