#ifndef _SwitchGUI_h_
#define _SwitchGUI_h_

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"

#include "toolbox/net/Utils.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/FormFile.h"

#define MAX_MACS 50

using namespace cgicc;
using namespace std;

class SwitchGUI: public xdaq::Application
{
 public:
  XDAQ_INSTANTIATOR();

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
  char status[24];
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
   MAC mac[MAX_MACS];
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



SwitchGUI(xdaq::ApplicationStub * s)throw (xdaq::exception::Exception): xdaq::Application(s) 
{	

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

/*
 //osu test
char* tip[8]={
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20",
 "192.168.10.20","192.168.10.20"
};
*/

//cern switch addresses
char* tip[8]={
 "192.168.10.101","192.168.10.102",
 "192.168.10.103","192.168.10.104",
 "192.168.10.107","192.168.10.108",
 "192.168.10.109","192.168.10.110"
};

/*
 PC_STATS pc_plus[2]={
  {"cmspc00","eth1"},
  {"cmspc00","eth1"}
 };

  PC_STATS pc_minus[2]={
   {"cmspc00","eth1"},
   {"cmspc00","eth1"}
   };
*/

PC_STATS pc_plus[2]={
 {"csc-pc1","eth2"},
  {"csc-pc1","eth2"}
};

PC_STATS pc_minus[2]={
  {"csc-pc2","eth2"},
  {"csc-pc2","eth2"}
};


  xgi::bind(this,&SwitchGUI::Default, "Default");
  xgi::bind(this,&SwitchGUI::MainPage, "MainPage");

  xgi::bind(this,&SwitchGUI::GotoMain, "GotoMain");
  xgi::bind(this,&SwitchGUI::GotoMacGUI,"GotoMacGUI");
  xgi::bind(this,&SwitchGUI::Maintenance, "Maintenance");
  xgi::bind(this,&SwitchGUI::ResetSwitch, "ResetSwitch");
  xgi::bind(this,&SwitchGUI::MacGUI, "MacGUI");
  xgi::bind(this,&SwitchGUI::CLRcounters,"CLRcounters");
  xgi::bind(this,&SwitchGUI::ProblemsGUI,"ProblemsGUI");

  int ierr=system("cp $BUILD_HOME/emu/emuDCS/PCSwitches/img/mainemus.jpg /tmp/mainemus.jpg");
      ierr=system("cp $BUILD_HOME/emu/emuDCS/PCSwitches/img/osulogo.jpg /tmp/osulogo.jpg");

  std::cout << "Hostname is: " << toolbox::net::getHostName() << std::endl;
  std::cout << "Domainname is: " << toolbox::net::getDomainName() << std::endl;
  std::string pcname=toolbox::net::getHostName();
  std::string pcplus="vmepcS2G18-09.cms";
  std::string pcminus="vmepcS2G18-08.cms";
  std::cout << "hostname " << pcname << "plusname " << pcplus << "minusname " << pcminus << std::endl;
  char *Side;
  if(pcname==pcplus)Side="plus";
  if(pcname==pcminus)Side="minus";
  
  std::cout << " Choosing " << Side << " side chambers " << std::endl;
  init=0;
  if(Side=="plus"){
     for(int i=0;i<33;i++)side[i]=plus[i];
     swadd=0;swadd2=0;
     for(int i=0;i<2;i++)pc[i]=pc_plus[i];
  }
  if(Side=="minus"){
     for(int i=0;i<33;i++)side[i]=minus[i];
     swadd=4;swadd2=6;
     for(int i=0;i<2;i++)pc[i]=pc_minus[i];
  }
  for(int i=0;i<8;i++)ip[i]=tip[i];

  fill_expected_mac_table();

  printf(" leave instantiation \n");
}  

void Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}

