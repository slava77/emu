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

void script_parse(char *fname);
int compare(char *a,char *b,int begin,int length);
void rename(char *fname,char *outname);

int main(int argc,char *argv[]){
  if(argc!=2){
    std::cout << " usage: ./SwitchPerlClearMacs <filename> " << std::endl;
    return 0;
  }
  char fname[128];
  for(int i=0;i<128;i++)fname[i]=argv[1][i];
  std::cout << "input file:" << fname << std::endl;
  script_parse(fname);
  return 0;
}

void script_parse(char *fname){
char line[128];
FILE *file;
  char outname[128];
  rename(fname,outname);
  std::ofstream f;
  f.open(outname);
  file=fopen(fname,"r");
  f << header();
  f << login();
  f << enable();
  char port[5]="";
  std::string mode="enable";
  std::string lastmode="enable";
  while(fgets(line,128,file)){
     printf("---- %s",line);
     for(int i=0;i<128;i++)if(line[i]=='\n'){line[i]='\0';break;}
     if(line[0]=='!')continue;            
     // remove network commands
     if(line[0]=='n'&&line[1]=='e'&&line[2]=='t')continue;
     if(compare("configure",line,0,9)==9){
       //printf(" *** configure \n");
       lastmode=mode;
       mode="configure";
     }
     if(compare("interface",line,0,9)==9){
       //printf(" *** interface \n");
       char tport[5];
       for(int i=0;i<2;i++)tport[i]=line[13+i];tport[3]='\0';
       int p;
       sscanf(tport,"%d",&p);
       sprintf(port,"0\\/%d",p);
       lastmode=mode;
       mode="interface";
     } 
     if(compare("vlan database",line,0,13)==13){
       //printf(" *** vlan database \n");
       lastmode=mode;
       mode="vlan";
     } 
     if(compare("lineconfig",line,0,10)==10){
       //printf(" *** lineconfig \n");
       lastmode=mode;
       mode="lineconfig";
     }
     if(compare("exit",line,0,4)==4){
       //printf(" *** exit \n");
       if(mode=="configure")mode="enable";
       if(mode=="interface")mode="configure";
       if(mode=="vlan")mode="enable";
       if(mode=="lineconfig")mode="configure";
     }
     char strbuf[60];
     sprintf(strbuf,"$telnet->print('%s');\n",line);
     f << strbuf;
     f << waitfor(mode,port);
  }
  fclose(file);
  f << exitenable();
  f << logout();
  f.close();
}

int compare(char *a,char *b,int begin,int length){
  int j=0;
  for(int i=begin;i<begin+length;i++){
    if(a[i]==b[i])j=j+1;
  }
  return j;
}

 void rename(char *fname,char *outname){
   int start,end;
   start=0;
   for(int i=0;i<128;i++)if(fname[i]=='.'&&fname[i+1]=='s'&&fname[i+2]=='c'&&fname[i+3]=='r')end=i;
   for(int i=0;i<end;i++)if(fname[i]=='/')start=i+1;
   std::cout << "fname " << fname << " " << end << " " << start << std::endl;
   for(int i=start;i<end+1;i++)outname[i-start]=fname[i];
   outname[end+1-start]='p';
   outname[end+2-start]='l';
   outname[end+3-start]='\0';
 }
   
