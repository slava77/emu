#include "emu/odmbdev/Action.h"
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

namespace emu { namespace odmbdev {

  Action::Action(Crate * crate):
    crate_(crate),
    dmbs_(crate_->daqmbs()),
    ddus_(crate_->ddus()),
    tmb_(crate_->tmbs().at(0)),
    ccb_(crate_->ccb()),
    alct_(tmb_->alctController()),
    manager_(NULL)
  {vme_wrapper_ = new VMEWrapper(crate_);}

  Action::Action(Crate * crate, emu::odmbdev::Manager* manager):
    crate_(crate),
    dmbs_(crate_->daqmbs()),
    ddus_(crate_->ddus()),
    tmb_(crate_->tmbs().at(0)),
    ccb_(crate_->ccb()),
    alct_(tmb_->alctController()),
    manager_(manager)
  {vme_wrapper_ = new VMEWrapper(crate_);}


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

  void Action::addButtonWithSlotTextBox(xgi::Output *out,
				    const string& button_name,
				    const string& textbox_name,
				    const string& textbox_default_value,
				    const string& button_style,
				    const string& textbox_style)
  {
    *out << 
      "Default slot number: " <<
      cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name) 
	 << endl
	 << cgicc::input()
      .set("type","submit")
      .set("style", button_style)
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
	 <<  cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name) 	
	 << endl;
  }

  void Action::addButtonWithMCSBox(xgi::Output *out,
				   const string& button_name,
				   const string& textbox_name,
				   const string& textbox_default_value,
				   const string& button_style,
				   const string& textbox_style)
  {

    *out << 
      cgicc::input()
      .set("type","text").set("style",std::string("width: 415px; "))
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name) 
	 << endl
	 << cgicc::input()
      .set("type","submit")
      .set("style", button_style)
      .set("value",button_name)
	 << endl;
  }

  void Action::addButtonWithNameBox(xgi::Output *out,
				    const string& button_name,
				    const string& textbox_name,
				    const string& textbox_default_value,
				    const string& button_style,
				    const string& textbox_style)
  {
    *out << 
      "Enter your intials: " <<
      cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name) 
	 << endl
	 << cgicc::input()
      .set("type","submit")
      .set("style", button_style)
      .set("value",button_name)
	 << endl;
  }
    
  void Action::addButtonWithRepeatOpt(xgi::Output *out,
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
	 << " Repeat "
	 << cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name)
	 << " times."
	 << endl

	 << endl;
  }
    
  void Action::addButtonWithParameter(xgi::Output *out,
				      const string& parameter, // units of the parameter (e.g., times, packets) -- JB-F
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
	 << " "
	 << cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value)
      .set("style", textbox_style)
      .set("name",textbox_name)
	 << " " << parameter << "."
	 << endl

	 << endl;
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
	 << endl << "V1 V2: " 
	 << cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value1)
      .set("style",textbox_style1)
      .set("name",textbox_name1) << " tol: "
	 << cgicc::input() 
      .set("type","text")
      .set("value",textbox_default_value2)
      .set("style",textbox_style2)
      .set("name",textbox_name2);
  }

  void Action::addButtonWithThreeTextBoxesLVMB(xgi::Output *out,
					       const string& button_name,
					       const string& textbox_name1,
					       const string& textbox_default_value1,
					       const string& textbox_name2,
					       const string& textbox_default_value2,
					       const string& textbox_name3,
					       const string& textbox_default_value3,
					       const string& button_style,
					       const string& textbox_style1,
					       const string& textbox_style2,
					       const string& textbox_style3)
  {
    *out << cgicc::input()
      .set("type","submit")
      .set("style",button_style)
      .set("value",button_name)
	 << endl << "V1 V2: " 
	 << cgicc::input()
      .set("type","text")
      .set("value",textbox_default_value1)
      .set("style",textbox_style1)
      .set("name",textbox_name1) << " tol: "
	 << cgicc::input() 
      .set("type","text")
      .set("value",textbox_default_value2)
      .set("style",textbox_style2)
      .set("name",textbox_name2) << " N: " 
	 << cgicc::input() 
      .set("type","text")
      .set("value",textbox_default_value3)
      .set("style",textbox_style3)
      .set("name",textbox_name3);
  }
  void Action::addButtonWithThreeTextBoxes(xgi::Output *out,
                                           const string& button_name,
                                           const string& textboxname1,
                                           const string& textbox_default_value1,
                                           const string& textboxname2,
                                           const string& textbox_default_value2,
                                           const string& textboxname3,
                                           const string& textbox_default_value3)
  {
    *out << cgicc::input().set("type","submit")
      .set("value",button_name)
         << endl  
	 << cgicc::input().set("type","text").set("style",std::string("width: 40px; "))
      .set("value",textbox_default_value1)
      .set("name",textboxname1) << " times" 
	 << cgicc::br() << " File: "
         << cgicc::input().set("type","text").set("style",std::string("width: 468px; margin-top: 1em; "))
      .set("value",textbox_default_value2)
      .set("name",textboxname2)
         << endl << cgicc::br() << endl
         << cgicc::textarea().set("style",
                                  std::string("width: 515px; ")
                                  + "margin-top: 1em; "
                                  + "height: 500px; ")
      .set("name", textboxname3)
         << textbox_default_value3
         << cgicc::textarea()
	 << endl;
    //         << cgicc::div();
  }
  
  void Action::addButtonWithFourTextBoxes(xgi::Output *out,
					  const string& button_name,
					  const string& textboxname1,
					  const string& textbox_default_value1,
					  const string& textboxname2,
					  const string& textbox_default_value2,
					  const string& textboxname3,
					  const string& textbox_default_value3,
					  const string& textboxname4,
					  const string& textbox_default_value4)
  {
    *out << cgicc::input().set("type","submit")
      .set("value",button_name)
         << endl  
	 << cgicc::input().set("type","text").set("style",std::string("width: 40px; "))
      .set("value",textbox_default_value1)
      .set("name",textboxname1) << " times. " << " " << "  Log Subdir: "
         << cgicc::input().set("type","text").set("style",std::string("width: 100px; margin-top: 1em; "))
      .set("value",textbox_default_value3)
      .set("name",textboxname3)
         << endl << cgicc::br() << endl
	 << cgicc::br() << " File: "
         << cgicc::input().set("type","text").set("style",std::string("width: 468px; margin-top: 1em; "))
      .set("value",textbox_default_value2)
      .set("name",textboxname2)
         << cgicc::textarea().set("style",
                                  std::string("width: 515px; ")
                                  + "margin-top: 1em; "
                                  + "height: 500px; ")
      .set("name", textboxname4)
         << textbox_default_value4
         << cgicc::textarea()
	 << endl;
    //         << cgicc::div();
  }

  void Action::addButtonWithRadio(xgi::Output *out,
				  const string& button_name,
				  const string& opt1,
				  const string& opt2
				  //const string& textbox_default_value,
				  )
  {
    cout << "Creating radio button" << endl;
    cout << "opt1/opt2: " << opt1 << "/" << opt2 << endl;
    *out << cgicc::input()
      .set("type","submit")
      .set("style", "width: 230px; ")
      .set("value",button_name)
	 << " "
	 << cgicc::input()
      .set("type","radio")
      .set("name","mode1")
      .set("id",opt1)
      .set("checked","checked")
	 << opt1
	 << " "
	 << cgicc::input()
      .set("type","radio")
      .set("name","mode2")
      .set("id",opt2)
	 << opt2
	 << endl
	 << endl;
  }

}
}

