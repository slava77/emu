#ifndef _emu_supervisor_exception_Exception_h_
#define _emu_supervisor_exception_Exception_h_

#include "xcept/Exception.h"

/**
 * Exceptions thrown by the  package in the
 * namespace emu::supervisor::exception
 */

namespace emu {
    
  /**
   * Generic exception raised by the emu::supervisor package.
   */
  XCEPT_DEFINE_EXCEPTION(supervisor, Exception)

}

#endif
