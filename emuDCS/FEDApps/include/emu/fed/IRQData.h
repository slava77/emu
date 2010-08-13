/*****************************************************************************\
* $Id: IRQData.h,v 1.2 2010/08/13 03:00:07 paste Exp $
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
		class Communicator;

		/** The data that are handed off between the IRQ threads and the mother
		*	program.
		**/
		class IRQData {
		
		public:
			IRQData(emu::fed::Communicator *myApplication):
			nCrates(0),
			resetCount(0),
			runNumber(0),
			fmmErrorThreshold(0),
			application(myApplication)
			{
				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
			}
			
			// Copy constructor
			IRQData(const IRQData &other)
			{

				crateQueue = other.crateQueue;
				nCrates = other.nCrates;
				resetCount = other.resetCount;
				errorCount = other.errorCount;
				errorFiberNames = other.errorFiberNames;
				runNumber = other.runNumber;
				fmmErrorThreshold = other.fmmErrorThreshold;
				application = other.application;

				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
			}
			
			// Asignment operator
			IRQData &operator=(const IRQData &other)
			{

				crateQueue = other.crateQueue;
				nCrates = other.nCrates;
				resetCount = other.resetCount;
				errorCount = other.errorCount;
				errorFiberNames = other.errorFiberNames;
				runNumber = other.runNumber;
				fmmErrorThreshold = other.fmmErrorThreshold;
				application = other.application;

				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
				
				return *this;
			}
		
			~IRQData() {
				pthread_mutex_destroy(&crateQueueMutex);
				pthread_mutex_destroy(&resetCountMutex);
				pthread_cond_destroy(&resetCountCondition);
				pthread_mutex_destroy(&errorCountMutex);
				pthread_mutex_destroy(&applicationMutex);
			}
			
			/// A queue of crates that each thread can pop.  Mutexed.
			pthread_mutex_t crateQueueMutex;
			std::queue<Crate *> crateQueue;
			unsigned int nCrates;
			
			/// A count of the number of crates that have reset
			pthread_mutex_t resetCountMutex;
			pthread_cond_t resetCountCondition;
			unsigned int resetCount;
			
			/// A count of the number of CSCs requesting reset
			pthread_mutex_t errorCountMutex;
			/// Indexed by crate number
			std::map<unsigned int, unsigned int> errorCount;
			/// The names of the fibers that have had an error since reset, indexed by crate number
			std::map<unsigned int, std::vector<std::string> > errorFiberNames;
			
			/// A mutex to make sure only one thread at a time is talking to the parent application
			pthread_mutex_t applicationMutex;
			
			/// The run number to which this particular set of data is assigned
			unsigned long int runNumber;
			
			/// Threshold number of chambers before releasing the FMM signal
			unsigned int fmmErrorThreshold;
			
			// "Local" variables -- each thread tries to increment only its own.
			// The number of FMM errors since reset, indexed by crate number
			//std::map<unsigned int, unsigned long int> errorCount;
			
			// The last DDU to have reported an error since reset, indexed by crate number
			//std::map<unsigned int, unsigned int> lastDDU;
			
			// The number of "ticks" (IRQ loops) the software has counted since the last reset, indexed by crate number
			//std::map<unsigned int, uint64_t> ticks;
			
			// Changed these to strings because right-shifting a time_t constant in a map sometimes crashes.
			// The datetime of the last tick, indexed by crate number
			//std::map<unsigned int, std::string> tickTime;
			
			// The datetime of the first tick, indexed by crate number
			//std::map<unsigned int, std::string> startTime;
			
			/// The application from which to send SOAP messages
			emu::fed::Communicator *application;
			
		};

	}
}

#endif
