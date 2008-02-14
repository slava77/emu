 

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _DcsControlWebXdaq_h_
#define _DcsControlWebXdaq_h_

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>


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



#include "dim/dis.hxx"
#include "Rc2Dcs.h" 

/*
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
*/

using namespace cgicc;
using namespace std;

class DcsControlWebXdaq: public xdaq::Application 
{
private:
  //
protected:
  //

string slow_control_select;
string detector_select;
string turn_select;
 Rc2Dcs *rc2dcs;
  /*
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
  ostringstream OutputStringTMBStatus[9];
  ostringstream OutputDMBTests[9];
  vector<TMB*>   tmbVector;
  vector<DAQMB*> dmbVector;
  Crate *thisCrate;
  std::string Operator_;
  std::string MPCBoardID_;
  std::string CCBBoardID_;
  std::string DMBBoardID_[9];
  std::string TMBBoardID_[9];
  int TMB_, DMB_;
  */
  //
public:
  //
  DcsControlWebXdaq(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    xgi::bind(this,&DcsControlWebXdaq::Default, "Default");
    xgi::bind(this,&DcsControlWebXdaq::selectSlowControl, "selectSlowControl");
    xgi::bind(this,&DcsControlWebXdaq::selectHighVoltage, "selectHighVoltage");

    rc2dcs= new Rc2Dcs(); 

  }
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web") << std::endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    ////        DimService dd;

    *out << cgicc::h1("Please select SLOW CONTROL");

      std::string method =
	toolbox::toString("/%s/selectSlowControl",getApplicationDescriptor()->getURN().c_str());

      //      //-----------------------------
  
      *out << cgicc::form().set("method","GET").set("action",method)  << std::endl ;
      /*
      *out << cgicc::input().set("type","text")
	.set("name","xmlFilename")
	.set("size","60")
	.set("ENCTYPE","multipart/form-data")
	.set("value","temp");
      //
      *out << std::endl;
      //
      */


      *out << cgicc::select().set("name","select_sc") << std::endl ;
      //--
      *out << cgicc::option().set("value","hv")<< std::endl ;
      *out << "HV";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","lv")<< std::endl ;
      *out << "LV: on-csc boards";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","mrtn")<< std::endl ;
      *out << "LV: Wiener Maratons";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","crb")<< std::endl ;
      *out << "LV: PeripheralCrate boards";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","temp")<< std::endl ;
      *out << "Temperature: on-csc boards";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","gas")<< std::endl ;
      *out << "GAS: mixers";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","pt100")<< std::endl ;
      *out << "GAS: pt100";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","wth")<< std::endl ;
      *out << "Weather Station";
      *out << cgicc::option() << std::endl ;
      /*
      //--
      *out << cgicc::option().set("value","")<< std::endl ;
      *out << "";
      *out << cgicc::option() << std::endl ;
      //--
      *out << cgicc::option().set("value","")<< std::endl ;
      *out << "";
      *out << cgicc::option() << std::endl ;
      //--
      */
      *out << cgicc::select() << std::endl ;


      *out << cgicc::input().set("type","submit").set("value","SUBMIT") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //------------------------------------------
  }
