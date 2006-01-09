// $Id: DDUHyperDAQ.h,v 1.2 2006/01/09 09:23:23 mey Exp $

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
    std::string DDURunNumberValue =
      toolbox::toString("%d",util.GetRunNumber());
    //
    *out << cgicc::form().set("method","GET").set("action",DDURunNumber) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDURunNumber")
      .set("size","20").set("value",DDURunNumberValue) << std::endl ;
    *out << "DDURunNumber" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUMaxEventPerFile =
      toolbox::toString("/%s/SetMaxEventPerFile",getApplicationDescriptor()->getURN().c_str());
    std::string DDUMaxEventPerFileValue =
      toolbox::toString("%d",util.GetMaxEventPerFile());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUMaxEventPerFile) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDUMaxEventPerFile")
      .set("size","20").set("value",DDUMaxEventPerFileValue) << std::endl ;
    *out << "DDUMaxEventPerFile" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUMaxEventTotal =
      toolbox::toString("/%s/SetMaxEventTotal",getApplicationDescriptor()->getURN().c_str());
    std::string DDUMaxEventTotalValue =
      toolbox::toString("%d",util.GetMaxEventTotal());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUMaxEventTotal) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDUMaxEventTotal")
      .set("size","20").set("value",DDUMaxEventTotalValue) << std::endl ;
    *out << "DDUMaxEventTotal" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUDevice =
      toolbox::toString("/%s/Device",getApplicationDescriptor()->getURN().c_str());
    std::string DDUDeviceValue = util.GetDevice() ;
    //
    *out << cgicc::form().set("method","GET").set("action",DDUDevice) << std::endl ;
    *out << cgicc::input().set("type","text").set("name","DDUDevice")
      .set("size","20").set("value",DDUDeviceValue) << std::endl ;
    *out << "DDUDevice" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUReset =
      toolbox::toString("/%s/DDUReset",getApplicationDescriptor()->getURN().c_str());
    std::string DDUResetValue =
      toolbox::toString("%d",util.GetReset());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUReset) << std::endl ;
    *out << cgicc::input().set("type","checkbox").set("name","DDUReset").set("checked","checked")
      .set("value",DDUResetValue) << std::endl ;
    *out << "DDU reset" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUdb =
      toolbox::toString("/%s/DDUdb",getApplicationDescriptor()->getURN().c_str());
    std::string DDUdbValue =
      toolbox::toString("%d",util.GetDB());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUdb) << std::endl ;
    *out << cgicc::input().set("type","checkbox").set("name","DDUupdateDB").set("checked","checked")
      .set("value",DDUdbValue) << std::endl ;
    *out << "DDU update DB" << std::endl;
    *out << cgicc::form() << std::endl ;
    //
    std::string DDUHardwareDumper =
      toolbox::toString("/%s/DDUHardwareDumper",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",DDUHardwareDumper) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Start dumping data...") << std::endl ;
    *out << cgicc::form() << std::endl ;
      //
  }
  //
  void DDUHardwareDumper(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    cgicc::Cgicc cgi(in);
    //
    if ( cgi.queryCheckbox("DDUReset") ) 
      {
	util.SetReset(true);
      } else {
	util.SetReset(false);
      }
    //
    if ( cgi.queryCheckbox("DDUupdateDB") ) 
      {
	util.SetDB(true);
      } else {
	util.SetDB(false);
      }
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
