/*****************************************************************************\
* $Id: VMEControllerDBAgent.cc,v 1.6 2009/11/13 09:03:11 paste Exp $
\*****************************************************************************/

#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/VMEController.h"

emu::fed::VMEControllerDBAgent::VMEControllerDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{ 
	table_ = "EMU_FED_CONTROLLERS";
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
	try {
		xdata::UnsignedShort device = getValue<xdata::UnsignedShort>(table.getValueAt(0, "CAEN_DEVICE"));
		xdata::UnsignedShort link = getValue<xdata::UnsignedShort>(table.getValueAt(0, "CAEN_LINK"));
		return new VMEController(device, link);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading controller parameters from database", e);
	}
}
