/*****************************************************************************\
* $Id: IRQData.h,v 1.3 2009/05/21 15:30:48 paste Exp $
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
			exit(true),
			runNumber(0),
			fmmErrorThreshold(0),
			application(myApplication)
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
			
			/// Threshold number of chambers before releasing the FMM signal
			unsigned int fmmErrorThreshold;
			
			// "Local" variables -- each thread tries to increment only its own.
			std::map<unsigned int, unsigned long int> errorCount;
			std::map<unsigned int, unsigned int> lastDDU;
			std::map<unsigned int, unsigned long int> ticks;
			// Changed these to strings because right-shifting a time_t constant
			// in a map sometimes crashes.
			std::map<unsigned int, std::string> tickTime;
			std::map<unsigned int, std::string> startTime;
			
			/// The application from which to send SOAP messages
			xdaq::WebApplication *application;
			
		};

	}
}

#endif
