#ifndef __emu_me11dev_Action_h__
#define __emu_me11dev_Action_h__

#include "emu/pc/Crate.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"
#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "xcept/tools.h"
#include "xcept/Exception.h"

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

using namespace std;
using namespace emu::pc;

namespace emu { namespace me11dev {
    class Action {
    public:
      Action(Crate * crate);
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
      virtual void respond(xgi::Input * in, ostringstream & out) = 0;
    protected:
      typedef vector<CFEB>::iterator CFEBItr;
      typedef vector<CFEB>::reverse_iterator CFEBrevItr;

      Crate * crate;
      vector <DAQMB*> dmbs;
      vector <DDU*> ddus;
      TMB* tmb;
      CCB* ccb;

      static int getFormValueInt(const string form_element, xgi::Input *in);
      static int getFormValueIntHex(const string form_element, xgi::Input *in);
      static float getFormValueFloat(const string form_element, xgi::Input *in);
      static string getFormValueString(const string form_element, xgi::Input *in);

      static void AddButton(xgi::Output *out,
			    const string button_name,
			    const string button_style="min-width: 25em; width: 25%; ")
	throw (xgi::exception::Exception);

      static void AddButtonWithTextBox(xgi::Output *out,
				       const string button_name,
				       const string textbox_name,
				       const string textbox_default_value,
				       const string button_style="min-width: 25em; width: 25%; ",
				       const string textbox_style="")
	throw (xgi::exception::Exception);

      static void AddButtonWithTwoTextBoxes(xgi::Output *out,
					    const string button_name,
					    const string textbox_name1,
					    const string textbox_default_value1,
					    const string textbox_name2,
					    const string textbox_default_value2,
					    const string button_style="min-width: 25em; width: 25%; ",
					    const string textbox_style1="",
					    const string textbox_style2="")
	throw (xgi::exception::Exception);

      static void AddButtonWithLongTextBox(xgi::Output *out,
					   const string button_name,
					   const string textbox_name,
					   const string textbox_default_value,
					   const string button_style="min-width: 25em; width: 25%; ",
					   const string textbox_style="width: 100%; margin-top: 1em; height: 10em; ")
	throw (xgi::exception::Exception);

    };
  }
}

#endif
