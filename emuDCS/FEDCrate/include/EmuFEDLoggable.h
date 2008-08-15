/*****************************************************************************\
* $Id: EmuFEDLoggable.h,v 3.4 2008/08/15 16:14:50 paste Exp $
*
* $Log: EmuFEDLoggable.h,v $
* Revision 3.4  2008/08/15 16:14:50  paste
* Fixed threads (hopefully).
*
* Revision 3.3  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMUFEDLOGGABLE_H__
#define __EMUFEDLOGGABLE_H__

#include <log4cplus/logger.h>

namespace emu {
	namespace fed {

		class EmuFEDLoggable
		{
			
		public:
			
			EmuFEDLoggable();
			~EmuFEDLoggable();
			
		protected:
			
		private:
		
		};

	}
}

#endif
