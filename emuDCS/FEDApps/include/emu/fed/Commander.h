/*****************************************************************************\
* $Id: Commander.h,v 1.12 2010/08/13 03:00:07 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_COMMANDER_H__
#define __EMU_FED_COMMANDER_H__

#include "emu/fed/Configurable.h"
#include "emu/fed/Exception.h"
#include "emu/base/FactFinder.h"

namespace emu {
	namespace fed {

		class Crate;

		/** @class Commander An XDAq application for diagnosing and controlling the CSC FED crates at an expert level.
		*
		*	@author Phillip Killewald
		**/
		class Commander: public virtual emu::fed::Configurable/*, public emu::base::FactFinder*/
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

			/** Reads multiple registers and returns data in JSON **/
			void webReadDDURegisters(xgi::Input *in, xgi::Output *out);

			/** Takes JSON data and displays it nicely **/
			void webDisplayRegisters(xgi::Input *in, xgi::Output *out);

			/** Reads multiple registers and returns data in JSON **/
			void webReadDCCRegisters(xgi::Input *in, xgi::Output *out);

			/** GUI for managing DDU firmware **/
			void webDDUFirmwareManager(xgi::Input *in, xgi::Output *out);
			
			/** Reset the entire crate (which loads the firmware into the DDU) **/
			void webDDUReset(xgi::Input *in, xgi::Output *out);

			/** GUI for managing DCC firmware **/
			void webDCCFirmwareManager(xgi::Input *in, xgi::Output *out);

			/** Backend for loading of firmware to boards **/
			void webFirmwareLoader(xgi::Input *in, xgi::Output *out);

			/** Backend for uploading of firmware to database **/
			void webFirmwareUploader(xgi::Input *in, xgi::Output *out);

			/** Backend for checking the status of a firmware load **/
			void webFirmwareCheck(xgi::Input *in, xgi::Output *out);

			/** Show a page for editing a register **/
			void webEditDDURegister(xgi::Input *in, xgi::Output *out);

			/** Backend for writing a register to a board **/
			void webWriteRegister(xgi::Input *in, xgi::Output *out);

			/** Returns common DDU and DCC status information **/
			void webGetStatus(xgi::Input *in, xgi::Output *out);

			/** Serializes the appropriate variables to send to whatever application requests them. **/
			xoap::MessageReference onGetParameters(xoap::MessageReference message);

			/** Returns a requested fact **/
			virtual emu::base::Fact findFact(const emu::base::Component& component, const std::string& factType);

			/** Returns all facts for dispatch, either by schedule or by explicit call to sendFacts() **/
			virtual emu::base::FactCollection findFacts();

		private:

			/** Firmware error for asynchronus loading **/
			xcept::Exception *firmwareException_;

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

			/** Return a string of cgicc::div elements from a given debugger map
			*
			*	@param debug is a map of string, string values with the index being a description of the debug and the value being the class of the debug.
			*	@sa emu::fed::DDUDebugger
			**/
			std::string printDebug(const std::map<std::string, std::string> &debug);
			
			/** Return a string of cgicc::div elements from a given debugger map
			 * 
			 *	@param debug is a multimap of string, string values with the index being the class of the debug and the value being the description of the debug.
			 *	@sa emu::fed::DDUDebugger
			 **/
			std::string printDebug(const std::multimap<std::string, std::string> &debug);

			/** Return a cgicc::div elements from a given debugger pair
			*
			*	@param debug is a pair of string, string values with the index being a description of the debug and the value being the class of the debug.
			*	@sa emu::fed::DDUDebugger
			**/
			std::string printDebug(const std::pair<std::string, std::string> &debug);
			
			/** Return a cgicc::div elements from a given debugger pair
			 * 
			 *	@param debug is a pair of string, string values with the index being the class of the debug and the value being a description of the debug.
			 *	@sa emu::fed::DDUDebugger
			 **/
			std::string printDebugReverse(const std::pair<std::string, std::string> &debug);

			/** Format a big number into nice groups of 4 hex characters **/
			std::string formatBigNum(const std::vector<uint16_t> &bigNum);

			/** Format a vector of debugging information using cgicc::divs **/
			std::string formatBigDebug(const std::vector<std::string> &debug);

			/** Make a nice box for displaying and editing a register **/
			std::string makeRegisterBox(const unsigned int &reg, const std::string &identifier, const std::string &id, const std::string &initialValue, const unsigned int &base = 10);

		};
	}
}

#endif
