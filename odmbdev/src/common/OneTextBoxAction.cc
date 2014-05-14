#include "emu/odmbdev/OneTextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu { 
  namespace odmbdev {

    OneTextBoxAction::OneTextBoxAction(Crate * crate, emu::odmbdev::Manager * manager, string buttonLabel)
      : Action(crate, manager)
    {
      this->buttonLabel = buttonLabel;
      this->textBoxContent = string("");
    }

    void OneTextBoxAction::display(xgi::Output * out)
    {
      addButtonWithSlotTextBox(out,
			   this->buttonLabel,
			   "textbox",
			   "");
    }
    // remember to call this base method with you override it, otherwise
    // textBoxContents will be empty!
    void OneTextBoxAction::respond(xgi::Input * in, ostringstream & out)
    {
      this->textBoxContent = getFormValueString("textbox", in);
    }
  } // end namespace emu
} // end namespace odmbdev
