#include "emu/farmer/ProcessDescriptor.h"
#include <sstream>
#include <iostream>

emu::farmer::ProcessDescriptor::ProcessDescriptor() :
  selected_( false )
{}

emu::farmer::ProcessDescriptor::ProcessDescriptor( const string& url ) 
  throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL ) :
  selected_( false ) 
{
  setURL( url );
}

void emu::farmer::ProcessDescriptor::setURL( const string& url ) 
  throw ( toolbox::net::exception::MalformedURL, toolbox::net::exception::BadURL )
{
  URL_ = url;
  // Check its format
  toolbox::net::URL u( url );
  // Try to resolve it
  normalizedURL_ = u.getNormalizedURL();
}

emu::farmer::ProcessDescriptor::~ProcessDescriptor(){
  environmentVariables_.clear();
}

void emu::farmer::ProcessDescriptor::print( ostream& os ) const {
  os << "Process"
     << endl << "   name: " << name_ 
     << endl << "   URL: " << URL_
     << endl << "   normalized URL: " << normalizedURL_
     << endl << "   selected: " << (selected_?"yes":"no")
     << endl << "   state: " << state_;
    //      << endl << "   jid: " << jid_;
  os << endl << "   environment variables: ";
  for ( map<string,string>::const_iterator e=environmentVariables_.begin(); e!=environmentVariables_.end(); ++e ){
    os << " " << e->first << "=" << e->second;
  }
  os << endl << "   JobControl: " << jobControlURI_
     << endl << "   config path: " << xdaqConfigPath_
     << endl;
}

string emu::farmer::ProcessDescriptor::getHost() const {
  // This should never throw as at this point URL_ has already been tested for format
  toolbox::net::URL u( URL_ );
  return u.getHost();
}

string emu::farmer::ProcessDescriptor::getNormalizedHost() const {
  // This should never throw as normalizedURL_ should be properly formatted by definition
  toolbox::net::URL u( normalizedURL_ );
  return u.getHost();
}

int emu::farmer::ProcessDescriptor::getPort() const {
  // This should never throw as at this point URL_ has already been tested for format
  toolbox::net::URL u( URL_ );
  return u.getPort();
}

string emu::farmer::ProcessDescriptor::getPath() const {
  // This should never throw as at this point URL_ has already been tested for format
  toolbox::net::URL u( URL_ );
  return u.getPath();
}

int emu::farmer::ProcessDescriptor::getLid() const {
  int lid = -1;
  size_t lidPos = URL_.rfind("lid=");
  if ( lidPos!=string::npos ){
    stringstream sslid;
    sslid << URL_.substr( lidPos+4 );
    sslid >> lid;
  }
  return lid;
}


string emu::farmer::ProcessDescriptor::getJid() const {
  stringstream ssport; ssport << getPort();
  return string("farmer://") + getHost() + ":" + ssport.str();
}

void emu::farmer::ProcessDescriptor::setEnvironmentString( const string& environmentString ){
  // Split environmentString string into NAME=value strings
  if ( environmentString.size() == 0 ) return;
  vector<string> env;
  size_t space = 0;
  do{
    size_t start = environmentString.find_first_not_of( ' ', space );
    space = environmentString.find_first_of( ' ', start );
    env.push_back( environmentString.substr( start, space-start ) );
  } while ( space != string::npos );
  // Split NAME=value strings
  for ( vector<string>::iterator e=env.begin(); e!=env.end(); ++e ){
    size_t equalSign = e->find_first_of( '=' );
    if ( equalSign != string::npos ){
      environmentVariables_[e->substr( 0, equalSign )] = ( equalSign+1 == string::npos ? "" : e->substr( equalSign+1 ) );
    }
  }
}

bool emu::farmer::ProcessDescriptor::operator<( const emu::farmer::ProcessDescriptor& pd ) const {
  return ( normalizedURL_+"/"+getPath() < pd.normalizedURL_+"/"+pd.getPath() ); 
}

bool emu::farmer::ProcessDescriptor::operator==( const emu::farmer::ProcessDescriptor& pd ) const {
  return ( normalizedURL_+"/"+getPath() == pd.normalizedURL_+"/"+pd.getPath() );
}
