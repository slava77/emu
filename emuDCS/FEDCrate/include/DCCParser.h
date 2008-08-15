/*****************************************************************************\
* $Id: DCCParser.h,v 3.5 2008/08/15 10:23:01 paste Exp $
*
* $Log: DCCParser.h,v $
* Revision 3.5  2008/08/15 10:23:01  paste
* Fixed EmuFCrateLoggable function definitions
*
* Revision 3.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DCCPARSER_H__
#define __DCCPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"
#include "EmuFEDLoggable.h"

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