void MainPage(xgi::Input * in, xgi::Output * out ) {
  


  printf(" initialize \n");
  // initialization
  for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      sw[swt][prt].nmacs=0;
    }
  }


  // fill statistic
  fill_pc_statistics();
  fill_switch_statistics();
  fill_switch_macs();

  if(init==0){
    for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      old[swt][prt]=sw[swt][prt];
    }
    }
    for(int i=0;i<2;i++)ethold[i]=eth[i];
    init=1;
  }


   *out<<Header("VME Gigabit Switch Statistics",false);
   
   *out << cgicc::table();
   *out << cgicc::tr();
   *out << cgicc::td();
   std::string GotoMain = toolbox::toString("/%s/GotoMain",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMain) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Refresh Page") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string CLRcounters = toolbox::toString("/%s/CLRcounters",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",CLRcounters) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","CLR Counters") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string Maintenance = toolbox::toString("/%s/Maintenance",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",Maintenance) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Maintenance") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string MacGUI = toolbox::toString("/%s/MacGUI",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",MacGUI) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Check Mac Table") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string ProblemsGUI = toolbox::toString("/%s/ProblemsGUI",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",ProblemsGUI) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Ethernet Faults") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::tr() << std::endl;
   *out << cgicc::table() << std::endl;
   *out << cgicc::hr() << std::endl;
  
   char buf[40];

   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"PC Gigabit Card Statistics");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","left") << cgicc::p() << std::endl ;
   *out << html_pc_status();
   *out << cgicc::fieldset() << endl;
   //*out << cgicc::table() << endl;


   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"Switch Port Statistics");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","left") << cgicc::p() << std::endl ;
   *out << html_port_status();
   *out << cgicc::fieldset() << endl;



    for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      old[swt][prt]=sw[swt][prt];
    }
    }
    for(int i=0;i<2;i++)ethold[i]=eth[i];

}

 void GotoMain(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);
   this->Default(in,out);
 }

 void ResetSwitch(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);
   int morder[4]={0,1,2,3};
   int porder[4]={1,2,3,0};
   int order[4];
   if(swadd==0)for(int i=0;i<4;i++)order[i]=morder[i];
   if(swadd==4)for(int i=0;i<4;i++)order[i]=porder[i];
   for(int i=0;i<4;i++){
     int swt=order[i];
     printf(" swt %d \n",order[i]+swadd2);
     char command[556];
     sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s reload",ip[swt+swadd]);
     printf("%d  %s \n",swt,command);
     system(command);
   }
   ::sleep(40);
   this->Default(in,out);
 }

  void CLRcounters(xgi::Input * in, xgi::Output * out )                                                                  throw (xgi::exception::Exception) {
    cgicc::Cgicc cgi(in);
    for(int swt=0;swt<4;swt++){
      char command[556];
      sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s clrcounters",ip[swt+swadd]);
      printf("%d  %s \n",swt,command);
      system(command);
    }
    this->Default(in,out);
  }


 void Maintenance(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);
      *out<<Header("VME Gigabit Switch Maintenance",false);
   
   *out << cgicc::table();
   *out << cgicc::tr();
   *out << cgicc::td();
   std::string GotoMain = toolbox::toString("/%s/GotoMain",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMain) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Go to Main") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   char *buf;
   if(swadd==0)buf="Reload Plus Switches";
   if(swadd==4)buf="Reload Minus Switches";
   *out << cgicc::td();
   std::string ResetSwitch = toolbox::toString("/%s/ResetSwitch",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",ResetSwitch) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::tr() << std::endl;
   *out << cgicc::table() << std::endl;
   *out << cgicc::hr() << std::endl;
   *out<<Footer()<<endl;

 }

 void MacGUI(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);

  printf(" initialize \n");
  // initialization
  for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      sw[swt][prt].nmacs=0;
    }
  }


   fill_switch_macs();

   *out<<Header("VME Gigabit Switch MAC Tables",false);
   
   *out << cgicc::table();
   *out << cgicc::tr();
   *out << cgicc::td();
   std::string GotoMain = toolbox::toString("/%s/GotoMain",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMain) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Go to Main") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string Maintenance = toolbox::toString("/%s/Maintenance",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",Maintenance) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Maintenance") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string GotoMacGUI = toolbox::toString("/%s/GotoMacGUI",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMacGUI) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Refresh Page") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::tr() << std::endl;
   *out << cgicc::table() << std::endl;
   *out << cgicc::hr() << std::endl;
   char buf[40];
   sprintf(buf,"Switch Mac Addresses");
   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","left") << cgicc::p() << std::endl ;
   *out << html_mac_table();
   *out << cgicc::fieldset() << endl;
   *out<<Footer()<<endl;

 }
 
 void GotoMacGUI(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);
   this->MacGUI(in,out);
 }

 void ProblemsGUI(xgi::Input * in, xgi::Output * out )
   throw (xgi::exception::Exception) {
   cgicc::Cgicc cgi(in);
   *out<<Header("VME Problem Tables",false);
   
   *out << cgicc::table();
   *out << cgicc::tr();
   *out << cgicc::td();
   std::string GotoMain = toolbox::toString("/%s/GotoMain",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMain) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Go to Main") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string Maintenance = toolbox::toString("/%s/Maintenance",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",Maintenance) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Maintenance") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::td();
   std::string GotoMacGUI = toolbox::toString("/%s/GotoMacGUI",getApplicationDescriptor()->getURN().c_str());
   *out << cgicc::form().set("method","GET").set("action",GotoMacGUI) << std::endl ;
   *out << cgicc::input().set("type","submit").set("value","Check MAC Tables") << std::endl ;
   *out << cgicc::form() << std::endl;
   *out << cgicc::td() << std::endl;
   *out << cgicc::tr() << std::endl;
   *out << cgicc::table() << std::endl;
   *out << cgicc::hr() << std::endl;
   for(int swt=0;swt<4;swt++){
     char buf[40];
     sprintf(buf,"Switch %d Problems",swt+swadd2+1);
     std::cout << buf << std::endl;
     *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
     *out << cgicc::legend(buf).set("style","color:blue").set("align","left") << cgicc::p() << std::endl ;
     *out << html_parse_problems(swt+swadd2+1);
     *out << cgicc::fieldset() << endl;
   }
   *out<<Footer()<<endl;
 }


