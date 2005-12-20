// $Id: EmuCrateHyperDAQ.h,v 1.24 2005/12/20 23:39:20 mey Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuCrateHyperDAQ_h_
#define _EmuCrateHyperDAQ_h_

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

// My Stuff

#include "PeripheralCrateParser.h"
#include "EmuController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "DDU.h"
#include "DAQMBTester.h"
#include "ALCTController.h"
#include "CrateSelector.h"
#include "CrateUtilities.h"
#include "geom.h"

using namespace cgicc;
using namespace std;

class EmuCrateHyperDAQ: public xdaq::Application 
{
private:
  //
protected:
  //
  xdata::String xmlFile_;
  xdata::UnsignedLong myParameter_;
  EmuController MyController;
  TMB *thisTMB(0) ;
  DAQMB* thisDMB(0) ;
  CCB* thisCCB(0) ;
  ALCTController *alct(0) ;
  MPC * thisMPC(0);
  CrateUtilities MyTest;
  ostringstream CrateTestsOutput;
  ostringstream OutputStringDMBStatus[9];
  ostringstream OutputStringTMBStatus[9];
  ostringstream OutputDMBTests[9];
  int TMBRegisterValue_;
  int CCBRegisterValue_;
  vector<TMB*>   tmbVector;
  vector<DAQMB*> dmbVector;
  Crate *thisCrate;
  std::string Operator_;
  std::string MPCBoardID_;
  std::string CCBBoardID_;
  std::string DMBBoardID_[9];
  std::string TMBBoardID_[9];
  int TMB_, DMB_;
  //
public:
  //
  EmuCrateHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    xgi::bind(this,&EmuCrateHyperDAQ::Default, "Default");
    xgi::bind(this,&EmuCrateHyperDAQ::setConfFile, "setConfFile");
    xgi::bind(this,&EmuCrateHyperDAQ::InitSystem, "InitSystem");
    xgi::bind(this,&EmuCrateHyperDAQ::setRawConfFile, "setRawConfFile");
    xgi::bind(this,&EmuCrateHyperDAQ::UploadConfFile, "UploadConfFile");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBStatus, "TMBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::LoadTMBFirmware, "LoadTMBFirmware");
    xgi::bind(this,&EmuCrateHyperDAQ::ReadTMBRegister, "ReadTMBRegister");
    xgi::bind(this,&EmuCrateHyperDAQ::ReadCCBRegister, "ReadCCBRegister");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBTests,  "TMBTests");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBUtils,  "TMBUtils");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBStatus, "DMBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBBoardID, "DMBBoardID");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBBoardID, "TMBBoardID");
    xgi::bind(this,&EmuCrateHyperDAQ::CCBStatus, "CCBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::CCBUtils, "CCBUtils");
    xgi::bind(this,&EmuCrateHyperDAQ::MPCStatus, "MPCStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTests, "DMBTests");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBUtils, "DMBUtils");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBLoadFirmware, "DMBLoadFirmware");
    xgi::bind(this,&EmuCrateHyperDAQ::CFEBLoadFirmware, "CFEBLoadFirmware");
    xgi::bind(this,&EmuCrateHyperDAQ::CFEBStatus, "CFEBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::ALCTStatus, "ALCTStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::CrateTests, "CrateTests");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTurnOff, "DMBTurnOff");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTurnOn, "DMBTurnOn");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBPrintCounters, "TMBPrintCounters");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBResetCounters, "TMBResetCounters");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBPrintCounters, "DMBPrintCounters");
    xgi::bind(this,&EmuCrateHyperDAQ::ALCTTiming, "ALCTTiming");
    xgi::bind(this,&EmuCrateHyperDAQ::ALCTScan, "ALCTScan");
    xgi::bind(this,&EmuCrateHyperDAQ::CFEBTiming, "CFEBTiming");
    xgi::bind(this,&EmuCrateHyperDAQ::CFEBScan, "CFEBScan");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBStartTrigger, "TMBStartTrigger");
    xgi::bind(this,&EmuCrateHyperDAQ::EnableL1aRequest, "EnableL1aRequest");
    xgi::bind(this,&EmuCrateHyperDAQ::TMBL1aTiming, "TMBL1aTiming");
    xgi::bind(this,&EmuCrateHyperDAQ::ALCTL1aTiming, "ALCTL1aTiming");
    xgi::bind(this,&EmuCrateHyperDAQ::ALCTvpf,"ALCTvpf");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest3, "DMBTest3");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest4, "DMBTest4");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest5, "DMBTest5");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest6, "DMBTest6");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest8, "DMBTest8");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest9, "DMBTest9");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest10, "DMBTest10");
    xgi::bind(this,&EmuCrateHyperDAQ::TriggerTestInjectALCT, "TriggerTestInjectALCT");
    xgi::bind(this,&EmuCrateHyperDAQ::TriggerTestInjectCLCT, "TriggerTestInjectCLCT");
    xgi::bind(this,&EmuCrateHyperDAQ::PowerUp,  "PowerUp");
    xgi::bind(this,&EmuCrateHyperDAQ::Operator, "Operator");
    xgi::bind(this,&EmuCrateHyperDAQ::Operator, "Operator");
    xgi::bind(this,&EmuCrateHyperDAQ::MPCBoardID, "MPCBoardID");
    xgi::bind(this,&EmuCrateHyperDAQ::CCBBoardID, "CCBBoardID");
    xgi::bind(this,&EmuCrateHyperDAQ::LogDMBTestsOutput, "LogDMBTestsOutput");
    xgi::bind(this,&EmuCrateHyperDAQ::SendSOAPMessage, "SendSOAPMessage");
    //
    myParameter_ =  0;
    //
    //xmlFile_     = 
    //"/afs/cern.ch/user/m/mey/scratch0/v3.2/TriDAS/emu/emuDCS/PeripheralCrate/timingME+3-1-16.xml" ;
    //
    xmlFile_     = 
      "/afs/cern.ch/user/m/mey/scratch0/v3.2/TriDAS/emu/emuDCS/PeripheralCrate/config.xml" ;
    //
    TMBRegisterValue_ = -1;
    CCBRegisterValue_ = -1;
    Operator_ = "Name...";
    MPCBoardID_ = "-1";
    CCBBoardID_ = "-1";
    for (int i=0; i<9; i++) { DMBBoardID_[i] = "-1" ; TMBBoardID_[i] = "-1" ; }
    //
    CrateTestsOutput << "Output..." << std::endl;
    for(int i=0; i<9;i++) {
      OutputStringDMBStatus[i] << "Output..." << std::endl;
      OutputStringTMBStatus[i] << "Output..." << std::endl;
      OutputDMBTests[i]        << "Output..." << std::endl;
    }
    //
  }
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    *out << cgicc::h1("904 Testing...");
    *out << cgicc::br();
    //
    std::string methodSOAP =
      toolbox::toString("/%s/SendSOAPMessage",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAP) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure") << std::endl ;
    *out << cgicc::form();
    //
    if (tmbVector.size()==0 && dmbVector.size()==0) {
      //
      std::string method =
	toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      //
      *out << cgicc::legend("Upload Configuration...").set("style","color:blue") 
	   << cgicc::p() << std::endl ;
      //
      *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
      *out << cgicc::input().set("type","text")
	.set("name","xmlFilename")
	.set("size","60")
	.set("ENCTYPE","multipart/form-data")
	.set("value",xmlFile_);
      //
      *out << std::endl;
      //
      *out << cgicc::input().set("type","submit")
	.set("value","Set configuration file local") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      // Upload file...
      //
      std::string methodUpload =
	toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","POST")
	.set("enctype","multipart/form-data")
	.set("action",methodUpload) << std::endl ;
      //
      *out << cgicc::input().set("type","file")
	.set("name","xmlFilenameUpload")
	.set("size","60") ;
    //
      *out << std::endl;
    //
      *out << cgicc::input().set("type","submit").set("value","Send") << std::endl ;
      *out << cgicc::form() << std::endl ;
    //
    //
    *out << std::endl;

    std::string methodRaw =
      toolbox::toString("/%s/setRawConfFile",getApplicationDescriptor()->getURN().c_str());

    *out << cgicc::form().set("method","POST").set("action",methodRaw) << std::endl ;
    *out << cgicc::textarea().set("name","Text")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << "Paste configuration..." << endl ;
    *out << cgicc::textarea();
    *out << cgicc::input().set("type","submit").set("value","Send");
    *out << cgicc::form() << std::endl ;
    
    //
    *out << std::endl;
    //    
    *out << cgicc::fieldset();
    *out << std::endl;
    //
    } else if (tmbVector.size()>0 || dmbVector.size()>0) {
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      //
      std::string Operator =
	toolbox::toString("/%s/Operator",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",Operator) << std::endl ;
      *out << cgicc::input().set("type","text").set("name","Operator")
	.set("size","20").set("value",Operator_) << std::endl ;
      *out << "Operator" << std::endl;
      *out << cgicc::form() << std::endl ;
      //
      std::string PowerUp =
	toolbox::toString("/%s/PowerUp",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",PowerUp)
	.set("target","_blank") << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Power Up") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      std::string InitSystem =
	toolbox::toString("/%s/InitSystem",getApplicationDescriptor()->getURN().c_str());
      //
      //
      *out << cgicc::form().set("method","GET").set("action",InitSystem) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Init System") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      *out << cgicc::fieldset();
      *out << std::endl;
      //
      if ( Operator_.find("Name",0) == string::npos ) {
	//
	*out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
	*out << std::endl;
	//
	*out << cgicc::legend("Crate Configuration...").set("style","color:blue") << 
	  cgicc::p() << std::endl ;
	//
	for(int ii=1; ii<22; ii++) {
	  //
	  *out << cgicc::table().set("border","1");
	  //
	  *out << cgicc::td();
	  //
	  *out << "Slot " << setfill('0') << setw(2) << dec << ii << endl;
	  //
	  *out << cgicc::td();
	  //
	  char Name[50] ;
	  std::string CCBStatus =
	    toolbox::toString("/%s/CCBStatus",getApplicationDescriptor()->getURN().c_str());
	  std::string CCBUtils =
	    toolbox::toString("/%s/CCBUtils",getApplicationDescriptor()->getURN().c_str());
	  std::string CCBBoardID =
	    toolbox::toString("/%s/CCBBoardID",getApplicationDescriptor()->getURN().c_str());
	  int slot = thisCrate->ccb()->slot() ;
	  sprintf(Name,"CCB Status slot=%d",slot);
	  if(slot == ii) {
	    *out << cgicc::td();
	    *out << "CCB Board ID" ;
	    *out << cgicc::form().set("method","GET").set("action",CCBBoardID) << std::endl ;
	    *out << cgicc::input().set("type","text").set("name","CCBBoardID")
	      .set("value",CCBBoardID_) << std::endl ;
	    *out << cgicc::form() << std::endl ;
	    *out << cgicc::td();
	    //
	    *out << cgicc::td();
	    //
	    if ( CCBBoardID_.find("-1") == string::npos ) {
	      //
	      *out << cgicc::form().set("method","GET").set("action",CCBStatus)
		.set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      char buf[20];
	      sprintf(buf,"%d",ii);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ccb");
	      *out << cgicc::form() << std::endl ;
	      //
	    }
	    *out << cgicc::td();
	    //
	    *out << cgicc::td();
	    //
	    if ( CCBBoardID_.find("-1") == string::npos ) {
	      //
	      sprintf(Name,"CCB Utils slot=%d",slot);
	      *out << cgicc::form().set("method","GET").set("action",CCBUtils)
		.set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      char buf[20];
	      sprintf(buf,"%d",ii);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ccb");
	      *out << cgicc::form() << std::endl ;
	      //
	    }
	    *out << cgicc::td();
	    //
	  }
	  //
	  std::string MPCStatus =
	    toolbox::toString("/%s/MPCStatus",getApplicationDescriptor()->getURN().c_str());
	  std::string MPCBoardID =
	    toolbox::toString("/%s/MPCBoardID",getApplicationDescriptor()->getURN().c_str());
	  slot = -1;
	  if ( thisMPC ) slot = thisCrate->mpc()->slot() ;
	  sprintf(Name,"MPC Status slot=%d",slot);
	  if(slot == ii) {
	    //
	    *out << cgicc::td();
	    *out << "MPC Board ID" ;
	    *out << cgicc::form().set("method","GET").set("action",MPCBoardID) << std::endl ;
	    *out << cgicc::input().set("type","text").set("name","MPCBoardID").set("value",MPCBoardID_) << std::endl ;
	    *out << cgicc::form() << std::endl ;
	    *out << cgicc::td();
	    //
	  *out << cgicc::td();
	  if ( MPCBoardID_.find("-1") == string::npos && thisMPC ) {
	    *out << cgicc::form().set("method","GET").set("action",MPCStatus)
	      .set("target","_blank") << std::endl ;
	    *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	    char buf[20];
	    sprintf(buf,"%d",ii);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","mpc");
	    *out << cgicc::form() << std::endl ;
	  }
	  *out << cgicc::td();
	}
	//
	std::string TMBStatus ;
	std::string TMBTests ;
	std::string TMBUtils ;
	std::string TMBBoardID ;
	//
	TMBStatus  =
	  toolbox::toString("/%s/TMBStatus",getApplicationDescriptor()->getURN().c_str());
	TMBBoardID =
	  toolbox::toString("/%s/TMBBoardID",getApplicationDescriptor()->getURN().c_str());
	TMBTests   =
	  toolbox::toString("/%s/TMBTests",getApplicationDescriptor()->getURN().c_str());
	TMBUtils   =
	  toolbox::toString("/%s/TMBUtils",getApplicationDescriptor()->getURN().c_str());
	//
	for (int i=0; i<tmbVector.size(); i++) {
	  //
	  int slot = tmbVector[i]->slot();
	  if(slot == ii) {
	    //
	    *out << cgicc::td();
	    *out << "TMB Board ID" ;
	    *out << cgicc::form().set("method","GET").set("action",TMBBoardID) << std::endl ;
	    char buf[20];
	    sprintf(buf,"TMBBoardID_%d",i);
	    *out << cgicc::input().set("type","text").set("name",buf).set("value",TMBBoardID_[i]) << std::endl ;
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	    *out << cgicc::form() << std::endl ;
	    *out << cgicc::td();
	    //
	    sprintf(Name,"TMB Status slot=%d",tmbVector[i]->slot());	
	    *out << cgicc::td();
	    if ( TMBBoardID_[i].find("-1") == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",TMBStatus)
		.set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	    sprintf(Name,"TMB Tests slot=%d",tmbVector[i]->slot());	  
	    *out << cgicc::td();
	    if ( TMBBoardID_[i].find("-1") == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",TMBTests)
		.set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	    sprintf(Name,"TMB Utils slot=%d",tmbVector[i]->slot());	  
	    *out << cgicc::td();
	    if ( TMBBoardID_[i].find("-1") == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",TMBUtils)
		.set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	    //Found TMB...look for DMB...
	    //
	    for (int iii=0; iii<dmbVector.size(); iii++) {
	      int dmbslot = dmbVector[iii]->slot();
	      std::string CrateTests =
		toolbox::toString("/%s/CrateTests",getApplicationDescriptor()->getURN().c_str());    
	      sprintf(Name,"Crate tests TMBslot=%d DMBslot=%d",slot,dmbslot);
	      //
	      if ( dmbslot == slot+1 ) {
		*out << cgicc::td();
		if ( TMBBoardID_[i].find("-1") == string::npos ) {
		  *out << cgicc::form().set("method","GET").set("action",CrateTests).set("target","_blank") << std::endl ;
		  *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
		  char buf[20];
		  sprintf(buf,"%d",i);
		  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
		  sprintf(buf,"%d",iii);
		  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
		  *out << cgicc::form() << std::endl ;
		}
		*out << cgicc::td();
	      }
	    }
	    //
	  }
	}
	//
	std::string DMBStatus;
	std::string DMBTests;
	std::string DMBUtils;
	std::string DMBBoardID;
	//
	for (int i=0; i<dmbVector.size(); i++) {
	  DMBStatus =
	    toolbox::toString("/%s/DMBStatus",getApplicationDescriptor()->getURN().c_str());
	  DMBTests =
	    toolbox::toString("/%s/DMBTests",getApplicationDescriptor()->getURN().c_str());
	  DMBUtils =
	    toolbox::toString("/%s/DMBUtils",getApplicationDescriptor()->getURN().c_str());
	  DMBBoardID =
	    toolbox::toString("/%s/DMBBoardID",getApplicationDescriptor()->getURN().c_str());
	  int slot = dmbVector[i]->slot();
	  if(slot == ii ) {
	    //
	    *out << cgicc::td();
	    *out << "DMB Board ID" ;
	    *out << cgicc::form().set("method","GET").set("action",DMBBoardID) << std::endl ;
	    char buf[20];
	    sprintf(buf,"DMBBoardID_%d",i);
	    *out << cgicc::input().set("type","text").set("name",buf).set("value",DMBBoardID_[i]) << std::endl ;
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	    *out << cgicc::form() << std::endl ;
	    *out << cgicc::td();
	    //
	    sprintf(Name,"DMB Status slot=%d",dmbVector[i]->slot());
	    *out << cgicc::td();
	    if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",DMBStatus).set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	    sprintf(Name,"DMB Tests  slot=%d",dmbVector[i]->slot());
	    //
	    *out << cgicc::td();
	    if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",DMBTests).set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	    sprintf(Name,"DMB Utils  slot=%d",dmbVector[i]->slot());
	    //
	    *out << cgicc::td();
	    if ( DMBBoardID_[i].find("-1",0) == string::npos ) {
	      *out << cgicc::form().set("method","GET").set("action",DMBUtils).set("target","_blank") << std::endl ;
	      *out << cgicc::input().set("type","submit").set("value",Name) << std::endl ;
	      sprintf(buf,"%d",i);
	      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
	      *out << cgicc::form() << std::endl ;
	    }
	    *out << cgicc::td();
	    //
	  }
	}
	//
	//*out<< cgicc::br() ;
	//
	*out << cgicc::table();
	//
      }
      //
	*out << cgicc::fieldset();
	//
      }
	   //
	   }
      //
      //cout << "Here4" << endl ;
      //
    }
  //
  void EmuCrateHyperDAQ::SendSOAPMessage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage" << std::endl;
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
	xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuCrateSOAP", 0);
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
  //
  void EmuCrateHyperDAQ::InitSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    cout << "Init System" << endl ;
    //
    MyController.configure();          // Init system
    //
    //thisTMB->StartTTC();
    //thisTMB->EnableL1aRequest();
    //
    thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("navigator");
    //
    int navigator;
    if(name != cgi.getElements().end()) {
      navigator = cgi["navigator"]->getIntegerValue();
      cout << "Navigator " << navigator << endl;
      if ( navigator == 1 ) this->CrateTests(in,out);
    } else {
      cout << "No navigator" << endl;
      this->Default(in,out);
    }
  }
  //
  void EmuCrateHyperDAQ::CCBBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    CCBBoardID_= cgi["CCBBoardID"]->getValue() ;
    //
    this->Default(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::MPCBoardID(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    MPCBoardID_= cgi["MPCBoardID"]->getValue() ;
    //
    this->Default(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::PowerUp(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    *out << h1("Run Valery's program");
    //
  }
  //
  void EmuCrateHyperDAQ::Operator(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    Operator_= cgi["Operator"]->getValue() ;
    //
    this->Default(in,out);
  }
  //
  void EmuCrateHyperDAQ::DMBTest3(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test3" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    thisDMB->test3();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest4(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test4" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test4();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest5(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test5" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test5();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest6(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test6" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test6();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest8(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test8" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test8();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuCrateHyperDAQ::DMBTest9(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test9" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test9();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuCrateHyperDAQ::DMBTest10(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    OutputDMBTests[dmb] << "DMB Test10" << endl ;
    //
    thisDMB->RedirectOutput(&OutputDMBTests[dmb]);
    int pass = thisDMB->test10();
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBTests(in,out);
  }
  //
  void EmuCrateHyperDAQ::DMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    thisDMB->RedirectOutput(&std::cout);
    thisDMB->PrintCounters(1);
    thisDMB->RedirectOutput(&std::cout);
    //
    this->DMBUtils(in,out);
  }
  //
  void EmuCrateHyperDAQ::CrateTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    //*out << cgicc::HTTPHTMLHeader();
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ;
    //
    int tmb, dmb;
    //
    //sscanf(crateStr.c_str(),"tmb=%d,dmb=%d",&tmb,&dmb);
    //
    //cout << tmb << " " << dmb << endl;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    thisDMB = dmbVector[dmb];
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("Crate Tests").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    std::string InitSystem =
      toolbox::toString("/%s/InitSystem",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",InitSystem) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Init System") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value","1").set("name","navigator");
    *out << cgicc::form() << std::endl ;
    //
    std::string TMBStartTrigger =
      toolbox::toString("/%s/TMBStartTrigger",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBStartTrigger) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMBStartTrigger") << std::endl ;
    char buf[20];
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string EnableL1aRequest =
      toolbox::toString("/%s/EnableL1aRequest",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",EnableL1aRequest) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","EnableL1aRequest") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string ALCTTiming =
      toolbox::toString("/%s/ALCTTiming",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ALCTTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::pre();
    *out << "ALCT rx Phase = " << MyTest.GetALCTrxPhase() << std::endl;
    //
    *out << "ALCT tx Phase = " << MyTest.GetALCTtxPhase() << std::endl;
    *out << cgicc::pre();
    //
    std::string CFEBTiming =
      toolbox::toString("/%s/CFEBTiming",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CFEBTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","CFEB Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::pre();
    for(int i=0;i<5;i++) *out << "CFEB " << i << " rx Phase = " << 
			   MyTest.GetCFEBrxPhase(i) << std::endl;
    *out << cgicc::pre();
    //
    alct = thisTMB->alctController();
    //
    if ( alct ) {
      //
      std::string ALCTScan =
	toolbox::toString("/%s/ALCTScan",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",ALCTScan) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","ALCT Scan") << std::endl ;
      char buf[20];
      sprintf(buf,"%d",tmb);
      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
      sprintf(buf,"%d",dmb);
      *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
      *out << cgicc::form() << std::endl ;
      //
      cgicc::pre();
      for (int Wire = 0; Wire<(alct->GetWGNumber())/6; Wire++) {
	*out << MyTest.GetALCTWireScan(Wire) ;
      }
      *out << cgicc::br();
      *out << std::endl;
      //
      cgicc::pre();
      //
    }
    //
    std::string CFEBScan =
      toolbox::toString("/%s/CFEBScan",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CFEBScan) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::input().set("type","submit").set("value","CFEB Scan") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    cgicc::pre();
    for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      *out << "CFEB Id="<<CFEBs<< " " ;
      for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) {
	*out << MyTest.GetCFEBStripScan(CFEBs,HalfStrip) ;
      }
      *out << cgicc::br();
      *out << std::endl;
    }
    cgicc::pre();
    //
    std::string TMBL1aTiming =
      toolbox::toString("/%s/TMBL1aTiming",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBL1aTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMB L1a Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << MyTest.GetTMBL1aTiming() ;
    //
    std::string ALCTL1aTiming =
      toolbox::toString("/%s/ALCTL1aTiming",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ALCTL1aTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT L1a Timing") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << MyTest.GetALCTL1aDelay() ;
    //
    std::string ALCTvpf =
      toolbox::toString("/%s/ALCTvpf",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ALCTvpf) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Find ALCT vpf") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << MyTest.GetALCTvpf() ;
    //
    *out << cgicc::br();
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << CrateTestsOutput.str() << endl ;
    CrateTestsOutput.str("");
    *out << cgicc::textarea();
    *out << cgicc::form() << std::endl ;
    //
  }
  //
    void EmuCrateHyperDAQ::TMBStartTrigger(xgi::Input * in, xgi::Output * out ) 
      throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    thisTMB->RedirectOutput(&CrateTestsOutput);
    thisTMB->StartTTC();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->CrateTests(in,out);
    //
  }
  //
    void EmuCrateHyperDAQ::EnableL1aRequest(xgi::Input * in, xgi::Output * out ) 
      throw (xgi::exception::Exception)
  {
    //
    cout << "Enable L1a request" << endl ;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    std::cout << "DMB slot " << thisDMB->slot() << " TMB slot " << thisTMB->slot() << std::endl;
    //
    thisTMB->EnableL1aRequest();
    thisCCB->setCCBMode(CCB::VMEFPGA);
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ALCTTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetCCB(thisCCB);
    MyTest.SetALCT(alct);
    //
    int RXphase, TXphase;
    MyTest.ALCTTiming();
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::CFEBTiming(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cout << "CFEBTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
    thisTMB->RedirectOutput(&CrateTestsOutput);
    MyTest.CFEBTiming();
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::TMBL1aTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "TMBL1aTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
    MyTest.FindTMB_L1A_delay(50,100);
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ALCTL1aTiming(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTL1aTiming" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
      dmb = DMB_;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
      tmb = TMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
    MyTest.FindALCT_L1A_delay(50,100);
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ALCTvpf(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTvpf" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
    MyTest.FindALCTvpf();
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ALCTScan(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "ALCTScan" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetCCB(thisCCB);
    MyTest.SetALCT(alct);
    //
    MyTest.ALCTChamberScan();
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::CFEBScan(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "CFEBScan" << endl;
    //
    cgicc::Cgicc cgi(in);
    //
    int tmb, dmb;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "No dmb" << endl;
    }
    //
    name = cgi.getElement("tmb");
    //
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "No tmb" << endl;
    }
    //
    thisDMB = dmbVector[dmb];
    thisTMB = tmbVector[tmb];
    //
    MyTest.SetTMB(thisTMB);
    MyTest.SetCCB(thisCCB);
    MyTest.SetDMB(thisDMB);
    //
    MyTest.CFEBChamberScan();
    //
    this->CrateTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::MakeReference(xgi::Input * in , xgi::Output * out )
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string dmbStr = env.getQueryString() ;
    int dmb = atoi(dmbStr.c_str());
    //
    std::string test =  env.getReferrer() ;
    cout << test << endl ;
    //
    *out << cgicc::a("Back").set("href",test) << endl;
  }
  //
  void EmuCrateHyperDAQ::DMBTurnOff(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    cout << "DMBTurnOff" << endl;
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x0);
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    cout << "DMBLoadFirmware" << endl;
    //
    if (thisDMB) {
      //
      char *out;
      thisDMB->epromload(MPROM,"dmb6cntl_v17_r2.svf",1,out);  // load mprom
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::CFEBLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    cout << "CFEBLoadFirmware" << endl;
    //
    if (thisDMB) {
      //
      char *out;
      //
      vector<CFEB> thisCFEBs = thisDMB->cfebs();
      for (int i=0; i<thisCFEBs.size(); i++) {
	thisDMB->febpromuser(thisCFEBs[i]);
	thisDMB->epromload(thisCFEBs[i].promDevice(),"cfeb_v4_r2.svf",1,out);  // load mprom
      }
      //
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //

  void EmuCrateHyperDAQ::DMBTurnOn(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    cout << "DMBTurnOn" << endl;
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x3f);
    }
    //
    this->DMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::TMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "TMBPrintCounters "<< endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->GetCounters();
    thisTMB->PrintCounters();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::TMBResetCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    thisTMB = tmbVector[tmb];
    thisTMB->ResetCounters();
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->TriggerTestInjectALCT();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    }
    //
    thisTMB = tmbVector[tmb];
    thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
    thisTMB->TriggerTestInjectCLCT();
    thisTMB->RedirectOutput(&std::cout);
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ALCTStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("ALCT Status").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    ALCTIDRegister sc_id ;
    int err = alct->alct_read_slowcontrol_id(&sc_id);
    *out <<  " ALCT Slowcontrol ID " << sc_id << std::endl;
    //
    *out << cgicc::br();
    //
    err = alct->alct_fast_read_id(sc_id);
    *out <<  " ALCT Fastcontrol ID " << sc_id << std::endl;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuCrateHyperDAQ::CFEBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CFEB Status").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    char buf[200];
    //
    std::vector<CFEB> cfebs = thisDMB->cfebs() ;
    //
    typedef std::vector<CFEB>::iterator CFEBItr;
    //
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
      sprintf(buf,"CFEB %d : ",(*cfebItr).number());
      *out << buf;
      //
      //*out << cgicc::br();
      //
      sprintf(buf,"CFEB prom user id : %08x CFEB fpga user id : %08x ",
	      thisDMB->febpromuser(*cfebItr),
	      thisDMB->febfpgauser(*cfebItr));
      //
      if ( thisDMB->febfpgauser(*cfebItr) == 0xcfeba042 ) {
	*out << cgicc::span().set("style","color:green");
	*out << buf;
	*out << cgicc::span();
      } else {
	*out << cgicc::span().set("style","color:red");
	*out << buf;
	*out << " (Should be 0xcfeb9061) ";
	*out << cgicc::span();
      }
      //
      *out << cgicc::br();
      //
    }
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
  }
  //
  void EmuCrateHyperDAQ::CCBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CCB Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    thisCCB->RedirectOutput(out);
    thisCCB->firmwareVersion();
    thisCCB->RedirectOutput(&std::cout);
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuCrateHyperDAQ::CCBUtils(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("CCB Utils").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    std::string ReadCCBRegister =
      toolbox::toString("/%s/ReadCCBRegister",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ReadCCBRegister) << std::endl ;
    *out << "Read Register (int)..." << std:: endl;
    *out << cgicc::input().set("type","text").set("value","0")
      .set("name","CCBRegister") << std::endl ;
    *out << "Register value : (hex) " << std::hex << CCBRegisterValue_ << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuCrateHyperDAQ::MPCStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("MPC Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    thisMPC->RedirectOutput(out);
    thisMPC->firmwareVersion();
    thisMPC->RedirectOutput(&std::cout);
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuCrateHyperDAQ::TMBTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    alct = thisTMB->alctController();
    //
  }
  //
  void EmuCrateHyperDAQ::TMBStatus(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string tmbStr = env.getQueryString() ;
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    alct = thisTMB->alctController();
    //
    if (alct) {
      std::string ALCTStatus =
	toolbox::toString("/%s/ALCTStatus",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::a("ALCT Status").set("href",ALCTStatus).set("target","_blank") << endl;
    }
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("TMB Info").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::pre();
    //
    sprintf(buf,"TMB Firmware date         : %02x/%02x/%04x ",((thisTMB->FirmwareDate()>>8)&0xff), 
	    (thisTMB->FirmwareDate()&0xff), 
	    (thisTMB->FirmwareYear()&0xffff)  );
    //
    if ( ((thisTMB->FirmwareDate()>>8)&0xff) == 0x11 &&
	 ((thisTMB->FirmwareDate()&0xff)) == 0x15 &&
	 ((thisTMB->FirmwareYear()&0xffff)) == 0x2004 ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Firmware Type             : %01x ",(thisTMB->FirmwareVersion()&0xf));       
    //
    if ( (thisTMB->FirmwareVersion()&0xf) == 0xc ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Firmware Version Code     : %01x ",((thisTMB->FirmwareVersion()>>4)&0xf));       
    //
    if ( ((thisTMB->FirmwareVersion()>>4)&0xf) == 0xd ){
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Geographic Address        : %02d ",((thisTMB->FirmwareVersion()>>8)&0xf));       
    if ( ((thisTMB->FirmwareVersion()>>8)&0xf) == thisTMB->slot() ){
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
      //
    }
    //
    *out << cgicc::br();
    //
    sprintf(buf,"Firmware Revision Code    : %04x ",((thisTMB->FirmwareRevCode())&0xffff));       
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"Power Comparator          : %02x ",((thisTMB->PowerComparator())&0x1f));       
    if ( (thisTMB->PowerComparator())&0x1f == 0x1f ) {
      *out << cgicc::span().set("style","color:green");
      *out << buf ;
      *out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf ;
      *out << cgicc::span();
    }
    //
    *out << cgicc::br();
    //
    *out << cgicc::pre();
    //
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset();
    *out << cgicc::legend("CLCT Info").set("style","color:blue") << cgicc::p() << std::endl ;
    *out << cgicc::pre();
    thisTMB->RedirectOutput(out);
    thisTMB->DecodeCLCT();
    thisTMB->RedirectOutput(&std::cout);
    *out << cgicc::pre();
    *out << cgicc::fieldset();
    //
    if (alct) {
      *out << cgicc::fieldset();
      *out << cgicc::legend("ALCT Info").set("style","color:blue") << cgicc::p() << std::endl ;
      *out << cgicc::pre();
      thisTMB->RedirectOutput(out);
      thisTMB->DecodeALCT();
      thisTMB->RedirectOutput(&std::cout);
      *out << cgicc::pre();
      *out << cgicc::fieldset();
    }
    //
    *out << std::endl;    
    //
  }
  //
  void EmuCrateHyperDAQ::DMBBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    char buf[20];
    sprintf(buf,"DMBBoardID_%d",dmb);
    DMBBoardID_[dmb] = cgi[buf]->getValue();
    //
    this->Default(in,out);
    //
    }
  //
  void EmuCrateHyperDAQ::TMBBoardID(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    char buf[20];
    sprintf(buf,"TMBBoardID_%d",tmb);
    TMBBoardID_[tmb] = cgi[buf]->getValue();
    //
    this->Default(in,out);
    //
    }
  //
  void EmuCrateHyperDAQ::DMBStatus(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    //const CgiEnvironment& env = cgi.getEnvironment();
    //
    //std::string dmbStr = env.getQueryString() ;
    //
    //int dmb = atoi(dmbStr.c_str());
    //
    //std::string test =  env.getReferrer() ;
    //cout << test << endl ;
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    if( thisDMB->cfebs().size() > 0 ) {
      std::string CFEBStatus =
	toolbox::toString("/%s/CFEBStatus",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::a("CFEB Status").set("href",CFEBStatus).set("target","_blank") << endl;
    }
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend("DMB IDs").set("style","color:blue") << cgicc::p() << std::endl ;
    //
    *out << cgicc::pre();
    //
    sprintf(buf,"DMB prom VME->Motherboard          : %08x ",thisDMB->mbpromuser(0));
    *out << buf ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller    : %08x ",thisDMB->mbpromuser(1));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB fpga id                        : %08x ",thisDMB->mbfpgaid());
    *out << buf  ;
    *out << cgicc::br();
      //
    sprintf(buf,"DMB prom VME->Motherboard ID       : %08x ",thisDMB->mbpromid(0));
    *out << buf  ;
    *out << cgicc::br();
    //
    sprintf(buf,"DMB prom Motherboard Controller ID : %08x ",thisDMB->mbpromid(1));
    *out << buf  ;
    *out << cgicc::br();
      //
    sprintf(buf,"DMB fpga user id                   : %x ", thisDMB->mbfpgauser());
    if ( thisDMB->mbfpgauser() == 0x48547172 ) {
	*out << cgicc::span().set("style","color:green");
	*out << buf;
	*out << cgicc::span();
    } else {
      *out << cgicc::span().set("style","color:red");
      *out << buf;
      *out << cgicc::span();
    }
    
    //
    *out << cgicc::pre();
    //
    *out << cgicc::br();
    //
    //thisDMB->lowv_dump();
    //thisDMB->daqmb_adc_dump();
    //thisDMB->daqmb_promfpga_dump();
    //
    *out << cgicc::fieldset();
    *out << endl ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("Voltages & Temperatures").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    *out << cgicc::table().set("border","1");
    //
    float value;
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 3.3V = %3.2f ",(value=thisDMB->lowv_adc(3,3))/1000.);
    if ( value/1000. < 3.3*(0.95) ||
	 value/1000. > 3.3*(1.05) ) {	 
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 5.0V = %3.2f ",(value=thisDMB->lowv_adc(3,4))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 6.0V = %3.2f ",(value=thisDMB->lowv_adc(3,5))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05  ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 3.3V = %3.2f ",(value=thisDMB->lowv_adc(3,6))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 5.0V = %3.2f ",(value=thisDMB->lowv_adc(3,7))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,0))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.95 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    *out << cgicc::td();
    sprintf(buf,"CFEB3 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,1))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 5.0V = %3.2f ",(value=thisDMB->lowv_adc(4,2))/1000.);
    if (  value/1000. < 5.0*0.95 ||
	  value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,3))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,4))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 5.0V = %3.2f ",(value=thisDMB->lowv_adc(4,5))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 6.0V = %3.2f ",(value=thisDMB->lowv_adc(4,6))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ){
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 3.3V = %3.2f ",(value=thisDMB->lowv_adc(4,7))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 5.0V = %3.2f ",(value=thisDMB->lowv_adc(5,0))/1000.);
    if ( value/1000. < 5.0*0.95 ||
	 value/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 6.0V = %3.2f ",(value=thisDMB->lowv_adc(5,1))/1000.);
    if ( value/1000. < 6.0*0.95 ||
	 value/1000. > 6.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  3.3V = %3.2f ",(value=thisDMB->lowv_adc(5,2))/1000.);
    if ( value/1000. < 3.3*0.95 ||
	 value/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  1.8V = %3.2f ",(value=thisDMB->lowv_adc(5,3))/1000.);
    if ( value/1000. < 1.8*0.95 ||
	 value/1000. > 1.8*1.95 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V B = %3.2f ",(value=thisDMB->lowv_adc(5,4))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V A = %3.2f ",(value=thisDMB->lowv_adc(5,5))/1000.);
    if ( value/1000. < 5.5*0.95 ||
	 value/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    sprintf(buf,"DMB temperature = %3.1f ",(value=thisDMB->readthermx(0)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB1 temperature = %3.1f ",(value=thisDMB->readthermx(1)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"FEB2 temperature = %3.1f ",(value=thisDMB->readthermx(2)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB3 temperature = %3.1f ",(value=thisDMB->readthermx(3)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB4 temperature = %3.1f ",(value=thisDMB->readthermx(4)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB5 temperature = %3.1f ",(value=thisDMB->readthermx(5)));
    if ( value > 50 && value < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::tr();
    //
    *out << cgicc::table();
    //
    *out << cgicc::br();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    float readout = thisDMB->adcplus(2,0) ;
    sprintf(buf,"DMB DAC1 = %3.1f ",readout);
    //
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    //
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,1) ;
    sprintf(buf,"DMB DAC2 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    //
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,2) ;
    sprintf(buf,"DMB DAC3 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,3) ;
    sprintf(buf,"DMB DAC4 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,4) ;
    sprintf(buf,"DMB DAC5 = %3.1f ",readout);
    if ( readout > 3400 && readout < 3600 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(1,6) ;
    sprintf(buf,"1.8V Chip1 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(2,6) ;
    sprintf(buf,"1.8V Chip2 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    readout = thisDMB->adcplus(3,6) ;
    sprintf(buf,"1.8V Chip3 = %3.1f ",readout);
    if ( readout > 1700 && readout < 1900 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
  }
  //
  void EmuCrateHyperDAQ::TMBUtils(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("TMB Utils").set("style","color:blue") ;
    //
    std::string LoadTMBFirmware =
      toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Load TMB Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string ReadTMBRegister =
      toolbox::toString("/%s/ReadTMBRegister",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ReadTMBRegister) << std::endl ;
    *out << "Read Register (int)..." << std:: endl;
    *out << cgicc::input().set("type","text").set("value","0")
      .set("name","TMBRegister") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << "Register value : (hex) " << std::hex << TMBRegisterValue_ << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
    std::string TMBPrintCounters =
      toolbox::toString("/%s/TMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBPrintCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Print Counters") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() ;
    //
    std::string TMBResetCounters =
      toolbox::toString("/%s/TMBResetCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBResetCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Reset Counters") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectALCT =
      toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectCLCT =
      toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::pre();
    *out << cgicc::textarea().set("name","CrateTestTMBOutput")
      .set("rows","20")
      .set("cols","100")
      .set("WRAP","OFF");
    *out << OutputStringTMBStatus[tmb].str() << endl ;
    *out << cgicc::textarea();
    OutputStringTMBStatus[tmb].str("");
    *out << cgicc::pre();
    *out << cgicc::form() << std::endl ;
    //
  }
  //
  void EmuCrateHyperDAQ::LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    int debugMode(0);
    int jch(5);
    string chamberType("ME21");
    ALCTController *alct = new ALCTController(thisTMB,chamberType);
    int status = alct->SVFLoad(&jch,"/afs/cern.ch/user/m/mey/scratch0/v3.2/TriDAS/emu/emuDCS/PeripheralCrate/tmb2005e.svf",debugMode);
    //
    if (status >= 0){
      cout << "=== Programming finished"<< endl;
      cout << "=== " << status << " Verify Errors  occured" << endl;
    }
    else{
      cout << "=== Fatal Error. Exiting with " <<  status << endl;
    }
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ReadTMBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      cout << "TMB " << tmb << endl;
      TMB_ = tmb;
    } else {
      cout << "Not tmb" << endl ;
      tmb = TMB_;
    }
    //
    thisTMB = tmbVector[tmb];
    //
    cgicc::form_iterator name2 = cgi.getElement("TMBRegister");
    int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      registerValue = cgi["TMBregister"]->getIntegerValue();
      cout << "Register " << registerValue << endl;
      //
      TMBRegisterValue_ = thisTMB->ReadRegister(registerValue);
      //
    }
    //
    this->TMBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::ReadCCBRegister(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("CCBRegister");
    int registerValue = -1;
    if(name2 != cgi.getElements().end()) {
      registerValue = cgi["CCBregister"]->getIntegerValue();
      cout << "Register " << registerValue << endl;
      //
      CCBRegisterValue_ = thisCCB->ReadRegister(registerValue);
      //
    }
    //
    this->CCBUtils(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBUtils(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string dmbStr = env.getQueryString() ;
    //int dmb = atoi(dmbStr.c_str());
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    thisDMB = dmbVector[dmb];
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("DMB Utils").set("style","color:blue") ;
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    //
    std::string DMBTurnOff =
      toolbox::toString("/%s/DMBTurnOff",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOff) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn Off LV") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::td();
    //
    std::string DMBTurnOn =
      toolbox::toString("/%s/DMBTurnOn",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOn) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn On LV") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    std::string DMBPrintCounters =
      toolbox::toString("/%s/DMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBPrintCounters)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Print Counters") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBLoadFirmware =
      toolbox::toString("/%s/DMBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Load Firmware") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string CFEBLoadFirmware =
      toolbox::toString("/%s/CFEBLoadFirmware",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBLoadFirmware)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","CFEB Load Firmware") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset();
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTests(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string dmbStr = env.getQueryString() ;
    //int dmb = atoi(dmbStr.c_str());
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    //std::string test =  env.getReferrer() ;
    //cout << test << endl ;
    //
    thisDMB = dmbVector[dmb];
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    //
    char buf[200] ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    *out << endl ;
    //
    *out << cgicc::legend("DMB Tests").set("style","color:blue") ;
    //
    std::string DMBTest3 =
      toolbox::toString("/%s/DMBTest3",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest3)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)") << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest4 =
      toolbox::toString("/%s/DMBTest4",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest4)
	 << std::endl ;
    if( thisDMB->GetTestStatus(4) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(4) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(4) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test4 (Check voltages)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //    
    std::string DMBTest5 =
      toolbox::toString("/%s/DMBTest5",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest5)
	 << std::endl ;
    if( thisDMB->GetTestStatus(5) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check Power Register)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(5) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check Power Register)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(5) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test5 (Check PowerRegister)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest6 =
      toolbox::toString("/%s/DMBTest6",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest6)
	 << std::endl ;
    if( thisDMB->GetTestStatus(6) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(6) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(6) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test6 (Check FPGA IDs)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest8 =
      toolbox::toString("/%s/DMBTest8",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest8)
	 << std::endl ;
    if( thisDMB->GetTestStatus(8) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(8) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(8) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test8 (Check Comparator DAC/ADC)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest9 =
      toolbox::toString("/%s/DMBTest9",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest9)
	 << std::endl ;
    if( thisDMB->GetTestStatus(9) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(9) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(9) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test9 (Check CDAC)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest10 =
      toolbox::toString("/%s/DMBTest10",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest10)
	 << std::endl ;
    if( thisDMB->GetTestStatus(10) == -1 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:blue") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(10) > 0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:red") 
	   << std::endl ;
    }
    if( thisDMB->GetTestStatus(10) ==0 ) {
      *out << cgicc::input().set("type","submit")
	.set("value","DMB Test10 (Check SFM)")
	.set("style","color:green") 
	   << std::endl ;
    }
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset() << std::endl;
    //
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestDMBOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << OutputDMBTests[dmb].str() << endl ;
    *out << cgicc::textarea();
    *out << cgicc::form();
    //
    std::string method =
      toolbox::toString("/%s/LogDMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",method) << std::endl ;
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::input().set("type","submit")
      .set("value","Log output").set("name","LogDMBTestsOutput") << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Clear")
      .set("name","ClearDMBTestsOutput") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::cout << "Done" << std::endl;
    //
  }
  //
  void EmuCrateHyperDAQ::LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "LogDMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    //
    int dmb;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      cout << "DMB " << dmb << endl;
      DMB_ = dmb;
    } else {
      cout << "Not dmb" << endl ;
      dmb = DMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearDMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      cout << "Clear..." << endl;
      cout << cgi["ClearDMBTestsOutput"]->getValue() << std::endl ;
      OutputDMBTests[dmb].str("");
      //
    this->DMBTests(in,out);
    }
    //
    thisDMB = dmbVector[dmb];
    //
    cout << DMBBoardID_[dmb] << endl ;
    //
    char buf[20];
    sprintf(buf,"DMBTestsLogFile_%d_%s.log",thisDMB->slot(),DMBBoardID_[dmb].c_str());
    //
    ofstream DMBTestsLogFile;
    DMBTestsLogFile.open(buf);
    DMBTestsLogFile << OutputDMBTests[dmb].str() ;
    DMBTestsLogFile.close();
    //
    OutputDMBTests[dmb].str("");
    //
    this->DMBTests(in,out);
    //
  }
  //
  void EmuCrateHyperDAQ::setRawConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	cout << "setRawConfFile" << endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	ofstream TextFile ;
	TextFile.open("MyTextConfigurationFile.xml");
	TextFile << (cgi["Text"]->getValue()) ; 
	TextFile.close();
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	//Configuring();
	//
	cout << "Out setRawConfFile" << endl ;
	//
	this->Default(in,out);
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
  //
  void EmuCrateHyperDAQ::Configuring(){
    //
    std::cout << "Configuring " << std::endl ;
    //
    //-- parse XML file
    //
    cout << "---- XML parser ----" << endl;
    cout << " Here parser " << endl;
    PeripheralCrateParser parser;
    cout << " Using file " << xmlFile_.toString() << endl ;
    parser.parseFile(xmlFile_.toString().c_str());
    //
    //-- Make sure that only one TMB in one crate is configured
    CrateSelector selector = MyController.selector();
    vector<Crate*> crateVector = selector.crates();
    //
    //if (crateVector.size() > 1){
    //cerr << "Error: only one PeripheralCrate allowed" << endl;
    //exit(1);
    //}
    //
    tmbVector = selector.tmbs(crateVector[0]);
    //if (tmbVector.size() > 1){
    //cerr << "Error: only one TMB in xml file allowed" << endl ;
    //exit(1);
    //}
    //
    dmbVector = selector.daqmbs(crateVector[0]);
    //if (dmbVector.size() > 1){
    //cerr << "Error: only one DMB in xml file allowed" << endl;
    //exit(1);
    //} 
    //
    //-- get pointers to CCB, TMB and DMB
    //
     thisCrate = crateVector[0];
     thisCCB = thisCrate->ccb();
     thisMPC = thisCrate->mpc();
     //thisTMB = tmbVector[0];
     //thisDMB = dmbVector[0];
     //
     //DDU * thisDDU = thisCrate->ddu();
     //if(thisTMB) alct = thisTMB->alctController();
     //
    std::cout << "Done" << std::endl ;
  }
  //

  void EmuCrateHyperDAQ::setConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("xmlFileName");
	//
	cout << "GetFiles string" << endl ;
	//
	if(file != cgi.getFiles().end()) (*file).writeToStream(cout);
	//
	string XMLname = cgi["xmlFileName"]->getValue() ; 
	//
	cout << XMLname  << endl ;
	//
	xmlFile_ = XMLname ;
	//
	Configuring();
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }

  void EmuCrateHyperDAQ::UploadConfFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cout << "UploadConfFileUpload" << endl ;
	//
	cgicc::Cgicc cgi(in);
	//
	const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");
	//
	cout << "GetFiles" << endl ;
	//
	if(file != cgi.getFiles().end()) {
	  ofstream TextFile ;
	  TextFile.open("MyTextConfigurationFile.xml");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	}
	//
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	//
	Configuring();
	//
	cout << "UploadConfFile done" << endl ;
	//
	this->Default(in,out);
	//
      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }

  
};

#endif
