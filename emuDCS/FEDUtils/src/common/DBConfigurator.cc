/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.3 2009/07/11 19:38:32 paste Exp $
\*****************************************************************************/

#include "emu/fed/DBConfigurator.h"



emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password, xdata::UnsignedInteger64 &key):
application_(application),
dbUsername_(username),
dbPassword_(password),
dbKey_(key)
{
	// Does nothing
}



std::vector<emu::fed::Crate *> emu::fed::DBConfigurator::setupCrates()
throw (emu::fed::exception::ConfigurationException)
{
	// Don't know how to do this yet
	XCEPT_RAISE(emu::fed::exception::ConfigurationException, "Can't configure from the online database yet");

	return crateVector_;
}
