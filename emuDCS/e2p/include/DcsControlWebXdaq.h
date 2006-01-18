 



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

//===============================

#include <dis.hxx>
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

    vector<string> slowcontrols;
    vector<string> slowcontrol_names;

    vector<string> chambers;
    vector<string> chamber_names;

    vector<string> pcrates;
    vector<string> pcrate_names;

    vector<string> maratons;
    vector<string> maraton_names;

    vector<string> chip_resets;
    vector<string> chip_resets_names;

string slow_control_select;
string detector_select;
string turn_select;
string value_to_set;
string option_select;

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


    //------------------------------
    slowcontrols.push_back("hv");
    slowcontrols.push_back("lv");
    slowcontrols.push_back("mrtn");
    slowcontrols.push_back("crb");
    slowcontrols.push_back("temp");
    slowcontrols.push_back("gas");
    slowcontrols.push_back("pt100");
    slowcontrols.push_back("wth");
    slowcontrols.push_back("chip");
    //    slowcontrols.push_back("");
    //    slowcontrols.push_back("");
    //    slowcontrols.push_back("");
    //    slowcontrols.push_back("");

    //------------------------------
    slowcontrol_names.push_back("HV");
    slowcontrol_names.push_back("LV: on-csc boards");
    slowcontrol_names.push_back("LV: Wiener Maratons");
    slowcontrol_names.push_back("LV: PeripheralCrate boards");
    slowcontrol_names.push_back("Temperature: on-csc boards");
    slowcontrol_names.push_back("GAS: mixers");
    slowcontrol_names.push_back("GAS: pt100");
    slowcontrol_names.push_back("Weather Station");
    slowcontrol_names.push_back("Chips reset");
    //    slowcontrol_names.push_back("");
    //    slowcontrol_names.push_back("");
    //    slowcontrol_names.push_back("");
    //    slowcontrol_names.push_back("");
    //    slowcontrol_names.push_back("");
    //    slowcontrol_names.push_back("");

    //------------------------------

    //------------------------------
    chambers.push_back("all");
    chambers.push_back("dp2r1c14");
    chambers.push_back("dp2r1c15");
    chambers.push_back("dp2r1c16");
    chambers.push_back("dp2r2c27");
    chambers.push_back("dp2r2c28");
    chambers.push_back("dp2r2c29");
    chambers.push_back("dp2r2c30");
    chambers.push_back("dp2r2c31");
    chambers.push_back("dp2r2c32");
    chambers.push_back("dp3r1c14");
    chambers.push_back("dp3r1c15");
    chambers.push_back("dp3r1c16");
    chambers.push_back("dp3r2c27");
    chambers.push_back("dp3r2c28");
    chambers.push_back("dp3r2c29");
    chambers.push_back("dp3r2c30");
    chambers.push_back("dp3r2c31");
    chambers.push_back("dp3r2c32");
    //------------------------------
    chamber_names.push_back("ALL CHAMBERS");
    chamber_names.push_back("ME+2/1/14");
    chamber_names.push_back("ME+2/1/15");
    chamber_names.push_back("ME+2/1/16");
    chamber_names.push_back("ME+2/2/27");
    chamber_names.push_back("ME+2/2/28");
    chamber_names.push_back("ME+2/2/29");
    chamber_names.push_back("ME+2/2/30");
    chamber_names.push_back("ME+2/2/31");
    chamber_names.push_back("ME+2/2/32");
    chamber_names.push_back("ME+3/1/14");
    chamber_names.push_back("ME+3/1/15");
    chamber_names.push_back("ME+3/1/16");
    chamber_names.push_back("ME+3/2/27");
    chamber_names.push_back("ME+3/2/28");
    chamber_names.push_back("ME+3/2/29");
    chamber_names.push_back("ME+3/2/30");
    chamber_names.push_back("ME+3/2/31");
    chamber_names.push_back("ME+3/2/32");
    //===================================
    pcrates.push_back("ALL");
    pcrates.push_back("sp2p1");
    pcrates.push_back("sp3p1");
    //------------------------
    pcrate_names.push_back("ALL PERIPHERAL CRATES");
    pcrate_names.push_back("ME+2/pcrate #1");
    pcrate_names.push_back("ME+3/pcrate #1");

    //===================================
    maratons.push_back("ALL");
    maratons.push_back("maraton01");
    maratons.push_back("maraton02");
    //------------------------
    maraton_names.push_back("ALL MARATONS");
    maraton_names.push_back("maraton #1");
    maraton_names.push_back("maraton #2");

    //===================================

    chip_resets.push_back("hr_all");
    chip_resets.push_back("hr_alct");
    chip_resets.push_back("hr_dmb");
    chip_resets.push_back("hr_tmb");
    chip_resets.push_back("hr_cfebs");
    chip_resets.push_back("hr_mpc");
    //------------------------
    chip_resets_names.push_back("hard rest all boards");
    chip_resets_names.push_back("hard reset alct");
    chip_resets_names.push_back("hard reset dmb");
    chip_resets_names.push_back("hard reset tmb");
    chip_resets_names.push_back("hard reset cfebs");
    chip_resets_names.push_back("hard reset mpc");

    //===================================


  }
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCSCONTROL_HYPERDAQ") << std::endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    //    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
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




    //------------------------------

      *out << cgicc::select().set("name","select_sc") << std::endl ;
      //--
      for(unsigned int i=0;i< slowcontrols.size();i++){

      *out << cgicc::option().set("value",slowcontrols[i])<< std::endl ;
      *out << slowcontrol_names[i];
      *out << cgicc::option() << std::endl ;
      }
      *out << cgicc::select() << std::endl ;

      /*
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

      *out << cgicc::select() << std::endl ;
     */

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
	string s=string("");
	/*	if(slow_control_select == "hv" || slow_control_select == "lv" ||
           slow_control_select == "temp" || slow_control_select == "mrtn" ||
           slow_control_select == "crb" )
	*/
         hv_page(in ,out, s);
   



      }
    catch (const std::exception & e )
      {
	//XECPT_RAISE(xgi::exception::Exception, e.what());
      }
  }