void parse_status(int swtch,int prt){
  char line[128];
  FILE *file;
  int i;
  file=fopen("/tmp/switch_status.dat","r");
  for(i=0;i<6;i++){
    fgets(line,128,file);
    // printf(" %s \n",line);
  }
  fill_char(sw[swtch][prt].port,line,0,4);
  //printf(" port %s \n",sw[swtch][prt].port);
  fill_char(sw[swtch][prt].media,line,8,6);
  //printf(" media %s \n",sw[swtch][prt].media);
  fill_char(sw[swtch][prt].link,line,49,4);
  //printf(" link %s \n",sw[swtch][prt].link); 
  fclose(file);
}

void parse_status_all(int swtch){
  char line[128];
  FILE *file;
  int prt;
  char port[3];
  int i;
  file=fopen("/tmp/switch_status.dat","r");
  for(i=0;i<17;i++){
    fgets(line,128,file);
    // printf(" %s \n",line);
    if(i>=5){
      fill_char(port,line,2,2);
      sscanf(port,"%d",&prt);
      prt=prt-1;
      fill_char(sw[swtch][prt].port,line,0,4);
      //printf(" port %s \n",sw[swtch][prt].port);
      fill_char(sw[swtch][prt].media,line,8,6);
      //printf(" media %s \n",sw[swtch][prt].media);
      fill_char(sw[swtch][prt].link,line,49,4);
      //printf(" link %s \n",sw[swtch][prt].link);
    }
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

void parse_interface(int swtch,int prt){
  char line[128];
  FILE *file;
  int i;
  char tmp[10];
  file=fopen("/tmp/switch_interface.dat","r");
  for(i=0;i<9;i++){
    fgets(line,128,file);
    // printf(" %s \n",line);
    fill_char(tmp,line,47,15);
    if(i==3){
         sscanf(tmp,"%d",&sw[swtch][prt].rx);
         //printf(" rx %d \n",sw[swtch][prt].rx);
    }
    if(i==4){
         sscanf(tmp,"%d",&sw[swtch][prt].rx_error);
         //printf(" rx_error %d \n",sw[swtch][prt].rx_error);
    }
    if(i==5){
         sscanf(tmp,"%d",&sw[swtch][prt].rx_broad);
         //printf(" rx_broad %d \n",sw[swtch][prt].rx_broad);
    }
    if(i==6){
         sscanf(tmp,"%d",&sw[swtch][prt].tx);
         //printf(" tx %d \n",sw[swtch][prt].tx);
    }
    if(i==7){
         sscanf(tmp,"%d",&sw[swtch][prt].tx_error);
         //printf(" tx_error %d \n",sw[swtch][prt].tx_error);
    }
    if(i==8){
         sscanf(tmp,"%d",&sw[swtch][prt].collision);
	 // printf(" collision %d \n",sw[swtch][prt].collision);
    }
  }
  fclose(file);
}
void parse_interface_all(int swtch){
  char line[128];
  FILE *file;
  char port[3];
  int i,j,prt;
  char tmp[10];
  file=fopen("/tmp/switch_interface.dat","r");
  for(j=0;j<12;j++){
  for(i=0;i<11;i++){
    fgets(line,128,file);
    // printf(" %s \n",line);
    if(i==0){
      fill_char(port,line,28,2);
      sscanf(port,"%d",&prt);
      prt=prt-1;
      if(prt!=j)printf(" ***************** Hugh port problem \n");
    }
    fill_char(tmp,line,47,15);
    if(i==3){
         sscanf(tmp,"%d",&sw[swtch][prt].rx);
         //printf(" rx %d \n",sw[swtch][prt].rx);
    }
    if(i==4){
         sscanf(tmp,"%d",&sw[swtch][prt].rx_error);
         //printf(" rx_error %d \n",sw[swtch][prt].rx_error);
    }
    if(i==5){
         sscanf(tmp,"%d",&sw[swtch][prt].rx_broad);
         //printf(" rx_broad %d \n",sw[swtch][prt].rx_broad);
    }
    if(i==6){
         sscanf(tmp,"%d",&sw[swtch][prt].tx);
         //printf(" tx %d \n",sw[swtch][prt].tx);
    }
    if(i==7){
         sscanf(tmp,"%d",&sw[swtch][prt].tx_error);
         //printf(" tx_error %d \n",sw[swtch][prt].tx_error);
    }
    if(i==8){
         sscanf(tmp,"%d",&sw[swtch][prt].collision);
         //printf(" collision %d \n",sw[swtch][prt].collision);
    }
  }
  }
  fclose(file);
}

void parse_mac(int swtch){
  char line[128];
  FILE *file;
  char mac[19];
  char port[3];
  int prt;
  char status[8];
  int k,n;
  file=fopen("/tmp/switch_mac.dat","r");
  while(fgets(line,128,file)!=NULL){
    if(line[2]==':'){
      fill_char(mac,line,0,17);
      fill_char(port,line,30,3);
      sscanf(port,"%d",&prt);
      fill_char(sw[swtch][prt-1].vlan,line,39,1);
      fill_char(status,line,48,10);
      // printf(" %s %s %s \n",mac,port,status);
      n=sw[swtch][prt-1].nmacs;
      if(sw[swtch][prt-1].mac[n].mac==NULL)sw[swtch][prt-1].mac[n].mac=(char *)malloc(19);
      if(n<MAX_MACS&&prt<13){
        for(k=0;k<19;k++)sw[swtch][prt-1].mac[n].mac[k]=mac[k];
        for(k=0;k<8;k++)sw[swtch][prt-1].mac[n].status[k]=status[k];
        sw[swtch][prt-1].nmacs=n+1;
      }
    }
    line[2]=0;
  }
  fclose(file);
}

std::string html_port_status(){
  char strbuf[500];
  int crate,swt,prt,slt;
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>crate</th><th>switch</th><th>port</th><th>link</th><th>rx</th><th>rx_error</th><th>tx</th><th>tx_error</th></tr> \n </tr> \n <tbody> \n");
  rtns=rtns+strbuf;
  for(crate=0;crate<33;crate++){
    swt=side[crate].nswitch-swadd2;
    prt=side[crate].nport;
    slt=side[crate].slot;
    sprintf(strbuf,"<tr><td>%s</td><td>%d</td><td>%d/%d</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",side[crate].name,swt+swadd2,slt,prt,sw[swt-1][prt-1].link,sw[swt-1][prt-1].rx,sw[swt-1][prt-1].rx_error,sw[swt-1][prt-1].tx,sw[swt-1][prt-1].tx_error);
    rtns=rtns+strbuf;
    sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",sw[swt-1][prt-1].rx-old[swt-1][prt-1].rx,sw[swt-1][prt-1].rx_error-old[swt-1][prt-1].rx_error,sw[swt-1][prt-1].tx-old[swt-1][prt-1].tx,sw[swt-1][prt-1].tx_error-old[swt-1][prt-1].tx_error);
    rtns=rtns+strbuf;

  }
  sprintf(strbuf,"</tbody> \n </table> \n");
  rtns=rtns+strbuf;
  return rtns;
}

/* std::string html_mac_table()
{
  char strbuf[500];
  int i,n;
  int crate,swt,prt,slt;
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>crate</th><th>switch</th><th>port</th><th> vlan </th><th>mac</th><th>status</th></tr> \n </tr> \n <tbody> \n");
  rtns=rtns+strbuf;
  for(crate=0;crate<33;crate++){
    swt=side[crate].nswitch-swadd2;
    prt=side[crate].nport;
    slt=side[crate].slot;
    n=sw[swt-1][prt-1].nmacs;
    for(i=0;i<n;i++){
      if(i==0)sprintf(strbuf,"<tr><td>%s</td><td>%d</td><td>%d/%d</td><td>%s</td><td>%s</td><td>%s</td></tr> \n",side[crate].name,swt+swadd2,slt,prt,sw[swt-1][prt-1].vlan,sw[swt-1][prt-1].mac[i].mac,sw[swt-1][prt-1].mac[i].status);
      if(i>0)sprintf(strbuf,"<tr><td> </td><td></td><td></td><td></td><td>%s</td><td>%s</td></tr> \n",sw[swt-1][prt-1].mac[i].mac,sw[swt-1][prt-1].mac[i].status);
      rtns=rtns+strbuf;
    }
  }
  sprintf(strbuf,"</tbody> \n </table> \n");
  rtns=rtns+strbuf;
  return rtns;
}
 */

std::string html_mac_table()
{
  char strbuf[500];
  std::cout << " enter mac " << std::endl;
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>switch</th><th>port</th><th>vlan</th><th>crate</th><th>mac</th><th>status</th></tr> \n </tr> \n </thead> \n <tbody> \n");
  rtns=rtns+strbuf;
  char *colvlan;
  char *colmac;
  char *colstat;
  for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      char *chmbr="";
      for(int i=0;i<33;i++){
        int s=side[i].nswitch;
        int p=side[i].nport;
        if(s-swadd2==swt+1&&p==prt+1)chmbr=side[i].name;
      }
      colvlan="";colmac=" bgcolor=\"red\"";colstat="";
      int ivlan;
      sscanf(sw[swt][prt].vlan,"%d",&ivlan);
      if(swadd==0){
	if(swt+1==4){
          if(ivlan!=1)colvlan=" bgcolor=\"red\"";
        }else{
          if(prt+1<=6&&ivlan!=1)colvlan=" bgcolor=\"red\"";
          if(prt+1>6&&ivlan!=2)colvlan=" bgcolor=\"red\"";
        }
      } 
      int first=0;
      int n=sw[swt][prt].nmacs;
      for(int i=0;i<n;i++){
        char val[7]="Static";
        colstat=0;
        if(compare(sw[swt][prt].mac[i].status,val,0,6)!=6)colstat=" bgcolor=\"red\"";
        int ne=sw[swt][prt].nmacs_expected;
        for(int j=0;j<ne;j++){
          if(compare(sw[swt][prt].mac[i].mac,sw[swt][prt].mac_expected[j].mac,0,17)==17)colmac="";  
        } 
        int slt=0;
        if(i==0)first=1;
	if(i==0)sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>%s</td></tr> \n",swt+swadd2+1,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr,colmac,sw[swt][prt].mac[i].mac,colstat,sw[swt][prt].mac[i].status);
        if(i!=0)sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td %s>%s</td><td %s>%s</td></tr> \n",colmac,sw[swt][prt].mac[i].mac,colstat,sw[swt][prt].mac[i].status);
        rtns=rtns+strbuf;
      }  
      colvlan=" bgcolor=\"orange\"";colmac=" bgcolor=\"orange\"";colstat=" bgcolor=\"orange\"";
      int ne=sw[swt][prt].nmacs_expected;   
      for(int i=0;i<ne;i++){
        int ipass=0;
        int n=sw[swt][prt].nmacs;
        for(int m=0;m<n;m++){
	  if(compare(sw[swt][prt].mac[m].mac,sw[swt][prt].mac_expected[i].mac,0,17)==17)ipass=1;
        }
        if(ipass==0){
          int slt=0;
          if(first==0)sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>Missing</td></tr> \n",swt+swadd2+1,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr,colmac,sw[swt][prt].mac_expected[i].mac,colstat);
          if(first==1)sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td %s>%s</td><td %s>Missing</td></tr> \n",colmac,sw[swt][prt].mac_expected[i].mac,colstat);
          first=1;
          rtns=rtns+strbuf;
        }
      }
    }
  }
  sprintf(strbuf,"</tbody> \n </table> \n");
  rtns=rtns+strbuf;
  return rtns;

}
 
