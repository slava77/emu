/*****************************************************************************\
* $Id: Monitor.h,v 1.1 2009/03/05 16:18:24 paste Exp $
*
* $Log: Monitor.h,v $
* Revision 1.1  2009/03/05 16:18:24  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
* * Added ForPageOne functionality to the Manager
*
\*****************************************************************************/
#ifndef __EMU_FED_MONITOR_H__
#define __EMU_FED_MONITOR_H__

#include <string>
#include <vector>

#include "Application.h"
#include "emu/fed/Exception.h"



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
			
			/** Default web page that displays simple crate statuses.
			*
			*	@param *in is a pointer to a standard xgi input object (for passing
			*	things like POST and GET variables to the function.)
			*	@param *out is the xgi output (basically, a stream that outputs to the
			*	browser window.)
			*
			*	@note The *in and *out parameters are common to all xgi-bound functions, and will herein not be
			*	included in the documentation.
			**/
			void webDefault(xgi::Input *in, xgi::Output *out);

			/** Returns whatever information is being asked of it in an AJAX form. **/
			void getAJAX(xgi::Input *in, xgi::Output *out);
			
			/** Configures the software using the XML configuration file. **/
			void configure()
			throw(emu::fed::exception::SoftwareException);
			
		private:
			/** Draws a basic crate.
			*
			*	@param myCrate is a pointer to the crate you want to draw.
			*
			*	@returns a string containing an HTML table representing the crate.
			**/
			std::string drawCrate(Crate *myCrate);
			
			/// The ever-useful crate vector.
			std::vector<Crate *> crateVector_;
			
			/// The XML configuration file name.
			xdata::String xmlFile_;
			
		};
		
	}
}

#endif
