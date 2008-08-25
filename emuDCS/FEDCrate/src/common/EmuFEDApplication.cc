/*****************************************************************************\
* $Id: EmuFEDApplication.cc,v 1.1 2008/08/25 13:33:31 paste Exp $
*
* $Log: EmuFEDApplication.cc,v $
* Revision 1.1  2008/08/25 13:33:31  paste
* Forgot this one...
*
*
\*****************************************************************************/
#include "EmuFEDApplication.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "cgicc/Cgicc.h"
#include "cgicc/HTMLClasses.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"

EmuFEDApplication::EmuFEDApplication(xdaq::ApplicationStub *stub)
	throw (xdaq::exception::Exception):
	EmuApplication(stub),
	runNumber_(0),
	autoRefresh_(20),
	NS_XSI("http://www.w3.org/2001/XMLSchema-instance"),
	STATE_UNKNOWN("unknown")
{
	xoap::bind(this, &EmuFEDApplication::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// PGK Making these available on the ApplicationInfoSpace will allow
	//  the CSCSV to set them with the "ParameterSet" SOAP command.
	getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
	getApplicationInfoSpace()->fireItemAvailable("runType", &runType_);

	// PGK We deal with INFO level unless we are doing a Debug run.
	getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);

	// Move the pictures to tmp for display
	// FIXME with something that will work with RPMs.
	std::vector< std::string > picNames;
	picNames.push_back("OSUBackground.gif");
	picNames.push_back("OSUCMS.png");
	picNames.push_back("EmuFEDSeal.png");
	for (std::vector< std::string >::iterator iName = picNames.begin(); iName != picNames.end(); iName++) {
		std::ifstream picIn;
		picIn.open((*iName).c_str(),std::ios_base::binary);
		if (picIn.is_open()) {
			std::ofstream picOut;
			std::ifstream picOutTest;
			std::string newName = "/tmp/" + *iName;
			// Check if file is already there...
			picOutTest.open(newName.c_str(),std::ios_base::binary | std::ios::in);
			if (!picOutTest.is_open()) {
				picOut.open(newName.c_str(),std::ios_base::binary);
				if (picOut.is_open()) {
					picOut << picIn.rdbuf();
					picOut.close();
				}
			} else {
				picOutTest.close();
			}
			picIn.close();
		}
	}
}



xoap::MessageReference EmuFEDApplication::getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
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



xoap::MessageReference EmuFEDApplication::getParameters(std::string applicationName, unsigned int instance)
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



void EmuFEDApplication::setParameter(std::string klass, std::string name, std::string type, std::string value, int instance)
	throw (xdaq::exception::Exception)
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
		if (instance < 0 || (int) (*i)->getInstance() == instance) {
			reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), **i);
			// Analysis here, if debugging needed.
			if (instance >= 0) break;
		}
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter successfully returning");

	return;
}



