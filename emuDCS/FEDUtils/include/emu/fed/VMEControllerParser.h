/*****************************************************************************\
* $Id: VMEControllerParser.h,v 1.2 2009/05/22 19:25:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERPARSER_H__
#define __EMU_FED_VMECONTROLLERPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class VMEController;

		/** @class DDUParser A parser that builds DDU objects to be loaded into a FEDCrate.
		*	@sa VMEController
		**/
		class VMEControllerParser : public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit VMEControllerParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);

			/** @returns a pointer to the parsed VMEController object. **/
			inline VMEController *getController() { return vmeController_; }
		
		private:

			/// A VMEController object built from the parsed attributes of the DOM node.
			VMEController *vmeController_;
		};

	}
}

#endif






