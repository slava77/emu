#include "emu/soap/Parameters.h"

using namespace std;

emu::soap::Parameters::Parameters() : usePrefix_( true ){}

emu::soap::Parameters&
emu::soap::Parameters::setUsePrefix( bool usePrefix ){
  usePrefix_ = usePrefix;
  return *this;
}

bool
emu::soap::Parameters::getUsePrefix() const {
  return usePrefix_;
}

emu::soap::Parameters&
emu::soap::Parameters::add( const std::string &name, xdata::Serializable* value, emu::soap::Attributes *attributes ){
  insert( make_pair( name, make_pair( value, attributes ) ) );
  return *this;
}

string
emu::soap::Parameters::getType( const string &name ) const {
  map< string, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
  if ( i != end() ) return i->second.first->type();
  return string();
}

xdata::Serializable*
emu::soap::Parameters::getValue( const string &name ) {
  map< string, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
  if ( i != end() ) return i->second.first;
  return NULL;
}

const emu::soap::Attributes* 
emu::soap::Parameters::getAttributes( const string &name ) const {
  map< string, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
  if ( i != end() ) return i->second.second;
  return NULL;  
}

ostream&
emu::soap::operator<<( ostream& os,  const emu::soap::Parameters& parameters ){
  emu::soap::Parameters::const_iterator p;  
  for ( p=parameters.begin(); p!=parameters.end(); ++p ){
    os << "(name:'"   << p->first
       << "' type:'"  << p->second.first->type()
       << "' value:'" << p->second.first->toString();
    if ( p->second.second != NULL ){
      os << " 'attributes:'" << *p->second.second;
    }
    os << "') ";
  }
  return os;
}
