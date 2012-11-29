/*****************************************************************************\
* $Id: IRQThreadManager.cc,v 1.38 2012/11/29 16:02:37 cvuosalo Exp $
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
waitTimeAfterFMM_(5),
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



void emu::fed::IRQThreadManager::attachCrates(std::vector<Crate *> &crateVec)
{
	threadVector_.clear();
	for (std::vector<Crate *>::iterator iCrate = crateVec.begin(); iCrate != crateVec.end(); iCrate++) {
		pthread_t threadID = 0;
		threadVector_.push_back(std::pair<Crate *, pthread_t>(*iCrate, threadID));
	}
}



void emu::fed::IRQThreadManager::startThreads(const unsigned int &runNumber)
throw (emu::fed::exception::FMMThreadException)
{

	// Make the shared data object that will be passed between threads and the
	// mother program.
	// data_ = new IRQData(application_); // Instantiate in constructor, not here
	data_->fmmErrorThreshold = fmmErrorThreshold_;
	data_->waitTimeAfterFMM = waitTimeAfterFMM_;

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
	data_->runNumStr.str("");		// Clear previous run num
	data_->runNumStr << runNumber;
	// Do not quit the threads immediately.
	//data_->exit = false;

	// First, load up the data_ object with the crates that I govern.
	while (data_->crateQueue.empty() == false)
		data_->crateQueue.pop();
	data_->crateVec.clear();
	data_->nCrates = 0;
	for (unsigned int iThread = 0; iThread < threadVector_.size(); ++iThread) {
		data_->crateQueue.push(threadVector_[iThread].first);
		data_->crateVec.push_back(threadVector_[iThread].first);
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
		LOG4CPLUS_INFO(logger, "Threads already stopped.");
		return;
	}
	LOG4CPLUS_INFO(logger, "Gracefully killing off all threads: " << threadVector_.size() );

	// We probably do not need to return the status of the threads,
	//  but this may be used later for whatever reason.
	//std::vector<int> returnStatus;

	for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {

		Crate *myCrate = threadVector_[iThread].first;
		LOG4CPLUS_INFO(logger, "Killing thread: " << threadVector_[iThread].second << ", " << myCrate );

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


static void setDDUerr(emu::fed::DDU *myDDU)
{
	myDDU->writeFMM( 0xf0ec );
	(void) usleep(100000);	// 100000 usec = 0.1 second
	(void) sleep(3);	// 3 seconds
	// 3.1 seconds total wait to allow GT to see Error and cause hard reset
	myDDU->writeFMM( 0xfed0 );
}


// Sets DDU into Error for 3.1 seconds to request that GT send a hard reset.
// If a request was made in the last 10 seconds, no new request is made.
// Returns true if the hard reset was requested, false if no request was made
// because there was already a request in the last 10 seconds.
// The only reason this function is a member function is so it can call sendFacts,
// which only a friend class like IRQThreadManager can call.

bool emu::fed::IRQThreadManager::DDUWarnMon::setDDUerror(emu::fed::DDU *myDDU, log4cplus::Logger &logger,
	const unsigned int crateNumber, emu::fed::IRQData *const locdata,
	const std::string &warnFibers, const std::string &warnChambers)
{
	if (lastErrTm_ < 0 || (time(NULL) - lastErrTm_) > 10) {	// 10 seconds between Errors
		lastErrTm_ = time(NULL);
		setDDUerr(myDDU);
		uint16_t ruiNum = myDDU->getRUI();
		LOG4CPLUS_ERROR(logger, endl << "DDU RUI #" << ruiNum <<
			" stuck in warning. Setting DDU Error to request hard reset from GT." << endl);
		// Make and send the fact to the expert system
		emu::base::TypedFact<emu::fed::DDUStuckInWarningFact> fact;
		std::ostringstream component;
		component << "DDU" << setfill('0') << setw(2) << ruiNum;
		fact.setComponentId(component.str())
			.setSeverity(emu::base::Fact::ERROR)
			.setRun(locdata->runNumStr.str())
			.setDescription("DDU stuck in Warning -- hard reset requested")
			.setParameter(emu::fed::DDUStuckInWarningFact::hardResetRequested, true)
			.setParameter(emu::fed::DDUStuckInWarningFact::chambersInWarning, warnChambers)
			.setParameter(emu::fed::DDUStuckInWarningFact::fibersInWarning, warnFibers);
			
		pthread_mutex_lock(&(locdata->applicationMutex));
		emu::fed::Communicator *application = locdata->application;
		application->storeFact(fact);
		application->sendFacts();
		pthread_mutex_unlock(&(locdata->applicationMutex));
		return (true);
	}
	return (false);
}


// Checks if any DDU is in Warning, and if so, requests a hard reset.
// See setDDUerror above for more details.
// The only reason checkDDUStatus is a member function is so it can call setDDUerror.

void emu::fed::IRQThreadManager::DDUWarnMon::checkDDUStatus(std::vector<emu::fed::DDU *> &dduVector, log4cplus::Logger &logger,
	const unsigned int crateNumber, emu::fed::IRQData *const locdata)
{
	bool statRep = false;
	std::ostringstream statusMsg, busyFibers, warnFibers, warnNowFibers, errFibers, oosFibers;
	std::stringstream chamberWarns, fiberWarns, subErrs;
	statusMsg <<     "DDU statuses            ";
	busyFibers << 	 "Fibers that had Busy    ";
	warnFibers << 	 "Fibers that had Warning ";
	warnNowFibers << "Fibers now in Warning   ";
	errFibers <<     "Fibers now in Error     ";
	oosFibers <<     "Fibers now in OOS       ";
	emu::fed::DDU *dduInWarn = NULL;
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
			if (statRep == false) {
				statRep = (fmmStat == 0x1);	// 0x1 means Warning
				// statRep = (busyStat | warnHStat | warnNStat | errStat | oosStat);
				dduInWarn = myDDU;
			}
			busyFibers << mkBitStr(busyStat) << " ";
			warnFibers << mkBitStr(warnHStat) << " ";
			warnNowFibers << mkBitStr(warnNStat) << " ";
			errFibers << mkBitStr(errStat) << " ";
			oosFibers << mkBitStr(oosStat) << " ";

			std::vector<std::string> unused;
			DDUDebugger::readErrors(myDDU, unused, subErrs);
			subErrs << " ";
			/*
			busyFibers << mkBitStr(myDDU->readBusyHistory()) << " ";
			warnFibers << mkBitStr(myDDU->readWarningHistory()) << " ";
			warnNowFibers << mkBitStr(myDDU->readFullWarning()) << " ";
			errFibers << mkBitStr(myDDU->readFMMError()) << " ";
			oosFibers << mkBitStr(myDDU->readFMMLostSync()) << " ";
			*/
			std::bitset<16> warnBits(warnNStat);
			bool moreFibers = false;
			for (unsigned int iFiber = 0; iFiber < 15; ++iFiber) {
				if (warnBits[iFiber]) {
					if (moreFibers) {
						chamberWarns << ", ";
						fiberWarns << ", ";
					}
					fiberWarns << iFiber;
					chamberWarns << myDDU->getFiber(iFiber)->getName();
					moreFibers = true;
				}
			}
		}
	}
	if (statRep) {
		if (index_++ >= delay_) {
			if (delay_ > 1000) {
				delay_ = 1;
				index_ = 1;
			} else delay_ *= 2;
			LOG4CPLUS_WARN(logger, endl << statusMsg.str() << endl << warnNowFibers.str()
				<< endl << warnFibers.str() << endl << subErrs.str() << endl);
				// << busyFibers.str() << endl << oosFibers.str() << endl << errFibers.str() << endl);
			if (index_ == 5 && dduInWarn != NULL) {
				uint16_t ruiNum = dduInWarn->getRUI();
				// Make and send the fact to the expert system
				emu::base::TypedFact<emu::fed::DDUStuckInWarningFact> fact;
				std::ostringstream component;
				component << "DDU" << setfill('0') << setw(2) << ruiNum;
				fact.setComponentId(component.str())
					.setSeverity(emu::base::Fact::ERROR)
					.setRun(locdata->runNumStr.str())
					.setDescription("More than one DDU-stuck-in-Warning incident occurred within 10 seconds")
					.setParameter(emu::fed::DDUStuckInWarningFact::hardResetRequested, false)
					.setParameter(emu::fed::DDUStuckInWarningFact::chambersInWarning, chamberWarns.str())
					.setParameter(emu::fed::DDUStuckInWarningFact::fibersInWarning, fiberWarns.str());
				pthread_mutex_lock(&(locdata->applicationMutex));
				emu::fed::Communicator *application = locdata->application;
				application->storeFact(fact);
				application->sendFacts();
				pthread_mutex_unlock(&(locdata->applicationMutex));
			}
		}
		// On second Warning, set Error
		if (got1Warn_ && dduInWarn != NULL &&
				setDDUerror(dduInWarn, logger, crateNumber, locdata, fiberWarns.str(), chamberWarns.str()))
			delay_ = index_ = 1;	// Reset delay_ if hard reset requested.
		got1Warn_ = true;
	} else if (got1Warn_) {
		got1Warn_ = false;
	}
}


