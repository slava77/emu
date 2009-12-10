/*****************************************************************************\
* $Id: CrateParser.h,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CRATEPARSER_H__
#define __EMU_FED_CRATEPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {
	namespace fed {

		class Crate;

		/** @namespace CrateParser A utility namespace that will parse a FED configuration XML node and build from it a FED Crate object.
		*	@sa Crate
		**/
		namespace CrateParser {


			/** @returns a vector of the crates parsed from the XML file. **/
			Crate *parse(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** @returns a DOMElement from a given crate object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, Crate *crate)
			throw (emu::fed::exception::ParseException);

		}

	}
}

#endif






