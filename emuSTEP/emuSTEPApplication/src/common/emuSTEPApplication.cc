// base object for STEP classes
// inherited from EmuApplication, but contains convenience methods borrowed from CSCSupervisor

#include <iostream>
#include <iomanip>

#include "emuSTEPApplication.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"
#include "xoap/domutils.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"

#include "xcept/tools.h"


using namespace cgicc;

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";

emuSTEPApplication::emuSTEPApplication(xdaq::ApplicationStub * s)
  throw (xdaq::exception::Exception): xdaq::Application(s)
{
}


void emuSTEPApplication::setParameter(
        string klass, string name, string type, string value)
{
    // find applications
    std::set<xdaq::ApplicationDescriptor *> apps;
    try 
	{
        apps = getApplicationContext()->getDefaultZone()
			->getApplicationDescriptors(klass);
    } 
	catch (xdaq::exception::ApplicationDescriptorNotFound e) 
	{
        return; // Do nothing if the target doesn't exist
    }

    // prepare a SOAP message
    xoap::MessageReference message = createParameterSetSOAP(
		klass, name, type, value);
    xoap::MessageReference reply;

    // send the message one-by-one
    std::set<xdaq::ApplicationDescriptor *>::iterator i;// = apps.begin();
    for (i= apps.begin(); i != apps.end(); ++i) 
	{
		LOG4CPLUS_INFO(getApplicationLogger(), 
					   "SetParameter for " << (*i)->getClassName() << ":" << 
					   (*i)->getLocalId() << " name: " << name << " value: " << value);	

		
        reply = getApplicationContext()->postSOAP(message, *(this->getApplicationDescriptor()), *(*i));
//        analyzeReply(message, reply, *i);
    }
}


