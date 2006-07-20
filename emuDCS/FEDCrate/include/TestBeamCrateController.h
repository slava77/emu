//-----------------------------------------------------------------------
// $Id: TestBeamCrateController.h,v 3.0 2006/07/20 21:16:10 geurts Exp $
// $Log: TestBeamCrateController.h,v $
// Revision 3.0  2006/07/20 21:16:10  geurts
// *** empty log message ***
//
// Revision 1.1  2006/01/21 20:21:36  gilmore
// *** empty log message ***
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
#ifndef TestBeamCrateController_h
#define TestBeamCrateController_h
#include "CrateSelector.h"
#include <string>

class CCB;
class Crate;
#ifdef USEDCS
class InfoMonitorService;
class EmuDcs;
#endif

class TestBeamCrateController {
public:
  TestBeamCrateController();
  ~TestBeamCrateController() {}

  void configure();
  void configureNoDCS();

  void enable();

  void disable();

  CrateSelector & selector() {return theSelector;}

  /// sends the command to the appropriate boards selected by the CrateSelector
  /// boardType comes from VMEModule.h
  void executeCommand(std::string boardType, std::string command);

#ifdef USEDCS
  // DCS sharing
  void DcsSetup();
  void DcsEnable();
  void DcsDisable();
  //
#endif

protected:
  void configure(Crate * crate);
  void configure(CCB * ccb);

  void enable(Crate * crate);

  void disable(Crate * crate);

#ifdef USEDCS
  // DCS sharing
  EmuDcs *mEmuDcs;
#endif

private:
  CrateSelector theSelector;
#ifdef USEDCS
  // DCS monitor
  InfoMonitorService *monitorService;
#endif
};

#endif
