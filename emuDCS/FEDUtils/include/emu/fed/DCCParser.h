/*****************************************************************************\
* $Id: DCCParser.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCPARSER_H__
#define __EMU_FED_DCCPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class DCC;

		/** @class DCCParser A parser that builds DCC objects to be loaded into a FEDCrate.
		*	@sa DCC
		**/
		class DCCParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit DCCParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** Default destructor **/
			~DCCParser();
			
			/** @returns a pointer to the parsed DCC object. **/
			inline DCC *getDCC() { return dcc_; }
		
		private:

			/// A DCC object built from the parsed attributes of the DOM node.
			DCC *dcc_;
		};

	}
}

#endif






