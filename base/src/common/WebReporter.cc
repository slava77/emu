#include "emu/base/WebReporter.h"

#include <time.h>
#include <iomanip>
#include <sstream>

using namespace std;

emu::base::WebReporter::WebReporter(xdaq::ApplicationStub *stub)
  throw (xdaq::exception::Exception)
  : xdaq::WebApplication(stub){
  xgi::bind(this, &emu::base::WebReporter::ForEmuPage1, "ForEmuPage1");
}
  
void 
emu::base::WebReporter::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  vector<emu::base::WebReportItem> items = materialToReportOnPage1();
  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localDateTime=\"" << getLocalDateTime() << "\">" << endl;
  for ( vector<WebReportItem>::const_iterator i = items.begin(); i != items.end(); ++i ){
    *out << "  <monitorable name=\"" << i->getName()
	 <<            "\" value=\"" << i->getValue()
	 <<  "\" nameDescription=\"" << i->getNameDescription()
	 << "\" valueDescription=\"" << i->getValueDescription()
	 <<          "\" nameURL=\"" << i->getNameURL()
	 <<         "\" valueURL=\"" << i->getValueURL()
	 << "\"/>" << endl;
  }
  *out << "</ForEmuPage1>" << endl;
}

string 
emu::base::WebReporter::getLocalDateTime(){
  time_t t;
  struct tm *tm;
    
  time( &t );
  tm = localtime( &t );
    
  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << " "
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec;
    
  return ss.str();
}
