#ifndef __emu_me11dev_LogAction_h__
#define __emu_me11dev_LogAction_h__

#include "emu/me11dev/Action.h"

/******************************************************************************
 * The LogAction Class
 *
 * These actions have a third parameter to respond which gives them access to
 * the web output log so that they can modify it.
 *****************************************************************************/


namespace emu { namespace me11dev {

    class LogAction : public Action
    {
    public:

      LogAction(emu::pc::Crate * crate);

      // "= 0" forces subclases to implement this
      virtual void display(xgi::Output * out) = 0;

      /************************************************************************
       * I know this is dumb, but I'm running out of time. Just don't use
       * LogActions as if they were actions. Pretend they're a wholly separate
       * thing.
       *
       * In a perfect world, I'd rewrite the class heirarchy to have a
       * "displayable" class. God, class-based object-oriented models are
       * annoying.
       ************************************************************************/
      void respond(xgi::Input * in, std::ostringstream & out);

      virtual void respond(xgi::Input * in, xgi::Output * out, std::ostringstream & ssout, std::ostringstream & webOutputLog) = 0;
    };
  }
}

#endif
