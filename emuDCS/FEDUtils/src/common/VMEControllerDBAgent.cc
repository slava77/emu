/*****************************************************************************\
* $Id: VMEControllerDBAgent.cc,v 1.3 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/

#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/VMEController.h"

emu::fed::VMEControllerDBAgent::VMEControllerDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_CONTROLLERS";
}



emu::fed::VMEController *emu::fed::VMEControllerDBAgent::getController(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["CRATE_ID"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_controller", parameters);
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
		return buildController(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



emu::fed::VMEController *emu::fed::VMEControllerDBAgent::getController(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"] = number.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_controller_by_key_crate", parameters);
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
		return buildController(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



emu::fed::VMEController *emu::fed::VMEControllerDBAgent::buildController(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	// Parse out the CAEN device and link numbers
	xdata::UnsignedShort device;
	xdata::UnsignedShort link;
	// There is only one row in the table
	try {
		device.setValue(*(table.getValueAt(0, "CAEN_DEVICE"))); // only way to get a serializable to something else
		link.setValue(*(table.getValueAt(0, "CAEN_LINK"))); // only way to get a serializable to something else
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
	
	return new VMEController(device, link);
}
