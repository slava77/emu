#ifndef _ConnectionsGUI_h_
#define _ConnectionsGUI_h_

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

class ConnectionsGUI: public xdaq::Application
{
 public:
  XDAQ_INSTANTIATOR();

  ConnectionsDB *condb;
  xdata::String CallBack_;
  std::string val;
  std::string ToPrint1;
  std::string ToPrint2;

ConnectionsGUI(xdaq::ApplicationStub * s)throw (xdaq::exception::Exception): xdaq::Application(s) 
{	
  xgi::bind(this,&ConnectionsGUI::Default, "Default");
  xgi::bind(this,&ConnectionsGUI::MainPage, "MainPage");
  xgi::bind(this,&ConnectionsGUI::CallBack, "CallBack");
 
  condb = new ConnectionsDB();

  val="";
}  

void Default(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/" <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}

void MainPage(xgi::Input * in, xgi::Output * out ) {

   std::string CallBack =
      toolbox::toString("/%s/CallBack",getApplicationDescriptor()->getURN().c_str());

   MyHeader(in,out,"Fiber Cable Connections/Translations");

  
   char buf[40];
   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"DDU: Crate/Chamber/FCTC");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","center") << cgicc::p() << std::endl ;
   *out << cgicc::br();
   *out << cgicc::h4();
   *out << cgicc::span().set("style","color:green;background-color:white");
   *out << cgicc::pre();
   *out << " Examples of Valid Search Types: " << std::endl << std::endl;
   *out << "     Crate:    VME+3/03" << std::endl;
   *out << "     Chamber:  ME+1/1/06" << std::endl;
   *out << "     DDU:      DDU1/06/01" << std::endl;
   *out << "     Cassette: FCTC2/2/f" << std::endl << std::endl;
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
    *out << ToPrint1 << std::endl;
    *out << cgicc::pre();
    *out << cgicc::h3() << std::endl;
    *out << cgicc::fieldset() <<std::endl;

   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
   sprintf(buf,"VMECC: Switch/MAC/FCTC/Chamber");
   *out << cgicc::legend(buf).set("style","color:blue").set("align","center") << cgicc::p() << std::endl ;
   *out << cgicc::br();
   *out << cgicc::h4();
   *out << cgicc::span().set("style","color:green;background-color:white");
   *out << cgicc::pre();
   *out << " Examples of Valid Search Types: " << std::endl << std::endl;
   *out << "     Crate:    VME+4/1" << std::endl;
   *out << "     Switch:   SWITCH1/5" << std::endl;
   *out << "     MAC:      02:00:00:00:00:0D" << std::endl;;
   *out << "     Cassette: FCTC7/5 (always a-position)" << std::endl;
   *out << "     Partial Strings: VME+4 VME+ SWITCH1 02 FCTC7:" << std::endl << std::endl;
   *out << cgicc::pre() << std::endl;
   *out << cgicc::span() << std::endl;
   *out <<  cgicc::h4() << std::endl;

    *out << "Enter: "; 
    *out << cgicc::form().set("method","GET").set("action",CallBack) << std::endl ;
    *out << cgicc::input().set("type","textdata").set("name","CallBack").set("size","20").set("value",val) << std::endl ;
	sprintf(buf,"2");
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","id");
    *out << cgicc::form() << std::endl ;
 
    *out << cgicc::h3();
    *out << pre();
    //   *out << cgicc::span().set("style","color:black;background-color:blue");
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
  int n = val.find(" ",0);
  if(n>0)val.replace(n,1,"+");
  std::cout << "FORM_ID " << form_id << std::endl;
  std::cout << "CallBack " << val << std::endl;
  if(form_id==1){
    if(val.size()>2){
        ToPrint1=condb->NametoDump(val);
     }else{
        ToPrint1=" ";
     }
     if(ToPrint1.size()==0){
        std::ostringstream dum;
        dum<<val<<":Not Found in DB";
        ToPrint1=dum.str();
     }
      ToPrint2="";
  }
  if(form_id==2){
    if(val.size()>2){
       ToPrint2=condb->sNametoDump(val);
    }else{
      ToPrint2=" ";
    }
    if(ToPrint2.size()==0){
      std::ostringstream dum;
      dum<<val<<":Not Found in DB";
      ToPrint2=dum.str();
    }
    ToPrint1=" ";
  }

  this->Default(in,out);
}

 protected:


};

#endif
