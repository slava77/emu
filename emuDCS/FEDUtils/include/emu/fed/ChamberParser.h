/*****************************************************************************\
* $Id: ChamberParser.h,v 1.2 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_CHAMBERPARSER_H__
#define __EMU_FED_CHAMBERPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Chamber;

		/** @class ChamberParser A parser that builds Chamber objects to be loaded into the DDU.
		*	@sa Chamber
		**/
		class ChamberParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit ChamberParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);

			/** @returns a pointer to the parsed Chamber object. **/
			inline Chamber *getChamber() { return chamber_; }

			/** @returns whether or not the chamber is marked to be masked off (killed) at the DDU. **/
			inline const bool isKilled() { return killed_; }

			/** @returns the parsed fiber number of the chamber. **/
			inline const unsigned int getFiber() { return fiber_; }
		
		private:

			/// A Chamber object built from the parsed attributes of the DOM node.
			Chamber *chamber_;

			/// Whether or not the fiber is killed.
			bool killed_;

			/// The fiber number of the chamber.
			unsigned int fiber_;
		};

	}
}

#endif
