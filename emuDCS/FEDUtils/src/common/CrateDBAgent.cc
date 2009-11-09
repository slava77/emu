/*****************************************************************************\
* $Id: CrateDBAgent.cc,v 1.5 2009/11/09 11:46:33 paste Exp $
\*****************************************************************************/

#include "emu/fed/CrateDBAgent.h"
#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/Crate.h"
#include "xdata/TableIterator.h"

emu::fed::CrateDBAgent::CrateDBAgent(xdaq::WebApplication *application):
DBAgent(application)
{ 
	table_ = "EMU_FED_CRATES";
}



std::vector<emu::fed::Crate *> emu::fed::CrateDBAgent::getCrates(xdata::UnsignedInteger64 &id)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = id.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("crates", parameters);
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
		return buildCrates(result,id);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Crate *> emu::fed::CrateDBAgent::getCrates(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number)
throw (emu::fed::exception::DBException)
{
	// Set up parameters
	std::map<std::string, std::string> parameters;
	parameters["KEY"] = key.toString();
	parameters["CRATE_NUMBER"] = number.toString();
	
	// Execute the query
	xdata::Table result;
	try {
		result = query("get_crates_by_key_number", parameters);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error posting query", e);
	}
	
	// Did we match anything
	switch (result.getRowCount()) {
	case 0: 
		XCEPT_RAISE(emu::fed::exception::DBException, "No matching rows found");
		break;
	case 1:
		break;
	default:
		//AB: is this an error? It seems to me that there can be more than one crate
		//XCEPT_RAISE(emu::fed::exception::DBException, "More than one matching row found");
		break;
	}

	try {
		return buildCrates(result,key);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
	}
}



std::vector<emu::fed::Crate *> emu::fed::CrateDBAgent::buildCrates(xdata::Table &table,xdata::UnsignedInteger64 &key)
throw (emu::fed::exception::DBException)
{
	std::vector<emu::fed::Crate *> returnMe;
	for (xdata::Table::iterator iRow = table.begin(); iRow != table.end(); iRow++) {
		// Parse out the ID and crate number
		//xdata::UnsignedInteger64 id;
		xdata::UnsignedShort number;
		Crate *newCrate=0;
		try {
			setValue(number,*iRow,"CRATE_NUMBER"); 
			newCrate=new Crate(number);
			//now load VMEs, DDUs, DCCs and FIFOs.
			VMEControllerDBAgent VMEAgent(application_);
			VMEAgent.setConnectionID(connectionID_);
			emu::fed::VMEController *controller=VMEAgent.getController(key,number);
			newCrate->setController(controller);
			
			DDUDBAgent DDUAgent(application_);
			DDUAgent.setConnectionID(connectionID_);
			
			std::vector<DDU *> DDUs=DDUAgent.getDDUs(key,number);
			for (std::vector<DDU *>::iterator ddu=DDUs.begin();ddu!=DDUs.end();++ddu) {
				newCrate->addBoard((VMEModule *)(*ddu));
			}
		
			DCCDBAgent DCCAgent(application_);
			DCCAgent.setConnectionID(connectionID_);
			
			std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> DCCs=DCCAgent.getDCCs(key,number);
			for (std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp>::iterator dcc=DCCs.begin();dcc!=DCCs.end();++dcc) {
				newCrate->addBoard((VMEModule *)(*dcc).second);
			}
			
		} catch (xdata::exception::Exception &e) {
			if (newCrate) delete newCrate;
			XCEPT_RETHROW(emu::fed::exception::DBException, "Error finding columns", e);
		}
		returnMe.push_back(newCrate);
	}
	
	return returnMe;
}
