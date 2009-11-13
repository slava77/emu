/*****************************************************************************\
* $Id: DDUDBAgent.cc,v 1.7 2009/11/13 09:03:11 paste Exp $
\*****************************************************************************/

#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/DDU.h"
#include "xdata/TableIterator.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger.h"

emu::fed::DDUDBAgent::DDUDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{ 
	table_ = "EMU_FED_DDUS";
}



std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::getDDUs(xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["CRATE_NUMBER"] = crateNumber.toString();
	parameters["KEY"] = key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_ddus_by_key_crate", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	try {
		return buildDDUs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::buildDDUs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<DDU *> returnMe;
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed variables
		try {
			xdata::UnsignedShort slot = getValue<xdata::UnsignedShort>(*iRow, "SLOT");
			xdata::UnsignedShort rui = getValue<xdata::UnsignedShort>(*iRow, "RUI");
			xdata::UnsignedInteger fmm_id = getValue<xdata::UnsignedInteger>(*iRow, "FMM_ID");
			xdata::UnsignedShort gbe_prescale = getValue<xdata::UnsignedShort>(*iRow, "GBE_PRESCALE");
			xdata::Boolean bit15 = getValue<xdata::Boolean>(*iRow, "ENABLE_FORCE_CHECKS");
			xdata::Boolean bit16 = getValue<xdata::Boolean>(*iRow, "FORCE_ALCT_CHECKS");
			xdata::Boolean bit17 = getValue<xdata::Boolean>(*iRow, "FORCE_TMB_CHECKS");
			xdata::Boolean bit18 = getValue<xdata::Boolean>(*iRow, "FORCE_CFEB_CHECKS");
			xdata::Boolean bit19 = getValue<xdata::Boolean>(*iRow, "FORCE_NORMAL_DMB");
			xdata::Boolean invert_ccb_signals = getValue<xdata::Boolean>(*iRow, "INVERT_CCB_COMMAND_SIGNALS");
			
			DDU *newDDU = new DDU(slot);
			newDDU->rui_ = rui & 0x3f;
			newDDU->fmm_id_ = fmm_id;
			newDDU->gbe_prescale_ = gbe_prescale;
			if (bit15) newDDU->killfiber_ |= (1 << 15);
			if (bit16) newDDU->killfiber_ |= (1 << 16);
			if (bit17) newDDU->killfiber_ |= (1 << 17);
			if (bit18) newDDU->killfiber_ |= (1 << 18);
			if (bit19) newDDU->killfiber_ |= (1 << 19);
			if (invert_ccb_signals) newDDU->rui_ = 0xc0;
			
			returnMe.push_back(newDDU);
			
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading DDU parameters from database", e);
		}
	}
	
	return returnMe;
}
