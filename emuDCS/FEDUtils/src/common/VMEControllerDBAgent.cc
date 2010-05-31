/*****************************************************************************\
* $Id: VMEControllerDBAgent.cc,v 1.11 2010/05/31 14:05:19 paste Exp $
\*****************************************************************************/

#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/VMEController.h"

emu::fed::VMEControllerDBAgent::VMEControllerDBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
{ 
	table_ = "EMU_FED_CONTROLLERS";
}



emu::fed::VMEController *emu::fed::VMEControllerDBAgent::getController(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number, const bool &fake)
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
		return buildController(result, fake);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



emu::fed::VMEController *emu::fed::VMEControllerDBAgent::buildController(xdata::Table &table, const bool &fake)
throw (emu::fed::exception::DBException)
{
	// Parse out the CAEN device and link numbers
	try {
		xdata::UnsignedShort device = getValue<xdata::UnsignedShort>(table.getValueAt(0, "CAEN_DEVICE"));
		xdata::UnsignedShort link = getValue<xdata::UnsignedShort>(table.getValueAt(0, "CAEN_LINK"));
		return new VMEController(device, link, fake);
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Error initializing controller hardware: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading controller values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}
}



void emu::fed::VMEControllerDBAgent::upload(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &crateNumber, emu::fed::VMEController *controller)
throw (emu::fed::exception::DBException)
{
	
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("CRATE_NUMBER", "unsigned short");
		table.addColumn("CAEN_DEVICE", "unsigned short");
		table.addColumn("CAEN_LINK", "unsigned short");
		
		// Make a new row
		xdata::TableIterator iRow = table.append();
		
		// Set values
		xdata::UnsignedShort device = controller->getDevice();
		xdata::UnsignedShort link = controller->getLink();
		iRow->setField("KEY", key);
		iRow->setField("CRATE_NUMBER", crateNumber);
		iRow->setField("CAEN_DEVICE", device);
		iRow->setField("CAEN_LINK", link);
		
		// Insert
		insert("VMEControllers", table);
		
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload system to database: " + std::string(e.what()), e);
	}
}
