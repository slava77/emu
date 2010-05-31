/*****************************************************************************\
* $Id: SystemDBAgent.cc,v 1.13 2010/05/31 14:05:19 paste Exp $
\*****************************************************************************/

#include "emu/fed/SystemDBAgent.h"
#include "xdata/TimeVal.h"
#include "toolbox/TimeVal.h"
#include "xdata/TableIterator.h"

emu::fed::SystemDBAgent::SystemDBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
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
	} catch (emu::fed::exception::DBException &e) {
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
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to parse system from database", e);
		}
	}
	
	// Sort for extreme usefulness
	for (xdata::Table::iterator iRow = result.begin(); iRow != result.end(); iRow++) {
		std::string name = getValue<xdata::String>(*iRow, "DESCRIPTION").toString();
		sort(returnMe[name].begin(), returnMe[name].end(), sorter_);
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
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading system values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}
	
	return make_pair(name.toString(), timeStamp);
}



void emu::fed::SystemDBAgent::upload(xdata::UnsignedInteger64 &key, xdata::String &name)
throw (emu::fed::exception::DBException)
{
	
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("ID", "unsigned int 64");
		table.addColumn("DESCRIPTION", "string");
		table.addColumn("TIMESTAMP", "time");
		
		// Make a new row
		xdata::TableIterator iRow = table.append();
		
		// Set values
		iRow->setField("ID", key);
		iRow->setField("DESCRIPTION", name);
		xdata::TimeVal now = toolbox::TimeVal(time(NULL));
		iRow->setField("TIMESTAMP", now);
		
		// Insert
		insert("configuration", table);
	
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload system to database: " + std::string(e.what()), e);
	}
}
