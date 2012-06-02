/*****************************************************************************\
* $Id: IRQThreadManager.cc,v 1.15 2012/06/02 19:05:41 cvuosalo Exp $
\*****************************************************************************/
#include "emu/fed/IRQThreadManager.h"

#include <utility>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <bitset>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>

#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/Facts.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "emu/fed/Communicator.h"



emu::fed::IRQThreadManager::IRQThreadManager(emu::fed::Communicator *application, const unsigned int &fmmErrorThreshold):
systemName_(""),
fmmErrorThreshold_(fmmErrorThreshold),
application_(application)
{
	data_ = new IRQData(application_);
	threadVector_.clear();
}



emu::fed::IRQThreadManager::~IRQThreadManager()
{
	try {
		endThreads();
	} catch (...) {
		// I don't care if this doesn't work.  Zombies will all die eventually, even if they leak a little memory (and brains).
	}
	delete data_;
}



void emu::fed::IRQThreadManager::attachCrate(Crate *crate)
{

	pthread_t threadID = 0;
	threadVector_.push_back(std::pair<Crate *, pthread_t>(crate, threadID));

}



void emu::fed::IRQThreadManager::startThreads(const unsigned long int &runNumber)
throw (emu::fed::exception::FMMThreadException)
{

	// Make the shared data object that will be passed between threads and the
	// mother program.
	data_ = new IRQData(application_);
	data_->fmmErrorThreshold = fmmErrorThreshold_;

	char datebuf[32];
	std::stringstream fileName;
	time_t theTime = time(NULL);

	// log file format: EmuFMMThread_(EndcapName_)YYYYMMDD-hhmmss_rRUNNUMBER.log
	strftime(datebuf, sizeof(datebuf), "%Y-%m-%d-%H-%M-%S", localtime(&theTime));
	fileName << "/tmp/EmuFMMThread-" << (systemName_ != "" ? systemName_ + "-" : "") << datebuf << "-r" << std::setw(6) << std::setfill('0') << std::dec << runNumber << ".log";

	log4cplus::SharedAppenderPtr myAppend(new log4cplus::FileAppender(fileName.str().c_str()));
	myAppend->setName("EmuFMMIRQAppender");

	//Appender Layout
	std::auto_ptr<log4cplus::Layout> myLayout = std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout(myLayout);

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");
	logger.addAppender(myAppend);

	data_->runNumber = runNumber;
	// Do not quit the threads immediately.
	//data_->exit = false;

	// First, load up the data_ object with the crates that I govern.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); ++iThread) {
		data_->crateQueue.push(threadVector_[iThread].first);
		data_->nCrates++;
	}

	// Next, execute the threads.

	// Check the crates to see if any of the DDUs are in an error state and
	//  make a note in the log.  This is because DDUs that are already throwing
	//  errors will not set interrupts, so we want to notify the user somehow
	//  about the possible problem.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); ++iThread) {

		emu::fed::Crate *myCrate = threadVector_[iThread].first;
		std::vector<emu::fed::DDU *> dduVector = myCrate->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			unsigned int cscStatus = 0;
			try {
				cscStatus = (*iDDU)->readFiberErrors();
				MY_REVOKE_ALARM("IRQThreadStartup");
			} catch (emu::fed::exception::DDUException &e) {
				std::ostringstream error;
				error << "Exception in communicating with DDU in crate " << myCrate->number() << ", slot " << (*iDDU)->slot();
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				tag << "FEDcrate " << myCrate->number();
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadStartupCommunication", "ERROR", error.str(), tag.str(), e);
				XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				e2.setProperty("tag", tag.str());
				throw e2;
			}
			if (cscStatus) {
				for (unsigned int iFiber = 0; iFiber <= 15; ++iFiber) {
					if (cscStatus & (1 << iFiber)) {

						std::ostringstream tag;
						std::string chamberName;
						if (iFiber == 15) {
							chamberName = "DDU";
							tag << "RUI " << (*iDDU)->getRUI() << " FEDCrate " << myCrate->number();

						} else {
							chamberName = (iFiber == 15) ? "DDU" : (*iDDU)->getFiber(iFiber)->getName();

							tag << "fiber " << iFiber << "RUI " << (*iDDU)->getRUI() << " FEDCrate " << myCrate->number() << " chamber " << chamberName;
						}

						std::ostringstream error;
						error << "Crate " << myCrate->number() << " Slot " << (*iDDU)->slot() << " shows error in " << chamberName << " before the threads actually started.  This may not show up as an interrupt!";
						LOG4CPLUS_WARN(logger, error.str());

						MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadStartupCondition", "WARN", error.str(), tag.str());
					}
				}
			} else {

				MY_REVOKE_ALARM("IRQThreadStartupCondition");

			}
		}

		int err = pthread_create(&(threadVector_[iThread].second), NULL, IRQThread, data_);

		if (err) {
			std::ostringstream error;
			error << "Exception in starting IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << myCrate->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadStart", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());
			throw e2;
		} else {
			MY_REVOKE_ALARM("IRQThreadStart");
			LOG4CPLUS_INFO(logger, "Started thread: " << threadVector_[iThread].second);
		}
	}
}



