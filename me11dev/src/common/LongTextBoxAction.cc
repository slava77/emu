#include "emu/me11dev/LongTextBoxAction.h"

#include "emu/me11dev/utils.h"

namespace emu { namespace me11dev {

    LongTextBoxAction::LongTextBoxAction(emu::pc::Crate * crate, const std::string& buttonLabel)
      : Action(crate),
        ActionValue<std::string>(""),
        buttonLabel_(buttonLabel)
    {}

    void LongTextBoxAction::display(xgi::Output * out)
    {
      addButtonWithLongTextBox(out,
                               buttonLabel_,
                               "longtextbox",
                               value());
    }

    // remember to call this base method with you override it, otherwise
    // textBoxContents will be empty!
    void LongTextBoxAction::respond(xgi::Input * in, std::ostringstream & out)
    {
      std::string textBoxContents = getFormValueString("longtextbox", in);
      value(textBoxContents); // save the value
    }
  }
}


