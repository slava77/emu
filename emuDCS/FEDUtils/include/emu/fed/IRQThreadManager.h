/*****************************************************************************\
* $Id: IRQThreadManager.h,v 1.4 2009/07/11 19:38:32 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_IRQTHREADMANAGER_H__
#define __EMU_FED_IRQTHREADMANAGER_H__

#include <vector>
#include <string>
#include <pthread.h>

#include "emu/fed/Exception.h"
#include "xdaq/WebApplication.h"
#include "emu/base/Alarm.h"

// My own versions of Karoly's macros
#define MY_RAISE_ALARM( TYPE, NAME, SEVERITY, MESSAGE, TAG) \
emu::base::Alarm<TYPE>().raise( NAME, SEVERITY, MESSAGE, TAG, __FILE__, __LINE__, __FUNCTION__, application_, NULL )

#define MY_RAISE_ALARM_NESTED( TYPE, NAME, SEVERITY, MESSAGE, TAG, NESTED_EXCEPTION ) \
emu::base::Alarm<TYPE>().raiseNested( NAME, SEVERITY, MESSAGE, TAG, __FILE__, __LINE__, __FUNCTION__, application_, NULL, NESTED_EXCEPTION )

#define MY_REVOKE_ALARM( NAME ) \
emu::base::Alarm<xcept::Exception>().revoke( NAME, __FILE__, __LINE__, __FUNCTION__, application_, NULL )


namespace emu {
	namespace fed {

		class Crate;
		class IRQData;

		/** @class IRQThreadManager A class that manages communication with DDUs concerning FMM interrupt handling. **/
		class IRQThreadManager {
		
		public:
		
			/** Constructor
			*
			*	@param application The application from which this thread is run (for SOAP alarms)
			*	@param systemName The name of the system from which this thread is run (for log file naming)
			**/
			IRQThreadManager(xdaq::WebApplication *application, const unsigned int &fmmErrorThreshold = 0);
			
			/** Default destructor **/
			~IRQThreadManager();
			
			/** Attach a crate object for monitoring. **/
			void attachCrate(Crate *crate);
			
			/** Begin monitoring with an optional run number. **/
			void startThreads(const unsigned long int &runNumber = 0)
			throw (emu::fed::exception::FMMThreadException);
			
			/** Stop the threads. **/
			void endThreads()
			throw (emu::fed::exception::FMMThreadException);
			
			/** Change the system name. **/
			inline void setSystemName(const std::string &systemName) { systemName_ = systemName; }
			
			/** Change the error threshold **/
			inline void setFMMErrorThreshold(const unsigned int &threshold) { fmmErrorThreshold_ = threshold; }
		
			/** The actual thread routine **/
			static void *IRQThread(void *data);
			
			/** Get the shared data from an outside application. **/
			inline IRQData *getData() { return data_; }
		
		private:
		
			/// The crates to monitor and their associated thread identifiers **/
			std::vector< std::pair<Crate *, pthread_t> > threadVector_;
			
			/// The shared data
			IRQData *data_;
			
			/// The name of the FED system (for log files)
			std::string systemName_;
			
			/// The number of chambers in error per system before the FMMs are released
			unsigned int fmmErrorThreshold_;
			
			/// The application from where to send the SOAP messages
			xdaq::WebApplication *application_;
		
		};

	}
}

#endif
