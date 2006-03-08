// EmuPeripheralCrate.cc

#include "EmuPeripheralCrate.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()

XDAQ_INSTANTIATOR_IMPL(EmuPeripheralCrate);

EmuPeripheralCrate::EmuPeripheralCrate(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuPeripheralCrate::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuPeripheralCrate::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuPeripheralCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuPeripheralCrate::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuPeripheralCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuPeripheralCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuPeripheralCrate::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuPeripheralCrate");
}

xoap::MessageReference EmuPeripheralCrate::onConfigure(xoap::MessageReference message)
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

xoap::MessageReference EmuPeripheralCrate::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void EmuPeripheralCrate::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuPeripheralCrate::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuPeripheralCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
