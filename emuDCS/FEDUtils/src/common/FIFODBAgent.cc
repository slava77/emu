/*****************************************************************************\
* $Id: FIFODBAgent.cc,v 1.5 2009/08/20 13:41:01 brett Exp $
\*****************************************************************************/

#include "emu/fed/FIFODBAgent.h"
#include "emu/fed/FIFO.h"
#include "xdata/Boolean.h"
#include "xdata/String.h"
#include "xdata/TableIterator.h"

emu::fed::FIFODBAgent::FIFODBAgent(xdaq::WebApplication *application)
throw (emu::fed::exception::DBException):
DBAgent(application)
{ 
	table_ = "EMU_FED_DCC_FIFOS";
}



std::vector<emu::fed::FIFO *> emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["DCC_ID"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fifos", parameters);
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
		return buildFIFOs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
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
	
	// Did we match anything
	switch (result.getRowCount()) {
		case 0: 
			XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
			break;
		default:
			break;
	}
	
	try {
		return buildFIFOs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::FIFO *> emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &fmm_id, xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["FMM_ID"] = fmm_id.toString();
	parameters["FIFO_NUMBER"] = number.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_fifos_by_key_fmmid_number", parameters);
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
		return buildFIFOs(result);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::FIFO *> emu::fed::FIFODBAgent::buildFIFOs(xdata::Table &table)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::FIFO *> returnMe;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); ++iRow) {
		// Parse out all needed elements
		xdata::UnsignedShort fifo_number;
		xdata::UnsignedShort rui;
		xdata::Boolean used;
		setValue(fifo_number,*iRow,"FIFO_NUMBER");
		setValue(rui,*iRow,"RUI");
		setValue(used,*iRow,"USED");
		
		// Don't want to kill myself here
		if ((xdata::UnsignedShortT) fifo_number > 9) XCEPT_RAISE(emu::fed::exception::DBException, "FIFO number is too large");
		// Set names now.
		returnMe.push_back(new FIFO(fifo_number, rui, used));
	}
	
	return returnMe;
}
