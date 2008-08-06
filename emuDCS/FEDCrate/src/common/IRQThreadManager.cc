
#include "IRQThreadManager.h"

IRQThreadManager::IRQThreadManager() {
	threadVector_.clear();
}



IRQThreadManager::~IRQThreadManager() {

	//endThreads();

}



void IRQThreadManager::attachCrate(Crate *crate) {

	pthread_t threadID;
	//cout << "IRQThreadManager::attachCrate Attaching crate with number " << crate->number() << endl;
	threadVector_.push_back(pair<Crate *, pthread_t>(crate, threadID));

}




void IRQThreadManager::startThreads(unsigned long int runNumber) {

	//cout << "IRQThreadManager::startThreads Create unique Logger for EmuFEDVME" << endl;

	// Make the shared data object that will be passed between threads and the
	// mother program.
	data_ = new IRQData();

	char datebuf[55];
	//char filebuf[255];
	ostringstream fileName;
	time_t theTime = time(NULL);

	// log file format: EmuFMMThread_Crate#_YYYYMMDD-hhmmss_rRUNNUMBER.log
	strftime(datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", localtime(&theTime));
	fileName << "EmuFMMThread_" << datebuf << "_r" << setw(5) << setfill('0') << dec << runNumber;
	//sprintf(filebuf,"EmuFMMThread_%s_r%05u.log",datebuf,(unsigned int) runNumber);

	log4cplus::SharedAppenderPtr myAppend = new FileAppender(fileName.str().c_str());
	myAppend->setName("EmuFMMIRQAppender");

	//Appender Layout
	std::auto_ptr<Layout> myLayout = std::auto_ptr<Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");
	logger.addAppender(myAppend);

	//cout << "IRQThreadManager::startThreads Clearing shared data" << endl;
	
	data_->runNumber = runNumber;
	// Do not quit the threads immediately.
	data_->exit = 0;

	// First, load up the data_ object with the crates that I govern.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {
		//cout << "IRQThreadManager::startThread Adding and clearing data for crate number " << threadVector_[i].first->number() << endl;

		// At this point, most of the variables in the data_ object have been
		// cleared.
		/*
		int crateNumber = threadVector_[i].first->number();
		data_->crateNumbers.push(crateNumber);
		data_->crate[crateNumber] = threadVector_[i].first;
		data_->Handles[crateNumber] = threadVector_[i].first->vmeController()->theBHandle;
		data_->startTime[crateNumber] = 0;
		data_->count[crateNumber] = 0;
		data_->countFMM[crateNumber] = 0;
		data_->countSync[crateNumber] = 0;
		data_->ticks[crateNumber] = 0;
		data_->tickTime[crateNumber] = 0;

		data_->lastDDU[crateNumber] = 0;
		data_->lastStatus[crateNumber] = 0;
		data_->lastErrs[crateNumber][0] = 0;
		data_->lastErrs[crateNumber][1] = 0;
		data_->lastErrs[crateNumber][2] = 0;
		data_->lastCountFMM[crateNumber] = 0;
		
		for (int j=0; j<21; j++) {
			data_->lastError[crateNumber][j] = 0; // Per DDU slot
			data_->lastFMMStat[crateNumber][j] = 0; // Per DDU slot
			data_->accError[crateNumber][j] = 0; // Per DDU slot
			data_->dduCount[crateNumber][j] = 0; // Per DDU slot
			data_->lastErrorTime[crateNumber][j] = 0; // Per DDU slot
			
			data_->previousProblem[crateNumber][j] = 0;
		}
		*/

		data_->crateQueue.push(threadVector_[iThread].first);
	}

	// Next, execute the threads.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {
		//cout << "IRQThreadManager::startThread Starting thread for crate number " << threadVector_[i].first->number() << endl;

		// Start the thread (as a static function)
		//int error = pthread_create(&(threadVector_[i].second), NULL, IRQThread, data_);
		pthread_create(&(threadVector_[iThread].second), NULL, IRQThread, data_);
		//cout << "IRQThreadManager::startThread pthread launched with status " << error << endl;
	}
}



