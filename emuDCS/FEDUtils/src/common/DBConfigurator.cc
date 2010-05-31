/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.10 2010/05/31 14:05:18 paste Exp $
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

emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key, xdata::Integer &instance):
application_(application),
dbUsername_(username),
dbPassword_(password),
dbKey_(key),
instance_(instance)
{
	systemName_ = "unnamed";
	timeStamp_ = time(NULL);
}



std::vector<emu::fed::Crate *> emu::fed::DBConfigurator::setupCrates(const bool &fake)
throw (emu::fed::exception::ConfigurationException)
{
	// Begin by setting up the system name
	std::string connectionID = "";
	try {
		SystemDBAgent systemAgent(application_, instance_);
		connectionID = systemAgent.connect(dbUsername_, dbPassword_);
		std::pair<std::string, time_t> values = systemAgent.getSystem(dbKey_);
		systemName_ = values.first;
		timeStamp_ = values.second;
	} catch (emu::fed::exception::DBException &e) {
		systemName_ = "unnamed";
		timeStamp_ = time(NULL);
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error setting system name and timestamp", e);
	}
	
	// Now get the crates.  The Crate agent gives us a vector of crates.
	try {
		CrateDBAgent crateAgent(application_, instance_);
		crateAgent.setConnectionID(connectionID);
		crateVector_ = crateAgent.getCrates(dbKey_);
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating crates", e);
	}
	
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); ++iCrate) {
		
		xdata::UnsignedShort crateNumber = (*iCrate)->getNumber();
		// Get VME controller
		try {
			VMEControllerDBAgent VMEAgent(application_, instance_);
			VMEAgent.setConnectionID(connectionID);
			emu::fed::VMEController *controller = VMEAgent.getController(dbKey_, crateNumber, fake);
			(*iCrate)->setController(controller);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating VME controller", e);
		}
		
		// Get DDUs
		std::vector<DDU *> ddus;
		try {
			DDUDBAgent DDUAgent(application_, instance_);
			DDUAgent.setConnectionID(connectionID);
			ddus = DDUAgent.getDDUs(dbKey_, crateNumber, fake);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating DDUs", e);
		}
		
		// Get Fibers for DDUs
		for (std::vector<DDU *>::iterator iDDU = ddus.begin(); iDDU != ddus.end(); ++iDDU) {
			
			xdata::UnsignedShort rui = (*iDDU)->getRUI();
			
			try {
				FiberDBAgent fiberAgent(application_, instance_);
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
			DCCDBAgent DCCAgent(application_, instance_);
			DCCAgent.setConnectionID(connectionID);
			dccs = DCCAgent.getDCCs(dbKey_, crateNumber, fake);
		} catch (emu::fed::exception::DBException &e) {
			XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error creating DCCs", e);
		}
		
		// Get FIFOs for DCCs
		for (std::vector<DCC *>::iterator iDCC = dccs.begin(); iDCC != dccs.end(); ++iDCC) {
			
			xdata::UnsignedInteger fmm_id = (*iDCC)->getFMMID();
			
			try {
				FIFODBAgent fifoAgent(application_, instance_);
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



void emu::fed::DBConfigurator::uploadToDB(const std::vector<emu::fed::Crate *> &crateVector, const std::string &systemName)
throw (emu::fed::exception::ConfigurationException)
{
	
	try {
		
		// Create system entry
		SystemDBAgent systemAgent(application_, instance_);
		std::string connectionID = systemAgent.connect(dbUsername_, dbPassword_);
		xdata::String xSystemName = systemName;
		LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading system");
		systemAgent.upload(dbKey_, xSystemName);
		
		CrateDBAgent crateAgent(application_, instance_);
		crateAgent.setConnectionID(connectionID);
		LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading crates");
		crateAgent.upload(dbKey_, crateVector);
		
		for (std::vector<Crate *>::const_iterator iCrate = crateVector.begin(); iCrate != crateVector.end(); ++iCrate) {
			
			xdata::UnsignedShort crateNumber = (*iCrate)->getNumber();
			
			VMEController *controller = (*iCrate)->getController();
			VMEControllerDBAgent controllerAgent(application_, instance_);
			controllerAgent.setConnectionID(connectionID);
			controllerAgent.upload(dbKey_, crateNumber, controller);
			
			const std::vector<DDU *> &dduVector = (*iCrate)->getDDUs();
			DDUDBAgent dduAgent(application_, instance_);
			dduAgent.setConnectionID(connectionID);
			LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading DDUs for crate " << crateNumber);
			dduAgent.upload(dbKey_, crateNumber, dduVector);
			
			for (std::vector<DDU *>::const_iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); ++iDDU) {
				
				xdata::UnsignedShort ruiNumber = (*iDDU)->getRUI();
				
				const std::vector<Fiber *> &fiberVector = (*iDDU)->getFibers();
				FiberDBAgent fiberAgent(application_, instance_);
				fiberAgent.setConnectionID(connectionID);
				LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading fibers for RUI " << ruiNumber);
				fiberAgent.upload(dbKey_, ruiNumber, fiberVector);
				
			}
			
			const std::vector<DCC *> &dccVector = (*iCrate)->getDCCs();
			DCCDBAgent dccAgent(application_, instance_);
			dccAgent.setConnectionID(connectionID);
			LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading DCCs for crate " << crateNumber);
			dccAgent.upload(dbKey_, crateNumber, dccVector);
			
			for (std::vector<DCC *>::const_iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); ++iDCC) {
				
				xdata::UnsignedInteger fmmidNumber = (*iDCC)->getFMMID();
				
				const std::vector<FIFO *> &fifoVector = (*iDCC)->getFIFOs();
				FIFODBAgent fifoAgent(application_, instance_);
				fifoAgent.setConnectionID(connectionID);
				LOG4CPLUS_DEBUG(application_->getApplicationLogger(), "Uploading FIFOs for FMMID " << fmmidNumber);
				fifoAgent.upload(dbKey_, fmmidNumber, fifoVector);
				
			}
			
		}
		
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to upload to database: " << e.what();
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, error.str(), e);
	}
	
}
