/*****************************************************************************\
* $Id: Application.cc,v 3.2 2009/01/30 19:14:16 paste Exp $
*
* $Log: Application.cc,v $
* Revision 3.2  2009/01/30 19:14:16  paste
* New emu::base namespace and emu::base::Supervised inheritance added.
*
* Revision 3.1  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.7  2008/11/15 13:59:15  paste
* Added initial support for AJAX communication.
*
* Revision 1.6  2008/11/03 23:33:47  paste
* Modifications to fix "missing stylesheet/javascript" problem.
*
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
#include "Application.h"

#include <sstream>
#include <fstream>

#include "cgicc/HTMLClasses.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xgi/Method.h"
#include "toolbox/fsm/FailedEvent.h"
#include "log4cplus/logger.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/configurator.h"

emu::fed::Application::Application(xdaq::ApplicationStub *stub):
emu::base::Supervised(stub),
runNumber_(0),
endcap_("?"),
soapLocal_(false)
{
	getApplicationInfoSpace()->fireItemAvailable("endcap", &endcap_);

	xoap::bind(this, &emu::fed::Application::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// PGK Making these available on the ApplicationInfoSpace will allow
	//  the CSCSV to set them with the "ParameterSet" SOAP command.
	getApplicationInfoSpace()->fireItemAvailable("runNumber", &runNumber_);
	
	getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);

	xgi::bind(this,&emu::fed::Application::webFire, "Fire");
	
	// Appender file name
	char datebuf[19];
	char filebuf[255];
	std::time_t theTime = time(NULL);
	std::strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H:%M:%S", localtime(&theTime));
	std::string fileName = getApplicationDescriptor()->getClassName() + "-%s.log";
	std::sprintf(filebuf, fileName.c_str(), datebuf);
	log4cplus::SharedAppenderPtr myAppender = new log4cplus::FileAppender(filebuf);
	myAppender->setName(getApplicationDescriptor()->getClassName() + "Appender");
	
	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	myAppender->setLayout( myLayout );
	getApplicationLogger().addAppender(myAppender);
}



xoap::MessageReference emu::fed::Application::getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
throw (emu::fed::SOAPException)
{

	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName command = envelope.createName("GetParameters", "xdaq", XDAQ_NS_URI);
	body.addBodyElement(command);

	try {
		return getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *applicationDescriptor);
	} catch (xdaq::exception::Exception &e) {
		std::ostringstream error;
		std::string temp;
		message->writeTo(temp);
		error << "Error in posting SOAP message with contents: " << temp;
		XCEPT_RETHROW(emu::fed::SOAPException, error.str(), e);
	}

}



xoap::MessageReference emu::fed::Application::getParameters(std::string applicationName, unsigned int instance)
throw (emu::fed::SOAPException)
{

	xoap::MessageReference reply;

	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(applicationName);
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Application name '" << applicationName << "' not found";
		XCEPT_RETHROW(emu::fed::SOAPException, error.str(), e);
	}

	std::set<xdaq::ApplicationDescriptor *>::iterator iAD;
	for (iAD = apps.begin(); iAD != apps.end(); iAD++) {
		if ((*iAD)->getInstance() != instance) continue;
		return getParameters(*iAD);
	}

	std::ostringstream error;
	error << "Application name '" << applicationName << "' instance " << instance << " not found";
	XCEPT_RAISE(emu::fed::SOAPException, error.str());

}



void emu::fed::Application::setParameter(std::string klass, std::string name, std::string type, std::string value, int instance)
throw (emu::fed::SOAPException)
{

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Application class name '" << klass << "' not found";
		XCEPT_RETHROW(emu::fed::SOAPException, error.str(), e);
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
				XCEPT_RETHROW(emu::fed::SOAPException, error.str(), e);
			}
			// Analysis here, if debugging needed.
			if (instance >= 0) break;
		}
	}

	//LOG4CPLUS_DEBUG(getApplicationLogger(), "setParameter successfully returning");

	return;
}



std::string emu::fed::Application::Header(std::string myTitle, std::vector<std::string> jsFileNames)
{
	std::stringstream *out = new std::stringstream();

	// *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	// This installation of CGICC is ancient and does not understand
	// the XHTML Doctype.  Make my own.
	*out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << std::endl;
	*out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << std::endl;
	*out << cgicc::head() << std::endl;
	*out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/tmp/emu/emuDCS/FEDCrate/css/Application.css\" />" << std::endl;
	
	*out << cgicc::title(myTitle) << std::endl;


	// Include the javascript files
    // Always include prototype
    *out << "<script type=\"text/javascript\" src=\"/tmp/emu/emuDCS/FEDCrate/js/prototype.js\"></script>" << std::endl;
	for (std::vector<std::string>::iterator iFile = jsFileNames.begin(); iFile != jsFileNames.end(); iFile++) {
		*out << "<script type=\"text/javascript\" src=\"/tmp/emu/emuDCS/FEDCrate/js/" << (*iFile) << "\"></script>" << std::endl;
	}

	*out << cgicc::head() << std::endl;

	// Dynamic backgrounds
	std::string endcapBackground = "/tmp/emu/emuDCS/FEDCrate/img/Background-" + endcap_.toString() + ".png";
	std::ifstream checkBackground(endcapBackground.c_str());
	if (!checkBackground) {
		endcapBackground = "/tmp/emu/emuDCS/FEDCrate/img/Background-Default.png";
	} else {
		checkBackground.close();
	}
	*out << "<body background=\"" << endcapBackground << "\">" << std::endl;

	*out << cgicc::fieldset()
		.set("class","header") << std::endl;

	*out << cgicc::a()
		.set("href","/"+getApplicationDescriptor()->getURN()+"/") << std::endl;

	*out << cgicc::img()
	.set("src","/tmp/emu/emuDCS/FEDCrate/img/EmuFEDSeal.png")
		.set("style","float: left; width: 100px; height: 100px") << std::endl;

	*out << cgicc::a() << std::endl;

	*out << cgicc::img()
	.set("src","/tmp/emu/emuDCS/FEDCrate/img/OSUCMS.png")
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



std::string emu::fed::Application::Footer()
{
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
		XCEPT_DECLARE_NESTED(emu::fed::SOAPException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
	}

	return reply;
}



std::string emu::fed::Application::dumpEnvironment(xgi::Input *in)
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
	
	return dump.str();
}


/*
void emu::fed::Application::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
{
	state_ = fsm.getStateName(fsm.getCurrentState());
	LOG4CPLUS_DEBUG(getApplicationLogger(), "StateChanged: " << state_.toString());
}
*/

