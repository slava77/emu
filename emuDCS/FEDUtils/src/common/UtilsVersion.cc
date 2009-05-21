/*****************************************************************************\
* $Id: UtilsVersion.cc,v 1.2 2009/05/21 15:30:49 paste Exp $
\*****************************************************************************/

#include "emu/fed/UtilsVersion.h"
#include "emu/fed/CoreVersion.h"

GETPACKAGEINFO(emuFEDUtils)

void emuFEDUtils::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(emuFEDCore);
}

std::set<std::string, std::less<std::string> > emuFEDUtils::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	ADDDEPENDENCY(dependencies, emuFEDCore);
	
	return dependencies;
}

