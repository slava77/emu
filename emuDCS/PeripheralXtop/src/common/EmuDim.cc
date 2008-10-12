// $Id: EmuDim.cc,v 1.1 2008/10/12 11:55:50 liu Exp $

#include "EmuDim.h"

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

namespace emu {
  namespace e2p {

EmuDim::EmuDim(xdaq::ApplicationStub * s): EmuApplication(s)
{	
  HomeDir_     = getenv("HOME");
  ConfigDir_   = HomeDir_+"/config/";
  //
  PeripheralCrateDimFile_  = "";
  XmasDcsUrl_ = "";

  // Bind SOAP callback
  //
  xoap::bind(this, &EmuDim::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuDim::onHalt,      "Halt",      XDAQ_NS_URI);

  xgi::bind(this,&EmuDim::Default, "Default");
  xgi::bind(this,&EmuDim::MainPage, "MainPage");
  //
  fsm_.addState('H', "Halted", this, &EmuDim::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuDim::stateChanged);
  //
  fsm_.addStateTransition('H', 'E', "Enable",    this, &EmuDim::dummyAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuDim::dummyAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuDim::dummyAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuDim::dummyAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();

  state_ = fsm_.getStateName(fsm_.getCurrentState());

  current_state_ = 0;
  getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &PeripheralCrateDimFile_);
  getApplicationInfoSpace()->fireItemAvailable("XmasDcsUrl", &XmasDcsUrl_ );

  // everything below for Monitoring
  timer_ = toolbox::task::getTimerFactory()->createTimer("EmuMonitorTimer");
  timer_->stop();
  inited=false;
  Monitor_On_ = false;
  Monitor_Ready_ = false;
  In_Monitor_ = false;
  In_Broadcast_ = false;
  fastloop=0;
  slowloop=0;
  extraloop=0;
  this->getApplicationInfoSpace()->fireItemAvailable("FastLoop", &fastloop);
  this->getApplicationInfoSpace()->fireItemAvailable("SlowLoop", &slowloop);
  this->getApplicationInfoSpace()->fireItemAvailable("ExtraLoop", &extraloop);
  xoap::bind(this, &EmuDim::SoapStart, "SoapStart", XDAQ_NS_URI);
  xoap::bind(this, &EmuDim::SoapStop, "SoapStop", XDAQ_NS_URI);
  xgi::bind(this,&EmuDim::ButtonStart      ,"ButtonStart");
  xgi::bind(this,&EmuDim::ButtonStop      ,"ButtonStop");

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
     if( In_Monitor_  || In_Broadcast_) return;
     In_Monitor_ = true;

     // always check DimCommand
     CheckCommand();

     std::string name = e.getTimerTask()->name;
     // std::cout << "timeExpired: " << name << std::endl;
     if(strncmp(name.c_str(),"EmuPCrateRead",13)==0) 
     {  ReadFromXmas();
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
             timer_->scheduleAtFixedRate(startTime,this, interval1, 0, "EmuPCrateCmnd" );
             std::cout << "Dim Command scheduled" << std::endl;
             ::sleep(2);
         }
         if(slowloop) 
         {   interval2.sec((time_t)slowloop);
             timer_->scheduleAtFixedRate(startTime,this, interval2, 0, "EmuPCrateRead" );
             std::cout << "XMAS read scheduled" << std::endl;
         }
         if(extraloop) 
         {   interval3.sec((time_t)extraloop);
             timer_->scheduleAtFixedRate(startTime,this, interval3, 0, "EmuPCrateExtra" );
             std::cout << "extra scheduled" << std::endl;
         }
         Monitor_Ready_=true;
     }
     Monitor_On_=true;
     time_t thistime = ::time(NULL);
     char cbuf[30];
     std::cout<< "EmuDim Started " << ::ctime_r(&thistime, cbuf) << std::endl;
}

void EmuDim::Stop () 
{
     if(Monitor_On_)
     {
         Monitor_On_=false;
         time_t thistime = ::time(NULL);
         char cbuf[30];
         std::cout << "EmuDim stopped " << ::ctime_r(&thistime, cbuf);
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
  LOG4CPLUS_INFO(getApplicationLogger(), "Main Page");
  //
  if(current_state_==2) return;
  //
  if(Monitor_On_)
  {
     *out << cgicc::span().set("style","color:green");
     *out << cgicc::b(cgicc::i("Monitor Status: On")) << cgicc::span() << std::endl ;
     std::string TurnOffMonitor = toolbox::toString("/%s/ButtonStop",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOffMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn Off Monitor") << std::endl ;
     *out << cgicc::form() << std::endl ;
  } else
  {
     *out << cgicc::span().set("style","color:red");
     *out << cgicc::b(cgicc::i("Monitor Status: Off")) << cgicc::span() << std::endl ;
     std::string TurnOnMonitor = toolbox::toString("/%s/ButtonStart",getApplicationDescriptor()->getURN().c_str());
     *out << cgicc::form().set("method","GET").set("action",TurnOnMonitor) << std::endl ;
     *out << cgicc::input().set("type","submit").set("value","Turn On Monitor") << std::endl ;
     *out << cgicc::form() << std::endl ;
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
   MyLoader = new LOAD();
   std::string url=XmasDcsUrl_;
   MyLoader->init(url.c_str());
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
   MyLoader->reload();
   // then fill the structure
   ch=ParseTXT(MyLoader->Content(), MyLoader->Content_Size(), 0);
   time_t thistime = ::time(NULL);
   char cbuf[30];
   std::cout << ch << " Chambers read at " << ::ctime_r(&thistime, cbuf) << std::endl;
   // 
   return ch;
}

int EmuDim::ParseTXT(char *buff, int buffsize, int source)
{
   int chmbs=0;
   bool more_line = true;
   char * start = buff;
   char *endstr;
   unsigned char *bbb=(unsigned char *)buff;

   if(buffsize < 40) return 0;
   for(int i=0; i<buffsize; i++)
   {  if((bbb[i]< 0x20 || bbb[i]>0x70) && bbb[i]!=0x0a)
      { std::cout << "ERROR at " << i << std::hex << bbb[i] << std::dec << std::endl;
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

   if(strlen(buff) < 4) return 0;
   endstr=strchr(buff, ' ');
   if(endstr==NULL) return 0;
   *endstr=0;
   int chnumb=ChnameToNumber(buff);
   label=buff;
//   std::cout << "Found chamber " << label << " with number " << chnumb << std::endl; 
   content = endstr+1;
   if(strlen(content)>400) std::cout<< label << " WARNING " << content << std::endl;
   if(chnumb>=0 && chnumb <TOTAL_CHAMBERS)
   {   if(source) chamb[chnumb].SetLabel(label);
       chamb[chnumb].Fill(content, source);
   }
   else
   {   std::cout << "ERROR in tag " << label << std::endl;
   }
   return 1;
}

int EmuDim::ChnameToNumber(const char *chname)
{
   if(strlen(chname)<7) return -1;
   if(strncmp(chname,"ME",2)) return -2;
   int station = std::atoi(chname+3);
   int ring = std::atoi(chname+5);
   int chnumb = std::atoi(chname+7);
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

void EmuDim::StartDim(int chs)
{
   int total=0, i=0;
   std::string dim_lv_name, dim_temp_name;

   while(total<chs && i < TOTAL_CHAMBERS)
   {
      if(chamb[i].Ready())
      {
         chamb[i].GetDimLV(1, &(EmuDim_lv[i]));
         chamb[i].GetDimTEMP(1, &(EmuDim_temp[i]));
         dim_lv_name = "LV_1_" + chamb[i].GetLabel(); 
         dim_temp_name = "TEMP_1_" + chamb[i].GetLabel(); 

         LV_1_Service[i]= new DimService(dim_lv_name.c_str(),"F:5;F:5;F:5;F:5;F:5;F:5;F:5;F:9;I:2;C:80",
           &(EmuDim_lv[i]), sizeof(LV_1_DimBroker));
         TEMP_1_Service[i]= new DimService(dim_temp_name.c_str(),"F:7;I:2;C:80",
           &(EmuDim_temp[i]), sizeof(TEMP_1_DimBroker));

         total++;
      }
      i++;
   }
   LV_1_Command = new DimCommand("LV_1_COMMAND","C");
   DimServer::start("Emu_Peripheral");
   time_t thistime = ::time(NULL);
   char cbuf[30];
   std::cout << total << " DIM serives start at " << ::ctime_r(&thistime, cbuf) << std::endl;
}

int EmuDim::UpdateDim(int ch)
{
   if(ch>=0 && ch < TOTAL_CHAMBERS && chamb[ch].Ready())
   {
         chamb[ch].GetDimLV(0, &(EmuDim_lv[ch]));
         chamb[ch].GetDimTEMP(0, &(EmuDim_temp[ch]));
         if(LV_1_Service[ch]) LV_1_Service[ch]->updateService();
         if(TEMP_1_Service[ch]) TEMP_1_Service[ch]->updateService();
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
   while(LV_1_Command->getNext())
   {
      std::string cmnd = LV_1_Command->getString();
      std::cout << "Dim Command:" << cmnd << std::endl;
      if(cmnd.substr(cmnd.length()-8,8)=="get_data")
      {
         int ch=ChnameToNumber(cmnd.c_str());
         UpdateDim(ch);
      }
      // all other commands are ignored
   }
}

////////////////////////////////////////////////////////////////////
// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuDim::PCsendCommand(std::string command, std::string klass)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
  }
  // 
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
    //
    //      PCanalyzeReply(message, reply, *i);
  }
}
//
xoap::MessageReference EmuDim::PCcreateCommandSOAP(std::string command) {
  //
  //This is copied from CSCSupervisor::createCommandSOAP
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
  envelope.getBody().addBodyElement(name);
  //
  return message;
}

void EmuDim::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  EmuApplication::stateChanged(fsm);
}

void EmuDim::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}
//
xoap::MessageReference EmuDim::onEnable (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Enable" << std::endl;
  //
  current_state_ = 2;
  fireEvent("Enable");
  //
  return createReply(message);
}

//
xoap::MessageReference EmuDim::onHalt (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Halt" << std::endl;
  //
  current_state_ = 0;
  fireEvent("Halt");
  //
  return createReply(message);
}

  }  // namespace emu::e2p
}  // namespace emu

//
// provides factory method for instantion of XDAQ application
//
XDAQ_INSTANTIATOR_IMPL(emu::e2p::EmuDim)
