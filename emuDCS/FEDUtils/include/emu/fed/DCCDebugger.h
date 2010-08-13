/*****************************************************************************\
* $Id: DCCDebugger.h,v 1.6 2010/08/13 02:53:42 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDEBUGGER_H__
#define __EMU_FED_DCCDEBUGGER_H__

#include <string>
#include <map>

/** A namespace that reads particular variables from the DCC and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*
*	@author Phillip Killewald
*	@author Jianhui Gu
**/

namespace emu {
	namespace fed {
		
		class DCC;

		namespace DCCDebugger
		{

			/** Reads from method DCC::readFMMStatus()
			 *
			 * @note Reads only the low 5 bits: those corresponding the the state of the DCC itself.
			 * Therefore it can read from both readFMMStatus() and readFMM(), with the same results.
			 **/
			const std::multimap<std::string, std::string> FMMStatus(const uint8_t stat);

			/** Reads from method DCC::readFMM()
			 *
			 * @note Decodes the full 15 bits.
			 **/
			const std::multimap<std::string, std::string> FMM(const uint16_t stat);

			/** Reads from method DCC::readSLinkStatus() **/
			const std::multimap<std::string, std::string> SLinkStatus(const uint8_t stat);

			/** Reads from method DCC::readFIFOStatus() **/
			const std::multimap<std::string, std::string> FIFOStatus(const uint16_t stat);

			/** Combines the reads from the above into one reading from DCC::readStatusHigh() **/
			const std::multimap<std::string, std::string> StatusHigh(const uint16_t stat);

			/** @returns a decoded status for a given FIFO from readFMMStatus() **/
			const std::pair<std::string, std::string> decodeFIFOStatus(const uint8_t stat, const size_t iFIFO);

			/** @returns a decoded status for a given SLink from the StatusHigh register **/
			const std::pair<std::string, std::string> decodeSLinkStatus(const uint8_t stat, const size_t iLink);

			/** Parses out from a standard-orded set of FIFO bits the slots/SLinks associated with them and attaches the supplied class name to them.  Useful with FIFOInUse register. **/
			const std::multimap<std::string, std::string> DebugFIFOs(const DCC *dcc, const uint16_t stat, const std::string &className);

			/** Reads from method DCC::readSoftwareSwitch() **/
			const std::multimap<std::string, std::string> SoftwareSwitch(const uint16_t stat);

			/** Reads from method DCC::readTTCCommand() **/
			const std::multimap<std::string, std::string> TTCCommand(const uint16_t stat);
		}

	}
}
#endif
