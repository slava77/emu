// $Id: EmuPeripheralCrateManager.cc,v 1.1 2007/12/26 11:23:53 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuPeripheralCrateManager.h"

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <unistd.h> // for sleep()
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <time.h>

using namespace cgicc;
using namespace std;

EmuPeripheralCrateManager::EmuPeripheralCrateManager(xdaq::ApplicationStub * s): EmuApplication(s)
{	
  xgi::bind(this,&EmuPeripheralCrateManager::Default, "Default");
  xgi::bind(this,&EmuPeripheralCrateManager::MainPage, "MainPage");
  xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConfigure, "SendSOAPMessageConfigure");
  xgi::bind(this,&EmuPeripheralCrateManager::SendSOAPMessageConfigureXRelay, "SendSOAPMessageConfigureXRelay");
  //
  // Normal SOAP call-back function
  // xoap::bind(this, &EmuPeripheralCrateManager::LTCResponse, "LTCResponse", XDAQ_NS_URI);
  // SOAP call-back functions, which relays to *Action method.
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnable,    "Enable",    XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onDisable,   "Disable",   XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onHalt,      "Halt",      XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onConfigCalCFEB,"ConfigCalCFEB", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBGains,"EnableCalCFEBGains", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk,"EnableCalCFEBCrossTalk", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBSCAPed,"EnableCalCFEBSCAPed", XDAQ_NS_URI);
  xoap::bind(this, &EmuPeripheralCrateManager::onEnableCalCFEBComparator,"EnableCalCFEBComparator", XDAQ_NS_URI);

  //
  // fsm_ is defined in EmuApplication
  fsm_.addState('H', "Halted",     this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('C', "Configured", this, &EmuPeripheralCrateManager::stateChanged);
  fsm_.addState('E', "Enabled",    this, &EmuPeripheralCrateManager::stateChanged);
  //
  fsm_.addStateTransition('H', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('C', 'C', "Configure", this, &EmuPeripheralCrateManager::configureAction);
  fsm_.addStateTransition('C', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
  fsm_.addStateTransition('E', 'E', "Enable",    this, &EmuPeripheralCrateManager::enableAction);
  fsm_.addStateTransition('E', 'C', "Disable",   this, &EmuPeripheralCrateManager::disableAction);
  fsm_.addStateTransition('C', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('E', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  fsm_.addStateTransition('H', 'H', "Halt",      this, &EmuPeripheralCrateManager::haltAction);
  //
  fsm_.setInitialState('H');
  fsm_.reset();
  //
  ConfigureState_ = "None";
  //
  // state_ is defined in EmuApplication
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  //
}  
//
void EmuPeripheralCrateManager::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  *out << "<meta HTTP-EQUIV=\"Refresh\" CONTENT=\"0; URL=/"
       <<getApplicationDescriptor()->getURN()<<"/"<<"MainPage"<<"\">" <<endl;
}
//
/////////////////////////////////////////////////////////////////////////////////
// Main page description
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) {
  //
  MyHeader(in,out,"EmuPeripheralCrateManager");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Crates in Configuration file").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Main Page");
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptor =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
  //
  std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
  for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) {
    std::string classNameStr = (*itDescriptor)->getClassName();
    *out << classNameStr << " " << std::endl ;
    std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
    *out << url << " " << std::endl;
    std::string urn = (*itDescriptor)->getURN();  	
    *out << urn << std::endl;
    //
    xoap::MessageReference reply;
    //
    bool failed = false ;
    //
    try {
      xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
      reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
    }
    //
    catch (xdaq::exception::Exception& e) {
      *out << cgicc::span().set("style","color:red");
      *out << "(Not running)"<<std::endl;
      *out << cgicc::span();
      failed = true;
    }
    //
    if(!failed) {
      //
      xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
      if (body.hasFault()) {
	std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
      } else {
	*out << cgicc::span().set("style","color:green");
	*out << "(" << extractState(reply) << ")";
	*out << cgicc::span();
      }
      //
    }
    //
    *out << cgicc::br();
    //
  }    
  //
  *out << cgicc::fieldset() ;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate");
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("XRelays in Configuration file").set("style","color:blue") << cgicc::p() << std::endl ;
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptorXRelay =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("XRelay");
  //
  std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorXRelay;
  for ( itDescriptorXRelay = descriptorXRelay.begin(); itDescriptorXRelay != descriptorXRelay.end(); itDescriptorXRelay++ ) {
    std::string classNameStr = (*itDescriptorXRelay)->getClassName();
    *out << classNameStr << " " << std::endl ;
    std::string url = (*itDescriptorXRelay)->getContextDescriptor()->getURL();
    *out << url << " " << std::endl;
    std::string urn = (*itDescriptorXRelay)->getURN();  	
    *out << urn << std::endl;
    //
    *out << cgicc::br();
    //
  }    
  //
  *out << cgicc::fieldset() ;
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "XRelay");
  //
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset() ;
  //
  *out << cgicc::fieldset() ;
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  /*
    std::string methodSOAPMessageInit =
    toolbox::toString("/%s/SendSOAPMessageInitXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInit) << std::endl ;
    *out << cgicc::input().set("type","submit")
    .set("value","Send SOAP message : Init Crate") << std::endl ;
    *out << cgicc::form();
    */
  //
  std::string methodSOAPMessageConfigure = toolbox::toString("/%s/SendSOAPMessageConfigureXRelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigure) << std::endl ;
  ostringstream output;
  output << "Send SOAP message : Configure Crates " ;
  output << "(" << ConfigureState_.toString() << ")" ;
  *out << cgicc::input().set("type","submit").set("value",output.str()) << std::endl ;
  *out << cgicc::form();
  //
  *out << cgicc::fieldset();
  //
}
//
//
/////////////////////////////////////////////////////////////////////////////////
// Talking to peripheral crate XDAQs
/////////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::CheckEmuPeripheralCrateState(xgi::Input * in, xgi::Output * out ) {
  //
  MyHeader(in,out,"CheckEmuperipheralCrate state");
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptor =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
  //
  std::set<xdaq::ApplicationDescriptor *>::iterator itDescriptor;
  for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) {
    std::string classNameStr = (*itDescriptor)->getClassName();
    *out << classNameStr << " " << std::endl ;
    std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
    *out << url << " " << std::endl;
    std::string urn = (*itDescriptor)->getURN();  	
    *out << urn << std::endl;
    //
    xoap::MessageReference reply;
    //
    bool failed = false ;
    //
    try {
      xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
      reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
    }
    //
    catch (xdaq::exception::Exception& e) {
      *out << cgicc::span().set("style","color:red");
      *out << "(Not running)"<<std::endl;
      *out << cgicc::span();
      failed = true;
    }
    //
    if(!failed) {
      //
      xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
      if (body.hasFault()) {
	std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
      } else {
	*out << cgicc::span().set("style","color:green");
	*out << "(" << extractState(reply) << ")";
	*out << cgicc::span();
      }
      //
    }
    //
    *out << cgicc::br();
    //
  }    
  //
}
//

