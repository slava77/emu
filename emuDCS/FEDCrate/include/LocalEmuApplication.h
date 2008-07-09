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

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/configurator.h>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

using namespace std;


class LocalEmuApplication: public EmuApplication {

public:

	xdata::UnsignedLong runNumber_;
	xdata::String runType_;

	/** Time in seconds to automatically refresh a HyperDAQ page.
	*	Negative numbers turn off auto-refresh.
	**/
	xdata::Integer autoRefresh_;

	LocalEmuApplication(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception):
		EmuApplication(stub),
		runNumber_(0),
		autoRefresh_(20),
		NS_XSI("http://www.w3.org/2001/XMLSchema-instance"),
		STATE_UNKNOWN("unknown")
	{
		xoap::bind(this, &LocalEmuApplication::onGetParameters, "GetParameters", XDAQ_NS_URI);

		// PGK Making these available on the ApplicationInfoSpace will allow
		//  the CSCSV to set them with the "ParameterSet" SOAP command.
		getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
		getApplicationInfoSpace()->fireItemAvailable("runType", &runType_);

		// PGK We deal with INFO level unless we are doing a Debug run.
		getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
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

	/** Sets a parameter in a remote application.  I don't know where the
	*	binding to this command is set, but it seems to affect all
	*	of the applications we are concerned with.
	*
	*	@param klass is the target application class name (this
	*	function sends to all of the instances of that class.)
	*	@param name is the name of the variable you want to set.
	*	@param type is the type of the varialbe you want to set.  Use
	*	strings like "xsd:string" and "xsd:unsignedLong".
	*	@param value is a string representation of the value to be set.
	*
	*	@note Because I don't know where this call is handled, I do not
	*	know anything about error handling.  Just try to make sure
	*	the variable exists in the exported InfoSpace of the target
	*	application, and that the types match.
	*
	*	@author Phillip Killewald (stolen from Laria's CSCSupervisor.cc)
	**/
	void setParameter(string klass, string name, string type, string value)
	{

		// find applications
		std::set<xdaq::ApplicationDescriptor *> apps;
		try {
			apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
		} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
			LOG4CPLUS_WARN(getApplicationLogger(), "There are no applications with the class name " << klass << " in this context.");
			return; // Do nothing if the target doesn't exist
		}

		LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter " << klass << " " << name << " " << type << " " << value);

		// prepare a SOAP message
		xoap::MessageReference message = xoap::createMessage();
		xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
		envelope.addNamespaceDeclaration("xsi", NS_XSI);

		xoap::SOAPName command = envelope.createName("ParameterSet", "xdaq", XDAQ_NS_URI);
		xoap::SOAPName properties = envelope.createName("properties", klass, "urn:xdaq-application:" + klass);
		xoap::SOAPName parameter = envelope.createName(name, klass, "urn:xdaq-application:" + klass);
		xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

		xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
		properties_e.addAttribute(xsitype, "soapenc:Struct");

		xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
		parameter_e.addAttribute(xsitype, type);
		parameter_e.addTextNode(value);

		xoap::MessageReference reply;

		// send the message one-by-one
		std::set<xdaq::ApplicationDescriptor *>::iterator i;
		for (i = apps.begin(); i != apps.end(); ++i) {
			reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), **i);
			// Analysis here, if debugging needed.
		}

		LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter successfully returning");

		return;
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

	/** Returns a standard Header for the EmuFCrate pages.  Displays a title,
	*	 the experts (with links), and some cool pictures.
	*
	*	@note The images that should be displayed are hard-linked to the /tmp
	*	 directory.  This means that the images in the FEDCrate directory must
	*	 be copied over to the /tmp directory on the machine that it runs to
	*	 make everything pretty.
	*
	*	@param myTitle is the title that will be displayed at the top of the
	*	 page.
	*	@returns a huge string that is basically the header of the page in
	*	 HTML.  Good for outputting straight to the xgi::Output.
	*
	*	@sa the autoRefresh_ member and the CSS method.
	**/
	virtual string Header(string myTitle,bool reload=true) {
		ostringstream *out = new ostringstream();

		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << endl;
		*out << "<html>" << endl;
		*out << cgicc::head() << endl;
		*out << CSS();
		*out << cgicc::title(myTitle) << endl;

		// Auto-refreshing
		// PGK use javascript:  it makes things easier.
		/*
		if (autoRefresh_ >= 0 && reload) {
			*out << cgicc::meta()
				.set("http-equiv","refresh")
				.set("content",autoRefresh_.toString() + ";/" + getApplicationDescriptor()->getURN() ) << endl;
		}
		*/

		// Auto-refreshing with javascript
		if (autoRefresh_ >= 0 && reload) {
			*out << "<script type='text/javascript'>" << endl;
			// The regex makes sure we don't constantly resubmit "GET" posts.
			*out << "setTimeout('window.location.href=window.location.href.replace(/[\\?].*$/,\"\");'," << (1000 * autoRefresh_) << ");" << endl;
			*out << "</script>" << endl;
		}

		// Javascript error flasher!
		*out << "<script type='text/javascript'>" << endl;
		*out << "var red = 1;" << endl;
		*out << "function getElementsByClassName(classname) {var node = document.getElementsByTagName(\"body\")[0];var a = [];var re = new RegExp('\\\\b' + classname + '\\\\b'); var els = node.getElementsByTagName(\"*\");for(var i=0,j=els.length; i<j; i++)if(re.test(els[i].className))a.push(els[i]);return a;}" << endl;
		*out << "function setcolor(){var x = getElementsByClassName('error');var c1;var c2; if (red){ red = 0; c1 = 'red'; c2 = 'black'; } else { red = 1; c1 = 'black'; c2 = 'red'; } for (i=0,j=x.length;i<j;i++) {x[i].style.backgroundColor = c1; x[i].style.color = c2;} setTimeout('setcolor();',400);}" << endl;
		*out << "setTimeout('setcolor();',400)" << endl;
		*out << "</script>" << endl;

		// Javascript table toggler
		*out << "<script type='text/javascript'>" << endl;
		*out << "function toggle(id) {var elem = document.getElementById(id);	elem.style.display = (elem.style.display != 'none' ? 'none' : '' );}" << endl;
		*out << "</script>" << endl;

		// Javascript bit-flipper
		*out << "<script type='text/javascript'>" << endl;
		*out << "function toggleBit(id,bit) { var elem = document.getElementById(id); oldValue = parseInt(elem.value); newValue = (oldValue ^ (1 << bit)); elem.value = \"0x\" + newValue.toString(16); }" << endl;
		*out << "</script>" << endl;

		*out << cgicc::head() << endl;

		*out << "<body background=\"/tmp/osu_fed_background2.png\">" << endl;

		*out << cgicc::fieldset()
			.set("class","header") << endl;

		*out << cgicc::img()
			.set("src","/tmp/osu_emu2.png")
			.set("style","float: left; width: 100px; height: 100px") << endl;

		*out << cgicc::img()
			.set("src","/tmp/osu_crate2.png")
			.set("style","float: right; width: 100px; height: 100px") << endl;

		*out << cgicc::div(myTitle)
			.set("class","title") << endl;

		*out << cgicc::div()
			.set("class","expert") << endl;
		*out << cgicc::span("Experts ")
			.set("style","font-weight: bold") << endl;
		*out << cgicc::a("Stan Durkin")
			.set("href","mailto:durkin@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Jason Gilmore")
			.set("href","mailto:gilmore@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Jianhui Gu")
			.set("href","mailto:gujh@mps.ohio-state.edu") << ", " << endl;
		*out << cgicc::a("Phillip Killewald")
			.set("href","mailto:paste@mps.ohio-state.edu") << endl;
		*out << cgicc::div() << endl;

		*out << cgicc::fieldset() << endl;

		*out << cgicc::br()
			.set("style","clear: both;") << endl;

		return out->str();
	}


	/** Returns the standard CSS code for the EmuFCrate applications.
	*
	*	@returns a huge string that is basically the CSS code in HTML.  Good
	*	 for outputting straight to the xgi::Output.
	*
	*	@sa the Header and Footer methods.
	**/
	virtual string CSS() {
		ostringstream *out = new ostringstream();

		*out << cgicc::style() << endl;
		*out << cgicc::comment() << endl;
		*out << "div.title {width: 80%; margin: 20px auto 20px auto; text-align: center; color: #000; font-size: 16pt; font-weight: bold;}" << endl;
		*out << "div.expert {width: 80%; margin: 2px auto 2px auto; text-align: center;}" << endl;
		*out << "fieldset.header {width: 95%; margin: 5px auto 5px auto; padding: 2px 2px 2px 2px; border: 2px solid #555; background-color: #FFF;}" << endl;
		*out << "fieldset.footer {width: 95%; margin: 20px auto 5px auto; padding: 2px 2px 2px 2px; font-size: 9pt; font-style: italic; border: 0px solid #555; text-align: center;}" << endl;
		*out << "fieldset.fieldset, fieldset.normal, fieldset.expert {width: 90%; margin: 10px auto 10px auto; padding: 2px 2px 2px 2px; border: 2px solid #555; background-color: #FFF;}" << endl;
		*out << "fieldset.expert {background-color: #CCC; border: dashed 2px #C00; clear: both;}" << endl;
		*out << "div.legend {width: 100%; padding-left: 20px; margin-bottom: 10px; color: #00D; font-size: 12pt; font-weight: bold;}" << endl;
		*out << ".openclose {border: 1px solid #000; padding: 0px; cursor: pointer; font-family: monospace; color: #000; background-color: #FFF;}" << endl;
		*out << "table.data {border-width: 0px; border-collapse: collapse; margin: 5px auto 5px auto; font-size: 9pt;} " << endl;
		*out << "table.data td {padding: 1px 8px 1px 8px;}" << endl;
		*out << ".Halted, .Enabled, .Disabled, .Configured, .Failed, .unknown {padding: 2px; background-color: #000; font-family: monospace;}" << endl;
		*out << ".Halted {color: #F99;}" << endl;
		*out << ".Enabled {color: #9F9;}" << endl;
		*out << ".Disabled {color: #FF9;}" << endl;
		*out << ".Configured {color: #99F;}" << endl;
		*out << ".Failed, .unknown {color: #F99; font-weight: bold; text-decoration: blink;}" << endl;
		*out << ".error {padding: 2px; background-color: #000; color: #F00; font-family: monospace;}" << endl;
		*out << ".warning {padding: 2px; background-color: #F60; color: #000; font-family: monospace;}" << endl;
		*out << ".orange {padding: 2px; color: #930; font-family: monospace;}" << endl;
		*out << ".caution {padding: 2px; background-color: #FF6; color: #000; font-family: monospace;}" << endl;
		*out << ".yellow {padding: 2px; color: #990; font-family: monospace;}" << endl;
		*out << ".ok {padding: 2px; background-color: #6F6; color: #000; font-family: monospace;}" << endl;
		*out << ".green {padding: 2px; color: #090; font-family: monospace;}" << endl;
		*out << ".bad {padding: 2px; background-color: #F66; color: #000; font-family: monospace;}" << endl;
		*out << ".red {padding: 2px; color: #900; font-family: monospace;}" << endl;
		*out << ".questionable {padding: 2px; background-color: #66F; color: #000; font-family: monospace;}" << endl;
		*out << ".blue {padding: 2px; color: #009; font-family: monospace;}" << endl;
		*out << ".none {padding: 2px; font-family: monospace;}" << endl;
		*out << ".undefined {padding: 2px; background-color: #CCC; color: #333; font-family: monospace;}" << endl;

		*out << ".button {padding: 2px; -moz-border-radius: 3px; -webkit-border-radius: 3px; border: 1px solid #000; cursor: pointer;}" << endl;
		//*out << "body {background-image: url('/tmp/osu_fed_background2.png'); background-repeat: repeat;}" << endl;
		*out << cgicc::comment() << endl;
		*out << cgicc::style() << endl;

		return out->str();
	}


	/** Returns the standard Footer for the EmuFCrate applications.
	*
	*	@returns a huge string that is basically the footer code in HTML.  Good
	*	 for outputting straight to the xgi::Output.
	*
	*	@sa the CSS method.
	**/
	virtual string Footer() {
		ostringstream *out = new ostringstream();

		*out << cgicc::fieldset()
			.set("class","footer") << endl;
		*out << "Built on " << __DATE__ << " at " << __TIME__ << "." << cgicc::br() << endl;
		*out << "Eddie the Emu thanks you." << endl;
		*out << cgicc::fieldset() << endl;

		*out << "</body>" << endl;
		*out << "</html>" << endl;

		return out->str();
	}

protected:

	// "Globals"
	string NS_XSI;
	string STATE_UNKNOWN;
	
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
