// Supervised.cc

#include "emu/base/Supervised.h"

#include "xdaq/NamespaceURI.h"  // XDAQ_NS_URI
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()
#include "toolbox/fsm/FailedEvent.h"

using namespace std;

emu::base::Supervised::Supervised(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		xdaq::WebApplication(stub)
{
	fsm_.setFailedStateTransitionAction(this, &emu::base::Supervised::transitionFailed);
	fsm_.setFailedStateTransitionChanged(this, &emu::base::Supervised::stateChanged);
	fsm_.setStateName('F', "Failed");

	state_ = "";
	getApplicationInfoSpace()->fireItemAvailable("State", &state_);
	getApplicationInfoSpace()->fireItemAvailable("stateName", &state_);

	LOG4CPLUS_INFO(getApplicationLogger(), "Supervised");
}

void emu::base::Supervised::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
    state_ = fsm.getStateName(fsm.getCurrentState());

	LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << (string)state_);
}

void emu::base::Supervised::transitionFailed(toolbox::Event::Reference event)
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

void emu::base::Supervised::fireEvent(string name)
		throw (toolbox::fsm::exception::Exception)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));

	fsm_.fireEvent(event);
}

xoap::MessageReference emu::base::Supervised::createReply(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	string command = "";

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

// End of file
// vim: set ai sw=4 ts=4:
