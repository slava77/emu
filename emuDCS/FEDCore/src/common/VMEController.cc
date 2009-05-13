//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEController.cc,v 1.3 2009/05/13 06:52:00 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEController.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "emu/fed/VMEModule.h"

emu::fed::VMEController::VMEController(int Device, int Link)
throw (emu::fed::exception::CAENException):
Device_(Device),
Link_(Link),
BHandle_(-1)
{
	// Initialize mutexes
	pthread_mutex_init(&mutex_, NULL);

	CVBoardTypes VMEBoard = cvV2718;

	int32_t BHandle;

	CVErrorCodes err = CAENVME_Init(VMEBoard, Device_, Link_, &BHandle);

	// Check to see if the board has been initialized.
	if (err == cvGenericError) {
		// If this failed, then maybe some other process has already opened the device.
		// There should be a file that has the BHandle in it.
		std::ostringstream fileName;
		fileName << "CAEN_" << Device_ << "_" << Link_ << ".BHandle";
		std::ifstream inFile(fileName.str().c_str());
		if (inFile.is_open()) {
			inFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
			try {
				inFile >> BHandle_;
			} catch (std::ifstream::failure &e) {
				inFile.close();
				std::ostringstream error;
				error << "Failure extracting pre-opened BHandle from file " << fileName;
				XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
				std::ostringstream tag;
				tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
				e2.setProperty("tag", tag.str());
				throw e2;
			}
			inFile.close();
		} else {
			std::ostringstream error;
			error << "Encountered CAEN bus error, but no open BHandles detected in file " << fileName;
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			std::ostringstream tag;
			tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
			e2.setProperty("tag", tag.str());
			throw e2;
		}

	} else if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
		e2.setProperty("tag", tag.str());
		throw e2;

	} else {
		BHandle_ = BHandle;

		// Now that you own the BHandle, make a file that shows this
		std::ostringstream fileName;
		fileName << "CAEN_" << Device_ << "_" << Link_ << ".BHandle";
		std::ofstream outFile(fileName.str().c_str(), std::ios_base::trunc);
		if (outFile.is_open()) {
			outFile << BHandle_;
			outFile.close();
			
			chmod(fileName.str().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		} else {
			std::ostringstream error;
			error << "Unable to save BHandle information to external file " << fileName;
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			std::ostringstream tag;
			tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
			e2.setProperty("tag", tag.str());
			throw e2;
		}
	}

}



emu::fed::VMEController::~VMEController() {
	// This will free the resources in the driver, but that is not what we want to do, because
	// multiple processes might be talking to the same controller.
	//CAENVME_End(BHandle_);
}



bool emu::fed::VMEController::waitIRQ(unsigned int mSecs)
throw (emu::fed::exception::CAENException)
{
	// If the BHandle is not set properly, just return a good signal (true)
	if (BHandle_ < 0) return true;

	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IRQEnable(BHandle_, cvIRQ1);
	pthread_mutex_unlock(&mutex_);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in waitIRQ: " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	pthread_mutex_lock(&mutex_);
	bool status = CAENVME_IRQWait(BHandle_, cvIRQ1, mSecs);
	pthread_mutex_unlock(&mutex_);
	return status;
}



uint16_t emu::fed::VMEController::readIRQ()
throw (emu::fed::exception::CAENException)
{
	// If the BHandle is not set properly, return nothing
	if (BHandle_ < 0) return 0;

	uint16_t errorData;

	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IACKCycle(BHandle_, cvIRQ1, &errorData, cvD16);
	pthread_mutex_unlock(&mutex_);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in readIRQ: " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "device:" << Device_ << ",link:" << Link_ << ",bhandle:" << BHandle_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	return errorData;
}

