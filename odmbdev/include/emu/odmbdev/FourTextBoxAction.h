#ifndef __emu_odmbdev_FourTextBoxAction_h__
#define __emu_odmbdev_FourTextBoxAction_h__

#include "emu/odmbdev/Action.h"
#include "emu/odmbdev/utils.h"

using namespace std;
using namespace emu::pc;

namespace emu { namespace odmbdev {
    class FourTextBoxAction : public Action {
    public:
      FourTextBoxAction(Crate * crate, emu::odmbdev::Manager * manager, string buttonLabel);

      void display(xgi::Output * out);
      void respond(xgi::Input * in, ostringstream & out);
    protected:
      string buttonLabel;
      string textBoxContent1;
      string textBoxContent2;
      string textBoxContent3;
      string textBoxContent4;
    };
  }
}

#endif
