#ifndef __emu_odmbdev_OneTextBoxAction_h__
#define __emu_odmbdev_OneTextBoxAction_h__

#include "emu/odmbdev/Action.h"

using namespace std;
using namespace emu::pc;

namespace emu { namespace odmbdev {
    class OneTextBoxAction : public Action {
    public:
      OneTextBoxAction(Crate * crate, Manager * manager, string buttonLabel);

      void display(xgi::Output * out);
      void respond(xgi::Input * in, ostringstream & out);
    protected:
      string buttonLabel;
      string textBoxContent;
    };
  }
}

#endif
