#ifndef DCCDebugger_h
#define DCCDebugger_h

//using namespace std;
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "DCC.h"

/** A namespace that reads particular variables from the DCC and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jianhui Gu
**/
namespace DCCDebugger
{

	/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
	std::map<std::string, std::string> FMMStat(short int stat);

	/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
	std::map<std::string, std::string> SLinkStat(short int stat);
};

#endif
