/*****************************************************************************\
* $Id: DDUDBAgent.cc,v 1.12 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/

#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/DDU.h"
#include "xdata/TableIterator.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger.h"

emu::fed::DDUDBAgent::DDUDBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
{ 
	table_ = "EMU_FED_DDUS";
}



std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::getDDUs(xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber, const bool &fake)
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
		return buildDDUs(result, fake);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::DDU *> emu::fed::DDUDBAgent::buildDDUs(xdata::Table &table, const bool &fake)
throw (emu::fed::exception::DBException)
{
	std::vector<DDU *> returnMe;
	try {
		for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
			// Parse out all needed variables
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
			
			DDU *newDDU = new DDU(slot, fake);
			newDDU->setRUI(rui & 0x3f);
			newDDU->setFMMID(fmm_id);
			newDDU->setGbEPrescale(gbe_prescale);
			
			uint32_t killfiber = 0;
			if (bit15) killfiber |= (1 << 15);
			if (bit16) killfiber |= (1 << 16);
			if (bit17) killfiber |= (1 << 17);
			if (bit18) killfiber |= (1 << 18);
			if (bit19) killfiber |= (1 << 19);
			
			newDDU->setKillFiber(killfiber);
			
			if (invert_ccb_signals) newDDU->setRUI(0xc0);
			
			returnMe.push_back(newDDU);
		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Error initializing DDU hardware: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading DDU values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}
	
	return returnMe;
}



void emu::fed::DDUDBAgent::upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const std::vector<emu::fed::DDU *> &dduVector)
throw (emu::fed::exception::DBException)
{
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("CRATE_NUMBER", "unsigned short");
		table.addColumn("SLOT", "unsigned short");
		table.addColumn("RUI", "unsigned short");
		table.addColumn("FMM_ID", "unsigned int");
		table.addColumn("ENABLE_FORCE_CHECKS", "bool");
		table.addColumn("FORCE_ALCT_CHECKS", "bool");
		table.addColumn("FORCE_TMB_CHECKS", "bool");
		table.addColumn("FORCE_CFEB_CHECKS", "bool");
		table.addColumn("FORCE_NORMAL_DMB", "bool");
		table.addColumn("GBE_PRESCALE", "unsigned short");
		table.addColumn("INVERT_CCB_COMMAND_SIGNALS", "bool");
		
		for (std::vector<DDU *>::const_iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
			// Make a new row
			xdata::TableIterator iRow = table.append();
			
			// Set values
			xdata::UnsignedShort slot = (*iDDU)->getSlot();
			xdata::UnsignedShort rui = (*iDDU)->getRUI();
			xdata::UnsignedInteger fmmid = (*iDDU)->getFMMID();
			xdata::Boolean force = (*iDDU)->getKillFiber() & (1 << 15);
			xdata::Boolean alct = (*iDDU)->getKillFiber() & (1 << 16);
			xdata::Boolean tmb = (*iDDU)->getKillFiber() & (1 << 17);
			xdata::Boolean cfeb = (*iDDU)->getKillFiber() & (1 << 18);
			xdata::Boolean dmb = (*iDDU)->getKillFiber() & (1 << 19);
			xdata::UnsignedShort gbe_prescale = (*iDDU)->getGbEPrescale();
			xdata::Boolean invert = ((*iDDU)->getRUI() == 0xc0);
			iRow->setField("KEY", key);
			iRow->setField("CRATE_NUMBER", crateNumber);
			iRow->setField("SLOT", slot);
			iRow->setField("RUI", rui);
			iRow->setField("FMM_ID", fmmid);
			iRow->setField("ENABLE_FORCE_CHECKS", force);
			iRow->setField("FORCE_ALCT_CHECKS", alct);
			iRow->setField("FORCE_TMB_CHECKS", tmb);
			iRow->setField("FORCE_CFEB_CHECKS", cfeb);
			iRow->setField("FORCE_NORMAL_DMB", dmb);
			iRow->setField("GBE_PRESCALE", gbe_prescale);
			iRow->setField("INVERT_CCB_COMMAND_SIGNALS", invert);
			
		}
		
		// Insert
		insert("ddu", table);
		
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload DDUs to database: " + std::string(e.what()), e);
	}
}
