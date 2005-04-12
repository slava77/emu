//-----------------------------------------------------------------------
// $Id: PeripheralCrateController.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: PeripheralCrateController.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "PeripheralCrateController.h"
#include "Crate.h"
#include "DAQMB.h"
#include "TMB.h"
#include "CrateSetup.h"
#include "Singleton.h"



PeripheralCrateController::PeripheralCrateController()
{
}


PeripheralCrateController::~PeripheralCrateController() { 
}


std::vector<Crate *> PeripheralCrateController::crates() const {
  std::vector<Crate *> allCrates = Singleton<CrateSetup>::instance()->crates();
  std::vector<Crate *> result;
  if(theSelectedCrates.empty()) {
    result = allCrates;
  } else {
    for(unsigned int icrate = 0; icrate < allCrates.size(); ++icrate) {
      for(unsigned int iSelect = 0; iSelect < theSelectedCrates.size(); ++iSelect) {
        if(allCrates[icrate]->number() == theSelectedCrates[iSelect]) {
          result.push_back(allCrates[icrate]);
        }
      }
    }
  }
  return result;
}


std::vector<DAQMB *> PeripheralCrateController::daqmbs(Crate * crate) const {
  std::vector<DAQMB *> result;
  std::vector<DAQMB*> cards = crate->daqmbs();
  for(unsigned int idmb = 0; idmb < cards.size(); ++idmb) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[idmb]);
    } else {
      for(unsigned int iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[idmb]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[idmb]);
        }
      }
    }
  }
  return result;
}


std::vector<TMB *> PeripheralCrateController::tmbs(Crate * crate) const {
  std::vector<TMB *> result;
  std::vector<TMB*> cards = crate->tmbs();
  for(unsigned int itmb = 0; itmb < cards.size(); ++itmb) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[itmb]);
    } else {
      for(unsigned int iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[itmb]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[itmb]);
        }
      }
    }
  }
  return result;
}