//===============================================================================
  void DcsControlWebXdaq::selectSlowControl(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
       
	////	std::string slow_control_select;
        slow_control_select= cgi["select_sc"]->getValue() ;
	//	const_file_iterator file;
	//	file = cgi.getFile("select_sc");
	/*
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
	*/

    //--------------------------------------
    if(slow_control_select == "hv")hv_page(in ,out);




      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
//===============================================================================
  int hv_page(xgi::Input * in, xgi::Output * out){


    vector<string> detectors;
    vector<string> detector_names;
    //------------------------------
    detectors.push_back("all");
    detectors.push_back("dp2r1c14");
    detectors.push_back("dp2r1c15");
    detectors.push_back("dp2r1c16");
    detectors.push_back("dp2r2c27");
    detectors.push_back("dp2r2c28");
    detectors.push_back("dp2r2c29");
    detectors.push_back("dp2r2c30");
    detectors.push_back("dp2r2c31");
    detectors.push_back("dp2r2c32");
    detectors.push_back("dp3r1c14");
    detectors.push_back("dp3r1c15");
    detectors.push_back("dp3r1c16");
    detectors.push_back("dp3r2c27");
    detectors.push_back("dp3r2c28");
    detectors.push_back("dp3r2c29");
    detectors.push_back("dp3r2c30");
    detectors.push_back("dp3r2c31");
    detectors.push_back("dp3r2c32");
    //------------------------------
    detector_names.push_back("ALL CHAMBERS");
    detector_names.push_back("ME+2/1/14");
    detector_names.push_back("ME+2/1/15");
    detector_names.push_back("ME+2/1/16");
    detector_names.push_back("ME+2/2/27");
    detector_names.push_back("ME+2/2/28");
    detector_names.push_back("ME+2/2/29");
    detector_names.push_back("ME+2/2/30");
    detector_names.push_back("ME+2/2/31");
    detector_names.push_back("ME+2/2/32");
    detector_names.push_back("ME+3/1/14");
    detector_names.push_back("ME+3/1/15");
    detector_names.push_back("ME+3/1/16");
    detector_names.push_back("ME+3/2/27");
    detector_names.push_back("ME+3/2/28");
    detector_names.push_back("ME+3/2/29");
    detector_names.push_back("ME+3/2/30");
    detector_names.push_back("ME+3/2/31");
    detector_names.push_back("ME+3/2/32");

    //------------------------------
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web") << std::endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    *out << cgicc::br();

    *out << slow_control_select << " has been selected"<< std::endl;
    *out << cgicc::h1(slow_control_select + " has been selected");
    *out << cgicc::br();


      std::string method =
	toolbox::toString("/%s/selectHighVoltage",getApplicationDescriptor()->getURN().c_str());

      //-----------------------------  
      *out << cgicc::form().set("method","GET").set("action",method)  << std::endl ;


      //--
      *out << cgicc::select().set("name","select_detector") << std::endl ;
      for(unsigned int i=0;i< detectors.size();i++){

      *out << cgicc::option().set("value",detectors[i])<< std::endl ;
      *out << detector_names[i];
      *out << cgicc::option() << std::endl ;
      }
      *out << cgicc::select() << std::endl ;
      //--
      //--
      *out << cgicc::select().set("name","select_turn") << std::endl ;

      *out << cgicc::option().set("value","none")<< std::endl ;
      *out << "NONE";
      *out << cgicc::option() << std::endl ;
      *out << cgicc::option().set("value","on")<< std::endl ;
      *out << "TURN ON";
      *out << cgicc::option() << std::endl ;
      *out << cgicc::option().set("value","off")<< std::endl ;
      *out << "TURN OFF";
      *out << cgicc::option() << std::endl ;

      *out << cgicc::select() << std::endl ;
      //--
     *out << cgicc::br();
     *out << "voltage-->";
      *out << cgicc::input().set("type","text")
	.set("name","value_to_set")
	.set("size","10")
	.set("ENCTYPE","multipart/form-data")
	.set("value","");

     *out << cgicc::br();
    *out << cgicc::textarea().set("name","result")
      .set("WRAP","OFF")
      .set("rows","20").set("cols","60");
    *out << detector_select + turn_select << endl ;
    *out << cgicc::textarea();



      *out << cgicc::input().set("type","submit").set("value","SUBMIT") << std::endl ;
      *out << cgicc::form() << std::endl ;
      //------------------------------------------




    return 1;
  }


//===============================================================================
//===============================================================================
  void DcsControlWebXdaq::selectHighVoltage(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    try
      {
	//
	cgicc::Cgicc cgi(in);
	//
       
	//	std::string detector_select;
	//	std::string turn_select;

        detector_select= cgi["select_detector"]->getValue() ;
        turn_select= cgi["select_turn"]->getValue() ;

        int ret=rc2dcs->readGAS();


        
        hv_page(in ,out);



    ////    if(detector_select == "hv")hv_page(detector_select, in ,out);




      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
//===============================================================================

//===============================================================================
  
};
#endif
