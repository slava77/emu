/*****************************************************************************\
* $Id: DDUParser.cc,v 1.7 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/DDUParser.h"

#include "emu/fed/DDU.h"

emu::fed::DDU *emu::fed::DDUParser::parse(xercesc::DOMElement *pNode, const bool &fake)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);
	
	unsigned int slot = 0;
	try {
		slot = parser.extract<unsigned int>("SLOT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLOT from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	DDU *ddu_ = new DDU(slot, fake);
	
	try {
		ddu_->setRUI(parser.extract<uint16_t>("RUI") & 0x3f);
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse RUI from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		if (parser.extract<bool>("INVERT_CCB_COMMAND_SIGNALS")) {
			ddu_->setRUI(0xc0);
		}
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse INVERT_CCB_COMMAND_SIGNALS from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		ddu_->setFMMID(parser.extract<uint16_t>("FMM_ID"));
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FMM_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	try {
		ddu_->setGbEPrescale(parser.extract<uint16_t>("GBE_PRESCALE", std::ios::hex));
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse GBE_PRESCALE from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	std::vector<std::string> optionNames;
	optionNames.push_back("ENABLE_FORCE_CHECKS");
	optionNames.push_back("FORCE_ALCT_CHECKS");
	optionNames.push_back("FORCE_TMB_CHECKS");
	optionNames.push_back("FORCE_CFEB_CHECKS");
	optionNames.push_back("FORCE_NORMAL_DMB");
	uint32_t killfiber = 0;
	
	for (unsigned int iOption = 0; iOption < optionNames.size(); iOption++) {
		std::string optionName = optionNames[iOption];
		try {
			if (parser.extract<bool>(optionName)) killfiber |= (1 << (15 + iOption));
		} catch (emu::fed::exception::ParseException &e) {
			std::ostringstream error;
			error << "Unable to parse " << optionName << " from element";
			XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
		}
	}
	
	ddu_->setKillFiber(killfiber);
	
	return ddu_;

}



xercesc::DOMElement *emu::fed::DDUParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::DDU *ddu)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a crate element
		xercesc::DOMElement *dduElement = document->createElement(X("DDU"));
		
		// Set attributes
		Parser::insert(dduElement, "SLOT", ddu->getSlot());
		Parser::insert(dduElement, "RUI", ddu->getRUI());
		if (ddu->getRUI() == 0xc0) Parser::insert(dduElement, "INVERT_CCB_COMMAND_SIGNALS", 1);
		else Parser::insert(dduElement, "INVERT_CCB_COMMAND_SIGNALS", 0);
		Parser::insert(dduElement, "FMM_ID", ddu->getFMMID());
		Parser::insert(dduElement, "GBE_PRESCALE", ddu->getGbEPrescale());
		
		std::vector<std::string> optionNames;
		optionNames.push_back("ENABLE_FORCE_CHECKS");
		optionNames.push_back("FORCE_ALCT_CHECKS");
		optionNames.push_back("FORCE_TMB_CHECKS");
		optionNames.push_back("FORCE_CFEB_CHECKS");
		optionNames.push_back("FORCE_NORMAL_DMB");
		
		uint32_t killFiber = ddu->getKillFiber();
		
		for (unsigned int iOption = 0; iOption < optionNames.size(); iOption++) {
			std::string optionName = optionNames[iOption];
			if (killFiber & (1 << (15 + iOption))) Parser::insert(dduElement, optionName, 1);
			else Parser::insert(dduElement, optionName, 0);
		}
		
		return dduElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create DDU element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}
