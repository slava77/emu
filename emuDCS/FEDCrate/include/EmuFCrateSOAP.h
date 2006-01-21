

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuFCrateSOAP_h_
#define _EmuFCrateSOAP_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xdata/UnsignedLong.h"
#include "xdata/String.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "EmuFController.h"

using namespace cgicc;
using namespace std;

class EmuFCrateSOAP: public xdaq::Application, public EmuFController
{
  
public:

  xdata::String xmlFile_;
  
  XDAQ_INSTANTIATOR();
  
  EmuFCrateSOAP(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    //
    xgi::bind (this,&EmuFCrateSOAP::Default, "Default");
    xoap::bind(this, &EmuFCrateSOAP::onMessage, "onMessage", XDAQ_NS_URI );    
    xoap::bind(this, &EmuFCrateSOAP::Configure, "Configure", XDAQ_NS_URI );    
    xoap::bind(this, &EmuFCrateSOAP::Init, "Init", XDAQ_NS_URI );    
    //
    this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
    //
  }  
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    printf(" EmuFCrateSOAP is running .....................\n");
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("EmuFCrateSOAP") << std::endl;
    //
    *out << cgicc::h1("EmuFCrateSOAP") << std::endl ;
    //
  }
  //
  // SOAP Callback  
  //
  xoap::MessageReference onMessage (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {   
    //
    // reply to caller
    //
    std::cout << "Received Message onMessage ****************" << std::endl ;
    //
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;
    
  }
  //
  xoap::MessageReference Configure (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {
    //
    printf(" CALL CONFIGURE NOW ********************************\n");
    configure();
    //
    // reply to caller
    //
    std::cout << "Received Message Configure" << std::endl ;
    //
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;    
  }
  //
  xoap::MessageReference Init (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {
    //
    SetConfFile(xmlFile_);
    //
    init();
    //
    // reply to caller
    //
    std::cout << "Received Message Init **********************" << std::endl ;
    //
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;
    
  }
  //
};

#endif
