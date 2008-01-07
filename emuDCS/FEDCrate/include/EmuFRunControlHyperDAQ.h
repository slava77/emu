// $Id: EmuFRunControlHyperDAQ.h,v 3.2 2008/01/07 18:19:04 gilmore Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuFRunControlHyperDAQ_h_
#define _EmuFRunControlHyperDAQ_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/ApplicationRegistry.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPName.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/Method.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"

#include "xdata/xdata.h"
#include "xdata/XMLDOM.h"
#include "xdata/Serializable.h"
#include "xdata/soap/Serializer.h"
#include "xdata/exception/Exception.h"
#include "xdata/XStr.h"
#include "xdata/Float.h"
#include "xdata/Integer.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xcept/tools.h"

using namespace cgicc;
using namespace std;

class EmuFRunControlHyperDAQ: public xdaq::Application
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  EmuFRunControlHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    // Bind SOAP callback
    //
    xgi::bind(this,&EmuFRunControlHyperDAQ::Default, "Default");
    xgi::bind(this,&EmuFRunControlHyperDAQ::SendSOAPMessageConfigure, "SendSOAPMessageConfigure");
    xgi::bind(this,&EmuFRunControlHyperDAQ::SendSOAPMessageInit, "SendSOAPMessageInit");
    xgi::bind(this,&EmuFRunControlHyperDAQ::SendSOAPMessageInitXRelay, "SendSOAPMessageInitXRelay");
    xgi::bind(this,&EmuFRunControlHyperDAQ::SendSOAPMessageConfigureXRelay, "SendSOAPMessageConfigureXRelay");
    xgi::bind(this,&EmuFRunControlHyperDAQ::SendSOAPMessageOpenFile, "SendSOAPMessageOpenFile");
    xoap::bind(this,&EmuFRunControlHyperDAQ::IRQSeen,"IRQSeen",XDAQ_NS_URI);
   //

  }  
  //

  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("EmuFRunControlHyperDAQ") << std::endl;
    //
    *out << cgicc::h1("EmuFRunControlHyperDAQ") << std::endl ;
    //
    std::string methodSOAPMessageInit =
      toolbox::toString("/%s/SendSOAPMessageInitXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInit) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Init Crate") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageConfigure =
      toolbox::toString("/%s/SendSOAPMessageConfigureXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigure) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure Crate") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageOpenFile =
      toolbox::toString("/%s/SendSOAPMessageOpenFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageOpenFile) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Open File") << std::endl ;
    *out << cgicc::form();
    //
  }
  //
  // Create a XRelay SOAP Message
  //
  xoap::MessageReference createXRelayMessage(const std::string & command, std::vector<xdaq::ApplicationDescriptor * > descriptor )
  {
    // Build a SOAP msg with the Xrelay header:
    xoap::MessageReference msg  = xoap::createMessage();
    //
    std::string topNode = "relay";
    std::string prefix = "xr";
    std::string httpAdd = "http://xdaq.web.cern.ch/xdaq/xsd/2004/XRelay-10";
    xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
    xoap::SOAPName envelopeName = envelope.getElementName();
    xoap::SOAPHeader header = envelope.addHeader();
    xoap::SOAPName relayName = envelope.createName(topNode, prefix,  httpAdd);
    xoap::SOAPHeaderElement relayElement = header.addHeaderElement(relayName);
    
    // Add the actor attribute
    xoap::SOAPName actorName = envelope.createName("actor", envelope.getElementName().getPrefix(),
						   envelope.getElementName().getURI());
    relayElement.addAttribute(actorName,httpAdd);
    
    // Add the "to" node
    std::string childNode = "to";
    // Send to all the destinations:
    vector <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
		
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();  	
	xoap::SOAPName toName = envelope.createName(childNode, prefix, " ");
	xoap::SOAPElement childElement = relayElement.addChildElement(toName);
	xoap::SOAPName urlName = envelope.createName("url");
	xoap::SOAPName urnName = envelope.createName("urn");
	childElement.addAttribute(urlName,url);
	childElement.addAttribute(urnName,urn);
	
      }
    
    // Create body
    
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName cmd = envelope.createName(command,"xdaq","urn:xdaq-soap:3.0");
    body.addBodyElement(cmd);
    
    msg->writeTo(std::cout);
	
    return msg;
    
  }
  
  // Post XRelay SOAP message to XRelay application
  void EmuFRunControlHyperDAQ::relayMessage (xoap::MessageReference msg) throw (xgi::exception::Exception)
  {
    // Retrieve the list of applications expecting this command and build the XRelay header
    
    try 
      {	
	/* Get the Xrelay application descriptor and post the message:
       	xdaq::ApplicationDescriptor * xrelay = getApplicationContext()->
	  getApplicationGroup()->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),4);
       
       xdaq::ApplicationDescriptor * xrelay = getApplicationContext()->getApplicationRegistry()->getApplication(getApplicationContext()->getContextDescriptor(),4)->getApplicationDescriptor();

	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, xrelay);
	*/
      } 
    catch (xdaq::exception::Exception& e) 
      {
		XCEPT_RETHROW (xgi::exception::Exception, "Cannot relay message", e);
      }
  }
  //
  void EmuFRunControlHyperDAQ::SendSOAPMessageOpenFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Open File   **************" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("OpenFile","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	/*	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("DDUHyperDAQ",0);
	*/
       xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationRegistry()->getApplication("DDUHyperDAQ",0)->getApplicationDescriptor();

	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //
  void EmuFRunControlHyperDAQ::SendSOAPMessageInit(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Init ******************" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("Init","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	/*	xdaq::ApplicationDescriptor * d = 
		getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuFCrateSOAP", 0);
 */
       xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrateSOAP",0)->getApplicationDescriptor();

	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	//
	/*	d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuFCrateSOAP", 1);
	*/
         d = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrateSOAP",1)->getApplicationDescriptor();
	reply    = getApplicationContext()->postSOAP(msg, d);
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //
  void EmuFRunControlHyperDAQ::SendSOAPMessageInitXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    /*
    std::vector<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuFCrateSOAP");
    
       xdaq::ApplicationDescriptor * descriptors = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrareSOAP",0)->getApplicationDescriptor();

    xoap::MessageReference configure = createXRelayMessage("Init", descriptors);
   
    this->relayMessage(configure);
    */ 
    this->Default(in,out);
    //
  }
  //
  void EmuFRunControlHyperDAQ::SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    /*
    std::vector<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuFCrateSOAP");
   
       std::vector<xdaq::ApplicationDescriptor * > descriptors = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrareSOAP")->getApplicationDescriptor();

    xoap::MessageReference configure = createXRelayMessage("Configure", descriptors);
    //
    this->relayMessage(configure);
    */
    this->Default(in,out);
    //
  }
  //
  void EmuFRunControlHyperDAQ::SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Configure" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("Configure","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	/*
	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuFCrateSOAP", 0);
	*/
       xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrateSOAP",0)->getApplicationDescriptor();

	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	/*
	d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuFCrateSOAP", 1);
	*/
        d = getApplicationContext()->getApplicationRegistry()->getApplication("EmuFCrateSOAP",1)->getApplicationDescriptor();

	reply    = getApplicationContext()->postSOAP(msg, d);
	//
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //  



  xoap::MessageReference IRQSeen (xoap::MessageReference msg) throw (xoap::exception::Exception)
  {   
    std::cout << "Received Message IRQSeen **********************" << std::endl ;
        xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");
	// msg->writeTo(std::cout);
	//  std::cout << std::endl;
	std::string data;
        msg->writeTo(data); 
        DOMDocument* doc = 0;
        doc = parser->parse (data);
	xdata::soap::Serializer serial;
	xdata::Integer pcrate;
	xdata::Integer pslot;
	xdata::Integer pstatus;
	DOMNodeList* dataNode = doc->getElementsByTagNameNS(xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("data"));
	DOMNodeList* dataElements = dataNode->item(0)->getChildNodes();
	
	for (unsigned int j = 0; j < dataElements->getLength(); j++)
	{
		DOMNode* n = dataElements->item(j);
		if (n->getNodeType() == DOMNode::ELEMENT_NODE)
		{	
			std::string nodeName = xoap::XMLCh2String(n->getNodeName());	
			if (nodeName == "xdaq:irqCrate")
			{
				serial.import (&pcrate, n);
			} else if (nodeName == "xdaq:irqSlot"){
				serial.import (&pslot, n);
			} else if (nodeName == "xdaq:irqStatus"){
				serial.import (&pstatus, n);
                        } 

		}
        }
             

        std::cout << " Crate: " << pcrate.toString() << " Slot: " << pslot.toString() << " Status: " << pstatus.toString() << std::endl;

    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName = envelope.createName( "onMessageResponse", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBodyElement e = envelope.getBody().addBodyElement ( responseName );
    return reply;
} 


};

#endif