int EmuPeripheralCrateManager::CompareEmuPeripheralCrateState(std::string state_compare){
  //
  int compare =0;
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptor =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
  //
  std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
  for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) {
    //
    std::string classNameStr = (*itDescriptor)->getClassName();
    std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
    std::string urn = (*itDescriptor)->getURN();  	
    //
    xoap::MessageReference reply;
    //
    bool failed = false ;
    //
    try {
      xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
      reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
    }
    //
    catch (xdaq::exception::Exception& e) {
      failed = true;
    }
    //
    if(!failed) {
      //
      xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
      if (body.hasFault()) {
	std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
      } else {
	std::string state = extractState(reply); 
	//std::cout << "States " << state << " " << state_compare << std::endl;
	if ( state == state_compare ) compare++;
      }
      //
    }
    //
  }    
  //
  return compare;
  //
}
//
//
//////////////////////////////////////////////////////////////////////////////
// Responses to soap messages
//////////////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::configureAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Configure");
}
//
void EmuPeripheralCrateManager::enableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Enable");
}  
//
void EmuPeripheralCrateManager::disableAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Disable");
}
//
void EmuPeripheralCrateManager::haltAction(toolbox::Event::Reference e) 
  throw (toolbox::fsm::exception::Exception) {
  LOG4CPLUS_INFO(getApplicationLogger(), "Received Message Halt");
}
//
void EmuPeripheralCrateManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception) {
  EmuApplication::stateChanged(fsm);
}
//
void EmuPeripheralCrateManager::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
  throw (xgi::exception::Exception) {
  //
  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  //
  //*out << cgicc::title(title) << std::endl;
  //*out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << h2(title) << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  //const CgiEnvironment& env = cgi.getEnvironment();
  //
  std::string myUrl = getApplicationDescriptor()->getContextDescriptor()->getURL();
  std::string myUrn = getApplicationDescriptor()->getURN();
  xgi::Utils::getPageHeader(out,title,myUrl,myUrn,"");
  //
}
//
string EmuPeripheralCrateManager::extractState(xoap::MessageReference message) {
  //
  //LOG4CPLUS_INFO(getApplicationLogger(), "extractState");
  //
  xoap::SOAPElement root = message->getSOAPPart().getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
  xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
  xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("stateName", "", "")))[0];
  //
  return state.getValue();
}
//
string EmuPeripheralCrateManager::extractRunNumber(xoap::MessageReference message) {
  xoap::SOAPElement root = message->getSOAPPart().getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
  xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
  xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("RunNumber", "", "")))[0];
  //
  return state.getValue();
}
//
//---------------------------------
// Create a XRelay SOAP Message
//---------------------------------
//
xoap::MessageReference EmuPeripheralCrateManager::QueryPeripheralCrateInfoSpace() {
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
  //
  xoap::SOAPName command    = envelope.createName("ParameterGet"    , "xdaq"              , "urn:xdaq-soap:3.0"                        );
  xoap::SOAPName properties = envelope.createName("properties"      , "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate"  );
  xoap::SOAPName parameter  = envelope.createName("stateName"       , "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate"  );
  xoap::SOAPName parameter2 = envelope.createName("CalibrationState", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate"  );
  xoap::SOAPName xsitype    = envelope.createName("type"            , "xsi"               , "http://www.w3.org/2001/XMLSchema-instance");
  //    
  xoap::SOAPElement properties_e = envelope.getBody().addBodyElement(command).addChildElement(properties);
  properties_e.addAttribute(xsitype, "soapenc:Struct");
  //
  xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
  parameter_e.addAttribute(xsitype, "xsd:string");
  //
  parameter_e = properties_e.addChildElement(parameter2);
  parameter_e.addAttribute(xsitype, "xsd:string");
  //
  return message;
}

xoap::MessageReference EmuPeripheralCrateManager::onConfigCalCFEB (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
   SendSOAPMessageXRelayBroadcast("ConfigCalCFEB","");
   //
   ::sleep(1);
   fireEvent("Configure");
  return createReply(message);
}
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBCrossTalk (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  SendSOAPMessageXRelayBroadcast("EnableCalCFEBCrossTalk","");
  //
  fireEvent("Enable");
  return createReply(message);
}
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBSCAPed (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  SendSOAPMessageXRelayBroadcast("EnableCalCFEBSCAPed","");
  //
  fireEvent("Enable");
  return createReply(message);
}
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBGains (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  SendSOAPMessageXRelayBroadcast("EnableCalCFEBGains","");
  //
  fireEvent("Enable");
  return createReply(message);
}
xoap::MessageReference EmuPeripheralCrateManager::onEnableCalCFEBComparator (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  SendSOAPMessageXRelayBroadcast("EnableCalCFEBComparator","");
  //
  fireEvent("Enable");
  return createReply(message);
}

xoap::MessageReference EmuPeripheralCrateManager::onConfigure (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  //  std::cout<< "This is a checking printing"<<std::endl;
  //
  ostringstream test;
  message->writeTo(test);
  //cout << test.str() <<endl;
  //cout << " Print check working "<<endl;
  //
  SendSOAPMessageXRelaySimple("Configure","");
  //
  // really should wait for OK message from EmuPeripheralCrate before change status
  ::sleep(1);
  fireEvent("Configure");
  //
  return createReply(message);
  //
}
//
xoap::MessageReference EmuPeripheralCrateManager::onEnable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  fireEvent("Enable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateManager::onDisable (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  fireEvent("Disable");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateManager::onHalt (xoap::MessageReference message) 
  throw (xoap::exception::Exception) {
  //
  fireEvent("Halt");
  //
  return createReply(message);
}
//
xoap::MessageReference EmuPeripheralCrateManager::createXRelayMessage(const std::string & command, const std::string & setting,
								      std::set<xdaq::ApplicationDescriptor * > descriptor ) {
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
  //
  // Add the actor attribute
  xoap::SOAPName actorName = envelope.createName("actor", 
						 envelope.getElementName().getPrefix(),
						 envelope.getElementName().getURI());
  relayElement.addAttribute(actorName,httpAdd);
  //
  // Add the "to" node
  std::string childNode = "to";
  // Send to all the destinations:
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptorsXrelays =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("broker")->getApplicationDescriptors("XRelay");
  //
  // xdata::UnsignedIntegerT lid4=4;
  // std::set<xdaq::ApplicationDescriptor * >  descriptorsXrelays =
  //     getApplicationContext()->getZone("default")->getApplicationGroup("broker")->getApplicationDescriptors("XRelay");
  //
  //
  std::cout << "descriptorXrelays size = " << descriptorsXrelays.size() << std::endl;
  //
  std::set<xdaq::ApplicationDescriptor * >::iterator  itDescriptorsXrelays = descriptorsXrelays.begin();
  //
  std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
  //
  for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) {
    //
    std::string classNameStr = (*itDescriptor)->getClassName();
    //
    std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
    std::string urn = (*itDescriptor)->getURN();
    //
    std::string urlXRelay = (*itDescriptorsXrelays)->getContextDescriptor()->getURL();
    std::string urnXRelay = (*itDescriptorsXrelays)->getURN();
    itDescriptorsXrelays++;
    if (itDescriptorsXrelays ==  descriptorsXrelays.end()) 
      itDescriptorsXrelays=descriptorsXrelays.begin();
    //
    xoap::SOAPName toName = envelope.createName(childNode, prefix, " ");
    xoap::SOAPElement childElement = relayElement.addChildElement(toName);
    xoap::SOAPName urlName = envelope.createName("url");
    xoap::SOAPName urnName = envelope.createName("urn");
    childElement.addAttribute(urlName,urlXRelay);
    childElement.addAttribute(urnName,urnXRelay);
    xoap::SOAPElement childElement2 = childElement.addChildElement(toName);
    childElement2.addAttribute(urlName,url);
    childElement2.addAttribute(urnName,urn);
    //
  }
  //
  // Create body
  //
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName cmd  = envelope.createName(command,"xdaq","urn:xdaq-soap:3.0");
  xoap::SOAPElement queryElement = body.addBodyElement(cmd);
  //
  if(setting != "" ) {
    xoap::SOAPName att  = envelope.createName("Setting");
    queryElement.addAttribute(att,setting);
  }
  //
  // msg->writeTo(std::cout);
  //
  return msg;
  //
}
//  
// Post XRelay SOAP message to XRelay application
//
void EmuPeripheralCrateManager::relayMessage (xoap::MessageReference msg) 
  throw (xgi::exception::Exception) {
  //
  // Retrieve the list of applications expecting this command and build the XRelay header
  //
  xoap::MessageReference reply;
  try {	
    // Get the Xrelay application descriptor and post the message:
    xdaq::ApplicationDescriptor * xrelay = getApplicationContext()->getDefaultZone()->
      getApplicationGroup("broker")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),4);
    //
    reply = getApplicationContext()->postSOAP(msg, xrelay);
    xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
    if (body.hasFault()) {
      std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
    } else {
      reply->writeTo(std::cout);
      std::cout << std::endl;
    }
  } 
  //
  catch (xdaq::exception::Exception& e) {
    XCEPT_RETHROW (xgi::exception::Exception, "Cannot relay message", e);
  }
  //
  std::cout << "Finish relayMessage" << std::endl;
  //  
}


