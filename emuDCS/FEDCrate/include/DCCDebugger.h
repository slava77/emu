#ifndef DCCDebugger_h
#define DCCDebugger_h

using namespace std;
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "DCC.h"

/** A class that reads particular variables from the DCC and returns debugging
*	information.  Everything is returned as a vector of pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jianhui Gu
**/
class DCCDebugger
{
public:
	/** Does nothing **/
	DCCDebugger();
	/** Does nothing **/
	~DCCDebugger();

	/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
	std::map<string, string> FMMStat(short int stat);

	/** Reads from method @sa DCC::readStatusHigh(DDUFPGA) **/
	std::map<string, string> SLinkStat(short int stat);
};

#endif
