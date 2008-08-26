/*****************************************************************************\
* $Id: ChamberParser.h,v 1.8 2008/08/26 13:40:08 paste Exp $
*
* $Log: ChamberParser.h,v $
* Revision 1.8  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
* Revision 1.6  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 1.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __CHAMBERPARSER_H__
#define __CHAMBERPARSER_H__

#include <vector>
#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"

namespace emu {
	namespace fed {

		/// \no
		class Chamber;

		/** @class ChamberParser A parser that builds Chamber objects to be loaded into the DDU.
		*	@sa Chamber
		**/
		class ChamberParser: public EmuParser
		{
		
		public:

			/** The constructor will automatically perform the parsing of Karoly's
			*	Chamber->RUI XML mapping file.
			*
			*	@param myFileName the absolute location to the Chamber->RUI XML file to parse.
			*	@param myCrate the crate number of the DDU in question (for parsing purposes).
			*	@param mySlot the slot number of the DDU in question (for parsing purposes).
			*
			*	@todo Change the DDU configuration file so that it contains the chamber
			*	definitions so this parser can work like all the rest.
			**/
			explicit ChamberParser(char *myFileName, int myCrate, int mySlot);
		
			/// @returns the vector of chambers for the given DDU as parsed from the XML file
			inline std::vector<Chamber *> getChambers() const { return chamberVector_; }
		
		private:
			// This parsing is annoying.  Use Stan's idea of parsing every step with a different method.
			std::vector<xercesc::DOMNode *> parseMaps(xercesc::DOMNode *myDoc);
			std::vector<xercesc::DOMNode *> parseRUIs(xercesc::DOMNode *myMap);
			std::vector<xercesc::DOMNode *> parseDDUs(xercesc::DOMNode *myRUI, int myCrate, int mySlot);
			void parseInput(xercesc::DOMNode *myDDU);
		
			std::vector<Chamber *> chamberVector_;
		};

	}
}

#endif
