//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrateSO.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: EmuPeripheralCrateSO.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.2  2004/06/01 21:53:11  tfcvs
// Allow for i2o messaging (LG/FG)
//
//-----------------------------------------------------------------------
#include "EmuPeripheralCrateSO.h"
#include "EmuPeripheralCrate.h"

void EmuPeripheralCrateSO::init() {
  emuPeripheralCrate = new EmuPeripheralCrate();
  executive->load(emuPeripheralCrate);
}


void EmuPeripheralCrateSO::shutdown() {
  delete emuPeripheralCrate;
}


extern "C" void * init_EmuPeripheralCrate() {
  return ( (void *) new EmuPeripheralCrateSO() );
}

