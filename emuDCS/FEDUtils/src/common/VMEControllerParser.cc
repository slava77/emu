/*****************************************************************************\
* $Id: VMEControllerParser.cc,v 1.2 2009/05/21 15:30:49 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEControllerParser.h"

#include <sstream>

#include "emu/fed/VMEController.h"

emu::fed::VMEControllerParser::VMEControllerParser(xercesc::DOMElement* pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	int Link, Device;
	try {
		Device = extract<int>("CAEN_DEVICE");
		Link = extract<int>("CAEN_LINK");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse CAEN_DEVICE or CAEN_LINK from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	try {
		vmeController_ = new VMEController(Device, Link);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Unable to instantiate VMEController object";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
}
