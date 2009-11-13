/*****************************************************************************\
* $Id: SystemDBAgent.cc,v 1.5 2009/11/13 09:03:11 paste Exp $
\*****************************************************************************/

#include "emu/fed/SystemDBAgent.h"
#include "xdata/String.h"

emu::fed::SystemDBAgent::SystemDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{
	table_ = "EMU_FED_SYSTEMS";
}



std::string emu::fed::SystemDBAgent::getSystem(xdata::UnsignedInteger64 &key)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_system", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	try {
		return buildSystem(result);
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
	
}



std::string emu::fed::SystemDBAgent::buildSystem(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	// Parse out system name
	xdata::String name;
	// There is only one row in the table
	try {
		xdata::String name = getValue<xdata::String>(table.getValueAt(0, "NAME"));
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading system name", e);
	}
	
	return name.toString();
}
