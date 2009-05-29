/*****************************************************************************\
* $Id: FIFODBAgent.cc,v 1.3 2009/05/29 11:25:09 paste Exp $
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



std::pair<uint16_t, std::vector<emu::fed::FIFO *> > emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 id)
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



std::pair<uint16_t, std::vector<emu::fed::FIFO *> > emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 key, xdata::UnsignedShort fmm_id)
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



std::pair<uint16_t, std::vector<emu::fed::FIFO *> > emu::fed::FIFODBAgent::getFIFOs(xdata::UnsignedInteger64 key, xdata::UnsignedShort fmm_id, xdata::UnsignedShort number)
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



std::pair<uint16_t, std::vector<emu::fed::FIFO *> > emu::fed::FIFODBAgent::buildFIFOs(xdata::Table table)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::FIFO *> returnMe(10, new FIFO());
	uint16_t fifoinuse = 0;
	
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out all needed elements
		xdata::UnsignedShort fifo_number;
		xdata::UnsignedShort rui;
		xdata::Boolean used;
		try {
			fifo_number.setValue(*(iRow->getField("FIFO_NUMBER"))); // only way to get a serializable to something else
			rui.setValue(*(iRow->getField("RUI"))); // only way to get a serializable to something else
			used.setValue(*(iRow->getField("USED"))); // only way to get a serializable to something else
		} catch (xdata::exception::Exception &e) {
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
		}
		
		// Don't want to kill myself here
		if ((unsigned int) fifo_number > 9) XCEPT_RAISE(emu::fed::exception::DBException, "FIFO number is too large");
		
		// Set names now.
		returnMe[fifo_number] = new FIFO(rui, used);
		if (used) fifoinuse |= (1 << fifo_number);
	}
	
	return make_pair(fifoinuse, returnMe);
}
