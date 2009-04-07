#include "emu/base/WebReporter.h"
#include "xcept/tools.h"

#include <iomanip>
#include <sstream>
#include <exception>

using namespace std;

emu::base::WebReporter::WebReporter(xdaq::ApplicationStub *stub)
  throw (xdaq::exception::Exception)
  : xdaq::WebApplication(stub){
  xgi::bind(this, &emu::base::WebReporter::ForEmuPage1, "ForEmuPage1");
}
  
void 
emu::base::WebReporter::ForEmuPage1(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  try{
    // Update only if more than 5 seconds have passed since last update
    pair<time_t,string> timeNow = getLocalDateTime();
    if ( timeNow.first > latestTime_.first + 5 ){
      vector<emu::base::WebReportItem> items = materialToReportOnPage1();
      latestItems_ = materialToReportOnPage1();
      latestTime_ = timeNow;
    }
  }
  catch( xcept::Exception e ){
    LOG4CPLUS_WARN( getApplicationLogger(), "Failed to get report for Page 1 : " << xcept::stdformat_exception_history(e) );
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to get report for Page 1 : ", e );
  }
  catch( const std::exception& e ){
    LOG4CPLUS_WARN( getApplicationLogger(), "Failed to get report for Page 1 : " << e.what() );
    XCEPT_DECLARE( xcept::Exception, ex, string("std::exception caught: ") +  e.what() );
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to get report for Page 1 : ", ex );
  }
  catch(...){
    LOG4CPLUS_WARN( getApplicationLogger(), "Failed to get report for Page 1 : Unexpected exception." );
    XCEPT_RAISE( xgi::exception::Exception, "Failed to get report for Page 1 : Unexpected exception." );
  }

  *out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl
       << "<?xml-stylesheet type=\"text/xml\" href=\"/emu/base/html/EmuPage1_XSL.xml\"?>" << endl
       << "<ForEmuPage1 application=\"" << getApplicationDescriptor()->getClassName()
       <<                   "\" url=\"" << getApplicationDescriptor()->getContextDescriptor()->getURL()
       <<         "\" localUnixTime=\"" << latestTime_.first 
       <<         "\" localDateTime=\"" << latestTime_.second 
       << "\">" << endl;
  for ( vector<WebReportItem>::const_iterator i = latestItems_.begin(); i != latestItems_.end(); ++i ){
    *out << "  <monitorable name=\"" << i->getName()
	 <<            "\" value=\"" << i->getValue()
	 <<  "\" nameDescription=\"" << i->getNameDescription()
	 << "\" valueDescription=\"" << i->getValueDescription()
	 <<          "\" nameURL=\"" << ( i->getNameURL().find_first_not_of(" ")  == string::npos ? "/" : i->getNameURL()  )
	 <<         "\" valueURL=\"" << ( i->getValueURL().find_first_not_of(" ") == string::npos ? "/" : i->getValueURL() )
	 << "\"/>" << endl;
  }
  *out << "</ForEmuPage1>" << endl;
}

pair<time_t,string> 
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
    
  return make_pair( t, ss.str() );
}
