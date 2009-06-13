/*****************************************************************************\
* $Id: CrateParser.cc,v 1.4 2009/06/13 17:59:45 paste Exp $
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
		number = extract<unsigned int>("CRATE_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse CRATE_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	crate_ = new Crate(number);
}



emu::fed::CrateParser::~CrateParser()
{
	//delete crate_;
}
