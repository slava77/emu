// $Id: EmuPeripheralCrateTimer.cc,v 1.10 2009/11/07 14:25:23 liu Exp $

#include "emu/pc/EmuPeripheralCrateTimer.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

namespace emu {
  namespace pc {

EmuPeripheralCrateTimer::EmuPeripheralCrateTimer(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  //
  xoap::bind(this, &EmuPeripheralCrateTimer::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::onHalt,      "Halt",      XDAQ_NS_URI);

  xgi::bind(this,&EmuPeripheralCrateTimer::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateTimer::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateTimer::SwitchBoard, "SwitchBoard");
  //
  fsm_.addState('H', "Halted", this, &EmuPeripheralCrateTimer::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateTimer::stateChanged);
  //
  fsm_.addStateTransition('H', 'E', "Enable",    this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateTimer::dummyAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();

  state_ = fsm_.getStateName(fsm_.getCurrentState());

  current_state_ = 0;

  // everything below for Monitoring
  timer_ = toolbox::task::getTimerFactory()->createTimer("EmuMonitorTimer");
  timer_->stop();
  Monitor_On_ = false;
  Monitor_Ready_ = false;
  In_Monitor_ = false;
  In_Broadcast_ = false;
  fastloop = 0;
  slowloop = 0;
  extraloop = 0;
  fastintv = -1;
  slowintv = -1;
  extraintv = -1;
  this->getApplicationInfoSpace()->fireItemAvailable("FastLoop", &fastloop);
  this->getApplicationInfoSpace()->fireItemAvailable("SlowLoop", &slowloop);
  this->getApplicationInfoSpace()->fireItemAvailable("ExtraLoop", &extraloop);
  xoap::bind(this, &EmuPeripheralCrateTimer::MonitorStart, "MonitorStart", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::MonitorStop, "MonitorStop", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::Locked, "Locked", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::Unlock, "Unlock", XDAQ_NS_URI);
}  

xoap::MessageReference EmuPeripheralCrateTimer::MonitorStart (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     if(!Monitor_Ready_)
     {
         toolbox::TimeInterval interval;
         toolbox::TimeVal startTime;
         time_t cycle=1;  // number of seconds
         interval.sec(cycle);

         if(fastintv<0) fastintv=fastloop;
         if(slowintv<0) slowintv=slowloop;
         if(extraintv<0) extraintv=extraloop;
         rtime_fast=0;
         rtime_slow=0;
         rtime_extra=0;

         timer_->start(); // must activate timer before submission, abort otherwise!!!
         startTime = toolbox::TimeVal::gettimeofday();
         timer_->scheduleAtFixedRate(startTime, this, interval, 0, "EmuPCrateTimer" );
         std::cout << "EmuPCrateTimer scheduled at 1 sec interval" << std::endl;
         std::cout << "Fast Loop: " << fastintv << ", Slow Loop: " << slowintv << ", Extra Loop: " << extraintv << std::endl;
         Monitor_Ready_=true;
     }
     Monitor_On_=true;
     fireEvent("Enable");
     time_t thistime = ::time(NULL);
     std::cout<< "Monitor Started " << ::ctime(&thistime) << std::endl;
     return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateTimer::MonitorStop (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     if(Monitor_On_)
     {
         Monitor_On_=false;
#if 0
         timer_->remove("EmuPCrateTimer" );
         timer_->stop(); 
#endif
         fireEvent("Halt");
         time_t thistime = ::time(NULL);
         std::cout << "Monitor stopped " << ::ctime(&thistime) << std::endl;
     }
     return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateTimer::Locked (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     In_Broadcast_=true;
     return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateTimer::Unlock (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     In_Broadcast_=false;
     return createReply(message);
}

void EmuPeripheralCrateTimer::timeExpired (toolbox::task::TimerEvent& e)
{

     if(! Monitor_On_ ) return;
     if( In_Monitor_  || In_Broadcast_) return;
     In_Monitor_ = true;

     time_t thistime=time(NULL);
     if(fastintv>0 && (thistime-rtime_fast)>fastintv)
     {
        PCsendCommand("FastLoop","emu::pc::EmuPeripheralCrateMonitor");
        rtime_fast=thistime=time(NULL);
     }
     if(extraintv>0 && (thistime-rtime_extra)>extraintv)
     {
        PCsendCommand("ExtraLoop","emu::pc::EmuPeripheralCrateMonitor");
        rtime_extra=thistime=time(NULL);
     }
     if(slowintv && (thistime-rtime_slow)>slowintv)
     {
        PCsendCommand("SlowLoop","emu::pc::EmuPeripheralCrateMonitor");
        rtime_slow=thistime=time(NULL);
     }

     // std::string name = e.getTimerTask()->name;
     // std::cout << "timeExpired: " << name << std::endl;
     In_Monitor_ = false;
}
//
// everything above is for Monitoring

//
void EmuPeripheralCrateTimer::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<std::endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateTimer::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  return;
  //
}

void EmuPeripheralCrateTimer::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}

void EmuPeripheralCrateTimer::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}

//
xoap::MessageReference EmuPeripheralCrateTimer::onEnable (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Enable" << std::endl;
  //
  current_state_ = 2;
  fireEvent("Enable");
  //
  return createReply(message);
}

//
xoap::MessageReference EmuPeripheralCrateTimer::onHalt (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Halt" << std::endl;
  //
  current_state_ = 0;
  fireEvent("Halt");
  //
  return createReply(message);
}

void EmuPeripheralCrateTimer::SwitchBoard(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
  Page=cgiEnvi.getQueryString();
  std::string command_name=Page.substr(0,Page.find("=", 0) );
  std::string command_argu=Page.substr(Page.find("=", 0)+1);

  if (command_name=="FAST")
  {
     int interval = atoi(command_argu.c_str());
     if(interval>=0) 
        {  fastintv=interval;
           std::cout << "FAST Loop set to " << fastintv << std::endl;
        }
  }  
  else if (command_name=="SLOW")
  {
     int interval = atoi(command_argu.c_str());
     if(interval>=0) 
        {  slowintv=interval;
           std::cout << "SLOW Loop set to " << slowintv << std::endl;
        }
  }  
  else if (command_name=="EXTRA")
  {
     int interval = atoi(command_argu.c_str());
     if(interval>=0) 
        {  extraintv=interval;
           std::cout << "EXTRA Loop set to " << extraintv << std::endl;
        }
  }
}

 }  // namespace emu::pc
}  // namespace emu

//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateTimer)
//
