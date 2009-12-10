/*****************************************************************************\
* $Id: FiberDBAgent.cc,v 1.11 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/

#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/Fiber.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"
#include <sstream>

emu::fed::FiberDBAgent::FiberDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{ 
	table_ = "EMU_FED_DDU_FIBERS";
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
	try {
		for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
			// Parse out all needed elements
			xdata::UnsignedShort fiber_number = getValue<xdata::UnsignedShort>(*iRow, "FIBER_NUMBER");
			xdata::String chamber = getValue<xdata::String>(*iRow, "CHAMBER");
			xdata::Boolean killed = getValue<xdata::Boolean>(*iRow, "KILLED");
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
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading fiber parameters from database: " + std::string(e.what()), e);
	}
	
	return returnMe;
	
}



void emu::fed::FiberDBAgent::upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &rui, const std::vector<emu::fed::Fiber *> &fiberVector)
throw (emu::fed::exception::DBException)
{
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("RUI", "unsigned short");
		table.addColumn("FIBER_NUMBER", "unsigned short");
		table.addColumn("CHAMBER", "string");
		table.addColumn("KILLED", "bool");
		
		for (std::vector<Fiber *>::const_iterator iFiber = fiberVector.begin(); iFiber != fiberVector.end(); ++iFiber) {
			// Make a new row
			xdata::TableIterator iRow = table.append();
			
			// Set values
			xdata::UnsignedShort fiberNumber = (*iFiber)->getFiberNumber();
			xdata::String chamber = (*iFiber)->getName();
			xdata::Boolean killed = (*iFiber)->isKilled();
			iRow->setField("KEY", key);
			iRow->setField("RUI", rui);
			iRow->setField("FIBER_NUMBER", fiberNumber);
			iRow->setField("CHAMBER", chamber);
			iRow->setField("KILLED", killed);
			
		}
		
		// Insert
		insert("fiber", table);
		
	} catch (xdaq::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload fibers to database: " + std::string(e.what()), e);
	}
}
