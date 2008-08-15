/*****************************************************************************\
* $Id: DCCDebugger.h,v 1.5 2008/08/15 08:35:50 paste Exp $
*
* $Log: DCCDebugger.h,v $
* Revision 1.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DCCDEBUGGER_H__
#define __DCCDEBUGGER_H__

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
		
			/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
			std::map<std::string, std::string> FMMStat(short int stat);
		
			/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
			std::map<std::string, std::string> SLinkStat(short int stat);
		};

	}
}
#endif
