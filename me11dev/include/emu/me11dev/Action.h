#ifndef __emu_me11dev_Action_h__
#define __emu_me11dev_Action_h__

#include "xgi/exception/Exception.h"
#include "xgi/Input.h"
#include "xgi/Output.h"

#include <iostream>
#include <sstream>
#include <vector>

/******************************************************************************
 * The Action Class
 *
 * The action class is the superclass/interface for all actions. Read below for
 * information on how to override it properly. The important methods to
 * override are display and respond, but the compiler should complain if you do
 * not do so.
 *****************************************************************************/



namespace emu { 

// forward declarations:
namespace pc {
class Crate;
class CFEB;
class DAQMB;
class CCB;
class DDU;
class TMB;
class ALCTController;
}

namespace me11dev {


  const std::string default_button_style="min-width: 25em; width: 25%; ";
  const std::string default_textbox_style="min-width: 4em; width: 8%; ";
  //long_textbox_style="width: 100%; margin-top: 1em; height: 10em; ";

  class Manager;


    class Action
    {

    public:

      Action(emu::pc::Crate * crate);
      Action(emu::pc::Crate * crate, Manager* manager);

      // a virtual destructor removes a warning about have a class with virtual
      // methods but a non-virtual destructor
      virtual ~Action() { };

      // virtual with "= 0" indicates that the method *must* be implemented by
      // any subclasses this ensures that this method can be called on
      // objects of type Action
      // http://en.wikibooks.org/wiki/C%2B%2B_Programming/Classes/Abstract_Classes

      /************************************************************************
       * display
       *
       * This method should be overridden to change how this action is
       * presented on the web page. This class contains a number of static
       * methods that you can use to ease this job (e.g. AddButton and
       * friends).
       *
       * Use classes in the cgicc namespace (e.g. input, p) as arguments to the
       * stream operator of the `out' parameter. 
       ***********************************************************************/
      virtual void display(xgi::Output * out) = 0;

      /************************************************************************
       * respond
       *
       * This method should be overridden to change how this action responds to
       * being called. Any input values which you specified in display can be
       * accessed here with the `getFormValue____' static methods.
       *
       * The out parameter can be used to print to the log displayed on the web
       * page.
       *
       * You'll also have access to the fields listed below in protected. In
       * particular, you will probably find dmbs, tmb, ccb, and ddus useful.
       *
       * Finally, note that we've already defined type aliases for the cfeb
       * iterators.
       ***********************************************************************/
      virtual void respond(xgi::Input * in, std::ostringstream & out) = 0;

      /**
       * This method allows to assign a TMB in a specific slot for this action.
       * By default, in the constructor, the first available TMB with the lowest
       * slot number is used. Also, if slot<=0, the assignment doesn't change.
       * Throws when tmb was not successfully assigned.
       */
      void useTMBInSlot(int slot);

    protected:

      typedef std::vector<emu::pc::CFEB>::iterator CFEBItr;
      typedef std::vector<emu::pc::CFEB>::reverse_iterator CFEBrevItr;

      emu::pc::Crate * crate_;
      std::vector<emu::pc::DAQMB*> dmbs_;
      std::vector<emu::pc::DDU*> ddus_;
      emu::pc::TMB* tmb_;
      emu::pc::CCB* ccb_;
      emu::pc::ALCTController* alct_;
      emu::me11dev::Manager* manager_;

      static void addButton(xgi::Output *out,
			    const std::string& button_name,
			    const std::string& button_style=default_button_style);

      static void addButtonWithTextBox(xgi::Output *out,
				       const std::string& button_name,
				       const std::string& textbox_name,
				       const std::string& textbox_default_value,
				       const std::string& button_style=default_button_style,
				       const std::string& textbox_style=default_textbox_style);

      static void addButtonWithTwoTextBoxes(xgi::Output *out,
					    const std::string& button_name,
					    const std::string& textbox_name1,
					    const std::string& textbox_default_value1,
					    const std::string& textbox_name2,
					    const std::string& textbox_default_value2,
					    const std::string& button_style=default_button_style,
					    const std::string& textbox_style1=default_textbox_style,
					    const std::string& textbox_style2=default_textbox_style);

    };
  }
}

#endif

