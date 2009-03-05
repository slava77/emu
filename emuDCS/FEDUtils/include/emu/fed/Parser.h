/*****************************************************************************\
* $Id: Parser.h,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: Parser.h,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.1  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.3  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_PARSER_H__
#define __EMU_FED_PARSER_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include "emu/fed/Exception.h"

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
			T extract(const char *item, std::ios_base::fmtflags flags = std::ios::dec)
			throw (emu::fed::exception::ParseException)
			{
				XMLCh *name = xercesc::XMLString::transcode(item);
				xercesc::DOMAttr *pAttributeNode;
				try {
					pAttributeNode = (xercesc::DOMAttr *) pAttributes_->getNamedItem(name);
				} catch (xercesc::DOMException &e) {
					XCEPT_RAISE(emu::fed::exception::ParseException, xercesc::XMLString::transcode(e.getMessage()));
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
					XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
				}
				return target;
			}
			
			template<class T>
			T extract(const std::string item, std::ios_base::fmtflags flags = std::ios::dec)
			throw (emu::fed::exception::ParseException)
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

