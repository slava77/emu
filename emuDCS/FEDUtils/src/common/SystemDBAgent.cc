/*****************************************************************************\
* $Id: SystemDBAgent.cc,v 1.6 2009/11/22 22:45:10 paste Exp $
\*****************************************************************************/

#include "emu/fed/SystemDBAgent.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "toolbox/TimeVal.h"

emu::fed::SystemDBAgent::SystemDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{
	table_ = "EMU_FED_CONFIGURATIONS";
}



std::pair<std::string, time_t> emu::fed::SystemDBAgent::getSystem(xdata::UnsignedInteger64 &key)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_configuration", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	try {
		return buildSystem(result);
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
	
}



std::map<std::string, std::vector<xdata::UnsignedInteger64> > emu::fed::SystemDBAgent::getAllKeys(const std::string &system)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	std::string qry;
	if (system != "") {
		parameters["DESCRIPTION"] = system;
		qry = "get_configurations_by_description";
	} else {
		parameters["TABLE"] = table_;
		qry = "get_all";
	}
	
	// Execute the query
	xdata::Table result;
	try {
		result = query(qry, parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query " + qry + " : " + std::string(e.what()), e);
	}
	
	// Make the map
	std::map<std::string, std::vector<xdata::UnsignedInteger64> > keyMap;
	size_t nRows = result.getRowCount();
	for (size_t iRow = 0; iRow < nRows; iRow++) {
		try {
			xdata::String description = getValue<xdata::String>(result.getValueAt(iRow, "DESCRIPTION"));
			xdata::UnsignedInteger64 key = getValue<xdata::UnsignedInteger64>(result.getValueAt(iRow, "ID"));
			keyMap[description.toString()].push_back(key);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading value in returned row: " + std::string(e.what()), e);
		}
	}
	
	return keyMap;	
}



std::pair<std::string, time_t> emu::fed::SystemDBAgent::buildSystem(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	// Parse out system name and time stamp
	xdata::String name;
	time_t dbTime;
	// There is only one row in the table
	try {
		name = getValue<xdata::String>(table.getValueAt(0, "DESCRIPTION"));
		dbTime = toolbox::TimeVal(getValue<xdata::TimeVal>(table.getValueAt(0, "TIMESTAMP"))).sec();
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading value in returned row", e);
	}
	
	return make_pair(name.toString(), dbTime);
}
