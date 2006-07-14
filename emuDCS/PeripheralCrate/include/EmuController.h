//-----------------------------------------------------------------------
// $Id: EmuController.h,v 1.2 2006/07/14 12:33:26 mey Exp $
// $Log: EmuController.h,v $
// Revision 1.2  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 1.1  2006/01/11 08:55:21  mey
// UPdate
//
// Revision 1.1  2005/12/20 14:38:07  mey
// Update
//
// Revision 2.2  2005/09/13 14:46:39  mey
// Get DMB crate id; and DCS
//
// Revision 2.1  2005/08/11 08:13:59  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.7  2004/06/18 23:52:33  tfcvs
// Introduced code for DCS/PCcontrol crate sharing. (FG)
//  -code additions still commented out, awaiting validation-
//
// Revision 1.6  2004/06/01 09:52:07  tfcvs
//  enabled CCB in enable(), added CVS-stuff, cleaned up includes (FG)
//
//-----------------------------------------------------------------------
#ifndef EmuController_h
#define EmuController_h
#include "CrateSelector.h"
#include <string>

class CCB;
class Crate;
class EmuSystem;

class EmuController {
public:
  EmuController();
  ~EmuController() {}

  void init();

  void configure();

  void enable();

  void disable();

  inline void SetConfFile(std::string xmlFile) { xmlFile_ = xmlFile; }

  inline EmuSystem * GetEmuSystem(){return emuSystem_;}

  CrateSelector & selector() {return theSelector;}

protected:

private:
  CrateSelector theSelector;
  std::string xmlFile_;
  EmuSystem * emuSystem_;
};

#endif
