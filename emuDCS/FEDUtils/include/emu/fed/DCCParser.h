/*****************************************************************************\
* $Id: DCCParser.h,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCPARSER_H__
#define __EMU_FED_DCCPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {
	namespace fed {

		class DCC;

		/** @namespace DCCParser A parser that builds DCC objects to be loaded into a FEDCrate.
		*	@sa DCC
		**/
		namespace DCCParser {
			
			/** @returns a pointer to the parsed DCC object. **/
			DCC *parse(xercesc::DOMElement *pNode, const bool &fake = false)
			throw (emu::fed::exception::ParseException);
			
			/** @returns a DOMElement from a given DCC object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, DCC *dcc)
			throw (emu::fed::exception::ParseException);
		}
	}
}

#endif






