/*****************************************************************************\
* $Id: ChamberParser.cc,v 1.9 2009/01/30 19:14:16 paste Exp $
*
* $Log: ChamberParser.cc,v $
* Revision 1.9  2009/01/30 19:14:16  paste
* New emu::base namespace and emu::base::Supervised inheritance added.
*
* Revision 1.8  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.7  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.6  2008/08/26 13:09:02  paste
* Documentation update.
*
* Revision 1.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "ChamberParser.h"

#include <sstream>

#include "Chamber.h"


emu::fed::ChamberParser::ChamberParser(xercesc::DOMElement *pNode)
throw (emu::fed::ParseException):
Parser(pNode)
{

	chamber_ = new Chamber();
	
	std::string chamberName, pCrateName;
	
	try {
		chamberName = extract<std::string>("Name");
		pCrateName = extract<std::string>("PeripheralCrate");
		fiber_ = extract<unsigned int>("Fiber");
		killed_ = (extract<int>("Killed")) ? true : false;
	} catch (emu::fed::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse attributes from element.";
		XCEPT_RETHROW(emu::fed::ParseException, error.str(), e);
	}

	if (sscanf(chamberName.c_str(), "%*c%1d/%1d/%02d", &(chamber_->station), &(chamber_->type), &(chamber_->number)) != 3) {
		std::ostringstream error;
		error << "Unable to parse chamber station, type, and number from '" << chamberName << "'";
		XCEPT_RAISE(emu::fed::ParseException, error.str());
	}

	std::string endcapString(chamberName.substr(0,1));
	chamber_->endcap = endcapString;

	if (endcapString == "-") chamber_->plusMinus = -1;
	else chamber_->plusMinus = 1;

	if (!sscanf(pCrateName.c_str(), "VME%*c%*d_%d", &(chamber_->peripheralCrateVMECrate_))) {
		std::ostringstream error;
		error << "Unable to parse chamber peripheral crate from '" << pCrateName << "'";
		XCEPT_RAISE(emu::fed::ParseException, error.str());
	}
	
}
