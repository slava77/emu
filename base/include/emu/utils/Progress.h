#ifndef _emu_utils_Progress_
#define _emu_utils_Progress_

#include "toolbox/BSem.h"

namespace emu {
  namespace utils {
    ///
    /// Thread-safe progress counter
    ///
    class Progress {
    public:
      Progress();		   ///< default ctor
      Progress( const int total ); ///< ctor setting the total count
      Progress& setTotal  ( const int total         ); ///< set the total count
      Progress& setCurrent( const int current       ); ///< set the current count
      Progress& increment ( const int increment = 1 ); ///< increment the current count, by one by default
      int    getTotal  () const; ///< get the total count
      int    getCurrent() const; ///< get the current count
      double getValue  () const; ///< get the ratio of current count to total count
      int    getPercent() const; ///< get the ratio of current count to total count in per cent
      int    getPermill() const; ///< get the ratio of current count to total count in per mill
    private:
      mutable toolbox::BSem bSem_; ///< binary semaphore, exempt from constness
      int current_;		   ///< the current count
      int total_;		   ///< the total count
    };
  }
}
#endif
