// $Id: EmuPeripheralCrateBroadcast.h,v 1.21 2009/02/18 15:44:32 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuPeripheralCrateBroadcast_h_
#define _EmuPeripheralCrateBroadcast_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/UnsignedShort.h>
#include <xdata/Table.h>

#include "toolbox/Event.h"
#include "toolbox/string.h"
#include "toolbox/net/URL.h"
#include "toolbox/task/TimerTask.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "EmuController.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateUtilities.h"
#include "TMB.h"
#include "DAQMB.h"
#include "CCB.h"
#include "ALCTController.h"
#include "MPC.h"
#include "RAT.h"
#include "VMECC.h"
#include "ConnectionsDB.h"
#include "BoardsDB.h"
#include "emu/base/Supervised.h"

namespace emu {
  namespace pc {
  
//class EmuPeripheralCrateBroadcast: public xdaq::Application
class EmuPeripheralCrateBroadcast: public emu::base::Supervised
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  std::string HomeDir_;
  std::string ConfigDir_;  
  std::string FirmwareDir_;
  //
  void DefineBroadcastCrate();
  xdata::String PeripheralCrateBroadcastXmlFile_;
  std::string DmbControlFPGAFirmwareFile_;
  std::string DmbVmeFPGAFirmwareFile_;
  std::string CfebFPGAFirmwareFile_;
  std::string RATFirmwareFile_;
  std::string CCBFirmwareFile_;
  std::string MPCFirmwareFile_;
  //
  emu::db::BoardsDB *brddb;
  emu::db::ConnectionsDB *condb;
  //
  EmuPeripheralCrateBroadcast(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void LoadCFEBcalchannel(xgi::Input * in, xgi::Output * out );
  void LoadCFEBinternal(xgi::Input * in, xgi::Output * out );
  void LoadCFEBexternal(xgi::Input * in, xgi::Output * out );
  void DmbTurnOnPower(xgi::Input * in, xgi::Output * out );
  void DmbTurnOffPower(xgi::Input * in, xgi::Output * out );
  void LoadDACandTrigger(xgi::Input * in, xgi::Output * out );
  void SetNumberOfLayersInTrigger(xgi::Input * in, xgi::Output * out );
  //
  // Firmware
  //
  void LoadDMBCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadDMBControlFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadDMBvmeFPGAFirmware(xgi::Input * in, xgi::Output * out ) throw(xgi::exception::Exception);
  void LoadCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadRATFirmware(xgi::Input * in, xgi::Output * out );
  void LoadCCBFirmware(xgi::Input * in, xgi::Output * out );
  void LoadMPCFirmware(xgi::Input * in, xgi::Output * out );
  void VMECCLoadFirmwareBcast(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void VMECCTestBcast(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception);
  void VMECCTestSkewClear(xgi::Input * in, xgi::Output * out )throw (xgi::exception::Exception);
  //
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception);
  //
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBGains (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBCrossTalk (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBSCAPed (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBComparator (xoap::MessageReference message) throw (xoap::exception::Exception);
  // Sending soap messages
  //
  xoap::MessageReference PCcreateCommandSOAP(std::string command);
  void PCsendCommand(std::string command, std::string klass) throw (xoap::exception::Exception, xdaq::exception::Exception);
  //
  // define states
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void dummyAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

private:

  int calsetup;
  emu::pc::EmuController * MyController;
  emu::pc::VMECC* vmecc;
  emu::pc::Crate * broadcastCrate;
  emu::pc::DAQMB * broadcastDMB;
  emu::pc::TMB * broadcastTMB;
  emu::pc::RAT * broadcastRAT;
  emu::pc::MPC * broadcastMPC;
  emu::pc::ALCTController * broadcastALCT;
  emu::pc::CCB * broadcastCCB;

  //
  int number_of_layers_pretrig_;
  int number_of_layers_pattern_;
  //
  bool In_Broadcast_;
  int current_state_;
};

  } // namespace emu::pc
} // namespace emu

#endif
