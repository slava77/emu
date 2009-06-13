/*****************************************************************************\
* $Id: Monitor.h,v 1.4 2009/06/13 17:59:08 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MONITOR_H__
#define __EMU_FED_MONITOR_H__

#include <string>
#include <vector>

#include "Application.h"
#include "emu/fed/Exception.h"
#include "emu/fed/JSONSpiritValue.h"



namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class Monitor An XDAq application for monitoring the status of the CSC FED crates at a non-expert level.
		*
		*	@author Phillip Killewald
		**/
		class Monitor: public emu::fed::Application
		{
			
		public:
			XDAQ_INSTANTIATOR();
			
			/** Default Constructor **/
			Monitor(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~Monitor();
			
			/** Default web page that displays simple crate statuses.
			*
			*	@param *in is a pointer to a standard xgi input object (for passing
			*	things like POST and GET variables to the function.)
			*	@param *out is the xgi output (basically, a stream that outputs to the
			*	browser window.)
			*
			*	@note The *in and *out parameters are common to all xgi-bound functions, and will herein not be included in the documentation.
			**/
			void webDefault(xgi::Input *in, xgi::Output *out);

			/** Returns DDU temperatures in an AJAX form. **/
			void webGetTemperatures(xgi::Input *in, xgi::Output *out);
			
			/** Returns DDU voltages in an AJAX form. **/
			void webGetVoltages(xgi::Input *in, xgi::Output *out);
			
			/** Returns DDU occupancies in an AJAX form. **/
			void webGetOccupancies(xgi::Input *in, xgi::Output *out);
			
			/** Returns DDU L1A counts in an AJAX form. **/
			void webGetCounts(xgi::Input *in, xgi::Output *out);
			
			/** Returns DDU fiber status in an AJAX form. **/
			void webGetFiberStatus(xgi::Input *in, xgi::Output *out);
			
			/** Returns common DDU status information **/
			void webGetDDUStatus(xgi::Input *in, xgi::Output *out);
			
			/** Returns DCC rate information in an AJAX form. **/
			void webGetDCCStatus(xgi::Input *in, xgi::Output *out);
			
			/** Diggs out a crate from the input xgi stream. **/
			Crate *parseCrate(xgi::Input *in)
			throw (emu::fed::exception::ParseException);
			
			/** Configures the software using the XML configuration file. **/
			void configure()
			throw (emu::fed::exception::ConfigurationException);
			
		private:
			
			/// The ever-useful crate vector.
			std::vector<Crate *> crateVector_;
			
			/// The XML configuration file name.
			xdata::String xmlFile_;
			
			/// The database username
			xdata::String dbUsername_;
			
			/// The database password
			xdata::String dbPassword_;
			
		};
		
	}
}

#endif
