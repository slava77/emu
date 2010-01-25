/*****************************************************************************\
* $Id: IRQData.h,v 1.6 2010/01/25 13:45:20 paste Exp $
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

#include "xdaq/WebApplication.h"


/** All of the information we need to know per IRQ error. **/
namespace emu {
	namespace fed {

		class Crate;
		class DDU;

	
		/** The data that are handed off between the IRQ threads and the mother
		*	program.
		**/
		class IRQData {
		
		public:
			IRQData(xdaq::WebApplication *myApplication):
			runNumber(0),
			fmmErrorThreshold(0),
			application(myApplication)
			{
				pthread_mutex_init(&crateQueueMutex, NULL);
			}
		
			~IRQData() {}
			
			/// A queue of crates that each thread can pop.  Mutexed.
			pthread_mutex_t crateQueueMutex;
			std::queue<Crate *> crateQueue;
			
			/// The run number to which this particular set of data is assigned
			unsigned long int runNumber;
			
			/// Threshold number of chambers before releasing the FMM signal
			unsigned int fmmErrorThreshold;
			
			// "Local" variables -- each thread tries to increment only its own.
			/// The number of FMM errors since reset, indexed by crate number
			std::map<unsigned int, unsigned long int> errorCount;
			
			/// The names of the fibers that have had an error since reset, indexed by crate number
			std::map<unsigned int, std::vector<std::string> > errorFiberNames;
			
			/// The last DDU to have reported an error since reset, indexed by crate number
			std::map<unsigned int, unsigned int> lastDDU;
			
			/// The number of "ticks" (IRQ loops) the software has counted since the last reset, indexed by crate number
			std::map<unsigned int, unsigned long int> ticks;
			
			// Changed these to strings because right-shifting a time_t constant in a map sometimes crashes.
			/// The datetime of the last tick, indexed by crate number
			std::map<unsigned int, std::string> tickTime;
			
			/// The datetime of the first tick, indexed by crate number
			std::map<unsigned int, std::string> startTime;
			
			/// The application from which to send SOAP messages
			xdaq::WebApplication *application;
			
		};

	}
}

#endif
