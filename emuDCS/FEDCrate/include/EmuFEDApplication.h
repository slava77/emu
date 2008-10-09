/*****************************************************************************\
* $Id: EmuFEDApplication.h,v 3.8 2008/10/09 11:21:19 paste Exp $
*
* $Log: EmuFEDApplication.h,v $
* Revision 3.8  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 3.7  2008/10/04 18:44:05  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
* Revision 3.6  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.5  2008/08/15 16:14:50  paste
* Fixed threads (hopefully).
*
* Revision 3.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMUFEDAPPLICATION_H__
#define __EMUFEDAPPLICATION_H__

#include "EmuApplication.h"
#include "EmuFEDLoggable.h"

#include <string>
#include <vector>

#include "xoap/DOMParser.h"
#include "xoap/MessageReference.h"
#include "xdata/soap/Serializer.h"
#include "xoap/SOAPName.h"
#include "xdata/xdata.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/domutils.h"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMNode.hpp"


class EmuFEDApplication: public emu::fed::EmuFEDLoggable, public EmuApplication {

public:

	xdata::UnsignedLong runNumber_;
	xdata::String runType_;

	/** Time in seconds to automatically refresh a HyperDAQ page.
	*	Negative numbers turn off auto-refresh.
	**/
	xdata::Integer autoRefresh_;

	EmuFEDApplication(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception);

	/** Sends the GetParameters SOAP command to a target application.
	*
	*	@param applicationDescriptor is a pointer to the target application's
	*	descriptor.
	*	@returns a SOAP message reply for use with @sa readParameter.
	*
	*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
	**/
	xoap::MessageReference getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
		throw (xdaq::exception::Exception);

	/** Sends the GetParameters SOAP command to a target application.
	*
	*	@param applicationName is the class name of the application from which
	*	you are requesting parameters.
	*	@param instance is the instance of the application to which you are
	*	sending the request.
	*	@returns a SOAP message reply for use with @sa readParameter.
	*
	*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
	**/
	xoap::MessageReference getParameters(std::string applicationName, unsigned int instance)
		throw (xdaq::exception::Exception);

	/** Sets a parameter in a remote application.  I don't know where the
	*	binding to this command is set, but it seems to affect all
	*	of the applications we are concerned with.
	*
	*	@param klass is the target application class name (this
	*	function sends to all of the instances of that class.)
	*	@param name is the name of the variable you want to set.
	*	@param type is the type of the varialbe you want to set.  Use
	*	std::strings like "xsd:string" and "xsd:unsignedLong".
	*	@param value is a std::string representation of the value to be set.
	*	@param instance is the instance of the application whose parameter
	*	you want to set.  Optional.  Negative numbers will result in all
	*	applications of class name klass being sent the message.
	*
	*	@note Because I don't know where this call is handled, I do not
	*	know anything about error handling.  Just try to make sure
	*	the variable exists in the exported InfoSpace of the target
	*	application, and that the types match.
	*
	*	@author Phillip Killewald (stolen from Laria's CSCSupervisor.cc)
	**/
	void setParameter(std::string klass, std::string name, std::string type, std::string value, int instance = -1)
		throw (xdaq::exception::Exception);

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
	*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
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
		xercesc::DOMDocument *doc = parser->parse(messageStr);

		//xoap::SOAPElement sInfoSpace = message->getSOAPPart().getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("GetParametersResponse", "", "")))[0];

		xoap::SOAPName *name = new xoap::SOAPName(parameterName, "xdaq", XDAQ_NS_URI);
		//std::cout << "Looking for " << name->getQualifiedName() << ", " << name->getURI() << std::endl;


		xercesc::DOMNodeList* dataNode = doc->getElementsByTagNameNS(xoap::XStr(XDAQ_NS_URI), xoap::XStr("data"));
		xercesc::DOMNodeList* dataElements = dataNode->item(0)->getChildNodes();

		for (unsigned int j = 0; j < dataElements->getLength(); j++) {
			xercesc::DOMNode* n = dataElements->item(j);
			if (n->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
				std::string nodeName = xoap::XMLCh2String(n->getNodeName());
				//std::cout << "Found " << nodeName << std::endl;
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
	*	@param myTitle is the title that will be displayed at the top of the
	*	 page.
	*	@param jsFileNames is a vector of file names to be included as javascripts,
	*	 see the directory FEDCrate/js for a list of files one can include.
	*	@returns a huge std::string that is basically the header of the page in
	*	 HTML.  Good for outputting straight to the xgi::Output.
	**/
	virtual std::string Header(std::string myTitle, std::vector<std::string> jsFileNames);
	
	virtual std::string Header(std::string myTitle);

	/** Returns the standard Footer for the EmuFCrate applications.
	*
	*	@returns a huge std::string that is basically the footer code in HTML.  Good
	*	 for outputting straight to the xgi::Output.
	**/
	virtual std::string Footer();

protected:

	// "Globals"
	std::string NS_XSI;
	std::string STATE_UNKNOWN;
	
	/** Serializes this application's ApplicationInfoSpace and returns it.
	*	As things stand, is bound to the "GetParameters" SOAP command.
	*
	*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
	**/
	xoap::MessageReference onGetParameters(xoap::MessageReference message)
		throw (xoap::exception::Exception);

private:

};


#endif
