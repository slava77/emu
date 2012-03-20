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
  this->push_back( make_pair( name, make_pair( value, attributes ) ) );
  return *this;
}

string
emu::soap::Parameters::getType( const size_t index ) const {
  if ( index < this->size() ) return this->at( index ).second.first->type();
  return string();
}

xdata::Serializable*
emu::soap::Parameters::getValue( const size_t index ) {
  if ( index < this->size() ) return this->at( index ).second.first;
  return NULL;
}

const emu::soap::Attributes* 
emu::soap::Parameters::getAttributes( const size_t index ) const {
  if ( index < this->size() ) return this->at( index ).second.second;
  return NULL;  
}

ostream&
emu::soap::operator<<( ostream& os, const emu::soap::Parameters& parameters ){
  emu::soap::Parameters::const_iterator p;
  for ( p=parameters.begin(); p!=parameters.end(); ++p ){
    emu::soap::Parameters::const_iterator pp = p;
    ++pp;
    os << ( p == parameters.begin() ? "[" : "" )
       << "(name:'"  << p->first << "'"
       << " type:'"  << p->second.first->type() << "'"
       << " value:'" << p->second.first->toString() << "'";
    if ( p->second.second != NULL ){
      os << " attributes:" << *p->second.second;
    }
    os << ")"
       << ( pp == parameters.end() ? "]" : ", " );
  }
  return os;
}
