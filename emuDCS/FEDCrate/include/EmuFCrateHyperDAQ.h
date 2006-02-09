
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
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCCommands,"DCCCommands");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
    xgi::bind(this,&EmuFCrateHyperDAQ::IRQTester,"IRQTester");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmwareReset,"DCCFirwareReset");
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
    //
    if (dduVector.size()==0 && dccVector.size()==0) {
      //

      std::string method =
	toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
      *out << std::endl;
      //
      *out << cgicc::legend("Upload Configuration...").set("style","color:blue") 
	   << std::endl ;
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
    *out << "Paste configuration..." << std::endl;
    *out << cgicc::textarea() << std::endl;
    *out << cgicc::input().set("type","submit").set("value","Send");
    *out << cgicc::form() << std::endl ;
    
    //
    *out << std::endl;
    //    
    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl; 
    } else if (dduVector.size()>0 || dccVector.size()>0) {
      //	std::string DDUBoardID ;
      //        char Name[50] ;
      *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

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
            int brdnum;
            // sleep(1);
            thisDDU->CAEN_err_reset();
            brdnum=thisDDU->read_page7();
            sprintf(buf,"Board: %d ",brdnum);
            *out << buf;
            if(thisDDU->CAEN_err()!=0){
            *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");           
            }
            else if(status==0x0000){
            *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }else{
            *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
            }
            sprintf(buf,"Status: %04X ",status);
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
	  *out << cgicc::form() << std::endl;
          if(slot<=21){
          std::string ddufpga =
          toolbox::toString("/%s/DDUFpga",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",ddufpga)
		.set("target","_blank") << std::endl;
	  *out << cgicc::input().set("type","submit").set("value","DDUFpga") << std::endl;  
          sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu") << std::endl;
          *out << cgicc::form() << std::endl;
          std::string infpga0 =
          toolbox::toString("/%s/INFpga0",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",infpga0)
		.set("target","_blank") << std::endl ;
	  *out << cgicc::input().set("type","submit").set("value","INFpga0")<< std::endl;  
          sprintf(buf,"%d",i);
          *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu")<< std::endl ;
          *out << cgicc::form() << std::endl;

          std::string infpga1 =
          toolbox::toString("/%s/INFpga1",getApplicationDescriptor()->getURN().c_str());
	  *out << cgicc::form().set("method","GET").set("action",infpga1)
		.set("target","_blank") << std::endl ;
	  *out << cgicc::input().set("type","submit").set("value","INFpga1")<< std::endl;  
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
    unsigned long int tuscode[8]={0xcf028a02,0xdf017a03,0xdf017a03,0xd0017a03,0xd1017a03,0xb0016a01,0xc028dd03,0xc128dd03};
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

    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DDU Firmware Form") << std::endl;
    //
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;
    *out << "(Load dual proms in order 1-0) (For hard reset use DCC TCC command)" <<std::endl;
    *out << br() << std::endl;
    *out << br() << std::endl;

    char buf[300] ;
    sprintf(buf,"DDU Firmware Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue") << std::endl ;
  
    //
    for(int i=0;i<8;i++){ 
      printf(" LOOP: %d \n",i);
    *out << cgicc::span().set("style","color:black");
    if(thisDDU->slot()>=21){
    if(i==0){sprintf(buf,"ddufpga  ");}
    if(i==1){sprintf(buf,"infpga0  ");}
    if(i==2){sprintf(buf,"infpga1  ");}
    if(i==3){sprintf(buf,"inprom0  ");}
    if(i==4){sprintf(buf,"inprom1  ");}
    if(i==5){sprintf(buf,"vmeprom  ");}
    if(i==6){sprintf(buf,"dduprom0 ");}
    if(i==7){sprintf(buf,"dduprom1 ");}
    printf(" %s ",buf);
    *out<< buf << std::endl;;

    }
    if(thisDDU->slot()<21){
    thisDDU->CAEN_err_reset();
    if(i==0){idcode=thisDDU->ddufpga_idcode(); sprintf(buf,"ddufpga  ");}
    if(i==1){idcode=thisDDU->infpga_idcode0(); sprintf(buf,"infpga0  ");}
    if(i==2){idcode=thisDDU->infpga_idcode1(); sprintf(buf,"infpga1  ");}
    if(i==3){idcode=thisDDU->inprom_idcode0(); sprintf(buf,"inprom0  ");}
    if(i==4){idcode=thisDDU->inprom_idcode1(); sprintf(buf,"inprom1  ");}
    if(i==5){idcode=thisDDU->vmeprom_idcode(), sprintf(buf,"vmeprom  ");}
    if(i==6){idcode=thisDDU->dduprom_idcode0();sprintf(buf,"dduprom0 ");}
    if(i==7){idcode=thisDDU->dduprom_idcode1();sprintf(buf,"dduprom1 ");}
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
    if(i==3){uscode=thisDDU->inprom_usercode0();}
    if(i==4){uscode=thisDDU->inprom_usercode1();}
    if(i==5){uscode=thisDDU->vmeprom_usercode();}
    if(i==6){uscode=thisDDU->dduprom_usercode0();}
    if(i==7){uscode=thisDDU->dduprom_usercode1();}
    *out << cgicc::span().set("style","color:black");
    sprintf(buf," usr: ");
    *out << buf;*out << cgicc::span();
    printf(" uscode %08lx \n",uscode);
    if(thisDDU->CAEN_err()!=0){
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
    if(i>=3&&i<8){
      std::string dduloadfirmware =
	toolbox::toString("/%s/DDULoadFirmware",getApplicationDescriptor()->getURN().c_str());
      //
      *out << cgicc::form().set("method","POST")
        .set("enctype","multipart/form-data")
	.set("action",dduloadfirmware) << std::endl;;	
      //.set("enctype","multipart/form-data")
      //
      *out << cgicc::input().set("type","file")
	.set("name","DDULoadSVF")
	.set("size","50") << std::endl;
    //    
      *out << cgicc::input().set("type","submit").set("value","LoadSVF") << std::endl;   
       sprintf(buf,"%d",ddu);
       *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu") << std::endl; 
       sprintf(buf,"%d",i);       
       *out << cgicc::input().set("type","hidden").set("value",buf).set("name","prom") << std::endl; 
      *out << cgicc::form() << std::endl ;
    }else{ 
     *out << std::endl; 
    }
    }
    *out << cgicc::fieldset()<< std::endl;
    *out << cgicc::body() << std::endl;
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
	  if(prom==3)thisDDU->epromload("INPROM0",INPROM0,"MySVFFile.svf",1,cbrdnum);
          if(prom==4)thisDDU->epromload("INPROM1",INPROM1,"MySVFFile.svf",1,cbrdnum);
          if(prom==5)thisDDU->epromload("RESET",RESET,"MySVFFile.svf",1,cbrdnum);
          if(prom==6||prom==7){
            int brdnum=thisDDU->read_page7();
            cbrdnum[0]=brdnum;
          } 
          if(prom==6)thisDDU->epromload("DDUPROM0",DDUPROM0,"MySVFFile.svf",1,cbrdnum);
          if(prom==7)thisDDU->epromload("DDUPROM1",DDUPROM1,"MySVFFile.svf",1,cbrdnum);
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
    sprintf(buf,"DDU INFPGA1 VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
    
    for(int i=200;i<235;i++){
      thisDDU->ddu_shift0=0x0000;
      thisDDU->CAEN_err_reset();
      // *out << cgicc::table().set("border","0");
      // *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->infpga_shift0=0x0000;
      if(i==200){
           sprintf(buf,"ddu reset:");
           sprintf(buf2," ");
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
	thisDDU->ddu_hifpgastat();
           sprintf(buf,"32-bit DDU Control DFPGA Status:");
           sprintf(buf2," %04X%04X ",thisDDU->infpga_code0,thisDDU->ddu_code1);
      }
      if(i==204){
	thisDDU->ddu_checkFIFOa();
           sprintf(buf,"FIFO-A Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==205){
	thisDDU->ddu_checkFIFOb();
           sprintf(buf,"FIFO-B Status [15-0]:");
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
           sprintf(buf2," %01X%04X ",thisDDU->infpga_code0,thisDDU->ddu_code1);
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
           sprintf(buf2," %02X%04X ",thisDDU->ddu_code0,thisDDU->ddu_code1);
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
      if(i==226){
	thisDDU->ddu_shfttst(0);
	unsigned short int tmp;
	tmp=thisDDU->infpga_shift0;
	thisDDU->ddu_shfttst(1);
           sprintf(buf,"DDU Shift Test:");
           sprintf(buf2," %04X %04X",tmp,thisDDU->infpga_shift0);
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
      if(i==233){
	thisDDU->ddu_rd_boardID();
           sprintf(buf,"16-bit DDU Board ID:DDR:"); 
           sprintf(buf2," 0x%04X ",thisDDU->ddu_code0);
      }
      if(i==234){
           sprintf(buf,"DDUFPGA VME L1A:"); 
           sprintf(buf2," ");
      } 
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
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else{ 
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2;*out << cgicc::span();
      if((thisDDU->ddu_shift0!=0xFACE)&&i!=226&&i!=200&&i!=234){
        sprintf(buf," shift:%04X",thisDDU->infpga_shift0);
        *out << buf;
      }
      if(i==200||i==208||i==225||i==234){
	 string xmltext="";
	 if(i==208)xmltext="7ffff";
         if(i==225)xmltext="fff";;
         if(i!=200&&i!=209){
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
         *out << cgicc::form() << std::endl ;
	 // *out << cgicc::td() << std::endl;
       }else{
	 *out << br() << std::endl;
	 // *out << cgicc::td() << std::endl ;
       }  
      // *out << cgicc::table() << std::endl;
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    
}
 
void EmuFCrateHyperDAQ::INFpga0(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
   
     try
      {
    printf(" enter: INFpga0 \n");
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
    *out << cgicc::title("INFPGA0 Web Form") << std::endl;
    //
    char buf[300],buf2[300] ;
    sprintf(buf,"DDU INFPGA1 VME  Slot %d",thisDDU->slot());
    //
     *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
    
    for(int i=300;i<326;i++){
      *out << cgicc::table().set("border","0");
      *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->infpga_shift0=0x0000; 
      thisDDU->CAEN_err_reset();
      *out << cgicc::span().set("style","color:black");
      if(i==300){
           sprintf(buf,"infpga0 reset:");
           sprintf(buf2," ");
      }
      if(i==301){
	thisDDU->infpga_rdscaler(INFPGA0);
           sprintf(buf,"infpga0 L1 Event Scaler0 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==302){
	thisDDU->infpga_rd1scaler(INFPGA0);
           sprintf(buf,"infpga0 L1 Event Scaler1 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==303){
	thisDDU->infpga_lowstat(INFPGA0);
           sprintf(buf,"infpga0 16-low-bits  Status:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==304){
	thisDDU->infpga_histat(INFPGA0);
           sprintf(buf,"infpga0 16-high-bits Status:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==305){
	thisDDU->infpga_rd1scaler(INFPGA0);
           sprintf(buf,"infpga0 32-bit Status:");
           sprintf(buf2," %04X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==306){
	thisDDU->infpga_shfttst(INFPGA0,0);
	unsigned short int tmp;
	tmp=thisDDU->infpga_shift0;
	thisDDU->infpga_shfttst(INFPGA0,1);
           sprintf(buf,"infpga0 shift test:");
           sprintf(buf2," %04X %04X",tmp,thisDDU->infpga_shift0);
      }
      if(i==307){
	thisDDU->infpga_CheckFiber(INFPGA0);
           sprintf(buf,"infpga0 Check Fiber Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==308){
	thisDDU->infpga_DMBsync(INFPGA0);
           sprintf(buf,"infpga0 DMB Sync Checks [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==309){
	thisDDU->infpga_FIFOstatus(INFPGA0);
           sprintf(buf,"infpga0 DDU Input FIFO Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==310){
	thisDDU->infpga_FIFOfull(INFPGA0);
           sprintf(buf,"infpga0 DDU Input FIFO Full[15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==311){
	thisDDU->infpga_RxErr(INFPGA0);
           sprintf(buf,"infpga0 DDU Rx Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==312){
	thisDDU->infpga_Timeout(INFPGA0);
           sprintf(buf,"infpga0 DDU Timeout [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      } 
      if(i==313){
	thisDDU->infpga_XmitErr(INFPGA0);
           sprintf(buf,"infpga0 Xmit Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==314){
	thisDDU->infpga_LostErr(INFPGA0);
           sprintf(buf,"infpga0 DDU Lost Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==315){
	thisDDU->infpga_CcodeStat(INFPGA0);
           sprintf(buf,"infpga0 DDU C-code Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==316){
	thisDDU->infpga_StatA(INFPGA0);
           sprintf(buf,"infpga0 DDU Status Reg A [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==317){
	thisDDU->infpga_StatB(INFPGA0);
           sprintf(buf,"infpga0 DDU Status Reg B [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==318){
	thisDDU->infpga_StatC(INFPGA0);
           sprintf(buf,"infpga0 DDU Status Reg C [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==319){
	thisDDU->infpga_DMBwarn(INFPGA0);
           sprintf(buf,"infpga0 DMB Full/Warn [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==320){
	thisDDU->infpga_MemAvail(INFPGA0);
           sprintf(buf,"infpga0 FIFO Memory Available:");
           sprintf(buf2,"0- %04X 1- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==321){
	thisDDU->infpga_WrMemActive(INFPGA0,0);
           sprintf(buf,"infpga0 FIFO Memory Available:");
           sprintf(buf2," 8- %04X  9- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==322){
	thisDDU->infpga_WrMemActive(INFPGA0,1);
           sprintf(buf,"infpga0 FIFO Memory Available:");
           sprintf(buf2,"10- %04X 11- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==323){
	thisDDU->infpga_WrMemActive(INFPGA0,2);
           sprintf(buf,"infpga0 FIFO Memory Available:");
           sprintf(buf2,"12- %04X 13- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==324){
	thisDDU->infpga_WrMemActive(INFPGA0,3);
           sprintf(buf,"infpga0 FIFO Memory Available:");
           sprintf(buf2,"14- %04X 15- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==325){
	thisDDU->infpga_Min_Mem(INFPGA0);
           sprintf(buf,"infpga0 Minimum FIFO Memory Availabile:");
           sprintf(buf2,"0- %04X 1- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      *out << buf << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
       *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else if((thisDDU->infpga_shift0!=0xFACE)&&(i!=306)){
      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{ 
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2;*out << cgicc::span();
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=306&&i!=300){
        sprintf(buf," shift:%04X",thisDDU->infpga_shift0);
        *out << buf;
      }
      if(i==300){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::input().set("type","submit")
	  .set("value","set");
         sprintf(buf,"%d",ddu);
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
         sprintf(buf,"%d",i);       
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
         *out << cgicc::form() << std::endl ;
         *out << cgicc::td() << std::endl;
       }else{
         *out << std::endl;
         *out << cgicc::td() << std::endl ;
       }  
      *out << cgicc::table() << std::endl;
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    
 

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
    *out << cgicc::title("INFPGA1 Web Form") << std::endl;
    //
     *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    char buf[300],buf2[300] ;
    sprintf(buf,"DDU INFPGA1 VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
    
    for(int i=400;i<426;i++){
      *out << cgicc::table().set("border","0");
      *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->infpga_shift0=0x0000; 
      thisDDU->CAEN_err_reset();
      *out << cgicc::span().set("style","color:black");
      if(i==400){
           sprintf(buf,"infpga1 reset:");
           sprintf(buf2," ");
      }
      if(i==401){
	thisDDU->infpga_rdscaler(INFPGA1);
           sprintf(buf,"infpga1 L1 Event Scaler0 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==402){
	thisDDU->infpga_rd1scaler(INFPGA1);
           sprintf(buf,"infpga1 L1 Event Scaler1 bits{23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==403){
	thisDDU->infpga_lowstat(INFPGA1);
           sprintf(buf,"infpga1 16-low-bits  Status:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==404){
	thisDDU->infpga_histat(INFPGA1);
           sprintf(buf,"infpga1 16-high-bits Status:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==405){
	thisDDU->infpga_rd1scaler(INFPGA1);
           sprintf(buf,"infpga1 32-bit Status:");
           sprintf(buf2," %04X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
      }
      if(i==406){
	thisDDU->infpga_shfttst(INFPGA1,0);
	unsigned short int tmp;
	tmp=thisDDU->infpga_shift0;
	thisDDU->infpga_shfttst(INFPGA1,1);
           sprintf(buf,"infpga1 shift test:");
           sprintf(buf2," %04X %04X",tmp,thisDDU->infpga_shift0);
      }
      if(i==407){
	thisDDU->infpga_CheckFiber(INFPGA1);
           sprintf(buf,"infpga1 Check Fiber Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==408){
	thisDDU->infpga_DMBsync(INFPGA1);
           sprintf(buf,"infpga1 DMB Sync Checks [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==409){
	thisDDU->infpga_FIFOstatus(INFPGA1);
           sprintf(buf,"infpga1 DDU Input FIFO Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==410){
	thisDDU->infpga_FIFOfull(INFPGA1);
           sprintf(buf,"infpga1 DDU Input FIFO Full[15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==411){
	thisDDU->infpga_RxErr(INFPGA1);
           sprintf(buf,"infpga1 DDU Rx Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==412){
	thisDDU->infpga_Timeout(INFPGA1);
           sprintf(buf,"infpga1 DDU Timeout [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      } 
      if(i==413){
	thisDDU->infpga_XmitErr(INFPGA1);
           sprintf(buf,"infpga1 Xmit Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==414){
	thisDDU->infpga_LostErr(INFPGA1);
           sprintf(buf,"infpga1 DDU Lost Error [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==415){
	thisDDU->infpga_CcodeStat(INFPGA1);
           sprintf(buf,"infpga1 DDU C-code Status [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==416){
	thisDDU->infpga_StatA(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg A [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==417){
	thisDDU->infpga_StatB(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg B [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==418){
	thisDDU->infpga_StatC(INFPGA1);
           sprintf(buf,"infpga1 DDU Status Reg C [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==419){
	thisDDU->infpga_DMBwarn(INFPGA1);
           sprintf(buf,"infpga1 DMB Full/Warn [15-0]:");
           sprintf(buf2," %04X ",thisDDU->infpga_code0);
      }
      if(i==420){
	thisDDU->infpga_MemAvail(INFPGA1);
           sprintf(buf,"infpga1 FIFO Memory Available:");
           sprintf(buf2,"0- %04X 1- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==421){
	thisDDU->infpga_WrMemActive(INFPGA1,0);
           sprintf(buf,"infpga1 FIFO Memory Available:");
           sprintf(buf2," 8- %04X  9- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==422){
	thisDDU->infpga_WrMemActive(INFPGA1,1);
           sprintf(buf,"infpga1 FIFO Memory Available:");
           sprintf(buf2,"10- %04X 11- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==423){
	thisDDU->infpga_WrMemActive(INFPGA1,2);
           sprintf(buf,"infpga1 FIFO Memory Available:");
           sprintf(buf2,"12- %04X 13- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==424){
	thisDDU->infpga_WrMemActive(INFPGA1,3);
           sprintf(buf,"infpga1 FIFO Memory Available:");
           sprintf(buf2,"14- %04X 15- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      if(i==425){
	thisDDU->infpga_Min_Mem(INFPGA1);
           sprintf(buf,"infpga1 Minimum FIFO Memory Availabile:");
           sprintf(buf2,"0- %04X 1- %04X",thisDDU->infpga_code0,thisDDU->infpga_code1);
      }
      *out << buf << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else if((thisDDU->infpga_shift0!=0xFACE)&&(i!=406)){
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else{ 
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2;*out << cgicc::span();
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=406&&i!=400){
        sprintf(buf," shift:%04X",thisDDU->infpga_shift0);
        *out << buf;
      }
      if(i==400){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::input().set("type","submit")
	  .set("value","set");
         sprintf(buf,"%d",ddu);
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
         sprintf(buf,"%d",i);       
         *out << cgicc::input().set("type","hidden").set("value",buf).set("name","val"); 
         *out << cgicc::form() << std::endl ;
         *out << cgicc::td() << std::endl;
       }else{
         *out << std::endl;
         *out << cgicc::td() << std::endl ;
       }  
      *out << cgicc::table() << std::endl;
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    

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
    int ddu;
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
    sprintf(buf,"DDU PARALLEL VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
   
    for(int i=500;i<524;i++){
      *out << cgicc::table().set("border","0");
      *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->CAEN_err_reset();  
      *out << cgicc::span().set("style","color:black");
      if(i==500){
           sprintf(buf,"CSC Status:");
           sprintf(buf2," %04X ",thisDDU->vmepara_status());
      }
      if(i==501){
           sprintf(buf,"CSC Busy:");
           sprintf(buf2," %04X ",thisDDU->vmepara_busy());
      }
      if(i==502){
           sprintf(buf,"Warn/Near Full:");
           sprintf(buf2," %04X ",thisDDU->vmepara_fullwarn());
      }
      if(i==503){
           sprintf(buf,"CSC LostSync:");
           sprintf(buf2," %04X ",thisDDU->vmepara_lostsync());
      }
      if(i==504){
           sprintf(buf,"CSC Error:");
           sprintf(buf2," %04X ",thisDDU->vmepara_error());
      }
      if(i==505){
           sprintf(buf,"CSC FMM Status:");
           sprintf(buf2," %04X ",thisDDU->vmepara_CSCstat());
      }
      if(i==507){
           sprintf(buf,"InReg0:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg0());
      }
      if(i==508){
           sprintf(buf,"InReg1:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg1());
      }
      if(i==509){
           sprintf(buf,"InReg2:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_inreg2());
      }
      if(i==510){
           sprintf(buf,"Wr InReg:");
           sprintf(buf2," ");
      }
      if(i==511){
           sprintf(buf,"Switches:");
           sprintf(buf2," %02X ",thisDDU->vmepara_switch()&0xff);
      }
      if(i==512){
           sprintf(buf,"GbE Prescale:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_GbEprescale());
      }
      if(i==513){
           sprintf(buf,"Fake L1 Reg:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_fakel1reg());
      }
      if(i==514){
           sprintf(buf,"FMM Reg:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_fmmreg());
      }
      if(i==515){
           sprintf(buf,"Test Reg0:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==516){
           sprintf(buf,"Test Reg1:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==517){
           sprintf(buf,"Test Reg2:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==518){
           sprintf(buf,"Test Reg3:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==519){
           sprintf(buf,"Test Reg4:");
           sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==520){
           sprintf(buf,"Toggle DCC_wait:");
           sprintf(buf2," ");
      }
      if(i==521){
           sprintf(buf,"Toggle All FakeL1A:");
           sprintf(buf2," ");
      }
      if(i==522){
           sprintf(buf,"Warn History:");
           sprintf(buf2," %04X ",thisDDU->vmepara_warnhist());
      }
      if(i==523){
           sprintf(buf,"Busy History:");
           sprintf(buf2," %04X ",thisDDU->vmepara_busyhist());
      }
      *out << buf << cgicc::span();
      // if(idcode!=tidcode[i]){
      // *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      // }else{ 
      if(thisDDU->CAEN_err()!=0){
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
      }else{
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      // }
      *out << buf2;*out << cgicc::span();
      *out << cgicc::td() << std::endl;
      printf(" now boxes \n");
      if(i==510||i==512||i==513||i==514||i==520||i==521){
         *out << cgicc::td();
	 string xmltext="";
	 if(i==510)xmltext="ffff";
         if(i==512)xmltext="ffff";
         if(i==513)xmltext="ffff";
         if(i==514)xmltext="ffff";
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",ddutextload);
         if(i!=520&&i!=521){
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
         *out << cgicc::form() << std::endl ;
         *out << cgicc::td() << std::endl;
       }else{
         *out << cgicc::td() << std::endl;
         *out << std::endl;
         *out << cgicc::td() << std::endl ;
       }  
      *out << cgicc::table() << std::endl;
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;     
}



void EmuFCrateHyperDAQ::VMESERI(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
   
    printf(" enter VMESERI \n");
    cgicc::Cgicc cgi(in);
    //
    printf(" initialize env \n");
    //
    const CgiEnvironment& env = cgi.getEnvironment();
    //
    printf(" getQueryString \n");
    std::string crateStr = env.getQueryString() ;
    //
    cout << crateStr << endl ;

    printf(" VMESERI ifs \n");
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
    printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("VMESERI Web Form") << std::endl;
    //  
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300],buf2[300] ;
    sprintf(buf,"DDU SERIAL VME  Slot %d",thisDDU->slot());
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    //
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    //   
   
    for(int i=600;i<612;i++){
      *out << cgicc::table().set("border","0");
      *out << cgicc::td();
      printf(" LOOP: %d \n",i);
      thisDDU->CAEN_err_reset();
      *out << cgicc::span().set("style","color:black");
      if(i==600){
           thisDDU->read_status(); 
           sprintf(buf,"Status:");
           sprintf(buf2," %02X ",thisDDU->rcv_serial[0]&0xff);
      }
      if(i==601){
           thisDDU->read_page1(); 
           sprintf(buf,"Flash Killch:");
           sprintf(buf2," %02X%02X ",thisDDU->rcv_serial[0]&0xff,thisDDU->rcv_serial[1]&0xff);
      }
      unsigned short int code[3];
      if(i==602){
           thisDDU->read_page4();
           code[0]=((thisDDU->rcv_serial[1]&0xFF))|((thisDDU->rcv_serial[0]&0xFF)<<8);
           code[1]=((thisDDU->rcv_serial[3]&0xFF))|((thisDDU->rcv_serial[2]&0xFF)<<8);
 
           sprintf(buf,"Flash DDRThresh:");
           sprintf(buf2," %04X/%04X",code[1],code[2]);
      }
      if(i==603){
           thisDDU->read_page5(); 
           sprintf(buf,"Flash GBEThresh:"); 
           code[0]=(((thisDDU->rcv_serial[0]&0xC0)>>6)|((thisDDU->rcv_serial[3]&0xFF)<<2)|((thisDDU->rcv_serial[2]&0x3F)<<10));
           code[1]=(((thisDDU->rcv_serial[2]&0xC0)>>6)|((thisDDU->rcv_serial[5]&0xFF)<<2)|((thisDDU->rcv_serial[4]&0x3F)<<10));
           code[2]=((thisDDU->rcv_serial[4]&0xC0)>>6);
           sprintf(buf2,"%01X/%04X/%04X",code[2],code[1],code[0]);
      }
      if(i==604){
           sprintf(buf,"Flash DDUid:");
           sprintf(buf2," %d ",thisDDU->read_page7());
      }
      if(i==605){
           thisDDU->read_vmesd0(); 
           sprintf(buf,"DDR_FIFOThresh0:"); 
           code[0]=((thisDDU->rcv_serial[1]&0xFF))|((thisDDU->rcv_serial[0]&0xFF)<<8);
           code[1]=((thisDDU->rcv_serial[3]&0xFF))|((thisDDU->rcv_serial[2]&0xFF)<<8);
           sprintf(buf2,"%04X/%04X\n",code[1],code[0]);
      }
      if(i==606){
           thisDDU->read_vmesd1(); 
           sprintf(buf,"DDR_FIFOThresh1:"); 
           code[0]=((thisDDU->rcv_serial[1]&0xFF))|((thisDDU->rcv_serial[0]&0xFF)<<8);
           code[1]=((thisDDU->rcv_serial[3]&0xFF))|((thisDDU->rcv_serial[2]&0xFF)<<8);
           sprintf(buf2,"%04X/%04X\n",code[1],code[0]);
      }
      if(i==607){
           thisDDU->read_vmesd2(); 
           sprintf(buf,"DDR_FIFOThresh2:"); 
           code[0]=((thisDDU->rcv_serial[1]&0xFF))|((thisDDU->rcv_serial[0]&0xFF)<<8);
           code[1]=((thisDDU->rcv_serial[3]&0xFF))|((thisDDU->rcv_serial[2]&0xFF)<<8);
           sprintf(buf2,"%04X/%04X\n",code[1],code[0]);
      }
      if(i==608){
           thisDDU->read_vmesd3(); 
           sprintf(buf,"DDR_FIFOThresh3:"); 
           code[0]=((thisDDU->rcv_serial[1]&0xFF))|((thisDDU->rcv_serial[0]&0xFF)<<8);
           code[1]=((thisDDU->rcv_serial[3]&0xFF))|((thisDDU->rcv_serial[2]&0xFF)<<8);
           sprintf(buf2,"%04X/%04X\n",code[1],code[0]);
      }
      if(i==609){
	sprintf(buf,"FORCE DDR LOAD:");
        sprintf(buf2," ");   
      }
      if(i==610){
         sprintf(buf,"Voltages V15 V25 V25A:");
         sprintf(buf2,"%5.2fmV %5.2fmV %5.2fmV %5.2fmV",thisDDU->adcplus(1,4),thisDDU->adcplus(1,5),thisDDU->adcplus(1,6),thisDDU->adcplus(1,7));
      }
      if(i==611){
         sprintf(buf,"Temperatures:");
         sprintf(buf2,"%5.2fF %5.2fF %5.2fF %5.2fF ",thisDDU->readthermx(0),thisDDU->readthermx(1),thisDDU->readthermx(2),thisDDU->readthermx(3));
      }
     *out << buf << cgicc::span();
      // if(idcode!=tidcode[i]){
      // *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      // }else{ 
     if(thisDDU->CAEN_err()!=0){
      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
     }else{
      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
     }
      // }
      *out << buf2;*out << cgicc::span();
      *out << cgicc::td() << std::endl;
      printf(" now boxes \n");
      if(i==601||i==602||i==603||i==604||i==609){
         *out << cgicc::td();
	 string xmltext="";
	 if(i==601)xmltext="7fff";
         if(i==602)xmltext="001000ff";
         if(i==603)xmltext="6800200000ff";
         if(i==604)xmltext="255";
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",ddutextload);
         if(i!=609){
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
         *out << cgicc::form() << std::endl ;
         *out << cgicc::td() << std::endl;
       }else{
         *out << cgicc::td() << std::endl;
         *out << std::endl;
         *out << cgicc::td() << std::endl ;
       }  
      *out << cgicc::table() << std::endl;
     }  

    *out << cgicc::fieldset() << std::endl;
    *out << cgicc::body() << std::endl;    
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
    long int send_val;
    if(val==200)thisDDU->ddu_reset();
    if(val==208){
      sscanf(XMLtext.data(),"%01hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2]);
      send_val=((thisDDU->snd_serial[1]<<16)&0x00ff0000)|((thisDDU->snd_serial[1]<<8)&0x0000ff00)|(thisDDU->snd_serial[2]&0x0000ff);
       printf(" loadkillfiber send %ld \n",send_val);
       thisDDU->ddu_loadkillfiber(send_val);
   } 
   if(val==225){
      sscanf(XMLtext.data(),"%01hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8)&0x0000ff00)|(thisDDU->snd_serial[1]&0x0000ff);
       printf(" ldbxorbit send %d \n",para_val);
       thisDDU->ddu_loadbxorbit(para_val);
       } 
    if(val==234)thisDDU->ddu_vmel1a(); 
    if(val==300)thisDDU->infpga_reset(INFPGA0);
    if(val==400)thisDDU->infpga_reset(INFPGA1);
    if(val==510){
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
    if(val==513){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fakel1reg(para_val);

    }
    if(val==514){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fmmreg(para_val);
    }
    if(val==520){
        unsigned short int scale= thisDDU->vmepara_rd_GbEprescale();
        if (scale&0x0008) { 
           para_val=scale&(0xFFFF-0x0008);
        } else {
           para_val=scale|0x0008;
        }
	printf("          vmepara_wr_GbEprescale, para_val=%d \n",para_val);
	thisDDU->vmepara_wr_GbEprescale(para_val);
    } 
    if(val==521){
	unsigned short int scale2=thisDDU-> vmepara_rd_fakel1reg();
        if (scale2&0x0004) {
          para_val=0x0000;
        } else {
          para_val=0xFFFF;
        }
	printf("         vmepara_wr_fakel1reg, para_val=%d \n",para_val);
	//	thisDDU->vmepara_wr_fakel1reg(para_val);
    }  
    if(val==601){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      //  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->write_page1();
      // sleep(1);
    }
     if(val==602){
      sscanf(XMLtext.data(),"%02hhx%02hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2],&thisDDU->snd_serial[3]);
      para_val=((thisDDU->snd_serial[2]<<8))&0xff00|(thisDDU->snd_serial[3]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->write_page4();
      // sleep(1);
    }
     if(val==603){
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
    if(val==604){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      //  printf(" SSCANF Return: %02x %02x \n",thisDDU->snd_serial[0]&0xff,thisDDU->snd_serial[1]&0xff);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_inreg(para_val);
      thisDDU->write_page7();
      //  sleep(1);
    }
    if(val==609){
      thisDDU->write_vmesdF();
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
    *out << bgsound().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/siren.wav") << std::endl;  
    *out << embed().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/siren.wav").set("hidden","true").set("autostart","true").set("loop","true") << std::endl;
    */
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
	    .set("target","_blank") << std::endl;
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
          if(prom==2)thisDCC->epromload("INPROM",INPROM,"MySVFFile.svf",1,cbrdnum);
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
    printf(" enter: DCCCommands \n");
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
    *out << cgicc::title("DCCComands Web Form") << std::endl;
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
            //unsigned long int fifouse=thisDDU->mctrl_rd_fifoinuse();
           sprintf(buf,"Set FIFOs Used:");
	   // sprintf(buf2," %04x ",fifouse); 
           sprintf(buf2," ");
      }
      if(i==104){ 
           //unsigned long int ttccmd=thisDDU->mctrl_rd_ttccmd();
           sprintf(buf,"TTC Command:");
	   // sprintf(buf2," %04x ",ttccmd); 
           sprintf(buf2," ");
      }
      if(i==105){
           sprintf(buf,"Load L1A:");
           sprintf(buf2," ");
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
	 if(i==103)xmltext="ffff";
         if(i==104)xmltext="ffff";;
         if(i==105)xmltext="2,5";;
         if(i==103||i==104|i==105){
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
      cout << "VAL " << val << endl;
    } 
    //
    thisDCC = dccVector[dcc]; 
    //

    string XMLtext = cgi["textdata"]->getValue() ; 
	//
    cout << XMLtext  << endl ;
    unsigned short int para_val;
    char snd_serial[2];
    static int rate,num;
    char arate;
    char anum;
    if(val==101)thisDCC->mctrl_bxr();
    if(val==102)thisDCC->mctrl_evnr();
    if(val==103){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&snd_serial[0],&snd_serial[1]);
      para_val=((snd_serial[0]<<8)&0x0000ff00)|(snd_serial[1]&0x0000ff);
       printf(" loadkillfiber send %04x \n",para_val);
       thisDCC->mctrl_fifoinuse(para_val);
    } 
    if(val==104){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&snd_serial[0],&snd_serial[1]);
      para_val=((snd_serial[0]<<8)&0x0000ff00)|(snd_serial[1]&0x0000ff);
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
}




};
#endif
