#ifndef __emu_farmer_utils_SOAP_h__
#define __emu_farmer_utils_SOAP_h__

#include "xoap/MessageFactory.h"

#include <string>

using namespace std;

namespace emu { namespace farmer { namespace utils {

  xoap::MessageReference createStartXdaqExeSOAPMsg( const string& host, const int port, const string& user,
						    const string& jid,
						    map<string,string> environmentVariables )
    throw (xcept::Exception);

  xoap::MessageReference createConfigureXdaqExeSOAPMsg( string& configuration )
    throw (xcept::Exception);

  xoap::MessageReference createKillByJidSOAPMsg( const string& jid )
    throw (xcept::Exception);

  xoap::MessageReference createParameterGetSOAPMsg
  ( const string appClass,
    const string paramName,
    const string paramType )
    throw (xcept::Exception);

  string extractScalarParameterValueFromSoapMsg
  ( xoap::MessageReference msg,
    const string           paramName )
    throw (xcept::Exception);

  string extractJidFromSoapMsg( xoap::MessageReference msg )
    throw (xcept::Exception);

  xoap::MessageReference postSOAP
  ( xoap::MessageReference message, 
    const string& fromURL,
    const string& toURL,
    const int localId ) 
    throw (xcept::Exception);

  DOMNode* findNode( DOMNodeList *nodeList, const string& nodeLocalName )
    throw (xcept::Exception);

}}}

#endif
