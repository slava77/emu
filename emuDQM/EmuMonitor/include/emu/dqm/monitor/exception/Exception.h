#ifndef _emu_dqm_monitor_exception_Exception_h_
#define _emu_dqm_monitor_exception_Exception_h_

#include "xcept/Exception.h"
#include "xdaq/version.h"

/**
 * Exceptions thrown by the  package in the
 * namespace emu::dqm::monitor::exception
 */

namespace emu { namespace dqm { 

  /** Check if XDAQ version is less than XDAQ7 */
#if (XDAQ_VERSION_MAJOR <= 3) && (XDAQ_VERSION_MINOR < 24)

  namespace monitor
    {
      namespace exception
	{
	  /**
	   * Exception raised by the package.
	   */
	  class Exception: public xcept::Exception
	    {
            public:

	      Exception(std::string name,
			std::string message,
			std::string module,
			int line,
			std::string function)
		: xcept::Exception(name, message, module, line, function)
		{
		}

	      Exception(std::string name,
			std::string message,
			std::string module,
			int line,
			std::string function,
			xcept::Exception & e)
		: xcept::Exception(name, message, module, line, function, e)
		{
		}
	    };

	  class I2O: public xcept::Exception
	    {
            public:

	      I2O(std::string name,
			std::string message,
			std::string module,
			int line,
			std::string function)
		: xcept::Exception(name, message, module, line, function)
		{
		}

	      I2O(std::string name,
			std::string message,
			std::string module,
			int line,
			std::string function,
			xcept::Exception & e)
		: xcept::Exception(name, message, module, line, function, e)
		{
		}
	    };


	}
    }

#else

  /**
   * Generic exception raised by the emu::daq::rui package.
   */
  XCEPT_DEFINE_EXCEPTION(monitor, Exception)

    /**
     * I2O exception raised by the emu::daq::rui package.
     */
  XCEPT_DEFINE_EXCEPTION(monitor, I2O)

#endif

    }}

#endif

