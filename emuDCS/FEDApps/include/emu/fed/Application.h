/*****************************************************************************\
* $Id: Application.h,v 1.7 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_APPLICATION_H__
#define __EMU_FED_APPLICATION_H__

#include "xdaq/WebApplication.h"
#include "emu/fed/Exception.h"

#include <string>
#include <vector>

#include "xoap/DOMParser.h"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xoap/MessageReference.h"
#include "xdata/soap/Serializer.h"
#include "xoap/SOAPName.h"
#include "xdata/xdata.h"
#include "xdaq/NamespaceURI.h"
#include "xdata/soap/NamespaceURI.h"
#include "xoap/domutils.h"
#include "xgi/Input.h"
#include "xoap/Method.h"

namespace emu {
	namespace fed {
		
		/** @class Application A class from which all FED XDAQ web applications should inherit.
		*	Includes routines for display, logging, SOAP communication, and useful pre-defined members.
		*
		*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
		**/
		class Application: public virtual xdaq::WebApplication
		{

		public:

			/** Default constructor. **/
			Application(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~Application();

			/** Sends the GetParameters SOAP command to a target application.
			*
			*	@param applicationDescriptor is a pointer to the target application's
			*	descriptor.
			*	@returns a SOAP message reply for use with @sa readParameter.
			*
			*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
			**/
			xoap::MessageReference getParameters(xdaq::ApplicationDescriptor *applicationDescriptor)
			throw (emu::fed::exception::SOAPException);

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
			xoap::MessageReference getParameters(const std::string &applicationName, const unsigned int &instance)
			throw (emu::fed::exception::SOAPException);

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
			void setParameter(const std::string &klass, const std::string &name, const std::string &type, const std::string &value, const int &instance = -1)
			throw (emu::fed::exception::SOAPException);

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
			*
			*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
			**/
			template<typename T>
			T readParameter(xoap::MessageReference &message, const std::string &parameterName)
			throw (emu::fed::exception::SOAPException)
			{
				T thingToGet;
				xdata::soap::Serializer serializer;

				std::string messageStr;
				message->writeTo(messageStr);
				
				xercesc::DOMDocument *doc;
				try {
					doc = soapParser_->parse(messageStr);
				} catch (xoap::exception::Exception &e) {
					std::ostringstream error;
					error << "Unable to parse SOAP message: " << messageStr;
					XCEPT_RETHROW(emu::fed::exception::SOAPException, error.str(), e);
				}

				xoap::SOAPName name(parameterName, "xdaq", XDAQ_NS_URI);
				bool found = false;
				xercesc::DOMNodeList *dataNode = doc->getElementsByTagNameNS(xoap::XStr(XDAQ_NS_URI), xoap::XStr("data"));
				if (dataNode->item(0)) {
					xercesc::DOMNodeList *dataElements = dataNode->item(0)->getChildNodes();
					for (unsigned int j = 0; j < dataElements->getLength(); j++) {
						xercesc::DOMNode *n = dataElements->item(j);
						if (n->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
							std::string nodeName = xoap::XMLCh2String(n->getNodeName());
							if (nodeName == name.getQualifiedName()) {
								serializer.import(&thingToGet, n);
								found = true;
								break;
							}
						}
					}
				}
				
				// Saves 50 KB from being lost forever!
				doc->release();
				
				if (!found) {
					std::ostringstream error;
					error << "Unable to find parameter " << parameterName;
					XCEPT_RAISE(emu::fed::exception::SOAPException, error.str());
				}
				return thingToGet;
			}
			
			/** Find the application with a given value for a SOAP parameter.
			*
			* @param myClass the class of the applications to search
			* @param parameter the name of the parameter to match
			* @param value the value of the parameter to match
			*
			* @returns the application descriptor of the matching application
			**/
			template<typename T, typename V>
			xdaq::ApplicationDescriptor *findMatchingApplication(const std::string &myClass, const std::string &parameter, const V &value)
			throw (emu::fed::exception::SoftwareException)
			{
				
				std::set<xdaq::ApplicationDescriptor *> descriptors = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors(myClass);
				
				for (std::set<xdaq::ApplicationDescriptor *>::iterator jDescriptor = descriptors.begin(); jDescriptor != descriptors.end(); jDescriptor++) {
					
					xoap::MessageReference reply;
					try {
						reply = getParameters((*jDescriptor));
					} catch (emu::fed::exception::SOAPException &e) {
						std::ostringstream error;
						error << "Unable to get parameters from application '" << (*jDescriptor)->getClassName() << "' instance " << (*jDescriptor)->getInstance();
						LOG4CPLUS_WARN(getApplicationLogger(), error.str());
						XCEPT_DECLARE_NESTED(emu::fed::exception::SOAPException, e2, error.str(), e);
						notifyQualified("WARN", e2);
						continue;
					}
					
					try {
						V myValue = readParameter<T>(reply, parameter);
						if (myValue == value) {
							return (*jDescriptor);
						}
					} catch (emu::fed::exception::SOAPException &e) {
						std::ostringstream error;
						error << "Unable to read parameter '" << parameter << "' from application '" << (*jDescriptor)->getClassName() << "' instance " << (*jDescriptor)->getInstance();
						LOG4CPLUS_WARN(getApplicationLogger(), error.str());
						XCEPT_DECLARE_NESTED(emu::fed::exception::SOAPException, e2, error.str(), e);
						notifyQualified("WARN", e2);
						continue;
					}
				}
				
				std::ostringstream error;
				error << "Unable to find an application of class '" << myClass << "' with a parameter '" << parameter << "' matching '" << value << "'";
				LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
				XCEPT_RAISE(emu::fed::exception::SoftwareException, error.str());
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
			virtual std::string Header(const std::string &myTitle, const std::vector<std::string> &jsFileNames);
			virtual inline std::string Header(const std::string &myTitle)
			{
				std::vector<std::string> fileNames;
				return Header(myTitle, fileNames);
			}

			/** Returns the standard Footer for the EmuFCrate applications.
			*
			*	@returns a huge std::string that is basically the footer code in HTML.  Good
			*	 for outputting straight to the xgi::Output.
			**/
			virtual std::string Footer();

			/** Dumps out information from the CGI input variable for debugging.
			*
			*	@param in is the standard CGI input variable that is passed to almost
			*	everything.
			*
			*	@returns a string with a mess of debugging text.
			**/
			std::string dumpEnvironment(xgi::Input *in);

			/** Serializes this application's ApplicationInfoSpace and returns it.
			*	As things stand, is bound to the "GetParameters" SOAP command.
			*
			*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
			**/
			xoap::MessageReference onGetParameters(xoap::MessageReference message);

			/* Creates a simple reply to a SOAP command.
			*
			*	@param message is the message to which a reply is generated
			**/
			//xoap::MessageReference createSOAPReply(xoap::MessageReference message);

			/** Redirect the browser to the default page.
			*
			*	@param in is the standard XGI input
			*	@param out is the standard XGI output
			*	@param location is the URL to where the browser will redirect
			*
			*	@author Phillip Killewald &lt;paste@mps.ohio-state.edu&gt;
			**/
			void webRedirect(xgi::Input *in, xgi::Output *out, const std::string &location = "");
			inline void webRedirect (xgi::Output *out, const std::string &location = "") { return webRedirect(new xgi::Input("", 0), out, location); }

			/** Create a simple SOAP command for FSM transitions.
			*
			*	@param command is the FSM transition command
			**/
			xoap::MessageReference createSOAPCommand(const std::string &command);

			/** Send a SOAP command to a given application or applications
			*
			*	@param command is the command to be sent
			*	@param klass is the name of the class to which the command should be sent
			*	@param instance is the instance of the given class to which the command should be sent.  A -1 value means send to all instances of the given class.
			**/
			void sendSOAPCommand(const std::string &command, const std::string &klass, const int instance = -1)
			throw (emu::fed::exception::SOAPException);
			
			/** Print all the exceptions in the history of a given exception.
			*
			* @param myException the exception you want to display
			* @returns a string with an HTML representation of the history of the exception
			*
			* @author Phillip Killewald
			**/
			std::string printException(xcept::Exception &myException);
			

		protected:
			
			/// Let the SOAP DOMParser be a member to avoid thrashing
			xoap::DOMParser *soapParser_;
			
			/// A string that holds my class name (without the namespace)
			std::string myClassName_;
			
		};
	}
}


#endif
