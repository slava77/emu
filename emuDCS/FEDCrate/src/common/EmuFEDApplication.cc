/*****************************************************************************\
* $Id: EmuFEDApplication.cc,v 1.5 2008/10/29 16:01:44 paste Exp $
*
* $Log: EmuFEDApplication.cc,v $
* Revision 1.5  2008/10/29 16:01:44  paste
* Updated interoperability with primative DCC commands, added new xdata variables for future use.
*
* Revision 1.4  2008/10/13 11:56:41  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 1.3  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 1.2  2008/10/04 18:44:06  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
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
#include "cgicc/HTMLDoctype.h"
#include "cgicc/CgiEnvironment.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"

EmuFEDApplication::EmuFEDApplication(xdaq::ApplicationStub *stub)
	throw (xdaq::exception::Exception):
	EmuApplication(stub),
	runNumber_(0),
	endcap_("?"),
	autoRefresh_(20),
	NS_XSI("http://www.w3.org/2001/XMLSchema-instance"),
	STATE_UNKNOWN("unknown")
{
	xoap::bind(this, &EmuFEDApplication::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// PGK Making these available on the ApplicationInfoSpace will allow
	//  the CSCSV to set them with the "ParameterSet" SOAP command.
	getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
	getApplicationInfoSpace()->fireItemAvailable("runType", &runType_);
	getApplicationInfoSpace()->fireItemAvailable("endcap", &endcap_);

	// PGK We deal with INFO level unless we are doing a Debug run.
	getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);

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



std::string EmuFEDApplication::Header(std::string myTitle) {
	std::vector<std::string> fileNames;
	return Header(myTitle, fileNames);
}



std::string EmuFEDApplication::Header(std::string myTitle, std::vector<std::string> jsFileNames) {
	std::stringstream *out = new std::stringstream();

	// *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	// This installation of CGICC is ancient and does not understand
	// the XHTML Doctype.  Make my own.
	*out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
	*out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
	*out << cgicc::head() << std::endl;
	*out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/emu/emuDCS/FEDCrate/css/EmuFEDApplication.css\" />" << std::endl;
	
	*out << cgicc::title(myTitle) << std::endl;


	// Include the javascript files
	for (std::vector<std::string>::iterator iFile = jsFileNames.begin(); iFile != jsFileNames.end(); iFile++) {
		*out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDCrate/js/" << (*iFile) << "\"></script>" << std::endl;
	}

	*out << cgicc::head() << std::endl;

	*out << "<body background=\"/emu/emuDCS/FEDCrate/img/OSUBackground.gif\">" << std::endl;

	*out << cgicc::fieldset()
		.set("class","header") << std::endl;

	*out << cgicc::a()
		.set("href","/"+getApplicationDescriptor()->getURN()+"/") << std::endl;

	*out << cgicc::img()
	.set("src","/emu/emuDCS/FEDCrate/img/EmuFEDSeal.png")
		.set("style","float: left; width: 100px; height: 100px") << std::endl;

	*out << cgicc::a() << std::endl;

	*out << cgicc::img()
	.set("src","/emu/emuDCS/FEDCrate/img/OSUCMS.png")
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



void EmuFEDApplication::dumpEnvironment(xgi::Input *in)
{
	std::ostringstream dump;
	cgicc::Cgicc cgi(in);
	const cgicc::CgiEnvironment& env = cgi.getEnvironment();

	dump << "Dumping CGI environment..." << std::endl;

	dump << "Request Method: " << env.getRequestMethod() << std::endl;
	dump << "Path Info: " << env.getPathInfo() << std::endl;
	dump << "Path Translated: " << env.getPathTranslated() << std::endl;
	dump << "Script Name: " << env.getScriptName() << std::endl;
	dump << "HTTP Referrer: " << env.getReferrer() << std::endl;
	dump << "HTTP Cookie: " << env.getCookies() << std::endl;
	dump << "Query String: " << env.getQueryString() << std::endl;
	dump << "Content Length: " << env.getContentLength() << std::endl;
	dump << "Post Data: " << env.getPostData() << std::endl;
	dump << "Remote Host: " << env.getRemoteHost() << std::endl;
	dump << "Remote Address: " << env.getRemoteAddr() << std::endl;
	dump << "Authorization Type: " << env.getAuthType() << std::endl;
	dump << "Remote User: " << env.getRemoteUser() << std::endl;
	dump << "Remote Identification: " << env.getRemoteIdent() << std::endl;
	dump << "Content Type: " << env.getContentType() << std::endl;
	dump << "HTTP Accept: " << env.getAccept() << std::endl;
	dump << "User Agent: " << env.getUserAgent() << std::endl;
	dump << "Server Software: " << env.getServerSoftware() << std::endl;
	dump << "Server Name: " << env.getServerName() << std::endl;
	dump << "Gateway Interface: " << env.getGatewayInterface() << std::endl;
	dump << "Server Protocol: " << env.getServerProtocol() << std::endl;
	dump << "Server Port: " << env.getServerPort() << std::endl;
	dump << "HTTPS: " << (env.usingHTTPS() ? "true" : "false") << std::endl;
	dump << "Redirect Request: " << env.getRedirectRequest() << std::endl;
	dump << "Redirect URL: " << env.getRedirectURL() << std::endl;
	dump << "Redirect Status: " << env.getRedirectStatus() << std::endl;
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), dump.str());
}
