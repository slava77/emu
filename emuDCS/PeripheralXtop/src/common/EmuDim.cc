// $Id: EmuDim.cc,v 1.23 2009/07/19 13:25:01 liu Exp $

#include "emu/x2p/EmuDim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <iomanip>
#include <sstream>

namespace emu {
  namespace x2p {

EmuDim::EmuDim(xdaq::ApplicationStub * s): xdaq::WebApplication(s)
{
  HomeDir_     = getenv("HOME");
  ConfigDir_   = HomeDir_+"/config/";
  //
  PeripheralCrateDimFile_  = "";
  BadChamberFile_ = "";
  XmasDcsUrl_ = "";
  BlueDcsUrl_ = "";
  FedcDcsUrl_ = "";
  TestPrefix_ = "";
  OpMode_ = 0;

  xgi::bind(this,&EmuDim::Default, "Default");
  xgi::bind(this,&EmuDim::MainPage, "MainPage");
  //
  current_state_ = 0;
  getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &PeripheralCrateDimFile_);
  getApplicationInfoSpace()->fireItemAvailable("BadChamberFileName", &BadChamberFile_);
  getApplicationInfoSpace()->fireItemAvailable("XmasDcsUrl", &XmasDcsUrl_ );
  getApplicationInfoSpace()->fireItemAvailable("BlueDcsUrl", &BlueDcsUrl_ );
  getApplicationInfoSpace()->fireItemAvailable("FedcDcsUrl", &FedcDcsUrl_ );
  getApplicationInfoSpace()->fireItemAvailable("TestPrefix", &TestPrefix_ );
  getApplicationInfoSpace()->fireItemAvailable("OperationMode", &OpMode_ );

  // everything below for Monitoring
  timer_ = toolbox::task::getTimerFactory()->createTimer("EmuMonitorTimer");
  timer_->stop();
  inited=false;
  Monitor_On_ = false;
  Monitor_Ready_ = false;
  In_Monitor_ = false;
  Suspended_ = false;
  fastloop=0;
  slowloop=0;
  extraloop=0;
  getApplicationInfoSpace()->fireItemAvailable("FastLoop", &fastloop);
  getApplicationInfoSpace()->fireItemAvailable("SlowLoop", &slowloop);
  getApplicationInfoSpace()->fireItemAvailable("ExtraLoop", &extraloop);
  xoap::bind(this, &EmuDim::SoapStart, "SoapStart", XDAQ_NS_URI);
  xoap::bind(this, &EmuDim::SoapStop, "SoapStop", XDAQ_NS_URI);
  xgi::bind(this,&EmuDim::ButtonStart      ,"ButtonStart");
  xgi::bind(this,&EmuDim::ButtonStop      ,"ButtonStop");
  xgi::bind(this,&EmuDim::SwitchBoard      ,"SwitchBoard");

  for(int i=0; i<30; i++) 
  {  crate_state[i]=0; 
     crate_name[i]=""; 
  }
}  

xoap::MessageReference EmuDim::SoapStart (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     Start();
     return createReply(message);
}

xoap::MessageReference EmuDim::SoapStop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     Stop();
     return createReply(message);
}

