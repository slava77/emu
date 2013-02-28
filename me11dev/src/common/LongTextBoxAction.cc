#include "emu/me11dev/LongTextBoxAction.h"

namespace emu { namespace me11dev {

    LongTextBoxAction::LongTextBoxAction(emu::pc::Crate * crate, const std::string& buttonLabel)
      : Action(crate)
    {
      this->buttonLabel = buttonLabel;
      this->textBoxContents = std::string("");
    }

    void LongTextBoxAction::display(xgi::Output * out){
      AddButtonWithLongTextBox(out,
			       this->buttonLabel,
			       "longtextbox",
			       "");
    }

    // remember to call this base method with you override it, otherwise
    // textBoxContents will be empty!
    void LongTextBoxAction::respond(xgi::Input * in, std::ostringstream & out){
      this->textBoxContents = getFormValueString("longtextbox", in);
    }
  }
}
