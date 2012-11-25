/*****************************************************************************\
* $Id: Supervised.cc,v 1.6 2012/11/25 23:29:50 banicz Exp $
\*****************************************************************************/
#include "emu/fed/Supervised.h"

#include <sstream>
#include <fstream>

#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xgi/Method.h"
#include "toolbox/fsm/FailedEvent.h"
#include "xcept/tools.h"

emu::fed::Supervised::Supervised(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::base::Supervised(stub),
runNumber_(0),
runType_("none"),
ignoreSOAP_(false),
fromSOAP_(true)
{
	getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
	getApplicationInfoSpace()->fireItemAvailable("runType", &runType_);
	getApplicationInfoSpace()->fireItemAvailable("state", &state_); // lower case for me
	getApplicationInfoSpace()->fireItemAvailable("ignoreSOAP", &ignoreSOAP_);
	xgi::bind(this, &emu::fed::Supervised::webFire, "Fire");
	xgi::bind(this, &emu::fed::Supervised::webIgnoreSOAP, "IgnoreSOAP");
}


void emu::fed::Supervised::fireEvent(std::string name)
{
	if (fromSOAP_) {
		if (ignoreSOAP_) {
			std::ostringstream error;
			error << "Ignoring SOAP event named " << name << " to application " << getApplicationDescriptor()->getClassName() << " instance " << getApplicationDescriptor()->getInstance();
			XCEPT_DECLARE(emu::fed::exception::FSMException, e2, error.str());
			LOG4CPLUS_WARN(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("WARN", e2);
			return;
		}
	} else {
		fromSOAP_ = true;
	}

	toolbox::Event::Reference event((new toolbox::Event(name, this)));
	try {
		fsm_.fireEvent(event);
	} catch (toolbox::fsm::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception caught firing event named " << name << " to application " << getApplicationDescriptor()->getClassName() << " instance " << getApplicationDescriptor()->getInstance();
		XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
		LOG4CPLUS_FATAL(getApplicationLogger(), xcept::stdformat_exception_history(e2));
		notifyQualified("FATAL", e2);
		throw e;
	}
}



void emu::fed::Supervised::webFire(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);
	fromSOAP_ = false;

	if (cgi.getElement("action") != cgi.getElements().end()) {
		std::string action = cgi["action"]->getValue();
		LOG4CPLUS_DEBUG(getApplicationLogger(), "FSM state change from web requested: " << action);
		try {
			fireEvent(action);
		} catch (emu::fed::exception::FSMException &e) {
			std::ostringstream error;
			error << "Error in FSM state change from web with action " << action;
			XCEPT_DECLARE_NESTED(emu::fed::exception::FSMException, e2, error.str(), e);
			LOG4CPLUS_FATAL(getApplicationLogger(), xcept::stdformat_exception_history(e2));
			notifyQualified("FATAL", e2);
		}
	}

	if (cgi.getElement("ajax") != cgi.getElements().end() && cgi["ajax"]->getIntegerValue() == 1) {

		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);

	} else {

		std::string url = in->getenv("PATH_TRANSLATED");
		url = url.substr(0, url.find("/" + in->getenv("PATH_INFO")));

		cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

		header.getStatusCode(303);
		header.getReasonPhrase("See Other");
		header.addHeader("Location", url);

	}
}



void emu::fed::Supervised::webIgnoreSOAP(xgi::Input *in, xgi::Output *out)
{
	cgicc::Cgicc cgi(in);

	if (cgi.getElement("ignoreSOAP") != cgi.getElements().end()) {
		ignoreSOAP_ = (cgi["ignoreSOAP"]->getIntegerValue() == 1 ? true : false);
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Ignore SOAP changed to " << ignoreSOAP_);
	}

}