void EmuPeripheralCrateManager::SendSOAPMessageXRelaySimple(std::string command,std::string setting) {
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptors =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
  //
  xoap::MessageReference configure = createXRelayMessage(command,setting,descriptors);
  //
  this->relayMessage(configure);
  //
}
//
void EmuPeripheralCrateManager::SendSOAPMessageXRelayBroadcast(std::string command,std::string setting) {
  //
  std::set<xdaq::ApplicationDescriptor * >  descriptors =
    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrateBroadcast");
  //
  xoap::MessageReference configure = createXRelayMessage(command,setting,descriptors);
  //
  this->relayMessage(configure);
  //
}
//
void EmuPeripheralCrateManager::SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  SendSOAPMessageXRelaySimple("Configure","");
  //
  // Now check
  //
  ConfigureState_ = "Failed";
  //
  for(int i=0;i<20; i++) {
    int compare=-1;
    compare = CompareEmuPeripheralCrateState("Configured");
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    if ( compare == (int) descriptor.size() ) {
      ConfigureState_ = "Configured";
      break;
    }
    //
  }
  //
  this->Default(in,out);
  //
}
//
//
void EmuPeripheralCrateManager::SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
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
  try {	
    std::set<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptors.begin(); itDescriptor != descriptors.end(); itDescriptor++ ) {
      xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, (*itDescriptor));
    }    
    //
  }
  // 
  catch (xdaq::exception::Exception& e) {
    XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
  }
  //
  this->Default(in,out);
  //
}
//  
////////////////////////////////////////////////////////////////////
// sending and receiving soap commands
////////////////////////////////////////////////////////////////////
void EmuPeripheralCrateManager::PCsendCommand(string command, string klass)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  //This is copied from CSCSupervisor::sendcommand;
  //
  // Exceptions:
  // xoap exceptions are thrown by analyzeReply() for SOAP faults.
  // xdaq exceptions are thrown by postSOAP() for socket level errors.
  //
  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
  }
  // 
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *i);
    //
    //      PCanalyzeReply(message, reply, *i);
  }
}
//
xoap::MessageReference EmuPeripheralCrateManager::PCcreateCommandSOAP(string command) {
  //
  //This is copied from CSCSupervisor::createCommandSOAP
  //
  xoap::MessageReference message = xoap::createMessage();
  xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
  xoap::SOAPName name = envelope.createName(command, "xdaq", "urn:xdaq-soap:3.0");
  envelope.getBody().addBodyElement(name);
  //
  return message;
}
//
//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrateManager)
