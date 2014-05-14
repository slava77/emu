#ifndef __emu_odmbdev_TextBoxAction_h__
#define __emu_odmbdev_TextBoxAction_h__

#include "emu/odmbdev/Action.h"

using namespace std;
using namespace emu::pc;

namespace emu{
  namespace odmbdev{
    class TextBoxAction : public Action {
    public:
      TextBoxAction(Crate * crate, Manager * manager, string buttonLabel);

      void display(xgi::Output * out);
      void respond(xgi::Input * in, ostringstream & out);
      virtual void respond(xgi::Input* in, ostringstream& out,
			   const string& textBoxContent_in);
    protected:
      string buttonLabel;
      string textBoxContent;
    };
  }
}

#endif
