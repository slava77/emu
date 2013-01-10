#include "emu/me11dev/Action.h"

using namespace cgicc;

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
      return form_value;
    }

    void Action::AddButton(xgi::Output *out,
			   const string button_name)
      throw (xgi::exception::Exception)
    {
      *out << cgicc::input().set("type","submit")
	                    .set("value",button_name)
	   << endl;
    }

    void Action::AddButtonWithTextBox(xgi::Output *out,
				      const string button_name,
				      const string textboxname,
				      const string textbox_default_value)
      throw (xgi::exception::Exception)
    {
      *out << cgicc::input().set("type","submit")
	                    .set("value",button_name)
	   << endl
	   << cgicc::input().set("type","text")
	                    .set("value",textbox_default_value)
                            .set("name",textboxname);
    }

    void Action::AddButtonWithTwoTextBoxes(xgi::Output *out,
					   const string button_name,
					   const string textboxname1,
					   const string textbox_default_value1,
					   const string textboxname2,
					   const string textbox_default_value2)
      throw (xgi::exception::Exception)
    {
      *out << cgicc::input().set("type","submit")
	                    .set("value",button_name)
	   << endl
	   << cgicc::input().set("type","text")
	                    .set("value",textbox_default_value1)
                            .set("name",textboxname1)
	   << cgicc::input().set("type","text")
	                    .set("value",textbox_default_value2)
                            .set("name",textboxname2);
    }

    void Action::AddButtonWithLongTextBox(xgi::Output *out,
					  const string button_name,
					  const string textboxname,
					  const string textbox_default_value)
      throw (xgi::exception::Exception) {
      *out << cgicc::div().set("style",
			       // we need a std::string to use `+'
			       string("border: #000 solid 1px; ")
			       + "padding: 1em; ")
	   << cgicc::input().set("type","submit")
	                    .set("value",button_name)
	   << endl << cgicc::br() << endl
	   << cgicc::textarea().set("style",
				    string("width: 100%; ")
				    + "margin-top: 1em; "
				    + "height: 10em; ")
	                       .set("name",textboxname)
	   << textbox_default_value
	   << cgicc::textarea()
	   << cgicc::div();
    }
  }
}