/* modifier Phillip Killewald HTML Stuff */

     virtual string Header(string myTitle,bool reload=true) {
		ostringstream *out = new ostringstream();

		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
		*out << "<html>" << endl;
		*out << cgicc::head() << endl;
		*out << CSS();
		*out << cgicc::title(myTitle) << endl;


		*out << cgicc::head() << endl;

		*out << "<body background=\"/tmp/osulogo.jpg\">" << endl;

		*out << cgicc::fieldset()
			.set("class","header") << endl;

		*out << cgicc::img()
			.set("src","/tmp/mainemus.jpg")
			.set("style","float: right; height: 100px") << endl;

		*out << cgicc::div(myTitle)
			.set("class","title") << endl;

		*out << cgicc::div()
			.set("class","expert") << endl;
		*out << cgicc::span("Experts ")
			.set("style","font-weight: bold") << endl;
		*out << cgicc::a("Stan Durkin")
			.set("href","mailto:durkin@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Ben Bylsma")
			.set("href","mailto:bylsma@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Jianhui Gu")
			.set("href","mailto:gujh@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Grayson Williams")
			.set("href","mailto:Grayson.Laurent.Williams@cern.ch") << endl;
		*out << cgicc::div() << endl;
          
		*out << cgicc::fieldset() << endl;

		*out << cgicc::br()
			.set("style","clear: both;") << endl;

		return out->str();
     }


	virtual string CSS() {
		ostringstream *out = new ostringstream();

		*out << cgicc::style() << endl;
		*out << cgicc::comment() << endl;
		*out << "div.title {width: 60%; text-align: left; color: #000; font-size: 16pt; font-weight: bold;}" << endl;
		*out << "div.expert {width: 60%; text-align: left;}" << endl;
		*out << "fieldset.header {width: 95%; margin: 5px auto 5px auto; padding: 1px; border: 2px solid #555; background-color: #DDD;}" << endl;
		*out << "fieldset.footer {width: 95%; margin: 20px auto 5px auto; padding: 2px 2px 2px 2px; font-size: 9pt; font-style: italic; border: 0px solid #555; text-align: center;}" << endl;
		*out << "fieldset.fieldset, fieldset.normal, fieldset.expert {width: 90%; margin: 10px auto 10px auto; padding: 2px 2px 2px 2px; border: 2px solid #555; background-color: #FFF;}" << endl;
		*out << "fieldset.expert {background-color: #CCC; border: dashed 2px #C00; clear: both;}" << endl;
		*out << "div.legend {width: 100%; padding-left: 20px; margin-bottom: 10px; color: #00D; font-size: 12pt; font-weight: bold;}" << endl;
		*out << ".openclose {border: 1px solid #000; padding: 0px; cursor: pointer; font-family: monospace; color: #000; background-color: #FFF;}" << endl;
		*out << "table.data {border-width: 0px; border-collapse: collapse; margin: 5px auto 5px auto; font-size: 9pt;} " << endl;
		*out << "table.data td {padding: 1px 8px 1px 8px;}" << endl;
		*out << ".Halted, .Enabled, .Disabled, .Configured, .Failed, .unknown {padding: 2px; background-color: #000; font-family: monospace;}" << endl;
		*out << ".Halted {color: #F99;}" << endl;
		*out << ".Enabled {color: #9F9;}" << endl;
		*out << ".Disabled {color: #FF9;}" << endl;
		*out << ".Configured {color: #99F;}" << endl;
		*out << ".Failed, .unknown {color: #F99; font-weight: bold; text-decoration: blink;}" << endl;
		*out << ".error {padding: 2px; background-color: #000; color: #F00; font-family: monospace;}" << endl;
		*out << ".warning {padding: 2px; background-color: #F60; color: #000; font-family: monospace;}" << endl;
		*out << ".orange {padding: 2px; color: #930; font-family: monospace;}" << endl;
		*out << ".caution {padding: 2px; background-color: #FF6; color: #000; font-family: monospace;}" << endl;
		*out << ".yellow {padding: 2px; color: #990; font-family: monospace;}" << endl;
		*out << ".ok {padding: 2px; background-color: #6F6; color: #000; font-family: monospace;}" << endl;
		*out << ".green {padding: 2px; color: #090; font-family: monospace;}" << endl;
		*out << ".bad {padding: 2px; background-color: #F66; color: #000; font-family: monospace;}" << endl;
		*out << ".red {padding: 2px; color: #900; font-family: monospace;}" << endl;
		*out << ".questionable {padding: 2px; background-color: #66F; color: #000; font-family: monospace;}" << endl;
		*out << ".blue {padding: 2px; color: #009; font-family: monospace;}" << endl;
		*out << ".none {padding: 2px; font-family: monospace;}" << endl;
		*out << ".undefined {padding: 2px; background-color: #CCC; color: #333; font-family: monospace;}" << endl;

		*out << ".button {padding: 2px; -moz-border-radius: 3px; -webkit-border-radius: 3px; border: 1px solid #000; cursor: pointer;}" << endl;
		//*out << "body {background-image: url('/tmp/osu_fed_background2.png'); background-repeat: repeat;}" << endl;
		*out << cgicc::comment() << endl;
		*out << cgicc::style() << endl;

		return out->str();
	}


	/** Returns the standard Footer for the EmuFCrate applications.
	*
	*	@returns a huge string that is basically the footer code in HTML.  Good
	*	 for outputting straight to the xgi::Output.
	*
	*	@sa the CSS method.
	**/
	virtual string Footer() {
		ostringstream *out = new ostringstream();

		*out << cgicc::fieldset()
			.set("class","footer") << endl;
		*out << cgicc::div()
			.set("class","switch") << endl;
		*out << cgicc::span("Switches(Experts Only!)")
			.set("style","font-weight: bold") << endl;
		*out << cgicc::a("switch 1")
			.set("href","http://192.168.10.101/") << ", " << endl;
		*out << cgicc::a("switch 2")
			.set("href","http://192.168.10.102/") << ", " << endl;
		*out << cgicc::a("switch 3")
			.set("href","http://192.168.10.103/") << ", " << endl;
		*out << cgicc::a("switch 4")
			.set("href","http://192.168.10.104/") << ", " << endl;
		*out << cgicc::a("switch 7")
			.set("href","http://localhost:8129") << ", " << endl;
		*out << cgicc::a("switch 8")
			.set("href","http://192.168.10.108/") << ", " << endl;
		*out << cgicc::a("switch 9")
			.set("href","http://192.168.10.109/") << ", " << endl;
		*out << cgicc::a("switch 10")
			.set("href","http://192.168.10.110/") << ", " << endl;
		*out << cgicc::div() << endl;
                *out << cgicc::hr() << endl;
		*out << "Built on " << __DATE__ << " at " << __TIME__ << "." << cgicc::br() << endl;
                *out << cgicc::fieldset() << endl;

		*out << "</body>" << endl;
		*out << "</html>" << endl;

		return out->str();
	}


