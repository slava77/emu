/*****************************************************************************\
* $Id: FEDCrate.cc,v 1.5 2008/08/30 14:49:04 paste Exp $
*
* $Log: FEDCrate.cc,v $
* Revision 1.5  2008/08/30 14:49:04  paste
* Attempts to make VME work under the new design model where VMEModules take over for the VMEController.
*
* Revision 1.4  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
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

emu::fed::FEDCrate::FEDCrate(int myNumber, VMEController *myController):
	number_(myNumber),
	moduleVector_(31),
	vmeController_(myController)
{
	// Does nothing
}



emu::fed::FEDCrate::~FEDCrate() {
	delete vmeController_;
	for(unsigned i = 0; i < moduleVector_.size(); ++i) {
		delete moduleVector_[i];
	}
}



void emu::fed::FEDCrate::addModule(VMEModule *module) {
	if (vmeController_) {
		module->setController(vmeController_);
	}
	moduleVector_[module->slot()] = module;
}



void emu::fed::FEDCrate::setController(VMEController *controller) {
	if (vmeController_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of crate " << number_ << std::endl;
	}
	std::cout << "Setting controller in crate " << number_ << std::endl;
	vmeController_ = controller;
	//vmeController_->setCrate(number_);
	for (unsigned int i=0; i<moduleVector_.size(); i++) {
		if (moduleVector_[i] == NULL) continue;
		moduleVector_[i]->setController(vmeController_);
	}
}



void emu::fed::FEDCrate::setBHandle(int32_t myBHandle) {
	if (vmeController_->getBHandle() != -1) {
		std::cout << "WARNING: Trying change the BHandle associated with crate " << number_ << std::endl;
	}
	std::cout << "Setting BHandle in crate " << number_ << std::endl;
	if (vmeController_ != NULL) vmeController_->setBHandle(myBHandle);
}



std::vector<emu::fed::DDU *> emu::fed::FEDCrate::getDDUs() const {
	std::vector<DDU *> result;
	for(unsigned i = 0; i < moduleVector_.size(); ++i) {
		DDU *ddu = dynamic_cast<DDU *>(moduleVector_[i]);
		if(ddu != 0) result.push_back(ddu);
	}
	return result;
}



std::vector<emu::fed::DCC *> emu::fed::FEDCrate::getDCCs() const {
	std::vector<DCC *> result;
	for(unsigned i = 0; i < moduleVector_.size(); ++i) {
		DCC *dcc = dynamic_cast<DCC *>(moduleVector_[i]);
		if(dcc != 0) result.push_back(dcc);
	}
	return result;
}



int emu::fed::FEDCrate::getRUI(int slot) {
	unsigned int rui = 9 * number_ + slot - 3;
	if (slot > 8) rui--;  // Correct for the DCC slot.
	if (number_ > 0) rui -= 9; // Correct for the First FED Crate = Crate 1, but the Test FED Crate (0) will act like FED Crate 1 in this case.
	if (number_>4) rui = 0; // This is the TF DDU.

	return rui;
}



void emu::fed::FEDCrate::enable() {
	std::cout << "emu::fed::FEDCrate::enable called " << std::endl;
}



void emu::fed::FEDCrate::disable() {

	std::cout << "emu::fed::FEDCrate::disable called " << std::endl;
}



void emu::fed::FEDCrate::configure() {
// JRG, downloads to all boards, then starts the IRQ handler.
	//printf(" ********   emu::fed::FEDCrate::configure is called with run number %u \n",(unsigned int) runnumber);
	std::vector<DDU*> myDdus = this->getDDUs();
	for(unsigned i =0; i < myDdus.size(); ++i) {
		myDdus[i]->configure();
	}
	std::vector<DCC*> myDccs = this->getDCCs();
	for(unsigned i =0; i < myDccs.size(); ++i) {
		myDccs[i]->configure();
	}

// LSD, move IRQ start to Init phase:
// JRG, we probably want to keep IRQ clear/reset here (End, then Start):
// PGK, new objects (IRQThread) in town.  Use these instead.
// PGK, better yet, just call init.
//	std::cout << " ********   emu::fed::FEDCrate::configure complete, running init..." << std::endl;
//	this->init(runnumber);
}



void emu::fed::FEDCrate::init() {
	// Does nothing.
}
