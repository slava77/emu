/*****************************************************************************\
* $Id: IRQData.h,v 1.7 2012/11/27 19:40:06 cvuosalo Exp $
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
			runNumStr("0"),
			fmmErrorThreshold(0),
			waitTimeAfterFMM(5),
			application(myApplication)
			{
				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
				pthread_mutex_init(&lastErrMutex, NULL);
			}
			
			// Copy constructor
			IRQData(const IRQData &other)
			{

				crateQueue = other.crateQueue;
				crateVec = other.crateVec ;
				nCrates = other.nCrates;
				resetCount = other.resetCount;
				errorCount = other.errorCount;
				ignErrCnt= other.ignErrCnt;
				errorFiberNames = other.errorFiberNames;
				errorHistory = other.errorHistory;
				runNumber = other.runNumber;
				runNumStr << other.runNumStr.str();
				fmmErrorThreshold = other.fmmErrorThreshold;
				waitTimeAfterFMM = other.waitTimeAfterFMM;
				application = other.application;

				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
				pthread_mutex_init(&lastErrMutex, NULL);
			}
			
			// Asignment operator
			IRQData &operator=(const IRQData &other)
			{

				crateQueue = other.crateQueue;
				crateVec = other.crateVec ;
				nCrates = other.nCrates;
				resetCount = other.resetCount;
				errorCount = other.errorCount;
				ignErrCnt= other.ignErrCnt;
				errorFiberNames = other.errorFiberNames;
				errorHistory = other.errorHistory;
				runNumber = other.runNumber;
				runNumStr << other.runNumStr.str();
				fmmErrorThreshold = other.fmmErrorThreshold;
				waitTimeAfterFMM = other.waitTimeAfterFMM;
				application = other.application;

				pthread_mutex_init(&crateQueueMutex, NULL);
				pthread_mutex_init(&resetCountMutex, NULL);
				pthread_cond_init(&resetCountCondition, NULL);
				pthread_mutex_init(&errorCountMutex, NULL);
				pthread_mutex_init(&applicationMutex, NULL);
				pthread_mutex_init(&lastErrMutex, NULL);
				
				return *this;
			}
		
			~IRQData() {
				pthread_mutex_destroy(&crateQueueMutex);
				pthread_mutex_destroy(&resetCountMutex);
				pthread_cond_destroy(&resetCountCondition);
				pthread_mutex_destroy(&errorCountMutex);
				pthread_mutex_destroy(&applicationMutex);
				pthread_mutex_destroy(&lastErrMutex);
			}
			
			/// A queue of crates that each thread can pop.  Mutexed.
			pthread_mutex_t crateQueueMutex;
			std::queue<Crate *> crateQueue;
			std::vector<Crate *> crateVec; // Intended as read-only list of crates
			unsigned int nCrates;
			
			/// A count of the number of crates that have reset
			pthread_mutex_t resetCountMutex;
			pthread_cond_t resetCountCondition;
			unsigned int resetCount;
			
			/// A count of the number of CSCs requesting reset
			pthread_mutex_t errorCountMutex;
			/// Indexed by crate number
			std::map<unsigned int, unsigned int> errorCount;
			std::map<unsigned int, unsigned int> ignErrCnt;
			/// The names of the fibers that have had an error since reset, indexed by crate number
			std::map<unsigned int, std::vector<std::string> > errorFiberNames;
			
			enum errSteps {CURR_ERR, FIRST_ERR, SECOND_ERR};

			// List of fibers currently in error, last in error, and previously in error
			typedef std::map<errSteps, unsigned int> fiberHistory;
			typedef std::map<unsigned int, fiberHistory> crateHistory;
			typedef std::map<unsigned int, crateHistory > endcapHistory;
			endcapHistory errorHistory;
			// std::map<unsigned int, std::map<unsigned int, std::map<errSteps, unsigned int> > > errorHistory;


			pthread_mutex_t lastErrMutex;

			/// A mutex to make sure only one thread at a time is talking to the parent application
			pthread_mutex_t applicationMutex;
			
			/// The run number to which this particular set of data is assigned
			unsigned int runNumber;
			std::stringstream runNumStr;
			
			/// Threshold number of chambers before releasing the FMM signal
			unsigned int fmmErrorThreshold;
			
			/// Number of seconds thread should wait after releasing FMMs, default is 5
			unsigned int waitTimeAfterFMM;
			
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
