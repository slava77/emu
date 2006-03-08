// EmuFED.cc

#include "EmuFED.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()

XDAQ_INSTANTIATOR_IMPL(EmuFED);

EmuFED::EmuFED(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuFED::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuFED::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuFED::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFED::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuFED::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuFED::configureAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuFED::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuFED");
}

xoap::MessageReference EmuFED::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	DOMNodeList *elements =
			message->getSOAPPart().getEnvelope().getBody()
			.getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE &&
				xoap::XMLCh2String(e->getLocalName()) == "runtype") {

			LOG4CPLUS_DEBUG(getApplicationLogger(),
					"==== PC: runtype: " <<
					xoap::XMLCh2String(e->getFirstChild()->getNodeValue()));
			break;
		}
	}

	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuFED::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void EmuFED::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFED::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFED::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
