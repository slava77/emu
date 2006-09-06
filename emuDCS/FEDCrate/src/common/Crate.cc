#include "Crate.h"
#include "VMEModule.h"
#include "VMEController.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "DDU.h"
#include "DCC.h"

int irqprob;
long int timer,xtimer;

Crate::Crate(int number, VMEController * controller) : 
  theNumber(number),  
  theModules(31),
  theController(controller) 
{
  Singleton<CrateSetup>::instance()->addCrate(number, this);
}


Crate::~Crate() {
  for(unsigned i = 0; i < theModules.size(); ++i) {
    delete theModules[i];
  }
  delete theController;
}


void Crate::addModule(VMEModule * module) {
  theModules[module->slot()] = module;
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
void Crate::configure(int crate) {
// JRG, downloads to all boards, then starts the IRQ handler.
  printf(" ********   Crate::configure is called %d \n",crate);
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
  printf(" Crate::configure: theController->vmeirq_start_=%d.  Calling irq+pthread_end \n", theController->vmeirq_start_);
  theController->irq_pthread_end(crate);
  irqprob=0;
  timer=0;
  xtimer=0;

  if(theController->vmeirq_start_==1){
    printf(" Crate::configure: theController->vmeirq_start_ %d, now irq+pthread_start \n", theController->vmeirq_start_);
    theController->irq_pthread_start(crate);
  }
/*
  if(theController->vmeirq_start_==1){
    theController->irq_pthread_start(crate);
  }else{
    theController->irq_pthread_end(crate);
  } 
*/
}

void Crate::init(int crate) {
// JRG, only starts the IRQ handler!
  printf(" Crate::init: theController->vmeirq_start_=%d.  Calling irq+pthread_end \n", theController->vmeirq_start_);
  theController->irq_pthread_end(crate);
  irqprob=0;
  timer=0;
  xtimer=0;
  if(theController->vmeirq_start_==1){
    printf(" Crate::init: theController->vmeirq_start_ %d, now irq+pthread_start \n", theController->vmeirq_start_);
    theController->irq_pthread_start(crate);
  }
}


int Crate::irqtest(int crate,int ival)
{
  return theController->irq_tester(crate,ival);
}
