#ifndef __DCCPARSER_H__
#define __DCCPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"

namespace emu {
	namespace fed {

		class DCC;
		
		class DCCParser: public EmuParser
		{
		
		public:
			DCCParser(){}
			explicit DCCParser(xercesc::DOMNode *pNode, int crate = 0);
				
			/// the last one parsed
			inline DCC *getDCC() { return dcc_; }
		
		private:
			DCC *dcc_;//last one parsed
		};

	}
}

#endif






