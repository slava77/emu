/*****************************************************************************\
* $Id: FiberParser.cc,v 1.5 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#include "emu/fed/FiberParser.h"

#include <sstream>

#include "emu/fed/Fiber.h"


emu::fed::FiberParser::FiberParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	unsigned int fiberNumber = 0;
	try {
		fiberNumber = extract<unsigned int>("FIBER_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIBER_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool killed;
	try {
		killed = extract<bool>("KILLED");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse KILLED from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	// This is optional
	std::string chamberName = "+0/0/00";
	try {
		chamberName = extract<std::string>("CHAMBER");
	} catch (emu::fed::exception::ParseException &e) {
		// already set to the default.
	}
	
	std::string endcap = "?";
	unsigned int station = 0;
	unsigned int ring = 0;
	unsigned int number = 0;
	
	// Check normal station name first
	if (sscanf(chamberName.c_str(), "%*c%1u/%1u/%02u", &station, &ring, &number) == 3) {
		endcap = chamberName.substr(0,1);
		// Else it's probably an SP, so check that
	} else if (sscanf(chamberName.c_str(), "SP%02u", &number) == 1) {
		endcap = (number <= 6) ? "+" : "-";
	}
	
	// Set names now.
	fiber_ = new Fiber(fiberNumber, endcap, station, ring, number, killed);
	
}



emu::fed::FiberParser::~FiberParser()
{
	//delete fiber_;
}
