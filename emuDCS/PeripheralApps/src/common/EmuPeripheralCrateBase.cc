// $Id: EmuPeripheralCrateBase.cc,v 1.2 2009/03/15 12:32:14 liu Exp $

#include "EmuPeripheralCrateBase.h"

#include "xdaq/NamespaceURI.h"  // XDAQ_NS_URI
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()
#include "toolbox/fsm/FailedEvent.h"

namespace emu {
  namespace pc {

EmuPeripheralCrateBase::EmuPeripheralCrateBase(xdaq::ApplicationStub * s): xdaq::WebApplication(s)
{	
        fsm_.setFailedStateTransitionAction(this, &EmuPeripheralCrateBase::transitionFailed);
        fsm_.setFailedStateTransitionChanged(this, &EmuPeripheralCrateBase::changeState);
        fsm_.setStateName('F', "Failed");

        state_ = "";
        getApplicationInfoSpace()->fireItemAvailable("State", &state_);
        getApplicationInfoSpace()->fireItemAvailable("stateName", &state_);

        LOG4CPLUS_INFO(getApplicationLogger(), "Supervised");
}  

void EmuPeripheralCrateBase::changeState(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    state_ = fsm.getStateName(fsm.getCurrentState());

    LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << (std::string)state_);
}

void EmuPeripheralCrateBase::transitionFailed(toolbox::Event::Reference event)
        throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent &failed =
			dynamic_cast<toolbox::fsm::FailedEvent &>(*event);

	LOG4CPLUS_INFO(getApplicationLogger(),
			"Failure occurred when performing transition"
			<< " from: " << failed.getFromState()
			<< " to: " << failed.getToState()
			<< " exception: " << failed.getException().what());
}

void EmuPeripheralCrateBase::fireEvent(std::string name)
		throw (toolbox::fsm::exception::Exception)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));

	fsm_.fireEvent(event);
}

xoap::MessageReference EmuPeripheralCrateBase::createReply(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	std::string command = "";

	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(
			command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

	return reply;
}

//
////////////////////////////////////////////////////////////////////
// sending soap commands
////////////////////////////////////////////////////////////////////
int EmuPeripheralCrateBase::PCsendCommand(std::string command, std::string klass, int instance)
  throw (xoap::exception::Exception, xdaq::exception::Exception){
  //
  int num = 0;

  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  //
  try {
    if(instance < 0)
       apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
    else
       apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
  }
  catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return num; // Do nothing if the target doesn't exist
  }
  //
  // prepare a SOAP message
  xoap::MessageReference message = PCcreateCommandSOAP(command);
  xoap::MessageReference reply;
  xdaq::ApplicationDescriptor *ori=this->getApplicationDescriptor();
  //
  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    try {
        reply = getApplicationContext()->postSOAP(message, *ori, *(*i));
        num++;
    }
    catch (xcept::Exception &e) {
        std::cout << "PCsendCommand failed, command=" << command << ", klass=" << klass << ", instance=" << instance << std::endl;
    }
    //
  }
  return num;
}
//
xoap::MessageReference EmuPeripheralCrateBase::PCcreateCommandSOAP(std::string command) {
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

 }  // namespace emu::pc
}  // namespace emu

