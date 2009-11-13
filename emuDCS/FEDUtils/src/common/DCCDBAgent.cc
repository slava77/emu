/*****************************************************************************\
* $Id: DCCDBAgent.cc,v 1.6 2009/11/13 09:03:11 paste Exp $
\*****************************************************************************/

#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/DCC.h"
#include "emu/fed/FIFODBAgent.h"
#include "xdata/TableIterator.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedShort.h"

emu::fed::DCCDBAgent::DCCDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{ 
	table_ = "EMU_FED_DCCS";
}



std::vector<emu::fed::DCC *> emu::fed::DCCDBAgent::getDCCs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"] = crateNumber.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_dccs_by_key_crate", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	try {
		return buildDCCs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::DCC *> emu::fed::DCCDBAgent::buildDCCs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<DCC *> returnMe;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed variables
		xdata::UnsignedShort slot = getValue<xdata::UnsignedShort>(*iRow, "SLOT");
		xdata::UnsignedInteger fmm_id = getValue<xdata::UnsignedInteger>(*iRow, "FMM_ID");
		xdata::UnsignedInteger slink1_id = getValue<xdata::UnsignedInteger>(*iRow, "SLINK1_ID");
		xdata::UnsignedInteger slink2_id = getValue<xdata::UnsignedInteger>(*iRow, "SLINK2_ID");
		xdata::Boolean enable_sw_switch = getValue<xdata::Boolean>(*iRow, "ENABLE_SW_SWITCH");
		xdata::Boolean ttcrx_not_ready = getValue<xdata::Boolean>(*iRow, "TTCRX_NOT_READY");
		xdata::Boolean ignore_slink_backpressure = getValue<xdata::Boolean>(*iRow, "IGNORE_SLINK_BACKPRESSURE");
		xdata::Boolean ignore_slink_not_present = getValue<xdata::Boolean>(*iRow, "IGNORE_SLINK_NOT_PRESENT");
		xdata::Boolean sw_bit4 = getValue<xdata::Boolean>(*iRow, "SW_BIT4");
		xdata::Boolean sw_bit5 = getValue<xdata::Boolean>(*iRow, "SW_BIT5");

		DCC *newDCC = new DCC(slot);
		newDCC->fmm_id_ = fmm_id;
		newDCC->slink1_id_ = slink1_id;
		newDCC->slink2_id_ = slink2_id;
		if (enable_sw_switch) newDCC->softsw_ |= 0x200;
		if (ttcrx_not_ready) newDCC->softsw_ |= 0x1000;
		if (ignore_slink_backpressure) newDCC->softsw_ |= 0x2000;
		if (ignore_slink_not_present) newDCC->softsw_ |= 0x4000;
		if (sw_bit4) newDCC->softsw_ |= 0x10;
		if (sw_bit5) newDCC->softsw_ |= 0x20;

		returnMe.push_back(newDCC);
	}
	
	return returnMe;
}
