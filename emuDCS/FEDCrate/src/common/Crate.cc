#include "Crate.h"
#include "VMEModule.h"
#include "VMEController.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "DDU.h"
#include "DCC.h"

int irqprob;
long int timer,xtimer;

Crate::Crate(int number, VMEController *theController):
	theNumber(number),
	theModules(31),
	theController(NULL)
{
	Singleton<CrateSetup>::instance()->addCrate(number, this);
}


Crate::~Crate() {
	for(unsigned i = 0; i < theModules.size(); ++i) {
		delete theModules[i];
	}
	delete theController;
}


void Crate::addModule(VMEModule *module) {
	module->setController(theController);
	theModules[module->slot()] = module;
}

void Crate::setController(VMEController *controller) {
	if (theController != NULL) {
		cout << "WARNING: Trying change the VMEController of crate " << theNumber << endl;
	}
	cout << "Setting controller in crate " << theNumber << endl;
	theController = controller;
	theController->setCrate(theNumber);
	for (unsigned int i=0; i<theModules.size(); i++) {
		if (theModules[i] == NULL) continue;
		theModules[i]->setController(theController);
	}
}

vector<DDU *> Crate::ddus() const {
  vector<DDU *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DDU * ddu = dynamic_cast<DDU *>(theModules[i]);
    if(ddu != 0) result.push_back(ddu);
  }
  return result;
}

vector<DCC *> Crate::dccs() const {
  vector<DCC *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DCC * dcc = dynamic_cast<DCC *>(theModules[i]);
    if(dcc != 0) result.push_back(dcc);
  }
  return result;
}

int Crate::getRUI(int slot) {
	unsigned int rui = 9 * theNumber + slot - 3;
	if (slot > 8) rui--;  // Correct for the DCC slot.
	if (theNumber > 0) rui -= 9; // Correct for the First FED Crate = Crate 1, but the Test FED Crate (0) will act like FED Crate 1 in this case.
	if (theNumber>4) rui = 0; // This is the TF DDU.

	return rui;
}

void Crate::enable() {
  //
  std::cout << "Crate::enable called " << std::endl;
}

//
void Crate::disable() {
  //
  std::cout << "Crate::disable called " << std::endl;
  //
}
//
void Crate::configure(long unsigned int runnumber = 0) {
// JRG, downloads to all boards, then starts the IRQ handler.
	printf(" ********   Crate::configure is called with run number %u \n",(unsigned int) runnumber);
	std::vector<DDU*> myDdus = this->ddus();
	for(unsigned i =0; i < myDdus.size(); ++i) {
		myDdus[i]->configure();
	}
	std::vector<DCC*> myDccs = this->dccs();
	for(unsigned i =0; i < myDccs.size(); ++i) {
		myDccs[i]->configure();
	}

	// PGK Once configured, we need to hard-reset through the DCC.
	//  Contact Stan Durkin for questions.
	if (myDccs.size()) {
		myDccs[0]->mctrl_swset(0x1000);
		myDccs[0]->mctrl_ttccmd(0x34);
		myDccs[0]->mctrl_ttccmd(0x3);
		myDccs[0]->mctrl_swset(0x0000);
	}

// LSD, move IRQ start to Init phase:
// JRG, we probably want to keep IRQ clear/reset here (End, then Start):
// PGK, new objects (IRQThread) in town.  Use these instead.
// PGK, better yet, just call init.
//	cout << " ********   Crate::configure complete, running init..." << endl;
//	this->init(runnumber);
}

void Crate::init(long unsigned int runnumber = 0) {
	// Does nothing.
}

