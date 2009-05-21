/*****************************************************************************\
* $Id: DCCDebugger.cc,v 1.2 2009/05/21 15:30:49 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCDebugger.h"

#include <iostream>
#include <iomanip>
#include <sstream>



std::map<std::string, std::string> emu::fed::DCCDebugger::FMMStat(short int stat)
{
	std::map<std::string, std::string> returnValues;

	if (0xf&stat == 0x3) returnValues["Error"] = "error";
	else if (0x1&stat) returnValues["Busy"] = "caution";
	else if (0x2&stat) returnValues["Ready"] = "ok";
	else if (0x4&stat) returnValues["Warning"] = "warning";
	else if (0x8&stat) returnValues["Out-of-Sync"] = "error";
	else returnValues["Undefined"] = "undefined";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DCCDebugger::SLinkStat(short int stat)
{
	std::map<std::string, std::string> returnValues;
	for (int iLink = 0; iLink < 2; iLink++) {
		if (stat & (1 << (iLink*2 + 1))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " active";
			returnValues[linkStat.str()] = "green";
		} else {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " inactive";
			returnValues[linkStat.str()] = "red";
		}

		if (!(stat & (1 << (iLink*2)))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " backpressure";
			returnValues[linkStat.str()] = "orange";
		}
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DCCDebugger::InFIFOStat(short int stat) {
	std::map<std::string, std::string> returnValues;
	for (unsigned int iBit = 0; iBit < 3; iBit++) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << (iBit*2 + 1) << "/" << (iBit*2 + 2) << " half full";
			returnValues[bitStat.str()] = "orange";
		}
	}
	for (unsigned int iBit = 3; iBit < 8; iBit++) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << ((iBit - 3)*2 + 1) << "/" << ((iBit - 3)*2 + 2) << " almost full";
			returnValues[bitStat.str()] = "red";
		}
	}
	
	return returnValues;
}


