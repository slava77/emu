#include "emu/utils/Chamber.h"
#include "emu/utils/String.h"
#include "emu/utils/IO.h"
#include "toolbox/regex.h"
#include <iomanip>
#include <vector>
#include <cmath>

emu::utils::Chamber::Chamber()
  : endcap_ ( 0 )
  , station_( 0 )
  , ring_   ( 0 )
  , chamber_( 0 )
  , name_( "" ){
}

emu::utils::Chamber::Chamber( const int endcap, const int station, const int ring, const int chamber )
  : endcap_ ( sgn( endcap ) )
  , station_( station )
  , ring_   ( ring    )
  , chamber_( chamber )
  , name_( canonicalName() ){
}

emu::utils::Chamber::Chamber( const char endcap, const int station, const int ring, const int chamber )
  : endcap_ ( endcapNumber( endcap ) )
  , station_( station )
  , ring_   ( ring    )
  , chamber_( chamber )
  , name_( canonicalName() ){
}

emu::utils::Chamber::Chamber( const string& name )  
  : endcap_ ( 0 )
  , station_( 0 )
  , ring_   ( 0 )
  , chamber_( 0 )
  , name_( "" ){
  const string regex("[Mm][Ee]([mpMP+-])([^/:_.-]+)[/:_.-]([^/:_.-]+)[/:_.-]([^/:_.-]+)");

  vector<string> matches;
  if ( toolbox::regx_match( name, regex, matches ) && matches.size() == 5 ){
    // cout << matches << endl;
    endcap_  = endcapNumber( matches[1].c_str()[0] );
    station_ = utils::stringTo<int>( matches[2] );
    ring_    = utils::stringTo<int>( matches[3] );
    chamber_ = utils::stringTo<int>( matches[4] );
  }
  if ( isValid() ) name_ = canonicalName();
  else             name_ = name;
}

string emu::utils::Chamber::canonicalName() const {
  if ( isValid() ){
    stringstream ss;
    ss << "ME" << ( endcap_ < 0 ? "-" : "+" )
       << station_ << "/"
       << ring_  << "/"
       << setfill( '0' ) << setw( 2 ) << chamber_;
    return ss.str();
  }
  return "";
}

int emu::utils::Chamber::endcapNumber( const int endcap ) const {
  switch( endcap ){
  case 'm':
  case 'M':
  case '-':
    return -1;
  case 'p':
  case 'P':
  case '+':
    return +1;
  default:
    return 0;
  }
}

bool emu::utils::Chamber::isValid() const {  
  if ( isnan( endcap_ ) || isnan( station_ ) || isnan( ring_ ) || isnan( chamber_ ) ||
       endcap_ == 0 || 
       station_ < 1 || station_ >  4 || 
       ring_    < 1 || ring_    >  3 || 
       chamber_ < 1 || chamber_ > 36    ) return false;
  if ( station_ > 1 )
    if ( ring_ > 2 || ( ring_ == 1 && chamber_ > 18 ) ) return false;
  return true;
}
