/*****************************************************************************\
* $Id: FiberParser.cc,v 1.3 2009/06/08 19:17:14 paste Exp $
\*****************************************************************************/
#include "emu/fed/FiberParser.h"

#include <sstream>
#include <boost/regex.hpp>

#include "emu/fed/Fiber.h"


emu::fed::FiberParser::FiberParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{

	try {
		number_ = extract<unsigned int>("FIBER_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIBER_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool killed;
	try {
		killed = (extract<int>("KILLED")) ? true : false;
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
	boost::regex chamberRegex("([+\\-])(\\d)/(\\d)/(\\d{2})");
	boost::smatch chamberMatch;
	if (boost::regex_match(chamberName, chamberMatch, chamberRegex)) {
		// Parse the text as numbers
		endcap = chamberMatch[1];
		std::istringstream parseMe(chamberMatch[2]);
		parseMe >> station;
		parseMe.str(chamberMatch[3]);
		parseMe >> ring;
		parseMe.str(chamberMatch[4]);
		parseMe >> number;
	} else {
		// Now check SPs
		boost::regex spRegex("SP[+\\-]?(\\d{2})");
		boost::smatch spMatch;
		if (boost::regex_match(chamberName, spMatch, spRegex)) {
			std::istringstream parseMe(spMatch[1]);
			parseMe >> number;
			endcap = (number <= 6) ? "+" : "-";
		}
	}
	
	// Set names now.
	fiber_ = new Fiber(endcap, station, ring, number, killed);

}
