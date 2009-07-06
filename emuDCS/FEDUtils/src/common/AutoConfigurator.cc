/*****************************************************************************\
* $Id: AutoConfigurator.cc,v 1.4 2009/07/06 15:59:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/AutoConfigurator.h"

#include <sstream>
#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"

emu::fed::AutoConfigurator::AutoConfigurator()
{
	systemName_ = "Autoconfigured";
	// Does nothing (nothing needed for auto configuration)
}



std::vector<emu::fed::Crate *> emu::fed::AutoConfigurator::setupCrates()
throw (emu::fed::exception::ConfigurationException)
{
	// Wipe present configuration
	crateVector_.clear();
	
	// For figuring out what system I am
	unsigned int ruiTotal = 0;
	
	// Discover which Device/Link pairs are active
	// 4 is a good number.  We shouldn't need any more than 4 devices or links per device.
	for (unsigned int iDevice = 0; iDevice < 4; iDevice++) {
		for (unsigned int iLink = 0; iLink < 4; iLink++) {
			
			Crate *testCrate = new Crate(0);
			VMEController *testController;
			
			// If this throws, then this is not a legal device/link combination
			try {
				testController = new VMEController(iDevice, iLink);
			} catch (emu::fed::exception::Exception &e) {
				continue;
			}
			
			testCrate->setController(testController);
			
			// Discover which slots are active
			for (unsigned int iSlot = 2; iSlot < 21; iSlot++) {

				// These will throw if there is no board at the given slot.
				try {
					DDU *testDDU = new DDU(iSlot);
					testDDU->setBHandle(testController->getBHandle());
					
					// Gives nonsense for DCCs, but standard answer for DDUs
					uint32_t idCode = testDDU->readIDCode(VMEPROM);
					if (idCode == 0x05036093) {
						// Read all the normal configuration information from the board itself
						testDDU->gbe_prescale_ = testDDU->readGbEPrescale();
						testDDU->killfiber_ = (testDDU->readKillFiber() & 0xf7000) | (testDDU->readFlashKillFiber());
						ruiTotal += testDDU->readFlashRUI();
						testCrate->addBoard(testDDU);
						continue;
					}
					delete testDDU;
					
					DCC *testDCC = new DCC(iSlot);
					testDCC->setBHandle(testController->getBHandle());
					
					// Gives nonsense for DDUs, but standard answer for DCCs
					idCode = testDCC->readIDCode(MPROM);
					if (idCode == 0xf5059093) {
						// Read all the normal configuration information from the board itself
						testDCC->fifoinuse_ = testDCC->readFIFOInUse();
						testDCC->softsw_ = testDCC->readSoftwareSwitch();
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
		if (dduVector.size() == 1 && dccVector.size() == 0 && dduVector[0]->slot() == 2) {
			systemName_ = "Track-Finder";
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
