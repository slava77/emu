#ifndef __emu_daq_rui_LogMessageCounter_h__
#define __emu_daq_rui_LogMessageCounter_h__

#include <stdint.h>
#include <string>
#include <map>

using namespace std;

namespace emu { 
  namespace daq { 
    namespace rui {

      ///
      /// Log message counter and progressive prescaler.
      ///      
      class LogMessageCounter{
      public:

	/// Dtor.
	///
	///
	~LogMessageCounter();

	/// Deletes all counters.
	///
	///
	void reset();

	/// Find out whether the number of occurrences of \e message passes the progressive prescaling.
	///
	/// @param message The log message.
	///
	/// @return TRUE if the number of occurrences of \e message passes the progressive prescaling.
	///
	bool isToBeLogged( const string& message );

	/// Get the number of occurrences of \e message .
	///
	/// @param message The log message.
	///
	/// @return The number of occurrences of \e message .
	///
	uint64_t getCount( const string& message ) const;

      private:

	/// Increment the counter of \e message . If it doesn't have a counter yet, and the number of counters is less than \c maxMessageTypes_ , create a new counter for it.
	///
	/// @param message The log message.
	///
	/// @return The number of occurrences of \e message  or 0 if \e message cannot have a counter because the number of counters has reached \c maxMessageTypes_ .
	///
	uint64_t increment( const string& message );

	/// Find out whether or not \e count passes the progressive prescaling.
	///
	/// @param count 
	///
	/// @return TRUE if \e count passes the progressive prescaling.
	///
	bool isPassingProgressivePrescaling( uint64_t count ) const;
	static const uint64_t maxMessageTypes_ = 0x100000 - 1; ///< The max number of counters (i.e., message types).
	map<string, uint64_t*> counts_;	///< Container of counts.
      };
      
}}} // namespace emu::daq::rui

#endif
