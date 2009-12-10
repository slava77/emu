/*****************************************************************************\
* $Id: CrateParser.cc,v 1.5 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/CrateParser.h"

#include <sstream>

#include "emu/fed/Crate.h"

emu::fed::Crate *emu::fed::CrateParser::parse(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);

	unsigned int number;
	try {
		number = parser.extract<unsigned int>("CRATE_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse CRATE_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	return new Crate(number);
}



xercesc::DOMElement *emu::fed::CrateParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::Crate *crate)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a crate element
		xercesc::DOMElement *crateElement = document->createElement(X("FEDCrate"));
		
		// Set attributes
		Parser::insert(crateElement, "CRATE_NUMBER", crate->getNumber());
		
		return crateElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create FEDCrate element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}
