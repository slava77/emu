/*****************************************************************************\
* $Id: SystemDBAgent.cc,v 1.2 2009/05/22 19:25:51 paste Exp $
\*****************************************************************************/

#include "emu/fed/SystemDBAgent.h"
#include "xdata/String.h"

emu::fed::SystemDBAgent::SystemDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{
	table_ = "EMU_FED_SYSTEMS";
}



std::pair<xdata::UnsignedInteger64, std::string> emu::fed::SystemDBAgent::getSystem(std::string hostname, xdata::UnsignedInteger64 key)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["HOSTNAME"] = hostname;
	parameters["KEY"] = key.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_system", parameters);
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
		return buildSystem(result);
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
	
}



std::pair<xdata::UnsignedInteger64, std::string> emu::fed::SystemDBAgent::getSystem(std::string hostname)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["HOSTNAME"] = hostname;
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_systems_by_hostname", parameters);
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
		return buildSystem(result);
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::pair<xdata::UnsignedInteger64, std::string> emu::fed::SystemDBAgent::buildSystem(xdata::Table table)
throw (emu::fed::exception::DBException)
{
	// Parse out the ID and system name
	xdata::UnsignedInteger64 id;
	xdata::String name;
	// There is only one row in the table
	try {
		id.setValue(*(table.getValueAt(0, "ID"))); // only way to get a serializable to something else
		name.setValue(*(table.getValueAt(0, "NAME"))); // only way to get a serializable to something else
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
	
	return std::make_pair(id, name.toString());
}
