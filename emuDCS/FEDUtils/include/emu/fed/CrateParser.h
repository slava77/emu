/*****************************************************************************\
* $Id: CrateParser.h,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CRATEPARSER_H__
#define __EMU_FED_CRATEPARSER_H__

#include <vector>
#include <string>
#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Crate;

		/** @class CrateParser A utility class that will parse a FED configuration XML node and build from it a FED Crate object.
		*	@sa Crate
		**/
		class CrateParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the name of the XML file to parse.
			**/
			CrateParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);
			
			/** Default destructor **/
			~CrateParser();

			/** @returns a vector of the crates parsed from the XML file. **/
			inline Crate *getCrate() { return crate_; }
		
		private:

			/// The crates as parsed from the XML file, filled with the appropriate objects.
			Crate *crate_;

		};

	}
}

#endif






