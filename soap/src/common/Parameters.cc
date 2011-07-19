#include "emu/soap/Parameters.h"

using namespace std;

const emu::soap::Parameters emu::soap::Parameters::none  = emu::soap::Parameters();

emu::soap::Parameters::Parameters() : usePrefixOfParent_( true ){}

emu::soap::Parameters&
emu::soap::Parameters::setUsePrefixOfParent( bool usePrefixOfParent ){
  usePrefixOfParent_ = usePrefixOfParent;
  return *this;
}

bool
emu::soap::Parameters::getUsePrefixOfParent() const {
  return usePrefixOfParent_;
}

emu::soap::Parameters&
emu::soap::Parameters::add( const emu::soap::QualifiedName& name, 
			    xdata::Serializable* value, 
			    emu::soap::Attributes *attributes ){
  insert( make_pair( name, make_pair( value, attributes ) ) );
  return *this;
}

string
emu::soap::Parameters::getType( const emu::soap::QualifiedName& name ) const {
  map< QualifiedName, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
  if ( i != end() ) return i->second.first->type();
  return string();
}

xdata::Serializable*
emu::soap::Parameters::getValue( const emu::soap::QualifiedName& name ) {
  map< QualifiedName, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
  if ( i != end() ) return i->second.first;
  return NULL;
}

const emu::soap::Attributes* 
emu::soap::Parameters::getAttributes( const emu::soap::QualifiedName& name ) const {
  map< QualifiedName, pair< xdata::Serializable*, const emu::soap::Attributes* > >::const_iterator i = find( name );
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
