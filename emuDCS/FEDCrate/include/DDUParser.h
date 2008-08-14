#ifndef __DDUPARSER_H__
#define __DDUPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"

namespace emu {

	namespace fed {

		class DDU;
		
		class DDUParser: public EmuParser
		{
		
		public:
			explicit DDUParser(xercesc::DOMNode * pNode, int crate=0, char *fileName=0);
		
			/// the last one parsed
			inline DDU *getDDU() { return ddu_; }
		
		private:
			DDU *ddu_;//last one parsed
			int slot_;
		};

	}
}

#endif






