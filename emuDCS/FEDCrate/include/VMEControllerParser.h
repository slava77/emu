/*****************************************************************************\
* $Id: VMEControllerParser.h,v 3.3 2008/08/15 10:23:02 paste Exp $
*
* $Log: VMEControllerParser.h,v $
* Revision 3.3  2008/08/15 10:23:02  paste
* Fixed EmuFCrateLoggable function definitions
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
#include "EmuFEDLoggable.h"

namespace emu {
	namespace fed {

		class VMEController;

		class VMEControllerParser : public EmuParser
		{
		
		public:
			explicit VMEControllerParser(xercesc::DOMNode * pNode, int crate = 0);
			
			inline VMEController *getController() { return vmeController_; }
		
		private:
			VMEController *vmeController_;
		};

	}
}

#endif