void IRQThreadManager::endThreads() {
	if (data_->exit == 1) {
		//cout << "IRQThreadManager::endThreads Threads already stopped." << endl << flush;
	} else {
	
		//cout << "IRQThreadManager::endThreads Gracefully killing off all threads." << endl << flush;
		//cout << "<PGK> Before exit=1" << endl << flush;
		data_->exit = 1;
		//cout << "<PGK> After exit=1" << endl << flush;
		
		// We probably do not need to return the status of the threads,
		//  but this may be used later for whatever reason.
		std::vector<int> returnStatus;
		
		// The threads should be stopping now.  Let's join them.
		for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {
			int *retStat = new int(-999); // Dumb default
			int error = pthread_join(threadVector_[iThread].second,(void **) &retStat); // Waits until the thread calls pthread_exit(void *return_status)

			// Note:  retStat points to a pointer of a value that
			//  the pthread returned, while error is the error
			//  status of the join routine itself.  If it is non-
			//  zero, there was a problem.

			if (error) {
				// I don't know what to do.  Log this, maybe?
				cout << "pthread_join iThread " << iThread << " returned " << error << endl;
			} else {
			
				returnStatus.push_back(*((int *) retStat)); // Pointer-fu!
			}
		}
		
		// Not needed.
		//sleep((unsigned int) 6);
		
		//cout << "<PGK> After sleep" << endl << flush;
		delete data_;
	}
}



