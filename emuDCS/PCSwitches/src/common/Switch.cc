#include "Switch.h"
#include "toolbox/net/Utils.h"
#include "xdaq/exception/Exception.h"

using namespace std;

namespace emu{
  namespace pcsw{

// constructor destructor

     Switch::Switch(){	

      #include "expected_macs_plus.h"
      #include "expected_macs_minus.h"

      //cern switch addresses
      char* tip[8]={
       "192.168.10.101","192.168.10.102",
       "192.168.10.103","192.168.10.104",
       "192.168.10.107","192.168.10.108",
       "192.168.10.109","192.168.10.110"
      };
      
      PC_STATS pc_plus[2]={
       {"csc-pc1","eth2"},
        {"csc-pc1","eth2"}
      };
      
      PC_STATS pc_minus[2]={
        {"csc-pc2","eth2"},
        {"csc-pc2","eth2"}
      };
   
      std::cout << " Entered Switch Instatiation " << std::endl;
/*    This cannot work--no $BUILD_HOME on production systems!!
      int ierr=system("cp $BUILD_HOME/emu/emuDCS/PCSwitches/img/redball.gif /tmp/redball.gif");
      ierr=system("cp $BUILD_HOME/emu/emuDCS/PCSwitches/img/grnball.gif /tmp/grnball.gif");
*/
      std::cout << " just instantiated " << std::endl;      
      std::cout << "Hostname is: " << toolbox::net::getHostName() << std::endl;
      std::cout << "Domainname is: " << toolbox::net::getDomainName() << std::endl;
      std::string pcname=toolbox::net::getHostName(); 
      std::string pcplus="vmepcS2G18-09.cms";
//std::string pcplus = "csc-pc1.cms";
//std::string pcminus = "csc-pc2.cms";
      std::string pcminus="vmepcS2G18-08.cms";
      std::cout << "hostname " << pcname << "plusname " << pcplus << "minusname " << pcminus << std::endl;
      std::string Side;
      if((pcname==pcplus) || (pcname=="emucom02.cern.ch")) Side="plus";
      if(pcname==pcminus)Side="minus";
        
      std::cout << " Choosing " << Side << " side chambers " << std::endl;
      init=0;
      if(Side=="plus"){
         for(int i=0;i<49;i++)side[i]=plus[i];
         swadd=0;swadd2=0;
         for(int i=0;i<2;i++)pc[i]=pc_plus[i];
      }
      if(Side=="minus"){
         for(int i=0;i<49;i++)side[i]=minus[i];
         swadd=4;swadd2=6;
         for(int i=0;i<2;i++)pc[i]=pc_minus[i];
      }
      for(int i=0;i<8;i++)ip[i]=tip[i];
      fill_expected_mac_table();
      printf(" leave instantiation \n");
    }  

    Switch::~Switch(){
    }

// switch configure commands

    void Switch::ResetSwitch(std::string switchTelnet){
      int morder[4]={2,3,0,1};
      int porder[4]={3,0,1,2};
      int order[4];
      if(swadd==0)for(int i=0;i<4;i++)order[i]=morder[i];
      if(swadd==4)for(int i=0;i<4;i++)order[i]=porder[i];
      for(int i=0;i<4;i++){
        int swt=order[i];
        std::string command;
        command = switchTelnet + " " + ip[swt+swadd] + " reload";
        system(command.c_str());
      }
      ::sleep(40);
    }
/*
    void Switch::BackupSwitch(std::string backupScript){
      OutputSwitch << "BackupSwitch" << std::endl;
      LogFileSwitch.open(filebuf.c_str());
      int morder[4]={2,3,0,1};
      int porder[4]={3,0,1,2};
      int order[4];
      if(swadd==0)for(int i=0;i<4;i++)order[i]=morder[i];
      if(swadd==4)for(int i=0;i<4;i++)order[i]=porder[i];
      for(int i=0;i<4;i++){
        int swt=order[i];
        OutputSwitch << " swt " << order[i]+swadd2 << std::endl;
//	    if(swadd==0) sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_backup_minus");
//		if(swadd==4) sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_backup_plus");
        OutputSwitch << swt << " " << swt+swadd << " " << ip[swt+swadd] << " " << backupScript << std::endl;
        system(backupScript.c_str());
      }
      ::sleep(40);
      LogFileSwitch << OutputSwitch.str() ;
      LogFileSwitch.close();
    }
*/

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

