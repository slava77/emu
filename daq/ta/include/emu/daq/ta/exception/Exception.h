#ifndef _emu_daq_ta_exception_Exception_h_
#define _emu_daq_ta_exception_Exception_h_

#include "xcept/Exception.h"

/**
 * Exceptions thrown by the  package in the
 * namespace emu::daq::ta::exception
 */

namespace emu { namespace daq {
    
  /**
   * Generic exception raised by the emu::daq::ta package.
   */
  XCEPT_DEFINE_EXCEPTION(ta, Exception)

  /**
   * I2O exception raised by the emu::daq::ta package.
   */
  XCEPT_DEFINE_EXCEPTION(ta, I2O)

}}

#endif
