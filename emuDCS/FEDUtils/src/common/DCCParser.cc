/*****************************************************************************\
* $Id: DCCParser.cc,v 1.8 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCParser.h"

#include <sstream>

#include "emu/fed/DCC.h"

emu::fed::DCC *emu::fed::DCCParser::parse(xercesc::DOMElement *pNode, const bool &fake)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);
	
	unsigned int slot;
	try {
		slot = parser.extract<unsigned int>("SLOT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse slot number from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	DCC *dcc_ = new DCC(slot, fake);

	try {
		dcc_->setFMMID(parser.extract<uint16_t>("FMM_ID"));
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FMM_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->setSLinkID(1, parser.extract<uint16_t>("SLINK1_ID"));
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLINK1_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->setSLinkID(2, parser.extract<uint16_t>("SLINK2_ID"));
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLINK2_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	// Switches
	uint16_t softsw = 0;

	try {
		if (parser.extract<bool>("ENABLE_SW_SWITCH")) softsw |= 0x200;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse ENABLE_SW_SWITCH from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		if (parser.extract<bool>("TTCRX_NOT_READY")) softsw |= 0x1000;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse TTCRX_NOT_READY from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		if (parser.extract<bool>("SW_BIT4")) softsw |= 0x10;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SW_BIT4 from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		if (parser.extract<bool>("SW_BIT5")) softsw |= 0x20;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SW_BIT5 from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool ignoreBackpressure = false;
	bool ignorePresent = false;
	try {
		ignoreBackpressure = parser.extract<bool>("IGNORE_SLINK_BACKPRESSURE");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse IGNORE_SLINK_BACKPRESSURE from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		ignorePresent = parser.extract<bool>("IGNORE_SLINK_NOT_PRESENT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse IGNORE_SLINK_NOT_PRESENT from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	if (ignorePresent) softsw |= 0x4000;
	else if (ignoreBackpressure) softsw |= 0x2000;
	
	dcc_->setSoftwareSwitch(softsw);
	
	return dcc_;

}



xercesc::DOMElement *emu::fed::DCCParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::DCC *dcc)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a crate element
		xercesc::DOMElement *dccElement = document->createElement(X("DCC"));
		
		// Set attributes
		Parser::insert(dccElement, "SLOT", dcc->getSlot());
		Parser::insert(dccElement, "FMM_ID", dcc->getFMMID());
		Parser::insert(dccElement, "SLINK1_ID", dcc->getSLinkID(1));
		Parser::insert(dccElement, "SLINK2_ID", dcc->getSLinkID(2));
		
		uint16_t swSwitch = dcc->getSoftwareSwitch();
		
		if (swSwitch & 0x200) Parser::insert(dccElement, "ENABLE_SW_SWITCH", 1);
		else Parser::insert(dccElement, "ENABLE_SW_SWITCH", 0);
		if (swSwitch & 0x1000) Parser::insert(dccElement, "TTCRX_NOT_READY", 1);
		else Parser::insert(dccElement, "TTCRX_NOT_READY", 0);
		if (swSwitch & 0x10) Parser::insert(dccElement, "SW_BIT4", 1);
		else Parser::insert(dccElement, "SW_BIT4", 0);
		if (swSwitch & 0x20) Parser::insert(dccElement, "SW_BIT5", 1);
		else Parser::insert(dccElement, "SW_BIT5", 0);
		if (swSwitch & 0x4000) {
			Parser::insert(dccElement, "IGNORE_SLINK_NOT_PRESENT", 1);
			Parser::insert(dccElement, "IGNORE_SLINK_BACKPRESSURE", 0);
		} else if (swSwitch & 0x2000) {
			Parser::insert(dccElement, "IGNORE_SLINK_NOT_PRESENT", 0);
			Parser::insert(dccElement, "IGNORE_SLINK_BACKPRESSURE", 1);
		} else {
			Parser::insert(dccElement, "IGNORE_SLINK_NOT_PRESENT", 0);
			Parser::insert(dccElement, "IGNORE_SLINK_BACKPRESSURE", 0);
		}
		
		return dccElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create DCC element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}

