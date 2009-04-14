/*****************************************************************************\
* $Id: CoreVersion.cc,v 1.2 2009/04/14 17:50:51 paste Exp $
\*****************************************************************************/

#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(emufedcore)

void emufedcore::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
}

std::set<std::string, std::less<std::string> > emufedcore::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	return dependencies;
}

