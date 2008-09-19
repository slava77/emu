/*****************************************************************************\
* $Id: VMEControllerParser.h,v 3.5 2008/09/19 16:53:51 paste Exp $
*
* $Log: VMEControllerParser.h,v $
* Revision 3.5  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.4  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __VMECONTROLLERPARSER_H__
#define __VMECONTROLLERPARSER_H__

#include <xercesc/dom/DOM.hpp>

#include "EmuParser.h"

namespace emu {
	namespace fed {

		class VMEController;

		class VMEControllerParser : public EmuParser
		{
		
		public:
			explicit VMEControllerParser(xercesc::DOMElement *pNode);
			
			inline VMEController *getController() { return vmeController_; }
		
		private:
			VMEController *vmeController_;
		};

	}
}

#endif