void emuSTEPApplication::sendCommand(string command, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception)
{
	// Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.

	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try 
	{
		apps = getApplicationContext()->getDefaultZone()
			->getApplicationDescriptors(klass);
	} 
	catch (xdaq::exception::ApplicationDescriptorNotFound e) 
	{
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createCommandSOAP(command);
	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) 
	{
		LOG4CPLUS_INFO(getApplicationLogger(), 
					   "sendCommand " << command << " to " << (*i)->getClassName() << ":" << 
					   (*i)->getLocalId() );	
		// postSOAP() may throw an exception when failed.
		reply = getApplicationContext()->postSOAP(message, *(this->getApplicationDescriptor()), **i);

		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference emuSTEPApplication::createParameterSetSOAP(
        string klass, string name, string type, string value)
{
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration("xsi", NS_XSI);

    xoap::SOAPName command = envelope.createName(
            "ParameterSet", "xdaq", XDAQ_NS_URI);
    xoap::SOAPName properties = envelope.createName(
            "properties", klass, "urn:xdaq-application:" + klass);
    xoap::SOAPName parameter = envelope.createName(
            name, klass, "urn:xdaq-application:" + klass);
    xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

    xoap::SOAPElement properties_e = envelope.getBody()
            .addBodyElement(command)
            .addChildElement(properties);
    properties_e.addAttribute(xsitype, "soapenc:Struct");

    xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
    parameter_e.addAttribute(xsitype, type);
    parameter_e.addTextNode(value);

    return message;
}


xoap::MessageReference emuSTEPApplication::createCommandSOAP(string command)
{
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
    envelope.getBody().addBodyElement(name);

    return message;
}

void emuSTEPApplication::analyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app)
{
	string message_str, reply_str;

	reply->writeTo(reply_str);
	ostringstream s;
	s << "Reply from "
			<< app->getClassName() << "(" << app->getInstance() << ")" << endl
			<< reply_str;
	// last_log_.add(s.str());
	LOG4CPLUS_DEBUG(getApplicationLogger(), reply_str);

	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

	// do nothing when no fault
	if (!body.hasFault()) { return; }

	ostringstream error;

	error << "SOAP message: " << endl;
	message->writeTo(message_str);
	error << message_str << endl;
	error << "Fault string: " << endl;
	error << reply_str << endl;

	LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
	XCEPT_RAISE(xoap::exception::Exception, "SOAP fault: \n" + reply_str);

	return;
}

// makes simple combobox inside a form
void emuSTEPApplication::Combo
(
	xgi::Output * out, // output stream
	string varname,  // variable name to return
	string selected, // previously selected value
	string txt[],    // text labels
	string val[],    // values to return
	int n			 // number of items
)
{
	cgicc::option opt; // option for combo
    HTMLAttributeList al; // empty attribute list

   	*out << cgicc::select().set("name", varname).set("style", "background-color:#aaffaa") << std::endl;

	for (int i = 0; i < n; ++i) 
	{
		opt.setAttributes(al); // reset all attributes

		opt.set("value", val[i]); // set value (this will be returned)

		if (val[i].compare(selected) == 0) 
			opt.set("selected", ""); // check if this option was selected

		*out << opt; // dump option into html
		*out << txt[i] << cgicc::option() << std::endl; //option name
	}
	*out << cgicc::select() << std::endl;
}

// makes simple button inside a form
void emuSTEPApplication::Button(xgi::Output * out, string name)
{
/*	*out << cgicc::input().set("type", "submit")
		.set("name", "command")
		.set("value", name)
		.set("size", toString(size)) << std::endl;
*/

	*out << cgicc::button().set("type", "submit")
		.set("name", "command")
		.set("value", name)
		 << "<font face=\"Courier\" size=\"2\">"
		 << name 
		 << "</font>"
		 << cgicc::button();

}

// makes simple checkbox inside a form
void emuSTEPApplication::Checkbox
(
	xgi::Output * out, 
	string varname, 	// name of variable
	bool defsel      // default selection
)
{
	cgicc::input inpt;

	inpt.set("type", "checkbox")
		.set("name", varname)
		.set("value", "1")
		.set("style", "color:#ffaaaa"); 

	if (defsel) inpt.set("checked", "");

	*out << inpt << std::endl;

}


// makes simple button with a form wrapped around
void emuSTEPApplication::FormButton(xgi::Output * out, string name, string command)
{
	beginForm(out, command);
	Button(out, name);
	endForm(out);
}

void emuSTEPApplication::TextField
(
	xgi::Output * out, 
	string varname, 	// name of variable
	string text,        // contents of the text field
	int size			// size
)
{
	*out << cgicc::input().set("type", "text")
		.set("name", varname)
		.set("value", text)
		.set("style", "background-color:#aaaaff")
		.set("size", toString(size)) << std::endl;
}

void emuSTEPApplication::br(xgi::Output * out)
{
	*out << cgicc::br() << std::endl;
}

void emuSTEPApplication::Label(xgi::Output * out, string name)
{
	*out << name << endl;
}


void emuSTEPApplication::beginForm(xgi::Output * out, string command)
{
		*out << cgicc::form().set
		(
			"action",
			 "/" + getApplicationDescriptor()->getURN() + "/" + command
		) << std::endl;
}

void emuSTEPApplication::endForm(xgi::Output * out)
{
	*out << cgicc::form() << endl;
}

void emuSTEPApplication::beginFieldSet(xgi::Output * out, string name)
{
	*out << "<fieldset style=\"font-size: 11pt; font-family: arial;\" >"
		 << "<legend style=\"color:blue\" >" << name << "</legend></p>" << std::endl;
}

void emuSTEPApplication::endFieldSet(xgi::Output * out)
{
	*out << "</fieldset>" << endl;
}




xoap::MessageReference emuSTEPApplication::createParameterGetSOAP(
        string klass, string name, string type)
{
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration("xsi", NS_XSI);

    xoap::SOAPName command = envelope.createName(
            "ParameterGet", "xdaq", XDAQ_NS_URI);
    xoap::SOAPName properties = envelope.createName(
            "properties", klass, "urn:xdaq-application:" + klass);
    xoap::SOAPName parameter = envelope.createName(
            name, klass, "urn:xdaq-application:" + klass);
    xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

    xoap::SOAPElement properties_e = envelope.getBody()
            .addBodyElement(command)
            .addChildElement(properties);
    properties_e.addAttribute(xsitype, "soapenc:Struct");

    xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
    parameter_e.addAttribute(xsitype, type);
    parameter_e.addTextNode("");

    return message;
}

xoap::MessageReference emuSTEPApplication::createParameterGetSOAP2(
        string klass, int length, string names[], string types[])
{
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    envelope.addNamespaceDeclaration("xsi", NS_XSI);

    xoap::SOAPName command = envelope.createName(
            "ParameterGet", "xdaq", XDAQ_NS_URI);
    xoap::SOAPName properties = envelope.createName(
            "properties", klass, "urn:xdaq-application:" + klass);
    xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

    xoap::SOAPElement properties_e = envelope.getBody()
            .addBodyElement(command)
            .addChildElement(properties);
    properties_e.addAttribute(xsitype, "soapenc:Struct");

    for (int i = 0; i < length; ++i) {
        xoap::SOAPName parameter = envelope.createName(
                names[i], klass, "urn:xdaq-application:" + klass);
        xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
        parameter_e.addAttribute(xsitype, types[i]);
        parameter_e.addTextNode("");
    }

    return message;
}


xoap::MessageReference emuSTEPApplication::createCommandSOAPWithAttr(
        string command, std::map<string, string> attr)
{
    xoap::MessageReference message = xoap::createMessage();
    xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
    xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
    xoap::SOAPElement element = envelope.getBody().addBodyElement(name);

    std::map<string, string>::iterator i;
    for (i = attr.begin(); i != attr.end(); ++i) {
        xoap::SOAPName p = envelope.createName((*i).first, "xdaq", XDAQ_NS_URI);
        element.addAttribute(p, (*i).second);
    }

    return message;
}

std::set<xdaq::ApplicationDescriptor *> emuSTEPApplication::getAppsList(string klass)
{
    std::set<xdaq::ApplicationDescriptor *> apps;
    try {
        apps = getApplicationContext()->getDefaultZone()
                ->getApplicationDescriptors(klass);
    } catch (xdaq::exception::ApplicationDescriptorNotFound e) {
	apps.clear();
    }
   return apps;

}


string emuSTEPApplication::extractParameter(
		xoap::MessageReference message, string name)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement parameter = properties.getChildElements(
			*(new xoap::SOAPName(name, "", "")))[0];

	return parameter.getValue();
}


