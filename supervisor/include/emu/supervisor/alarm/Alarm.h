#ifndef _emu_supervisor_alarm_Alarm_h_
#define _emu_supervisor_alarm_Alarm_h_

#include "sentinel/exception/Exception.h"

namespace emu { namespace supervisor { namespace alarm { 

  /// Generic alarm emitted by emu::supervisor
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

  /// Alarm emitted by emu::supervisor when local DAQ is not accessible
  class NoLocalDAQ: public sentinel::exception::Exception 
  {
  public: 
    NoLocalDAQ( std::string name, std::string message, std::string module, int line, std::string function ): 
      sentinel::exception::Exception(name, message, module, line, function)
    {}

    NoLocalDAQ( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception& e ): 
      sentinel::exception::Exception(name, message, module, line, function, e)
    {} 
  };

}}}

#endif
