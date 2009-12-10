/*****************************************************************************\
* $Id: FIFOParser.h,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIFOPARSER_H__
#define __EMU_FED_FIFOPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {
	namespace fed {

		class FIFO;

		/** @namespace FIFOParser A parser that builds FIFO objects to be loaded into the DCC.
		*	@sa FIFO
		**/
		namespace FIFOParser {

			/** @returns a pointer to the parsed FIFO object. **/
			FIFO *parse(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** @returns a DOMElement from a given FIFO object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, FIFO *fifo)
			throw (emu::fed::exception::ParseException);

		}

	}
}

#endif