std::string EmuFEDApplication::Header(std::string myTitle,bool reload) {
	std::stringstream *out = new std::stringstream();

	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << "<html>" << std::endl;
	*out << cgicc::head() << std::endl;
	*out << CSS();
	*out << cgicc::title(myTitle) << std::endl;

	// Auto-refreshing
	// PGK use javascript:  it makes things easier.
	/*
	if (autoRefresh_ >= 0 && reload) {
		*out << cgicc::meta()
			.set("http-equiv","refresh")
			.set("content",autoRefresh_.toString() + ";/" + getApplicationDescriptor()->getURN() ) << std::endl;
	}
	*/

	// Auto-refreshing with javascript
	if (autoRefresh_ >= 0 && reload) {
		*out << "<script type='text/javascript'>" << std::endl;
		// The regex makes sure we don't constantly resubmit "GET" posts.
		*out << "setTimeout('window.location.href=window.location.href.replace(/[\\?].*$/,\"\");'," << (1000 * autoRefresh_) << ");" << std::endl;
		*out << "</script>" << std::endl;
	}

	// Javascript error flasher!
	*out << "<script type='text/javascript'>" << std::endl;
	*out << "var red = 1;" << std::endl;
	*out << "function getElementsByClassName(classname) {var node = document.getElementsByTagName(\"body\")[0];var a = [];var re = new RegExp('\\\\b' + classname + '\\\\b'); var els = node.getElementsByTagName(\"*\");for(var i=0,j=els.length; i<j; i++)if(re.test(els[i].className))a.push(els[i]);return a;}" << std::endl;
	*out << "function setcolor(){var x = getElementsByClassName('error');var c1;var c2; if (red){ red = 0; c1 = 'red'; c2 = 'black'; } else { red = 1; c1 = 'black'; c2 = 'red'; } for (i=0,j=x.length;i<j;i++) {x[i].style.backgroundColor = c1; x[i].style.color = c2;} setTimeout('setcolor();',400);}" << std::endl;
	*out << "setTimeout('setcolor();',400)" << std::endl;
	*out << "</script>" << std::endl;

	// Javascript table toggler
	*out << "<script type='text/javascript'>" << std::endl;
	*out << "function toggle(id) {var elem = document.getElementById(id);	elem.style.display = (elem.style.display != 'none' ? 'none' : '' );}" << std::endl;
	*out << "</script>" << std::endl;

	// Javascript bit-flipper
	*out << "<script type='text/javascript'>" << std::endl;
	*out << "function toggleBit(id,bit) { var elem = document.getElementById(id); oldValue = parseInt(elem.value); newValue = (oldValue ^ (1 << bit)); elem.value = \"0x\" + newValue.toString(16); }" << std::endl;
	*out << "</script>" << std::endl;

	// Javascript bit-setter
	*out << "<script type='text/javascript'>" << std::endl;
	*out << "function setBit(id,bit) { var elem = document.getElementById(id); oldValue = parseInt(elem.value); newValue = (oldValue | (1 << bit)); elem.value = \"0x\" + newValue.toString(16); }" << std::endl;
	*out << "</script>" << std::endl;

	// Javascript bit-clearer
	*out << "<script type='text/javascript'>" << std::endl;
	*out << "function clearBit(id,bit) { var elem = document.getElementById(id); oldValue = parseInt(elem.value); newValue = (oldValue & ~(1 << bit)); elem.value = \"0x\" + newValue.toString(16); }" << std::endl;
	*out << "</script>" << std::endl;

	*out << cgicc::head() << std::endl;

	*out << "<body background=\"/tmp/OSUBackground.gif\">" << std::endl;

	*out << cgicc::fieldset()
		.set("class","header") << std::endl;

	*out << cgicc::a()
		.set("href","/"+getApplicationDescriptor()->getURN()+"/") << std::endl;

	*out << cgicc::img()
		.set("src","/tmp/EmuFEDSeal.png")
		.set("style","float: left; width: 100px; height: 100px") << std::endl;

	*out << cgicc::a() << std::endl;

	*out << cgicc::img()
		.set("src","/tmp/OSUCMS.png")
		.set("style","float: right; width: 100px; height: 100px") << std::endl;

	*out << cgicc::div(myTitle)
		.set("class","title") << std::endl;

	*out << cgicc::div()
		.set("class","expert") << std::endl;
	*out << cgicc::span("Experts ")
		.set("style","font-weight: bold") << std::endl;
	*out << cgicc::a("Stan Durkin")
		.set("href","mailto:durkin@mps.ohio-state.edu") << ", " << std::endl;
	*out << cgicc::a("Jason Gilmore")
		.set("href","mailto:gilmore@mps.ohio-state.edu") << ", " << std::endl;
	*out << cgicc::a("Jianhui Gu")
		.set("href","mailto:gujh@mps.ohio-state.edu") << ", " << std::endl;
	*out << cgicc::a("Phillip Killewald")
		.set("href","mailto:paste@mps.ohio-state.edu") << std::endl;
	*out << cgicc::div() << std::endl;

	*out << cgicc::fieldset() << std::endl;

	*out << cgicc::br()
		.set("style","clear: both;") << std::endl;

	return out->str();
}



