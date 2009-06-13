/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.2 2009/06/13 17:59:45 paste Exp $
\*****************************************************************************/

#include "emu/fed/DBConfigurator.h"



emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, const std::string &username, const std::string &password):
application_(application),
dbUsername_(username),
dbPassword_(password)
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
