#ifndef DcsEmuController_h
#define DcsEmuController_h

#include "EmuController.h"
#include "Crate.h"
#ifdef USEDCS
class InfoMonitorService;
class EmuDcs;
#endif
class DcsEmuController : public EmuController{

public:
  DcsEmuController();
  ~DcsEmuController() {}



 void  configure_simple();
 void  configure_simple(Crate * crate);
#ifdef USEDCS
  // DCS sharing
  void DcsSetup();
  void DcsEnable();
  void DcsDisable();
  EmuDcs *mEmuDcs;
#endif

private:
#ifdef USEDCS
  // DCS monitor
  InfoMonitorService *monitorService;
#endif

};

#endif