      int morder[4]={2,3,0,1};
      int porder[4]={3,0,1,2};
      int order[4];
      if(swadd==0)for(int i=0;i<4;i++)order[i]=morder[i];
      if(swadd==4)for(int i=0;i<4;i++)order[i]=porder[i];
      for(int i=0;i<4;i++){
        int swt=order[i];
//	./switch_telnet.pl 192.168.10.101 runningconfig > ../scr/switch01_backup.scr
	std::string command;
	command = switchTelnet + " " + ip[swt+swadd] + " runningconfig > " + BackupDir + "/" + "switch_" + ip[swt+swadd] + "." + time + ".scr";
//	    if(swadd==0) sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_backup_minus");
//		if(swadd==4) sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_backup_plus");
	std::cout << command << std::endl;
        system(command.c_str());
      }
      ::sleep(40);
    }


    
    void Switch::ResetCounters(int swtch,int prt, std::string switchTelnet){
      stringstream port;
      string command;
      if(swadd==4) {
//	sprintf(command,"$BUILD_HOME/emu/emuDCS/Switch/bin/switch_telnet.pl %s resetcounter 0/%d",ip[swtch-swadd+1],prt);
	port << switchTelnet << " " << ip[swtch-swadd+1] << " resetcounter 0/" << prt;
	port >> command;
      } else {
	port << switchTelnet << " " << ip[swtch-swadd-1] << " resetcounter 0/" << prt;
	port >> command;
      }
      int ierr;
      ierr=system(command.c_str());
    }
        
    void Switch::CLRcounters(std::string switchTelnet){
      for(int swt=0;swt<4;swt++) {
        std::string command;
        std::string symb="  ";
        if(swt!=3)symb="& ";
	command = switchTelnet + " " + ip[swt+swadd] + " clrcounters " + symb;
//        sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s clrcounters%s",ip[swt+swadd],symb);
        system(command.c_str());
      }
      usleep(250000);
    }
     
// switch read commands


    void Switch::fill_switch_statistics(std::string switchTelnet){
      for(int swt=0;swt<4;swt++){
        std::string command;
	std::string command2;
	std::stringstream switch_status;
        std::string symb="  ";
        if(swt!=3) symb="& ";
	command = switchTelnet + " " + ip[swt+swadd] + " status all > /tmp/switch_status";
	if(swt+swadd2+1 == 10) switch_status << swt+swadd2+1;
	else switch_status << "0" << swt+swadd2+1;
	switch_status >> command2;
	command += command2 + ".dat" + symb;
//        sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s status all > /tmp/switch_status%02d.dat %s \n",ip[swt+swadd],swt+swadd2+1,symb);
	
        std::cout << swt << "  "  << command << std::endl;
        int ierr;
        ierr=system(command.c_str());
      }
      usleep(250000);
      for(int swt=0;swt<4;swt++){
        parse_status_all(swt);
        char tmp[100];
	sprintf(tmp,"rm /tmp/switch_status%02d.dat",swt+swadd2+1);
        int ierr;
        ierr=system(tmp);
      } 
      for(int swt=0;swt<4;swt++){
        std::string symb=" ";
        if(swt!=3)symb="& ";
        std::string command;
	std::string command2;
	std::stringstream switch_status;
	command = switchTelnet + " " + ip[swt+swadd] + " interfaceall > /tmp/switch_interface";
	if(swt+swadd2+1 == 10) switch_status << swt+swadd2+1;
	else switch_status << "0" << swt+swadd2+1;
	switch_status >> command2;
	command += command2 + ".dat" + symb;
//        sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s interfaceall > /tmp/switch_interface%02d.dat %s\n",ip[swt+swadd],swt+swadd2+1,symb);
        std::cout << swt << " " << command << std::endl;
        int ierr;
        ierr=system(command.c_str());
      }
      usleep(250000);
      for(int swt=0;swt<4;swt++){
        parse_interface_all(swt);
        char tmp[100];
        sprintf(tmp,"rm /tmp/switch_interface%02d.dat",swt+swadd2+1);
        int ierr;
        ierr=system(tmp); 
      }
    
    }
    
