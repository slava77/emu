/*****************************************************************************\
* $Id: VMEControllerParser.cc,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEControllerParser.h"

#include <sstream>

#include "emu/fed/VMEController.h"


emu::fed::VMEController *emu::fed::VMEControllerParser::parse(xercesc::DOMElement* pNode, const bool &fake)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);
	
	int Link, Device;
	try {
		Device = parser.extract<int>("CAEN_DEVICE");
		Link = parser.extract<int>("CAEN_LINK");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse CAEN_DEVICE or CAEN_LINK from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		return new VMEController(Device, Link, fake);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Unable to instantiate VMEController object";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
}



xercesc::DOMElement *emu::fed::VMEControllerParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::VMEController *controller)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a controller element
		xercesc::DOMElement *controllerElement = document->createElement(X("VMEController"));
		
		// Set attributes
		Parser::insert(controllerElement, "CAEN_DEVICE", controller->getDevice());
		Parser::insert(controllerElement, "CAEN_LINK", controller->getLink());
		
		return controllerElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create VMEController element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}
