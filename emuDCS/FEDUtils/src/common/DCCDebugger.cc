/*****************************************************************************\
* $Id: DCCDebugger.cc,v 1.8 2010/08/13 02:53:42 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCDebugger.h"

#include "emu/fed/DCC.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>


const std::multimap<std::string, std::string> emu::fed::DCCDebugger::FMMStatus(const uint8_t stat)
{
	// Only read the low 5 bits
	std::multimap<std::string, std::string> returnMe;
	if (0xf & stat == 0xc) returnMe.insert(std::make_pair("error", "Error")); // used to be 3
	else if (0x4 & stat) returnMe.insert(std::make_pair("caution", "Busy")); // used to be 1
	else if (0x8 & stat) returnMe.insert(std::make_pair("ok", "Ready")); // used to be 2
	else if (0x1 & stat) returnMe.insert(std::make_pair("warning", "Warning")); // used to be 4
	else if (0x2 & stat) returnMe.insert(std::make_pair("error", "Out-of-Sync")); // used to be 8
	else returnMe.insert(std::make_pair("undefined", "Undefined"));
	if (stat & 0x10) returnMe.insert(std::make_pair("none", "Custom Status Forced"));

	return returnMe;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::FMM(const uint16_t stat)
{
	// Read all 15 bits
	const std::string names[3] = {"DCC", "SLink 0", "Slink 1"};
	std::multimap<std::string, std::string> returnMe;

	for (unsigned int i = 0; i < 3; ++i) {
		const uint16_t myStat = (stat >> (i * 5));
		const std::string name = names[i];
		if (0xf & myStat == 0xc) returnMe.insert(std::make_pair("error", name + " Error")); // used to be 3
		else if (0x4 & myStat) returnMe.insert(std::make_pair("caution", name + " Busy")); // used to be 1
		else if (0x8 & myStat) returnMe.insert(std::make_pair("ok", name + " Ready")); // used to be 2
		else if (0x1 & myStat) returnMe.insert(std::make_pair("warning", name + " Warning")); // used to be 4
		else if (0x2 & myStat) returnMe.insert(std::make_pair("error", name + " Out-of-Sync")); // used to be 8
		else returnMe.insert(std::make_pair("undefined", name + " Undefined"));
		if (myStat & 0x10) returnMe.insert(std::make_pair("none", name + " Custom Status Forced"));
	}

	return returnMe;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::SLinkStatus(const uint8_t stat)
{
	std::multimap<std::string, std::string> returnValues;
	char names[2] = {'A', 'B'};
	for (int iLink = 0; iLink < 2; iLink++) {
		if (stat & (1 << (iLink*2 + 1))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << names[iLink] << " active";
			returnValues.insert(std::make_pair("green", linkStat.str()));
		} else {
			std::stringstream linkStat;
			linkStat << "S-Link " << names[iLink] << " inactive";
			returnValues.insert(std::make_pair("red", linkStat.str()));
		}

		if (!(stat & (1 << (iLink*2)))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << names[iLink] << " backpressure";
			returnValues.insert(std::make_pair("orange", linkStat.str()));
		}
	}

	return returnValues;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::FIFOStatus(const uint16_t stat) {
	std::multimap<std::string, std::string> returnValues;
	for (unsigned int iBit = 0; iBit < 5; ++iBit) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << (iBit*2 + 1) << "/" << (iBit*2 + 2) << " half full";
			returnValues.insert(std::make_pair("orange", bitStat.str()));
		}
	}
	for (unsigned int iBit = 5; iBit < 10; ++iBit) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << ((iBit - 5)*2 + 1) << "/" << ((iBit - 5)*2 + 2) << " almost full";
			returnValues.insert(std::make_pair("red", bitStat.str()));
		}
	}

	return returnValues;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::StatusHigh(const uint16_t stat)
{
	std::multimap<std::string, std::string> returnValues;

	const std::multimap<std::string, std::string> fmmValues = FMMStatus((stat >> 12) & 0xf);
	returnValues.insert(fmmValues.begin(), fmmValues.end());

	const std::multimap<std::string, std::string> sLinkValues = SLinkStatus(stat & 0xf);
	returnValues.insert(sLinkValues.begin(), sLinkValues.end());

	const std::multimap<std::string, std::string> fifoValues = FIFOStatus((stat >> 4) & 0xff);
	returnValues.insert(fifoValues.begin(), fifoValues.end());

	return returnValues;
}



const std::pair<std::string, std::string> emu::fed::DCCDebugger::decodeFIFOStatus(const uint8_t stat, const size_t iFIFO)
{
	if (iFIFO < 0 || iFIFO > 9) return std::make_pair("undefined", "undefined");

	// Each bit corresponds to two FIFOs.
	// We have to make up for the fact that iFIFO 6 correspons to an SLink.
	unsigned int fifoBit = iFIFO/2;

	// Bits 0, 1, and 2 correspond to a 1/2-full signal from FIFOs 0-1, 2-3, and 4-5 respectively.  Bit low is bad.
	// Bits 3, 4, 5, 6, and 7 correspond to a full signal from FIFOs 0-1. 2-3. 4-5. 6-7, and 8-9 respectively.  Bit low is bad.
	// Bit low is true.
	std::string status = "ok";
	std::string message = "ok";
	if (!(stat & (1 << (fifoBit + 5)))) {
		status = "error";
		message = "full";
	} else if (!(stat & (1 << fifoBit))) {
		status = "warning";
		message = "1/2 full";
	}

	return std::make_pair(status, message);
}



const std::pair<std::string, std::string> emu::fed::DCCDebugger::decodeSLinkStatus(const uint8_t stat, const size_t iLink)
{
	if (iLink < 0 || iLink > 1) return std::make_pair("undefined", "undefined");

	// Bits 0 and 2 correspond to backpressure warnings for SLinks 1 and 2 respectively.  Bit low is bad.
	// Bits 1 and 3 correspond to inactive warnings for SLinks 1 and 2 respectively.  Bit low is bad.
	std::string status = "ok";
	std::string message = "ok";
	if (!(stat & (1 << (iLink * 2 + 1)))) {
		status = "undefined";
		message = "inactive";
	} else if (!(stat & (1 << (iLink * 2 )))) {
		status = "error";
		message = "backpressure";
	}

	return std::make_pair(status, message);
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::DebugFIFOs(const DCC *dcc, const uint16_t stat, const std::string &className)
{

	std::multimap<std::string, std::string> returnMe;

	for (unsigned int iBit = 0; iBit < 12; ++iBit) {
		if (stat & (1 << iBit)) {
			try {
				std::ostringstream name;
				name << "RUI in slot " << dcc->getDDUSlotFromFIFO(iBit);
				returnMe.insert(std::make_pair(className, name.str()));
				continue;
			} catch (...) {}
			try {
				std::ostringstream name;
				name << "SLink " << dcc->getSLinkFromFIFO(iBit);
				returnMe.insert(std::make_pair(className, name.str()));
				continue;
			} catch (...) {}
			std::ostringstream name;
			name << "Unknown FIFO " << iBit;
			returnMe.insert(std::make_pair(className, name.str()));
		}
	}

	return returnMe;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::SoftwareSwitch(const uint16_t stat)
{

	std::multimap<std::string, std::string> returnMe;

	if (stat & 0x7) {
		returnMe.insert(std::make_pair("none", "DCM on control FPGA reset"));
	} else if (stat & 0x1) {
		returnMe.insert(std::make_pair("none", "Fake L1A has been sent"));
	} else if (stat & 0x6) {
		returnMe.insert(std::make_pair("orange", "Generating Fake L1As at 1.6 ms intervals"));
	}
	if (stat & 0x18) {
		returnMe.insert(std::make_pair("orange", "Serial loopback on custom backplane RocketIO engaged"));
	}/* else if (stat & 0x8) {
		returnMe.insert(std::make_pair("none", "Bit 3 set"));
	} else if (stat & 0x10) {
		returnMe.insert(std::make_pair("none", "Bit 4 set"));
	} else {
		returnMe.insert(std::make_pair("none", "???"));
	}*/
	// bit 13 (ignore slink "0")?
	// bit 14 (ignore slink "1")?
	// 12 && ~15 (disable TTC ready)?
	std::ostringstream xilinxRevision;
	xilinxRevision << "Xilinx revision " << ((stat >> 5) & 0x7);
	returnMe.insert(std::make_pair("none", xilinxRevision.str()));

	return returnMe;
}



const std::multimap<std::string, std::string> emu::fed::DCCDebugger::TTCCommand(const uint16_t stat)
{

	std::multimap<std::string, std::string> returnMe;

	if (stat & 0x1000 && !(stat & 0x8000)) {
		returnMe.insert(std::make_pair("orange", "Ignoring TTC signals"));
	}
	if (stat & 0x200 && !(stat & 0x1)) {
		returnMe.insert(std::make_pair("orange", "Software Switch active"));
	}
	if (stat & 0x2000 && !(stat & 0x4000)) {
		returnMe.insert(std::make_pair("none", "Ignore SLink full"));
	} else if (stat & 0x4000 && !(stat & 0x2000)) {
		returnMe.insert(std::make_pair("none", "Ignore all SLink errors"));
	}
	if (stat & 0x10) {
		returnMe.insert(std::make_pair("none", "Bit 4 active"));
	}
	if (stat & 0x20) {
		returnMe.insert(std::make_pair("none", "Bit 5 active"));
	}

	return returnMe;
}
