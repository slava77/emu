#include "emu/odmbdev/ThreeTextBoxAction.h"
#include "emu/odmbdev/Manager.h"

namespace emu{
  namespace odmbdev {
    ThreeTextBoxAction::ThreeTextBoxAction(Crate* crate,
					   emu::odmbdev::Manager* manager,
					   string buttonLabel_in):
      Action(crate, manager),
      buttonLabel(buttonLabel_in),
      textBoxContent("4.5  3.0"),
      textBoxContent2("0.05"),
      textBoxContent3("300"){
    }

    void ThreeTextBoxAction::display(xgi::Output * out){
      addButtonWithThreeTextBoxesLVMB(out, buttonLabel, "textbox", textBoxContent,
				      "textbox2", textBoxContent2, "textbox3",
				      textBoxContent3, "width: 120px;", "width: 100px;",
				      "width: 35px;", "width: 80px;");
    }

    void ThreeTextBoxAction::respond(xgi::Input* in, ostringstream& out){
      textBoxContent = getFormValueString("textbox", in);
      textBoxContent2 = getFormValueString("textbox2", in);
      textBoxContent3 = getFormValueString("textbox3", in);
      respond(in, out, textBoxContent3);
    }

    void ThreeTextBoxAction::respond(xgi::Input* in, ostringstream& out,
					     const string& textBoxContent3_in){
      textBoxContent3=textBoxContent3_in;
    }
  }
}
