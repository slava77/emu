// EmuDAQManager.cc

#include "EmuDAQManager.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"  // XMLCh2String()

XDAQ_INSTANTIATOR_IMPL(EmuDAQManager);

EmuDAQManager::EmuDAQManager(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuDAQManager::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuDAQManager::onEnable,    "Enable",    XDAQ_NS_URI);
	xoap::bind(this, &EmuDAQManager::onDisable,   "Disable",   XDAQ_NS_URI);
	xoap::bind(this, &EmuDAQManager::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuDAQManager::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuDAQManager::stateChanged);
	fsm_.addState('E', "Enabled",    this, &EmuDAQManager::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuDAQManager::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuDAQManager::configureAction);
	fsm_.addStateTransition(
			'C', 'E', "Enable",    this, &EmuDAQManager::enableAction);
	fsm_.addStateTransition(
			'E', 'C', "Disable",   this, &EmuDAQManager::disableAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuDAQManager::haltAction);
	fsm_.addStateTransition(
			'E', 'H', "Halt",      this, &EmuDAQManager::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuDAQManager");
}

xoap::MessageReference EmuDAQManager::onConfigure(xoap::MessageReference message)
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

xoap::MessageReference EmuDAQManager::onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Enable");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Disable");

	return createReply(message);
}

xoap::MessageReference EmuDAQManager::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void EmuDAQManager::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuDAQManager::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
