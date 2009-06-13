/*****************************************************************************\
* $Id: DDUDBAgent.cc,v 1.4 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/

#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/DDU.h"
#include "xdata/TableIterator.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger.h"

emu::fed::DDUDBAgent::DDUDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_DDUS";
}



std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> emu::fed::DDUDBAgent::getDDUs(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["CRATE_ID"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_ddus", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
	case 0: 
		XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
		break;
	default:
		break;
	}
	
	try {
		return buildDDUs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> emu::fed::DDUDBAgent::getDDUs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["RUI"] = rui.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_ddus_by_key_rui", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
	case 0: 
		XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
		break;
	case 1:
		break;
	default:
		XCEPT_RAISE(emu::fed::exception::DBException, "More than one matching row found");
		break;
	}

	try {
		return buildDDUs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::DDU *, emu::fed::DBAgent::comp> emu::fed::DDUDBAgent::buildDDUs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::map<xdata::UnsignedInteger64, DDU *, DBAgent::comp> returnMe;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed variables
		xdata::UnsignedInteger64 id;
		xdata::UnsignedShort slot;
		xdata::UnsignedShort rui;
		xdata::UnsignedInteger fmm_id;
		xdata::UnsignedShort gbe_prescale;
		xdata::Boolean bit15;
		xdata::Boolean bit16;
		xdata::Boolean bit17;
		xdata::Boolean bit18;
		xdata::Boolean bit19;
		xdata::Boolean invert_ccb_signals;
		try {
			id.setValue(*(iRow->getField("ID"))); // only way to get a serializable to something else
			slot.setValue(*(iRow->getField("SLOT"))); // only way to get a serializable to something else
			rui.setValue(*(iRow->getField("RUI"))); // only way to get a serializable to something else
			fmm_id.setValue(*(iRow->getField("FMM_ID"))); // only way to get a serializable to something else
			gbe_prescale.setValue(*(iRow->getField("GBE_PRESCALE"))); // only way to get a serializable to something else
			bit15.setValue(*(iRow->getField("ENABLE_FORCE_CHECKS"))); // only way to get a serializable to something else
			bit16.setValue(*(iRow->getField("FORCE_ALCT_CHECKS"))); // only way to get a serializable to something else
			bit17.setValue(*(iRow->getField("FORCE_TMB_CHECKS"))); // only way to get a serializable to something else
			bit18.setValue(*(iRow->getField("FORCE_CFEB_CHECKS"))); // only way to get a serializable to something else
			bit19.setValue(*(iRow->getField("FORCE_NORMAL_DMB"))); // only way to get a serializable to something else
			invert_ccb_signals.setValue(*(iRow->getField("INVERT_CCB_COMMAND_SIGNALS"))); // only way to get a serializable to something else
		} catch (xdata::exception::Exception &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
		}
		
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
		returnMe[id] = newDDU;
		delete newDDU;
	}
	
	return returnMe;
}
