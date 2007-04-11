#include "emu/emuFarmer/include/EmuProcessDescriptor.h"
#include <sstream>
#include <iostream>

EmuProcessDescriptor::EmuProcessDescriptor() :
  URL_(""),
  normalizedURL_(""),
  startingLogLevel_("WARN"),
  logLevel_(""),
  jobControlNormalizedURL_(""),
  deviceName_(""),
  jobControlAppDescriptor_(NULL),
  selected_( false ), 
  jobId_( -1 ),    
  startTime_( 0 )
{}

EmuProcessDescriptor::EmuProcessDescriptor( const string& url ) 
  throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL ) :
  URL_(""),
  normalizedURL_(""),
  startingLogLevel_("WARN"),
  logLevel_(""),
  jobControlNormalizedURL_(""),
  deviceName_(""),
  jobControlAppDescriptor_(NULL),
  selected_( false ), 
  jobId_( -1 ),    
  startTime_( 0 )
{
  setURL( url );
}

void EmuProcessDescriptor::setURL( const string& url ) 
  throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL )
{
  URL_ = url;
  // Check its format
  toolbox::net::URL u( url );
  // Try to resolve it
  normalizedURL_ = u.getNormalizedURL();
}


EmuProcessDescriptor::EmuProcessDescriptor( const EmuProcessDescriptor& ep ){
  *this = ep;
}

EmuProcessDescriptor::~EmuProcessDescriptor(){
  applications_.clear();
}

EmuProcessDescriptor& EmuProcessDescriptor::operator=( const EmuProcessDescriptor& ep ){
  URL_                     = ep.URL_;
  normalizedURL_           = ep.normalizedURL_;
  startingLogLevel_        = ep.startingLogLevel_;
  logLevel_                = ep.logLevel_;
  jobControlNormalizedURL_ = ep.jobControlNormalizedURL_;
  deviceName_              = ep.deviceName_;
  jobControlAppDescriptor_ = ep.jobControlAppDescriptor_;
  selected_                = ep.selected_;
  jobId_                   = ep.jobId_;
  startTime_               = ep.startTime_;
  applications_.insert( ep.applications_.begin(), ep.applications_.end() );
  return *this;
}

void EmuProcessDescriptor::print( ostream& os ) const {
  os << "URL: " << URL_
     << "   normalized URL: " << normalizedURL_
     << "   starting log level: " << startingLogLevel_
     << "   selected: " << (selected_?"yes":"no")
     << "   job id: " << jobId_
     << "   device: " << ( deviceName_.size() ? deviceName_ : string("-") )
     << "   apps: ";
  for ( set< pair<string, int> >::iterator a = applications_.begin();
	a != applications_.end();
	++a )
    if ( a->second >= 0 ) os << " " << a->first << "." << a->second;
    else                  os << " " << a->first;
  os << endl;
}

void EmuProcessDescriptor::setJobId( const string& jobId ){
  stringstream id;
  id << jobId;
  id >> jobId_;
}

string EmuProcessDescriptor::getHost() const {
  // This should never throw as at this point URL_ has already been tested for format
  toolbox::net::URL u( URL_ );
  return u.getHost();
}

string EmuProcessDescriptor::getNormalizedHost() const {
  // This should never throw as normalizedURL_ should be properly formatted by definition
  toolbox::net::URL u( normalizedURL_ );
  return u.getHost();
}

int EmuProcessDescriptor::getPort() const {
  // This should never throw as at this point URL_ has already been tested for format
  toolbox::net::URL u( URL_ );
  return u.getPort();
}

bool EmuProcessDescriptor::hasApplication( const string& name ) const {
  for ( set< pair<string, int> >::iterator a = applications_.begin();
	a != applications_.end();
	++a )
    if ( a->first.find( name, 0 ) != string::npos ) return true;
  return false;
}

bool EmuProcessDescriptor::operator<( const EmuProcessDescriptor& epd ) const {
  return ( normalizedURL_ < epd.normalizedURL_ ); 
}

bool EmuProcessDescriptor::operator==( const EmuProcessDescriptor& epd ) const {
  return ( normalizedURL_ == epd.normalizedURL_ );
}
