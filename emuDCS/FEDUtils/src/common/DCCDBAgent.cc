/*****************************************************************************\
* $Id: DCCDBAgent.cc,v 1.5 2009/11/09 11:46:33 paste Exp $
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



std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> emu::fed::DCCDBAgent::getDCCs(xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
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
	
	// Did we match anything
	switch (result.getRowCount()) {
		case 0: 
			XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
			break;
		default:
			break;
	}
	
	try {
		return buildDCCs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> emu::fed::DCCDBAgent::getDCCs(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmm_id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["FMM_ID"] = fmm_id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_dccs_by_key_fmmid", parameters);
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
		return buildDCCs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> emu::fed::DCCDBAgent::buildDCCs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::map<xdata::UnsignedInteger64, DCC *, DBAgent::comp> returnMe;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed variables
		xdata::UnsignedInteger64 id;
		xdata::UnsignedShort slot;
		xdata::UnsignedInteger fmm_id;
		xdata::UnsignedInteger slink1_id;
		xdata::UnsignedInteger slink2_id;
		xdata::Boolean enable_sw_switch;
		xdata::Boolean ttcrx_not_ready;
		xdata::Boolean ignore_slink_backpressure;
		xdata::Boolean ignore_slink_not_present;
		xdata::Boolean sw_bit4;
		xdata::Boolean sw_bit5;
		//id.setValue(*(iRow->getField("ID"))); // only way to get a serializable to something else
		setValue(slot,*iRow,"SLOT");
		setValue(fmm_id,*iRow,"FMM_ID");
		setValue(slink1_id,*iRow,"SLINK1_ID");
		setValue(slink2_id,*iRow,"SLINK2_ID");
		setValue(enable_sw_switch,*iRow,"ENABLE_SW_SWITCH");
		setValue(ttcrx_not_ready,*iRow,"TTCRX_NOT_READY"); 
		setValue(ignore_slink_backpressure,*iRow,"IGNORE_SLINK_BACKPRESSURE"); 
		setValue(ignore_slink_not_present,*iRow,"IGNORE_SLINK_NOT_PRESENT");
		setValue(sw_bit4,*iRow,"SW_BIT4");
		setValue(sw_bit5,*iRow,"SW_BIT5");
		
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
		FIFODBAgent FIFOAgent(application_);
		FIFOAgent.setConnectionID(connectionID_);
		xdata::UnsignedInteger64 *key=dynamic_cast<xdata::UnsignedInteger64 *>(iRow->getField("KEY"));
		if (key) {
			std::vector<emu::fed::FIFO *> FIFOs=FIFOAgent.getFIFOs(*key,fmm_id);
			newDCC->setFIFOs(FIFOs);
		}
		returnMe[id] = newDCC;
	}
	
	return returnMe;
}
