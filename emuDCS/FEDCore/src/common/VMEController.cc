//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEController.cc,v 1.11 2009/12/10 16:24:29 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEController.h"

#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdlib>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "emu/fed/JTAG_constants.h"

emu::fed::VMEController::VMEController(const int &Device, const int &Link, const bool &fake)
throw (emu::fed::exception::CAENException):
Device_(Device),
Link_(Link),
BHandle_(-1),
fake_(true)
{

	try {
		setFake(fake);
	} catch (emu::fed::exception::CAENException &e) {
		throw;
	}

}



emu::fed::VMEController::~VMEController() {

}



bool emu::fed::VMEController::waitIRQ(const unsigned int &mSecs)
throw (emu::fed::exception::CAENException)
{
	if (!fake_) {
		// If the BHandle is not set properly, just return a good signal (true)
		if (getBHandle() < 0) return true;
		
		CVErrorCodes err = CAENVME_IRQEnable(getBHandle(), cvIRQ1);
		
		if (err != cvSuccess) {
			std::ostringstream error;
			error << "Exception in waitIRQ: " << CAENVME_DecodeError(err);
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			throw e2;
		}
		
		return CAENVME_IRQWait(getBHandle(), cvIRQ1, mSecs);
		
	} else {
		// Determine if we are going to fake an error or not.
		srand(time(NULL));
		if (rand() % 10 < 5) {
			usleep(mSecs * 1000);
			return true;
		}
		
		usleep((rand() % mSecs) * 1000);
		return false;
	}
}



uint16_t emu::fed::VMEController::readIRQ()
throw (emu::fed::exception::CAENException)
{
	if (!fake_) {
		// If the BHandle is not set properly, return nothing
		if (getBHandle() < 0) return 0;
		
		uint16_t errorData = 0;
		
		CVErrorCodes err = CAENVME_IACKCycle(getBHandle(), cvIRQ1, &errorData, cvD16);
		
		if (err != cvSuccess) {
			std::ostringstream error;
			error << "Exception in readIRQ: " << CAENVME_DecodeError(err);
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			throw e2;
		}
		
		return errorData;
	
	} else {
		srand(time(NULL));
		return (uint16_t) rand() % 0xffff;
	}
}



void emu::fed::VMEController::setFake(const bool &fake)
throw (emu::fed::exception::CAENException)
{
	
	if (!fake) {
		CVBoardTypes VMEBoard = cvV2718;
		
		int32_t BHandle = 0;
		
		CVErrorCodes err = CAENVME_Init(VMEBoard, getDevice(), getLink(), &BHandle);
		
		// The file that will hold the BHandle.  It might already exist and have a valid handle, but we will see later.
		std::ostringstream fileName;
		fileName << "/tmp/CAEN_" << getDevice() << "_" << getLink() << ".BHandle";
		
		// Check to see if the board has been initialized.
		if (err == cvGenericError) {
			// If this failed, then maybe some other process has already opened the device.
			// There should be a file that has the BHandle in it.
			std::ifstream inFile(fileName.str().c_str());
			if (inFile.is_open()) {
				inFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
				try {
					inFile >> BHandle;
					setBHandle(BHandle);
				} catch (std::ifstream::failure &e) {
					inFile.close();
					std::ostringstream error;
					error << "Failure extracting pre-opened BHandle from file " << fileName.str();
					XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
					throw e2;
				}
				inFile.close();
			} else {
				std::ostringstream error;
				error << "Encountered CAEN bus error, but no open BHandles detected in file " << fileName;
				XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
				throw e2;
			}
			
		} else if (err != cvSuccess) {
			std::ostringstream error;
			error << "error " << err << ": " << CAENVME_DecodeError(err);
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			throw e2;
			
		} else {
			setBHandle(BHandle);
			
			// Now that you own the BHandle, make a file that shows this
			std::ofstream outFile(fileName.str().c_str(), std::ios_base::trunc);
			if (outFile.is_open()) {
				outFile << getBHandle();
				outFile.close();
				
				chmod(fileName.str().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			} else {
				std::ostringstream error;
				error << "Unable to save BHandle information to external file " << fileName.str();
				XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
				throw e2;
			}
		}
		
		// I can use the device and link numbers to make a unique lock file name for this controller on this manchine.
		std::ostringstream lockName;
		lockName << "/tmp/CAEN_" << getDevice() << "_" << getLink() << ".lock";
		try {
			mutex_ = new VMELock(lockName.str());
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Unable to create mutex: " << e.what();
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}
		
	} else {
		
		srand(time(NULL));
		unsigned int BHandle = rand();
		setBHandle(BHandle);
		
		std::ostringstream fileName;
		fileName << "/tmp/CAEN_" << getDevice() << "_" << getLink() << ".FakeBHandle";
		
		// Now that you own the BHandle, make a file that shows this
		std::ofstream outFile(fileName.str().c_str(), std::ios_base::trunc);
		if (outFile.is_open()) {
			outFile << getBHandle();
			outFile.close();
			
			chmod(fileName.str().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		} // I don't care if this doesn't work.
		
		// I can use the device and link numbers to make a unique lock file name for this controller on this manchine.
		std::ostringstream lockName;
		lockName << "/tmp/CAEN_" << getDevice() << "_" << getLink() << ".fakelock";
		try {
			mutex_ = new VMELock(lockName.str());
		} catch (emu::fed::exception::Exception &e) {
			// Don't care
		}
		
	}
	
	fake_ = fake;
	
}
