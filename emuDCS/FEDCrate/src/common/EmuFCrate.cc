#include "EmuFCrate.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"  // xoap::bind()
#include "xgi/Method.h"  // xgi::bind()

#include "cgicc/HTMLClasses.h"

using namespace cgicc;
using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuFCrate);
  
EmuFCrate::EmuFCrate(xdaq::ApplicationStub *s): EmuApplication(s) 
{
    //
    // State machine definition
    //

    // SOAP call-back functions, which relays to *Action method.
    xoap::bind(this, &EmuFCrate::onConfigure, "Configure", XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrate::onEnable,    "Enable",    XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrate::onDisable,   "Disable",   XDAQ_NS_URI);
    xoap::bind(this, &EmuFCrate::onHalt,      "Halt",      XDAQ_NS_URI);

    // 'fsm_' is defined in EmuApplication
    fsm_.addState('H', "Halted",     this, &EmuFCrate::stateChanged);
    fsm_.addState('C', "Configured", this, &EmuFCrate::stateChanged);
    fsm_.addState('E', "Enabled",    this, &EmuFCrate::stateChanged);

    fsm_.addStateTransition(
	    'H', 'C', "Configure", this, &EmuFCrate::configureAction);
    fsm_.addStateTransition(
	    'C', 'C', "Configure", this, &EmuFCrate::configureAction);
    fsm_.addStateTransition(
	    'C', 'E', "Enable",    this, &EmuFCrate::enableAction);
    fsm_.addStateTransition(
	    'E', 'C', "Disable",   this, &EmuFCrate::disableAction);
    fsm_.addStateTransition(
	    'C', 'H', "Halt",      this, &EmuFCrate::haltAction);
    fsm_.addStateTransition(
	    'E', 'H', "Halt",      this, &EmuFCrate::haltAction);
    fsm_.addStateTransition(
	    'H', 'H', "Halt",      this, &EmuFCrate::haltAction);

    fsm_.setInitialState('H');
    fsm_.reset();

    // 'state_' is defined in EmuApplication
    state_ = fsm_.getStateName(fsm_.getCurrentState());

    // Exported parameters
    getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);

    // HyperDAQ pages
    xgi::bind(this, &EmuFCrate::webDefault, "Default");
}

xoap::MessageReference EmuFCrate::onConfigure(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
    fireEvent("Configure");

    return createReply(message);
}

xoap::MessageReference EmuFCrate::onEnable(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
    fireEvent("Enable");

    return createReply(message);
}

xoap::MessageReference EmuFCrate::onDisable(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
    fireEvent("Disable");

    return createReply(message);
}

xoap::MessageReference EmuFCrate::onHalt(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
    fireEvent("Halt");

    return createReply(message);
}

void EmuFCrate::configureAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
    // calls only EmuFController::configure().
    // init() is called in the nexe 'Enable' step.
    configure();
    cout << "Configure " << xmlFile_.toString() << endl;
    cout << "Received Message Configure" << endl ;
}

void EmuFCrate::enableAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
    SetConfFile(xmlFile_);
    init();
    cout << "Received Message Enable" << endl ;
}

void EmuFCrate::disableAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
    // do nothing
    cout << "Received Message Disable" << endl ;
}

void EmuFCrate::haltAction(toolbox::Event::Reference e) 
	throw (toolbox::fsm::exception::Exception)
{
    // do nothing
    cout << "Received Message Halt" << endl ;
}

// HyperDAQ pages
void EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
	throw (xgi::exception::Exception)
{
    //
    *out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
    //
    *out << html().set("lang", "en").set("dir", "ltr") << endl;
    *out << title("EmuFCrate") << endl;
    //
    *out << h1("EmuFCrate") << endl ;
    //
    *out << br() << endl;
}

//
void EmuFCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
	throw (toolbox::fsm::exception::Exception)
{
    EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set sw=4 ts=4:
