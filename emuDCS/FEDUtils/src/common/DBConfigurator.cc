/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.4 2009/08/20 13:41:01 brett Exp $
\*****************************************************************************/

#include "emu/fed/DBConfigurator.h"
#include "emu/fed/CrateDBAgent.h"

emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key):
application_(application),
dbUsername_(username),
dbPassword_(password),
dbKey_(key)
{
	//id_.fromString(configID);
}



std::vector<emu::fed::Crate *> emu::fed::DBConfigurator::setupCrates()
throw (emu::fed::exception::ConfigurationException)
{
	try {
		//connecting to database is done by a DBAgent.
		//We need to instantiate a CrateDBAgent
		CrateDBAgent crateAgent(application_);
		
		//call connect
		crateAgent.connect(dbUsername_,dbPassword_);
		//get crates
		//unlike with XML, each DBAgent reads in its own child objects, e.g. CrateDBAgent configures also the DCCs, DDUs etc. of the crates it configures.
		//so all we need to do is get the crates.
		crateVector_=crateAgent.getCrates(dbKey_);
		return crateVector_;
	} catch (emu::fed::exception::DBException &e) {
		XCEPT_RETHROW(emu::fed::exception::ConfigurationException, "Error setting up crates", e);

	}
}
