/*****************************************************************************\
* $Id: Parser.h,v 1.5 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_PARSER_H__
#define __EMU_FED_PARSER_H__

#include <string>
#include <iostream>
#include <sstream>
#include "xercesc/dom/DOM.hpp"
#include "emu/fed/Exception.h"

#define X(str) xercesc::XMLString::transcode(str)

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

			/** Extract a named attribute from the DOM node and return it converted into a variable of type T.
			*
			*	@param item the name of the attribute to extract.
			*	@param flags the format flags to be used when extracting from the string representation of the attribute value to the type T.
			**/
			template<class T>
			T extract(const char *item, const std::ios_base::fmtflags &flags = std::ios::dec)
			throw (emu::fed::exception::ParseException)
			{
				XMLCh *name = X(item);
				xercesc::DOMAttr *pAttributeNode = (xercesc::DOMAttr *) pAttributes_->getNamedItem(name);
				
				if (pAttributeNode == NULL) {
					std::ostringstream error;
					error << "Attribute " << item << " does not appear to exist";
					XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
				}

				std::stringstream scan;
				scan.exceptions(std::stringstream::failbit | std::stringstream::badbit);
				scan << X(pAttributeNode->getNodeValue());
				T target;
				try {
					scan.flags(flags);
					scan >> target;
				} catch (std::stringstream::failure &e) {
					std::ostringstream error;
					error << "Unable to parse " << item << "=" << X(pAttributeNode->getNodeValue()) << ": " << e.what();
					XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
				}
				return target;
			}
			
			template<class T>
			T extract(const std::string &item, const std::ios_base::fmtflags &flags = std::ios::dec)
			throw (emu::fed::exception::ParseException)
			{
				try {
					return extract<T>(item.c_str(), flags);
				} catch (...) {
					throw;
				}
			}
			
			/** Insert a name/value pair into a given DOMElement
			*
			* @param name the name of the attribute to insert
			* @param value the value of the attribute
			**/
			template<class T>
			static void insert(xercesc::DOMElement *element, const std::string &name, const T &value, const std::ios_base::fmtflags &flags = std::ios::dec)
			throw (emu::fed::exception::ParseException)
			{
				std::ostringstream valStream;
				valStream.flags(flags);
				valStream << value;
				try {
					element->setAttribute(X(name.c_str()), X(valStream.str().c_str()));
				} catch (xercesc::DOMException &e) {
					std::ostringstream error;
					error << "Unable to set attribute " << name << " with value " << valStream.str() << ": " << X(e.getMessage());
					XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
				}
			}
		
		private:

			/// The attributes from the given DOM node.
			xercesc::DOMNamedNodeMap *pAttributes_;
		
		};

	}
}

#endif

