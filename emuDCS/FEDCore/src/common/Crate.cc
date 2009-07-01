/*****************************************************************************\
* $Id: Crate.cc,v 1.7 2009/07/01 14:17:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/Crate.h"

#include <sstream>

#include "emu/fed/VMEController.h"

emu::fed::Crate::Crate(const unsigned int &myNumber):
number_(myNumber)
{
	boardVector_.reserve(18);

	broadcastDDU_ = new DDU(28); // broadcast slot.
}



emu::fed::Crate::~Crate()
{
	delete broadcastDDU_;
	delete vmeController_;
}



void emu::fed::Crate::addBoard(VMEModule *myBoard)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (boardVector_.size() == 18) {
		std::ostringstream error;
		error << "The physical limitations of FED crates limit the number of boards to 18";
		XCEPT_DECLARE(emu::fed::exception::ConfigurationException, e2, error.str());
		std::ostringstream tag;
		tag << "FEDCrate " << number_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
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



uint16_t emu::fed::Crate::getRUI(const int &slot) {
	// TF is special.
	if (number_ == 5) return 192;
	// Test crate is special
	if (number_ < 1 || number_ > 5) return 0;

	uint16_t rui = 9 * number_ + slot - 3;
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
			XCEPT_DECLARE_NESTED(emu::fed::exception::ConfigurationException, e2, error.str(), e);
			std::ostringstream tag;
			tag << "FEDCrate " << number_;
			e2.setProperty("tag", tag.str());
			throw e2;
		}
	}
}


