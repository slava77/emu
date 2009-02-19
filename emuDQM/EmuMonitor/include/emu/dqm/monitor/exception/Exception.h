#ifndef _emu_dqm_monitor_exception_Exception_h_
#define _emu_dqm_monitor_exception_Exception_h_

#include "xcept/Exception.h"

/**
 * Exceptions thrown by the  package in the
 * namespace emu::dqm::monitor::exception
 */

namespace emu { namespace dqm { 

  /**
   * Generic exception raised by the emu::daq::rui package.
   */
  XCEPT_DEFINE_EXCEPTION(monitor, Exception)

  /**
   * I2O exception raised by the emu::daq::rui package.
   */
  XCEPT_DEFINE_EXCEPTION(monitor, I2O)

}}

#endif

