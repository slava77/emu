/*****************************************************************************\
* $Id: IRQThreadManager.h,v 3.7 2008/08/15 08:35:51 paste Exp $
*
* $Log: IRQThreadManager.h,v $
* Revision 3.7  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __IRQTHREADMANAGER_H__
#define __IRQTHREADMANAGER_H__

#include <vector>
#include <map>
#include <queue>
#include <pthread.h>

#include "EmuFEDLoggable.h"

namespace emu {
	namespace fed {

		class FEDCrate;
		class IRQData;

		class IRQThreadManager: public EmuFEDLoggable {
		
		public:
		
			IRQThreadManager();
			~IRQThreadManager();
			
			void attachCrate(FEDCrate *crate);
			void startThreads(unsigned long int runNumber = 0);
			void endThreads();
		
			static void *IRQThread(void *data);
			
			inline IRQData *data() { return data_; }
		
		private:
		
			std::vector< std::pair<FEDCrate *, pthread_t> > threadVector_;
			IRQData *data_;
		
		};

	}
}

#endif
