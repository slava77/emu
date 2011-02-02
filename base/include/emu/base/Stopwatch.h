#ifndef _emu_base_Stopwatch_
#define _emu_base_Stopwatch_

#include <iostream>
#include <time.h> // time

namespace emu { namespace base {
  class Stopwatch {
  public:
    Stopwatch();

    void start();

    time_t read() const;

    time_t readAndRestart();

    void reset();

  private:
    time_t start_;
  };
}}
#endif
