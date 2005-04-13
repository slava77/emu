//-----------------------------------------------------------------------
// $Id: EmuRUI.h,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUI.h,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef __EmuRUI_H__
#define __EmuRUI_H__

#include "GenericRUI.h"
#include "EmuConfigRUI.h"

class EmuRUI : public EmuConfigRUI, public GenericRUI
{
public:
  void Configure() throw (xdaqException);
};

#endif // ifndef __EmuRUI_H__
