/*****************************************************************************\
* $Id: FIFODBAgent.cc,v 1.8 2009/11/23 09:20:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/FIFODBAgent.h"
#include "emu/fed/FIFO.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"

emu::fed::FIFODBAgent::FIFODBAgent(xdaq::WebApplication *application):
DBAgent(application)
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
	} catch (xdata::exception::Exception &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error reading FIFO parameters from database: " + std::string(e.what()), e);
	}
	
	return returnMe;
}
