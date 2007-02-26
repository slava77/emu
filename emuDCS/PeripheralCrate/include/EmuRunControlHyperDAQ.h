// $Id: EmuRunControlHyperDAQ.h,v 3.7 2007/02/26 01:56:40 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuRunControlHyperDAQ_h_
#define _EmuRunControlHyperDAQ_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/Table.h>


using namespace cgicc;
using namespace std;

class EmuRunControlHyperDAQ: public xdaq::Application
{
  
public:
  
  XDAQ_INSTANTIATOR();

  xdata::UnsignedLong runNumber_;
  
  EmuRunControlHyperDAQ(xdaq::ApplicationStub * s); 
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
    throw (xgi::exception::Exception);
  string extractState(xoap::MessageReference message);
  string extractRunNumber(xoap::MessageReference message);
  xoap::MessageReference QueryLTCInfoSpace();
  xoap::MessageReference QueryPeripheralCrateInfoSpace();
  xoap::MessageReference QueryJobControlInfoSpace();
  xoap::MessageReference createXRelayMessage(const std::string & command, std::set <xdaq::ApplicationDescriptor * > descriptor );
  void relayMessage (xoap::MessageReference msg) throw (xgi::exception::Exception);
  void SendSOAPMessageConnectTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageDisconnectTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void OutputTable(xgi::Output * out,xdata::Table &results);
  void SendSOAPMessageOpenFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageConfigureLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageJobControlExecuteCommand(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageQueryLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageInit(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageInitXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
  void SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception);
};

#endif