    void Switch::fill_switch_macs(std::string switchTelnet){

      for(int swt=0;swt<4;swt++){
        for(int prt=0;prt<12;prt++){
          sw[swt][prt].nmacs=0;
        }
      }

      for(int swt=0;swt<4;swt++){
        printf(" swt %d \n",swt);
        std::string command;
	command = switchTelnet + " " + ip[swt+swadd] + " mac-addr-table > /tmp/switch_mac.dat";
//        sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s mac-addr-table > /tmp/switch_mac.dat \n",ip[swt+swadd]);
        std::cout << swt << command << std::endl;
        int ierr;
	ierr=system(command.c_str());
        parse_mac(swt);
        ierr=system("rm /tmp/switch_mac.dat");
      }
    
    }
    
    void Switch::fill_pc_statistics()
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

    
    void Switch::fill_ping(std::string switchTelnet){ 
      char line[128];
      FILE *file;
      for(int swt=0;swt<4;swt++){
        std::string command;
	std::string command2;
	std::stringstream connecttest;
        command = switchTelnet + " " + ip[swt+swadd] + " connecttest >& /tmp/connect";
	if(swt+swadd2+1 == 10) connecttest << swt+swadd2+1;
	else connecttest << "0" << swt+swadd2+1;
	connecttest >> command2;
	command += command2 + ".dat";

//        sprintf(command,"/nfshome0/cscpro/TriDAS/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s connecttest >& /tmp/connect%02d.dat",ip[i+swadd],i+swadd2+1);
	std::cout << command << std::endl;
        system(command.c_str());
      }
      usleep(250000);
      std::cout << " after usleep " << std::endl;
      for(int i=0;i<4;i++){
        char tmp[100];
        sprintf(tmp,"/tmp/connect%02d.dat",i+swadd2+1);
        file=fopen(tmp,"r");
        line[0]=' ';line[1]='\0';
        fgets(line,128,file);
        printf(" --%s--\n ",line);
        if(line[0]=='1'){
          link[i]=1;
        } else {
          link[i]=0;
        }
	std::cout << " switch link "<< i<< " " <<link[i]<<std::endl;
        fclose(file);
        sprintf(tmp,"rm /tmp/connect%02d.dat",i+swadd2+1);
        system(tmp);
      }
    }

// parse commands

    void Switch::parse_status(int swtch,int prt){
      char line[128];
      FILE *file;
      int i;
      char tmp[50];
      sprintf(tmp,"/tmp/switch_status%02d.dat",swtch+swadd2+1);
      file=fopen(tmp,"r");
      for(i=0;i<6;i++){
        fgets(line,128,file);
      }
      fill_char(sw[swtch][prt].port,line,0,4);
      fill_char(sw[swtch][prt].media,line,8,6);
      fill_char(sw[swtch][prt].link,line,49,4);
      fclose(file);
    }
    
