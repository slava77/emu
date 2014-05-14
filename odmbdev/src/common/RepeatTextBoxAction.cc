#include "emu/odmbdev/RepeatTextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu{
  namespace odmbdev{
    RepeatTextBoxAction::RepeatTextBoxAction(Crate* crate,
					     emu::odmbdev::Manager* manager,
					     string buttonLabel_in,
					     unsigned int default_val):
      Action(crate, manager),
      buttonLabel(buttonLabel_in){
      char s_default[20];
      sprintf (s_default,"%d",default_val);
      textBoxContent = s_default;
    }

    void RepeatTextBoxAction::display(xgi::Output * out){
      addButtonWithRepeatOpt(out, buttonLabel, "textbox", textBoxContent);
    }

    void RepeatTextBoxAction::respond(xgi::Input* in, ostringstream& out){
      textBoxContent = getFormValueString("textbox", in);
      respond(in, out, textBoxContent);
    }

    void RepeatTextBoxAction::respond(xgi::Input * in, ostringstream & out,
				      const string& textBoxContent_in){
      textBoxContent = textBoxContent_in;
    }
  }
}
