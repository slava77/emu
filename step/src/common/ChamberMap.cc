#include "emu/step/ChamberMap.h"

void emu::step::ChamberMap::registerFields( xdata::Bag<ChamberMap>* bag ){
  bag->addField( "chamberLabel", &chamberLabel_ );
  bag->addField( "dmbSlot"     , &dmbSlot_      );
  bag->addField( "crateId"     , &crateId_      );
}

std::string emu::step::ChamberMap::toString(){
  return "(chamberLabel:" + chamberLabel_.toString()
    + " dmbSlot:" + dmbSlot_.toString() 
    + " crateId:" + crateId_.toString() 
    + ")";
}