void EmuDim::timeExpired (toolbox::task::TimerEvent& e)
{

     if(! Monitor_On_ ) return;
     if( In_Monitor_ ) return;
     In_Monitor_ = true;

     // always check DimCommand
     CheckCommand();

     // if( Suspended_ ) return;
     std::string name = e.getTimerTask()->name;
     // std::cout << "timeExpired: " << name << std::endl;
     if(strncmp(name.c_str(),"EmuDimRead",13)==0) 
     {  if( !Suspended_ ) ReadFromXmas();
        UpdateAllDim();
     }
     In_Monitor_ = false;
}
//
void EmuDim::Start () 
{
     if(!Monitor_Ready_)
     {
         toolbox::TimeInterval interval1, interval2, interval3;
         toolbox::TimeVal startTime;
         startTime = toolbox::TimeVal::gettimeofday();

         if(!inited) Setup();
         timer_->start(); // must activate timer before submission, abort otherwise!!!
         if(fastloop) 
         {   interval1.sec((time_t)fastloop);
             timer_->scheduleAtFixedRate(startTime,this, interval1, 0, "EmuDimCmnd" );
             std::cout << "Dim Command scheduled" << std::endl;
             ::sleep(2);
         }
         if(slowloop) 
         {   interval2.sec((time_t)slowloop);
             timer_->scheduleAtFixedRate(startTime,this, interval2, 0, "EmuDimRead" );
             std::cout << "XMAS Read scheduled" << std::endl;
         }
         if(extraloop) 
         {   interval3.sec((time_t)extraloop);
             timer_->scheduleAtFixedRate(startTime,this, interval3, 0, "EmuDimExtra" );
             std::cout << "extra scheduled" << std::endl;
         }
         Monitor_Ready_=true;
     }
     Monitor_On_=true;
     Suspended_ = false;
     std::cout<< getLocalDateTime() << " EmuDim Started " << std::endl;
}

void EmuDim::Stop () 
{
     if(Monitor_On_)
     {
         Monitor_On_=false;
         std::cout << getLocalDateTime() << " EmuDim stopped " << std::endl;
     }
}

void EmuDim::ButtonStart(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     Start();
     this->Default(in,out);
}

void EmuDim::ButtonStop(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
     Stop();
     this->Default(in,out);
}

void EmuDim::SwitchBoard(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    Page=cgiEnvi.getQueryString();
    std::string command_name=Page.substr(0,Page.find("=", 0) );
    std::string command_argu=Page.substr(Page.find("=", 0)+1);

    if (command_name=="STATUS")
    {
       if(Monitor_On_) *out << "ON ";
       else *out << "OFF ";

       if(Suspended_) *out << "SUSPENDED" << std::endl;
       else *out << "READING" << std::endl;

    }
    else if (command_name=="READING")
    {
       if(command_argu=="ON" || command_argu=="on")
       {   
           XmasLoader->reload(xmas_start);
           Suspended_ = false;
           std::cout << getLocalDateTime() << " SwitchBoard: Resume Reading" << std::endl;
       }
       else if (command_argu=="OFF" || command_argu=="off")
       { 
           XmasLoader->reload(xmas_stop);
           Suspended_ = true;
           std::cout << getLocalDateTime() << " SwitchBoard: Stop Reading" << std::endl;
       }
    }
    else
    {
       std::cout << getLocalDateTime() << " Unknown command: " << command_name << " " << command_argu << std::endl;
    }
}

void EmuDim::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuDim::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuDim");
  //
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("X2P Status: On")) << cgicc::span() << std::endl ;
     std::string TurnOffMonitor = toolbox::toString("/%s/ButtonStop",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOffMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn Off X2P") << std::endl ;
     *out << cgicc::form() << std::endl ;
  } else
  {
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("X2P Status: Off")) << cgicc::span() << std::endl ;
     std::string TurnOnMonitor = toolbox::toString("/%s/ButtonStart",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOnMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn On X2P") << std::endl ;
     *out << cgicc::form() << std::endl ;
  }
  if(Suspended_) 
  {  *out << cgicc::span().set("style","color:yellow")
          << cgicc::b("XMAS suspended") << cgicc::span() << std::endl;
  }
  //
}
//
void EmuDim::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << h2(title) << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  //const CgiEnvironment& env = cgi.getEnvironment();
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN();
  xgi::Utils::getPageHeader(out,title,myUrl,myUrn,"");
  //
}

