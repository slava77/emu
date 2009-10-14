/*****************************************************************************\
* $Id: CSSElement.h,v 1.2 2009/10/14 20:02:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CSSELEMENT_H__
#define __EMU_FED_CSSELEMENT_H__

#include <string>
#include <vector>
#include <ostream>

#include "xgi/Output.h"

namespace emu {
	namespace fed {
		
		/** @class CSSElement a class that contains simple methods to edit CSS ID and class attributes. **/
		class CSSElement
		{
			
		public:
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes the CSS class of the table.
			**/
			CSSElement(const std::string &id = "", const std::string &classes = "");
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			CSSElement(const std::string &id, const std::vector<std::string> &classes);
			
			/** Standard constructor.
			*
			* @param id the CSS ID of the table.
			* @param classes a vector of CSS classes of the table.
			**/
			CSSElement(const std::vector<std::string> &classes, const std::string &id = "");
			
			/** Copy constructor. **/
			CSSElement(const CSSElement &myElement);
			CSSElement(CSSElement &myElement);
			
			/** Destructor. **/
			virtual ~CSSElement();
			
			/** Assignment operator. **/
			CSSElement &operator=(const CSSElement &myElement);
			CSSElement &operator=(CSSElement &myElement);
			
			/** @return the CSS ID of the table. **/
			std::string getID() const;
			
			/** Set the CSS ID of the table. **/
			CSSElement &setID(const std::string &id);
			
			/** @return the CSS classes of the table. **/
			std::string getClass() const;
			
			/** @return the CSS classes of the table in vector form. **/
			std::vector<std::string> getClasses() const;
			
			/** Set the CSS classes of the table. **/
			CSSElement &setClass(const std::string &classes);
			
			/** Set the CSS classes of the table. **/
			CSSElement &setClasses(const std::vector<std::string> &classes);
			
			/** Return an HTML representation of the element as a string.  **/
			virtual std::string toHTML() = 0;
			
			/** Return a formatted text representation of the element as a string.  **/
			virtual std::string toText() = 0;
			
		private:
			
			std::string id_;
			std::vector<std::string> classes_;
			
		};
	}
}

#endif
