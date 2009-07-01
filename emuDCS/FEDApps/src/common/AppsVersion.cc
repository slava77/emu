/*****************************************************************************\
* $Id: AppsVersion.cc,v 1.5 2009/07/01 14:54:03 paste Exp $
\*****************************************************************************/

#include "emu/fed/AppsVersion.h"
//#include "emu/base/version.h"
#include "emu/fed/CoreVersion.h"
#include "emu/fed/UtilsVersion.h"
//#include "sentinel/version.h"

GETPACKAGEINFO(EmuFEDApps)

void EmuFEDApps::checkPackageDependencies()
throw (config::PackageInfo::VersionException)
{
	//CHECKDEPENDENCY(emubase);
	CHECKDEPENDENCY(EmuFEDCore);
	CHECKDEPENDENCY(EmuFEDUtils);
	//CHECKDEPENDENCY(sentinel);
}

std::set<std::string, std::less<std::string> > EmuFEDApps::getPackageDependencies()
{
	std::set<std::string, std::less<std::string> > dependencies;
	
	//ADDDEPENDENCY(dependencies, emubase);
	ADDDEPENDENCY(dependencies, EmuFEDCore);
	ADDDEPENDENCY(dependencies, EmuFEDUtils);
	//ADDDEPENDENCY(dependencies, sentinel);
	
	return dependencies;
}

