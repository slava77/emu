/*****************************************************************************\
* $Id: CrateDBAgent.cc,v 1.11 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/

#include "emu/fed/CrateDBAgent.h"
#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/Crate.h"
#include "xdata/TableIterator.h"

emu::fed::CrateDBAgent::CrateDBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
{ 
	table_ = "EMU_FED_CRATES";
}



std::vector<emu::fed::Crate *> emu::fed::CrateDBAgent::getCrates(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("crates", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	try {
		return buildCrates(result,id);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Crate *> emu::fed::CrateDBAgent::buildCrates(xdata::Table &table, xdata::UnsignedInteger64 &key)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::Crate *> returnMe;
	try {
		for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
			// Parse out the crate number
			xdata::UnsignedShort number = getValue<xdata::UnsignedShort>(*iRow, "CRATE_NUMBER");
			returnMe.push_back(new Crate(number));
		}
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading crate values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}

	return returnMe;
}



void emu::fed::CrateDBAgent::upload(xdata::UnsignedInteger64 &key, const std::vector<emu::fed::Crate *> &crateVector)
throw (emu::fed::exception::DBException)
{
	
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("CRATE_NUMBER", "unsigned short");
		
		for (std::vector<Crate *>::const_iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); ++iCrate) {
			
			// Make a new row
			xdata::TableIterator iRow = table.append();
			
			// Set values
			xdata::UnsignedShort crateNumber = (*iCrate)->getNumber();
			iRow->setField("KEY", key);
			iRow->setField("CRATE_NUMBER", crateNumber);

		}
		
		// Insert
		insert("crate", table);
		
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload crates to database: " + std::string(e.what()), e);
	}
}
