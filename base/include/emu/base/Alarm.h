#ifndef _emu_base_Alarm_h_
#define _emu_base_Alarm_h_

#include <string>

#include "xdaq/Application.h"
#include "log4cplus/logger.h"
#include "xcept/tools.h"
#include "xdata/InfoSpace.h"
#include "sentinel/utils/Alarm.h"

/// Macro to raise alarm to Sentinel.
///
/// @param TYPE alarm type; must be an class derived from xcept::Exception
/// @param NAME name of alarm
/// @param SEVERITY warning | error | fatal
/// @param MESSAGE alarm message
/// @param TAG hardware id in Emu
/// @param LOGGER_POINTER pointer to a logger; if NULL, no log message will be logged in the case of failure
///
#define RAISE_ALARM( TYPE, NAME, SEVERITY, MESSAGE, TAG, LOGGER_POINTER ) \
emu::base::Alarm<TYPE>().raise( NAME, SEVERITY, MESSAGE, TAG, __FILE__, __LINE__, __FUNCTION__, this, LOGGER_POINTER )

/// Macro to raise alarm to Sentinel, including a nested exception.
///
/// @param TYPE alarm type; must be an class derived from xcept::Exception
/// @param NAME name of alarm
/// @param SEVERITY warning | error | fatal
/// @param MESSAGE alarm message
/// @param TAG hardware id in Emu
/// @param LOGGER_POINTER pointer to a logger; if NULL, no log message will be logged in the case of failure
/// @param NESTED_EXCEPTION an exception to be embedded in this alarm; must be an class derived from xcept::Exception
///
#define RAISE_ALARM_NESTED( TYPE, NAME, SEVERITY, MESSAGE, TAG, LOGGER_POINTER, NESTED_EXCEPTION ) \
emu::base::Alarm<TYPE>().raiseNested( NAME, SEVERITY, MESSAGE, TAG, __FILE__, __LINE__, __FUNCTION__, this, LOGGER_POINTER, NESTED_EXCEPTION )

/// Macro to revoke an alarm identified by @param NAME
///
/// @param NAME name of alarm
/// @param LOGGER_POINTER pointer to a logger; if NULL, no log message will be logged in the case of failure
///
// No alarm type needed for revoking; use xcept::Exception as dummy template class:
#define REVOKE_ALARM( NAME, LOGGER_POINTER ) \
emu::base::Alarm<xcept::Exception>().revoke( NAME, __FILE__, __LINE__, __FUNCTION__, this, LOGGER_POINTER )

using namespace std;

namespace emu { namespace base {

  ///
  /// Template for convenience classes for raising and revoking alarms of \c alarmType. 
  ///
  template <class alarmType>
  class Alarm{
  public:
    /// Raise alarm of \c alarmType.
    ///
    /// @param name name of alarm
    /// @param severity warning | error | fatal
    /// @param message alarm message
    /// @param tag hardware id in Emu
    /// @param file source code file that raised this alarm
    /// @param line source code line that raised this alarm
    /// @param function source code method that raised this alarm
    /// @param owner pointer to the xdaq::Application that raised this alarm
    /// @param logger pointer to a logger; if NULL, no log message will be logged in the case of failure
    ///
    void raise( const string name,
		const string severity,
		const string message,
		const string tag,
		const string file,
		const int line,
		const string function,
		xdaq::Application *owner,
		Logger *logger ){
      // Get info space for alarms:
      xdata::InfoSpace* is = xdata::InfoSpace::get("urn:xdaq-sentinel:alarms");
      // Declare an exception that will be the alarm:
      alarmType ex( name, message, file, line, function );
      // Set 'tag' property:
      ex.setProperty( "tag", tag );
      // Create an xdata container for it, specifying severity and owner:
      sentinel::utils::Alarm *alarm = new sentinel::utils::Alarm(severity, ex, owner);
      try {
	// Name and emit it:
	is->fireItemAvailable ( name, alarm );
      } catch( xdata::exception::Exception e ){
	// Log failure to emit alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to emit alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : " << xcept::stdformat_exception_history(e) );
	}
      } catch( ... ){
	// Log failure to emit alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to emit alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : Unknown exception." );
	}
      }
    }

    /// Raise alarm of \c alarmType.
    ///
    /// @param name name of alarm
    /// @param severity warning | error | fatal
    /// @param message alarm message
    /// @param tag hardware id in Emu
    /// @param file source code file that raised this alarm
    /// @param line source code line that raised this alarm
    /// @param function source code method that raised this alarm
    /// @param owner pointer to the xdaq::Application that raised this alarm
    /// @param logger pointer to a logger; if NULL, no log message will be logged in the case of failure
    /// @param nestedException an exception of \c nestedExceptionType to be embedded in this alarm; must be an class derived from xcept::Exception
    ///
    template <class nestedExceptionType>
    void raiseNested( const string name, 
		      const string severity, 
		      const string message, 
		      const string tag,
		      const string file, 
		      const int line, 
		      const string function,
		      xdaq::Application *owner,
		      log4cplus::Logger *logger,
		      nestedExceptionType &nestedException ){
      // Get info space for alarms:
      xdata::InfoSpace* is = xdata::InfoSpace::get("urn:xdaq-sentinel:alarms");
      // Declare an exception that will be the alarm:
      alarmType ex( name, message, file, line, function, nestedException );
      // Set 'tag' property:
      ex.setProperty( "tag", tag );
      // Create an xdata container for it, specifying severity and owner:
      sentinel::utils::Alarm *alarm = new sentinel::utils::Alarm(severity, ex, owner);
      try {
	// Name and emit it:
	is->fireItemAvailable ( name, alarm );
      } catch( xdata::exception::Exception e ){
	// Log failure to emit alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to emit alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : " << xcept::stdformat_exception_history(e) );
	}
      } catch( ... ){
	// Log failure to emit alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to emit alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : Unknown exception." );
	}
      }
    }

    /// Revoke an alarm identified by @param name
    ///
    /// @param name name of alarm
    /// @param file source code file that raised this alarm
    /// @param line source code line that raised this alarm
    /// @param function source code method that raised this alarm
    /// @param owner pointer to the xdaq::Application that raised this alarm
    /// @param logger pointer to a logger; if NULL, no log message will be logged in the case of failure
    ///
    void revoke( const string name,
		 const string file,
		 const int line,
		 const string function,
		 xdaq::Application *owner,
		 Logger *logger ){
      // Get info space for alarms:
      xdata::InfoSpace* is = xdata::InfoSpace::get("urn:xdaq-sentinel:alarms");
      try {
	// See if this alarm has already been raised:
	sentinel::utils::Alarm *alarm = dynamic_cast<sentinel::utils::Alarm*>(is->find( name ));
	// If it has already been raised, revoke and delete it:
	if ( alarm ){
	  is->fireItemRevoked( name, owner );
	  delete alarm;
	}
      } catch( xdata::exception::Exception e ){
	// Log failure to revoke alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to revoke alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : " << xcept::stdformat_exception_history(e) );
	}
      } catch( ... ){
	// Log failure to revoke alarm:
	if ( logger ){
	  LOG4CPLUS_WARN((*logger), "Failed to revoke alarm \"" << name << "\" from " << function << " at "
			 << file << ":" << line << " : Unknown exception." );
	}
      }
    }

  };

}} // namespace emu::base

#endif
