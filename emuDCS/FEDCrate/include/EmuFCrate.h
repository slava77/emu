#ifndef _EmuFCrate_h_
#define _EmuFCrate_h_

#include "LocalEmuApplication.h"
#include "EmuFController.h"

#include "xdata/String.h"

#include "IRQThreadManager.h"

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>

//#include "xdata/soap/Serializer.h"
#include "xdata/Vector.h"

using namespace std;
using namespace cgicc;

class EmuFCrate : public LocalEmuApplication, public EmuFController
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
	//xoap::MessageReference onGetParameters(xoap::MessageReference message)
	//	throw (xoap::exception::Exception);

	// Action methods called at state transitions.
	void configureAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);

	void setTTSBitsAction(toolbox::Event::Reference e) 
		throw (toolbox::fsm::exception::Exception);

	// HyperDAQ pages
	/** Outputs the HTML head tags and all that jazz.
	*
	*	@param out is the html output stream.
	*	@param myTitle is the title of the page.
	**/
	void Title(xgi::Output *out, string myTitle);

	/** Outputs the CSS style for simpler styling code.  Hopefully.
	*
	*	@param out is the html output stream.
	**/
	void CSS(xgi::Output *out);
	
	void webDefault(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
		
	void webFire(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);
		
	void webConfigure(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

	void webSetTTSBits(xgi::Input *in, xgi::Output *out)
		throw (xgi::exception::Exception);

// addition for STEP

	xdata::UnsignedInteger step_killfiber_;

	xoap::MessageReference onPassthru(xoap::MessageReference message)
		throw (xoap::exception::Exception);

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

	xdata::UnsignedInteger ttsID_;
	xdata::UnsignedInteger ttsCrate_;
	xdata::UnsignedInteger ttsSlot_;
	xdata::UnsignedInteger ttsBits_;

	IRQThreadManager *TM;
	
	bool soapConfigured_;
	bool soapLocal_;
	xdata::UnsignedLong runNumber_;
	
	xdata::Vector<xdata::Vector<xdata::UnsignedInteger> > dccInOut_;
	xdata::soap::Serializer serializer; // This makes SOAP so much easier!

	//string ttsIDStr_, ttsCrateStr_, ttsSlotStr_, ttsBitsStr_;

};

#endif  // ifndef _EmuFCrate_h_
// vim: set sw=4 ts=4:
