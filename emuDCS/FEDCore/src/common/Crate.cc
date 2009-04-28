/*****************************************************************************\
* $Id: Crate.cc,v 1.3 2009/04/28 02:05:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/Crate.h"

#include <sstream>

#include "emu/fed/VMEController.h"

emu::fed::Crate::Crate(unsigned int myNumber):
number_(myNumber)
{
	boardVector_.reserve(18);

	broadcastDDU_ = new DDU(28); // broadcast slot.
}



void emu::fed::Crate::addBoard(VMEModule *myBoard)
{
	if (vmeController_ != NULL) {
		myBoard->setBHandle(vmeController_->getBHandle());
	}
	boardVector_.push_back(myBoard);
}



void emu::fed::Crate::setController(VMEController *controller) {
	vmeController_ = controller;

	broadcastDDU_->setBHandle(vmeController_->getBHandle());

	for (std::vector<VMEModule *>::iterator iBoard = boardVector_.begin(); iBoard != boardVector_.end(); iBoard++) {
		(*iBoard)->setBHandle(vmeController_->getBHandle());
	}
}



int emu::fed::Crate::getRUI(const int slot) {
	// TF is special.
	if (number_ == 5) return 192;
	// Test crate is special
	if (number_ < 1 || number_ > 5) return 0;

	unsigned int rui = 9 * number_ + slot - 3;
	if (slot > 8) rui--;  // Correct for the DCC slot.
	if (number_ > 0) rui -= 9; // Correct for the First FED Crate = Crate 1, but the Test FED Crate (0) will act like FED Crate 1 in this case.
	if (number_ > 4) rui = 0; // This is the TF DDU.

	return rui;
}



void emu::fed::Crate::configure()
throw (emu::fed::exception::ConfigurationException)
{
	for(std::vector<VMEModule *>::iterator iBoard = boardVector_.begin(); iBoard != boardVector_.end(); iBoard++) {
		try {
			(*iBoard)->configure();
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Configuration of board in crate " << number_ << " slot " << (*iBoard)->slot() << " has failed";
			XCEPT_DECLARE_NESTED(emu::fed::exception::OutOfBoundsException, e2, error.str(), e);
			std::ostringstream tag;
			tag << "crate:" << number_ << ",slot:" << (*iBoard)->slot();
			e2.setProperty("tag", tag.str());
			throw e2;
		}
	}
}


