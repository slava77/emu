//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEController.cc,v 3.23 2009/02/04 18:28:11 paste Exp $
*
* $Log: VMEController.cc,v $
* Revision 3.23  2009/02/04 18:28:11  paste
* Updated for 6.10 release.  Fixed some obvious bugs.  Still problems with EmuFCrateHyperDAQ display.
*
* Revision 3.22  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.21  2008/10/01 14:10:04  paste
* Fixed phantom reset bug in IRQ threads and shifted IRQ handling functions to VMEController object.
*
* Revision 3.20  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.19  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.18  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.17  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.16  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.15  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.14  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.13  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.12  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.11  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEController.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "VMEModule.h"

emu::fed::VMEController::VMEController(int Device, int Link)
throw (CAENException):
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
				XCEPT_RAISE(CAENException, error.str());
			}
			inFile.close();
		} else {
			std::ostringstream error;
			error << "Encountered CAEN bus error, but no open BHandles detected in file " << fileName;
			XCEPT_RAISE(CAENException, error.str());
		}
		
	} else if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		XCEPT_RAISE(CAENException, error.str());
		
	} else {
		BHandle_ = BHandle;
		
		// Now that you own the BHandle, make a file that shows this
		std::ostringstream fileName;
		fileName << "CAEN_" << Device_ << "_" << Link_ << ".BHandle";
		std::ofstream outFile(fileName.str().c_str(), std::ios_base::trunc);
		if (outFile.is_open()) {
			outFile << BHandle_;
			outFile.close();
		} else {
			std::ostringstream error;
			error << "Unable to save BHandle information to external file " << fileName;
			XCEPT_RAISE(CAENException, error.str());
		}
	}

}



emu::fed::VMEController::~VMEController() {
	// This will free the resources in the driver, but that is not what we want to do, because
	// multiple processes might be talking to the same controller.
	//CAENVME_End(BHandle_);
}



bool emu::fed::VMEController::waitIRQ(unsigned int mSecs)
throw (CAENException)
{
	// If the BHandle is not set properly, just return a good signal (true)
	if (BHandle_ < 0) return true;
	
	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IRQEnable(BHandle_, cvIRQ1);
	pthread_mutex_unlock(&mutex_);
	
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(CAENException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	pthread_mutex_lock(&mutex_);
	bool status = CAENVME_IRQWait(BHandle_, cvIRQ1, mSecs);
	pthread_mutex_unlock(&mutex_);
	return status;
}



uint16_t emu::fed::VMEController::readIRQ()
throw (CAENException)
{
	// If the BHandle is not set properly, return nothing
	if (BHandle_ < 0) return 0;
	
	uint16_t errorData;
	
	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IACKCycle(BHandle_, cvIRQ1, &errorData, cvD16);
	pthread_mutex_unlock(&mutex_);
	
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(CAENException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	return errorData;
}

