// $Id: EmuPeripheralCrateManager.cc,v 1.13 2009/03/15 12:32:14 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuPeripheralCrateManager.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

//using namespace cgicc;
//using namespace std;

namespace emu {
  namespace pc {

EmuPeripheralCrateManager::EmuPeripheralCrateManager(xdaq::ApplicationStub * s): EmuPeripheralCrateBase(s)
{	
  xgi::bind(this,&EmuPeripheralCrateManager::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateManager::MainPage, "MainPage");
  //
  // Normal SOAP call-back function
  // SOAP call-back functions, which relays to *Action method.
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onHalt,      "Halt",      XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigCalCFEB,"ConfigCalCFEB", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBGains,"EnableCalCFEBGains", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk,"EnableCalCFEBCrossTalk", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBSCAPed,"EnableCalCFEBSCAPed", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBComparator,"EnableCalCFEBComparator", XDAQ_NS_URI);

  //
  // fsm_ is defined in EmuApplication
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateManager::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateManager::disableAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();
  //
  ConfigureState_ = "None";
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //
}  
//
void EmuPeripheralCrateManager::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuPeripheralCrateManager");
}
//
//////////////////////////////////////////////////////////////////////////////
// Responses to soap messages
//////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Configure");
}
//
void EmuPeripheralCrateManager::enableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
}  
//
void EmuPeripheralCrateManager::disableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
}
//
void EmuPeripheralCrateManager::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
}
//
void EmuPeripheralCrateManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  changeState(fsm);
}
//
void EmuPeripheralCrateManager::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
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


xoap::MessageReference EmuPeripheralCrateManager::onConfigCalCFEB (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  std::cout << "Get SOAP message ConfigCalCFEB " << std::endl;
  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  PCsendCommand("ConfigCalCFEB","emu::pc::EmuPeripheralCrateBroadcast");
   //
  std::cout << "SOAP message ConfigCalCFEB relayed to Broadcast" << std::endl;
//   ::sleep(1);
   fireEvent("Configure");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  PCsendCommand("EnableCalCFEBCrossTalk","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBSCAPed (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  PCsendCommand("EnableCalCFEBSCAPed","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBGains (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  PCsendCommand("EnableCalCFEBGains","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBComparator (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  PCsendCommand("EnableCalCFEBComparator","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onConfigure (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  time_t thistime = ::time(NULL);
  std::cout << "SOAP message Configure: " << ::ctime(&thistime) << std::endl;
  //
  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  PCsendCommand("Configure","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Configure","emu::pc::EmuPeripheralCrateBroadcast");
  //
  fireEvent("Configure");
  //
  return createReply(message);
  //
}
//
xoap::MessageReference EmuPeripheralCrateManager::onEnable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  time_t thistime = ::time(NULL);
  std::cout << "SOAP message Enable: " << ::ctime(&thistime) << std::endl;
  //
  PCsendCommand("Enable","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Enable","emu::pc::EmuPeripheralCrateBroadcast");
  PCsendCommand("MonitorStart","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateManager::onDisable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  time_t thistime = ::time(NULL);
  std::cout << "SOAP message Disable: " << ::ctime(&thistime) << std::endl;
  //
  PCsendCommand("Disable","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Disable","emu::pc::EmuPeripheralCrateBroadcast");
  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Disable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateManager::onHalt (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  time_t thistime = ::time(NULL);
  std::cout << "SOAP message Halt: " << ::ctime(&thistime) << std::endl;
  //
  PCsendCommand("Halt","emu::pc::EmuPeripheralCrateCommand");
  PCsendCommand("Halt","emu::pc::EmuPeripheralCrateBroadcast");
  PCsendCommand("MonitorStop","emu::pc::EmuPeripheralCrateMonitor");
  //
  fireEvent("Halt");
  //
  return createReply(message);
}
//

  } // namespace emu::pc
} // namespace emu
//
//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(emu::pc::EmuPeripheralCrateManager)
