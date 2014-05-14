#include "emu/odmbdev/RadioButtonAction.h"
#include "emu/odmbdev/Manager.h"

#include "cgicc/Cgicc.h"

using namespace std;

namespace emu{
  namespace odmbdev{
    RadioButtonAction::RadioButtonAction(Crate* crate,
					 emu::odmbdev::Manager* manager,
					 string buttonLabel_in,
					 string opt1_in, string opt2_in):
      Action(crate, manager),
      buttonLabel(buttonLabel_in),
      opt1(opt1_in),
      opt2(opt2_in),
      default_opt(true)
    {
    }

    void RadioButtonAction::display(xgi::Output * out){
      addButtonWithRadio(out, buttonLabel, opt1, opt2);
    }

    void RadioButtonAction::respond(xgi::Input * in, ostringstream & out){
      cgicc::Cgicc cgi;
      cout << "opt1/opt2: " << opt1 << "/" << opt2 << endl;
      cout << "option 1 checked: " << cgi.queryCheckbox("INJPLS") << endl;
      cout << "option 2 checked: " << cgi.queryCheckbox(opt2) << endl;
      if(cgi.queryCheckbox(opt2)) default_opt = false;
      else default_opt = true;
    }
  }
}
