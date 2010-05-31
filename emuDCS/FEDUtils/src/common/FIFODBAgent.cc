/*****************************************************************************\
* $Id: FIFODBAgent.cc,v 1.12 2010/05/31 14:05:18 paste Exp $
\*****************************************************************************/

#include "emu/fed/FIFODBAgent.h"
#include "emu/fed/FIFO.h"
#include "xdata/UnsignedShort.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"

emu::fed::FIFODBAgent::FIFODBAgent(xdaq::WebApplication *application, const int &instance):
DBAgent(application, instance)
{ 
	table_ = "EMU_FED_DCC_FIFOS";
}



std::vector<emu::fed::FIFO *> emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmm_id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["FMM_ID"] = fmm_id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fifos_by_key_fmmid", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}

	try {
		return buildFIFOs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::FIFO *> emu::fed::FIFODBAgent::buildFIFOs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::FIFO *> returnMe;
	try {
		for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); ++iRow) {
			// Parse out all needed elements
			xdata::UnsignedShort fifo_number = getValue<xdata::UnsignedShort>(*iRow, "FIFO_NUMBER");
			xdata::UnsignedShort rui = getValue<xdata::UnsignedShort>(*iRow, "RUI");
			xdata::Boolean used = getValue<xdata::Boolean>(*iRow, "USED");
			
			// Don't want to kill myself here
			if ((xdata::UnsignedShortT) fifo_number > 9) XCEPT_RAISE(emu::fed::exception::DBException, "FIFO number is too large");
			// Set names now.
			returnMe.push_back(new FIFO(fifo_number, rui, used));
		}
	} catch (emu::fed::exception::DBException &e) {
		std::ostringstream error;
		error << "Error reading FIFO values from database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	} catch (xdata::exception::Exception &e) {
		std::ostringstream error;
		error << "Error getting value from table: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::DBException, error.str(), e);
	}
	
	return returnMe;
}



void emu::fed::FIFODBAgent::upload(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmmid, const std::vector<emu::fed::FIFO *> &fifoVector)
throw (emu::fed::exception::DBException)
{
	try {
		// Make a table
		xdata::Table table;
		
		// Add column names and types
		table.addColumn("KEY", "unsigned int 64");
		table.addColumn("FMM_ID", "unsigned int");
		table.addColumn("FIFO_NUMBER", "unsigned short");
		table.addColumn("RUI", "unsigned short");
		table.addColumn("USED", "bool");
		
		for (std::vector<FIFO *>::const_iterator iFIFO = fifoVector.begin(); iFIFO != fifoVector.end(); ++iFIFO) {
			// Make a new row
			xdata::TableIterator iRow = table.append();
			
			// Set values
			xdata::UnsignedShort fifoNumber = (*iFIFO)->getNumber();
			xdata::UnsignedShort rui = (*iFIFO)->getRUI();
			xdata::Boolean used = (*iFIFO)->isUsed();
			iRow->setField("KEY", key);
			iRow->setField("FMM_ID", fmmid);
			iRow->setField("FIFO_NUMBER", fifoNumber);
			iRow->setField("RUI", rui);
			iRow->setField("USED", used);
			
		}
		
		// Insert
		insert("fifo", table);
		
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Unable to upload FIFOs to database: " + std::string(e.what()), e);
	}
}
