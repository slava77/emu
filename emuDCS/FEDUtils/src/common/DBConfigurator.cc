/*****************************************************************************\
* $Id: DBConfigurator.cc,v 1.1 2009/05/16 18:55:20 paste Exp $
\*****************************************************************************/

#include "emu/fed/DBConfigurator.h"



emu::fed::DBConfigurator::DBConfigurator(xdaq::WebApplication *application, std::string username, std::string password):
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
