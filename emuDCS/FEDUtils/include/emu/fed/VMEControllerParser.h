/*****************************************************************************\
* $Id: VMEControllerParser.h,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERPARSER_H__
#define __EMU_FED_VMECONTROLLERPARSER_H__

#include "emu/fed/Parser.h"

namespace emu {
	namespace fed {

		class VMEController;

		/** @namespace VMEControllerParser A parser that builds VMEController objects to be loaded into a FEDCrate.
		*	@sa VMEController
		**/
		namespace VMEControllerParser {
			
			/** Parse and return a (real or fake) VMEController **/
			VMEController *parse(xercesc::DOMElement *pNode, const bool &fake = false)
			throw (emu::fed::exception::ParseException);

			/** @returns a DOMElement from a given VMEController object **/
			xercesc::DOMElement *makeDOMElement(xercesc::DOMDocument *document, VMEController *controller)
			throw (emu::fed::exception::ParseException);
			
		}
	}
}

#endif






