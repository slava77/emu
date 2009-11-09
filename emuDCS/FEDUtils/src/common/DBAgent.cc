/*****************************************************************************\
* $Id: DBAgent.cc,v 1.8 2009/11/09 11:46:33 paste Exp $
\*****************************************************************************/
#include "emu/fed/DBAgent.h"

#include <sstream>

#include "emu/base/TStoreRequest.h"
#include "toolbox/TimeInterval.h"
#include "tstore/client/Client.h"
#include "tstore/client/AttachmentUtils.h"
#include "xdata/TableIterator.h"



emu::fed::DBAgent::DBAgent(xdaq::WebApplication *application):
application_(application)
{
	// This is to read the configuration from the view file
	// but it's completely unnecessary. :)
	/*
	std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUFEDsystem");
	emu::base::TStoreRequest request("getConfiguration", viewClass);
	
	//add the view ID
	request.addTStoreParameter("id", "urn:tstore-view-SQL:EMUFEDsystem");
	
	//add view specific parameter
	// This is a standard location
	request.addTStoreParameter("path", "/opt/xdaq/htdocs/emu/emuDCS/FEDUtils/xml/TStoreConfiguration.xml");
	
	xoap::MessageReference message = request.toSOAP();
	xoap::MessageReference response;
	try {
		response = sendSOAPMessage(message, "tstore::TStore");
	} catch (emu::fed::exception::SOAPException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error sending SOAP message", e);
	}
	
	if (response->getSOAPPart().getEnvelope().getBody().hasFault()) {
		XCEPT_RAISE(emu::fed::exception::DBException, "Error attempting to set configuration file");
	}
	
	
	DOMNode *configNode=tstoreclient::getNodeNamed(response,"getConfigurationResponse");
	//configNode contains the requested configuration.
	std::cout << "configuration corresponding to xpath " << xpath << " is: " << tstoreclient::writeXML(configNode) << std::endl;
	*/
}


emu::fed::DBAgent::~DBAgent()
{
	// This causes bad things to happen
	// disconnect();
}


//if you have already opened a connection and want to use the existing connection to query, set the connection ID instead of calling connect()
void emu::fed::DBAgent::setConnectionID(const std::string &connectionID) {
	connectionID_ = connectionID;
	//should maybe renew the connection or otherwise check that it's valid?
}



void emu::fed::DBAgent::connect(const std::string &username, const std::string &password)
throw (emu::fed::exception::DBException)
{
	//maybe if a connectionID has already been set using setConnectionID, this function should just renew it, or do nothing.
	
	emu::base::TStoreRequest request("connect");
	
	// Add view ID
	request.addTStoreParameter("id", "urn:tstore-view-SQL:EMUFEDsystem");
	
	// This parameter is mandatory. "basic" is the only value allowed at the moment
	request.addTStoreParameter("authentication", "basic");
	
	//login credentials in format username/password
	request.addTStoreParameter("credentials", username + "/" + password);
	
	//connection will time out after 10 minutes
	toolbox::TimeInterval timeout(600, 0); 
	request.addTStoreParameter("timeout", timeout.toString("xs:duration"));
	
	xoap::MessageReference message = request.toSOAP();
	
	xoap::MessageReference response;
	
	try {
		response = sendSOAPMessage(message, "tstore::TStore");
	} catch (emu::fed::exception::SOAPException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error sending SOAP message", e);
	}
	
	//use the TStore client library to extract the response from the reply
	try {
		connectionID_ = tstoreclient::connectionID(response);
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to parse connection ID", e);
	}
}



void emu::fed::DBAgent::disconnect()
throw (emu::fed::exception::DBException) {
	emu::base::TStoreRequest request("disconnect");
	
	//add the connection ID
	request.addTStoreParameter("connectionID", connectionID_);
	
	xoap::MessageReference message = request.toSOAP();
	
	try {
		sendSOAPMessage(message, "tstore::Tstore");
	} catch (emu::fed::exception::SOAPException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error sending SOAP message", e);
	}
}