//===============================================================================
  int hv_page(xgi::Input * in, xgi::Output * out, string &textareaoutput){

    *out << cgicc::a("DCSCONTROL HOME").set("href","http://emuslice03.cern.ch:1972/urn:xdaq-application:lid=30/") << endl;


    vector<string> detectors;
    vector<string> detector_names;    

	if(slow_control_select == "hv" || slow_control_select == "lv" || 
           slow_control_select == "temp" || slow_control_select == "chip"){
	  detectors=chambers;
	  detector_names=chamber_names;
	}
	else if(slow_control_select == "crb"){
	  detectors=pcrates;
	  detector_names=pcrate_names;
	}
	else if(slow_control_select == "mrtn"){
	  detectors=maratons;
	  detector_names=maraton_names;
	}

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("DCSCONTROL") << std::endl;
    //   *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    *out << cgicc::br();


    unsigned int i=0;
    for(i=0;i< slowcontrols.size();i++){if(slowcontrols[i]==slow_control_select)break;}

    *out << cgicc::h1(slowcontrol_names[i]) << " slow control ";
    *out << "has been selected" << std::endl;
    *out << cgicc::br();

    std::string method;
	method=toolbox::toString("/%s/selectHighVoltage",getApplicationDescriptor()->getURN().c_str());

      //-----------------------------  
      *out << cgicc::form().set("method","POST").set("action",method)  << std::endl ;


     if(slow_control_select == "hv" || slow_control_select == "lv" || slow_control_select == "temp" || 
        slow_control_select == "crb" || slow_control_select == "mrtn" || slow_control_select == "chip" ){
      //--
      *out << cgicc::select().set("name","select_detector") << std::endl ;
      for(unsigned int i=0;i< detectors.size();i++){
      *out << cgicc::option().set("value",detectors[i])<< std::endl ;
      *out << detector_names[i];
      *out << cgicc::option() << std::endl ;
      }
      *out << cgicc::select() << std::endl ;
      //--
     }

     if(slow_control_select == "hv" || slow_control_select == "lv" || 
        slow_control_select == "crb" || slow_control_select == "mrtn" ){
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
     }
      *out << cgicc::br();
     if(slow_control_select == "hv"){
      *out << "voltage-->";
      *out << cgicc::input().set("type","text")
	.set("name","value_to_set")
	.set("size","10")
	.set("ENCTYPE","multipart/form-data")
	.set("value","");
      *out << cgicc::br();
     //--
     }
     if(slow_control_select == "chip" ){
      //--
      *out << cgicc::select().set("name","select_option") << std::endl ;
      for(unsigned int i=0;i< chip_resets.size();i++){
      *out << cgicc::option().set("value",chip_resets[i])<< std::endl ;
      *out << chip_resets_names[i];
      *out << cgicc::option() << std::endl ;
      }
      *out << cgicc::select() << std::endl ;
      *out << cgicc::br();
      //--
     }

    *out << cgicc::textarea().set("name","result")
      .set("WRAP","OFF")
      .set("rows","30").set("cols","100");
    //    *out << detector_select + turn_select << endl ;
      *out << textareaoutput << endl ;
     //--

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

        float value_to_set_float;
        int turn_select_int;
        int option_select_int;

     if(slow_control_select == "hv" || slow_control_select == "lv" || slow_control_select == "temp" || 
        slow_control_select == "crb" || slow_control_select == "mrtn" || slow_control_select == "chip" ){
        detector_select= cgi["select_detector"]->getValue() ;
     }
     if(slow_control_select == "hv" || slow_control_select == "lv" || 
        slow_control_select == "crb" || slow_control_select == "mrtn" ){
        turn_select= cgi["select_turn"]->getValue() ;
     }

    if(slow_control_select == "hv"){
        value_to_set= cgi["value_to_set"]->getValue() ;
        if(value_to_set != "")
	 value_to_set_float=atof(value_to_set.c_str());
        else value_to_set_float=-1;
    }

    if(slow_control_select == "chip"){
        option_select= cgi["select_option"]->getValue() ;

         if(option_select=="hr_all")option_select_int=0;          //all
	 else if(option_select=="hr_alct")option_select_int=1;     //alct
	 else if(option_select=="hr_dmb")option_select_int=2;     //dmb
	 else if(option_select=="hr_tmb")option_select_int=3;     //cfebs
	 else if(option_select=="hr_cfebs")option_select_int=4;     //tmb
	 else if(option_select=="hr_mpc")option_select_int=5;     //mpc
    }
        if(turn_select=="off")turn_select_int=0;
	else if(turn_select=="on")turn_select_int=1;
        else if(turn_select=="none")turn_select_int=-1;

//-------------------------------------------------------
        if(slow_control_select == "hv"){
         if(turn_select != "none"){
          if(value_to_set_float < 0)rc2dcs->turnHV(turn_select_int,-1,(char *)detector_select.c_str());
          else rc2dcs->turnHV(turn_select_int,(int)value_to_set_float,(char *)detector_select.c_str());
         }
	 else rc2dcs->readHV((char *)detector_select.c_str());
//-------------
	}
        else if(slow_control_select == "lv"){
         if(turn_select != "none")rc2dcs->turnLV(turn_select_int,(char *)detector_select.c_str());
         else rc2dcs->readLV((char *)detector_select.c_str());
	}
//--------------------------------------------------------
        else if(slow_control_select == "temp"){
         rc2dcs->readTEMP((char *)detector_select.c_str());
	}
//--------------------------------------------------------
        else  if(slow_control_select=="mrtn"){
         if(turn_select != "none")rc2dcs->turnMRTN(turn_select_int,(char *)detector_select.c_str());
         else rc2dcs->readMRTN((char *)detector_select.c_str());
        }
//--------------------------------------------------------
        else  if(slow_control_select=="crb"){
         if(turn_select != "none")rc2dcs->turnCRB(turn_select_int,(char *)detector_select.c_str());
         else rc2dcs->readCRB((char *)detector_select.c_str());
        }
//--------------------------------------------------------
        else if(slow_control_select == "gas"){
         rc2dcs->readGAS();
	}
//--------------------------------------------------------
        else if(slow_control_select == "wth"){
         rc2dcs->readWTH();
	}
//--------------------------------------------------------
        else if(slow_control_select == "pt100"){
         rc2dcs->readPT100();
	}
//--------------------------------------------------------
        else if(slow_control_select == "chip"){
         rc2dcs->controlCHIP(option_select_int, (char *)detector_select.c_str());
	}
//--------------------------------------------------------

        FILE *fp;
        char line[200];
        string textareaoutput;
        fp=fopen("/home/dcs/bin/dcscontrol_web.txt", "r");
        while(1){
         if(fgets(line,200,fp) == NULL)break;
         textareaoutput += string(line);
	}
        
        hv_page(in ,out, textareaoutput);



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