void fill_switch_statistics(){
  for(int swt=0;swt<4;swt++){
    printf(" swt %d \n",swt);
    char command[556];
    sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s status all > /tmp/switch_status.dat \n",ip[swt+swadd]);
    printf("%d  %s \n",swt,command);
    int ierr;
    ierr=system(command);
    parse_status_all(swt);
    ierr=system("rm /tmp/switch_status.dat"); 

    sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s interfaceall > /tmp/switch_interface.dat \n",ip[swt+swadd]);
    printf("%d  %s \n",swt,command);
    ierr=system(command);
    parse_interface_all(swt);
    ierr=system("rm /tmp/switch_interface.dat"); 
  }

}

void fill_switch_macs(){
  for(int swt=0;swt<4;swt++){
    printf(" swt %d \n",swt);
    char command[556];
    sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s mac-addr-table > /tmp/switch_mac.dat \n",ip[swt+swadd]);
    printf("%d  %s \n",swt,command);
    int ierr=system(command);
    parse_mac(swt);
    ierr=system("rm /tmp/switch_mac.dat");
  }

}

void fill_pc_statistics()
{  
  char command[128];
  int ierr;
  sprintf(command,"grep %s /proc/net/dev > out.lis",pc[0].eth);
  printf(" command: %s \n",command);
  ierr=system(command);
   parser_pc(0);
  ierr=system("rm out.lis");
  // sprintf(command,"ssh %s grep %s /proc/net/dev > out.lis",pc[1].machine,pc[1].eth);
  sprintf(command,"grep %s /proc/net/dev > out.lis",pc[1].eth);
  printf("command: %s \n",command);
  ierr=system(command);
  parser_pc(1);
  ierr=system("rm out.lis");
}

