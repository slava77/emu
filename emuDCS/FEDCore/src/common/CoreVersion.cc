/*****************************************************************************\
* $Id: CoreVersion.cc,v 1.3 2009/05/21 15:33:44 paste Exp $
\*****************************************************************************/

#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(emuFEDCore)

void emuFEDCore::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
}

std::set<std::string, std::less<std::string> > emuFEDCore::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	return dependencies;
}

