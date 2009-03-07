#include "emu/base/WebReporter.h"

#include <time.h>
#include <iomanip>
#include <sstream>

emu::base::WebReporter::WebReporter(xdaq::ApplicationStub *stub)
  throw (xdaq::exception::Exception)
  : xdaq::WebApplication(stub){
  xgi::bind(this, &emu::base::WebReporter::ForEmuPage1, "ForEmuPage1");
}
  
void emu::base::WebReporter::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  std::map<std::string,std::string> name_value = materialToReportOnPage1();
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << std::endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<           "\" url=\""         << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<           "\" dateTime=\""    << getDateTime() << "\">" << std::endl;
  for ( std::map<std::string,std::string>::const_iterator nv = name_value.begin(); nv != name_value.end(); ++nv ){
    *out << "  <monitorable name=\"" << nv->first << "\" value=\"" << nv->second << "\"/>" << std::endl;
  }
  *out << "</ForEmuPage1>" << std::endl;
}

std::string emu::base::WebReporter::getDateTime(){
  time_t t;
  struct tm *tm;
    
  time( &t );
  tm = gmtime( &t );
    
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << tm->tm_year+1900 << "-"
     << std::setfill('0') << std::setw(2) << tm->tm_mon+1     << "-"
     << std::setfill('0') << std::setw(2) << tm->tm_mday      << " "
     << std::setfill('0') << std::setw(2) << tm->tm_hour      << ":"
     << std::setfill('0') << std::setw(2) << tm->tm_min       << ":"
     << std::setfill('0') << std::setw(2) << tm->tm_sec       << " UTC";
    
  return ss.str();
}
