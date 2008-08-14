#ifndef __CHAMBERPARSER_H__
#define __CHAMBERPARSER_H__
/*
 *  class ChamberParser
 *  author Phillip Killewald 1/31/08
 *     
 */

#include <vector>
#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"

//XERCES_CPP_NAMESPACE_USE

namespace emu {
	namespace fed {

		class Chamber;

		class ChamberParser: public EmuParser
		{
		
		public:

			explicit ChamberParser(char *myFileName, int myCrate, int mySlot);
		
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
