#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>


// instantiate and login/logout
std::string header();
std::string header2(char *ip);
std::string login();
std::string logout();

// show commands
std::string macaddrtable();
std::string show_interface(char *s);

// enable commands (show->enable)
std::string enable();
std::string save();
std::string reload();
std::string exitenable();

// vlan commands
std::string vlandatabase();
std::string vlanid(int vlan);
std::string vlanname(int vlan,char *name);
std::string vlanmakestatic(int vlan);
std::string exitvlan();

// configure commands (show -> enable -> configure)
std::string configure();
std::string logginghost();
std::string loggingsyslog();
std::string spanningtree(bool flag);
std::string portsecurity(bool flag);
std::string snmpservertrapslinkmode(bool flag);
std::string snmptrapslinkstatus(bool flag);
std::string mtu(int size);
std::string portchanneladminmodeall(bool flag);
std::string portchannellinktrapall(bool flag);
std::string stormcontrolbroadcast(bool flag);
std::string stormcontrolmulticast(bool flag);
std::string stormcontrolunicast(bool flag);
std::string stormcontrolflowcontrol(bool flag);
std::string exitconfig();

// interface commands (show -> enable -> configure -> interface)
std::string interface(char *port);
std::string interfacerange(char *port);
std::string adminmode(bool flag,char *port);
std::string portlacpmode(bool flag,char *port);
std::string autonegotiate(bool flag,char *port);
std::string speed(char *port,int tspeed);
std::string vlan(char *port,int tvlan);
std::string vlaningressfilter(bool flag,char *port);
std::string vlanparticipation(char *port,char *include,int vlan);
std::string vlanacceptframeall(bool flag,char *port);
std::string vlanpriority(char *port,int priority);
std::string portsecurity(bool flag,char *port);
std::string portsecuritymaxdynamic(char *port,int number);
std::string portsecuritymaxsstatic(char *port,int number);
std::string portsecuritymacaddress(bool flag,char *port,char *mac,int tvlan);
std::string portchannelstaticenable(bool flag,char *port);
std::string snmpservertrapsviolation(bool flat,char *port);
std::string exitinterface();
char *tport(char *port);
// scr2pl commands
std::string waitfor(std::string mode,char *port);
char buf[6];


