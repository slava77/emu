/*****************************************************************************\
* $Id: FIFOParser.cc,v 1.2 2009/05/29 11:25:09 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFOParser.h"

#include <sstream>

#include "emu/fed/FIFO.h"


emu::fed::FIFOParser::FIFOParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{

	try {
		number_ = extract<unsigned int>("FIFO_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIFO_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool used;
	try {
		used = (extract<int>("USED")) ? true : false;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse USED from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	unsigned int rui = 0;
	try {
		rui = extract<unsigned int>("RUI");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse RUI from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	fifo_ = new FIFO(rui, used);

}
