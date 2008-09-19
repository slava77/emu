/*****************************************************************************\
* $Id: DCCParser.h,v 3.7 2008/09/19 16:53:51 paste Exp $
*
* $Log: DCCParser.h,v $
* Revision 3.7  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.6  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
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

namespace emu {
	namespace fed {

		class DCC;
		
		class DCCParser: public EmuParser
		{
		
		public:
			DCCParser(){}
			explicit DCCParser(xercesc::DOMElement *pNode);
				
			/// the last one parsed
			inline DCC *getDCC() { return dcc_; }
		
		private:
			DCC *dcc_;//last one parsed
		};

	}
}

#endif






