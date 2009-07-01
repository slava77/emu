/*****************************************************************************\
* $Id: Monitor.h,v 1.5 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_MONITOR_H__
#define __EMU_FED_MONITOR_H__

#include <string>
#include <vector>

#include "emu/fed/Configurable.h"
#include "emu/fed/Exception.h"
#include "emu/fed/JSONSpiritValue.h"



namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class Monitor An XDAq application for monitoring the status of the CSC FED crates at a non-expert level.
		*
		*	@author Phillip Killewald
		**/
		class Monitor: public virtual emu::fed::Configurable
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
			
			/** Serializes the appropriate variables to send to whatever application requests them. **/
			xoap::MessageReference onGetParameters(xoap::MessageReference message);
			
		private:
			
			/** Diggs out a crate from the input xgi stream. **/
			Crate *parseCrate(xgi::Input *in)
			throw (emu::fed::exception::ParseException);
			
		};
		
	}
}

#endif
