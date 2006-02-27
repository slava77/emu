
/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuFCrateHyperDAQ_h_
#define _EmuFCrateHyperDAQ_h_


#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <stdio.h>

#include "xdaq/Application.h"
#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/FormFile.h"

// My Stuff

#include "FEDCrateParser.h"
#include "TestBeamCrateController.h"
#include "Crate.h"
#include "DDU.h"
#include "DCC.h"
#include "VMEController.h"
#include "CrateSelector.h"
#include "JTAG_constants.h"

#include "EmuFController.h"

extern int irq_start[4];

using namespace cgicc;
using namespace std;

class EmuFCrateHyperDAQ: public xdaq::Application 
{
private:
  //
protected:
  //
  xdata::String svfFile_;
  xdata::String xmlFile_;
  xdata::UnsignedLong myParameter_;
  TestBeamCrateController tbController;
  DDU* thisDDU(0) ;
  DCC* thisDCC(0) ;
  ostringstream CrateTestsOutput;
  ostringstream OutputStringDDUStatus[9];
  ostringstream OutputStringDCCStatus[9];
  vector<DDU*> dduVector;
  vector<DCC*> dccVector;
  Crate *thisCrate;
  std::string Operator_;
  std::string DDUBoardID_[9];
  std::string DCCBoardID_[9];
  int DDU_, DCC_;
  long int timer,ltime;
  int irqprob;
  //
public:
  //
  EmuFCrateHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    xgi::bind(this,&EmuFCrateHyperDAQ::Default, "Default");
    xgi::bind(this,&EmuFCrateHyperDAQ::setConfFile, "setConfFile");
    xgi::bind(this,&EmuFCrateHyperDAQ::setRawConfFile, "setRawConfFile");
    xgi::bind(this,&EmuFCrateHyperDAQ::UploadConfFile, "UploadConfFile");
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUFirmware, "DDUFirmware"); 
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUFpga, "DDUFpga");
    xgi::bind(this,&EmuFCrateHyperDAQ::INFpga0, "INFpga0");
    xgi::bind(this,&EmuFCrateHyperDAQ::INFpga1, "INFpga1");
    xgi::bind(this,&EmuFCrateHyperDAQ::VMEPARA, "VMEPARA");
    xgi::bind(this,&EmuFCrateHyperDAQ::VMESERI, "VMESERI");
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUTextLoad, "DDUTextLoad");
    xgi::bind(this,&EmuFCrateHyperDAQ::DDULoadFirmware,"DDULoadFirmware");
    xgi::bind(this,&EmuFCrateHyperDAQ::VMEIntIRQ,"VMEIntIRQ");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmware,"DCCFirmware");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCLoadFirmware,"DCCLoadFirmware"); 
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmwareReset,"DCCFirmwareReset");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCCommands,"DCCCommands");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
    xgi::bind(this,&EmuFCrateHyperDAQ::IRQTester,"IRQTester");
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUVoltMon,"DDUVoltMon");

    //
    myParameter_ =  0;
    irqprob=0;
    //
    //
    xmlFile_     = 
      "/home/fastdducaen/v3.2/TriDAS/emu/emuDCS/FEDCrate/xml/config.xml" ;
    //
    Operator_ = "Name...";
    for (int i=0; i<9; i++) { DDUBoardID_[i] = "-1" ; DCCBoardID_[i] = "-1" ; }
    //
    this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName",&xmlFile_);
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
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");
    //
    *out << img().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/ddudcc.gif") << std::endl;
    *out << img() << std::endl;
    *out << cgicc::br();

    if (dduVector.size()==0 && dccVector.size()==0) {
      std::string method =
	toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      *out << cgicc::legend("Upload Configuration...").set("style","color:blue") 
	   << std::endl ;
      *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
      *out << cgicc::input().set("type","text")
	.set("name","xmlFilename")
	.set("size","60")
	.set("ENCTYPE","multipart/form-data")
	.set("value",xmlFile_);
      *out << std::endl;
      *out << cgicc::input().set("type","submit")
	.set("value","Set configuration file local") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //
      // Upload file...
      //
      std::string methodUpload =
	toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());
      *out << cgicc::form().set("method","POST")
	.set("enctype","multipart/form-data")
	.set("action",methodUpload) << std::endl ;
      *out << cgicc::input().set("type","file")
	.set("name","xmlFilenameUpload")
	.set("size","60") ;
      *out << std::endl;
      *out << cgicc::input().set("type","submit").set("value","Send") << std::endl ;
      *out << cgicc::form() << std::endl ;
      *out << std::endl;

      std::string methodRaw =
	toolbox::toString("/%s/setRawConfFile",getApplicationDescriptor()->getURN().c_str());

      *out << cgicc::form().set("method","POST").set("action",methodRaw) << std::endl ;
      *out << cgicc::textarea().set("name","Text")
	.set("WRAP","OFF")
	.set("rows","20").set("cols","60");
      *out << "Paste configuration..." << std::endl;
      *out << cgicc::textarea() << std::endl;
      *out << cgicc::input().set("type","submit").set("value","Send");
      *out << cgicc::form() << std::endl ;
      *out << std::endl;
      *out << cgicc::fieldset() << std::endl;

      //JRG, move to End      *out << cgicc::body() << std::endl; 
    } else if (dduVector.size()>0 || dccVector.size()>0) {
      //	std::string DDUBoardID ;
      //        char Name[50] ;
      //JRG, No Need?      *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

      *out << br() << std::endl;
      *out << br() << std::endl;

      *out << "Documents: ";
      *out << a().set("href","http://www.physics.ohio-state.edu/~cms/ddu"); 
      *out <<"DDU WebPage" << std::endl; 
      *out << a() << std::endl;
      *out << " | " << std::endl;
      *out << a().set("href","http://www.physics.ohio-state.edu/~cms/dcc"); 
      *out <<"DCC WebPage" << std::endl; 
      *out << a() << std::endl;
      *out << br() << std::endl;
      *out << "Experts: ";
      *out << a().set("href","mailto:durkin@mps.ohio-state.edu"); 
      *out <<" Stan Durkin " << std::endl; 
      *out << a() << std::endl;
      *out << " | " << std::endl; 
      *out << a().set("href","mailto:gujh@mps.ohio-state.edu"); 
      *out <<" Jianhui Gu " << std::endl; 
      *out << a() << std::endl; 
      *out << " | " << std::endl;
      *out << a().set("href","mailto:gilmore@mps.ohio-state.edu"); 
      *out <<" Jason Gilmore " << std::endl; 
      *out << a() << std::endl; 
      *out << br() << std::endl;
      *out << br() << std::endl;

      DDU_=-99;
      *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
      *out << endl ;
      if(irq_start[0]==0){
	std::string vmeintirq =
	  toolbox::toString("/%s/VMEIntIRQ",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",vmeintirq)
	  .set("target","_blank") << std::endl; 
        *out << cgicc::input().set("type","submit")
	  .set("value","Start VME IRQ Interrupt") << std::endl;
	*out << cgicc::input().set("type","hidden").set("value","0").set("name","ddu") << std::endl;
	*out << cgicc::form() << std::endl ;
      }else{
	std::string vmeintirq2 =
	  toolbox::toString("/%s/VMEIntIRQ",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",vmeintirq2)
	  .set("target","_blank") << std::endl; 
        *out << cgicc::input().set("type","submit")
	  .set("value","Start VME IRQ Monitor") << std::endl;
          *out << cgicc::input().set("type","hidden").set("value","0").set("name","ddu") << std::endl;
         *out << cgicc::form() << std::endl ; 
           
         std::string vmeintirq =
	 toolbox::toString("/%s/VMEIntIRQ",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",vmeintirq)
             .set("target","_blank") << std::endl; 
        *out << cgicc::input().set("type","submit")
	  .set("value","End VME IRQ Interrupt") << std::endl;
          *out << cgicc::input().set("type","hidden").set("value","99").set("name","ddu") << std::endl;
         *out << cgicc::form() << std::endl ;
        }
         std::string dduvoltmon =
	 toolbox::toString("/%s/DDUVoltMon",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",dduvoltmon)
             .set("target","_blank") << std::endl; 
        *out << cgicc::input().set("type","submit")
	  .set("value","Start Volt/Temp Monitor") << std::endl;
         *out << cgicc::form() << std::endl ;

        *out << cgicc::fieldset() << std::endl;
        *out << cgicc::br() << std::endl;

        *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
        *out << endl ;
        printf(" dduVector.size() %d \n",dduVector.size());
	for (int i=0; i<(int)dduVector.size(); i++) {
         *out << cgicc::table().set("border","0").set("rules","none").set("frame","void"); 
          *out << cgicc::tr();
          thisDDU=dduVector[i];
	  int slot = thisDDU->slot();
	  char buf[20];	
          sprintf(buf,"DDU Slot: %d ",slot);
	  printf(" %s \n",buf);      
          *out << buf; 
          if(slot<=21){
            // float adc1=thisDDU->adcplus(1,5);
            // float adc2=thisDDU->adcplus(1,7);
            unsigned short int status=thisDDU->vmepara_CSCstat();
            unsigned short int DDU_FMM=((thisDDU->vmepara_status()>>8)&0x000F);
            int brdnum,iblink=0;
            // sleep(1);
            thisDDU->CAEN_err_reset();
            brdnum=thisDDU->read_page7();
            sprintf(buf,"Board: %d ",brdnum);
            *out << buf;

            if(DDU_FMM==4){    // Busy
	      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
            }
            else if(DDU_FMM==1){    // Warn, near full: reduce trigger rate
	      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	      iblink=1;
            }
            else if(DDU_FMM==8){    // Ready
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }
            else if(DDU_FMM==2 || DDU_FMM==0xC){     // Error or Sync
	      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	    }
	    else{  // Not Defined
	      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	      iblink=1;
            }
	    if(iblink==1)sprintf(buf,"<blink>DDU FMM: %1X</blink> ",DDU_FMM);
	    else sprintf(buf,"DDU-FMM: %1X ",DDU_FMM);
            *out << buf;
            *out << cgicc::span();
	    iblink=0;

            if(thisDDU->CAEN_err()!=0){
	      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
            }
            else if(status==0x0000){
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }else{
	      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
            }
            sprintf(buf,"Status: %04Xh ",status);
            *out << buf;
            *out << cgicc::span()<< std::endl;
          }else{
	    sprintf(buf,"Crate Broadcast");
	    *out << buf;
          }

          std::string ddufirmware =
	  toolbox::toString("/%s/DDUFirmware",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",ddufirmware)
	    .set("target","_blank") << std::endl;
	  *out << cgicc::input().set("type","submit").set("value","Firmware")<<std::endl;  
          sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu") << std::endl;
	  *out << cgicc::form() << br() << std::endl;
//	  *out << cgicc::form() << std::endl;
          if(slot<=21){
	    std::string ddufpga =
	      toolbox::toString("/%s/DDUFpga",getApplicationDescriptor()->getURN().c_str());
	    *out << " &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp ";
	    *out << cgicc::form().set("method","GET").set("action",ddufpga)
	      .set("target","_blank") << std::endl;
	    *out << cgicc::input().set("type","submit").set("value","DDUFPGA") << std::endl;  
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu") << std::endl;
	    *out << cgicc::form() << std::endl;
	    std::string infpga0 =
	      toolbox::toString("/%s/INFpga0",getApplicationDescriptor()->getURN().c_str());
	    *out << cgicc::form().set("method","GET").set("action",infpga0)
	      .set("target","_blank") << std::endl ;
	    *out << cgicc::input().set("type","submit").set("value","InFPGA0")<< std::endl;  
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu")<< std::endl ;
	    *out << cgicc::form() << std::endl;
	    
	    std::string infpga1 =
	      toolbox::toString("/%s/INFpga1",getApplicationDescriptor()->getURN().c_str());
	    *out << cgicc::form().set("method","GET").set("action",infpga1)
	      .set("target","_blank") << std::endl ;
	    *out << cgicc::input().set("type","submit").set("value","InFPGA1")<< std::endl;  
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu")<< std::endl ;
	    *out << cgicc::form() << std::endl;
	    
	    std::string vmepara =
	      toolbox::toString("/%s/VMEPARA",getApplicationDescriptor()->getURN().c_str());
	    *out << cgicc::form().set("method","GET").set("action",vmepara)
	      .set("target","_blank") << std::endl ;
	    *out << cgicc::input().set("type","submit").set("value","VMEPARA")<< std::endl;  
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu")<< std::endl ;
	    *out << cgicc::form() << std::endl;
	    
	    std::string vmeseri =
	      toolbox::toString("/%s/VMESERI",getApplicationDescriptor()->getURN().c_str());
	    *out << cgicc::form().set("method","GET").set("action",vmeseri)
	      .set("target","_blank");
	    *out << cgicc::input().set("type","submit").set("value","VMESERI")<< std::endl;  
	    sprintf(buf,"%d",i);
	    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu")<< std::endl ;
	    *out << cgicc::form() << std::endl;
          } 
          *out << cgicc::tr() << std::endl; 
          *out << cgicc::table() << cgicc::div() << std::endl;
        }
        *out << cgicc::fieldset() << std::endl;
        *out <<cgicc::br() << std::endl;
	*out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
        *out << endl ;


        printf(" dccVector.size() %d \n",dduVector.size());
	for (int i=0; i<(int)dccVector.size(); i++) {
         *out << cgicc::table().set("border","0").set("rules","none").set("frame","void"); 
          *out << cgicc::tr();
          thisDCC=dccVector[i];
	  int slot = thisDCC->slot();
	  char buf[20];	
          sprintf(buf,"DCC Slot: %d ",slot);
	  printf(" %s \n",buf);      
          *out << buf; 
          if(thisDCC->slot()<21){
            thisDDU->CAEN_err_reset();
            unsigned short int statush=thisDCC->mctrl_stath();
            unsigned short int statusl=thisDCC->mctrl_statl();
            if(thisDCC->CAEN_err()!=0){
	      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");              
            }else if(statush==0x0000&&statusl==0x0000){
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }else{
	      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
            }
            sprintf(buf,"Status H:%04X L:%04X ",statush,statusl);
            *out << buf; 
            *out << cgicc::span()<< std::endl;
          }else{
	    sprintf(buf,"Crate Broadcast");
	    *out << buf; 
          }

          std::string dccfirmware =
	  toolbox::toString("/%s/DCCFirmware",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",dccfirmware)
	    .set("target","_blank") << std::endl;
	  *out << cgicc::input().set("type","submit").set("value","Firmware")<<std::endl;     
         sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl;
	  *out << cgicc::form() << std::endl;
          if(slot<=21){
          std::string dcccommands =
          toolbox::toString("/%s/DCCCommands",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",dcccommands)
		.set("target","_blank") << std::endl;
	  *out << cgicc::input().set("type","submit").set("value","Commands") << std::endl;  
          sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl;
          *out << cgicc::form() << std::endl;
          }
          *out << cgicc::tr() << std::endl; 
          *out << cgicc::table() << cgicc::div() << std::endl;
        }
        *out << cgicc::fieldset() << std::endl;

    }
    *out << cgicc::body() << std::endl; 
    *out << cgicc::html() << std::endl;    
}

  
  //
   //
void EmuFCrateHyperDAQ::setRawConfFile(xgi::Input * in, xgi::Output * out ) 
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
  void EmuFCrateHyperDAQ::Configuring(){
    //
    std::cout << "Configuring " << std::endl ;
    //
    //-- parse XML file
    //
    cout << "---- XML parser ----" << endl;
    cout << " Here parser " << endl;
    FEDCrateParser parser;
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
    dduVector = selector.ddus(crateVector[0]);
    //if (tmbVector.size() > 1){
    //cerr << "Error: only one TMB in xml file allowed" << endl ;
    //exit(1);
    //}
    //
    dccVector = selector.dccs(crateVector[0]);
    //if (dmbVector.size() > 1){
    //cerr << "Error: only one DMB in xml file allowed" << endl;
    //exit(1);
    //} 
    //
    //-- get pointers to CCB, TMB and DMB
    //
     thisCrate = crateVector[0];
 
     //thisTMB = tmbVector[0];
     //thisDMB = dmbVector[0];
     //
     //DDU * thisDDU = thisCrate->ddu();
     //if(thisTMB) alct = thisTMB->alctController();
     //
    std::cout << "Done" << std::endl ;
  }
  //

  void EmuFCrateHyperDAQ::setConfFile(xgi::Input * in, xgi::Output * out ) 
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

  void EmuFCrateHyperDAQ::UploadConfFile(xgi::Input * in, xgi::Output * out ) 
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

  void EmuFCrateHyperDAQ::DDUFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    unsigned long int idcode,uscode;
    unsigned long int tidcode[8]={0x2124a093,0x31266093,0x31266093,0x05036093,0x05036093,0x05036093,0x05036093,0x05036093};
    unsigned long int tuscode[8]={0xcf029a01,0xdf020a01,0xdf020a01,0xb0016a04,0xc029dd99,0xc129dd99,0xd0020a01,0xd1020a01};
// JRG, 5-->3, 6-->4, 7-->5, 3-->6, 4-->7  --done
    //
    printf(" entered DDUFirmware \n");
    cgicc::Cgicc cgi(in);
    printf(" initialize env \n");
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    printf(" getQueryString \n");
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ; 

    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    thisDDU = dduVector[ddu];

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DDU Firmware Form") << std::endl;
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;
    *out << "(Load dual proms in order 1-0) (For hard reset use DCC TTC command)" <<std::endl;
    *out << br() << std::endl;
    *out << br() << std::endl;

    char buf[300];
    int j;
    sprintf(buf,"DDU Firmware Slot %d",thisDDU->slot());
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << cgicc::legend(buf).set("style","color:blue") << std::endl ;

    for(int i=0;i<8;i++){ 
      printf(" LOOP: %d \n",i);
      j=i;
      *out << cgicc::span().set("style","color:black");
      if(thisDDU->slot()>=21){
	sprintf(buf," ");
	//	if(i==0){sprintf(buf,"ddufpga  ");}
	//	if(i==1){sprintf(buf,"infpga0  ");}
	//	if(i==2){sprintf(buf,"infpga1  ");}
	if(i==1)sprintf(buf," <font size=+1> DDU Broadcast Firmware Load</font> ");
	if(i==6){
	  sprintf(buf,"inprom0  ");
	  j=3;
	}
	if(i==7){
	  sprintf(buf,"inprom1  ");
	  j=4;
	}
	if(i==3){
	  sprintf(buf,"vmeprom  ");
	  j=5;
	}
	if(i==4){
	  sprintf(buf,"dduprom0 ");
	  j=6;
	}
	if(i==5){
	  sprintf(buf,"dduprom1 ");
	  j=7;
	}
// JRG, 5-->3, 6-->4, 7-->5, 3-->6, 4-->7  --done

	printf(" %s ",buf);
	*out << buf << cgicc::span() << std::endl;
      }
      if(thisDDU->slot()<21){
	thisDDU->CAEN_err_reset();
	if(i==0){idcode=thisDDU->ddufpga_idcode(); sprintf(buf,"ddufpga  ");}
	if(i==1){idcode=thisDDU->infpga_idcode0(); sprintf(buf,"infpga0  ");}
	if(i==2){idcode=thisDDU->infpga_idcode1(); sprintf(buf,"infpga1  ");}
	if(i==6){idcode=thisDDU->inprom_idcode0(); sprintf(buf,"inprom0  ");}
	if(i==7){idcode=thisDDU->inprom_idcode1(); sprintf(buf,"inprom1  ");}
	if(i==3){idcode=thisDDU->vmeprom_idcode(), sprintf(buf,"vmeprom  ");}
	if(i==4){idcode=thisDDU->dduprom_idcode0();sprintf(buf,"dduprom0 ");}
	if(i==5){idcode=thisDDU->dduprom_idcode1();sprintf(buf,"dduprom1 ");}
	printf(" %s idcode %08lx ",buf,idcode);
	*out<<buf;
	sprintf(buf,"  id: ");
	*out << buf;*out << cgicc::span();
	if(thisDDU->CAEN_err()!=0){ 
	  *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	}else if(idcode!=tidcode[i]){
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	}else{ 
	  *out << cgicc::span().set("style","color:green;background-color:#dddddd");
	}
	sprintf(buf,"%08lX",idcode);
	*out << buf;*out << cgicc::span();
	thisDDU->CAEN_err_reset();
	if(i==0){uscode=thisDDU->ddufpga_usercode();}
	if(i==1){uscode=thisDDU->infpga_usercode0();}
	if(i==2){uscode=thisDDU->infpga_usercode1();}
	if(i==6){uscode=thisDDU->inprom_usercode0();}
	if(i==7){uscode=thisDDU->inprom_usercode1();}
	if(i==3){uscode=thisDDU->vmeprom_usercode();}
	if(i==4){uscode=thisDDU->dduprom_usercode0();}
	if(i==5){uscode=thisDDU->dduprom_usercode1();}
	*out << cgicc::span().set("style","color:black");
	sprintf(buf," usr: ");
	*out << buf;*out << cgicc::span();
	if(i==4||i==5){
	  printf(" uscode %06lx ",(uscode>>8)&0x00ffffff);
	  printf(" BoardID %2ld \n",uscode&0x000000ff);
	}
	else{
	  printf(" uscode %08lx \n",uscode);
	}
	if(thisDDU->CAEN_err()!=0){
	  *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	}else if(i!=4 && i!=5 && uscode!=tuscode[i]){
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	}else if((i==4 || i==5) && (uscode&0xffffff00)!=(tuscode[i]&0xffffff00)){
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	}else{ 
	  *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
	}

	if(i==4||i==5){
	  sprintf(buf,"%06lX",(uscode>>8)&0x00ffffff);
	  *out << buf;*out << cgicc::span();
	  *out << cgicc::span().set("style","color:black");
	  sprintf(buf," BoardID: ");
	  *out << buf;*out << cgicc::span();
	  *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
	  sprintf(buf,"%2ld",uscode&0x000000ff);
	}
	else{
	  sprintf(buf,"%08lX",uscode);
	}
	//    sprintf(buf,"%08lX",uscode);
	*out << buf << cgicc::span() << std::endl;
	if(i==2) *out << br() << std::endl;
      }
      *out << br() << std::endl;
      printf(" now boxes \n");
// JRG, 5-->3, 6-->4, 7-->5, 3-->6, 4-->7  --done
      if(i>=3&&i<8){
	std::string dduloadfirmware =
	  toolbox::toString("/%s/DDULoadFirmware",getApplicationDescriptor()->getURN().c_str());
	//
	*out << cgicc::form().set("method","POST")
	  .set("enctype","multipart/form-data")
	  .set("action",dduloadfirmware) << std::endl;;	
	//.set("enctype","multipart/form-data")
	*out << cgicc::input().set("type","file")
	  .set("name","DDULoadSVF")
	  .set("size","50") << std::endl;

	*out << cgicc::input().set("type","submit").set("value","LoadSVF") << std::endl;   
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu") << std::endl; 
	sprintf(buf,"%d",i);
	//       sprintf(buf,"%d",j);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","prom") << std::endl; 
	*out << cgicc::form() << std::endl ;
      }else{ 
	*out << std::endl; 
      }
    }
    *out << cgicc::fieldset()<< std::endl;
    *out << cgicc::body() << std::endl;
    *out << cgicc::html() << std::endl;    
}

  void EmuFCrateHyperDAQ::DDULoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU " << ddu << endl;
      DDU_ = ddu;
    } 
    int prom;
    cgicc::form_iterator name2 = cgi.getElement("prom");
    //
    if(name2 != cgi.getElements().end()) {
      prom = cgi["prom"]->getIntegerValue();
      cout << "PROM " << prom << endl;
    } 
    //
    thisDDU = dduVector[ddu]; 
    //
	//
	cout << "UploadConfFileUpload" << endl ;
	//

	
	const_file_iterator file;
	file = cgi.getFile("DDULoadSVF");
	//string XMLname = (*file).getFilename();
	// cout <<"SVF FILE: " << XMLname  << endl ;


	//
	cout << "GetFiles" << endl ;
	//

	if(file != cgi.getFiles().end()) {
 	  ofstream TextFile ;
	  TextFile.open("MySVFFile.svf");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	} 
	char buf[400];
	  FILE *dwnfp;
          dwnfp    = fopen("MySVFFile.svf","r");	
          while (fgets(buf,256,dwnfp) != NULL)printf("%s",buf);
          fclose(dwnfp);
          printf(" I am done so prom wont be called %d \n",prom);  
          
          char *cbrdnum;
          cbrdnum=(char*)malloc(5);
          cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
	  if(prom==6)thisDDU->epromload("INPROM0",INPROM0,"MySVFFile.svf",1,cbrdnum);
          if(prom==7)thisDDU->epromload("INPROM1",INPROM1,"MySVFFile.svf",1,cbrdnum);
          if(prom==3)thisDDU->epromload("RESET",RESET,"MySVFFile.svf",1,cbrdnum);
          if(prom==4||prom==5){
            int brdnum=thisDDU->read_page7();
            cbrdnum[0]=brdnum;
          } 
          if(prom==4)thisDDU->epromload("DDUPROM0",DDUPROM0,"MySVFFile.svf",1,cbrdnum);
          if(prom==5)thisDDU->epromload("DDUPROM1",DDUPROM1,"MySVFFile.svf",1,cbrdnum);
          free(cbrdnum);
          in=NULL;
	  this->DDUFirmware(in,out);
          // this->Default(in,out);

 }
 


 void EmuFCrateHyperDAQ::DDUFpga(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
    printf(" enter: DDUFpga \n");
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ;

    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      // cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    //  printf(" DDU %d \n",ddu);
    thisDDU = dduVector[ddu];
    //  printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DDUFPGA Web Form") << std::endl;
    // 
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300],buf2[300] ;
    sprintf(buf,"DDU Control FPGA, VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
    
    for(int i=200;i<235;i++){
     if(i<226||i>232){
      thisDDU->infpga_shift0=0x0000;
      thisDDU->CAEN_err_reset();
      // *out << cgicc::table().set("border","0");
      // *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->ddu_shift0=0x0000;
      if(i==200){
           sprintf(buf,"ddu reset: ");
           sprintf(buf2," EXPERT ONLY! ");
      }
      if(i==201){
	thisDDU->ddu_lowfpgastat();
           sprintf(buf,"16-low-bits  DDU Control FPGA Status:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==202){
	thisDDU->ddu_hifpgastat();
           sprintf(buf,"16-high-bits DDU Control FPGA Status:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==203){
	thisDDU->ddu_fpgastat();
           sprintf(buf,"32-bit DDU Control DFPGA Status:");
           sprintf(buf2," %04X%04X ",thisDDU->ddu_code1,thisDDU->ddu_code0);
      }
      if(i==204){
	thisDDU->ddu_checkFIFOa();
           sprintf(buf,"FIFO-A Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==205){
	thisDDU->ddu_checkFIFOb();
           sprintf(buf,"FIFO-B Status [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==206){
	thisDDU->ddu_checkFIFOc();
           sprintf(buf,"FIFO-C Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==207){
	thisDDU->ddu_rdfferr();
           sprintf(buf,"FIFO Full Register bits [9-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==208){
	thisDDU->ddu_rdkillfiber();
           sprintf(buf,"KillFiber Register bits [19-0]:");
           sprintf(buf2," %01X%02X ",thisDDU->ddu_code1,thisDDU->ddu_code0);
      }

      if(i==209){
	thisDDU->ddu_rdcrcerr();
           sprintf(buf,"CRC Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==210){
	thisDDU->ddu_rdxmiterr();
           sprintf(buf,"Data Transmit Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==211){
	thisDDU->ddu_rddmberr();
           sprintf(buf,"DMB Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==212){
	thisDDU->ddu_rdtmberr();
           sprintf(buf,"TMB Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==213){
	thisDDU->ddu_rdalcterr();
           sprintf(buf,"ALCT Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==214){
	thisDDU->ddu_rdlieerr();
           sprintf(buf,"Lost-In-Event Error Register bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==215){
	thisDDU->ddu_rdscaler();
           sprintf(buf,"DDU L1 Event Scaler, bits [23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->ddu_code1,thisDDU->ddu_code0);
      }
      if(i==216){
	thisDDU->ddu_rderareg();
           sprintf(buf,"Error bus A Register bits [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==217){
	thisDDU->ddu_rderbreg();
           sprintf(buf,"Error bus B Register bits [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==218){
	thisDDU->ddu_rdercreg();
           sprintf(buf,"Error bus C Register bits [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==219){
	thisDDU->ddu_InRDstat();
           sprintf(buf,"InRDctrl Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==220){
	thisDDU->ddu_InC_Hist();
           sprintf(buf,"InRDctrl MxmitErr Reg [15-12] & C-code History [8-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==221){
	thisDDU->ddu_dmblive();
           sprintf(buf,"DMBLIVE bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==222){
	thisDDU->ddu_pdmblive();
           sprintf(buf,"Permanent DMBLIVE bits [14-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==223){
	thisDDU->ddu_rd_WarnMon();
           sprintf(buf,"16-bit DDU FMM-Warn Monitor:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==224){
	thisDDU->ddu_rdostat();
           sprintf(buf,"16-bit DDU Output Status:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==225){
	thisDDU->ddu_rdbxorbit();
           sprintf(buf,"BX_Orbit Register bits [11-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }

/* Expert only registers for special DDU Test Firmware, 226-232:
      if(i==226){
	thisDDU->ddu_shfttst(0);
	unsigned short int tmp;
	tmp=thisDDU->ddu_shift0;
	thisDDU->ddu_shfttst(1);
           sprintf(buf,"DDU Shift Test:");
           sprintf(buf2," %04X %04X",tmp,thisDDU->ddu_shift0);
      }
      if(i==227){
	thisDDU->ddu_rd_verr_cnt();
           sprintf(buf,"DDR Vote Error Count =");
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
      if(i==228){
	thisDDU->ddu_fifo0verr_cnt();
           sprintf(buf,"DDR FIFO-0 Error Count ="); 
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
      if(i==229){
	thisDDU->ddu_fifo1verr_cnt();
           sprintf(buf,"DDR FIFO-1 Error Count ="); 
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
      if(i==230){
	thisDDU->ddu_earlyVerr_cnt();
           sprintf(buf,"DDR Early 200ns  Error Count ="); 
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
      if(i==231){
	thisDDU->ddu_verr23cnt();
           sprintf(buf,"DDR VoteError23 Count ="); 
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
      if(i==232){
	thisDDU->ddu_verr55cnt();
           sprintf(buf,"DDR VoteError55 Count ="); 
           sprintf(buf2," %d 0x%04X ",thisDDU->ddu_code0,thisDDU->ddu_code0);
      }
*/

      if(i==233){
	thisDDU->ddu_rd_boardID();
           sprintf(buf,"16-bit DDU Board ID:"); 
           sprintf(buf2," 0x%04X ",thisDDU->ddu_code0);
      }
      if(i==234){
           sprintf(buf,"DDUFPGA VME L1A:"); 
           sprintf(buf2," EXPERT ONLY! ");
      } 

// JRGhere
      if(i==200||i==208||i==225||i==234){
	std::string ddutextload =
	  toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",ddutextload) << std::endl;
      }
      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
       *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else if((thisDDU->ddu_shift0!=0xFACE)&&(i!=226)){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2;*out << cgicc::span();
      if((thisDDU->ddu_shift0!=0xFACE)&&i!=226&&i!=200&&i!=234){
        sprintf(buf," shift:%04X",thisDDU->ddu_shift0);
        *out << buf;
      }
      if(i==200||i==208||i==225||i==234){  //JRG, def. SET button properties
	string xmltext="";
	if(i==208)xmltext="f7fff";
	if(i==225)xmltext="fff";
	if(i==200||i==234){
	  *out << cgicc::input().set("type","hidden")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value",xmltext)
	   .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
	}
	else {
	  *out << cgicc::input().set("type","text")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value",xmltext)
	   .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
	}
	*out << cgicc::input().set("type","submit")
	  .set("value","set");
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
	sprintf(buf,"%d",i);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");
	// *out << cgicc::form() << br() << std::endl ;
	*out << cgicc::form() << std::endl ;
	// *out << cgicc::td() << std::endl;
      }else{
	*out << br() << std::endl;
	// *out << cgicc::td() << std::endl ;
      }
      // *out << cgicc::table() << std::endl;
     }
    }

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    
    *out << cgicc::html() << std::endl;    
}



void EmuFCrateHyperDAQ::INFpga0(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     try
       {
    printf(" enter: INFpga0 \n");
    cgicc::Cgicc cgi(in);
    const CgiEnvironment& env = cgi.getEnvironment();
    std::string crateStr = env.getQueryString() ;
    cout << crateStr << endl ;
    cgicc::form_iterator name = cgi.getElement("ddu");
    int ddu,icrit,icond,icond2;
    unsigned long int stat;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    printf(" DDU %d \n",ddu);
    thisDDU = dduVector[ddu];
    printf(" set up web page \n");
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("INFPGA0 Web Form") << std::endl;
    char buf[300],buf2[300],buf3[300],buf4[300];
    sprintf(buf,"DDU INFPGA0, VME  Slot %d",thisDDU->slot());
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;

    for(int i=300;i<321;i++){
      printf(" LOOP: %d \n",i);
      thisDDU->infpga_shift0=0x0000; 
      thisDDU->CAEN_err_reset();
      sprintf(buf3," ");
      sprintf(buf4," ");
      icond=0;
      icond2=0;
      if(i==300){
	thisDDU->infpgastat(INFPGA0);
	stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
	sprintf(buf,"infpga0 32-bit Status:");
	sprintf(buf2," %08lX ",stat);
	icrit=0;
	if((0x00004000&stat)>0)icond=1;
	if((0x00008000&stat)>0){
	  icrit=1;
	  icond=2;
	}
	else if((0xf0004202&stat)>0)icond=1;
      }
      if(i==301){
	thisDDU->infpga_rdscaler(INFPGA0);
           sprintf(buf,"infpga0 L1 Event Scaler0 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==302){
	thisDDU->infpga_rd1scaler(INFPGA0);
           sprintf(buf,"infpga0 L1 Event Scaler1 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==303){
	thisDDU->infpga_DMBwarn(INFPGA0);
//       	*out << br() << " <font color=blue> All 8-bit Fiber Flags below show if the condition has occurred since last Reset</font>" << br() << std::endl;
       	*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset (historical)</font>" << br() << std::endl;
	sprintf(buf,"infpga0 DMB Full, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DMB Warn, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	//	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==304){
	thisDDU->infpga_CheckFiber(INFPGA0);
	sprintf(buf,"infpga0 Connection Error, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp Link Active, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
      }
      if(i==305){
	thisDDU->infpga_DMBsync(INFPGA0);
	sprintf(buf,"infpga0 Stuck Data, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp L1A Mismatch, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
      }
      if(i==306){
	thisDDU->infpga_RxErr(INFPGA0);
	sprintf(buf,"infpga0 DDU Rx Error, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp DDU Timeout-start, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==307){
	thisDDU->infpga_Timeout(INFPGA0);
	sprintf(buf,"infpga0 DDU Timeout-EndBusy, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp DDU Timeout-EndWait, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      } 
      if(i==308){
	thisDDU->infpga_XmitErr(INFPGA0);
	sprintf(buf,"infpga0 SCA Full history, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp CSC Xmit Error, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==309){
	thisDDU->infpga_LostErr(INFPGA0);
	sprintf(buf,"infpga0 DDU LostInEvent Error, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DDU LostInData Error, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==310){
	thisDDU->infpga_FIFOstatus(INFPGA0);
       	*out << br() << " <font color=blue> These registers need individual bit Decoding</font>" << br() << std::endl;
	sprintf(buf,"infpga0 Input Buffer Empty, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	sprintf(buf3," &nbsp &nbsp special decode (8-bit):");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	stat=thisDDU->infpga_code0;
// JRG, 410: bits 15-8 need decode in Blockquote below, black.
	//	sprintf(buf,"infpga0 DDU Input FIFO Status [15-0]:");
	//	sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==311){
	thisDDU->infpga_FIFOfull(INFPGA0);
	sprintf(buf,"infpga0 Input Buffer Full history, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond=2;
	sprintf(buf3," &nbsp &nbsp special decode (4-bit):");
	sprintf(buf4," %01X ",(thisDDU->infpga_code0&0x0f00)>>8);
	if(thisDDU->infpga_code0&0x0f00)icond2=2;
	stat=thisDDU->infpga_code0;
//	if(i==311&&(stat&0x0f00)>0){
// JRG, 411: bits 11-8 need decode in Blockquote below, red.
//	sprintf(buf,"infpga0 DDU Input FIFO Full[11-0]:");
//	sprintf(buf2," %03X ",thisDDU->infpga_code0);
	if(thisDDU->infpga_code0&0x0fff)icond=2;
      }
      if(i==312){
	thisDDU->infpga_CcodeStat(INFPGA0);
	sprintf(buf,"infpga0 InRD0 C-code status:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x0020)icond=1;
	if(thisDDU->infpga_code0&0x00df)icond=2;
	sprintf(buf3," &nbsp &nbsp InRD1 C-code status:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0x2000)icond2=1;
	if(thisDDU->infpga_code0&0xdf00)icond2=2;
	stat=thisDDU->infpga_code0;
      }
      if(i==313){
	thisDDU->infpga_MemAvail(INFPGA0);
       	*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << std::endl;
	sprintf(buf,"infpga0 FIFO Memory Available:");
	sprintf(buf2,"MemCtrl-2 = %2d free, &nbsp MemCtrl-3 = %2d free",thisDDU->infpga_code0&0x001f,thisDDU->infpga_code1);
      }
      if(i==314){
	thisDDU->infpga_Min_Mem(INFPGA0);
	sprintf(buf,"infpga0 Minimum FIFO Memory Availabile:");
	sprintf(buf2,"MemCtrl-2 min = %d free, &nbsp MemCtrl-3 min = %d free",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==315){
	thisDDU->infpga_WrMemActive(INFPGA0,0);
	sprintf(buf,"infpga0 Write Memory Active 0-1:");
	sprintf(buf2," Fiber 0 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 0 Unused, &nbsp ");
	sprintf(buf4," Fiber 1 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 1 Unused");
      }
      if(i==316){
	thisDDU->infpga_WrMemActive(INFPGA0,1);
	sprintf(buf,"infpga0 Write Memory Active 2-3:");
	sprintf(buf2," Fiber 2 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 2 Unused, &nbsp ");
	sprintf(buf4," Fiber 3 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 3 Unused");
      }
      if(i==317){
	thisDDU->infpga_WrMemActive(INFPGA0,2);
	sprintf(buf,"infpga0 Write Memory Active 4-5:");
	sprintf(buf2," Fiber 4 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 4 Unused, &nbsp ");
	sprintf(buf4," Fiber 5 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 5 Unused");
      }
      if(i==318){
	thisDDU->infpga_WrMemActive(INFPGA0,3);
	sprintf(buf,"infpga0 Write Memory Active 6-7:");
	sprintf(buf2," Fiber 6 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 6 Unused, &nbsp ");
	sprintf(buf4," Fiber 7 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 7 Unused");
      }
      if(i==319){
	if(icrit>0){
	  thisDDU->infpga_trap(INFPGA0);
	  *out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << std::endl;
	  sprintf(buf,"infpga0 diagnostic trap:");
	  sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->infpga_lcode[5],thisDDU->infpga_lcode[4],thisDDU->infpga_lcode[3],thisDDU->infpga_lcode[2],thisDDU->infpga_lcode[1],thisDDU->infpga_lcode[0]);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->infpga_shift0=0xFACE;
	}
      }
      if(i==320){
	sprintf(buf,"infpga0 reset: ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
      }

      // JRG, form Needed for 420?  *out << cgicc::form() << std::endl;
      if(i==320){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",ddutextload) << std::endl;
      }

      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(icond==1){
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2 << cgicc::span();
      *out << cgicc::span().set("style","color:black");
      *out << buf3 << cgicc::span();

      if(icond2==1){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else if(icond2==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf4 << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	*out << " **CAEN Error " << cgicc::span();
      }
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=320){
        sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
        *out << buf << cgicc::span();
      }

      if(i==320){
	  *out << cgicc::input().set("type","hidden")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value","")
	   .set("style","font-size: 13pt; font-family: arial;")<<std::endl;

	*out << cgicc::input().set("type","submit")
	  .set("value","set");
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
	sprintf(buf,"%d",i);       
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
	*out << cgicc::form() << std::endl ;
      }else if(i==300&&(stat&0x0000f000)>0){
        *out << "<blockquote><font size=-1 color=red face=arial>";
	if((stat&0xF0000000)>0){
	  if((0x80000000&stat)>0) *out << " DDL-2 Not Locked &nbsp ";
	  if((0x40000000&stat)>0) *out << " DDL-1 Not Locked &nbsp ";
	  if((0x20000000&stat)>0) *out << " RdCtrl-1 Not Ready &nbsp ";
	  if((0x10000000&stat)>0) *out << " RdCtrl-0 Not Ready";
	  *out << br();
	}
	if((stat&0x0F000000)>0){
	  if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber Warn</font> &nbsp ";
	  if((0x04000000&stat)>0) *out << " <font color=blue>DLL Error occurred</font> &nbsp ";
	  if((0x02000000&stat)>0) *out << " <font color=black>InRD1 DMB Warn</font> &nbsp ";
	  if((0x01000000&stat)>0) *out << " <font color=black>InRD0 DMB Warn</font>";
	  *out << br();
	}
	if((stat&0x00F00000)>0){
	  if((0x00800000&stat)>0) *out << " <font color=black>InRD1 DMB Full</font> &nbsp ";
	  if((0x00400000&stat)>0) *out << " Mem/FIFO-InRD1 Error &nbsp ";
	  if((0x00200000&stat)>0) *out << " MultL1A Error-InRD1 &nbsp ";
	  if((0x00100000&stat)>0) *out << " <font color=black>NoLiveFiber4-7</font>";
	  *out << br();
	}
	if((stat&0x000F0000)>0){
	  if((0x00080000&stat)>0) *out << " <font color=blue>InRD0 DMB Full</font> &nbsp ";
	  if((0x00040000&stat)>0) *out << " Mem/FIFO-InRD0 Error &nbsp ";
	  if((0x00020000&stat)>0) *out << " MultL1A Error-InRD0 &nbsp ";
	  if((0x00010000&stat)>0) *out << " <font color=black>NoLiveFiber0-3</font>";
	  *out << br();
	}
// JRG, low-order 16-bit status (most serious errors):
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
	  if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=blue>Single Warning</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=black>Near Full Warning</font>";
	  *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>RX Error</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=blue>Special Word Error</font>";
	  *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " Stuck Data Error occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " Timeout Error &nbsp ";
	  if((0x00000020&stat)>0) *out << " Critical Data Error &nbsp ";
	  if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
	  *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
	  if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
	  if((0x00000002&stat)>0) *out << " <font color=orange>L1A Mismatch Error</font> &nbsp ";
	  if((0x00000001&stat)>0) *out << " Not Ready Error";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==319 && icrit>0){
	*out << "<blockquote><font size=-1 color=black face=arial>Trap decode goes here" << "</font></blockquote>";
	//	infpga_trap_decode();
      }else if(i==310&&(stat&0xff00)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00005500)>0){
	  *out << " &nbsp InRD0 Status: &nbsp <font color=orange>";
	  if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
	if((stat&0x0000AA00)>0){
	  *out << " &nbsp InRD1 Status: &nbsp <font color=orange>";
	  if((0x00008000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00002000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000800&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000200&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000200&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==311&&(stat&0x0f00)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00000500)>0){
	  *out << " &nbsp InRD0 Status: &nbsp <font color=red>";
	  if((0x00000400&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Full Occurred";
	  *out << "</font>" << br();
	}
	if((stat&0x00000A00)>0){
	  *out << " &nbsp InRD1 Status: &nbsp <font color=red>";
	  if((0x00000800&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " L1A FIFO Full Occurred";
	  *out << "</font>" << br();
	}
        *out << "</font></blockquote>";
      }else if(i==312&&(stat&0xffff)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x000000ff)>0){
	  *out << " &nbsp InRD0: &nbsp <font color=red>";
	  if((0x000080&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
	  if((0x000040&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
	  if((0x000020&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
	  if((0x000010&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
	  if((0x000008&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
	  if((0x000004&stat)>0) *out << " Multi-Xmit Error &nbsp &nbsp ";
	  if((0x000002&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x000001&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	if((stat&0x0000ff00)>0){
	  *out << " &nbsp InRD1 &nbsp <font color=red>";
	  if((0x0008000&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
	  if((0x00004000&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
	  if((0x00000800&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " Multi-Xmit Error &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	*out  << "</font></blockquote>" << br();
      }else{
	*out << br() << std::endl;
      }
    }

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;
    *out << cgicc::html() << std::endl;

      }
     catch (const std::exception & e )
       {
	 //XECPT_RAISE(xgi::exception::Exception, e.what());
       }
  }



void EmuFCrateHyperDAQ::INFpga1(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     try
      {
    printf(" enter: INFpga1 \n");
    cgicc::Cgicc cgi(in);
    const CgiEnvironment& env = cgi.getEnvironment();
    std::string crateStr = env.getQueryString() ;
    cout << crateStr << endl ;
    cgicc::form_iterator name = cgi.getElement("ddu");
    int ddu,icrit,icond,icond2;
    unsigned long int stat;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    printf(" DDU %d \n",ddu);
    thisDDU = dduVector[ddu];
    printf(" set up web page \n");
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("INFPGA1 Web Form") << std::endl;
     *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    char buf[300],buf2[300],buf3[300],buf4[300];
    sprintf(buf,"DDU INFPGA1, VME  Slot %d",thisDDU->slot());
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    
    for(int i=400;i<421;i++){
      printf(" LOOP: %d \n",i);
      thisDDU->infpga_shift0=0x0000; 
      thisDDU->CAEN_err_reset();
      sprintf(buf3," ");
      sprintf(buf4," ");
      icond=0;
      icond2=0;
      if(i==400){
	thisDDU->infpgastat(INFPGA1);
	stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
	sprintf(buf,"infpga1 32-bit Status:");
	sprintf(buf2," %08lX ",stat);
	icrit=0;
	if((0x00004000&stat)>0)icond=1;
	if((0x00008000&stat)>0){
	  icrit=1;
	  icond=2;
	}
	else if((0xf0004202&stat)>0)icond=1;
      }
      if(i==401){
	thisDDU->infpga_rdscaler(INFPGA1);
           sprintf(buf,"infpga1 L1 Event Scaler0 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==402){
	thisDDU->infpga_rd1scaler(INFPGA1);
           sprintf(buf,"infpga1 L1 Event Scaler1 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==403){
	thisDDU->infpga_DMBwarn(INFPGA1);
//       	*out << br() << " <font color=blue> All 8-bit Fiber Flags below show if the condition has occurred since last Reset</font>" << br() << std::endl;
       	*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset (historical)</font>" << br() << std::endl;
	sprintf(buf,"infpga1 DMB Full, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DMB Warn, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	//	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==404){
	thisDDU->infpga_CheckFiber(INFPGA1);
	sprintf(buf,"infpga1 Connection Error, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp Link Active, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
      }
      if(i==405){
	thisDDU->infpga_DMBsync(INFPGA1);
	sprintf(buf,"infpga1 Stuck Data, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp L1A Mismatch, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
      }
      if(i==406){
	thisDDU->infpga_RxErr(INFPGA1);
	sprintf(buf,"infpga1 DDU Rx Error, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp DDU Timeout-start, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==407){
	thisDDU->infpga_Timeout(INFPGA1);
	sprintf(buf,"infpga1 DDU Timeout-EndBusy, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp DDU Timeout-EndWait, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      } 
      if(i==408){
	thisDDU->infpga_XmitErr(INFPGA1);
	sprintf(buf,"infpga1 SCA Full history, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp CSC Xmit Error, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==409){
	thisDDU->infpga_LostErr(INFPGA1);
	sprintf(buf,"infpga1 DDU LostInEvent Error, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DDU LostInData Error, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==410){
	thisDDU->infpga_FIFOstatus(INFPGA1);
       	*out << br() << " <font color=blue> These registers need individual bit Decoding</font>" << br() << std::endl;
	sprintf(buf,"infpga1 Input Buffer Empty, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	sprintf(buf3," &nbsp &nbsp special decode (8-bit):");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	stat=thisDDU->infpga_code0;
// JRG, 410: bits 15-8 need decode in Blockquote below, black.
	//	sprintf(buf,"infpga1 DDU Input FIFO Status [15-0]:");
	//	sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==411){
	thisDDU->infpga_FIFOfull(INFPGA1);
	sprintf(buf,"infpga1 Input Buffer Full history, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond=2;
	sprintf(buf3," &nbsp &nbsp special decode (4-bit):");
	sprintf(buf4," %01X ",(thisDDU->infpga_code0&0x0f00)>>8);
	if(thisDDU->infpga_code0&0x0f00)icond2=2;
	stat=thisDDU->infpga_code0;
//	if(i==411&&(stat&0x0f00)>0){
// JRG, 411: bits 11-8 need decode in Blockquote below, red.
//	sprintf(buf,"infpga1 DDU Input FIFO Full[11-0]:");
//	sprintf(buf2," %03X ",thisDDU->infpga_code0);
	if(thisDDU->infpga_code0&0x0fff)icond=2;
      }
      if(i==412){
	thisDDU->infpga_CcodeStat(INFPGA1);
	sprintf(buf,"infpga1 InRD2 C-code status:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x0020)icond=1;
	if(thisDDU->infpga_code0&0x00df)icond=2;
	sprintf(buf3," &nbsp &nbsp InRD3 C-code status:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0x2000)icond2=1;
	if(thisDDU->infpga_code0&0xdf00)icond2=2;
	stat=thisDDU->infpga_code0;
      }
/* JRG, Not Useful
      if(i==413){
	thisDDU->infpga_StatA(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg A [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==414){
	thisDDU->infpga_StatB(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg B [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==415){
	thisDDU->infpga_StatC(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg C [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
*/
      if(i==413){
	thisDDU->infpga_MemAvail(INFPGA1);
       	*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << std::endl;
	sprintf(buf,"infpga1 FIFO Memory Available:");
	sprintf(buf2,"MemCtrl-2 = %2d free, &nbsp MemCtrl-3 = %2d free",thisDDU->infpga_code0&0x001f,thisDDU->infpga_code1);
      }
      if(i==414){
	thisDDU->infpga_Min_Mem(INFPGA1);
	sprintf(buf,"infpga1 Minimum FIFO Memory Availabile:");
	sprintf(buf2,"MemCtrl-2 min = %d free, &nbsp MemCtrl-3 min = %d free",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==415){
	thisDDU->infpga_WrMemActive(INFPGA1,0);
	sprintf(buf,"infpga1 Write Memory Active 8-9: ");
	sprintf(buf2," Fiber 8 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 8 Unused, &nbsp ");
	sprintf(buf4," Fiber 9 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 9 Unused");
	//	sprintf(buf2," Fiber 8 writing to %02Xh,  Fiber 9 writing to %02Xh",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==416){
	thisDDU->infpga_WrMemActive(INFPGA1,1);
	sprintf(buf,"infpga1 Write Memory Active 10-11: ");
	sprintf(buf2," Fiber 10 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 10 Unused, &nbsp ");
	sprintf(buf4," Fiber 11 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 11 Unused");
	//	sprintf(buf2," Fiber 10 writing to %02Xh,  Fiber 11 writing to %02Xh",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==417){
	thisDDU->infpga_WrMemActive(INFPGA1,2);
	sprintf(buf,"infpga1 Write Memory Active 12-13: ");
	sprintf(buf2," Fiber 12 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 12 Unused, &nbsp ");
	sprintf(buf4," Fiber 13 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 13 Unused");
	//	sprintf(buf2," Fiber 12 writing to %02Xh,  Fiber 13 writing to %02Xh",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==418){
	thisDDU->infpga_WrMemActive(INFPGA1,3);
	sprintf(buf,"infpga1 Write Memory Active 14: ");
	sprintf(buf2," Fiber 14 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf2," Fiber 14 Unused");
      }
      if(i==419){
	if(icrit>0){
	  thisDDU->infpga_trap(INFPGA1);
	  *out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << std::endl;
	  sprintf(buf,"infpga1 diagnostic trap:");
	  sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->infpga_lcode[5],thisDDU->infpga_lcode[4],thisDDU->infpga_lcode[3],thisDDU->infpga_lcode[2],thisDDU->infpga_lcode[1],thisDDU->infpga_lcode[0]);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->infpga_shift0=0xFACE;
	}
      }
      if(i==420){
	sprintf(buf,"infpga1 reset: ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
      }
      // JRG, form Needed for 420?  *out << cgicc::form() << std::endl;
      if(i==420){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",ddutextload) << std::endl;
      }

      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(icond==1){
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2 << cgicc::span();
      *out << cgicc::span().set("style","color:black");
      *out << buf3 << cgicc::span();

      if(icond2==1){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else if(icond2==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf4 << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	*out << " **CAEN Error " << cgicc::span();
      }
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=420){
        sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
        *out << buf << cgicc::span();
      }

      if(i==420){
	//	std::string ddutextload =
	//	  toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
	//	*out << cgicc::form().set("method","GET").set("action",ddutextload) << std::endl;
	// JRG, Not Needed here?  *out << cgicc::form() << std::endl;
	  *out << cgicc::input().set("type","hidden")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value","")
	   .set("style","font-size: 13pt; font-family: arial;")<<std::endl;

	*out << cgicc::input().set("type","submit")
	  .set("value","set");
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
	sprintf(buf,"%d",i);       
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
	*out << cgicc::form() << std::endl ;
      }else if(i==400&&(stat&0x0000f000)>0){
	//	*out << br() << std::endl;
        *out << "<blockquote><font size=-1 color=red face=arial>";
	if((stat&0xF0000000)>0){
	  if((0x80000000&stat)>0) *out << " DDL-2 Not Locked &nbsp ";
	  if((0x40000000&stat)>0) *out << " DDL-1 Not Locked &nbsp ";
	  if((0x20000000&stat)>0) *out << " RdCtrl-3 Not Ready &nbsp ";
	  if((0x10000000&stat)>0) *out << " RdCtrl-2 Not Ready";
	  *out << br();
	}
	if((stat&0x0F000000)>0){
	  if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber Warn</font> &nbsp ";
	  if((0x04000000&stat)>0) *out << " <font color=blue>DLL Error occurred</font> &nbsp ";
	  if((0x02000000&stat)>0) *out << " <font color=black>InRD3 DMB Warn</font> &nbsp ";
	  if((0x01000000&stat)>0) *out << " <font color=black>InRD2 DMB Warn</font>";
	  *out << br();
	}
	if((stat&0x00F00000)>0){
	  if((0x00800000&stat)>0) *out << " <font color=black>InRD3 DMB Full</font> &nbsp ";
	  if((0x00400000&stat)>0) *out << " Mem/FIFO-InRD3 Error &nbsp ";
	  if((0x00200000&stat)>0) *out << " MultL1A Error-InRD3 &nbsp ";
	  if((0x00100000&stat)>0) *out << " <font color=black>NoLiveFiber12-14</font>";
	  *out << br();
	}
	if((stat&0x000F0000)>0){
	  if((0x00080000&stat)>0) *out << " <font color=blue>InRD2 DMB Full</font> &nbsp ";
	  if((0x00040000&stat)>0) *out << " Mem/FIFO-InRD2 Error &nbsp ";
	  if((0x00020000&stat)>0) *out << " MultL1A Error-InRD2 &nbsp ";
	  if((0x00010000&stat)>0) *out << " <font color=black>NoLiveFiber8-11</font>";
	  *out << br();
	}
// JRG, low-order 16-bit status (most serious errors):
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
	  if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=blue>Single Warning</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=black>Near Full Warning</font>";
	  *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>RX Error</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=blue>Special Word Error</font>";
	  *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " Stuck Data Error occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " Timeout Error &nbsp ";
	  if((0x00000020&stat)>0) *out << " Critical Data Error &nbsp ";
	  if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
	  *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
	  if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
	  if((0x00000002&stat)>0) *out << " <font color=orange>L1A Mismatch Error</font> &nbsp ";
	  if((0x00000001&stat)>0) *out << " Not Ready Error";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==419 && icrit>0){
	*out << "<blockquote><font size=-1 color=black face=arial>Trap decode goes here" << "</font></blockquote>";
	//	infpga_trap_decode();
      }else if(i==410&&(stat&0xff00)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00005500)>0){
	  *out << " &nbsp InRD2 Status: &nbsp <font color=orange>";
	  if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
	if((stat&0x0000AA00)>0){
	  *out << " &nbsp InRD3 Status: &nbsp <font color=orange>";
	  if((0x00008000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00002000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000800&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000200&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000200&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==411&&(stat&0x0f00)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00000500)>0){
	  *out << " &nbsp InRD2 Status: &nbsp <font color=red>";
	  if((0x00000400&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Full Occurred";
	  *out << "</font>" << br();
	}
	if((stat&0x00000A00)>0){
	  *out << " &nbsp InRD3 Status: &nbsp <font color=red>";
	  if((0x00000800&stat)>0) *out << " Ext.FIFO Full Occurred &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " L1A FIFO Full Occurred";
	  *out << "</font>" << br();
	}
        *out << "</font></blockquote>";
      }else if(i==412&&(stat&0xffff)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x000000ff)>0){
	  *out << " &nbsp InRD2: &nbsp <font color=red>";
	  if((0x000080&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
	  if((0x000040&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
	  if((0x000020&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
	  if((0x000010&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
	  if((0x000008&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
	  if((0x000004&stat)>0) *out << " Multi-Xmit Error &nbsp &nbsp ";
	  if((0x000002&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x000001&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	if((stat&0x0000ff00)>0){
	  *out << " &nbsp InRD3 &nbsp <font color=red>";
	  if((0x0008000&stat)>0) *out << " Critical Error ** needs reset ** &nbsp &nbsp ";
	  if((0x00004000&stat)>0) *out << " Sync Error ** needs reset ** &nbsp &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=orange>Single Error</font> &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " FIFO Overflow detected &nbsp &nbsp ";
	  if((0x00000800&stat)>0) *out << " Fiber Connection Error &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " Multi-Xmit Error &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	*out  << "</font></blockquote>" << br();
      }else{
	*out << br() << std::endl;
      }
    }
    *out << cgicc::fieldset() << std::endl; 
    *out << cgicc::body() << std::endl;    
    *out << cgicc::html() << std::endl;

      }
     catch (const std::exception & e )
       {
	 //XECPT_RAISE(xgi::exception::Exception, e.what());
       }
}


void EmuFCrateHyperDAQ::VMEPARA(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    printf(" enter: VMEPARA \n");
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ;

    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu,stat;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    printf(" DDU %d \n",ddu);
    thisDDU = dduVector[ddu];
    printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("VMEPARA Web Form") << std::endl;
    //
     *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    char buf[300],buf2[300] ;
    int iblink=0;
    unsigned short int DDU_FMM=0;
    sprintf(buf,"DDU PARALLEL VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
   
    for(int i=500;i<523;i++){
      printf(" LOOP: %d \n",i);
      thisDDU->CAEN_err_reset();  
      if(i==500){
           sprintf(buf,"VMEctrl Status Register:");
	   stat=thisDDU->vmepara_status();
	   DDU_FMM=(stat>>8)&0x000F;
           sprintf(buf2," %04X ",stat);
      }
      if(i==502){
	sprintf(buf,"CSC Busy:");
	sprintf(buf2," %04X ",thisDDU->vmepara_busy());
      }
      if(i==503){
           sprintf(buf,"CSC Warn/Near Full:");
           sprintf(buf2," %04X ",thisDDU->vmepara_fullwarn());
      }
      if(i==504){
           sprintf(buf,"CSC LostSync:");
           sprintf(buf2," %04X ",thisDDU->vmepara_lostsync());
      }
      if(i==505){
           sprintf(buf,"CSC Error:");
           sprintf(buf2," %04X ",thisDDU->vmepara_error());
      }
      if(i==501){
       	*out << " <font color=blue> Items below have 1+15 bits to report FMM status: 1 flag for DDU plus 15 for its CSC inputs</font>" << br() << std::endl;
           sprintf(buf,"CSC FMM Problem Report:");
           sprintf(buf2," %04X ",thisDDU->vmepara_CSCstat());
      }
// JRG, 506 D.N.E.
// 507->08, 508._09, 509->10, 510->11, 511->16, 512->SAME, 513->14,
// 514->17, 515->18, 516->19, 517->20, 518->21, 519->22, 520->15,
// 521->13, 522->06, 523->07
//  then 508->09, 509->10, 510->11, 511->08, 515->13, 513->15
      if(i==509){
           sprintf(buf,"InReg0:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg0());
      }
      if(i==510){
           sprintf(buf,"InReg1:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg1());
      }
      if(i==511){
           sprintf(buf,"InReg2:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg2());
      }
      if(i==508){
	*out << br() << " <font color=blue> 3 16-bit InRegisters, pipelined 0 ->> 1 ->> 2; use this to pre-load VMEserial writes</font>" << br() << std::endl;
	sprintf(buf,"Write to InReg0:");
	sprintf(buf2," ");
      }
      if(i==516){
           sprintf(buf,"Switches:");
           sprintf(buf2," %02X ",thisDDU->vmepara_switch()&0xff);
      }
      if(i==512){
	*out << br() << " <font color=blue> Set SPY Rate (bits 2:0); set Ignore DCC Wait (bit 3) <br>  Rate 0-7 will transmit 1 event out ot 1,8,32,128,1024,8192,32768,never</font>" << br() << std::endl;
	//	*out << br() << " <font color=blue> Select 0-7 for SPY rate = 1 per 1,8,32,128,1024,8192,32768,never</font>" << br() << std::endl;
	sprintf(buf,"GbE Prescale:");
	sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_GbEprescale());
      }
      if(i==514){
	*out << " <font color=blue> Enable Fake L1A/Data Passthrough for DDU FPGAs: bit 0=InFPGA0, 1=InFPGA1, 2=DDUctrl</font>" << br() << std::endl;
	sprintf(buf,"Fake L1 Reg:");
	sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_fakel1reg());
      }
      if(i==517){
	*out << hr() << " <font color=blue> Items below are Only for Board Testing, Do Not Use!</font>" << br() << std::endl;
	//	*out << br() << " <font color=blue> Only for 4-bit FMM function testing, Do Not Use!</font>" << br() << std::endl;
	sprintf(buf,"4-bit FMM Reg:");
	sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_fmmreg());
      }
      if(i==518){
           sprintf(buf,"Test Reg0:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==519){
           sprintf(buf,"Test Reg1:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==520){
           sprintf(buf,"Test Reg2:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
// 507->08, 508._09, 509->10, 510->11, 511->16, 512->SAME, 513->14,
// 514->17, 515->18, 516->19, 517->20, 518->21, 519->22, 520->15,
// 521->13, 522->06, 523->07
//  then 508->09, 509->10, 510->11, 511->08, 515->013, 513->15
      if(i==521){
           sprintf(buf,"Test Reg3:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==522){
           sprintf(buf,"Test Reg4:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==513){
           sprintf(buf,"Toggle DCC_wait:");
           sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
      }
      if(i==515){
           sprintf(buf,"Toggle All FakeL1A:");
           sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
      }
      if(i==506){
           sprintf(buf,"CSC Warn History:");
           sprintf(buf2," %04X ",thisDDU->vmepara_warnhist());
      }
      if(i==507){
           sprintf(buf,"CSC Busy History:");
           sprintf(buf2," %04X ",thisDDU->vmepara_busyhist());
      }
      if(i==508||i==512||i==514||i==517||i==515||i==513){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",ddutextload);
      }
      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }
      else{
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
// 507->08, 508._09, 509->10, 510->11, 511->16, 512->SAME, 513->14,
// 514->17, 515->18, 516->19, 517->20, 518->21, 519->22, 520->15,
// 521->13, 522->06, 523->07
//  then 508->09, 509->10, 510->11, 511->08, 515->013, 513->15
      *out << buf2;
      *out << cgicc::span();
      if(i==500){
	*out << "<font color=black> &nbsp &nbsp " << "Current DDU FMM Report: </font>";
	sprintf(buf2," READY ");
	if(DDU_FMM==4){   // Busy
	  *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	  sprintf(buf2," BUSY ");
	}
	else if(DDU_FMM==1){    // Warn, near full: reduce trigger rate
	  *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	  iblink=1;
	  sprintf(buf2," WARNING, Near Full ");
	}
	else if(DDU_FMM==8) *out << cgicc::span().set("style","color:green;background-color:#dddddd;");  // Ready
	else if(DDU_FMM==2){    // Sync Lost
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	  sprintf(buf2," Lost Sync ");
	}
	else if(DDU_FMM==0xC){    // Error
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	  sprintf(buf2," ERROR ");
	}
	else{  // Not Defined
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	  sprintf(buf2," UNDEFINED ");
	  iblink=1;
	}
	if(iblink==1)sprintf(buf,"<%1X,<blink> ",DDU_FMM);
	else sprintf(buf,"%1X, ",((stat>>8)&0x000F));
	*out << buf << buf2;
	*out << cgicc::span();

	sprintf(buf2,"<blockquote><font size=-1 color=black face=arial> slot=%d. &nbsp &nbsp ",((~stat)&0x003F));
	*out << buf2;
	if(stat&0x0080) *out << " VME DLL-2 Not Locked. &nbsp ";
	if(stat&0x0040) *out << " VME DLL-1 Not Locked. &nbsp ";
	if((stat&0x00C0)&&(stat&0xF000)) *out << br();
	if(stat&0x8000) *out << " VME FPGA detects a problem &nbsp ";
	if(stat&0x4000) *out << " &nbsp VME FPGA has a clock problem &nbsp ";
	if(stat&0x2000) *out << " &nbsp VME FPGA is Not Ready &nbsp ";
	if(stat&0x1000) *out << " &nbsp DDU is Not Ready";
	*out << "  </font></blockquote>";
	iblink=0;
      }
      printf(" now boxes \n");
      if(i==508||i==512||i==514||i==517||i==515||i==513){
	 string xmltext="";
	 if(i==508)xmltext="ffff";
         if(i==512)xmltext="f0f0";
         if(i==514)xmltext="f0f0";
         if(i==517)xmltext="0f0e";
         if(i!=515&&i!=513){
	   *out << cgicc::input().set("type","text")
	     .set("name","textdata")
	     .set("size","10")
	     .set("ENCTYPE","multipart/form-data")
	     .set("value",xmltext)
	     .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
         }
	 else{
	   *out << cgicc::input().set("type","hidden")
	     .set("name","textdata")
	     .set("size","10")
	     .set("ENCTYPE","multipart/form-data")
	     .set("value",xmltext)
	     .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
         }
	 *out << cgicc::input().set("type","submit")
	   .set("value","set");
         sprintf(buf,"%d",ddu);
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
         sprintf(buf,"%d",i);       
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
	 //         *out << cgicc::form() << std::endl ;
         *out << cgicc::form() << std::endl;
	 //	*out << buf << br() 
      }
      else{
	*out << br() << std::endl;
      }
    }

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;     
    *out << cgicc::html() << std::endl;
}



void EmuFCrateHyperDAQ::VMESERI(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
    printf(" enter VMESERI \n");
    cgicc::Cgicc cgi(in);
    printf(" initialize env \n");
    const CgiEnvironment& env = cgi.getEnvironment();
    printf(" getQueryString \n");
    std::string crateStr = env.getQueryString() ;
    cout << crateStr << endl ;
    printf(" VMESERI ifs \n");
    cgicc::form_iterator name = cgi.getElement("ddu");
    int ddu,istat;
    float radc;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }

    thisDDU = dduVector[ddu];
    printf(" set up web page \n");
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("VMESERI Web Form") << std::endl;
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300],buf2[300] ;
    sprintf(buf,"DDU SERIAL VME  Slot %d",thisDDU->slot());
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
   
    for(int i=600;i<606;i++){
      printf(" LOOP: %d \n",i);
      thisDDU->CAEN_err_reset();
      if(i==602){
	thisDDU->read_status(); 
	istat=thisDDU->rcv_serial[1]&0xff;
	sprintf(buf,"%02X</font> &nbsp ",istat);
	sprintf(buf2,"<font color=green>");
	if (istat&0x003c!=0x000c)sprintf(buf2,"<font color=yellow>");
	else if (istat&0x0080!=0x0080)sprintf(buf2,"<font color=red>");
	*out << "Serial Flash RAM Status: " << buf2 << buf << br() << std::endl;

	//           sprintf(buf,"Serial Flash RAM Status:");
	//           sprintf(buf2," %02X ",thisDDU->rcv_serial[1]&0xff);
      }
      if(i==603){  // EXPERT!
           thisDDU->read_page1(); 
           sprintf(buf,"Flash Live Channel Page:");
           sprintf(buf2," %02X%02X <font color=red> EXPERT ONLY! </font> ",thisDDU->rcv_serial[0]&0xff,thisDDU->rcv_serial[1]&0xff);
      }
      unsigned short int code[3];
      if(i==604){  // EXPERT!
           thisDDU->read_page5(); 
           sprintf(buf,"Flash GBE Threshold Page:"); 
           code[0]=(((thisDDU->rcv_serial[0]&0xC0)>>6)|((thisDDU->rcv_serial[3]&0xFF)<<2)|((thisDDU->rcv_serial[2]&0x3F)<<10));
           code[1]=(((thisDDU->rcv_serial[2]&0xC0)>>6)|((thisDDU->rcv_serial[5]&0xFF)<<2)|((thisDDU->rcv_serial[4]&0x3F)<<10));
           code[2]=((thisDDU->rcv_serial[4]&0xC0)>>6);
           sprintf(buf2,"%01X/%04X/%04X <font color=red> EXPERT ONLY! </font> ",code[2],code[1],code[0]);
      }
      if(i==605){  // EXPERT!
           sprintf(buf,"Flash DDU Board ID Page:");
           sprintf(buf2," %d <font color=red> EXPERT ONLY! </font> ",thisDDU->read_page7()&0x0000ffff);
      }
      if(i==600){
	radc=thisDDU->adcplus(1,4);
	sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<1450||radc>1550)sprintf(buf2,"<font color=yellow>");
	if (radc<1400||radc>1600)sprintf(buf2,"<font color=red>");
	*out << "Voltages: V15=" << buf2 << buf;

	radc=thisDDU->adcplus(1,5);
	sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<2450||radc>2550)sprintf(buf2,"<font color=yellow>");
	if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
	*out << "V25=" << buf2 << buf;

	radc=thisDDU->adcplus(1,6);
	sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<2450||radc>2550)sprintf(buf2,"<font color=yellow>");
	if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
	*out << "V25A=" << buf2 << buf;

	radc=thisDDU->adcplus(1,7);
	sprintf(buf,"%5.fmV</font>",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<3250||radc>3400)sprintf(buf2,"<font color=yellow>");
	if (radc<3200||radc>3450)sprintf(buf2,"<font color=red>");
	*out << "V33=" << buf2 << buf << br() << std::endl;
	//	sprintf(buf2,"<font color=black>V25A=</font>%5.2fmV &nbsp <font color=black>V33=</font>%5.2fmV",thisDDU->adcplus(1,6),thisDDU->adcplus(1,7));
	//	sprintf(buf,"Voltages: V15=<font color=green>%5.2fmV</font> &nbsp V25=<font color=green>%5.2fmV</font> &nbsp ",thisDDU->adcplus(1,4),thisDDU->adcplus(1,5));
	//	sprintf(buf2,"<font color=black>V25A=</font>%5.2fmV &nbsp <font color=black>V33=</font>%5.2fmV",thisDDU->adcplus(1,6),thisDDU->adcplus(1,7));
      }
      if(i==601){
	radc=thisDDU->readthermx(0);
	sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=yellow>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
	*out << "Temperatures: Sensor0=" << buf2 << buf;

	radc=thisDDU->readthermx(1);
	sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"color=yellow>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"color=red>");
	*out << "Sensor1=<font " << buf2 << buf;

	radc=thisDDU->readthermx(2);
	sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=yellow>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
	*out << "Sensor2=" << buf2 << buf;

	radc=thisDDU->readthermx(3);
	sprintf(buf,"%5.2fF</font>",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=yellow>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
	*out << "Sensor3=" << buf2 << buf << br() << std::endl;
	//         sprintf(buf,"Temperatures:");
	//         sprintf(buf2,"%5.2fF %5.2fF %5.2fF %5.2fF ",thisDDU->readthermx(0),thisDDU->readthermx(1),thisDDU->readthermx(2),thisDDU->readthermx(3));
      }
      if(i==603||i==604||i==605){
	std::string ddutextload =
	  toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",ddutextload);
      }

      if(i==603||i==604||i==605){
      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else{
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2 << cgicc::span();
      *out << std::endl;
      printf(" now boxes \n");
      //      if(i==603||i==604||i==605){
	string xmltext="7fff";
	if(i==604)xmltext="000880440";
	if(i==605)xmltext="ff";
	*out << cgicc::input().set("type","text")
	  .set("name","textdata")
	  .set("size","10")
	  .set("ENCTYPE","multipart/form-data")
	  .set("value",xmltext)
	  .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
	*out << cgicc::input().set("type","submit")
	  .set("value","set");
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
	sprintf(buf,"%d",i);       
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
	*out << cgicc::form() << std::endl ;
      }else{
	*out << br() << std::endl;
      }
    }
    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    
    *out << cgicc::html() << std::endl;
}




void EmuFCrateHyperDAQ::DDUTextLoad(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  try
   {    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      cout << "DDU " << ddu << endl;
      DDU_ = ddu;
    } 
    int val;
    cgicc::form_iterator name2 = cgi.getElement("val");
    //
    if(name2 != cgi.getElements().end()) {
      val = cgi["val"]->getIntegerValue();
      cout << "VAL " << val << endl;
    } 
    //
    thisDDU = dduVector[ddu]; 
    //

    string XMLtext = cgi["textdata"]->getValue() ; 
	//
    cout << XMLtext  << endl ;
    unsigned short int para_val;
    unsigned long int send_val;
    if(val==200)thisDDU->ddu_reset();
    if(val==208){
      sscanf(XMLtext.data(),"%01hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2]);
      send_val=((thisDDU->snd_serial[0]<<16)&0x000f0000)|((thisDDU->snd_serial[1]<<8)&0x0000ff00)|(thisDDU->snd_serial[2]&0x0000ff);
       printf(" loadkillfiber send %05lx \n",send_val);
       thisDDU->ddu_loadkillfiber(send_val);
   } 
   if(val==225){
      sscanf(XMLtext.data(),"%01hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8)&0x00000f00)|(thisDDU->snd_serial[1]&0x0000ff);
       printf(" ldbxorbit send %d (%03xh) \n",para_val,para_val);
       thisDDU->ddu_loadbxorbit(para_val);
       } 
    if(val==234)thisDDU->ddu_vmel1a(); 
    if(val==320)thisDDU->infpga_reset(INFPGA0);
    if(val==420)thisDDU->infpga_reset(INFPGA1);
    if(val==508){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);

    }
     if(val==512){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_GbEprescale(para_val);

    }
// 507->08, 508._09, 509->10, 510->11, 511->16, 512->SAME, 513->14,
// 514->17, 515->18, 516->19, 517->20, 518->21, 519->22, 520->15,
// 521->13, 522->06, 523->07
//  then 508->09, 509->10, 510->11, 511->08, 515->013, 513->15
    if(val==514){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fakel1reg(para_val);

    }
    if(val==517){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fmmreg(para_val);
    }
    if(val==513){
        unsigned short int scale= thisDDU->vmepara_rd_GbEprescale();
        if (scale&0x0008) { 
           para_val=scale&(0xFFFF-0x0008);
        } else {
           para_val=scale|0x0008;
        }
	printf("          vmepara_wr_GbEprescale, para_val=%d \n",para_val);
	thisDDU->vmepara_wr_GbEprescale(para_val);
    } 
    if(val==515){
	unsigned short int scale2=thisDDU-> vmepara_rd_fakel1reg();
        if (scale2&0x0004) {
          para_val=0x0000;
        } else {
          para_val=0xFFFF;
        }
	printf("         vmepara_wr_fakel1reg, para_val=%d \n",para_val);
	//	thisDDU->vmepara_wr_fakel1reg(para_val);
    }
    if(val==603){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      //  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
      //      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      para_val=thisDDU->snd_serial[0]&0xff;
      if(thisDDU->snd_serial[1]&0xff)para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->vmepara_rd_inreg0();
      thisDDU->write_page1();
      // sleep(1);
    }
    if(val==604){
      thisDDU->snd_serial[5]=0x00;
      sscanf(XMLtext.data(),"%02hhx%02hhx%02hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2],&thisDDU->snd_serial[3],&thisDDU->snd_serial[4]);
      para_val=((thisDDU->snd_serial[4]<<8))&0xff00|(thisDDU->snd_serial[5]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      para_val=((thisDDU->snd_serial[2]<<8))&0xff00|(thisDDU->snd_serial[3]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->write_page5();
      // sleep(1);
    }
    if(val==605){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      //  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
      para_val=thisDDU->snd_serial[0]&0xff;
      if(thisDDU->snd_serial[1]&0xff)para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->vmepara_rd_inreg0();
      thisDDU->write_page7();
      //      sleep(1);
    }
    if(val/100==2){
      DDU_=ddu;
      this->DDUFpga(in,out);
    }
    if(val/100==3){
      DDU_=ddu;
      this->INFpga0(in,out);
    }
    if(val/100==4){
      DDU_=ddu;
      this->INFpga1(in,out);
    }
     if(val/100==6){
      DDU_=ddu;
      this->VMESERI(in,out);
    }
    if(val/100==5){
      DDU_=ddu;
      this->VMEPARA(in,out);
      } 

  }   
 catch (const std::exception & e )
  {
    printf(" exception raised in DDUTextLoad \n");
	//XECPT_RAISE(xgi::exception::Exception, e.what());
  }  
}  
  
void EmuFCrateHyperDAQ::VMEIntIRQ(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
   cgicc::Cgicc cgi(in);
    //
   const CgiEnvironment& env = cgi.getEnvironment();
    //
   std::string crateStr = env.getQueryString() ;
    //
   //  cout << crateStr << endl ;
    cgicc::form_iterator name = cgi.getElement("ddu");
    //
    int ddu;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      //  cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }

    if(irq_start[0]==0){
      VMEController *theController=thisCrate->vmeController();
      theController->irq_pthread_start(0);
      timer=0;
      for(int i=0;i<(int)dduVector.size();i++){
	thisDDU=dduVector[i];
        int slot=thisDDU->slot();
        if(slot<21){
	  if(thisDDU->vmepara_CSCstat()!=0x0000)irqprob=1;
        }
      }
    }

    thisDDU = dduVector[0];
 
   //   printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << meta().set("http-equiv","refresh").set("content","2") << std::endl;
    *out << cgicc::title("VMEIRQ Web Form") << std::endl;
    //  
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300];
    sprintf(buf,"DDU VME IRQ Interrupt Monitor");
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;



    if(ddu!=99){
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    if(thisDDU->irq_tester(0,0)!=0||irqprob!=0){ 
    *out << img().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/redlight.gif") << std::endl;
    /* 
S. Durkin kludge

for IE sound the following line must be added to 
HTMLClasses.h:  ATOMIC_ELEMENT  (bgsound,        "bgsound");
for Mozilla sound the following line must be added to 
HTMLClasses.h:  ATOMIC_ELEMENT  (embed,        "embed"); 
one must then recompile the libcgicc.so library 
    *out << bgsound().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/siren.wav") << std::endl;  */

    //*out << embed().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/siren.wav").set("hidden","true").set("autostart","true").set("loop","true") << std::endl;

    *out << br() << std::endl;
   *out << cgicc::span().set("style","font-size: 25pt;color:red;background-color:yellow;");
    long int xtimer=timer;
    long int xsec=xtimer%60;
    xtimer=(xtimer-xsec)/60;
    long int xmin=xtimer%60;
    long int xhr=(xtimer-xmin)/60;
    sprintf(buf,"%02ld:%02ld:%02ld",xhr,xmin,xsec);
    *out << buf << std::endl;
    *out << cgicc::span() << std::endl;

    if(thisDDU->irq_tester(0,0)!=0){
      *out << cgicc::span().set("style","font-size: 20pt;color:red");
      sprintf(buf," Failure in Crate %d Slot %d Status %04x",thisDDU->irq_tester(0,1),thisDDU->irq_tester(0,2),thisDDU->irq_tester(0,3));
    }else{ 
     *out << cgicc::span().set("style","font-size: 20pt;color:red");
      sprintf(buf," DDUs have Interrupted previously");
    } 
    *out << buf << std::endl;
    }else{ 
    *out << cgicc::span().set("style","color:green");
    *out << "Time without interrupt" << std::endl;
    *out << cgicc::span() << std::endl;
    *out << cgicc::span().set("style","font-size: 25pt;color:blue;background-color:black;");
    //  printf(" timer %ld start %d irq_start \n",timer,irq_start[0]);
    long int xtimer=timer;
    long int xsec=xtimer%60;
    xtimer=(xtimer-xsec)/60;
    long int xmin=xtimer%60;
    long int xhr=(xtimer-xmin)/60;
    timer=timer+2;
    sprintf(buf,"%02ld:%02ld:%02ld",xhr,xmin,xsec);
    *out << buf << std::endl;
    }
    *out << cgicc::span() << std::endl;
    std::string irqtester =
    toolbox::toString("/%s/IRQTester",getApplicationDescriptor()->getURN().c_str());

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;
    }else{
           VMEController *theController=thisCrate->vmeController();
           theController->irq_pthread_end(0); 
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << " VME IRQ Interrupt has been disabled \n"; 
    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;

    }
    *out << cgicc::html() << std::endl;
}  

void EmuFCrateHyperDAQ::DCCFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
     unsigned long int idcode,uscode;
    unsigned long int tidcode[2]={0x05035093,0xf5059093};
    unsigned long int tuscode[2]={0xdcc30046,0xdcc31061};
    //
    cgicc::Cgicc cgi(in);
    //    printf(" initialize env \n");
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    printf(" getQueryString \n");
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ; 

    cgicc::form_iterator name = cgi.getElement("dcc");
    //
    int dcc;
    if(name != cgi.getElements().end()) {
      dcc = cgi["dcc"]->getIntegerValue();
      cout << "DCC inside " << dcc << endl;
      DCC_ = dcc;
    }else{
      dcc=DCC_;
    }
    thisDCC = dccVector[dcc];

    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCC Firmware Form") << std::endl;
    //
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;
 
    char buf[300] ;
    sprintf(buf,"DCC Firmware Slot %d",thisDCC->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue") << std::endl ;
  
    //
    for(int i=0;i<2;i++){ 
      //*out << cgicc::table().set("border","0").set("rules","none").set("frame","void"); 
      // *out << cgicc::tr();
      printf(" LOOP: %d \n",i);
    *out << cgicc::span().set("style","color:black");
    if(thisDCC->slot()>=21){
    if(i==0){sprintf(buf,"inprom  ");}
    if(i==1){sprintf(buf,"mprom  ");}
    printf(" %s ",buf);
    *out<< buf << std::endl;;
    }
    if(thisDCC->slot()<21){
    if(i==0){idcode=thisDCC->inprom_chipid(); sprintf(buf,"inprom  ");}
    if(i==1){idcode=thisDCC->mprom_chipid(); sprintf(buf,"mprom  ");}
    printf(" %s idcode %08lx ",buf,idcode);
    *out<<buf;
    sprintf(buf,"  id: ");    
    *out << buf;*out << cgicc::span();
    if(idcode!=tidcode[i]){
    *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
    }else{ 
    *out << cgicc::span().set("style","color:green;background-color:#dddddd");
    }
    sprintf(buf,"%08lX",idcode);
    *out << buf;*out << cgicc::span();
    thisDCC->CAEN_err_reset();
    if(i==0){uscode=thisDCC->inprom_userid();}
    if(i==1){uscode=thisDCC->mprom_userid();}
    *out << cgicc::span().set("style","color:black");
    sprintf(buf," usr: ");
    *out << buf;*out << cgicc::span();
    printf(" uscode %08lx \n",uscode);
    if(thisDCC->CAEN_err()!=0){ 
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
    }else if(uscode!=tuscode[i]){
    *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
    }else{ 
    *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
    }
    sprintf(buf,"%08lX",uscode);
    *out << buf ;*out << cgicc::span() << std::endl;
    }
    *out << br() << std::endl;
    printf(" now boxes \n");
    if(i<2){
      std::string dccloadfirmware =
	toolbox::toString("/%s/DCCLoadFirmware",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","POST")
        .set("enctype","multipart/form-data")
	.set("action",dccloadfirmware) << std::endl;;	
      //.set("enctype","multipart/form-data")
      //
      *out << cgicc::input().set("type","file")
	.set("name","DCCLoadSVF")
	.set("size","50") << std::endl;
    //    
      *out << cgicc::input().set("type","submit").set("value","LoadSVF") << std::endl;   
       sprintf(buf,"%d",dcc);
       *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl; 
       sprintf(buf,"%d",i);       
       *out << cgicc::input().set("type","hidden").set("value",buf).set("name","prom") << std::endl; 
      *out << cgicc::form() << std::endl ;
           std::string dccfirmwarereset =
	  toolbox::toString("/%s/DCCFirmwareReset",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",dccfirmwarereset)
	    << std::endl;
       if(i==0){
	  *out << cgicc::input().set("type","submit").set("value","Reset InFPGA")<<std::endl;
       }else{
	  *out << cgicc::input().set("type","submit").set("value","Reset MFPGA")<<std::endl;
       }      
       sprintf(buf,"%d",dcc);
       *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc") << std::endl;  
         sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","val") << std::endl;
	  *out << cgicc::form() << std::endl;
    }else{ 
     *out << std::endl; 
    }
    // *out << cgicc::tr() << std::endl;
    // *out << cgicc::table() << std::endl; 

    *out << br() << std::endl;
    }
    *out << cgicc::legend() << std::endl;
    *out << cgicc::fieldset()<< std::endl;
    *out << cgicc::body() << std::endl; 
    *out << cgicc::html() << std::endl;
}  

void EmuFCrateHyperDAQ::DCCLoadFirmware(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  try{
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dcc");
    //
    int dcc;
    if(name != cgi.getElements().end()) {
      dcc = cgi["dcc"]->getIntegerValue();
      cout << "DCC " << dcc << endl;
      DCC_ = dcc;
    } 
    int prom;
    cgicc::form_iterator name2 = cgi.getElement("prom");
    //
    if(name2 != cgi.getElements().end()) {
      prom = cgi["prom"]->getIntegerValue();
      cout << "PROM " << prom << endl;
    } 
    //
    printf(" Entered DCCLoadFirmware prom %d \n",prom);
    thisDCC = dccVector[dcc]; 
    //
	//
	cout << "UploadConfFileUpload" << endl ;
	//

	
	const_file_iterator file;
	file = cgi.getFile("DCCLoadSVF");
	

	//
	cout << "GetFiles" << endl ;
	//

	if(file != cgi.getFiles().end()) {
          string XMLname = (*file).getFilename();
          cout <<"SVF FILE: " << XMLname  << endl ;
        
 	  ofstream TextFile ;
	  TextFile.open("MySVFFile.svf");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	} 
	/*  char buf[400];
	FILE *dwnfp;
        dwnfp    = fopen("MySVFFile.svf","r");	
        while (fgets(buf,256,dwnfp) != NULL)printf("%s",buf);
        fclose(dwnfp); */
          char *cbrdnum;
          printf(" DCC epromload %d \n",prom);
          cbrdnum=(char*)malloc(5);
          cbrdnum[0]=0x00;cbrdnum[1]=0x00;cbrdnum[2]=0x00;cbrdnum[3]=0x00;
	  if(prom==1)thisDCC->epromload("MPROM",RESET,"MySVFFile.svf",1,cbrdnum);
          if(prom==0)thisDCC->epromload("INPROM",INPROM,"MySVFFile.svf",1,cbrdnum);
          in=NULL;
          this->DCCFirmware(in,out);
  }
  catch (const std::exception & e )
  {
    printf(" exception raised in DCCLoadFirmware \n");
	//XECPT_RAISE(xgi::exception::Exception, e.what());
  }  
}  

void EmuFCrateHyperDAQ::DCCCommands(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
    printf(" enter: DCC Commands \n");
    cgicc::Cgicc cgi(in);
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ;

    cgicc::form_iterator name = cgi.getElement("dcc");
    //
    int dcc;
    if(name != cgi.getElements().end()) {
      dcc = cgi["dcc"]->getIntegerValue();
      cout << "DCC inside " << dcc << endl;
      DCC_ = dcc;
    }else{
      dcc=DCC_;
    }
    printf(" DCC %d \n",dcc);
    thisDCC = dccVector[dcc];
    printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCC Comands Web Form") << std::endl;
    //
     *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    char buf[300],buf2[300] ;
    sprintf(buf,"DCC Commands VME  Slot %d",thisDCC->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
    
    for(int i=100;i<107;i++){  
      thisDCC->CAEN_err_reset();
      if(i==100){
           unsigned short int statush=thisDCC->mctrl_stath();
           unsigned short int statusl=thisDCC->mctrl_statl();  
           sprintf(buf,"Status:");
           sprintf(buf2," H: %04X L: %04X ",statush,statusl);
      }
      if(i==101){
           sprintf(buf,"BX Reset:");
           sprintf(buf2," ");
      }
      if(i==102){
           sprintf(buf,"EVN Reset:");
           sprintf(buf2," ");
      }
      if(i==103){
           unsigned short int fifouse=thisDCC->mctrl_rd_fifoinuse();
           sprintf(buf,"Set FIFOs Used:");
	   sprintf(buf2," %04X ",(fifouse&0x7ff)); 
	   //           sprintf(buf2," ");
      }
      if(i==104){ 
           unsigned short int ttccmd=thisDCC->mctrl_rd_ttccmd();
           sprintf(buf,"TTC Command:");
	   sprintf(buf2," %04X ",(ttccmd>>2)&0x3f); 
	   //           sprintf(buf2," ");
      }
      if(i==105){
           unsigned short int fifouse=thisDCC->mctrl_rd_fifoinuse();
           unsigned short int ttccmd=thisDCC->mctrl_rd_ttccmd();
           sprintf(buf,"Load L1A:");
           sprintf(buf2," %02x , %02x",((ttccmd>>9)&0x60)+((fifouse>>11)&0x1f),(ttccmd>>7)&0x7e );
      }
      if(i==106){
           sprintf(buf," Load L1A(no prompt):");
           sprintf(buf2," ");
      }
  

      if(i>100){
         std::string dcctextload =
	 toolbox::toString("/%s/DCCTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",dcctextload) << std::endl;
      }
      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(thisDCC->CAEN_err()!=0){
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else{
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2;
      *out << cgicc::span();
      if(i>100){
	 string xmltext="";
	 if(i==103) {
           //int readback=thisDCC->mctrl_rd_fifoinuse();
           xmltext="ffff";
	   //       xmltext=(readback);
	 }
         if(i==104) {
	   // int readback2= thisDCC->mctrl_rd_tcccmd();
           xmltext="ffff";
	   //  xmltext=(readback2);
	 }
         if(i==105)xmltext="2,5";;
         if(i==103|i==104|i==105){
         *out << cgicc::input().set("type","text")
	.set("name","textdata")
	.set("size","10")
	.set("ENCTYPE","multipart/form-data")
	.set("value",xmltext)
        .set("style","font-size: 13pt; font-family: arial;")<<std::endl;
         }
        *out << cgicc::input().set("type","submit")
	  .set("value","set");
         sprintf(buf,"%d",dcc);
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dcc"); 
         sprintf(buf,"%d",i);       
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
         *out << cgicc::form() << std::endl ;
       }else{
	 *out << br() << std::endl;
       }  
    
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;   
    *out << cgicc::html() << std::endl;
    

}  

void EmuFCrateHyperDAQ::DCCTextLoad(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  try
   {    // 
     printf(" inside DCCTextLoad \n");    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dcc");
    //
    int dcc;
    if(name != cgi.getElements().end()) {
      dcc = cgi["dcc"]->getIntegerValue();
      cout << "DCC " << dcc << endl;
      DCC_ = dcc;
    } 
    int val;
    cgicc::form_iterator name2 = cgi.getElement("val");
    //
    if(name2 != cgi.getElements().end()) {
      val = cgi["val"]->getIntegerValue();
      cout << "VAL _new" << val << endl;
    } 
    //
    thisDCC = dccVector[dcc]; 
    //

    string XMLtext;
    if (val==103 || val==104 || val==105) {
       XMLtext = cgi["textdata"]->getValue() ; 
	//
     cout << XMLtext  << endl ;
    }
    unsigned  short para_val;
    char snd_serial[2];
    static int rate,num;
    char arate;
    char anum;
    if(val==101)thisDCC->mctrl_bxr();
    if(val==102)thisDCC->mctrl_evnr();
    snd_serial[0]=0;
    snd_serial[1]=0;
    if(val==103){
      //      sscanf(XMLtext.data(),"%04x",&para_val);
      std::istringstream test(XMLtext);
	test >> hex >> para_val;
      printf(" mctrl_fifouse send %04x \n",para_val);
      thisDCC->mctrl_fifoinuse(para_val);
    } 
    if(val==104){
      //      std::istringstream test(XMLtext);
      //	test >> hex >> para_val;
       sscanf(XMLtext.data(),"%x",&para_val); 
       printf(" ttc command send %04x \n",para_val);
       thisDCC->mctrl_ttccmd(para_val);
    } 
    if(val==105){
      sscanf(XMLtext.data(),"%d,%d",&rate,&num);
      if(rate>255)rate=255;
      if(num>255)num=255;
      arate=rate;anum=num;
       printf(" load L1A send rate:%d num:%d \n",arate,anum);
       thisDCC->mctrl_fakeL1A(arate,anum);
    } 
    if(val==106){      
      arate=rate;anum=num;
       printf(" load (no prompt) L1A send rate:%d num:%d \n",arate,anum);
       thisDCC->mctrl_fakeL1A(arate,anum);
    } 

      DCC_=dcc;
      this->DCCCommands(in,out);
      

  }
  catch (const std::exception & e )
  {
    printf(" exception raised in DCCLoadFirmware \n");
	//XECPT_RAISE(xgi::exception::Exception, e.what());
  }  
}

void EmuFCrateHyperDAQ::IRQTester(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  try
  {    // 
    cgicc::Cgicc cgi(in);
    printf(" start sleep \n");
    sleep(10);
    printf(" end sleep \n");
    this->VMEIntIRQ(in,out);
  }
  catch (const std::exception & e )
  {
    printf(" exception raised in DCCLoadFirmware \n");
	//XECPT_RAISE(xgi::exception::Exception, e.what());
  }  
}

void EmuFCrateHyperDAQ::DCCFirmwareReset(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  try
  {
    // 
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("dcc");
    //
    int dcc;
    if(name != cgi.getElements().end()) {
      dcc = cgi["dcc"]->getIntegerValue();
      cout << "DCC " << dcc << endl;
      DCC_ = dcc;
    } 
    int i;
    cgicc::form_iterator name2 = cgi.getElement("val");
    //
    if(name2 != cgi.getElements().end()) {
      i = cgi["val"]->getIntegerValue();
      cout << "val" << i << endl;
    } 
    //
    thisDCC = dccVector[dcc]; 
    if(i==0)thisDCC->hdrst_in();
    if(i==1)thisDCC->hdrst_main();
    sleep(1);
    in=NULL;    
    DCC_=dcc;
    this->DCCFirmware(in,out);
  }
  catch (const std::exception & e )
  {
    printf(" exception raised in DCCLoadFirmware \n");
	//XECPT_RAISE(xgi::exception::Exception, e.what());
  }  
}

void EmuFCrateHyperDAQ::DDUVoltMon(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
  float v_val[4]={1500.,2500.,2500.,3300.};
  float v_delt={100.};  
  float t_val[3]={85.,85.,85.};
  float t_delt={7.};

  cgicc::Cgicc cgi(in);
    //
        printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << meta().set("http-equiv","refresh").set("content","60") << std::endl;
    *out << cgicc::title("DDU Temp/Volt  Web Form") << std::endl;
    //  
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300];
    sprintf(buf,"DDU Volt/Temp Monitor");
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
	for (int i=0; i<(int)dduVector.size(); i++) {
         *out << cgicc::table().set("border","0").set("rules","none").set("frame","void"); 
	 *out << cgicc::tr();
          thisDDU=dduVector[i];
	  int slot = thisDDU->slot();
          if(slot<=21){
	    char buf[20];	
            sprintf(buf,"DDU Slot: %d ",slot);
	    printf(" %s \n",buf);      
            *out << buf; 
            sprintf(buf,"Voltages:");
            *out << buf;
            for(int j=4;j<8;j++){
              thisDDU->CAEN_err_reset();
	      float adc=thisDDU->adcplus(1,j);
	      //  printf(" caen_err %d \n",thisDDU->CAEN_err());
              if(thisDDU->CAEN_err()!=0){
                 *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");           
              }
              else if((adc>v_val[j-4]-v_delt)&&(adc<v_val[j-4]+v_delt)){
                 *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
              }else{
                 *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
              }
              char buf2[20];
              sprintf(buf2," %5.0fmV",adc);
              *out << buf2;
              *out << cgicc::span() << std::endl;
            }
            sprintf(buf,"Temperatures:");
            *out << buf;
            for(int j=0;j<3;j++){
              thisDDU->CAEN_err_reset();
	      float adc=thisDDU->readthermx(j);
              if(thisDDU->CAEN_err()!=0){
                 *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");           
              }
              else if((adc>t_val[j]-t_delt)&&(adc<t_val[j]+t_delt)){
                 *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
              }else{
                 *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
              }
              char buf2[20];
              sprintf(buf2," %5.2fF",adc);
              *out << buf2;
              *out << cgicc::span() << std::endl;
            }
          }      
          *out << cgicc::tr() << std::endl;
          *out << cgicc::table() << std::endl; 
        }
    *out << cgicc::fieldset()<< std::endl;
    *out << cgicc::body() << std::endl; 
    *out << cgicc::html() << std::endl;
}




};
#endif
