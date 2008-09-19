/*****************************************************************************\
* $Id: IRQData.h,v 3.8 2008/09/19 16:53:51 paste Exp $
*
* $Log: IRQData.h,v $
* Revision 3.8  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.7  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __IRQDATA_H__
#define __IRQDATA_H__

#include <time.h>
#include <queue>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <pthread.h> // For mutexes


/** All of the information we need to know per IRQ error. **/
namespace emu {
	namespace fed {

		class FEDCrate;
		class DDU;

		class IRQError {
		
		public:
		
			IRQError(FEDCrate *myCrate, DDU *myDDU):
				crate(myCrate),
				ddu(myDDU),
				fibers(0),
				errorTime(0),
				errorEvent(0),
				reset(0),
				action("")
			{
				// Store the time when the error was recorded immediately.
				time(&errorTime);

			}
		
			~IRQError() {}
		
			// All public members for ease of access.
			
			FEDCrate *crate;
			DDU *ddu;
			unsigned int fibers;
			time_t errorTime;
			unsigned long int errorEvent;
			unsigned long int reset;
			std::string action;
		
		};
		
		
		/** The data that are handed off between the IRQ threads and the mother
		*	program.
		**/
		class IRQData {
		
		public:
			IRQData(unsigned long int runNumber = 0):
				exit(true),
				runNumber(runNumber)
			{
				pthread_mutex_init(&crateQueueMutex, NULL);
			}
		
			~IRQData() {}
		
			// Handle for the VME Controller this thread talks to.
			//std::map<int,long> Handles;
			
			// A queue of crates that each thread can pop.  Mutexed.
			pthread_mutex_t crateQueueMutex;
			std::queue<FEDCrate *> crateQueue;
			
			// "Global" variables -- all threads read these together.
			bool exit;
			unsigned long int runNumber;
			
			// "Local" variables -- each thread tries to increment only its own.
			std::map<FEDCrate *, unsigned long int> errorCount;
			std::map<FEDCrate *,DDU *> lastDDU;
			std::map<FEDCrate *,unsigned long int> ticks;
			std::map<FEDCrate *,time_t> tickTime;
			std::map<FEDCrate *,time_t> startTime;
		
			std::map<FEDCrate *,std::vector<IRQError *> > errorVectors;
			
		};

	}
}

#endif
