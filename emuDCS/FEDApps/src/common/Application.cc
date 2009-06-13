/*****************************************************************************\
* $Id: Application.cc,v 1.6 2009/06/13 17:59:08 paste Exp $
\*****************************************************************************/
#include "emu/fed/Application.h"

#include <sstream>
#include <fstream>

#include "cgicc/HTMLClasses.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "log4cplus/logger.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/configurator.h"
#include "emu/fed/DataTable.h"

emu::fed::Application::Application(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
systemName_("unnamed")
{

	getApplicationInfoSpace()->fireItemAvailable("systemName", &systemName_);

	xoap::bind(this, &emu::fed::Application::onGetParameters, "GetParameters", XDAQ_NS_URI);

	getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
	
	// Appender file name
	char datebuf[32];
	char filebuf[255];
	std::time_t theTime = time(NULL);
	std::strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H-%M-%S", localtime(&theTime));
	std::string fileName = getApplicationDescriptor()->getClassName() + "-%s.log";
	// Get rid of the emu::fed:: crap
	if (fileName.substr(0,10) == "emu::fed::") fileName = "emu-" + fileName.substr(10);
	//fileName = "testing" + fileName.substr(10);
	std::sprintf(filebuf, fileName.c_str(), datebuf);
	log4cplus::SharedAppenderPtr myAppender = new log4cplus::FileAppender(filebuf);
	myAppender->setName(getApplicationDescriptor()->getClassName() + "Appender");
	
	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	myAppender->setLayout( myLayout );
	getApplicationLogger().addAppender(myAppender);
}



xoap::MessageReference emu::fed::Application::getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
throw (emu::fed::exception::SOAPException)
{

	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName command = envelope.createName("GetParameters", "xdaq", XDAQ_NS_URI);
	body.addBodyElement(command);

	try {
		return getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *applicationDescriptor);
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		std::string temp;
		message->writeTo(temp);
		error << "Error in posting SOAP message with contents: " << temp;
		XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
	}

}



xoap::MessageReference emu::fed::Application::getParameters(const std::string &applicationName, const unsigned int instance)
throw (emu::fed::exception::SOAPException)
{

	xoap::MessageReference reply;

	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(applicationName);
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Application name '" << applicationName << "' not found";
		XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
	}

	std::set<xdaq::ApplicationDescriptor *>::const_iterator iAD;
	for (iAD = apps.begin(); iAD != apps.end(); iAD++) {
		if ((*iAD)->getInstance() != instance) continue;
		return getParameters(*iAD);
	}

	std::ostringstream error;
	error << "Application name '" << applicationName << "' instance " << instance << " not found";
	XCEPT_RAISE(emu::fed::exception::SOAPException, error.str());

}



void emu::fed::Application::setParameter(const std::string &klass, const std::string &name, const std::string &type, const std::string &value, const int instance)
throw (emu::fed::exception::SOAPException)
{

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Application class name '" << klass << "' not found";
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
	}

	//LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter " << klass << " " << name << " " << type << " " << value);

	// prepare a SOAP message
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", XSI_NAMESPACE_URI);

	xoap::SOAPName command = envelope.createName("ParameterSet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName("properties", klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(name, klass, "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", XSI_NAMESPACE_URI);

	xoap::SOAPElement properties_e = envelope.getBody()
		.addBodyElement(command)
		.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode(value);

	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator iAD;
	for (iAD = apps.begin(); iAD != apps.end(); iAD++) {
		if (instance < 0 || (int) (*iAD)->getInstance() == instance) {
			try {
				reply = getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), **iAD);
			} catch (xdaq::exception::Exception &e) {
				std::ostringstream error;
				std::string temp;
				message->writeTo(temp);
				error << "Error in posting SOAP message with contents: " << temp;
				LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
				XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
			}
			// Analysis here, if debugging needed.
			if (instance >= 0) break;
		}
	}

	//LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter successfully returning");

	return;
}



