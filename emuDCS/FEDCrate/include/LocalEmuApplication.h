#ifndef __LOCALEMUAPPLICATION_H__
#define __LOCALEMUAPPLICATION_H__

#include "xdaq/WebApplication.h"
#include "EmuApplication.h"

#include <string>

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/DOMParser.h"
#include "xoap/SOAPElement.h"
#include "xoap/domutils.h"
#include "xoap/Method.h"

#include "xdata/xdata.h"
#include "xdata/soap/Serializer.h"

using namespace std;

class LocalEmuApplication: public EmuApplication {

public:

	LocalEmuApplication(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception):
		EmuApplication(stub)
	{
		xoap::bind(this, &LocalEmuApplication::onGetParameters, "GetParameters", XDAQ_NS_URI);
	}

	/** Sends the GetParameters SOAP command to a target application.
	*
	*	@param applicationDescriptor is a pointer to the target application's
	*	descriptor.
	*	@returns a SOAP message reply for use with @sa readParameter.
	*
	*	@author Phillip Killewald <paste@mps.ohio-state.edu>
	**/
	xoap::MessageReference getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
		throw (xdaq::exception::Exception)
	{
	
		xoap::MessageReference message = xoap::createMessage();
		xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
		xoap::SOAPBody body = envelope.getBody();
		
		xoap::SOAPName command = envelope.createName("GetParameters", "xdaq", XDAQ_NS_URI);
		body.addBodyElement(command);
		
		xoap::MessageReference reply;
		try {
			reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *applicationDescriptor);
		} catch (xdaq::exception::Exception &e) {
			// PGK WOAH!  Proper exception handling!
			LOG4CPLUS_ERROR(getApplicationLogger(), e.what());
			XCEPT_RAISE(xdaq::exception::Exception, "Error in posting SOAP message");
		}
		
		return reply;
	
	}
	
	/** Sends the GetParameters SOAP command to a target application.
	*
	*	@param applicationName is the class name of the application from which
	*	you are requesting parameters.
	*	@param instance is the instance of the application to which you are
	*	sending the request.
	*	@returns a SOAP message reply for use with @sa readParameter.
	*
	*	@author Phillip Killewald <paste@mps.ohio-state.edu>
	**/
	xoap::MessageReference getParameters(string applicationName, unsigned int instance)
		throw (xdaq::exception::Exception)
	{

		xoap::MessageReference reply;

		std::set<xdaq::ApplicationDescriptor *> apps;
		try {
			apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(applicationName);
		} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
			// PGK WOAH!  Proper exception handling!
			LOG4CPLUS_WARN(getApplicationLogger(), e.what());
			XCEPT_RAISE(xdaq::exception::Exception, "Application class name not found.  Can't continue.");
		}
		
		std::set<xdaq::ApplicationDescriptor *>::iterator i;
		bool instanceFound = false;
		for (i = apps.begin(); i != apps.end(); ++i) {
			if ((*i)->getInstance() != instance) continue;
			instanceFound = true;
			reply = getParameters(*i);
			break;
		}
		
		if (!instanceFound) XCEPT_RAISE(xdaq::exception::Exception, "Application instance not found.  Can't continue.");
		return reply;
	
	}

	/** Reads a reply from onGetParameters and returns a named parameter from
	*	a target ApplicationInfoSpace.
	*
	*	@note We cannot separate templated declarations from templated definitions.
	*	Sorry.
	*
	*	@param message is the SOAP reply from onGetParameters containing a
	*	serialized form of the target ApplicationInfoSpace.
	*	@param parameterName is the name of whatever you want from the
	*	target ApplicationInfoSpace.
	*	@class T is an xdata data type that xoap can serialize:  anything that
	*	extends the xdata::Serializable class (but not an xdata::Serializable
	*	instance itself).
	*
	*	@author Phillip Killewald <paste@mps.ohio-state.edu>
	**/
	template<class T>
	T readParameter(xoap::MessageReference message, std::string parameterName)
		throw (xoap::exception::Exception)
	{
		T thingToGet;
		xdata::soap::Serializer serializer;
		xoap::DOMParser* parser = new xoap::DOMParser();
		std::string messageStr;
		message->writeTo(messageStr);
		DOMDocument *doc = parser->parse(messageStr);
		
		//xoap::SOAPElement sInfoSpace = message->getSOAPPart().getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("GetParametersResponse", "", "")))[0];
		
		xoap::SOAPName *name = new xoap::SOAPName(parameterName, "xdaq", XDAQ_NS_URI);
		//cout << "Looking for " << name->getQualifiedName() << ", " << name->getURI() << endl;
		
		
		DOMNodeList* dataNode = doc->getElementsByTagNameNS(xoap::XStr(XDAQ_NS_URI), xoap::XStr("data"));
		DOMNodeList* dataElements = dataNode->item(0)->getChildNodes();
		
		for (unsigned int j = 0; j < dataElements->getLength(); j++) {
			DOMNode* n = dataElements->item(j);
			if (n->getNodeType() == DOMNode::ELEMENT_NODE) {
				std::string nodeName = xoap::XMLCh2String(n->getNodeName());
				//cout << "Found " << nodeName << endl;
				if (nodeName == name->getQualifiedName()) {
					serializer.import (&thingToGet, n);
				}
			}
		}
		return thingToGet;
	}

protected:

	/** Serializes this application's ApplicationInfoSpace and returns it.
	*	As things stand, is bound to the "GetParameters" SOAP command.
	*
	*	@author Phillip Killewald <paste@mps.ohio-state.edu>
	**/
	xoap::MessageReference onGetParameters(xoap::MessageReference message)
		throw (xoap::exception::Exception)
	{
		xdata::soap::Serializer serializer;

		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
		xoap::SOAPBody body = envelope.getBody();
		
		xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

		//bool first = true;

		std::map<string, xdata::Serializable *>::iterator iParam;
		for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
			string name = (*iParam).first;
			xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
			xoap::SOAPElement element = bodyElement.addChildElement(elementName);
			serializer.exportAll((*iParam).second, dynamic_cast<DOMElement*>(element.getDOMNode()), true);
			//first = false;
		}

		//reply->writeTo(cout);
		//cout << endl;
		return reply;

	}

private:

};


#endif