/// The big one
void *IRQThreadManager::IRQThread(void *data)
{

	// Recast the void pointer as something more useful.
	IRQData *locdata = (IRQData *)data;

	// Grab the crate that I will be working with (and pop off the crate so
	//  that I am the only one working with this particular crate.)
	// Use mutexes to serialize
	pthread_mutex_lock(&(locdata->crateQueueMutex));
	Crate *myCrate = locdata->crateQueue.front();
	locdata->crateQueue.pop();
	pthread_mutex_unlock(&(locdata->crateQueueMutex));

	// We need the handle of the controller we are talking to.
	long int BHandle = myCrate->vmeController()->theBHandle;

	//char buf[300];
	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");

	// Knowing what DDUs we are talking to is useful as well.
	vector<DDU *> dduVector = myCrate->ddus();

	// This is when we started.  Don't know why this screws up sometimes...
	time(&(locdata->startTime[myCrate]));

	// A local tally of what the last error on a given DDU was.
	std::map<DDU *, int> lastError;
	
	// Continue unless someone tells us to stop.
	while (locdata->exit == false) {

		// Increase the ticks.
		locdata->ticks[myCrate]++;

		// Set the time of the last tick.
		time(&(locdata->tickTime[myCrate]));

		// Enable the IRQ and wait for something to happen for 5 seconds...
		CAENVME_IRQEnable(BHandle,0x1);
		int allClear = CAENVME_IRQWait(BHandle,0x1,5000);

		// If allClear is non-zero, then there was not an error.
		// If there was no error, check to see if we were in an error state
		//  before...
		if(allClear && locdata->errorCount[myCrate] > 0) {
			DDU *myDDU = locdata->lastDDU[myCrate];

			// If my status has cleared, then all is cool, right?
			//  Reset all my data.
			if (myDDU->readCSCStat() < lastError[myDDU]) {
				LOG4CPLUS_INFO(logger, "Reset detected on crate " << myCrate->number());
				LOG4CPLUS_ERROR(logger, " ErrorData RESET Detected" << endl);

				// Increment the reset count on all the errors from that crate...
				std::vector<IRQError *> myErrors = locdata->errorVectors[myCrate];
				for (std::vector<IRQError *>::iterator iError = myErrors.begin(); iError != myErrors.end(); iError++) {
					(*iError)->reset++;
				}

				// Reset the total error count and the saved errors.
				locdata->errorCount[myCrate] = 0;
				locdata->lastDDU[myCrate] = NULL;
				lastError.clear();

			}
			
		}
		
		// If there was no error, and there was no previous error (or the
		//  previous error was not cleared), then do nothing.
		else if (allClear) continue;

		// We have an error!

		//CVDataWidth DW=cvD16;
		//CVIRQLevels IRQLevel=cvIRQ1;
		//unsigned int ERR,SYNC,FMM,NUM_ERR,NUM_SYNC,SLOT;

		// Read out the error information into a local variable.
		unsigned char errorData[2] = {
			0,
			0
		};
		CAENVME_IACKCycle(BHandle,cvIRQ1,errorData,cvD16);

		// In which slot did the error occur?  Get the DDU that matches.
		DDU *myDDU = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			if ((*iDDU)->slot() == (errorData[1] & 0x1f)) {
				locdata->lastDDU[myCrate] = (*iDDU);
				myDDU = (*iDDU);
				break;
			}
		}

		// Problem if there is no matching DDU...
		if (myDDU == NULL) {
			// Looks like this happens all the time.  Squelch errors.
			//LOG4CPLUS_FATAL(logger, "IRQ set from an unrecognized slot!  Crate " << myCrate->number() << " slot " << dec << (errorData[1] & 0x1f) << " error data " << hex << setw(2) << setfill('0') << (int) errorData[1] << setw(2) << setfill('0') << (int) errorData[0]);
			continue;
		}

		// Collect the present CSC status and store...
		unsigned int cscStatus = myDDU->readCSCStat();
		unsigned int xorStatus = cscStatus^lastError[myDDU];
		
		// What type of error did I see?
		bool hardError = (errorData[1] & 0x80);
		bool syncError = (errorData[1] & 0x40);

		// If the DDU wants a reset, it will request it (basically an OR of
		//  the two above values.)
		bool resetWanted = (errorData[1] & 0x20);

		// How many CSCs are in an error state on the given DDU?
		unsigned int cscsWithHardError = ((errorData[0] >> 4) & 0x0f);

		// How many CSCs are in a bad sync state on the given DDU?
		unsigned int cscsWithSyncError = (errorData[0] & 0x0f);
		
		// Log everything now.
		LOG4CPLUS_ERROR(logger, "Interrupt detected!");
		time_t theTime = time(NULL);
		LOG4CPLUS_ERROR(logger, " ErrorData " << dec << myCrate->number() << " " << myDDU->slot() << " " << uppercase << hex << setw(4) << setfill('0') << cscStatus << " " << dec << (uintmax_t) theTime);

		ostringstream fiberErrors, chamberErrors;
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (xorStatus & (1<<iFiber)) {
				fiberErrors << iFiber << " ";
				chamberErrors << myDDU->getChamber(iFiber)->name() << " ";
			}
		}
		
		LOG4CPLUS_INFO(logger, "Decoded information follows" << endl
			<< "FEDCrate   : " << myCrate->number() << endl
			<< "Slot       : " << myDDU->slot() << endl
			<< "RUI        : " << myCrate->getRUI(myDDU->slot()) << endl
			<< "DDU error  : " << ((cscStatus & 0x8000) == 0x8000) << endl
			<< "Fibers     : " << fiberErrors.str() << endl
			<< "Chambers   : " << chamberErrors.str() << endl
			<< "Hard Error : " << hardError << endl
			<< "Sync Error : " << syncError << endl
			<< "Wants Reset: " << resetWanted);
		
		LOG4CPLUS_INFO(logger, cscsWithHardError << " CSCs on this DDU have hard errors");
		LOG4CPLUS_INFO(logger, cscsWithSyncError << " CSCs on this DDU have sync errors");
		
		// Record the error in an accessable history of errors.
		lastError[myDDU] = cscStatus;
		IRQError *myError = new IRQError(myCrate, myDDU);
		myError->fibers = xorStatus;
		
		// Log all errors in persisting array...
		// PGK I am not so worried about DDU-only errors...
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			if (xorStatus & (1<<iFiber)) {
				locdata->errorCount[myCrate]++;
			}
		}
		locdata->lastDDU[myCrate] = myDDU;


		// Check to see if any of the fibers are troublesome and report
		std::vector<IRQError *> errorVector = locdata->errorVectors[myCrate];
		unsigned int liveFibers = myDDU->ddu_rdkillfiber();
		//LOG4CPLUS_DEBUG(logger, "Checking for problem fibers in crate " << myCrate->number() << " slot " << myDDU->slot());
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			// Skip it if it is already killed or if it didn't cause a problem
			if (!(liveFibers & (1<<iFiber)) || !(xorStatus & (1<<iFiber))) {
				//LOG4CPLUS_DEBUG(logger, "Fiber is either killed (killFiber " << hex << liveFibers << ") or did not cause a problem (xorStatus " << hex << xorStatus << ")");
				continue;
			}
			// Look through the history of problem fibers and count them
			unsigned long int problemCount = 0;
			for (std::vector<IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
				// Make sure it's the correct DDU
				if ((*iError)->ddu != myDDU) {
					//LOG4CPLUS_DEBUG(logger, "This error had DDU " << (*iError)->ddu << " and mine is " << myDDU);
					continue;
				}
				if ((*iError)->fibers & (1<<iFiber)) {
					//LOG4CPLUS_DEBUG(logger, "Problem detected, error fibers were " << (*iError)->fibers);
					problemCount++;
				}
			}
			// If the threshold has been reached, Warn (no death yet)
			if (problemCount >= 3) {
				LOG4CPLUS_INFO(logger, "Fiber " << iFiber << " in crate " << myCrate->number() << " slot " << myDDU->slot() << " (RUI " << myCrate->getRUI(myDDU->slot()) << ", chamber " << myDDU->getChamber(iFiber)->name() << ") has set an error " << problemCount << " times.  Please check this chamber for harware problems.");
				// Forgot this last time...  oops.
				//liveFibers &= ~(1<<iFiber); // Bit-foo!
				//myDDU->ddu_loadkillfiber(liveFibers);
				// Record the action taken.
				ostringstream actionTaken;
				actionTaken << "Fiber " << iFiber << " (" << myDDU->getChamber(iFiber)->name() << ") has had " << problemCount << " errors since the last hard reset.  Check for hardware problems. ";
				myError->action += actionTaken.str();
			}
		}
		
		// Discover the error counts of the other crates.
		unsigned long int totalChamberErrors = 0;
		for (std::map<Crate *, unsigned long int>::iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); iCount++) {
			if (iCount->first != myCrate) {
				LOG4CPLUS_INFO(logger,"Crate " << iCount->first->number() << " reports " << iCount->second << " CSCs in an error state.");
			}
			totalChamberErrors += iCount->second;
		}
		
		// Check if we have sufficient error conditions to reset.
		if (totalChamberErrors > 2) {
			LOG4CPLUS_INFO(logger, "A resync will be requested because the total number of CSCs in an error state on this endcap is " << totalChamberErrors);
			// Make a note of it in the error log.
			ostringstream actionTaken;
			actionTaken << "A resync has been requested for this endcap. ";
			myError->action += actionTaken.str();
			// Loop over the crates and take away SFTU
			for (std::map<Crate *, unsigned long int>::iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); iCount++) {
				// Find the broadcast slot on this crate.
				std::vector<DDU *> myDDUs = iCount->first->ddus();
				for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
					if ((*iDDU)->slot() > 21) (*iDDU)->vmepara_wr_fmmreg(0xFED8);
				}
			}
		}

		// Save the error.
		locdata->errorVectors[myCrate].push_back(myError);
	}

	//cout << " IRQ_Int call pthread_exit" << endl;
	int returnValue = 0; // This may be more useful later.  I don't know.
	pthread_exit((void *) &returnValue);
}



