/*****************************************************************************\
* $Id: DBAgent.cc,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/DBAgent.h"

#include "emu/fed/TStoreRequest.h"
#include "toolbox/TimeInterval.h"


emu::fed::DBAgent::DBAgent(xdaq::WebApplication *application):
application_(application)
{

}

void emu::fed::DBAgent::connect(const std::string &username, const std::string &password)
throw (emu::fed::exception::DBException)
{
	TStoreRequest request("connect");
	
	// Add view ID
	request.addTStoreParameter("id", "urn:tstore-view-SQL:EMUsystem");
	
	// This parameter is mandatory. "basic" is the only value allowed at the moment
	request.addTStoreParameter("authentication", "basic");
	
	//login credentials in format username/password
	request.addTStoreParameter("credentials", username + "/" + password);
	
	//connection will time out after 10 minutes
	toolbox::TimeInterval timeout(600,0); 
	request.addTStoreParameter("timeout", timeout.toString("xs:duration"));
	
	xoap::MessageReference message = request.toSOAP();
	
	xoap::MessageReference response;
	
	try {
		response = sendSOAPMessage(message, "");
	} catch (emu::fed::exception::SOAPException &e) {
		
		
		
	}
	
	//use the TStore client library to extract the response from the reply
	try {
		//connectionID_ = tstoreclient::connectionID(response);
	} catch (xcept::Exception &e) {
		
		
		
	}
}



xoap::MessageReference emu::fed::DBAgent::sendSOAPMessage(xoap::MessageReference message, std::string klass, int instance)
throw (emu::fed::exception::SOAPException)
{
	
	// find application
	xdaq::ApplicationDescriptor *app;
	try {
		if (instance == -1) {
			app = *(application_->getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass).begin());
		} else {
			app = application_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance);
		}
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Found no applications matching klass=" << klass << ", instance=" << instance;
		XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
	}
	
	// send the message
	try {
		return sendSOAPMessage(message, app);
	} catch (emu::fed::exception::SOAPException &e) {
		throw e;
	}
}


xoap::MessageReference emu::fed::DBAgent::sendSOAPMessage(xoap::MessageReference message, xdaq::ApplicationDescriptor *app)
throw (emu::fed::exception::SOAPException)
{
	// postSOAP() may throw an exception when failed.
	unsigned int iTries = 5;
	while (iTries > 0) {
		try {
			return application_->getApplicationContext()->postSOAP(message, *(application_->getApplicationDescriptor()), *app);
		} catch (xcept::Exception &e) {
			iTries--;
		}
	}
	
	if (iTries == 0) {
		std::ostringstream error;
		error << "Reached the maximum number of retries sending message";
		XCEPT_RAISE(emu::fed::exception::SOAPException, error.str());
	}

	// Prevents warnings
	return xoap::MessageReference();
}
