/*****************************************************************************\
* $Id: DDUParser.h,v 3.6 2008/08/15 10:23:01 paste Exp $
*
* $Log: DDUParser.h,v $
* Revision 3.6  2008/08/15 10:23:01  paste
* Fixed EmuFCrateLoggable function definitions
*
* Revision 3.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DDUPARSER_H__
#define __DDUPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"
#include "EmuFEDLoggable.h"

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






