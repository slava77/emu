//-----------------------------------------------------------------------
// $Id: EmuRUI.cc,v 2.0 2005/04/13 10:52:59 geurts Exp $
// $Log: EmuRUI.cc,v $
// Revision 2.0  2005/04/13 10:52:59  geurts
// Makefile
//
// Revision 1.3  2004/10/09 00:04:33  tfcvs
// updated to DDUReadout
//
// Revision 1.2  2004/08/18 15:42:10  tfcvs
// RUI -> EmuRUI class
//
// Revision 1.1  2004/08/17 23:13:35  tfcvs
// initial version of EMU RUI for the new event builder
//
// Revision 1.3  2004/06/12 13:26:10  tfcvs
// minor changes, mostly additional output to stdout identifying the calling routine (FG)
//
//-----------------------------------------------------------------------
#include "EmuRUI.h"
#include "DDUReader.h"

void EmuRUI::Configure() throw (xdaqException)
{
  std::cout << "EmuRUI: configuring EmuConfigRUI" << std::endl;
  EmuConfigRUI::Configure();

  std::cout << "EmuRUI: configuring GenericRUI" << std::endl;
  GenericRUI::Configure();

  eventReader_ = ddu_;
  eventWriter_ = NULL;  // doesn't write out data locally
}
