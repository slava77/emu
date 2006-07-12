

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
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xdata/xdata.h"
#include "xdata/XMLDOM.h"
#include "xdata/InfoSpace.h"
#include "xdata/ActionListener.h"
#include "xdata/UnsignedLong.h"
#include "xdata/String.h"
#include "xdata/Table.h"
#include "xdata/Serializable.h"
#include "xdata/soap/Serializer.h"
#include "xdata/exception/Exception.h"
#include "xdata/XStr.h"
#include "xdata/Float.h"
#include "xdata/Integer.h"
#include "xdata/Boolean.h"
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

#include "toolbox/task/TimerFactory.h"

#include "EmuFController.h"
#include "EmuApplication.h"

using namespace cgicc;
using namespace std;

class EmuFCrateSOAP: public EmuApplication, public toolbox::task::TimerListener, public EmuFController
{
  
public:

  xdata::String xmlFile_;
  
  XDAQ_INSTANTIATOR();
  
  EmuFCrateSOAP(xdaq::ApplicationStub * s): EmuApplication(s) 
  {	
    //
    //
    xgi::bind (this,&EmuFCrateSOAP::Default, "Default");
    xoap::bind(this, &EmuFCrateSOAP::onMessage, "onMessage", XDAQ_NS_URI );    
    xoap::bind(this, &EmuFCrateSOAP::Configure, "Configure", XDAQ_NS_URI );    
    xoap::bind(this, &EmuFCrateSOAP::Init, "Init", XDAQ_NS_URI );        
    //
    // State machine definition
    //

    // SOAP call-back functions, which relays to *Action method.
    xoap::bind(this, &EmuFCrateSOAP::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrateSOAP::onEnable,    "Enable",    XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrateSOAP::onDisable,   "Disable",   XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrateSOAP::onHalt,      "Halt",      XDAQ_NS_URI);
    //
    // fsm_ is defined in EmuApplication
    fsm_.addState('H', "Halted",     this, &EmuFCrateSOAP::stateChanged);
    fsm_.addState('C', "Configured", this, &EmuFCrateSOAP::stateChanged);
    fsm_.addState('E', "Enabled",    this, &EmuFCrateSOAP::stateChanged);
    //
    fsm_.addStateTransition(
      'H', 'C', "Configure", this, &EmuFCrateSOAP::configureAction);
    fsm_.addStateTransition(
      'C', 'C', "Configure", this, &EmuFCrateSOAP::configureAction);
    fsm_.addStateTransition(
      'C', 'E', "Enable",    this, &EmuFCrateSOAP::enableAction);
    fsm_.addStateTransition(
      'E', 'C', "Disable",   this, &EmuFCrateSOAP::disableAction);
    fsm_.addStateTransition(
      'C', 'H', "Halt",      this, &EmuFCrateSOAP::haltAction);
    fsm_.addStateTransition(
      'E', 'H', "Halt",      this, &EmuFCrateSOAP::haltAction);
    fsm_.addStateTransition(
      'H', 'H', "Halt",      this, &EmuFCrateSOAP::haltAction);

    fsm_.setInitialState('H');
    fsm_.reset();

    //
    // state_ is defined in EmuApplication
    state_ = fsm_.getStateName(fsm_.getCurrentState());
    //

    //	
    counter_ = 0;

    this->getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
    //
    //timer_ = toolbox::task::getTimerFactory()->getTimer("MonitoringProducerTimer");
    //timer_->stop();
    //
  }  
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("EmuFCrateSOAP") << std::endl;
    //
    *out << cgicc::h1("EmuFCrateSOAP") << std::endl ;
    //
    *out << cgicc::br() << std::endl;
    *out << "Counter: " << counter_.toString() << cgicc::br() << std::endl;
  }


  xoap::MessageReference EmuFCrateSOAP::onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Configure");

    return createReply(message);
  }

  //
  xoap::MessageReference EmuFCrateSOAP::onEnable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Enable");

    return createReply(message);
  }

  //
  xoap::MessageReference EmuFCrateSOAP::onDisable (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Disable");

    return createReply(message);
  }

  //
  xoap::MessageReference EmuFCrateSOAP::onHalt (xoap::MessageReference message) throw (xoap::exception::Exception)
  {
    fireEvent("Halt");

    return createReply(message);
  }

  void EmuFCrateSOAP::configureAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
     configure();
    std::cout << "Configure" << std::endl ;
    std::cout << xmlFile_.toString() << std::endl;
    std::cout << "Received Message Configure" << std::endl ;
  }

  //
  void EmuFCrateSOAP::enableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    SetConfFile(xmlFile_);
    init();
    std::cout << "Received Message Enable" << std::endl ;
  }

  //
  void EmuFCrateSOAP::disableAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    // do nothing
    std::cout << "Received Message Disable" << std::endl ;
  }

  //
  void EmuFCrateSOAP::haltAction(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
  {
    // do nothing
    std::cout << "Received Message Halt" << std::endl ;
  }

  //
  void EmuFCrateSOAP::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
    throw (toolbox::fsm::exception::Exception)
  {
    EmuApplication::stateChanged(fsm);
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
    configure();
    //
    // start irq monitor
	// Create an infospace and launch a workloop task that populates data
	// every 1 seconds
    /*	std::string infoSpaceName = toolbox::toString("urn:xdaq-monitorable:IRQMonitor");
	infoSpace_ = xdata::InfoSpace::get(infoSpaceName);
	
	infoSpace_->fireItemAvailable("counter", &counter_, 0);
	
	toolbox::TimeInterval interval;
	interval.fromString("00:00:00:00:01");
	toolbox::TimeVal startTime;
	startTime = toolbox::TimeVal::gettimeofday();	

	// Pass a pointer to the infoSpace as the context
	// The name of this timer task is "MonitoringProducer"
	timer_->start(); // must activate timer before submission, abort otherwise!!!
	timer_->scheduleAtFixedRate(startTime,this, interval, infoSpace_, "IRQMonitor" ); */  // end IRQ Monitor Start
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

  /* void stopmon(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	timer_->remove("IRQMonitor");
	timer_->stop();
	
	// remove InfoSpace
	std::string infoSpaceName = toolbox::toString("urn:xdaq-monitorable:IRQMonitor", getApplicationDescriptor()->getInstance());
	xdata::InfoSpace::remove(infoSpaceName);
	this->Default(in,out);
} */



void timeExpired (toolbox::task::TimerEvent& e) 
{ 
	std::string name = e.getTimerTask()->name;
	// To assure atomic update of all values, lock the InfoSpace
	// Note: Do not lock the InfoSpace in a callback of an InfoSpace event,
	// e.g. ItemRetrieved
	infoSpace_->lock();
	
	// increment the counter
        int irqval=irqtest();
        counter_=irqval;
	lastExecutionTime_ = e.getTimerTask()->lastExecutionTime.toString(toolbox::TimeVal::loc);  
	infoSpace_->unlock();	
        if(irqval!=0){ 
             int crate=irqcrate;
             int slot=irqslot;
             int status=irqstatus;
             char status2[20];
             sprintf(status2,"%04x",irqstatus);
	     std::cout << " Crate " << crate << " Slot " << slot  <<" Status " << status2 << std::endl;


             xoap::MessageReference msg = xoap::createMessage();
             xoap::SOAPPart soap = msg->getSOAPPart();
             xoap::SOAPEnvelope envelope = soap.getEnvelope();
             xoap::SOAPBody body = envelope.getBody();
             xoap::SOAPName command = envelope.createName("IRQSeen","xdaq", "urn:xdaq-soap:3.0");
             body.addBodyElement (command);
	     xoap::SOAPName dataname = envelope.createName("data","xdaq", "urn:xdaq-soap:3.0");
	     xoap::SOAPBodyElement dataelement = body.addBodyElement (dataname);
             xdata::soap::Serializer serializer;
	     xoap::SOAPName integer1name = envelope.createName ("irqCrate", "xdaq", "urn:xdaq-soap:3.0");
	     xoap::SOAPElement integer1element = dataelement.addChildElement(integer1name);
	     xdata::Integer pcrate;
             pcrate=crate;
	     serializer.exportAll(&pcrate, dynamic_cast<DOMElement*>(integer1element.getDOMNode()), true);
	     xoap::SOAPName integer2name = envelope.createName ("irqSlot", "xdaq", "urn:xdaq-soap:3.0");
	     xoap::SOAPElement integer2element = dataelement.addChildElement(integer2name);
	     xdata::Integer pslot;
             pslot=slot;
	     serializer.exportAll(&pslot, dynamic_cast<DOMElement*>(integer2element.getDOMNode()), true);
	     xoap::SOAPName integer3name = envelope.createName ("irqStatus", "xdaq", "urn:xdaq-soap:3.0");
	     xoap::SOAPElement integer3element = dataelement.addChildElement(integer3name);
	     xdata::Integer pstatus;
             pstatus=status;
	     serializer.exportAll(&pstatus, dynamic_cast<DOMElement*>(integer3element.getDOMNode()), true);

	     //  msg->writeTo(std::cout);
	     // std::cout << std::endl;




             xdaq::ApplicationDescriptor * d = 
	     getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuFRunControlHyperDAQ",0);
	     xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);


        }
}	


	xdata::UnsignedLong counter_;
	xdata::String lastExecutionTime_; 
        xdata::Table* table_;
        xdata::InfoSpace * infoSpace_;
        toolbox::task::Timer * timer_;

};

#endif
