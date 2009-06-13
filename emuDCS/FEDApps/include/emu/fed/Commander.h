/*****************************************************************************\
* $Id: Commander.h,v 1.2 2009/06/13 17:59:08 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_COMMANDER_H__
#define __EMU_FED_COMMANDER_H__

#include "Application.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class Commander An XDAq application for diagnosing and controlling the CSC FED crates at an expert level.
		*
		*	@author Phillip Killewald
		**/
		class Commander: public emu::fed::Application
		{
			
		public:
			XDAQ_INSTANTIATOR();
			
			/** Default Constructor **/
			Commander(xdaq::ApplicationStub *stub);
			
			/** Default destructor **/
			~Commander();
			
			/** Default web page that displays the main Commander interface.
			*
			*	@param *in is a pointer to a standard xgi input object (for passing
			*	things like POST and GET variables to the function.)
			*	@param *out is the xgi output (basically, a stream that outputs to the
			*	browser window.)
			*
			*	@note The *in and *out parameters are common to all xgi-bound functions,
			*	and will herein not be included in the documentation.
			**/
			void webDefault(xgi::Input *in, xgi::Output *out);
			
			/** Does firmware management **/
			//void webFirmware(xgi::Input *in, xgi::Output *out);
			
			/** Backend for firmware uploading and downloading **/
			//void firmwareBackend(xgi::Input *in, xgi::Output *out);
			
			/** Configures the software using the XML configuration file. **/
			void configure()
			throw(emu::fed::exception::SoftwareException);
			
		private:
			
			/// The ever-useful crate vector.
			std::vector<Crate *> crateVector_;
			
			/// The XML configuration file name.
			xdata::String xmlFile_;
			
		};
	}
}

#endif
