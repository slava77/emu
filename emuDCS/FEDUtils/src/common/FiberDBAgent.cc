/*****************************************************************************\
* $Id: FiberDBAgent.cc,v 1.4 2009/06/08 19:17:14 paste Exp $
\*****************************************************************************/

#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/Fiber.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"
#include <sstream>
#include <boost/regex.hpp>

emu::fed::FiberDBAgent::FiberDBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_DDU_FIBERS";
}



std::pair<uint32_t, std::vector<emu::fed::Fiber *> > emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 id)
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



std::pair<uint32_t, std::vector<emu::fed::Fiber *> > emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 key, xdata::UnsignedShort rui)
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



std::pair<uint32_t, std::vector<emu::fed::Fiber *> > emu::fed::FiberDBAgent::getFibers(xdata::UnsignedInteger64 key, xdata::UnsignedShort rui, xdata::UnsignedShort number)
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



std::pair<uint32_t, std::vector<emu::fed::Fiber *> > emu::fed::FiberDBAgent::buildFibers(xdata::Table table)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::Fiber *> returnMe(15, new Fiber());
	uint32_t killfiber = 0;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed elements
		xdata::UnsignedShort fiber_number;
		xdata::String chamber;
		xdata::Boolean killed;
		try {
			fiber_number.setValue(*(iRow->getField("FIBER_NUMBER"))); // only way to get a serializable to something else
			chamber.setValue(*(iRow->getField("CHAMBER"))); // only way to get a serializable to something else
			killed.setValue(*(iRow->getField("KILLED"))); // only way to get a serializable to something else
		} catch (xdata::exception::Exception &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
		}
		
		// Don't want to kill myself here
		if ((unsigned int) fiber_number > 14) XCEPT_RAISE(emu::fed::exception::DBException, "Fiber number is too large");
		
		std::string endcap = "?";
		unsigned int station = 0;
		unsigned int ring = 0;
		unsigned int number = 0;
		
		// Check normal station name first
		boost::regex chamberRegex("([+\\-])(\\d)/(\\d)/(\\d{2})");
		boost::smatch chamberMatch;
		if (boost::regex_match(chamber.toString(), chamberMatch, chamberRegex)) {
			// Parse the text as numbers
			endcap = chamberMatch[1];
			std::istringstream parseMe(chamberMatch[2]);
			parseMe >> station;
			parseMe.str(chamberMatch[3]);
			parseMe >> ring;
			parseMe.str(chamberMatch[4]);
			parseMe >> number;
		} else {
			// Now check SPs
			boost::regex spRegex("SP[+\\-]?(\\d{2})");
			boost::smatch spMatch;
			if (boost::regex_match(chamber.toString(), spMatch, spRegex)) {
				std::istringstream parseMe(spMatch[1]);
				parseMe >> number;
				endcap = (number <= 6) ? "+" : "-";
			}
		}
		
		// Set names now.
		returnMe[fiber_number] = new Fiber(endcap, station, ring, number, killed);
		if (!killed) killfiber |= (1 << fiber_number);
	}
	
	return make_pair(killfiber, returnMe);
}
