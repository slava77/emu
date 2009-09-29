/*****************************************************************************\
* $Id: DCCDebugger.h,v 1.4 2009/09/29 13:57:58 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDEBUGGER_H__
#define __EMU_FED_DCCDEBUGGER_H__

#include <string>
#include <map>

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
		
			/** Reads from method DCC::readStatusHigh(DDUFPGA) **/
			std::pair<std::string, std::string> FMMStat(const uint8_t &stat);
		
			/** Reads from method DCC::readStatusHigh(DDUFPGA) **/
			std::map<std::string, std::string> SLinkStat(const uint8_t &stat);
			
			/** Reads from method DCC::readStatusHigh(DDUFPGA) **/
			std::map<std::string, std::string> InFIFOStat(const uint8_t &stat);
			
			/** @returns a decoded status for a given FIFO from the StatusHigh register **/
			std::pair<std::string, std::string> decodeFIFOStatus(const uint8_t &stat, const size_t &iFIFO);
			
			/** @returns a decoded status for a given SLink from the StatusHigh register **/
			std::pair<std::string, std::string> decodeSLinkStatus(const uint8_t &stat, const size_t &iLink);
		}

	}
}
#endif
