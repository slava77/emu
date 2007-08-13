#include "EmuFCrate.h"

// addition for STEP
#include "Crate.h"
#include "DDU.h"
// end addition for STEP


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
	xoap::bind(this, &EmuFCrate::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);

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
	fsm_.addStateTransition(
			'H', 'H', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	fsm_.addStateTransition(
			'C', 'C', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);
	fsm_.addStateTransition(
			'E', 'E', "SetTTSBits", this, &EmuFCrate::setTTSBitsAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	// 'state_' is defined in EmuApplication
	state_ = fsm_.getStateName(fsm_.getCurrentState());

	// Exported parameters
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);

	getApplicationInfoSpace()->fireItemAvailable("ttsCrate", &ttsCrate_);
	getApplicationInfoSpace()->fireItemAvailable("ttsSlot",  &ttsSlot_);
	getApplicationInfoSpace()->fireItemAvailable("ttsBits",  &ttsBits_);

	// HyperDAQ pages
	xgi::bind(this, &EmuFCrate::webDefault, "Default");
	xgi::bind(this, &EmuFCrate::webConfigure, "Configure");
	xgi::bind(this, &EmuFCrate::webSetTTSBits, "SetTTSBits");

	// addition for STEP
	getApplicationInfoSpace()->fireItemAvailable ("step_killfiber", &step_killfiber_);
	xoap::bind(this, &EmuFCrate::onPassthru,  "Passthru", XDAQ_NS_URI);
	// end addition for STEP
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

xoap::MessageReference EmuFCrate::onSetTTSBits(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("SetTTSBits");

	return createReply(message);
}

void EmuFCrate::configureAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	// calls only EmuFController::init().
	// configure() is called in the nexe 'Enable' step.
	SetConfFile(xmlFile_);
	init();
	cout << "Configure " << xmlFile_.toString() << endl;
	cout << "Received Message Configure" << endl ;
}

void EmuFCrate::enableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	configure();
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

void EmuFCrate::setTTSBitsAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception)
{
	// set sTTS bits
	writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);

	// read back sTTS bits
	ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);

	cout << "Received Message SetTTSBits" << endl ;
}

// HyperDAQ pages
void EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	*out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
	*out << html().set("lang", "en").set("dir", "ltr") << endl;

	//
	*out << title("EmuFCrate") << endl;

	//
	*out << h1("EmuFCrate") << endl ;
	*out << br() << endl;

	// One click configure
	*out << "Config file: " << xmlFile_.toString() << endl;
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Configure") << endl;
	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "Configure") << endl;
	*out << form() << endl;

	// sTTS control
	*out << hr() << endl;
	*out << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/SetTTSBits") << endl;

	*out << "Crate # (1-4): " << endl;
	*out << input().set("type", "text")
			.set("name", "ttscrate")
			.set("value", ttsCrateStr_)
			.set("size", "10") << br() << endl;

	*out << "Slot # (4-13): " << endl;
	*out << input().set("type", "text")
			.set("name", "ttsslot")
			.set("value", ttsSlotStr_)
			.set("size", "10") << br() << endl;

	*out << "TTS value (0-15, decimal): " << endl;
	*out << input().set("type", "text")
			.set("name", "ttsbits")
			.set("value", ttsBitsStr_)
			.set("size", "10") << br() << endl;

	*out << input().set("type", "submit")
			.set("name", "command")
			.set("value", "SetTTSBits") << endl;
	*out << form() << endl;
}

void EmuFCrate::webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	fireEvent("Configure");

	fireEvent("Enable");

	webRedirect(in, out);
}

void EmuFCrate::webSetTTSBits(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	ttsCrateStr_ = getCGIParameter(in, "ttscrate");
	ttsSlotStr_  = getCGIParameter(in, "ttsslot");
	ttsBitsStr_  = getCGIParameter(in, "ttsbits");

	ttsCrate_.fromString(ttsCrateStr_);
	ttsSlot_.fromString(ttsSlotStr_);
	ttsBits_.fromString(ttsBitsStr_);

	fireEvent("SetTTSBits");

	ttsCrateStr_ = ttsCrate_.toString();
	ttsSlotStr_  = ttsSlot_.toString();
	ttsBitsStr_  = ttsBits_.toString();

	webRedirect(in, out);
}

void EmuFCrate::webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception)
{
	string url = in->getenv("PATH_TRANSLATED");

	HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location",
			url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

string EmuFCrate::getCGIParameter(xgi::Input *in, string name)
{
	cgicc::Cgicc cgi(in);
	string value;

	form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}

//
void EmuFCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// addition for STEP

// this routine is written in the assumption that we have only one DDU at this time, for STEP setup.
// if used on multy-ddu setup, will program them all with the same killfiber.
// user must program step_killfiber_ before calling this routine.
xoap::MessageReference EmuFCrate::onPassthru(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	// find crates
	std::vector<Crate*> myCrates = selector().crates();

	for(unsigned i = 0; i < myCrates.size(); ++i) 
	{
		// find DDUs in each crate
		std::vector<DDU*> myDdus = myCrates[i]->ddus();
		for(unsigned j =0; j < myDdus.size(); ++j) 
		{
			cout << "Setting passthru mode for crate: " << i << " DDU: " << j << " slot: " << myDdus[j]->slot()
				 << " fiber mask: 0x" << hex << step_killfiber_.toString() << dec << endl;

			if (myDdus[j]->slot() < 21) 
			{
				myDdus[j]->vmepara_wr_GbEprescale (0xf0f0); // no prescaling
				myDdus[j]->vmepara_wr_fakel1reg   (0x8787); // fake L1A for each event
				myDdus[j]->ddu_loadkillfiber      (step_killfiber_); // user selects which inputs to use
				myDdus[j]->ddu_reset(); // sync reset via VME
			}
		}
	}
	return createReply(message);
}

// end addition for STEP


// End of file
// vim: set sw=4 ts=4:
