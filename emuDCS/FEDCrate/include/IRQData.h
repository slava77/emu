/* Global variables for DDU IRQ interrupt */
#ifndef IRQDATA_h
#define IRQDATA_h

#include <time.h>
#include <queue>
#include <map>
#include "Crate.h"

class IRQData {

public:
	IRQData(unsigned long int runNumber = 0):
		exit(1),
		runNumber(runNumber)
	{}

	~IRQData() {}

	// Handle for the VME Controller this thread talks to.
	std::map<int,long> Handles;
	
	// Crate number this thread is in control of.
	std::queue<int> crateNumbers;
	
	// "Global" variables -- all threads read these together.
	int exit;
	unsigned long int runNumber;
	
	// "Local" variables -- each thread tries to only increment its own.
	std::map<int,Crate *> crate;
	std::map<int,int> count;
	std::map<int,int> countFMM;
	std::map<int,int> countSync;
	std::map<int,int> lastDDU;
	std::map<int,unsigned int> ticks;
	std::map<int,time_t> tickTime;
	
	std::map<int,int> lastError[21]; // Per DDU slot
	std::map<int,int> lastFMMStat[21]; // Per DDU slot
	std::map<int,int> accError[21]; // Per DDU slot
	std::map<int,int> dduCount[21]; // Per DDU slot
	std::map<int,time_t> lastErrorTime[21]; // Per DDU slot
	
	std::map<int,int> previousProblem[21];
	
	std::map<int,unsigned short int> lastStatus;
	std::map<int,int> lastErrs[3];
	std::map<int,int> lastCountFMM;
	
	std::map<int,time_t> startTime;
};

#endif
