#include "emu/odmbdev/LogAction.h"

namespace emu { namespace odmbdev {

    LogAction::LogAction(emu::pc::Crate * crate)
      : Action(crate)
    {
      /* ... nothing to see here ... */
    }
    
    LogAction::LogAction(emu::pc::Crate * crate, emu::odmbdev::Manager* manager)
      : Action(crate)
    {
      /* 10/11--new version for production test log */
    }

    void LogAction::respond(xgi::Input * in, std::ostringstream & out) {
      XCEPT_RAISE( xcept::Exception, "Don't use LogActions as Actions." );
    }
  }
}