xdata::Table emu::fed::DBAgent::query(const std::string &queryViewName, const std::map<std::string, std::string> &queryParameters)
throw (emu::fed::exception::DBException) {
	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUFEDsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	emu::base::TStoreRequest request("query", viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID", connectionID_);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	request.addViewSpecificParameter("name", queryViewName);
	
	// add parameter names and values
	for (std::map<std::string, std::string>::const_iterator iPair = queryParameters.begin(); iPair != queryParameters.end(); iPair++) {
		request.addViewSpecificParameter(iPair->first, iPair->second);
	}
	
	xoap::MessageReference message = request.toSOAP();
	xoap::MessageReference response;
	try {
		response = sendSOAPMessage(message, "tstore::TStore");
	} catch (emu::fed::exception::SOAPException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error sending SOAP message", e);
	}	

	//use the TStore client library to extract the first attachment of type "table"
	//from the SOAP response
	xdata::Table results;
	if (!tstoreclient::getFirstAttachmentOfType(response, results)) {
		XCEPT_RAISE (emu::fed::exception::DBException, "Server returned no data");
	}
	return results;
}



void emu::fed::DBAgent::insert(const std::string &insertViewName, const xdata::Table &newRows)
throw (emu::fed::exception::DBException) {

	//for a query, we need to send some parameters which are specific to SQLView.
	//these use the namespace tstore-view-SQL. 
	
	//In general, you might have the view name in a variable, so you won't know the view class. In this
	//case you can find out the view class using the TStore client library:
	std::string viewClass = tstoreclient::classNameForView("urn:tstore-view-SQL:EMUFEDsystem");
	
	//If we give the name of the view class when constructing the TStoreRequest, 
	//it will automatically use that namespace for
	//any view specific parameters we add.
	emu::base::TStoreRequest request("insert", viewClass);
	
	//add the connection ID
	request.addTStoreParameter("connectionID", connectionID_);
	
	//for an SQLView, the name parameter refers to the name of a query section in the configuration
	//We'll use the "test" one.
	request.addViewSpecificParameter("name", insertViewName);
	
	xoap::MessageReference message = request.toSOAP();
	
	//add our new rows as an attachment to the SOAP message
	//the last parameter is the ID of the attachment. The SQLView does not mind what it is, as there should only be one attachment per message.
	// For shame!
	xdata::Table myNewRows = newRows;
	tstoreclient::addAttachment(message, myNewRows, "whatever");
	
	xoap::MessageReference response;
	try {
		response = sendSOAPMessage(message, "tstore::TStore");
	} catch (emu::fed::exception::SOAPException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error sending SOAP message", e);
	}
	
	if (response->getSOAPPart().getEnvelope().getBody().hasFault()) {
		XCEPT_RAISE(emu::fed::exception::DBException, "Error inserting data into database");
	}
}



xdata::Table emu::fed::DBAgent::getAll()
throw (emu::fed::exception::DBException)
{
	// Push back the table name
	std::map<std::string, std::string> parameters;
	parameters["TABLE"] = table_;
	
	// Return the results of the query
	try {
		return query("get_all", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
}



xdata::Table emu::fed::DBAgent::getByID(const xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Push back the table name
	std::map<std::string, std::string> parameters;
	parameters["TABLE"] = table_;
	
	// Push back the ID
	// XDATA people are lazy
	xdata::UnsignedInteger64 myID = id;
	parameters["ID"] = myID.toString();
	
	// Return the results of the query
	try {
		return query("get_all_by_id", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
}



xdata::Table emu::fed::DBAgent::getByKey(const xdata::UnsignedInteger64 &key)
throw (emu::fed::exception::DBException)
{
	// Push back the table name
	std::map<std::string, std::string> parameters;
	parameters["TABLE"] = table_;
	
	// Push back the key
	// XDATA people are lazy
	xdata::UnsignedInteger64 myKey = key;
	parameters["KEY"] = myKey.toString();
	
	// Return the results of the query
	try {
		return query("get_all_by_key", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
}



xoap::MessageReference emu::fed::DBAgent::sendSOAPMessage(const xoap::MessageReference &message, const std::string &klass, const int &instance)
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
		//message->writeTo(std::cerr);
		//xoap::MessageReference reply= sendSOAPMessage(message, app);
		//std::cerr << std::endl << "Response: " << std::endl;
		//reply->writeTo(std::cerr);
		//return reply;
		return sendSOAPMessage(message, app);
	} catch (emu::fed::exception::SOAPException &e) {
		throw e;
	}
}


xoap::MessageReference emu::fed::DBAgent::sendSOAPMessage(const xoap::MessageReference &message, xdaq::ApplicationDescriptor *app)
throw (emu::fed::exception::SOAPException)
{
	// XDAQ people are lazy
	xoap::MessageReference myMessage = message;
	// postSOAP() may throw an exception when failed.
	try {
		return application_->getApplicationContext()->postSOAP(myMessage, *(application_->getApplicationDescriptor()), *app);
	} catch (xcept::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::SOAPException, "Error sending SOAP message", e);
	}
}

void emu::fed::DBAgent::setValue(xdata::Serializable &destination,xdata::Table::Row &sourceRow,const std::string &columnName) throw (emu::fed::exception::DBException) {
	try {
		setValue(destination,sourceRow.getField(columnName));
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException,"Can't convert column "+columnName+" to expected type",e);
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException,"Can't read column "+columnName,e);
	}
}

void emu::fed::DBAgent::setValue(xdata::Serializable &destination,xdata::Serializable *source) throw (emu::fed::exception::DBException) {
	if (!source) {
		XCEPT_RAISE(emu::fed::exception::DBException,"Can't convert NULL value to "+destination.type());
	}
	if (source->type()!=destination.type()) {
		XCEPT_RAISE(emu::fed::exception::DBException,"Can't convert "+source->type()+" value to "+destination.type());
	}
	try {
		//could also do this by making the destination a template type and using dynamic_cast, but this way is a bit simpler
		//and probably no less efficient since the value is copied anyway.
		destination.setValue(*source);
	} catch (std::bad_cast &e) {
		//if this happens then the two objects profess to have the same type but are in fact different types; should not happen unless there is a bug in xdata.
		XCEPT_RAISE(emu::fed::exception::DBException,"Can't copy one "+source->type()+" to another. Maybe two xdata types return the same value from type()");
	} catch (std::exception &e) {
		XCEPT_RAISE(emu::fed::exception::DBException,"Can't copy one "+source->type()+" to another. "+e.what());
	}
}
