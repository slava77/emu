/*****************************************************************************\
* $Id: FiberParser.h,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIBERPARSER_H__
#define __EMU_FED_FIBERPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {
	namespace fed {

		class Fiber;

		/** @namespace FiberParser A parser that builds Fiber objects to be loaded into the DDU.
		*	@sa Fiber
		**/
		namespace FiberParser {

			/** @returns a pointer to the parsed Fiber object. **/
			Fiber *parse(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);

			/** @returns a DOMElement from a given Fiber object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, Fiber *fiber)
			throw (emu::fed::exception::ParseException);

		}

	}
}

#endif
