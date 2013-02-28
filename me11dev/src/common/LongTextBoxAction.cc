#include "emu/me11dev/LongTextBoxAction.h"

#include "emu/me11dev/utils.h"

namespace emu { namespace me11dev {

    LongTextBoxAction::LongTextBoxAction(emu::pc::Crate * crate, const std::string& buttonLabel)
      : Action(crate),
        buttonLabel_(buttonLabel),
        textBoxContents_("")
    {}

    void LongTextBoxAction::display(xgi::Output * out){
      addButtonWithLongTextBox(out,
			       buttonLabel_,
			       "longtextbox",
			       "");
    }

    // remember to call this base method with you override it, otherwise
    // textBoxContents will be empty!
    void LongTextBoxAction::respond(xgi::Input * in, std::ostringstream & out){
      textBoxContents_ = getFormValueString("longtextbox", in);
    }
  }
}

