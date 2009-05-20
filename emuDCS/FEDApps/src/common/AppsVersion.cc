/*****************************************************************************\
* $Id: AppsVersion.cc,v 1.4 2009/05/20 18:18:38 paste Exp $
\*****************************************************************************/

#include "emu/fed/AppsVersion.h"
#include "emu/base/version.h"
#include "emu/fed/CoreVersion.h"
#include "emu/fed/UtilsVersion.h"

GETPACKAGEINFO(emuFEDApps)

void emuFEDApps::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(emubase);
	CHECKDEPENDENCY(emuFEDCore);
	CHECKDEPENDENCY(emuFEDUtils);
}

std::set<std::string, std::less<std::string> > emuFEDApps::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	ADDDEPENDENCY(dependencies, emubase);
	ADDDEPENDENCY(dependencies, emuFEDCore);
	ADDDEPENDENCY(dependencies, emuFEDUtils);
	
	return dependencies;
}

