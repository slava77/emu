#ifndef __emu_odmbdev_RepeatTextBoxAction_h__
#define __emu_odmbdev_RepeatTextBoxAction_h__

#include "emu/odmbdev/Action.h"

using namespace std;
using namespace emu::pc;

namespace emu{
  namespace odmbdev {
    class RepeatTextBoxAction : public Action {
    public:
      RepeatTextBoxAction(Crate* crate, Manager* manager,
			  string buttonLabel, unsigned int default_val=1);

      void display(xgi::Output* out);
      void respond(xgi::Input* in, ostringstream& out);
      virtual void respond(xgi::Input* in, ostringstream& out,
			   const string& textBoxContent_in);
    protected:
      string buttonLabel;
      string textBoxContent;
    };
  }
}

#endif
