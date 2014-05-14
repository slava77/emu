#include "emu/odmbdev/MCSTextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu { 
  namespace odmbdev {

    MCSTextBoxAction::MCSTextBoxAction(Crate * crate, emu::odmbdev::Manager * manager, string buttonLabel)
      : Action(crate, manager)
    {
      this->buttonLabel = buttonLabel;
      this->textBoxContent = string("");
    }

    void MCSTextBoxAction::display(xgi::Output * out)
    {
      addButtonWithMCSBox(out,
			   this->buttonLabel,
			   "textbox",
			   "");
    }
    // remember to call this base method with you override it, otherwise
    // textBoxContents will be empty!
    void MCSTextBoxAction::respond(xgi::Input * in, ostringstream & out)
    {
      this->textBoxContent = getFormValueString("textbox", in);
    }
  } // end namespace emu
} // end namespace odmbdev
