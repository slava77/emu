/*****************************************************************************\
* $Id: SystemDBAgent.cc,v 1.8 2009/11/23 09:20:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/SystemDBAgent.h"
#include "xdata/String.h"
#include "xdata/TimeVal.h"
#include "toolbox/TimeVal.h"
#include "xdata/TableIterator.h"

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
	parameters["ID"] = key.toString();
	
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



std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > emu::fed::SystemDBAgent::getAllKeys()
throw (emu::fed::exception::DBException)
{
	// Execute the query
	xdata::Table result;
	try {
		result = getAll();
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	std::map<std::string, std::vector<std::pair<xdata::UnsignedInteger64, time_t> > > returnMe;
	for (xdata::Table::iterator iRow = result.begin(); iRow != result.end(); iRow++) {
		try {
			xdata::String name = getValue<xdata::String>(*iRow, "DESCRIPTION");
			xdata::UnsignedInteger64 id = getValue<xdata::UnsignedInteger64>(*iRow, "ID");
			toolbox::TimeVal timeStamp = getValue<xdata::TimeVal>(*iRow, "TIMESTAMP");
			
			returnMe[name.toString()].push_back(std::make_pair(id, timeStamp.sec()));
		} catch (xdata::exception::Exception &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to parse system from database", e);
		}
	}
	
	return returnMe;
	
}



std::pair<std::string, time_t> emu::fed::SystemDBAgent::buildSystem(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	// Parse out system name and timestamp
	xdata::String name;
	time_t timeStamp;
	// There is only one row in the table
	try {
		name = getValue<xdata::String>(table.getValueAt(0, "DESCRIPTION"));
		toolbox::TimeVal timeVal = getValue<xdata::TimeVal>(table.getValueAt(0, "TIMESTAMP"));
		timeStamp = timeVal.sec();
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading value in returned row: " + std::string(e.what()), e);
	}
	
	return make_pair(name.toString(), timeStamp);
}
