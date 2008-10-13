// $Id: EmuPeripheralCrateTimer.cc,v 1.1 2008/10/13 13:11:07 liu Exp $

#include "EmuPeripheralCrateTimer.h"

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

EmuPeripheralCrateTimer::EmuPeripheralCrateTimer(xdaq::ApplicationStub * s): EmuApplication(s)
{	
  //
  xoap::bind(this, &EmuPeripheralCrateTimer::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::onHalt,      "Halt",      XDAQ_NS_URI);

  xgi::bind(this,&EmuPeripheralCrateTimer::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateTimer::MainPage, "MainPage");
  //
  fsm_.addState('H', "Halted", this, &EmuPeripheralCrateTimer::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateTimer::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateTimer::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateTimer::dummyAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateTimer::dummyAction);
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
  fastloop=0;
  slowloop=0;
  extraloop=0;
  this->getApplicationInfoSpace()->fireItemAvailable("FastLoop", &fastloop);
  this->getApplicationInfoSpace()->fireItemAvailable("SlowLoop", &slowloop);
  this->getApplicationInfoSpace()->fireItemAvailable("ExtraLoop", &extraloop);
  xoap::bind(this, &EmuPeripheralCrateTimer::MonitorStart, "MonitorStart", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateTimer::MonitorStop, "MonitorStop", XDAQ_NS_URI);

}  

xoap::MessageReference EmuPeripheralCrateTimer::MonitorStart (xoap::MessageReference message) 
  throw (xoap::exception::Exception) 
{
     if(!Monitor_Ready_)
     {
         toolbox::TimeInterval interval1, interval2, interval3;
         toolbox::TimeVal startTime;
         startTime = toolbox::TimeVal::gettimeofday();

         timer_->start(); // must activate timer before submission, abort otherwise!!!
         if(fastloop) 
         {   interval1.sec((time_t)fastloop);
             timer_->scheduleAtFixedRate(startTime,this, interval1, 0, "EmuPCrateFast" );
             std::cout << "fast scheduled" << std::endl;
         }
         if(slowloop) 
         {   interval2.sec((time_t)slowloop);
             timer_->scheduleAtFixedRate(startTime,this, interval2, 0, "EmuPCrateSlow" );
             std::cout << "slow scheduled" << std::endl;
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
         if(fastloop) timer_->remove("EmuPCrateFast" );
         if(slowloop) timer_->remove("EmuPCrateSlow" );
         if(extraloop) timer_->remove("EmuPCrateExtra" );
         timer_->stop(); 
#endif
         time_t thistime = ::time(NULL);
         std::cout << "Monitor stopped " << ::ctime(&thistime) << std::endl;
     }
     return createReply(message);
}

void EmuPeripheralCrateTimer::timeExpired (toolbox::task::TimerEvent& e)
{

     if(! Monitor_On_ ) return;
     if( In_Monitor_  || In_Broadcast_) return;
     In_Monitor_ = true;
     std::string name = e.getTimerTask()->name;
     // std::cout << "timeExpired: " << name << std::endl;
     if(strncmp(name.c_str(),"EmuPCrateFast",13)==0)  PCsendCommand("FastLoop","EmuPeripheralCrateMonitor");
     else if(strncmp(name.c_str(),"EmuPCrateSlow",13)==0) PCsendCommand("SlowLoop","EmuPeripheralCrateMonitor");
     else if(strncmp(name.c_str(),"EmuPCrateExtra",14)==0) PCsendCommand("ExtraLoop","EmuPeripheralCrateMonitor");
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
//
//
////////////////////////////////////////////////////////////////////
// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateTimer::PCsendCommand(std::string command, std::string klass)
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
xoap::MessageReference EmuPeripheralCrateTimer::PCcreateCommandSOAP(std::string command) {
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

void EmuPeripheralCrateTimer::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception) {
  EmuApplication::stateChanged(fsm);
}

void EmuPeripheralCrateTimer::dummyAction(toolbox::Event::Reference e)
    throw (toolbox::fsm::exception::Exception) {
  // currently do nothing
}
//
xoap::MessageReference EmuPeripheralCrateTimer::onConfigure (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Configure" << std::endl;
  //
  current_state_ = 1;
  fireEvent("Configure");
  //
  return createReply(message);
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
xoap::MessageReference EmuPeripheralCrateTimer::onDisable (xoap::MessageReference message)
  throw (xoap::exception::Exception) {
  std::cout << "SOAP Disable" << std::endl;
  //
  current_state_ = 1;
  fireEvent("Disable");
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

//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateTimer)
//
