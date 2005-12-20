// $Id: EmuCrateSOAP.h,v 1.2 2005/12/20 14:48:15 mey Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuCrateSOAP_h_
#define _EmuCrateSOAP_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
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

#include "EmuCrateController.h"

using namespace cgicc;
using namespace std;

class EmuCrateSOAP: public xdaq::Application, public EmuCrateController
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  EmuCrateSOAP(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    // Bind SOAP callback
    //
    xgi::bind (this,&EmuCrateSOAP::Default, "Default");
    xoap::bind(this, &EmuCrateSOAP::onMessage, "onMessage", XDAQ_NS_URI );    
    xoap::bind(this, &EmuCrateSOAP::Configure, "Configure", XDAQ_NS_URI );    
    //
  }  
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("EmuCrateSOAP") << std::endl;
    //
    *out << cgicc::h1("EmuCrateSOAP") << std::endl ;
    //
  }
  //
  // SOAP Callback  
  //
  xoap::MessageReference onMessage (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {
    
    // reply to caller
    
    std::cout << "Received Message onMessage" << std::endl ;
    
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;
    
  }
  //
  xoap::MessageReference Configure (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {
    
    // reply to caller
    
    std::cout << "Received Message Configure" << std::endl ;
    
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;
    
  }
  //
};

#endif
