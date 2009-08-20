/*****************************************************************************\
* $Id: FiberDBAgent.cc,v 1.7 2009/08/20 13:41:01 brett Exp $
\*****************************************************************************/

#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/Fiber.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"
#include <sstream>

emu::fed::FiberDBAgent::FiberDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_DDU_FIBERS";
}



std::vector<emu::fed::Fiber *> emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["DDU_ID"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fibers", parameters);
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
		return buildFibers(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Fiber *> emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["RUI"] = rui.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fibers_by_key_rui", parameters);
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
		return buildFibers(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Fiber *> emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui, xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["RUI"] = rui.toString();
	parameters["FIBER_NUMBER"] = number.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fibers_by_key_rui_number", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	if (!result.getRowCount()) XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
	
	try {
		return buildFibers(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Fiber *> emu::fed::FiberDBAgent::buildFibers(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::Fiber *> returnMe;
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed elements
		xdata::UnsignedShort fiber_number;
		xdata::String chamber;
		xdata::Boolean killed;
		setValue(fiber_number,*iRow,"FIBER_NUMBER");
		setValue(chamber,*iRow,"CHAMBER");
		setValue(killed,*iRow,"KILLED"); 
		// Don't want to kill myself here
		if ((xdata::UnsignedShortT) fiber_number > 14) XCEPT_RAISE(emu::fed::exception::DBException, "Fiber number is too large");
		
		std::string chamberName = chamber.toString();
		std::string endcap = "?";
		unsigned int station = 0;
		unsigned int ring = 0;
		unsigned int number = 0;
		
		// Check normal station name first
		if (sscanf(chamberName.c_str(), "%*c%1u/%1u/%02u", &station, &ring, &number) == 3) {
			endcap = chamberName.substr(0,1);
			// Else it's probably an SP, so check that
		} else if (sscanf(chamberName.c_str(), "SP%02u", &number) == 1) {
			endcap = (number <= 6) ? "+" : "-";
		}
		
		// Set names now.
		returnMe.push_back(new Fiber(fiber_number, endcap, station, ring, number, killed));
	}
	
	return returnMe;
	
}
