#ifndef __EMU_DAQ_READER_CLOCK_H__
#define __EMU_DAQ_READER_CLOCK_H__

#include <sys/times.h>

namespace emu { namespace daq { namespace reader {

  /// An auxiliary class for timing periodic debug prints.
  class Clock{
    clock_t period_;		///< period in units of cca 10 ms
    clock_t start_;		///< start time of this period
    clock_t stop_;		///< stop time of this period
    struct tms dummy_;		///< yes, that's what it is
  
  public:

    /// constructor

    ///
    /// @param period period in units of cca 10 ms
    ///
    Clock( clock_t period );
  
    /// Checks whether at least \ref period_ has passed since the last time time was up.

    ///
    /// @return \c TRUE if at least \ref period_ has passed
    ///
    bool timeIsUp();
  };

}}} // namespace emu::daq::reader
#endif
