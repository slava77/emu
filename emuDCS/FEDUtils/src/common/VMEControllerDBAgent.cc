/*****************************************************************************\
* $Id: VMEControllerDBAgent.cc,v 1.7 2009/11/23 09:20:20 paste Exp $
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
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading controller parameters from database: " + std::string(e.what()), e);
	}
}
