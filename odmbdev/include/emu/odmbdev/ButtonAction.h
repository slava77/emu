#ifndef __emu_odmbdev_ButtonAction_h__
#define __emu_odmbdev_ButtonAction_h__

#include "xgi/Input.h"
#include "xgi/Output.h"

#include "emu/odmbdev/Action.h"

/******************************************************************************
 * The ButtonAction Class
 *
 * This is a class that shortens the amount of code necessary to add only a
 * button (and no text box). Basically, it allows you to skip the call to
 * `AddButton'. Instead, call the ButtonAction constructor (see examples in
 * Buttons.h) in the derived class's constructor with the crate and a label for
 * the button.
 *****************************************************************************/

using namespace std;
using namespace emu::pc;

namespace emu{
  namespace odmbdev{
    class ButtonAction : public Action {
    public:
      ButtonAction(Crate* crate, string buttonLabel);
      ButtonAction(Crate* crate, emu::odmbdev::Manager* manager,
		   string buttonLabel_in);

      void display(xgi::Output * out);

      // "= 0" forces subclases to implement this
      virtual void respond(xgi::Input* in, ostringstream& out) = 0;
    protected:
      string buttonLabel;
    };
  }
}

#endif
