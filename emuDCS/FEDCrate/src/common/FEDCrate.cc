/*****************************************************************************\
* $Id: FEDCrate.cc,v 1.11 2009/01/29 15:31:24 paste Exp $
*
* $Log: FEDCrate.cc,v $
* Revision 1.11  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.10  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 1.9  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 1.8  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.7  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 1.6  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 1.3  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "FEDCrate.h"

#include <sstream>

#include "VMEController.h"

emu::fed::FEDCrate::FEDCrate(unsigned int myNumber):
number_(myNumber)
{
	boardVector_.reserve(18);

	broadcastDDU_ = new DDU(28); // broadcast slot.
}



void emu::fed::FEDCrate::addBoard(VMEModule *myBoard)
{
	if (vmeController_ != NULL) {
		myBoard->setBHandle(vmeController_->getBHandle());
	}
	boardVector_.push_back(myBoard);
}



void emu::fed::FEDCrate::setController(VMEController *controller) {

	vmeController_ = controller;

	broadcastDDU_->setBHandle(vmeController_->getBHandle());
	
	for(std::vector<VMEModule *>::iterator iBoard = boardVector_.begin(); iBoard != boardVector_.end(); iBoard++) {
		(*iBoard)->setBHandle(vmeController_->getBHandle());
	}
}



int emu::fed::FEDCrate::getRUI(const int slot) {
	// TF is special.
	if (number_ == 5) return 192;
	// Test crate is special
	if (number_ < 1 || number_ > 5) return 0;
	
	unsigned int rui = 9 * number_ + slot - 3;
	if (slot > 8) rui--;  // Correct for the DCC slot.
	if (number_ > 0) rui -= 9; // Correct for the First FED Crate = Crate 1, but the Test FED Crate (0) will act like FED Crate 1 in this case.
	if (number_>4) rui = 0; // This is the TF DDU.

	return rui;
}



void emu::fed::FEDCrate::configure()
throw (ConfigurationException)
{
	for(std::vector<VMEModule *>::iterator iBoard = boardVector_.begin(); iBoard != boardVector_.end(); iBoard++) {
		try {
			(*iBoard)->configure();
		} catch (emu::fed::Exception &e) {
			std::ostringstream error;
			error << "Configuration of board in crate " << number_ << " slot " << (*iBoard)->slot() << " has failed";
			XCEPT_RETHROW(ConfigurationException, error.str(), e);
		}
	}
}


