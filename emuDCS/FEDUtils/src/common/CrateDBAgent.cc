/*****************************************************************************\
* $Id: CrateDBAgent.cc,v 1.7 2009/11/23 09:20:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/CrateDBAgent.h"
#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/Crate.h"
#include "xdata/TableIterator.h"

emu::fed::CrateDBAgent::CrateDBAgent(xdaq::WebApplication *application):
DBAgent(application)
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
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading crate parameters from database: " + std::string(e.what()), e);
	}

	return returnMe;
}
