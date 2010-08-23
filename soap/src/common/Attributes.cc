#include "emu/soap/Attributes.h"

const emu::soap::Attributes emu::soap::Attributes::none  = emu::soap::Attributes();

using namespace std;

emu::soap::Attributes::Attributes() : usePrefix_( true ){}

emu::soap::Attributes&
emu::soap::Attributes::setUsePrefix( bool usePrefix ){
  usePrefix_ = usePrefix;
  return *this;
}

bool
emu::soap::Attributes::getUsePrefix() const {
  return usePrefix_;
}

emu::soap::Attributes&
emu::soap::Attributes::add( const std::string &name, xdata::Serializable* value ){
  insert( make_pair( name, value ) );
  return *this;
}

string
emu::soap::Attributes::getType( const string &name ) const {
  map< string, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second->type();
  return string();
}

xdata::Serializable*
emu::soap::Attributes::getValue( const string &name ) {
  map< string, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second;
  return NULL;
}

ostream&
emu::soap::operator<<( ostream& os,  const emu::soap::Attributes& attributes ){
  emu::soap::Attributes::const_iterator p;  
  for ( p=attributes.begin(); p!=attributes.end(); ++p ){
      os << "(name:'"   << p->first
	 << "' type:'"  << p->second->type()
	 << "' value:'" << p->second->toString()
	 << "') ";
  }
  return os;
}
