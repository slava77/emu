#ifndef __emu_odmbdev_RadioButtonAction_h__
#define __emu_odmbdev_RadioButtonAction_h__

#include "emu/odmbdev/Action.h"
#include "cgicc/Cgicc.h"

using namespace std;
using namespace emu::pc;

namespace emu{
  namespace odmbdev {
    class RadioButtonAction : public Action {
    public:
      RadioButtonAction(Crate* crate, Manager* manager, string buttonLabel, string opt1, string opt2);

      void display(xgi::Output* out);
      void respond(xgi::Input* in, ostringstream& out);
    protected:
      string buttonLabel, opt1, opt2;
      bool default_opt;
    };
  }
}

#endif
