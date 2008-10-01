#include "Switch.h"
#include "toolbox/net/Utils.h"
#include "xdaq/exception/Exception.h"
#include <unistd.h>
#include "XMLParser.h"

using namespace std;

namespace emu{
  namespace pcsw{

// constructor destructor

  Switch::Switch() { }

  void Switch::initialize() {
    std::cout << " Entered Switch Instantiation " << std::endl;
    std::cout << "Hostname is: " << toolbox::net::getHostName() << std::endl;
    std::cout << " Choosing " << sidelabel << " side chambers " << std::endl;
    fill_expected_mac_table();
    printf(" leave instantiation \n");
  }  

  Switch::~Switch(){ }

// switch configure commands

  void Switch::ResetSwitch(std::string switchTelnet){
    int morder[4]={1,0,3,2};
    int porder[4]={2,3,0,1};
    int order[4];
    if(sidelabel=="MINUS")for(int i=0;i<4;i++)order[i]=morder[i];
    if(sidelabel=="PLUS")for(int i=0;i<4;i++)order[i]=porder[i];
    for(int i=0;i<4;i++){
      int ierr;
      int swt=order[i];
      std::string command;
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " reload";
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    if(toolbox::net::getHostName() != "emucom02.cern.ch")
      ::sleep(40);
  }

  void Switch::BackupSwitch(std::string switchTelnet, std::string BackupDir){
    std::cout << "BackupDir = " <<  BackupDir << std::endl;
    time_t rawtime;
    time(&rawtime);
    std::string time_dump = ctime(&rawtime);
    std::string time = time_dump.substr(0,time_dump.length()-1);
    while( time.find(" ",0) != std::string::npos ) {
      int thispos = time.find(" ",0);
      time.replace(thispos,1,"_");
    }

    for(int i=0;i<4;i++){
    //  ./switch_telnet.pl 192.168.10.101 runningconfig > ../scr/switch01_backup.scr
      std::string command;
      int ierr;
      command = switchTelnet + " " + ip_addresses[i].ipaddr + " runningconfig > " + BackupDir + "/" + "switch_" + ip_addresses[i].ipaddr + "." + time + ".scr";
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());

    }
    if(toolbox::net::getHostName() != "emucom02.cern.ch")
      ::sleep(40);
  }

  void Switch::ResetCounters(int swtch,int prt, std::string switchTelnet){
    char port[2];
    sprintf(port, "%d", prt);
    string command;
    command = switchTelnet + " " + ip_addresses[swtch-1].ipaddr + " resetcounter 0/" + port;
    int ierr;
    printf("%s\n", command.c_str());
    if(toolbox::net::getHostName() != "emucom02.cern.ch")
      ierr=system(command.c_str());
    else
      printf("%s\n", command.c_str());
  }

