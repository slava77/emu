#include "emu/soap/Attributes.h"

using namespace std;

const emu::soap::Attributes emu::soap::Attributes::none  = emu::soap::Attributes();


emu::soap::Attributes::Attributes() : usePrefixOfParent_( true ){}

emu::soap::Attributes&
emu::soap::Attributes::setUsePrefixOfParent( bool usePrefixOfParent ){
  usePrefixOfParent_ = usePrefixOfParent;
  return *this;
}

bool
emu::soap::Attributes::getUsePrefixOfParent() const {
  return usePrefixOfParent_;
}

emu::soap::Attributes&
emu::soap::Attributes::add( const emu::soap::QualifiedName& name, 
			    xdata::Serializable* value ){
  insert( make_pair( name, value ) );
  return *this;
}

string
emu::soap::Attributes::getType( const emu::soap::QualifiedName& name ) const {
  map< QualifiedName, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second->type();
  return string();
}

xdata::Serializable*
emu::soap::Attributes::getValue( const emu::soap::QualifiedName& name ) {
  map< QualifiedName, xdata::Serializable* >::const_iterator i = find( name );
  if ( i != end() ) return i->second;
  return NULL;
}

ostream&
emu::soap::operator<<( ostream& os,  const emu::soap::Attributes& attributes ){
  emu::soap::Attributes::const_iterator p;
  for ( p=attributes.begin(); p!=attributes.end(); ++p ){
    emu::soap::Attributes::const_iterator pp = p;
    ++pp;
    os << ( p == attributes.begin() ? "[" : "" )
       << "(name:'"   << p->first << "'"
       << " type:'"  << p->second->type() << "'"
       << " value:'" << p->second->toString() << "'"
       << ")"
       << ( pp == attributes.end() ? "]" : ", " );
  }
  return os;
}
