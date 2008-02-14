 

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
  }
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {

    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("Simple Web") << std::endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
    *out << cgicc::a("Visit the XDAQ Web site").set("href","http://xdaq.web.cern.ch") << endl;
        DimService dd;

      //
  }
    //  
};
#endif
