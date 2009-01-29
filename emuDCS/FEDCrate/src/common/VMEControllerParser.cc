/*****************************************************************************\
* $Id: VMEControllerParser.cc,v 3.5 2009/01/29 15:31:24 paste Exp $
*
* $Log: VMEControllerParser.cc,v $
* Revision 3.5  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.4  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.3  2008/08/19 14:51:03  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.2  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEControllerParser.h"

#include <sstream>

#include "VMEController.h"

emu::fed::VMEControllerParser::VMEControllerParser(xercesc::DOMElement* pNode)
throw (emu::fed::ParseException):
Parser(pNode)
{
	int Link, Device;
	try {
		Device = extract<int>("Device");
		Link = extract<int>("Link");
	} catch (emu::fed::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse Device or Link numbers from element";
		XCEPT_RETHROW(emu::fed::ParseException, error.str(), e);
	}

	try {
		vmeController_ = new VMEController(Device, Link);
	} catch (emu::fed::CAENException &e) {
		std::ostringstream error;
		error << "Unable to instantiate VMEController object";
		XCEPT_RETHROW(emu::fed::ParseException, error.str(), e);
	}
	
}
