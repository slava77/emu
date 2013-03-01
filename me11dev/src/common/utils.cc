#include "emu/me11dev/utils.h"

#include "xcept/Exception.h"

#include <bitset>
#include <limits>

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

using namespace std;

namespace emu{ namespace me11dev{


unsigned int binaryStringToUInt(const std::string& s)
{
  return  static_cast<unsigned int>( bitset<numeric_limits<unsigned long>::digits>(s).to_ulong() );
}


std::string withoutSpecialChars(const std::string& s)
{
  std::string sout = s;
  char chars[10] = " /#\t\n";
  for (int i = 0; i < 5; ++i)
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
  if(name != cgi.getElements().end())
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
  if(name != cgi.getElements().end())
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
  if(name != cgi.getElements().end())
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
  if(name != cgi.getElements().end())
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


}} // namespaces

