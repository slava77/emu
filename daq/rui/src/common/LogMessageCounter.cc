#include "emu/daq/rui/LogMessageCounter.h"

emu::daq::rui::LogMessageCounter::~LogMessageCounter(){
  reset();
}

void emu::daq::rui::LogMessageCounter::reset(){
  for ( map<string, uint64_t*>::iterator i = counts_.begin(); i != counts_.end(); ++i ) delete i->second;
  counts_.clear();
}

uint64_t emu::daq::rui::LogMessageCounter::increment( const string& message ){
  map<string, uint64_t*>::iterator i = counts_.find( message );
  if ( i != counts_.end() ){
    ++*i->second;
    return *i->second;
  }
  else if ( maxMessageTypes_ > counts_.size() ){
    counts_.insert( pair<string, uint64_t*>( message, new uint64_t(1) ) );
    return 1;
  }
  return 0;
}

uint64_t emu::daq::rui::LogMessageCounter::getCount( const string& message ) const {
   map<string, uint64_t*>::const_iterator i = counts_.find( message );
   if ( i !=  counts_.end() ) return *i->second;
   return 0;
}

bool emu::daq::rui::LogMessageCounter::isPassingProgressivePrescaling( uint64_t count ) const {
  if ( count >= 10000000 ) return ( ( count % 10000000 ) == 0 );
  if ( count >=  1000000 ) return ( ( count %  1000000 ) == 0 );
  if ( count >=   100000 ) return ( ( count %   100000 ) == 0 );
  if ( count >=    10000 ) return ( ( count %    10000 ) == 0 );
  if ( count >=     1000 ) return ( ( count %     1000 ) == 0 );
  if ( count >=      100 ) return ( ( count %      100 ) == 0 );
  if ( count >=       10 ) return ( ( count %       10 ) == 0 );
  return true;
}

bool emu::daq::rui::LogMessageCounter::isToBeLogged( const string& message ){
  return isPassingProgressivePrescaling( increment( message ) );
}
