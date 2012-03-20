#include "emu/soap/QualifiedName.h"
#include <iostream>

using namespace std;

const string emu::soap::QualifiedName::emptyString = string();


emu::soap::QualifiedName::QualifiedName( const string& name, const string& namespaceURI, const string& prefix )
  : name_        ( name         )
  , namespaceURI_( namespaceURI )
  , prefix_      ( prefix       )
{}

emu::soap::QualifiedName::QualifiedName( const char* name )
  : name_        ( name )
  , namespaceURI_( ""   )
  , prefix_      ( ""   )
{}

int emu::soap::QualifiedName::operator<( const QualifiedName& other ) const {
  // This method is used by std::map, and is needed to ensure that no duplicate attributes can be added to elements.
  // Comparison is by the names and the namespace URIs, the prefixes don't matter.
  int result = 0;
  if      ( name_  < other.getName() ) result = 1;
  else if ( name_ == other.getName() ) result = ( namespaceURI_ < other.getNamespaceURI() ? 1 : 0 );
  //cout << *this << " < " << other << " ? " << result << endl;
  return result;
}

int emu::soap::QualifiedName::operator==( const QualifiedName& other ) const {
  // This method may not be needed. std::map doesn't seem to use it. (It only uses operator<.)
  // If the names and the namespace URIs are the same, they are equal, regardless of the prefix.
  cout << *this << " == " << other << " ? " 
       << ( name_ == other.getName() && namespaceURI_ == other.getNamespaceURI() ? 1 : 0 ) << endl;
  return ( name_ == other.getName() && namespaceURI_ == other.getNamespaceURI() ? 1 : 0 );
}

ostream& emu::soap::operator<<( ostream& os, const emu::soap::QualifiedName& qualifiedName ){
  if ( qualifiedName.prefix_.size() > 0 ){
    os << qualifiedName.prefix_ << ":";
  }
  os << qualifiedName.name_;
  if ( qualifiedName.namespaceURI_.size() > 0 ){
    os << " xmlns";
    if ( qualifiedName.prefix_.size() > 0 ){
      os << ":" << qualifiedName.prefix_;
    }
    os << "=\"" << qualifiedName.namespaceURI_ << "\"";
  }
  return os;
}
