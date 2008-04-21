
#ifndef _SetKillFibersGUI_h_
#define _SetKillFibersGUI_h_

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
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/FormFile.h"

#include "ConnectionsDB.h"

using namespace cgicc;
using namespace std;


class SetKillFibersGUI: public xdaq::Application
{
 public:
  XDAQ_INSTANTIATOR();

  ConnectionsDB *condb;
  xdata::String CallBack_;
  std::string val;
  std::string ToPrint;
  std::string ToPrint2;
  int tddus;
  int Live[4][9][15];


SetKillFibersGUI(xdaq::ApplicationStub * s)throw (xdaq::exception::Exception): xdaq::Application(s) 
{	
  xgi::bind(this,&SetKillFibersGUI::Default, "Default");
  xgi::bind(this,&SetKillFibersGUI::MainPage, "MainPage");
  xgi::bind(this,&SetKillFibersGUI::CallBack, "CallBack");
  xgi::bind(this,&SetKillFibersGUI::CallBack2, "CallBack2");
  xgi::bind(this,&SetKillFibersGUI::CallBack3, "CallBack3");

  condb = new ConnectionsDB();

  for(unsigned int i=0;i<4;i++){
    for(unsigned int j=0;j<9;j++){
      for(unsigned int k=0;k<15;k++){
        Live[i][j][k]=0;
      }
    }
  }
  ToPrint=" 0 total ddus ";
  ToPrint2=" ";
  tddus=0;
  val="";
}  

void Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}

void MainPage(xgi::Input * in, xgi::Output * out ) {

   std::string CallBack =
      toolbox::toString("/%s/CallBack",getApplicationDescriptor()->getURN().c_str());

   MyHeader(in,out,"Set DDU Kill Fiber Flags");

  
   char buf[40];
   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"DDU: add or kill fibers");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","center") << cgicc::p() << std::endl ;
   *out << cgicc::br();
   *out << cgicc::h4();
   *out << cgicc::span().set("style","color:green;background-color:white");
   *out << cgicc::pre();
   *out << " Examples of Valid Command Types: " << std::endl << std::endl;
   *out << "     add ALL" << std::endl;
   *out << "     kill ALL" << std::endl;
   *out << "     kill VME+3/03" << std::endl;
   *out << "     add ME+1/1/06" << std::endl;
   *out << "     add DDU1/06/01" << std::endl;
   *out << "     kill FCTC2/2/f" << std::endl << std::endl;
   *out << "     Partial Strings: VME+3 ME+1/1 ME+1 DDU1/06 FCTC2/2 " << std::endl << std::endl;
   *out << cgicc::pre() << std::endl;
   *out << cgicc::span() << std::endl;
   *out <<  cgicc::h4() << std::endl;

    *out << "Enter: "; 
    *out << cgicc::form().set("method","GET").set("action",CallBack) << std::endl ;
    *out << cgicc::input().set("type","textdata").set("name","CallBack").set("size","20").set("value",val) << std::endl ;
	sprintf(buf,"1");
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","id");
    *out << cgicc::form() << std::endl ;
     *out << cgicc::h3();
    *out << pre();
    //   *out << cgicc::span().set("style","color:black;background-color:blue");
    *out << ToPrint << std::endl;
    *out << cgicc::pre();
    *out << cgicc::h3() << std::endl;
    *out << cgicc::fieldset() <<std::endl;
   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"DDU: output xml file");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","center") << cgicc::p() << std::endl ;
   *out << cgicc::br();
  std::string CallBack2 = toolbox::toString("/%s/CallBack2",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CallBack2) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Write xml Files") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::fieldset() <<std::endl;
   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"DDU: show live chambers ");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","center") << cgicc::p() << std::endl ;
   *out << cgicc::br();
  std::string CallBack3 = toolbox::toString("/%s/CallBack3",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CallBack3) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","List Live Chambers") << std::endl ;
  *out << cgicc::form() << std::endl ;
    *out << cgicc::h3();
    *out << pre();;
    *out << ToPrint2 << std::endl;
    *out << cgicc::pre();
    *out << cgicc::h3() << std::endl;
  *out << cgicc::fieldset() <<std::endl;

}

void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << std::endl;
  //
  std::string myUrn = getApplicationDescriptor()->getURN().c_str();
  xgi::Utils::getPageHeader(out,title,myUrn,"","");
  //
}

