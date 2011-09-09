#ifndef __emu_utils_SimpleTimer_h__
#define __emu_utils_SimpleTimer_h__


namespace emu { namespace utils {

class SimpleTimer
{
public:
  typedef unsigned long long timestamp_t;

  /// constructor calls reset  
  SimpleTimer();

  /// sets reference time t0 to the current time
  void reset();
  
  /// passed time in seconds since last reset or constructor
  double sec();
  
  /// passed time in milliseconds since last reset or constructor
  double msec();
  
  /// time in microseconds
  static timestamp_t timestamp();
  
private:
  /// reference time in microseconds
  timestamp_t t0_;
};

}}

#endif
