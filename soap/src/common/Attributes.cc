#include "emu/soap/Attributes.h"

using namespace std;

const emu::soap::Attributes emu::soap::Attributes::none  = emu::soap::Attributes();
const string emu::soap::Attributes::emptyString = string();


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
emu::soap::Attributes::add( const std::string &name, 
			    xdata::Serializable* value, 
			    const std::string &prefix, 
			    const std::string &namespaceURI ){
  insert( make_pair( QualifiedName( name, prefix, namespaceURI ), value ) );
  return *this;
}

string
emu::soap::Attributes::getType( const string &name, const string& namespaceURI ) const {
  map< QualifiedName, xdata::Serializable* >::const_iterator i = find( QualifiedName( name, namespaceURI ) );
  if ( i != end() ) return i->second->type();
  return string();
}

xdata::Serializable*
emu::soap::Attributes::getValue( const string& name, const string& namespaceURI ) {
  map< QualifiedName, xdata::Serializable* >::const_iterator i = find( QualifiedName( name, namespaceURI ) );
  if ( i != end() ) return i->second;
  return NULL;
}

ostream&
operator<<( ostream& os,  const emu::soap::Attributes& attributes ){
  emu::soap::Attributes::const_iterator p;  
  for ( p=attributes.begin(); p!=attributes.end(); ++p ){
      os << "(name:"   << p->first
	 << " type:'"  << p->second->type()
	 << "' value:'" << p->second->toString()
	 << "') ";
  }
  return os;
}
