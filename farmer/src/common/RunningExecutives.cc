#include "emu/farmer/RunningExecutives.h"

emu::farmer::RunningExecutives& 
emu::farmer::RunningExecutives::add( const string& URI, const string& ownerDuck ){
  if ( isRunning( URI ) ) at( URI ) = ownerDuck;
  else                    insert( pair<string,string>( URI, ownerDuck ) );
  return *this;
}

emu::farmer::RunningExecutives& 
emu::farmer::RunningExecutives::remove( const string& URI ){
  if ( isRunning( URI ) ) erase( URI );
  return *this;
}

bool
emu::farmer::RunningExecutives::isActiveDuck( const string& duck ) const {
  for ( map<string,string>::const_iterator i=begin(); i!=end(); ++i )
    if ( i->second == duck ) return true;
  return false;
}

size_t
emu::farmer::RunningExecutives::getExecutiveCount( const string& duck ) const {
  size_t count( 0 ); 
  for ( map<string,string>::const_iterator i=begin(); i!=end(); ++i )
    if ( i->second == duck ) ++count;
  return count;
}

string
emu::farmer::RunningExecutives::getOwnerDuck( const string& URI ) const {
  if ( isRunning( URI ) ) return at( URI );
  return string();
}