void CallBack(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  cgicc::Cgicc cgi(in);

  int form_id = cgi["id"]->getIntegerValue();
  CallBack_= cgi["CallBack"]->getValue() ;
  std::string val=CallBack_;
  std::cout << "FORM_ID " << form_id << std::endl;
  std::cout << "CallBack " << val << std::endl;
  if(form_id==1){
  int command;
  std::string val2=" ";
  if(val.compare(0,4,"add ",0,4)==0){
     command=1;
     val2=val.substr(4,val.size()-4);
  }
  if(val.compare(0,5,"kill ",0,5)==0){
     command=-1;
     val2=val.substr(5,val.size()-5);
  }
  std::cout << " command " << command << std::endl; 
  int n = val2.find(" ",0);
  if(n>0)val2.replace(n,1,"+");
  std::cout << " val2 " << val2 << std::endl;
  if(command!=0){
    if(val2.compare("ALL")==0){
      All(command,val2);
    }else{
      Process(command,val2);
    }
  } 
  }

  this->Default(in,out);
}

 void All(int command,std::string val)
{
  ToPrint=" ";
  int set;
  if(command==1){set=1;}else{set=0;}
  tddus=0;
  for(unsigned int i=0;i<4;i++){
    for(unsigned int j=0;j<9;j++){
      for(unsigned int k=0;k<15;k++){
        Live[i][j][k]=set;
        if(set==1)tddus=tddus+1;
      }
    }
  }
  // remove ME+4/2 ME-4/2
  if(set==1){
    std::string temp;
    temp="ME+4/2";
    Process(-1,temp);
    temp="ME-4/2";
    Process(-1,temp);
  }
  std::ostringstream dum;
  int ddus=tddus;
  dum << tddus << " total ddus " << ddus << " chambers added " << std::endl;
  ToPrint=dum.str();
}

 void Process(int command,std::string val)
{
  ToPrint=" ";
  int set;
  if(command==1){set=1;}else{set=0;}

  std::string temp=condb->NametoDump(val);
  std::cout << temp << std::endl;
  // process temp
  int ddus=0;
  int n;
  int m=0;
  while(((n=temp.find("DDU",m))>=0)&&ddus<540){
    ddus=ddus+1;
    m=n+3; 
    std::string crate=temp.substr(m,1);
    m=m+2;
    int l=temp.find('/',m);
    std::string slot=temp.substr(m,l-m); 
    m=l+1;
    std::string fiber=temp.substr(m,2);
    std::cout << " crate " << crate << " slot " << slot <<" fiber " <<  fiber << std::endl;
    int cr,sl,fi;
    sscanf(crate.c_str(),"%d",&cr);
    cr=cr-1;
    sscanf(slot.c_str(),"%d",&sl);
    sscanf(fiber.c_str(),"%d",&fi);
    int slot2ddu[14]={0,0,0,0,0,1,2,3,0,4,5,6,7,8};
    Live[cr][slot2ddu[sl]][fi]=set;
  }
  tddus=0;
    for(unsigned int i=0;i<4;i++){
      for(unsigned int j=0;j<9;j++){
        for(unsigned int k=0;k<15;k++){
          if(Live[i][j][k]==1)tddus=tddus+1;
        }
      }
    }
    std::ostringstream dum;
    dum << tddus << " total ddus " << ddus << " chambers added " << std::endl;
    ToPrint=dum.str();
}

