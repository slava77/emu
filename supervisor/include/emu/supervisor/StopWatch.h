#ifndef _emu_supervisor_StopWatch_
#define _emu_supervisor_StopWatch_

#include <iostream>
#include <time.h> // time

namespace emu { namespace supervisor {
  class StopWatch {
  public:
    StopWatch(){
      start_ = time( NULL );
    }
    time_t read(){
      return time( NULL ) - start_;
    }
    void reset(){ start_ = 0; }
  private:
    time_t start_;
  };
}}
#endif
