//-----------------------------------------------------------------------
// $Id: EmuRUISO.h,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUISO.h,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
// Revision 1.2  2004/08/18 15:42:10  tfcvs
// RUI -> EmuRUI class
//
// Revision 1.1  2004/08/17 23:13:35  tfcvs
// initial version of EMU RUI for the new event builder
//
// Revision 1.2  2004/06/11 08:22:23  tfcvs
// *** empty log message ***
//
//-----------------------------------------------------------------------
#ifndef __EmuRUI_SO_H__
#define __EmuRUI_SO_H__

#include "xdaq.h"
class EmuRUI;

class EmuRUISO: public xdaqSO {
public:
  void init();
  void shutdown();

protected:
  EmuRUI *instance_;
};

#endif  // ifndef __EmuRUI_SO_H__
