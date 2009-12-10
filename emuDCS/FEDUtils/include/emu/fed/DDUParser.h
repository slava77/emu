/*****************************************************************************\
* $Id: DDUParser.h,v 1.3 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUPARSER_H__
#define __EMU_FED_DDUPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {

	namespace fed {

		class DDU;

		/** @namespace DDUParser A parser that builds DDU objects to be loaded into a FEDCrate.
		*	@sa DDU
		**/
		namespace DDUParser {
		
			/** @returns a pointer to the parsed DDU object. **/
			DDU *parse(xercesc::DOMElement *pNode, const bool &fake = false)
			throw (emu::fed::exception::ParseException);
			
			/** @returns a DOMElement from a given DDU object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, DDU *ddu)
			throw (emu::fed::exception::ParseException);

		}

	}
}

#endif






