/*****************************************************************************\
* $Id:
*
* $Log:
\*****************************************************************************/
#include "emu/fed/Supervised.h"

#include <sstream>
#include <fstream>

#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xgi/Method.h"
#include "toolbox/fsm/FailedEvent.h"

emu::fed::Supervised::Supervised(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::base::Supervised(stub),
runNumber_(0),
soapLocal_(false),
soapConfigured_(false)
{
	getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
	xgi::bind(this, &emu::fed::Supervised::webFire, "Fire");
}


/*
void emu::fed::Supervised::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
{
	state_ = fsm.getStateName(fsm.getCurrentState());
	LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << state_.toString());
}
*/

/*
void emu::fed::Supervised::transitionFailed(toolbox::Event::Reference event)
{
	toolbox::fsm::FailedEvent &failed = dynamic_cast<toolbox::fsm::FailedEvent &>(*event);
	
	std::ostringstream error;
	error <<  "Failure occurred when performing transition from " << failed.getFromState() << " to " << failed.getToState() << ", exception: " << failed.getException().what();
	LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
	XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e, error.str(), failed.getException());
	notifyQualified("FATAL", e);
}
*/


void emu::fed::Supervised::fireEvent(std::string name)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));
	try {
		fsm_.fireEvent(event);
	} catch (toolbox::fsm::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception caught firing event named " << name << " to application " << getApplicationDescriptor()->getClassName() << " instance " << getApplicationDescriptor()->getInstance();
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
	}
}



void emu::fed::Supervised::webFire(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);
	soapLocal_ = true;
	
	std::string action = "";
	cgicc::form_iterator name = cgi.getElement("action");
	if (name != cgi.getElements().end()) {
		action = cgi["action"]->getValue();
		LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM state change from web requested: " << action);
		try {
			fireEvent(action);
		} catch (emu::fed::exception::FSMException &e) {
			std::ostringstream error;
			error << "Error in FSM state change from web with action " << action;
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
			notifyQualified("FATAL", e2);
		}
	}
	
	std::string url = in->getenv("PATH_TRANSLATED");
	url = url.substr(0, url.find("/" + in->getenv("PATH_INFO")));
	
	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
	
	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location", url);
}

