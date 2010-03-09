#include "emu/base/Component.h"
#include "toolbox/regex.h"

using namespace std;

bool 
emu::base::Component::operator==( const emu::base::Component& other ) const {
  set<string>::const_iterator i1;
  set<string>::const_iterator i2;
  set<string> ids = other.getIds();
  for ( i2=ids.begin(); i2!=ids.end(); ++i2 ){
    for ( i1=ids_.begin(); i1!=ids_.end(); ++i1 ){
      if ( *i1 == *i2 ) return true;
    }
  }
  return false;
}

bool 
emu::base::Component::isMatchedBy( const string& regex, vector<string>& matches ) const {
  try{
    set<string>::const_iterator id;
    for ( id=ids_.begin(); id!=ids_.end(); ++id ){
      if ( toolbox::regx_match( *id, regex, matches) ) return true;
    }
  }
  catch(...){} // May throw bad_alloc or invalid_argument, but we'll ignore them anyway.
  return false;
}

ostream& emu::base::operator<<( ostream& os, const emu::base::Component& c ){
  os << "ComponentIds( ";
  set<string>::const_iterator i;
  for ( i=c.getIds().begin(); i!=c.getIds().end(); ++i ){
    set<string>::iterator j = i;
    ++j;
    os << *i << ( j == c.getIds().end() ? " )" : ", " );
  }
  return os;
}

ostream& emu::base::operator<<( ostream& os, emu::base::Component& c ){
  os << "ComponentIds( ";
  set<string>::iterator i;
  set<string> ids = c.getIds();
  for ( i=ids.begin(); i!=ids.end(); ++i ){
    set<string>::iterator j = i;
    ++j;
    os << *i << ( j == ids.end() ? " )" : ", " );
  }
  return os;
}
