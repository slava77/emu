/*****************************************************************************\
* $Id: Chamber.cc,v 1.3 2009/04/28 02:05:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/Chamber.h"

#include <sstream>
#include <iomanip>

emu::fed::Chamber::Chamber():
endcap(""),
station(0),
ring(0),
number(0),
//fiberCassetteCrate_(0),
//fiberCassettePos_(0),
//fiberCassetteSocket_(""),
//peripheralCrateId_(0),
peripheralCrateVMECrate_(0)
//peripheralCrateVMESlot_(0)
{
	// All that for nothing...
}



std::string emu::fed::Chamber::name()
{
	std::stringstream nameStream;
	//nameStream << "ME" << endcap << station << "/" << type << "/" << number;
	nameStream << endcap << station << "/" << ring << "/" << std::setw(2) << std::setfill('0') << number;
	return nameStream.str();
}



std::string emu::fed::Chamber::peripheralCrate()
{
	std::stringstream nameStream;
	nameStream << "VME" << (endcap == "+" ? "p" : "m") << station << "_" << peripheralCrateVMECrate_;
	return nameStream.str();
}


/*
std::string emu::fed::Chamber::fiberCassette()
{
	std::stringstream nameStream;
	nameStream << fiberCassetteCrate_ << "/" << fiberCassettePos_ << "/" << fiberCassetteSocket_;
	return nameStream.str();
}
*/
