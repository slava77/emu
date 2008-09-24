/*****************************************************************************\
* $Id: FEDCrate.cc,v 1.10 2008/09/24 18:38:38 paste Exp $
*
* $Log: FEDCrate.cc,v $
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

#include <iostream>

#include "VMEModule.h"
#include "VMEController.h"
#include "DDU.h"
#include "DCC.h"

emu::fed::FEDCrate::FEDCrate(int myNumber):
	number_(myNumber)
{
	dduVector_.reserve(14);
	dccVector_.reserve(2);

	broadcastDDU_ = new DDU(28); // Slot > 21 => broadcast slot.
}


emu::fed::FEDCrate::~FEDCrate() {
	for(unsigned int iDDU = 0; iDDU < dduVector_.size(); iDDU++) {
		delete dduVector_[iDDU];
	}
	for(unsigned int iDCC = 0; iDCC < dccVector_.size(); iDCC++) {
		delete dccVector_[iDCC];
	}
	delete vmeController_;
}

/*
void emu::fed::FEDCrate::addModule(VMEModule *module) {
	module->setController(vmeController_);
	module->setBHandle(vmeController_->getBHandle());
	moduleVector_[module->slot()] = module;
}
*/
void emu::fed::FEDCrate::addDDU(DDU *myDDU) {
	//myDDU->setController(vmeController_);
	if (vmeController_ != NULL) {
		myDDU->setBHandle(vmeController_->getBHandle());
	}
	dduVector_.push_back(myDDU);
}



void emu::fed::FEDCrate::addDCC(DCC *myDCC) {
	//myDCC->setController(vmeController_);
	if (vmeController_ != NULL) {
		myDCC->setBHandle(vmeController_->getBHandle());
	}
	dccVector_.push_back(myDCC);
}



void emu::fed::FEDCrate::setController(VMEController *controller) {
	if (vmeController_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of crate " << number_ << std::endl;
	}

	vmeController_ = controller;
	//vmeController_->setCrate(number_);

	//broadcastDDU_->setController(vmeController_); // Will be removed later.
	broadcastDDU_->setBHandle(vmeController_->getBHandle());
	
	for(std::vector<DDU *>::iterator iDDU = dduVector_.begin(); iDDU != dduVector_.end(); iDDU++) {
		//(*iDDU)->setController(vmeController_);
		(*iDDU)->setBHandle(vmeController_->getBHandle());
	}
	for(std::vector<DCC *>::iterator iDCC = dccVector_.begin(); iDCC != dccVector_.end(); iDCC++) {
		//(*iDCC)->setController(vmeController_);
		(*iDCC)->setBHandle(vmeController_->getBHandle());
	}
}


void emu::fed::FEDCrate::setBHandle(int32_t BHandle) {
	if (vmeController_->getBHandle() != -1) {
		std::cout << "WARNING: Trying change the BHandle of crate " << number_ << std::endl;
	}
	std::cout << "Setting BHandle in crate " << number_ << std::endl;
	vmeController_->setBHandle(BHandle);
	//vmeController_->setCrate(number_);
	
	broadcastDDU_->setBHandle(vmeController_->getBHandle());
	
	for(std::vector<DDU *>::iterator iDDU = dduVector_.begin(); iDDU != dduVector_.end(); iDDU++) {
		(*iDDU)->setBHandle(vmeController_->getBHandle());
	}
	for(std::vector<DCC *>::iterator iDCC = dccVector_.begin(); iDCC != dccVector_.end(); iDCC++) {
		(*iDCC)->setBHandle(vmeController_->getBHandle());
	}
}



int emu::fed::FEDCrate::getRUI(int slot) {
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



//
void emu::fed::FEDCrate::configure() {
// JRG, downloads to all boards, then starts the IRQ handler.
	//printf(" ********   emu::fed::FEDCrate::configure is called with run number %u \n",(unsigned int) runnumber);
	for(std::vector<DDU *>::iterator iDDU = dduVector_.begin(); iDDU != dduVector_.end(); iDDU++) {
		(*iDDU)->configure();
	}
	for(std::vector<DCC *>::iterator iDCC = dccVector_.begin(); iDCC != dccVector_.end(); iDCC++) {
		(*iDCC)->configure();
	}

// LSD, move IRQ start to Init phase:
// JRG, we probably want to keep IRQ clear/reset here (End, then Start):
// PGK, new objects (IRQThread) in town.  Use these instead.
// PGK, better yet, just call init.
//	std::cout << " ********   emu::fed::FEDCrate::configure complete, running init..." << std::endl;
//	this->init(runnumber);
}


