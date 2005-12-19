// $Id: DDUHyperDAQ.h,v 1.1 2005/12/19 13:30:38 mey Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _DDUHyperDAQ_h_
#define _DDUHyperDAQ_h_

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

// My stuff

#include "DDUUtilities.h"

using namespace cgicc;
using namespace std;

class DDUHyperDAQ: public xdaq::Application 
{
  //
private:
  std::string DDURunNumber_;
  std::string DDUMaxEventPerFile_;
  std::string DDUMaxEventTotal_;
  DDUUtilities util;
  //
public:
  //
  DDUHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    xgi::bind(this,&DDUHyperDAQ::Default, "Default");
    xgi::bind(this,&DDUHyperDAQ::SetRunNumber, "SetRunNumber");
    xgi::bind(this,&DDUHyperDAQ::SetMaxEventPerFile, "SetMaxEventPerFile");
    xgi::bind(this,&DDUHyperDAQ::SetMaxEventTotal, "SetMaxEventTotal");
    xgi::bind(this,&DDUHyperDAQ::DDUHardwareDumper, "DDUHardwareDumper");
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
    *out << cgicc::h1("DDUHyperDAQ") << std::endl;
    //
    std::string DDURunNumber =
      toolbox::toString("/%s/SetRunNumber",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DDURunNumber) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDURunNumber")
      .set("size","20").set("value",DDURunNumber_) << std::endl ;
    *out << "DDURunNumber" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUMaxEventPerFile =
      toolbox::toString("/%s/SetMaxEventPerFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUMaxEventPerFile) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDUMaxEventPerFile")
      .set("size","20").set("value",DDUMaxEventPerFile_) << std::endl ;
    *out << "DDUMaxEventPerFile" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUMaxEventTotal =
      toolbox::toString("/%s/SetMaxEventTotal",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUMaxEventTotal) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDUMaxEventTotal")
      .set("size","20").set("value",DDUMaxEventTotal_) << std::endl ;
    *out << "DDUMaxEventTotal" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUHardwareDumper =
      toolbox::toString("/%s/DDUHardwareDumper",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUHardwareDumper) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Set configuration file local") << std::endl ;
    *out << cgicc::form() << std::endl ;
      //
  }
  //
  void DDUHardwareDumper(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    util.HardwareDumper();
  }
  //
  void SetRunNumber(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    DDURunNumber_= cgi["DDURunNumber"]->getValue() ;
    int value = cgi["DDURunNumber"]->getIntegerValue() ;
    //
    util.SetRunNumber(value);
    //
    this->Default(in,out);
  }
  //
  void SetMaxEventPerFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    DDUMaxEventPerFile_= cgi["DDUMaxEventPerFile"]->getValue() ;
    int value = cgi["DDUMaxEventPerFile"]->getIntegerValue() ;
    //
    util.SetMaxEventPerFile(value);
    //
    this->Default(in,out);
  }
  //
  void SetMaxEventTotal(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    cgicc::Cgicc cgi(in);
    //
    DDUMaxEventTotal_= cgi["DDUMaxEventTotal"]->getValue() ;
    int value = cgi["DDUMaxEventTotal"]->getIntegerValue() ;
    //
    util.SetMaxEventTotal(value);
    //
    this->Default(in,out);
  }
  //
};

#endif
