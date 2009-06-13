/*****************************************************************************\
* $Id: FIFOParser.cc,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFOParser.h"

#include <sstream>

#include "emu/fed/FIFO.h"


emu::fed::FIFOParser::FIFOParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	unsigned int number = 0;
	try {
		number = extract<unsigned int>("FIFO_NUMBER");
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
	
	fifo_ = new FIFO(number, rui, used);

}



emu::fed::FIFOParser::~FIFOParser()
{
	//delete fifo_;
}
