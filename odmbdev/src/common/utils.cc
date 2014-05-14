#include "emu/odmbdev/utils.h"

#include "xcept/Exception.h"

#include <bitset>
#include <limits>
#include <sstream>
#include <string>
/*****************************************************************************
 * Utility functions
 *****************************************************************************/

using namespace std;

namespace emu{ namespace odmbdev{
  std::string fix_width(const double number, const std::streamsize width){
    std::ostringstream oss("");
    std::ios_base::fmtflags opts(std::ios::showpoint & ~std::ios::showpos | std::ios::right | std::ios::dec | std::ios::fixed);
    oss.fill('0');
    oss.flags(opts);
    oss.precision(width);
    oss << number;
    std::string printed_num(oss.str());
    std::string::size_type decimal_location(printed_num.find('.'));
    printed_num.resize((static_cast<std::string::size_type>(width)>decimal_location)?width:(decimal_location+1u));
    if(printed_num.size()>0 && printed_num.at(printed_num.size()-1)=='.'){
      printed_num.resize(printed_num.size()-1);
      printed_num=" "+printed_num;
    }
    return printed_num;
  }

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
    //cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
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
    //cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
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
    //cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
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
    //cout<<"\""<<form_element<<"\"->"<<form_value<<endl;
    return form_value;
  }


  string GetLogFileName(unsigned int port_number) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char year[10], month[10], day[10], hour[10], minute[10], second[10];
    strftime(year, 10, "%g", timeinfo ); strftime(month, 10, "%m", timeinfo ); strftime(day, 10, "%d", timeinfo ); 
    strftime(hour, 10, "%H", timeinfo ); strftime(minute, 10, "%M", timeinfo ); strftime(second, 10, "%S", timeinfo );
    string file_name("/local/data/cscme11/ucsb/logfiles/odmb_log_");
    char time_stamp[100];
    sprintf (time_stamp,"%s%s%s_%s%s%s_p%d.log",year,month,day,hour,minute,second,port_number);
    file_name+=time_stamp;
    return file_name;
  }

  string FixLength(unsigned int Number, unsigned int Length, bool isHex){
    std::stringstream Stream;
    if(isHex) Stream << std::hex << Number;
    else Stream << std::dec << Number;
    string sNumber = Stream.str();
    for(unsigned int cha=0; cha<sNumber.size(); cha++) sNumber[cha] = toupper(sNumber[cha]);
    while(sNumber.size() < Length) sNumber = " " + sNumber;
    return sNumber;
  }

  void UpdateLog(VMEWrapper* vme_wrapper, unsigned int slot, ostringstream& out_local) {
    unsigned int addr_unique_id(0x4100), addr_fwv(0x4200), addr_fwb(0x4300);
    string unique_id = FixLength(vme_wrapper->VMERead(addr_unique_id,slot,"Read unique ID"),4,true);
    string fwv =  FixLength(vme_wrapper->VMERead(addr_fwv,slot,"Read FW version"),4,true);
    string fwb =  FixLength(vme_wrapper->VMERead(addr_fwb,slot,"Read FW version"),4,true);
    time_t rawtime;
    time (&rawtime);
    char time_stamp[100];
    sprintf(time_stamp,"%s",ctime(&rawtime));
    
    // locate log file
    string filename("/local/data/cscme11/ucsb/logfiles/button_output/odmb_"+unique_id+".log");
    //std::cout << "Wrote to: " << filename << std::endl;
    ofstream logfile(filename.c_str(), std::ofstream::app);
    logfile << "FW version " << fwv << ", build " << fwb << " -- " << time_stamp;
    logfile << out_local.str();
    logfile.close();
  }

  void JustifyHdr(string &hdr) { // align the PASSED/NOT PASSED messages for production tests
    unsigned int nSpaces(45-hdr.size());
    for (unsigned int space(0); space<nSpaces; space++) {
      hdr+=" ";
    }
    return;
  }

}} // namespaces

