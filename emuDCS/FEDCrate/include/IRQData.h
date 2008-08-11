/* Global variables for DDU IRQ interrupt */
#ifndef IRQDATA_h
#define IRQDATA_h

#include <time.h>
#include <queue>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <pthread.h> // For mutexes

#include "Crate.h"

/** All of the information we need to know per IRQ error. **/
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
	std::map<Crate *, unsigned long int> errorCount;
	std::map<Crate *,DDU *> lastDDU;
	std::map<Crate *,unsigned long int> ticks;
	std::map<Crate *,time_t> tickTime;
	std::map<Crate *,time_t> startTime;

	std::map<Crate *,std::vector<IRQError *> > errorVectors;
	
};

#endif
