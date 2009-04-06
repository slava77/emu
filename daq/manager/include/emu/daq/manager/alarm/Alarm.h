#ifndef _emu_daq_manager_alarm_Alarm_h_
#define _emu_daq_manager_alarm_Alarm_h_

#include "sentinel/exception/Exception.h"

namespace emu { namespace daq { namespace manager { namespace alarm { 

  /// Generic alarm emitted by emu::daq::manager
  class Alarm: public sentinel::exception::Exception 
  {
  public: 
    Alarm( std::string name, std::string message, std::string module, int line, std::string function ): 
      sentinel::exception::Exception(name, message, module, line, function)
    {}

    Alarm( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception& e ): 
      sentinel::exception::Exception(name, message, module, line, function, e)
    {} 
  };

  /// Alarm emitted by emu::daq::manager when it's in unsupervised mode
  class Unsupervised: public sentinel::exception::Exception 
  {
  public: 
    Unsupervised( std::string name, std::string message, std::string module, int line, std::string function ): 
      sentinel::exception::Exception(name, message, module, line, function)
    {}

    Unsupervised( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception& e ): 
      sentinel::exception::Exception(name, message, module, line, function, e)
    {} 
  };

}}}}
#endif
