//-----------------------------------------------------------------------
// $Id: EmuFEDCrateSO.h,v 1.1 2006/01/21 20:20:14 gilmore Exp $
// $Log: EmuFEDCrateSO.h,v $
// Revision 1.1  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
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

