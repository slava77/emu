// $Id: EmuPeripheralCrateManager.cc,v 1.21 2007/09/13 20:22:09 gujh Exp $

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
#include<unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>


using namespace cgicc;
using namespace std;

  EmuPeripheralCrateManager::EmuPeripheralCrateManager(xdaq::ApplicationStub * s): EmuApplication(s)
  {	
    //
    // Bind SOAP callback
    //
    xgi::bind(this,&EmuPeripheralCrateManager::Default, "Default");
    xgi::bind(this,&EmuPeripheralCrateManager::MainPage, "MainPage");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConfigure, "SendSOAPMessageConfigure");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConfigureXRelay, "SendSOAPMessageConfigureXRelay");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageCalibrationXRelay, 
	      "SendSOAPMessageCalibrationXRelay");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConnectTStore, "SendSOAPMessageConnectTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageDisconnectTStore, "SendSOAPMessageDisconnectTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageQueryTStore, "SendSOAPMessageQueryTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageDefinitionTStore, "SendSOAPMessageDefinitionTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageInsertTStore, "SendSOAPMessageInsertTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageDeleteTStore, "SendSOAPMessageDeleteTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageClearTStore, "SendSOAPMessageClearTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageUpdateTStore, "SendSOAPMessageUpdateTStore");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConfigureLTC, "SendSOAPMessageConfigureLTC");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageExecuteSequence, "SendSOAPMessageExecuteSequence");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageJobControlStartAllEmuperipheralCrate, 
	      "SendSOAPMessageJobControlStartAllEmuperipheralCrate");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageJobControlkillAll, 
	      "SendSOAPMessageJobControlkillAll");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageQueryLTC, "SendSOAPMessageQueryLTC");
    xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageQueryLTC, "SendSOAPMessageQueryJobControl");
    xgi::bind(this,&EmuPeripheralCrateManager::CheckEmuPeripheralCrateCalibrationState, "CheckEmuPeripheralCrateCalibrationState");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadCFEBcalchannel, "LoadCFEBcalchannel");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadDACandTrigger, "LoadDACandTrigger");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadCFEBinternal, "LoadCFEBinternal");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadCFEBexternal, "LoadCFEBexternal");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadDMBCFEBFPGAFirmware, "LoadDMBCFEBFPGAFirmware");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadDMBControlFPGAFirmware, "LoadDMBControlFPGAFirmware");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadDMBvmeFPGAFirmware, "LoadDMBvmeFPGAFirmware");
    xgi::bind(this,&EmuPeripheralCrateManager::LoadCFEBFPGAFirmware, "LoadCFEBFPGAFirmware");
    //
    xgi::bind(this,&EmuPeripheralCrateManager::UploadDB, "UpLoadDB");
    xgi::bind(this,&EmuPeripheralCrateManager::RetrieveTStoreTable, "RetrieveTStoreTable");
    //
    // Normal SOAP call-back function
    // xoap::bind(this, &EmuPeripheralCrateManager::LTCResponse, "LTCResponse", XDAQ_NS_URI);
    // SOAP call-back functions, which relays to *Action method.
    xoap::bind(this, &EmuPeripheralCrateManager::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onConfigCalCFEB, "ConfigCalCFEB", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBGains, "EnableCalCFEBGains", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk, "EnableCalCFEBCrossTalk", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBSCAPed, "EnableCalCFEBSCAPed", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBComparator, "EnableCalCFEBComparator", XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onEnable,    "Enable",    XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onDisable,   "Disable",   XDAQ_NS_URI);
    xoap::bind(this, &EmuPeripheralCrateManager::onHalt,      "Halt",      XDAQ_NS_URI);
    //
    // fsm_ is defined in EmuApplication
    fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateManager::stateChanged);
    fsm_.addState('C', "Configured", this, &EmuPeripheralCrateManager::stateChanged);
    fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateManager::stateChanged);
    //
    fsm_.addStateTransition(
      'H', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
    fsm_.addStateTransition(
      'C', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
    fsm_.addStateTransition(
      'C', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
    fsm_.addStateTransition(
      'E', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
    fsm_.addStateTransition(
      'E', 'C', "Disable",   this, &EmuPeripheralCrateManager::disableAction);
    fsm_.addStateTransition(
      'C', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
    fsm_.addStateTransition(
      'E', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
    fsm_.addStateTransition(
      'H', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);

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
  void EmuPeripheralCrateManager::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
    //
    MyHeader(in,out,"EmuPeripheralCrateManager");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("Crates in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "Main Page");
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptor)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("XRelays in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorXRelay =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("XRelay");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorXRelay;
    for ( itDescriptorXRelay = descriptorXRelay.begin(); itDescriptorXRelay != descriptorXRelay.end(); itDescriptorXRelay++ ) 
      {
	std::string classNameStr = (*itDescriptorXRelay)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorXRelay)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorXRelay)->getURN();  	
	*out << urn << std::endl;
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "XRelay");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("TStore in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorTStore =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("TStore");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorTStore;
    for ( itDescriptorTStore = descriptorTStore.begin(); itDescriptorTStore != descriptorTStore.end(); itDescriptorTStore++ ) 
      {
	std::string classNameStr = (*itDescriptorTStore)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorTStore)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorTStore)->getURN();  	
	*out << urn << std::endl;
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "TStore");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("JobControl in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorJobControl =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("JobControl");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorJobControl;
    for ( itDescriptorJobControl = descriptorJobControl.begin(); 
	  itDescriptorJobControl != descriptorJobControl.end(); itDescriptorJobControl++ ) 
      {
	std::string classNameStr = (*itDescriptorJobControl)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorJobControl)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorJobControl)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryJobControlInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptorJobControl));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	    //
	    //*out << cgicc::br();
	    //
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "JobControl");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("LTCControl in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    try{
      std::set<xdaq::ApplicationDescriptor * >  descriptorLTCControl =
	getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("LTCControl");
      //
      std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorLTCControl;
      for ( itDescriptorLTCControl = descriptorLTCControl.begin(); 
	    itDescriptorLTCControl != descriptorLTCControl.end(); itDescriptorLTCControl++ ) 
	{
	//
	std::string classNameStr = (*itDescriptorLTCControl)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorLTCControl)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorLTCControl)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryLTCInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptorLTCControl));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	    //
	    *out << cgicc::br();
	    *out << "Run number = " << extractRunNumber(reply) << std::endl;
	    //
	  }
	}
	//
	*out << cgicc::br();
	//
	}   
    } 
    catch (xdaq::exception::Exception& e) 
      {
	LOG4CPLUS_INFO(getApplicationLogger(), "LTCControl not in XML file");	
      }
    //
    *out << cgicc::fieldset() ;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "LTCControl");
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    /*
    std::string methodSOAPMessageInit =
      toolbox::toString("/%s/SendSOAPMessageInitXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInit) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Init Crate") << std::endl ;
    *out << cgicc::form();
    */
    //
    std::string methodSOAPMessageConfigure =
      toolbox::toString("/%s/SendSOAPMessageConfigureXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigure) << std::endl ;
    ostringstream output;
    output << "Send SOAP message : Configure Crates " ;
    output << "(" << ConfigureState_.toString() << ")" ;
    *out << cgicc::input().set("type","submit")
      .set("value",output.str()) << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageCalibration =
      toolbox::toString("/%s/SendSOAPMessageCalibrationXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageCalibration) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Calibration") << std::endl ;
    *out << cgicc::form();
    //
    std::string CheckCalibrationState =
      toolbox::toString("/%s/CheckEmuPeripheralCrateCalibrationState",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CheckCalibrationState) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Check Calibration State") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    std::string LoadDMBCFEBFPGA =
      toolbox::toString("/%s/LoadDMBCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadDMBCFEBFPGA) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","!!!      BroadCast Load DMB/CFEB FPGA Firmware      !!!") << std::endl ;
    *out << cgicc::form();

    std::string LoadCFEBchannel =
      toolbox::toString("/%s/LoadCFEBcalchannel",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadCFEBchannel) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","-----   Load CFEB Buckeye Patterns for Calibration    -----") << std::endl ;
    *out << cgicc::form();

    //
    /*
    std::string methodSOAPMessageOpenFile =
      toolbox::toString("/%s/SendSOAPMessageOpenFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageOpenFile) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Open File") << std::endl ;
    *out << cgicc::form();
    */
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string UploadDB =
      toolbox::toString("/%s/UpLoadDB",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",UploadDB) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Upload DB") << std::endl ;
    *out << cgicc::form();
    //
    std::string RetrieveTStoreTable =
      toolbox::toString("/%s/RetrieveTStoreTable",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",RetrieveTStoreTable) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Retrieve TStore table") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageConnectTStore =
      toolbox::toString("/%s/SendSOAPMessageConnectTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConnectTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Connect TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageDisconnectTStore =
      toolbox::toString("/%s/SendSOAPMessageDisconnectTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageDisconnectTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Disconnect TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageQueryTStore =
      toolbox::toString("/%s/SendSOAPMessageQueryTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageQueryTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Query TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageDefinitionTStore =
      toolbox::toString("/%s/SendSOAPMessageDefinitionTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageDefinitionTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Definition TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageInsertTStore =
      toolbox::toString("/%s/SendSOAPMessageInsertTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInsertTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Insert TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageDeleteTStore =
      toolbox::toString("/%s/SendSOAPMessageDeleteTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageDeleteTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Delete TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageClearTStore =
      toolbox::toString("/%s/SendSOAPMessageClearTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageClearTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Clear TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageUpdateTStore =
      toolbox::toString("/%s/SendSOAPMessageUpdateTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageUpdateTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Update TStore") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageConfigureLTC =
      toolbox::toString("/%s/SendSOAPMessageConfigureLTC",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigureLTC) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure LTC") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageExecuteSequence =
      toolbox::toString("/%s/SendSOAPMessageExecuteSequence",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageExecuteSequence) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Execute Sequence") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageQueryLTC =
      toolbox::toString("/%s/SendSOAPMessageQueryLTC",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageQueryLTC) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Query LTC InfoSpace") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageJobControlStartAllEmuperipheralCrate =
      toolbox::toString("/%s/SendSOAPMessageJobControlStartAllEmuperipheralCrate",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageJobControlStartAllEmuperipheralCrate) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : JobControl Start All EmuPeripheralCrate") << std::endl ;
    *out << cgicc::form();
    /*
    std::string methodSOAPMessageJobControlStartAllProc =
      toolbox::toString("/%s/SendSOAPMessageJobControlStartAll",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageJobControlStartAll) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : JobControl Start All proc for EmuPeripheralCrate") << std::endl ;
    *out << cgicc::form();
    */
    std::string methodSOAPMessageJobControlkillAll =
      toolbox::toString("/%s/SendSOAPMessageJobControlkillAll",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageJobControlkillAll) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : JobControl kill All proc") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
  }
  //
  void EmuPeripheralCrateManager::CheckEmuPeripheralCrateState(xgi::Input * in, xgi::Output * out ){
    //
    MyHeader(in,out,"CheckEmuperipheralCrate state");
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set<xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptor)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
  }
  //
  int EmuPeripheralCrateManager::CompareEmuPeripheralCrateCalibrationState(std::string state_compare){
    //
    int compare =0;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	//
	std::string classNameStr = (*itDescriptor)->getClassName();
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();  	
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    // reply->writeTo(std::cout);
	    std::string state = extractCalibrationState(reply); 
	    std::cout << "States " << state << " " << state_compare << std::endl;
	    if ( state == state_compare ) compare++;
	  }
	}
	//
      }    
    //
    return compare;
    //
  }
  //
  void EmuPeripheralCrateManager::UploadDB(xgi::Input * in, xgi::Output * out ){
    //
    MyHeader(in,out,"Upload DB");
    //
    EmuController myController;
    myController.SetConfFile("/home/meydev/config/configFall2006ME1b.xml");
    myController.init();
    //
    std::vector<Crate*> myCrates = (myController.selector()).crates();
    //
    for(unsigned int crate=0; crate<myCrates.size();crate++){
      //
      CrateUtilities crateUtility;
      crateUtility.SetCrate(myCrates[crate]);
      crateUtility.CreateTstoreTables();
      crateUtility.DumpTstoreTables();
      //
      this->SendSOAPMessageConnectTStore(in,out);
      //
      std::string CrateLabel = myCrates[crate]->GetLabel();
      ostringstream tableName ;
      /*
      tableName.str("");
      tableName << CrateLabel << "_periph";
      //
      std::vector <std::string> PeriphTable = crateUtility.GetPeriphTable();
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
      //
      for(unsigned int debug=0; debug<PeriphTable.size(); debug++) std::cout << " " << PeriphTable[debug];
      //
      AddRow(0,table_,PeriphTable);
      //
      this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      */
      //
      tableName.str("");
      tableName << CrateLabel << "_csc";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      for (int i=0; i<9; i++) {
	//
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	std::vector <std::string> CSCTable = crateUtility.GetCSCTable(i);
	//
	for(unsigned int debug=0; debug<CSCTable.size(); debug++) std::cout << " " << CSCTable[debug];
	//
	AddRow(0,table_,CSCTable);
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      //
      tableName.str("");
      tableName << CrateLabel << "_tmb";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      for (int i=0; i<9; i++) {
	//
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	std::vector <std::string> TmbTable = crateUtility.GetTmbTable(i);
	//
	for(unsigned int debug=0; debug<TmbTable.size(); debug++) std::cout << " " << TmbTable[debug];
	//
	std::cout << std::endl;
	//
	AddRow(0,table_,TmbTable);
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      //
      tableName.str("");
      tableName << CrateLabel << "_dmb";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      for (int i=0; i<9; i++) {
	//
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	std::vector <std::string> DmbTable = crateUtility.GetDmbTable(i);
	//
	for(unsigned int debug=0; debug<DmbTable.size(); debug++) std::cout << " " << DmbTable[debug];
	//
	AddRow(0,table_,DmbTable);
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      //
      tableName.str("");
      tableName << CrateLabel << "_alct";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      for (int i=0; i<9; i++) {
      //
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	std::vector <std::string> AlctTable = crateUtility.GetAlctTable(i);
	//
	AddRow(0,table_,AlctTable);
	//
	for(unsigned int debug=0; debug<AlctTable.size(); debug++) std::cout << " " << AlctTable[debug];
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      //
      tableName.str("");
      tableName << CrateLabel << "_afeb";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      std::vector<std::vector<std::string> > AfebTable = crateUtility.GetAfebTable();
      //
      for (unsigned int i=0; i<AfebTable.size(); i++) {
	//
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	for(unsigned int debug=0; debug<AfebTable[i].size(); debug++) std::cout << " " << AfebTable[i][debug];
	//
	AddRow(0,table_,AfebTable[i]);
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      //
      tableName.str("");
      tableName << CrateLabel << "_cfeb";
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
      std::vector<std::vector<std::string> > CfebTable = crateUtility.GetCfebTable();
      //
      for (unsigned int i=0; i<CfebTable.size(); i++) {
	//
	this->SendSOAPMessageDefinitionTStore(in,out,tableName.str());
	//
	AddRow(0,table_,CfebTable[i]);
	//
	this->SendSOAPMessageUpdateTStore(in,out,tableName.str());
	//
      }
      //
      this->SendSOAPMessageQueryTStore(in,out,tableName.str());
      //
    }
    //
  }
  //
  void EmuPeripheralCrateManager::AddRow(int Row,xdata::Table & table, std::vector<std::string> NewColumn){
    //
    int Column=0;
    std::vector<std::string> columns=table.getColumns();
    //
    if( NewColumn.size() != columns.size() ) {
      std::cout << "Column size doesn't match...expect= " << columns.size() << " got= " << NewColumn.size() << std::endl;
      return;
    }
    //
    vector<std::string>::iterator columnIterator;
    for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++,Column++) {
      string columnType=table.getColumnType(*columnIterator);
      xdata::Serializable *xdataValue=NULL;
      string columnName = (*columnIterator);
      //
      //int test=1;
      if (columnType=="string") {
	xdataValue=new xdata::String(NewColumn[Column]); 
      } else if (columnType=="int") {
	istringstream input(NewColumn[Column]);
	int out;
	input >> out;
	xdataValue=new xdata::Integer(out); 
      } else if (columnType=="float") {
	istringstream input(NewColumn[Column]);
	float out;
	input >> out;
	xdataValue=new xdata::Float(out); 
      } else if (columnType=="double") {
	istringstream input(NewColumn[Column]);
	double out;
	input >> out;
	xdataValue=new xdata::Double(out);
      } else if (columnType=="bool") {
	istringstream input(NewColumn[Column]);
	bool out;
	input >> out;
	xdataValue= new xdata::Boolean(out);
      }
      if (xdataValue) {
	//
	table.setValueAt(Row,*columnIterator,*xdataValue);
	delete xdataValue;
      }
      //
    } 
    //
  }
  //
  int EmuPeripheralCrateManager::CompareEmuPeripheralCrateState(std::string state_compare){
    //
    int compare =0;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	//
	std::string classNameStr = (*itDescriptor)->getClassName();
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();  	
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    std::string state = extractState(reply); 
	    //std::cout << "States " << state << " " << state_compare << std::endl;
	    if ( state == state_compare ) compare++;
	  }
	}
	//
      }    
    //
    return compare;
    //
  }
  //
  void EmuPeripheralCrateManager::CheckEmuPeripheralCrateCalibrationState(xgi::Input * in, xgi::Output * out ){
    //
    MyHeader(in,out,"Check EmuperipheralCrate Calibration State");
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptor)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    // reply->writeTo(std::cout);
	    std::string state = extractCalibrationState(reply); 
	    *out << "(" <<  state << ")";
	    *out << cgicc::span();
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
  }
  //
  void EmuPeripheralCrateManager::LoadCFEBcalchannel(xgi::Input * in, xgi::Output * out ){

    MyHeader(in,out,"Load CFEB buckeye channels, broadcast !!!");

    //define broadcast crate and board, if not defined before
    if (!broadcastCrate) {
      cout <<" Broadcast crate has not been defined yet"<<endl;
      MyController = new EmuController();
      MyController->SetConfFile("/nfshome0/cscpro/config/pc/broadcast.xml");
      MyController->init();
      CrateSelector selector = MyController->selector();
      vector<Crate *> tmpcrate=selector.broadcast_crate();
      broadcastCrate=tmpcrate[0];
      broadcastDMB=selector.daqmbs(tmpcrate[0])[0];
      broadcastTMB=selector.tmbs(tmpcrate[0])[0];
    }
    cout <<" Broadcast Crate and DMB are defined "<<endl;

    // load the DAQMB Controller FPGA firmware
    
    cout <<" Buckeye shift channels......"<<endl;

    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");

    std::string LoadDACandTrigger =
      toolbox::toString("/%s/LoadDACandTrigger",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadDACandTrigger) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load the DAC to 1V, and trigger to DMB-->LCT, LTC-->L1A") << std::endl ;
    *out << cgicc::form()<<std::endl;
    std::string LoadCFEBinternal =
      toolbox::toString("/%s/LoadCFEBinternal",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadCFEBinternal) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load Buckeye half-strip pattern 8") << std::endl ;
    *out << cgicc::form()<<std::endl;
    std::string LoadCFEBexternal =
      toolbox::toString("/%s/LoadCFEBexternal",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadCFEBexternal) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load Buckeye external channel 8") << std::endl ;
    *out << cgicc::form()<<std::endl;

    *out << cgicc::fieldset()<<std::endl;
    //
  }
  //
  void EmuPeripheralCrateManager::LoadCFEBinternal(xgi::Input * in, xgi::Output * out )  {

    // load the DAQMB Controller FPGA firmware
    unsigned short int dword[2];
    dword[0]=0;
    char *outp=(char *)dword;
    cout <<" Loading all Buckeye half-strip pattern 8 ..."<<endl;
    broadcastDMB->buck_shift_comp_bc(8);
    in=NULL;
    this->LoadCFEBcalchannel(in, out);
  }
  //
  void EmuPeripheralCrateManager::LoadCFEBexternal(xgi::Input * in, xgi::Output * out )  {

    // load the DAQMB Controller FPGA firmware
    unsigned short int dword[2];
    dword[0]=0;
    char *outp=(char *)dword;
    cout <<" Loading all Buckeye external pattern 8 ..."<<endl;
    broadcastDMB->buck_shift_ext_bc(8);
    in=NULL;
    this->LoadCFEBcalchannel(in, out);
  }
  //
  void EmuPeripheralCrateManager::LoadDACandTrigger(xgi::Input * in, xgi::Output * out )  {

    // load the DAQMB Controller FPGA firmware
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
  void EmuPeripheralCrateManager::LoadDMBCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ){

    MyHeader(in,out,"Load DAQMB/CFEB FPGA Firmware,  Be extra careful    !!!");

    //define broadcast crate and board, if not defined before
    if (!broadcastCrate) {
      cout <<" Broadcast crate has not been defined yet"<<endl;
      MyController = new EmuController();
      MyController->SetConfFile("/nfshome0/cscpro/config/pc/broadcast.xml");
      MyController->init();
      CrateSelector selector = MyController->selector();
      vector<Crate *> tmpcrate=selector.broadcast_crate();
      broadcastCrate=tmpcrate[0];
      broadcastDMB=selector.daqmbs(tmpcrate[0])[0];
      broadcastTMB=selector.tmbs(tmpcrate[0])[0];
    }
    cout <<" Broadcast Crate and DMB are defined "<<endl;

    // load the DAQMB Controller FPGA firmware
    
    cout <<" Loading all the DMBs, CFEBs FPGAs firmware ..."<<endl;

    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");

    std::string LoadDMBControlFPGA =
      toolbox::toString("/%s/LoadDMBControlFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadDMBControlFPGA) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load DMBs Control FPGA Firmware") << std::endl ;
    *out << cgicc::form()<<std::endl;
    std::string LoadDMBvmeFPGA =
      toolbox::toString("/%s/LoadDMBvmeFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadDMBvmeFPGA) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load DMBs VME FPGA Firmware") << std::endl ;
    *out << cgicc::form()<<std::endl;

    std::string LoadCFEBFPGA =
      toolbox::toString("/%s/LoadCFEBFPGAFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadCFEBFPGA) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Load CFEBs FPGA Firmware") << std::endl ;
    *out << cgicc::form()<<std::endl;

    *out << cgicc::fieldset()<<std::endl;
    //

  }
  //
  void EmuPeripheralCrateManager::LoadDMBControlFPGAFirmware(xgi::Input * in, xgi::Output * out )  {

    // load the DAQMB Controller FPGA firmware
    unsigned short int dword[2];
    dword[0]=0;
    char *outp=(char *)dword;
    cout <<" Loading all the DMB's Controller FPGAs firmware ..."<<endl;
    broadcastDMB->epromload(MPROM,"/nfshome0/cscpro/firmware/dmb/dmb6cntl_pro.svf",1,outp);
    in=NULL;
    this->LoadDMBCFEBFPGAFirmware(in, out);
  }
  //
  void EmuPeripheralCrateManager::LoadDMBvmeFPGAFirmware(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
    //    cgicc::Cgicc cgi(in);
    // load the DAQMB VME FPGA firmware
    char *outp="0";
    cout <<" Loading all the DMB's VME FPGAs firmware ..."<<endl;
    cout <<" Step 1: Sending soup message to all the crates to readback the VME_PROM_ID"<<endl;
    cout <<"         This is the DMB board number"<<endl;

    //SOAP message to read back the DMB board ID:
    PCsendCommand("ReadVmePromUserid","EmuPeripheralCrate");

    cout <<" Step 2: Broadcast programming the VME until the 'loading USERCODE' point"<<endl;
    broadcastDMB->epromload_broadcast(VPROM,"/nfshome0/cscpro/firmware/dmb/dmb6vme_pro.svf",1,outp,1);

    cout <<" Step 3: Sending SOAP message to program PROM_USERCODE"<<endl;
    //SOAP message to individual crates to program the PROM_USERCODE
    PCsendCommand("LoadVmePromUserid","EmuPeripheralCrate");

    cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<endl;
    broadcastDMB->epromload_broadcast(VPROM,"/nfshome0/cscpro/firmware/dmb/dmb6vme_pro.svf",1,outp,3);

    this->LoadDMBCFEBFPGAFirmware(in, out);
  }
  //
  void EmuPeripheralCrateManager::LoadCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out ) {

    // load the CFEB FPGA firmware
    char *outp="0";
    cout <<" Loading all the CFEBs FPGAs firmware ..."<<endl;

    cout <<" Step 1: Sending soup message to all the crates to readback the CFEB_PROM_ID"<<endl;
    cout <<"         This is the CFEB board number"<<endl;
    //SOAP message to read back the CFEB board ID:
    PCsendCommand("ReadCfebPromUserid","EmuPeripheralCrate");

    cout <<" Step 2: Broadcast programming the CFEB until the 'loading USERCODE' point"<<endl;
    broadcastDMB->epromload_broadcast(FAPROM,"/nfshome0/cscpro/firmware/cfeb/cfeb_pro.svf",1,outp,1);

    cout <<" Step 3: Sending SOAP message to program CFEB PROM_USERCODE"<<endl;
    //SOAP message to individual crates to program the CFEB PROM_USERCODE
    PCsendCommand("LoadCfebPromUserid","EmuPeripheralCrate");

    cout <<" Step 4: Broadcast the remaining part of the PROM/SVF"<<endl;
    broadcastDMB->epromload_broadcast(FAPROM,"/nfshome0/cscpro/firmware/cfeb/cfeb_pro.svf",1,outp,3);

    this->LoadDMBCFEBFPGAFirmware(in, out);
  }
  //
  void EmuPeripheralCrateManager::configureAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
      //
      LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Configure");
      //
    }
  
  //
  void EmuPeripheralCrateManager::enableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
      //
      LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
    //
    }
  
  //
  void EmuPeripheralCrateManager::disableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
      LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
    }
  
  //
  void EmuPeripheralCrateManager::haltAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
      LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
    }
  //
  void EmuPeripheralCrateManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception)
    {
      EmuApplication::stateChanged(fsm);
    }
  //
  void EmuPeripheralCrateManager::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
    throw (xgi::exception::Exception)
    {
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
  string EmuPeripheralCrateManager::extractCalibrationState(xoap::MessageReference message)
    {
      //
      //LOG4CPLUS_INFO(getApplicationLogger(), "extractCalibrationState");
      //
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
      xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
      xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("CalibrationState", "", "")))[0];
      //
      return state.getValue();
    }
  //
  string EmuPeripheralCrateManager::extractState(xoap::MessageReference message)
    {
      //
      //LOG4CPLUS_INFO(getApplicationLogger(), "extractState");
      //
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
      xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
      xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("stateName", "", "")))[0];
      
      return state.getValue();
    }
  //
  string EmuPeripheralCrateManager::extractRunNumber(xoap::MessageReference message)
    {
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
      xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
      xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("RunNumber", "", "")))[0];
      
      return state.getValue();
    }
  // 
  // Create a XRelay SOAP Message
  //
  xoap::MessageReference EmuPeripheralCrateManager::QueryLTCInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      //
      xoap::SOAPName command    = envelope.createName("ParameterGet","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties",  "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName parameter  = envelope.createName("stateName",   "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName parameter2 = envelope.createName("RunNumber",   "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      //
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      //
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");
      //
      xoap::SOAPElement parameter_e2 = properties_e.addChildElement(parameter2);
      parameter_e2.addAttribute(xsitype, "xsd:unsignedLong");      
      //
      return message;
    }
  //
  xoap::MessageReference EmuPeripheralCrateManager::QueryPeripheralCrateInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPName command = envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate");
      xoap::SOAPName parameter   = envelope.createName("stateName", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate");
      xoap::SOAPName parameter2  = envelope.createName("CalibrationState", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");

      parameter_e = properties_e.addChildElement(parameter2);
      parameter_e.addAttribute(xsitype, "xsd:string");

      return message;
    }
  //
  xoap::MessageReference EmuPeripheralCrateManager::QueryJobControlInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPName command = envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties", "JobControl", "urn:xdaq-application:JobControl");
      xoap::SOAPName parameter  = envelope.createName("stateName", "JobControl", "urn:xdaq-application:JobControl");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");

      return message;
    }
//
/* xoap::MessageReference EmuPeripheralCrateManager::LTCResponse(xoap::MessageReference message) throw (xoap::exception::Exception)
{
  //
  cout << "LTC Response Received \n";
  LTCDone=1;
  //
  
  //return createReply(message);
  //
} */


  //
  xoap::MessageReference EmuPeripheralCrateManager::onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //
    std::cout<< "This is a checking printing"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;
    cout << " Print check working "<<endl;

    fireEvent("Configure");
    //
    SendSOAPMessageXRelaySimple("Configure","");
    //
    return createReply(message);
    //
  }
  //
  xoap::MessageReference EmuPeripheralCrateManager::onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    //
    std::cout<< "This is a checking printing for OnConfigCal0"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;
    cout << " Print check working in OnConfigCal0 "<<endl;

    //implement the cal0 configure process:
    float dac;
    int nsleep = 100;  
    std::cout << "DMB setup for calibration " << std::endl;
    calsetup=0;
    dac=1.00;

    //define broadcast crate and board, if not defined before
    if (!broadcastCrate) {
      cout <<" Broadcast crate has not been defined yet"<<endl;
      MyController = new EmuController();
      MyController->SetConfFile("/nfshome0/cscpro/config/pc/broadcast.xml");
      MyController->init();
      CrateSelector selector = MyController->selector();
      vector<Crate *> tmpcrate=selector.broadcast_crate();
      broadcastCrate=tmpcrate[0];
      broadcastDMB=selector.daqmbs(tmpcrate[0])[0];
      broadcastTMB=selector.tmbs(tmpcrate[0])[0];
    }
    cout <<" Broadcast Crate and DMB are defined "<<endl;

    broadcastTMB->DisableCLCTInputs();
    std::cout << "Disabling inputs for TMB slot  " << broadcastTMB->slot() << std::endl;
    broadcastTMB->DisableALCTInputs();

    std::cout << "Set DAC for DMB slot  " << broadcastDMB->slot() << std::endl;
    broadcastDMB->set_cal_dac(dac,dac);
    cout <<" DAC is set to: "<<dac<<endl;
    //Enable CLCT (bit0=1), disable L1A (bit1=0) on DMB calibration
    broadcastDMB->settrgsrc(1);
    
    //set the default DMB Calibration timing:
    /* // change the itim to adjust the pulse position
      int cal_delay_bits = (calibration_LCT_delay_ & 0xF)
      | (calibration_l1acc_delay_ & 0x1F) << 4
      | (itim & 0x1F) << 9
      | (inject_delay_ & 0x1F) << 14;
    */
    int dword= (6 | (20<<4) | (10<<9) | (15<<14) ) &0xfffff;
    broadcastDMB->setcaldelay(dword);

    cout << " The Peripheral Crate configure finished "<<endl;
    usleep(nsleep);

    //    fireEvent("Configure");
    //
    return createReply(message);
    //
  }
  //
  xoap::MessageReference EmuPeripheralCrateManager::onEnable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Enable");

    return createReply(message);
  }
  //
  xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBComparator (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    float dac, threshold;
    int nsleep = 100, highthreshold;  
    //  std::cout<< "This is a checking printing for OnEnableCalCFEBComparator"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;

    calsetup++;

    //implement the comparator setup process:
    std::cout << "DMB setup for CFEB Comparator, calsetup= " <<calsetup<< std::endl;

    //Start the setup process:

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

    return createReply(message);
  }
  xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBGains (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    float dac;
    int nsleep = 100;  
    //  std::cout<< "This is a checking printing for OnEnableCalCFEBGains"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;

    calsetup++;

    //implement the cal0 setup process:
    std::cout << "DMB setup for CFEB Gain, calsetup= " <<calsetup<< std::endl;

    //Start the setup process:
    int gainsetting =((calsetup-1)%20);
    int nstrip=(calsetup-1)/20;
    if (!gainsetting) broadcastDMB->buck_shift_ext_bc(nstrip);
    dac=0.1+0.25*gainsetting;
    broadcastDMB->set_cal_dac(dac,dac);
    cout <<" The strip was set to: "<<nstrip<<" DAC was set to: "<<dac <<endl;
    usleep(nsleep);
    //    fireEvent("Enable");

    return createReply(message);
  }


  xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    int nsleep = 100;  
    //
    std::cout<< "This is a checking printing for OnEnableCalCFEBCrossTalk"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;

    calsetup++;

    //implement the cal0 setup process:
    std::cout << "DMB setup for CFEB Time, calsetup= " <<calsetup<< std::endl;

    //Start the setup process:
    int timesetting =((calsetup-1)%10);
    int nstrip=(calsetup-1)/10;
    if (!timesetting) broadcastDMB->buck_shift_ext_bc(nstrip);
    broadcastDMB->set_cal_tim_pulse(timesetting+5);
    cout <<" The strip was set to: "<<nstrip<<" Time was set to: "<<timesetting <<endl;
    usleep(nsleep);
    //    fireEvent("Enable");

    return createReply(message);
  }

  xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBSCAPed (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    float dac;
    int nsleep = 100;  
    //
    std::cout<< "This is a checking printing for OnEnableCalCFEBSCAPed"<<std::endl;
    ostringstream test;
    message->writeTo(test);
    cout << test.str() <<endl;

    calsetup++;

    //implement the CFEB_Pedestal setup process:
    std::cout << "DMB setup for CFEB Pedestal, calsetup= " <<calsetup<< std::endl;

    //Start the setup process: Set all channel to normal, DAC to 0:
    broadcastDMB->buck_shift_ext_bc(-1);
    dac=0.0;
    broadcastDMB->set_cal_dac(dac,dac);
    cout <<" The strip was set to: -1, " <<" DAC was set to: "<<dac <<endl;
    usleep(nsleep);
    //    fireEvent("Enable");

    return createReply(message);
  }

  xoap::MessageReference EmuPeripheralCrateManager::onDisable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Disable");

    return createReply(message);
  }
  //
  xoap::MessageReference EmuPeripheralCrateManager::onHalt (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Halt");

    return createReply(message);
  }
 //
  xoap::MessageReference EmuPeripheralCrateManager::createXRelayMessage(const std::string & command, const std::string & setting,
					     std::set<xdaq::ApplicationDescriptor * > descriptor )
    {
      // Build a SOAP msg with the Xrelay header:
      xoap::MessageReference msg  = xoap::createMessage();
    //
    std::string topNode = "relay";
    std::string prefix = "xr";
    std::string httpAdd = "http://xdaq.web.cern.ch/xdaq/xsd/2004/XRelay-10";
    xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
    xoap::SOAPName envelopeName = envelope.getElementName();
    xoap::SOAPHeader header = envelope.addHeader();
    xoap::SOAPName relayName = envelope.createName(topNode, prefix,  httpAdd);
    xoap::SOAPHeaderElement relayElement = header.addHeaderElement(relayName);
    
    // Add the actor attribute
    xoap::SOAPName actorName = envelope.createName("actor", envelope.getElementName().getPrefix(),
						   envelope.getElementName().getURI());
    relayElement.addAttribute(actorName,httpAdd);
    
    // Add the "to" node
    std::string childNode = "to";
    // Send to all the destinations:
    //
     std::set<xdaq::ApplicationDescriptor * >  descriptorsXrelays =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("broker")->getApplicationDescriptors("XRelay");
    // xdata::UnsignedIntegerT lid4=4;
    // std::set<xdaq::ApplicationDescriptor * >  descriptorsXrelays =
    //     getApplicationContext()->getZone("default")->getApplicationGroup("broker")->getApplicationDescriptors("XRelay");
 
   //
    std::cout << "descriptorXrelays size = " << descriptorsXrelays.size() << std::endl;
    //
    std::set<xdaq::ApplicationDescriptor * >::iterator  itDescriptorsXrelays = descriptorsXrelays.begin();
    
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
        std::string classNameStr = (*itDescriptor)->getClassName();
	//
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();
	//
	std::string urlXRelay = (*itDescriptorsXrelays)->getContextDescriptor()->getURL();
 	std::string urnXRelay = (*itDescriptorsXrelays)->getURN();
	itDescriptorsXrelays++;
	if (itDescriptorsXrelays ==  descriptorsXrelays.end()) itDescriptorsXrelays=descriptorsXrelays.begin();
	//
	xoap::SOAPName toName = envelope.createName(childNode, prefix, " ");
	xoap::SOAPElement childElement = relayElement.addChildElement(toName);
	xoap::SOAPName urlName = envelope.createName("url");
	xoap::SOAPName urnName = envelope.createName("urn");
	childElement.addAttribute(urlName,urlXRelay);
	childElement.addAttribute(urnName,urnXRelay);
	xoap::SOAPElement childElement2 = childElement.addChildElement(toName);
	childElement2.addAttribute(urlName,url);
	childElement2.addAttribute(urnName,urn);
	//
      }
    //
    // Create body
    //
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName cmd  = envelope.createName(command,"xdaq","urn:xdaq-soap:3.0");
    xoap::SOAPElement queryElement = body.addBodyElement(cmd);
    //
    if(setting != "" ) {
      xoap::SOAPName att  = envelope.createName("Setting");
      queryElement.addAttribute(att,setting);
    }
    //
    //
    // msg->writeTo(std::cout);
    //
    return msg;
    //
  }
  
  // Post XRelay SOAP message to XRelay application
  void EmuPeripheralCrateManager::relayMessage (xoap::MessageReference msg) throw (xgi::exception::Exception)
  {
    // Retrieve the list of applications expecting this command and build the XRelay header
    xoap::MessageReference reply;
    try 
      {	
	// Get the Xrelay application descriptor and post the message:
	xdaq::ApplicationDescriptor * xrelay = getApplicationContext()->getDefaultZone()->
	  getApplicationGroup("broker")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),4);
	
	reply = getApplicationContext()->postSOAP(msg, xrelay);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	} else {
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	}
      } 
    catch (xdaq::exception::Exception& e) 
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot relay message", e);
      }
    
     std::cout << "Finish relayMessage" << std::endl;
    
  }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageConnectTStore(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "Send SOAP message connect tstore" <<std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
	xoap::SOAPName msgName = envelope.createName( "connect", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(id, "myTStore");
	xoap::SOAPName passwordName = envelope.createName("password", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	//queryElement.addAttribute(passwordName, "alct2_emu");       
	queryElement.addAttribute(passwordName, "config_emu_pass");       
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" <<std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	//
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection to TStore. " << body.getFault().getFaultString() << std::endl;
	}
      } 
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" <<std::endl;
      }
      //
      //this->Default(in,out);      
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageDisconnectTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "Send SOAP message disconnect tstore" <<std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
	xoap::SOAPName msgName = envelope.createName( "disconnect", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	
	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(id, "myTStore");
	xoap::SOAPName passwordName = envelope.createName("password", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(passwordName, "alct2_emu");       
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" <<std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	//
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection to TStore" <<std::endl;
	}
      } 
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" <<std::endl;
      }
      //
      this->Default(in,out);      
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception)
    {
      //
      this->SendSOAPMessageQueryTStore(in,out,"VME+2/5_alct");
      //
    }
  //
  void EmuPeripheralCrateManager::RetrieveTStoreTable(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception)
    {
      this->RetrieveTStoreTable(in,out,"VME+2/5");
    }
  //
  std::vector < std::vector <std::string > > EmuPeripheralCrateManager::ConvertTable(xdata::Table thisTable) 
    throw (xgi::exception::Exception)
    {
      //
      std::vector < std::vector <std::string > > push;
      //
      for(unsigned int rows=0; rows<thisTable.getRowCount(); rows++){
	//
	push.push_back(std::vector<std::string>());
	std::cout << std::endl;      
	//
	vector<std::string>::iterator columnIterator;
	std::vector<std::string> columns=thisTable.getColumns();
	for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
	  //
	  push[rows].push_back(thisTable.getValueAt(rows,*columnIterator)->toString());
	  std::cout << " " << thisTable.getValueAt(rows,*columnIterator)->toString() ;
	  //
	}
	//
      }
      return push;
    }
  //
  void EmuPeripheralCrateManager::RetrieveTStoreTable(xgi::Input * in, xgi::Output * out, std::string CrateLabel) 
    throw (xgi::exception::Exception){
    //
    this->SendSOAPMessageConnectTStore(in,out);
    ostringstream tableName ;
    tableName << CrateLabel << "_periph";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > periph_table = ConvertTable(table_);
    //
    tableName.str("");
    tableName << CrateLabel << "_csc";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > csc_table = ConvertTable(table_);
    //
    tableName.str("");
    tableName << CrateLabel << "_tmb";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > tmb_table = ConvertTable(table_);
    //
    tableName.str("");
    tableName << CrateLabel << "_dmb";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > dmb_table = ConvertTable(table_);
    //
    std::cout << "ALCT query" << std::endl;
    //
    tableName.str("");
    tableName << CrateLabel << "_alct";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > alct_table = ConvertTable(table_);
    //
    std::cout << "AFEB query" << std::endl;
    //
    tableName.str("");
    tableName << CrateLabel << "_afeb";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());    
    //
    std::vector < std::vector <std::string > > afeb_table = ConvertTable(table_);
    //
    tableName.str("");
    tableName << CrateLabel << "_cfeb";
    this->SendSOAPMessageQueryTStore(in,out,tableName.str());
    //
    std::vector < std::vector <std::string > > cfeb_table = ConvertTable(table_);
    //
    TStoreParser(periph_table,csc_table,tmb_table,dmb_table,alct_table,afeb_table,cfeb_table);
    //
  }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout <<"Send SOAP query" << std::endl;
      //
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      //
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("EmuPeripheralCrateManager") << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "query", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, TableName);
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong query. " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good query" << std::endl;
	  //
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  //
	  std::list<xoap::AttachmentPart*>::iterator j;
	  for ( j = attachments.begin(); j != attachments.end(); j++ )
	    {//
	      if ((*j)->getContentType() == "application/xdata+table")
		{
		  xdata::exdr::FixedSizeInputStreamBuffer inBuffer((*j)->getContent(),(*j)->getSize());
		  std::string contentEncoding = (*j)->getContentEncoding();
		  std::string contentId = (*j)->getContentId();
		  //
		  try 
		    {
		      xdata::exdr::Serializer serializer;
		      serializer.import(&table_, &inBuffer );
		      OutputTable(in,out,table_);
		    }
		  catch(xdata::exception::Exception & e )
		    {
		      // failed to import table
		      std::cout << "Failed to import table" << std::endl;
		    }
		}
	      else
		{
		  // unknown attachment type 
		  std::cout << "Unknown attachment type" <<std::endl;
		}
	    }
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);      
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageDefinitionTStore(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception)
    {
      //
      this->SendSOAPMessageDefinitionTStore(in,out,"myTable2");
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageDefinitionTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout <<"Send SOAP definition" << std::endl;
      //
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      //
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("EmuPeripheralCrateManager") << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "definition", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, TableName);
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong definition " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good definitiion " << std::endl;
	  //
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  //
	  std::list<xoap::AttachmentPart*>::iterator j;
	  for ( j = attachments.begin(); j != attachments.end(); j++ )
	    {//
	      if ((*j)->getContentType() == "application/xdata+table")
		{
		  xdata::exdr::FixedSizeInputStreamBuffer inBuffer((*j)->getContent(),(*j)->getSize());
		  std::string contentEncoding = (*j)->getContentEncoding();
		  std::string contentId = (*j)->getContentId();
		  //
		  try 
		    {
		      xdata::exdr::Serializer serializer;
		      serializer.import(&table_, &inBuffer );
		      OutputTable(in,out,table_);
		    }
		  catch(xdata::exception::Exception & e )
		    {
		      // failed to import table
		      std::cout << "Failed to import table" << std::endl;
		    }
		}
	      else
		{
		  // unknown attachment type 
		  std::cout << "Unknown attachment type" <<std::endl;
		}
	    }
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);      
      //
    }
  //
  std::string EmuPeripheralCrateManager::MIMETypeFromXdataType(std::string xdataType) {
    std::replace(xdataType.begin(),xdataType.end(),' ','+');
    return "application/xdata+"+xdataType;
  }
  // 
  void EmuPeripheralCrateManager::addAttachment(xoap::MessageReference message,xdata::Serializable &data,std::string contentId) {
    xdata::exdr::AutoSizeOutputStreamBuffer outBuffer;
    xdata::exdr::Serializer serializer;
    serializer.exportAll( &data, &outBuffer );
    xoap::AttachmentPart * attachment = message->createAttachmentPart(outBuffer.getBuffer(), outBuffer.tellp(),MIMETypeFromXdataType(data.type()));
    attachment->setContentEncoding("exdr");
    //attachment->setContentId(contentId);
    message->addAttachmentPart(attachment);
  }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageInsertTStore(xgi::Input * in, xgi::Output * out) 
    throw (xgi::exception::Exception)
    {
      this->SendSOAPMessageInsertTStore(in,out,"myTable1");
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageInsertTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << table_.getRowCount() << std::endl;
      //SetRandomDataTable(table_,0);
      //
      OutputTable(in,out,table_);
      //
      std::cout <<"Send SOAP insert" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "insert", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, TableName);
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      //
      try {
	addAttachment(msg,table_,"");
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong Insert. " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good Insert" << std::endl;
	  //
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageDeleteTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout <<"Send SOAP delete" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "delete", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, "myTable1");
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      //
      try {
	addAttachment(msg,table_,"");
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong Delete " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good Delete" << std::endl;
	  //
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageClearTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout <<"Send SOAP insert" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "drop", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, "myTable1");
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      //
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong Clear " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good Clear" << std::endl;
	  //
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageUpdateTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      this->SendSOAPMessageUpdateTStore(in,out,"myTable1");
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageUpdateTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << table_.getRowCount() << std::endl;
      //SetRandomDataTable(table_,1);
      //
      //OutputTable(in,out,table_);
      //
      std::cout <<"Send SOAP update" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "update", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, TableName);
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      //
      try {
	addAttachment(msg,table_,"");
	xdaq::ApplicationDescriptor * tstoreDescriptor = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong Update " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good Update" << std::endl;
	  //
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      //this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SetRandomDataTable(xdata::Table & table,int Row){
    //
    int columnIndex=1;
    if(Row == -1 ) Row = table.getRowCount();
    std::vector<std::string> columns=table.getColumns();
    vector<std::string>::iterator columnIterator;
    for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++,columnIndex++) {
      string columnType=table.getColumnType(*columnIterator);
      xdata::Serializable *xdataValue=NULL;
      string columnName = (*columnIterator);
      if( columnName.find("KEY") == string::npos) {
	if (columnType=="string" /*&& (*columnIterator)!="SOMETIME"*/) {
	  char letters[] = "aaaaaaaaabbccddddeeeeeeeeeeeeffggghhiiiiiiiiijklmmnnnnnnooooooooppqrrrrrrssssttttttuuuuvvwwxyyz??";
	  random_shuffle(letters, letters+strlen(letters));
	  xdataValue=new xdata::String(string(letters,7)); //voila, a full rack of English Scrabble tiles.
	  //xdataValue=new xdata::String("MyTestNenwnwwn"); //voila, a full rack of English Scrabble tiles.
	} else if (columnType=="int") {
	  xdataValue=new xdata::Integer(rand()%42000);
	} else if (columnType=="float") {
	  //xdataValue=new xdata::Float(20.);
	  xdataValue=new xdata::Float(((rand())%42000)/(float)10000);
	} else if (columnType=="double") {
	xdataValue=new xdata::Double((rand()%42000)/(double)1000000);
	} else if (columnType=="bool") {
	  xdataValue= new xdata::Boolean(rand()%2);
	}
	if (xdataValue) {
	  //
	  //table.setValueAt(table.getRowCount(),*columnIterator,*xdataValue);
	  table.setValueAt(Row,*columnIterator,*xdataValue);
	  delete xdataValue;
	}
      }
    }    
  }
  //
  void EmuPeripheralCrateManager::OutputTable(xgi::Input * in, xgi::Output * out,xdata::Table &results) {
    //
    MyHeader(in,out,"Table");
    //
    std::vector<std::string> columns=results.getColumns();
    vector<std::string>::iterator columnIterator;
    *out << results.getRowCount() << " rows";
    *out << cgicc::table().set("border","2");
    *out << cgicc::tr();
    for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
      *out << cgicc::td() << *columnIterator << " (" << results.getColumnType(*columnIterator) << ")" << cgicc::td();
    }
    *out << cgicc::tr();
    unsigned long rowIndex;
    for (rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
      *out << cgicc::tr();
      for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
	*out << cgicc::td() << results.getValueAt(rowIndex,*columnIterator)->toString() << cgicc::td();
	
      }
      *out << cgicc::tr();
    }
    *out << cgicc::table();
  }
  /*
  void EmuPeripheralCrateManager::SendSOAPMessageOpenFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Open File" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("OpenFile","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("DDUHyperDAQ",0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  */
  void EmuPeripheralCrateManager::SendSOAPMessageConfigureLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage Configure LTC" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command = envelope.createName("Configure","xdaq", "urn:xdaq-soap:3.0");
      body.addBodyElement(command);
      //
      printf(" EmuPeripheralCrateManager: ConfigureLTC \n"); 
      try
	{	
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("LTCControl", 0);
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageExecuteSequence(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage Execute Sequence" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command = envelope.createName("ExecuteSequence","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPBodyElement elem = body.addBodyElement(command);
      xoap::SOAPName name = xoap::SOAPName("Param", "xdaq", XDAQ_NS_URI);
      elem.addAttribute(name, "Dmb_cfeb_calibrate0");
      //
      try
	{	
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("LTCControl", 0);
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  } else {
	    reply->writeTo(std::cout);
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      printf(" SendSoapMessageExecuteSequence \n"); 
      this->Default(in,out);
      //this->SendSOAPMessageExecuteSequence(in,out);
      //
    }
  //
  xoap::MessageReference EmuPeripheralCrateManager::ExecuteCommandMessage(std::string port)
    {
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command  = envelope.createName("executeCommand","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName user     = envelope.createName("user", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName argv     = envelope.createName("argv", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName execPath = envelope.createName("execPath", "", "http://www.w3.org/2001/XMLSchema-instance");
      //
      xoap::SOAPName ldLibraryPath = envelope.createName("LD_LIBRARY_PATH", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName xdaqRoot = envelope.createName("XDAQ_ROOT", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName home = envelope.createName("HOME", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName environment = envelope.createName("EnvironmentVariable","","http://www.w3.org/2001/XMLSchema-instance");
      //
      xoap::SOAPBodyElement itm = body.addBodyElement(command);
      itm.addAttribute(execPath,"/home/meydev/DAQkit/3.9/TriDAS/daq/xdaq/bin/linux/x86/xdaq.exe");
      itm.addAttribute(user,"meydev");
      ostringstream dummy;
      dummy << "-p " << port << " -c /home/cscpc/DAQkit/v3.9/TriDAS/emu/emuDCS/PeripheralCrate/xml/EmuCluster.xml";
      itm.addAttribute(argv,dummy.str());
      xoap::SOAPElement itm2 = itm.addChildElement(environment);
      itm2.addAttribute(home,"/home/meydev");
      itm2.addAttribute(xdaqRoot,"/home/meydev/DAQkit/3.9/TriDAS");
      itm2.addAttribute(ldLibraryPath,"/home/meydev/DAQkit/3.9/TriDAS/emu/emuDCS/PeripheralCrate/lib/linux/x86:/lib/linux/x86:/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/xerces/linuxx86/lib:/home/meydev/DAQkit/3.9/TriDAS/daq/exter:/home/meydev/DAQkit/3.9/TriDAS/daq/xdaq/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/xdata/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/log4cplus/linuxx86/lib:/home/meydev/DAQkit/3.9/TriDAS/daq/toolbox/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/xoap/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/cgicc/linuxx86/lib:/home/meydev/DAQkit/3.9/TriDAS/daq/xcept/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/xgi/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/pt/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/mimetic/linuxx86/lib:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/log4cplus/xmlappender/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/log4cplus/udpappender/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/pt/soap/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/pt/tcp/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/emu/extern/dim/linuxx86/linux:/home/meydev/DAQkit/3.9/TriDAS/emu/emuDCS/e2p/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/emu/cscSV/lib/linux/x86:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/asyncresolv/linuxx86/lib:/home/meydev/DAQkit/3.9/TriDAS/daq/extern/oracle/linuxx86");
      //
      return msg;
      //
    }
  //
  xoap::MessageReference EmuPeripheralCrateManager::killAllMessage()
    {
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command  = envelope.createName("killAll","xdaq", "urn:xdaq-soap:3.0");
      //
      xoap::SOAPBodyElement itm = body.addBodyElement(command);
      //
      return msg;
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageJobControlStartAllEmuperipheralCrate(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage JobControl executeCommand" << std::endl;
      //
      std::set<xdaq::ApplicationDescriptor * >  descriptor =
	getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
      //
      std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
      for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
	{
	  std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	  std::cout << url << std::endl;
	  //
	  unsigned int index = url.find_last_of(":");
	  //
	  if (  index != string::npos ) {
	    //	    
	    std::string port = url.substr(index+1);
	    //
	    xoap::MessageReference msg = ExecuteCommandMessage(port);
	    //
	    try
	      {
		//
		msg->writeTo(std::cout);
		std::cout << std::endl;
		//
		xdaq::ApplicationDescriptor * d = 
		  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("JobControl",0);
		std::cout << d << std::endl;
		xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
		xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
		reply->writeTo(std::cout);
		std::cout << std::endl;
		if (body.hasFault()) {
		  std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
		}
		//
	      } 
	    catch (xdaq::exception::Exception& e)
	      {
		XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	      }
	    //
	  }
	  //
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageJobControlkillAll(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage JobControl killAll" << std::endl;
      //
      xoap::MessageReference msg = killAllMessage();
      //
      try
	{
	  //
	  msg->writeTo(std::cout);
	  std::cout << std::endl;
	  //
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("JobControl",0);
	  std::cout << d << std::endl;
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageQueryLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage Query LTC" << std::endl;
      //
      xoap::MessageReference queryLTC = QueryLTCInfoSpace();
      //
      try
	{	
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("LTCControl", 0);
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(queryLTC, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  std::cout << "Reply" << std::endl;
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageXRelaySimple(std::string command,std::string setting){
    //
      std::set<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
      xoap::MessageReference configure = createXRelayMessage(command,setting,descriptors);
    //
      this->relayMessage(configure);
    //
  }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      SendSOAPMessageXRelaySimple("Configure","");
      //
      // Now check
      //
      ConfigureState_ = "Failed";
      //
      for(int i=0;i<20; i++) {
	int compare=-1;
	compare = CompareEmuPeripheralCrateState("Configured");
	//
	std::set<xdaq::ApplicationDescriptor * >  descriptor =
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
	//
	if ( compare == (int) descriptor.size() ) {
	  ConfigureState_ = "Configured";
	  break;
	}
	//
      }
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageCalibrationXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      SendSOAPMessageXRelaySimple("Calibration","Reset Now");
      //
      for (int i=0; i<20; i++) {
	int compare = -1;
	ostringstream output;  
	output << "Next Setting " << i ;
	SendSOAPMessageXRelaySimple("Calibration",output.str());
	while (compare!=1){
	  compare = CompareEmuPeripheralCrateCalibrationState(output.str());
	  ostringstream compare_string;
	  compare_string << "compare " <<  compare << std::endl;
	  LOG4CPLUS_INFO(getApplicationLogger(), compare_string.str());
	}
        LTCDone=0;
	this->SendSOAPMessageExecuteSequence(in,out);
	// while(LTCDone==0){
        //  output << "waiting for LTCResponse" << std::endl;
	//  ::usleep(200); 
        // }
      }
      //
      this->Default(in,out);
      //
    }
  //
  void EmuPeripheralCrateManager::SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Configure" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("Configure","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	std::set<xdaq::ApplicationDescriptor * >  descriptors =
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
	//
	std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
	for ( itDescriptor = descriptors.begin(); itDescriptor != descriptors.end(); itDescriptor++ ) 
	  {
	    xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	  }    
	//
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //  
//This is copied from CSCSupervisor::sendcommand;
  void EmuPeripheralCrateManager::PCsendCommand(string command, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
  {
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
    std::set<xdaq::ApplicationDescriptor *> apps;
    try {
      apps = getApplicationContext()->getDefaultZone()
		    ->getApplicationDescriptors(klass);
    } catch (xdaq::exception::ApplicationDescriptorNotFound e) {
      return; // Do nothing if the target doesn't exist
    }

    // prepare a SOAP message
    xoap::MessageReference message = PCcreateCommandSOAP(command);
    xoap::MessageReference reply;

    // send the message one-by-one
    std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
    for (; i != apps.end(); ++i) {
      // postSOAP() may throw an exception when failed.
      reply = getApplicationContext()->postSOAP(message, *i);

      //      PCanalyzeReply(message, reply, *i);
    }
  }
//
//This is copied from CSCSupervisor::createCommandSOAP
  xoap::MessageReference EmuPeripheralCrateManager::PCcreateCommandSOAP(string command)
  {
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
    envelope.getBody().addBodyElement(name);

    return message;
  }
//
/*This is copied from CSCSupervisor::analyzeReply
  void EmuPeripheralCrateManager::PCanalyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app)
  {
    string message_str, reply_str;

    reply->writeTo(reply_str);
    ostringstream s;
    s << "Reply from "
      << app->getClassName() << "(" << app->getInstance() << ")" << endl
      << reply_str;
    last_log_.add(s.str());
    LOG4CPLUS_DEBUG(getApplicationLogger(), reply_str);

    xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

    // do nothing when no fault
    if (!body.hasFault()) { return; }

    ostringstream error;

    error << "SOAP message: " << endl;
    message->writeTo(message_str);
    error << message_str << endl;
    error << "Fault string: " << endl;
    error << reply_str << endl;

    LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
    XCEPT_RAISE(xoap::exception::Exception, "SOAP fault: \n" + reply_str);

    return;
  }
*/

//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateManager)
