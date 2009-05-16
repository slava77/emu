/*****************************************************************************\
* $Id: DCCParser.cc,v 1.4 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCParser.h"

#include <sstream>

#include "emu/fed/DCC.h"

emu::fed::DCCParser::DCCParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	int slot;
	try {
		slot = extract<unsigned int>("Slot");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse slot number from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	dcc_ = new DCC(slot);

	try {
		dcc_->fifoinuse_ = extract<unsigned int>("FIFO_in_use", std::ios::hex);
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIFO-in-use from element";
		XCEPT_DECLARE_NESTED(emu::fed::exception::ParseException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		dcc_->softsw_ = extract<unsigned int>("Software_Switch", std::ios::hex);
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse Software Switch from element";
		XCEPT_DECLARE_NESTED(emu::fed::exception::ParseException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

}



