    void Switch::parse_status_all(int swtch){
      char line[128];
      FILE *file;
      int prt;
      char port[3];
      int i;
      char tmp[100];
      sprintf(tmp,"/tmp/switch_status%02d.dat",swtch+swadd2+1);
      file=fopen(tmp,"r");
      for(i=0;i<17;i++) {
        fgets(line,128,file);
        // printf(" %s \n",line);
        if(i>=5) {
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
    
    void Switch::parse_interface(int swtch,int prt){
      char line[128];
      FILE *file;
      int i;
      char tmp[100];
      sprintf(tmp,"/tmp/switch_interface%02d.dat",swtch+swadd2+1);
      file=fopen(tmp,"r");
      for(i=0;i<9;i++) {
        fgets(line,128,file);
        // printf(" %s \n",line);
        fill_char(tmp,line,47,15);
	    switch(i) {  
	      case 3:
            sscanf(tmp,"%d",&sw[swtch][prt].rx);
		    break;
             //printf(" rx %d \n",sw[swtch][prt].rx);
	      case 4:
            sscanf(tmp,"%d",&sw[swtch][prt].rx_error);
		    break;
             //printf(" rx_error %d \n",sw[swtch][prt].rx_error);
		  case 5:
            sscanf(tmp,"%d",&sw[swtch][prt].rx_broad);
		    break;
             //printf(" rx_broad %d \n",sw[swtch][prt].rx_broad);
		  case 6:
            sscanf(tmp,"%d",&sw[swtch][prt].tx);
		    break;
             //printf(" tx %d \n",sw[swtch][prt].tx);
		  case 7:
            sscanf(tmp,"%d",&sw[swtch][prt].tx_error);
			break;
             //printf(" tx_error %d \n",sw[swtch][prt].tx_error);
		  case 8:
            sscanf(tmp,"%d",&sw[swtch][prt].collision);
		    break;
    	 // printf(" collision %d \n",sw[swtch][prt].collision);
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
      sprintf(tmp,"/tmp/switch_interface%02d.dat",swtch+swadd2+1);
      file=fopen(tmp,"r");
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
    
    void Switch::parse_mac(int swtch){
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
          // if(sw[swtch][prt-1].mac[n].mac==NULL)sw[swtch][prt-1].mac[n].mac=(char *)malloc(19);
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
     
 

    
    void Switch::parser_pc(int ieth){
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
    

    std::string Switch::html_mac_table()
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

    
  std::string Switch::html_pc_status(){
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
    

    void Switch::fill_problems(std::string switchTelnet){
      std::string command;
      for(int swt=0;swt<4;swt++){
	std::string symb=" ";
        if(swt!=3)symb="& ";
	std::string command2;
	std::stringstream fillproblems;
	command = switchTelnet + " " + ip[swt+swadd] + " problems >& /tmp/problems";
	if(swt+swadd2+1 == 10) fillproblems << swt+swadd2+1;
	else fillproblems << "0" << swt+swadd2+1;
	fillproblems >> command2;
	command += command2 + ".dat" + symb;
//        sprintf(command,"$BUILD_HOME/emu/emuDCS/PCSwitches/bin/switch_telnet.pl %s problems > /tmp/problems%02d.dat%s \n",ip[swt+swadd],swt+1+swadd2,symb);
	std::cout << swt << " " << command << std::endl;
//        printf("%d  %s \n",swt,command);
        int ierr;
        ierr=system(command.c_str());
      }
      usleep(250000);
    }

    std::string Switch::html_parse_problems(int swtch){
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
      
      FILE *file;
      int i,j,k,l,g,il;
      char strbuf[500];
      std::string rtns="";
      sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
      rtns=rtns+strbuf;
      sprintf(strbuf,"<thead> \n   <tr><th>switch</th><th>port</th><th>name</th><th>errors</th></tr> \n </tr> \n </thead> \n <tbody> \n");
      rtns=rtns+strbuf;
      char temp[30];
      sprintf(temp,"/tmp/problems%02d.dat",swtch);
      file=fopen(temp,"r");
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
      sprintf(temp,"rm /tmp/problems%02d.dat",swtch);
      int ierr;
      ierr=system(temp);
      sprintf(strbuf,"</tbody> \n </table> \n"); 
      rtns=rtns+strbuf;
      return rtns;
    }

   std::string  Switch::html_port_status(){
     char strbuf[500];
     int crate,swt,prt,slt;
     std::string rtns="";
     sprintf(strbuf,"<table cellpadding=6 rules=groups frame=box>");
     rtns=rtns+strbuf;
     sprintf(strbuf,"<thead> \n   <tr><th>crate</th><th>switch</th><th>port</th><th>link</th><th>rx</th><th>rx_error</th><th>tx</th><th>tx_error</th></tr> \n </tr> \n <tbody> \n");
     rtns=rtns+strbuf;
     for(crate=0;crate<49;crate++){
       swt=side[crate].nswitch-swadd2;
       prt=side[crate].nport;
       slt=side[crate].vlan;
       if(crate<33||crate>36){
       sprintf(strbuf,"<tr><td>%s</td><td>%d</td><td>0/%d</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",side[crate].name,swt+swadd2,prt,sw[swt-1][prt-1].link,sw[swt-1][prt-1].rx,sw[swt-1][prt-1].rx_error,sw[swt-1][prt-1].tx,sw[swt-1][prt-1].tx_error);
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
      int i=swadd2;
      sprintf(strbuf,"<tr><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td><td><font color=\"white\">Switch%d</font></td></tr>\n",i+1,i+2,i+3,i+4);
      rtns=rtns+strbuf;
      char ballimg[4][128];
      for(int j=0;j<4;j++){
//        sprintf(ballimg[j],"<img src=\"/tmp/redball.gif\" style=\"center:\"/>");
        sprintf(ballimg[j],"<font color=\"red\">OFF</font>");
//        if(link[j]==1)sprintf(ballimg[j],"<img src=\"/tmp/grnball.gif\" style=\"center:\"/>");
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
   
    int Switch::compare(char *a,char *b,int begin,int length){
      int j=0;
      for(int i=begin;i<begin+length;i++){
        if(a[i]==b[i])j=j+1;
      }
      return j;
    }

    void Switch::fill_expected_mac_table(){
	  std::cout << " enter fill_expected_mac_table" << std::endl;
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
        for(int k=0;k<18;k++)sw[swt][prt].mac_expected[n].mac[k]=side[i].pmac.mac[k];
        n=n+1;
        sw[swt][prt].nmacs_expected=n;
      }
      // now primary switch transfers
      for(int i=37;i<49;i++){
        char tmp[3];
        tmp[0]=side[i].name[8];tmp[1]=side[i].name[9];tmp[3]='\0';
        int swt;
        sscanf(tmp,"%d",&swt);
        printf(" swt %d \n",swt);
        for(int j=0;j<37;j++){
          if(side[j].nswitch==swt&&(side[j].vlan==side[i].vlan||(side[j].vlan==0&&(i<40||i>42))||(j<3))){
            int swt=side[i].nswitch-1-swadd2;
            int prt=side[i].nport-1;
            int n=sw[swt][prt].nmacs_expected;
            for(int k=0;k<18;k++)sw[swt][prt].mac_expected[n].mac[k]=side[j].pmac.mac[k];
            n=n+1;
            sw[swt][prt].nmacs_expected=n;
          }
        }
      }
      dump_expected_macs(swadd2);
    }

    
    void Switch::dump_expected_macs(int ioff){
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
      sprintf(line," %s %s switch %d port %d \n",side[s[i]].pmac.mac,side[s[i]].name,side[s[i]].nswitch,side[s[i]].nport);
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
//      sprintf(command,"/nfshome0/cscpro/TriDAS/emu/emuDCS/PeripheralApps/xml/test.pl 192.168.10.1%02d %d > /tmp/error.dat",side[s[i]].nswitch,side[s[i]].nport);
      std::cout << command << std::endl;
      int ierr;
      ierr=system(command.c_str());
      FILE *file;
      file=fopen("/tmp/error.dat","r");
      while(fgets(line,128,file)){
        if(line[0]!='(')rtns=rtns+line;
      }
      fclose(file);
      ierr=system("rm -f /tmp/error.dat");
    }
    return rtns;
  }

  } // namespace pcsw
} //namespace emu
