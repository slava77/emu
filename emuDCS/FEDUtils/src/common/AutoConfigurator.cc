/*****************************************************************************\
* $Id: AutoConfigurator.cc,v 1.9 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/

#include "emu/fed/AutoConfigurator.h"

#include <sstream>
#include <unistd.h>
#include <time.h>
#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"

emu::fed::AutoConfigurator::AutoConfigurator()
{
	systemName_ = "Autoconfigured";
	timeStamp_ = time(NULL);
}



std::vector<emu::fed::Crate *> emu::fed::AutoConfigurator::setupCrates(const bool &fake)
throw (emu::fed::exception::ConfigurationException)
{
	timeStamp_ = time(NULL);
	
	// An incrementing crate number
	unsigned int crateNumber = 0;
	
	// Wipe present configuration
	crateVector_.clear();
	
	// For figuring out what system I am
	unsigned int ruiTotal = 0;
	
	// Discover which Device/Link pairs are active
	// 4 is a good number.  We shouldn't need any more than 4 devices or links per device.
	for (unsigned int iDevice = 0; iDevice < 4; iDevice++) {
		bool noLinks = false;
		for (unsigned int iLink = 0; iLink < 4; iLink++) {
			
			Crate *testCrate;
			try {
				testCrate = new Crate(crateNumber++);
			} catch (emu::fed::exception::Exception &e) {
				std::ostringstream error;
				error << "Unable to create crate: " << e.what();
				XCEPT_DECLARE_NESTED(emu::fed::exception::ConfigurationException, e2, error.str(), e);
				std::ostringstream tag;
				tag << "FEDCrate " << crateNumber;
				e2.setProperty("tag", tag.str());
				throw e2;
			}
			
			VMEController *testController;

			try {
				testController = new VMEController(iDevice, iLink, fake);
			} catch (emu::fed::exception::Exception &e) {
				// As soon as I can't find a link, I am done with this device
				// If this is the first link I am checking, then I am completely done.
				if (iLink == 0) noLinks = true;
				break;
			}
			
			testCrate->setController(testController);
			
			// Discover which slots are active
			for (unsigned int iSlot = 2; iSlot < 21; iSlot++) {

				// These will throw if there is no board at the given slot.
				try {
					DDU *testDDU = new DDU(iSlot, fake);
					testDDU->setBHandle(testController->getBHandle());
					testDDU->setMutex(testController->getMutex());
					
					// Gives nonsense for DCCs, but standard answer for DDUs
					uint32_t idCode = testDDU->readIDCode(VMEPROM);
					if (idCode == 0x05036093) {
						// Read all the normal configuration information from the board itself
						testDDU->setGbEPrescale(testDDU->readGbEPrescale());
						testDDU->setKillFiber((testDDU->readKillFiber() & 0xf7000) | (testDDU->readFlashKillFiber()));
						testDDU->setRUI(testDDU->readFlashRUI());
						ruiTotal += testDDU->getRUI();
						testCrate->addBoard(testDDU);
						continue;
					}
					delete testDDU;
					
					DCC *testDCC = new DCC(iSlot, fake);
					testDCC->setBHandle(testController->getBHandle());
					
					// Gives nonsense for DDUs, but standard answer for DCCs
					idCode = testDCC->readIDCode(MPROM);
					if (idCode == 0xf5059093) {
						// Read all the normal configuration information from the board itself
						testDCC->setFIFOInUse(testDCC->readFIFOInUse());
						testDCC->setSoftwareSwitch(testDCC->readSoftwareSwitch());
						testCrate->addBoard(testDCC);
						continue;
					}
					delete testDCC;
					
				} catch (emu::fed::exception::Exception &e) {
					// probe failed
					continue;
				}
				
			}
			
			// Save the crate
			crateVector_.push_back(testCrate);
		}
		
		if (noLinks) break;
		
	}
	
	// The only way this can fail is if I don't find any controllers attached to the machine
	if (crateVector_.size() == 0) {
		std::ostringstream error;
		error << "Could not auto-detect any controllers on this system.";
		XCEPT_RAISE(emu::fed::exception::ConfigurationException, error.str());
	}
	
	if (crateVector_.size() == 1) {
		// Am I a TrackFinder crate?
		std::vector<DDU *> dduVector = crateVector_[0]->getDDUs();
		std::vector<DCC *> dccVector = crateVector_[0]->getDCCs();
		if (crateVector_[0]->isTrackFinder()) {
			systemName_ = "Track-Finder";
			crateVector_[0]->setNumber(5);
		} else {
			systemName_ = "Test-Crate";
		}
	} else if (crateVector_.size() == 2) {
		
		bool isLiveSystem = false;
		unsigned int totalDDUs = 0;
		
		// Do I have the right combination of DCCs and DDUs to be an actual system crate?
		for (std::vector<Crate *>::const_iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
			unsigned int nDDUs = (*iCrate)->getDDUs().size();
			unsigned int nDCCs = (*iCrate)->getDCCs().size();
			if ((nDCCs == 1 && nDDUs == 9) || (nDCCs == 2 && nDDUs == 15)) {
				isLiveSystem = true;
				totalDDUs += nDDUs;
			} else {
				isLiveSystem = false; // One strike and you're out!
				break;
			}
		}
		
		if (isLiveSystem) {
			// No matter the possible RUI naming scheme in the SLHC, any sensible scheme will make this true
			if (ruiTotal / totalDDUs < 25) {
				systemName_ = "Plus-Side";
			} else {
				systemName_ = "Minus-Side";
			}
		} else {
			systemName_ = "Test-Crate";
		}
	}
	
	return crateVector_;
}
