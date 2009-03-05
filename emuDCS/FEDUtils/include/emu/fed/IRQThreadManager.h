/*****************************************************************************\
* $Id: IRQThreadManager.h,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: IRQThreadManager.h,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.9  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.8  2008/09/30 08:32:40  paste
* Updated IRQ Threads so that the endcap name is mentioned in the log filename
*
* Revision 3.7  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_IRQTHREADMANAGER_H__
#define __EMU_FED_IRQTHREADMANAGER_H__

#include <vector>
#include <string>
#include <map>
#include <queue>
#include <pthread.h>

#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Crate;
		class IRQData;

		/** @class IRQThreadManager A class that manages communication with DDUs concerning FMM interrupt handling. **/
		class IRQThreadManager {
		
		public:
		
			IRQThreadManager(std::string myEndcap);
			~IRQThreadManager();
			
			void attachCrate(Crate *crate);
			void startThreads(unsigned long int runNumber = 0)
			throw (emu::fed::exception::FMMThreadException);
			void endThreads()
			throw (emu::fed::exception::FMMThreadException);
			
			inline void setEndcap(std::string myEndcap) { endcap_ = myEndcap; }
		
			static void *IRQThread(void *data);
			
			inline IRQData *data() { return data_; }
		
		private:
		
			std::vector< std::pair<Crate *, pthread_t> > threadVector_;
			IRQData *data_;
			std::string endcap_;
		
		};

	}
}

#endif
