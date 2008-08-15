/*****************************************************************************\
* $Id: Chamber.cc,v 1.6 2008/08/15 08:35:51 paste Exp $
*
* $Log: Chamber.cc,v $
* Revision 1.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "Chamber.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

emu::fed::Chamber::Chamber():
	endcap(""),
	station(0),
	type(0),
	number(0),
	fiberCassetteCrate_(0),
	fiberCassettePos_(0),
	fiberCassetteSocket_(""),
	peripheralCrateId_(0),
	peripheralCrateVMECrate_(0),
	peripheralCrateVMESlot_(0)
{
	// All that for nothing...
}

emu::fed::Chamber::~Chamber()
{
	// Nothing.
}

/**
@returns a human-readable chamber name, like ME+1/2/12
**/
std::string emu::fed::Chamber::name()
{
	std::stringstream nameStream;
	//nameStream << "ME" << endcap << station << "/" << type << "/" << number;
	nameStream << endcap << station << "/" << type << "/" << std::setw(2) << std::setfill('0') << number;
	return nameStream.str();
}

/**
@returns a human-readable peripheral crate/slot name, like +1/03 slot 2
**/
std::string emu::fed::Chamber::peripheralCrate()
{
	std::stringstream nameStream;
	nameStream << endcap << station << "/" << std::setw(2) << std::setfill('0') << peripheralCrateVMECrate_ << " slot " << peripheralCrateVMESlot_;
	return nameStream.str();
}

/**
@returns a human-readable fiber cassette name, like 1/2/b or something...
**/
std::string emu::fed::Chamber::fiberCassette()
{
	std::stringstream nameStream;
	nameStream << fiberCassetteCrate_ << "/" << fiberCassettePos_ << "/" << fiberCassetteSocket_;
	return nameStream.str();
}
