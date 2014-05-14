#include "emu/odmbdev/TextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu{
  namespace odmbdev {
    TextBoxAction::TextBoxAction(Crate* crate, emu::odmbdev::Manager* manager,
				 string buttonLabel_in):
      Action(crate, manager),
      buttonLabel(buttonLabel_in),
      textBoxContent(""){
    }

    void TextBoxAction::display(xgi::Output * out){
      addButtonWithTextBox(out, buttonLabel, "textbox",
			   textBoxContent,"width: 185px;", "width: 100px;");
    }

    void TextBoxAction::respond(xgi::Input* in, ostringstream& out){
      textBoxContent = getFormValueString("textbox", in);
      respond(in, out, textBoxContent);
    }

    void TextBoxAction::respond(xgi::Input* in, ostringstream& out,
				     const string& textBoxContent_in){
      textBoxContent=textBoxContent_in;
    }
  }
}