std::string header(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"#!/usr/local/bin/perl\n\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"use Net::Telnet;\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$IP = shift @ARGV;\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet = new Net::Telnet (Port=>23, Timeout=>10, Errmode=>'die');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->open($IP);\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string header2(char *ip){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"#!/usr/local/bin/perl\n\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"use Net::Telnet;\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet = new Net::Telnet (Port=>23, Timeout=>10, Errmode=>'die');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->open('%s');\n",ip);
  rtns=rtns+strbuf;
  return rtns;
}

std::string login(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->waitfor('/User:$/');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->print('admin');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/Password:$/');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->print('');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\>/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string logout(){
   char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('logout');\n");
  rtns=rtns+strbuf;
  //sprintf(strbuf,"$telnet->waitfor('/Would you like to save them now? \\(y\\/n\\) /'); \n");
  //rtns=rtns+strbuf;
  //sprintf(strbuf,"$telnet->print('n');\n");
  //rtns=rtns+strbuf;
  return rtns;
}


std::string show_interface(char *port){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('show interface %s');\n",port);
  rtns=rtns+strbuf;
  sprintf(strbuf,"@lines = $telnet->waitfor('/\\(GSM7212\\) \\>/');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"print @lines;\n");
  rtns=rtns+strbuf;
  return rtns; 
}

std::string macaddrtable(){
  char strbuf[250];
  std::string rtns;
  sprintf(strbuf,"$telnet->print('show mac-addr-table');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"my $line = 0;\nwhile ($line = $telnet->get()) {\n print $line; \nif ($line =~ /More/) {\n$telnet->print('');\n}\nif ($line =~ /\\(GSM7212\\)/) {\nlast;\n}\n}\n");
  rtns=rtns+strbuf;
  return rtns;
}


std::string enable(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('enable');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/Password:$/');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->print('');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string save(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('save');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/Are you sure you want to save \?/');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->print('y');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  rtns=rtns+strbuf;
  return rtns;
}


std::string reload(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('reload');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string exitenable(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('exit');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\>/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlandatabase(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan database');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Vlan\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanid(int vlan){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan %d');\n",vlan);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Vlan\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanname(int vlan,char *name){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan name %d %s');\n",vlan,name);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Vlan\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanmakestatic(int vlan){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan makestatic %d');\n",vlan);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Vlan\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string exitvlan(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('exit');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  rtns=rtns+strbuf;
  return rtns;
}



std::string configure(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('configure');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string logginghost(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('logging host 192.168.10.1 514 6');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string loggingsyslog(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('logging syslog');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string spanningtree(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('spanning-tree');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no spanning-tree');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string portsecurity(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-security');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port-security');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}


std::string snmpservertrapslinkmode(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('snmp-server traps linkmode');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no snmp-server traps linkmode');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string snmptrapslinkstatus(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('snmp trap link-status all');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no snmp trap link-status all');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string mtu(int size){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('mtu %d');\n",size);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string portchanneladminmodeall(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-channel adminmode');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port-channel adminmode');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}
std::string portchannellinktrapall(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-channel linktrap all');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port-channel linktrap all');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string stormcontrolbroadcast(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('storm-control broadcst');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no storm-control broadcast');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string stormcontrolflowcontrol(bool flag){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('storm-control flowcontrol');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no storm-control flowcontrol');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

std::string exitconfig(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('exit');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  rtns=rtns+strbuf;
  return rtns;
}


std::string interface(char *port){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('interface %s');\n",port);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string interfacerange(char *port){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('interface range %s');\n",port);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string adminmode(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('adminmode');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no adminmode');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string portlacpmode(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port lacpmode');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port lacpmode');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string autonegotiate(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('auto-negotiate');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no auto-negotiate');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string speed(char *port,int tspeed){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('speed %d full-duplex');\n",tspeed);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlan(char *port,int tvlan){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan pvid %d');\n",tvlan);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlaningressfilter(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('vlan ingressfilter');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no vlan ingressfilter');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanparticipation(char *port,char *include,int vlan){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan participation %s %d');\n",include,vlan);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanacceptframeall(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('vlan acceptframe all');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no vlan acceptframe all');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string vlanpriority(char *port,int level){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('vlan priority %d');\n",level);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string portsecurity(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-security');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port-security');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;

}

std::string portsecuritymaxdynamic(char *port,int number){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('port-security max-dynamic %d');\n",number);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}


std::string portsecuritymaxsstatic(char *port,int number){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('port-security max-static %d');\n",number);
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string portsecuritymacaddress(bool flag,char *port,char *mac,int tvlan){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-security mac-address %s %d');\n",mac,tvlan);
  }else{
    sprintf(strbuf,"$telnet->print('no port-security mac-address %s %d');\n",mac,tvlan);
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string portchannelstaticenable(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('port-channel static enable');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no port-channel static enable');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}

std::string snmpservertrapsviolation(bool flag,char *port){
  char strbuf[50];
  std::string rtns="";
  if(flag){
    sprintf(strbuf,"$telnet->print('snmp-server traps violation');\n");
  }else{
    sprintf(strbuf,"$telnet->print('no snmp-server traps violation');\n");
  }
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",tport(port));
  rtns=rtns+strbuf;
  return rtns;
}


std::string exitinterface(){
  char strbuf[50];
  std::string rtns="";
  sprintf(strbuf,"$telnet->print('exit');\n");
  rtns=rtns+strbuf;
  sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}

char *tport(char *port){
  int s,p;
  sscanf(port,"%d/%d",&s,&p);
  sprintf(buf,"%d\\/%d",s,p);
  return buf;
}

std::string waitfor(std::string mode,char *port){
  char strbuf[50];
  std::string rtns="";
  if(mode=="enable")sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) #/');\n");
  if(mode=="configure")sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Config\\)#/');\n");
  if(mode=="vlan")sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Vlan\\)#/');\n");
  if(mode=="interface")sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Interface %s\\)#/');\n",port);
  if(mode=="lineconfig")sprintf(strbuf,"$telnet->waitfor('/\\(GSM7212\\) \\(Line\\)#/');\n");
  rtns=rtns+strbuf;
  return rtns;
}