std::string EmuFEDApplication::CSS() {
	std::stringstream *out = new std::stringstream();

	*out << cgicc::style() << std::endl;
	*out << cgicc::comment() << std::endl;
	*out << "div.title {width: 80%; margin: 20px auto 20px auto; text-align: center; color: #000; font-size: 16pt; font-weight: bold;}" << std::endl;
	*out << "div.expert {width: 80%; margin: 2px auto 2px auto; text-align: center;}" << std::endl;
	*out << "fieldset.header {width: 95%; margin: 5px auto 5px auto; padding: 2px 2px 2px 2px; border: 2px solid #555; background-color: #FFF;}" << std::endl;
	*out << "fieldset.footer {width: 95%; margin: 20px auto 5px auto; padding: 2px 2px 2px 2px; font-size: 9pt; font-style: italic; border: 0px solid #555; text-align: center;}" << std::endl;
	*out << "fieldset.fieldset, fieldset.normal, fieldset.expert {width: 90%; margin: 10px auto 10px auto; padding: 2px 2px 2px 2px; border: 2px solid #555; background-color: #FFF;}" << std::endl;
	*out << "fieldset.expert {background-color: #CCC; border: dashed 2px #C00; clear: both;}" << std::endl;
	*out << "div.legend {width: 100%; padding-left: 20px; margin-bottom: 10px; color: #00D; font-size: 12pt; font-weight: bold;}" << std::endl;
	*out << ".openclose {border: 1px solid #000; padding: 0px; cursor: pointer; font-family: monospace; color: #000; background-color: #FFF;}" << std::endl;
	*out << "table.data {border-width: 0px; border-collapse: collapse; margin: 5px auto 5px auto; font-size: 9pt;} " << std::endl;
	*out << "table.data td {padding: 1px 8px 1px 8px;}" << std::endl;
	*out << ".Halted, .Enabled, .Disabled, .Configured, .Failed, .unknown {padding: 2px; background-color: #000; font-family: monospace;}" << std::endl;
	*out << ".Halted {color: #F99;}" << std::endl;
	*out << ".Enabled {color: #9F9;}" << std::endl;
	*out << ".Disabled {color: #FF9;}" << std::endl;
	*out << ".Configured {color: #99F;}" << std::endl;
	*out << ".Failed, .unknown {color: #F99; font-weight: bold; text-decoration: blink;}" << std::endl;
	*out << ".error {padding: 2px; background-color: #000; color: #F00; font-family: monospace;}" << std::endl;
	*out << ".warning {padding: 2px; background-color: #F60; color: #000; font-family: monospace;}" << std::endl;
	*out << ".orange {padding: 2px; color: #930; font-family: monospace;}" << std::endl;
	*out << ".caution {padding: 2px; background-color: #FF6; color: #000; font-family: monospace;}" << std::endl;
	*out << ".yellow {padding: 2px; color: #990; font-family: monospace;}" << std::endl;
	*out << ".ok {padding: 2px; background-color: #6F6; color: #000; font-family: monospace;}" << std::endl;
	*out << ".green {padding: 2px; color: #090; font-family: monospace;}" << std::endl;
	*out << ".bad {padding: 2px; background-color: #F66; color: #000; font-family: monospace;}" << std::endl;
	*out << ".red {padding: 2px; color: #900; font-family: monospace;}" << std::endl;
	*out << ".questionable {padding: 2px; background-color: #66F; color: #000; font-family: monospace;}" << std::endl;
	*out << ".blue {padding: 2px; color: #009; font-family: monospace;}" << std::endl;
	*out << ".none {padding: 2px; font-family: monospace;}" << std::endl;
	*out << ".undefined {padding: 2px; background-color: #CCC; color: #333; font-family: monospace;}" << std::endl;

	*out << ".button {padding: 2px; -moz-border-radius: 3px; -webkit-border-radius: 3px; border: 1px solid #000; cursor: pointer;}" << std::endl;
	//*out << "body {background-image: url('/tmp/osu_fed_background2.png'); background-repeat: repeat;}" << std::endl;
	*out << cgicc::comment() << std::endl;
	*out << cgicc::style() << std::endl;

	return out->str();
}



std::string EmuFEDApplication::Footer() {
	std::stringstream *out = new std::stringstream();

	*out << cgicc::fieldset()
		.set("class","footer") << std::endl;
	*out << "Built on " << __DATE__ << " at " << __TIME__ << "." << cgicc::br() << std::endl;
	*out << "Eddie the Emu thanks you." << std::endl;
	*out << cgicc::fieldset() << std::endl;

	*out << "</body>" << std::endl;
	*out << "</html>" << std::endl;

	return out->str();
}



xoap::MessageReference EmuFEDApplication::onGetParameters(xoap::MessageReference message)
	throw (xoap::exception::Exception)
{
	xdata::soap::Serializer serializer;

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

	//bool first = true;

	std::map<std::string, xdata::Serializable *>::iterator iParam;
	for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
		std::string name = (*iParam).first;
		xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
		xoap::SOAPElement element = bodyElement.addChildElement(elementName);
		serializer.exportAll((*iParam).second, dynamic_cast<DOMElement *>(element.getDOMNode()), true);
		//first = false;
	}

	//reply->writeTo(std::cout);
	//std::cout << std::endl;
	return reply;

}