void emu::fed::IRQThreadManager::endThreads()
throw (emu::fed::exception::FMMThreadException)
{

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");

	if (threadVector_.size() == 0) {
		LOG4CPLUS_DEBUG(logger, "Threads already stopped.");
		return;
	}
	LOG4CPLUS_INFO(logger, "Gracefully killing off all threads: " << threadVector_.size() );

	// We probably do not need to return the status of the threads,
	//  but this may be used later for whatever reason.
	//std::vector<int> returnStatus;

	for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {

		Crate *myCrate = threadVector_[iThread].first;
		LOG4CPLUS_DEBUG(logger, "Killing thread: " << threadVector_[iThread].second << ", " << myCrate );

		// Cancel threads first.
		int err = pthread_cancel(threadVector_[iThread].second);

		// err == ESRCH means the thread already exited on its own, so there was no need to cancel it.
		if (err == ESRCH) {
			LOG4CPLUS_WARN(logger, "IRQThread " << iThread << " for crate " << myCrate->number() <<
				" exited prematurely before it could be canceled");
		} else if (err != 0) {
			std::ostringstream error;
			error << "Exception cancelling IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << myCrate->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadEnd", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());

			LOG4CPLUS_INFO(logger, "Removing appender with exception");
			logger.removeAllAppenders();
			throw e2;
		}

		// The threads should be stopping now.  Let's join them.
		void *tempException = NULL; // I have to do this for type safety.
		err = pthread_join(threadVector_[iThread].second, &tempException);
			// Waits until the thread calls pthread_exit(void *return_status)

		// tempException is not used right now -- it doesn't provide additional info.

		if (err) {
			std::ostringstream error;
			error << "Exception joining IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << myCrate->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadEnd", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());

			LOG4CPLUS_INFO(logger, "Removing appender with exception");
			logger.removeAllAppenders();
			throw e2;
		}

		LOG4CPLUS_INFO(logger, "Joined IRQThread " << iThread << " for crate " << myCrate->number());

	}

	MY_REVOKE_ALARM("IRQThreadEnd");

	threadVector_.clear();

	LOG4CPLUS_INFO(logger, "Removing appender");
	logger.removeAllAppenders();
}

static std::string mkBitStr(uint16_t bits)
{
	std::bitset<16> setOBits(bits);
	return (setOBits.to_string<char, char_traits<char>, allocator<char> >());
}


static void checkDDUStatus(std::vector<emu::fed::DDU *> &dduVector, log4cplus::Logger &logger)
{
	static long int index = 1, delay = 1;
	bool statRep = false;
	std::ostringstream statusMsg, busyFibers, warnFibers, warnNowFibers, errFibers, oosFibers;
	statusMsg <<     "DDU statuses            ";
	busyFibers << 	 "Fibers that had Busy    ";
	warnFibers << 	 "Fibers that had Warning ";
	warnNowFibers << "Fibers now in Warning   ";
	errFibers <<     "Fibers now in Error     ";
	oosFibers <<     "Fibers now in OOS       ";
	for (std::vector<emu::fed::DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		emu::fed::DDU *myDDU = (*iDDU);
		if (myDDU != NULL) {
			uint16_t ruiNum = myDDU->getRUI();
			uint8_t fmmStat = myDDU->readRealFMM();
			std::pair<std::string, std::string> fmmStrStat = emu::fed::DDUDebugger::RealFMM(fmmStat);
			// uint16_t paraStat = myDDU->readParallelStatus();
			statusMsg << std::setw(3) << ruiNum << " " << fmmStrStat.first << "        ";
			// statusMsg << std::hex << paraStat << " ";
			uint16_t busyStat = myDDU->readBusyHistory();
			uint16_t warnHStat = myDDU->readWarningHistory();
			uint16_t warnNStat = myDDU->readFMMFullWarning();
			uint16_t errStat = myDDU->readFMMError();
			uint16_t oosStat = myDDU->readFMMLostSync();
			if (statRep == false)
				statRep = (busyStat | warnHStat | warnNStat | errStat | oosStat);
			busyFibers << mkBitStr(busyStat) << " ";
			warnFibers << mkBitStr(warnHStat) << " ";
			warnNowFibers << mkBitStr(warnNStat) << " ";
			errFibers << mkBitStr(errStat) << " ";
			oosFibers << mkBitStr(oosStat) << " ";
			/*
			busyFibers << mkBitStr(myDDU->readBusyHistory()) << " ";
			warnFibers << mkBitStr(myDDU->readWarningHistory()) << " ";
			warnNowFibers << mkBitStr(myDDU->readFullWarning()) << " ";
			errFibers << mkBitStr(myDDU->readFMMError()) << " ";
			oosFibers << mkBitStr(myDDU->readFMMLostSync()) << " ";
			*/
		}
	}
	if (statRep && index++ >= delay) {
		if (delay > 500) {
			delay = 1;
			index = 1;
		} else delay *= 2;
		LOG4CPLUS_DEBUG(logger, endl << statusMsg.str() << endl << warnNowFibers.str()
			<< endl << warnFibers.str() << endl
			<< busyFibers.str() << endl << oosFibers.str() << endl << errFibers.str() << endl);
	}
}


