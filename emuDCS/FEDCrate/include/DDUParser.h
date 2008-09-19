/*****************************************************************************\
* $Id: DDUParser.h,v 3.8 2008/09/19 16:53:51 paste Exp $
*
* $Log: DDUParser.h,v $
* Revision 3.8  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.7  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
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

namespace emu {

	namespace fed {

		class DDU;
		
		class DDUParser: public EmuParser
		{
		
		public:
			//explicit DDUParser(xercesc::DOMNode * pNode, int crate=0, char *fileName=0);
			explicit DDUParser(xercesc::DOMElement *pNode);
		
			/// the last one parsed
			inline DDU *getDDU() { return ddu_; }
			inline int getOptions() { return options_; }
		
		private:
			DDU *ddu_;//last one parsed
			int slot_;
			int options_;
		};

	}
}

#endif






