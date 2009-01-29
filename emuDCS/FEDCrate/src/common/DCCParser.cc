/*****************************************************************************\
* $Id: DCCParser.cc,v 3.7 2009/01/29 15:31:23 paste Exp $
*
* $Log: DCCParser.cc,v $
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
#include "DCCParser.h"

#include <sstream>

#include "DCC.h"

emu::fed::DCCParser::DCCParser(xercesc::DOMElement *pNode)
throw (emu::fed::ParseException):
Parser(pNode)
{
	int slot;
	try {
		slot = extract<int>("Slot");
	} catch (emu::fed::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse slot number from element";
		XCEPT_RETHROW(emu::fed::ParseException, error.str(), e);
	}
	
	dcc_ = new DCC(slot);
	
	try {
		dcc_->fifoinuse_ = extract<int>("FIFO_in_use", std::ios::hex);
	} catch (emu::fed::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIFO-in-use from element";
		XCEPT_RETHROW(emu::fed::ParseException, error.str(), e);
	}

}



























