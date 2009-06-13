/*****************************************************************************\
* $Id: DDUParser.h,v 1.2 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUPARSER_H__
#define __EMU_FED_DDUPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {

	namespace fed {

		class DDU;

		/** @class DDUParser A parser that builds DDU objects to be loaded into a FEDCrate.
		*	@sa DDU
		**/
		class DDUParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit DDUParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** Default destructor **/
			~DDUParser();
		
			/** @returns a pointer to the parsed DDU object. **/
			inline DDU *getDDU() { return ddu_; }
		
		private:

			/// A DDU object built from the parsed attributes of the DOM node.
			DDU *ddu_;

		};

	}
}

#endif






