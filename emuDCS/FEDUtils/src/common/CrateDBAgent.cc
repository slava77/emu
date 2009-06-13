/*****************************************************************************\
* $Id: CrateDBAgent.cc,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/

#include "emu/fed/CrateDBAgent.h"
#include "emu/fed/Crate.h"
#include "xdata/TableIterator.h"

emu::fed::CrateDBAgent::CrateDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_CRATES";
}



std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> emu::fed::CrateDBAgent::getCrates(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["SYSTEM_ID"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_crates", parameters);
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
		return buildCrates(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> emu::fed::CrateDBAgent::getCrates(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"] = number.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_crates_by_key_number", parameters);
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
		return buildCrates(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::map<xdata::UnsignedInteger64, emu::fed::Crate *, emu::fed::DBAgent::comp> emu::fed::CrateDBAgent::buildCrates(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::map<xdata::UnsignedInteger64, Crate *, DBAgent::comp> returnMe;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out the ID and crate number
		xdata::UnsignedInteger64 id;
		xdata::UnsignedShort number;
		try {
			id.setValue(*(iRow->getField("ID"))); // only way to get a serializable to something else
			number.setValue(*(iRow->getField("CRATE_NUMBER"))); // only way to get a serializable to something else
		} catch (xdata::exception::Exception &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
		}
		
		returnMe[id] = new Crate(number);
	}
	
	return returnMe;
}
