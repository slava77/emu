/*****************************************************************************\
* $Id: DDUParser.cc,v 1.1 2009/03/05 16:07:52 paste Exp $
*
* $Log: DDUParser.cc,v $
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.11  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.10  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.9  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.8  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.7  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "emu/fed/DDUParser.h"

#include <sstream>

#include "emu/fed/DDU.h"

emu::fed::DDUParser::DDUParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	unsigned int slot;
	try {
		slot = extract<unsigned int>("Slot");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse slot number from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	ddu_ = new DDU(slot);

	try {
		ddu_->gbe_prescale_ = extract<int>("GbE_prescale", std::ios::hex);
		ddu_->killfiber_ = (extract<int>("Options", std::ios::hex)) << 15;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse GbE prescale or options from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
}


