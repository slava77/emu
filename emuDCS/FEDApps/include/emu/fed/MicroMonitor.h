/*****************************************************************************\
* $Id: MicroMonitor.h,v 1.1 2009/07/06 15:50:10 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MICROMONITOR_H__
#define __EMU_FED_MICROMONITOR_H__

#include "emu/fed/Configurable.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
		
		/** @class Monitor An XDAq application for monitoring only vital information from the FED crates for passing data to DCS.
		*
		*	@author Phillip Killewald
		**/
		class MicroMonitor: public virtual emu::fed::Configurable
		{
			
		public:
			XDAQ_INSTANTIATOR();
			
			/** Default Constructor **/
			MicroMonitor(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~MicroMonitor();
			
			// HyperDAQ pages
			void DCSOutput(xgi::Input *in, xgi::Output *out);
			
			
		private:
			
		};
		
	}
}

#endif
