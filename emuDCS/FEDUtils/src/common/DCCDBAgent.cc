/*****************************************************************************\
* $Id: DCCDBAgent.cc,v 1.11 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/

#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/DCC.h"
#include "emu/fed/FIFODBAgent.h"
#include "xdata/TableIterator.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedShort.h"

emu::fed::DCCDBAgent::DCCDBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
{ 
	table_ = "EMU_FED_DCCS";
}



std::vector<emu::fed::DCC *> emu::fed::DCCDBAgent::getDCCs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const bool &fake)
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
		return buildDCCs(result, fake);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::DCC *> emu::fed::DCCDBAgent::buildDCCs(xdata::Table &table, const bool &fake)
throw (emu::fed::exception::DBException)
{
	std::vector<DCC *> returnMe;
	try {
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

			DCC *newDCC = new DCC(slot, fake);
			newDCC->setFMMID(fmm_id);
			newDCC->setSLinkID(1, slink1_id);
			newDCC->setSLinkID(2, slink2_id);
			
			uint16_t softsw = 0;
			if (enable_sw_switch) softsw |= 0x200;
			if (ttcrx_not_ready) softsw |= 0x1000;
			if (ignore_slink_not_present) softsw |= 0x4000;
			else if (ignore_slink_backpressure) softsw |= 0x2000;
			if (sw_bit4) softsw |= 0x10;
			if (sw_bit5) softsw |= 0x20;
			
			newDCC->setSoftwareSwitch(softsw);

			returnMe.push_back(newDCC);
		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Error initializing DCC hardware: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading DCC values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}
	
	return returnMe;
}



void emu::fed::DCCDBAgent::upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, const std::vector<emu::fed::DCC *> &dccVector)
throw (emu::fed::exception::DBException)
{
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("CRATE_NUMBER", "unsigned short");
		table.addColumn("SLOT", "unsigned short");
		table.addColumn("FMM_ID", "unsigned int");
		table.addColumn("SLINK1_ID", "unsigned int");
		table.addColumn("SLINK2_ID", "unsigned int");
		table.addColumn("ENABLE_SW_SWITCH", "bool");
		table.addColumn("TTCRX_NOT_READY", "bool");
		table.addColumn("IGNORE_SLINK_BACKPRESSURE", "bool");
		table.addColumn("IGNORE_SLINK_NOT_PRESENT", "bool");
		table.addColumn("SW_BIT4", "bool");
		table.addColumn("SW_BIT5", "bool");
		
		for (std::vector<DCC *>::const_iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
			// Make a new row
			xdata::TableIterator iRow = table.append();
			
			// Set values
			xdata::UnsignedShort slot = (*iDCC)->getSlot();
			xdata::UnsignedInteger fmmid = (*iDCC)->getFMMID();
			xdata::UnsignedInteger slink1 = (*iDCC)->getSLinkID(1);
			xdata::UnsignedInteger slink2 = (*iDCC)->getSLinkID(2);
			xdata::Boolean sw = (*iDCC)->getSoftwareSwitch() & 0x200;
			xdata::Boolean ttcrx = (*iDCC)->getSoftwareSwitch() & 0x1000;
			xdata::Boolean backpressure = (*iDCC)->getSoftwareSwitch() & 0x2000;
			xdata::Boolean slinknp = (*iDCC)->getSoftwareSwitch() & 0x4000;
			xdata::Boolean bit4 = (*iDCC)->getSoftwareSwitch() & 0x10;
			xdata::Boolean bit5 = (*iDCC)->getSoftwareSwitch() & 0x20;
			iRow->setField("KEY", key);
			iRow->setField("CRATE_NUMBER", crateNumber);
			iRow->setField("SLOT", slot);
			iRow->setField("FMM_ID", fmmid);
			iRow->setField("SLINK1_ID", slink1);
			iRow->setField("SLINK2_ID", slink2);
			iRow->setField("ENABLE_SW_SWITCH", sw);
			iRow->setField("TTCRX_NOT_READY", ttcrx);
			iRow->setField("IGNORE_SLINK_BACKPRESSURE", backpressure);
			iRow->setField("IGNORE_SLINK_NOT_PRESENT", slinknp);
			iRow->setField("SW_BIT4", bit4);
			iRow->setField("SW_BIT5", bit5);
			
		}
		
		// Insert
		insert("dcc", table);
		
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload DCCs to database: " + std::string(e.what()), e);
	}
}
