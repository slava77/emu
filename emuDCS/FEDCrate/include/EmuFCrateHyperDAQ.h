
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
extern int irqprob;
extern long int timer,xtimer;

using namespace cgicc;
using namespace std;

class EmuFCrateHyperDAQ: public xdaq::Application 
{
private:
  int reload;
  //  int irqprob;
  //  long int timer,xtimer;
  long int ltime;
  //
protected:
  //
  xdata::String svfFile_;
  xdata::String xmlFile_;
  xdata::UnsignedLong myParameter_;
  TestBeamCrateController tbController;
  DDU* thisDDU;
  DCC* thisDCC;
  ostringstream CrateTestsOutput;
  ostringstream OutputStringDDUStatus[9];
  ostringstream OutputStringDCCStatus[9];
  vector<DDU*> dduVector;
  vector<DCC*> dccVector;
  Crate *thisCrate;
  std::string Operator_;
  std::string DDUBoardID_[9];
  std::string DCCBoardID_[9];
  int DCC_ratemon[50][12],DCC_ratemon_cnt,DCC_ratemon_ch;
  int DDU_, DCC_;
  //
public:
  EmuFCrateHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
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
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUtrapDecode,"DDUtrapDecode");
    xgi::bind(this,&EmuFCrateHyperDAQ::LoadXMLconf,"LoadXMLconf");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmware,"DCCFirmware");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCLoadFirmware,"DCCLoadFirmware"); 
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCFirmwareReset,"DCCFirmwareReset");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCCommands,"DCCCommands");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCTextLoad, "DCCTextLoad");
    xgi::bind(this,&EmuFCrateHyperDAQ::IRQTester,"IRQTester");
    xgi::bind(this,&EmuFCrateHyperDAQ::DDUVoltMon,"DDUVoltMon");
    xgi::bind(this,&EmuFCrateHyperDAQ::DCCRateMon,"DCCRateMon");
    xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate0,"getDataDCCRate0");
    xgi::bind(this,&EmuFCrateHyperDAQ::getDataDCCRate1,"getDataDCCRate1");
    myParameter_ =  0;
    //    irqprob=0;

    xmlFile_     = 
      "/home/fastdducaen/v3.4/TriDAS/emu/emuDCS/FEDCrate/xml/config.xml" ;
    Operator_ = "Name...";
    for (int i=0; i<9; i++) { DDUBoardID_[i] = "-1" ; DCCBoardID_[i] = "-1" ; }
    this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName",&xmlFile_);
    for(int i=0;i<12;i++){
      for(int j=0;j<50;j++){
	DCC_ratemon[j][i]=i+1;
      }
    }
    DCC_ratemon_cnt=0;
  }

  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {

/* JRG, Logger stuff to add:
    // if (getApplicationLogger().exists(getApplicationLogger().getName())) {
*/
    LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: server startup" << endl);
    std::string LoggerName = getApplicationLogger().getName() ;
    std::cout << "Name of Logger is " <<  LoggerName <<std::endl;

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web Form") << std::endl;
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");
    //
    *out << img().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/ddudcc.gif") << std::endl;
    *out << img() << std::endl;
    *out << cgicc::br();

//    int reload=0;

/* JRG, This cgicc line causes crash, but required to access XML tag data?
    cgicc::Cgicc cgi(in);

    cout << endl << "  JRGdebug: checking for Reload" << endl;
    cgicc::form_iterator name = cgi.getElement("Reload");
    if(name != cgi.getElements().end()) {
      reload = cgi["Reload"]->getIntegerValue();
    }else{
      reload=0;
    }
*/
//    cout << "  JRGdebug: Reload value = " << reload << endl;

    if (reload>0||(dduVector.size()==0 && dccVector.size()==0)) {
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
      *out << cgicc::input().set("type","submit").set("name","buttonid")
	.set("value","Normal Init & Config") << std::endl;
// JRG Note: buttonid="Init Only" is critical string, do not change
      *out << cgicc::input().set("type","submit").set("name","buttonid")
	.set("value","Init Only") << std::endl;
      *out << cgicc::form() << std::endl;

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

    } else if (dduVector.size()>0 || dccVector.size()>0) {
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

       	 std::string dccratemon =
	   toolbox::toString("/%s/DCCRateMon",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",dccratemon)
	   .set("target","_blank") << std::endl;
	 *out << cgicc::input().set("type","submit")
	   .set("value","Start DCC Rate Monitor") << std::endl;
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
            unsigned short int status=thisDDU->vmepara_CSCstat();
            unsigned short int DDU_FMM=((thisDDU->vmepara_status()>>8)&0x000F);
            int brdnum,iblink=0;
            // sleep(1);
            thisDDU->CAEN_err_reset();
            brdnum=thisDDU->read_page7();
            sprintf(buf,"Board: %d ",brdnum);
            *out << buf;

            if(DDU_FMM==4){    // Busy
	      *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
	      irqprob=1;
            }
            else if(DDU_FMM==1){    // Warn, near full: reduce trigger rate
	      *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
	      iblink=1;
            }
            else if(DDU_FMM==8){    // Ready
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }
            else if(DDU_FMM==2 || DDU_FMM==0xC){     // Error or Sync
	      *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	      irqprob=1;
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
          *out << cgicc::table() << std::endl;
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
	    //
	    std::string status;
	    /*
            if(thisDCC->CAEN_err()!=0){
	      *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");              
            }else if(statush==0x0000&&statusl==0x0000){
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }else{
	      *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
            }
	    */
	    if((statush&0xc000)!=0x0)
	      {
		*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
		status ="(Out of Sync)";
	      } else {
		*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
		status ="(OK)";
	      }
	    //
	    /*
	    // New DCC firmware
	    //
	    std::cout << "status ............. " << std::hex << (statush&0xf000) << std::endl;
	    //
	    //
	    if((statush&0xf000)==0x2000)
	    {
	    *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
	    status ="(Ready)";
	    } else if((statush&0xf000)==0x4000)
	    {
	    *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");              
	    status ="(Warning)";
	    } else if((statush&0xf000)==0x8000 || (statush&0xf000)==0x3000 )
	    {
	    *out << cgicc::span().set("style","color:red;background-color:#dddddd;");              
	    status ="(Out of sync or error)";
	    } else if((statush&0xf000)==0x1000)
	    {
	    *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");              
	    status ="(Busy)";
	    } else 
	    {
	    *out << cgicc::span().set("style","color:black;background-color:#dddddd;");              
	    status ="(CAEN error)";
	    }
	    */
	    //
            sprintf(buf,"Status H:%04X L:%04X ",statush,statusl);
            *out << buf << " " << status ; 
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
          *out << cgicc::table() << std::endl;
        }
        *out << cgicc::fieldset() << std::endl;

// JRG, inside end of "normal" DDU/DCC Control top-level page
//   --add "Go to XML Reload" button here for Martin:
	*out << "<blockquote> &nbsp; &nbsp; </blockquote>" << std::endl;
	*out << "<P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp; <P> &nbsp;" << std::endl;
	*out << hr();
    std::string loadxmlconf =
      toolbox::toString("/%s/LoadXMLconf",getApplicationDescriptor()->getURN().c_str());
    std::string defurl =
      toolbox::toString("/%s/",getApplicationDescriptor()->getURN().c_str());
    //    cout<<endl<<"  JRGdebug: loadxmlconf string="<<loadxmlconf<<endl;
    *out << cgicc::form().set("method","POST").set("action",loadxmlconf) << std::endl;
    *out << "<center> <font color=red>  EXPERT ONLY! </font>" << std::endl;
    *out << "Reload " << std::endl;
    //    *out << "Reload XML Config: " << std::endl;
//  JRG, for now Reload value below not used
    *out << cgicc::input().set("type","hidden")
      .set("name","Reload")
      .set("value","1");
    *out << std::endl;
    *out << cgicc::input().set("type","submit").set("value","XMLconfig") << std::endl;
    *out << "<font color=red>  EXPERT ONLY! </font> </center>" << cgicc::form() << std::endl;


    }

    reload=0;
    *out << cgicc::body() << std::endl; 
    *out << cgicc::html() << std::endl;    
}

  
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

  void EmuFCrateHyperDAQ::Configuring(int SkipConfig){
/*
    cgicc::Cgicc cgi(in);
    const CgiEnvironment& env = cgi.getEnvironment();
    std::string crateStr = env.getQueryString() ;
    //    cout << crateStr << endl ;
*/

    std::cout << "Configuring: SkipConfig=" << SkipConfig << std::endl ;
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
    dduVector = selector.ddus(crateVector[0]);
    dccVector = selector.dccs(crateVector[0]);
    thisCrate = crateVector[0];
    timer=0;
    xtimer=0;
    irqprob=0;

// LSD, Make these optional with buttons
// JRG, only start/reset the IRQ handler:
    if(SkipConfig>0)thisCrate->init(0);

// JRG, download setup to all boards, then start/reset the IRQ handler:
     if(SkipConfig==0)thisCrate->configure(0);
 
    std::cout << "EmuFCrateHyperDAQ::Configuring  >done< " << std::endl ;
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

	int SkipConfig=0;
	std::string Bid;
	SkipConfig=0;
	cgicc::form_iterator name = cgi.getElement("buttonid");
	if(name != cgi.getElements().end()) {
	  Bid = cgi["buttonid"]->getValue();
	  if(Bid=="Init Only")SkipConfig=1;
	}
	std::cout << "setConfFile: ButtonID=" << Bid << ".   SkipConfig=" << SkipConfig << std::endl ;

	const_file_iterator file;
	file = cgi.getFile("xmlFileName");
	cout << "GetFiles string" << endl ;
	if(file != cgi.getFiles().end()) (*file).writeToStream(cout);
	string XMLname = cgi["xmlFileName"]->getValue() ; 
	cout << XMLname  << endl ;
	xmlFile_ = XMLname ;
	Configuring(SkipConfig);
	this->Default(in,out);
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
	cout << "UploadConfFileUpload" << endl ;
	cgicc::Cgicc cgi(in);
	const_file_iterator file;
	file = cgi.getFile("xmlFileNameUpload");
	cout << "GetFiles" << endl ;
	if(file != cgi.getFiles().end()) {
	  ofstream TextFile ;
	  TextFile.open("MyTextConfigurationFile.xml");
	  (*file).writeToStream(TextFile);
	  TextFile.close();
	}
	xmlFile_ = "MyTextConfigurationFile.xml" ;
	Configuring(0);
	cout << "UploadConfFile done" << endl ;
	this->Default(in,out);
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
    unsigned long int tuscode[8]={0xcf038a04,0xdf023a01,0xdf023a01,0xb0017a01,0xc038dd99,0xc138dd99,0xd0023a01,0xd1023a01};

    printf(" entered DDUFirmware \n");
    cgicc::Cgicc cgi(in);
    printf(" initialize env \n");
    const CgiEnvironment& env = cgi.getEnvironment();
    printf(" getQueryString \n");
    std::string crateStr = env.getQueryString() ;
    cout << crateStr << endl ; 

    cgicc::form_iterator name = cgi.getElement("ddu");
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

    string xmltext="";
    for(int i=0;i<8;i++){ 
      xmltext="data/ddu_config/";
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
	  xmltext="http://www.physics.ohio-state.edu/~cms/firmwares/ddu5ctrl_0.svf";
	}
	if(i==5){
	  sprintf(buf,"dduprom1 ");
	  j=7;
	  xmltext="http://www.physics.ohio-state.edu/~cms/firmwares/ddu5ctrl_1.svf";
	}

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
      if(i>=3&&i<8){
	std::string dduloadfirmware =
	  toolbox::toString("/%s/DDULoadFirmware",getApplicationDescriptor()->getURN().c_str());
	//

/*  JRG, try to put a default value in text field--no good
	*out << cgicc::form()
	  .set("method","POST")
	  .set("action",dduloadfirmware)
	  .set("name","textdata")
	  .set("size","10")
	  .set("enctype","multipart/form-data")
	  .set("value",xmltext) << std::endl;
	*out << cgicc::input().set("type","file")
	  .set("name","DDULoadSVF")
	  .set("size","50") << std::endl;
*/
/*  Orig: */
	*out << cgicc::form().set("method","POST")
	  .set("enctype","multipart/form-data")
//	  .set("value",xmltext)
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
	  printf("ready to download PROM %d \n",prom);
// JRG, Need to debug Broadcast case	  sleep(59);
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
    const CgiEnvironment& env = cgi.getEnvironment();
    std::string crateStr = env.getQueryString() ;
    cout << crateStr << endl ;
    cgicc::form_iterator name = cgi.getElement("ddu");
    int j,ddu,icrit,icond,icond2,icond3,icond4;
    unsigned long int stat=0,live_fiber=0;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }
    thisDDU = dduVector[ddu];
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DDUFPGA Web Form") << std::endl;
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");
    // << std::endl;

    char buf[300],buf2[300],buf3[300],buf4[200];
    char buf5[300],buf6[10],buf7[300],buf8[10];
    unsigned long int ndmb,nalct,ntmb,ncfeb;
    sprintf(buf,"DDU Control FPGA, VME  Slot %d",thisDDU->slot());
    //    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;") << std::endl;
    //    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
    *out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << std::endl;
    for(int i=200;i<230;i++){
      thisDDU->infpga_shift0=0x0000;
      thisDDU->CAEN_err_reset();
      sprintf(buf3," ");
      sprintf(buf4," ");
      sprintf(buf5," ");
      sprintf(buf6," ");
      sprintf(buf7," ");
      sprintf(buf8," ");
      icond=0;
      icond2=0;
      icond3=0;
      icond4=0;
      printf(" LOOP: %d \n",i);
      thisDDU->ddu_shift0=0x0000;
      if(i==200){
	thisDDU->ddu_rd_boardID();
	sprintf(buf,"16-bit DDU Board ID:"); 
	sprintf(buf2," %04Xh ",thisDDU->ddu_code0);
      }
      if(i==201){
	thisDDU->ddu_fpgastat();
	sprintf(buf,"32-bit DDU Control FPGA Status:");
	stat=((0xffff&thisDDU->ddu_code1)<<16)|(0xffff&thisDDU->ddu_code0);
	//	stat=0xffffffff;
	sprintf(buf2," %08lX ",stat);
	icrit=0;
	if((0x00008000&stat)){
	  icrit=1;
	  icond=2;
	}
	else if((0xDE4F4BFF&stat)>0)icond=1;
      }
      if(i==202){
	thisDDU->ddu_rdostat();
	sprintf(buf,"16-bit DDU Output Status:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
	stat=0xffff&thisDDU->ddu_code0;
	//	stat=0xffff;
	if((0x00000080&stat)){
	  icond=2;
	}
	else if((0x00004000&stat)>0)icond=1;
      }
      if(i==203){
	thisDDU->ddu_rdscaler();
	sprintf(buf,"DDU L1 Event Scaler, bits [23-0]:");
	sprintf(buf2," %02X%04Xh ",thisDDU->ddu_code1,thisDDU->ddu_code0);
	stat=((thisDDU->ddu_code1)<<16)+thisDDU->ddu_code0;
	sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
      }
      if(i==204){
	thisDDU->ddu_rderareg();
	sprintf(buf,"Error bus A Register bits [15-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
	stat=0xffff&thisDDU->ddu_code0;
	//	stat=0xffff;
	if((0x0000C00C&stat)){
	  icond=2;
	}
	else if((0x000001C8&stat)>0)icond=1;
      }
      if(i==205){
	thisDDU->ddu_rderbreg();
	sprintf(buf,"Error bus B Register bits [15-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
	stat=0xffff&thisDDU->ddu_code0;
	//	stat=0xffff;
	if((0x00000011&stat)){
	  icond=2;
	}
	else if((0x0000D08E&stat)>0)icond=1;
      }
      if(i==206){
	thisDDU->ddu_rdercreg();
	sprintf(buf,"Error bus C Register bits [15-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
	stat=0xffff&thisDDU->ddu_code0;
	//	stat=0xffff;
	// if((0x00008000&stat)>0)icond=1;
	if((0x0000bfbf&stat)>0)icond=1;
      }
      if(i==207){
	*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition</font>" << br() << std::endl;
	thisDDU->ddu_pdmblive();
	sprintf(buf,"First Event DMBLIVE, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
	live_fiber=0x7fff&thisDDU->ddu_code0;
      }
      if(i==208){
	thisDDU->ddu_dmblive();
	sprintf(buf,"Latest Event DMBLIVE, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==209){
	thisDDU->ddu_rdcrcerr();
	sprintf(buf,"CRC Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==210){
	thisDDU->ddu_rdxmiterr();
	sprintf(buf,"Data Transmit Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==211){
	thisDDU->ddu_rddmberr();
	sprintf(buf,"DMB Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==212){
	thisDDU->ddu_rdtmberr();
	sprintf(buf,"TMB Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==213){
	thisDDU->ddu_rdalcterr();
	sprintf(buf,"ALCT Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==214){
	thisDDU->ddu_rdlieerr();
	sprintf(buf,"Lost-In-Event Error, Fiber[14-0]:");
	sprintf(buf2," %04X ",thisDDU->ddu_code0);
      }
      if(i==215){
	*out << br() << " <font color=blue> 4-bit Registers below flag which InRD unit experienced each condition, plus" << br() << " &nbsp &nbsp some individual bit Decoding</font>" << br() << std::endl;
	thisDDU->ddu_checkFIFOa();
	// sprintf(buf,"FIFO-A Status [15-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"Stuck Data Error occurred [3-0]:");
	sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
	if(0xF000&thisDDU->ddu_code0)icond=2;
	sprintf(buf3," &nbsp &nbsp Fiber or FIFO Connection Error occurred [3-0]:");
	sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
	if(0x0F00&thisDDU->ddu_code0)icond2=2;
	sprintf(buf5,"L1A Mismatch occurred [3-0]:");
	sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
	if(0x00F0&thisDDU->ddu_code0)icond3=1;
	sprintf(buf7," &nbsp &nbsp InRD with active fiber [3-0]:");
	sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
      }
      if(i==216){
	thisDDU->ddu_checkFIFOb();
	// sprintf(buf,"FIFO-B Status [15-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"Active Ext.FIFO Empty [3-0]:");
	sprintf(buf2," %01Xh ",(0x3C00&thisDDU->ddu_code0)>>10);
	sprintf(buf3," &nbsp &nbsp InRD Near Full Warn occurred [3-0]:");
	sprintf(buf4," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
	if(0x00F0&thisDDU->ddu_code0)icond2=3;
	sprintf(buf5,"Ext.FIFO Almost-Full occurred [3-0]:");
	sprintf(buf6," %01Xh ",(0x000F&thisDDU->ddu_code0));
	if(0x000F&thisDDU->ddu_code0)icond3=3;
	sprintf(buf7," &nbsp &nbsp special decode bits:");
	sprintf(buf8," %02Xh ",(0x4300&thisDDU->ddu_code0)>>8);
	stat=(0xC300&thisDDU->ddu_code0)>>8;
	if(0x0081&stat)icond4=1;
	//	stat=0x00ff;
      }
      if(i==217){
	thisDDU->ddu_checkFIFOc();
	// sprintf(buf,"FIFO-C Status [15-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"Timeout-EndBusy occurred [3-0]:");
	sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
	if(0xF000&thisDDU->ddu_code0)icond=2;
	sprintf(buf3," &nbsp &nbsp Timeout-EndWait occurred [3-0]:");
	sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
	if(0x0F00&thisDDU->ddu_code0)icond2=2;
	sprintf(buf5,"Timeout-Start occurred [3-0]:");
	sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
	if(0x00F0&thisDDU->ddu_code0)icond3=2;
	sprintf(buf7," &nbsp &nbsp Lost In Data Error occurred [3-0]:");
	sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
	if(0x000F&thisDDU->ddu_code0)icond4=2;
      }
      if(i==218){
	thisDDU->ddu_rdfferr();
	// sprintf(buf,"FIFO Full Register bits [14-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"Raw Ext.FIFO Empty [3-0]:");
	sprintf(buf2," %01Xh ",(0x3C00&thisDDU->ddu_code0)>>10);
	sprintf(buf3," &nbsp &nbsp InRD FIFO Full occurred [3-0]:");
	sprintf(buf4," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
	if(0x00F0&thisDDU->ddu_code0)icond2=2;
	sprintf(buf5,"Ext.FIFO Full occurred [3-0]:");
	sprintf(buf6," %01Xh ",(0x000F&thisDDU->ddu_code0));
	if(0x000F&thisDDU->ddu_code0)icond3=2;
	sprintf(buf7," &nbsp &nbsp special decode bits:");
	sprintf(buf8," %02Xh ",(0x4300&thisDDU->ddu_code0)>>8);
	stat=(0x4300&thisDDU->ddu_code0)>>8;
	//	stat=0x00ff;
	if(0x0001&stat)icond4=2;
      }
      if(i==219){
	thisDDU->ddu_InRDstat();
	// sprintf(buf,"InRDctrl Status [15-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"InRD Hard Error occurred [3-0]:");
	sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
	if(0xF000&thisDDU->ddu_code0)icond=2;
	sprintf(buf3," &nbsp &nbsp InRD Sync Error occurred [3-0]:");
	sprintf(buf4," %01Xh ",(0x0F00&thisDDU->ddu_code0)>>8);
	if(0x0F00&thisDDU->ddu_code0)icond2=2;
	sprintf(buf5,"InRD Single Event Error occurred [3-0]:");
	sprintf(buf6," %01Xh ",(0x00F0&thisDDU->ddu_code0)>>4);
	sprintf(buf7," &nbsp &nbsp InRD Timeout Error occurred [3-0]:");
	sprintf(buf8," %01Xh ",(0x000F&thisDDU->ddu_code0));
	if(0x000F&thisDDU->ddu_code0)icond4=2;
      }
      if(i==220){
	thisDDU->ddu_InC_Hist();
	// sprintf(buf,"InRDctrl MxmitErr Reg [15-12] & C-code History [8-0]:");
	// sprintf(buf2," %04X ",thisDDU->ddu_code0);
	sprintf(buf,"InRD Mutliple Transmit Errors occurred [3-0]:");
	sprintf(buf2," %01Xh ",(0xF000&thisDDU->ddu_code0)>>12);
	if(0xF000&thisDDU->ddu_code0)icond=2;

	sprintf(buf3," &nbsp &nbsp special decode bits:");
	sprintf(buf4," %03Xh ",(0x0FFF&thisDDU->ddu_code0));
	stat=(0x0FFF&thisDDU->ddu_code0);
	//	stat=0x0fff;
	if(0x0C00&stat)icond2=1;
	if(0x02DF&stat)icond2=2;
      }
      if(i==221){
	*out << br() << std::endl;
	thisDDU->ddu_rd_WarnMon();
	sprintf(buf,"8-bit DDU Near Full Warning, current:");
	sprintf(buf2," %02Xh ",(0x00FF&thisDDU->ddu_code0));
	if(0x00FF&thisDDU->ddu_code0)icond=3;
	sprintf(buf3," &nbsp &nbsp historical:");
	sprintf(buf4," %02Xh ",(0xFF00&thisDDU->ddu_code0)>>8);
	if(0xFF00&thisDDU->ddu_code0)icond2=3;
      }
      if(i==222){
	*out << br() << std::endl;
	thisDDU->ddu_rdkillfiber();
	sprintf(buf,"KillFiber Register bits [19-0] (0=dead, 1=alive):");
	sprintf(buf2," %01X%04X ",thisDDU->ddu_code1,thisDDU->ddu_code0);
	stat=((0x000f&thisDDU->ddu_code1)<<16)|(0xFFFF&thisDDU->ddu_code0);
      }
      if(i==223){
	thisDDU->ddu_rdbxorbit();
	sprintf(buf,"BX_Orbit Register bits [11-0]:");
	sprintf(buf2," %03Xh ",thisDDU->ddu_code0);
      }
      if(i==224){
	if(icrit>0){
	  thisDDU->ddu_fpgatrap();
	  *out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << std::endl;
	  sprintf(buf,"DDU Control diagnostic trap:");
	  sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->ddu_shift0=0xFACE;
	}
      }
      if(i==225){
	if(icrit>=0){
	  thisDDU->ddu_maxTimeCount();
//	  sprintf(buf,"DDU Control max time counted, EndTimeout <font size=-1>(1 unit = 6.4 usec)</font>:");
//	  sprintf(buf2," %02Xh ",(0xff00&thisDDU->ddu_code0)>>8);
	  sprintf(buf,"DDU Control max process time, L1A-to-Start <font size=-1>(1 unit = 400 ns)</font>:");
	  sprintf(buf2," %02Xh ",(0x00ff&thisDDU->ddu_code0));
//	  sprintf(buf3," &nbsp &nbsp StartTimeout <font size=-1>(1 unit = 400 ns)</font>:");
//	  sprintf(buf4," %02Xh ",(0x00ff&thisDDU->ddu_code0));
	  sprintf(buf3," &nbsp &nbsp Start-to-End <font size=-1>(1 unit = 6.4 usec)</font>:");
	  sprintf(buf4," %02Xh ",(0xff00&thisDDU->ddu_code0)>>8);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->ddu_shift0=0xFACE;
	}
      }
      if(i==226){
	sprintf(buf,"Generate one Fake DDU L1A via VME:"); 
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
	*out << hr();
      }
      if(i==227){
	sprintf(buf,"Toggle DDU 'CFEB Calib-Pulse==L1A' feature (default False): ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
// JRG, do better later: move to VMEpara and use bit3 of Fake L1A Reg.
//   --> No...Can't do that because no free lines from VMEfpga to DDUfpga.
//  add Reg Read & Print on page some other way?  Make F31 shiftable?
//    sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",0x0008&thisDDU->vmepara_rd_fakel1reg());
      }
      if(i==228){
	sprintf(buf,"DDU Sync Reset via VME: ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
      }
      if(i==229){
	int err=0;
	sprintf(buf3," ");
	sprintf(buf4," ");
	*out << br() << " <font color=blue size=+1> CSC Board Occupancies </font>" << br() << std::endl;
	//	*out << cgicc::table().set("border","0").set("rules","none").set("frame","void"); 
	*out << cgicc::table().set("align","center").set("width","740").set("cellpadding","5%").set("border","3").set("rules","all").set("frame","border");
       	*out << cgicc::colgroup().set("align","center");
       	*out << cgicc::col().set("span","2").set("align","center").set("width","20");
       	*out << cgicc::col().set("span","4").set("align","center");
	*out << cgicc::thead() << std::endl;
	*out << cgicc::tr() << std::endl;
	*out << cgicc::th().set("colspan","2") << " DDU " << cgicc::th() << cgicc::th().set("colspan","4") << " Board Occupancy " << cgicc::th() << cgicc::tr();
	*out << cgicc::tr() << std::endl;
	*out << cgicc::th() << "Input" << cgicc::th();
	*out << cgicc::th() << "Reg.#" << cgicc::th();
	*out << cgicc::th().set("width","150") << "DMB" << cgicc::th();
	*out << cgicc::th().set("width","150") << "ALCT" << cgicc::th();
	*out << cgicc::th().set("width","150") << "TMB" << cgicc::th();
	*out << cgicc::th().set("width","150") << "CFEB" << cgicc::th() << cgicc::tr() << std::endl;
	*out << cgicc::thead() << std::endl;
	*out << cgicc::tbody() << std::endl;
	for(j=0;j<15;j++){
	  thisDDU->ddu_rdscaler();
	  stat=((thisDDU->ddu_code1)<<16)+thisDDU->ddu_code0;
	  thisDDU->ddu_occmon();
	  ndmb=0x0fffffff&thisDDU->fpga_lcode[0];
	  nalct=0x0fffffff&thisDDU->fpga_lcode[1];
	  ntmb=0x0fffffff&thisDDU->fpga_lcode[2];
	  ncfeb=0x0fffffff&thisDDU->fpga_lcode[3];
	  sprintf(buf,"%d </td> <td> %ld ",j,0x0000000f&(thisDDU->fpga_lcode[0]>>28));
// For CSCs with data, for each board print #events & percent vs nDMB;
//   for DMB print percent vs # L1A.  Useful to detect hot/dead CSCs?
//   Make RED if no DMBs seen from a LiveFiber.  May need %4.2f for ME1/3...
	  while(stat<ndmb){
	    stat+=0x01000000;
	  }
	  if(ndmb>0&&stat>0)sprintf(buf2," %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% </td><td align=\"center\"> %ld &nbsp; &nbsp; %3.1f%% ",ndmb,100.0*ndmb/stat,nalct,100.0*nalct/ndmb,ntmb,100.0*ntmb/ndmb,ncfeb,100.0*ncfeb/ndmb);
	  else sprintf(buf2," %ld </td><td> %ld </td><td> %ld </td><td> %ld",ndmb,nalct,ntmb,ncfeb);
	  if((ndmb==0&&(live_fiber&(0x0001<<j))==0)||(ndmb>0&&(live_fiber&(0x0001<<j))))
	  *out << cgicc::tr() << cgicc::td() << buf << cgicc::td() << cgicc::td() << "<font color=green>" << buf2 << "</font>";
	  else *out << cgicc::tr() << cgicc::td() << buf << cgicc::td() << cgicc::td() << "<font color=red>" << buf2 << "</font>";
	  if((0xf0000000&thisDDU->fpga_lcode[0])!=(0xf0000000&thisDDU->fpga_lcode[3])){
	    err++;
	    *out << "<font=red> * </font>";
	    sprintf(buf3," &nbsp * %d End error, last was DDU input = %d read as %ld",err,j,0x0000000f&(thisDDU->fpga_lcode[3]>>28));
	  }
	  if(thisDDU->ddu_shift0!=0xFACE){
	    *out << "<font=orange> * </font>";
	    sprintf(buf4," &nbsp ** JTAG Error in i=%d, DDU input #%d: Shifted %04X",i,j,thisDDU->ddu_shift0);
	  }
	  *out << cgicc::td() << cgicc::tr() << std::endl;
	}
	*out << cgicc::tbody() << std::endl;
	*out << cgicc::table() << std::endl;
	*out << "&nbsp; &nbsp; <font size=-1> DMB percentage is relative to # L1As; other board percentages are relative to # LCTxL1A hits on the CSC.</font>" << std::endl;
	if(err>0){
	  *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
	  *out << buf3 << cgicc::span();
	}
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
	*out << buf4 << cgicc::span();
	sprintf(buf," ");
	sprintf(buf2," ");
	sprintf(buf3," ");
	sprintf(buf4," ");
      }

      if(i==228||i==227||i==222||i==223||i==226){
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
      }else if(icond==3){
        *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{
        *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2 << cgicc::span();
      *out << cgicc::span().set("style","color:black");
      *out << buf3 << cgicc::span();
      if(icond2==1){
        *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond2==2){
        *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else if(icond2==3){
        *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{
        *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf4 << cgicc::span();

      if(thisDDU->CAEN_err()!=0){
        *out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
        *out << " **CAEN Error " << cgicc::span();
      }
      if((thisDDU->ddu_shift0!=0xFACE)&&i<226){
        sprintf(buf," **JTAG Error in i=%d, Shifted:%04X",i,thisDDU->ddu_shift0);
        *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
        *out << buf << cgicc::span();
      }

      if(i>=215&&i<=219) *out << br();
      *out << cgicc::span().set("style","color:black");
      *out << buf5 << cgicc::span();
      if(icond3==1){
        *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond3==2){
        *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else if(icond3==3){
        *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{
        *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf6 << cgicc::span();
      *out << cgicc::span().set("style","color:black");
      *out << buf7 << cgicc::span();
      if(icond4==1){
        *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond4==2){
        *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else{
        *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf8 << cgicc::span();


      if(i==228||i==227||i==222||i==223||i==226){
	string xmltext="";
	if(i==222)xmltext="f7fff";
	if(i==223)xmltext="deb";
	if(i==228||i==227||i==226){
	  *out << cgicc::input().set("type","hidden")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value",xmltext)
	   .set("style","font-size: 13pt; font-family: arial;") << std::endl;
	}
	else {
	  *out << cgicc::input().set("type","text")
	   .set("name","textdata")
	   .set("size","10")
	   .set("ENCTYPE","multipart/form-data")
	   .set("value",xmltext)
	   .set("style","font-size: 13pt; font-family: arial;") << std::endl;
	}
	*out << cgicc::input().set("type","submit")
	  .set("value","set");
	sprintf(buf,"%d",ddu);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ddu"); 
	sprintf(buf,"%d",i);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","val");

	if(i==222){
	  //	  *out << " &nbsp &nbsp bits [14-0]==DDU Fiber Readout Enable";
	  *out << cgicc::form();
	  *out << std::endl;
	  *out << "<blockquote><font size=-1 face=arial>";
	  *out << "key: &nbsp b15==0 forces all DDU Checks Enabled, &nbsp b[14-0]==DDU Fiber Readout Enable (high True)" << br();
	  *out << "DDU Check Enable bits (high True): &nbsp b19==Normal DMB Checks (zero to enable only SP/TF checks)," << br() << " &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp &nbsp b18==CFEB Checks (zero disables DAV checks),  &nbsp b17==TMB Checks,  &nbsp b16==ALCT Checks" << br();
	  if((stat&0x00018000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp ALCT checking is disabled</font>";
	  if((stat&0x00028000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp TMB checking is disabled</font>";
	  if((stat&0x00008000)>0&&(stat&0x00030000)<0x00030000) *out << br() ;
	  if((stat&0x00048000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp CFEB DAV/LCT/MOVLP/L1A checks disabled</font>";
	  if((stat&0x00088000)==0x8000) *out << "<font color=blue> &nbsp &nbsp &nbsp Some DMB checks disabled for SP/TF compatibility</font>";
	  if((stat&0x00008000)==0) *out << "<font color=green> &nbsp &nbsp &nbsp All checks are Enabled</font>";
	  *out << "</font></blockquote>" << std::endl;
	}
	else *out << cgicc::form() << std::endl;

      }else if(i==201&&(stat&0x0000f000)>0){
        *out << "<blockquote><font size=-1 color=red face=arial>";
	if((stat&0xF0000000)>0){
	  if((0x80000000&stat)>0) *out << " DMB LCT/DAV/MOVLP Mismatch &nbsp ";
	  if((0x40000000&stat)>0) *out << " CFEB L1A Mismatch &nbsp ";
	  if((0x20000000&stat)>0) *out << " <font color=blue>DDUsawNoGoodDMB-CRCs</font> &nbsp ";
	  if((0x10000000&stat)>0) *out << " CFEB Count Mismatch";
	  if(0x0fffffff&stat) *out << br();
	}
	if((stat&0x0F000000)>0){
	  if((0x08000000&stat)>0) *out << " FirstDat Error &nbsp ";
	  if((0x04000000&stat)>0) *out << " L1A-FIFO Full occurred &nbsp ";
	  if((0x02000000&stat)>0) *out << " Data Stuck in FIFO occurred &nbsp ";
	  if((0x01000000&stat)>0) *out << " <font color=blue>NoLiveFiber warning</font>";
	  if(0x00ffffff&stat) *out << br();
	}
	if((stat&0x00F00000)>0){
	  if((0x00800000&stat)>0) *out << " <font color=blue>Special-word voted-bit warning</font> &nbsp ";
	  if((0x00400000&stat)>0) *out << " InRDctrl Error &nbsp ";
	  if((0x00200000&stat)>0) *out << " <font color=blue>DAQ Stop bit set</font> &nbsp ";
	  if((0x00100000&stat)>0) *out << " <font color=blue>DAQ says Not Ready</font>";
	  if((0x00300000&stat)==0x00200000) *out << " <font color=blue>DAQ Applied Backpressure</font>";
	  if(0x000fffff&stat) *out << br();
	}
	if((stat&0x000F0000)>0){
	  *out << "<font color=orange>";
	  if((0x00080000&stat)>0) *out << " TMB Error &nbsp ";
	  if((0x00040000&stat)>0) *out << " ALCT Error &nbsp ";
	  if((0x00020000&stat)>0) *out << " Trigger Wordcount Error &nbsp ";
	  if((0x00010000&stat)>0) *out << " Trigger L1A Match Error";
	  if(0x0000ffff&stat) *out << br();
	  *out << "</font>";
	}
// JRG, low-order 16-bit status (most serious errors):
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " <font color=red>Critical Error ** needs reset **</font> &nbsp ";
	  if((0x00004000&stat)>0) *out << " <font color=orange>Single Error, bad event</font> &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
	  if(0x00000fff&stat) *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>64-bit Alignment Error</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>DDU Control DLL Error (recent)</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=orange>DMB Error in event</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=orange>Lost In Event Error</font>";
	  if(0x000000ff&stat) *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " Lost In Data Error occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " Timeout Error occurred &nbsp ";
	  if((0x00000020&stat)>0) *out << " <font color=orange>Trigger CRC Error</font> &nbsp ";
	  if((0x00000010&stat)>0) *out << " Multiple Transmit Errors occurred";
	  if(0x0000000f&stat) *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Lost Sync occurred (FIFO Full or L1A Error) &nbsp ";
	  if((0x00000004&stat)>0) *out << " Fiber/FIFO Connection Error occurred &nbsp ";
	  if((0x00000002&stat)>0) *out << " <font color=orange>Single L1A Mismatch</font> &nbsp ";
	  if((0x00000001&stat)>0) *out << " <font color=orange>DMB or CFEB CRC Error</font>";
	}
        *out << "</font></blockquote>";

      }else if(i==202&&(stat&0x0000D981)>0){
	//        *out << "<blockquote><font size=-1 color=black face=arial>";
        *out << "<font size=-1 color=black face=arial>";
	if(0x00001000&stat) *out << " &nbsp &nbsp <font color=green>DDU S-Link Not Present</font>";
	if(0x00000100&stat) *out << " &nbsp &nbsp <font color=blue>SPY/GbE Fiber Disconnected</font>" << std::endl;
	else if((0x00000200&stat)>0) *out << " &nbsp  &nbsp SPY/GbE FIFO Always Empty";
	if((stat&0x0000ECEF)>0){
	  *out << "<blockquote>";
	  if((stat&0x0000E000)>0){
	    if((0x00008000&stat)>0) *out << " DDU Output Limited Buffer Overflow occurred &nbsp ";
	    if((0x00004000&stat)>0) *out << " <font color=blue>DAQ (DCC/S-Link) Wait occurred</font> &nbsp ";
	    if((0x00002000&stat)>0) *out << " DDU S-Link Full occurred &nbsp ";
	    //	  if((0x00001000&stat)>0) *out << " DDU S-Link Never Ready";
	    if((0x000000cef&stat)>0&&(0x000000cef&stat)<=0x00ff) *out << br();
	  }
	  if((stat&0x00000E00)>0){
	    if((stat&0x00000900)==0x0800||(stat&0x00000500)==0x0400) *out << br();
	    if((0x00000800&stat)>0&&(0x00000100&stat)==0) *out << " <font color=blue>DDU GbE Overflow occurred</font> &nbsp ";
	    if((0x00000400&stat)>0&&(0x00000100&stat)==0) *out << " GbE Transmit Limit occurred &nbsp ";
	    //	    if((0x00000200&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Always Empty &nbsp ";
	    //	  if((0x00000100&stat)>0) *out << " <font color=blue>SPY/GbE Fiber Disconnect occurred</font>";
	    if(0x000000ef&stat) *out << br();
	  }
	  if((stat&0x000000F0)>0){
	    if((0x00000080&stat)>0) *out << " <font color=red>DDU DAQ-Limited Overflow occurred (DCC/S-Link Wait)</font> &nbsp ";
	    if((0x00000040&stat)>0) *out << " <font color=blue>DAQ (DCC/S-Link) Wait</font> &nbsp ";
	    if((0x00000020&stat)>0) *out << " DDU S-Link Full/Stop &nbsp ";
	    if((0x00000010&stat)>0&&(0x00001000&stat)==0) *out << " <font color=red>DDU S-Link Not Ready</font>";
	  }
	  if((stat&0x0000000F)>0){
	    if((0x0000000e&stat)>0&&(0x00000001&stat)==0) *out << br();
	    if((0x00000008&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Full &nbsp ";
	    if((0x00000004&stat)>0&&(0x00000100&stat)==0) *out << " DDU Skipped SPY Event (GbE data not sent) &nbsp ";
	    if((0x00000002&stat)>0&&(0x00000100&stat)==0) *out << " GbE FIFO Not Empty &nbsp ";
	    if((0x00000001&stat)>0) *out << " <font color=blue>DCC Link Not Ready</font>";
	  }
	  //        *out << "</font></blockquote>";
	  *out << "</blockquote></font>";
	}
	else *out << "</font>" << br();

      }else if(i==204&&(stat&0x0000ffff)>0){
        *out << "<blockquote><font size=-1 color=orange face=arial>";
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " <font color=red>DMB Timeout signal, ** needs reset **</font> &nbsp ";
	  if((0x00004000&stat)>0) *out << " <font color=red>Mult L1A Error occurred</font> &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=blue>L1A-FIFO Near Full Warning</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=black>GbE FIFO Almost-Full</font>";
	  if(0x0fff&stat) *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>Ext.FIFO Near Full Warning</font> &nbsp ";
//	  if((0x00000400&stat)>0) *out << " <font color=blue>Near Full Warning</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>InSingle Warning</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=black>CFEB-CRC not OK</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " CFEB-CRC End Error";
	  if(0x00ff&stat) *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " CFEB-CRC Count Error &nbsp ";
	  if((0x00000040&stat)>0) *out << " CFEB-CRC Error occurred &nbsp ";
	  //	  if((0x00000020&stat)>0) *out << " <font color=black>Latched Trigger Trail</font> &nbsp ";
	  if((0x00000020&stat)>0) *out << " Trigger Readout Error &nbsp ";
	  if((0x00000010&stat)>0) *out << " <font color=black>Trigger Trail Done</font>";
	  if(0x000f&stat) *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " <font color=red>Start Timeout</font> &nbsp ";
	  if((0x00000004&stat)>0) *out << " <font color=red>End Timeout</font> &nbsp ";
	  if((0x00000002&stat)>0) *out << " SP/TF Error in last event &nbsp ";
	  if((0x00000001&stat)>0) *out << " SP/TF data detected in last event";
	}
        *out << "</font></blockquote>";

      }else if(i==205&&(stat&0x0000ffff)>0){
	if((0x00000020&stat)>0) *out << " &nbsp  &nbsp <font color=green size=-1>Empty CSC in Event flag</font>";
	if((0x0000FFDF&stat)>0) *out << "<blockquote><font size=-1 color=orange face=arial>";
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " Lost In Event Error &nbsp ";
	  if((0x00004000&stat)>0) *out << " DMB Error in Event&nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=blue>Control DLL Error occured</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " 2nd Header First flag";
	  if(0x0fdf&stat) *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=black>Early 2nd Trailer flag</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=black>Extra 1st Trailer flag</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=black>Extra 1st Header flag</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=black>Extra 2nd Header flag</font>";
	  if(0x00df&stat) *out << br();
	}
	if((stat&0x000000D0)>0){
	  if((0x00000080&stat)>0) *out << " SCA Full detected this Event &nbsp ";
	  if((0x00000040&stat)>0) *out << " <font color=blue>Probable DMB Full occurred</font> &nbsp ";
	  //	  if((0x00000020&stat)>0) *out << " <font color=green>Empty Event flag</font> &nbsp ";
	  if((0x00000010&stat)>0) *out << " <font color=red>Bad Control Word Error occurred</font>";
	  if(0x000f&stat) *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Missed Trigger Trailer Error &nbsp ";
	  if((0x00000004&stat)>0) *out << " First Dat Error &nbsp ";
	  if((0x00000002&stat)>0) *out << " Bad First Word &nbsp ";
	  if((0x00000001&stat)>0) *out << " <font color=red>Confirmed DMB Full occured</font>";
	  //	  if((0x00000001&stat)>0) *out << " <font color=red>Lost In Data occured</font>";
	}
        if((0x0000FFDF&stat)>0) *out << "</font></blockquote>";
	else *out << br();

      }else if(i==206&&(stat&0x0000ffff)>0){
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x0000F000)>0){
	  if((0x00008000&stat)>0) *out << " <font color=orange>Trigger Readout Error</font> &nbsp ";
	  if((0x00004000&stat)>0) *out << " ALCT Trailer Done &nbsp ";
	  if((0x00002000&stat)>0) *out << " <font color=red>2nd ALCT Trailer detected</font> &nbsp ";
	  //	  if((0x00002000&stat)>0) *out << " ALCT DAV Vote True occurred &nbsp ";
	  if((0x00001000&stat)>0) *out << " ALCT L1A mismatch error occurred";
	  if(0x0fff&stat) *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " ALCT CRC Error occurred &nbsp ";
	  if((0x00000400&stat)>0) *out << " ALCT Wordcount Error occurred &nbsp ";
	  if((0x00000200&stat)>0) *out << " Missing ALCT Trailer occurred &nbsp ";
	  if((0x00000100&stat)>0) *out << " ALCT Error occurred";
	  if(0x00ff&stat) *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " DMB Critical Error occurred &nbsp ";
	  //	  if((0x00000080&stat)>0) *out << " Compare Trigger CRC flag &nbsp ";
	  if((0x00000040&stat)>0) *out << " TMB Trailer Done &nbsp ";
	  if((0x00000020&stat)>0) *out << " <font color=red>2nd TMB Trailer detected</font> &nbsp ";
	  //	  if((0x00000020&stat)>0) *out << " TMB DAV Vote True occurred &nbsp ";
	  if((0x00000010&stat)>0) *out << " TMB L1A mismatch error occurred";
	  if(0x000f&stat) *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " TMB CRC Error occurred &nbsp ";
	  if((0x00000004&stat)>0) *out << " TMB Word Count Error occurred &nbsp ";
	  if((0x00000002&stat)>0) *out << " Missing TMB Trailer occurred &nbsp ";
	  if((0x00000001&stat)>0) *out << " TMB Error occurred";
	}
        *out << "</font></blockquote>";

      }else if(i==216&&(stat&0x000000ff)>0){
	*out << std::endl;
        *out << "<font size=-2>";
	if((0x00000040&stat)>0) *out << " &nbsp &nbsp &nbsp L1A FIFO Empty";
	if((0x00000040&stat)==0) *out << " &nbsp &nbsp &nbsp L1A FIFO Not Empty";
	if((0x00000083&stat)>0){
	  *out << "<blockquote>";
	  if((0x00000080&stat)>0) *out << " <font color=blue>DDU C-code L1A error</font>";
	  if((0x00000002&stat)>0) *out << " GbE FIFO Almost-Full occurred &nbsp ";
	  if((0x00000001&stat)>0) *out << " <font color=blue>L1A FIFO Almost-Full occurred</font>";
	  *out << "</blockquote></font>";
	}
	else *out << "</font>" << br();
      }else if(i==218&&(stat&0x000000ff)>0){
        *out << "<font size=-2>";
	if((0x00000040&stat)>0) *out << " &nbsp &nbsp &nbsp L1A FIFO Empty";
	if((0x00000040&stat)==0) *out << " &nbsp &nbsp &nbsp L1A FIFO Not Empty";
	*out << std::endl;
	if((0x00000003&stat)>0){
	  *out << "<blockquote>";
	  if((0x00000002&stat)>0) *out << " GbE FIFO Full occurred &nbsp ";
	  if((0x00000001&stat)>0) *out << " <font color=red>L1A FIFO Full occurred</font>";
	  *out << "</blockquote></font>";
	}
	else *out << "</font>" << br();

      }else if(i==220&&(stat&0x00000fff)>0){
	*out << std::endl;
        *out << "<blockquote><font size=-1 color=red face=arial>";
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " InRD End C-Code Error occurred &nbsp ";
	  if((0x00000400&stat)>0) *out << " InRD Begin C-Code Error occurred &nbsp ";
	  if((0x00000200&stat)>0) *out << " InRD Multiple L1A Mismatches occurred &nbsp ";
	  else if((0x00000100&stat)>0) *out << " <font color=blue>InRD Single L1A Mismatch occurred</font>";
	  *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " InRD Hard Error occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " InRD Sync Error occurred &nbsp ";
	  if((0x00000020&stat)>0) *out << " <font color=blue>InRD Single Error occurred</font> &nbsp ";
	  if((0x00000010&stat)>0) *out << " InRD Mem/FIFO Error occurred";
	  *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " InRD Fiber Connection Error occurred &nbsp ";
	  if((0x00000004&stat)>0) *out << " InRD Multiple Transmit Errors occurred &nbsp ";
	  if((0x00000002&stat)>0) *out << " InRD Stuck Data Error occurred &nbsp ";
	  if((0x00000001&stat)>0) *out << " InRD Timeout Error occurred";
	  *out << br();
	}
        *out << "</font></blockquote>";

      }else if(i==221 && (icond>0 || icond2>0)){
	*out  << std::endl;
	*out << "<blockquote><font size=-1 face=arial>";
	*out << "key for Near Full bits: &nbsp b7=DDU set FMM Warn, &nbsp b6=DMB set Warn," << br();
	*out << " &nbsp &nbsp &nbsp &nbsp b5=Ext. FIFO set Warn, &nbsp b4==L1A FIFO set Warn, &nbsp b[3-0]=InRD 3-0 set Warn";
	*out << "</font></blockquote>" << std::endl;

      }else if(i==224 && icrit>0){
	*out << "<blockquote><font size=-2 color=black face=arial>";
	//	DDUtrapDecode(in, out, thisDDU->fpga_lcode);
	this->DDUtrapDecode(in, out);
	*out << "</font></blockquote>";
      }else{
	*out << br() << std::endl;
      }
    }

    *out << cgicc::body() << std::endl;    
    *out << cgicc::html() << std::endl;    
}


//void EmuFCrateHyperDAQ::DDUtrapDecode(xgi::Input * in, xgi::Output * out,  unsigned long int lcode[10])
//void DDUtrapDecode(xgi::Input * in, xgi::Output * out,unsigned long int lcode[10])
void EmuFCrateHyperDAQ::DDUtrapDecode(xgi::Input * in, xgi::Output * out)
     // throw (xgi::exception::Exception)
{
  printf(" enter DDUtrapDecode \n");
  unsigned long int lcode[10];
  int i;
  //  cgicc::Cgicc cgi(in);
  char buf[100],buf1[100],buf2[100],buf3[100],buf4[100];
  char cbuf1[20],cbuf2[20],cbuf3[20],cbuf4[20];
  char sred[20]="<font color=red>";
  char syel[20]="<font color=orange>";
  char sblu[20]="<font color=blue>";
  char sgrn[20]="<font color=green>";
  char snul[20]="</font>";
  sprintf(buf1," ");
  sprintf(buf2," ");
  sprintf(buf3," ");
  sprintf(buf4," ");
  sprintf(cbuf1," ");
  sprintf(cbuf2," ");
  sprintf(cbuf3," ");
  sprintf(cbuf4," ");
  lcode[0]=thisDDU->fpga_lcode[0];
  lcode[1]=thisDDU->fpga_lcode[1];
  lcode[2]=thisDDU->fpga_lcode[2];
  lcode[3]=thisDDU->fpga_lcode[3];
  lcode[4]=thisDDU->fpga_lcode[4];
  lcode[5]=thisDDU->fpga_lcode[5];
  *out << "<pre>" << std::endl;
  sprintf(buf,"  192-bit DDU Control Diagnostic Trap (24 bytes)");
  *out << buf << std::endl;
  i=23;
  sprintf(buf,"                        o-stat  fful  fifo-c fifo-b");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0x09010000&lcode[5])sprintf(cbuf1,"%s",sblu);
  if(0x40000000&lcode[5])sprintf(cbuf1,"%s",syel);
  if(0x80800000&lcode[5])sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[5])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if(0x01ff&lcode[5])sprintf(cbuf2,"%s",sred);
  sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[5],snul);
  sprintf(cbuf3,"%s",sgrn);
  if(0xffff0000&lcode[4])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[4])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0x01ff&lcode[4])sprintf(cbuf4,"%s",sblu);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[4],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

  i=15;
  sprintf(buf,"                        fifo-a instat c-code  erc");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0xfff00000&lcode[3])sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[3])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if(0x00f0&lcode[3])sprintf(cbuf2,"%s",syel);
  if(0xff0f&lcode[3])sprintf(cbuf2,"%s",sred);
  sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[3],snul);
  sprintf(cbuf3,"%s",sgrn);
  if(0x00200000&lcode[2])sprintf(cbuf3,"%s",syel);
  if(0xffdf0000&lcode[2])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[2])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0x9f1f&lcode[2])sprintf(cbuf4,"%s",syel);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[2],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

  i=7;
  sprintf(buf,"                         erb    era   32-bit status");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0x00110000&lcode[1])sprintf(cbuf1,"%s",syel);
  if(0xd08e0000&lcode[1])sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[1])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if(0x2c00&lcode[1])sprintf(cbuf2,"%s",sblu);
  if(0x01e0&lcode[1])sprintf(cbuf2,"%s",syel);
  if(0xc00c&lcode[1])sprintf(cbuf2,"%s",sred);
  sprintf(buf2,"%s   %04lx%s",cbuf2,0xffff&lcode[1],snul);
  sprintf(cbuf3,"%s",sgrn);
  if(0x21800000&lcode[0])sprintf(cbuf3,"%s",sblu);
  if(0xd00f0000&lcode[0])sprintf(cbuf3,"%s",syel);
  if(0x0e400000&lcode[0])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s   %04lx%s",cbuf3,(0xffff0000&lcode[0])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0x3400&lcode[0])sprintf(cbuf4,"%s",sblu);
  if(0x4b23&lcode[0])sprintf(cbuf4,"%s",syel);
  if(0x80dc&lcode[0])sprintf(cbuf4,"%s",sred);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[0],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;



// JRGhere, new code section:
/*
*/
  unsigned int CSCstat=0;
  unsigned long int i0trap[8];
  unsigned long int i1trap[8];
  unsigned long int i0stat,i1stat,ddustat,erastat;
  short int igot_i0,igot_i1,solved,iFill,iTimeout;
  igot_i0=0;  igot_i1=0;  solved=0;  iFill=0;  iTimeout=0;
  *out << "DDU Diagnosis results:" << std::endl;
  CSCstat=thisDDU->vmepara_CSCstat();
  thisDDU->ddu_fpgastat();  // Begin Global DDUstatus check
  ddustat=((0xffff&thisDDU->ddu_code1)<<16)|(0xffff&thisDDU->ddu_code0);
  thisDDU->ddu_rderareg();
  erastat=0x0000ffff&thisDDU->ddu_code0;
  thisDDU->infpgastat(INFPGA0);  // Begin Global InFPGA0 check
  i0stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
  //  sprintf(buf,"infpga0 32-bit Status: %08lXh ",i0stat);
  //  *out << buf << std::endl;
  if(i0stat&0x04000000){            // DLL Error
    *out << "**DLLerror detected on InFPGA0** " << std::endl;
    if((i0stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << std::endl;
    solved=1;
  }
  else if(i0stat&0x00000004){       // Fiber Change
    thisDDU->infpga_CheckFiber(INFPGA0);
    sprintf(buf,"**Fiber Connection error detected for DDUinput[7:0]=0x%02x** ",(thisDDU->infpga_code0&0xff00)>>8);
    *out << buf << std::endl;
    if((i0stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << std::endl;
    else if((i0stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << std::endl;
    solved=1;
  }
  else if((i0stat&0x00000800)>0){   // GT-Rx Error
    thisDDU->infpga_RxErr(INFPGA0);
    sprintf(buf," *GT-Rx Error for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0xff00)>>8);
    *out << buf << std::endl;
    if((i0stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << std::endl;
    solved=1;
  }
  else if((lcode[1]&0x00010000)>0&&(i0stat&0x00880000)>0){  // DMB-Full
    thisDDU->infpga_DMBwarn(INFPGA0);
    sprintf(buf," *confirmed DMB Full for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0xff00)>>8);
    *out << buf << std::endl;
    if((lcode[0]&0x00040000)>0&&(lcode[2]&0x00001c00)==0)*out << " ^^^DMB Full FIFO for ALCT " << std::endl;
    else if((lcode[0]&0x00080000)>0&&(lcode[2]&0x0000001c)==0)*out << " ^^^DMB Full FIFO for TMB " << std::endl;
    else *out << " ^^^DMB Full FIFO, probably for CFEB " << std::endl;
    if((i0stat&0x00000130)>0)*out << "       --probable cause of SpecialWord/Xmit errors  " << std::endl;
    solved=1;
  }


  thisDDU->infpgastat(INFPGA1);  // Begin InFPGA1 check, repeat from InFPGA0^^
  i1stat=((0xffff&thisDDU->infpga_code1)<<16)|(0xffff&thisDDU->infpga_code0);
  //  sprintf(buf2,"infpga1 32-bit Status: %08lXh ",i1stat);
  //  *out << buf << std::endl;
  if(i1stat&0x04000000){
    *out << "**DLLerror detected on InFPGA1** " << std::endl;
    if((i1stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << std::endl;
    solved=1;
  }
  else if(i1stat&0x00000004){
    thisDDU->infpga_CheckFiber(INFPGA1);
    sprintf(buf,"**Fiber Connection error detected for DDUinput[14:8]=0x%02x** ",(thisDDU->infpga_code0&0x7f00)>>8);
    *out << buf << std::endl;
    if((i1stat&0x00000800)>0)*out << "  ^^^probable cause of Gt-Rx errors " << std::endl;
    else if((i1stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << std::endl;
    solved=1;
  }
  else if((i1stat&0x00000800)>0){
    thisDDU->infpga_RxErr(INFPGA1);
    sprintf(buf," *GT-Rx Error for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x7f00)>>8);
    *out << buf << std::endl;
    if((i1stat&0x00000130)>0)*out << "  ^^^probable cause of SpecialWord/Xmit errors " << std::endl;
    solved=1;
  }
  else if((lcode[1]&0x00010000)>0&&(i1stat&0x00880000)>0){
    thisDDU->infpga_DMBwarn(INFPGA1);
    sprintf(buf," *confirmed DMB Full for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x7f00)>>8);
    *out << buf << std::endl;
    if((lcode[0]&0x00040000)>0&&(lcode[2]&0x00001c00)==0)*out << " ^^^DMB Full FIFO for ALCT " << std::endl;
    else if((lcode[0]&0x00080000)>0&&(lcode[2]&0x0000001c)==0)*out << " ^^^DMB Full FIFO for TMB " << std::endl;
    else *out << " ^^^DMB Full FIFO, probably for CFEB " << std::endl;
    if((i1stat&0x00000130)>0)*out << "       --probable cause of SpecialWord/Xmit errors " << std::endl;
    solved=1;
  }
  //  ^^^^ InFPGA big-problem analysis solved it? ^^^^

  if(lcode[0]&0x00000400){
    *out << "**DLLerror detected on DDUctrl FPGA** " << std::endl;
    solved=1;
  }
  if((lcode[5]&0x00800000)>0&&(lcode[2]&0x0c000000)==0){  // DAQ-induced Buffer overflow, FullFIFO
    *out << "**DAQ-induced Buffer Overflow** " << std::endl;
    solved=1;
  }
  if((lcode[0]&0x4000000A)==8&&(lcode[1]&0x00004000)==0&&(lcode[2]&0x0c000000)==0){ // DDU Buff ovfl
    *out << "**DDU FIFO Full** " << std::endl;
    if(lcode[5]&0x0000000f){	// Ext.FIFO
      sprintf(buf," ^^^Memory error for DDU Ext.FIFO[3:0]=0x%01lx ",lcode[5]&0x0000000f);
      *out << buf;
    }
    if(lcode[5]&0x00000100)*out << "  ^^^L1A FIFO Full ";
    *out << " " << std::endl;
    solved=1;
  }

  if(solved<1&&(((erastat|lcode[1])&0x00008000)>0)){  // DMB-Timeout?  check at and after Critical error point, get DMB Error Reg
    *out << "**DMB Timeout signal detected** " << std::endl;
    thisDDU->ddu_rddmberr();
    sprintf(buf,"    ^^^Error on DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
    *out << buf << std::endl;

// Note: if ALCT Timeout on DMB then LIE, CRC & WC errors are likely from
//       overrun to TMB trail; look for lcode-LIE caused by bad/missTrigTrail.
//    If DMB end-timeout causes 64-bit misalignment then maybe check that too.
    solved=1;
    iTimeout=1;
    if((lcode[0]&0x00040000)>0&&(lcode[1]&0x00080000)>0)*out << " ^^^DMB Timeout for ALCT " << std::endl;
    else if((lcode[0]&0x00080000)>0&&(lcode[1]&0x00080000)>0)*out << " ^^^DMB Timeout for TMB " << std::endl;
    else if((lcode[0]&0x00000800)==0)*out << " ^^^DMB Timeout, probably for CFEB " << std::endl;
    else{
      *out << " ^^^DMB Timeout w/64-bit misalignment, possibly from CFEB " << std::endl;
      solved=0;
    }
    if((i0stat&0x0000e000)>0)*out << "       --probable cause of problems on InFPGA0 " << std::endl;
    if((i1stat&0x0000e000)>0)*out << "       --probable cause of problems on InFPGA1 " << std::endl;
  }
  else if((lcode[2]&0x0C000000)>0||(lcode[1]&0x00020000)>0){  // DDU FIFO Transfer/C-code Error
    *out << "**DDU FIFO Transfer error detected** " << std::endl;
    if(lcode[2]&0x04000000)*out << "  ^^^Begin C-code error " << std::endl;
    if(lcode[2]&0x08000000)*out << "  ^^^End C-code error " << std::endl;
    sprintf(buf,"  ^^^probably occurred on Ext.FIFO[3:0]=0x%01lx  (from InMxmit Reg) ",(lcode[2]>>28)&0x0000000f);
    *out << buf << std::endl;
    solved=1;
  }

// InCtrlErr at CritErr point:
  if(solved<1&&((lcode[0]&0x00400000)>0)){ // InCtrlErr & NotDDUfullFIFO
    //    *out << "-debug> inside 3>" << std::endl;
    if((i0stat|i1stat)&0x40000000){    // Filler=64bit-misalign
      if((i0stat&0x40000000)>0){       //   for InFPGA0
	iFill=1;
	thisDDU->infpga_XmitErr(INFPGA0);
	sprintf(buf," *64-bit Align Error for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
	*out << buf << std::endl;
	if((i0stat&0x00000130)>0)*out << "  ^^^also associated with SpecialWord errors  " << std::endl;
      }
      if((i1stat&0x40000000)>0){       //   for InFPGA1
	iFill+=2;
	thisDDU->infpga_XmitErr(INFPGA1);
	sprintf(buf," *64-bit Align Error for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
	*out << buf << std::endl;
	if((i1stat&0x00000130)>0)*out << "  ^^^also associated with SpecialWord errors  " << std::endl;
      }
      //      solved=1;
    }
// If InCtrlErr and not solved, get InTrap registers
    if(i0stat&0x00008000){
      //      *out << "-debug> inside 4>" << std::endl;
      thisDDU->infpga_trap(INFPGA0);
      i0trap[5]=thisDDU->fpga_lcode[5];
      i0trap[4]=thisDDU->fpga_lcode[4];
      i0trap[3]=thisDDU->fpga_lcode[3];
      i0trap[2]=thisDDU->fpga_lcode[2];
      i0trap[1]=thisDDU->fpga_lcode[1];
      i0trap[0]=thisDDU->fpga_lcode[0];
      igot_i0=1;
    }
    if(i1stat&0x00008000){
      thisDDU->infpga_trap(INFPGA0);
      i1trap[5]=thisDDU->fpga_lcode[5];
      i1trap[4]=thisDDU->fpga_lcode[4];
      i1trap[3]=thisDDU->fpga_lcode[3];
      i1trap[2]=thisDDU->fpga_lcode[2];
      i1trap[1]=thisDDU->fpga_lcode[1];
      i1trap[0]=thisDDU->fpga_lcode[0];
      igot_i1=1;
    }
  }


  if(solved<1&&iFill>0){  //  check for cause of misalignment early
    if(lcode[0]&0x80000002)*out << "  ^^^possible that DMB may have caused 64-bit Align Error" << std::endl;  // LCT/DAV(lcode[0]31), DMBL1A(lcode[0]1)
    else if((lcode[2]&0x00000c00)>0||(lcode[0]&0x00040000)>0){  // ALCTerr
      thisDDU->ddu_rdalcterr();
      *out << "  ^^^possible that ALCT may have caused 64-bit Align Error" << std::endl;
      if((lcode[1]&0x00080000)>0&&(lcode[0]&0x00040000)>0)*out << "    ^^probable ALCT Trail word problem" << std::endl;
      else if(lcode[0]&0x00000020)*out << "    ^^probable ALCT CRC mismatch" << std::endl;
      else if(lcode[0]&0x00020000)*out << "    ^^probable ALCT Wordcount mismatch" << std::endl;
      if((thisDDU->ddu_code0)&0x7fff){
	sprintf(buf,"      ^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
    }
    else if((lcode[2]&0x0000000c)>0||(lcode[0]&0x00080000)>0){  // TMBerr
      thisDDU->ddu_rdtmberr();
      *out << "  ^^^possible that TMB may have caused 64-bit Align Error" << std::endl;
      if((lcode[1]&0x00080000)>0&&(lcode[0]&0x00080000)>0)*out << "    ^^probable TMB Trail word problem" << std::endl;
      else if(lcode[0]&0x00000020)*out << "    ^^probable TMB CRC mismatch" << std::endl;
      else if(lcode[0]&0x00020000)*out << "    ^^probable TMB Wordcount mismatch" << std::endl;
      if((thisDDU->ddu_code0)&0x7fff){
	sprintf(buf,"      ^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
    }
    else if(lcode[1]&0x00080000){  // TrgTrail error, maybe never get this one
      thisDDU->ddu_rdalcterr();
      *out << "  ^^^probable that ALCT or TMB caused 64-bit Align Error" << std::endl;
      *out << "    ^^Trigger Trail word problem" << std::endl;
      if((thisDDU->ddu_code0)&0x7fff){
	sprintf(buf,"      ^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
      thisDDU->ddu_rdtmberr();
      if((thisDDU->ddu_code0)&0x7fff){
	sprintf(buf,"      ^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
    }
    else if(lcode[0]&0x50000001){  // CFEBerr
      *out << "  ^^^CFEB may have caused 64-bit Align Error" << std::endl;
      if(lcode[0]&0x10000000)*out << "    ^^CFEB count mismatch" << std::endl;
      if(lcode[0]&0x40000000)*out << "    ^^CFEB L1A mismatch" << std::endl;
      if(lcode[0]&0x00000001)*out << "    ^^CFEB or DMB CRC mismatch" << std::endl;
      if(lcode[1]&0x00000380){
	*out << "      ^confirmed CFEB CRC error" << std::endl;
	if(lcode[1]&0x00000080)*out << "        ^CFEB CRC-count error" << std::endl;
	if(lcode[1]&0x00000100)*out << "        ^CFEB CRC-end error" << std::endl;
	if(lcode[1]&0x00000200)*out << "        ^CFEB CRC mismatch" << std::endl;
      }
      else if(lcode[0]&0x00000201)*out << "      ^confirmed DMB CRC mismatch at least" << std::endl;
    }
    thisDDU->ddu_rddmberr();
    if((thisDDU->ddu_code0)&0x7fff){
      sprintf(buf,"      ^^CSC Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
    }
    if(iTimeout>0){
      if(lcode[1]&0x00008000)*out << "  ^^^Timeout at DMB caused 64-bit Align Error" << std::endl;
      solved=1;
    }
  }

// If InCtrlErr, determine what happened at CritErr point:
//        Timeout/StuckDat/MultXmit/MemErr/MultL1A
  if(solved<1&&((lcode[0]&0x00400000)>0)){
    //    *out << "-debug> inside 5>" << std::endl;
    if(igot_i0>0){  // got_i0trap;
      //      *out << "-debug> inside 6>" << std::endl;
      if((i0trap[0]&0x00000040)>0){
	if((i0trap[3]&0x00ff0000)>0)sprintf(buf," *Start Timeout for DDUinput[7:0] = 0x%02lx* ",((i0trap[3]>>16)&0x000000ff));
	else if((i0trap[4]&0x0000ffff)>0)sprintf(buf," *End Timeout for DDUinput[7:0] = 0x%02lx* ",((i0trap[4]>>8)|i0trap[4])&0x000000ff);
	*out << buf << std::endl;
	if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << std::endl;
	solved=1;
      }
      else if((i0trap[0]&0x00000080)>0){  // StuckData
	sprintf(buf," *StuckData error for DDUinput[7:0] = 0x%02lx* ",((i0trap[2]>>24)&0x000000ff));
	*out << buf << std::endl;
	if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << std::endl;
	solved=1;
      }
      else if((i0trap[0]&0x00000010)>0){  // Multi-Xmit error
	thisDDU->infpga_XmitErr(INFPGA0);
	sprintf(buf," *Multiple SpecialWord bit-errors for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
	if(iFill==1||iFill==3)sprintf(buf," *Extra or Missing 16-bit words for DDUinput[7:0]=0x%02x* ",(thisDDU->infpga_code0&0x00ff));
	*out << buf << std::endl;
	if(((i0trap[0]&0x00000020)>0))*out << "  ^^^multiple bit-errors in the same word " << std::endl;  // typical for offset in DMBhdr1+2, maybe TMBtr/DMBtr
	else *out << "  ^^^single bit-errors in different words " << std::endl;
	if(iFill==1||iFill==3)*out << "  ^^^probably related to 64-bit Align Error for InFPGA0" << std::endl;
	solved=1;
      }
      else if((i0trap[0]&0x00000008)>0){  // InFPGA0 Memory Full
	if((i0trap[0]&0x00040000)>0){
	  sprintf(buf," *Memory error for DDU InRD0* ");
	  *out << buf << std::endl;
	  if(((i0trap[5]&0x0000001f)>0)){
	    sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",i0trap[5]&0x0000001f);
	    *out << buf << std::endl;
	  }
	  else{
	    *out << "  ^^^all InMem units used " << std::endl;
	    if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << std::endl;
	  }
	}
	else if((i0trap[0]&0x00400000)>0){
	  sprintf(buf," *Memory error for DDU InRD1* ");
	  *out << buf << std::endl;
	  if(((i0trap[5]&0x000003e0)>0)){
	    sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",(i0trap[5]>>5)&0x0000001f);
	    *out << buf << std::endl;
	  }
	  else{
	    *out << "  ^^^all InMem units used " << std::endl;
	    if(iFill==1||iFill==3)*out << "  ^^^may have caused 64-bit Align Error for InFPGA0" << std::endl;
	  }
	}
	solved=1;
      }
      else if((i0trap[0]&0x002001fc)==0x00200000){
	sprintf(buf," *Multiple L1A errors for DDUinput[7:0] = 0x%02lx* ",((i0trap[2]>>16)&0x000000ff));
	*out << buf << std::endl;
	solved=1;
      }
    }
    if(igot_i1>0){  // got_i1trap;
      //      *out << "-debug> inside 7>" << std::endl;
      if((i1trap[0]&0x00000040)>0){
	if((i1trap[3]&0x007f0000)>0)sprintf(buf," *Start Timeout for DDUinput[14:8] = 0x%02lx* ",((i1trap[3]>>16)&0x0000007f));
	else if((i1trap[4]&0x00007f7f)>0)sprintf(buf," *End Timeout for DDUinput[14:8] = 0x%02lx* ",((i1trap[4]>>8)|i1trap[4])&0x0000007f);
	*out << buf << std::endl;
	if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << std::endl;
	solved=1;
      }
      else if((i1trap[0]&0x00000080)>0){  // StuckData
	sprintf(buf," *StuckData error for DDUinput[14:8] = 0x%02lx* ",((i1trap[2]>>24)&0x0000007f));
	*out << buf << std::endl;
	if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << std::endl;
	solved=1;
      }
      else if((i1trap[0]&0x00000010)>0){  // Multi-Xmit error
	thisDDU->infpga_XmitErr(INFPGA1);
	sprintf(buf," *Multiple SpecialWord bit-errors for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
	if(iFill>1)sprintf(buf," *Extra or Missing 16-bit words for DDUinput[14:8]=0x%02x* ",(thisDDU->infpga_code0&0x007f));
	*out << buf << std::endl;
	if(((i1trap[0]&0x00000020)>0))*out << "  ^^^multiple bits in the same word " << std::endl;
	else *out << "  ^^^single bits in different words " << std::endl;
	if(iFill>1)*out << "  ^^^probably related to 64-bit Align Error for InFPGA1" << std::endl;
	solved=1;
      }
      else if((i1trap[0]&0x00000008)>0){  // InFPGA1 Memory Full
	if((i1trap[0]&0x00040000)>0){
	  sprintf(buf," *Memory error for DDU InRD2* ");
	  *out << buf << std::endl;
	  if(((i1trap[5]&0x0000001f)>0)){
	    sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",i1trap[5]&0x0000001f);
	    *out << buf << std::endl;
	  }
	  else{
	    *out << "  ^^^all InMem units used " << std::endl;
	    if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << std::endl;
	  }
	}
	else if((i1trap[0]&0x00400000)>0){
	  sprintf(buf," *Memory error for DDU InRD3* ");
	  *out << buf << std::endl;
	  if(((i1trap[5]&0x000003e0)>0)){
	    sprintf(buf,"  ^^^L1A buffer overflow (%ld memories available) ",(i1trap[5]>>5)&0x0000001f);
	    *out << buf << std::endl;
	  }
	  else{
	    *out << "  ^^^all InMem units used " << std::endl;
	    if(iFill>1)*out << "  ^^^may have caused 64-bit Align Error for InFPGA1" << std::endl;
	  }
	}
	solved=1;
      }
      else if((i1trap[0]&0x002001fc)==0x00200000){
	sprintf(buf," *Multiple L1A errors for DDUinput[14:8] = 0x%02lx* ",((i1trap[2]>>16)&0x0000007f));
	*out << buf << std::endl;
	solved=1;
      }
    }
    if(solved<1&&iFill<1)*out << "  InFPGAs are not related to the cause of the problem" << std::endl;
  }

// if it's not InFPGA related:
  if(solved<1){
    if((lcode[0]&0x4000000A)==8 && (lcode[1]&0x00004000)==0){  // DDU Buff ovfl
      *out << "**DDU FIFO Full** " << std::endl;
      if(lcode[5]&0x000000f0){  // InRd Mem
	sprintf(buf," ^^^Memory error for DDU InRd[3:0]=0x%01lx ",(lcode[5]>>4)&0x0000000f);
	*out << buf;
      }
      *out << "  ^^^Should've been detected at InFPGA level... ";
      *out << " " << std::endl;
      solved=1;
    }

    if((lcode[2]&0x00000080)>0&&(lcode[1]&0x00080000)==0&&(lcode[0]&0x90000000)==0x80000000){  //  LCT-DAV error == DMBcrit + !BadTrgTrail + !CFEBcntErr
      thisDDU->ddu_rddmberr();
      *out << " *DDUctrl saw mismatch for DMB-CFEB LCT/DAV/Movlp* " << std::endl;
      sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
      solved=1;
    }

    if((lcode[2]&0x00000080)>0&&(lcode[1]&0x00080000)>0){  //  DMBcrit + BadTrgTrail
      thisDDU->ddu_rddmberr();
      *out << " *DDUctrl saw Trigger Trailer Error* " << std::endl;
      sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
      if(lcode[2]&0x00000008){    // HALCTerr
	thisDDU->ddu_rdalcterr();
	sprintf(buf,"    ^^^ALCT Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
      if(lcode[2]&0x00000001){    // HTMBerr
	thisDDU->ddu_rdtmberr();
	sprintf(buf,"    ^^^TMB Errors from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
      }
      if(lcode[0]&0x00000010)*out << "    ^^^Probably caused Multi-SpecialWord errors" << std::endl;
      solved=1;
    }
    else if((lcode[2]&0x00000080)>0&&(lcode[0]&0x10000000)>0){  //  DMBcrit + CFEBcntErr
      thisDDU->ddu_rddmberr();
      *out << " *DDUctrl saw wrong CFEB count in data* " << std::endl;
      sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
    }

    if(lcode[0]&0x00000010){  // DDUctrl Multi-Xmit-Err
      thisDDU->ddu_rdxmiterr();
      sprintf(buf," *DDUctrl Multiple SpecialWord bit-errors for DDUinput[14:0]=0x%04x* ",thisDDU->ddu_code0);
      *out << buf << std::endl;
      if(lcode[0]&0x0000800)*out << "  ^^^DDUctrl FPGA saw 64-bit-misalign flag, reported at InFPGA level? "  << std::endl;
      solved=1;
    }
    if(lcode[0]&0x00000004){  // DDUctrl Fiber/FIFO Connect error
	sprintf(buf," *DDUctrl Fiber/FIFO error for Ext.FIFO[3:0]=0x%01x* ",(lcode[3]>>24)&0x0000000f);
	*out << buf << std::endl;
	if((CSCstat&0x7fff)>0){
	  sprintf(buf,"  ^^^possibly for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
	  *out << buf << std::endl;
	}
	*out << "  ^^^Fiber error not reported at InFPGA level "  << std::endl;
	solved=1;
    }
    if(lcode[0]&0x00000040){  // DDUctrl Timeout error
	sprintf(buf," *DDUctrl Timeout error, ");
	if(lcode[4]&0x00f00000)sprintf(buf2,"Start Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>20)&0x0000000f);
	if(lcode[4]&0x0f000000)sprintf(buf2,"End-Wait Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>24)&0x0000000f);
	if(lcode[4]&0xf0000000)sprintf(buf2,"End-Busy Timeout for Ext.FIFO[3:0]=0x%1lx* ",(lcode[3]>>28)&0x0000000f);
	*out << buf << buf2 << std::endl;
	if((CSCstat&0x7fff)>0){
	  sprintf(buf,"  ^^^probably for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
	  *out << buf << std::endl;
	}
	solved=1;
    }
    if(lcode[0]&0x02000000){  // DDUctrl StuckDat error
	sprintf(buf," *DDUctrl StuckData error, Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>28)&0x0000000f);
	*out << buf << std::endl;
	if((CSCstat&0x7fff)>0){
	  sprintf(buf,"  ^^^probably for DDUinput[14:0]=0x%04x ",(CSCstat&0x7fff));
	  *out << buf << std::endl;
	}
	solved=1;
    }
  }

  if(solved<1){ // Stopgap measures if not solved:
//                check for LID+cause, else LIE+cause, related errors...
    if(lcode[0]&0x00000080){  // LID error, critical error
      *out << " **Big data corruption problem detected, DDU Lost in Data** " << std::endl;
      if(lcode[1]&0x08000000)*out << "   Bad 2nd DMBtr";
      if(lcode[1]&0x04000000)*out << "   Extra 1st DMBtr";
      if(lcode[1]&0x02000000)*out << "   Extra 1st DMBhdr";
      if(lcode[1]&0x01000000)*out << "   Extra 2nd DMBhdr";
      if(lcode[1]&0x00200000)*out << "   invalid Lone Word signal";
      if(lcode[1]&0x00080000)*out << "   Bad Trig Trail";
      *out << " " << std::endl;
    }
    else if(lcode[0]&0x00000100){  // LIE error, not critical
      *out << " *Small data corruption problem detected, DDU Lost in Event*" << std::endl;
      if(lcode[1]&0x00080000)*out << "   Missed Trig Trail";
      if(lcode[1]&0x00040000)*out << "   Bad 1st DMBhdr";
      if(lcode[1]&0x10000000)*out << "   2nd DMBhdr 1st";
// Should have been detected and solved above:
      if(lcode[1]&0x00020000)*out << "   DDU FIFO Transfer error (bad C-code)";
      *out << " " << std::endl;
    }
    if(lcode[0]&0x00040000){  // ALCT error, not critical
      thisDDU->ddu_rdalcterr();
      *out << " *ALCT data problem detected:";
      if(lcode[2]&0x00000800)*out << "   CRCerr";
      if(lcode[2]&0x00001000)*out << "   L1A mismatch";
      if(lcode[2]&0x00000400)*out << "   WordCountErr";
      if((lcode[2]&0x00001c00)==0)*out << "   ALCT problem on DMB, likely Full FIFO, maybe Timeout";
      *out << "* " << std::endl;
      sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
      //      solved=1;
    }
    if(lcode[0]&0x00080000){  // TMB error, not critical
      thisDDU->ddu_rdtmberr();
      *out << " *TMB data problem detected:";
      if(lcode[2]&0x00000008)*out << "   CRCerr";
      if(lcode[2]&0x00000010)*out << "   L1A mismatch";
      if(lcode[2]&0x00000004)*out << "   WordCountErr";
      if((lcode[2]&0x0000001c)==0)*out << "   TMB problem on DMB, Timeout or Full FIFO";
      *out << "* " << std::endl;
      sprintf(buf,"    ^^^Detected Error from DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
      *out << buf << std::endl;
      //      solved=1;
    }

    if((lcode[1]&0x00004000)>0||(lcode[0]&0x00000008)>0){  // Mult-L1A error:
//        confirmed CFEB L1err; DMB(hdr/tr), TMB or ALCT combined & accumulated
      thisDDU->ddu_rddmberr();
      *out << " *Cause was Multiple L1A errors* " << std::endl;
      if((lcode[0]&0x90400fff)==0x0000000a&&(lcode[4]&0x00008000)>0){
	sprintf(buf,"  ^^^DDU C-code L1A error, Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);  // TrgL1 & DMBtrL1 not involved
	*out << buf << std::endl;
	solved=1;
      }
      else if((lcode[0]&0x9fc00fff)==0x0000020a){
	sprintf(buf,"  ^^^DMB L1A Mismatch (not from InFPGA) for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
	solved=1;
      }
      else if((lcode[0]&0x9fc00dff)==0x0040000a){
	*out << "  ^^^DMB L1A Mismatch error (from InFPGA) " << std::endl;
	if(lcode[3]&0x00f00000){
	  sprintf(buf,"   ^^errors found for Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);
	  *out << buf << std::endl;
	}
	solved=2;
      }
      else if((lcode[0]&0x00000002)>0){
	*out << "  ^^^Likely caused by DMB L1A Mismatch, other errors too " << std::endl;  // other things wrong too
	if(lcode[3]&0x00f00000){
	  sprintf(buf,"   ^^errors found for Ext.FIFO[3:0]=0x%01lx* ",(lcode[3]>>20)&0x0000000f);
	  *out << buf << std::endl;
	}
	solved=2;
      }
      if((lcode[0]&0xdf800dfd)==0x40000008)*out << "  ^^^CFEB L1A Mismatch " << std::endl;
      else if((lcode[0]&0x40000008)>0)*out << "  ^^^CFEB L1A Mismatch, other errors too " << std::endl;  // other things wrong too
      if((lcode[0]&0x40000008)>0)solved=2;
      if(((thisDDU->ddu_code0)&0x7fff)>0&&solved==2){
	sprintf(buf,"   ^^errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	*out << buf << std::endl;
	solved=1;
      }

      if((lcode[0]&0x9f810df5)==0x00010000)*out << "  ^^^Trigger L1A Mismatch " << std::endl;
      else if((lcode[0]&0x00010000)>0)*out << "  ^^^Trigger L1A Mismatch, other errors too " << std::endl;
      if((lcode[0]&0x00010000)>0){
	solved=1;
	thisDDU->ddu_rdalcterr();
	if(lcode[2]&0x00001000)*out << "   ^^ALCT L1A Mismatch " << std::endl;
	else if(lcode[0]&0x00040000)*out << "   ^^ALCT errors present " << std::endl;
	if((thisDDU->ddu_code0)&0x7fff){
	  sprintf(buf,"    ^ALCT errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	  *out << buf << std::endl;
	}

	thisDDU->ddu_rdtmberr();
	if(lcode[2]&0x00000010)*out << "   ^^TMB L1A Mismatch " << std::endl;
	else if(lcode[0]&0x00080000)*out << "   ^^TMB errors present " << std::endl;
	if((thisDDU->ddu_code0)&0x7fff){
	  sprintf(buf,"    ^TMB errors found for DDUinput[14:0]=0x%04x ",(thisDDU->ddu_code0)&0x7fff);
	  *out << buf << std::endl;
	}
      }
    }

  }
  if(solved<1){
      *out << " *Error cause not clearly determined*" << std::endl;
      sprintf(buf,"    ^^^likely came from DDUinput[14:0]=0x%04x ",CSCstat&0x7fff);
      *out << buf << std::endl;
  }

/*
 Are all DDU critical problems covered?
   --if SPWD/Mxmit (esp. from stat) check for ALCT/TMB errors (from lcode)
Ideas (firmware & software):
  -FILLed does not Solve...want to see if lost/offset TrgTrail caused it
     -> check for Fill combined with others: .not.C-codeErr (lcode[2]27:26)!
         distinguish DMB Trail/CRC error from CFEB/CRC error?
	    ==> CRCerr+!DMBerr+(DMBcrcOK?) @CFEB
	    ==> CRCerr+DMBerr @DMB
  -include TMB/ALCT Err in CSCsumErrReg that goes in DAQ path?
     -> how does DMB handle it?  Good enough?  Remove SCAovfl case?
     -> any way to set CSC-specific FMM path bits?
  -include logic for multi-TMB/ALCT L1error (WC,CRC?) sets Crit/SyncErr?
     -> how does DMB handle it?  Good enough?
     -> Multi-Trg-L1A causes error?
     -> CRC/WC go to a "MultiXmitErr" Reg?
  -InMl1aErr could go someplace in FPGA logic?
  */
// JRGhere, End new code section^^^^^


  *out << "</pre>" << br() << std::endl;
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
    char buf[300],buf2[300],buf3[300],buf4[30];
    sprintf(buf,"DDU INFPGA0, VME  Slot %d",thisDDU->slot());
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");
    *out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << std::endl;

    for(int i=300;i<322;i++){
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
           sprintf(buf,"infpga0 L1 Event Scaler0 bits[23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
	   stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
           sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
      }
      if(i==302){
	thisDDU->infpga_rd1scaler(INFPGA0);
           sprintf(buf,"infpga0 L1 Event Scaler1 bits[23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
	   stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
           sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
      }
      if(i==303){
	thisDDU->infpga_DMBwarn(INFPGA0);
       	*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset</font>" << br() << std::endl;
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
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==306){
	thisDDU->infpga_RxErr(INFPGA0);
	sprintf(buf,"infpga0 DDU GT-Rx Error, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp Timeout-Start, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0xff00)icond2=3;
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==307){
	thisDDU->infpga_Timeout(INFPGA0);
	sprintf(buf,"infpga0 Timeout-EndBusy, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp Timeout-EndWait, F[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      } 
      if(i==308){
	thisDDU->infpga_XmitErr(INFPGA0);
	sprintf(buf,"infpga0 SCA Full history, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp CSC Transmit Error, Fiber[7-0]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==309){
	thisDDU->infpga_LostErr(INFPGA0);
	sprintf(buf,"infpga0 DDU LostInEvent Error, Fiber[7-0]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DDU LostInData Error, F[7-0]:");
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
	if(0xfc00&stat)icond2=1;
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
       	*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << std::endl;
	thisDDU->infpga_FiberDiagA(INFPGA0);
	sprintf(buf,"infpga0 MemCtrl-0 #FIFOs Used, Fibers 3-0: ");
	sprintf(buf2,"%ld, %ld, %ld, %ld <br>",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
	thisDDU->infpga_FiberDiagB(INFPGA0);
	sprintf(buf3,"infpga0 MemCtrl-1 #FIFOs Used, Fibers 7-4: ");
	sprintf(buf4,"%ld, %ld, %ld, %ld",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
      }
      if(i==314){
	thisDDU->infpga_MemAvail(INFPGA0);
	sprintf(buf,"infpga0 Current FIFO Memory Available:");
	sprintf(buf2,"MemCtrl-0 = %2d free, ",thisDDU->infpga_code0&0x001f);
	sprintf(buf4," &nbsp MemCtrl-1 = %2d free",thisDDU->infpga_code1);
	if(thisDDU->infpga_code0==1)icond=1;
	if(thisDDU->infpga_code0==0)icond=2;
	if(thisDDU->infpga_code1==1)icond2=1;
	if(thisDDU->infpga_code1==0)icond2=2;
      }
      if(i==315){
	thisDDU->infpga_Min_Mem(INFPGA0);
	sprintf(buf,"infpga0 Minimum FIFO Memory Availabile:");
	sprintf(buf2,"MemCtrl-0 min = %d free, ",thisDDU->infpga_code0&0x001f);
	sprintf(buf4," &nbsp MemCtrl-1 min = %d free",thisDDU->infpga_code1);
	if(thisDDU->infpga_code0==1)icond=1;
	if(thisDDU->infpga_code0==0)icond=2;
	if(thisDDU->infpga_code1==1)icond2=1;
	if(thisDDU->infpga_code1==0)icond2=2;
      }

      if(i==316){
	thisDDU->infpga_WrMemActive(INFPGA0,0);
	sprintf(buf,"infpga0 Write Memory Active 0-1:");
	sprintf(buf2," Fiber 0 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 0 Unused, &nbsp ");
	sprintf(buf4," Fiber 1 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 1 Unused");
      }
      if(i==317){
	thisDDU->infpga_WrMemActive(INFPGA0,1);
	sprintf(buf,"infpga0 Write Memory Active 2-3:");
	sprintf(buf2," Fiber 2 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 2 Unused, &nbsp ");
	sprintf(buf4," Fiber 3 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 3 Unused");
      }
      if(i==318){
	thisDDU->infpga_WrMemActive(INFPGA0,2);
	sprintf(buf,"infpga0 Write Memory Active 4-5:");
	sprintf(buf2," Fiber 4 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 4 Unused, &nbsp ");
	sprintf(buf4," Fiber 5 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 5 Unused");
      }
      if(i==319){
	thisDDU->infpga_WrMemActive(INFPGA0,3);
	sprintf(buf,"infpga0 Write Memory Active 6-7:");
	sprintf(buf2," Fiber 6 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 6 Unused, &nbsp ");
	sprintf(buf4," Fiber 7 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 7 Unused");
      }
      if(i==320){
	if(icrit>0){
	  thisDDU->infpga_trap(INFPGA0);
	  *out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << std::endl;
	  sprintf(buf,"infpga0 diagnostic trap:");
	  sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->infpga_shift0=0xFACE;
	}
      }
      if(i==321){
	sprintf(buf,"infpga0 reset: ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
      }

      if(i==321){
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
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond2==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else if(icond2==3){
	*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf4 << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	*out << " **CAEN Error " << cgicc::span();
      }
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=321){
        sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
        *out << buf << cgicc::span();
      }

      if(i==321){
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
	*out << cgicc::form() << std::endl;
      }else if(i==300&&(stat&0x0000f000)>0){
        *out << "<blockquote><font size=-1 color=red face=arial>";
	if((stat&0xF0000000)>0){
	  if((0x80000000&stat)>0) *out << " DLL2 Lock Error &nbsp ";
	  // if((0x80000000&stat)>0) *out << " DLL-2 Not Locked &nbsp ";
	  // if((0x40000000&stat)>0) *out << " DLL-1 Not Locked &nbsp ";
	  if((0x40000000&stat)>0) *out << " 64-bit Filler was used &nbsp ";
	  if((0x20000000&stat)>0) *out << " RdCtrl-1 Not Ready &nbsp ";
	  if((0x10000000&stat)>0) *out << " RdCtrl-0 Not Ready";
	  *out << br();
	}
	if((stat&0x0F000000)>0){
	  if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber 0 or 1</font> &nbsp ";
	  if((0x04000000&stat)>0) *out << " <font color=blue>DLL Error occurred</font> &nbsp ";
	  if((0x02000000&stat)>0) *out << " <font color=black>InRD1 DMB Warn</font> &nbsp ";
	  if((0x01000000&stat)>0) *out << " <font color=black>InRD0 DMB Warn</font>";
	  *out << br();
	}
	if((stat&0x00F00000)>0){
	  if((0x00800000&stat)>0) *out << " <font color=blue>InRD1 DMB Full</font> &nbsp ";
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
	  if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
	  *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>RX Error occurred</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=blue>Special Word voted-bit warning</font>";
	  *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " Stuck Data occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " Timeout occurred &nbsp ";
	  if((0x00000020&stat)>0) *out << " Multiple voted-bit Errors &nbsp ";
	  if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
	  *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
	  if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
	  if((0x00000002&stat)>0) *out << " <font color=orange>L1A Match Error</font> &nbsp ";
	  if((0x00000001&stat)>0) *out << " Not Ready Error";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==320 && icrit>0){
	*out << "<blockquote><font size=-2 color=black face=arial>";
	DDUinTrapDecode(in, out, thisDDU->fpga_lcode);
	*out << "</font></blockquote>";
      }else if(i==310&&(stat&0xff00)>0){
	*out << std::endl ;
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00005500)>0){
	  *out << " &nbsp InRD0 Status: &nbsp <font color=blue>";
	  if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
	if((stat&0x0000AA00)>0){
	  *out << " &nbsp InRD1 Status: &nbsp <font color=blue>";
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
	  if((0x000004&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
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
	  if((0x00000400&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	*out  << "</font></blockquote>" << br();
      }else{
	*out << br() << std::endl;
      }
    }

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

    char buf[300],buf2[300],buf3[300],buf4[30];
    sprintf(buf,"DDU INFPGA1, VME  Slot %d",thisDDU->slot());
    *out << "<h2 align=center><font color=blue>" << buf << "</font></h2>" << std::endl;
    
    for(int i=400;i<422;i++){
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
           sprintf(buf,"infpga1 L1 Event Scaler0 bits[23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
	   stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
           sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
      }
      if(i==402){
	thisDDU->infpga_rd1scaler(INFPGA1);
           sprintf(buf,"infpga1 L1 Event Scaler1 bits[23-0]:");
           sprintf(buf2," %02X%04X ",thisDDU->infpga_code1,thisDDU->infpga_code0);
	   stat=((thisDDU->infpga_code1)<<16)+thisDDU->infpga_code0;
           sprintf(buf4," = %8lu Dec",stat&0x00ffffff);
      }
      if(i==403){
	thisDDU->infpga_DMBwarn(INFPGA1);
       	*out << br() << " <font color=blue> Fiber Registers below flag which CSCs experienced each condition since last Reset</font>" << br() << std::endl;
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
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==406){
	thisDDU->infpga_RxErr(INFPGA1);
	sprintf(buf,"infpga1 DDU GT-Rx Error, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp DDU Timeout-start, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0xff00)icond2=3;
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      }
      if(i==407){
	thisDDU->infpga_Timeout(INFPGA1);
	sprintf(buf,"infpga1 DDU Timeout-EndBusy, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=2;
	sprintf(buf3," &nbsp &nbsp DDU Timeout-EndWait, F[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=2;
      } 
      if(i==408){
	thisDDU->infpga_XmitErr(INFPGA1);
	sprintf(buf,"infpga1 SCA Full history, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	sprintf(buf3," &nbsp &nbsp CSC Transmit Error, Fiber[14-8]:");
	sprintf(buf4," %02X ",(thisDDU->infpga_code0&0x00ff));
	if(thisDDU->infpga_code0&0x00ff)icond2=1;
      }
      if(i==409){
	thisDDU->infpga_LostErr(INFPGA1);
	sprintf(buf,"infpga1 DDU LostInEvent Error, Fiber[14-8]:");
	sprintf(buf2," %02X ",(thisDDU->infpga_code0&0xff00)>>8);
	if(thisDDU->infpga_code0&0xff00)icond=1;
	sprintf(buf3," &nbsp &nbsp DDU LostInData Error, F[14-8]:");
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
	if(0xfc00&stat)icond2=1;
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
      if(i==413){
       	*out << br() << " <font color=blue> Each MemCtrl unit has a pool of 22 internal FIFOs</font>" << br() << std::endl;
	thisDDU->infpga_FiberDiagA(INFPGA1);
	sprintf(buf,"infpga1 MemCtrl-2 #FIFOs Used, Fibers 11-8: ");
	sprintf(buf2,"%ld, %ld, %ld, %ld <br>",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x001f0000&thisDDU->fpga_lcode[0])>>16,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
	thisDDU->infpga_FiberDiagB(INFPGA1);
	sprintf(buf3,"infpga1 MemCtrl-3 #FIFOs Used, Fibers 14-12: ");
	sprintf(buf4,"%ld, %ld, %ld",(0x1f000000&thisDDU->fpga_lcode[0])>>24,(0x00001f00&thisDDU->fpga_lcode[0])>>8,0x0000001f&thisDDU->fpga_lcode[0]);
      }
      if(i==414){
	thisDDU->infpga_MemAvail(INFPGA1);
	sprintf(buf,"infpga1 Current FIFO Memory Available:");
	sprintf(buf2,"MemCtrl-2 = %2d free, ",thisDDU->infpga_code0&0x001f);
	sprintf(buf4," &nbsp MemCtrl-3 = %2d free",thisDDU->infpga_code1);
	if(thisDDU->infpga_code0==1)icond=1;
	if(thisDDU->infpga_code0==0)icond=2;
	if(thisDDU->infpga_code1==1)icond2=1;
	if(thisDDU->infpga_code1==0)icond2=2;
      }
      if(i==415){
	thisDDU->infpga_Min_Mem(INFPGA1);
	sprintf(buf,"infpga1 Minimum FIFO Memory Availabile:");
	sprintf(buf2,"MemCtrl-2 min = %d free, ",thisDDU->infpga_code0&0x001f);
	sprintf(buf4," &nbsp MemCtrl-3 min = %d free",thisDDU->infpga_code1);
	if(thisDDU->infpga_code0==1)icond=1;
	if(thisDDU->infpga_code0==0)icond=2;
	if(thisDDU->infpga_code1==1)icond2=1;
	if(thisDDU->infpga_code1==0)icond2=2;
      }

      if(i==416){
	thisDDU->infpga_WrMemActive(INFPGA1,0);
	sprintf(buf,"infpga1 Write Memory Active 8-9: ");
	sprintf(buf2," Fiber 8 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 8 Unused, &nbsp ");
	sprintf(buf4," Fiber 9 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 9 Unused");
      }
      if(i==417){
	thisDDU->infpga_WrMemActive(INFPGA1,1);
	sprintf(buf,"infpga1 Write Memory Active 10-11: ");
	sprintf(buf2," Fiber 10 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 10 Unused, &nbsp ");
	sprintf(buf4," Fiber 11 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 11 Unused");
      }
      if(i==418){
	thisDDU->infpga_WrMemActive(INFPGA1,2);
	sprintf(buf,"infpga1 Write Memory Active 12-13: ");
	sprintf(buf2," Fiber 12 writing to %02Xh, &nbsp ",thisDDU->infpga_code0);
	if((thisDDU->infpga_code0&0x001f)==0x001f)sprintf(buf2," Fiber 12 Unused, &nbsp ");
	sprintf(buf4," Fiber 13 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf4," Fiber 13 Unused");
      }
      if(i==419){
	thisDDU->infpga_WrMemActive(INFPGA1,3);
	sprintf(buf,"infpga1 Write Memory Active 14: ");
	sprintf(buf2," Fiber 14 writing to %02Xh",thisDDU->infpga_code1);
	if((thisDDU->infpga_code1&0x001f)==0x001f)sprintf(buf2," Fiber 14 Unused");
      }
      if(i==420){
	if(icrit>0){
	  thisDDU->infpga_trap(INFPGA1);
	  *out << br() << " <font color=blue> Diagnostic data only valid after Critical Error, traps conditions at that instant</font>" << br() << std::endl;
	  sprintf(buf,"infpga1 diagnostic trap:");
	  sprintf(buf2," %08lX %08lX %08lX %08lX %08lX %08lX",thisDDU->fpga_lcode[5],thisDDU->fpga_lcode[4],thisDDU->fpga_lcode[3],thisDDU->fpga_lcode[2],thisDDU->fpga_lcode[1],thisDDU->fpga_lcode[0]);
	}else{
	  sprintf(buf," ");
	  sprintf(buf2," ");
	  thisDDU->infpga_shift0=0xFACE;
	}
      }
      if(i==421){
	sprintf(buf,"infpga1 reset: ");
	sprintf(buf2," EXPERT ONLY! ");
	icond=2;
      }

      if(i==421){
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
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond2==2){
	*out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else if(icond2==3){
	*out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{ 
	*out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf4 << cgicc::span();
      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	*out << " **CAEN Error " << cgicc::span();
      }
      if((thisDDU->infpga_shift0!=0xFACE)&&i!=421){
        sprintf(buf," **JTAG Error, Shifted:%04X",thisDDU->infpga_shift0);
	*out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
        *out << buf << cgicc::span();
      }

      if(i==421){
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
	  if((0x80000000&stat)>0) *out << " DLL-2 Lock Error &nbsp ";
	  // if((0x80000000&stat)>0) *out << " DLL-2 Not Locked &nbsp ";
	  // if((0x40000000&stat)>0) *out << " DLL-1 Not Locked &nbsp ";
	  if((0x40000000&stat)>0) *out << " 64-bit Filler was used &nbsp ";
	  if((0x20000000&stat)>0) *out << " RdCtrl-3 Not Ready &nbsp ";
	  if((0x10000000&stat)>0) *out << " RdCtrl-2 Not Ready";
	  *out << br();
	}
	if((stat&0x0F000000)>0){
	  if((0x08000000&stat)>0) *out << " <font color=blue>NoLiveFiber 0 or 1</font> &nbsp ";
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
	  if((0x00002000&stat)>0) *out << " <font color=blue>Single warning, possible data problem</font> &nbsp ";
	  if((0x00001000&stat)>0) *out << " <font color=blue>Near Full Warning</font>";
	  *out << br();
	}
	if((stat&0x00000F00)>0){
	  if((0x00000800&stat)>0) *out << " <font color=blue>RX Error occurred</font> &nbsp ";
	  if((0x00000400&stat)>0) *out << " <font color=blue>DLL Error (recent)</font> &nbsp ";
	  if((0x00000200&stat)>0) *out << " <font color=orange>SCA Full detected</font> &nbsp ";
	  if((0x00000100&stat)>0) *out << " <font color=blue>Special Word voted-bit warning</font>";
	  *out << br();
	}
	if((stat&0x000000F0)>0){
	  if((0x00000080&stat)>0) *out << " Stuck Data occurred &nbsp ";
	  if((0x00000040&stat)>0) *out << " Timeout &nbsp ";
	  if((0x00000020&stat)>0) *out << " Multiple voted-bit Errors &nbsp ";
	  if((0x00000010&stat)>0) *out << " Multiple Transmit Errors";
	  *out << br();
	}
	if((stat&0x0000000F)>0){
	  if((0x00000008&stat)>0) *out << " Mem/FIFO Full Error &nbsp ";
	  if((0x00000004&stat)>0) *out << " Fiber Error &nbsp ";
	  if((0x00000002&stat)>0) *out << " <font color=orange>L1A Match Error</font> &nbsp ";
	  if((0x00000001&stat)>0) *out << " Not Ready Error";
	  *out << br();
	}
        *out << "</font></blockquote>";
      }else if(i==420 && icrit>0){
	//	*out << "<blockquote><font size=-1 color=black face=arial>Trap decode goes here" << "</font></blockquote>";
	*out << "<blockquote><font size=-2 color=black face=arial>";
	DDUinTrapDecode(in, out, thisDDU->fpga_lcode);
	*out << "</font></blockquote>";
      }else if(i==410&&(stat&0xff00)>0){
	*out << std::endl;
        *out << "<blockquote><font size=-1 color=black face=arial>";
	if((stat&0x00005500)>0){
	  *out << " &nbsp InRD2 Status: &nbsp <font color=blue>";
	  if((0x00004000&stat)>0) *out << " Ext.FIFO 3/4 Full &nbsp &nbsp ";
	  if((0x00001000&stat)>0) *out << " L1A FIFO Almost Full &nbsp &nbsp ";
	  if((0x00000400&stat)>0) *out << " MemCtrl Almost Full &nbsp &nbsp ";
	  *out << "</font>";
	  if((0x00000100&stat)>0) *out << " L1A FIFO Empty";
	  if((0x00000100&stat)==0) *out << " L1A FIFO Not Empty";
	  *out << br();
	}
	if((stat&0x0000AA00)>0){
	  *out << " &nbsp InRD3 Status: &nbsp <font color=blue>";
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
	  if((0x000004&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
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
	  if((0x00000400&stat)>0) *out << " Multi-Transmit Error &nbsp &nbsp ";
	  if((0x00000200&stat)>0) *out << " Stuck Data &nbsp &nbsp ";
	  if((0x00000100&stat)>0) *out << " Timeout";
	  *out  << "</font>" << br();
	}
	*out  << "</font></blockquote>" << br();
      }else{
	*out << br() << std::endl;
      }
    }
    //    *out << cgicc::fieldset() << std::endl; 
    *out << cgicc::body() << std::endl;    
    *out << cgicc::html() << std::endl;

      }
     catch (const std::exception & e )
       {
	 //XECPT_RAISE(xgi::exception::Exception, e.what());
       }
}


void DDUinTrapDecode(xgi::Input * in, xgi::Output * out,  unsigned long int lcode[10]) throw (xgi::exception::Exception)
{
  printf(" enter DDUinTrapDecode \n");
  int i;
  cgicc::Cgicc cgi(in);
  char buf[100],buf1[100],buf2[100],buf3[100],buf4[100];
  char cbuf1[20],cbuf2[20],cbuf3[20],cbuf4[20];
  char sred[20]="<font color=red>";
  char syel[20]="<font color=orange>";
  char sblu[20]="<font color=blue>";
  char sgrn[20]="<font color=green>";
  char snul[20]="</font>";
  sprintf(buf1," ");
  sprintf(buf2," ");
  sprintf(buf3," ");
  sprintf(buf4," ");
  sprintf(cbuf1," ");
  sprintf(cbuf2," ");
  sprintf(cbuf3," ");
  sprintf(cbuf4," ");

  *out << "<pre>" << std::endl;
  sprintf(buf,"  192-bit DDU InFPGA Diagnostic Trap (24 bytes) \n");
  *out << buf << std::endl;

  i=23;
  sprintf(buf,"                        LFfull MemAvail C-code End-TO");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0x000f8000&lcode[5]<3)sprintf(cbuf1,"%s",sblu);
  if(0x000f8000&lcode[5]<2)sprintf(cbuf1,"%s",syel);
  if((0x000f8000&lcode[5]<1)||(0xfff00000&lcode[5]))sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[5])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if((0x001f&lcode[5]<3)||(0x03e0&lcode[5]<3)||(0x7c00&lcode[5]<3)||(0x000f8000&lcode[5]<3))sprintf(cbuf2,"%s",sblu);
  if((0x001f&lcode[5]<2)||(0x03e0&lcode[5]<2)||(0x7c00&lcode[5]<2)||(0x000f8000&lcode[5]<2))sprintf(cbuf2,"%s",syel);
  if((0x001f&lcode[5]<1)||(0x03e0&lcode[5]<1)||(0x7c00&lcode[5]<1)||(0x000f8000&lcode[5]<1))sprintf(cbuf2,"%s",sred);
  if(0x000f8000&lcode[5]<3){
    sprintf(buf2,"%s    <blink>%04lx</blink>%s",cbuf2,0xffff&lcode[5],snul);
  }else{
    sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[5],snul);
  }
  sprintf(cbuf3,"%s",sgrn);
  if(0x20200000&lcode[4])sprintf(cbuf3,"%s",syel);
  if(0xdfdf0000&lcode[4])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[4])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0xffff&lcode[4])sprintf(cbuf4,"%s",sred);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[4],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

  i=15;
  sprintf(buf,"                      Start-TO FAF/Nrdy L1err  DMBwarn");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0xff000000&lcode[3])sprintf(cbuf1,"%s",sblu);
  if(0x00ff0000&lcode[3])sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[3])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if(0xfc00&lcode[3])sprintf(cbuf2,"%s",sblu);
  sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[3],snul);
  sprintf(cbuf3,"%s",sgrn);
  if(0x00ff0000&lcode[2])sprintf(cbuf3,"%s",syel);
  if(0xff000000&lcode[2])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[2])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0x00ff&lcode[2])sprintf(cbuf4,"%s",sblu);
  if(0xff00&lcode[2])sprintf(cbuf4,"%s",sred);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[2],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;

  i=7;
  sprintf(buf,"                        32-bit-Empty0M  32-bit-status");
  *out << buf << std::endl;
  sprintf(buf,"      rcv bytes %2d-%2d:",i,i-7);
  sprintf(cbuf1,"%s",sgrn);
  if(0xffff0000&lcode[1]==0xf8000000)sprintf(cbuf1,"%s",sred);
  sprintf(buf1,"%s   %04lx%s",cbuf1,(0xffff0000&lcode[1])>>16,snul);
  sprintf(cbuf2,"%s",sgrn);
  if(0xffff&lcode[1]==0xf800)sprintf(cbuf2,"%s",sred);
  sprintf(buf2,"%s    %04lx%s",cbuf2,0xffff&lcode[1],snul);
  sprintf(cbuf3,"%s",sgrn);
  if(0x0c080000&lcode[0])sprintf(cbuf3,"%s",sblu);
  if(0xf0660000&lcode[0])sprintf(cbuf3,"%s",sred);
  sprintf(buf3,"%s    %04lx%s",cbuf3,(0xffff0000&lcode[0])>>16,snul);
  sprintf(cbuf4,"%s",sgrn);
  if(0x2d00&lcode[0])sprintf(cbuf4,"%s",sblu);
  if(0x4202&lcode[0])sprintf(cbuf4,"%s",syel);
  if(0x80fd&lcode[0])sprintf(cbuf4,"%s",sred);
  sprintf(buf4,"%s   %04lx%s",cbuf4,0xffff&lcode[0],snul);
  *out << buf << buf1 << buf2 << buf3 << buf4 << std::endl;
  *out << "</pre>" << br() << std::endl;
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
    int ddu,stat,icond;
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
      icond=0;
      if(i==500){
        sprintf(buf,"VMEctrl Status Register:");
	stat=thisDDU->vmepara_status();
	DDU_FMM=(stat>>8)&0x000F;
	sprintf(buf2," %04X ",stat);
      }
      if(i==501){
       	*out << " <font color=blue> Items below have 1+15 bits to report FMM status: 1 flag for DDU plus 15 for its CSC Fibers</font>" << br() << std::endl;
	sprintf(buf,"CSC FMM Problem Report:");
	sprintf(buf2," %04X ",thisDDU->vmepara_CSCstat());
      }
      if(i==502){
	sprintf(buf,"CSC Busy:");
	stat=thisDDU->vmepara_busy();
	if(0xffff&stat)icond=1;
	sprintf(buf2," %04X ",stat);
      }
      if(i==503){
	sprintf(buf,"CSC Warn/Near Full:");
	stat=thisDDU->vmepara_fullwarn();
	if(0xffff&stat)icond=3;
	sprintf(buf2," %04X ",stat);
      }
      if(i==504){
	sprintf(buf,"CSC LostSync:");
	stat=thisDDU->vmepara_lostsync();
	if(0xffff&stat)icond=2;
	sprintf(buf2," %04X ",stat);
      }
      if(i==505){
	sprintf(buf,"CSC Error:");
	stat=thisDDU->vmepara_error();
	if(0xffff&stat)icond=2;
	sprintf(buf2," %04X ",stat);
      }
      if(i==506){
	sprintf(buf,"CSC Warn History:");
	stat=thisDDU->vmepara_warnhist();
	if(0xffff&stat)icond=3;
	sprintf(buf2," %04X ",stat);
      }
      if(i==507){
	sprintf(buf,"CSC Busy History:");
	stat=thisDDU->vmepara_busyhist();
	if(0xffff&stat)icond=1;
	sprintf(buf2," %04X ",stat);
      }
      if(i==508){
	*out << br() << " <font color=blue> 3 16-bit InRegisters, pipelined 0 ->> 1 ->> 2; use this to pre-load VMEserial writes</font>" << br() << std::endl;
	sprintf(buf,"Write to InReg0:");
	sprintf(buf2," ");
      }
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
      if(i==512){
	*out << br() << " <font color=blue> Set SPY Rate (bits 2:0); set Ignore DCC/S-Link Wait (bit 3) <br>  Rate 0-7 will transmit 1 event out of 1,8,32,128,1024,8192,32768,never</font>" << br() << std::endl;
	//	*out << br() << " <font color=blue> Select 0-7 for SPY rate = 1 per 1,8,32,128,1024,8192,32768,never</font>" << br() << std::endl;
	sprintf(buf,"GbE Prescale*:");
	sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_GbEprescale());
      }
      if(i==513){
	sprintf(buf,"Toggle DCC Wait/Backpressure Enable:");
	sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
      }
      if(i==514){
	*out << " <font color=blue> Enable Fake L1A/Data Passthrough for DDU FPGAs: bit 2=DDUctrl, 1=InFPGA1, 0=InFPGA0</font>" << br() << std::endl;
	sprintf(buf,"Fake L1A Reg*:");
	sprintf(buf2," %04X <font color=red> EXPERT ONLY! </font> ",thisDDU->vmepara_rd_fakel1reg());
      }
      if(i==515){
	sprintf(buf,"Toggle All FakeL1A:");
	sprintf(buf2," <font color=red> EXPERT ONLY! </font> ");
      }
      if(i==516){
	sprintf(buf,"Switches:");
	sprintf(buf2," %02X ",thisDDU->vmepara_switch()&0xff);
      }
      if(i==517){
	*out << hr() << " <font color=blue> Items below are Only for Board Testing, Do Not Use!</font>" << br() << std::endl;
	//	*out << br() << " <font color=blue> Only for 4-bit FMM function testing, Do Not Use!</font>" << br() << std::endl;
	sprintf(buf,"'F0E' + 4-bit FMM Reg:");
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
      if(i==521){
	sprintf(buf,"Test Reg3:");
	sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==522){
	sprintf(buf,"Test Reg4:");
	sprintf(buf2," %04X ",thisDDU->vmepara_rd_testreg0());
      }
      if(i==508||i==512||i==514||i==517||i==515||i==513){
         std::string ddutextload =
	 toolbox::toString("/%s/DDUTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",ddutextload);
      }
      *out << cgicc::span().set("style","color:black");
      *out << buf << cgicc::span();
      if(icond==1){
        *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
      }else if(icond==2){
        *out << cgicc::span().set("style","color:red;background-color:#dddddd;");
      }else if(icond==3){
        *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
      }else{
        *out << cgicc::span().set("style","color:green;background-color:#dddddd;");
      }
      *out << buf2 << cgicc::span();

      if(thisDDU->CAEN_err()!=0){
	*out << cgicc::span().set("style","color:yellow;background-color:#dddddd;");
	*out << " **CAEN Error " << cgicc::span();
      }
      if(i==500){
	*out << "<font color=black> &nbsp &nbsp " << "Current DDU FMM Report: </font>";
	sprintf(buf2," READY ");
	if(DDU_FMM==4){   // Busy
	  *out << cgicc::span().set("style","color:orange;background-color:#dddddd;");
	  sprintf(buf2," BUSY ");
	}
	else if(DDU_FMM==1){    // Warn, near full: reduce trigger rate
	  *out << cgicc::span().set("style","color:blue;background-color:#dddddd;");
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

	sprintf(buf2,"<blockquote><font size=-1 color=black face=arial> slot=%d &nbsp &nbsp ",((~stat)&0x001F));
	*out << buf2;
	if(stat&0x0080) *out << " VME DLL-2 Not Locked &nbsp ";
	if(stat&0x0040) *out << " VME DLL-1 Not Locked &nbsp ";
	if((stat&0x000000C0)&&(stat&0x0000F000)) *out << br();
	if(stat&0x8000) *out << " VME FPGA detects a problem &nbsp ";
	if(stat&0x4000) *out << " &nbsp VME FPGA has a clock problem &nbsp ";
	if(stat&0x2000) *out << " &nbsp VME FPGA is Not Ready &nbsp ";
	if(stat&0x1000) *out << " &nbsp DDU is Not Ready";
	*out << "</font></blockquote>";
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
         *out << cgicc::form() << std::endl;
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
	if (istat&0x003c!=0x000c)sprintf(buf2,"<font color=orange>");
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
	if (radc<1450||radc>1550)sprintf(buf2,"<font color=orange>");
	if (radc<1400||radc>1600)sprintf(buf2,"<font color=red>");
	*out << "Voltages: V15=" << buf2 << buf;

	radc=thisDDU->adcplus(1,5);
	sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<2450||radc>2550)sprintf(buf2,"<font color=orange>");
	if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
	*out << "V25=" << buf2 << buf;

	radc=thisDDU->adcplus(1,6);
	sprintf(buf,"%5.fmV</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<2450||radc>2550)sprintf(buf2,"<font color=orange>");
	if (radc<2400||radc>2600)sprintf(buf2,"<font color=red>");
	*out << "V25A=" << buf2 << buf;

	radc=thisDDU->adcplus(1,7);
	sprintf(buf,"%5.fmV</font>",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<3250||radc>3400)sprintf(buf2,"<font color=orange>");
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
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
	*out << "Temperatures: Sensor0=" << buf2 << buf;

	radc=thisDDU->readthermx(1);
	sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"color=orange>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"color=red>");
	*out << "Sensor1=<font " << buf2 << buf;

	radc=thisDDU->readthermx(2);
	sprintf(buf,"%5.2fF</font> &nbsp &nbsp ",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
	if (radc<35.0||radc>110.0)sprintf(buf2,"<font color=red>");
	*out << "Sensor2=" << buf2 << buf;

	radc=thisDDU->readthermx(3);
	sprintf(buf,"%5.2fF</font>",radc);
	sprintf(buf2,"<font color=green>");
	if (radc<60.0||radc>99.0)sprintf(buf2,"<font color=orange>");
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
    if(val==227)thisDDU->ddu_l1calonoff();
    if(val==228)thisDDU->ddu_reset();
    if(val==222){
      sscanf(XMLtext.data(),"%01hhx%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1],&thisDDU->snd_serial[2]);
      send_val=((thisDDU->snd_serial[0]<<16)&0x000f0000)|((thisDDU->snd_serial[1]<<8)&0x0000ff00)|(thisDDU->snd_serial[2]&0x0000ff);
      printf(" loadkillfiber send %05lx \n",send_val);
      thisDDU->ddu_loadkillfiber(send_val);
   }
   if(val==223){
     sscanf(XMLtext.data(),"%01hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
     para_val=((thisDDU->snd_serial[0]<<8)&0x00000f00)|(thisDDU->snd_serial[1]&0x0000ff);
     printf(" ldbxorbit send %d (%03xh) \n",para_val,para_val);
     thisDDU->ddu_loadbxorbit(para_val);
   }
   if(val==226)thisDDU->ddu_vmel1a(); 
   if(val==321)thisDDU->infpga_reset(INFPGA0);
   if(val==421)thisDDU->infpga_reset(INFPGA1);
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
    if(val==513){
        unsigned short int scale= thisDDU->vmepara_rd_GbEprescale();
        if (scale&0x0008) { 
           para_val=(scale&0xF7F7)|0x8080;
        } else {
           para_val=(scale|0x0808)&0x7f7f;
        }
	printf("          vmepara_wr_GbEprescale, para_val=%d \n",para_val);
	thisDDU->vmepara_wr_GbEprescale(para_val);
    } 
    if(val==514){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fakel1reg(para_val);

    }
    if(val==515){
	unsigned short int scale2=thisDDU-> vmepara_rd_fakel1reg();
        if (scale2&0x0007) {
          para_val=0xF0F0;
        } else {
          para_val=0x8787;
        }
	printf("         vmepara_wr_fakel1reg, para_val=%d \n",para_val);
	thisDDU->vmepara_wr_fakel1reg(para_val);
    }
    if(val==517){
      sscanf(XMLtext.data(),"%02hhx%02hhx",&thisDDU->snd_serial[0],&thisDDU->snd_serial[1]);
      para_val=((thisDDU->snd_serial[0]<<8))&0xff00|(thisDDU->snd_serial[1]&0x00ff);
      printf(" para_val %04x \n",para_val);
      thisDDU->vmepara_wr_fmmreg(para_val);
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
    unsigned int FEDVME_CSCstat=0;
    long int xsec,xmin,xhr;
    if(name != cgi.getElements().end()) {
      ddu = cgi["ddu"]->getIntegerValue();
      //      cout << "DDU inside " << ddu << endl;
      DDU_ = ddu;
    }else{
      ddu=DDU_;
    }

    if(irq_start[0]==0){   // if IRQ service is not active then start it up
      VMEController *theController=thisCrate->vmeController();
      theController->irq_pthread_start(0);
// only get cleared if VMEirq service is started via Button:
      timer=0;
      xtimer=0;
      irqprob=0;  // bad idea?
      printf(" <--DEBUG-->  VMEIntIRQ, pt0: timer & irqprob set to zero <----> \n");
      for(int i=0;i<(int)dduVector.size();i++){
	thisDDU=dduVector[i];
	int slot=thisDDU->slot();
        if(slot<21){
	  FEDVME_CSCstat=thisDDU->vmepara_CSCstat();
	  if(FEDVME_CSCstat!=0x0000){
// only gets set if there's a previous problem at VMEirq service startup:
	    irqprob=1;
	    printf(" <--DEBUG-->  VMEIntIRQ, pt1: irqprob set to 1 <----> \n");
	    //	    printf(" ** EmuFEDVME: Interrupt was set for Slot %d, CSCstat=0x%04x ** \n",slot,FEDVME_CSCstat);
	  }
        }
      }
    }


    thisDDU = dduVector[0];
    //   printf(" set up web page \n");
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << meta().set("http-equiv","refresh").set("content","5") << std::endl;
    *out << cgicc::title("VMEIRQ Web Form") << std::endl;
    //  
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg") << std::endl;

    char buf[300];
    sprintf(buf,"DDU VME IRQ Interrupt Monitor");
    //
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;

    if(ddu!=99){
      *out << cgicc::legend(buf).set("style","color:blue")  << std::endl ;
      unsigned int FEDVME_int=thisDDU->irq_tester(0,0);
      int FEDVMEslot=thisDDU->irq_tester(0,2);
      if(FEDVME_int!=0 || irqprob!=0){ 
	printf(" <--DEBUG-->  VMEIntIRQ, pt2: DDUslot=%d   FEDVMEint=%04xh   irqprob=%d <----> \n",FEDVMEslot,FEDVME_int,irqprob);
	*out << img().set("src","http://www.physics.ohio-state.edu/~durkin/xdaq_files/redlight.gif") << std::endl;
	//	LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Interrupt xx detected");
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

/*
	long int xtimer=timer;
	long int xsec=xtimer%60;
	xtimer=(xtimer-xsec)/60;
	long int xmin=xtimer%60;
	long int xhr=(xtimer-xmin)/60;
*/
//	xtimer=timer;
	timer=xtimer;
	xsec=timer%60;
	timer=(timer-xsec)/60;
	xmin=timer%60;
	xhr=(timer-xmin)/60;
	timer=0;
	printf(" <--DEBUG-->  VMEIntIRQ, pt5: timer set to zero, xtimer=%d <----> \n",xtimer);

	sprintf(buf,"%02ld:%02ld:%02ld",xhr,xmin,xsec);
	*out << buf << std::endl;
	*out << cgicc::span() << std::endl;

	if(FEDVME_int!=0){
	printf(" <--DEBUG-->  VMEIntIRQ, pt3: DDUslot=%d   FEDVMEint=%04xh <----> \n",FEDVMEslot,FEDVME_int);
	  //	  printf(" ** EmuFEDVME: Interrupt detected, 0x%04x ** \n",FEDVME_int);
	  *out << cgicc::span().set("style","font-size: 20pt;color:red");
	  printf("     *** EmuFEDVME: Failure in Crate %d, Slot %d:  Status %04xh ***\n",thisDDU->irq_tester(0,1),thisDDU->irq_tester(0,2),thisDDU->irq_tester(0,3));
	  sprintf(buf,"  Failure in Crate %d, Slot %d:  Status %04xh",thisDDU->irq_tester(0,1),thisDDU->irq_tester(0,2),thisDDU->irq_tester(0,3));
	  //	LOG4CPLUS_INFO(getApplicationLogger(), buf);
	}else{
	  printf(" <--DEBUG-->  VMEIntIRQ, pt4: DDUslot=%d   FEDVMEint=%04xh   irqprob=%d <----> \n",FEDVMEslot,FEDVME_int,irqprob);
	  *out << cgicc::span().set("style","font-size: 20pt;color:red");
	  sprintf(buf," DDUs set Interrupt previously, still in error state");
	}
	*out << buf << std::endl;
      }else{ 
	*out << cgicc::span().set("style","color:green");
	*out << "Time without interrupt" << std::endl;
	*out << cgicc::span() << std::endl;
	*out << cgicc::span().set("style","font-size: 25pt;color:white;background-color:black;");
	//  printf(" timer %ld start %d irq_start \n",timer,irq_start[0]);

/*
	long int xtimer=timer;
	long int xsec=xtimer%60;
	xtimer=(xtimer-xsec)/60;
	long int xmin=xtimer%60;
	long int xhr=(xtimer-xmin)/60;
*/
	xtimer=timer;
	xsec=xtimer%60;
	xtimer=(xtimer-xsec)/60;
	xmin=xtimer%60;
	xhr=(xtimer-xmin)/60;

	sprintf(buf,"%02ld:%02ld:%02ld",xhr,xmin,xsec);
	*out << buf << std::endl;
	timer=timer+2;
	xtimer=timer;
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
    unsigned long int tuscode[2]={0xdcc30081,0xdcc310A8};
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
      //    }else if(uscode!=tuscode[i]){
    }else if(0xffff0000&uscode!=0xffff0000&tuscode[i]){
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


void EmuFCrateHyperDAQ::LoadXMLconf(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
    printf(" enter: LoadXMLconf \n");
    cgicc::Cgicc cgi(in);
    //    const CgiEnvironment& env = cgi.getEnvironment();
    //    string URLname = cgi["DefURL"]->getValue() ; 
    //    cout << "  should go to this URL: " << URLname  << endl ;
    printf(" dduVector.size() %d \n",dduVector.size());
    printf(" dccVector.size() %d \n",dccVector.size());
    //    dduVector.size()=0;  // How to set these to zero?!?
    //    dccVector.size()=0;  // Then go to   this->Default(in.out);
    reload=1;

    std::string loadxmlconf =
      toolbox::toString("/%s/",getApplicationDescriptor()->getURN().c_str());
    //    cout<<endl<<"  JRGdebug: LoadXMLconf string="<<loadxmlconf<<endl;
    this->Default(in,out);
}

void EmuFCrateHyperDAQ::DCCCommands(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
{
    printf(" enter: DCC Commands \n");
    cgicc::Cgicc cgi(in);
    const CgiEnvironment& env = cgi.getEnvironment();
    std::string crateStr = env.getQueryString();
    cout << crateStr << endl;
    cgicc::form_iterator name = cgi.getElement("dcc");
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
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCC Comands Web Form") << std::endl;
    *out << body().set("background","http://www.physics.ohio-state.edu/~durkin/xdaq_files/bgndcms.jpg");

    char buf[300],buf2[300],buf3[300];
    sprintf(buf,"DCC Commands VME  Slot %d",thisDCC->slot());
    *out << cgicc::fieldset().set("style","font-size: 13pt; font-family: arial;");
    *out << std::endl;
    *out << cgicc::legend(buf).set("style","color:blue")  << std::endl;
    int igu;
    for(int i=100;i<109;i++){
      thisDCC->CAEN_err_reset();
      sprintf(buf3," ");
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
	   sprintf(buf3,"<font size=-1> (selects which DDUs must be processed by DCC)</font>");
      }
      if(i==104){
           unsigned short int ttccmd=thisDCC->mctrl_rd_ttccmd();
           sprintf(buf,"TTC Command:");
	   sprintf(buf2," %04X ",(ttccmd>>2)&0x3f);
	   //           sprintf(buf2," ");
	   sprintf(buf3,"<font size=-1> (only works when TTC fiber input to DCC is disabled)</font>");
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

      if (i==107) {
	   sprintf(buf,"Date Rate Slink0:");
	   unsigned short int status[6];
	   int dr[6];
	   for (igu=0;igu<6;igu++) {
	     status[igu]=thisDCC->mctrl_ratemon(igu);
	     dr[igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
	   }
           sprintf(buf2," %d  ddu3: %d  ddu13: %d  ddu4: %d  ddu12 %d  ddu5: %d",dr[0],dr[1],dr[2],dr[3],dr[4],dr[5]);
      }
      if (i==108) {
	sprintf(buf,"Date Rate Slink1:");
	unsigned short int status[6];
	int dr[6];
	for (igu=6;igu<12;igu++) {
	  status[igu-6]=thisDCC->mctrl_ratemon(igu);
	  dr[igu-6]=((status[igu-6]&0x3fff)<<(((status[igu-6]>>14)&0x3)*4));
	}
	sprintf(buf2," %d  ddu11: %d  ddu6: %d  ddu10: %d  ddu7: %d  ddu9: %d",dr[0],dr[1],dr[2],dr[3],dr[4],dr[5]);
      }


      if(i>100 && i<107){
         std::string dcctextload =
	 toolbox::toString("/%s/DCCTextLoad",getApplicationDescriptor()->getURN().c_str());
         *out << cgicc::form().set("method","GET").set("action",dcctextload)
	   //	   .set("style","margin-bottom: 0") 
	      << std::endl;
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
      if(i>100&&i<107){
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
	if(i==105)xmltext="2,5";
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
	*out << buf3 << cgicc::form() << std::endl;
      }else{
	*out << br() << std::endl;
      }
      if(i==103){
	*out << "<blockquote><font size=-1 face=arial>";
	*out << "DCC-FIFO bits map to DDU-Slots" << br();
	*out << " &nbsp &nbsp &nbsp &nbsp b0=Slot3, &nbsp b1=Slot13, &nbsp b2=Slot4, &nbsp b3=Slot12, &nbsp b4=Slot5 &nbsp ---->> Top S-Link" << br();
	*out << " &nbsp &nbsp &nbsp &nbsp b5=Slot11, &nbsp b6=Slot6, &nbsp b7=Slot10, &nbsp b8=Slot7, &nbsp b9=Slot9 &nbsp ---->> Bottom S-Link";
	*out << "</font></blockquote>" << std::endl;
      }
      if(i==104){
	  *out << "<blockquote><font size=-1 face=arial>";
	  *out << "Command Code examples (hex):" << br();
	  *out << " &nbsp &nbsp &nbsp &nbsp 3=SyncRst, &nbsp 4=HardRst, &nbsp 1C=SoftRst";
	  *out << "</font></blockquote>" << std::endl;
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
    unsigned short para_val;
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
    unsigned int tim=10;
    sleep(tim);
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
    unsigned int tim=1;
    sleep(tim);
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


void EmuFCrateHyperDAQ::DCCRateMon(xgi::Input * in, xgi::Output * out )
     throw (xgi::exception::Exception)
{
    cgicc::CgiEnvironment cgiEnvi(in);
    std::string Page=cgiEnvi.getPathInfo()+"?"+cgiEnvi.getQueryString();
    //
    *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"30; URL=/"
         <<getApplicationDescriptor()->getURN()<<"/"<<Page<<"\">" <<endl;
 

  //
  *out << "<HTML>" <<std::endl;
  *out << "<BODY bgcolor=\"#FFFFFF\">" <<std::endl;
 
    thisDCC=dccVector[0];
    unsigned short int status[12];
                                            
    for(int igu=0;igu<12;igu++) {
      status[igu]=thisDCC->mctrl_ratemon(igu);
      DCC_ratemon[DCC_ratemon_cnt][igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
    }
    DCC_ratemon_cnt=DCC_ratemon_cnt+1;
    if(DCC_ratemon_cnt>49)DCC_ratemon_cnt=0;
    for(int igu=0;igu<12;igu++) {
      DCC_ratemon[DCC_ratemon_cnt%50][igu]=0;
      DCC_ratemon[(DCC_ratemon_cnt+1)%50][igu]=0;
    }
 
    *out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<std::endl;
    *out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<std::endl;
    ostringstream output;
    output << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate0" << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    *out << output.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //
    ostringstream output2;
    output2 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate0"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    //
    *out << output2.str() << std::endl;
    *out << "</OBJECT>" << std::endl;

    *out << "<OBJECT classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" WIDTH=\"565\" HEIGHT=\"420\" id=\"FC_2_3_MSLine\">" <<std::endl;
    *out << "<PARAM NAME=movie VALUE=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\">" <<std::endl;
    ostringstream output3;
    output3 << "<PARAM NAME=\"FlashVars\" VALUE=\"&dataURL=getDataDCCRate1" << "&chartWidth=565&chartHeight=420"<<"\">"<<std::endl;
    *out << output3.str() << std::endl ;
    *out << "<PARAM NAME=quality VALUE=high>" << std::endl ;
    *out << "<PARAM NAME=bgcolor VALUE=#FFFFFF>" << std::endl ;
    //
    ostringstream output4;
    output4 << "<EMBED src=\"/daq/extern/FusionCharts/Charts/FC_2_3_MSLine.swf\" FlashVars=\"&dataURL=getData"<< "DCCRate1"<<"\" quality=high bgcolor=#FFFFFF WIDTH=\"565\" HEIGHT=\"420\" NAME=\"FC_2_3_MSLine\" TYPE=\"application/x-shockwave-flash\" PLUGINSPAGE=\"http://www.macromedia.com/go/getflashplayer\"></EMBED>" << std::endl;
    //
    *out << output4.str() << std::endl;
    *out << "</OBJECT>" << std::endl;
 



    *out << "</BODY>" << std::endl;
    *out << "</HTML>" << std::endl;
 
}

  void EmuFCrateHyperDAQ::getDataDCCRate0(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
      //
      *out << "<graph caption='DCC S-Link 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //      *out << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //std::cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      *out << "<categories>" << std::endl;
      //std::cout << "<categories>" << std::endl;
      for(int i=0;i<50;i++){
        *out << "<category name='"<<i<<"' hoverText='' />" << std::endl;
        //std::cout << "<category name='"<<i<<"' hoverText='' />" << std::endl;
      }
      *out << "</categories>" <<std::endl;
      //std::cout << "</categories>" <<std::endl;
      for(int ch=0;ch<6;ch++){
      if(ch==0)*out << "<dataset seriesName='S-Link 0 ' color='0080C0' showValues='0'>" <<std::endl;
      if(ch==1)*out << "<dataset seriesName='Slot 3 DDU ' color='008040' showValues='0'>" <<std::endl;
      if(ch==2)*out << "<dataset seriesName='Slot 13 DDU ' color='800080' showValues='0'>" <<std::endl;
      if(ch==3)*out << "<dataset seriesName='Slot 4 DDU ' color='FF8040' showValues='0'>" <<std::endl;
      if(ch==4)*out << "<dataset seriesName='Slot 12 DDU ' color='FFF000' showValues='0'>" <<std::endl;
      if(ch==5)*out << "<dataset seriesName='Slot 5 DDU ' color='FF0080' showValues='0'>" <<std::endl;
      for(unsigned int i=0;i<50;i++) {
        ostringstream output;
	// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
	output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
        *out << output.str() << std::endl ;
	//std::cout << output.str() << std::endl ;
      }
      *out << "</dataset>"<<std::endl;
      //std::cout << "</dataset>"<<std::endl;
      }
      *out << "</graph>" << std::endl;
      //std::cout << "</graph>" << std::endl;
    }
 
   void EmuFCrateHyperDAQ::getDataDCCRate1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
      //
      *out << "<graph caption='DCC S-Link 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //      *out << "<graph caption='DCC FPGA 1' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      //std::cout << "<graph caption='DCC FPGA 0' subcaption='' xAxisName='time' yAxisName='Rate' numberPrefix='' showNames='1' animation='0'>" << std::endl;
      *out << "<categories>" << std::endl;
      //std::cout << "<categories>" << std::endl;
      for(int i=0;i<50;i++){
        *out << "<category name='"<<i<<"' hoverText='' />" << std::endl;
        //std::cout << "<category name='"<<i<<"' hoverText='' />" << std::endl;
      }
      *out << "</categories>" <<std::endl;
      //std::cout << "</categories>" <<std::endl;
      for(int ch=6;ch<12;ch++){
      if(ch==6)*out << "<dataset seriesName='S-Link 1 ' color='0080C0' showValues='0'>" <<std::endl;
      if(ch==7)*out << "<dataset seriesName='Slot 11 DDU ' color='008040' showValues='0'>" <<std::endl;
      if(ch==8)*out << "<dataset seriesName='Slot 6 DDU ' color='800080' showValues='0'>" <<std::endl;
      if(ch==9)*out << "<dataset seriesName='Slot 10 DDU ' color='FF8040' showValues='0'>" <<std::endl;
      if(ch==10)*out << "<dataset seriesName=' Slot 7 DDU ' color='FFF000' showValues='0'>" <<std::endl;
      if(ch==11)*out << "<dataset seriesName='DDU Slot 9 DDU ' color='FF0080' showValues='0'>" <<std::endl;
      for(unsigned int i=0;i<50;i++) {
        ostringstream output;
	// output << "<set name='" << ch <<"'"<< " value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
	output << "<set value='" << DCC_ratemon[i][ch] << "'" << " />" << std::endl;
        *out << output.str() << std::endl ;
	//std::cout << output.str() << std::endl ;
      }
      *out << "</dataset>"<<std::endl;
      //std::cout << "</dataset>"<<std::endl;
      }
      *out << "</graph>" << std::endl;
      //std::cout << "</graph>" << std::endl;
    }
 


};
#endif