// The only reason this function is a member function is so it can call sendFacts,
// which only a friend class like IRQThreadManager can call.

void emu::fed::IRQThreadManager::sendRepErrFact(const unsigned int crateNumber, emu::fed::IRQData *const locdata,
	const std::string &repErrChambers, const unsigned int totRepErrs)
{
	// Make and send the fact to the expert system
	emu::base::TypedFact<emu::fed::FedRepeatErrorFact> fact;
	std::ostringstream component;
	char endcap = '+';
	if (crateNumber >= 3)
		endcap = '-';
	component << "ME" << endcap;
	fact.setComponentId(component.str())
		.setSeverity(emu::base::Fact::INFO)
		.setRun(locdata->runNumStr.str())
		.setParameter(emu::fed::FedRepeatErrorFact::chambersInError, repErrChambers)
		.setParameter(emu::fed::FedRepeatErrorFact::numChambersInError, totRepErrs);
		
	emu::fed::Communicator *application = locdata->application;
	pthread_mutex_lock(&(locdata->applicationMutex));
	application->storeFact(fact);
	application->sendFacts();
	pthread_mutex_unlock(&(locdata->applicationMutex));
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
	std::map<unsigned int, unsigned int> nIgnFibers;

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

	DDUWarnMon dduWarnMonitor;
	int unused = 0; // Unused 2nd parameter of pthread_setcancelstate

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

		LOG4CPLUS_DEBUG(logger, "Starting outer loop for crate " << crateNumber <<
		", run number " << locdata->runNumStr.str());

		// Make sure thread can be cancelled
		if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
			LOG4CPLUS_ERROR(logger, "pthread_setcancelstate enable error -- trying to set bad state");

		// Immediate check for cancel
		pthread_testcancel();
		
		pthread_mutex_lock(&locdata->errorCountMutex);
		locdata->errorCount[crateNumber] = 0;
		locdata->ignErrCnt[crateNumber] = 0;
		for (IRQData::crateHistory::iterator iSlot = locdata->errorHistory[crateNumber].begin();
				iSlot != locdata->errorHistory[crateNumber].end(); ++iSlot )
			iSlot->second[IRQData::CURR_ERR] = 0;
		pthread_mutex_unlock(&locdata->errorCountMutex);
		
		nErrors.clear();
		nIgnFibers.clear();
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

			if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
				LOG4CPLUS_ERROR(logger, "pthread_setcancelstate enable error -- trying to set bad state");
			pthread_testcancel();
			// Increase the ticks.
			//locdata->ticks[crateNumber]++;

			// Set the time of the last tick.
			//time_t tick;
			//time(&tick);
			//tm *tickInfo = localtime(&tick);
			//std::string tickText(asctime(tickInfo));
			//locdata->tickTime[crateNumber] = tickText;

			// If other thread has reset, then reset this thread, too.
			if (locdata->resetCount)
				break;

			try {

				// I know this means the TF DDU will use interrupts, but we'll deal with that hurdle when we come to it.
				if (myCrate->getController()->waitIRQ(1000) == false) {
					pthread_testcancel();
					// Prevent crash that occurs when thread canceled during logging
					if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &unused) != 0)
						LOG4CPLUS_ERROR(logger, "pthread_setcancelstate error -- trying to set bad state");

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
						--numBits;	// Bit 15 (top bit) tells if DDU is in error, so if it's set, ignore it.
					nErrors[slot] = numBits;

					// Count number of fibers in error now that are in dynamic ignore list
					std::bitset<16> ignFibBits(locdata->errorHistory[crateNumber][slot][IRQData::SECOND_ERR] & combinedStatus);
					unsigned int numIgnFibBits = ignFibBits.count();
					nIgnFibers[slot] = numIgnFibBits;
					
					xorStatus = xorStatus & combinedStatus;
					// And with combinedStatus to get only new chamber errors, not any that turned off.

					xorStatus = xorStatus & 0x7fff;
					// Turn off high bit that represents DDU error. We only want to count chamber errors.

					uint32_t l1a = myDDU->readL1Scaler(DDUFPGA); // for debugging purposes
					if (doReset == false && l1a > 0) {
						doReset = (l1a < lastL1A);
						if (doReset)
							LOG4CPLUS_DEBUG(logger, "New L1A " << l1a << " is less than previous " << lastL1A
								<< " -- resync occurred");
					}
					lastL1A = l1a;

					// Sometimes an interrupt does not have any errors to report.  Ignore these.
					if (!xorStatus) {
						std::ostringstream logMsg, debugMsg;
						logMsg << "IRQ detected on crate " << crateNumber << " slot " << slot << " but there are no ";
						if ((combinedStatus  & 0x7fff) != 0) {
							// Ignore high bit that represents DDU error. We only want to count chamber errors.
							logMsg << "new ";
							debugMsg << "Bits for repeated chamber errors being ignored -- Combined Status: " << statusBitString;
						}
						logMsg << "errors to report. Ignoring.";
						LOG4CPLUS_INFO(logger, logMsg.str());
						if ((combinedStatus  & 0x7fff) != 0)
							LOG4CPLUS_DEBUG(logger, debugMsg.str());
						// LOG4CPLUS_WARN(logger, "IRQ detected on crate " << crateNumber << " slot " << slot << " with no new errors.  Ignoring.");

						// Emergency break if we think a reset has happened under our noses.
						if (doReset || locdata->resetCount)
							break;

						continue;
					}
					
					pthread_mutex_lock(&locdata->lastErrMutex);

					// Save current fibers in error so other thread can read them
					locdata->errorHistory[crateNumber][slot][IRQData::CURR_ERR] = combinedStatus & 0x7fff;
					// Turn off high bit that represents DDU error. We only want to count chamber errors.

					pthread_mutex_unlock(&locdata->lastErrMutex);
					
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
					bool moreFibers = false;
					for (unsigned int iFiber = 0; iFiber < 15; ++iFiber) {
						if (xorBits[iFiber]) {
							if (moreFibers) {
								fiberErrors << ", ";
								chamberErrors << ", ";
							}
							fiberErrors << iFiber;
							chamberErrors << myDDU->getFiber(iFiber)->getName();
							moreFibers = true;
						}
					}
					
					std::bitset<16> cscBits(cscStatus);
					
					std::string cscBitString = cscBits.to_string<char, char_traits<char>, allocator<char> >();
					std::string xorBitString = xorBits.to_string<char, char_traits<char>, allocator<char> >();
					
					LOG4CPLUS_INFO(logger, "Decoded information follows" << std::endl
						<< "Run number : " << locdata->runNumStr.str() << std::endl
						<< "FEDCrate   : " << crateNumber << std::endl
						<< "Slot       : " << myDDU->getSlot() << std::endl
						<< "RUI        : " << myDDU->getRUI() << std::endl
						<< "CSC Status : " << cscBitString << std::endl
						<< "Cmb Status : " << statusBitString << std::endl
						<< "XOR Status : " << xorBitString << std::endl
						<< "DDU error  : " << ((cscStatus & 0x8000) == 0x8000) << std::endl
						<< "Fibers     : " << fiberErrors.str() << std::endl
						<< "Chambers   : " << chamberErrors.str() << std::endl
						<< "Hard reset needed      : " << hardError << std::endl
						<< "Resync needed          : " << syncError << std::endl
						<< "Reset or resync needed : " << resetWanted);
					
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
					
					// Record the error in an accessible history of errors.
					lastDDUError[myDDU->slot()] = combinedStatus;
					
					// Just in case there is some bizarre error at this point that causes this to
					// overflow...
					/*
					if (locdata->errorCount[crateNumber] > 15) {
						locdata->errorCount[crateNumber] = 15;
					}
					*/
					lastDDU = slot;
					
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
						} else { // REPORT TO SENTINEL!
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
					locdata->ignErrCnt[crateNumber] = 0;
					for (std::map<unsigned int, unsigned int>::iterator iCount = nIgnFibers.begin(); iCount != nIgnFibers.end(); ++iCount) {
						locdata->ignErrCnt[crateNumber] += iCount->second;
					}
					
					// Now count all the errors from all the threads
					int totalErrors = 0;
					int numIgnFibers = 0;
					bool fmmsReleased = false;
					for (std::map<unsigned int, unsigned int>::const_iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); ++iCount) {
						totalErrors += iCount->second;
					}
					for (std::map<unsigned int, unsigned int>::const_iterator iCount = locdata->ignErrCnt.begin(); iCount != locdata->ignErrCnt.end(); ++iCount) {
						numIgnFibers += iCount->second;
					}
					pthread_mutex_unlock(&locdata->errorCountMutex);

					// If resync occurred, only count errors from current DDU because rest should have been cleared
					if (doReset) {
						totalErrors = nErrors[slot] - nIgnFibers[slot];
						LOG4CPLUS_DEBUG(logger,
							"Ignoring repeated errors, so errors from current DDU reduced by " <<  nIgnFibers[slot]);
					} else if (numIgnFibers > 0) {
						totalErrors -= numIgnFibers;
						if (totalErrors  < 0) // Shouldn't happen, but just in case
							totalErrors = 0;
						LOG4CPLUS_DEBUG(logger,
							"Ignoring repeated errors, so total errors on this endcap reduced by " << numIgnFibers);
					}
					if (totalErrors >= (int) locdata->fmmErrorThreshold) {
						LOG4CPLUS_INFO(logger,
							"Releasing FMMs because total errors on this endcap = " << totalErrors << " is >= threshold of " <<
							locdata->fmmErrorThreshold << std::endl);
						if (!myCrate->isTrackFinder()) {
							if (hardError)
								myCrate->getBroadcastDDU()->enableFMM();
								// Briefly release the FMMs.
							else {
								setDDUerr(myDDU);	// Ensure DDU sets Error, not Out-of-sync
								LOG4CPLUS_DEBUG(logger,
									"Overriding DDU OOS with Error to ensure we receive a hard reset");
							}
							fmmsReleased = true;
						}
					} else {
						LOG4CPLUS_INFO(logger, "Total errors on this endcap = " << totalErrors 
							<< " is less than FMM error threshold of " <<
							locdata->fmmErrorThreshold << std::endl);
					}
					// Make and send the fact to the expert system
					emu::base::TypedFact<emu::fed::DDUFMMErrorFact> fact;
					std::ostringstream component;
					component << "DDU" << setfill('0') << setw(2) << myDDU->getRUI();
					fact.setComponentId(component.str())
						.setSeverity(emu::base::Fact::ERROR)
						.setRun(locdata->runNumStr.str())
						.setDescription("DDU FMM IRQ information")
						.setParameter(emu::fed::DDUFMMErrorFact::hardResetRequested, hardError)
						.setParameter(emu::fed::DDUFMMErrorFact::resyncRequested, syncError)
						.setParameter(emu::fed::DDUFMMErrorFact::combinedStatus, combinedStatus)
						.setParameter(emu::fed::DDUFMMErrorFact::combinedStatusStr, statusBitString)
						.setParameter(emu::fed::DDUFMMErrorFact::fibersInError, fiberErrors.str())
						.setParameter(emu::fed::DDUFMMErrorFact::chambersInError, chamberErrors.str())
						.setParameter(emu::fed::DDUFMMErrorFact::numChambersInErrorForEndcap, totalErrors)
						.setParameter(emu::fed::DDUFMMErrorFact::fmmErrorThreshold, locdata->fmmErrorThreshold)
						.setParameter(emu::fed::DDUFMMErrorFact::fmmsReleased, fmmsReleased)
						.setParameter(emu::fed::DDUFMMErrorFact::ddufpgaDebugTrap, ddufpgaStream.str())
						.setParameter(emu::fed::DDUFMMErrorFact::infpga0DebugTrap, infpga0Stream.str())
						.setParameter(emu::fed::DDUFMMErrorFact::infpga1DebugTrap, infpga1Stream.str());
						
					pthread_mutex_lock(&(locdata->applicationMutex));
					application_->storeFact(fact);
					application_->sendFacts();
					pthread_mutex_unlock(&(locdata->applicationMutex));

					std::stringstream firstErrStr, secondErrStr;
					if (fmmsReleased) {
						std::stringstream repErrChambers;
						unsigned int totReppErrs = 0;
						bool moreFibers = false;
						pthread_mutex_lock(&locdata->lastErrMutex);

						for (IRQData::endcapHistory::iterator iCrate = locdata->errorHistory.begin(); iCrate != locdata->errorHistory.end(); ++iCrate) {
							unsigned int currCrateNum = iCrate->first;
							firstErrStr << "Crate " << currCrateNum << " ";
							secondErrStr << "Crate " << currCrateNum << " ";
							for (unsigned int iThread = 0; iThread < locdata->crateVec.size(); ++iThread) {
								emu::fed::Crate *currCrate = locdata->crateVec[iThread];
								if (currCrate->number() == currCrateNum) {
									std::vector<emu::fed::DDU *> currDDUvec = currCrate->getDDUs();
									for (IRQData::crateHistory::iterator iSlot = iCrate->second.begin(); iSlot != iCrate->second.end(); ++iSlot ) {
										IRQData::fiberHistory &fibHist = iSlot->second;
										fibHist[IRQData::SECOND_ERR] = fibHist[IRQData::SECOND_ERR] |
											(fibHist[IRQData::CURR_ERR] & fibHist[IRQData::FIRST_ERR]);
										fibHist[IRQData::FIRST_ERR] |= fibHist[IRQData::CURR_ERR];
										fibHist[IRQData::CURR_ERR] = 0;
										if (fibHist[IRQData::FIRST_ERR] > 0) {
											std::bitset<16> statusBits(fibHist[IRQData::FIRST_ERR]);
											std::string statusBitString = statusBits.to_string<char, char_traits<char>, allocator<char> >();
											firstErrStr << " Slot " << iSlot->first << " " << statusBitString << " ";
										}
										if (fibHist[IRQData::SECOND_ERR] > 0) {
											std::bitset<16> statusBits = fibHist[IRQData::SECOND_ERR];
											std::string statusBitString = statusBits.to_string<char, char_traits<char>, allocator<char> >();
											secondErrStr << " Slot " << iSlot->first << " " << statusBitString << " ";
											for (std::vector<DDU *>::iterator iDDU = currDDUvec.begin(); iDDU != currDDUvec.end(); ++iDDU) {
												if ((*iDDU)->slot() ==  iSlot->first) {
													for (unsigned int iFiber = 0; iFiber < 15; ++iFiber) {
														if (statusBits[iFiber]) {
															if (moreFibers) {
																repErrChambers << ", ";
															}
															repErrChambers << (*iDDU)->getFiber(iFiber)->getName();
															totReppErrs++;
															moreFibers = true;
														}
													}
													break; // Done, just needed the right DDU
												}
											}
										}
									}
									break; // Done, just need the right crate
								}
							}
						}
						pthread_mutex_unlock(&locdata->lastErrMutex);
						LOG4CPLUS_DEBUG(logger,
							"Chambers in error once " << std::endl << firstErrStr.str() <<  std::endl <<
							"Chambers in error twice " << std::endl << secondErrStr.str() <<  std::endl);
						if (totReppErrs > 0)
							sendRepErrFact(crateNumber, locdata, repErrChambers.str(), totReppErrs);
					}
					MY_REVOKE_ALARM("IRQThreadGeneralError");
					
					if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
						LOG4CPLUS_ERROR(logger, "pthread_setcancelstate enable error -- trying to set bad state");
					pthread_testcancel();
					
					if (fmmsReleased && locdata->waitTimeAfterFMM > 0 && locdata->waitTimeAfterFMM < 61) {
						// If wait time has reasonable value, wait after releasing FMMs.
						// This wait avoids hitting the GT limit on the number of errors.
						// If the GT receives 3 Errors within 9 seconds, it doesn't issue
						// hard reset but requires a manual hard reset from the DAQ shifter
						(void) sleep(locdata->waitTimeAfterFMM);	// Time in seconds
					}
					// Emergency break if we think a reset has happened under our noses.
					if (doReset || locdata->resetCount)
						break;
				} else {
					pthread_testcancel();

					// Prevent thread cancellation because cancel during logging causes crash
					if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &unused) != 0)
						LOG4CPLUS_ERROR(logger, "pthread_setcancelstate disable error -- trying to set bad state");

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

								if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
									LOG4CPLUS_ERROR(logger, "pthread_setcancelstate enable error -- trying to set bad state");
								usleep(100);
								pthread_testcancel();
								// Prevent crash that occurs when thread canceled during logging
								if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &unused) != 0)
									LOG4CPLUS_ERROR(logger, "pthread_setcancelstate error -- trying to set bad state");

								if (myDDU->readFiberErrors() < lastDDUError[lastDDU]) {
									LOG4CPLUS_INFO(logger, "Resync confirmed on crate " << crateNumber);
									
									emu::base::TypedFact<emu::fed::DDUFMMResetFact> fact;
									std::ostringstream component;
									component << "FEDCrate" << crateNumber;
									fact.setComponentId(component.str())
										.setSeverity(emu::base::Fact::DEBUG)
										.setRun(locdata->runNumStr.str())
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

						dduWarnMonitor.checkDDUStatus(dduVector, logger, crateNumber, locdata);

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
					if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
						LOG4CPLUS_ERROR(logger, "pthread_setcancelstate enable error -- trying to set bad state");
					pthread_testcancel();
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



