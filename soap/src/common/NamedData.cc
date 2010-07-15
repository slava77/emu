#include "emu/soap/NamedData.h"

using namespace std;

emu::soap::NamedData&
emu::soap::NamedData::add( const std::string &name, xdata::Serializable* value ){
  insert( make_pair( name, value ) );
  return *this;
}

string
emu::soap::NamedData::getType( const string &name ) const{
  map< string, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second->type();
  return string();
}

xdata::Serializable*
emu::soap::NamedData::getValue( const string &name ) {
  map< string, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second;
  return NULL;
}

ostream&
emu::soap::operator<<( ostream& os,  const emu::soap::NamedData& parameters ){
  emu::soap::NamedData::const_iterator p;  
  for ( p=parameters.begin(); p!=parameters.end(); ++p ){
    os << "(name:'" << p->first
       << "' type:'" << p->second->type()
       << "' value:'" << p->second->toString()
       << "') ";
  }
  return os;
}
