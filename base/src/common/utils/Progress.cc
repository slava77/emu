#include "emu/utils/Progress.h"
#include <cmath>

emu::utils::Progress::Progress()
  : bSem_( toolbox::BSem::EMPTY ) // born locked
  , current_( 0. )  
  , total_( 0. ){
  bSem_.give();
}
  
emu::utils::Progress::Progress( const int total )
  : bSem_( toolbox::BSem::EMPTY ) // born locked
  , current_( 0. )  
  , total_( total ){
  bSem_.give();
}
  
emu::utils::Progress& emu::utils::Progress::setTotal( const int total ){
  bSem_.take();
  total_ = total;
  bSem_.give();
  return *this;
}

emu::utils::Progress& emu::utils::Progress::setCurrent( const int current ){
  bSem_.take();
  current_ = current;
  bSem_.give();
  return *this;
}

emu::utils::Progress& emu::utils::Progress::increment( const int increment ){
  bSem_.take();
  current_ += increment;
  bSem_.give();
  return *this;
}

int emu::utils::Progress::getCurrent() const {
  bSem_.take();
  double c = current_;
  bSem_.give();
  return c;
}

int emu::utils::Progress::getTotal() const {
  bSem_.take();
  double t = total_;
  bSem_.give();
  return t;
}

double emu::utils::Progress::getValue() const {
  bSem_.take();
  double value = ( total_ == 0 ? 0. : double( current_ ) / double( total_ ) );
  bSem_.give();
  return value;
}

int emu::utils::Progress::getPercent() const {
  bSem_.take();
  bool done = ( current_ == total_ );
  bSem_.give();
  if ( done ) return 100;
  double p = 100. * getValue();
  return ( p < 0. ? ceil( p ) : floor( p ) );
}

int emu::utils::Progress::getPermill() const {
  bSem_.take();
  bool done = ( current_ == total_ );
  bSem_.give();
  if ( done ) return 1000;
  double p = 1000. * getValue();
  return ( p < 0. ? ceil( p ) : floor( p ) );
}
