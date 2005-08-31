#include "FEDCrateController.h"
#include "Crate.h"
#include "DDU.h"
#include "DCC.h"
#include "CrateSetup.h"
#include "Singleton.h"



FEDCrateController::FEDCrateController()
{
}


FEDCrateController::~FEDCrateController() { 
}


vector<Crate *> FEDCrateController::crates() const {
  vector<Crate *> allCrates = Singleton<CrateSetup>::instance()->crates();
  vector<Crate *> result;
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


vector<DDU *> FEDCrateController::ddus(Crate * crate) const {
  vector<DDU *> result;
  vector<DDU*> cards = crate->ddus();
  for(unsigned int iddu = 0; iddu < cards.size(); ++iddu) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[iddu]);
    } else {
      for(unsigned int iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[iddu]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[iddu]);
        }
      }
    }
  }
  return result;
}

vector<DCC *> FEDCrateController::dccs(Crate * crate) const {
  vector<DCC *> result;
  vector<DCC*> cards = crate->dccs();
  for(unsigned int idcc = 0; idcc < cards.size(); ++idcc) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[idcc]);
    } else {
      for(unsigned int iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[idcc]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[idcc]);
        }
      }
    }
  }
  return result;
}

