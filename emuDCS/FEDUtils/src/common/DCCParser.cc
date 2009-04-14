/*****************************************************************************\
* $Id: DCCParser.cc,v 1.3 2009/04/14 22:59:13 paste Exp $
*
* $Log: DCCParser.cc,v $
* Revision 1.3  2009/04/14 22:59:13  paste
* Version bump.
* Added proper reporting of hardware with "tag" exception property.
*
* Revision 1.2  2009/03/05 18:23:07  paste
* * Added parsing for new attribute in DCC tag:  Software_Switch.  This is a required attribute, which means an update in XML file format is required.
*
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.7  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.6  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
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



























