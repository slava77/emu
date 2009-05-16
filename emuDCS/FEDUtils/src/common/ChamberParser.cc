/*****************************************************************************\
* $Id: ChamberParser.cc,v 1.3 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/
#include "emu/fed/ChamberParser.h"

#include <sstream>

#include "emu/fed/Chamber.h"


emu::fed::ChamberParser::ChamberParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{

	std::string chamberName, pCrateName;

	try {
		fiber_ = extract<unsigned int>("Fiber");
		killed_ = (extract<int>("Killed")) ? true : false;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse attributes from element.";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	// These are optional
	try {
		chamberName = extract<std::string>("Name");
	} catch (emu::fed::exception::ParseException &e) {
		chamberName = "+0/0/00";
	}
	try {
		pCrateName = extract<std::string>("PeripheralCrate");
	} catch (emu::fed::exception::ParseException &e) {
		pCrateName = "VMEp0_0";
	}

	std::string endcap = "?";
	unsigned int station = 0;
	unsigned int ring = 0;
	unsigned int number = 0;

	if (sscanf(chamberName.c_str(), "%*c%1u/%1u/%02u", &station, &ring, &number) != 3) {
		std::ostringstream error;
		error << "Unable to parse chamber station, ring, and number from '" << chamberName << "'";
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}

	endcap = chamberName.substr(0,1);
	
	unsigned int pCrateTriggerSector = 0;

	if (!sscanf(pCrateName.c_str(), "VME%*c%*u_%u", &pCrateTriggerSector)) {
		std::ostringstream error;
		error << "Unable to parse chamber peripheral crate from '" << pCrateName << "'";
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
	
	// Set names now.
	chamber_ = new Chamber(endcap, station, ring, number, pCrateTriggerSector);

}
