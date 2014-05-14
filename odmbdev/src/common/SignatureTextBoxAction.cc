#include "emu/odmbdev/SignatureTextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu { namespace odmbdev {
  SignatureTextBoxAction::SignatureTextBoxAction(Crate * crate, emu::odmbdev::Manager * manager, string buttonLabel)
    : LogAction(crate, manager)
  {
    this->buttonLabel = buttonLabel;
    this->textBoxContent = string("");
  }

  void SignatureTextBoxAction::display(xgi::Output * out){
    addButtonWithNameBox(out,
				this->buttonLabel,
				"textbox",
				this->textBoxContent);
  }
  // remember to call this base method with you override it, otherwise
  // textBoxContents will be empty!
  void SignatureTextBoxAction::respond(xgi::Input * in, xgi::Output * out, ostringstream & ssout){
   this->textBoxContent = getFormValueString("textbox", in);
  }
}
}
