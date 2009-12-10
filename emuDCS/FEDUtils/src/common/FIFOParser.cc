/*****************************************************************************\
* $Id: FIFOParser.cc,v 1.4 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/FIFOParser.h"

#include <sstream>

#include "emu/fed/FIFO.h"


emu::fed::FIFO *emu::fed::FIFOParser::parse(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);
	
	unsigned int number = 0;
	try {
		number = parser.extract<unsigned int>("FIFO_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIFO_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool used;
	try {
		used = parser.extract<bool>("USED");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse USED from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	unsigned int rui = 0;
	try {
		rui = parser.extract<unsigned int>("RUI");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse RUI from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	return new FIFO(number, rui, used);

}



xercesc::DOMElement *emu::fed::FIFOParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::FIFO *fifo)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a crate element
		xercesc::DOMElement *fifoElement = document->createElement(X("FIFO"));
		
		// Set attributes
		Parser::insert(fifoElement, "FIFO_NUMBER", fifo->getNumber());
		if (fifo->isUsed()) Parser::insert(fifoElement, "USED", 1);
		else Parser::insert(fifoElement, "USED", 0);
		Parser::insert(fifoElement, "RUI", fifo->getRUI());
		
		return fifoElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create FIFO element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}
