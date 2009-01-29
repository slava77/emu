/*****************************************************************************\
* $Id: IRQThreadManager.h,v 3.9 2009/01/29 15:31:22 paste Exp $
*
* $Log: IRQThreadManager.h,v $
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
#ifndef __IRQTHREADMANAGER_H__
#define __IRQTHREADMANAGER_H__

#include <vector>
#include <string>
#include <map>
#include <queue>
#include <pthread.h>

#include "FEDException.h"

namespace emu {
	namespace fed {

		class FEDCrate;
		class IRQData;

		class IRQThreadManager {
		
		public:
		
			IRQThreadManager(std::string myEndcap);
			~IRQThreadManager();
			
			void attachCrate(FEDCrate *crate);
			void startThreads(unsigned long int runNumber = 0)
			throw (emu::fed::FMMThreadException);
			void endThreads()
			throw (emu::fed::FMMThreadException);
			
			inline void setEndcap(std::string myEndcap) { endcap_ = myEndcap; }
		
			static void *IRQThread(void *data);
			
			inline IRQData *data() { return data_; }
		
		private:
		
			std::vector< std::pair<FEDCrate *, pthread_t> > threadVector_;
			IRQData *data_;
			std::string endcap_;
		
		};

	}
}

#endif