void EmuDim::Setup()
{
   std::string fn=PeripheralCrateDimFile_;
   int ch=ReadFromFile(fn.c_str());
   if(ch>0 && ch<TOTAL_CHAMBERS) 
      StartDim(ch);
   else
      std::cout << "ERROR in read file " << fn << " DIM services cannot start." << std::endl;
   XmasLoader = new LOAD();
   BlueLoader = new LOAD();
   FedcLoader = new LOAD();

   xmas_root=XmasDcsUrl_;
   xmas_load=xmas_root + "/DCSOutput";
   xmas_start=xmas_root + "/MonitorStart";
   xmas_stop=xmas_root + "/MonitorStop";
   xmas_info=xmas_root + "/SwitchBoard";
   XmasLoader->init(xmas_load.c_str());

   blue_root=BlueDcsUrl_;
   blue_info=blue_root + "/SwitchBoard";
   BlueLoader->init(blue_info.c_str());

   fedc_root=FedcDcsUrl_;
   fedc_load=fedc_root + "/DCSOutput";
   FedcLoader->init(fedc_load.c_str());
   inited=true;
}

int EmuDim::ReadFromFile(const char *filename)
{
   FILE * fl;
   char *buffer;
   int readsize, ch=0;

   if(strlen(filename)==0) return 0;
   fl=fopen(filename, "r");
   if(fl==NULL) return 0;
   buffer=(char *)malloc(100000);
   if(buffer==NULL) return 0;
   // then fill the structure
   readsize=fread(buffer, 1, 100000, fl);
   if(readsize>40) ch=ParseTXT(buffer, readsize, 1);
   free(buffer);
   fclose(fl);
   std::cout << ch << " Chambers read from file " << filename << std::endl;
   return ch;
}

int EmuDim::ReadFromXmas()
{
  int ch=0;
   // read
   XmasLoader->reload(xmas_load);

   // then fill the structure
   ch=ParseTXT(XmasLoader->Content(), XmasLoader->Content_Size(), 0);
   std::cout << ch << " Chambers read at " << getLocalDateTime() << std::endl;
   // 
   return ch;
}

int EmuDim::ParseTXT(char *buff, int buffsize, int source)
{
//
// source==0  from Xmas
//       ==1  from file
//
   int chmbs=0;
   bool more_line = true;
   char * start = buff;
   char *endstr;
   unsigned char *bbb=(unsigned char *)buff;

   if(buffsize < 100) return 0;
   for(int i=0; i<buffsize; i++)
   {  if((bbb[i]< 0x20 || bbb[i]>0x7e) && bbb[i]!=0x0a)
      { std::cout << "ERROR at " << i << " " << std::hex << (int)(bbb[i]) << std::dec << std::endl;
        bbb[i]=0x0a;
      }
   }
   do
   {
       endstr=strchr(start, '\n');
       if(endstr==NULL || ((endstr-buff)>buffsize)) more_line=false;
       else
       {
           *endstr=0;
           chmbs += FillChamber(start, source);
       }
       start = endstr+1;
   } while(more_line);
   return chmbs;
}

int EmuDim::FillChamber(char *buff, int source)
{
   char *content;
   char * endstr;
   std::string label;

   if(strlen(buff) < 100) return 0;
   endstr=strchr(buff, ' ');
   if(endstr==NULL) return 0;
   *endstr=0;
   int chnumb=ChnameToNumber(buff);
   label=buff;
//   std::cout << "Found chamber " << label << " with number " << chnumb << std::endl; 
   content = endstr+1;
   if(strlen(content)>430) std::cout<< label << " WARNING " << content << std::endl;
   if(chnumb>=0 && chnumb <TOTAL_CHAMBERS)
   {   if(source) chamb[chnumb].SetLabel(label);
       chamb[chnumb].Fill(content, source);
       return 1;
   }
   else
   {   std::cout << "WRONG tag " << label << std::endl;
       return 0;
   }
}

int EmuDim::ChnameToNumber(const char *chname)
{
   if(strlen(chname)<7) return -1;
   if(strncmp(chname,"ME",2)) return -2;
   int station = std::atoi(chname+3);
   int ring = std::atoi(chname+5);
   int chnumb = std::atoi(chname+7);
   if(ring<1 || ring>3) return -3;
   switch(station)
   {   case 1:
         return     (ring-1)*36 + chnumb-1;
       case 2:
         return 108+(ring-1)*18 + chnumb-1;
       case 3:
         return 162+(ring-1)*18 + chnumb-1;
       case 4:
         return 216+(ring-1)*18 + chnumb-1;
       default:
         return -3;
   }
}

