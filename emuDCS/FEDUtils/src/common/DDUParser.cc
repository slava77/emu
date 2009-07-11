/*****************************************************************************\
* $Id: DDUParser.cc,v 1.6 2009/07/11 19:38:32 paste Exp $
\*****************************************************************************/
#include "emu/fed/DDUParser.h"

#include <sstream>
#include <vector>
#include <string>

#include "emu/fed/DDU.h"

emu::fed::DDUParser::DDUParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	unsigned int slot = 0;
	try {
		slot = extract<unsigned int>("SLOT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLOT from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	ddu_ = new DDU(slot);
	
	try {
		ddu_->rui_ = extract<uint16_t>("RUI") & 0x3f;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse RUI from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		if (extract<bool>("INVERT_CCB_COMMAND_SIGNALS")) {
			ddu_->rui_ = 0xc0;
		}
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse INVERT_CCB_COMMAND_SIGNALS from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		ddu_->fmm_id_ = extract<uint16_t>("FMM_ID");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FMM_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	try {
		ddu_->gbe_prescale_ = extract<uint16_t>("GBE_PRESCALE", std::ios::hex);
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
	ddu_->killfiber_ = 0;
	
	for (unsigned int iOption = 0; iOption < optionNames.size(); iOption++) {
		std::string optionName = optionNames[iOption];
		try {
			if (extract<bool>(optionName)) ddu_->killfiber_ |= (1 << (15 + iOption));
		} catch (emu::fed::exception::ParseException &e) {
			std::ostringstream error;
			error << "Unable to parse " << optionName << " from element";
			XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
		}
	}

}



emu::fed::DDUParser::~DDUParser()
{
	//delete ddu_;
}
