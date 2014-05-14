#include "emu/odmbdev/FourTextBoxAction.h"

namespace emu { namespace odmbdev {
  FourTextBoxAction::FourTextBoxAction(Crate * crate, emu::odmbdev::Manager * manager, string buttonLabel)
    : Action(crate, manager)
  {
    this->buttonLabel = buttonLabel;
    this->textBoxContent1 = string("");
    this->textBoxContent2 = string("");
    this->textBoxContent3 = string("");
    this->textBoxContent4 = string("");
  }

  void FourTextBoxAction::display(xgi::Output * out){
    addButtonWithFourTextBoxes(out,
				this->buttonLabel,
				"textbox1",
				"1",
				"textbox2",
				"",
				"textbox3",
				"",
				"textbox4",
				"");
  }
  // remember to call this base method with you override it, otherwise
  // textBoxContents will be empty!
 void FourTextBoxAction::respond(xgi::Input * in, ostringstream & out){
    this->textBoxContent1 = getFormValueString("textbox1", in);
    this->textBoxContent2 = getFormValueString("textbox2", in);
    this->textBoxContent3 = getFormValueString("textbox3", in);
    this->textBoxContent4 = getFormValueString("textbox4", in);
  }
}
}
