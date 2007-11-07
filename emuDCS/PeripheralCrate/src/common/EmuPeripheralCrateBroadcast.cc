// $Id: EmuPeripheralCrateBroadcast.cc,v 1.4 2007/11/07 09:14:16 rakness Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuPeripheralCrateBroadcast.h"

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

using namespace cgicc;
using namespace std;

EmuPeripheralCrateBroadcast::EmuPeripheralCrateBroadcast(xdaq::ApplicationStub * s): EmuApplication(s)
{	
  HomeDir_     = getenv("HOME");
  FirmwareDir_ = HomeDir_+"/firmware/";
  ConfigDir_   = HomeDir_+"/config/";
  //
  PeripheralCrateBroadcastXmlFile_  = ConfigDir_+"pc/broadcast.xml";
  DmbControlFPGAFirmwareFile_       = FirmwareDir_+"dmb/dmb6cntl_pro.svf";
  DmbVmeFPGAFirmwareFile_           = FirmwareDir_+"dmb/dmb6vme_pro.svf";
  CfebFPGAFirmwareFile_             = FirmwareDir_+"cfeb/cfeb_pro.svf";
  //
  //In order to load firmware automatically from the firmware values in the xml files, 
  //the firmware needs to reside in directories in the form:
  //    TMB  ->  $HOME/firmware/tmb/YEARMONTHDAY/tmb.xsvf   <-- N.B. xsvf format for TMB
  //    RAT  ->  $HOME/firmware/rat/YEARMONTHDAY/rat.svf
  //    ALCT ->  $HOME/firmware/alct/YEARMONTHDAY/alctXXX/alctXXX.svf
  // with the zero-values filled in with 0's.  
  // In other words:  9 April 2007 firmware should reside in YEARMONTHDAY=20070409
  //
  // The XXX in the ALCT firmware specification corresponds to the following structure:
  //  ALCT192FirmwareFile_       = ALCTFirmwareDirectory_+"alct192/alct192.svf";
  //  ALCT288FirmwareFile_       = ALCTFirmwareDirectory_+"alct288/alct288.svf";
  //  ALCT288bnFirmwareFile_     = ALCTFirmwareDirectory_+"alct288bn/alct288bn.svf";
  //  ALCT288bpFirmwareFile_     = ALCTFirmwareDirectory_+"alct288bp/alct288bp.svf";
  //  ALCT288fpFirmwareFile_     = ALCTFirmwareDirectory_+"alct288fp/alct288fp.svf";
  //  ALCT384FirmwareFile_       = ALCTFirmwareDirectory_+"alct384/alct384.svf";
  //  ALCT384MirrorFirmwareFile_ = ALCTFirmwareDirectory_+"alct384mirror/alct384mirror.svf";
  //  ALCT576MirrorFirmwareFile_ = ALCTFirmwareDirectory_+"alct576mirror/alct576mirror.svf";
  //  ALCT672FirmwareFile_       = ALCTFirmwareDirectory_+"alct672/alct672.svf";  
  //
  //    std::cout << "PeripheralCrateBroadcastXmlFile_ = " << PeripheralCrateBroadcastXmlFile_ << std::endl;
  //    std::cout << "DmbControlFPGAFirmwareFile_      = " << DmbControlFPGAFirmwareFile_      << std::endl;
  //    std::cout << "DmbVmeFPGAFirmwareFile_          = " << DmbVmeFPGAFirmwareFile_          << std::endl;
  //    std::cout << "CfebFPGAFirmwareFile_            = " << CfebFPGAFirmwareFile_            << std::endl;
  //
  // Bind SOAP callback
  //
  xgi::bind(this,&EmuPeripheralCrateBroadcast::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBcalchannel, "LoadCFEBcalchannel");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDACandTrigger, "LoadDACandTrigger");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBinternal, "LoadCFEBinternal");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBexternal, "LoadCFEBexternal");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::DmbTurnOnPower, "DmbTurnOnPower");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::DmbTurnOffPower, "DmbTurnOffPower");
  //
  // Bind firmware loading
  //
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBCFEBFPGAFirmware, "LoadDMBCFEBFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBControlFPGAFirmware, "LoadDMBControlFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadDMBvmeFPGAFirmware, "LoadDMBvmeFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadCFEBFPGAFirmware, "LoadCFEBFPGAFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadTMBFirmware, "LoadTMBFirmware");
  xgi::bind(this,&EmuPeripheralCrateBroadcast::LoadALCTFirmware, "LoadALCTFirmware");

  xoap::bind(this, &EmuPeripheralCrateBroadcast::onConfigCalCFEB, "ConfigCalCFEB", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBGains, "EnableCalCFEBGains", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBCrossTalk, "EnableCalCFEBCrossTalk", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBSCAPed, "EnableCalCFEBSCAPed", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateBroadcast::onEnableCalCFEBComparator, "EnableCalCFEBComparator", XDAQ_NS_URI);
  //
}  
//
void EmuPeripheralCrateBroadcast::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuPeripheralCrateBroadcast");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Crates in Configuration file").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Main Page");
  //
  //
  std::string LoadDMBCFEBFPGA = toolbox::toString("/%s/LoadDMBCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBCFEBFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","!!!      Broadcast Download Firmware      !!!") << std::endl ;
  *out << cgicc::form();
  //
  std::string LoadCFEBchannel = toolbox::toString("/%s/LoadCFEBcalchannel",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBchannel) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----   Load CFEB Buckeye Patterns for Calibration    -----") << std::endl ;
  *out << cgicc::form();
  //
  std::string DmbTurnOnPower = toolbox::toString("/%s/DmbTurnOnPower",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DmbTurnOnPower) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----  Broadcast Turn ON On-chamber electronics power -----") << std::endl ;
  *out << cgicc::form();
  //
  std::string DmbTurnOffPower = toolbox::toString("/%s/DmbTurnOffPower",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",DmbTurnOffPower) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","-----  Broadcast Turn OFF On-chamber electronics power ----") << std::endl ;
  *out << cgicc::form();
  //
  *out << cgicc::fieldset() ;
  //
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBcalchannel(xgi::Input * in, xgi::Output * out ) {
  //
  MyHeader(in,out,"Load CFEB buckeye channels, broadcast !!!");
  //
  DefineBroadcastCrate();
  //
  cout <<" Buckeye shift channels......"<<endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  std::string LoadDACandTrigger = toolbox::toString("/%s/LoadDACandTrigger",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDACandTrigger) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load the DAC to 1V, and trigger to DMB-->LCT, LTC-->L1A") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBinternal = toolbox::toString("/%s/LoadCFEBinternal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBinternal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load Buckeye half-strip pattern 8") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBexternal = toolbox::toString("/%s/LoadCFEBexternal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBexternal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load Buckeye external channel 8") << std::endl ;
  *out << cgicc::form()<<std::endl;
  //
  *out << cgicc::fieldset()<<std::endl;
  //
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBinternal(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  char *outp=(char *)dword;
  cout << " Loading all Buckeye half-strip pattern 8 ..."<<endl;
  broadcastDMB->buck_shift_comp_bc(8);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBexternal(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  char *outp=(char *)dword;
  cout <<" Loading all Buckeye external pattern 8 ..."<<endl;
  broadcastDMB->buck_shift_ext_bc(8);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadDACandTrigger(xgi::Input * in, xgi::Output * out )  {
  //
  unsigned short int dword[2];
  dword[0]=0;
  char *outp=(char *)dword;
  cout <<" Loading all Buckeye external pattern 8 ..."<<endl;
  broadcastDMB->set_cal_dac(1.0,1.0);
  broadcastDMB->settrgsrc(1);
  in=NULL;
  this->LoadCFEBcalchannel(in, out);
}
//
/////////////////////////////////////////////////////////////////////////////////
// Setup broadcast classes
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::DefineBroadcastCrate() {
  //
  //define broadcast crate and board, if not defined before
  //
  if (!broadcastCrate) {
    cout <<" Broadcast crate has not been defined yet"<<endl;
    cout <<" Defining Broadcast crate from " << PeripheralCrateBroadcastXmlFile_ << endl;
    //
    MyController = new EmuController();
    MyController->SetConfFile(PeripheralCrateBroadcastXmlFile_);
    MyController->init();
    CrateSelector selector = MyController->selector();
    vector<Crate *> tmpcrate=selector.broadcast_crate();
    broadcastCrate=tmpcrate[0];
    broadcastDMB=selector.daqmbs(tmpcrate[0])[0];
    broadcastTMB=selector.tmbs(tmpcrate[0])[0];
    broadcastALCT = broadcastTMB->alctController();
  }
  //
  cout <<" Broadcast components are defined "<<endl;
  //
  return;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Firmware page and methods
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::LoadDMBCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ){
  //
  MyHeader(in,out,"Broadcast Download Firmware,  Be extra careful    !!!");
  //
  DefineBroadcastCrate();
  //
  // load the DAQMB Controller FPGA firmware
  //
  cout <<"Ready to load firmware for all components ..."<<endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  std::string LoadDMBControlFPGA = toolbox::toString("/%s/LoadDMBControlFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBControlFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load DMBs Control FPGA Firmware") << std::endl ;
  *out << DmbControlFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadDMBvmeFPGA = toolbox::toString("/%s/LoadDMBvmeFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadDMBvmeFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load DMBs VME FPGA Firmware") << std::endl ;
  *out << DmbVmeFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  std::string LoadCFEBFPGA = toolbox::toString("/%s/LoadCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCFEBFPGA) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load CFEBs FPGA Firmware") << std::endl ;
  *out << CfebFPGAFirmwareFile_;
  *out << cgicc::form()<<std::endl;
  //
  //
  //create TMB filename for firmware based on expected dates...
  char date[8];
  sprintf(date,"%4x%1x%1x%1x%1x",
	  broadcastTMB->GetExpectedTmbFirmwareYear()&0xffff,
	  (broadcastTMB->GetExpectedTmbFirmwareMonth()>>4)&0xf,
	  (broadcastTMB->GetExpectedTmbFirmwareMonth()   )&0xf,
	  (broadcastTMB->GetExpectedTmbFirmwareDay()  >>4)&0xf,
	  (broadcastTMB->GetExpectedTmbFirmwareDay()     )&0xf);
  TMBFirmwareFile_ = FirmwareDir_+"tmb/"+date+"/tmb";   // Note:  ".xsvf" is added in SetXsvfFilename
  //
  std::string LoadTMBFirmware = toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load TMB Firmware") << std::endl ;
  *out << TMBFirmwareFile_ << ".xsvf";
  *out << cgicc::form()<<std::endl;
  //
  //
  char alctdate[8];
  //create ALCT filename for firmware based on expected dates...
  int expected_year  = broadcastALCT->GetExpectedFastControlYear() ;
  int expected_month = broadcastALCT->GetExpectedFastControlMonth();
  int expected_day   = broadcastALCT->GetExpectedFastControlDay()  ;
  //
  sprintf(alctdate,"%04u%02u%02u",
  	  expected_year,
  	  expected_month,
  	  expected_day);
  //
  // pre-DAQ06 format
  //  int expected_year       = broadcastALCT->GetExpectedFastControlYear() & 0xffff;
  //  int expected_month_tens = (broadcastALCT->GetExpectedFastControlMonth()>>4) & 0xf;
  //  int expected_month_ones = (broadcastALCT->GetExpectedFastControlMonth()>>0) & 0xf;
  //  int expected_day_tens   = (broadcastALCT->GetExpectedFastControlDay()  >>4) & 0xf;
  //  int expected_day_ones   = (broadcastALCT->GetExpectedFastControlDay()  >>0) & 0xf;
  //
  //  sprintf(alctdate,"%4x%1x%1x%1x%1x",
  //	  expected_year,
  //	  expected_month_tens,
  //	  expected_month_ones,
  //	  expected_day_tens,
  //	  expected_day_ones);
  //
  ALCTFirmwareDirectory_     = FirmwareDir_+"alct/"+alctdate+"/";
  //
  ALCT192FirmwareFile_       = ALCTFirmwareDirectory_+"alct192/alct192.svf";
  ALCT288FirmwareFile_       = ALCTFirmwareDirectory_+"alct288/alct288.svf";
  ALCT288bnFirmwareFile_     = ALCTFirmwareDirectory_+"alct288bn/alct288bn.svf";
  ALCT288bpFirmwareFile_     = ALCTFirmwareDirectory_+"alct288bp/alct288bp.svf";
  ALCT288fpFirmwareFile_     = ALCTFirmwareDirectory_+"alct288fp/alct288fp.svf";
  ALCT384FirmwareFile_       = ALCTFirmwareDirectory_+"alct384/alct384.svf";
  ALCT384MirrorFirmwareFile_ = ALCTFirmwareDirectory_+"alct384mirror/alct384mirror.svf";
  ALCT576MirrorFirmwareFile_ = ALCTFirmwareDirectory_+"alct576mirror/alct576mirror.svf";
  ALCT672FirmwareFile_       = ALCTFirmwareDirectory_+"alct672/alct672.svf";  
  //
  std::string LoadALCTFirmware = toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Load ALCT Firmware") << std::endl ;
  *out << ALCTFirmwareDirectory_ << "...";
  *out << cgicc::form()<<std::endl;
  //
  *out << cgicc::fieldset()<<std::endl;
  //
}
//
//
void EmuPeripheralCrateBroadcast::LoadDMBControlFPGAFirmware(xgi::Input * in, xgi::Output * out )  {
  //
  // load the DAQMB Controller FPGA firmware
  //
  unsigned short int dword[2];
  dword[0]=0;
  char *outp=(char *)dword;
  cout <<" Loading all the DMB's Controller FPGAs firmware from " << DmbControlFPGAFirmwareFile_ <<endl;
  broadcastDMB->epromload(MPROM,DmbControlFPGAFirmwareFile_.c_str(),1,outp);
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadDMBvmeFPGAFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  // load the DAQMB VME FPGA firmware
  //
  char *outp="0";
  cout <<" Loading all the DMB's VME FPGAs firmware from " << DmbVmeFPGAFirmwareFile_ <<endl;
  cout <<" Step 1: Sending SOAP message to all the crates to readback the VME_PROM_ID"<<endl;
  cout <<"         This is the DMB board number"<<endl;
  //
  //SOAP message to read back the DMB board ID:
  PCsendCommand("ReadVmePromUserid","EmuPeripheralCrate");
  //
  cout <<" Step 2: Broadcast programming the VME until the 'loading USERCODE' point"<<endl;
  broadcastDMB->epromload_broadcast(VPROM,DmbVmeFPGAFirmwareFile_.c_str(),1,outp,1);
  //
  cout <<" Step 3: Sending SOAP message to program PROM_USERCODE"<<endl;
  //SOAP message to individual crates to program the PROM_USERCODE
  PCsendCommand("LoadVmePromUserid","EmuPeripheralCrate");
  //
  cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<endl;
  broadcastDMB->epromload_broadcast(VPROM,DmbVmeFPGAFirmwareFile_.c_str(),1,outp,3);
  //
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ) {
  //
  // load the CFEB FPGA firmware
  //
  char *outp="0";
  //
  cout <<" Loading all the CFEBs FPGAs firmware from " << CfebFPGAFirmwareFile_ <<endl;
  //
  cout <<" Step 1: Sending SOAP message to all the crates to readback the CFEB_PROM_ID"<<endl;
  cout <<"         This is the CFEB board number"<<endl;
  //SOAP message to read back the CFEB board ID:
  PCsendCommand("ReadCfebPromUserid","EmuPeripheralCrate");
  //
  cout <<" Step 2: Broadcast programming the CFEB until the 'loading USERCODE' point"<<endl;
  broadcastDMB->epromload_broadcast(FAPROM,CfebFPGAFirmwareFile_.c_str(),1,outp,1);
  //
  cout <<" Step 3: Sending SOAP message to program CFEB PROM_USERCODE"<<endl;
  //SOAP message to individual crates to program the CFEB PROM_USERCODE
  PCsendCommand("LoadCfebPromUserid","EmuPeripheralCrate");
  //
  cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<endl;
  broadcastDMB->epromload_broadcast(FAPROM,CfebFPGAFirmwareFile_.c_str(),1,outp,3);
  //
  this->LoadDMBCFEBFPGAFirmware(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::LoadTMBFirmware(xgi::Input * in, xgi::Output * out )  {
  //
  // load the TMB firmware
  //
  std::cout <<" Loading all TMBs with firmware from " << TMBFirmwareFile_ << ".xsvf" << std::endl;
  //
  broadcastTMB->SetXsvfFilename(TMBFirmwareFile_);
  broadcastTMB->ProgramTMBProms();
  broadcastTMB->ClearXsvfFilename();
  //
  in=NULL;
  this->LoadDMBCFEBFPGAFirmware(in, out);
}
//
void EmuPeripheralCrateBroadcast::LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) {
  //
  // load the ALCT firmware
  //
  const bool program192       = false; // ME1/3
  const bool program384       = true;  // ME1/2, ME2/2
  const bool program288       = false; // ME1/1
  const bool program288bn     = false; // ME1/1
  const bool program288bp     = false; // ME1/1
  const bool program288fp     = false; // ME1/1
  const bool program672       = true;  // ME2/1
  const bool program576Mirror = true;  // ME3/1, ME4/1
  const bool program384Mirror = true;  // ME3/2
  //
  int debugMode(0);
  int jch(3);
  int status;
  //
  std::cout << "Loading ALCT firmware to all boards from the base directory:  " << ALCTFirmwareDirectory_ << std::endl;
  //
  if (program192) {
    //---------------------
    // ALCT192 boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT192 firmware");
    //
    std::cout << "ALCT192: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT192: Enable JTAG write for TMBs connected to ALCT192..." << std::endl;
    PCsendCommand("EnableALCT192","EmuPeripheralCrate");
    //
    std::cout << "ALCT192: Broadcast ALCT192 firmware from " << ALCT192FirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT192FirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program384) {
    //---------------------
    // ALCT384 boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT384 firmware");
    //
    std::cout << "ALCT384: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT384: Enable JTAG write for TMBs connected to ALCT384..." << std::endl;
    PCsendCommand("EnableALCT384","EmuPeripheralCrate");
    //
    std::cout << "ALCT384: Broadcast ALCT384 firmware from " << ALCT384FirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT384FirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program288) {
    //---------------------
    // ALCT288 boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT288 firmware");
    //
    std::cout << "ALCT288: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT288: Enable JTAG write for TMBs connected to ALCT288..." << std::endl;
    PCsendCommand("EnableALCT288","EmuPeripheralCrate");
    //
    std::cout << "ALCT288: Broadcast ALCT288 firmware from " << ALCT288FirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT288FirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program288bn) {
    //---------------------
    // ALCT288bn boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT288bn firmware");
    //
    std::cout << "ALCT288bn: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT288bn: Enable JTAG write for TMBs connected to ALCT288bn..." << std::endl;
    PCsendCommand("EnableALCT288bn","EmuPeripheralCrate");
    //
    std::cout << "ALCT288bn: Broadcast ALCT288bn firmware from " << ALCT288bnFirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT288bnFirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program288bp) {
    //---------------------
    // ALCT288bp boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT288bp firmware");
    //
    std::cout << "ALCT288bp: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT288bp: Enable JTAG write for TMBs connected to ALCT288bp..." << std::endl;
    PCsendCommand("EnableALCT288bp","EmuPeripheralCrate");
    //
    std::cout << "ALCT288bp: Broadcast ALCT288bp firmware from " << ALCT288bpFirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT288bpFirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program288fp) {
    //---------------------
    // ALCT288fp boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT288fp firmware");
    //
    std::cout << "ALCT288fp: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT288fp: Enable JTAG write for TMBs connected to ALCT288fp..." << std::endl;
    PCsendCommand("EnableALCT288fp","EmuPeripheralCrate");
    //
    std::cout << "ALCT288fp: Broadcast ALCT288fp firmware from " << ALCT288fpFirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT288fpFirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program672) {
    //---------------------
    // ALCT672 boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT672 firmware");
    //
    std::cout << "ALCT672: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT672: Enable JTAG write for TMBs connected to ALCT672..." << std::endl;
    PCsendCommand("EnableALCT672","EmuPeripheralCrate");
    //
    std::cout << "ALCT672: Broadcast ALCT672 firmware from " << ALCT672FirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT672FirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program576Mirror) {
    //---------------------
    // ALCT576Mirror boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT576Mirror firmware");
    //
    std::cout << "ALCT576Mirror: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT576Mirror: Enable JTAG write for TMBs connected to ALCT576Mirror..." << std::endl;
    PCsendCommand("EnableALCT576Mirror","EmuPeripheralCrate");
    //
    std::cout << "ALCT576Mirror: Broadcast ALCT576Mirror firmware from " << ALCT576MirrorFirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT576MirrorFirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  if (program384Mirror) {
    //---------------------
    // ALCT384Mirror boards
    //---------------------
    LOG4CPLUS_INFO(getApplicationLogger(), "Broadcast ALCT384Mirror firmware");
    //
    std::cout << "ALCT384Mirror: Broadcast disable JTAG write to all TMBs..." << std::endl;
    broadcastTMB->SetJtagDisableWriteToAdr10(1);
    broadcastTMB->WriteRegister(0xD4);
    //  
    std::cout << "ALCT384Mirror: Enable JTAG write for TMBs connected to ALCT384Mirror..." << std::endl;
    PCsendCommand("EnableALCT384Mirror","EmuPeripheralCrate");
    //
    std::cout << "ALCT384Mirror: Broadcast ALCT384Mirror firmware from " << ALCT384MirrorFirmwareFile_ << std::endl;
    //
    broadcastTMB->disableAllClocks();
    status = broadcastALCT->SVFLoad(&jch,ALCT384MirrorFirmwareFile_.c_str(),debugMode);
    broadcastTMB->enableAllClocks();
  }
  //
  // Allow the user JTAG register to work again on all TMBs..
  std::cout << "Broadcast enable JTAG write to all TMBs..." << std::endl;
  broadcastTMB->SetJtagDisableWriteToAdr10(0);
  broadcastTMB->WriteRegister(0xD4);
  //  
  this->LoadDMBCFEBFPGAFirmware(in, out);
  //
}
//
/////////////////////////////////////////////////////////////////////////////
// Other (nice) broadcast commands
/////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::DmbTurnOnPower(xgi::Input * in, xgi::Output * out )  {
  //
  DefineBroadcastCrate();
  //
  cout <<" Broadcast to turn ON the On-chamber electronics power ..."<<endl;
  broadcastDMB->lowv_onoff(0x3f);
  in=NULL;
  this->Default(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::DmbTurnOffPower(xgi::Input * in, xgi::Output * out )  {
  //
  DefineBroadcastCrate();
  //
  cout <<" Broadcast to turn ON the On-chamber electronics power ..."<<endl;
  broadcastDMB->lowv_onoff(0x00);
  in=NULL;
  this->Default(in, out);
  //
}
//
void EmuPeripheralCrateBroadcast::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
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
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onConfigCalCFEB (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  std::cout<< "This is a checking printing for OnConfigCal0"<<std::endl;
  ostringstream test;
  message->writeTo(test);
  cout << test.str() <<endl;
  cout << " Print check working in OnConfigCal0 "<<endl;
  //
  //implement the cal0 configure process:
  float dac;
  int nsleep = 100;  
  std::cout << "DMB setup for calibration " << std::endl;
  calsetup=0;
  dac=1.00;
  //
  DefineBroadcastCrate();
  //
  //  
  broadcastTMB->DisableCLCTInputs();
  std::cout << "Disabling inputs for TMB slot  " << broadcastTMB->slot() << std::endl;
  broadcastTMB->DisableALCTInputs();
  //
  // DMB fifo Master_Reset
  broadcastDMB->calctrl_fifomrst();
  std::cout << "DMB Fifo reset and initialization "<<std::endl;
  usleep(1000);
  //
  std::cout << "Set DAC for DMB slot  " << broadcastDMB->slot() << std::endl;
  broadcastDMB->set_cal_dac(dac,dac);
  cout <<" DAC is set to: "<<dac<<endl;
  //Enable CLCT (bit0=1), disable L1A (bit1=0) on DMB calibration
  broadcastDMB->settrgsrc(1);
  //
  //set the default DMB Calibration timing:
  /* // change the itim to adjust the pulse position
     int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
     | (calibration_l1acc_delay_ & 0x1F) << 4
     | (itim & 0x1F) << 9
     | (inject_delay_ & 0x1F) << 14;
  */
  int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
  broadcastDMB->setcaldelay(dword);
  //
  cout << " The Peripheral Crate configure finished "<<endl;
  usleep(nsleep);
  //
  //    fireEvent("Configure");
  //
  return createReply(message);
  //
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBComparator (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac, threshold;
  int nsleep = 100, highthreshold;  
  //  std::cout<< "This is a checking printing for OnEnableCalCFEBComparator"<<std::endl;
  ostringstream test;
  message->writeTo(test);
  cout << test.str() <<endl;
  //
  calsetup++;
  //
  //implement the comparator setup process:
  std::cout << "DMB setup for CFEB Comparator, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  //
  if (calsetup==1) {
    broadcastTMB->SetTmbAllowClct(1);
    broadcastTMB->SetTmbAllowMatch(0);
    broadcastTMB->WriteRegister(0x86,broadcastTMB->FillTMBRegister(0x86));
    broadcastTMB->SetAlctMatchWindowSize(7);
    broadcastTMB->WriteRegister(0xb2,broadcastTMB->FillTMBRegister(0xb2));
    broadcastTMB->SetL1aDelay(154);
    broadcastTMB->SetL1aWindowSize(7);
    broadcastTMB->WriteRegister(0x74,broadcastTMB->FillTMBRegister(0x74));
    //
    broadcastTMB->EnableCLCTInputs(0x1f); //enable TMB's CLCT inputs
    broadcastDMB->settrgsrc(0); //disable the DMB internal LCT & L1A
  }
  int thresholdsetting =((calsetup-1)%20);   //35 Comparator threshold setting for each channel
  int nstrip=(calsetup-1)/20;           //16 channels, total loop: 32*35=1120
  highthreshold=nstrip/16;
  dac=0.02+0.18*highthreshold;
  nstrip=nstrip%16;
  if (!thresholdsetting) {
    broadcastDMB->buck_shift_comp_bc(nstrip);
    if (!nstrip) broadcastDMB->set_cal_dac(dac,dac);
  }
  threshold=0.003*thresholdsetting+0.01+ (0.19+0.007*thresholdsetting)*highthreshold;
  broadcastDMB->set_comp_thresh_bc(threshold);
  cout <<" The strip was set to: "<<nstrip<<" DAC was set to: "<<dac <<endl;
  usleep(nsleep);
  //    fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBGains (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac;
  int nsleep = 100;  
  //  std::cout<< "This is a checking printing for OnEnableCalCFEBGains"<<std::endl;
  ostringstream test;
  message->writeTo(test);
  cout << test.str() <<endl;
  //
  calsetup++;
  //
  //implement the cal0 setup process:
  std::cout << "DMB setup for CFEB Gain, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  int gainsetting =((calsetup-1)%20);
  int nstrip=(calsetup-1)/20;
  if (!gainsetting) broadcastDMB->buck_shift_ext_bc(nstrip);
  dac=0.1+0.25*gainsetting;
  broadcastDMB->set_cal_dac(dac,dac);
  cout <<" The strip was set to: "<<nstrip<<" DAC was set to: "<<dac <<endl;
  usleep(nsleep);
  //    fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBCrossTalk (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  int nsleep = 100;  
  //
  std::cout<< "This is a checking printing for OnEnableCalCFEBCrossTalk"<<std::endl;
  ostringstream test;
  message->writeTo(test);
  cout << test.str() <<endl;
  //
  calsetup++;
  //
  //implement the cal0 setup process:
  std::cout << "DMB setup for CFEB Time, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process:
  int timesetting =((calsetup-1)%10);
  int nstrip=(calsetup-1)/10;
  if (!timesetting) broadcastDMB->buck_shift_ext_bc(nstrip);
  broadcastDMB->set_cal_tim_pulse(timesetting+5);
  cout <<" The strip was set to: "<<nstrip<<" Time was set to: "<<timesetting <<endl;
  usleep(nsleep);
  //    fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::onEnableCalCFEBSCAPed (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  float dac;
  int nsleep = 100;  
  //
  std::cout<< "This is a checking printing for OnEnableCalCFEBSCAPed"<<std::endl;
  ostringstream test;
  message->writeTo(test);
  cout << test.str() <<endl;
  //
  calsetup++;
  //
  //implement the CFEB_Pedestal setup process:
  std::cout << "DMB setup for CFEB Pedestal, calsetup= " <<calsetup<< std::endl;
  //
  //Start the setup process: Set all channel to normal, DAC to 0:
  broadcastDMB->buck_shift_ext_bc(-1);
  dac=0.0;
  broadcastDMB->set_cal_dac(dac,dac);
  cout <<" The strip was set to: -1, " <<" DAC was set to: "<<dac <<endl;
  usleep(nsleep);
  //    fireEvent("Enable");
  //
  return createReply(message);
}
//
////////////////////////////////////////////////////////////////////
// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateBroadcast::PCsendCommand(string command, string klass)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  //This is copied from CSCSupervisor::sendcommand;
  //
  // Exceptions:
  // xoap exceptions are thrown by analyzeReply() for SOAP faults.
  // xdaq exceptions are thrown by postSOAP() for socket level errors.
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
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *i);
    //
    //      PCanalyzeReply(message, reply, *i);
  }
}
//
xoap::MessageReference EmuPeripheralCrateBroadcast::PCcreateCommandSOAP(string command) {
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
//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateBroadcast)
