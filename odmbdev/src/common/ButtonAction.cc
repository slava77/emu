#include "emu/odmbdev/ButtonAction.h"
 
namespace emu{
  namespace odmbdev{
    ButtonAction::ButtonAction(Crate* crate, string buttonLabel_in):
      Action(crate),
      buttonLabel(buttonLabel_in){
    }

    ButtonAction::ButtonAction(Crate* crate, emu::odmbdev::Manager* manager,
			       string buttonLabel_in):
      Action(crate, manager),
      buttonLabel(buttonLabel_in){
    }

    void ButtonAction::display(xgi::Output * out){
      addButton(out, this->buttonLabel, "width: 230px; ");
    }
  }
}