string emuSTEPApplication::getCGIParameter(xgi::Input *in, string name)
{
    cgicc::Cgicc cgi(in);
    string value;

    form_iterator i = cgi.getElement(name);
    if (i != cgi.getElements().end()) {
        value = (*i).getValue();
    }

    return value;
}


std::string emuSTEPApplication::toString(long int i) 
{
    ostringstream s;
    s << i;

    return s.str();
}

std::string emuSTEPApplication::toHexString(long int i, int mindig) 
{
    ostringstream s;
    s << hex << std::setw(mindig) << std::setfill('0') << i;

    return s.str();
}


std::string emuSTEPApplication::toString(long int i, int mindig)
{
    ostringstream s;
    s << std::setw(mindig) << std::setfill('0') << i;

    return s.str();
}



Table::Table(int rows, int cols, int border, bool hcenter)
{
	r = rows;
	c = cols;
	b = border;
	hc = hcenter;
}

xgi::Output *Table::cell(int row, int col)
{

	if (row > r || row < 0 || col > c || col < 0) return &dummy;
	else return &(v[row * c + col]);
}

void Table::flush(xgi::Output * out)
{
	// header
	*out << "<table border=" << b << " bordercolor=0 bordercolorlight=0 bordercolordark=0 cellpadding=2 cellspacing=0";
	if (b > 0) *out << " rules=\"all\""; 
	*out << ">" << endl;

	for (int i = 0; i < r; i++) // row loop
	{
		*out << "<tr>" << endl; // row start
		for (int j = 0; j < c; j++) // col loop
		{
			*out << "<td align=\"" <<  (hc ? "center" : "left") << "\" valign=\"top\">"; 
			*out << cell(i, j)->str(); 
			*out << "</td>\n"; // item
		}
		*out << "</tr>" << endl; // row end
	}

	// trailer
	*out << "</table>" << endl;
		
}


string emuSTEPApplication::getParameter(string klass, string name, string type)
{
	string result = "";
	xdaq::ApplicationDescriptor *app_descr_;
	xoap::MessageReference _param_;

	try 
	{
		app_descr_ = getApplicationContext()->getDefaultZone()
			->getApplicationDescriptor(klass, 0);
	} 
	catch (xdaq::exception::ApplicationDescriptorNotFound e) 
	{
		return result; // Do nothing if the target doesn't exist
	}

	_param_ = createParameterGetSOAP(klass, name, type);

	xoap::MessageReference reply;
	try 
	{
		reply = getApplicationContext()->postSOAP(_param_, *(this->getApplicationDescriptor()), *app_descr_);
		analyzeReply(_param_, reply, app_descr_);

		result = extractParameter(reply, name);
	} 
	catch (xdaq::exception::Exception e) 
	{
		result = "Unknown";
	}

	return result;
}