int EmuDim::CrateToNumber(const char *chname)
{
   if(strlen(chname)<7) return -1;
   if(strncmp(chname,"VME",2)) return -2;
   int station = std::atoi(chname+4);
   int chnumb = std::atoi(chname+6);
   if(chnumb>12 || chnumb<1) return -3;

   if (station >4 || station<1) return -4;
   else
   {  int cr = chnumb-1;
      if (station>1) cr += station*6;
      if(crate_name[cr]=="") crate_name[cr] = chname;
      return cr;
   }
}

void EmuDim::StartDim(int chs)
{
   int total=0, i=0;
   std::string dim_lv_name, dim_temp_name, dim_command, dim_server, pref;

   pref=TestPrefix_;
   while(total<chs && i < TOTAL_CHAMBERS)
   {
      if(chamb[i].Ready())
      {
         chamb[i].GetDimLV(1, &(EmuDim_lv[i]));
         chamb[i].GetDimTEMP(1, &(EmuDim_temp[i]));
         dim_lv_name = pref + "LV_1_" + chamb[i].GetLabel(); 
         dim_temp_name = pref + "TEMP_1_" + chamb[i].GetLabel(); 

         LV_1_Service[i]= new DimService(dim_lv_name.c_str(),"F:5;F:5;F:5;F:5;F:5;F:5;F:5;F:9;I:2;C:80",
           &(EmuDim_lv[i]), sizeof(LV_1_DimBroker));
         TEMP_1_Service[i]= new DimService(dim_temp_name.c_str(),"F:7;I:2;C:80",
           &(EmuDim_temp[i]), sizeof(TEMP_1_DimBroker));

         total++;
      }
      i++;
   }
   Confirmation_Service = new DimService("LV_CONFIRMATION_SERVICE","C:80", (void *)&pvssrespond, sizeof(pvssrespond));

   dim_command = pref + "LV_1_COMMAND";
   LV_1_Command = new DimCommand( dim_command.c_str(),"C");
   dim_server = pref + "Emu-Dcs Dim Server";
   DimServer::start(dim_server.c_str());

//   std::string confirm_cmd = pref + "LV_1_COMMAND_CONFIRMATION";
//   DimClient::sendCommand( confirm_cmd.c_str(), "SOFT_START");
   strcpy(pvssrespond.command, "SOFT_START");
   Confirmation_Service->updateService();
 
   std::cout << total << " DIM serives";
   if(pref!="") std::cout << " ( with prefix " << pref << " )";
   std::cout << " started at " << getLocalDateTime() << std::endl;
}

int EmuDim::UpdateDim(int ch)
{
   int mode = OpMode_;
   if(mode<=0) mode = 2;
   if(ch>=0 && ch < TOTAL_CHAMBERS && chamb[ch].Ready())
   {
         chamb[ch].GetDimLV(mode, &(EmuDim_lv[ch]));
         chamb[ch].GetDimTEMP(mode, &(EmuDim_temp[ch]));
     try 
     {
         if(LV_1_Service[ch]) LV_1_Service[ch]->updateService();
         if(TEMP_1_Service[ch]) TEMP_1_Service[ch]->updateService();
     } catch (...) {}
         return 1;
   }
   else return 0;
}

void EmuDim::UpdateAllDim()
{
   int j=0;

   for(int i=0; i < TOTAL_CHAMBERS; i++)
   {
      j += UpdateDim(i);
   }
   // std::cout << j << " Dim services updated" << std::endl;
}

