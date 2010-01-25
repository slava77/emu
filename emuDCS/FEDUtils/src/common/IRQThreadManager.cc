/*****************************************************************************\
* $Id: IRQThreadManager.cc,v 1.17 2010/01/25 13:45:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/IRQThreadManager.h"

#include <utility>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>

#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/IRQData.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"



emu::fed::IRQThreadManager::IRQThreadManager(xdaq::WebApplication *application, const unsigned int &fmmErrorThreshold):
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
		// I don't care if this doesn't work.  Zombies will all die eventually, even if they leak a little memory.
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
	strftime(datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", localtime(&theTime));
	fileName << "/tmp/EmuFMMThread_" << (systemName_ != "" ? systemName_ + "_" : "") << datebuf << "_r" << std::setw(6) << std::setfill('0') << std::dec << runNumber << ".log";

	log4cplus::SharedAppenderPtr myAppend = new log4cplus::FileAppender(fileName.str().c_str());
	myAppend->setName("EmuFMMIRQAppender");

	//Appender Layout
	std::auto_ptr<log4cplus::Layout> myLayout = std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
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
							tag << "RUI " << (*iDDU)->readRUI() << " FEDCrate " << myCrate->number();
							
						} else {
							chamberName = (iFiber == 15) ? "DDU" : (*iDDU)->getFiber(iFiber)->getName();
							
							tag << "fiber " << iFiber << "RUI " << (*iDDU)->readRUI() << " FEDCrate " << myCrate->number() << " chamber " << chamberName;
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
	LOG4CPLUS_INFO(logger, "Gracefully killing off all threads.");

	// We probably do not need to return the status of the threads,
	//  but this may be used later for whatever reason.
	//std::vector<int> returnStatus;

	for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {
		
		Crate *myCrate = threadVector_[iThread].first;
		
		// Cancel threads first.
		int err = pthread_cancel(threadVector_[iThread].second);
		if (err) {
			std::ostringstream error;
			error << "Exception cancelling IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << myCrate->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadEnd", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());
			throw e2;
		}
		
		// The threads should be stopping now.  Let's join them.
		void *tempException = NULL; // I have to do this for type safety.
		err = pthread_join(threadVector_[iThread].second, &tempException); // Waits until the thread calls pthread_exit(void *return_status)

		if (err || tempException != PTHREAD_CANCELED) {
			std::ostringstream error;
			error << "Exception joining IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << myCrate->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadEnd", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());
			throw e2;
		}

		LOG4CPLUS_INFO(logger, "Joined IRQThread " << iThread << " for crate " << myCrate->number());

	}
	
	MY_REVOKE_ALARM("IRQThreadEnd");

	threadVector_.clear();
}



/// The big one
void *emu::fed::IRQThreadManager::IRQThread(void *data)
{

	// Recast the void pointer as something more useful.
	IRQData *locdata = (IRQData *) data;
	
	// Make sure we have an application_ variable for the macros.  Static functions cannot access class members.
	xdaq::WebApplication *application_ = locdata->application;

	// Grab the crate that I will be working with (and pop off the crate so
	//  that I am the only one working with this particular crate.)
	// Use mutexes to serialize
	pthread_mutex_lock(&(locdata->crateQueueMutex));
	Crate *myCrate = locdata->crateQueue.front();
	locdata->crateQueue.pop();
	pthread_mutex_unlock(&(locdata->crateQueueMutex));

	unsigned int crateNumber = myCrate->number();

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");

	// Knowing what boards we are talking to is useful as well.
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	std::vector<DCC *> dccVector = myCrate->getDCCs();

	// This is when we started.  Don't know why this screws up sometimes...
	time_t tock;
	time(&tock);
	tm *tockInfo = localtime(&tock);
	std::string tockText(asctime(tockInfo));
	locdata->startTime[crateNumber] = tockText;

	// A local tally of what the last error on a given DDU was.
	std::map<unsigned int, int> lastDDUError;
	
	// A local tally of what the last FMM error on a given DCC was.
	std::map<unsigned int, int> lastDCCError;
	
	// A local tally of what the last FIFO error on a given DCC was.
	std::map<unsigned int, int> lastFIFOError;
	
	// A local tally of what the last SLink error on a given DCC was.
	std::map<unsigned int, int> lastSLinkError;
	
	// Begin by clearing all the Sentinel alarms that might have popped up in past runs.  If there is a problem, they will be caught again.
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			std::ostringstream alarmName;
			alarmName << "IRQThreadFiber" << crateNumber << "_" << (*iDDU)->slot() << "_" << iFiber;
			MY_REVOKE_ALARM(alarmName.str());
		}
	}
	
	// Clear all DCC errors in the same fashion
	for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
		lastDCCError[(*iDCC)->getSlot()] = 0x2;
		lastFIFOError[(*iDCC)->getSlot()] = 0xff;
		lastSLinkError[(*iDCC)->getSlot()] = 0x0;
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

	// Continue unless someone tells us to stop.
	while (1) { // Always looping until the thread is canceled

		// Immediate check for cancel
		pthread_testcancel();
		
		//LOG4CPLUS_DEBUG(logger, "Start of loop reached.");

		// Increase the ticks.
		locdata->ticks[crateNumber]++;

		// Set the time of the last tick.
		time_t tick;
		time(&tick);
		tm *tickInfo = localtime(&tick);
		std::string tickText(asctime(tickInfo));
		locdata->tickTime[crateNumber] = tickText;

		// Enable the IRQ and wait for something to happen for 5 seconds...
		bool allClear = false;
		
		if (!myCrate->isTrackFinder()) { // The regular crates will just use the IRQ as normal
			try {
				// This will either immediately generate a false on detection of an IRQ,
				// or will timeout after the specified number of milliseconds and return true if no IRQ is received.
				// Let's do this 5 times with 1 second intervals, testing for cancel at each pulse.
				for (unsigned int iTimes = 0; iTimes < 5; iTimes++) {
					pthread_testcancel();
					if (!(allClear = myCrate->getController()->waitIRQ(1000))) break;
				}
				MY_REVOKE_ALARM("IRQThreadWait");
				pthread_testcancel();
			} catch (emu::fed::exception::CAENException &e) {
				std::ostringstream error;
				error << "Exception waiting for IRQ in crate number " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				tag << "FEDcrate " << crateNumber;
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadWait", "ERROR", error.str(), tag.str(), e);
				//XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				//e2.setProperty("tag", tag.str());
				//pthread_exit((void *) &e2);
				pthread_exit(NULL);
			}
		} else { // The TF crate will, for now, fake the IRQ and just sleep
			for (unsigned int iTimes = 0; iTimes < 5; iTimes++) {
				pthread_testcancel();
				sleep((unsigned int) 1);
			}
			pthread_testcancel();
		}


		// If allClear is true, then there was not an error.
		// If there was no error, check to see if we were in an error state
		//  before...
		if (allClear && locdata->errorCount[crateNumber]) {

			// Get the last DDU to report an error
			DDU *myDDU = NULL;
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() == locdata->lastDDU[crateNumber]) {
					myDDU = (*iDDU);
					break;
				}
			}
			
			// Make sure there was a previous DDU
			if (myDDU != NULL) {

				// If my status has cleared, then all is cool, right?
				//  Reset all my data.
				try {
					if (myDDU->readFiberErrors() < lastDDUError[myDDU->slot()]) {
						LOG4CPLUS_INFO(logger, "Reset detected on crate " << crateNumber << ": checking again to make sure...");
						pthread_testcancel();
						usleep(100);
						pthread_testcancel();

						if (myDDU->readFiberErrors() < lastDDUError[myDDU->slot()]) {
							LOG4CPLUS_INFO(logger, "Reset confirmed on crate " << crateNumber);
							LOG4CPLUS_ERROR(logger, " ErrorData RESET Detected" << std::endl);

							// Reset the total error count and the saved errors.
							locdata->errorCount[crateNumber] = 0;
							locdata->errorFiberNames[crateNumber].clear();
							locdata->lastDDU[crateNumber] = 0;
							lastDDUError.clear();
							
							// Clear all alarms that have been set.  This is difficult because all the alarms have different names.
							for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
								std::ostringstream alarmName;
								alarmName << "IRQThreadFiber" << crateNumber << "_" << myDDU->slot() << "_" << iFiber;
								MY_REVOKE_ALARM(alarmName.str());
							}

						} else {

							LOG4CPLUS_INFO(logger, "No reset.  Continuing as normal.");

						}

					}
				} catch (emu::fed::exception::Exception &e) {
					std::ostringstream error;
					error << "Exception reading last DDU status for crate number " << crateNumber;
					LOG4CPLUS_FATAL(logger, error.str());
					std::ostringstream tag;
					tag << "RUI " << myDDU->readRUI() << " " << "FEDcrate " << crateNumber;
					MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadLastDDUStatus", "ERROR", error.str(), tag.str(), e);
					pthread_exit(NULL);
				}

				MY_REVOKE_ALARM("IRQThreadLastDDUStatus");
			}
		}
		
		// Here is a good place to put things you want scanned every 5 seconds,
		// but are not covered by the IRQ, like DCC statuses.
		for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			
			try {
				
				// Report the FMM status to Hotspot
				uint16_t dccFMMStatus = (*iDCC)->readFMMStatus();
				if (dccFMMStatus != lastDCCError[(*iDCC)->getSlot()]) {
					if (dccFMMStatus != 0x2) {
						std::pair<std::string, std::string> dccFMMDecoded = DCCDebugger::FMMStat(dccFMMStatus);
						if (dccFMMDecoded.second != "ok") {
							std::ostringstream error;
							error << "FMM status for DCC in crate " << crateNumber << " slot " << (*iDCC)->getSlot() << " is now " << std::hex << dccFMMStatus << std::dec << "(" <<  dccFMMDecoded.first << ")";
							LOG4CPLUS_ERROR(logger, error.str());
							std::ostringstream tag;
							tag << "FMM " << (*iDCC)->getFMMID() << " FEDcrate " << crateNumber;
							
							// What kind of alarm is this raising?
							std::string alarmType = "ERROR";
							if (dccFMMDecoded.second != "error") alarmType = "WARN";
							
							// Distinct alarm for each DCC
							std::ostringstream alarmName;
							alarmName << "IRQThreadDCCFMM" << crateNumber << "_" << (*iDCC)->getSlot();
							MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), alarmType, error.str(), tag.str());
						} else {
							std::ostringstream alarmName;
							alarmName << "IRQThreadDCCFMM" << crateNumber << "_" << (*iDCC)->getSlot();
							MY_REVOKE_ALARM(alarmName.str());
						}
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
								error << "FIFO " << (*iFIFO)->getNumber() << " (DDU " << (*iFIFO)->getRUI() << ") status for DCC in crate " << crateNumber << " is now " <<  fifoDecoded.first;
								
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
								error << "SLink " << iLink << " status for DCC in crate " << crateNumber << " is now " <<  slinkDecoded.first;

							
								
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
				
			} catch (emu::fed::exception::Exception &e) {
				std::ostringstream error;
				error << "Exception reading DCC status for crate number " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				tag << "FMM " << (*iDCC)->getFMMID() << " FEDcrate " << crateNumber;
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadDCCStatus", "ERROR", error.str(), tag.str(), e);
				//XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				//e2.setProperty("tag", tag.str());
				//pthread_exit((void *) &e2);
				pthread_exit(NULL);
			}
			
			MY_REVOKE_ALARM("IRQThreadDCCStatus");
			
			// Loop through all all the possible FIFOs
			
		}

		// If there was no error, and there was no previous error (or the
		//  previous error was not cleared), then we are done here.  Except if this is a TF.
		if (allClear && !myCrate->isTrackFinder()) continue;

		// We have an error (or we are a TF)!

		// Read out the error information into a local variable.
		uint16_t errorData = 0;
		
		if (!myCrate->isTrackFinder()) { // Only try to read the IRQ for normal FED crates
			try {
				errorData = myCrate->getController()->readIRQ();
				MY_REVOKE_ALARM("IRQThreadReadIRQ");
			} catch (emu::fed::exception::CAENException &e) {
				std::ostringstream error;
				error << "Exception reading IRQ for crate number " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				tag << "FEDcrate " << crateNumber;
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadReadIRQ", "ERROR", error.str(), tag.str(), e);
				//XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				//e2.setProperty("tag", tag.str());
				//pthread_exit((void *) &e2);
				pthread_exit(NULL);
			}
		} else { // The TF crate has to fake this data for now.  Do that by manually specifying the slot.
			errorData = (2 << 8);
		}

		// In which slot did the error occur?  Get the DDU that matches.
		DDU *myDDU = NULL;
		unsigned int targetSlot = ((errorData & 0x1f00) >> 8);
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			if ((*iDDU)->slot() == targetSlot) {
				locdata->lastDDU[crateNumber] = (*iDDU)->slot();
				myDDU = (*iDDU);
				break;
			}
		}

		// Problem if there is no matching DDU...
		if (myDDU == NULL) {
			// Looks like this happens all the time.  Squelch errors.
			//LOG4CPLUS_FATAL(logger, "IRQ set from an unrecognized slot!  Crate " << myCrate->number() << " slot " << std::dec << (errorData[1] & 0x1f) << " error data " << std::hex << std::setw(2) << std::setfill('0') << (int) errorData[1] << std::setw(2) << std::setfill('0') << (int) errorData[0]);
			continue;
		}

		// Collect the present CSC status and store...
		try {
			unsigned int cscStatus = myDDU->readCSCStatus();
			unsigned int advStatus = myDDU->readAdvancedFiberErrors();
			unsigned int xorStatus = (cscStatus | advStatus)^lastDDUError[myDDU->slot()];
			
			// Moved this up and got rid of report so that the TF log doesn't explode
			if (!xorStatus) {
				//LOG4CPLUS_INFO(logger, "No CSC or DDU errors detected...  Ignoring interrupt");
				continue;
			}

			// What type of error did I see?
			bool hardError = (errorData & 0x8000);
			bool syncError = (errorData & 0x4000);

			// If the DDU wants a reset, it will request it (basically an OR of
			//  the two above values.)
			bool resetWanted = (errorData & 0x2000);

			// How many CSCs are in an error state on the given DDU?
			//unsigned int cscsWithHardError = ((errorData >> 4) & 0x000f);

			// How many CSCs are in a bad sync state on the given DDU?
			//unsigned int cscsWithSyncError = (errorData & 0x000f);

			// Log everything now.
			LOG4CPLUS_ERROR(logger, "Interrupt detected!");
			time_t theTime = time(NULL);
			LOG4CPLUS_ERROR(logger, " ErrorData " << std::dec << crateNumber << " " << myDDU->slot() << " " << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << cscStatus << " " << std::dec << (uintmax_t) theTime);

			std::stringstream fiberErrors, chamberErrors;
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				if (xorStatus & (1<<iFiber)) {
					fiberErrors << iFiber << " ";
					chamberErrors << myDDU->getFiber(iFiber)->getName() << " ";
				}
			}

			LOG4CPLUS_INFO(logger, "Decoded information follows" << std::endl
				<< "FEDCrate   : " << crateNumber << std::endl
				<< "Slot       : " << myDDU->slot() << std::endl
				<< "RUI        : " << myCrate->getRUI(myDDU->slot()) << std::endl
				<< "CSC Status : " << std::hex << cscStatus << std::endl
				<< "ADV Status : " << advStatus << std::endl
				<< "XOR Status : " << xorStatus << std::dec << std::endl
				<< "DDU error  : " << ((cscStatus & 0x8000) == 0x8000) << std::endl
				<< "Fibers     : " << fiberErrors.str() << std::endl
				<< "Chambers   : " << chamberErrors.str() << std::endl
				<< "Hard Error : " << hardError << std::endl
				<< "Sync Error : " << syncError << std::endl
				<< "Wants Reset: " << resetWanted);

			//LOG4CPLUS_INFO(logger, cscsWithHardError << " CSCs on this DDU have hard errors");
			//LOG4CPLUS_INFO(logger, cscsWithSyncError << " CSCs on this DDU have sync errors");

			// These only show up if there has been a hard error.
			if (hardError) {
				std::vector<std::string> trapInfo = DDUDebugger::DDUDebugTrap(myDDU->readDebugTrap(DDUFPGA), myDDU);
				std::ostringstream trapStream;
				for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
					trapStream << (*iTrap) << std::endl;
				}

				LOG4CPLUS_INFO(logger, "Logging DDUFPGA diagnostic trap information:" << std::endl << trapStream.str());

				trapInfo = DDUDebugger::INFPGADebugTrap(INFPGA0, myDDU->readDebugTrap(INFPGA0));
				trapStream.str("");
				for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
					trapStream << (*iTrap) << std::endl;
				}

				LOG4CPLUS_INFO(logger, "Logging INFPGA0 diagnostic trap information:" << std::endl << trapStream.str());

				trapInfo = DDUDebugger::INFPGADebugTrap(INFPGA1, myDDU->readDebugTrap(INFPGA1));
				trapStream.str("");
				for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
					trapStream << (*iTrap) << std::endl;
				}

				LOG4CPLUS_INFO(logger, "Logging INFPGA1 diagnostic trap information:" << std::endl << trapStream.str());
			}

			// Record the error in an accessable history of errors.
			lastDDUError[myDDU->slot()] = (cscStatus | advStatus);

			// Log all errors in persisting array...
			// I am not so worried about DDU-only errors, so I ignore iFiber==15
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				if (xorStatus & (1 << iFiber)) {
					locdata->errorCount[crateNumber]++;
					// Record the name of the fiber, and hope that I don't have a repeat here (I shouldn't)
					locdata->errorFiberNames[crateNumber].push_back(myDDU->getFiber(iFiber)->getName());
				}
			}
			// Just in case there is some bizarre error at this point that causes this to
			// overflow...
			if (locdata->errorCount[crateNumber] > 15) {
				locdata->errorCount[crateNumber] = 15;
			}
			locdata->lastDDU[crateNumber] = myDDU->slot();


			// Check to see if any of the fibers are troublesome and report
			unsigned int liveFibers = myDDU->readKillFiber();
			//LOG4CPLUS_DEBUG(logger, "Checking for problem fibers in crate " << myCrate->number() << " slot " << myDDU->slot());
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				std::ostringstream alarmName;
				alarmName << "IRQThreadFiber" << crateNumber << "_" << myDDU->slot() << "_" << iFiber;
				// Skip it if it is already killed or if it didn't cause a problem
				if (!(liveFibers & (1<<iFiber)) || !(xorStatus & (1<<iFiber))) {
					MY_REVOKE_ALARM(alarmName.str());
					continue;
				} else { // REPORT TO SENTENEL!
					std::ostringstream error;
					error << "Fiber error read on crate " << crateNumber << " slot " << myDDU->slot() << " fiber " << iFiber << " (" << myDDU->getFiber(iFiber)->getName() << ")";
					LOG4CPLUS_ERROR(logger, error.str());
					std::ostringstream tag;
					tag << "FEDcrate " << crateNumber << " RUI " << myDDU->readRUI() << " fiber " << std::setw(2) << std::setfill('0') << iFiber << " chamber " << myDDU->getFiber(iFiber)->getName();
					MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), "ERROR", error.str(), tag.str());
				}
			}

			// Discover the error counts of the other crates.
			unsigned long int totalChamberErrors = 0;
			for (std::map<unsigned int, unsigned long int>::iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); iCount++) {
				if (iCount->first != crateNumber) {
					LOG4CPLUS_INFO(logger,"Crate " << iCount->first << " reports " << iCount->second << " CSCs in an error state.");
				}
				totalChamberErrors += iCount->second;
			}

			// Check if we have sufficient error conditions to reset.
			if (totalChamberErrors > locdata->fmmErrorThreshold && !myCrate->isTrackFinder()) {
				LOG4CPLUS_INFO(logger, "A resync will be requested because the total number of CSCs in an error state on this system greater than " << locdata->fmmErrorThreshold);

				// I only have to do this to my crate:  eventually, a reset will come.
				myCrate->getBroadcastDDU()->enableFMM();
			}
		
			MY_REVOKE_ALARM("IRQThreadGeneralError");

		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception dealing with IRQ on crate " << crateNumber;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDcrate " << crateNumber;
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadGeneralError", "ERROR", error.str(), tag.str());
			//XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
			//e2.setProperty("tag", tag.str());
			//pthread_exit((void *) &e2);
			pthread_exit(NULL);
		}
	}
	
	// I will never make it here, but just in case.
	pthread_exit(PTHREAD_CANCELED);
}



