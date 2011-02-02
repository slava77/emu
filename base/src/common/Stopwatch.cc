#include "emu/base/Stopwatch.h"


emu::base::Stopwatch::Stopwatch() :
  start_( 0 ){}

void
emu::base::Stopwatch::start(){
  start_ = time( NULL );
}

time_t 
emu::base::Stopwatch::read() const {
  return time( NULL ) - start_;
}

time_t
emu::base::Stopwatch::readAndRestart(){
  time_t reading = read();
  start();
  return reading;
}

void
emu::base::Stopwatch::reset(){
  start_ = 0;
}
