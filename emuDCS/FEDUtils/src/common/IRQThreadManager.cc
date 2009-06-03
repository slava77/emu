/*****************************************************************************\
* $Id: IRQThreadManager.cc,v 1.5 2009/06/03 09:05:11 paste Exp $
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
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/IRQData.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"



emu::fed::IRQThreadManager::IRQThreadManager(xdaq::WebApplication *application, unsigned int fmmErrorThreshold):
systemName_(""),
fmmErrorThreshold_(fmmErrorThreshold),
application_(application)
{
	data_ = new IRQData(application_);
	threadVector_.clear();
}



emu::fed::IRQThreadManager::~IRQThreadManager()
{

	//endThreads();

}



void emu::fed::IRQThreadManager::attachCrate(Crate *crate)
{

	pthread_t threadID = 0;
	threadVector_.push_back(std::pair<Crate *, pthread_t>(crate, threadID));

}



void emu::fed::IRQThreadManager::startThreads(unsigned long int runNumber)
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
	fileName << "EmuFMMThread_" << (systemName_ != "" ? systemName_ + "_" : "") << datebuf << "_r" << std::setw(6) << std::setfill('0') << std::dec << runNumber;

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
	data_->exit = false;

	// First, load up the data_ object with the crates that I govern.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {
		data_->crateQueue.push(threadVector_[iThread].first);
	}

	// Next, execute the threads.

	// Check the crates to see if any of the DDUs are in an error state and
	//  make a note in the log.  This is because DDUs that are already throwing
	//  errors will not set interrupts, so we want to notify the user somehow
	//  about the possible problem.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {

		emu::fed::Crate *myCrate = threadVector_[iThread].first;
		std::vector<emu::fed::DDU *> dduVector = myCrate->getDDUs();
		for (std::vector<emu::fed::DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			unsigned int cscStatus = 0;
			try {
				cscStatus = (*iDDU)->readCSCStatus() | (*iDDU)->readAdvancedFiberErrors();
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
				for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
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

	if (data_->exit || threadVector_.size() == 0) {
		LOG4CPLUS_DEBUG(logger, "Threads already stopped.");
		return;
	}
	LOG4CPLUS_DEBUG(logger, "Gracefully killing off all threads.");

	data_->exit = true;


	// We probably do not need to return the status of the threads,
	//  but this may be used later for whatever reason.
	//std::vector<int> returnStatus;

	// The threads should be stopping now.  Let's join them.
	for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {
		void *tempException = NULL; // I have to do this for type safety.
		int err = pthread_join(threadVector_[iThread].second, &tempException); // Waits until the thread calls pthread_exit(void *return_status)

		if (err || tempException != NULL) {
			std::ostringstream error;
			error << "Exception joining IRQThread for crate " << threadVector_[iThread].first->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			std::ostringstream tag;
			tag << "FEDCrate " << threadVector_[iThread].first->number();
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadEnd", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE(emu::fed::exception::FMMThreadException, e2, error.str());
			e2.setProperty("tag", tag.str());
			throw e2;
		}

		LOG4CPLUS_INFO(logger, "Joined IRQThread " << iThread << " for crate " << threadVector_[iThread].first->number());

	}
	
	MY_REVOKE_ALARM("IRQThreadEnd");

	data_ = new IRQData(application_);
	threadVector_.clear();
}



/// The big one
void *emu::fed::IRQThreadManager::IRQThread(void *data)
{

	// Recast the void pointer as something more useful.
	IRQData *locdata = (IRQData *)data;
	
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

	// Knowing what DDUs we are talking to is useful as well.
	std::vector<DDU *> dduVector = myCrate->getDDUs();

	// This is when we started.  Don't know why this screws up sometimes...
	time_t tock;
	time(&tock);
	tm *tockInfo = localtime(&tock);
	std::string tockText(asctime(tockInfo));
	locdata->startTime[crateNumber] = tockText;

	// A local tally of what the last error on a given DDU was.
	std::map<unsigned int, int> lastError;

	// Continue unless someone tells us to stop.
	while (locdata->exit == false) {

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
		bool allClear = true;
		
		if (myCrate->number() != 5) { // The regular crates will just use the IRQ as normal
			try {
				allClear = myCrate->getController()->waitIRQ(5000);
				MY_REVOKE_ALARM("IRQThreadWait");
			} catch (emu::fed::exception::CAENException &e) {
				std::ostringstream error;
				error << "Exception waiting for IRQ in crate number " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				tag << "FEDcrate " << myCrate->number();
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadWait", "ERROR", error.str(), tag.str(), e);
				XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				e2.setProperty("tag", tag.str());
				pthread_exit((void *) &e2);
			}
		} else { // The TF crate will, for now, fake the IRQ and just sleep
			sleep((unsigned int) 5);
		}


		// If allClear is non-zero, then there was not an error.
		// If there was no error, check to see if we were in an error state
		//  before...
		if (allClear && locdata->errorCount[crateNumber] > 0) {

			// Get the last DDU to report an error
			DDU *myDDU = NULL;
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() == locdata->lastDDU[crateNumber]) {
					myDDU = (*iDDU);
					break;
				}
			}

			// If my status has cleared, then all is cool, right?
			//  Reset all my data.
			try {
				if (myDDU == NULL || (myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors()) < lastError[myDDU->slot()]) {
					LOG4CPLUS_INFO(logger, "Reset detected on crate " << crateNumber << ": checking again to make sure...");
					usleep(100);

					if (myDDU == NULL || (myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors()) < lastError[myDDU->slot()]) {
						LOG4CPLUS_INFO(logger, "Reset confirmed on crate " << crateNumber);
						LOG4CPLUS_ERROR(logger, " ErrorData RESET Detected" << std::endl);

						// Reset the total error count and the saved errors.
						locdata->errorCount[crateNumber] = 0;
						locdata->lastDDU[crateNumber] = 0;
						lastError.clear();
						
						MY_REVOKE_ALARM("IRQThread");
					} else {

						LOG4CPLUS_INFO(logger, "No reset.  Continuing as normal.");

					}

				}
			} catch (emu::fed::exception::Exception &e) {
				std::ostringstream error;
				error << "Exception reading last DDU status for crate number " << crateNumber;
				LOG4CPLUS_FATAL(logger, error.str());
				std::ostringstream tag;
				if (myDDU != NULL) tag << "RUI " << myDDU->readRUI() << " ";
				tag << "FEDcrate " << crateNumber;
				MY_RAISE_ALARM_NESTED(emu::fed::exception::FMMThreadException, "IRQThreadLastDDUStatus", "ERROR", error.str(), tag.str(), e);
				XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				e2.setProperty("tag", tag.str());
				pthread_exit((void *) &e2);
			}

			MY_REVOKE_ALARM("IRQThreadLastDDUStatus");
		}

		// If there was no error, and there was no previous error (or the
		//  previous error was not cleared), then do a DCS-like check of temps and voltages
		if (allClear) {
			
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				
				for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
					float myTemp = 999;
					try {
						myTemp = (*iDDU)->readTemperature(iTemp);
					} catch (emu::fed::exception::DDUException &e) {
						// 999 is large enough to make a noise.
					}
					std::ostringstream alarmName;
					alarmName << "IRQThreadTemp" << crateNumber << (*iDDU)->slot() << iTemp;
					std::string errorType;
					// Use the DDUDebugger to our advantage
					std::pair<std::string, std::string> debugged = DDUDebugger::Temperature(myTemp);
					std::string status = debugged.second;
					if (status == "error") { // error
						errorType = "ERROR";
					} else if (status == "warning") { // warning
						errorType = "WARN";
					} else { // everything is fine
						MY_REVOKE_ALARM(alarmName.str());
						continue;
					}
					
					// If we made it here, we have a problem
					std::ostringstream error;
					error << "Temperature " << status << " for crate " << crateNumber << " slot " << (*iDDU)->slot() << " sensor " << iTemp << ": " << myTemp;
					LOG4CPLUS_ERROR(logger, error.str());
					std::ostringstream tag;
					tag << "RUI " << (*iDDU)->readRUI() << " FEDcrate " << crateNumber;
					MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), errorType, error.str(), tag.str());
				}
				
				for (unsigned int iVolt = 0; iVolt < 4; iVolt++) {
					float myVolt = 9999;
					try {
						myVolt = (*iDDU)->readVoltage(iVolt);
					} catch (emu::fed::exception::DDUException &e) {
						// 9999 will make enough noise
					}

					std::ostringstream alarmName;
					alarmName << "IRQThreadVolt" << crateNumber << (*iDDU)->slot() << iVolt;
					std::string errorType;
					// Use the DDUDebugger to our advantage
					std::pair<std::string, std::string> debugged = DDUDebugger::Voltage(iVolt, myVolt);
					std::string status = debugged.second;
					if (status == "error") { // error
						errorType = "ERROR";
					} else if (status == "warning") { // warning
						errorType = "WARN";
					} else { // everything is fine
						MY_REVOKE_ALARM(alarmName.str());
						continue;
					}
					
					// If we made it here, we have a problem
					std::ostringstream error;
					error << "Voltage " << status << " for crate " << crateNumber << " slot " << (*iDDU)->slot() << " sensor " << iVolt << ": " << myVolt;
					LOG4CPLUS_ERROR(logger, error.str());
					std::ostringstream tag;
					tag << "RUI " << (*iDDU)->readRUI() << " FEDcrate " << crateNumber;
					MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, alarmName.str(), errorType, error.str(), tag.str());
				}
				
			}
			
			// This is the end of non-TF checks, but the TF has to fake the IRQ data for now
			if (myCrate->number() != 5) continue;
		}

		// We have an error!

		// Read out the error information into a local variable.
		uint16_t errorData = 0;
		
		if (myCrate->number() == 5) { // Only try to read the IRQ for normal FED crates
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
				XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				e2.setProperty("tag", tag.str());
				pthread_exit((void *) &e2);
			}
		} else { // The TF crate has to fake this data for now
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
			unsigned int xorStatus = (cscStatus | advStatus)^lastError[myDDU->slot()];
			
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
			unsigned int cscsWithHardError = ((errorData >> 4) & 0x000f);

			// How many CSCs are in a bad sync state on the given DDU?
			unsigned int cscsWithSyncError = (errorData & 0x000f);

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

			LOG4CPLUS_INFO(logger, cscsWithHardError << " CSCs on this DDU have hard errors");
			LOG4CPLUS_INFO(logger, cscsWithSyncError << " CSCs on this DDU have sync errors");

			std::vector<std::string> trapInfo = DDUDebugger::DDUDebugTrap(myDDU->readDebugTrap(DDUFPGA), myDDU);
			std::ostringstream trapStream;
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}

			LOG4CPLUS_INFO(logger, "Logging DDUFPGA diagnostic trap information:" << std::endl << trapStream.str());

			trapInfo = DDUDebugger::INFPGADebugTrap(myDDU->readDebugTrap(INFPGA0), INFPGA0);
			trapStream.str("");
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}

			LOG4CPLUS_INFO(logger, "Logging INFPGA0 diagnostic trap information:" << std::endl << trapStream.str());

			trapInfo = DDUDebugger::INFPGADebugTrap(myDDU->readDebugTrap(INFPGA1), INFPGA1);
			trapStream.str("");
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}

			LOG4CPLUS_INFO(logger, "Logging INFPGA1 diagnostic trap information:" << std::endl << trapStream.str());

			// Record the error in an accessable history of errors.
			lastError[myDDU->slot()] = (cscStatus | advStatus);

			// Log all errors in persisting array...
			// PGK I am not so worried about DDU-only errors...
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				if (xorStatus & (1<<iFiber)) {
					locdata->errorCount[crateNumber]++;
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
				alarmName << "IRQThreadFiber" << crateNumber << myDDU->slot() << iFiber;
				// Skip it if it is already killed or if it didn't cause a problem
				if (!(liveFibers & (1<<iFiber)) || !(xorStatus & (1<<iFiber))) {
					MY_REVOKE_ALARM(alarmName.str());
					continue;
				} else { // REPORT TO SENTENEL!
					std::ostringstream error;
					error << "Fiber error read on crate " << crateNumber << " slot " << myDDU->slot() << " fiber " << iFiber << " chamber " << myDDU->getFiber(iFiber)->getName();
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
			if (totalChamberErrors > locdata->fmmErrorThreshold) {
				LOG4CPLUS_INFO(logger, "A resync will be requested because the total number of CSCs in an error state on this system greater than " << locdata->fmmErrorThreshold);

				// I only have to do this to my crate:  eventually, a reset will come.
				myCrate->getBroadcastDDU()->enableFMM();
			}
		
			MY_REVOKE_ALARM("IRQThreadGeneralError");

		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception dealing with IRQ on crate " << crateNumber;
			LOG4CPLUS_ERROR(logger, error.str());
			std::ostringstream tag;
			tag << "FEDcrate " << crateNumber;
			MY_RAISE_ALARM(emu::fed::exception::FMMThreadException, "IRQThreadGeneralError", "ERROR", error.str(), tag.str());
			XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
			e2.setProperty("tag", tag.str());
			pthread_exit((void *) &e2);
		}
	}
	pthread_exit(NULL);
}



