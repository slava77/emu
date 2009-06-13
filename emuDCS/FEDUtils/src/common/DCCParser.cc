/*****************************************************************************\
* $Id: DCCParser.cc,v 1.6 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCCParser.h"

#include <sstream>

#include "emu/fed/DCC.h"

emu::fed::DCCParser::DCCParser(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException):
Parser(pNode)
{
	unsigned int slot;
	try {
		slot = extract<unsigned int>("SLOT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse slot number from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	dcc_ = new DCC(slot);

	try {
		dcc_->fmm_id_ = extract<uint16_t>("FMM_ID");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FMM_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->slink1_id_ = extract<uint16_t>("SLINK1_ID");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLINK1_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->slink2_id_ = extract<uint16_t>("SLINK2_ID");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SLINK2_ID from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	// Switches
	dcc_->softsw_ = 0;

	try {
		dcc_->softsw_ |= (extract<bool>("ENABLE_SW_SWITCH") == 0) ? 0 : 0x200;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse ENABLE_SW_SWITCH from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->softsw_ |= (extract<bool>("TTCRX_NOT_READY") == 0) ? 0 : 0x1000;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse TTCRX_NOT_READY from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->softsw_ |= (extract<bool>("SW_BIT4") == 0) ? 0 : 0x10;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SW_BIT4 from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		dcc_->softsw_ |= (extract<bool>("SW_BIT5") == 0) ? 0 : 0x20;
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse SW_BIT5 from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool ignoreBackpressure = false;
	bool ignorePresent = false;
	try {
		ignoreBackpressure = extract<bool>("IGNORE_SLINK_BACKPRESSURE");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse IGNORE_SLINK_BACKPRESSURE from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	try {
		ignorePresent = extract<bool>("IGNORE_SLINK_NOT_PRESENT");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse IGNORE_SLINK_NOT_PRESENT from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	if (ignorePresent) dcc_->softsw_ |= 0x4000;
	else if (ignoreBackpressure) dcc_->softsw_ |= 0x2000;

}



emu::fed::DCCParser::~DCCParser() {
	//delete dcc_;
}
