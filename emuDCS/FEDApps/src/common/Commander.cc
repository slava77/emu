/*****************************************************************************\
* $Id: Commander.cc,v 1.2 2009/04/28 02:23:43 paste Exp $
\*****************************************************************************/
#include "emu/fed/Commander.h"

#include <iostream>
#include <iomanip>

#include "xgi/Method.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/Crate.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Commander)

emu::fed::Commander::Commander(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
Application(stub)
{
	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Commander::webDefault, "Default");
	//xgi::bind(this, &emu::fed::Commander::getAJAX, "getAJAX");
}



void emu::fed::Commander::webDefault(xgi::Input *in, xgi::Output *out)
{
	
	configure();
	
}



void emu::fed::Commander::configure()
throw(emu::fed::exception::SoftwareException)
{
	// Wipe present configuration
	crateVector_.clear();
	
	// Discover which Device/Link pairs are active
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Finding CAEN devices...");
	// 16 is a good number.  We shouldn't need any more than 4.
	for (unsigned int iDevice = 0; iDevice < 4; iDevice++) {
		for (unsigned int iLink = 0; iLink < 4; iLink++) {
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Probing Device " << iDevice << " Link " << iLink << "...");
			
			Crate *testCrate = new Crate(0);
			VMEController *testController;
			
			try {
				testController = new VMEController(iDevice, iLink);
			} catch (emu::fed::exception::Exception &e) {
				LOG4CPLUS_DEBUG(getApplicationLogger(), "Probe of Device " << iDevice << " Link " << iLink << " failed");
				continue;
			}
			
			LOG4CPLUS_DEBUG(getApplicationLogger(), "Device " << iDevice << " Link " << iLink << " returned BHandle " << testController->getBHandle());
			testCrate->setController(testController);
			
			// Discover which slots are active
			for (unsigned int iSlot = 2; iSlot < 21; iSlot++) {
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "Probing Slot " << iSlot << "...");
				
				// These will throw if there is no board at the given slot.
				try {
					DDU *testDDU = new DDU(iSlot);
					testDDU->setBHandle(testController->getBHandle());
					
					// Gives nonsense for DCC
					uint32_t idCode = testDDU->readIDCode(VMEPROM);
					if (idCode == 0x05036093) {
						uint16_t rui = testDDU->readFlashRUI();
						LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU RUI " << rui << " discovered in Slot " << iSlot);
						testCrate->addBoard(testDDU);
						continue;
					}
					
					DCC *testDCC = new DCC(iSlot);
					testDCC->setBHandle(testController->getBHandle());
					
					// Gives nonsense for DDU
					idCode = testDCC->readIDCode(MPROM);
					if (idCode == 0xf5059093) {
						LOG4CPLUS_TRACE(getApplicationLogger(), "DCC discovered in Slot " << iSlot);
						testCrate->addBoard(testDCC);
						continue;
					}
					
				} catch (emu::fed::exception::Exception &e) {
					LOG4CPLUS_DEBUG(getApplicationLogger(), "Probe of Slot " << iSlot << " failed");
				}
				
			}
			
			// Save the crate
			crateVector_.push_back(testCrate);
			
		}
	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Found " << crateVector_.size() << " crate" << (crateVector_.size() == 1 ? "" : "s"));
}
