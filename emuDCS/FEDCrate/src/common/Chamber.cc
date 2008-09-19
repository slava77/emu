/*****************************************************************************\
* $Id: Chamber.cc,v 1.8 2008/09/19 16:53:52 paste Exp $
*
* $Log: Chamber.cc,v $
* Revision 1.8  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.7  2008/08/26 13:09:02  paste
* Documentation update.
*
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
//fiberCassetteCrate_(0),
//fiberCassettePos_(0),
//fiberCassetteSocket_(""),
//peripheralCrateId_(0),
	peripheralCrateVMECrate_(0)
//peripheralCrateVMESlot_(0)
{
	// All that for nothing...
}



emu::fed::Chamber::~Chamber()
{
	// Nothing.
}



std::string emu::fed::Chamber::name()
{
	std::stringstream nameStream;
	//nameStream << "ME" << endcap << station << "/" << type << "/" << number;
	nameStream << endcap << station << "/" << type << "/" << std::setw(2) << std::setfill('0') << number;
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
