//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrateSO.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: EmuPeripheralCrateSO.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef EmuPeripheralCrateSO_h
#define EmuPeripheralCrateSO_h
#include "xdaq.h"

class EmuPeripheralCrate;

class EmuPeripheralCrateSO : public xdaqSO
{
public:
  void init();
  void shutdown();
protected:
  EmuPeripheralCrate * emuPeripheralCrate;
};
#endif