/// The big one
void *emu::fed::IRQThreadManager::IRQThread(void *data)
{
	
	// Recast the void pointer as something more useful.
	IRQData *locdata = (IRQData *) data;

	// Make sure we have an application_ variable for the macros.  Static functions cannot access class members.
	emu::fed::Communicator *application_ = locdata->application;

	// Grab the crate that I will be working with (and pop off the crate so
	//  that I am the only one working with this particular crate.)
	// Use mutexes to serialize
	pthread_mutex_lock(&(locdata->crateQueueMutex));
	Crate *myCrate = locdata->crateQueue.front();
	locdata->crateQueue.pop();
	pthread_mutex_unlock(&(locdata->crateQueueMutex));

	unsigned int crateNumber = myCrate->getNumber();

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");

	// Knowing what boards we are talking to is useful as well.
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	std::vector<DCC *> dccVector = myCrate->getDCCs();

	// This is when we started.  Don't know why this screws up sometimes...
	//time_t tock;
	//time(&tock);
	//tm *tockInfo = localtime(&tock);
	//std::string tockText(asctime(tockInfo));
	//locdata->startTime[crateNumber] = tockText;

	// A local tally of what the last CSC status error on a given DDU was.
	std::map<unsigned int, unsigned int> lastDDUError;
	
	// The last DDU slot to report an IRQ
	unsigned int lastDDU = 0;
	
	// The L1A at the last IRQ (approximate)
	unsigned int lastL1A = 0;
	
	// A local count of the number or errors for this crate, indexed by slot
	std::map<unsigned int, unsigned int> nErrors;

	// A local list of the fibers in error for this crate, indexed by slot
	std::map<unsigned int, std::vector<std::string> > errFibersBySlot;
	
	// Whether or not the debugging trap has been sprung on a given DDU
	std::map<unsigned int, bool> trapSprung;

	// A local tally of what the last FMM error on a given DCC was.
	std::map<unsigned int, unsigned int> lastDCCError;

	// A local tally of what the last FIFO error on a given DCC was.
	std::map<unsigned int, unsigned int> lastFIFOError;

	// A local tally of what the last SLink error on a given DCC was.
	std::map<unsigned int, unsigned int> lastSLinkError;

	// Continue unless someone tells us to stop.
	while (1) { // Always looping until the thread is canceled

		// New plan of attack:
		// Reset everything and wait for all sister threads to reset
		// Check every second for an interrupt
		// If there is an interrupt, handle it
		// Otherwise, do misc checks
		//  check for reset
		//  check TF?
		//  check DCCs
		// (if at any point a reset is detected, immediately restart the loop)

		// Immediate check for cancel
		pthread_testcancel();
		
		// Clear the stored number of errors
		unsigned int totalErrors;
		for (std::map<unsigned int, unsigned int>::const_iterator iError = nErrors.begin(); iError != nErrors.end(); ++iError) {
			totalErrors += iError->second;
		}
		
		pthread_mutex_lock(&locdata->errorCountMutex);
		locdata->errorCount[crateNumber] = 0;
		pthread_mutex_unlock(&locdata->errorCountMutex);
		
		nErrors.clear();
		errFibersBySlot.clear();
		trapSprung.clear();
		lastDDUError.clear();
		lastDDU = 0;
		lastL1A = 0;
		
		// Tell the DDUs on this crate to stop broadcasting FMM errors
		if (!myCrate->isTrackFinder()) myCrate->getBroadcastDDU()->disableFMM();

		// Begin by clearing all the Sentinel alarms that might have popped up in past runs.  If there is a problem, they will be caught again.
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			// This resets the last set of CSC errors in the DDU
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				std::ostringstream alarmName;
				alarmName << "IRQThreadFiber" << crateNumber << "_" << (*iDDU)->getSlot() << "_" << iFiber;
				MY_REVOKE_ALARM(alarmName.str());
			}
		}

		// Clear all DCC errors in the same fashion
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
			lastDCCError[(*iDCC)->getSlot()] = 0x8; // This is FMM for READY
			lastFIFOError[(*iDCC)->getSlot()] = 0xff; // No FIFO errors FIXME
			lastSLinkError[(*iDCC)->getSlot()] = 0x0; // No SLink problems FIXME
			std::ostringstream alarmName;
			alarmName << "IRQThreadDCCFMM" << crateNumber << "_" << (*iDCC)->getSlot();
			MY_REVOKE_ALARM(alarmName.str());
			std::vector<FIFO *> fifoVector = (*iDCC)->getFIFOs();
			for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); iFIFO++) {
				std::ostringstream alarmName;
				alarmName << "IRQThreadDCCFIFO" << crateNumber << "_" << (*iDCC)->slot() << "_" << (*iFIFO)->getNumber();
				MY_REVOKE_ALARM(alarmName.str());
			}
			for (unsigned int iLink = 0; iLink <= 1; iLink++) {
				std::ostringstream alarmName;
				alarmName << "IRQThreadDCCSlink" << crateNumber << "_" << (*iDCC)->slot() << "_" << iLink;
				MY_REVOKE_ALARM(alarmName.str());
			}
		}
		
		// Now wait until the other threads have also reset
		pthread_mutex_lock(&locdata->resetCountMutex);
		
		// Increment the number of crates that have reset
		locdata->resetCount++;
		
		if (locdata->nCrates > 1) { // No need to wait if there is only one thread!
			// If this is the number of crates in existence, signal everybody else to continue
			if (locdata->resetCount >= locdata->nCrates) {
				LOG4CPLUS_INFO(logger, "Reset count has reached " << locdata->resetCount << "--unlocking threads");
				pthread_cond_broadcast(&locdata->resetCountCondition);
			}
			
			// Otherwise, wait until you are signaled by someone else!
			else {
				unsigned int remaining = locdata->nCrates - locdata->resetCount;
				LOG4CPLUS_INFO(logger, "Thread waiting on " << remaining << " crate" << (remaining == 1 ? "" : "s") << " to reset...");
				pthread_cond_wait(&locdata->resetCountCondition, &locdata->resetCountMutex);
			}
		}
		
		// Decrement the number of crates that have reset as long as I have the lock
		locdata->resetCount = 0;
		
		pthread_mutex_unlock(&locdata->resetCountMutex);
		
		// Now I start the inner loop that waits on IRQs.
		// Break out of this loop to reset the thread.
		while (1) {

			// Increase the ticks.
			//locdata->ticks[crateNumber]++;

			// Set the time of the last tick.
			//time_t tick;
			//time(&tick);
			//tm *tickInfo = localtime(&tick);
			//std::string tickText(asctime(tickInfo));
			//locdata->tickTime[crateNumber] = tickText;

			try {

				// I know this means the TF DDU will use interrupts, but we'll deal with that hurdle when we come to it.
				if (myCrate->getController()->waitIRQ(1000) == false) {
					pthread_testcancel();

					// INTERRUPT!
					// Read out the error information into a local variable.
					uint16_t errorData = myCrate->getController()->readIRQ();
					LOG4CPLUS_DEBUG(logger, "Successful readIRQ: " << errorData);

					// In which slot did the error occur?  Get the DDU that matches.
					DDU *myDDU = NULL;
					unsigned int slot = ((errorData & 0x1f00) >> 8);
					for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
						if ((*iDDU)->slot() == slot) {
							lastDDU = (*iDDU)->slot();
							myDDU = (*iDDU);
							break;
						}
					}
				
					// If there is no matching DDU, then ignore
					if (myDDU == NULL) {
						LOG4CPLUS_WARN(logger, "IRQ detected in crate " << crateNumber << " with unknown slot ("<< slot << ").  Ignoring.");
						continue;
					}
				
					unsigned int cscStatus = myDDU->readCSCStatus();
					unsigned int advStatus = myDDU->readAdvancedFiberErrors(); // Technically this is the only thing I should be reading (so sayeth Jason)
					unsigned int combinedStatus = cscStatus | advStatus;
					unsigned int ignBits = 0;
					for (unsigned int iFiber = 0; iFiber < 15; ++iFiber) {
						if ( myDDU->getFiber(iFiber)->ignoreErr()) {
							ignBits = ignBits | (1<<iFiber);
							if (combinedStatus & (1<<iFiber))
								LOG4CPLUS_DEBUG(logger, "Chamber in error but set to be ignored: " << myDDU->getFiber(iFiber)->getName());
						}
					}
					if (ignBits != 0) {
						ignBits = ~ignBits;  // Complement so ignored bits are 0.
						combinedStatus = combinedStatus & ignBits;
						// Turn off bits in combinedStatus corresponding to fibers set to
						// be ignored.
					}
					unsigned int xorStatus = combinedStatus^lastDDUError[myDDU->slot()];
					
					// Emergency check to see if a reset has occurred!
					// This checks if any bits have been turned off.  Example:
					/*
					 *  1100
					 *^ 1010
					 *  ----
					 *  0110
					 *&~1010 (= 0101)
					 *  ----
					 *  0100
					 */
					bool doReset = (xorStatus & (~combinedStatus));
					
					// The number of bits set high in combinedStatus tells me the number of CSCs requesting a resync.  Count them.
					std::bitset<16> statusBits(combinedStatus);
					std::string statusBitString = statusBits.to_string<char, char_traits<char>, allocator<char> >();
					unsigned int numBits = statusBits.count();
					if (combinedStatus >= 32768 && numBits > 0)
						--numBits;	// Bit 15 (top bit) is meaningless, so if it's set, ignore it.
					nErrors[slot] = numBits;
					
					// Sometimes an interrupt does not have any errors to report.  Ignore these.
					if (!xorStatus) {
						std::ostringstream logMsg, debugMsg;
						logMsg << "IRQ detected on crate " << crateNumber << " slot " << slot << " but there are no ";
						if (combinedStatus != 0) {
							logMsg << "new ";
							debugMsg << "Bits for repeated chamber errors being ignored -- ADV Status: " << statusBitString;
						}
						logMsg << "errors to report. Ignoring.";
						LOG4CPLUS_INFO(logger, logMsg.str());
						if (combinedStatus != 0)
							LOG4CPLUS_DEBUG(logger, debugMsg.str());
						// LOG4CPLUS_WARN(logger, "IRQ detected on crate " << crateNumber << " slot " << slot << " with no new errors.  Ignoring.");
						continue;
					}
					
					uint32_t l1a = myDDU->readL1Scaler(DDUFPGA); // for debugging purposes
					
					// What type of error did I see?
					bool hardError = (errorData & 0x8000);
					bool syncError = (errorData & 0x4000);
					
					// If the DDU wants a reset, it will request it (basically an OR of the two above values.)
					bool resetWanted = (errorData & 0x2000);
					
					// How many CSCs are in an error state on the given DDU?
					//unsigned int cscsWithHardError = ((errorData >> 4) & 0x000f);
					
					// How many CSCs are in a bad sync state on the given DDU?
					//unsigned int cscsWithSyncError = (errorData & 0x000f);
					
					// Log everything now.
					LOG4CPLUS_ERROR(logger, "Interrupt detected!  (L1A " << l1a << ")");
					//time_t theTime = time(NULL);
					//LOG4CPLUS_ERROR(logger, " ErrorData " << std::dec << crateNumber << " " << myDDU->slot() << " " << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << cscStatus << " " << std::dec << (uintmax_t) theTime);
					
					std::bitset<16> xorBits(xorStatus);
					
					std::stringstream fiberErrors, chamberErrors;
					for (unsigned int iFiber = 0; iFiber < 15; ++iFiber) {
						if (xorBits[iFiber]) {
							fiberErrors << iFiber << " ";
							chamberErrors << myDDU->getFiber(iFiber)->getName() << " ";
						}
					}
					
					std::bitset<16> cscBits(cscStatus);
					
					std::string cscBitString = cscBits.to_string<char, char_traits<char>, allocator<char> >();
					std::string xorBitString = xorBits.to_string<char, char_traits<char>, allocator<char> >();
					
					LOG4CPLUS_INFO(logger, "Decoded information follows" << std::endl
						<< "FEDCrate   : " << crateNumber << std::endl
						<< "Slot       : " << myDDU->getSlot() << std::endl
						<< "RUI        : " << myDDU->getRUI() << std::endl
						<< "CSC Status : " << cscBitString << std::endl
						<< "ADV Status : " << statusBitString << std::endl
						<< "XOR Status : " << xorBitString << std::endl
						<< "DDU error  : " << ((cscStatus & 0x8000) == 0x8000) << std::endl
						<< "Fibers     : " << fiberErrors.str() << std::endl
						<< "Chambers   : " << chamberErrors.str() << std::endl
						<< "Hard Error : " << hardError << std::endl
						<< "Sync Error : " << syncError << std::endl
						<< "Wants Reset: " << resetWanted);
					
					//LOG4CPLUS_INFO(logger, cscsWithHardError << " CSCs on this DDU have hard errors");
					//LOG4CPLUS_INFO(logger, cscsWithSyncError << " CSCs on this DDU have sync errors");
					
					// These only show up if there has been a hard error.
					std::vector<uint16_t> ddufpgaTrap; 
					std::vector<uint16_t> infpga0Trap; 
					std::vector<uint16_t> infpga1Trap; 
					
					std::ostringstream ddufpgaStream;
					std::ostringstream infpga0Stream;
					std::ostringstream infpga1Stream;
					
					if (hardError && !trapSprung[slot]) {
						
						ddufpgaTrap = myDDU->readDebugTrap(DDUFPGA);
						infpga0Trap = myDDU->readDebugTrap(INFPGA0);
						infpga1Trap = myDDU->readDebugTrap(INFPGA1);
						/*
						for (unsigned int iBits = 0; iBits < 12; iBits++) {
							ddufpgaStream << std::setw(4) << std::setfill('0') << std::hex << ddufpgaTrap[11 - iBits];
						}
						for (unsigned int iBits = 0; iBits < 12; iBits++) {
							infpga0Stream << std::setw(4) << std::setfill('0') << std::hex << infpga0Trap[11 - iBits];
						}
						for (unsigned int iBits = 0; iBits < 12; iBits++) {
							infpga1Stream << std::setw(4) << std::setfill('0') << std::hex << infpga1Trap[11 - iBits];
						}
						*/
						std::vector<std::string> trapInfo = DDUDebugger::DDUDebugTrap(ddufpgaTrap, myDDU);
						for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
							ddufpgaStream << (*iTrap) << std::endl;
						}
						
						LOG4CPLUS_INFO(logger, "Logging DDUFPGA diagnostic trap information:" << std::endl << ddufpgaStream.str());
						
						trapInfo = DDUDebugger::INFPGADebugTrap(INFPGA0, infpga0Trap);
						for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
							infpga0Stream << (*iTrap) << std::endl;
						}
						
						LOG4CPLUS_INFO(logger, "Logging INFPGA0 diagnostic trap information:" << std::endl << infpga0Stream.str());
						
						trapInfo = DDUDebugger::INFPGADebugTrap(INFPGA1, infpga1Trap);
						for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
							infpga1Stream << (*iTrap) << std::endl;
						}
						
						LOG4CPLUS_INFO(logger, "Logging INFPGA1 diagnostic trap information:" << std::endl << infpga1Stream.str());
						
						trapSprung[slot] = true;
						
					} else if (!hardError) {
						LOG4CPLUS_INFO(logger, "No hard error reported.  Diagnostic trap information unavailable.");
						ddufpgaStream << "Diagnostic trap information unavailable";
						infpga0Stream << "Diagnostic trap information unavailable";
						infpga1Stream << "Diagnostic trap information unavailable";
					} else {
						LOG4CPLUS_INFO(logger, "Diagnostic trap has already been sprung on this DDU.  No new diagnostic information available until next reset.");
						ddufpgaStream << "Diagnostic trap information unavailable";
						infpga0Stream << "Diagnostic trap information unavailable";
						infpga1Stream << "Diagnostic trap information unavailable";
					}

					// Make and send the fact to the expert system
					emu::base::TypedFact<emu::fed::DDUFMMIRQFact> fact;
					std::ostringstream component;
					component << "DDU" << myDDU->getRUI();
					fact.setComponentId(component.str())
						.setSeverity(emu::base::Fact::WARN)
						.setDescription("DDU FMM IRQ information")
						.setParameter(emu::fed::DDUFMMIRQFact::crateNumber, crateNumber)
						.setParameter(emu::fed::DDUFMMIRQFact::slotNumber, myDDU->getSlot())
						/*.setParameter(emu::fed::DDUFMMIRQFact::rui, myDDU->getRUI())*/
						.setParameter(emu::fed::DDUFMMIRQFact::hardError, hardError)
						.setParameter(emu::fed::DDUFMMIRQFact::syncError, syncError)
						.setParameter(emu::fed::DDUFMMIRQFact::resetRequested, resetWanted)
						.setParameter(emu::fed::DDUFMMIRQFact::fiberStatus, cscStatus)
						.setParameter(emu::fed::DDUFMMIRQFact::advancedFiberStatus, advStatus)
						.setParameter(emu::fed::DDUFMMIRQFact::xorStatus, xorStatus)
						.setParameter(emu::fed::DDUFMMIRQFact::ddufpgaDebugTrap, ddufpgaStream.str())
						.setParameter(emu::fed::DDUFMMIRQFact::infpga0DebugTrap, infpga0Stream.str())
						.setParameter(emu::fed::DDUFMMIRQFact::infpga1DebugTrap, infpga1Stream.str());
						
					pthread_mutex_lock(&(locdata->applicationMutex));
					application_->storeFact(fact);
					application_->sendFacts();
					pthread_mutex_unlock(&(locdata->applicationMutex));
					
					// Record the error in an accessable history of errors.
					lastDDUError[myDDU->slot()] = combinedStatus;
					
					// Just in case there is some bizarre error at this point that causes this to
					// overflow...
					/*
					if (locdata->errorCount[crateNumber] > 15) {
						locdata->errorCount[crateNumber] = 15;
					}
					*/
					lastDDU = slot;
					lastL1A = l1a;
					
					
					// Report the names of the bad fibers to hotspot, but ignore killed fibers
					unsigned int liveFibers = myDDU->readKillFiber();
					//LOG4CPLUS_DEBUG(logger, "Checking for problem fibers in crate " << myCrate->number() << " slot " << myDDU->slot());
					for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
						std::ostringstream alarmName;
						alarmName << "IRQThreadFiber" << crateNumber << "_" << myDDU->slot() << "_" << iFiber;
						// Skip it if it is already killed or if it didn't cause a problem
						// or if it's set to be ignored
						if (!(liveFibers & (1<<iFiber)) || !(xorStatus & (1<<iFiber)) ||
								myDDU->getFiber(iFiber)->ignoreErr()) {
							MY_REVOKE_ALARM(alarmName.str());
							continue;
						} else { // REPORT TO SENTENEL!
							std::ostringstream error;
							error << "Fiber error read on crate " << crateNumber << " slot " << myDDU->slot() << " fiber " << iFiber << " (" << myDDU->getFiber(iFiber)->getName() << ")";
							LOG4CPLUS_ERROR(logger, error.str());
							std::ostringstream tag;
							tag << "FEDcrate " << crateNumber << " RUI " << myDDU->getRUI() << " fiber " << std::setw(2) << std::setfill('0') << iFiber << " chamber " << myDDU->getFiber(iFiber)->getName();
							MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), "ERROR", error.str(), tag.str());
						}
					}
					
					
					// Discover the error counts from this thread.
					pthread_mutex_lock(&locdata->errorCountMutex);
					
					// Clear out list that contains previous chambers in error for crate.
					locdata->errorFiberNames[crateNumber].clear();

					// Clear out list that contains previous chambers in error for slot.
					errFibersBySlot[slot].clear();

					// Get list of chamber errors for this slot.
					for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
						if (statusBits[iFiber]) {
							errFibersBySlot[slot].push_back(myDDU->getFiber(iFiber)->getName());
							// locdata->errorFiberNames[crateNumber].push_back(myDDU->getFiber(iFiber)->getName());
						}
					}
					// Copy error fibers from slot array to external crate list.
					// This way the bad fibers for other slots are restored to the
					// external list, while the bad fibers for the current slot are reset
					// to the current fibers in error.
					for (std::map<unsigned int, std::vector<std::string> >::iterator
							iCount = errFibersBySlot.begin(); iCount != errFibersBySlot.end();
							++iCount) {
						for (std::vector<std::string>::const_iterator iName =
								iCount->second.begin(); iName != iCount->second.end();
								iName++) {
							locdata->errorFiberNames[crateNumber].push_back(*iName);
						}
					}
					
					locdata->errorCount[crateNumber] = 0;
					for (std::map<unsigned int, unsigned int>::iterator iCount = nErrors.begin(); iCount != nErrors.end(); ++iCount) {
						
						locdata->errorCount[crateNumber] += iCount->second;
						
					}
					
					// Now count all the errors from all the threads
					unsigned int totalErrors = 0;
					for (std::map<unsigned int, unsigned int>::const_iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); ++iCount) {
						totalErrors += iCount->second;
					}

					if (totalErrors >= locdata->fmmErrorThreshold) {
						LOG4CPLUS_INFO(logger,
							"FMMs will be released because the total number of CSCs in an error state on this system greater than " <<
							locdata->fmmErrorThreshold);
						if (!myCrate->isTrackFinder()) myCrate->getBroadcastDDU()->enableFMM();
						// Briefly release the FMMs.
					}
					
					pthread_mutex_unlock(&locdata->errorCountMutex);
					
					MY_REVOKE_ALARM("IRQThreadGeneralError");
					
					pthread_testcancel();
					
					// Emergency break if we think a reset has happened under our noses.
					if (doReset || locdata->resetCount) break;

				} else {
					pthread_testcancel();

					// ALL CLEAR!

					// Check for reset only if there was a previous error
					unsigned int totalChamberErrors = 0;
					for (std::map<unsigned int, unsigned int>::iterator iCount = nErrors.begin(); iCount != nErrors.end(); ++iCount) {
						
						totalChamberErrors += iCount->second;
						
					}
					
					if (totalChamberErrors) {

						// Find the last DDU to have an error
						DDU *myDDU = NULL;
						for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
							if ((*iDDU)->slot() == lastDDU) {
								myDDU = (*iDDU);
								break;
							}
						}

						if (myDDU != NULL) {

							// Check if the last DDU's error is now different from what it was before
							if (myDDU->readFiberErrors() < lastDDUError[lastDDU]) {
								LOG4CPLUS_INFO(logger, "Resync detected on crate " << crateNumber << ": checking again to make sure...");

								usleep(100);
								pthread_testcancel();

								if (myDDU->readFiberErrors() < lastDDUError[lastDDU]) {
									LOG4CPLUS_INFO(logger, "Resync confirmed on crate " << crateNumber);
									
									emu::base::TypedFact<emu::fed::DDUFMMResetFact> fact;
									std::ostringstream component;
									component << "FEDCrate" << crateNumber;
									fact.setComponentId(component.str())
										.setSeverity(emu::base::Fact::DEBUG)
										.setDescription("FED crate reset detection")
										.setParameter(emu::fed::DDUFMMResetFact::crateNumber, crateNumber);
									pthread_mutex_lock(&(locdata->applicationMutex));
									application_->storeFact(fact);
									application_->sendFacts();
									pthread_mutex_unlock(&(locdata->applicationMutex));
									
									break; // Go to the beginning of the outer loop, which will do the resets and start everything over.
									
								} else {
									
									LOG4CPLUS_INFO(logger, "No resync.  Continuing as normal.");
									
								}
								
							}

						} else {
							LOG4CPLUS_WARN(logger, "Configuration might have changed on crate " << crateNumber << ", and that spells disaster!");
							LOG4CPLUS_WARN(logger, "Assuming reset!");
							
							break;
						}

					} // End check for reset

					// Do TF-related checks
					if (myCrate->isTrackFinder()) {
						// Nothing to do currently.
					} else { // Do DCC-related checks

						checkDDUStatus(dduVector, logger);

						for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
								
							// Report the FMM status to Hotspot
							uint16_t dccFMMStatus = (*iDCC)->readFMMStatus();
							if (dccFMMStatus != lastDCCError[(*iDCC)->getSlot()]) {
								std::multimap<std::string, std::string> dccFMMDecoded = DCCDebugger::FMMStatus(dccFMMStatus);
								
								std::ostringstream error;
								error << "FMM status for DCC in crate " << crateNumber << " slot " << (*iDCC)->getSlot() << " is now " << std::hex << dccFMMStatus << std::dec << "(";
								for (std::multimap<std::string, std::string>::const_iterator iStatus = dccFMMDecoded.begin(); ; ) {
									error << iStatus->second;
									if (++iStatus != dccFMMDecoded.end()) 
										error << ", ";
									else break;
								}
								error << ")";
								
								if (dccFMMDecoded.find("ok") == dccFMMDecoded.end()) {
									LOG4CPLUS_ERROR(logger, error.str());
									
									std::ostringstream tag;
									tag << "FMM " << (*iDCC)->getFMMID() << " FEDcrate " << crateNumber;

									// What kind of alarm is this raising?
									std::string alarmType = "ERROR";
									if (dccFMMDecoded.find("error") == dccFMMDecoded.end()) alarmType = "WARN";

									// Distinct alarm for each DCC
									std::ostringstream alarmName;
									alarmName << "IRQThreadDCCFMM" << crateNumber << "_" << (*iDCC)->getSlot();
									MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), alarmType, error.str(), tag.str());
								} else {
									LOG4CPLUS_INFO(logger, error.str());
									
									std::ostringstream alarmName;
									alarmName << "IRQThreadDCCFMM" << crateNumber << "_" << (*iDCC)->getSlot();
									MY_REVOKE_ALARM(alarmName.str());
								}
								lastDCCError[(*iDCC)->getSlot()] = dccFMMStatus;
							}

							// Report the FIFO statuses to Hotspot
							uint8_t dccFIFOStatus = (*iDCC)->readFIFOStatus();
							if (dccFIFOStatus != lastFIFOError[(*iDCC)->getSlot()]) {
								if (dccFIFOStatus != 0xff) {
									std::vector<FIFO *> fifoVector = (*iDCC)->getFIFOs();
									for (std::vector<FIFO *>::iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); iFIFO++) {
										std::pair<std::string, std::string> fifoDecoded = DCCDebugger::decodeFIFOStatus(dccFIFOStatus, (*iFIFO)->getNumber());
										if (fifoDecoded.second != "green") {
											std::ostringstream error;
											error << "FIFO " << (*iFIFO)->getNumber() << " (DDU " << (*iFIFO)->getRUI() << ") status for DCC in crate " << crateNumber << " is now " <<  fifoDecoded.second;

											std::ostringstream tag;
											tag << "FMM " << (*iDCC)->getFMMID() << " FEDcrate " << crateNumber << " RUI " << (*iFIFO)->getRUI();

											// What kind of alarm is this raising?
											std::string alarmType;
											if (fifoDecoded.second != "error" && fifoDecoded.second != "red") {
												alarmType = "WARN";
												LOG4CPLUS_WARN(logger, error.str());
											} else {
												alarmType = "ERROR";
												LOG4CPLUS_ERROR(logger, error.str());
											}

											// Distinct alarm for each FIFO
											std::ostringstream alarmName;
											alarmName << "IRQThreadDCCFIFO" << crateNumber << "_" << (*iDCC)->slot() << "_" << (*iFIFO)->getNumber();
											MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), alarmType, error.str(), tag.str());
										} else {
											std::ostringstream alarmName;
											alarmName << "IRQThreadDCCFIFO" << crateNumber << "_" << (*iDCC)->slot() << "_" << (*iFIFO)->getNumber();
											MY_REVOKE_ALARM(alarmName.str());
										}
									}
								}
								lastFIFOError[(*iDCC)->getSlot()] = dccFIFOStatus;
							}

							// Report the SLink statuses to Hotspot
							uint16_t dccSLinkStatus = (*iDCC)->readSLinkStatus();
							if (dccSLinkStatus != lastSLinkError[(*iDCC)->getSlot()]) {
								if (dccSLinkStatus & 0xa) {

									for (unsigned int iLink = 0; iLink <= 1; iLink++) {
										std::pair<std::string, std::string> slinkDecoded = DCCDebugger::decodeSLinkStatus(dccSLinkStatus, iLink);
										if (slinkDecoded.second != "ok") {
											std::ostringstream error;
											error << "SLink " << iLink << " status for DCC in crate " << crateNumber << " is now " <<  slinkDecoded.second;

											std::ostringstream tag;
											tag << "FMM " << (*iDCC)->getFMMID() << " FEDcrate " << crateNumber;

											// What kind of alarm is this raising?
											std::string alarmType;
											if (slinkDecoded.second != "error" && slinkDecoded.second != "red") {
												alarmType = "WARN";
												LOG4CPLUS_WARN(logger, error.str());
											} else {
												alarmType = "ERROR";
												LOG4CPLUS_ERROR(logger, error.str());
											}

											// Distinct alarm for each SLink
											std::ostringstream alarmName;
											alarmName << "IRQThreadDCCSlink" << crateNumber << "_" << (*iDCC)->slot() << "_" << iLink;
											MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), alarmType, error.str(), tag.str());
										} else {
											std::ostringstream alarmName;
											alarmName << "IRQThreadDCCSlink" << crateNumber << "_" << (*iDCC)->slot() << "_" << iLink;
											MY_REVOKE_ALARM(alarmName.str());
										}
									}

								}
								lastSLinkError[(*iDCC)->getSlot()] = dccSLinkStatus;
							}
							
							MY_REVOKE_ALARM("IRQThreadDCCStatus");

						}

					} // End TF/DCC checks

				} // End all clear
				
			} catch (emu::fed::exception::Exception &e) {
				std::ostringstream error;
				error << "Exception in IRQ handling for crate " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				LOG4CPLUS_FATAL(logger, e.toJSON());
				std::ostringstream tag;
				tag << "FEDcrate " << crateNumber;
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadWait", "ERROR", error.str(), tag.str(), e);
				pthread_exit(NULL);
			}
			
		}

	}

	// I will never make it here, but just in case.
	pthread_exit(PTHREAD_CANCELED);
}



