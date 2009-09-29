/*****************************************************************************\
* $Id: DCCDebugger.cc,v 1.5 2009/09/29 13:57:58 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCDebugger.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>


std::pair<std::string, std::string> emu::fed::DCCDebugger::FMMStat(const uint8_t &stat)
{
	if (0xf&stat == 0x3) return std::make_pair("Error", "error");
	else if (0x1&stat) return std::make_pair("Busy", "caution");
	else if (0x2&stat) return std::make_pair("Ready", "ok");
	else if (0x4&stat) return std::make_pair("Warning", "warning");
	else if (0x8&stat) return std::make_pair("Out-of-Sync", "error");
	else return std::make_pair("Undefined", "undefined");
}



std::map<std::string, std::string> emu::fed::DCCDebugger::SLinkStat(const uint8_t &stat)
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



std::map<std::string, std::string> emu::fed::DCCDebugger::InFIFOStat(const uint8_t &stat) {
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



std::pair<std::string, std::string> emu::fed::DCCDebugger::decodeFIFOStatus(const uint8_t &stat, const size_t &iFIFO)
{
	if (iFIFO == 6 || iFIFO < 1 || iFIFO > 11) return std::make_pair("undefined", "undefined");

	// Each bit corresponds to two FIFOs.
	// We have to make up for the fact that iFIFO 6 correspons to an SLink.
	unsigned int fifoBit = (iFIFO < 6 ? (iFIFO - 1)/2 : (iFIFO - 2)/2);
	
	// Bits 0, 1, and 2 correspond to a 1/2-full signal from FIFOs 1-2, 3-4, and 5-7 respectively.
	// Bits 3, 4, 5, 6, and 7 correspond to a full signal from FIFOs 1-2. 3-4. 5-7. 8-9, and 10-11 respectively.
	// Bit low is true.
	std::string status = "ok";
	std::string message = "ok";
	if (!(stat & (1 << (fifoBit + 3)))) {
		status = "error";
		message = "full";
	} else if (fifoBit < 3 && !(stat & (1 << fifoBit))) {
		status = "warning";
		message = "1/2 full";
	}

	return std::make_pair(message, status);
}



std::pair<std::string, std::string> emu::fed::DCCDebugger::decodeSLinkStatus(const uint8_t &stat, const size_t &iLink)
{
	if (iLink < 1 || iLink > 2) return std::make_pair("undefined", "undefined");
	
	// Bits 0 and 1 correspond to backpressure warnings for SLinks 1 and 2 respectively.
	// Bits 2 and 3 correspond to inactive warnings for SLinks 1 and 2 respectively.
	std::string status = "ok";
	std::string message = "ok";
	if (!(stat & (1 << (iLink * 2 - 1)))) {
		status = "undefined";
		message = "inactive";
	} else if (!(stat & (1 << ((iLink - 1) * 2 )))) {
		status = "error";
		message = "backpressure";
	}
	
	return std::make_pair(message, status);
}



