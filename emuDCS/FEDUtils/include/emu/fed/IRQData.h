/*****************************************************************************\
* $Id: IRQData.h,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: IRQData.h,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.10  2008/10/30 12:56:10  paste
* Fixing more map-related bugs in IRQData
* Changing IRQ FMM threshold to > 8 chambers (from > 1 chamber)
*
* Revision 3.9  2008/10/15 00:46:56  paste
* Attempt to solve certain crashes on Enable/Disable commands.
*
* Revision 3.8  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.7  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMU_FED_IRQDATA_H__
#define __EMU_FED_IRQDATA_H__

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

		class Crate;
		class DDU;

		class IRQError {
		
		public:
		
			IRQError(Crate *myCrate, DDU *myDDU):
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
			
			Crate *crate;
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
			std::queue<Crate *> crateQueue;
			
			// "Global" variables -- all threads read these together.
			bool exit;
			unsigned long int runNumber;
			
			// "Local" variables -- each thread tries to increment only its own.
			std::map<unsigned int, unsigned long int> errorCount;
			std::map<unsigned int,DDU *> lastDDU;
			std::map<unsigned int,unsigned long int> ticks;
			// Changed these to strings because right-shifting a time_t constant
			// in a map sometimes crashes.
			std::map<unsigned int,std::string> tickTime;
			std::map<unsigned int,std::string> startTime;
		
			std::map<unsigned int,std::vector<IRQError *> > errorVectors;
			
		};

	}
}

#endif
