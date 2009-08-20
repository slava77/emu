/*****************************************************************************\
* $Id: DDUDBAgent.cc,v 1.5 2009/08/20 13:47:25 brett Exp $
\*****************************************************************************/

#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/FiberDBAgent.h"
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



std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::getDDUs(xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["CRATE_NUMBER"] =crateNumber.toString();
	parameters["KEY"] = key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_ddus_by_key_crate", parameters);
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

/*

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

*/

std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::buildDDUs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<DDU *> returnMe;
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed variables
		//xdata::UnsignedInteger64 id;
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
		//id.setValue(*(iRow->getField("ID"))); 
		setValue(slot,*iRow,"SLOT"); 
		setValue(rui,*iRow,"RUI");
		setValue(fmm_id,*iRow,"FMM_ID");
		setValue(gbe_prescale,*iRow,"GBE_PRESCALE"); 
		setValue(bit15,*iRow,"ENABLE_FORCE_CHECKS");
		setValue(bit16,*iRow,"FORCE_ALCT_CHECKS");
		setValue(bit17,*iRow,"FORCE_TMB_CHECKS");
		setValue(bit18,*iRow,"FORCE_CFEB_CHECKS");
		setValue(bit19,*iRow,"FORCE_NORMAL_DMB"); 
		setValue(invert_ccb_signals,*iRow,"INVERT_CCB_COMMAND_SIGNALS"); 
		
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
		
		FiberDBAgent fiberAgent(application_);
		fiberAgent.setConnectionID(connectionID_);
		xdata::Serializable *k=iRow->getField("KEY");
		xdata::UnsignedInteger64 *key=dynamic_cast<xdata::UnsignedInteger64 *>(iRow->getField("KEY"));
		if (key) {
			std::vector<emu::fed::Fiber *> fibers=fiberAgent.getFibers(*key,rui);
			newDDU->setFibers(fibers);
		} else std::cout << "key is of type " << k->type() << std::endl;
		returnMe.push_back(newDDU);
		//why delete a pointer we just put into a vector? This will have to be done elsewhere, or else the DDU should be copied into the vector
		//delete newDDU;
	}
	
	return returnMe;
}
