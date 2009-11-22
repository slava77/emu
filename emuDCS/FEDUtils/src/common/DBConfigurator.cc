/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.7 2009/11/22 22:45:10 paste Exp $
\*****************************************************************************/

#include "emu/fed/DBConfigurator.h"
#include "emu/fed/SystemDBAgent.h"
#include "emu/fed/CrateDBAgent.h"
#include "emu/fed/VMEControllerDBAgent.h"
#include "emu/fed/DDUDBAgent.h"
#include "emu/fed/FiberDBAgent.h"
#include "emu/fed/DCCDBAgent.h"
#include "emu/fed/FIFODBAgent.h"
#include "emu/fed/Crate.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"

emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key):
application_(application),
dbUsername_(username),
dbPassword_(password),
dbKey_(key)
{
	systemName_ = "unnamed";
	timeStamp_ = time(NULL);
}



std::vector<emu::fed::Crate *> emu::fed::DBConfigurator::setupCrates()
throw (emu::fed::exception::ConfigurationException)
{
	// Begin by setting up the system name
	std::string connectionID = "";
	try {
		SystemDBAgent systemAgent(application_);
		connectionID = systemAgent.connect(dbUsername_, dbPassword_);
		std::pair<std::string, time_t> nameTime = systemAgent.getSystem(dbKey_);
		systemName_ = nameTime.first;
		timeStamp_ = nameTime.second;
	} catch (emu::fed::exception::DBException &e) {
		systemName_ = "unnamed";
		//XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error setting system name", e);
	}
	
	// Now get the crates.  The Crate agent gives us a vector of crates.
	try {
		CrateDBAgent crateAgent(application_);
		crateAgent.setConnectionID(connectionID);
		crateVector_ = crateAgent.getCrates(dbKey_);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating crates", e);
	}
	
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
		
		xdata::UnsignedShort crateNumber = (*iCrate)->getNumber();
		// Get VME controller
		try {
			VMEControllerDBAgent VMEAgent(application_);
			VMEAgent.setConnectionID(connectionID);
			emu::fed::VMEController *controller = VMEAgent.getController(dbKey_, crateNumber);
			(*iCrate)->setController(controller);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating VME controller", e);
		}
		
		// Get DDUs
		std::vector<DDU *> ddus;
		try {
			DDUDBAgent DDUAgent(application_);
			DDUAgent.setConnectionID(connectionID);
			ddus = DDUAgent.getDDUs(dbKey_, crateNumber);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating DDUs", e);
		}
		
		// Get Fibers for DDUs
		for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
			
			xdata::UnsignedShort rui = (*iDDU)->getRUI();
			
			try {
				FiberDBAgent fiberAgent(application_);
				fiberAgent.setConnectionID(connectionID);
				(*iDDU)->setFibers(fiberAgent.getFibers(dbKey_, rui));
			} catch (emu::fed::exception::DBException &e) {
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating Fibers", e);
			}
			
			// Add the DDU
			(*iCrate)->addBoard((VMEModule *) (*iDDU));
		}
		
		// Get DCCs
		std::vector<DCC *> dccs;
		try {
			DCCDBAgent DCCAgent(application_);
			DCCAgent.setConnectionID(connectionID);
			dccs = DCCAgent.getDCCs(dbKey_, crateNumber);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating DCCs", e);
		}
		
		// Get FIFOs for DCCs
		for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
			
			xdata::UnsignedInteger fmm_id = (*iDCC)->getFMMID();
			
			try {
				FIFODBAgent fifoAgent(application_);
				fifoAgent.setConnectionID(connectionID);
				(*iDCC)->setFIFOs(fifoAgent.getFIFOs(dbKey_, fmm_id));
			} catch (emu::fed::exception::DBException &e) {
				XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating FIFOs", e);
			}
			
			// Add the DCC
			(*iCrate)->addBoard((VMEModule *) (*iDCC));
		}
	}
	
	return crateVector_;

}