void EmuDim::CheckCommand()
{
   if(LV_1_Command==NULL) return;
   bool start_powerup=false;
   while(LV_1_Command->getNext())
   {
      std::string cmnd = LV_1_Command->getString();
      std::cout << getLocalDateTime() << " Dim Command:" << cmnd << std::endl;
      if(cmnd.substr(0,10)=="STOP_SLOW_")
      {
         XmasLoader->reload(xmas_stop);
         Suspended_ = true;
         start_powerup=false;
         for(int i=0; i<TOTAL_CRATES; i++)
         {  
            crate_state[i]=0;
         }
      }
      else if(cmnd.substr(0,12)=="RESUME_SLOW_")
      {
         XmasLoader->reload(xmas_start);
         Suspended_ = false;
         start_powerup=false;
         for(int i=0; i<TOTAL_CRATES; i++)
         {
            crate_state[i]=0;
         }
      }
      else if(cmnd.substr(cmnd.length()-8,8)=="get_data")
      {
         int ch=ChnameToNumber(cmnd.c_str());
         if(ch>=0 && ch<TOTAL_CHAMBERS) UpdateDim(ch);
      }
      else if(cmnd.substr(0,13)=="PREPARE_POWER")
      {
         int cr=CrateToNumber(cmnd.substr(17).c_str());
         if(cr>=0 && cr<TOTAL_CRATES) crate_state[cr] = 1;
         if(!Suspended_)
         {
            XmasLoader->reload(xmas_stop);
            Suspended_ = true;  
         }
      }
      else if(cmnd.substr(0,13)=="EXECUTE_POWER")
      {
         start_powerup=true;
      }
      // all other commands are ignored
   }
   if(start_powerup) PowerUp();
}

int EmuDim::PowerUp()
{
   std::string pref = TestPrefix_;
   std::string confirm_cmd = pref + "LV_1_COMMAND_CONFIRMATION";
   std::cout << "Start Power Up" << std::endl;
   for(int i=0; i<TOTAL_CRATES; i++)
   {  
      if(crate_state[i]==1) 
      {  BlueLoader->reload(blue_info+"?POWERUP="+crate_name[i]);
         // check return message
         if(BlueLoader->Content_Size() > 27)
         {
           if(strncmp(BlueLoader->Content(), "Power Up Successful",19)==0)
           {
              crate_state[i] = 0;
              std::string confirm = "INIT_IS_DONE;" + crate_name[i];              
              strcpy(pvssrespond.command, confirm.c_str());
              Confirmation_Service->updateService();
              std::cout << getLocalDateTime() << " Return: " << confirm << std::endl;
           }
           else
           {
              std::cout << getLocalDateTime() << " Init failed: " << crate_name[i] << std::endl;
           }
         }
      }
   }
   return 0;
}

xoap::MessageReference EmuDim::createReply(xoap::MessageReference message)
                throw (xoap::exception::Exception)
{
        std::string command = "";

        DOMNodeList *elements =
                        message->getSOAPPart().getEnvelope().getBody()
                        .getDOMNode()->getChildNodes();

        for (unsigned int i = 0; i < elements->getLength(); i++) {
                DOMNode *e = elements->item(i);
                if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
                        command = xoap::XMLCh2String(e->getLocalName());
                        break;
                }
        }

        xoap::MessageReference reply = xoap::createMessage();
        xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
        xoap::SOAPName responseName = envelope.createName(
                        command + "Response", "xdaq", XDAQ_NS_URI);
        envelope.getBody().addBodyElement(responseName);

        return reply;
}

std::string EmuDim::getLocalDateTime(){
  time_t t;
  struct tm *tm;

  time( &t );
  tm = localtime( &t );

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << tm->tm_year+1900 << "-"
     << std::setfill('0') << std::setw(2) << tm->tm_mon+1     << "-"
     << std::setfill('0') << std::setw(2) << tm->tm_mday      << " "
     << std::setfill('0') << std::setw(2) << tm->tm_hour      << ":"
     << std::setfill('0') << std::setw(2) << tm->tm_min       << ":"
     << std::setfill('0') << std::setw(2) << tm->tm_sec;

  return ss.str();
}

  }  // namespace emu::x2p
}  // namespace emu

//
// provides factory method for instantion of XDAQ application
//
XDAQ_INSTANTIATOR_IMPL(emu::x2p::EmuDim)
