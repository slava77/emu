/*****************************************************************************\
* $Id: Commander.h,v 1.3 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_COMMANDER_H__
#define __EMU_FED_COMMANDER_H__

#include "emu/fed/Configurable.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {
		
		class Crate;
		
		/** @class Commander An XDAq application for diagnosing and controlling the CSC FED crates at an expert level.
		*
		*	@author Phillip Killewald
		**/
		class Commander: public virtual emu::fed::Configurable
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
			
			/** Returns common DDU and DCC status information **/
			void webGetStatus(xgi::Input *in, xgi::Output *out);
			
			/** Serializes the appropriate variables to send to whatever application requests them. **/
			xoap::MessageReference onGetParameters(xoap::MessageReference message);
			
		private:
			
			/** Does firmware management **/
			//void webFirmware(xgi::Input *in, xgi::Output *out);
			
			/** Backend for firmware uploading and downloading **/
			//void firmwareBackend(xgi::Input *in, xgi::Output *out);
			
			/// @class Register a class for generating various register checkboxes with ease
			class Register
			{
				public:
				Register(const std::string &myDescription = "", const std::string &myID = "", const std::string &myStyle = ""):
				description(myDescription),
				id(myID),
				style(myStyle)
				{};
				
				std::string description;
				
				std::string id;
				
				std::string style;
			};
			
			/** Print a formatted table of commands.
			*
			*	@param registers is a vector of Register objects to print.
			*	@param id is the identifier to use for the checkboxes.
			**/
			std::string printRegisterTable(const std::vector<Register> &registers, const std::string &id);
			
		};
	}
}

#endif
