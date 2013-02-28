#include "emu/me11dev/utils.h"

#include "xcept/Exception.h"

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

using namespace std;

namespace emu{ 
  namespace me11dev{

    unsigned int binaryStringToUInt(const std::string& s)
    {
      // brute force way to turn a string of 0 and 1 into an unsigned int
      unsigned int uint=0; // return value
      int nstr = s.size(); // # bits in string
      int nmax = 8*int(sizeof(unsigned int)); // # bits in unsigned int
    
      for(int i = nstr-1; i >= 0 && i >= nstr-nmax; i--){
	if(s[i] != '0') uint = uint | (0x1 << (nstr-i-1));
      }
      return uint;
    }

    std::string withoutSpecialChars(const std::string& s)
    {
      std::string sout = s;
      char chars[10] = " /#\t\n";
      for (int i; i < 5; ++i)
      {
        char c = chars[i];
        while(sout.find(c,0) != std::string::npos) sout.erase(sout.find(c,0),1);
      }
      return sout;
    }


    int getFormValueInt(const string& form_element, xgi::Input *in)
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

    int getFormValueIntHex(const string& form_element, xgi::Input *in)
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


    float getFormValueFloat(const string& form_element, xgi::Input *in)
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

    string getFormValueString(const string& form_element, xgi::Input *in)
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

  }
}

