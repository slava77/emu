#include "emu/farmer/utils/Cgi.h"

#include "cgicc/HTMLClasses.h"
#include "toolbox/regex.h"

#include <exception>
#include <sstream>
#include <iostream>

using namespace std;

map<string,string>
emu::farmer::utils::selectFromQueryString( std::vector<cgicc::FormEntry>& fev, const string namePattern )
  throw( xcept::Exception ){

  map<string,string> nameValue;

  try{

    std::vector<cgicc::FormEntry>::const_iterator fe;

    for ( fe=fev.begin(); fe!=fev.end(); ++fe ){
      bool isMatched = false;

      try{
	isMatched = toolbox::regx_match( fe->getName(), namePattern );
      }catch( xcept::Exception& e ){
	stringstream ss;
	ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": ";
	XCEPT_RETHROW( xcept::Exception, ss.str(), e );
      }catch( std::exception& e ){
	stringstream ss;
	ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": " << e.what();
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }catch(...){
	stringstream ss;
	ss << "Regex match of pattern \"" << namePattern << "\" failed in \"" << fe->getName() << "\": Unexpected exception";
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }

      if ( isMatched ) nameValue[fe->getName()] = fe->getValue();
    }

  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to select from HTTP query string: ", e );
  }catch( std::exception& e ){
    stringstream ss; ss << "Failed to select from HTTP query string: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to select from HTTP query string: Unexpected exception" );
  }

  return nameValue;
}

void
emu::farmer::utils::redirect(xgi::Input *in, xgi::Output *out){
  // Redirect for the action not to be repeated if the 'reload' button is hit.
  std::string url = in->getenv( "PATH_TRANSLATED" );
  std::size_t p = url.find( "/" + in->getenv( "PATH_INFO" ) + "?" );
  if ( p != std::string::npos ){
    cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
    header.getStatusCode( 303 );
    header.getReasonPhrase( "See Other" );
    // Use the following line to redirect to the default web page in the same directory.
    header.addHeader( "Location", url.substr(0, p) + "/" );
    // Use the following line to redirect to the same page stripped of the query string.
    // header.addHeader( "Location", url.substr(0, p) + "/" + in->getenv("PATH_INFO") );
  }
}

void emu::farmer::utils::redirectTo( const string newURL, xgi::Input *in, xgi::Output *out ){
  cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader( "Location", newURL );
}
