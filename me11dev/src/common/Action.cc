#include "emu/me11dev/Action.h"

#include "emu/pc/Crate.h"
#include "emu/pc/CFEB.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/DDU.h"
#include "emu/pc/TMB.h"

#include "cgicc/HTMLClasses.h"

using namespace cgicc;
using namespace std;
using namespace emu::pc;

namespace emu { namespace me11dev {

    Action::Action(Crate * crate) {
      this->crate = crate;

      ccb = this->crate->ccb();
      ddus = this->crate->ddus();
      tmb = this->crate->tmbs().at(0);
      dmbs = this->crate->daqmbs();
    }

    int Action::getFormValueInt(const string form_element, xgi::Input *in)
    {
      const cgicc::Cgicc cgi(in);
      int form_value;
      cgicc::const_form_iterator name = cgi.getElement(form_element);
      if(name !=cgi.getElements().end())
	{
	  form_value = cgi[form_element]->getIntegerValue();
	}
      else
	{
	  XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
	}
      cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
      return form_value;
    }

    int Action::getFormValueIntHex(const string form_element, xgi::Input *in)
    {
      const cgicc::Cgicc cgi(in);
      int form_value;
      cgicc::const_form_iterator name = cgi.getElement(form_element);
      if(name !=cgi.getElements().end())
	{
	  stringstream convertor;
	  string hex_as_string = cgi[form_element]->getValue();
	  convertor << hex << hex_as_string;
	  convertor >> form_value; 
	}
      else
	{
	  XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
	}
      cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
      return form_value;
    }


    float Action::getFormValueFloat(const string form_element, xgi::Input *in)
    {
      const cgicc::Cgicc cgi(in);
      float form_value;
      cgicc::const_form_iterator name = cgi.getElement(form_element);
      if(name !=cgi.getElements().end())
	{
	  form_value = cgi[form_element]->getDoubleValue();
	}
      else
	{
	  XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
	}
      cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
      return form_value;
    }

    string Action::getFormValueString(const string form_element, xgi::Input *in)
    {
      const cgicc::Cgicc cgi(in);
      string form_value;
      cgicc::const_form_iterator name = cgi.getElement(form_element);
      if(name !=cgi.getElements().end())
	{
	  form_value = cgi[form_element]->getValue();
	}
      else
	{
	  XCEPT_RAISE( xcept::Exception, "Form element, " + form_element + ", was not found." );
	}
      cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
      return form_value;
    }

    void Action::AddButton(xgi::Output *out,
			   const string button_name,
			   const string button_style)
      throw (xgi::exception::Exception)
    {
      *out << cgicc::input()
	.set("type","submit")
	.set("style",button_style)
	.set("value",button_name)
	   << endl;
    }

    void Action::AddButtonWithTextBox(xgi::Output *out,
				      const string button_name,
				      const string textbox_name,
				      const string textbox_default_value,
				      const string button_style,
				      const string textbox_style)
      throw (xgi::exception::Exception)
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

    void Action::AddButtonWithTwoTextBoxes(xgi::Output *out,
					   const string button_name,
					   const string textbox_name1,
					   const string textbox_default_value1,
					   const string textbox_name2,
					   const string textbox_default_value2,
					   const string button_style,
					   const string textbox_style1,
					   const string textbox_style2)
      throw (xgi::exception::Exception)
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

    void Action::AddButtonWithLongTextBox(xgi::Output *out,
					  const string button_name,
					  const string textbox_name,
					  const string textbox_default_value,
					  const string button_style,
					  const string textbox_style)
      throw (xgi::exception::Exception) {
      *out << cgicc::div()
	.set("style", "border: #000 solid 1px; padding: 1em; ")
	   << cgicc::input()
	.set("type","submit")
	.set("style",button_style)
	.set("value",button_name)
	   << endl << cgicc::br() << endl
	   << cgicc::textarea()
	.set("style", textbox_style)
	.set("name",textbox_name)
	   << textbox_default_value
	   << cgicc::textarea()
	   << cgicc::div();
    }
  }
}
