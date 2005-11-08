// $Id: EmuCrateHyperDAQ.h,v 1.5 2005/11/08 08:10:16 mey Exp $

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


#include "xdaq/Application.h"
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

// My Stuff

#include "PeripheralCrateParser.h"
#include "TestBeamCrateController.h"
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
  TestBeamCrateController tbController;
  TMB *thisTMB(0) ;
  DAQMB* thisDMB(0) ;
  CCB* thisCCB(0) ;
  ALCTController *alct(0) ;
  MPC * thisMPC(0);
  CrateUtilities MyTest;
  ostringstream OutputString;
  ostringstream OutputStringDMBStatus[9];
  ostringstream OutputStringTMBStatus;
  ostringstream* Output;
  vector<TMB*>   tmbVector;
  vector<DAQMB*> dmbVector;
  Crate *thisCrate;
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
    xgi::bind(this,&EmuCrateHyperDAQ::DMBStatus, "DMBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::CCBStatus, "CCBStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::MPCStatus, "MPCStatus");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTests, "DMBTests");
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
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest3, "DMBTest3");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest4, "DMBTest4");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest5, "DMBTest5");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest6, "DMBTest6");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest8, "DMBTest8");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest9, "DMBTest9");
    xgi::bind(this,&EmuCrateHyperDAQ::DMBTest10, "DMBTest10");
    xgi::bind(this,&EmuCrateHyperDAQ::TriggerTestInjectALCT, "TriggerTestInjectALCT");
    xgi::bind(this,&EmuCrateHyperDAQ::TriggerTestInjectCLCT, "TriggerTestInjectCLCT");
    //
    myParameter_ =  0;
    //
    //xmlFile_     = "/afs/cern.ch/user/m/mey/configurations/timingME+3-2-32.xml" ;
    //
    xmlFile_     = 
      "/afs/cern.ch/user/m/mey/scratch0/v3.2/TriDAS/emu/emuDCS/PeripheralCrate/config.xml" ;
    //
    OutputString << "Output..." << std::endl;
    for(int i=0; i<8;i++) OutputStringDMBStatus[i] << "Output..." << std::endl;
    OutputStringTMBStatus << "Output..." << std::endl;
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
    if (tmbVector.size()==0 && dmbVector.size()==0) {
      //
      std::string method =
	toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      //
      *out << cgicc::legend("Upload Configuration...").set("style","color:blue") << cgicc::p() << std::endl ;
      *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
      *out << cgicc::input().set("type","text")
	.set("name","xmlFilename")
	.set("size","60")
	.set("ENCTYPE","multipart/form-data")
	.set("value",xmlFile_);
      //
      *out << std::endl;
      //
      *out << cgicc::input().set("type","submit").set("value","Set configuration file local") << std::endl ;
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
    

    *out << std::endl;

    *out << cgicc::fieldset();
    *out << std::endl;
    //
    } else if (tmbVector.size()>0 || dmbVector.size()>0) {
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      //
      *out << cgicc::legend("Crate Configuration...").set("style","color:blue") << cgicc::p() << std::endl ;
      //
      for(int ii=0; ii<24; ii++) {
	//
	*out << "Slot " << setfill('0') << setw(2) << dec << ii << endl;
	//
	char Name[20] ;
	std::string CCBStatus =
	  toolbox::toString("/%s/CCBStatus",getApplicationDescriptor()->getURN().c_str());
	int slot = thisCrate->ccb()->slot() ;
	sprintf(Name,"CCB Status slot=%d",slot);
	if(slot == ii) *out << cgicc::a(Name).set("href",CCBStatus).set("target","_blank") << endl;
	//
	std::string MPCStatus =
	  toolbox::toString("/%s/MPCStatus",getApplicationDescriptor()->getURN().c_str());
	slot = thisCrate->mpc()->slot() ;
	sprintf(Name,"MPC Status slot=%d",slot);
	if(slot == ii) *out << cgicc::a(Name).set("href",MPCStatus).set("target","_blank") << endl;
	//
	std::string TMBStatus[9] ;
	for (int i=0; i<tmbVector.size(); i++) {
	  TMBStatus[i] =
	    toolbox::toString("/%s/TMBStatus?%d",getApplicationDescriptor()->getURN().c_str(),i);
	  int slot = tmbVector[i]->slot();
	  sprintf(Name,"TMB Status slot=%d",tmbVector[i]->slot());
	  if(slot == ii) *out << cgicc::a(Name).set("href",TMBStatus[i]).set("target","_blank") << endl;
	}
	//
	std::string DMBStatus[9];
	std::string DMBTests[9];
	//
	for (int i=0; i<dmbVector.size(); i++) {
	  DMBStatus[i] =
	    toolbox::toString("/%s/DMBStatus?%d",getApplicationDescriptor()->getURN().c_str(),i);
	  DMBTests[i] =
	    toolbox::toString("/%s/DMBTests?%d",getApplicationDescriptor()->getURN().c_str(),i);
	  int slot = dmbVector[i]->slot();
	  if(slot == ii ) {
	    sprintf(Name,"DMB Status slot=%d",dmbVector[i]->slot());
	    *out << cgicc::a(Name).set("href",DMBStatus[i]).set("target","_blank") << endl;
	    sprintf(Name,"DMB Tests  slot=%d",dmbVector[i]->slot());
	    *out << cgicc::a(Name).set("href",DMBTests[i]).set("target","_blank") << endl;
	  }
	}
	//
	*out<< cgicc::br() ;
	//
      }
      //
      *out << cgicc::fieldset();
      //
      std::string InitSystem =
	toolbox::toString("/%s/InitSystem",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","GET").set("action",InitSystem) << std::endl ;
      *out << cgicc::input().set("type","submit").set("value","Init System") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
    }
    //
    //std::string CrateTests =
    //toolbox::toString("/%s/CrateTests",getApplicationDescriptor()->getURN().c_str());
    //*out << cgicc::a("Crate Tests").set("href",CrateTests).set("target","_blank") << endl;
    //
    cout << "Here4" << endl ;
    //
  }
  //  
  void EmuCrateHyperDAQ::InitSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    cout << "Init System" << endl ;
    //
    MakeReference(in,out);
    //
    tbController.configureNoDCS();          // Init system
    //thisTMB->StartTTC();
    //thisTMB->EnableL1aRequest();
    thisCCB->setCCBMode(CCB::VMEFPGA);      // It needs to be in FPGA mode to work.
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest3(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test3" << endl ;
    //
    MakeReference(in,out);
    //
    //thisDMB->RedirectOutput(&OutputStringDMBStatus);
    thisDMB->RedirectOutput(&std::cout);
    thisDMB->test3();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest4(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test4" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test4();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest5(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test5" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test5();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest6(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test6" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test6();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest8(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test8" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test8();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest9(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test9" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test9();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBTest10(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    cout << "DMB Test10" << endl ;
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    int pass = thisDMB->test10();
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::DMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MakeReference(in,out);
    //
    thisDMB->RedirectOutput(Output);
    thisDMB->PrintCounters(1);
    thisDMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::CrateTests(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    //*out << cgicc::HTTPHTMLHeader();
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
    *out << cgicc::form() << std::endl ;
    //
    std::string TMBStartTrigger =
      toolbox::toString("/%s/TMBStartTrigger",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBStartTrigger) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","TMBStartTrigger") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string EnableL1aRequest =
      toolbox::toString("/%s/EnableL1aRequest",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",EnableL1aRequest) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","EnableL1aRequest") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string ALCTTiming =
      toolbox::toString("/%s/ALCTTiming",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ALCTTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT Timing") << std::endl ;
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
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::pre();
    for(int i=0;i<5;i++) *out << "CFEB " << i << " rx Phase = " << 
			   MyTest.GetCFEBrxPhase(i) << std::endl;
    *out << cgicc::pre();
    //
    std::string ALCTScan =
      toolbox::toString("/%s/ALCTScan",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",ALCTScan) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ALCT Scan") << std::endl ;
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
    std::string CFEBScan =
      toolbox::toString("/%s/CFEBScan",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",CFEBScan) << std::endl ;
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
    *out << cgicc::form() << std::endl ;
    //
    *out << MyTest.GetTMBL1aTiming() ;
    //
    *out << cgicc::fieldset();
    *out << std::endl;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << OutputString.str() << endl ;
    OutputString.clear();
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
    thisTMB->RedirectOutput(&OutputString);
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
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
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
    MyTest.SetTMB(thisTMB);
    MyTest.SetDMB(thisDMB);
    MyTest.SetCCB(thisCCB);
    //
    MyTest.TMBL1aTiming();
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
    cout << "DMBTurnOff" << endl;
    //
    MakeReference(in,out);
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x0);
    }
    //

  }
  //
  void EmuCrateHyperDAQ::DMBTurnOn(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cout << "DMBTurnOn" << endl;
    //
    MakeReference(in,out);
    //
    if (thisDMB) {
      thisDMB->lowv_onoff(0x3f);
    }
    //
  }
  //
  void EmuCrateHyperDAQ::TMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MakeReference(in,out);
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus);
    thisTMB->GetCounters();
    thisTMB->PrintCounters();
    thisTMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::TMBResetCounters(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MakeReference(in,out);
    //
    thisTMB->ResetCounters();
    //
  }
  //
  void EmuCrateHyperDAQ::TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MakeReference(in,out);
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus);
    thisTMB->TriggerTestInjectALCT();
    thisTMB->RedirectOutput(&std::cout);
    //
  }
  //
  void EmuCrateHyperDAQ::TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    MakeReference(in,out);
    //
    thisTMB->RedirectOutput(&OutputStringTMBStatus);
    thisTMB->TriggerTestInjectCLCT();
    thisTMB->RedirectOutput(&std::cout);
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
	sprintf(buf,"CFEB %d",(*cfebItr).number());
	*out << buf;
	*out << cgicc::br();
	sprintf(buf,"CFEB prom user id : %08x CFEB fpga user id : %08x ",
		thisDMB->febpromuser(*cfebItr),
		thisDMB->febfpgauser(*cfebItr));
	*out << buf;
	*out << cgicc::br();
      }
      //
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
    *out << cgicc::fieldset();
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
    int tmb = atoi(tmbStr.c_str());
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
    *out << std::endl;    
    //
    std::string TMBPrintCounters =
      toolbox::toString("/%s/TMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBPrintCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Print Counters") ;
    *out << cgicc::form() ;
    //
    std::string TMBResetCounters =
      toolbox::toString("/%s/TMBResetCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TMBResetCounters) ;
    *out << cgicc::input().set("type","submit").set("value","TMB Reset Counters") ;
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectALCT =
      toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
    *out << cgicc::form() << std::endl ;
    //
    std::string TriggerTestInjectCLCT =
      toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
    *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::pre();
    *out << cgicc::textarea().set("name","CrateTestTMBOutput")
      .set("rows","20")
      .set("cols","100")
      .set("WRAP","OFF");
    *out << OutputStringTMBStatus.str() << endl ;
    *out << cgicc::textarea();
    OutputStringTMBStatus.clear();
    *out << cgicc::pre();
    *out << cgicc::form() << std::endl ;
    //
  }
  //
  void EmuCrateHyperDAQ::DMBStatus(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
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
    thisDMB = dmbVector[dmb];
    Output  = &OutputStringDMBStatus[dmb];
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
    *out << cgicc::td();
    sprintf(buf,"CFEB1 3.3V = %3.2f ",thisDMB->lowv_adc(3,3)/1000.);
    if ( thisDMB->lowv_adc(3,3)/1000. < 3.3*(0.95) ||
	 thisDMB->lowv_adc(3,3)/1000. > 3.3*(1.05) ) {	 
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 5.0V = %3.2f ",thisDMB->lowv_adc(3,4)/1000.);
    if ( thisDMB->lowv_adc(3,4)/1000. < 5.0*0.95 ||
	 thisDMB->lowv_adc(3,4)/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB1 6.0V = %3.2f ",thisDMB->lowv_adc(3,5)/1000.);
    if ( thisDMB->lowv_adc(3,5)/1000. < 6.0*0.95 ||
	 thisDMB->lowv_adc(3,5)/1000. > 6.0*1.05  ) {
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
    sprintf(buf,"CFEB2 3.3V = %3.2f ",thisDMB->lowv_adc(3,6)/1000.);
    if ( thisDMB->lowv_adc(3,6)/1000. < 3.3*0.95 ||
	 thisDMB->lowv_adc(3,6)/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 5.0V = %3.2f ",thisDMB->lowv_adc(3,7)/1000.);
    if ( thisDMB->lowv_adc(3,7)/1000. < 5.0*0.95 ||
	 thisDMB->lowv_adc(3,7)/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB2 6.0V = %3.2f ",thisDMB->lowv_adc(4,0)/1000.);
    if ( thisDMB->lowv_adc(4,0)/1000. < 6.0*0.95 ||
	 thisDMB->lowv_adc(4,0)/1000. > 6.0*1.95 ) {
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
    sprintf(buf,"CFEB3 3.3V = %3.2f ",thisDMB->lowv_adc(4,1)/1000.);
    if ( thisDMB->lowv_adc(4,1)/1000. < 3.3*0.95 ||
	 thisDMB->lowv_adc(4,1)/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 5.0V = %3.2f ",thisDMB->lowv_adc(4,2)/1000.);
    if (  thisDMB->lowv_adc(4,2)/1000. < 5.0*0.95 ||
	  thisDMB->lowv_adc(4,2)/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB3 6.0V = %3.2f ",thisDMB->lowv_adc(4,3)/1000.);
    if ( thisDMB->lowv_adc(4,3)/1000. < 6.0*0.95 ||
	 thisDMB->lowv_adc(4,3)/1000. > 6.0*1.05 ) {
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
    sprintf(buf,"CFEB4 3.3V = %3.2f ",thisDMB->lowv_adc(4,4)/1000.);
    if ( thisDMB->lowv_adc(4,4)/1000. < 3.3*0.95 ||
	 thisDMB->lowv_adc(4,4)/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");      
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 5.0V = %3.2f ",thisDMB->lowv_adc(4,5)/1000.);
    if ( thisDMB->lowv_adc(4,5)/1000. < 5.0*0.95 ||
	 thisDMB->lowv_adc(4,5)/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB4 6.0V = %3.2f ",thisDMB->lowv_adc(4,6)/1000.);
    if ( thisDMB->lowv_adc(4,6)/1000. < 6.0*0.95 ||
	 thisDMB->lowv_adc(4,6)/1000. > 6.0*1.05 ){
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
    sprintf(buf,"CFEB5 3.3V = %3.2f ",thisDMB->lowv_adc(4,7)/1000.);
    if ( thisDMB->lowv_adc(4,7)/1000. < 3.3*0.95 ||
	 thisDMB->lowv_adc(4,7)/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 5.0V = %3.2f ",thisDMB->lowv_adc(5,0)/1000.);
    if ( thisDMB->lowv_adc(5,0)/1000. < 5.0*0.95 ||
	 thisDMB->lowv_adc(5,0)/1000. > 5.0*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"CFEB5 6.0V = %3.2f ",thisDMB->lowv_adc(5,1)/1000.);
    if ( thisDMB->lowv_adc(5,1)/1000. < 6.0*0.95 ||
	 thisDMB->lowv_adc(5,1)/1000. > 6.0*1.05 ) {
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
    sprintf(buf,"ALCT  3.3V = %3.2f ",thisDMB->lowv_adc(5,2)/1000.);
    if ( thisDMB->lowv_adc(5,2)/1000. < 3.3*0.95 ||
	 thisDMB->lowv_adc(5,2)/1000. > 3.3*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  1.8V = %3.2f ",thisDMB->lowv_adc(5,3)/1000.);
    if ( thisDMB->lowv_adc(5,3)/1000. < 1.8*0.95 ||
	 thisDMB->lowv_adc(5,3)/1000. > 1.8*1.95 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V B = %3.2f ",thisDMB->lowv_adc(5,4)/1000.);
    if ( thisDMB->lowv_adc(5,4)/1000. < 5.5*0.95 ||
	 thisDMB->lowv_adc(5,4)/1000. > 5.5*1.05 ) {
      *out << cgicc::span().set("style","color:red");
    } else {
      *out << cgicc::span().set("style","color:green");  
    }
    *out << buf ;
    *out << cgicc::span();
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"ALCT  5.5V A = %3.2f ",thisDMB->lowv_adc(5,5)/1000.);
    if ( thisDMB->lowv_adc(5,5)/1000. < 5.5*0.95 ||
	 thisDMB->lowv_adc(5,5)/1000. > 5.5*1.05 ) {
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
    sprintf(buf,"DMB temperature = %3.1f ",thisDMB->readthermx(0));
    if ( thisDMB->readthermx(0) > 50 && thisDMB->readthermx(0) < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB1 temperature = %3.1f ",thisDMB->readthermx(1));
    if ( thisDMB->readthermx(1) > 50 && thisDMB->readthermx(1) < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
      //
    *out << cgicc::td();
    sprintf(buf,"FEB2 temperature = %3.1f ",thisDMB->readthermx(2));
    if ( thisDMB->readthermx(2) > 50 && thisDMB->readthermx(2) < 95 ) {
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
    sprintf(buf,"FEB3 temperature = %3.1f ",thisDMB->readthermx(3));
    if ( thisDMB->readthermx(3) > 50 && thisDMB->readthermx(3) < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB4 temperature = %3.1f ",thisDMB->readthermx(4));
    if ( thisDMB->readthermx(4) > 50 && thisDMB->readthermx(4) < 95 ) {
      *out << cgicc::span().set("style","color:green");
    } else {
      *out << cgicc::span().set("style","color:red");
    }
    *out << buf ;
    *out << cgicc::span() ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"FEB5 temperature = %3.1f ",thisDMB->readthermx(5));
    if ( thisDMB->readthermx(5) > 50 && thisDMB->readthermx(5) < 95 ) {
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
    sprintf(buf,"DMB DAC1 = %3.1f ",thisDMB->adcplus(2,0));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"DMB DAC2 = %3.1f ",thisDMB->adcplus(2,1));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"DMB DAC3 = %3.1f ",thisDMB->adcplus(2,2));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"DMB DAC4 = %3.1f ",thisDMB->adcplus(2,3));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"DMB DAC5 = %3.1f ",thisDMB->adcplus(2,4));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::table().set("border","1");;
    //
    *out << cgicc::td();
    sprintf(buf,"1.8V Chip1 = %3.1f ",thisDMB->adcplus(1,6));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"1.8V Chip2 = %3.1f ",thisDMB->adcplus(2,6));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::td();
    sprintf(buf,"1.8V Chip3 = %3.1f ",thisDMB->adcplus(3,6));
    *out << buf ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::fieldset();
    *out << std::endl;
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
    int dmb = atoi(dmbStr.c_str());
    //
    std::string test =  env.getReferrer() ;
    cout << test << endl ;
    //
    thisDMB = dmbVector[dmb];
    Output  = &OutputStringDMBStatus[dmb];
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
    std::string DMBTurnOff =
      toolbox::toString("/%s/DMBTurnOff",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOff) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn Off LV") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTurnOn =
      toolbox::toString("/%s/DMBTurnOn",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTurnOn) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Turn On LV") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBPrintCounters =
      toolbox::toString("/%s/DMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBPrintCounters)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Print Counters") << std::endl ;
    *out << cgicc::form() << std::endl ;
    //
    std::string DMBTest3 =
      toolbox::toString("/%s/DMBTest3",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DMBTest3)
	 << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","DMB Test3 (Check DMB Fifos)") << std::endl ;
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
    *out << cgicc::form() << std::endl ;
    //
    *out << cgicc::fieldset() << std::endl;
    //
    *out << cgicc::form().set("method","GET") << std::endl ;
    *out << cgicc::textarea().set("name","CrateTestDMBOutput")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << Output->str() << endl ;
    *out << cgicc::textarea();
    *out << cgicc::form() << std::endl ;
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
    CrateSelector selector = tbController.selector();
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
     //thisTMB = tmbVector[0];
     //thisDMB = dmbVector[0];
     //thisMPC = thisCrate->mpc();
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
