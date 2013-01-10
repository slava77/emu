#include "emu/me11dev/ButtonAction.h"

namespace emu { namespace me11dev {
    ButtonAction::ButtonAction(Crate * crate, string buttonLabel)
      : Action(crate)
    {
      this->buttonLabel = buttonLabel;
    }

    void ButtonAction::display(xgi::Output * out){
      AddButton(out, this->buttonLabel);
    }
  }
}