void parser_pc(int ieth){
  char line[500];
  char line2[500];
  FILE *file;
  int i;
  file=fopen("out.lis","r");
  fgets(line,500,file);
  // printf(" line %s \n",line);
  for(i=7;i<500;i++)line2[i-7]=line[i];
  sscanf(line2,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",&eth[ieth].rx_bytes,&eth[ieth].rx,&eth[ieth].rx_err,&eth[ieth].rx_drop,&eth[ieth].rx_fifo,&eth[ieth].rx_frame,&eth[ieth].rx_compressed,&eth[ieth].rx_multicast,&eth[ieth].tx_bytes,&eth[ieth].tx,&eth[ieth].tx_err,&eth[ieth].tx_drop,&eth[ieth].tx_fifo,&eth[ieth].tx_colls,&eth[ieth].tx_carrier,&eth[ieth].tx_compressed);
  fclose(file);
}


std::string html_pc_status(){
  char strbuf[500];
  int i;
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>machine</th><th>eth</th><th>rx</th><th>rx_err</th><th> tx</th><th>tx_err</th><th></tr> \n </tr> \n <tbody> \n");
  rtns=rtns+strbuf;
  for(i=0;i<1;i++){
    sprintf(strbuf,"<tr><td>%s</td><td>%s</td><td>%ld</td><td>%ld</td><td>%ld</td><td>%ld</td></tr> \n",pc[i].machine,pc[i].eth,eth[i].rx,eth[i].rx_err,eth[i].tx,eth[i].tx_err);
    rtns=rtns+strbuf;
    sprintf(strbuf,"<tr><td></td><td></td><td>%ld</td><td>%ld</td><td>%ld</td><td>%ld</td></tr> \n",eth[i].rx-ethold[i].rx,eth[i].rx_err-ethold[i].rx_err,eth[i].tx-ethold[i].tx,eth[i].tx_err-ethold[i].tx_err);
    rtns=rtns+strbuf;

  }
  sprintf(strbuf,"</tbody> \n </table> \n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string html_parse_problems(int swtch){
  int ninteresting_lines=30;
  int interesting_lines[30]={13,14,16,17,18,19,20,22,25,26,27,28,29,30,31,32,33,
      40,41,43,44,45,46,49,50,51,52,53,54,56};
  int port;
  char line[256];
  char name[50];
  char num[12];
  int ntline;
  int tn[25];
  char tline[25][50];
  int n;

  char command[556];
  sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s problems > /tmp/problems.dat \n",ip[swtch-swadd2-1+swadd]);
  printf("%d  %s \n",swtch,command);
  int ierr;
  ierr=system(command);

  FILE *file;
  int i,j,k,l,g,il;
  char strbuf[500];
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>switch</th><th>port</th><th>name</th><th>errors</th></tr> \n </tr> \n </thead> \n <tbody> \n");
  rtns=rtns+strbuf;
  file=fopen("/tmp/problems.dat","r");
  // printf(" file opened \n"); 
  for(i=0;i<924;i++){
    il=i%77;
    fgets(line,256,file);
    // printf(" %d %s ",il,line);
    if(il==0){
      ntline=0;
      k=0;
      if(i>76)k=11;
      fill_char(num,line,26+k,2);
      //printf(" num %s \n",num);
      sscanf(num,"%d",&port);
    }
    for(j=0;j<ninteresting_lines;j++){
      if(il==interesting_lines[j]){
        fill_char(num,line,48,10);
        //printf("%d %d %s %s ",i,il,line,num);
        fill_name(name,line);
        sscanf(num,"%d",&n);
        //printf("-> %d %s %d  \n",ntline,name,n);
        if(n!=0){
          for(g=0;g<50;g++)tline[ntline][g]=name[g];
          tn[ntline]=n;
          ntline=ntline+1;
          printf(" *** ntline %d %s %d \n",ntline-1,tline[ntline-1],tn[ntline-1]);
        }
      }
    } 
    if(il==76){
      int ido=0;
      for(l=0;l<ntline;l++)if(tn[l]!=0)ido=1;
      if(ido==1){
        sprintf(strbuf,"<tr><td>%d</td><td>%d</td><td></td><td></td></tr> \n",swtch,port);
        rtns=rtns+strbuf;
      }
      printf(" ntline %d \n",ntline);
      for(l=0;l<ntline;l++){
          sprintf(strbuf,"<tr><td></td><td></td><td>%s</td><td>%d</td></tr>",tline[l],tn[l]);
          rtns=rtns+strbuf;
      }
    }
  }
  fclose(file);
  ierr=system("rm /tmp/problems.dat");
  sprintf(strbuf,"</tbody> \n </table> \n"); 
  rtns=rtns+strbuf;
  return rtns;
}

void fill_name(char *var, char *line){
  int i;
  int j,k;
  k=0;
  j=0;
  for(i=0;i<128;i++){
    if(k==0){
    if(line[i]=='.'&&line[i+1]=='.'){
      k=1;
    }else{
      var[i]=line[i];
      j=j+1;
    }
    }
  }
  var[j]='\0';
}


void fill_expected_mac_table(){

  typedef struct expected_macs{
    char *name;
    char *mac;
  }EXPECTED_MACS;
#include "expected_macs_plus.h"
#include "expected_macs_minus.h"
  EXPECTED_MACS em[37];  
  if(swadd==0)for(int i=0;i<37;i++)em[i]=expplus[i];
  if(swadd==4)for(int i=0;i<37;i++)em[i]=expminus[i];
   
  // initialization
  for(int swt=0;swt<4;swt++){
    for(int prt=0;prt<12;prt++){
      sw[swt][prt].nmacs_expected=0;
    }
  }
  std::cout << " after initialization " << std:: endl;
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
    std::cout << " entered plus " << std::endl;
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
    std::cout << " entered minus " << std::endl;
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

 protected:


};
#endif