void CallBack2(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {

  cgicc::Cgicc cgi(in);
  std::cout << " CallBack2 call" << std::endl;
  // std::string DDUXMLDir = getenv("$BUILD_HOME");
  std::string DDUXMLDir = "/nfshome0/cscpro";
  //  std::string DDUXMLDir = "/home/cscpcnew";
  DDUXMLDir += "/config/fed/";
  std::string DDUXMLFile=""; 
  std::string DDUXMLNewFile="";
  std::string Crate;
  std::string Slot;
  std::string KillFiber;
  int slot2ddu[14]={0,0,0,0,0,1,2,3,0,4,5,6,7,8};
  for(int tcrate=1;tcrate<5;tcrate++){
  int crate=-99;
  int slot=-99;
  int killfiber=0xf3ffff;
  if(tcrate==1){
    DDUXMLFile = DDUXMLDir+"config_c1_allCSCs.xml";
    DDUXMLNewFile =DDUXMLDir+"config_c1_SetKillFibersGUI.xml";
  }
  if(tcrate==2){
    DDUXMLFile = DDUXMLDir+"config_c2_allCSCs.xml";
    DDUXMLNewFile =DDUXMLDir+"config_c2_SetKillFibersGUI.xml";
  }
  if(tcrate==3){
    DDUXMLFile = DDUXMLDir+"config_c3_allCSCs.xml";
    DDUXMLNewFile =DDUXMLDir+"config_c3_SetKillFibersGUI.xml";
  }
  if(tcrate==4){
    DDUXMLFile = DDUXMLDir+"config_c4_allCSCs.xml";
    DDUXMLNewFile =DDUXMLDir+"config_c4_SetKillFibersGUI.xml";
  }
  std::ifstream Readfile;
  Readfile.open(DDUXMLFile.c_str());
  std::ofstream Writefile; 
  Writefile.open(DDUXMLNewFile.c_str());
  if ( Readfile.is_open()&& Writefile.is_open()) {
  while ( Readfile.good() ) {
	std::string line;
	std::getline(Readfile,line);
	std::cout << line << std::endl;
        int n=line.find("Crate=",0);
        if(n>0){
          int m=line.find('"',n+7);
          Crate=line.substr(n+7,m-n-7);
          sscanf(Crate.c_str(),"%d",&crate);
	  //  std::cout << "Crate " << crate << std::endl;
        }
        n=line.find("DDU slot=",0);
        if(n>0){
          int m=line.find('"',n+10);
          Slot=line.substr(n+10,m-n-10);
          sscanf(Slot.c_str(),"%d",&slot);
	  // std::cout << "Slot " << slot << std::endl;
        }
        n=line.find("killfiber=",0);
        if(n>0){
          int m=line.find('"',n+11);
          KillFiber=line.substr(n+11,m-n-11);
          sscanf(KillFiber.c_str(),"%06x",&killfiber);
	  // std::cout << "KillFiber " << hex << killfiber<< dec  << std::endl;
          unsigned int newkill=0xf0000;
          for(int fibr=0;fibr<15;fibr++){
	    if(Live[crate-1][slot2ddu[slot]][fibr]==1)newkill=newkill|(0x00000001<<fibr);
          }
          std::ostringstream dum;
          dum<<hex<<newkill <<dec;
	  std::string NewKill = dum.str();
	  //std::cout <<" NewKill " << NewKill << std::endl;
          line.replace(n+11,m-n-11,NewKill);
        }
        Writefile << line << std::endl;
  }
  }
  Readfile.close();
  Writefile.close();
  }
  system("cd /nfshome0/cscpro/config/fed;rm -f config_c1.xml;ln -s config_c1_SetKillFibersGUI.xml config_c1.xml");
  system("cd /nfshome0/cscpro/config/fed;rm -f config_c2.xml;ln -s config_c2_SetKillFibersGUI.xml config_c2.xml");
  system("cd /nfshome0/cscpro/config/fed;rm -f config_c3.xml;ln -s config_c3_SetKillFibersGUI.xml config_c3.xml");
  system("cd /nfshome0/cscpro/config/fed;rm -f config_c4.xml;ln -s config_c4_SetKillFibersGUI.xml config_c4.xml"); 
  //  system("cd /home/cscpcnew/config/fed;rm -f config_c1.xml;ln -s config_c1_SetKillFibersGUI.xml config_c1.xml");
  // system("cd /home/cscpcnew/config/fed;rm -f config_c2.xml;ln -s config_c2_SetKillFibersGUI.xml config_c2.xml");
  //  system("cd /home/cscpcnew/config/fed;rm -f config_c3.xml;ln -s config_c3_SetKillFibersGUI.xml config_c3.xml");
  //  system("cd /home/cscpcnew/config/fed;rm -f config_c4.xml;ln -s config_c4_SetKillFibersGUI.xml config_c4.xml");
  system("cd /nfshome0/cscpro/config/fed;Make_c12;Make_c34");
  this->Default(in,out);
}

void CallBack3(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  int count=0;
  int ddu2slot[9]={4,5,6,7,9,10,11,12,13};
  cgicc::Cgicc cgi(in);
  std::cout << " CallBack3 call" << std::endl;
  std::ostringstream dum;
  for(int crate=0;crate<4;crate++){ 
    for(int sddu=0;sddu<9;sddu++){
      for(int fiber=0;fiber<15;fiber++){
        if(Live[crate][sddu][fiber]==1){
          count=count+1;
	  std::string temp=ddu_str(crate+1,ddu2slot[sddu],fiber);
          dum << count <<" "  << condb->NametoDump(temp);
        }
      }
    }
  }
  ToPrint2=dum.str();
  this->Default(in,out);
}


std::string ddu_str(int crate,int slot,int fiber)
{
      std::ostringstream dum;
      std::string b1="";
      std::string b2="";
      if(slot<10)b1="0";
      if(fiber<10)b2="0";
      dum<<"DDU"<<crate<<"/" << b1 << slot<<"/"<< b2 << fiber;     
      return dum.str();
}

 protected:


};

#endif
