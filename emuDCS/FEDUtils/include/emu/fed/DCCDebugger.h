/*****************************************************************************\
* $Id: DCCDebugger.h,v 1.5 2010/02/04 10:41:48 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDEBUGGER_H__
#define __EMU_FED_DCCDEBUGGER_H__

#include <string>
#include <map>

#include "emu/fed/DCC.h"

/** A namespace that reads particular variables from the DCC and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jianhui Gu
**/

namespace emu {
	namespace fed {

		namespace DCCDebugger
		{
		
			/** Reads from method DCC::readFMMStatus() **/
			std::pair<std::string, std::string> FMMStatus(const uint16_t &stat);
		
			/** Reads from method DCC::readSLinkStatus() **/
			std::map<std::string, std::string> SLinkStatus(const uint8_t &stat);
			
			/** Reads from method DCC::readFIFOStatus() **/
			std::map<std::string, std::string> FIFOStatus(const uint8_t &stat);
			
			/** Combines the reads from the above into one reading from DCC::readStatusHigh() **/
			std::map<std::string, std::string> StatusHigh(const uint16_t &stat);
			
			/** @returns a decoded status for a given FIFO from readFMMStatus() **/
			std::pair<std::string, std::string> decodeFIFOStatus(const uint8_t &stat, const size_t &iFIFO);
			
			/** @returns a decoded status for a given SLink from the StatusHigh register **/
			std::pair<std::string, std::string> decodeSLinkStatus(const uint8_t &stat, const size_t &iLink);
			
			/** Parses out from a standard-orded set of FIFO bits the slots/SLinks associated with them and attaches the supplied class name to them.  Useful with FIFOInUse register. **/
			std::map<std::string, std::string> DebugFIFOs(DCC *dcc, const uint16_t &stat, const std::string &className);
			
			/** Reads from method DCC::readSoftwareSwitch() **/
			std::map<std::string, std::string> SoftwareSwitch(const uint16_t &stat);
			
			/** Reads from method DCC::readTTCCommand() **/
			std::map<std::string, std::string> TTCCommand(const uint16_t &stat);
		}

	}
}
#endif
