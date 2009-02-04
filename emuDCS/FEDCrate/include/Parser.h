/*****************************************************************************\
* $Id: Parser.h,v 3.2 2009/02/04 18:28:11 paste Exp $
*
* $Log: Parser.h,v $
* Revision 3.2  2009/02/04 18:28:11  paste
* Updated for 6.10 release.  Fixed some obvious bugs.  Still problems with EmuFCrateHyperDAQ display.
*
* Revision 3.1  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.3  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __PARSER_H__
#define __PARSER_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include "FEDException.h"

namespace emu {
	namespace fed {

		/** @class Parser A utility class that allows for easy parsing of attributes of an XML DOM node. **/
		class Parser
		{
		public:

			/** Default constructor.
			*
			*	@param pNode the node to parse.
			**/
			Parser(xercesc::DOMNode *pNode);

		protected:

			/** Extract a named attribute from the DOM node and return it converted into a variable of type T.
			*
			*	@param item the name of the attribute to extract.
			*	@param flags the format flags to be used when extracting from the string representation of the attribute value to the type T.
			**/
			template<class T>
			T Parser::extract(const char *item, std::ios_base::fmtflags flags = std::ios::dec)
			throw (ParseException)
			{
				XMLCh *name = xercesc::XMLString::transcode(item);
				xercesc::DOMAttr *pAttributeNode;
				try {
					pAttributeNode = (xercesc::DOMAttr *) pAttributes_->getNamedItem(name);
				} catch (xercesc::DOMException &e) {
					XCEPT_RAISE(XMLException, xercesc::XMLString::transcode(e.getMessage()));
				}
				std::stringstream scan;
				scan.exceptions(std::stringstream::failbit | std::stringstream::badbit);
				scan << xercesc::XMLString::transcode(pAttributeNode->getNodeValue());
				T target;
				try {
					scan.flags(flags);
					scan >> target;
				} catch (std::stringstream::failure &e) {
					std::ostringstream error;
					error << "Unable to read " << item << "=" << xercesc::XMLString::transcode(pAttributeNode->getNodeValue()) << ": " << e.what();
					XCEPT_RAISE(ParseException, error.str());
				}
				return target;
			}
			
			template<class T>
			T emu::fed::Parser::extract(const std::string item, std::ios_base::fmtflags flags = std::ios::dec)
			throw (ParseException)
			{
				try {
					return extract<T>(item.c_str(), flags);
				} catch (...) {
					throw;
				}
			}
		
		private:

			/// The attributes from the given DOM node.
			xercesc::DOMNamedNodeMap *pAttributes_;
		
		};

	}
}

#endif

