//-----------------------------------------------------------------------
// $Id: EmuConfigRUI.h,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuConfigRUI.h,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
// Revision 1.1  2004/08/17 23:13:35  tfcvs
// initial version of EMU RUI for the new event builder
//
// Revision 1.4  2004/06/12 13:26:10  tfcvs
// minor changes, mostly additional output to stdout identifying the calling routine (FG)
//
// Revision 1.3  2004/06/11 08:22:22  tfcvs
// *** empty log message ***
//
//-----------------------------------------------------------------------
#ifndef EmuConfigRUI_h
#define EmuConfigRUI_h
#include <string>
// XDAQ
#include "xdaqApplication.h"

class DDUReader;

class EmuConfigRUI : public virtual xdaqApplication{
public:
  EmuConfigRUI();
  virtual void Configure() throw(exception);

protected:
  DDUReader * ddu_;

private:
  /// can be "file" or "hardware"
  std::string dduMode_;
  /// should be something like "/dev/schar" for hardware
  std::string dduInput_;
};

#endif // EmuConfigRUI_h
