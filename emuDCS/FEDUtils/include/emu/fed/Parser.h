/*****************************************************************************\
* $Id: Parser.h,v 1.2 2009/05/16 18:55:20 paste Exp $
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
				xercesc::DOMAttr *pAttributeNode = (xercesc::DOMAttr *) pAttributes_->getNamedItem(name);
				
				if (pAttributeNode == NULL) {
					std::ostringstream error;
					error << "Attribute " << item << " does not appear to exist";
					XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
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

