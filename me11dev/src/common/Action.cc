#include "emu/me11dev/Action.h"
#include "emu/utils/String.h"
#include "emu/pc/Crate.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ALCTController.h"

#include "cgicc/HTMLClasses.h"

using namespace cgicc;
using namespace std;
using namespace emu::pc;

namespace emu { namespace me11dev {

    Action::Action(Crate * crate):
      crate_(crate),
      dmbs_(crate_->daqmbs()),
      ddus_(crate_->ddus()),
      tmb_(crate_->tmbs().at(0)),
      ccb_(crate_->ccb()),
      alct_(tmb_->alctController()),
      manager_(NULL)
    {}

    Action::Action(Crate * crate, emu::me11dev::Manager* manager):
      crate_(crate),
      dmbs_(crate_->daqmbs()),
      ddus_(crate_->ddus()),
      tmb_(crate_->tmbs().at(0)),
      ccb_(crate_->ccb()),
      alct_(tmb_->alctController()),
      manager_(manager)
    {}


    void Action::useTMBInSlot(int slot)
    {
      if (slot <= 0 && tmb_ != NULL) return; // keep the default tmb_

      vector<TMB*> tmbs = crate_->tmbs();
      for (size_t k = 0; k < tmbs.size(); ++k)
      {
        if (tmbs[k]->GetTmbSlot() == slot)
        {
          tmb_ = tmbs[k];
          return;
        }
      }

      // don't have this specified slot
      XCEPT_RAISE(xcept::Exception,
          string("Misconfiguration: could not assign TMB with slot ") +
          emu::utils::stringFrom(slot) );
    }


    void Action::addButton(xgi::Output *out,
			   const string& button_name,
			   const string& button_style)
    {
      *out << cgicc::input()
	.set("type","submit")
	.set("style",button_style)
	.set("value",button_name)
	   << endl;
    }

    void Action::addButtonWithTextBox(xgi::Output *out,
				      const string& button_name,
				      const string& textbox_name,
				      const string& textbox_default_value,
				      const string& button_style,
				      const string& textbox_style)
    {
      *out << cgicc::input()
	.set("type","submit")
	.set("style", button_style)
	.set("value",button_name)
	   << endl
	   << cgicc::input()
	.set("type","text")
	.set("value",textbox_default_value)
	.set("style", textbox_style)
	.set("name",textbox_name);
    }

    void Action::addButtonWithTwoTextBoxes(xgi::Output *out,
					   const string& button_name,
					   const string& textbox_name1,
					   const string& textbox_default_value1,
					   const string& textbox_name2,
					   const string& textbox_default_value2,
					   const string& button_style,
					   const string& textbox_style1,
					   const string& textbox_style2)
    {
      *out << cgicc::input()
	.set("type","submit")
	.set("style",button_style)
	.set("value",button_name)
	   << endl
	   << cgicc::input()
	.set("type","text")
	.set("value",textbox_default_value1)
	.set("style",textbox_style1)
	.set("name",textbox_name1)
	   << cgicc::input()
	.set("type","text")
	.set("value",textbox_default_value2)
	.set("style",textbox_style2)
	.set("name",textbox_name2);
    }

  }
}

