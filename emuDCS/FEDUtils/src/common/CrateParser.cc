/*****************************************************************************\
* $Id: CrateParser.cc,v 1.2 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/CrateParser.h"

#include <sstream>

#include "emu/fed/Crate.h"

emu::fed::CrateParser::CrateParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{

	unsigned int number;
	try {
		number = extract<unsigned int>("Number");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse crate number from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	crate_ = new Crate(number);
}
