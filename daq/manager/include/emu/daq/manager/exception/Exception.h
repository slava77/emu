#ifndef _emu_daq_manager_exception_Exception_h_
#define _emu_daq_manager_exception_Exception_h_

#include "xcept/Exception.h"

/**
 * Exceptions thrown by the  package in the
 * namespace emu::daq::manager::exception
 */

namespace emu { namespace daq {
    
  /**
   * Generic exception raised by the emu::daq::manager package.
   */
//   XCEPT_DEFINE_EXCEPTION(manager, Exception)

// TODO: revert to definition by macro

  namespace manager{
    namespace exception{
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
    }
  }

}}

#endif