  void Switch::CLRcounters(std::string switchTelnet){
    for(int swt=0;swt<4;swt++) {
      std::string command;
      std::string symb="  ";
      int ierr;
      if(swt!=3)symb="& ";
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " clrcounters " + symb;
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    usleep(250000);
  }
     
  // switch read commands

  void Switch::fill_switch_statistics(std::string switchTelnet) {
    int swt;
    std::string command;
    for(swt=0;swt<4;swt++) {
      std::string symb="  ";
      if(swt!=3) symb="& ";
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " status all > /tmp/switch_status" + ip_addresses[swt].ipaddr + ".dat" + symb;
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    usleep(250000);
    for(swt=0;swt<4;swt++) {
      parse_status_all(swt);
      std::string tmp;
      tmp = "/tmp/switch_status" + ip_addresses[swt].ipaddr + ".dat";
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch") ierr=unlink(tmp.c_str());
    }
    for(int swt=0;swt<4;swt++){
      std::string symb=" ";
      if(swt!=3)symb="& ";
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " interfaceall > /tmp/switch_interface" + ip_addresses[swt].ipaddr + ".dat" + symb;
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    usleep(250000);
    for(int swt=0;swt<4;swt++){
      parse_interface_all(swt);
      std::string tmp;
      tmp = "/tmp/switch_interface" + ip_addresses[swt].ipaddr + ".dat";
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch") ierr=unlink(tmp.c_str());
    }
    copy_stats_new2old();
  }

  void Switch::fill_switch_macs(std::string switchTelnet) {
    for(int swt=0;swt<4;swt++) {
      for(int prt=0;prt<12;prt++) {
        sw[swt][prt].nmacs=0;
      }
    }

    for(int swt=0;swt<4;swt++) {
      std::string command;
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " mac-addr-table > /tmp/switch_mac.dat";
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch") 
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
      parse_mac(swt);
      if(toolbox::net::getHostName()  != "emucom02.cern.ch") ierr=system("rm /tmp/switch_mac.dat");
    }
    copy_stats_new2old();
  }
    
  void Switch::fill_pc_statistics() {  
    std::string command;
    int ierr;
    command = "grep " + pc[0].eth + " /proc/net/dev > out.lis";
    ierr=system(command.c_str());
    parser_pc(0);
    ierr=unlink("out.lis");
    // sprintf(command,"ssh %s grep %s /proc/net/dev > out.lis",pc[1].machine,pc[1].eth);
    command = "grep " + pc[1].eth + " /proc/net/dev > out.lis";
    ierr=system(command.c_str());
    parser_pc(1);
    ierr=unlink("out.lis");
    copy_stats_new2old();
  }

  void Switch::fill_ping(std::string switchTelnet){ 
    char line[128];
    FILE *file;
    std::string command2;
    std::stringstream connecttest;
    int ierr;
    for(int swt=0;swt<4;swt++){
      std::string command;
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " connecttest >& /tmp/connect" + ip_addresses[swt].ipaddr + ".dat";
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    usleep(250000);
    for(int i=0;i<4;i++){
      std::string tmp;
      tmp = "/tmp/connect" + ip_addresses[i].ipaddr + ".dat";
      file=fopen(tmp.c_str(),"r");
      line[0]=' ';line[1]='\0';
      fgets(line,128,file);
      if(line[0]=='1')
        link[i]=1;
      else
        link[i]=0;
      fclose(file);
      if(toolbox::net::getHostName() != "emucom02.cern.ch") unlink(tmp.c_str());
    }
    copy_stats_new2old();
  }

// parse commands

  void Switch::parse_status_all(int swtch){
    char line[128];
    FILE *file;
    int prt;
    char port[3];
    int i;
    std::string tmp;
    tmp = "/tmp/switch_status" + ip_addresses[swtch].ipaddr + ".dat";
    file=fopen(tmp.c_str(),"r");
    for(i=0;i<17;i++) {
      fgets(line,128,file);
      // printf(" %s \n",line);
      if(i>=5) {
        fill_char(port,line,2,2);
        sscanf(port,"%d",&prt);
        prt=prt-1;
        fill_char(sw[swtch][prt].port,line,0,4);
//        printf(" port %s \n",sw[swtch][prt].port);
        fill_char(sw[swtch][prt].media,line,8,6);
//        printf(" media %s \n",sw[swtch][prt].media);
        fill_char(sw[swtch][prt].link,line,49,4);
//        printf(" link %s \n",sw[swtch][prt].link);
      }
    } 
    fclose(file);
  }

  void Switch::parse_interface_all(int swtch){
    char line[128];
    FILE *file;
    char port[3];
    int i,j,prt;
    char tmp[100];
    sprintf(tmp, "/tmp/switch_interface%s.dat", ip_addresses[swtch].ipaddr.c_str());
    file=fopen(tmp,"r");
    for(j=0;j<12;j++) {
      for(i=0;i<11;i++) {
        fgets(line,128,file);
        if(i==0) {
          fill_char(port,line,28,2);
          sscanf(port,"%d",&prt);
          prt=prt-1;
          if(prt!=j)printf(" ***************** Huge port problem \n");
        }
        fill_char(tmp,line,47,15);
        if(i==3) {
          sscanf(tmp,"%d",&sw[swtch][prt].rx);
        }
        if(i==4){
          sscanf(tmp,"%d",&sw[swtch][prt].rx_error);
        }
        if(i==5){
          sscanf(tmp,"%d",&sw[swtch][prt].rx_broad);
        }
        if(i==6){
          sscanf(tmp,"%d",&sw[swtch][prt].tx);
        }
        if(i==7){
          sscanf(tmp,"%d",&sw[swtch][prt].tx_error);
        }
        if(i==8){
          sscanf(tmp,"%d",&sw[swtch][prt].collision);
        }
      }
    }
    fclose(file);
  }
    
  void Switch::parse_mac(int swtch){
    char line[128];
    FILE *file;
    char mac[19];
    char port[3];
    int prt;
    char status[8];
    int n;
    if(toolbox::net::getHostName() != "emucom02.cern.ch") {
      file=fopen("/tmp/switch_mac.dat","r");
    } else {
      char fileName[30];
      sprintf(fileName, "/tmp/switch_mac0%d.dat", swtch+1);
      file=fopen(fileName,"r");
    }
    while(fgets(line,128,file)!=NULL){
      if(line[2]==':'){
        fill_char(mac,line,0,17);
        fill_char(port,line,30,5);
        sscanf(port,"%d",&prt);
        fill_char(sw[swtch][prt-1].vlan,line,39,1);
        fill_char(status,line,48,10);
        n=sw[swtch][prt-1].nmacs;
        if(n<MAX_MACS&&prt<13){
          sw[swtch][prt-1].mac[n].mac=mac;
          sw[swtch][prt-1].mac[n].status=status;
          sw[swtch][prt-1].nmacs++;
        }
      }
      line[2]=0;
    }
    fclose(file);
  }
 
  void Switch::parser_pc(int ieth){
    char line[500];
    char line2[500];
    FILE *file;
    int i;
    file=fopen("out.lis","r");
    fgets(line,500,file);

    for(i=7;i<500;i++)line2[i-7]=line[i];
    sscanf(line2,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",&eth[ieth].rx_bytes,&eth[ieth].rx,&eth[ieth].rx_err,&eth[ieth].rx_drop,&eth[ieth].rx_fifo,&eth[ieth].rx_frame,&eth[ieth].rx_compressed,&eth[ieth].rx_multicast,&eth[ieth].tx_bytes,&eth[ieth].tx,&eth[ieth].tx_err,&eth[ieth].tx_drop,&eth[ieth].tx_fifo,&eth[ieth].tx_colls,&eth[ieth].tx_carrier,&eth[ieth].tx_compressed);
    fclose(file);
  }
    
  std::string Switch::html_mac_table() {
    char strbuf[500];
    std::cout << " enter mac " << std::endl;
    std::string rtns="";
    sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
    rtns=rtns+strbuf;
    sprintf(strbuf,"<thead> \n<tr><th>switch</th><th>port</th><th>vlan</th><th>crate</th><th>mac</th><th>status</th></tr> \n </tr> \n </thead> \n <tbody> \n");
    rtns=rtns+strbuf;
    char *colvlan;
    char *colmac;
    char *colstat;
    for(int swt=0;swt<4;swt++) {
      for(int prt=0;prt<12;prt++){
        std::string chmbr="";
        for(int i=0;i<33;i++) {
          int s=side[i].nswitch;
          int p=side[i].nport;
          if(s==swt+1&&p==prt+1)
            chmbr=side[i].name;
        }
        colvlan="";colmac="";colstat="";
        int ivlan;
        sscanf(sw[swt][prt].vlan,"%d",&ivlan);
//        if(sidelabel=="PLUS") {
//          if(swt==3){
//            if(ivlan!=1)colvlan=" bgcolor=\"red\"";
//          }else{
//            if(prt+1<=6&&ivlan!=1)colvlan=" bgcolor=\"red\"";
//            if(prt+1>6&&ivlan!=2)colvlan=" bgcolor=\"red\"";
//          }
//        } 
        int first=0;
        int n=sw[swt][prt].nmacs;
        for(int i=0;i<n;i++) {
          colstat="";
          if(sw[swt][prt].mac[i].status != "Static    ")
	    colstat=" bgcolor=\"red\"";
	  else
	    colstat="";
          int slt=0;
//          if(i==0) {
            first=1;
	    if(sidelabel=="PLUS")
              sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>%s</td></tr> \n",swt+1,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr.c_str(),colmac,sw[swt][prt].mac[i].mac.c_str(),colstat,sw[swt][prt].mac[i].status.c_str());
            else
              sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>%s</td></tr> \n",swt+1+6,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr.c_str(),colmac,sw[swt][prt].mac[i].mac.c_str(),colstat,sw[swt][prt].mac[i].status.c_str());
//          } else {
//            sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td %s>%s</td><td %s>%s</td></tr> \n",colmac,sw[swt][prt].mac[i].mac.c_str(),colstat,sw[swt][prt].mac[i].status.c_str());
//          }
          rtns=rtns+strbuf;
        }
        colvlan=" bgcolor=\"orange\"";colmac=" bgcolor=\"orange\"";colstat=" bgcolor=\"orange\"";
        int ne=sw[swt][prt].nmacs_expected;
        for(int i=0;i<ne;i++){
          int ipass=0;
          int n=sw[swt][prt].nmacs;
          for(int m=0;m<n;m++) {
            if(sw[swt][prt].mac[m].mac == sw[swt][prt].mac_expected[i].mac)ipass=1;
            printf("%s %s\n", sw[swt][prt].mac[m].mac.c_str(), sw[swt][prt].mac_expected[i].mac.c_str());
          }
          if(ipass==0){
            int slt=0;
//            if(first==0)sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>Missing</td></tr> \n",swt,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr.c_str(),colmac,sw[swt][prt].mac_expected[i].mac.c_str(),colstat);
            sprintf(strbuf,"<tr><td>%d</td><td>%d/%d</td><td %s>%s</td><td>%s</td><td %s>%s</td><td %s>Missing</td></tr> \n",swt,slt,prt+1,colvlan,sw[swt][prt].vlan,chmbr.c_str(),colmac,sw[swt][prt].mac_expected[i].mac.c_str(),colstat);
//            if(first==1)sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td %s>%s</td><td %s>Missing</td></tr> \n",colmac,sw[swt][prt].mac_expected[i].mac.c_str(),colstat);
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

    
std::string Switch::html_pc_status(){
  char strbuf[500];
  int i;
  std::string rtns="";
  sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
  rtns=rtns+strbuf;
  sprintf(strbuf,"<thead> \n   <tr><th>machine</th><th>eth</th><th>rx</th><th>rx_err</th><th> tx</th><th>tx_err</th><th></tr> \n </tr> \n <tbody> \n");
  rtns=rtns+strbuf;
  for(i=0;i<2;i++){
    sprintf(strbuf,"<tr><td>%s</td><td>%s</td><td>%ld</td><td>%ld</td><td>%ld</td><td>%ld</td></tr> \n",pc[i].machine.c_str(),pc[i].eth.c_str(),eth[i].rx,eth[i].rx_err,eth[i].tx,eth[i].tx_err);
    rtns=rtns+strbuf;
    sprintf(strbuf,"<tr><td></td><td></td><td>%ld</td><td>%ld</td><td>%ld</td><td>%ld</td></tr> \n",eth[i].rx-ethold[i].rx,eth[i].rx_err-ethold[i].rx_err,eth[i].tx-ethold[i].tx,eth[i].tx_err-ethold[i].tx_err);
    rtns=rtns+strbuf;
  } 
  sprintf(strbuf,"</tbody> \n </table> \n");
  rtns=rtns+strbuf;
  return rtns;
}
    

  void Switch::fill_problems(std::string switchTelnet){
    std::string command;
    for(int swt=0;swt<4;swt++){
      std::string symb=" ";
      if(swt!=3)symb="& ";
      command = switchTelnet + " " + ip_addresses[swt].ipaddr + " problems >& /tmp/problems" + ip_addresses[swt].ipaddr + ".dat" + symb;
      std::cout << swt << " " << command << std::endl;
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
    }
    usleep(250000);
  }

  std::string Switch::html_parse_problems(int swtch){
    int ninteresting_lines=30;
    int interesting_lines[30]={13,14,16,17,18,19,20,22,25,26,27,28,29,30,31,32,33,40,41,43,44,45,46,49,50,51,52,53,54,56};
    int port;
    char line[256];
    char name[50];
    char num[12];
    int ntline;
    int tn[25];
    char tline[25][50];
    int n;
    
    FILE *file;
    int i,j,k,l,g,il;
    char strbuf[500];
    std::string rtns="";
    sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
    rtns=rtns+strbuf;
    sprintf(strbuf,"<thead> \n   <tr><th>switch</th><th>port</th><th>name</th><th>errors</th></tr> \n </tr> \n </thead> \n <tbody> \n");
    rtns=rtns+strbuf;
    char temp[30];
    sprintf(temp,"/tmp/problems%s.dat",ip_addresses[swtch-1].ipaddr.c_str());
    printf("%s\n", temp);
    file=fopen(temp,"r");
    for(i=0;i<924;i++){
      il=i%77;
      fgets(line,256,file);
      if(il==0){
        ntline=0;
        k=0;
        if(i>76)k=11;
        fill_char(num,line,26+k,2);
        sscanf(num,"%d",&port);
      }
      for(j=0;j<ninteresting_lines;j++){
        if(il==interesting_lines[j]){
          fill_char(num,line,48,10);
          fill_name(name,line);
          sscanf(num,"%d",&n);
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
	  if(sidelabel== "PLUS")
            sprintf(strbuf,"<tr><td>%d</td><td>%d</td><td></td><td></td></tr> \n",swtch,port);
	  else
            sprintf(strbuf,"<tr><td>%d</td><td>%d</td><td></td><td></td></tr> \n",swtch+6,port);
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
    sprintf(temp,"rm /tmp/problems%02d.dat",swtch-1);
    int ierr;
    if(toolbox::net::getHostName() != "emucom02.cern.ch")
      ierr=system(temp);
    else
      printf("%s\n", temp);
    sprintf(strbuf,"</tbody> \n </table> \n"); 
    rtns=rtns+strbuf;
    return rtns;
  }

  std::string Switch::html_port_status(){
    char strbuf[500];
    int crate,swt,prt,slt;
    std::string rtns="";
    sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
    rtns=rtns+strbuf;
    sprintf(strbuf,"<thead> \n <tr><th>crate</th><th>switch</th><th>port</th><th>link</th><th>rx</th><th>rx_error</th><th>tx</th><th>tx_error</th></tr> \n </tr> \n <tbody> \n");
    rtns=rtns+strbuf;
    for(crate=0;crate<49;crate++){
      swt=side[crate].nswitch;
      prt=side[crate].nport;
      slt=side[crate].vlan;
      if(crate<33||crate>36){
        sprintf(strbuf,"<tr><td>%s</td><td>%d</td><td>0/%d</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",side[crate].name.c_str(),swt,prt,sw[swt-1][prt-1].link,sw[swt-1][prt-1].rx,sw[swt-1][prt-1].rx_error,sw[swt-1][prt-1].tx,sw[swt-1][prt-1].tx_error);
        rtns=rtns+strbuf;
        sprintf(strbuf,"<tr><td></td><td></td><td></td><td></td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",sw[swt-1][prt-1].rx-old[swt-1][prt-1].rx,sw[swt-1][prt-1].rx_error-old[swt-1][prt-1].rx_error,sw[swt-1][prt-1].tx-old[swt-1][prt-1].tx,sw[swt-1][prt-1].tx_error-old[swt-1][prt-1].tx_error);
        rtns=rtns+strbuf;
      }
    }
    sprintf(strbuf,"</tbody> \n </table> \n");
    rtns=rtns+strbuf;
    return rtns;
  }

  std::string Switch::html_ping(){
    char strbuf[500];
    std::string rtns="";
    sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>\n");
    rtns=rtns+strbuf;
    sprintf(strbuf,"<tbody align=\"center\" bgcolor=\"black\">\n");
    rtns=rtns+strbuf;
    if(sidelabel=="PLUS")
      sprintf(strbuf,"<tr><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td></tr>\n",1,2,3,4);
    else
      sprintf(strbuf,"<tr><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td></tr>\n",7,8,9,10);
    rtns=rtns+strbuf;
    char ballimg[4][128];
    for(int j=0;j<4;j++){
      sprintf(ballimg[j],"<font color=\"red\">OFF</font>");
      if(link[j]==1)sprintf(ballimg[j],"<font color=\"green\">ON</font>");
    } 
    sprintf(strbuf,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",ballimg[0],ballimg[1],ballimg[2],ballimg[3]);
    rtns=rtns+strbuf;
    sprintf(strbuf,"</tbody>\n");
    rtns=rtns+strbuf;
    sprintf(strbuf,"</table>\n");
    rtns=rtns+strbuf;
    return rtns;
  }
  
// utilities

  void Switch::fill_char(char *var,char *line,int first,int length) {
    int i;
    for(i=first;i<first+length;i++)
      var[i-first]=line[i];
    var[length]='\0';
  }

  void Switch::fill_name(char *var, char *line){
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

  void Switch::fill_expected_mac_table(){
    std::cout << "enter fill_expected_mac_table" << std::endl;
    for(int swt=0;swt<4;swt++){
      for(int prt=0;prt<12;prt++){
        sw[swt][prt].nmacs_expected=0;
      }
    }
    for(int i=0;i<33;i++){
      int swt=side[i].nswitch-1;
      int prt=side[i].nport-1;
      int n=sw[swt][prt].nmacs_expected;
      sw[swt][prt].mac_expected[n].mac=side[i].pmac.mac;
      n++;
      sw[swt][prt].nmacs_expected=n;
    }

    // now primary switch transfers
    for(int i=37;i<43;i++){
      char tmp[3];
      tmp[0]=side[i].name[8];tmp[1]=side[i].name[9];tmp[3]='\0';
      int sweetch;
      sscanf(tmp,"%d",&sweetch);
      //printf(" swt %d %s\n",swt,side[i].name);
      for(int j=0;j<37;j++){
        if(side[j].nswitch==sweetch&&((side[j].vlan==side[i].vlan&&((sweetch!=4&&sweetch!=7)||j<3))||(side[j].vlan==0&&(i<40||i>42))||(j<3||side[j].nport==13))){
          int swt=side[i].nswitch-1;
          int prt=side[i].nport-1;
          int n=sw[swt][prt].nmacs_expected;
          sw[swt][prt].mac_expected[n].mac=side[j].pmac.mac;
          n++;
          sw[swt][prt].nmacs_expected=n;
	  std::cout << "swt=" << swt << " switch=" << sweetch << " " << side[j].pmac.mac << std::endl;
        }
      }
    }
    dump_expected_macs();
  }

  void Switch::dump_expected_macs(){
    std::cout << " Expected Switch Addresses " << std::endl;
    for(int swt=0;swt<4;swt++){
      std::cout << "Switch" <<swt+1<<std::endl;
      for(int prt=0;prt<12;prt++){
        int n=sw[swt][prt].nmacs_expected;
        for(int m=0;m<n;m++)
          std::cout << swt+1 << " " << prt+1 << " " << sw[swt][prt].mac_expected[m].mac << std::endl;
      }
    }
  }
    
  void Switch::copy_stats_new2old(){
    for(int swt=0;swt<4;swt++){
      for(int prt=0;prt<12;prt++){
        old[swt][prt]=sw[swt][prt];
      }
    }
    for(int i=0;i<2;i++)ethold[i]=eth[i];
  }

  std::string Switch::dlink_stats(){ 
    FILE *dlink;
    char line[128];
    std::string rtns="";
    dlink=fopen("/proc/net/dev","r");
    int nlines=0;
    while(fgets(line,128,dlink)){
      nlines=nlines+1;
      if(nlines<3)rtns=rtns+line;
      if(line[2]=='e'&&line[3]=='t'&&line[4]=='h'&&line[5]=='2')rtns=rtns+line;
    }
    fclose(dlink);
    return rtns;
  }

  std::string Switch::eth_hook_stats(){
    FILE *ehook;
    char line[128];
    std::string rtns="";
    ehook=fopen("/proc/sys/dev/schar/0","r");
    int nlines=0;
    while(fgets(line,128,ehook)){
      nlines=nlines+1;
      rtns=rtns+line;
    }
    fclose(ehook);
    return rtns;
  }


  std::string Switch::switch_stats(int lasthex, std::string testScript){ 

    // find port with problems
    int s[2];   
    for(int i=0;i<33;i++){
      char xhex[3];
      int hex;
      xhex[0]=side[i].pmac.mac[15];
      xhex[1]=side[i].pmac.mac[16];
      xhex[2]='\n';
      sscanf(xhex,"%x",&hex);
      if(hex==lasthex){s[0]=0;s[1]=i;}
    }
    
    
    char line[128];
    std::string rtns="";
    for(int i=0;i<2;i++){
      sprintf(line," %s %s switch %d port %d \n",side[s[i]].pmac.mac.c_str(),side[s[i]].name.c_str(),side[s[i]].nswitch,side[s[i]].nport);
      rtns=rtns+line;
      std::string command;
      std::stringstream testpl;
      command = testScript + " 192.168.10.1";
      
      if(side[s[i]].nswitch == 10) testpl << side[s[i]].nswitch;
      else testpl << "0" <<  side[s[i]].nswitch;
      testpl >> command;
      command += " ";
      testpl <<side[s[i]].nport;
      testpl >> command;
      command += " > /tmp/error.dat"; 
      std::cout << command << std::endl;
      int ierr;
      if(toolbox::net::getHostName() != "emucom02.cern.ch")
        ierr=system(command.c_str());
      else
        printf("%s\n", command.c_str());
      FILE *file;
      file=fopen("/tmp/error.dat","r");
      while(fgets(line,128,file))
        if(line[0]!='(')rtns=rtns+line;
      fclose(file);
      if(toolbox::net::getHostName()  != "emucom02.cern.ch") ierr=system("rm -f /tmp/error.dat");
    }
    return rtns;
  }

  } // namespace pcsw
} //namespace emu
