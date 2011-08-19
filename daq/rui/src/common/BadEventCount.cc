#include "emu/daq/rui/BadEventCount.h"

bool emu::daq::rui::BadEventCount::passesPrescaling() const {
  if ( value_ >= 10000000 ) return ( ( value_ % 10000000 ) == 0 );
  if ( value_ >=  1000000 ) return ( ( value_ %  1000000 ) == 0 );
  if ( value_ >=   100000 ) return ( ( value_ %   100000 ) == 0 );
  if ( value_ >=    10000 ) return ( ( value_ %    10000 ) == 0 );
  if ( value_ >=     1000 ) return ( ( value_ %     1000 ) == 0 );
  if ( value_ >=      100 ) return ( ( value_ %      100 ) == 0 );
  if ( value_ >=       10 ) return ( ( value_ %       10 ) == 0 );
  return true;
}