std::string emu::fed::Application::Header(const std::string &myTitle, const std::vector<std::string> &jsFileNames)
{
	std::stringstream out;

	// out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	// This installation of CGICC is ancient and does not understand
	// the XHTML Doctype.  Make my own.
	out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
	out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
	out << cgicc::head() << std::endl;
	out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/emu/emuDCS/FEDApps/html/FEDApps.css\" />" << std::endl;
	
	out << cgicc::title(myTitle) << std::endl;


	// Include the javascript files
	// This is a universal global that I want to always have around.
	out << "<script type=\"text/javascript\">var URL = \"" << getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << getApplicationDescriptor()->getURN() << "\";</script>";
	// Always include prototype
	out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/prototype.js\"></script>" << std::endl;
	for (std::vector<std::string>::const_iterator iFile = jsFileNames.begin(); iFile != jsFileNames.end(); iFile++) {
		out << "<script type=\"text/javascript\" src=\"/emu/emuDCS/FEDApps/html/" << (*iFile) << "\"></script>" << std::endl;
	}

	out << cgicc::head() << std::endl;

	// Dynamic backgrounds
	out << "<body style=\"background-image: url(/emu/emuDCS/FEDApps/images/Background-" + systemName_.toString() + ".png);\">" << std::endl;

	out << cgicc::fieldset()
		.set("class","header") << std::endl;

	out << cgicc::a()
		.set("href","/"+getApplicationDescriptor()->getURN()+"/") << std::endl;

	out << cgicc::img()
	.set("src","/emu/emuDCS/FEDApps/images/EmuFEDSeal.png")
		.set("style","float: left; width: 100px; height: 100px") << std::endl;

	out << cgicc::a() << std::endl;

	out << cgicc::img()
	.set("src","/emu/emuDCS/FEDApps/images/OSUCMS.png")
		.set("style","float: right; width: 100px; height: 100px") << std::endl;

	out << cgicc::div(myTitle)
		.set("class","title")
		.set("id", "application_title") << std::endl;

	out << cgicc::div()
		.set("class","expert_names") << std::endl;
	out << cgicc::span("Experts ")
		.set("style","font-weight: bold") << std::endl;
	out << cgicc::a("Stan Durkin")
		.set("href","mailto:durkin@mps.ohio-state.edu") << ", " << std::endl;
	out << cgicc::a("Jason Gilmore")
		.set("href","mailto:gilmore@mps.ohio-state.edu") << ", " << std::endl;
	out << cgicc::a("Jianhui Gu")
		.set("href","mailto:gujh@mps.ohio-state.edu") << ", " << std::endl;
	out << cgicc::a("Phillip Killewald")
		.set("href","mailto:paste@mps.ohio-state.edu") << std::endl;
	out << cgicc::div() << std::endl;

	out << cgicc::fieldset() << std::endl;

	out << cgicc::br()
		.set("style","clear: both;") << std::endl;

	return out.str();
}



std::string emu::fed::Application::Footer()
{
	std::stringstream out;

	out << cgicc::fieldset()
		.set("class","footer") << std::endl;
	out << "Built on " << __DATE__ << " at " << __TIME__ << "." << cgicc::br() << std::endl;
	out << "Eddie the Emu thanks you." << std::endl;
	out << cgicc::fieldset() << std::endl;

	out << "</body>" << std::endl;
	out << "</html>" << std::endl;

	return out.str();
}



xoap::MessageReference emu::fed::Application::onGetParameters(xoap::MessageReference message)
{
	xdata::soap::Serializer serializer;

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

	std::map<std::string, xdata::Serializable *>::iterator iParam;
	try {
		for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
			std::string name = (*iParam).first;
			xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
			xoap::SOAPElement element = bodyElement.addChildElement(elementName);
			serializer.exportAll((*iParam).second, dynamic_cast<DOMElement *>(element.getDOMNode()), true);
		}
	} catch (xcept::Exception &e) {
		std::ostringstream error;
		error << "Exception in onGetParamters";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::exception::SOAPException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
	}

	return reply;
}



std::string emu::fed::Application::dumpEnvironment(xgi::Input *in)
{
	std::ostringstream dump;
	cgicc::Cgicc cgi(in);
	const cgicc::CgiEnvironment &env = cgi.getEnvironment();

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
	
	return dump.str();
}


/*
xoap::MessageReference emu::fed::Application::createSOAPReply(xoap::MessageReference message)
{
	std::string command = "";
	DOMNodeList *elements = message->getSOAPPart().getEnvelope().getBody().getDOMNode()->getChildNodes();

	for (unsigned int i = 0; i < elements->getLength(); i++) {
		DOMNode *e = elements->item(i);
		if (e->getNodeType() == DOMNode::ELEMENT_NODE) {
			command = xoap::XMLCh2String(e->getLocalName());
			break;
		}
	}

	if (command == "") {
		std::ostringstream error;
		std::string temp;
		message->writeTo(temp);
		error <<  "Cannot create reply for SOAP message: " << temp;
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::exception::FSMException, e, error.str());
		notifyQualified("ERROR", e);
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

	return reply;
}
*/


void emu::fed::Application::webRedirect(xgi::Input *in, xgi::Output *out, const std::string &location)
{
	std::string url;
	if (location == "") {
		url = in->getenv("PATH_TRANSLATED");
		url = url.substr(0, url.find("/" + in->getenv("PATH_INFO")));
	} else {
		url = "/" + getApplicationDescriptor()->getURN() + "/" + location;
	}

	cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();

	header.getStatusCode(303);
	header.getReasonPhrase("See Other");
	header.addHeader("Location", url);
}



xoap::MessageReference emu::fed::Application::createSOAPCommand(const std::string &command)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(name);
	
	return message;
}



void emu::fed::Application::sendSOAPCommand(const std::string &command, const std::string &klass, const int instance)
throw (emu::fed::exception::SOAPException)
{

	// find applications
	LOG4CPLUS_DEBUG(getApplicationLogger(), "sendCommand with command=" << command << ", klass=" << klass << ", instance=" << instance);
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		if (instance == -1) {
			apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
		} else {
			apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
		}
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Found no applications matching klass=" << klass << ", instance=" << instance;
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
	}

	// prepare a SOAP message
	xoap::MessageReference message = createSOAPCommand(command);

	// send the message
	// postSOAP() may throw an exception when failed.
	for (std::set<xdaq::ApplicationDescriptor *>::iterator iApp = apps.begin(); iApp != apps.end(); iApp++) {
		for (unsigned int iTries = 5; iTries > 0; iTries--) {
			try {
				getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *(*iApp));
				return;
			} catch (xcept::Exception &e) {
				std::ostringstream error;
				std::string messageOut;
				message->writeTo(messageOut);
				error << "sendCommand failed sending command=" << command << " to klass=" << klass << ", instance=" << instance << ": message " << messageOut;
				LOG4CPLUS_WARN(getApplicationLogger(), error.str());
			}
		}

		std::ostringstream error;
		error << "sendCommand reached the maximum number of retries";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_RAISE(emu::fed::exception::SOAPException, error.str());
	}

}



std::string emu::fed::Application::printException(xcept::Exception &myException)
{
	std::ostringstream out;
	
	std::vector<xcept::ExceptionInformation> history = myException.getHistory();
	for (std::vector<xcept::ExceptionInformation>::iterator iError = history.begin(); iError != history.end(); iError++) {
		DataTable exceptionTable;
		unsigned int iRow = 0;
		std::map<std::string, std::string> messages = iError->getProperties();
		for (std::map<std::string, std::string>::iterator iMessage = messages.begin(); iMessage != messages.end(); iMessage++) {
			exceptionTable(iRow, 0) << iMessage->first << ":";
			exceptionTable(iRow, 1) << iMessage->second;
			iRow++;
		}
		out << cgicc::div(exceptionTable.toHTML())
			.set("class", "exception");
	}
	
	return out.str();
}

