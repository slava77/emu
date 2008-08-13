#include "FEDCrate.h"
#include "VMEModule.h"
#include "VMEController.h"
#include "DDU.h"
#include "DCC.h"

int irqprob;
long int timer,xtimer;

FEDCrate::FEDCrate(int number, VMEController *theController):
	theNumber(number),
	theModules(31),
	theController(NULL)
{
	//Singleton<CrateSetup>::instance()->addCrate(number, this);
}


FEDCrate::~FEDCrate() {
	for(unsigned i = 0; i < theModules.size(); ++i) {
		delete theModules[i];
	}
	delete theController;
}


void FEDCrate::addModule(VMEModule *module) {
	module->setController(theController);
	theModules[module->slot()] = module;
}

void FEDCrate::setController(VMEController *controller) {
	if (theController != NULL) {
		std::cout << "WARNING: Trying change the VMEController of crate " << theNumber << std::endl;
	}
	std::cout << "Setting controller in crate " << theNumber << std::endl;
	theController = controller;
	theController->setCrate(theNumber);
	for (unsigned int i=0; i<theModules.size(); i++) {
		if (theModules[i] == NULL) continue;
		theModules[i]->setController(theController);
	}
}

std::vector<DDU *> FEDCrate::ddus() const {
  std::vector<DDU *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DDU * ddu = dynamic_cast<DDU *>(theModules[i]);
    if(ddu != 0) result.push_back(ddu);
  }
  return result;
}

std::vector<DCC *> FEDCrate::dccs() const {
  std::vector<DCC *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DCC * dcc = dynamic_cast<DCC *>(theModules[i]);
    if(dcc != 0) result.push_back(dcc);
  }
  return result;
}

int FEDCrate::getRUI(int slot) {
	unsigned int rui = 9 * theNumber + slot - 3;
	if (slot > 8) rui--;  // Correct for the DCC slot.
	if (theNumber > 0) rui -= 9; // Correct for the First FED Crate = Crate 1, but the Test FED Crate (0) will act like FED Crate 1 in this case.
	if (theNumber>4) rui = 0; // This is the TF DDU.

	return rui;
}

void FEDCrate::enable() {
  //
  std::cout << "FEDCrate::enable called " << std::endl;
}

//
void FEDCrate::disable() {
  //
  std::cout << "FEDCrate::disable called " << std::endl;
  //
}
//
void FEDCrate::configure() {
// JRG, downloads to all boards, then starts the IRQ handler.
	//printf(" ********   Crate::configure is called with run number %u \n",(unsigned int) runnumber);
	std::vector<DDU*> myDdus = this->ddus();
	for(unsigned i =0; i < myDdus.size(); ++i) {
		myDdus[i]->configure();
	}
	std::vector<DCC*> myDccs = this->dccs();
	for(unsigned i =0; i < myDccs.size(); ++i) {
		myDccs[i]->configure();
	}

// LSD, move IRQ start to Init phase:
// JRG, we probably want to keep IRQ clear/reset here (End, then Start):
// PGK, new objects (IRQThread) in town.  Use these instead.
// PGK, better yet, just call init.
//	std::cout << " ********   Crate::configure complete, running init..." << std::endl;
//	this->init(runnumber);
}

void FEDCrate::init() {
	// Does nothing.
}