/*
void emu::fed::Application::transitionFailed(toolbox::Event::Reference event)
{
	toolbox::fsm::FailedEvent &failed = dynamic_cast<toolbox::fsm::FailedEvent &>(*event);

	std::ostringstream error;
	error <<  "Failure occurred when performing transition from " << failed.getFromState() << " to " << failed.getToState() << ", exception: " << failed.getException().what();
	LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
	XCEPT_DECLARE_NESTED(emu::fed::FSMException, e, error.str(), failed.getException());
	notifyQualified("FATAL", e);
}
*/

/*
void emu::fed::Application::fireEvent(std::string name)
{
	toolbox::Event::Reference event((new toolbox::Event(name, this)));
	try {
		fsm_.fireEvent(event);
	} catch (toolbox::fsm::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception caught firing event named " << name << " to application " << getApplicationDescriptor()->getClassName() << " instance " << getApplicationDescriptor()->getInstance();
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::FSMException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
	}
}
*/

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
		XCEPT_DECLARE(emu::fed::FSMException, e, error.str());
		notifyQualified("ERROR", e);
	}

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName(command + "Response", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(responseName);

	return reply;
}
*/


void emu::fed::Application::webFire(xgi::Input *in, xgi::Output *out)
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
		} catch (emu::fed::FSMException &e) {
			std::ostringstream error;
			error << "Error in FSM state change from web with action " << action;
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::FSMException, e2, error.str(), e);
			notifyQualified("FATAL", e2);
		}
	}

	webRedirect(in, out);
}



void emu::fed::Application::webRedirect(xgi::Input *in, xgi::Output *out, std::string location)
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



xoap::MessageReference emu::fed::Application::createSOAPCommand(std::string command)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(name);
	
	return message;
}



void emu::fed::Application::sendSOAPCommand(std::string command, std::string klass, int instance)
throw (emu::fed::SOAPException)
{

	// find applications
	LOG4CPLUS_DEBUG(getApplicationLogger(), "sendCommand with command=" << command << ", klass=" << klass << ", instance=" << instance);
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		if (instance == -1) {
			apps.insert(getApplicationContext()->getDefaultZone()->getApplicationDescriptor(klass, instance));
		} else {
			apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
		}
	} catch (xdaq::exception::ApplicationDescriptorNotFound &e) {
		std::ostringstream error;
		error << "Found no applications matching klass=" << klass << ", instance=" << instance;
		LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		XCEPT_RETHROW(emu::fed::SOAPException, error.str(), e);
	}

	// prepare a SOAP message
	xoap::MessageReference message = createSOAPCommand(command);

	// send the message
	// postSOAP() may throw an exception when failed.
	for (std::set<xdaq::ApplicationDescriptor *>::iterator iApp = apps.begin(); iApp != apps.end(); iApp++) {
		unsigned int iTries = 5;
		while (iTries > 0) {
			try {
				getApplicationContext()->postSOAP(message, *getApplicationDescriptor(), *(*iApp));
				break;
			} catch (xcept::Exception &e) {
				std::ostringstream error;
				std::string messageOut;
				message->writeTo(messageOut);
				error << "sendCommand failed sending command=" << command << " to klass=" << klass << ", instance=" << instance << ": message " << messageOut;
				LOG4CPLUS_WARN(getApplicationLogger(), error.str());
				iTries--;
			}
		}

		if (iTries == 0) {
			std::ostringstream error;
			error << "sendCommand reached the maximum number of retries";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			XCEPT_RAISE(emu::fed::SOAPException, error.str());
		}
	}

}
