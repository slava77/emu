#ifndef _EmuFCrate_h_
#define _EmuFCrate_h_

#include "EmuFEDApplication.h"

#include "xdata/String.h"
#include "xdata/Vector.h"
#include <vector>

#include "Crate.h"
#include "IRQThreadManager.h"

using namespace std;
using namespace cgicc;

class EmuFCrate : public EmuFEDApplication
{

public:
	XDAQ_INSTANTIATOR();

	EmuFCrate(xdaq::ApplicationStub *s);

	// SOAP call-back methods
	xoap::MessageReference onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
		throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
		throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	xoap::MessageReference onSetTTSBits(xoap::MessageReference message)
		throw (xoap::exception::Exception);
	//xoap::MessageReference onUpdateFlash(xoap::MessageReference message)
	//	throw (xoap::exception::Exception);
	//xoap::MessageReference onGetParameters(xoap::MessageReference message)
	//	throw (xoap::exception::Exception);

	// Ugly, but it must be done.
	xoap::MessageReference onGetParameters(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	// Action methods called at state transitions.
	void configureAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception);

	//void setTTSBitsAction(toolbox::Event::Reference e)
	//	throw (toolbox::fsm::exception::Exception);
	//void updateFlashAction(toolbox::Event::Reference e)
	//	throw (toolbox::fsm::exception::Exception);

	// HyperDAQ pages

	void webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	void webFire(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	void webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	//void webSetTTSBits(xgi::Input *in, xgi::Output *out)
	//	throw (xgi::exception::Exception);

// addition for STEP

	xdata::UnsignedInteger step_killfiber_;

	xoap::MessageReference onPassthru(xoap::MessageReference message)
		throw (xoap::exception::Exception);

	// Copied over from EmuFController, now defunct
	void writeTTSBits(int crate, int slot, unsigned int bits);
	unsigned int readTTSBits(int crate, int slot);
	

// end addition for STEP

private:
	//
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
		throw (toolbox::fsm::exception::Exception);
	//
	void webRedirect(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	//
	string getCGIParameter(xgi::Input *in, string name);

	xdata::String xmlFile_;
	xdata::Vector<xdata::String> errorChambers_;

	xdata::UnsignedInteger ttsID_;
	xdata::UnsignedInteger ttsCrate_;
	xdata::UnsignedInteger ttsSlot_;
	xdata::UnsignedInteger ttsBits_;

	IRQThreadManager *TM;

	bool soapConfigured_;
	bool soapLocal_;

	xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > dccInOut_;
	xdata::soap::Serializer serializer; // This makes SOAP so much easier!

	//string ttsIDStr_, ttsCrateStr_, ttsSlotStr_, ttsBitsStr_;
	xdata::String endcap_; // Will say something like "plus" or "minus"

	std::vector<Crate *> crateVector; // Very useful, just like in EFCHD

};

#endif  // ifndef _EmuFCrate_h_
// vim: set sw=4 ts=4:
