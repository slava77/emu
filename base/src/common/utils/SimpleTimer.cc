// $Id: SimpleTimer.cc,v 1.1 2012/04/11 21:34:47 khotilov Exp $

#include "emu/utils/SimpleTimer.h"

#include <sys/time.h>

emu::utils::SimpleTimer::timestamp_t  emu::utils::SimpleTimer::timestamp()
{
  struct timeval now;
  gettimeofday (&now, 0);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}


emu::utils::SimpleTimer::SimpleTimer()
{
  reset();
}


void emu::utils::SimpleTimer::reset()
{
  t0_ = timestamp();
}


double emu::utils::SimpleTimer::sec()
{
  timestamp_t t1 = timestamp();
  return (t1 - t0_) / 1000000.0L;
}


double emu::utils::SimpleTimer::msec()
{
  timestamp_t t1 = timestamp();
  return (t1 - t0_) / 1000.0L;
}
